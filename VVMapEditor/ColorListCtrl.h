#if !defined(AFX_COLORLISTCTRL_H__83BAF882_7F94_4030_8775_380C8B1070EC__INCLUDED_)
#define AFX_COLORLISTCTRL_H__83BAF882_7F94_4030_8775_380C8B1070EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorListCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColorListCtrl window
#pragma warning(disable : 4786)
#include <map>

typedef std::map< int, DWORD > ColorMap_t;

class CColorListCtrl : public CListCtrl
{
// Construction
	ColorMap_t	m_ColorMap;
public:
	CColorListCtrl();

// Attributes
public:

	void				SetColorMap( const ColorMap_t& cm)	{ m_ColorMap = cm;	}
	const ColorMap_t&	GetColorMap() const					{ return m_ColorMap;}

	void				AddColor( int Number, DWORD dwColor );

	void				Init( CString strName, CString strColors, int nNameWidth, int nColorWidth );
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CColorListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CColorListCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual  void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	afx_msg BOOL OnClick ( NMHDR* pNotifyStruct, LRESULT* result );
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORLISTCTRL_H__83BAF882_7F94_4030_8775_380C8B1070EC__INCLUDED_)
