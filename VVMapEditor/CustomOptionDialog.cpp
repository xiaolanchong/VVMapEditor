// CustomOptionDialog.cpp : implementation file
//

#include "stdafx.h"
#include "VVMapEditor.h"
#include "CustomOptionDialog.h"
/*include "VVMapEditorDoc.h"*/
#include "Settings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCustomOptionDialog dialog

CCustomOptionPage::CCustomOptionPage()
:	CPropertyPage(CCustomOptionPage::IDD)
	/*	m_pDoc(pDoc)*/
{
	//{{AFX_DATA_INIT(CCustomOptionDialog)
	m_nMRU = 0;
	//}}AFX_DATA_INIT
}


void CCustomOptionPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCustomOptionDialog)
	DDX_Control(pDX, IDC_SPIN_THICKNESS, m_wndThickness);
	DDX_Control(pDX, IDC_SPIN_EXTENT, m_wndExtent);
	DDX_Control(pDX, IDC_SPIN_MRU, m_wndSpinMRU);
	DDX_Text(pDX, IDC_EDIT_MRU, m_nMRU);
	DDV_MinMaxUInt(pDX, m_nMRU, 0, 16);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCustomOptionPage, CPropertyPage)
	//{{AFX_MSG_MAP(CCustomOptionDialog)
	ON_EN_CHANGE(IDC_EDIT_EXTENT, OnChangeEditExtent)
	ON_EN_CHANGE(IDC_EDIT_THICKNESS, OnChangeEditThickness)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCustomOptionDialog message handlers

BOOL CCustomOptionPage::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	m_wndToolTip.Create( this );
	m_wndToolTip.AddTool( GetDlgItem( IDC_FILTER_NONE ), IDS_FILTER_NONE );
	m_wndToolTip.AddTool( GetDlgItem( IDC_FILTER_WEAK ), IDS_FILTER_WEAK );
	m_wndToolTip.AddTool( GetDlgItem( IDC_FILTER_STRONG ), IDS_FILTER_STRONG );
	m_wndToolTip.AddTool( GetDlgItem( IDC_EDIT_EXTENT ), IDS_TT_CROSS_EXTENT );
	m_wndToolTip.AddTool( GetDlgItem( IDC_EDIT_THICKNESS ), IDS_TT_CROSS_THICKNESS );

	UINT nID = 0;
	switch ( GetSettings().GetFilter() )
	{
		case CSettings::FILTER_NONE: nID = IDC_FILTER_NONE; break;
		case CSettings::FILTER_WEAK: nID = IDC_FILTER_WEAK; break;
		case CSettings::FILTER_STRONG: nID = IDC_FILTER_STRONG; break;
	}

	static_cast< CButton* > ( GetDlgItem( nID ) )->SetCheck( 1 );

	int nMRU = GetSettings().GetProperty( prop_MRU_Number );
	m_wndSpinMRU.SetBuddy( GetDlgItem( IDC_EDIT_MRU ) );
	m_wndSpinMRU.SetRange( 0, 16 );
	m_wndSpinMRU.SetPos( nMRU );

	//TODO: In regestry
	CSize	sizeMaxCross( 50, 10);
	CSize	sizeMinCross( 1, 1 );
	m_wndCross.SubclassDlgItem( IDC_STATIC_CROSS, this );
	m_wndCross.SetExtent( GetSettings().GetProperty(prop_CrossExtent) );
	m_wndCross.SetThickness( GetSettings().GetProperty(prop_CrossThickness) );

	m_wndExtent.SetBuddy( GetDlgItem( IDC_EDIT_EXTENT ) );
	m_wndThickness.SetBuddy( GetDlgItem( IDC_EDIT_THICKNESS ) );

	m_wndExtent.SetRange( sizeMinCross.cx, sizeMaxCross.cx );
	m_wndThickness.SetRange( sizeMinCross.cy, sizeMaxCross.cy );

	m_wndExtent.SetPos( GetSettings().GetProperty(prop_CrossExtent)  );
	m_wndThickness.SetPos( GetSettings().GetProperty(prop_CrossThickness) );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CCustomOptionPage::PreTranslateMessage( MSG *pMsg )
{
   	m_wndToolTip.RelayEvent(pMsg);

	return CDialog::PreTranslateMessage(pMsg);
}

bool IsButtonCheck( CWnd* pParent, UINT nID )
{
	CWnd* pWnd = pParent->GetDlgItem( nID );
	return static_cast< CButton* > ( pWnd )->GetCheck() == 1;
}

void CCustomOptionPage::OnOK() 
{
	// TODO: Add extra validation here
	CSettings::Filter_t Filter;

	if		(  IsButtonCheck(this, IDC_FILTER_NONE )  ) Filter = CSettings::FILTER_NONE;
	else if (  IsButtonCheck(this, IDC_FILTER_WEAK )  ) Filter = CSettings::FILTER_WEAK;
	else if (  IsButtonCheck(this, IDC_FILTER_STRONG )  ) Filter = CSettings::FILTER_STRONG;

	GetSettings().SetFilter ( Filter );
//	GetSettings().SetRecentFiles( m_wndSpinMRU.GetPos() );

//	GetSettings().SetCrossSize( CSize( m_wndExtent.GetPos(), m_wndThickness.GetPos() ) );*/

	GetSettings().SetProperty( prop_CrossExtent,	m_wndExtent.GetPos() );
	GetSettings().SetProperty( prop_CrossThickness, m_wndThickness.GetPos() );
	GetSettings().SetProperty( prop_MRU_Number,		m_wndSpinMRU.GetPos() );
	CDialog::OnOK();
}

void CCustomOptionPage::OnChangeEditExtent() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the C???::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
	int z = m_wndExtent.GetPos();
	m_wndCross.SetExtent( z );
}

void CCustomOptionPage::OnChangeEditThickness() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the C???::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	int z = m_wndThickness.GetPos();
	m_wndCross.SetThickness( z );	
}
