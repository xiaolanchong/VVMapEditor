// AnchorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VVMapEditor.h"
#include "AnchorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAnchorDlg dialog


CAnchorDlg::CAnchorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAnchorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAnchorDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CAnchorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAnchorDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAnchorDlg, CDialog)
	//{{AFX_MSG_MAP(CAnchorDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnchorDlg message handlers
