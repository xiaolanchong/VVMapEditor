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
#include "GuiLib.h"
#include "Guiminiframe.h"
#include "..\header\guiminiframe.h"
#include "GuiDrawlayer.h"
#include "GuiControlBar.h"
#include "GuiToolBarWnd.h"
#include "MenuBar.h"
#include "DllResource.h"
// CGuiMiniFrame

IMPLEMENT_DYNCREATE(CGuiMiniFrame, CMiniDockFrameWnd)


//*********************

BEGIN_MESSAGE_MAP(CGuiMiniFrame, CMiniDockFrameWnd)
	//{{AFX_MSG_MAP(CGuiMiniFrame)
	ON_WM_CLOSE()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCLBUTTONUP()
	ON_WM_NCRBUTTONDOWN()
	ON_WM_NCLBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
	ON_WM_MOUSEACTIVATE()
	ON_WM_CREATE()
	ON_WM_NCCALCSIZE()
	ON_WM_NCPAINT()
	ON_WM_NCACTIVATE()
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_NCHITTEST()
	ON_WM_NCMOUSEMOVE()

END_MESSAGE_MAP()

int CGuiMiniFrame::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	if (nHitTest >= HTSIZEFIRST && nHitTest <= HTSIZELAST) // resizing
		return MA_ACTIVATE   ;
	return CMiniDockFrameWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

CGuiMiniFrame::CGuiMiniFrame() //: m_wndDockBar(TRUE)
{
	IsGuiControlBar=-1;
	m_stateBtn=NORMAL;
	IsMenuBar=FALSE;
//	m_dwAllowDockingState = 0;
//	m_wndDockBar.m_bAutoDelete = FALSE;
}


#define _countof(array) (sizeof(array)/sizeof(array[0]))
void CGuiMiniFrame::RecalcLayout(BOOL bNotify)
{
	if (!m_bInRecalcLayout)
	{
		CMiniDockFrameWnd::RecalcLayout(bNotify);

		// syncronize window text of frame window with dockbar itself
		TCHAR szTitle[_MAX_PATH];
		m_wndDockBar.GetWindowText(szTitle, _countof(szTitle));
		AfxSetWindowText(m_hWnd, szTitle);
	}
}

void CGuiMiniFrame::OnClose()
{
	CWnd* pBar=GetChildWnd();
	if (pBar && pBar->IsKindOf(RUNTIME_CLASS(CGuiControlBar)))
	{
		static_cast<CGuiControlBar*>( pBar ) -> OnCloseBtn();
	}
	else
		m_wndDockBar.ShowAll(FALSE);
}

BOOL CGuiMiniFrame::Create(CWnd* pParent, DWORD dwBarStyle)
{
	// set m_bInRecalcLayout to avoid flashing during creation
	// RecalcLayout will be called once something is docked
	if(!CMiniDockFrameWnd::Create(pParent, dwBarStyle))
		return FALSE;
	ModifyStyle( MFS_4THICKFRAME, 0 );
	ModifyStyleEx( 0, WS_EX_TOOLWINDOW );
	ModifyStyle( WS_SYSMENU, 0 );
	CDllResource DllRes;
	m_btn.SetData(6, DllRes.LoadString( IDS_CLOSE ));
	m_btn.SetImageList(IDB_DOCKBAR,9,10,RGB(255,0,255));
	m_btn.SetTypeButton(CGuiControlBarButton::GUITOOLBUTTON);
	SetTimer(1010,100,NULL);
	
	return TRUE;
}

void CGuiMiniFrame::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
//	SetTimer(1,100,NULL);
	if (nHitTest == HTCLOSE)
	{
		m_stateBtn=PRESSBTN;
		SetTimer(1,100,0);
		SendMessage(WM_NCPAINT);
	
		CWnd* pBar=GetChildWnd();
		if (pBar== NULL) return;
		if (pBar->IsKindOf(RUNTIME_CLASS(CGuiToolBarWnd)))
		{
			m_stateBtn=NORMAL;
			KillTimer(1);
			GetParentFrame()->ShowControlBar((CControlBar*)pBar, FALSE, FALSE);
		}
	}
	else 
	{
//		SetCursor (LoadCursor(NULL, IDC_SIZEALL));
		SendMessage(WM_NCPAINT);//SendMessage(WM_NCCALCSIZE);
		CWnd* pBar=GetChildWnd();
		if (pBar->IsKindOf(RUNTIME_CLASS(CGuiToolBarWnd)))
		{
			if (nHitTest != HTCAPTION)
			 return;
		}
	}
	SendMessage(WM_NCPAINT);
	CMiniDockFrameWnd::OnNcLButtonDown(nHitTest, point);
}

void CGuiMiniFrame::OnNcLButtonUp(UINT nHitTest, CPoint point)
{
//	SetTimer(1,100,NULL);
	if (nHitTest == HTCLOSE)
	{
		CWnd* pBar=GetChildWnd();
		if (pBar== NULL) return;
    /*
		if (pBar->IsKindOf(RUNTIME_CLASS(CGuiToolBarWnd)))
		{
    */
			m_stateBtn=NORMAL;
			KillTimer(1);
			GetParentFrame()->ShowControlBar((CControlBar*)pBar, FALSE, FALSE);
    /*
		}
    */	
	}
	SendMessage(WM_NCPAINT);
	CMiniDockFrameWnd::OnNcLButtonUp(nHitTest, point);
}

void CGuiMiniFrame::OnNcRButtonDown(UINT nHitTest, CPoint point)
{
	if( IsGuiControlBar && nHitTest == HTCAPTION)
	{
		CWnd* pWnd = GetChildWnd();
		pWnd->SendMessage( WM_NCRBUTTONDOWN, HTCAPTION, point.y << 16 | point.x   );
	} 
	else
		CMiniDockFrameWnd::OnNcRButtonUp(nHitTest, point);
}

void CGuiMiniFrame::OnLButtonUp(UINT nHitTest, CPoint point)
{
	SetTimer(1,100,NULL);
	if (nHitTest == HTCLOSE)
	{
		CWnd* pBar=GetChildWnd();
		if (pBar== NULL) return;
		if (pBar->IsKindOf(RUNTIME_CLASS(CGuiToolBarWnd)))
		{
			m_stateBtn=NORMAL;
			KillTimer(1);
			GetParentFrame()->ShowControlBar((CControlBar*)pBar, FALSE, FALSE);
		}
	}
	SendMessage(WM_NCPAINT);

	CMiniDockFrameWnd::OnLButtonUp(nHitTest, point);
}

void CGuiMiniFrame::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
{
//	SetTimer(1,100,NULL);
	SendMessage(WM_NCACTIVATE);
	CMiniDockFrameWnd::OnNcLButtonDblClk(nHitTest, point);
}



int CGuiMiniFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMiniDockFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// TODO:  Add your specialized creation code here

	return 0;
}

void CGuiMiniFrame::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	SendMessage( WM_NCACTIVATE );
	CMiniDockFrameWnd::OnNcCalcSize(bCalcValidRects, lpncsp);	
}

void CGuiMiniFrame::OnNcPaint()
{
	if (IsGuiControlBar)
	{
		CMiniDockFrameWnd::OnNcPaint();
		return; 
	}
	CRect rcWindow;
	CRect rcClient;
	CWindowDC	dc(this);
	CDC			m_dc;		//contexto de dispositivo en memoria
	CBitmap		m_bitmap;
	//la idea es tomar el area de la ventana y area  cliente , luego debemos
	//igualar el area de coordenadas de ventana al cliente
	GetWindowRect(&rcWindow);
	GetClientRect(&rcClient);
	int nSize=rcClient.right;
	ScreenToClient(rcWindow);
    rcClient.OffsetRect(-rcWindow.TopLeft());
    rcWindow.OffsetRect(-rcWindow.TopLeft());
  	
	m_dc.CreateCompatibleDC(&dc);
	m_bitmap.CreateCompatibleBitmap(&dc,rcWindow.Width(),rcWindow.Height());
	CBitmap *m_OldBitmap=m_dc.SelectObject(&m_bitmap);
	//aqui debe utilizarse la brocha que define GuiDrawLayer, si no hacemos la siguiente
	//linea usted vera un horrible color negro, a cambio del dibujo.
	CBrush cb;
	cb.CreateSolidBrush(GuiDrawLayer::GetRGBColorShadow());
	m_dc.FillRect(rcWindow, &cb);
	
	CRect rcCaption=rcClient;
	rcCaption.InflateRect(1,1);
	
	dc.Draw3dRect(rcCaption,GuiDrawLayer::GetRGBColorBTNHigh(),GuiDrawLayer::GetRGBColorBTNHigh());
	
  DrawGripper(&m_dc,&rcClient);
	dc.IntersectClipRect(rcWindow);
    dc.ExcludeClipRect(rcClient);//asi evitamos el parpadeo

	
	//rcWindow.InflateRect(1,1);
	dc.BitBlt(rcWindow.left,rcWindow.top,rcWindow.Width(),rcWindow.Height(),&m_dc,0,0,SRCCOPY);
	ReleaseDC(&dc);
	m_dc.SelectObject(m_OldBitmap);
	m_bitmap.DeleteObject();
	m_dc.DeleteDC();
//	SetTimer(1,200,NULL);
	
	// TODO: Add your message handler code here
	// Do not call CMiniDockFrameWnd::OnNcPaint() for painting messages
}

void CGuiMiniFrame::DrawGripper(CDC* pDC,CRect* rc)
{
	CRect gripper = rc;
	gripper.top =3;
	gripper.left-=1;
	gripper.right+=2;
	gripper.bottom +=2;
	int nCaption=::GetSystemMetrics(SM_CYSMCAPTION);
	//si la ventana esta activa pintamos el caption o el area del titulo de color azul 	
	CBrush cb;
	cb.CreateSolidBrush(GuiDrawLayer::GetRGBColorShadow());//GuiDrawLayer::GetRGBCaptionXP());
	pDC->Draw3dRect(gripper,GuiDrawLayer::GetRGBColorFace(),GuiDrawLayer::GetRGBColorFace());
	CRect rcLineLeft=gripper;
	rcLineLeft.top+=nCaption+1;
	rcLineLeft.left+=2;
	rcLineLeft.right-=1;
	rcLineLeft.bottom-=1;
	pDC->Draw3dRect(rcLineLeft,GuiDrawLayer::GetRGBColorFace(),GuiDrawLayer::GetRGBColorFace());	

	gripper.DeflateRect(1, 1);
	CString m_caption;
	GetWindowText(m_caption);
	CFont m_cfont;

	m_cfont.CreateFont(-11,0,0,0,FW_BOLD,0,0,0,0,1,2,1,34,_T("Verdana"));
		
	CFont* m_fontOld=pDC->SelectObject(&m_cfont);
	int nMode = pDC->SetBkMode(TRANSPARENT);
	CSize SizeCad=pDC->GetTextExtent(m_caption);
	CRect rCText=gripper;
	rCText.top=6;
	rCText.bottom =rCText.top+14;
	
	int cont=SizeCad.cx;
	while(cont > 1 && gripper.Width() > 0)
		{
			CSize coor=pDC->GetTextExtent(m_caption,m_caption.GetLength());
			if(coor.cx > gripper.Width()-10)
			{
				m_caption=m_caption.Left(m_caption.GetLength()-1);
			}
			else
				break;
			cont--;
				
		}
	if (gripper.Width() > 0	)
	{
		pDC->SetTextColor(GuiDrawLayer::GetRGBTitleMenu());
		pDC->TextOut(rCText.left+3,rCText.top,m_caption);
	}
   
  if (GetChildWnd()->GetStyle() & WS_SYSMENU)
  {
	  //CRect gripper;
	  //------------------------------------------------
	  GetWindowRect( gripper );
	  ScreenToClient( gripper );
	  gripper.OffsetRect( -gripper.left, -gripper.top );
	  gripper.left=gripper.right-20;
	  gripper.right-=4;
	  gripper.top+=4;
	  gripper.bottom=gripper.top+nCaption;
	  m_rcCloseBtn=gripper;
	  if (IsMenuBar==FALSE)
	  {
      if (m_stateBtn==NORMAL)
        m_btn.Paint(pDC,m_stateBtn,gripper,GuiDrawLayer::GetRGBColorShadow());
      else if (m_stateBtn==OVER)
        m_btn.Paint(pDC,m_stateBtn,gripper,GuiDrawLayer::GetRGBFondoXP());
      else
        m_btn.Paint(pDC,m_stateBtn,gripper,GuiDrawLayer::GetRGBPressBXP());
	  }	
  }
	pDC->SetBkMode(nMode);
	pDC->SelectObject(m_fontOld);	
}


void CGuiMiniFrame::OnNcMouseMove(UINT nHitTest, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_stateBtn != NORMAL) return;
	if (nHitTest == HTCLOSE)
	{
		m_stateBtn=OVERBTN;
		SetTimer(1,100,0);
		SendMessage(WM_NCPAINT);
	}
	
	CMiniDockFrameWnd::OnNcMouseMove(nHitTest, point);
}



	
void CGuiMiniFrame::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	
	if (nIDEvent == 1010)
	{
		KillTimer(1010);
		SendMessage(WM_NCACTIVATE);
	}
	else
	{
		if (m_stateBtn==NORMAL) return;
		CRect rc;
		CPoint pt(GetMessagePos());
		
		CRect rcT=m_rcCloseBtn;
		ClientToScreen(rcT);
		pt.y+=23;
		pt.x+=5;
		if (!rcT.PtInRect(pt))
		{
			m_stateBtn=NORMAL;
			KillTimer(1);
			SendMessage(WM_NCPAINT); 		
		}
	}
//-------------------------------


	
	CMiniDockFrameWnd::OnTimer(nIDEvent);
}

LRESULT CGuiMiniFrame::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CRect rcWindow;
	//no se convierte las coordenadas de pantalla porque el punto
	//entregado por esta funci�n esta dado en el mismo sentido.
	GetWindowRect(rcWindow);
	int it=0;
	CRect rcT=m_rcCloseBtn;
	ClientToScreen(rcT);
	CPoint pt=point;
	pt.y+=23;
	pt.x+=5;
	if (rcT.PtInRect(pt) && (GetChildWnd()->GetStyle() & WS_SYSMENU))
		return HTCLOSE;

	return CMiniDockFrameWnd::OnNcHitTest(point);
}


BOOL CGuiMiniFrame::OnNcActivate(BOOL bActive)
{
	// TODO: Add your message handler code here and/or call default
	//the followin line is the real secret
	BOOL bNc=CMiniDockFrameWnd::OnNcActivate(bActive);
	TypeBar();	
	SendMessage( WM_NCPAINT );
	return bNc;
	
}


void CGuiMiniFrame::TypeBar()
{
	if (IsGuiControlBar != -1) return;
	CWnd* pBar=GetChildWnd();
	if (pBar==NULL) return;

	ModifyStyle( WS_SYSMENU, 0 );
	IsGuiControlBar=FALSE;

	if (pBar->IsKindOf(RUNTIME_CLASS(CGuiControlBar)))
	{
//	ModifyStyleEx( WS_EX_TOOLWINDOW,0 );
		ModifyStyle( 0,WS_SYSMENU );
//		SendMessage(WM_NCLBUTTONDOWN);
		IsGuiControlBar=TRUE;
	}
	else
	{
		if (pBar->IsKindOf(RUNTIME_CLASS(CMenuBar)))
			IsMenuBar=TRUE;
		//ModifyStyleEx( 0, WS_EX_TOOLWINDOW );
		ModifyStyle( WS_SYSMENU, 0 );
		IsGuiControlBar=FALSE;
	}
}


CWnd* CGuiMiniFrame::GetChildWnd()
{
	
	CWnd* pBar = GetWindow( GW_CHILD );
    if( pBar == NULL ) return NULL;
	pBar = pBar->GetWindow( GW_CHILD );
    if( pBar == NULL ) return NULL;
	return pBar;
}



void CGuiMiniFrame::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	SendMessage(WM_NCACTIVATE);


}


void CGuiMiniFrame::OnSetFocus(CWnd* pOldWnd)
{
	SendMessage(WM_NCACTIVATE);
	CMiniDockFrameWnd::OnSetFocus(pOldWnd);

	// TODO: Add your message handler code here
}
