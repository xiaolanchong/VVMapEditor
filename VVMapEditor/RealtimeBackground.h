//========================= Copyright © 2004, Elvees ==========================
// Author: Eugene V. Gorbachev
// Purpose: Orwis & mapimg.dll streams, holds & renders image samples
//
//=============================================================================

// RealtimeBackground.h: interface for the CRealtimeBackground class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REALTIMEBACKGROUND_H__66725E81_A287_4AB3_9241_CB53166E813E__INCLUDED_)
#define AFX_REALTIMEBACKGROUND_H__66725E81_A287_4AB3_9241_CB53166E813E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Background.h"
#include "Exception.h"
#include "Sync.h"
#include <atlbase.h>
#include <dshow.h>

MACRO_EXCEPTION(RealtimeVideoException, CommonException);
MACRO_EXCEPTION(ConnectErrorException, RealtimeVideoException);
MACRO_EXCEPTION(VideoFormatException, RealtimeVideoException);
MACRO_EXCEPTION(RecordException, RealtimeVideoException);

#ifdef ORWELL_CAM
using namespace Elvees::Win32;
#else

class CImage
{
public:
	virtual ~CImage() = default;
	virtual void AddRef() = 0;
	virtual void Release() = 0;

	virtual void CreateCopy(CImage** ppImage) = 0;
	virtual void GetImageInfo(BITMAPINFO* bmi) = 0;
	virtual void GetImageBytes(LPBYTE* bytes) = 0;
};

bool CreateImage(CImage** ppImage, int CamID);

#endif

class CRealtimeBackground : public CControlBackground  
{
//-----------------------------------------------------------------------------
// Purpose: smart ptr for CImage class
//-----------------------------------------------------------------------------
	template <class T>
	class Ptr
	{
		
	public:
		typedef T _PtrClass;
		Ptr()
		{
			p=NULL;
		}
		Ptr(T* lp)
		{
			if ((p = lp) != NULL)
				p->AddRef();
		}
		Ptr(const CComPtr<T>& lp)
		{
			if ((p = lp.p) != NULL)
				p->AddRef();
		}
		~Ptr()
		{
			if (p)
				p->Release();
		}
		void Release()
		{
			_PtrClass* pTemp = p;
			if (pTemp)
			{
				p = NULL;
				pTemp->Release();
			}
		}
		T* operator->() const
		{
			ASSERT(p);
			return p;
		}

		_PtrClass*p;
	};

	mutable CSync		m_Sync;
	HANDLE		m_hEvent;
	HANDLE		m_hFeedback;
	CWnd*		m_pWnd;
	Ptr<CImage>		m_pImage;

	static UINT ThreadProc(void * Param);

	CComPtr<IBaseFilter>	GetFilter(DWORD dwFourCC);
	CComPtr<IBaseFilter>	GetFilter();
	HRESULT					AddToRot(IUnknown *pUnkGraph) ;
	void					RemoveFromRot();

	DWORD					m_dwRegister;

	CComQIPtr<ICaptureGraphBuilder2, &IID_ICaptureGraphBuilder2>   m_pBuild ;
	
public:
	CRealtimeBackground(CWnd* pWnd, int CamID);
	virtual ~CRealtimeBackground();

	virtual void	Draw(Graphics& gr, const CRectF& rc) const ;
	virtual CSize	GetSize()							 const ;

	void	Record(DWORD dwFourCC, CString StrIn, CString StrOut );
	void	StopRecord();

	void	Stop();
	void	Play();

	bool	IsPlay() { return ( WaitForSingleObject( m_hEvent, 0) == WAIT_OBJECT_0); };
};

#endif // !defined(AFX_REALTIMEBACKGROUND_H__66725E81_A287_4AB3_9241_CB53166E813E__INCLUDED_)