#if !defined(AFX_ANCHORBAR_H__17C0BC6A_78B0_4E76_ADEB_876AFE3D9B42__INCLUDED_)
#define AFX_ANCHORBAR_H__17C0BC6A_78B0_4E76_ADEB_876AFE3D9B42__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AnchorBar.h : header file
//

#include <GuiControlBar.h>
#include <GuiTabWnd.h>
#include "AnchorDlg.h"
#include "AnchorVIew.h"
#include "MapScaleView.h"

/////////////////////////////////////////////////////////////////////////////
// CAnchorBar window

class CAnchorBar : public CGuiControlBar
{
// Construction
//	CAnchorDlg		m_wndDialog;
	CGuiTabWnd		m_wndTab;
/*	CDialog	*/CMapScaleView*		m_pwndMapScale;
//	CImageList		m_ilTab;

	CAnchorView*		m_pwndView;		
public:
	CAnchorBar();

// Attributes
public:

	BOOL	Create(CWnd* pParentWnd);
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAnchorBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAnchorBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CAnchorBar)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	virtual void OnUpdateCmdUI( CFrameWnd* pTarget, BOOL bDisableIfNoHndler ); 

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANCHORBAR_H__17C0BC6A_78B0_4E76_ADEB_876AFE3D9B42__INCLUDED_)
