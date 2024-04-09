// RealtimeBackground.cpp: implementation of the CRealtimeBackground class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <atomic>
#include "RealtimeBackground.h"
#ifdef DIRECTX
#include <dxerr9.h>
#endif
#include "Helper.h"
#include <vector>
//#include "EditorState.h"
//#include "ResourceMgr.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRealtimeBackground::CRealtimeBackground(CWnd* pWnd, int CamID):
	m_hEvent( NULL ),
	m_hFeedback( NULL ),
	m_pWnd( pWnd )
{
	bool res =  CreateImage(&m_pImage.p, CamID);
	if(!res)
	{
		CString str;
		str.LoadString( IDS_EXC_VIDEO_SERVER );
		throw ConnectErrorException( Helper::ConvertA(  str ) );
	}

	Ptr<CImage> pImg = NULL;	// try to get copy
	m_pImage->CreateCopy(&pImg.p);
	if(!pImg.p) 
	{
		CString str;
		str.LoadString( IDS_EXC_VIDEO_IMAGE );
		throw ConnectErrorException(  Helper::ConvertA( str ) );
	}
	

	BITMAPINFO bmi;
	pImg->GetImageInfo(&bmi);

	CSize VideoSize( bmi.bmiHeader.biWidth, bmi.bmiHeader.biHeight );


	m_hEvent	= ::CreateEvent( 0, TRUE, TRUE, 0 ) ;
	m_hFeedback	= ::CreateEvent( 0, TRUE, FALSE, 0 );
	Play();
}

CRealtimeBackground::~CRealtimeBackground()
{
	Stop();
	m_pImage.Release();
	CloseHandle( m_hEvent );
	CloseHandle( m_hFeedback );
}

void CRealtimeBackground::Draw(Graphics& gr, const CRectF& rc) const
{
	if(m_Sync.Try() && m_pImage.p )
	{
		CImage* pImg;
		m_pImage->CreateCopy( &pImg );

		BITMAPINFO bmi;
		pImg->GetImageInfo(&bmi);

		LPBYTE bytes = NULL;
		pImg->GetImageBytes(&bytes);

		Bitmap bmp( & bmi, bytes );
		gr.DrawImage( &bmp, rc);

		pImg->Release();
		m_Sync.Unlock();
	}
}

CSize	CRealtimeBackground::GetSize()	 const 
{
//	if( pImg->GetImageInfo(&bmi); )
	return CSize( 352 ,288);
}


UINT CRealtimeBackground::ThreadProc(void * Param)
{
	CRealtimeBackground* pRBG = (CRealtimeBackground*)Param;	
	CImage* pImg = NULL;
	
	while( WaitForSingleObject(pRBG->m_hEvent, 0) == WAIT_OBJECT_0)
	{
/*		if(pRBG->m_Sync.Try())
		{  
			pRBG->m_pImage->CreateCopy(&pImg);
			if (pImg)
			{    

			//	pRBG->m_pImage.Release();
			//	pRBG->m_pImage.p = pImg;
			//	pImg->Release();
			}
			
			pRBG->m_Sync.Unlock();*/
			if(::IsWindow(pRBG->m_pWnd->GetSafeHwnd())) pRBG->m_pWnd->Invalidate(FALSE);
			Sleep(50);
	//	}
	}
	
	::SetEvent( pRBG->m_hFeedback );
	::AfxEndThread(0);
	return 0;
}

void	CRealtimeBackground::Stop()
{
	ResetEvent( m_hEvent );
	StopRecord();
	Sleep(200);
	DWORD dw = ::WaitForSingleObject( m_hFeedback, INFINITE );
}

void	CRealtimeBackground::Play()
{

	SetEvent(m_hEvent);
	ResetEvent(m_hFeedback);
	::AfxBeginThread( ThreadProc, this );
}

/////////////////////// record ///////////////////////////////

HRESULT CRealtimeBackground::AddToRot(IUnknown *pUnkGraph) 
{
    IMoniker * pMoniker;
    IRunningObjectTable *pROT;
    if (FAILED(GetRunningObjectTable(0, &pROT))) {
        return E_FAIL;
    }
    WCHAR wsz[256];
    wsprintfW(wsz, L"FilterGraph %08x pid %08x", (DWORD_PTR)pUnkGraph, GetCurrentProcessId());
    HRESULT hr = CreateItemMoniker(L"!", wsz, &pMoniker);
    if (SUCCEEDED(hr)) {
        hr = pROT->Register(ROTFLAGS_REGISTRATIONKEEPSALIVE, pUnkGraph,pMoniker, &m_dwRegister);
        pMoniker->Release();
    }
    pROT->Release();
    return hr;
}


void CRealtimeBackground::RemoveFromRot()
{
    IRunningObjectTable *pROT;
    if (SUCCEEDED(GetRunningObjectTable(0, &pROT))) {
        pROT->Revoke(m_dwRegister);
        pROT->Release();
    }
}

class DirectShowError  
{
public:
	void operator = (HRESULT hr) 
	{
		if( hr != S_OK )
		{
			CString str;
#ifdef DIRECTX
			str = DXGetErrorString9(hr);
#else
			str.Format(_T("HResult=0x%x"), hr);
#endif
			throw RecordException( Helper::ConvertA(str) );
		}
	}
};

void	CRealtimeBackground::Record(DWORD dwFourCC, CString StrIn, CString StrOut )
{
	DirectShowError hr;

	hr =  m_pBuild.CoCreateInstance(	CLSID_CaptureGraphBuilder2 );
/*	
	CString fmt, out;
	int CameraID = GetDlgItemInt( IDC_CAMERA );
	fmt.Format("orwis://%d", CameraID);
	out.Format( "rec%d.avi", CameraID );
*/
	std::vector<wchar_t> InFile, OutFile;
#ifndef _UNICODE
	int size = mbstowcs( 0, StrIn, StrIn.GetLength());
	InFile.resize( size + 1 );
	mbstowcs( &InFile[0], StrIn, StrIn.GetLength() );

	size = mbstowcs( 0, StrOut, StrOut.GetLength());
	OutFile.resize( size + 1 );
	mbstowcs( &OutFile[0], StrOut, StrOut.GetLength() );
#else 
	InFile.insert( InFile.end(),  (LPCWSTR)StrIn, (LPCWSTR)StrIn + StrIn.GetLength() + 1);
	OutFile.insert( OutFile.end(),  (LPCWSTR)StrOut, (LPCWSTR)StrOut + StrOut.GetLength() + 1);
#endif

	CComPtr<IBaseFilter> pMux;
	hr = m_pBuild->SetOutputFileName(
			&MEDIASUBTYPE_Avi, // File type. 
			&OutFile[0],     // File name, as a wide-character string.
			&pMux,             // Receives a pointer to the multiplexer.
			NULL);             // Receives a pointer to the file writer. 


	CComPtr<IGraphBuilder> pGraph ;
	hr = m_pBuild->GetFiltergraph(&pGraph);

	CComPtr<IBaseFilter> pSrc = NULL;
	hr = pGraph->AddSourceFilter(&InFile[0], L"Source Filter", &pSrc);
	CComPtr<IBaseFilter> pVComp = GetFilter(dwFourCC);
	hr = pGraph->AddFilter(pVComp, L"Compressor");

	hr = m_pBuild->RenderStream(
			NULL,       // Output pin category
			NULL,       // Media type
			pSrc,       // Source filter
			pVComp,     // Compression filter
			pMux);      // Sink filter (the AVI Mux)

//	hr = pBuild->RenderStream(NULL, NULL, pSrc, NULL, pMux);

	hr = AddToRot(pGraph);


    CComQIPtr< IMediaControl, &IID_IMediaControl > pControl(pGraph );
	hr = pControl->Run( );

}

void	CRealtimeBackground::StopRecord()
{
	HRESULT hr;

	if( !m_pBuild ) return;
	CComPtr<IGraphBuilder> pGraph ;

	RemoveFromRot();
	hr = m_pBuild->GetFiltergraph(&pGraph);
    CComQIPtr< IMediaControl, &IID_IMediaControl > pControl(pGraph );
	hr = pControl->Stop( );
	m_pBuild.Release();
}


CComPtr<IBaseFilter>	CRealtimeBackground::GetFilter(DWORD dwFourCC)
{
	HRESULT hr;
	CComPtr<IBaseFilter> pFilter;
    CComPtr<IMoniker> pMoniker ;

		WCHAR szFormat[] = L"@device:cm:{33D9A760-90C8-11D0-BD43-00A0C911CE86}\\";
		CHAR	Char4[4] = 
				{
					(dwFourCC & 0xFF),
					(dwFourCC & 0xFF00) >> 8,
					(dwFourCC & 0xFF0000) >> 16,
					dwFourCC >> 24
				};
		
		WCHAR	CharW[5];
		mbtowc( CharW+0, Char4+0, 1 );
		mbtowc( CharW+1, Char4+1, 1 );
		mbtowc( CharW+2, Char4+2, 1 );
		mbtowc( CharW+3, Char4+3, 1 );
		CharW[4] = L'\0';

		std::wstring strMon( szFormat );
		strMon += CharW;

		IBindCtx *pBindCtx;
		hr = CreateBindCtx(0, &pBindCtx);

		ULONG chEaten = 0;
		hr = MkParseDisplayName(pBindCtx, strMon.c_str(), &chEaten, &pMoniker.p);
		pBindCtx->Release();
		if (SUCCEEDED(hr))
		{
								
			// Get the display name, or bind to a DirectShow filter.
			hr = pMoniker->BindToObject( 0, 0, IID_IBaseFilter, (void**)&pFilter.p );
		}
	pMoniker.Release();
	return pFilter;	
}

// this dummy implementation is provided to avoid dependency on Elvees's camera translation software
class CDummyImage: public CImage
{
public:
	virtual void AddRef() override
	{
		++m_ref;
	}
	virtual void Release() override
	{ 
		if (--m_ref)
		{
			delete this;
		}
	}

	virtual void CreateCopy(CImage** ppImage) override 
	{
		*ppImage = new CDummyImage{};
	}
	virtual void GetImageInfo(BITMAPINFO* bmi) override
	{
		*bmi = {};
		BITMAPINFOHEADER& header = bmi->bmiHeader;
		header.biSize = sizeof(BITMAPINFOHEADER);
		header.biWidth = 320;
		header.biHeight = 240;
		header.biBitCount = 24;
		header.biCompression = 0;
		header.biSizeImage = header.biWidth * header.biHeight * header.biBitCount / 3;
	}
	virtual void GetImageBytes(LPBYTE* bytes) override 
	{
		memset(bytes, 0, 320 * 240 * 3);
	}
private:
	std::atomic<int> m_ref{1};
};

bool CreateImage(CImage** ppImage, int CamID)
{
	*ppImage = new CDummyImage{};
	return true;
}