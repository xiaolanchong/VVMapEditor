#if !defined(AFX_SORTLISTBOX_H__FDAB0C0C_A1BF_4195_891B_FE28E0CC2358__INCLUDED_)
#define AFX_SORTLISTBOX_H__FDAB0C0C_A1BF_4195_891B_FE28E0CC2358__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SortListBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSortListBox window

//-----------------------------------------------------------------------------
// Purpose: List w/ sorting on camera id
//-----------------------------------------------------------------------------
//TODO: Main logic is missing
class CSortListBox : public CListBox
{
// Construction
public:
	CSortListBox();

// Attributes
public:

	virtual int CompareItem( LPCOMPAREITEMSTRUCT lpCompareItemStruct );

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSortListBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSortListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSortListBox)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SORTLISTBOX_H__FDAB0C0C_A1BF_4195_891B_FE28E0CC2358__INCLUDED_)
