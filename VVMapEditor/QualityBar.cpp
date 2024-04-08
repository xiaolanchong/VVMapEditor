// QualityBar.cpp : implementation file
//

#include "stdafx.h"
#include "VVMapEditor.h"
#include "QualityBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQualityBar

CQualityBar::CQualityBar()
{
}

CQualityBar::~CQualityBar()
{
}


BEGIN_MESSAGE_MAP(CQualityBar, CGuiControlBar)
	//{{AFX_MSG_MAP(CQualityBar)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQualityBar message handlers

BOOL	CQualityBar::Create( CWnd* pParentWnd )
{	
	CString strColumn; 
	strColumn.LoadString( IDS_COLUMN_QUALITY );
	if (!CGuiControlBar::Create
			( strColumn,WS_CLIPCHILDREN|WS_CHILD | WS_VISIBLE | 
							CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC | WS_SYSMENU, 
							pParentWnd, 0x200 ))
		return FALSE;

	if( !m_wndQuality.Create(
		WS_CHILD|WS_VISIBLE|WS_BORDER|LVS_REPORT,
		CRect(0,0,0,0), this, 1) )
		return FALSE;

	strColumn.LoadString( IDS_COLUMN_FROM );
	m_wndQuality.InsertColumn( 0, strColumn, LVCFMT_LEFT, 50  );
	strColumn; strColumn.LoadString( IDS_COLUMN_TO );
	m_wndQuality.InsertColumn( 1, strColumn, LVCFMT_LEFT, 50  );
	strColumn; strColumn.LoadString( IDS_COLUMN_QUALITY );
	m_wndQuality.InsertColumn( 2, strColumn, LVCFMT_LEFT, 75 );
	m_wndQuality.SetExtendedStyle( LVS_EX_FULLROWSELECT );

	return TRUE;
}

void	CQualityBar::Refresh(const std::vector<MergeQuality>& ArrQuality)
{
	m_wndQuality.DeleteAllItems();

	for( int i=0; i<ArrQuality.size(); ++i )
	{
		CString strFrom, strTo, strQuality;
		strFrom.Format( _T("%d"), ArrQuality[i].m_nFrom );
		ArrQuality[i].m_nTo == -1? strTo = _T("Map") : strTo.Format( _T("%d"), ArrQuality[i].m_nTo );
		strQuality.Format( _T("%d"), ArrQuality[i].m_nQuality );

		int nIndex = m_wndQuality.GetItemCount();
		m_wndQuality.InsertItem(nIndex, strFrom, 0);
		m_wndQuality.SetItem(nIndex, 1,LVIF_TEXT, strTo,NULL,NULL,NULL,NULL);
		m_wndQuality.SetItem(nIndex, 2,LVIF_TEXT, strQuality,NULL,NULL,NULL,NULL);		
	}
}