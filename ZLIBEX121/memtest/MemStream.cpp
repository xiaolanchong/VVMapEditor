#include "stdafx.h"
#include "MemStream.h"

#ifndef ASSERT
#ifdef _DEBUG
_inline void ASSERT(void* x) 
{ 
	if(!x) __asm int 3;
}
#else
#define ASSERT(x)
#endif
#endif

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif /* _DEBUG */

struct CCriticalSectionWrap : CRITICAL_SECTION
{
	CCriticalSectionWrap()
	{
		InitializeCriticalSection(this);
	}

	~CCriticalSectionWrap()
	{
		DeleteCriticalSection(this);
	}
};

class CCriticalSectionEnterWrap
{
	CRITICAL_SECTION	m_cs;
public:
	CCriticalSectionEnterWrap( CRITICAL_SECTION& cs  ) : m_cs(cs)
	{
		EnterCriticalSection( &m_cs );
	}

	~CCriticalSectionEnterWrap()
	{
		LeaveCriticalSection( &m_cs);
	}
};

class MemStream : public IMemStream 
{
public:
		//NOTENOTE: IUnknown
        STDMETHOD (QueryInterface)(REFIID riid, void **ppvObj);
        STDMETHOD_(ULONG, AddRef)();
        STDMETHOD_(ULONG, Release)();
		//NOTENOTE: ISequenceStream
        STDMETHOD (Read)(void *pv, ULONG cb, ULONG *pcbRead);
        STDMETHOD (Write)(void const *pv, ULONG cb, ULONG *pcbWritten);
		//NOTENOTE: IStream
        STDMETHOD (Seek)(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition);
        STDMETHOD (SetSize)(ULARGE_INTEGER libNewSize);
        STDMETHOD (CopyTo)(IStream *, ULARGE_INTEGER, ULARGE_INTEGER *, ULARGE_INTEGER *);
        STDMETHOD (Commit)(DWORD);
        STDMETHOD (Revert)();
        STDMETHOD (LockRegion)(ULARGE_INTEGER, ULARGE_INTEGER, DWORD);
        STDMETHOD (UnlockRegion)(ULARGE_INTEGER, ULARGE_INTEGER, DWORD);
        STDMETHOD (Stat)(STATSTG *, DWORD);
        STDMETHOD (Clone)(IStream **);
		//NOTENOTE: IMemStream
		virtual HRESULT STDMETHODCALLTYPE GetPtr( void** ppBuffer ) ;

		MemStream(IMalloc* pAlloc,  PBYTE pBuffer = 0, UINT cbSize = 0 );
private:
        PBYTE		GrowBuffer(ULONG);

	    LONG        m_cRef;           // Reference count
		PBYTE		m_pBuf;           // Buffer pointer
		UINT        m_cbAlloc;        // The allocated size of the buffer
		UINT        m_cbData;         // The used size of the buffer
		UINT        m_iSeek;          // Where we are in the buffer.
		DWORD       m_grfMode;        // mode used at creation (for Stat, and to enforce)		

		CCriticalSectionWrap	m_cs;		// for sinc
		
		IMalloc*	m_pAlloc;
};


MemStream::MemStream( IMalloc* pAlloc, PBYTE pBuffer, UINT cbSize ):
	m_pAlloc(pAlloc),
	m_grfMode(STGM_READWRITE),
	m_cRef(1),
	m_pBuf(pBuffer),
	m_cbAlloc(cbSize),
	m_cbData(cbSize),
	m_iSeek(0)
{
	pAlloc->AddRef();	
}

//////// IUnknown stuff /////////	
STDMETHODIMP MemStream::QueryInterface(REFIID riid, void **ppvObj)
{
    if (IsEqualIID(riid, IID_IStream)	|| 
		IsEqualIID(riid, IID_IUnknown)	||
		IsEqualIID(riid, IID_ISequentialStream) )
    {
        *ppvObj=this;
        InterlockedIncrement( &this->m_cRef );
        return S_OK;
    }
	else
	{
		*ppvObj = NULL;
		return E_NOINTERFACE;
	}
}

STDMETHODIMP_(ULONG) MemStream::AddRef()
{
    return InterlockedIncrement( &this->m_cRef );
}

STDMETHODIMP_(ULONG) MemStream::Release()
{
    if ( InterlockedDecrement( &this->m_cRef )  > 0)
        return m_cRef;

    // Free the data buffer that is allocated to the stream
    if ( m_pBuf)
        m_pAlloc->Free( m_pBuf);

	//NOTENOTE: we can change our own mem alloc stategy
	m_pAlloc->Release();
    ::delete this;
    return 0;
}

/////// ISequentialStream ////////////////////

STDMETHODIMP MemStream::Read(void *pv, ULONG cb, ULONG *pcbRead)
{
#ifdef ENFORCE_GRFMODE
    if (( m_grfMode & (STGM_READ|STGM_WRITE|STGM_READWRITE)) == STGM_WRITE)
    {
        if (pcbRead != NULL)
            *pcbRead = 0;
        return STG_E_ACCESSDENIED;
    }
#endif

    ASSERT(pv);

    // I guess a null read is ok.
    if (!cb)
    {
        if (pcbRead != NULL)
            *pcbRead = 0;
        return S_OK;
    }

	//NOTENOTE: we want to get seek pointer but other thread may write to it
	CCriticalSectionEnterWrap csew( m_cs);

    if (this->m_iSeek >= this->m_cbData)
    {
        if (pcbRead != NULL)
            *pcbRead = 0;   // nothing read
    }

    else
    {
        if ((this->m_iSeek + cb) > this->m_cbData)
            cb = this->m_cbData - this->m_iSeek;

        // Now Copy the memory
        ASSERT(this->m_pBuf);
        CopyMemory(pv, (PBYTE)this->m_pBuf + this->m_iSeek, cb);
        this->m_iSeek += (UINT)cb;

        if (pcbRead != NULL)
            *pcbRead = cb;
    }

    return S_OK;
}

PBYTE MemStream::GrowBuffer(ULONG cbNew)
{
    if (this->m_pBuf == NULL)
    {
        this->m_pBuf = (PBYTE)m_pAlloc->Alloc( cbNew );
    }
    else
    {
        PBYTE pTemp = (PBYTE)m_pAlloc->Realloc(this->m_pBuf, cbNew/*, LMEM_MOVEABLE | LMEM_ZEROINIT*/);
        if (pTemp)
        {
            this->m_pBuf = m_pBuf;
        }
        else
        {
   //         TraceMsg(TF_ERROR, "Stream buffer realloc failed");
            return NULL;
        }
    }
    if (this->m_pBuf)
        this->m_cbAlloc = cbNew;

    return this->m_pBuf;
}

#define SIZEINCR    0x1000


STDMETHODIMP MemStream::Write(void const *pv, ULONG cb, ULONG *pcbWritten)
{
#ifdef ENFORCE_GRFMODE
    if ((this->m_grfMode & (STGM_READ|STGM_WRITE|STGM_READWRITE)) == STGM_READ)
    {
        if (pcbWritten != NULL)
            *pcbWritten = 0;
        return STG_E_ACCESSDENIED;
    }
#endif

    // I guess a null write is ok.
    if (!cb)
    {
        if (pcbWritten != NULL)
            *pcbWritten = 0;
        return S_OK;
    }

	CCriticalSectionEnterWrap csew( m_cs);
    // See if the data will fit into our current buffer
    if ((this->m_iSeek + cb) > this->m_cbAlloc)
    {
        // enlarge the buffer
        // Give it a little slop to avoid a lot of reallocs.
        if (GrowBuffer(this->m_iSeek + (UINT)cb + SIZEINCR) == NULL)
            return STG_E_INSUFFICIENTMEMORY;
    }

    ASSERT(this->m_pBuf);

    // See if we need to fill the area between the data size and
    // the seek position
    if (this->m_iSeek > this->m_cbData)
    {
        ZeroMemory(this->m_pBuf + this->m_cbData, this->m_iSeek - this->m_cbData);
    }

    CopyMemory(this->m_pBuf + this->m_iSeek, pv, cb);
    this->m_iSeek += (UINT)cb;
    if (this->m_iSeek > this->m_cbData)
        this->m_cbData = this->m_iSeek;

    if (pcbWritten != NULL)
        *pcbWritten = cb;

    return S_OK;
}

STDMETHODIMP MemStream::Seek(LARGE_INTEGER dlibMove,
               DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition)
{
    LONG lNewSeek;

	CCriticalSectionEnterWrap csew( m_cs);
    // Note: curently not testing for error conditions for number wrap...
    switch (dwOrigin)
    {
    case STREAM_SEEK_SET:
        lNewSeek = (LONG)dlibMove.LowPart;
        break;
    case STREAM_SEEK_CUR:
        lNewSeek = (LONG)this->m_iSeek + (LONG)dlibMove.LowPart;
        break;
    case STREAM_SEEK_END:
        lNewSeek = (LONG)this->m_cbData + (LONG)dlibMove.LowPart;
        break;
    default:
        return STG_E_INVALIDPARAMETER;
    }

    if (lNewSeek < 0)
        return STG_E_INVALIDFUNCTION;

    this->m_iSeek = (UINT)lNewSeek;

    if (plibNewPosition != NULL)
    {
        plibNewPosition->LowPart = (DWORD)lNewSeek;
        plibNewPosition->HighPart = 0;
    }
    return S_OK;
}

STDMETHODIMP MemStream::SetSize(ULARGE_INTEGER libNewSize)
{
#ifdef ENFORCE_GRFMODE
    if ((this->m_grfMode & (STGM_READ|STGM_WRITE|STGM_READWRITE)) == STGM_READ)
    {
        return STG_E_ACCESSDENIED;
    }
#endif

    UINT cbNew = (UINT)libNewSize.LowPart;

	CCriticalSectionEnterWrap csew( m_cs);
    // See if the data will fit into our current buffer
    if (cbNew > this->m_cbData)
    {
        // See if we have to Enlarge the buffer.
        if (cbNew > this->m_cbAlloc)
        {
            // enlarge the buffer - Does not check wrap...
            // Give it a little slop to avoid a lot of reallocs.
            if (GrowBuffer(cbNew) == NULL)
                return STG_E_INSUFFICIENTMEMORY;
        }

        // Now fill some memory
        ZeroMemory(this->m_pBuf + this->m_cbData, cbNew - this->m_cbData);
    }

    // Save away the new size.
    this->m_cbData = cbNew;
    return S_OK;
}

STDMETHODIMP MemStream::CopyTo(IStream *pstmTo,
             ULARGE_INTEGER cb, ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten)
{
#ifdef ENFORCE_GRFMODE
    if ((this->m_grfMode & (STGM_READ|STGM_WRITE|STGM_READWRITE)) == STGM_WRITE)
    {
        if (pcbRead != NULL)
            ZeroMemory(pcbRead, SIZEOF(pcbRead));
        if (pcbWritten != NULL)
            ZeroMemory(pcbWritten, SIZEOF(pcbWritten));
        return STG_E_ACCESSDENIED;
    }
#endif

	CCriticalSectionEnterWrap csew( m_cs);
    HRESULT hres = S_OK;
    UINT cbRead = this->m_cbData - this->m_iSeek;
    ULONG cbWritten = 0;

    if (cb.HighPart == 0 && cb.LowPart < cbRead)
    {
        cbRead = cb.LowPart;
    }

    if (cbRead > 0)
    {
        hres = pstmTo->Write(this->m_pBuf + this->m_iSeek, cbRead, &cbWritten);
        this->m_iSeek += cbRead;
    }

    if (pcbRead)
    {
        pcbRead->LowPart = cbRead;
        pcbRead->HighPart = 0;
    }
    if (pcbWritten)
    {
        pcbWritten->LowPart = cbWritten;
        pcbWritten->HighPart = 0;
    }

    return hres;
}

STDMETHODIMP MemStream::Commit(DWORD grfCommitFlags)
{
    return E_NOTIMPL;
}

STDMETHODIMP MemStream::Revert()
{
    return E_NOTIMPL;
}

STDMETHODIMP MemStream::LockRegion(ULARGE_INTEGER libOffset,
                 ULARGE_INTEGER cb, DWORD dwLockType)

{
    return E_NOTIMPL;
}

STDMETHODIMP MemStream::UnlockRegion(ULARGE_INTEGER libOffset,
                 ULARGE_INTEGER cb, DWORD dwLockType)
{
    return E_NOTIMPL;
}

// Trident calls this to determine the size of the structure.
// No reason to not support this one.
STDMETHODIMP MemStream::Stat(STATSTG *pstatstg, DWORD grfStatFlag)
{
    ZeroMemory(pstatstg, sizeof(*pstatstg));

	CCriticalSectionEnterWrap csew( m_cs);
    // we have no name
    pstatstg->type = STGTY_STREAM;
    pstatstg->cbSize.LowPart = this->m_cbData;
    // blow off modify, create, access times (we don't track anyway)
    pstatstg->grfMode = this->m_grfMode;
    // we're not transacting, so we have no lock modes
    // we're the null clsid already
    // we're not based on storage, so we have no state or storage bits
    
    return S_OK;
}

STDMETHODIMP MemStream::Clone(IStream **ppstm)
{
    *ppstm = NULL;
    return E_NOTIMPL;
}

STDMETHODIMP  MemStream::GetPtr( void** ppBuffer )
{
	if( !ppBuffer ) return E_INVALIDARG;
	*ppBuffer = this->m_pBuf; 
	return S_OK;
}

IMemStream*	CreateStreamOnMalloc( IMalloc* pAlloc, BYTE* pBuffer, UINT uSize )
{
#ifdef _DEBUG
		_CrtSetDbgFlag( _CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF );
#endif	
	if( ! pAlloc ) return 0;
	return ::new MemStream( pAlloc, pBuffer, uSize );
}

