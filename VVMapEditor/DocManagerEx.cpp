// DocManagerEx.cpp: implementation of the CDocManagerEx class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VVMapEditor.h"
#include "DocManagerEx.h"
#include "FileDialogShortEx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDocManagerEx::CDocManagerEx()
{

}

CDocManagerEx::~CDocManagerEx()
{

}

AFX_STATIC void AFXAPI _AfxAppendFilterSuffix(CString& filter, OPENFILENAME& ofn,
	CDocTemplate* pTemplate, CString* pstrDefaultExt)
{
	ASSERT_VALID(pTemplate);
	ASSERT_KINDOF(CDocTemplate, pTemplate);

	CString strFilterExt, strFilterName;
	if (pTemplate->GetDocString(strFilterExt, CDocTemplate::filterExt) &&
	 !strFilterExt.IsEmpty() &&
	 pTemplate->GetDocString(strFilterName, CDocTemplate::filterName) &&
	 !strFilterName.IsEmpty())
	{
		// a file based document template - add to filter list
		ASSERT(strFilterExt[0] == '.');
		if (pstrDefaultExt != NULL)
		{
			// set the default extension
			*pstrDefaultExt = ((LPCTSTR)strFilterExt) + 1;  // skip the '.'
			ofn.lpstrDefExt = (LPTSTR)(LPCTSTR)(*pstrDefaultExt);
			ofn.nFilterIndex = ofn.nMaxCustFilter + 1;  // 1 based number
		}

		// add to filter
		filter += strFilterName;
		ASSERT(!filter.IsEmpty());  // must have a file type name
		filter += (TCHAR)'\0';  // next string please
		filter += (TCHAR)'*';
		filter += strFilterExt;
		filter += (TCHAR)'\0';  // next string please
		ofn.nMaxCustFilter++;
	}
}

BOOL CDocManagerEx::DoPromptFileName(CString& fileName, UINT nIDSTitle,
			DWORD lFlags, BOOL bOpenFileDialog, CDocTemplate* pTemplate)
{
/*	if( bOpenFileDialog )
		return CDocManager::DoPromptFileName( fileName, nIDSTitle, lFlags,  bOpenFileDialog,  pTemplate );
	CFileDialogShortEx dlgFile(bOpenFileDialog);*/

   CFileDialog* pDlg = OnCreateFileDialog(bOpenFileDialog);
   ASSERT(pDlg);
   CFileDialog& dlgFile = *pDlg;

	CString title;
	VERIFY(title.LoadString(nIDSTitle));

	dlgFile.m_ofn.Flags |= lFlags | OFN_EXPLORER | ( bOpenFileDialog ? 0 : OFN_ENABLETEMPLATE );

	CString strFilter;
	CString strDefault;
	if (pTemplate != NULL)
	{
		ASSERT_VALID(pTemplate);
		_AfxAppendFilterSuffix(strFilter, dlgFile.m_ofn, pTemplate, &strDefault);
	}
	else
	{
		// do for all doc template
		POSITION pos = m_templateList.GetHeadPosition();
		BOOL bFirst = TRUE;
		while (pos != NULL)
		{
			CDocTemplate* pTemplate = (CDocTemplate*)m_templateList.GetNext(pos);
			_AfxAppendFilterSuffix(strFilter, dlgFile.m_ofn, pTemplate,
				bFirst ? &strDefault : NULL);
			bFirst = FALSE;
		}
	}

	// append the "*.*" all files filter
	CString allFilter;
	VERIFY(allFilter.LoadString(AFX_IDS_ALLFILTER));
	strFilter += allFilter;
	strFilter += (TCHAR)'\0';   // next string please
	strFilter += _T("*.*");
	strFilter += (TCHAR)'\0';   // last string
	dlgFile.m_ofn.nMaxCustFilter++;

	dlgFile.m_ofn.lpstrFilter = strFilter;
	dlgFile.m_ofn.lpstrTitle = title;
	dlgFile.m_ofn.lpstrFile = fileName.GetBuffer(_MAX_PATH);
	if(!bOpenFileDialog)
	dlgFile.m_ofn.lpTemplateName = MAKEINTRESOURCE (IDD_SHORT);

	int nResult = dlgFile.DoModal();
	fileName.ReleaseBuffer();

	if( !bOpenFileDialog )
	{
		CFileDialogShortEx* pDlgShort = dynamic_cast<CFileDialogShortEx*>(pDlg);
		static_cast<CVVMapEditorApp*>(AfxGetApp())->m_bShort = pDlgShort->m_bShort;
	}

	delete pDlg;
	
	return nResult == IDOK;
	
}

CFileDialog* CDocManagerEx::OnCreateFileDialog(BOOL bOpenFileDialog)
{
   TRACE(_T("CDocManagerEx::OnCreateFileDialog\n"));
   CFileDialog* pDlg;
   if(bOpenFileDialog)
   {
	   pDlg = new CFileDialogEx(bOpenFileDialog);
   }
   else
   {
		pDlg = new CFileDialogShortEx(bOpenFileDialog);
   }
   return pDlg;
}