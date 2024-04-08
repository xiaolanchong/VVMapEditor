// SortListBox.cpp : implementation file
//

#include "stdafx.h"
#include "VVMapEditor.h"
#include "SortListBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSortListBox

CSortListBox::CSortListBox()
{
}

CSortListBox::~CSortListBox()
{
}


BEGIN_MESSAGE_MAP(CSortListBox, CListBox)
	//{{AFX_MSG_MAP(CSortListBox)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSortListBox message handlers


int CSortListBox::CompareItem( LPCOMPAREITEMSTRUCT lpCompareItemStruct )
{

	 ASSERT(lpCompareItemStruct->CtlType == ODT_LISTBOX);

   LPCTSTR lpszText1 = (LPCTSTR) lpCompareItemStruct->itemData1;
   ASSERT(lpszText1 != NULL);
   LPCTSTR lpszText2 = (LPCTSTR) lpCompareItemStruct->itemData2;
   ASSERT(lpszText2 != NULL);

   return _tcscmp(lpszText2, lpszText1);

}
