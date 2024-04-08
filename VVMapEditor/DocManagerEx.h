// DocManagerEx.h: interface for the CDocManagerEx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DOCMANAGEREX_H__8152A010_231B_4169_9996_B888B55C70FF__INCLUDED_)
#define AFX_DOCMANAGEREX_H__8152A010_231B_4169_9996_B888B55C70FF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDocManagerEx : public CDocManager  
{
public:
	CDocManagerEx();
	virtual ~CDocManagerEx();

	virtual BOOL DoPromptFileName(CString& fileName, UINT nIDSTitle,
			DWORD lFlags, BOOL bOpenFileDialog, CDocTemplate* pTemplate);
protected:
	virtual CFileDialog* OnCreateFileDialog(BOOL bOpenFileDialog);
};

#endif // !defined(AFX_DOCMANAGEREX_H__8152A010_231B_4169_9996_B888B55C70FF__INCLUDED_)
