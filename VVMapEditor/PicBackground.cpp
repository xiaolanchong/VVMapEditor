// PicBackground.cpp: implementation of the CPicBackground class.
//
//////////////////////////////////////////////////////////////////////

#pragma warning( disable : 4786 )

#include "stdafx.h"
#include "PicBackground.h"
#include "Helper.h"
//#include "ResourceMgr.h"
//#include "EditorState.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/*
CFileBackground::CFileBackground(const std::wstring& FileName):
	m_Image( FileName.c_str(), FALSE )
{
	if (m_Image.GetLastStatus() != Ok) 
	{
		CString str ;
		str.LoadString( IDS_EXC_INVALID_IMAGE_FILE );
		throw ImageFileException( str );
	}
}

CFileBackground::~CFileBackground()
{

}


void CFileBackground::Draw(Graphics& gr, const CRectF& rc) const
{
	Image* img = const_cast<Image*>(&m_Image);
	gr.DrawImage( img, rc );
}

CSize	CFileBackground::GetSize() const
{
	Image* img = const_cast<Image*>(&m_Image);
	return CSize(img->GetWidth(), img->GetHeight());
}
*/
CBitmapBackground::CBitmapBackground(BITMAPINFO* pbi, void * pbits):
	m_bi(*pbi),
	m_bits( new BYTE[ m_bi.bmiHeader.biSizeImage ] ),
	m_copy( m_bits.get(), (BYTE*)pbits, m_bi.bmiHeader.biSizeImage ),
	m_Image( &m_bi, m_bits.get() )
{
	if (m_Image.GetLastStatus() != Ok) 
	{
		CString str ;
		str.LoadString( IDS_EXC_INVALID_IMAGE_FILE );
		throw ImageFileException( Helper::ConvertA( str ) );
	}
}

CBitmapBackground::CBitmapBackground(const std::wstring& FileName):
	m_Image( FileName.c_str(), FALSE )
{
	if (m_Image.GetLastStatus() != Ok) 
	{
		CString str ;
		str.LoadString( IDS_EXC_INVALID_IMAGE_FILE );
		throw ImageFileException( Helper::ConvertA( str ) );
	}
}



void CBitmapBackground::Draw(Graphics& gr, const CRectF& rc) const
{
	Bitmap* img = const_cast<Bitmap*>(&m_Image);
	gr.DrawImage( img, rc );
}

CSize	CBitmapBackground::GetSize() const
{
	Image* img = const_cast<Bitmap*>(&m_Image);
	return CSize(img->GetWidth(), img->GetHeight());
}


CBitmapBackground:: ~CBitmapBackground()
{
//	m_bits.reset();
//	OutputDebugString( "~CBitmapBackground\n" );
}