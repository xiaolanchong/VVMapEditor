// ColorListCtrl.cpp : implementation file
//

#include "stdafx.h"
//#include "test_ColorList.h"
#include "ColorListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorListCtrl

CColorListCtrl::CColorListCtrl()
{
}

CColorListCtrl::~CColorListCtrl()
{
}


BEGIN_MESSAGE_MAP(CColorListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CColorListCtrl)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT_EX(NM_CLICK, OnClick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorListCtrl message handlers

int CColorListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	

	return 0;
}

void	CColorListCtrl::AddColor( int Number, DWORD dwColor )
{
	std::pair<ColorMap_t::iterator, bool> p = m_ColorMap.insert( std::make_pair( Number, dwColor ) );
	//NOTENOTE: if inserting is valid then update list
	if (p.second) 
	{
		InsertItem( 0, _T("") );
	}
}

void CColorListCtrl::Init(CString strName, CString strColors, int nNameWidth, int nColorWidth )
{
	InsertColumn( 0, strName, LVCFMT_CENTER, nNameWidth );
	InsertColumn( 1, strColors, LVCFMT_CENTER, nColorWidth);

	ColorMap_t::iterator itr = m_ColorMap.begin();
	for( ; itr != m_ColorMap.end() ;++itr)
	{
		InsertItem( 0, _T("") );
	}
/*
	AddColor( 1, RGB(255 ,0 ,0 ) );
	AddColor( 2, RGB(0 ,255 ,0 ) );
	AddColor( 3, RGB(0 ,255 ,255 ) );*/
}

void CColorListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDC dc;
	dc.Attach( lpDrawItemStruct->hDC );
	CRect rcItem( lpDrawItemStruct->rcItem );

	int nNameWidth	= GetColumnWidth(0);
	int nColorWidth	= GetColumnWidth(1);

	CRect rcName( rcItem ); rcName.right = nNameWidth -1;
	CRect rcColor( rcItem ); rcColor.left = nNameWidth ;

	int nIndex = lpDrawItemStruct->itemID;
	ColorMap_t::const_iterator itr = m_ColorMap.begin();
	std::advance( itr, nIndex );
	
	CString strName;
	strName.Format(_T("%d"), itr->first );
	Color cl( itr->second );
	COLORREF rgColor = cl.ToCOLORREF();
	
	rcColor.DeflateRect( 1 , 1 ,1 ,1);
//	dc.FillSolidRect( rcName, RGB(255,0,0) );
	dc.DrawText( strName, -1, rcName, DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_END_ELLIPSIS  );
	dc.FillSolidRect( rcColor, rgColor );

	dc.Detach();
}

BOOL CColorListCtrl::OnClick ( NMHDR* /*NMITEMACTIVATE */ pNotifyStruct, LRESULT* result )
{
	NMITEMACTIVATE* pNMActivate = (NMITEMACTIVATE*)pNotifyStruct;
	int nIndex = pNMActivate->iItem;
	ColorMap_t::iterator itr = m_ColorMap.begin();
	std::advance( itr, nIndex );
	
	CColorDialog dlg( itr->second, 0 ,this  );
	if ( dlg.DoModal() == IDOK)
	{
		COLORREF dwColor	=  dlg.GetColor( );
		Gdiplus::Color cl;
		cl.SetFromCOLORREF( dwColor );
		itr->second = cl.GetValue();
		Invalidate();
	}

	*result = 0;
	return TRUE;
}