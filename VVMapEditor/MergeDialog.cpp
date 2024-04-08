// MergeDialog.cpp : implementation file
//

#include "stdafx.h"
#include "VVMapEditor.h"
#include "MergeDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMergeDialog dialog


CMergeDialog::CMergeDialog(CWnd* pParent /*=NULL*/, const std::vector<int>& Ids)
	: CDialog(CMergeDialog::IDD, pParent),
		m_Ids(Ids)
{
	//{{AFX_DATA_INIT(CMergeDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMergeDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMergeDialog)
	DDX_Control(pDX, IDC_RIGHT, m_wndRight);
	DDX_Control(pDX, IDC_LEFT, m_wndLeft);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMergeDialog, CDialog)
	//{{AFX_MSG_MAP(CMergeDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMergeDialog message handlers

void CMergeDialog::OnOK() 
{
	// TODO: Add extra validation here
	
	CString left, right;
	GetDlgItemText(IDC_LEFT, left);
	GetDlgItemText(IDC_RIGHT, right);

	if(left == right)
	{
		AfxMessageBox( IDS_DIFF_IDS );
	}
	else
	{
		CString left, right;
		m_wndLeft.GetWindowText( left );
		m_wndRight.GetWindowText( right );
		m_nLeft =  _ttoi(left);
		m_nRight = _ttoi(right);
		CDialog::OnOK();
	}
}

BOOL CMergeDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	for( int i=0; i< m_Ids.size(); ++i )
	{
		CString str;
		str.Format(_T("%d"), m_Ids[i]);
		m_wndLeft.AddString( str );
		m_wndRight.AddString( str );
	}
	
	m_wndLeft.SetCurSel(0);
	m_wndRight.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
