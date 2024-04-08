// PointTree.cpp : implementation file
//

#include "stdafx.h"
#include "PointTree.h"

#include "MainFrm.h"
#include "VVMapEditorDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPointTree

CPointTree::CPointTree():m_nSelectGroup( std::make_pair( false,  -1))
{
}

CPointTree::~CPointTree()
{
}


BEGIN_MESSAGE_MAP(CPointTree, COXTreeCtrl)
	//{{AFX_MSG_MAP(CPointTree)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_POINTS_DELETE, OnPointDelete)
	ON_COMMAND(ID_GROUP_DELETE, OnGroupDelete)
	ON_WM_CREATE()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPointTree message handlers

void CPointTree::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	
	COXTreeCtrl::OnLButtonDown(nFlags, point);
}

void CPointTree::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
/*
	CPoint pt(point);
	HTREEITEM hti = HitTest( pt , &nFlags );
	if(hti > 0 )
	{
		if(!ItemHasChildren( hti ))
		{
			CString s = GetItemText(hti );
			AfxMessageBox( s );
		}
	}*/
	COXTreeCtrl::OnLButtonDblClk(nFlags, point);
}

CVVMapEditorDoc*	GetDocument()
{
	CMainFrame* pFrame = dynamic_cast<CMainFrame*>(AfxGetMainWnd());
	if( pFrame )
		return (CVVMapEditorDoc*)(pFrame->GetActiveDocument());
	else return 0;
}

void CPointTree::OnRButtonDown(UINT nFlags, CPoint point) 
{
	const int sub_point = 1;
	const int sub_group = 0;
	// TODO: Add your message handler code here and/or call default
	if (GetDocument()->m_State != CVVMapEditorDoc::edit_points) return ;

	CPoint pt(point);
	UINT Flags;
	int TreeIndex = HitTest( pt , &Flags );
	HTREEITEM hti = GetItemFromIndex( TreeIndex );
	HTREEITEM htiSel = GetSelectedItem();
	if(hti != 0 && hti == htiSel && (Flags& LVHT_ONITEMLABEL))
	{
		HTREEITEM htiParent = GetParentItem( hti );
		if(htiParent == GetRootItem() )
		{
			CPoint pt(point);
			ClientToScreen(&pt);
			m_Menu.GetSubMenu(sub_group)->TrackPopupMenu( 0, pt.x, pt.y, this );
			m_ClickPoint = point;
		}
		else
		{
			CPoint pt(point);
			ClientToScreen(&pt);
			m_Menu.GetSubMenu(sub_point)->TrackPopupMenu( 0, pt.x, pt.y, this );
		}
	}

	COXTreeCtrl::OnRButtonDown(nFlags, point);
}


void CPointTree::OnGroupDelete() 
{
	// TODO: Add your command handler code here
	
	HTREEITEM hti = GetSelectedItem();

	if( hti )
	{
		int nIndex = GetItemData(hti);
		GetDocument()->DeleteGroup(nIndex);
		Refresh();
	}
}


void CPointTree::OnPointDelete() 
{
	// TODO: Add your command handler code here
	
	HTREEITEM hti = GetSelectedItem();
	HTREEITEM htiParent = GetParentItem( hti );

	if( hti && htiParent && htiParent != GetRootItem() )
	{
		int nGroup = GetItemData(htiParent);
		int nMaster = GetItemData(hti);

		CString zz  ;
		zz.Format( _T("Master #%d, Point #%d\n"), nMaster, nGroup);
		OutputDebugString( zz);
		
		DeleteItem( hti );
		GetDocument()->DeletePoint(  nGroup, nMaster);
//		Refresh();
	}
}

int CPointTree::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (COXTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	

	m_Menu.LoadMenu(IDR_POINTS);
	return 0;
}

void	CPointTree::Refresh()
{
	CVVMapEditorDoc* pDoc = GetDocument();
	if(!pDoc) return;
	DeleteAllItems();
	if( pDoc->IsCurrent())
	{
		int j;
		int Max = pDoc->GetMaxGroup();

		std::auto_ptr<CFilterCache> pFilter = pDoc->GetFilterCache();
		std::vector<HTREEITEM> htiParents( Max, (HTREEITEM)0 );

		std::set<int> AllGroups = pDoc->GetAllValidGroups();
		std::set<int>::const_iterator itr = AllGroups.begin();

		
		for(; itr != AllGroups.end(); ++itr)
		{
			int z = *itr;
			bool bFiltered = pFilter->IsGroupValid( z );
			if( bFiltered )
			{
				htiParents[z] = InsertGroup( z );
			}
		}
		
		for( j = 0; j < pDoc->m_Ids.size(); ++j)
		{
			const PointArr_t& v = pDoc->Points().m_Points[j];
			CString Name;
			Name.Format( IDS_FMT_ID ,  pDoc->m_Ids[j]);
			for( int i = 0; i < v.size(); ++i)
			{
				if( v[i].first && htiParents[i] != 0 && pFilter->IsPointValid( i, j ) )
				{
					HTREEITEM h = htiParents[i];
					if( !h )	h = htiParents[i] = InsertGroup( i );
					CString strP = 	GetItemText( h );
					HTREEITEM hSubItem = InsertItem( Name, h );
					SetItemData( hSubItem, j );
				}
			}
		}

		if( pDoc->m_bMap )
		{
			const PointArr_t& v = pDoc->Points().m_MapPoints;
			
			for( int i = 0; i < v.size(); ++i)
			{
				if( v[i].first && pFilter->IsPointValid( i, c_nMapIndex ))
				{
					HTREEITEM h = htiParents[i];
					if( !h )	h = htiParents[i] = InsertGroup( i );
					CString Name ; Name.LoadString( IDS_MAP );
					HTREEITEM hSubItem = InsertItem( Name, h );
					SetItemData( hSubItem, -1 );

				}
			}
		}

		for( j = 0; j < Max; ++j )
		{
			if( htiParents[j] )
			Expand( htiParents[j], TVE_EXPAND  );
		}
	}
	m_nSelectGroup = std::make_pair( false,  -1);
}

HTREEITEM CPointTree::InsertGroup( int nGroup )
{
	CVVMapEditorDoc* pDoc = GetDocument();
			CString Name;
			Name.Format( IDS_FMT_GROUP , nGroup);
			HTREEITEM h = InsertItem( Name );
			Color cl = GetSettings().GetPointColor( nGroup );
			SetItemColor(h, cl.ToCOLORREF());
			SetItemData( h, nGroup );
	return h;
}

void CPointTree::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	if ( nChar == VK_DELETE && GetDocument()->m_State == CVVMapEditorDoc::edit_points) 
	{
	
		HTREEITEM hti = GetSelectedItem();
		HTREEITEM htiParent = GetParentItem( hti );

		if( hti )
		{
			if( htiParent == GetRootItem() )	OnGroupDelete();
			else OnPointDelete();
		}
	}
	COXTreeCtrl::OnKeyDown( nChar, nRepCnt, nFlags );
}

void	CPointTree::InvertGroup(int nGroup)
{
	if( m_nSelectGroup.second != nGroup )
	{
		InvalidateInvertGroup();
		m_nSelectGroup = std::make_pair( false,  -1);
	}
	
	if( m_nSelectGroup.second != -1 )
	{
		m_nSelectGroup.first = !m_nSelectGroup.first;
		InvertGroupColor( nGroup );
	}
	else
	{
		m_nSelectGroup = std::make_pair( true,  nGroup);
		InvertGroupColor( nGroup );
	}
}

void	CPointTree::InvalidateInvertGroup()
{
	if( m_nSelectGroup.second != -1 )
	{
		if( m_nSelectGroup.first )
		{
			InvertGroupColor( m_nSelectGroup.second );
			m_nSelectGroup = std::make_pair( false,  -1);
		}
	}
	else
		m_nSelectGroup = std::make_pair( false,  -1);
}

void	CPointTree::InvertGroupColor( int nGroup )
{
	int z = GetItemCount();
	CString Name;
	Name.Format(IDS_FMT_GROUP, nGroup);
	for( int i = 0; i< z; ++i)
	{
		HTREEITEM hti = GetItemFromIndex( i );
		CString str = GetItemText(hti );
		if( str == Name )
		{
			COLORREF cl = GetItemColor(hti);
			COLORREF clXor = RGB( ~GetRValue(cl), ~GetGValue(cl), ~GetBValue(cl)  ) ;
			SetItemColor( hti, clXor );
			break;
		}
	}

}