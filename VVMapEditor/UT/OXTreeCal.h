#ifndef _OX_TREE_CAL_H
#define _OX_TREE_CAL_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"

#include "OXCalendar.h"


// ==========================================================================
// 					Class Specification : 
//						COXTreeCal
// ==========================================================================

// Header file : COXTreeCal.h

// Copyright � Dundas Software Ltd. 1997 - 1998, All Rights Reserved
                         
// //////////////////////////////////////////////////////////////////////////

// Properties:
//	NO	Abstract class (does not have any objects)
//	YES	Derived from COXCalendar

//	YES	Is a Cwnd.                     
//	YES	Two stage creation (constructor & Create())
//	YES	Has a message map
//	NO	Needs a resource (template)

//	NO	Persistent objects (saveable on disk)      
//	NO	Uses exceptions

// //////////////////////////////////////////////////////////////////////////

//Description:
// This is a small helper class designed for
//in-place date picker in COXTreeCtrl

class OX_CLASS_DECL COXTreeCal : public COXCalendar
{
public:

	virtual void PostNcDestroy();
	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : Called by the default OnNcDestroy member function after the window 
	//				has been destroyed. Our class uses this function for cleanup

	BOOL Create(DWORD dwStyle,const CRect& r ,CWnd * pParentWnd,UINT nID);
	// --- In  :	dwStyle		-	Specifies the window style attributes.
	//				r			-   The size and position of the window, in client 
	//								coordinates of pParentWnd.
	//				pParentWnd	-   The parent window.
	//				nID			-	The ID of the child window.
	// --- Out : 
	// --- Returns: TRUE if the calendar was successfully created or FALSE otherwise
	// --- Effect : Creates calendar popup control

	void Init(HTREEITEM hItem,int iItem,int iSubItem);
	// --- In  :	hItem - Specifies handle to the item
	//				iItem - Specifies the zero-based index of the item 
	//				iSubItem - Specifies the index of column
	// --- Out : 
	// --- Returns:
	// --- Effect : performs initialization

	COXTreeCal();
	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : constructor

public:

public:

public:
	virtual ~COXTreeCal();
	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : destructor

protected:
	void FinishEdit(BOOL bOK);
	BOOL PreTranslateMessage(MSG* pMsg);
	int m_iSubItem;
	int m_iItem;
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnKillFocusEdit();
	DECLARE_MESSAGE_MAP()
};

#endif