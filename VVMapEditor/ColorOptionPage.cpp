// ColorOptionPage.cpp : implementation file
//

#include "stdafx.h"
#include "VVMapEditor.h"
#include "ColorOptionPage.h"
#include "Settings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorOptionPage property page

IMPLEMENT_DYNCREATE(CColorOptionPage, CPropertyPage)

CColorOptionPage::CColorOptionPage() : CPropertyPage(CColorOptionPage::IDD),
	m_wndStAnchorColor		( prop_AnchorColor		),
	m_wndStMapAnchorColor	( prop_AnchorMapColor	),
	m_wndStRefPointColor	( prop_RefPointColor	),
	m_wndStPointNumberColor	( prop_PointNumberColor	),
	m_wndStExtentColor		( prop_ExtentColor		)
{
	//{{AFX_DATA_INIT(CColorOptionPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CColorOptionPage::~CColorOptionPage()
{
}

void CColorOptionPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CColorOptionPage)
	DDX_Control(pDX, IDC_STATIC_EXTENT_COLOR, m_wndStExtentColor);
	DDX_Control(pDX, IDC_STATIC_COLOR_POINTNUMBER, m_wndStPointNumberColor);
	DDX_Control(pDX, IDC_STATIC_REF_COLOR, m_wndStRefPointColor);
	DDX_Control(pDX, IDC_STATIC_MAPANCHOR_COLOR, m_wndStMapAnchorColor);
	DDX_Control(pDX, IDC_STATIC_ANCHOR_COLOR, m_wndStAnchorColor);
	DDX_Control(pDX, IDC_LIST_POINT, m_wndPoint);
	DDX_Control(pDX, IDC_LIST_MASTER, m_wndMaster);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CColorOptionPage, CPropertyPage)
	//{{AFX_MSG_MAP(CColorOptionPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorOptionPage message handlers

BOOL CColorOptionPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	CString strMaster, strGroup, strColor;
	strMaster.LoadString( IDS_MASTER );
	strGroup.LoadString( IDS_GROUP );
	strColor.LoadString( IDS_COLOR );
	m_wndMaster.Init(strMaster , strColor, 50, 50 );
	m_wndPoint.Init(strGroup , strColor, 50, 50 );
/*
	m_wndMaster.AddColor( 1, RGB(255, 0, 0 ) );
	m_wndMaster.AddColor( 2, RGB(255, 0, 0 ) );
	m_wndMaster.AddColor( 3, RGB(255, 0, 0 ) );
*/
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
