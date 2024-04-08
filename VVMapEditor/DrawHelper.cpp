// DrawHelper.cpp: implementation of the DrawHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VVMapEditor.h"
#include "DrawHelper.h"
//#include "Setings.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DrawHelper::DrawHelper()
{

}

DrawHelper::~DrawHelper()
{

}

void	DrawHelper::DrawPointGroup( Graphics& gr, PointF point, int Number, Brush& br)
{
   FontFamily  fontFamily(L"Arial");
   Font        font(&fontFamily, 14, FontStyleRegular, UnitPixel);
   
   StringFormat stringFormat;
   stringFormat.SetLineAlignment(StringAlignmentFar);
	stringFormat.SetAlignment(StringAlignmentNear);
	
	WCHAR	szBuffer[10];
	_snwprintf( szBuffer, 10, L"%d", Number );
	point.X += 2;
	point.Y += 2;
	gr.DrawString( szBuffer, -1, &font, point, &stringFormat,&br );

}

void	DrawHelper::DrawPointNumber(Graphics& gr, PointF point, int Number, Brush& br)
{
	FontFamily  fontFamily(L"Times New Roman");
	Font        font(&fontFamily, 14, FontStyleRegular, UnitPixel);
   
	StringFormat stringFormat;
	stringFormat.SetLineAlignment(StringAlignmentFar);
	stringFormat.SetAlignment(StringAlignmentNear);
	
	WCHAR	szBuffer[10];
	_snwprintf( szBuffer, 10, L"%d", Number );

//	SolidBrush br( Color::Red );
	gr.DrawString( szBuffer, -1, &font, point, &stringFormat,&br );	
}