// EmptyBackground.cpp: implementation of the CEmptyBackground class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EmptyBackground.h"
#include "Helper.h"
//#include "EditorState.h"
//#include "ResourceMgr.h"
//#include <boost/format.hpp>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//std::wstring CEmptyBackground::s_NoImage;


CEmptyBackground::CEmptyBackground()
{
/*	if( s_NoImage.empty() )	
	{
		CString str;
		str.LoadString( IDS_NOIMAGE );
		s_NoImage = Helper::Convert( str );
	}*/
}

CEmptyBackground::~CEmptyBackground()
{

}

void CEmptyBackground::Draw(Graphics& gr, const CRectF& rc) const
{
   FontFamily  fontFamily(L"Arial");
   Font        font(&fontFamily, 24, FontStyleRegular, UnitPixel);
   
   StringFormat stringFormat;
   stringFormat.SetLineAlignment(StringAlignmentCenter);
	stringFormat.SetAlignment(StringAlignmentCenter);

//	GetResourceMgr().GetAnsi( IDS_NOIMAGE )
	//FIXME: Unicode hack
	CString strImage;
	strImage.LoadString( IDS_NO_IMAGE );
	std::wstring str = strImage;			// RESOURCE!!!

	gr.DrawString( str.c_str(), -1, &font, rc, &stringFormat, &SolidBrush( Color::Black ));


}
