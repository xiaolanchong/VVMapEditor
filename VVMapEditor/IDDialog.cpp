// IDDialog.cpp : implementation file
//

#include "stdafx.h"
#include "VVMapEditor.h"
#include "IDDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIDDialog dialog


CIDDialog::CIDDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CIDDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIDDialog)
	m_nID = 0;
	//}}AFX_DATA_INIT
}


void CIDDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIDDialog)
	DDX_Text(pDX, IDC_EDIT1, m_nID);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIDDialog, CDialog)
	//{{AFX_MSG_MAP(CIDDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIDDialog message handlers
