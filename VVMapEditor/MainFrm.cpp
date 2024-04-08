// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "VVMapEditor.h"

#include "VVMapEditorDoc.h"
#include "VVMapEditorView.h"
#include "MainFrm.h"

#include "IDDialog.h"
#include "SetIDDialog.h"
#include "CreateCADlg.h"
#include "MergeDialog.h"

#include "RealtimeBackground.h"
#include "AllMapView.h"

// settings & options
#include "CustomOptionDialog.h"
#include "ColorOptionPage.h"

#include <GuiMiniFrame.h>

#if 0
#include  "Accessor.h"
#else
#include "db/DBSerializer.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int IDT_COMPILE	= 1;
const int IDT_MERGE		= 2;

const int c_nMapCoordPane	= 1;
const int c_nAbsCoordPane	= 2;
const int c_nCompilePane	= 3;


/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CGuiFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CGuiFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_COR_COMPILE, OnCorCompile)
	ON_UPDATE_COMMAND_UI(ID_COR_COMPILE, OnUpdateCorCompile)
	ON_COMMAND(ID_COR_CREATE, OnCorCreate)
	ON_UPDATE_COMMAND_UI(ID_COR_CREATE, OnUpdateCorCreate)
	ON_COMMAND(ID_COR_MERGE_2MASTERS, OnCorMerge2masters)
	ON_UPDATE_COMMAND_UI(ID_COR_MERGE_2MASTERS, OnUpdateCorMerge2masters)
	ON_COMMAND(ID_COR_STOP, OnCorStop)
	ON_UPDATE_COMMAND_UI(ID_COR_STOP, OnUpdateCorStop)
	ON_COMMAND(ID_EDIT_EXTENTS, OnEditExtents)
	ON_UPDATE_COMMAND_UI(ID_EDIT_EXTENTS, OnUpdateEditExtents)
	ON_COMMAND(ID_EDIT_MAP, OnEditMap)
	ON_UPDATE_COMMAND_UI(ID_EDIT_MAP, OnUpdateEditMap)
	ON_COMMAND(ID_EDIT_POINTS, OnEditPoints)
	ON_UPDATE_COMMAND_UI(ID_EDIT_POINTS, OnUpdateEditPoints)
	ON_COMMAND(ID_VIEW_EXTENTS, OnViewExtents)
	ON_UPDATE_COMMAND_UI(ID_VIEW_EXTENTS, OnUpdateViewExtents)
	ON_COMMAND(ID_VIEW_GETXY_POINTS, OnViewGetxyPoints)
	ON_UPDATE_COMMAND_UI(ID_VIEW_GETXY_POINTS, OnUpdateViewGetxyPoints)
	ON_COMMAND(ID_VIEW_POINTS, OnViewPoints)
	ON_UPDATE_COMMAND_UI(ID_VIEW_POINTS, OnUpdateViewPoints)
	ON_COMMAND(ID_VIEW_POLYGONS, OnViewPolygons)
	ON_UPDATE_COMMAND_UI(ID_VIEW_POLYGONS, OnUpdateViewPolygons)
	ON_COMMAND(ID_VIEW_TOOLBAR, OnViewToolbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLBAR, OnUpdateViewToolbar)
	ON_COMMAND(ID_VIEW_ADDWINDOW, OnViewAddwindow)
	ON_WM_TIMER()
	ON_COMMAND(ID_WINDOW_POINTS, OnWindowPoints)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_POINTS, OnUpdateWindowPoints)
	ON_COMMAND(ID_VIDEO_PLAY, OnVideoPlay)
	ON_UPDATE_COMMAND_UI(ID_VIDEO_PLAY, OnUpdateVideoPlay)
	ON_COMMAND(ID_VIDEO_STOP, OnVideoStop)
	ON_UPDATE_COMMAND_UI(ID_VIDEO_STOP, OnUpdateVideoStop)
	ON_COMMAND(ID_VIDEO_PAUSE, OnVideoPause)
	ON_UPDATE_COMMAND_UI(ID_VIDEO_PAUSE, OnUpdateVideoPause)
	ON_COMMAND(ID_VIDEO_RECORD, OnVideoRecord)
	ON_UPDATE_COMMAND_UI(ID_VIDEO_RECORD, OnUpdateVideoRecord)
	ON_COMMAND(ID_VIDEO_PROPERTY, OnVideoProperty)
	ON_UPDATE_COMMAND_UI(ID_VIDEO_PROPERTY, OnUpdateVideoProperty)
	ON_COMMAND(ID_WINDOW_QUALITY, OnWindowQuality)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_QUALITY, OnUpdateWindowQuality)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_COMMAND(ID_VIEW_HEIGHTS, OnViewHeights)
	ON_UPDATE_COMMAND_UI(ID_VIEW_HEIGHTS, OnUpdateViewHeights)
	ON_COMMAND(ID_EDIT_IZ, OnEditIz)
	ON_UPDATE_COMMAND_UI(ID_EDIT_IZ, OnUpdateEditIz)
	ON_COMMAND(ID_EDIT_MAP_ORIGIN, OnEditMapOrigin)
	ON_UPDATE_COMMAND_UI(ID_EDIT_MAP_ORIGIN, OnUpdateEditMapOrigin)
	ON_COMMAND(ID_VIEW_IZ, OnViewIz)
	ON_UPDATE_COMMAND_UI(ID_VIEW_IZ, OnUpdateViewIz)
	ON_COMMAND(ID_EDIT_RP, OnEditRp)
	ON_UPDATE_COMMAND_UI(ID_EDIT_RP, OnUpdateEditRp)
	ON_COMMAND(ID_EDIT_SP, OnEditSp)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SP, OnUpdateEditSp)
	ON_COMMAND(ID_EDIT_OPTIONS, OnEditOptions)
	ON_COMMAND(ID_VIEW_POINT_NUMBER, OnViewPointNumber)
	ON_UPDATE_COMMAND_UI(ID_VIEW_POINT_NUMBER, OnUpdateViewPointNumber)
	ON_COMMAND(ID_EDIT_INTERFACE_OPTIONS, OnEditInterfaceOptions)
	ON_COMMAND(ID_WINDOW_CLOSE_ALL, OnWindowCloseAll)
	ON_COMMAND(ID_COR_MAP_PANORAMA, OnCorMapPanorama)
	ON_UPDATE_COMMAND_UI(ID_COR_MAP_PANORAMA, OnUpdateCorMapPanorama)
	ON_WM_INITMENUPOPUP()
	ON_WM_CANCELMODE()
	//}}AFX_MSG_MAP

/*	ON_UPDATE_COMMAND_UI(ID_MASTER_WINDOW,  OnUpdateMasterWindowNoMaster )
	ON_UPDATE_COMMAND_UI(ID_MERGE,			OnUpdateMasterWindowNoMaster )
	ON_UPDATE_COMMAND_UI(ID_EXTENT_MAP,		OnUpdateMasterWindowNoMaster )
	ON_UPDATE_COMMAND_UI(ID_MERGE_DELETE,	OnUpdateMasterWindowNoMaster )*/

	ON_COMMAND_RANGE( ID_MERGE+1, ID_MERGE+99, OnMergeDynamic )

	ON_COMMAND_RANGE( ID_MASTER_WINDOW+1, ID_MASTER_WINDOW+99, OnMasterWindowDynamic )
	ON_UPDATE_COMMAND_UI_RANGE( ID_MASTER_WINDOW+1, ID_MASTER_WINDOW+99, OnUpdateMasterWindowDynamic )

	ON_COMMAND_RANGE( ID_IZ_EDIT, ID_IZ_EDIT+99, OnIZEditMenu )
	ON_UPDATE_COMMAND_UI_RANGE( ID_IZ_EDIT, ID_IZ_EDIT+99, OnUpdateIZEditMenu)

	ON_COMMAND_RANGE( ID_IZ_VIEW, ID_IZ_VIEW+99, OnIZViewMenu )
	ON_UPDATE_COMMAND_UI_RANGE( ID_IZ_VIEW, ID_IZ_VIEW+99, OnUpdateIZViewMenu)

	ON_COMMAND_RANGE( ID_EXTENT_MAP+1, ID_EXTENT_MAP+99, OnExtentDynamic )
	ON_UPDATE_COMMAND_UI_RANGE( ID_EXTENT_MAP+1, ID_EXTENT_MAP+99, OnUpdateExtentDynamic )
	
	ON_COMMAND_RANGE( ID_MERGE_DELETE+1, ID_MERGE_DELETE+99, OnMergeDeleteDynamic )

	ON_MESSAGE( WM_CLOSE_MERGE, OnCloseMerge )
	ON_MESSAGE( WM_CLOSE_MASTER, OnCloseMaster)
	ON_MESSAGE( WM_SCALE, OnScaleMap )
	ON_MESSAGE( WM_CLOSE_EXTENT, OnCloseExtent )
/*
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	*/
	
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_ABSOLUTE_COORD,
	ID_MAP_COORD,
	ID_COMPILE_PROGRESS,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame():	m_bStoped(false), 
							m_pActiveView(0), 
							m_pMapView(0),
							m_pAllMapView(0)
{
	// TODO: add member initialization code here
	CoInitialize(0);
}

CMainFrame::~CMainFrame()
{
	CoUninitialize();
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CGuiFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	InitMenu( IDR_MAINFRAME );
	InitToolBar( IDR_MAINFRAME );
//	InitStatusBar(indicators, sizeof(indicators)/sizeof(UINT));

	if (!m_wndCompileBar.Create(this) ||
		!m_wndCompileBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	m_wndCool.LoadToolbar( IDR_MENU );

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable

	EnableDocking(CBRS_ALIGN_ANY);

	

	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);	
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndMenuBar,m_dockTop);
	DockControlBar(&m_wndToolBar,m_dockTop);

	m_wndToolBar.CreateCombo( &m_wndScaleComboBox, ID_SCALECOMBO, 60, WS_VISIBLE|CBS_DROPDOWNLIST|WS_VSCROLL);
	m_wndScaleComboBox.SetWindowPos( 0, 0, 0 , 60, 100, SWP_NOZORDER|SWP_NOMOVE );

	m_Menu.LoadMenu( IDR_MASTER);

	CreateCameraWnd();

	m_wndPointBar.Create(this);
	m_wndPointBar.EnableDocking(CBRS_ALIGN_ANY);

	m_wndQualityBar.Create(this);
	m_wndQualityBar.EnableDocking( CBRS_ALIGN_ANY );

	m_wndAnchorBar.Create(this);
	m_wndAnchorBar.EnableDocking( CBRS_ALIGN_ANY );
	
	DockControlBar(&m_wndPointBar,AFX_IDW_DOCKBAR_LEFT);
	DockControlBar(&m_wndQualityBar, AFX_IDW_DOCKBAR_LEFT);
	DockControlBar(&m_wndAnchorBar, AFX_IDW_DOCKBAR_LEFT);

	DockControlBarNextTo( &m_wndQualityBar, &m_wndPointBar );
	DockControlBarNextTo( &m_wndAnchorBar, &m_wndQualityBar );
	m_wndCompileBar.CreateCompileBar(c_nCompilePane);
//		
	UINT nID, nStyle;
	int nWidth;
	m_wndCompileBar.GetPaneInfo(c_nAbsCoordPane, nID, nStyle, nWidth);
	m_wndCompileBar.SetPaneInfo(c_nAbsCoordPane, nID, nStyle,  110);
	m_wndCompileBar.SetPaneText(c_nAbsCoordPane, _T(""));
	m_wndCompileBar.GetPaneInfo(c_nMapCoordPane, nID, nStyle, nWidth);
	m_wndCompileBar.SetPaneInfo(c_nMapCoordPane, nID, nStyle, 60);
	m_wndCompileBar.SetPaneText(c_nMapCoordPane, _T(""));

	//FIXME: to resources
	m_wndMapPanoramaBar.Create( this, _T("Map panorama") );

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CGuiFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

//	cs.style |= WS_CLIPSIBLINGS;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CGuiFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CGuiFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

bool	CMainFrame::CreateCameraWnd()
{
#if 0
	CCamHolder ch;
#else
	DBSerializer dbs;
#endif
	try
	{
		CControlBar* pTarget = 0;
		std::vector<int> Cameras;
		bool bUseStub = GetSettings().UseMasterStubList();
		if( bUseStub )
			Cameras = GetSettings().GetMasterStubList();
		else
		{
#if 0
			Cameras = ch.GetCamID();
#else
			dbs.GetCameraList(Cameras);
#endif
		}
		for( int i = 0; i < Cameras.size(); ++i )
		{
			int nID = Cameras[i];
			CString Name;
			Name.Format( IDS_FMT_ID, nID );
			boost::shared_ptr<CCameraBar> p ( new CCameraBar(nID, this) );
			p->ShowWindow(FALSE);
			p->EnableDocking(CBRS_ALIGN_ANY);
			DockControlBar(p.get(), AFX_IDW_DOCKBAR_RIGHT);
			if( i )
			{
			
				DockControlBarNextTo( p.get(), pTarget );
				pTarget = p.get();
			}
			else 
				pTarget = p.get(); 
			RecalcLayout();
			m_Windows.insert( std::make_pair( nID, p ) );
		}
		CreateMasterWndMenu();
	}
#if 0
	catch( CameraIDHolderException&ex )
	{
		AfxMessageBox( IDS_INVALID_DATA_SOURCE, MB_OK|MB_ICONERROR );
		return false;
	}
#else
	catch( DatabaseException )
	{

	}
#endif
	return true;
}

//********************************************* Corres ***************************************************************/

void CMainFrame::OnCorCompile() 
{
	// TODO: Add your command handler code here
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*) GetActiveDocument();
	m_bStoped = false;
	bool timer = pDoc->Compile();
	if(timer)
	{
		KillTimer(0);
		StartProgress();
		SetTimer(IDT_COMPILE, 200, 0);
	}	
}

void CMainFrame::OnUpdateCorCompile(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	if( !pDoc->CanCompile() || pDoc->IsCompiling()) pCmdUI->Enable(FALSE);		
}

void CMainFrame::OnCorCreate() 
{
	// TODO: Add your command handler code here
	CVVMapEditorDoc* pDoc = static_cast<CVVMapEditorDoc*>(GetActiveDocument());

	ResetColors();
	CCreateCADlg dlg;

	std::vector<int> Ids = GetIds();
	dlg.SetAllMasters(Ids);

	std::vector<int>	Selected;
	for(int i=0; i< pDoc->m_Ids.size(); ++i )
	{
		Selected.push_back( pDoc->m_Ids[i] );
	}
	dlg.SetSelectMasters( Selected );


	CVVMapEditorView* pView = pDoc->GetMapView();
	if( pView->GetBackground() )
	{
		bool IsMap = pDoc->m_bMap;
		dlg.SetMap( true, IsMap );
	}
	if( dlg.DoModal() == IDOK )
	{	

		std::vector<int> ids = dlg.GetMasters();
		bool bMap = dlg.IsMapSelect().second;
		pDoc->CreateCommonArea(ids, bMap);
	}

	WndMap_t::iterator itr = m_Windows.begin();
	for( ; itr != m_Windows.end(); ++itr)
	{
		itr->second->SendMessage(WM_NCPAINT);
	}	
}

void CMainFrame::OnUpdateCorCreate(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	if( pDoc->IsCompiling()) pCmdUI->Enable(FALSE);	
}

void CMainFrame::OnCorMerge2masters() 
{
	// TODO: Add your command handler code here
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*) GetActiveDocument();

//	PointMap_t::const_iterator itr = pDoc->Points().m_Points.begin();
	m_bStoped = false;
	int First, Second;
	CMergeDialog dlg(this, pDoc->m_Ids);
	if( dlg.DoModal() == IDOK )
	{	
		First	= dlg.m_nLeft;
		Second	= dlg.m_nRight;
		bool timer = pDoc->Merge(First, Second);
		if(timer)
		{
			KillTimer(0);
			StartProgress();
			SetTimer(IDT_MERGE, 200, 0);
		}	
	}	
}

void CMainFrame::OnUpdateCorMerge2masters(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	if( !pDoc->IsCurrent()  || pDoc->IsCompiling())
	{
		pCmdUI->Enable(FALSE);
	}		
}

void CMainFrame::OnCorStop() 
{
	// TODO: Add your command handler code here
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	pDoc->m_State = CVVMapEditorDoc::usual;
	m_bStoped = true;
	pDoc->m_VVMapTrans->StopProcess();
//	OutputDebugString("*****Stopped*****");	
}

void CMainFrame::OnUpdateCorStop(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	if( !pDoc->IsCompiling()) pCmdUI->Enable(FALSE);	
}

//*********************** Edit **************************

void CMainFrame::OnEditExtents() 
{
	// TODO: Add your command handler code here
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	if(pDoc->m_State == CVVMapEditorDoc::edit_extents) 
	{
		pDoc->m_State = CVVMapEditorDoc::usual;
	}
	else
	{
		pDoc->m_State = CVVMapEditorDoc::edit_extents;
		pDoc->m_bShowExtents = true;
	}	
}

void CMainFrame::OnUpdateEditExtents(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	pCmdUI->SetCheck( pDoc->m_State == CVVMapEditorDoc::edit_extents );
	if( !pDoc->IsCurrent() || pDoc->IsCompiling())
	{
		pCmdUI->Enable(FALSE);
	}	
}

void CMainFrame::OnEditMap() 
{
	// TODO: Add your command handler code here
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
/*
	if(pDoc->m_State == CVVMapEditorDoc::edit_map) 
	{
		pDoc->m_State = CVVMapEditorDoc::usual;
	}
	else 
	{
		pDoc->m_State = CVVMapEditorDoc::edit_map;
	}	*/
}

void CMainFrame::OnUpdateEditMap(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
//FIXME: Add code for map editting
	pCmdUI->Enable(FALSE);
}

void CMainFrame::OnEditPoints() 
{
	// TODO: Add your command handler code here
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	if(pDoc->m_State == CVVMapEditorDoc::edit_points) 
	{
		pDoc->m_State = CVVMapEditorDoc::usual;
	}
	else
	{
		pDoc->m_State = CVVMapEditorDoc::edit_points;
		pDoc->m_bShowCommonPoints = true;
		ShowControlBar(& m_wndPointBar, TRUE, TRUE );
	}	
}

void CMainFrame::OnUpdateEditPoints(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	pCmdUI->SetCheck( pDoc->m_State == CVVMapEditorDoc::edit_points );
	if( !pDoc->IsCurrent()  || pDoc->IsCompiling())
	{
		pCmdUI->Enable(FALSE);
	}	
}
/*
void CMainFrame::OnEditRedo() 
{
	// TODO: Add your command handler code here
	
}

void CMainFrame::OnUpdateEditRedo(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
}

void CMainFrame::OnEditUndo() 
{
	// TODO: Add your command handler code here
	
}

void CMainFrame::OnUpdateEditUndo(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
}*/


void CMainFrame::OnEditIz() 
{
	// TODO: Add your command handler code here
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	if(pDoc->m_State == CVVMapEditorDoc::edit_iz) 
	{
		pDoc->m_State = CVVMapEditorDoc::usual;
	}
	else
	{
		pDoc->m_State = CVVMapEditorDoc::edit_iz;
		pDoc->SetShowIZ();
	}	
}

void CMainFrame::OnUpdateEditIz(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	pCmdUI->SetCheck( pDoc->m_State == CVVMapEditorDoc::edit_iz );
	if( !pDoc->IsCurrent()  || pDoc->IsCompiling())
	{
		pCmdUI->Enable(FALSE);
	}	
}

void CMainFrame::OnEditMapOrigin() 
{
	// TODO: Add your command handler code here
/*	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	if(pDoc->m_State == CVVMapEditorDoc::edit_map_origin) 
	{
		pDoc->m_State = CVVMapEditorDoc::usual;
	}
	else
	{
		pDoc->m_State = CVVMapEditorDoc::edit_map_origin;
	}*/	
}

void CMainFrame::OnUpdateEditMapOrigin(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	//NOTENOTE: soon we remove this edit state
/*	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	pCmdUI->SetCheck( pDoc->m_State == CVVMapEditorDoc::edit_map_origin);
	if( !pDoc->IsCurrent()  || pDoc->IsCompiling())
	{
		pCmdUI->Enable(FALSE);
	}*/
	pCmdUI->Enable(FALSE);
}

void CMainFrame::OnEditRp() 
{
	// TODO: Add your command handler code here
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
/*	if(pDoc->m_State == CVVMapEditorDoc::edit_ref_points) 
	{
		pDoc->m_State = CVVMapEditorDoc::usual;
	}
	else
	{
		pDoc->m_State = CVVMapEditorDoc::edit_ref_points;
	}*/	
//	pDoc->CreateViewBitmap();

	CView *pOldView = m_pActiveView;
	CView* pNewView = (pOldView == m_pAllMapView? m_pMapView : m_pAllMapView);
	SetActiveView( pNewView, TRUE);
	m_pActiveView = pNewView;
	pNewView->SetDlgCtrlID( AFX_IDW_PANE_FIRST );
	pOldView->SetDlgCtrlID( AFX_IDW_PANE_FIRST + 1);
	pNewView->ShowWindow(SW_SHOW);
	pOldView->ShowWindow(SW_HIDE);
	RecalcLayout();
}

void CMainFrame::OnUpdateEditRp(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
//	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	CView * pView = GetActiveView();
	pCmdUI->SetCheck( m_pAllMapView == pView);
/*	if( !pDoc->IsCurrent()  || pDoc->IsCompiling())
	{
		pCmdUI->Enable(FALSE);
	}*/	
}

void CMainFrame::OnEditSp() 
{
	// TODO: Add your command handler code here
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	if(pDoc->m_State == CVVMapEditorDoc::edit_anchor_points) 
	{
		pDoc->m_State = CVVMapEditorDoc::usual;
	}
	else
	{
		pDoc->m_State = CVVMapEditorDoc::edit_anchor_points;
	}	
}

void CMainFrame::OnUpdateEditSp(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	pCmdUI->SetCheck( pDoc->m_State == CVVMapEditorDoc::edit_anchor_points);
	if( !pDoc->IsCurrent()  || pDoc->IsCompiling())
	{
		pCmdUI->Enable(FALSE);
	}	
}

void CMainFrame::OnEditOptions() 
{
	// TODO: Add your command handler code here
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	CPropertySheet sheet( IDS_OPTIONS, this );
	sheet.m_psh.dwFlags |= PSH_NOAPPLYNOW;
	CCustomOptionPage	dlgCustom;
	CColorOptionPage	dlgColor;
	sheet.AddPage( &dlgCustom );
	sheet.AddPage( &dlgColor );
	
	//NOTENOTE: init all master colors
	WndMap_t::const_iterator itr = m_Windows.begin();
	ColorMap_t	MasterMap, PointMap;
	for(  ; itr != m_Windows.end(); ++itr)
	{
		DWORD	dwColor = GetSettings().GetMasterColor( itr->first );
		MasterMap.insert( std::make_pair( itr->first, dwColor ) );
	}
	dlgColor.m_wndMaster.SetColorMap( MasterMap );

	//NOTENOTE: init all point group colors
	std::set<int> Groups = pDoc->GetAllValidGroups();
	std::set<int>::const_iterator itrGroup = Groups.begin();
	for( ; itrGroup != Groups.end(); ++itrGroup)
	{
		DWORD	dwColor = GetSettings().GetPointColor( *itrGroup );
		PointMap.insert( std::make_pair( *itrGroup, dwColor ) );
	}
	dlgColor.m_wndPoint.SetColorMap( PointMap );

	GetSettings().Transact();
	if( sheet.DoModal() == IDOK )
	{
		GetSettings().Commit();
		//NOTENOTE: invalidate mru numbers
		static_cast<CVVMapEditorApp*>(AfxGetApp())->ReloadMRU( GetSettings().GetProperty( prop_MRU_Number ) );

		// invalidate master colors settings
		itr = m_Windows.begin();
		MasterMap = dlgColor.m_wndMaster.GetColorMap();
		ColorMap_t::const_iterator itrMaster = MasterMap.begin();
		for( ; itrMaster != MasterMap.end(); ++itrMaster)
		{
			GetSettings().SetMasterColor( itrMaster->first, itrMaster->second );
		}

		// invalidate master windows
		for(  ; itr != m_Windows.end(); ++itr)
		{
			itr->second->SendMessage( WM_NCPAINT );
			itr->second->Invalidate();
			//p->SendMessage( WM_NC );
		}
		PointMap = dlgColor.m_wndPoint.GetColorMap();
		ColorMap_t::const_iterator itrPoint = PointMap.begin();
		for( ; itrPoint != PointMap.end(); ++itrPoint)
		{
			GetSettings().SetPointColor( itrPoint->first, itrPoint->second );
		}

		pDoc->OnOptionChange();
		GetSettings().Save();
	}
	else
		GetSettings().Rollback();
}


void CMainFrame::OnWindowPoints() 
{
	// TODO: Add your command handler code here
	ShowHideBar( & m_wndPointBar);
}

void CMainFrame::OnUpdateWindowPoints(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(  m_wndPointBar.IsVisible() ? 1 : 0 );
}


void CMainFrame::OnWindowQuality() 
{
	// TODO: Add your command handler code here
	ShowHideBar( & m_wndQualityBar);
}

void CMainFrame::OnUpdateWindowQuality(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(  m_wndQualityBar.IsVisible() ? 1 : 0 );
}

//****************** View *****************************
/*
void CMainFrame::OnViewExtentTables() 
{
	// TODO: Add your command handler code here
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	pDoc->m_bShowHeights = ! pDoc->m_bShowHeights;
	pDoc->UpdateAllViews(0);	
}

void CMainFrame::OnUpdateViewExtentTables(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	if( pDoc->m_bShowHeights ) pCmdUI->SetCheck(TRUE);
	else pCmdUI->SetCheck(FALSE);	
}*/

void CMainFrame::OnViewExtents() 
{
	// TODO: Add your command handler code here
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	pDoc->m_bShowExtents = ! pDoc->m_bShowExtents;
	pDoc->UpdateAllViews(0);	
}

void CMainFrame::OnUpdateViewExtents(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	if( pDoc->m_bShowExtents ) pCmdUI->SetCheck(TRUE);
	else pCmdUI->SetCheck(FALSE);	
}

void CMainFrame::OnViewGetxyPoints() 
{
	// TODO: Add your command handler code here
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	pDoc->m_bShowGetPoints = ! pDoc->m_bShowGetPoints;
	pDoc->UpdateAllViews(0);	
}

void CMainFrame::OnUpdateViewGetxyPoints(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	if( pDoc->m_bShowGetPoints ) pCmdUI->SetCheck(TRUE);
	else pCmdUI->SetCheck(FALSE);	
}

void CMainFrame::OnViewPoints() 
{
	// TODO: Add your command handler code here
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	pDoc->m_bShowCommonPoints = ! pDoc->m_bShowCommonPoints;
	pDoc->UpdateAllViews(0);	
}

void CMainFrame::OnUpdateViewPoints(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	if( pDoc->m_bShowCommonPoints ) pCmdUI->SetCheck(TRUE);
	else pCmdUI->SetCheck(FALSE);	
}

void CMainFrame::OnViewPolygons() 
{
	// TODO: Add your command handler code here
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	pDoc->m_bShowPolygons = ! pDoc->m_bShowPolygons;
	pDoc->UpdateAllViews(0);	
}

void CMainFrame::OnUpdateViewPolygons(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	if( pDoc->m_bShowPolygons ) pCmdUI->SetCheck(TRUE);
	else pCmdUI->SetCheck(FALSE);	
}

void CMainFrame::OnViewToolbar() 
{
	// TODO: Add your command handler code here
	ShowControlBar( &m_wndToolBar, !m_wndToolBar.IsVisible(), TRUE );
}

void CMainFrame::OnUpdateViewToolbar(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
	pCmdUI->SetCheck( m_wndToolBar.IsVisible() ? 1 : 0 );
}

void CMainFrame::OnUpdateViewHeights(CCmdUI* pCmdUI) 
{
	// TODO: Add your command handler code here
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	pCmdUI->SetCheck( pDoc->m_bShowHeights ? 1 : 0);
}

void CMainFrame::OnViewHeights() 
{
	// TODO: Add your command update UI handler code here
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	pDoc->m_bShowHeights = !pDoc->m_bShowHeights;
}


void CMainFrame::OnViewIz() 
{
	// TODO: Add your command handler code here
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	pDoc->ToggleShowIZ();
	pDoc->UpdateAllViews(0);
}

void CMainFrame::OnUpdateViewIz(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	pCmdUI->SetCheck( pDoc->IsShowIZ() ? 1 : 0);
}

void CMainFrame::OnViewPointNumber() 
{
	// TODO: Add your command handler code here
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	pDoc->m_bShowPointNumber = !pDoc->m_bShowPointNumber;
	pDoc->UpdateAllViews(0);
}

void CMainFrame::OnUpdateViewPointNumber(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	pCmdUI->SetCheck( pDoc->m_bShowPointNumber ? 1 : 0);	
}

//******************************************** Window


void CMainFrame::OnViewAddwindow() 
{
	// TODO: Add your command handler code here
	CIDDialog dlg;
	if( dlg.DoModal() == IDOK )
	{
		int nID = dlg.m_nID;

		WndMap_t::const_iterator itr = m_Windows.find( nID );
		if( itr != m_Windows.end() )
		{
			AfxMessageBox( _T("Id already exist"), MB_OK|MB_ICONERROR );
			return;
		}
		CreateFrame(nID,true);
	}	
}

void CMainFrame::OnMasterWindowDynamic(UINT nID)
{
	int nIndex = nID - ID_MASTER_WINDOW -1;

	if( m_Windows.size() <= nIndex ) throw VVMapTransException ("OnMasterDynamic::[Unknown ID]");
	WndMap_t::iterator itr =  m_Windows.begin();
	std::advance( itr, nIndex );
	ShowHideBar( itr->second.get()/*, TRUE, TRUE*/ );
	RecalcLayout();
}

void CMainFrame::OnUpdateMasterWindowDynamic( CCmdUI* pCmdUI)
{
	int nIndex = pCmdUI->m_nID - ID_MASTER_WINDOW -1;

	if( m_Windows.size() <= nIndex ) throw VVMapTransException ("OnMasterDynamic::[Unknown ID]");
	WndMap_t::iterator itr =  m_Windows.begin();
	std::advance( itr, nIndex );
	BOOL z = itr->second->IsVisible();
	pCmdUI->SetCheck( z? 1 : 0 );
}

void CMainFrame::OnUpdateMasterWindowNoMaster(CCmdUI* pCmdUI)
{
	if (pCmdUI->m_pSubMenu) {
		// update submenu item (File | Submenu)
		UINT fGray = MF_GRAYED;
		pCmdUI->m_pMenu->EnableMenuItem(pCmdUI->m_nIndex,
			MF_BYPOSITION|fGray);
	} else
		// update command item (File | Submenu | Foo)
		pCmdUI->Enable(0);	
}

void CMainFrame::OnMergeDynamic(UINT nID)
{
	int nIndex = nID - ID_MERGE -1;

	if( m_Merges.find( nIndex ) != m_Merges.end()) return;
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();

	try
	{
		CRect rc = pDoc->GetMergeRect( nIndex );

/*		if( rc.Width() > sizeMax.cx || rc.Height() > sizeMax.cy )
		{
			CString Exc;
			Exc.Format( IDS_FMT_MERGE_AREA_SIZE , rc.Width(), rc.Height() );
			throw VVMapTransException( Helper::ConvertA(Exc) );
		}*/


		CString Name;
		Name.Format( IDS_FMT_MERGE, pDoc->IndexToId(nIndex) );
		boost::shared_ptr<CMergeBar> p ( new CMergeBar( this, Name, rc,  nIndex  ) );

		m_Merges.insert( std::make_pair( nIndex, p));
			
		p->EnableDocking(0);
		p->m_pView->SetWindowPos(0, 0, 0, rc.Width(), rc.Height(), SWP_NOMOVE|SWP_NOZORDER);
		FloatControlBar( p.get(), CPoint(0,0) );
		RecalcLayout();
	}
	catch( VVMapTransException & ex  )
	{
		AfxMessageBox( Helper::Convert(ex.what()));
	}
}

void CMainFrame::OnExtentDynamic(UINT nID)
{
	int nIndex = nID - ID_EXTENT_MAP -1;

	if( m_Windows.size() <= nIndex ) throw VVMapTransException ("OnExtentDynamic::[Unknown ID]");
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	int Id = pDoc->m_Ids[nIndex];
	if( m_ExtentWnds.find( Id ) == m_ExtentWnds.end() )
	{
		CString strName; //strName.Format(_T("Extent %d"), Id );
		strName.Format( IDS_FMT_EXTENT, Id );
		boost::shared_ptr<CExtentBar> p ( new CExtentBar( this,strName, nIndex  ) );

		m_ExtentWnds.insert( std::make_pair( nIndex, p));
			
		p->EnableDocking(0);
//		p->SetDockStyle()
//		p->SetWindowPos(0, 0, 0, rc.Width(), rc.Height(), SWP_NOMOVE|SWP_NOZORDER);
//		DockControlBar(p.get(), AFX_IDW_DOCKBAR_RIGHT);			
		FloatControlBar( p.get(), CPoint(0,0) );
		RecalcLayout();		
	}
}

void CMainFrame::OnUpdateExtentDynamic( CCmdUI* pCmdUI)
{
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	int nIndex = pCmdUI->m_nID - ID_EXTENT_MAP -1;

	if( m_Windows.size() <= nIndex ) throw VVMapTransException ("OnUpdateExtentDynamic::[Unknown ID]");
	ExtentWndMap_t::iterator itr =  m_ExtentWnds.find( nIndex );
	if( itr != m_ExtentWnds.end() )
	{
		BOOL z = itr->second->IsVisible();
		pCmdUI->SetCheck( z? 1 : 0 );
	}
	BOOL bEnable = !pDoc->m_ExtentTable[nIndex].empty();
	pCmdUI->Enable( bEnable );

}

void CMainFrame::OnIZEditMenu(UINT nID)
{
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	int nIndex = nID - ID_IZ_EDIT;	

	pDoc->SetIZMaster(nIndex);
}

void CMainFrame::OnUpdateIZEditMenu( CCmdUI* pCmdUI)
{
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	int nIndex = pCmdUI->m_nID - ID_IZ_EDIT;	

	if(nIndex == pDoc->GetIZMaster()) pCmdUI->SetCheck();
	else pCmdUI->SetCheck(0);
}

void CMainFrame::OnUpdateIZViewMenu(CCmdUI* pCmdUI)
{
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	int nIndex = pCmdUI->m_nID - ID_IZ_VIEW;

	pCmdUI->SetCheck( pDoc->IsIZView( nIndex )? 1 : 0 );
}

void CMainFrame::OnIZViewMenu(UINT nID)
{
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	int nIndex = nID - ID_IZ_VIEW;	
	pDoc->ToggleIZView( nIndex );
	pDoc->UpdateAllViews(0);
}

LRESULT CMainFrame::OnScaleMap( WPARAM wp, LPARAM lp )
{
	float scale = *(float*)&wp;
	static_cast<CVVMapEditorView*>( m_pMapView)->Scale(scale);
	return TRUE;
}

//********************************************* Progress bar *************************************************************/

void	CMainFrame::SetProgress( DWORD all, DWORD done )
{
//	m_wndStatusBar.SetBarProgress( 1, TRUE, all, done );
	m_wndCompileBar.SetProgress( float(done)/ all );
}


void	CMainFrame::EndProgress()
{
//	m_wndStatusBar.SetUpBar(1, TRUE, FALSE);
	m_wndCompileBar.ResetProgress(  );
}

void	CMainFrame::StartProgress()
{
//	m_wndStatusBar.SetUpBar(1, TRUE, TRUE);
	m_wndCompileBar.ResetProgress(   );
}

std::vector<int>	CMainFrame::GetIds() const
{
	std::vector<int> Ids;
	WndMap_t::const_iterator itr = m_Windows.begin();
	for( ;itr != m_Windows.end(); ++itr )
	{
		Ids.push_back( itr->first );
	}	
	return Ids;
}

//******************************* Renumber ****************************************

void	CMainFrame::Renumber(const std::vector< int >& Ids )
{
	WndMap_t::const_iterator itr = m_Windows.begin();
	std::vector<int> Wnds, Difference;

	Difference.reserve( Ids.size() );
	Wnds.reserve(m_Windows.size());
	for(  ; itr != m_Windows.end(); ++itr)
	{
		Wnds.push_back( itr->first );
	}

	std::vector<int > Idz = Ids;

	std::sort ( Wnds.begin(), Wnds.end() );
	std::sort ( Idz.begin(), Idz.end() );
	std::set_difference< std::vector<int>::const_iterator,
						 std::vector<int>::const_iterator,
						 std::back_insert_iterator< std::vector<int> > >
						 ( Idz.begin(), Idz.end(), Wnds.begin(), Wnds.end(), 
						 std::back_inserter( Difference )	 );

	if( Difference.empty() ) return;

//	if( Ids ==  Wnds) return;
	if( Wnds.empty() )
	{
		for( int i =0 ; i < Ids.size(); ++i)
		{
			CreateFrame( Ids[i], true );
		}
		return;
	}

	bool bShowRenumberDlg = ( GetSettings().GetProperty( prop_ShowRenumberDialog ) != 0 );

	CLoadIDDialog dlg(this, Ids, Wnds);

	if( bShowRenumberDlg )
	{
		dlg.DoModal();
	}
	else
	{
		dlg.Create( CLoadIDDialog::IDD, this );
		dlg.SendMessage( WM_COMMAND, MAKELONG( IDOK, BN_CLICKED ) );
		dlg.DestroyWindow();
	}

	const std::vector< std::pair<int, int> >& arr =  dlg.m_Result;

	BOOL bShowWnd = ( GetSettings().GetProperty( prop_OpenWindowNumber ) >= Ids.size() );

	for( int i =0 ; i < arr.size(); ++i)
	{
		if(arr[i].first != arr[i].second )
		{
			if( arr[i].second == -1 )
			{
				int nID = arr[i].first;
				CreateFrame( nID , true );
				ShowControlBar( m_Windows[nID].get(), bShowWnd, TRUE );
			}
			else
			{
				WndMap_t::iterator itr =  m_Windows.find( arr[i].second );
				boost::shared_ptr<CCameraBar> p = itr->second;
				m_Windows.erase(itr);
				p->SetNumber( arr[i].first  );
				m_Windows.insert( std::make_pair( arr[i].first, p ) );
			}
		}
	}
/*
	for( i = 1; i < Ids.size(); ++i )
	{
		DockControlBarNextTo( m_Windows[ Ids[i] ].get(), m_Windows[ Ids[i-1] ].get() );
	}*/

	CreateMasterWndMenu();
}

void	CMainFrame::Renumber(const std::vector<int>& Current, const std::vector<int>& Need)
{
	WndMap_t EndMap;
	for( int i =0 ; i < Current.size(); ++i)
	{
		WndMap_t::iterator itr =  m_Windows.find( Current[i] );
		boost::shared_ptr<CCameraBar> p = itr->second;
		EndMap.insert( std::make_pair( Need[i], p ) );

		p->SetNumber( Need[i]  );
		p->SendMessage( WM_NCPAINT );
	}
	m_Windows = EndMap;
	CreateMasterWndMenu();
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	CreateUniteWndMenu(pDoc->m_Ids);
//	CreateExtentMenu();
	SetMerged();
}

void	CMainFrame::SetMerged()
{
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	m_MergedWnd.clear();
	long num;
	pDoc->m_VVMapTrans->GetNumOfMasters(&num);

//	ResetColors();
	for( int i = 0; i < num; ++i)
	{
//		CString Caption;
		int idLeft = pDoc->IndexToId( i );
		CString Caption;
		Caption.Format( IDS_FMT_ID, idLeft );
		m_Windows[idLeft]->SetWindowText(Caption);
		if( m_Windows[idLeft]->IsFloating() )
		{
			m_Windows[idLeft]->GetParentFrame()->SetWindowText(Caption);
			m_Windows[idLeft]->GetParentFrame()->SendMessage(WM_NCPAINT);
		}
		m_Windows[idLeft]->SendMessage(WM_NCPAINT);
	}

	for(  i = 0; i < num; ++i)
	{
		for( int j = 0; j < num; ++j)
		{
			if( i != j && !pDoc->m_VVMapTrans->CheckMerge( i, j ) )
			{
				CString CaptionLeft, CaptionRight;
				int idLeft = pDoc->IndexToId( i );
				int idRight = pDoc->IndexToId( j );
				CaptionLeft.Format(_T("ID = %d[%d:%d]"),idLeft, idLeft, idRight );
				CaptionRight.Format(_T("ID = %d[%d:%d]"), idRight, idLeft, idRight);
				m_Windows[idLeft]->SetWindowText(CaptionLeft);
				if( m_Windows[idLeft]->IsFloating() )
				{
					m_Windows[idLeft]->GetParent()->SetWindowText(CaptionLeft);
					m_Windows[idLeft]->GetParent()->SendMessage(WM_NCPAINT);
				}
				m_Windows[idRight]->SetWindowText(CaptionRight);
				if( m_Windows[idRight]->IsFloating() )
				{
					m_Windows[idRight]->GetParent()->SetWindowText(CaptionRight);
					m_Windows[idRight]->GetParent()->SendMessage(WM_NCPAINT);
				}
				m_Windows[idLeft]->SendMessage(WM_NCPAINT);
				m_Windows[idRight]->SendMessage(WM_NCPAINT);

				m_MergedWnd.push_back( std::make_pair( i, j ) );
			}
		}
	}
	CreateDeleteWndMenu();
}

void	CMainFrame::SetVisible(const std::vector<int>& Ids)
{
	//NOTENOTE: show windows hen number of windows >= setting value
	DWORD dwNumber = GetSettings().GetProperty( prop_OpenWindowNumber );
	if( dwNumber >= Ids.size() )
		for( int i = 0; i < Ids.size(); ++i )
		{
			ShowControlBar( m_Windows[Ids[i]].get(), TRUE, TRUE);
		}
}


//*******************************

void CMainFrame::CreateFrame(int nID, bool bUpdate/*, bool bSet*/)
{
	CString Name;
	Name.Format( _T("ID = %d"), nID );
	boost::shared_ptr<CCameraBar> p ( new CCameraBar(nID, this) );			
	ShowHideBar( p.get() );
	p->EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(p.get(), AFX_IDW_DOCKBAR_RIGHT);	
	RecalcLayout();

//		if(bUpdate) p->m_pView->SendMessage( WM_INITIALUPDATE );

	m_Windows.insert( std::make_pair( nID, p ) );
	DockCameraBar( nID );

	ShowControlBar( p.get(), TRUE, TRUE );
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*) GetActiveDocument();
	
	if(!pDoc) return;

	CreateMasterWndMenu();
}

void	CMainFrame::CreateMasterWndMenu()
{
	const int sub_window = 4;
	const int sub_sub_masters = 0;

	CMenu mn;
	HMENU hMenu = m_wndMenuBar.m_hMenu;

	mn.Attach( hMenu );

	CMenu* pMenu = mn.GetSubMenu(sub_window);
	pMenu = pMenu->GetSubMenu( sub_sub_masters );

	WndMap_t::const_iterator itr = m_Windows.begin();
	for( int i = 0 ; itr != m_Windows.end(); ++itr, ++i)
	{
		CString str;
		str.Format(_T("%d"), itr->first );
		BOOL bVisible = itr->second->IsVisible();
		UINT state = /*bVisible ? MF_CHECKED :*/ MF_UNCHECKED; 
		BOOL res = pMenu->ModifyMenu( i, MF_BYPOSITION | state , ID_MASTER_WINDOW+i+1, str );
		if( !res) pMenu->InsertMenu( i, MF_BYPOSITION | state, ID_MASTER_WINDOW+i+1, str );
		
	}
	int MapSize = m_Windows.size();
	int MenuSize = pMenu->GetMenuItemCount();
	if( MapSize < MenuSize )
	{
		for( int i = MapSize; i < MenuSize; ++i)
		{
			pMenu->DeleteMenu( i, MF_BYPOSITION );
		}
	}
	mn.Detach();
}

void CMainFrame::CreateUniteWndMenu(const std::vector<int >& Ids)
{
	int sub_correspondence = 3;
	int sub_unite = 3;
	CMenu* pMenu = GetMainSubMenu(sub_correspondence, sub_unite );
	ASSERT(pMenu);


	m_menuIZEdit.DestroyMenu();
	m_menuIZView.DestroyMenu();
	m_menuIZEdit.CreatePopupMenu();
	m_menuIZView.CreatePopupMenu();
	
	for(  int i = 0; i< Ids.size(); ++i)
	{
		CString str;
		str.Format(_T("%d"), Ids[i] );
		BOOL res = pMenu->ModifyMenu( i, MF_BYPOSITION, ID_MERGE+i+1, str );
		if( !res) pMenu->InsertMenu( i, MF_BYPOSITION, ID_MERGE+i+1, str );
		m_menuIZEdit.AppendMenu( MF_BYPOSITION , ID_IZ_EDIT + i, str);
		m_menuIZView.AppendMenu( MF_BYPOSITION , ID_IZ_VIEW + i, str);
	}

	int MapSize = Ids.size();
	int MenuSize = pMenu->GetMenuItemCount();
	if( MapSize < MenuSize )
	{
		for( int i = MapSize; i < MenuSize; ++i)
		{
			pMenu->DeleteMenu( i, MF_BYPOSITION );
		}
	}

	int nIndex; UINT nID; UINT nStyle; int iImage ;
	nIndex = m_wndToolBar.CommandToIndex(ID_EDIT_IZ);
	m_wndToolBar.GetButtonInfo(  nIndex, nID,  nStyle,  iImage ) ;
	m_wndToolBar.SetButtonDropDown( ID_EDIT_IZ, iImage,  m_menuIZEdit.GetSafeHmenu());
	nIndex = m_wndToolBar.CommandToIndex(ID_VIEW_IZ);
	m_wndToolBar.GetButtonInfo(  nIndex, nID,  nStyle,  iImage ) ;
	m_wndToolBar.SetButtonDropDown( ID_VIEW_IZ, iImage,  m_menuIZView.GetSafeHmenu());
//	m_wndToolBar.SetButtonDropDown( ID_VIEW_IZ, iImage,  m_menuIZView.GetSafeHmenu());

	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	for( i = 0; pDoc&&i < pDoc->m_Ids.size(); ++i )
	{
		int z = pDoc->m_Ids[i];
		Color cl = GetSettings().GetMasterColor( z );
		m_Windows[ z ]->SetColorFondo( cl.ToCOLORREF() );
	}
	CreateExtentMenu(Ids);
}

void	CMainFrame::CreateExtentMenu(const std::vector<int >& Ids)
{
//	const CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
//	const std::vector<int>& Ids = pDoc->m_Ids;
	int sub_correspondence = 3;
	int sub_extents = 5;
	CMenu* pMenu = GetMainSubMenu(sub_correspondence, sub_extents );
	ASSERT(pMenu);
	for(  int i = 0; i< Ids.size(); ++i)
	{
		CString str;
		str.Format(_T("%d"), Ids[i] );
		BOOL res = pMenu->ModifyMenu( i, MF_BYPOSITION, ID_EXTENT_MAP+i+1, str );
		if( !res) pMenu->InsertMenu( i, MF_BYPOSITION, ID_EXTENT_MAP+i+1, str );
	}
	int MapSize = Ids.size();
	int MenuSize = pMenu->GetMenuItemCount();
	if( MapSize < MenuSize )
	{
		for( int i = MapSize; i < MenuSize; ++i)
		{
			pMenu->DeleteMenu( i, MF_BYPOSITION );
		}
	}
/*
	for( i = 0; pDoc&&i < pDoc->m_Ids.size(); ++i )
	{
		int z = pDoc->m_Ids[i];
		Color cl = pDoc->GetColor( z );
		m_Windows[ z ]->SetColorFondo( cl.ToCOLORREF() );
	}*/	
}

void CMainFrame::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();

	switch(nIDEvent)
	{

	case IDT_COMPILE:

		if ( (pDoc->m_hCompile && WaitForSingleObject(  pDoc->m_hCompile, 0 ) == WAIT_OBJECT_0) ||
			 m_bStoped )
		{	
			//NOTENOTE: Free handle
			pDoc->EndCompile();
			EndProgress();
			KillTimer(IDT_COMPILE);
			pDoc->m_hCompile = 0;
			if(pDoc->m_nThreadRes == 0) pDoc->CreatePolygon(true);
			UINT uFlags = ( pDoc->m_nThreadRes == ERR_NO_ERROR )?	MB_OK|MB_ICONINFORMATION :
																	MB_OK|MB_ICONERROR;
			USES_CONVERSION;
			LPCWSTR szError = pDoc->m_VVMapTrans->GetErrorStringW( pDoc->m_nThreadRes );
			//FIXME resources
#if 1
			szError = (szError && *szError) ? szError : L"Operation completed successfully";
#endif
			LPCTSTR szConvError = W2CT( szError );
			CString strError = szConvError/*Helper::Convert() */;
			AfxMessageBox( strError, uFlags );
		}
		else if(pDoc->m_hCompile)
		{
			long progress;
			pDoc->m_VVMapTrans->GetProgress(&progress);

			SetProgress( 100, progress );
		}
		break;
	case IDT_MERGE:
		if( m_bStoped )
		{
			//NOTENOTE: free handle
			pDoc->EndMerge();
			EndProgress();
			KillTimer(IDT_MERGE);
			pDoc->m_hMerge = 0;
		}
		else if ( pDoc->m_hMerge && WaitForSingleObject(  pDoc->m_hMerge, 0 ) == WAIT_OBJECT_0 )
		{	
			EndProgress();
			KillTimer(IDT_MERGE);
			pDoc->m_hMerge = 0;
			if(pDoc->m_nThreadRes == ERR_NO_ERROR) pDoc->CreatePolygon(false);
			UINT uFlags = ( pDoc->m_nThreadRes == ERR_NO_ERROR )?	MB_OK|MB_ICONINFORMATION :
																	MB_OK|MB_ICONERROR;
			USES_CONVERSION;
			LPCWSTR szError = pDoc->m_VVMapTrans->GetErrorStringW( pDoc->m_nThreadRes );
			LPCTSTR szConvError = W2CT( szError );
			AfxMessageBox( szConvError, uFlags );
		}
		else if(pDoc->m_hMerge)
		{
			long progress;
			pDoc->m_VVMapTrans->GetProgress(&progress);

			SetProgress( 100, progress );
		}
		break;

	}

	
	CGuiFrameWnd::OnTimer(nIDEvent);
}

CMenu*	CMainFrame::GetMainSubMenu(int nPos, int nSub)
{
	HMENU hMenu = m_wndMenuBar.m_hMenu;
	CMenu mn;
	mn.Attach( hMenu );
	CMenu* pMenu = mn.GetSubMenu(nPos);
	pMenu = pMenu->GetSubMenu( nSub );	
	mn.Detach();
	return pMenu;
}

void CMainFrame::OnInitMenuPopup( CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu )
{
	UpdateWindow();
//	m_wndMenuBar.UpdateWindow();
    CFrameWnd::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
    CNewMenu::OnInitMenuPopup(m_hWnd,pPopupMenu, nIndex, bSysMenu);
}

// Undocument feature of MFC. may not work on other versions

void CMainFrame::DockControlBarNextTo(CControlBar* pBar, CControlBar* pTargetBar)
{
    ASSERT(pBar != NULL);
    ASSERT(pTargetBar != NULL);
    ASSERT(pBar != pTargetBar);

    // the neighbour must be already docked
    CDockBar* pDockBar = pTargetBar->m_pDockBar;
    ASSERT(pDockBar != NULL);
    UINT nDockBarID = pTargetBar->m_pDockBar->GetDlgCtrlID();
    ASSERT(nDockBarID != AFX_IDW_DOCKBAR_FLOAT);

    bool bHorz = (nDockBarID == AFX_IDW_DOCKBAR_TOP ||
        nDockBarID == AFX_IDW_DOCKBAR_BOTTOM);

    // dock normally (inserts a new row)
    DockControlBar(pBar, nDockBarID);

    // delete the new row (the bar pointer and the row end mark)
    pDockBar->m_arrBars.RemoveAt(pDockBar->m_arrBars.GetSize() - 1);
    pDockBar->m_arrBars.RemoveAt(pDockBar->m_arrBars.GetSize() - 1);

    // find the target bar
    for (int i = 0; i < pDockBar->m_arrBars.GetSize(); i++)
    {
        void* p = pDockBar->m_arrBars[i];
        if (p == pTargetBar) // and insert the new bar after it
            pDockBar->m_arrBars.InsertAt(i + 1, pBar);
    }

    // move the new bar into position
    CRect rBar;
    pTargetBar->GetWindowRect(rBar);
    rBar.OffsetRect(bHorz ? 1 : 0, bHorz ? 0 : 1);
    pBar->MoveWindow(rBar);
}

void	CMainFrame::DockCameraBar( int nID )
{
	if( m_Windows.size() > 1 )
	{
		WndMap_t::reverse_iterator itr = m_Windows.rbegin();
		while( itr != m_Windows.rend() )
		{
			if( itr->first != nID )
			{
				DockControlBarNextTo(  m_Windows[nID].get(), itr->second.get());
				return;
			}
			++itr;
		}
	}
}

void CMainFrame::ResetColors()
{
	WndMap_t::const_iterator itr = m_Windows.begin();
	for(  ;itr != m_Windows.end(); ++itr)
	{
	 if(	::IsWindow(itr->second->m_hWnd) )
		{
			itr->second->SetColorFondo( GuiDrawLayer::GetRGBColorFace() );	
			itr->second->SendMessage( WM_NCPAINT );
		}
	}
}

LRESULT CMainFrame::OnCloseMerge( WPARAM wp, LPARAM lp )
{
	m_Merges.erase( wp );
	RecalcLayout();
//	CreateMergeWndMenu();
	return TRUE;
}

LRESULT CMainFrame::OnCloseMaster( WPARAM wp, LPARAM lp )
{
	RecalcLayout();

	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	pDoc->OnCloseMaster();
	return TRUE;
}


LRESULT CMainFrame::OnCloseExtent( WPARAM wp, LPARAM lp )
{
	m_ExtentWnds.erase( wp );
	RecalcLayout();
	return TRUE;
}

/////////////////////// VIDEO ///////////////////////////////////////////////////

void CMainFrame::OnVideoPlay() 
{
	// TODO: Add your command handler code here
	
}

void CMainFrame::OnUpdateVideoPlay(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
#ifdef USE_VIDEO_RECORD	
#else
	pCmdUI->Enable(FALSE);
#endif		
}

void CMainFrame::OnVideoStop() 
{
	// TODO: Add your command handler code here
	
}

void CMainFrame::OnUpdateVideoStop(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
#ifdef USE_VIDEO_RECORD	
#else
	pCmdUI->Enable(FALSE);
#endif		
}

void CMainFrame::OnVideoPause() 
{
	// TODO: Add your command handler code here
	
}

void CMainFrame::OnUpdateVideoPause(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
#ifdef USE_VIDEO_RECORD	
#else
	pCmdUI->Enable(FALSE);
#endif	
}

void CMainFrame::OnVideoRecord() 
{
	// TODO: Add your command handler code here
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	if( pDoc->m_rs == CVVMapEditorDoc::record_play )
	{
		pDoc->m_rs = CVVMapEditorDoc::record_stop;
		RecordStop();
	}
	else
	{
		pDoc->m_rs = CVVMapEditorDoc::record_play;
		RecordPlay();
	}
}

void CMainFrame::OnUpdateVideoRecord(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();

#ifdef USE_VIDEO_RECORD
	Compressor& cmp = pDoc->GetCompressor();
	if( cmp.m_dwFourCC == 0)
	{
		pCmdUI->Enable(FALSE);
	}

	if(  pDoc->m_rs == CVVMapEditorDoc::record_play) pCmdUI->SetCheck(TRUE);
#else
	pCmdUI->Enable(FALSE);
#endif
}

void CMainFrame::OnVideoProperty() 
{
	// TODO: Add your command handler code here
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();

	Compressor& cmp = pDoc->GetCompressor();
	cmp.ChooseCompressor( GetSafeHwnd() );
}

void CMainFrame::OnUpdateVideoProperty(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
#ifdef USE_VIDEO_RECORD	
#else
	pCmdUI->Enable(FALSE);
#endif		
}

/////////////////////// VIDEO ///////////////////////////////////////////////////

void	CMainFrame::RecordPlay()
{
	WndMap_t::iterator itr = m_Windows.begin();
	try
	{
		CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
		Compressor& cmp = pDoc->GetCompressor();

	for( ;itr != m_Windows.end(); ++itr )
	{
		CBackground* pBG = itr->second->GetBackground();
		CRealtimeBackground* pRBG = 0;
		if( pRBG = dynamic_cast<CRealtimeBackground*>(pBG) )
		{
			CString infile, outfile;
			infile.Format(_T("orwis://%d"), itr->first);

			CString zz;
			zz.Format(_T("ID=%d ") , itr->first);
			outfile = zz + COleDateTime::GetCurrentTime().Format(_T("%Y-%m-%d %H-%M-%S"))+_T(".avi");

			pRBG->Record( cmp.m_dwFourCC, infile, outfile);
		}
	}
	}
	catch(  CommonException& ex)
	{
		AfxMessageBox( Helper::Convert(ex.what()));
	}
}

void	CMainFrame::RecordStop()
{
	WndMap_t::iterator itr = m_Windows.begin();
	try
	{
	for( ;itr != m_Windows.end(); ++itr )
	{
		CBackground* pBG = itr->second->GetBackground();
		CRealtimeBackground* pRBG = 0;
		if( pRBG = dynamic_cast<CRealtimeBackground*>(pBG) )
		{
			pRBG->StopRecord();
		}
	}
	}
	catch(  CommonException& ex)
	{
		AfxMessageBox( Helper::Convert( ex.what()) );
	}
}

void	CMainFrame::CreateDeleteWndMenu()
{
	const int sub_correspondence = 3;
	const int sub_sub_delete = 4;
	CMenu* pMenu = GetMainSubMenu(sub_correspondence, sub_sub_delete );
	ASSERT(pMenu);

	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	for( int i = 0 ; i != m_MergedWnd.size(); ++i)
	{
		CString str;
		int f = pDoc->IndexToId( m_MergedWnd[i].first);
		int s = pDoc->IndexToId( m_MergedWnd[i].second);

		str.Format(_T("%d::%d"), f , s );
//		UINT state = MF_UNCHECKED; 
		BOOL res = pMenu->ModifyMenu( i, MF_BYPOSITION | MF_UNCHECKED , ID_MERGE_DELETE+i+1, str );
		if( !res) pMenu->InsertMenu( i, MF_BYPOSITION | MF_UNCHECKED, ID_MERGE_DELETE+i+1, str );
	}

	int MapSize = m_MergedWnd.size();
	int MenuSize = pMenu->GetMenuItemCount();
	if( MapSize < MenuSize )
	{
		for( int i = MapSize; i < MenuSize; ++i)
		{
			pMenu->DeleteMenu( i, MF_BYPOSITION );
		}
	}
}

void	CMainFrame::OnMergeDeleteDynamic(UINT nID)
{
	int nIndex = nID - ID_MERGE_DELETE -1;

	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	if( m_MergedWnd.size() <= nIndex ) throw VVMapTransException ("OnMergeDeleteDynamic::[Unknown ID]");
	
	int f = m_MergedWnd[nIndex].first;
	int s = m_MergedWnd[nIndex].second;

	VVError err(pDoc->m_VVMapTrans);
	
	err = pDoc->m_VVMapTrans->DeleteMerge(f ,s  );

	//CreateDeleteWndMenu();
	SetMerged();
}

void CMainFrame::OnDestroy() 
{
	CGuiFrameWnd::OnDestroy();
	
	// TODO: Add your message handler code here


}

void CMainFrame::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
//	m_wndQualityBar.SaveBar(CString());
//	m_wndPointBar.SaveBar( CString() );	


	CGuiFrameWnd::OnClose();
}

void	CMainFrame::CloseMergeExtentWindows()
{
	MergeMap_t::iterator itr = m_Merges.begin();
	for( ;itr != m_Merges.end(); ++itr )
	{
		itr->second->DestroyWindow();
	}
	ExtentWndMap_t::iterator itr2 = m_ExtentWnds.begin();
	for( ;itr2 != m_ExtentWnds.end(); ++itr2 )
	{
		itr2->second->DestroyWindow();
	}
}


BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class

	BOOL res=  CGuiFrameWnd::OnCreateClient(lpcs, pContext);
	m_pAllMapView = (CAllMapView*)RUNTIME_CLASS(CAllMapView)->CreateObject();

	m_pAllMapView->Create( 0, 0, WS_BORDER|WS_VISIBLE, CRect(0,0,0,0), this, AFX_IDW_PANE_FIRST + 1 );
	pContext->m_pCurrentDoc->AddView( m_pAllMapView );


	CWnd* pWnd = GetDescendantWindow(AFX_IDW_PANE_FIRST, TRUE);
	if (pWnd != NULL && pWnd->IsKindOf(RUNTIME_CLASS(CView)))
	{
		m_pMapView = (CView*)pWnd;
	}
	else
	{
		ASSERT(FALSE);
	}
	m_pActiveView = m_pMapView;

	return res;
}

void CMainFrame::OnEditInterfaceOptions() 
{
	// TODO: Add your command handler code here
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	pDoc->m_VVMapTrans->RunOptionsDialog( GetSafeHwnd() );
}

void	CMainFrame::SetMapCoords( CString strText )
{
	m_wndCompileBar.SetPaneText( c_nMapCoordPane, strText );
}

void	CMainFrame::SetAbsCoords( CString strText )
{
	m_wndCompileBar.SetPaneText( c_nAbsCoordPane, strText );
}

void CMainFrame::OnWindowCloseAll() 
{
	// TODO: Add your command handler code here
	WndMap_t::iterator itr = m_Windows.begin();
	for( ; itr != m_Windows.end(); ++itr)
	{
		ShowControlBar( itr->second.get(), FALSE, TRUE) ;
	}		
}

void CMainFrame::OnCorMapPanorama() 
{
	// TODO: Add your command handler code here
	ShowControlBar( &m_wndMapPanoramaBar, TRUE, TRUE );
	m_wndMapPanoramaBar.CreateBitmap();
}

void CMainFrame::OnUpdateCorMapPanorama(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetActiveDocument();
	if( !pDoc->IsMapInCurrent() || pDoc->IsCompiling()/* || m_wndMapPanoramaBar.IsWindowVisible()*/)
	{
		pCmdUI->Enable(FALSE);
	}
	else
		pCmdUI->Enable(TRUE);
}

void CMainFrame::OnCancelMode() 
{
	CGuiFrameWnd::OnCancelMode();
	
	// TODO: Add your message handler code here
	WndMap_t::iterator itrWnd = m_Windows.begin();
	for( ; itrWnd != m_Windows.end(); ++itrWnd)
	{
		itrWnd->second->SendMessage(WM_CANCELMODE);
	}
	
	MergeMap_t::iterator itrMrg = m_Merges.begin();
	for( ; itrMrg != m_Merges.end(); ++itrMrg)
	{
		itrMrg->second->SendMessage(WM_CANCELMODE);
	}
	
	ExtentWndMap_t::iterator itrExt = m_ExtentWnds.begin();
	for( ; itrExt != m_ExtentWnds.end(); ++itrExt)
	{
		itrExt->second->SendMessage(WM_CANCELMODE);
	}

	m_pMapView->SendMessage( WM_CANCELMODE);
	m_pAllMapView->SendMessage( WM_CANCELMODE );
}
