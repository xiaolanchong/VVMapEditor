// ScaleComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "ScaleComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScaleComboBox

CScaleComboBox::CScaleComboBox()
{
}

CScaleComboBox::~CScaleComboBox()
{
}


BEGIN_MESSAGE_MAP(CScaleComboBox, CGuiComboBoxExt)
	//{{AFX_MSG_MAP(CScaleComboBox)
	ON_WM_CREATE()
	ON_CONTROL_REFLECT(CBN_EDITCHANGE, OnEditchange)
	ON_CONTROL_REFLECT(CBN_EDITUPDATE, OnEditupdate)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScaleComboBox message handlers

int CScaleComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here

	m_Scales.push_back( 3.0f );
	m_Scales.push_back( 2.0f );
	m_Scales.push_back( 1.5f );
	m_Scales.push_back( 1.0f );
	m_Scales.push_back( 0.75f );
	m_Scales.push_back( 0.50f );
	m_Scales.push_back( 0.25f );
//	m_Scales.push_back( 0.75f );
	
	AddString(_T("300%"));
	AddString(_T("200%"));
	AddString(_T("150%"));
	AddString(_T("100%"));
	AddString(_T("75%"));
	AddString(_T("50%"));
	AddString(_T("25%"));
//	AddString("10%");
	SetCurSel(3);

	LOGFONT lf;
	GetParent()->GetFont()->GetLogFont(&lf);

	m_Font.CreateFontIndirect(&lf);
	SetFont( &m_Font );
	return 0;
}

void CScaleComboBox::OnEditchange() 
{
	// TODO: Add your control notification handler code here
	
}

void CScaleComboBox::OnEditupdate() 
{
	// TODO: Add your control notification handler code here
	
}

void CScaleComboBox::OnSelchange() 
{
	// TODO: Add your control notification handler code here
	int nIndex = GetCurSel();
	float size = m_Scales[nIndex];
	AfxGetMainWnd()->SendMessage(WM_SCALE, *(DWORD*)&size);
/*	switch( nIndex )

	{
	case 0:	size = 1.5f ;
		{	
			AfxGetMainWnd()->SendMessage(WM_SCALE, *(DWORD*)&size);
			break;
		}
	case 1:	size = 1.0f ;
		{	
			AfxGetMainWnd()->SendMessage(WM_SCALE, *(DWORD*)&size);
			break;
		}
	case 2: size = 0.75f;
		{	
			AfxGetMainWnd()->SendMessage(WM_SCALE, *(DWORD*)&size);
			break;
		}
	case 3: size = 0.5f;
		{	
			AfxGetMainWnd()->SendMessage(WM_SCALE, *(DWORD*)&size);
			break;
		}
	case 4: size = 0.25f;
		{	
			AfxGetMainWnd()->SendMessage(WM_SCALE, *(DWORD*)&size);
			break;
		}
	case 5: size = 0.1f;
		{	
			AfxGetMainWnd()->SendMessage(WM_SCALE, *(DWORD*)&size);
			break;
		}
	case 6: 
		{	
			AfxGetMainWnd()->SendMessage(WM_FIT);
			SetCurSel(-1);
			break;
		}
	}*/
}
