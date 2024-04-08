// SetIDDialog.cpp : implementation file
//

#include "stdafx.h"
#include "VVMapEditor.h"
#include "SetIDDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetIDDialog dialog


CSetIDDialog::CSetIDDialog(CWnd* pParent /*=NULL*/, const std::vector<int>& Ids)
	: CDialog(CSetIDDialog::IDD, pParent),
		m_Ids(Ids)
{
	//{{AFX_DATA_INIT(CSetIDDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSetIDDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetIDDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetIDDialog, CDialog)
	//{{AFX_MSG_MAP(CSetIDDialog)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetIDDialog message handlers

BOOL CSetIDDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	m_wndProperty.SubclassDlgItem( IDC_LIST2, this );

	for( int i=0; i< m_Ids.size(); ++i)
	{
		CString str;
		str.Format(_T("%d"), m_Ids[i]);
		m_wndProperty.SetEditItem( str, str );
	}
	
//	GetClientRect(&rc);
//	m_sizeWnd = CSize( rc.Width() , rc.Height());
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSetIDDialog::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
}

void CSetIDDialog::OnOK()
{
	
	for(int i=0; i<m_Ids.size(); ++i)
	{
		CString str;
		str.Format( _T("%d"), m_Ids[i] );
		CString Value = m_wndProperty[str];
		int num = _ttoi(Value);
		m_Result.push_back( num );
	}

	std::vector<int > AnResult = m_Result;
	std::sort( AnResult.begin(), AnResult.end()); 
	std::vector<int>::const_iterator itr = std::adjacent_find( AnResult.begin(), AnResult.end() );
	if( itr != AnResult.end() )
	{
		AfxMessageBox(IDS_ERR_WRONGID  );
		return;
	}

	CDialog::OnOK();
}

/////////////===============================================///////////////////////////////////


CLoadIDDialog::CLoadIDDialog(CWnd* pParent , 
					const std::vector<int>&	Need,
					const std::vector<int>&	Have)
	: CDialog(CLoadIDDialog::IDD, pParent),
		m_Have(Have), 
		m_Need(Need)
{
	//{{AFX_DATA_INIT(CSetIDDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CLoadIDDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetIDDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLoadIDDialog, CDialog)
	//{{AFX_MSG_MAP(CSetIDDialog)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetIDDialog message handlers

BOOL CLoadIDDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	m_wndProperty.SubclassDlgItem( IDC_LIST2, this );

	std::vector<CString> str;
	str.resize( m_Have.size() + 1 );

	for( int i=0; i< m_Have.size(); ++i)
	{
		str[i].Format( _T("%d"), m_Have[i]);
	}
	str.back() = _T("New");

	for(  i=0; i< m_Need.size(); ++i)
	{
		CString Name;
		Name.Format(_T("%d"), m_Need[i]);
		std::vector<int>::const_iterator itr = std::find(m_Have.begin(), m_Have.end(), m_Need[i]);
		int nDefault = str.size()-1;
		if( itr != m_Have.end()) nDefault = itr - m_Have.begin();
		m_wndProperty.SetComboBoxItem( Name, str, nDefault, false  );
	}
	
//	GetClientRect(&rc);
//	m_sizeWnd = CSize( rc.Width() , rc.Height());
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CLoadIDDialog::OnOK()
{
	std::vector<int> AnResult;
	m_Result.resize( m_Need.size() );
	for( int i = 0; i< m_Need.size(); ++i)
	{
		m_Result[i].first = m_Need[i];
		CString str;
		str.Format(_T("%d"), m_Need[i]  );
		CString Value = m_wndProperty[str];
		if( Value == _T("New")) m_Result[i].second = -1;
		else
		{
			int n = _ttoi( Value );
			m_Result[i].second = n;
			AnResult.push_back( n );
		}
	}

	std::sort( AnResult.begin(), AnResult.end()); 
	std::vector<int>::const_iterator itr = std::adjacent_find( AnResult.begin(), AnResult.end() );
	if( itr != AnResult.end() )
	{
		AfxMessageBox(IDS_ERR_WRONGID  );
		return;
	}	

	CDialog::OnOK();
}