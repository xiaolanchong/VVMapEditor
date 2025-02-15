/****************************************************************************
 *																			*		 
 *								GuiToolKit  								*	
 *							 (MFC extension)								*			 
 * Created by Francisco Campos G. www.beyondata.com fcampos@beyondata.com	*
 *--------------------------------------------------------------------------*		   
 *																			*
 * This program is free software;so you are free to use it any of your		*
 * applications (Freeware, Shareware, Commercial),but leave this header		*
 * intact.																	*
 *																			*
 * These files are provided "as is" without warranty of any kind.			*
 *																			*
 *			       GuiToolKit is forever FREE CODE !!!!!					*
 *																			*
 *--------------------------------------------------------------------------*
 * Created by: Francisco Campos G.											*
 * Bug Fixes and improvements : (Add your name)								*
 * -Francisco Campos														*				
 *																			*	
 ****************************************************************************/


#include "stdafx.h"
#include "GuiDrawLayer.h"
#include "DllResource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(GuiTabbedButton, CGuiNormalButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
END_MESSAGE_MAP()


GuiTabbedButton::GuiTabbedButton()
{
	bPress=FALSE;
	m_iMilliseconds=50;
}

GuiTabbedButton::~GuiTabbedButton()
{

}


void GuiTabbedButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CGuiNormalButton::OnLButtonUp(nFlags, point);
	if (!bPress) return;
	bPress=FALSE;
	KillTimer(1001);
	
}
void GuiTabbedButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CGuiNormalButton::OnLButtonDown(nFlags, point);
	if (bPress) return;
	bPress=TRUE;
	SetTimer(1001,m_iMilliseconds,NULL);
	
}

void GuiTabbedButton::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	CGuiNormalButton::OnTimer(nIDEvent);
	if (bPress == FALSE) return;
	CRect rc;
	CPoint pt(GetMessagePos());
	ScreenToClient(&pt);
	GetClientRect(rc);
	if (rc.PtInRect(pt))
	{
		CWnd* pParent=GetParent();
		if (IsWindowEnabled())
			pParent->SendMessage(WM_COMMAND,GetDlgCtrlID());
		//bPress=FALSE;
		//KillTimer(1);
	}
	else
	{
		bPress=FALSE;
		KillTimer(1001);
		CGuiNormalButton::OnTimer(nIDEvent);
	}
	
}

//------------------------------------------------------------------------------






COLORREF GuiDrawLayer::DarkenColor( long lScale, COLORREF lColor)
{ 
  long R = MulDiv(GetRValue(lColor),(255-lScale),255);
  long G = MulDiv(GetGValue(lColor),(255-lScale),255);
  long B = MulDiv(GetBValue(lColor),(255-lScale),255);

  return RGB(R, G, B); 
}

GuiDrawLayer::GuiDrawLayer()
{
	CDllResource DllRes;
	if (!m_img.Attach( DllRes.LoadImageList( IDB_DOCKBAR,9,10,RGB(255,0,255)) ))
	{
		TRACE0("error");
	}
	m_iWidthDrowDown=GetSystemMetrics(SM_CXHTHUMB);
	CBitmap cbmp;
	BITMAP bmp;
	cbmp.Attach( DllRes.LoadBitmap( IDB_DOCKBAR ));
	cbmp.GetBitmap(&bmp);
	mHeight=CPoint(bmp.bmWidth,bmp.bmHeight);	
	

}

COLORREF GuiDrawLayer::GetRGBColorWhite()
{
	return RGB(255,255,255);
}

COLORREF GuiDrawLayer::GetRGBColorTabs()
{
	BYTE byRvalue =GetRValue(GetRGBColorXP());
	BYTE byGvalue =GetGValue(GetRGBColorXP());
	BYTE byBvalue =GetBValue(GetRGBColorXP());
//	byRvalue =byRvalue>>2;
//	byGvalue =byGvalue>>2;
//	byBvalue =byBvalue>>2;
	int iMaximo=max(byRvalue,byGvalue);
	iMaximo=max(iMaximo,byBvalue);
	iMaximo= 255-iMaximo;
	iMaximo=iMaximo-(int)(iMaximo*0.51);
	return RGB(byRvalue+iMaximo,byGvalue+iMaximo,byBvalue+iMaximo);	
	
}

/*BYTE byRvalue =GetRValue(::GetSysColor(COLOR_HIGHLIGHT));
	BYTE byGvalue =GetGValue(::GetSysColor(COLOR_HIGHLIGHT));
	BYTE byBvalue =GetBValue(::GetSysColor(COLOR_HIGHLIGHT));
	byRvalue =byRvalue>>2;
	byGvalue =byGvalue>>2;
	byBvalue =byBvalue>>2;
	int iMaximo=max(byRvalue,byGvalue);
	iMaximo=max(iMaximo,byBvalue);
	iMaximo= 255-iMaximo;
	iMaximo=iMaximo-(int)(iMaximo*0.14);
	return RGB(byRvalue+iMaximo,byGvalue+iMaximo,byBvalue+iMaximo);	
*/

COLORREF GuiDrawLayer::GetRGBSkinMenu()
{
	BYTE byRvalue =GetRValue(GuiDrawLayer::GetRGBColorFace());
	BYTE byGvalue =GetGValue(GuiDrawLayer::GetRGBColorFace());
	BYTE byBvalue =GetBValue(GuiDrawLayer::GetRGBColorFace());
	byRvalue =byRvalue>>2;
	byGvalue =byGvalue>>2;
	byBvalue =byBvalue>>2;
	int iMaximo=max(byRvalue,byGvalue);
	iMaximo=max(iMaximo,byBvalue);
	iMaximo= 255-iMaximo;
	iMaximo=iMaximo-(int)(iMaximo*0.018);
	return RGB(byRvalue+iMaximo,byGvalue+iMaximo,byBvalue+iMaximo);	
	
}

COLORREF GuiDrawLayer::GetRGBColorBTNHigh()
{
	return ::GetSysColor(COLOR_BTNHIGHLIGHT);
}



COLORREF GuiDrawLayer::GetRGBColorGrayText()
{
	BYTE byRvalue =0;
	BYTE byGvalue =0;
	BYTE byBvalue =0;
	byRvalue =byRvalue>>1;
	byGvalue =byGvalue>>1;
	byBvalue =byBvalue>>1;
	int iMaximo=max(byRvalue,byGvalue);
	iMaximo=max(iMaximo,byBvalue);
	iMaximo= 255-iMaximo;
	iMaximo=iMaximo-(int)(iMaximo*0.60);
	return RGB(byRvalue+iMaximo,byGvalue+iMaximo,byBvalue+iMaximo);	
	
}


COLORREF GuiDrawLayer::GetRGBColorFace()
{
	return ::GetSysColor(COLOR_BTNFACE);
}

COLORREF GuiDrawLayer::GetRGBColorShadow()
{
	return ::GetSysColor(COLOR_BTNSHADOW);
}

COLORREF GuiDrawLayer::GetRGBPressBXP()
{
	COLORREF clrPress = ::GetSysColor (COLOR_HIGHLIGHT);

	BYTE byRvalue =GetRValue(::GetSysColor(COLOR_HIGHLIGHT));
	BYTE byGvalue =GetGValue(::GetSysColor(COLOR_HIGHLIGHT));
	BYTE byBvalue =GetBValue(::GetSysColor(COLOR_HIGHLIGHT));
	byRvalue =byRvalue>>2;
	byGvalue =byGvalue>>2;
	byBvalue =byBvalue>>2;
	int iMaximo=max(byRvalue,byGvalue);
	iMaximo=max(iMaximo,byBvalue);
	iMaximo= 255-iMaximo;
	iMaximo=iMaximo-(int)(iMaximo*0.21);
	return RGB(byRvalue+iMaximo,byGvalue+iMaximo,byBvalue+iMaximo);	
	
}

COLORREF GuiDrawLayer::GetRGBMenu()
{
	COLORREF clrPress = ::GetSysColor (COLOR_BTNSHADOW);

	BYTE byRvalue =GetRValue(::GetSysColor(COLOR_HIGHLIGHT));
	BYTE byGvalue =GetGValue(::GetSysColor(COLOR_HIGHLIGHT));
	BYTE byBvalue =GetBValue(::GetSysColor(COLOR_HIGHLIGHT));
	byRvalue =byRvalue>>3;
	byGvalue =byGvalue>>3;
	byBvalue =byBvalue>>3;
	int iMaximo=max(byRvalue,byGvalue);
	iMaximo=max(iMaximo,byBvalue);
	iMaximo= 255-iMaximo;
	iMaximo=iMaximo-(int)(iMaximo*0.47);
	return RGB(byRvalue+iMaximo,byGvalue+iMaximo,byBvalue+iMaximo);	
	
}

COLORREF GuiDrawLayer::GetRGBTitleMenu()
{
	COLORREF clrPress = ::GetSysColor (COLOR_BTNSHADOW);

	BYTE byRvalue =GetRValue(::GetSysColor(clrPress));
	BYTE byGvalue =GetGValue(::GetSysColor(clrPress));
	BYTE byBvalue =GetBValue(::GetSysColor(clrPress));
	byRvalue =byRvalue>>2;
	byGvalue =byGvalue>>2;
	byBvalue =byBvalue>>2;
	int iMaximo=max(byRvalue,byGvalue);
	iMaximo=max(iMaximo,byBvalue);
	iMaximo= 255-iMaximo;
	iMaximo=iMaximo-(int)(iMaximo*0.8);
	return RGB(byRvalue+iMaximo,byGvalue+iMaximo,byBvalue+iMaximo);	
	
}

COLORREF GuiDrawLayer::GetRGBCaptionXP()
{

	BYTE byRvalue =GetRValue(::GetSysColor(COLOR_HIGHLIGHT));
	BYTE byGvalue =GetGValue(::GetSysColor(COLOR_HIGHLIGHT));
	BYTE byBvalue =GetBValue(::GetSysColor(COLOR_HIGHLIGHT));
	return RGB(byRvalue+10,byGvalue+10,byBvalue+10);	
}

COLORREF GuiDrawLayer::GetRGBLightCaptionXP()
{

	BYTE byRvalue =GetRValue(::GetSysColor(COLOR_HIGHLIGHT));
	BYTE byGvalue =GetGValue(::GetSysColor(COLOR_HIGHLIGHT));
	BYTE byBvalue =GetBValue(::GetSysColor(COLOR_HIGHLIGHT));
	return RGB((byRvalue+640)/4,(byGvalue+640)/4,(byBvalue+640)/4);	
}



COLORREF GuiDrawLayer::GetRGBDarkCaptionXP()
{

	BYTE byRvalue =GetRValue(::GetSysColor(COLOR_HIGHLIGHT));
	BYTE byGvalue =GetGValue(::GetSysColor(COLOR_HIGHLIGHT));
	BYTE byBvalue =GetBValue(::GetSysColor(COLOR_HIGHLIGHT));
	return RGB(byRvalue * 2/3,byGvalue * 2/3,byBvalue * 2/3);	
}

COLORREF GuiDrawLayer::GetRGBFondoXP()
{
	BYTE byRvalue =GetRValue(::GetSysColor(COLOR_HIGHLIGHT));
	BYTE byGvalue =GetGValue(::GetSysColor(COLOR_HIGHLIGHT));
	BYTE byBvalue =GetBValue(::GetSysColor(COLOR_HIGHLIGHT));
	byRvalue =byRvalue>>2;
	byGvalue =byGvalue>>2;
	byBvalue =byBvalue>>2;
	int iMaximo=max(byRvalue,byGvalue);
	iMaximo=max(iMaximo,byBvalue);
	iMaximo= 255-iMaximo;
	iMaximo=iMaximo-(int)(iMaximo*0.14);
	return RGB(byRvalue+iMaximo,byGvalue+iMaximo,byBvalue+iMaximo);	
}

COLORREF GuiDrawLayer::GetRGBLightFondoXP()
{
	COLORREF crFondo = GetRGBFondoXP();
	BYTE byRvalue =GetRValue(crFondo);
	BYTE byGvalue =GetGValue(crFondo);
	BYTE byBvalue =GetBValue(crFondo);
	return RGB((byRvalue+640)/4,(byGvalue+640)/4,(byBvalue+640)/4);
}


COLORREF GuiDrawLayer::GetRGBDarkFondoXP()
{
	BYTE byRvalue =GetRValue(::GetSysColor(COLOR_HIGHLIGHT));
	BYTE byGvalue =GetGValue(::GetSysColor(COLOR_HIGHLIGHT));
	BYTE byBvalue =GetBValue(::GetSysColor(COLOR_HIGHLIGHT));
	byRvalue =byRvalue;
	byGvalue =byGvalue;
	byBvalue =byBvalue;
	int iMaximo=max(byRvalue,byGvalue);
	iMaximo=max(iMaximo,byBvalue);
	iMaximo= 255-iMaximo;
	iMaximo=iMaximo-(int)(iMaximo*0.4);
	return RGB(byRvalue+iMaximo,byGvalue+iMaximo,byBvalue+iMaximo);	
}

BOOL GuiDrawLayer::IsGrayed()
{
	BYTE byRvalue =GetRValue(::GetSysColor(COLOR_3DFACE));
	BYTE byGvalue =GetGValue(::GetSysColor(COLOR_3DFACE));
	BYTE byBvalue =GetBValue(::GetSysColor(COLOR_3DFACE));
	if ((byRvalue ==byGvalue) && (byGvalue == byBvalue) &&
		 (byBvalue > 190 && byBvalue < 193) || (byBvalue == 216))
		return TRUE;
	else
		return FALSE;
}

COLORREF GuiDrawLayer::GetRGBColorXP()
{

	
	COLORREF clr3DFace = ::GetSysColor (COLOR_3DFACE);
	return RGB (((3*GetRValue (clr3DFace)+240)/4)+1,
				((3*GetGValue (clr3DFace)+240)/4)+1,
				((3*GetBValue (clr3DFace)+240)/4)+1);

}

GuiDrawLayer::~GuiDrawLayer()
{

}

HICON GuiDrawLayer::LoadIconLib(UINT uIcon)
{
	//NOTENOTE: Not from global resource storage but from our resource
/*	HINSTANCE hIrs = AfxFindResourceHandle(MAKEINTRESOURCE(uIcon),
											RT_GROUP_ICON);*/
	HINSTANCE hIrs = CDllResource().GetHandle();
	return (HICON)::LoadImage(hIrs, MAKEINTRESOURCE(uIcon), IMAGE_ICON,16, 16, 0);
}

void GuiDrawLayer::DrawArrow(CDC* pDC,CRect m_rc,BOOL m_bDown)
{
	int difh =m_rc.Height()-mHeight.y;
	difh/=2;
	m_rc.left=m_rc.right-m_iWidthDrowDown;
	m_img.Draw(pDC,m_bDown?0:2,CPoint(m_rc.left+2,m_rc.top+difh),ILD_TRANSPARENT);		

}

/*
	HEUZ modifiyng	: check state
*/

void GuiDrawLayer::DrawCheck(CDC* pDC,CRect m_rcTemp)
{
//	const int LeftLineSize = 3;
//	const int RightLineSize = 8;
	int iMediaAltura=(m_rcTemp.Height()/2)-2;
	int iMedioBox= m_rcTemp.Width()/2;
	int iSize = min( m_rcTemp.Height(), m_rcTemp.Width() );
	iSize -= iSize%3;
	CPoint pt = m_rcTemp.CenterPoint();
	pt.Offset( -iSize/2, -iSize/2 - 3 );
	CPen cp(PS_SOLID,1,GuiDrawLayer::GetRGBCaptionXP());
	CPen *pOld=pDC->SelectObject(&cp);
	
/*	pDC->MoveTo(m_rcTemp.left+1,m_rcTemp.top+iMediaAltura+4);
	pDC->LineTo(m_rcTemp.left+3,m_rcTemp.bottom-1);
	pDC->LineTo(m_rcTemp.right-1,m_rcTemp.top+2);
				
	pDC->MoveTo(m_rcTemp.left+1,m_rcTemp.top+iMediaAltura+3);
	pDC->LineTo(m_rcTemp.left+3,m_rcTemp.bottom-2);
	pDC->LineTo(m_rcTemp.right-1,m_rcTemp.top+1);

	pDC->MoveTo(m_rcTemp.left+1,m_rcTemp.top+iMediaAltura+2);
	pDC->LineTo(m_rcTemp.left+3,m_rcTemp.bottom-3);
	pDC->LineTo(m_rcTemp.right-1,m_rcTemp.top);*/
	pDC->MoveTo( pt.x, pt.y + 2*iSize/3 );
	pDC->LineTo( pt.x + iSize/3, pt.y + iSize );
	pDC->LineTo( pt.x + iSize, pt.y + iSize/3 );

	pDC->MoveTo( pt.x, pt.y + 2*iSize/3 + 1 );
	pDC->LineTo( pt.x + iSize/3, pt.y + iSize + 1 );
	pDC->LineTo( pt.x + iSize, pt.y + iSize/3 + 1 );

	pDC->MoveTo( pt.x, pt.y + 2*iSize/3 + 2);
	pDC->LineTo( pt.x + iSize/3, pt.y + iSize +2 );
	pDC->LineTo( pt.x + iSize, pt.y + iSize/3 + 2);
	
	pDC->SelectObject(pOld);
}

HICON GuiDrawLayer::GetIcon(int nIcon)
{
	return m_img.ExtractIcon(nIcon);
}

//function of Bruno Podetti

void GuiDrawLayer::DrawShade( CRect Rect,CPoint screen,CDC* pDC)
{ 
//  if(!SystemParametersInfo(SPI_GETDROPSHADOW,0,&bEnabled,0))
  //  return;

  // Get the size of the menu... 

  long winW = Rect.Width(); 
  long winH = Rect.Height(); 
  long xOrg = screen.x;  
  long yOrg = screen.y;

  // Get the desktop hDC... 
  HDC hDcDsk = GetWindowDC(0) ;
  
  int X,Y;
  // Simulate a shadow on right edge... 
  for (X=1; X<=2 ;X++)
  { 
    for (Y=0; Y<2 ;Y++)
    {
      SetPixel(pDC->m_hDC,winW-X,Y, GetPixel(hDcDsk,xOrg+winW-X,yOrg+Y) );
    }
    for (Y=4; Y<8 ;Y++)
    {
      COLORREF c = GetPixel(hDcDsk, xOrg + winW - X, yOrg + Y) ;
      SetPixel(pDC->m_hDC,winW-X,Y,DarkenColor(3 * X * (Y - 3), c)) ;
    }
    for (Y=8; Y<=(winH-5) ;Y++)
    {
      COLORREF c = GetPixel(hDcDsk, xOrg + winW - X, yOrg + Y) ;
      SetPixel(pDC->m_hDC, winW - X, Y, DarkenColor(15 * X, c) );
    }
    for (Y=(winH-4); Y<=(winH-1) ;Y++)
    {
      COLORREF c = GetPixel(hDcDsk, xOrg + winW - X, yOrg + Y) ;
      SetPixel( pDC->m_hDC, winW - X, Y, DarkenColor(3 * X * -(Y - winH), c)) ;
    }
  } 

  // Simulate a shadow on the bottom edge... 
  for(Y=1; Y<=2 ;Y++)
  {
    for(X=0; X<=2 ;X++)
    {
      SetPixel(pDC->m_hDC,X,winH-Y, GetPixel(hDcDsk,xOrg+X,yOrg+winH-Y)) ;
    }
    for(X=4; X<=7 ;X++)
    {
      COLORREF c = GetPixel(hDcDsk, xOrg + X, yOrg + winH - Y) ;
      SetPixel(pDC->m_hDC, X, winH - Y, DarkenColor(3 * (X - 3) * Y, c)) ;
    }
    for(X=8; X<=(winW-5) ;X++)
    {
      COLORREF  c = GetPixel(hDcDsk, xOrg + X, yOrg + winH - Y); 
      SetPixel(pDC->m_hDC, X, winH - Y, DarkenColor(15 * Y, c)) ;
    }
  }   
  // Release the desktop hDC... 
  ReleaseDC(0,hDcDsk);
}



CGuiControlBarButton::CGuiControlBarButton()
{
	bTypeButton=GUINORMAL;
	m_bEnabled=TRUE;
}


CGuiControlBarButton::~CGuiControlBarButton()
{

}

void CGuiControlBarButton::SetTypeButton(GuiType bTypeb)
{
	bTypeButton=bTypeb;
}


void CGuiControlBarButton::SetEnabled(BOOL bEnabled)
{
	m_bEnabled=bEnabled;
}

void CGuiControlBarButton::Paint(CDC* pDC,int st,CRect rc,COLORREF clrFondo)
{
	CBrush cb;
	if(bTypeButton==GUINORMAL) 
		cb.CreateSolidBrush(clrFondo);
	else
	{
		if (m_bEnabled)
			if (st == OVERBTN)
				cb.CreateSolidBrush(GuiDrawLayer::GetRGBFondoXP());
			else if(st == PRESSBTN)
				cb.CreateSolidBrush(GuiDrawLayer::GetRGBPressBXP());
			else
				cb.CreateSolidBrush(clrFondo);
	}

	if (m_bEnabled)
	{
		pDC->FillRect(rc,&cb);
		if (st == OVERBTN)
			pDC->Draw3dRect(rc,bTypeButton==GUINORMAL?::GetSysColor(COLOR_BTNHIGHLIGHT):GuiDrawLayer::GetRGBCaptionXP(),
							   bTypeButton==GUINORMAL?::GetSysColor(COLOR_BTNSHADOW):GuiDrawLayer::GetRGBCaptionXP());
		if (st == PRESSBTN)
			pDC->Draw3dRect(rc,bTypeButton==GUINORMAL?::GetSysColor(COLOR_BTNSHADOW):GuiDrawLayer::GetRGBCaptionXP(),
			bTypeButton==GUINORMAL?::GetSysColor(COLOR_BTNHIGHLIGHT):GuiDrawLayer::GetRGBCaptionXP());
	}
	if(bTypeButton==GUINORMAL) 
		m_imgList.Draw(pDC,clrFondo == ::GetSysColor(COLOR_ACTIVECAPTION)? m_nIcon+1:m_nIcon, 
						CPoint(rc.left+1,rc.top+2), ILD_TRANSPARENT);				
	else
	{
		HICON m_Icon=m_imgList.ExtractIcon(m_nIcon);
		//if (m_bEnabled)
		IMAGEINFO ImageInfo;
		
		m_imgList.GetImageInfo(m_nIcon,&ImageInfo);
		CRect rcbt=ImageInfo.rcImage;
		int difh= rc.Height()-rcbt.Height();
		int difw= rc.Width()-rcbt.Width();
		if (difh< 0) difh=0;
		if (difw< 0) difw=1;
		difh/=2;
		difw/=2;
		m_imgList.Draw(pDC,m_nIcon, CPoint(rc.left+1+difw,bTypeButton==GUINORMAL?rc.top+difh+1:rc.top+difh+1),m_bEnabled?ILD_TRANSPARENT:ILD_BLEND25);				
		
		DestroyIcon (m_Icon);

	
	}
	cb.DeleteObject();
}

void CGuiControlBarButton::SetData(int nIcon,LPCTSTR lpMsg)
{
	m_nIcon=nIcon;
	m_lpMsg=lpMsg;
}

void  CGuiControlBarButton::SetImageList(UINT nBitmapID, int cx, int nGrow, COLORREF crMask)
{
#if 0
	CBitmap cbmp;
	BITMAP bmp;
	m_imgList.Create(nBitmapID,cx,nGrow,crMask);
	cbmp.LoadBitmap(nBitmapID);
	cbmp.GetBitmap(&bmp);
	m_imgList.Add(&cbmp,crMask);
#endif
	m_imgList.Create( nBitmapID,cx,nGrow,crMask );
}

void  CGuiControlBarButton::SetImageList(HINSTANCE hInst, UINT nBitmapID, int cx, int nGrow, COLORREF crMask)
{
	LPCTSTR szResource = MAKEINTRESOURCE( nBitmapID );
	m_imgList.Attach( ::ImageList_LoadBitmap(hInst, szResource,cx,nGrow,crMask) );
}
