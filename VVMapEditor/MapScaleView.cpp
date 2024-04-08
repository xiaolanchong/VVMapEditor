// MapScaleView.cpp : implementation file
//

#include "stdafx.h"
#include "VVMapEditor.h"
#include "MapScaleView.h"
#include <atlconv.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMapScaleView

#define ID_MENU_NEW		1000

IMPLEMENT_DYNCREATE(CMapScaleView, CFormView)

CMapScaleView::CMapScaleView()
	: CFormView(CMapScaleView::IDD), m_bInit(false)
{
	//{{AFX_DATA_INIT(CMapScaleView)
	m_strEditX = _T("");
	m_strEditY = _T("");
	//}}AFX_DATA_INIT
}

CMapScaleView::~CMapScaleView()
{
}

void CMapScaleView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMapScaleView)
	DDX_Control(pDX, IDC_LIST_OBJECT,	m_wndObject);
//	DDX_Control(pDX, IDC_LIST_DISTANCE, m_wndAnchorDistance);
	DDX_Text(pDX, IDC_EDIT_X, m_strEditX);
	DDX_Text(pDX, IDC_EDIT_Y, m_strEditY);

	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMapScaleView, CFormView)
	//{{AFX_MSG_MAP(CMapScaleView)
	ON_EN_CHANGE(IDC_EDIT_X, OnChangeEditX)
	ON_EN_CHANGE(IDC_EDIT_Y, OnChangeEditY)
	ON_WM_CREATE()
	ON_NOTIFY(NM_RCLICK, IDC_LIST_OBJECT, OnRclickListObject)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_OBJECT, OnEndlabeleditListObject)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_OBJECT, OnKeydownListObject)
	//}}AFX_MSG_MAP
/*	ON_MESSAGE(WM_VALIDATE, OnEndLabelEditVariableCriteria)
	ON_MESSAGE(WM_SET_ITEMS, PopulateComboList)*/
	ON_COMMAND( ID_MENU_NEW, OnMenuNew )
	ON_MESSAGE( WM_FILL_EDIT, OnFillEdit )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapScaleView diagnostics

#ifdef _DEBUG
void CMapScaleView::AssertValid() const
{
	CFormView::AssertValid();
}

void CMapScaleView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMapScaleView message handlers

CVVMapEditorDoc*	CMapScaleView::GetDocument() { return (CVVMapEditorDoc*)m_pDocument;}

/////////////////////////////////////////////////////////////////////////////
// CAnchorView message handlers

BOOL CMapScaleView::Create(LPCTSTR cl, LPCTSTR w, DWORD s,
		const RECT& r, CWnd* p , UINT id, CCreateContext* c)
{
	BOOL res = CFormView::Create(cl, w, s, r, p, id, c);

	return res;
}
void CMapScaleView::OnChangeEditX() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here

	CVVMapEditorDoc* pDoc = GetDocument();
	UpdateData(TRUE);
	double x = _tcstod( m_strEditX, 0 );
	if( x > 0.0 )
		pDoc->SetScaleX( x );
	else
		pDoc->InvalidateScaleY();
	pDoc->UpdateAllViews(0);
//	pDoc->SetScaleX();
	
}

void CMapScaleView::OnChangeEditY() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CVVMapEditorDoc* pDoc = GetDocument();
	UpdateData(TRUE);
	double y = _tcstod( m_strEditY, 0 );
	if( y > 0.0 )
		pDoc->SetScaleY( y );
	else 
		pDoc->InvalidateScaleX();
	pDoc->UpdateAllViews(0);
}

void CMapScaleView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	// TODO: Add your specialized code here and/or call the base class
	CVVMapEditorDoc* pDoc = GetDocument();
	if( pDoc->GetScaleX().first /*&& m_strEditX.IsEmpty() */)
	{
		m_strEditX.Format(_T("%.3f"), pDoc->GetScaleX().second);
	}
	if( pDoc->GetScaleY().first /*&& m_strEditY.IsEmpty()*/ )
	{
		m_strEditY.Format(_T("%.3f"), pDoc->GetScaleY().second);
	}
	UpdateData(FALSE);
}

int CMapScaleView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here

	return 0;
}

void CMapScaleView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class
	if( !m_bInit )
	{
		m_wndObject.ModifyStyle( 0, TVS_HASBUTTONS | TVS_HASLINES|TVS_LINESATROOT|TVS_EDITLABELS);
		m_wndObject.ModifyExStyle(0, TVOXS_VGRID | TVOXS_ROWSEL);
		//NOTENOTE: on click edit containment
		m_wndObject.SetExtendedStyle( m_wndObject.GetExtendedStyle() & ~LVS_EX_ONECLICKACTIVATE );
//		m_wndObject.InsertColumn( 0, _T("Points") );
		CString strColumn;
		strColumn.LoadString( IDS_COLUMN_DISTANCES );
		m_wndObject.InsertColumn( 1, strColumn );
		m_wndObject.SetColumnWidth(0, 100);
		m_wndObject.SetColumnWidth(1, 100);
		LVCOLUMN	lvc;
		lvc.mask = LVCF_TEXT;
		strColumn.LoadString( IDS_COLUMN_POINTS );
		USES_CONVERSION;
		lvc.pszText = /*T2A*/LPTSTR( LPCTSTR( strColumn ) );
		m_wndObject.SetColumn( 0, &lvc);

		CFont* pFont = m_wndObject.GetFont();
		LOGFONT lf;
		pFont->GetLogFont( &lf );
		//FIXME: some sort of Hack
		lf.lfHeight = -15;
		lf.lfWeight = FW_BOLD;
		m_fnObject.CreateFontIndirect( &lf );
	//	lf.lfWeight = FW_NORMAL;
		m_fnPoint.CreateFontIndirect( &lf);
		lf.lfWeight = FW_NORMAL;
		m_fnDistance.CreateFontIndirect( &lf);
		m_bInit = true;
	}

	UpdateTree();
}


void CMapScaleView::OnRclickListObject(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	NMITEMACTIVATE* pNmItem = (LPNMITEMACTIVATE) pNMHDR;
	CPoint pt = pNmItem->ptAction;
	int nIndex = m_wndObject.HitTest(pt);
	HTREEITEM hti = m_wndObject.GetItemFromIndex(nIndex);
	bool bShowMenu = true;
	CString strMenuText;
	if( hti && 
		m_wndObject.GetParentItem(hti) == m_wndObject.GetRootItem()  )
	{
		m_htiRootMenu = hti;
		//strMenuText = _T("New distance");
		strMenuText.LoadString( IDS_NEW_DISTANCE );
	}
	else if(hti == m_wndObject.GetRootItem())
	{
		m_htiRootMenu = 0;
		//strMenuText = _T("New object");
		strMenuText.LoadString( IDS_NEW_REFERENCE_POINT );
	}
	else
	{
		
		bShowMenu = false;
	}

	if(bShowMenu)
	{
	m_menuNew.DestroyMenu();
	m_menuNew.CreatePopupMenu();
	m_menuNew.AppendMenu( MF_BYPOSITION, ID_MENU_NEW, strMenuText  );
	CPoint point;
	GetCursorPos(&point);
	m_menuNew.TrackPopupMenu( 0, point.x, point.y, this );
	}
	
	*pResult = 0;
}

void CMapScaleView::OnMenuNew()
{
	CVVMapEditorDoc* pDoc = GetDocument();
	if( !m_htiRootMenu )
	{
		int nSize = pDoc->RefPoints().size();
		AddObject( nSize + 1 );
		pDoc->AddRefPoint();
	}
	else
	{
		const int	 nStart = 0;
		const double fStart = 0.0;
		AddPoint( m_htiRootMenu, nStart, 0.0 );
		std::set<HTREEITEM> Parents;
		FillParentMap(Parents);
		std::set<HTREEITEM>::const_iterator itrParent = Parents.find( m_htiRootMenu );
		ASSERT( itrParent != Parents.end() );
		int nRefPoint = std::distance< std::set<HTREEITEM>::const_iterator > ( Parents.begin(), itrParent );
		pDoc->AddRefPointDistance( nRefPoint, nStart, fStart );
	}
}

void	CMapScaleView::OnUpdateCmdUI()
{
	CVVMapEditorDoc* pDoc = GetDocument();
//	BOOL bEnable = pDoc->m_State == CVVMapEditorDoc::edit_ref_points;
	BOOL bEnable = pDoc->m_State == CVVMapEditorDoc::edit_anchor_points;

	GetDlgItem(IDC_LIST_OBJECT)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_X)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_Y)->EnableWindow(bEnable);
}

void CMapScaleView::OnEndlabeleditListObject(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	int nItem		= pDispInfo->item.iItem;
	int nSubItem	= pDispInfo->item.iSubItem;
	CVVMapEditorDoc* pDoc = GetDocument();
	
	HTREEITEM hti = m_wndObject.GetItemFromIndex( nItem );	
	HTREEITEM htiParent = m_wndObject.GetParentItem( hti );
//	int nRefPoint = m_wndObject.GetItemData(htiParent);
//	int nNumber = m_wndObject.GetItemData(hti);
	TreeMap_t tm;
	FillTreeMap(tm);
	TreeMap_t::const_iterator itr = tm.find( hti );
	ASSERT( itr != tm.end() );
	const std::pair<int , int>& pr = itr->second;
//	if( nSubItem == COL_POINT )
	{

		CString strText = m_wndObject.GetItemText( hti, COL_POINT );
		int nPoint = _ttoi( strText );
//		bool res = pDoc->ChangeRefPointPoint( pr.first, pr.second, nPoint );
//		m_wndObject.SetItemColor( htiParent, res? RGB(0,0,0): RGB(255 , 0, 0) );

		strText = m_wndObject.GetItemText( hti, COL_DISTANCE );
		double fDist = _tcstod( strText, 0 );
		bool res = pDoc->ChangeRefPoint(pr.first, pr.second, nPoint, fDist );
		res = pDoc->IsValidRefPoint(  pr.first);
		m_wndObject.SetItemColor( htiParent, res? RGB(0,0,0): RGB(255 , 0, 0) );
	}
	*pResult = 0;
}

//NOTENOTE: since COXTreeCtrl eat LVN_BEGINLABELEDIT we catch this user message, see CEditTreeCtrl
LRESULT CMapScaleView::OnFillEdit(WPARAM wParam, LPARAM lParam)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)lParam;
	CVVMapEditorDoc* pDoc = GetDocument();
	const AnchorPointMap_t& apm = pDoc->AnchorPoints();
	HTREEITEM hti = m_wndObject.GetItemFromIndex( pDispInfo->item.iItem );
	if(	pDispInfo->item.iSubItem	== COL_POINT && 
		m_wndObject.GetParentItem(hti)	!= m_wndObject.GetRootItem() )
	{
		CStringArray sa;
		for(int i=0; i < apm.size(); ++i)
		{
			CString str;
			str.Format(_T("%d"), i);
			sa.Add(str);
		}
		HTREEITEM hti= m_wndObject.GetItemFromIndex( pDispInfo->item.iItem );
		m_wndObject.SetEditMode( hti, OXET_COMBO, sa );
	}
	return TRUE;
}

void CMapScaleView::OnKeydownListObject(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here

	if (pLVKeyDow->wVKey != VK_DELETE) return;

	TreeMap_t tm;
	FillTreeMap(tm);

	HTREEITEM hti = m_wndObject.GetSelectedItem();

	CVVMapEditorDoc* pDoc = GetDocument();
	TreeMap_t::const_iterator itr = tm.find( hti );
	std::set<HTREEITEM> Parents;
	FillParentMap(Parents);
	std::set<HTREEITEM>::const_iterator itrParent = Parents.find( hti );
	if( itr != tm.end() )
	{
		const std::pair<int, int> &pr = itr->second;
		pDoc->RemoveRefPointDistance( pr.first, pr.second );
		bool res = pDoc->IsValidRefPoint(  pr.first);
		HTREEITEM htiParent = m_wndObject.GetParentItem( hti  );
		m_wndObject.SetItemColor( htiParent , res? RGB(0,0,0): RGB(255 , 0, 0) );
		
	}
	else if( itrParent != Parents.end() )
	{
		int dist = std::distance< std::set<HTREEITEM>::const_iterator >( Parents.begin(), itrParent );
		pDoc->RemoveRefPoint( dist );
	}
	m_wndObject.DeleteItem( hti );
	

	*pResult = 0;
}

void	CMapScaleView::FillTreeMap( TreeMap_t& tm )
{
	HTREEITEM htiParent = m_wndObject.GetRootItem();
	htiParent = m_wndObject.GetChildItem( htiParent );
	int nParentIndex = 0;
	while ( htiParent )
	{
//		int nParentIndex = m_wndObject.GetItemIndex( htiParent );
		if (m_wndObject.ItemHasChildren(htiParent))
		{
		   HTREEITEM hNextItem;
		   HTREEITEM hChildItem = m_wndObject.GetChildItem(htiParent);

		   int nChildIndex = 0;
		   while (hChildItem != NULL)
		   {
				tm.insert( std::make_pair( hChildItem, std::make_pair( nParentIndex, nChildIndex ) ) );
				hNextItem = m_wndObject.GetNextItem(hChildItem, TVGN_NEXT);
				hChildItem = hNextItem;
				++nChildIndex;
		   }
		}
		htiParent = m_wndObject.GetNextItem( htiParent, TVGN_NEXT );
		++nParentIndex;
	}
}

void	CMapScaleView::UpdateTree()
{
	m_wndObject.DeleteAllItems();
	const CVVMapEditorDoc* pDoc = GetDocument();
	for(int i =0; i < pDoc->RefPoints().size(); ++i)
	{
		const RefPoint_t&rp = pDoc->RefPoints()[i];
		HTREEITEM hti = AddObject( i + 1 );
		
		for( int j =0; j < rp.size(); ++j )
		{
			AddPoint(hti, rp[j].first, rp[j].second);
		}
	}
}

HTREEITEM	CMapScaleView::AddObject(int Number)
{
		CString strRefPoint;
		strRefPoint.Format( IDS_REFERENCE_POINT, Number );
	//	str.Format(_T("Object %d"), Number);
		HTREEITEM hti = m_wndObject.InsertItem( strRefPoint );
		m_wndObject.SetItemFont( hti, &m_fnObject );
		m_wndObject.SetNoEditMode( hti, 0 );
		CVVMapEditorDoc* pDoc = GetDocument();
		bool res= pDoc->IsValidRefPoint( Number-1 );
		if(!res) m_wndObject.SetItemColor( hti, RGB(255 , 0, 0) );
		return hti;
}

void	CMapScaleView::AddPoint(HTREEITEM htiParent, int nPoint, double fDistance)
{
		CString strValue;
		strValue.Format(_T("%d"), nPoint);
		HTREEITEM hti = m_wndObject.InsertItem( strValue, htiParent );
		strValue.Format(_T("%3.f"), fDistance);
		m_wndObject.SetSubItem(hti, 1,OX_SUBITEM_TEXT, strValue );
		m_wndObject.SetItemFont( hti, &m_fnPoint, COL_POINT );
		m_wndObject.SetItemFont( hti, &m_fnDistance, COL_DISTANCE );
}

void	CMapScaleView::FillParentMap( std::set<HTREEITEM> & Parents)
{
	Parents.clear();
	HTREEITEM htiParent = m_wndObject.GetRootItem();
	htiParent = m_wndObject.GetChildItem( htiParent );
	while ( htiParent )
	{
		Parents.insert( htiParent );
		htiParent = m_wndObject.GetNextItem(htiParent, TVGN_NEXT);
	}
}