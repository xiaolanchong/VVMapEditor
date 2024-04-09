// MemStream.h: interface for the CMemStream class.
//
//////////////////////////////////////////////////////////////////////

#ifndef ELVEES_MEMSTREAM_H__INCLUDED
#define ELVEES_MEMSTREAM_H__INCLUDED

#if defined (_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

//////////////////////////////////////////////////////////////////////
// CMemStream
//////////////////////////////////////////////////////////////////////

class CMemStream : public IStream
{
public:
	explicit CMemStream(BYTE* pBuffer = NULL, DWORD size = 0);
	virtual ~CMemStream();

	ULONG Size();
	BYTE* GetBuffer(bool bDetachBuffer = true);

	// IUnknown
	STDMETHOD(QueryInterface)(REFIID riid, LPVOID* ppvObject);
	STDMETHOD_(ULONG, AddRef)(void);
	STDMETHOD_(ULONG, Release)(void);

	// ISequentialStream
	STDMETHOD(Read)(LPVOID pv, ULONG cb, ULONG *pcbRead);
	STDMETHOD(Write)(LPCVOID pv, ULONG cb, ULONG *pcbWritten);

	// IStream
	STDMETHOD(Seek)(
			LARGE_INTEGER dlibMove,
			DWORD dwOrigin,
			ULARGE_INTEGER *plibNewPosition);

	STDMETHOD(SetSize)(
			ULARGE_INTEGER libNewSize);

	STDMETHOD(CopyTo)( 
			IStream *pstm,
			ULARGE_INTEGER cb,
			ULARGE_INTEGER *pcbRead,
			ULARGE_INTEGER *pcbWritten);

	STDMETHOD(Commit)(DWORD grfCommitFlags);

	STDMETHOD(Revert)(void);

	STDMETHOD(LockRegion)(
			ULARGE_INTEGER libOffset,
			ULARGE_INTEGER cb,
			DWORD dwLockType);

	STDMETHOD(UnlockRegion)(
			ULARGE_INTEGER libOffset,
			ULARGE_INTEGER cb,
			DWORD dwLockType);

	STDMETHOD(Stat)(
			STATSTG *pstatstg,
			DWORD grfStatFlag);

	STDMETHOD(Clone)(IStream **ppstm);

protected:
	bool Open();
	bool Close();

private:
	BYTE* m_pBuffer;
	ULONG m_ulSize;
	bool  m_bFreeOnClose;  // 

	ULONG m_cRef;       // Reference count (not used).
	ULONG m_ulReadPos;  // Current index position for reading from the buffer.
	ULONG m_ulWritePos; // Current index position for writing to the buffer.

	// No copies do not implement
	CMemStream(const CMemStream &rhs);
	CMemStream &operator=(const CMemStream &rhs);
};

#endif // ELVEES_MEMSTREAM_H__INCLUDED