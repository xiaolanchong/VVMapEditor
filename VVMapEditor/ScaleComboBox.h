//========================= Copyright © 2004, Elvees ==========================
// Author: Eugene V. Gorbachev
// Purpose: Word-like ComboBox  for scaling
//
//=============================================================================


#if !defined(AFX_SCALECOMBOBOX_H__59D40EC2_FDDF_4253_B860_26F55AE017B3__INCLUDED_)
#define AFX_SCALECOMBOBOX_H__59D40EC2_FDDF_4253_B860_26F55AE017B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScaleComboBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CScaleComboBox window

#include <vector>
#include <GuiComboBoxExt.h>

#define WM_SCALE		WM_USER + 0x50
#define WM_FIT			WM_USER + 0x51

//-----------------------------------------------------------------------------
// Purpose: When scale changed send to parent window message
//-----------------------------------------------------------------------------
class CScaleComboBox : public CGuiComboBoxExt
{
	std::vector<float>	m_Scales;
// Construction
public:
	CScaleComboBox();
	CFont m_Font;
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScaleComboBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CScaleComboBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CScaleComboBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnEditchange();
	afx_msg void OnEditupdate();
	afx_msg void OnSelchange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCALECOMBOBOX_H__59D40EC2_FDDF_4253_B860_26F55AE017B3__INCLUDED_)