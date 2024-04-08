// EditTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "VVMapEditor.h"
#include "EditTreeCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditTreeCtrl

CEditTreeCtrl::CEditTreeCtrl()
{
}

CEditTreeCtrl::~CEditTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CEditTreeCtrl, COXTreeCtrl)
	//{{AFX_MSG_MAP(CEditTreeCtrl)
	ON_NOTIFY_REFLECT_EX(LVN_BEGINLABELEDIT, OnBeginlabeledit)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditTreeCtrl message handlers

//NOTENOTE: also eat notify message but before send in to parent
BOOL CEditTreeCtrl::OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult)
{
//	GetParent()->SendMessage( WM_NOTIFY, pNMHDR->idFrom, LPARAM(pNMHDR) );
	GetParent()->SendMessage( WM_FILL_EDIT, pNMHDR->idFrom, LPARAM(pNMHDR) );
	COXTreeCtrl::OnBeginlabeledit(pNMHDR, pResult);
	return TRUE;
}