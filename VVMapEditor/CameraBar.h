#if !defined(AFX_CAMERABAR_H__BDAD9851_BB63_40E5_9D76_BB50E6FE1D30__INCLUDED_)
#define AFX_CAMERABAR_H__BDAD9851_BB63_40E5_9D76_BB50E6FE1D30__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CameraBar.h : header file
//

#include <GuiControlBar.h>
#include <GuiOfficeBar.h>
#include <GuiContainer.h>
#include "CameraView.h"

#define WM_CLOSE_MASTER	WM_USER + 22

/////////////////////////////////////////////////////////////////////////////
// CCameraBar window

//const int nID_Start = 1;

class CCameraBar : public CGuiControlBar
{
	CCameraView*	m_pView;

	int						m_statePause;
	CGuiControlBarButton	m_PauseButton;
//	bool					m_bRun;

	void		OnPause();
// Construction
public:
	CCameraBar( int nID, CWnd* pParentWnd);

	void	SetNumber( int nID );

	CBackground*	GetBackground() 
	{
		ASSERT(m_pView);
		return m_pView->GetBackground();
	}
// Attributes
public:

// Operations

//	CGuiContainer m_ctCamera;
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCameraBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCameraBar();

	CSize CalcDynamicLayout(int nLength, DWORD nMode);
	CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);


	CSize CalcLayout(DWORD dwMode, int nLength= -1);
	afx_msg void OnNcCalcSize( BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp );

	virtual void OnCloseBtn(  );
	// Generated message map functions
protected:
	//{{AFX_MSG(CCameraBar)
	afx_msg void OnDestroy();
	afx_msg void OnNcPaint();
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonUp(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg void OnCancelMode();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CAMERABAR_H__BDAD9851_BB63_40E5_9D76_BB50E6FE1D30__INCLUDED_)
