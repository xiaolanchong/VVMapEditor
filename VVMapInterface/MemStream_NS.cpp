// MemStream.cpp: implementation of the CMemStream class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <Objidl.h>

#include "MemStream_NS.h"

CMemStream::CMemStream(BYTE* pBuffer, DWORD ulSize)
{
	m_cRef = 0;
	m_ulReadPos	= 0;
	m_ulWritePos = 0;

	m_pBuffer = pBuffer;
	m_ulSize = ulSize;
	m_bFreeOnClose = (pBuffer == 0) ? true : false;
}

CMemStream::~CMemStream()
{
	Close();
}

bool CMemStream::Open()
{
	if(m_pBuffer)
		return false;	// Can't re-open without closing first

	m_ulReadPos	= 0;
	m_ulWritePos = 0;

	m_pBuffer = (BYTE*)malloc(0);
	m_ulSize = 0;
	m_bFreeOnClose = true;

	return (m_pBuffer != 0);
}

bool CMemStream::Close()
{
	if( (m_pBuffer) && (m_bFreeOnClose) )
	{
		free(m_pBuffer);
		m_pBuffer = NULL;
		m_ulSize = 0;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
// IUnknown
//////////////////////////////////////////////////////////////////////////

STDMETHODIMP CMemStream::QueryInterface(REFIID riid, LPVOID* ppvObject)
{
	*ppvObject = NULL;

	if(riid == IID_IUnknown) *ppvObject = this;
	if(riid == IID_IStream) *ppvObject = this;
	if(riid == IID_ISequentialStream) *ppvObject = this;
	
	if(*ppvObject)
	{
		( (IUnknown*) *ppvObject )->AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CMemStream::AddRef(void)
{
	return ++m_cRef;
}

STDMETHODIMP_(ULONG) CMemStream::Release(void)
{
	return --m_cRef;
}

//////////////////////////////////////////////////////////////////////////
// ISequentialStream
//////////////////////////////////////////////////////////////////////////

STDMETHODIMP CMemStream::Read(LPVOID pv, ULONG cb, ULONG *pcbRead)
{
	//Elvees::OutputF(Elvees::TTrace, TEXT("CMemStream::Read(%ld)"), cb);

	// Check parameters.
	if ( pcbRead ) *pcbRead = 0;
	if ( !pv ) return STG_E_INVALIDPOINTER;
	if ( 0 == cb ) return S_OK; 

	// Calculate bytes left and bytes to read.
	ULONG cBytesLeft = m_ulSize - m_ulReadPos;
	ULONG cBytesRead = cb > cBytesLeft ? cBytesLeft : cb;

	// If no more bytes to retrieve return S_FALSE.
	if ( 0 == cBytesLeft ) return S_FALSE;

	// Copy to users buffer the number of bytes requested or remaining
	memcpy( pv, m_pBuffer + m_ulReadPos, cBytesRead );
	m_ulReadPos += cBytesRead;

	// Return bytes read to caller.
	if ( pcbRead ) *pcbRead = cBytesRead;
	if ( cb != cBytesRead ) return S_FALSE; 

	return S_OK;
}

STDMETHODIMP CMemStream::Write(LPCVOID pv, ULONG cb, ULONG *pcbWritten)
{
	//Elvees::OutputF(Elvees::TTrace, TEXT("CMemStream::Write(%ld)"), cb);

	// Check parameters.
	if ( !pv ) return STG_E_INVALIDPOINTER;
	if ( pcbWritten ) *pcbWritten = 0;
	if ( 0 == cb ) return S_OK;

	// Enlarge the current buffer.
	m_ulSize += cb;

	// Grow internal buffer to new size.
	m_pBuffer = (BYTE*)realloc( m_pBuffer, m_ulSize );

	// Check for out of memory situation.
	if ( NULL == m_pBuffer ) 
	{
		Close();
		return E_OUTOFMEMORY;
	}

	// Copy callers memory to internal buffer and update write position.
	memcpy(m_pBuffer + m_ulWritePos, pv, cb);
	m_ulWritePos += cb;

	// Return bytes written to caller.
	if ( pcbWritten ) *pcbWritten = cb;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// IStream
//////////////////////////////////////////////////////////////////////////

STDMETHODIMP CMemStream::Seek(
	LARGE_INTEGER dlibMove,
	DWORD dwOrigin,
	ULARGE_INTEGER *plibNewPosition)
{
	//Elvees::OutputF(Elvees::TTrace, TEXT("CMemStream::Seek(%ld:%ld, %ld, %X)"),
	//	dlibMove.LowPart, dlibMove.HighPart, dwOrigin, plibNewPosition);

	if(dlibMove.HighPart > 0)
		return STG_E_INVALIDPOINTER;

	if(dwOrigin == STREAM_SEEK_SET)
	{

	}
	else if(dwOrigin == STREAM_SEEK_CUR)
	{
		if(dlibMove.LowPart <= m_ulSize)
		{
			m_ulReadPos = dlibMove.LowPart;

			if(plibNewPosition)
			{
				plibNewPosition->LowPart = m_ulReadPos;
				plibNewPosition->HighPart = 0;
			}

			return S_OK;
		}
	}
	else if(dwOrigin == STREAM_SEEK_END)
	{
		if(dlibMove.LowPart <= m_ulSize)
		{
			m_ulReadPos = m_ulSize - dlibMove.LowPart;

			if(plibNewPosition)
			{
				plibNewPosition->LowPart = m_ulReadPos;
				plibNewPosition->HighPart = 0;
			}

			return S_OK;
		}
	}
	else
		return STG_E_INVALIDFUNCTION;

	return E_FAIL;
}

STDMETHODIMP CMemStream::SetSize(ULARGE_INTEGER libNewSize)
{
	//Elvees::Output(Elvees::TTrace, TEXT("CMemStream::SetSize"));
	return E_NOTIMPL;
}

STDMETHODIMP CMemStream::CopyTo( 
	IStream *pstm,
	ULARGE_INTEGER cb,
	ULARGE_INTEGER *pcbRead,
	ULARGE_INTEGER *pcbWritten)
{
	//Elvees::Output(Elvees::TTrace, TEXT("CMemStream::CopyTo"));
	return E_NOTIMPL;
}

STDMETHODIMP CMemStream::Commit(DWORD grfCommitFlags)
{
	//Elvees::Output(Elvees::TTrace, TEXT("CMemStream::Commit"));
	return E_NOTIMPL;
}

STDMETHODIMP CMemStream::Revert(void)
{
	//Elvees::Output(Elvees::TTrace, TEXT("CMemStream::Revert"));
	return E_NOTIMPL;
}

STDMETHODIMP CMemStream::LockRegion(
	ULARGE_INTEGER libOffset,
	ULARGE_INTEGER cb,
	DWORD dwLockType)
{
	//Elvees::Output(Elvees::TTrace, TEXT("CMemStream::LockRegion"));
	return E_NOTIMPL;
}

STDMETHODIMP CMemStream::UnlockRegion(
	ULARGE_INTEGER libOffset,
	ULARGE_INTEGER cb,
	DWORD dwLockType)
{
	//Elvees::Output(Elvees::TTrace, TEXT("CMemStream::UnlockRegion"));
	return E_NOTIMPL;
}

STDMETHODIMP CMemStream::Stat(STATSTG *pstatstg, DWORD grfStatFlag)
{
  memset(pstatstg, 0, sizeof(STATSTG));
  pstatstg->cbSize.QuadPart = m_ulSize;
  return S_OK;
}

STDMETHODIMP CMemStream::Clone(IStream **ppstm)
{
	//Elvees::Output(Elvees::TTrace, TEXT("CMemStream::Clone"));
	return E_NOTIMPL;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

ULONG CMemStream::Size()
{
	return m_ulSize;
}

BYTE* CMemStream::GetBuffer(bool bDetachBuffer)
{
	m_bFreeOnClose = !bDetachBuffer;
	return m_pBuffer;
}