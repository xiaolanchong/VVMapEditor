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





#if !defined(AFX_GUIDRAWLAYER_H__0BFB5AA0_94B4_47FC_8F93_84DB87A364F8__INCLUDED_)
#define AFX_GUIDRAWLAYER_H__0BFB5AA0_94B4_47FC_8F93_84DB87A364F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "GuiLib.h" 
#include "GuiNormalButton.h"
#define NORMALBTN   1
#define OVERBTN   1
#define PRESSBTN  2
//##ModelId=41776BEE000F
class GUILIBDLLEXPORT CGuiControlBarButton 
{
	
public:
	//##ModelId=41776BEE007F
	enum GuiType
	{
		//##ModelId=41776BEE0081
		GUINORMAL=0,
		//##ModelId=41776BEE008C
		GUITOOLBUTTON
	};
	//##ModelId=41776BEE0010
	CRect rcArea;
protected:

	//##ModelId=41776BEE001F
	int m_nIcon;
	//##ModelId=41776BEE0020
	LPCTSTR m_lpMsg;
	//##ModelId=41776BEE0022
	CImageList m_imgList;
	//##ModelId=41776BEE002F
	GuiType bTypeButton;
	//##ModelId=41776BEE0033
	BOOL m_bEnabled;
	//##ModelId=41776BEE003F
	CToolTipCtrl   m_toolTip;
public:

	//##ModelId=41776BEE0043
	CGuiControlBarButton();
	//##ModelId=41776BEE0044
	void SetData(int nIcon,LPCTSTR lpMsg);
	//##ModelId=41776BEE004E
	~CGuiControlBarButton();
	//##ModelId=41776BEE004F
	void Paint(CDC* pDC,int st,CRect rc,COLORREF clrFondo);
	//##ModelId=41776BEE0054
	void SetImageList(UINT nBitmapID, int cx, int nGrow, COLORREF crMask);
	//##ModelId=41776BEE0061
	void SetImageList(HINSTANCE hInst, UINT nBitmapID, int cx, int nGrow, COLORREF crMask);
	//##ModelId=41776BEE0071
	void SetTypeButton(GuiType bTypeb=GUINORMAL);
	//##ModelId=41776BEE007D
	void SetEnabled(BOOL bEnabled);
	
	
};

//##ModelId=41776BEE009C
class GUILIBDLLEXPORT GuiTabbedButton: public CGuiNormalButton
{
public:
	//##ModelId=41776BEE00AB
	GuiTabbedButton();
	//##ModelId=41776BEE00AC
	virtual ~GuiTabbedButton();
	
protected:
	//##ModelId=41776BEE00AE
	BOOL	bPress;
	//##ModelId=41776BEE00AF
	int		m_iMilliseconds;
protected:
	DECLARE_MESSAGE_MAP()
	//##ModelId=41776BEE00B0
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
public:
	//##ModelId=41776BEE00BE
	void SetWait(int iMilliseconds=50){m_iMilliseconds=iMilliseconds; };
	//##ModelId=41776BEE00C0
	afx_msg void OnTimer(UINT nIDEvent);
	//##ModelId=41776BEE00C3
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};

//##ModelId=41776BEE0119
class GUILIBDLLEXPORT GuiDrawLayer  
{
public:
	//##ModelId=41776BEE011A
		GuiDrawLayer();
	//##ModelId=41776BEE0128
		virtual ~GuiDrawLayer();
public:

	//************************************************************
	//##ModelId=41776BEE012A
	void		DrawArrow(CDC* pDC,CRect m_rc,BOOL m_bDown=TRUE);
	//##ModelId=41776BEE012E
	HICON		GetIcon(int nIcon);
	//##ModelId=41776BEE0130
	static HICON		LoadIconLib(UINT uIcon);
	//##ModelId=41776BEE0139
	static COLORREF	GetRGBColorXP();


	//##ModelId=41776BEE013B
	static COLORREF	GetRGBFondoXP();
	//##ModelId=41776BEE013D
	static COLORREF	GetRGBLightFondoXP();
	//##ModelId=41776BEE013F
	static COLORREF	GetRGBDarkFondoXP();

	//##ModelId=41776BEE0141
	static COLORREF	GetRGBCaptionXP();
	//##ModelId=41776BEE0148
	static COLORREF	GetRGBLightCaptionXP();
	//##ModelId=41776BEE014A
	static COLORREF	GetRGBDarkCaptionXP();

	//##ModelId=41776BEE014C
	static COLORREF	GetRGBPressBXP();
	//##ModelId=41776BEE014E
	static COLORREF	GetRGBColorFace();
	//##ModelId=41776BEE0150
	static COLORREF	GetRGBColorShadow();
	//##ModelId=41776BEE0152
	static COLORREF	GetRGBColorBTNHigh();
	//##ModelId=41776BEE0158
	static COLORREF	GetRGBColorTabs();
	//##ModelId=41776BEE015A
	static COLORREF	GetRGBColorWhite();
	//##ModelId=41776BEE015C
	static COLORREF GetRGBTitleMenu();
	//##ModelId=41776BEE015E
	static COLORREF GetRGBMenu();
	//##ModelId=41776BEE0160
	static COLORREF GetRGBColorGrayText();
	//##ModelId=41776BEE0162
	static void DrawCheck(CDC* pDC,CRect m_rcTemp);
	//##ModelId=41776BEE016A
	static BOOL		IsGrayed();
	//##ModelId=41776BEE016C
	static void DrawShade( CRect Rect,CPoint cp,CDC* pDC);
	//##ModelId=41776BEE0171
	static COLORREF DarkenColor( long lScale, COLORREF lColor);
	//##ModelId=41776BEE017A
	static COLORREF GetRGBSkinMenu();
	//************************************************************

protected:
	//**********************************
	//##ModelId=41776BEE017D
	CImageList	m_img;
	//##ModelId=41776BEE0186
	CPoint		mHeight;
	//##ModelId=41776BEE0187
	int			m_iWidthDrowDown;
	//**********************************

};

#endif // !defined(AFX_GUIDRAWLAYER_H__0BFB5AA0_94B4_47FC_8F93_84DB87A364F8__INCLUDED_)
