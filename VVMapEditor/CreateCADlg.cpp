// CreateCADlg.cpp : implementation file
//

#include "stdafx.h"
#include "VVMapEditor.h"
#include "CreateCADlg.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCreateCADlg dialog


CCreateCADlg::CCreateCADlg(CWnd* pParent /*=NULL*/)
: CDialog(CCreateCADlg::IDD, pParent), m_Map( std::make_pair( false, false ) )
{
	//{{AFX_DATA_INIT(CCreateCADlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CCreateCADlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCreateCADlg)
	DDX_Control(pDX, IDC_ID_SELECT, m_listSelect);
	DDX_Control(pDX, IDC_ID_ALL, m_listUnselect);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCreateCADlg, CDialog)
	//{{AFX_MSG_MAP(CCreateCADlg)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_ADD_ALL, OnAddAll)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_REMOVE_ALL, OnRemoveAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCreateCADlg message handlers

void CCreateCADlg::OnAdd() 
{
	// TODO: Add your control notification handler code here
	int n = m_listUnselect.GetCurSel();
	if(n == LB_ERR) return;
	CString s ;
	m_listUnselect.GetText(n, s);
	m_listUnselect.DeleteString(n);
	m_listSelect.AddString(s);
}

void CCreateCADlg::OnAddAll() 
{
	// TODO: Add your control notification handler code here
	std::vector<CString> v;
	for(int i=0; i < m_listUnselect.GetCount(); ++i )
	{
		CString s;
		m_listUnselect.GetText(i, s);
		v.push_back(s);
	}
	m_listUnselect.ResetContent();
	for( i=0; i < v.size(); ++i )
	{
		m_listSelect.AddString( v[i] );
	}	
}

void CCreateCADlg::OnRemove() 
{
	// TODO: Add your control notification handler code here
	int n = m_listSelect.GetCurSel();
	if(n == LB_ERR) return;
	CString s ;
	m_listSelect.GetText(n, s);
	m_listSelect.DeleteString(n);
	m_listUnselect.AddString(s);	
}

void CCreateCADlg::OnRemoveAll() 
{
	// TODO: Add your control notification handler code here
	std::vector<CString> v;
	for(int i=0; i < m_listSelect.GetCount(); ++i )
	{
		CString s;
		m_listSelect.GetText(i, s);
		v.push_back(s);
	}
	m_listSelect.ResetContent();
	for( i=0; i < v.size(); ++i )
	{
		m_listUnselect.AddString( v[i] );
	}
}

BOOL CCreateCADlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	for( int i = 0; i < m_All.size(); ++i)
	{
		CString str; str.Format(_T("%d"), m_All[i]);
		std::vector<int>::const_iterator itr = std::find( m_Select.begin(), m_Select.end(), m_All[i] );
		if(itr != m_Select.end())
		{
			m_listSelect.AddString( str );
		}
		else
		{
			m_listUnselect.AddString( str );
		}
	}
	CString strMap;
	strMap.LoadString( IDS_MAP );
	if(m_Map.first)	m_Map.second? m_listSelect.AddString(strMap) : m_listUnselect.AddString(strMap);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCreateCADlg::OnOK() 
{
	// TODO: Add extra validation here
	if( m_listSelect.GetCount() < 2 )
	{
		//AfxMessageBox(_T("Add more items")); 
		AfxMessageBox( IDS_ADD_ITEMS );
		return;
	}
	m_All.clear();
	m_Select.clear();
	CString strMap;
	strMap.LoadString( IDS_MAP );
	for(int i=0; i < m_listSelect.GetCount(); ++i )
	{
		CString s;
		m_listSelect.GetText(i, s);
		if( s == strMap )	m_Map.second = true;
		else 
		{
			int Id = _ttoi(s);
			m_Select.push_back(Id);
		}
	}

	
	CDialog::OnOK();
}

int CCreateCADlg::OnCompareItem( int nIDCtl, LPCOMPAREITEMSTRUCT lpCompareItemStruct )
{
	return 0;
}