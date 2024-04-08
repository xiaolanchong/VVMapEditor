#if !defined(AFX_QUALITYBAR_H__B2B5EECE_323B_4FAF_BB42_E8EAA0DAD81E__INCLUDED_)
#define AFX_QUALITYBAR_H__B2B5EECE_323B_4FAF_BB42_E8EAA0DAD81E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// QualityBar.h : header file
//

#include "GuiControlBar.h"
#include <vector>

struct MergeQuality
{
	int		m_nFrom;
	int		m_nTo;
	long	m_nQuality;

	MergeQuality(int From, int To, long nQuality): m_nFrom(From), m_nTo(To), m_nQuality(nQuality){}
};



/////////////////////////////////////////////////////////////////////////////
// CQualityBar window

class CQualityBar : public CGuiControlBar
{
// Construction
	CListCtrl		m_wndQuality;
public:
	CQualityBar();

// Attributes
public:

	BOOL			Create( CWnd* pWndParent );
	void			Refresh(const std::vector<MergeQuality>& ArrQuality);
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQualityBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CQualityBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CQualityBar)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QUALITYBAR_H__B2B5EECE_323B_4FAF_BB42_E8EAA0DAD81E__INCLUDED_)
