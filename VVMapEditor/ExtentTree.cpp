// ExtentTree.cpp : implementation file
//

#include "stdafx.h"
#include "VVMapEditor.h"
#include "ExtentTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtentTree

CExtentTree::CExtentTree()
{
}

CExtentTree::~CExtentTree()
{
}


BEGIN_MESSAGE_MAP(CExtentTree, COXTreeCtrl)
	//{{AFX_MSG_MAP(CExtentTree)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExtentTree message handlers

int CExtentTree::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (COXTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	return 0;
}
