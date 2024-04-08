// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__CBC22750_5F2A_4216_B495_516977B648F1__INCLUDED_)
#define AFX_MAINFRM_H__CBC22750_5F2A_4216_B495_516977B648F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <map>
#include <boost/shared_ptr.hpp>
#include <GuiFrameWnd.h>
// Bars
#include "CameraBar.h"
#include "PointBar.h"
#include "MergeBar.h"
#include "CompileBar.h"
#include "QualityBar.h"
#include "ExtentBar.h"
#include "AnchorBar.h"
// Scale combobox in menubar
#include "ScaleComboBox.h"

class CAllMapView;

class CMainFrame : public CGuiFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

protected:

	typedef std::map<int, boost::shared_ptr<CCameraBar> > WndMap_t ;
	WndMap_t m_Windows;

	typedef std::map<int, boost::shared_ptr<CMergeBar> > MergeMap_t ;
	MergeMap_t m_Merges;

	typedef std::map<int, boost::shared_ptr<CExtentBar> > ExtentWndMap_t ;
	ExtentWndMap_t m_ExtentWnds;

	std::vector< std::pair<int, int> >	m_MergedWnd;

	CCompileBar		m_wndCompileBar;

	CPointBar		m_wndPointBar;
	CQualityBar		m_wndQualityBar;
	CAnchorBar		m_wndAnchorBar;

	CMapPanoramaBar	m_wndMapPanoramaBar;

	CScaleComboBox	m_wndScaleComboBox;
	CMenu			m_Menu;

	CMenu			m_menuIZEdit;
	CMenu			m_menuIZView;

	bool			m_bStoped;

	CView*			m_pAllMapView;
	CView*			m_pMapView;
	//NOTENOTE: cache view since GetActiveVIew can retrieve CCameraView or other
	CView*			m_pActiveView;

public:

	// for progress bar
	void	StartProgress();
	void	SetProgress( DWORD dwAllTime, DWORD dwTimeDone );
	void	EndProgress();
	// for window submenu

	void	SetMapCoords( CString strText );
	void	SetAbsCoords( CString strText );

	void	Renumber(const std::vector<int>& Ids );
	void	Renumber(const std::vector<int>& Current, const std::vector<int>& Need);

	void	SetVisible(const std::vector<int>& Ids);
	void	SetMerged();

	CMenu*	GetSubMenu(int Number) { return m_Menu.GetSubMenu(Number);}

	CMenu*	GetMainSubMenu(int nPos, int nSub);
	std::vector<int>	GetIds() const;

	void	CreateUniteWndMenu(const std::vector<int >& Ids);

	CPointTree*		GetPointTree()	{ return &m_wndPointBar.m_wndPoint; }
	CQualityBar*	GetQualityBar() { return &m_wndQualityBar; }
	void	ResetColors();

	void	CloseMergeExtentWindows();

protected:
	// create window submenu
	void	CreateMasterWndMenu();
//	void	CreateMergeWndMenu();

	bool	CreateCameraWnd();

	void	CreateDeleteWndMenu();

	void	CreateFrame(int nID, bool bUpdate/*, bool bSet*/)	;
	void	CreateExtentMenu(const std::vector<int >& Ids);
	

	// record all cameras
	void	RecordPlay();
	void	RecordStop();

	//
//	bool	CreateCameraWnd();

	void	DockControlBarNextTo(CControlBar* pBar, CControlBar* pTargetBar);
	void	DockCameraBar( int nID );

	void OnInitMenuPopup( CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu );




public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnCorCompile();
	afx_msg void OnUpdateCorCompile(CCmdUI* pCmdUI);
	afx_msg void OnCorCreate();
	afx_msg void OnUpdateCorCreate(CCmdUI* pCmdUI);
	afx_msg void OnCorMerge2masters();
	afx_msg void OnUpdateCorMerge2masters(CCmdUI* pCmdUI);
	afx_msg void OnCorStop();
	afx_msg void OnUpdateCorStop(CCmdUI* pCmdUI);
	afx_msg void OnEditExtents();
	afx_msg void OnUpdateEditExtents(CCmdUI* pCmdUI);
	afx_msg void OnEditMap();
	afx_msg void OnUpdateEditMap(CCmdUI* pCmdUI);
	afx_msg void OnEditPoints();
	afx_msg void OnUpdateEditPoints(CCmdUI* pCmdUI);
	afx_msg void OnViewExtents();
	afx_msg void OnUpdateViewExtents(CCmdUI* pCmdUI);
	afx_msg void OnViewGetxyPoints();
	afx_msg void OnUpdateViewGetxyPoints(CCmdUI* pCmdUI);
	afx_msg void OnViewPoints();
	afx_msg void OnUpdateViewPoints(CCmdUI* pCmdUI);
	afx_msg void OnViewPolygons();
	afx_msg void OnUpdateViewPolygons(CCmdUI* pCmdUI);
	afx_msg void OnViewToolbar();
	afx_msg void OnUpdateViewToolbar(CCmdUI* pCmdUI);
	afx_msg void OnViewAddwindow();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnWindowPoints();
	afx_msg void OnUpdateWindowPoints(CCmdUI* pCmdUI);
	afx_msg void OnVideoPlay();
	afx_msg void OnUpdateVideoPlay(CCmdUI* pCmdUI);
	afx_msg void OnVideoStop();
	afx_msg void OnUpdateVideoStop(CCmdUI* pCmdUI);
	afx_msg void OnVideoPause();
	afx_msg void OnUpdateVideoPause(CCmdUI* pCmdUI);
	afx_msg void OnVideoRecord();
	afx_msg void OnUpdateVideoRecord(CCmdUI* pCmdUI);
	afx_msg void OnVideoProperty();
	afx_msg void OnUpdateVideoProperty(CCmdUI* pCmdUI);
	afx_msg void OnWindowQuality();
	afx_msg void OnUpdateWindowQuality(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	afx_msg void OnViewHeights();
	afx_msg void OnUpdateViewHeights(CCmdUI* pCmdUI);
	afx_msg void OnEditIz();
	afx_msg void OnUpdateEditIz(CCmdUI* pCmdUI);
	afx_msg void OnEditMapOrigin();
	afx_msg void OnUpdateEditMapOrigin(CCmdUI* pCmdUI);
	afx_msg void OnViewIz();
	afx_msg void OnUpdateViewIz(CCmdUI* pCmdUI);
	afx_msg void OnEditRp();
	afx_msg void OnUpdateEditRp(CCmdUI* pCmdUI);
	afx_msg void OnEditSp();
	afx_msg void OnUpdateEditSp(CCmdUI* pCmdUI);
	afx_msg void OnEditOptions();
	afx_msg void OnViewPointNumber();
	afx_msg void OnUpdateViewPointNumber(CCmdUI* pCmdUI);
	afx_msg void OnEditInterfaceOptions();
	afx_msg void OnWindowCloseAll();
	afx_msg void OnCorMapPanorama();
	afx_msg void OnUpdateCorMapPanorama(CCmdUI* pCmdUI);
	afx_msg void OnCancelMode();
	//}}AFX_MSG

	afx_msg void OnMasterWindowDynamic(UINT nID);
	afx_msg void OnUpdateMasterWindowDynamic(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMasterWindowNoMaster(CCmdUI* pCmdUI);

	afx_msg void OnUpdateIZEditMenu(CCmdUI* pCmdUI);
	afx_msg void OnIZEditMenu(UINT nID);

	afx_msg void OnUpdateIZViewMenu(CCmdUI* pCmdUI);
	afx_msg void OnIZViewMenu(UINT nID);

//	afx_msg void OnMergeWindowDynamic(int nID);
	afx_msg void OnMergeDynamic(UINT nID);
	
	afx_msg void OnMergeDeleteDynamic(UINT nID);

	afx_msg void OnExtentDynamic(UINT nID);
	afx_msg void OnUpdateExtentDynamic(CCmdUI* pCmdUI);
	
	afx_msg LRESULT OnCloseMaster( WPARAM wp, LPARAM lp );
	afx_msg LRESULT OnCloseMerge( WPARAM wp, LPARAM lp );
	afx_msg LRESULT OnCloseExtent( WPARAM wp, LPARAM lp );
	afx_msg LRESULT OnScaleMap( WPARAM wp, LPARAM lp );
	afx_msg LRESULT OnFitMap( WPARAM wp, LPARAM lp );	
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__CBC22750_5F2A_4216_B495_516977B648F1__INCLUDED_)
