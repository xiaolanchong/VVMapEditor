// AnchorView.cpp : implementation file
//

#include "stdafx.h"
#include "VVMapEditor.h"
#include "VVMapEditorDoc.h"



#include "AnchorView.h"
#include  <atlbase.h>

//#include "res/resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAnchorView

IMPLEMENT_DYNCREATE(CAnchorView, CParentView)

CAnchorView::CAnchorView()
	: /*CParentView(CAnchorView::IDD), */m_bInit(FALSE), m_nSelectedItem(-1), m_wndGrid(this)
{
	//{{AFX_DATA_INIT(CAnchorView)
	//}}AFX_DATA_INIT
}

CAnchorView::~CAnchorView()
{
	
}

void CAnchorView::DoDataExchange(CDataExchange* pDX)
{
	CParentView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAnchorView)
//	DDX_Control(pDX, IDC_ANCHOR_GRID, m_wndGrid);
//	DDX_Control(pDX, IDC_LIST_ANCHORS, m_wndAnchor);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAnchorView, CParentView)
	//{{AFX_MSG_MAP(CAnchorView)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_NOTIFY( GVN_ENDLABELEDIT, IDC_ANCHOR_GRID, OnNotifyEndLabelEdit )
	//}}AFX_MSG_MAP
//	ON_UPDATE_COMMAND_UI( IDC_EDIT_X, OnUpdateEditX )
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CGridCtrlEx, CGridCtrl)
	//{{AFX_MSG_MAP(CGridCtrlEx)
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnchorView diagnostics

#ifdef _DEBUG
void CAnchorView::AssertValid() const
{
	CParentView::AssertValid();
}

void CAnchorView::Dump(CDumpContext& dc) const
{
	CParentView::Dump(dc);
}
#endif //_DEBUG

CVVMapEditorDoc*	CAnchorView::GetDocument() { return (CVVMapEditorDoc*)m_pDocument;}

/////////////////////////////////////////////////////////////////////////////
// CAnchorView message handlers

BOOL CAnchorView::Create(LPCTSTR cl, LPCTSTR w, DWORD s,
		const RECT& r, CWnd* p , UINT id, CCreateContext* c)
{
	s &= ~WS_HSCROLL;
	s &= ~WS_VSCROLL;
	s &= ~WS_BORDER;
	BOOL res = CParentView::Create(cl, w, s, r, p, id, c);
	return res;
}

void CAnchorView::OnInitialUpdate() 
{
	CParentView::OnInitialUpdate();

	SetScrollSizes( MM_TEXT, CSize(1,1) );
	if(!m_bInit)
	{
		// TODO: Add your specialized creation code here
		 m_bInit = true;

		 m_ilMapAnchor.Create( IDB_ANCHOR_ON_MAP, 16, 1, RGB(255, 255, 255)   );
		 m_wndGrid.SetImageList( &m_ilMapAnchor );

		 m_wndGrid.SetBkColor( GetSysColor( COLOR_BTNFACE ) );
	}

	UpdateList();
}

void CAnchorView::OnSize(UINT nType, int cx, int cy) 
{
	if(  ::IsWindow(  m_wndGrid.GetSafeHwnd() ) )
		m_wndGrid.SetWindowPos( 0, 0, 0, cx, cy, SWP_NOZORDER |SWP_NOMOVE );
	CParentView::OnSize(nType, cx, cy);
}

int CAnchorView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CParentView::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_wndGrid.Create( CRect(0,0, 200, 50), this, IDC_ANCHOR_GRID );
//	m_wndGrid
	return 0;
}

void CAnchorView::UpdateList()
{
	
	m_wndGrid.DeleteAllItems();

	CVVMapEditorDoc* pDoc = GetDocument();
	const AnchorPointMap_t& ap = pDoc->AnchorPoints();

	
	m_wndGrid. SetFixedRowCount(1);
	m_wndGrid. SetFixedColumnCount(1);

	//FIXME: to resources
		m_wndGrid.InsertColumn( _T( "X" ) );
		m_wndGrid.InsertColumn( _T( "Y" ) );
		m_wndGrid.InsertColumn( _T( "Z" ) );
		m_wndGrid.InsertColumn( _T( "Rel" ) );

		m_wndGrid.SetColumnWidth( 0, 30);
		m_wndGrid.SetColumnWidth( 1, 50);
		m_wndGrid.SetColumnWidth( 2, 50);
		m_wndGrid.SetColumnWidth( 3, 50);
		m_wndGrid.SetColumnWidth( 4, 40);
		m_wndGrid.EnableSelection();

		m_wndGrid.SetColumnResize(TRUE);
		m_wndGrid.SetRowResize(FALSE);
		m_wndGrid.SetSingleRowSelection(TRUE);

	const UINT c_uItemFormat = DT_RIGHT|DT_SINGLELINE|DT_END_ELLIPSIS ;

	for(int i=0; i< ap.size(); ++i)
	{
		CString strNumber, strX, strY,strZ, strRel;

		int nRow = InsertRow( i );

		if( ap[i].IsOnMap() )
		{
			m_wndGrid.GetCell( nRow, COLUMN_NUMBER )->SetBackClr( GetSysColor( COLOR_WINDOW ) );
		}

		strX.Format(_T("%.3f"), ap[i].m_x);
		m_wndGrid.SetItemText( nRow, COLUMN_X_RO, strX );
		m_wndGrid.SetItemFormat( nRow, COLUMN_X_RO, c_uItemFormat );

		strY.Format(_T("%.3f"), ap[i].m_y);
		m_wndGrid.SetItemText( nRow, COLUMN_Y_PHI, strY );
		m_wndGrid.SetItemFormat( nRow, COLUMN_Y_PHI, c_uItemFormat );

		strZ.Format(_T("%.3f"), ap[i].m_z);
		m_wndGrid.SetItemText( nRow, COLUMN_Z_PSI, strZ );
		m_wndGrid.SetItemFormat( nRow, COLUMN_Z_PSI, c_uItemFormat );
		
		if(ap[i].m_Relative != AnchorPoint_t::NO_DEPENDENCE)
		{
			strRel.Format(_T("%d"), ap[i].m_Relative);
			m_wndGrid.SetItemText( nRow, COLUMN_RELATIVE, strRel );
			m_wndGrid.SetItemFormat( nRow, COLUMN_RELATIVE, c_uItemFormat );
		}

//		m_wndGrid.SetRowHeight( nRow, 25 );
		
	}
	InsertRow( ap.size()  );
}

void	CAnchorView::ChangeCoords(int nRow)
{
//	int nItems = m_.GetItemCount();
	CString strX, strY, strZ, strRel;
	AnchorPoint_t Anchor;
	strX = m_wndGrid.GetItemText(nRow, COLUMN_X_RO );
	strY = m_wndGrid.GetItemText(nRow, COLUMN_Y_PHI);
	strZ = m_wndGrid.GetItemText(nRow, COLUMN_Z_PSI);
	strRel = m_wndGrid.GetItemText(nRow, COLUMN_RELATIVE);
	Anchor.m_x = _tcstod( strX, 0 );
	Anchor.m_y = _tcstod( strY, 0 );
	Anchor.m_z = _tcstod( strZ, 0 );
	Anchor.m_Relative = strRel.IsEmpty() ? AnchorPoint_t::NO_DEPENDENCE : _ttoi( strRel );
	CVVMapEditorDoc* pDoc = GetDocument();

	int nIndex = RowToAnchorIndex( nRow );
	pDoc->ChangeAnchorPoint(nIndex, Anchor);
}

void	CAnchorView::InsertCoords(int nRow)
{
	CString strX, strY, strZ, strRel;
//	AnchorPoint_t Anchor;
	strX = m_wndGrid.GetItemText(nRow, COLUMN_X_RO );
	strY = m_wndGrid.GetItemText(nRow, COLUMN_Y_PHI);
	strZ = m_wndGrid.GetItemText(nRow, COLUMN_Z_PSI);
	strRel = m_wndGrid.GetItemText(nRow, COLUMN_RELATIVE);
	double x = _tcstod( strX, 0 );
	double y = _tcstod( strY, 0 );
	double z = _tcstod( strZ, 0 );
	double Relative = strRel.IsEmpty() ? AnchorPoint_t::NO_DEPENDENCE : _ttoi( strRel );
	CVVMapEditorDoc* pDoc = GetDocument();

	int nIndex = RowToAnchorIndex( nRow );
	pDoc->AddAnchorPoint( x, y, z, Relative );
}

void	CAnchorView::OnUpdateCmdUI()
{
	CVVMapEditorDoc* pDoc = GetDocument();
	BOOL bEnable = pDoc->m_State == CVVMapEditorDoc::edit_anchor_points;

	GetDlgItem(IDC_ANCHOR_GRID)->EnableWindow(bEnable);
}

void	CAnchorView::SelectItem	(int nItem)
{
	m_nSelectedItem = nItem;
	int nRow = AnchorIndexToRow(nItem);
	m_clSelectColor = m_wndGrid.GetCell( nRow, COLUMN_NUMBER )->GetBackClr();
	m_wndGrid.GetCell( nRow, COLUMN_NUMBER )->SetBackClr( RGB(255, 0, 0) );
	m_wndGrid.RedrawCell( nRow, COLUMN_NUMBER );
}

void	CAnchorView::InvalidateSelectItem()
{
	if(m_nSelectedItem != -1)
	{
		int nRow = AnchorIndexToRow(m_nSelectedItem );
		m_wndGrid.GetCell( nRow, COLUMN_NUMBER )->SetBackClr( m_clSelectColor );
		m_wndGrid.RedrawCell( nRow, COLUMN_NUMBER );
		m_nSelectedItem= -1;
	}

}

void CAnchorView::OnNotifyEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_GRIDVIEW* pGridView = (NM_GRIDVIEW*)pNMHDR;
	*pResult = TRUE;
}

void  CGridCtrlEx::OnEndEditCell(int nRow, int nCol, CString str)
{
	CGridCtrl::OnEndEditCell( nRow, nCol, str );
	m_pView->OnEndEditCell( nRow, nCol, str);
}

BOOL  CGridCtrlEx::ValidateEdit(int nRow, int nCol, LPCTSTR str)
{
	return m_pView->ValidateEdit( nRow, nCol, str );
}

void CGridCtrlEx::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	if( nChar == VK_DELETE )
	{
		bool bShift = ( GetKeyState( VK_SHIFT ) < 0 );
		m_pView->OnDeleteKey( bShift );
	}
	else 
		CGridCtrl::OnKeyDown( nChar, nRepCnt, nFlags );
}

void  CAnchorView::OnEndEditCell(int nRow, int nCol, CString str)
{
	CVVMapEditorDoc* pDoc = GetDocument();
	int z = m_wndGrid.GetRowCount() - 1;

	if( nRow == z && ValidateEdit( nRow,  nCol,  str))
	{
		const AnchorPointMap_t& ap = pDoc->AnchorPoints();
		InsertRow( ap.size() + 1 );

		for( int i= COLUMN_X_RO; i <= COLUMN_Z_PSI; ++i)
		{
			CString str = m_wndGrid.GetItemText( nRow, i );
			if( str.IsEmpty() )
			{
				str.Format(_T("%.3f"), 0.0);
				m_wndGrid.SetItemText( nRow, i, str );
			}
		}
		InsertCoords(nRow);
		m_wndGrid.SetFocusCell( nRow, nCol );

		m_wndGrid.RedrawWindow();
	}
	else
	{
		ChangeCoords( nRow );
	}
}

BOOL  CAnchorView::ValidateEdit(int nRow, int nCol, LPCTSTR str)
{
	LPTSTR pLast;
	int nRel; double fCoord;
	
	if( nCol == COLUMN_RELATIVE )
	{
		nRel = _tcstol( str, & pLast, 10 ); 
	}
	else
	{
		fCoord = _tcstod( str, &pLast );
	}

	bool bInputValid = ( pLast && *pLast == _T('\0') );

	if( !bInputValid ) return false;

	if( nCol != COLUMN_RELATIVE ) return true;

	const CVVMapEditorDoc* pDoc = GetDocument();
	const AnchorPointMap_t& apm = pDoc->AnchorPoints();

	return nRel >= 0 && nRel <= ( apm.size() -1 ) ;
	
}

int	CAnchorView::InsertRow(int nName)
{
	CString strNumber;
	strNumber.Format( _T("%d"), nName ); 
	int z = m_wndGrid.InsertRow( strNumber  );	
		
	m_wndGrid.SetItemFormat( z, COLUMN_NUMBER, DT_CENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
	for( int i = COLUMN_X_RO; i <= COLUMN_RELATIVE; ++i)
	{
		m_wndGrid.SetItemFormat( z, i, DT_RIGHT | DT_SINGLELINE | DT_END_ELLIPSIS);
	}
	return z;
}

void	CAnchorView::OnDeleteKey( bool bShift )
{
	CVVMapEditorDoc* pDoc = GetDocument();
	CCellRange Range = m_wndGrid.GetSelectedCellRange();
	int nRowCount = m_wndGrid.GetRowCount();

	for( int i = Range.GetMinRow(); i <= Range.GetMaxRow(); ++i )
	{
		if( nRowCount - 1 != i )
		{
			int nIndex = RowToAnchorIndex(i );
			bShift? pDoc->RemoveAnchorPointFromMap( nIndex ) : pDoc->RemoveAnchorPoint( nIndex )  ;
		}
	}
	UpdateList();

}
