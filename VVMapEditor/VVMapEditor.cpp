// VVMapEditor.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "VVMapEditor.h"

#include "MainFrm.h"
#include "VVMapEditorDoc.h"
#include "VVMapEditorView.h"

#include "DocManagerEx.h"
#include <atlbase.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVVMapEditorApp

BEGIN_MESSAGE_MAP(CVVMapEditorApp, CWinApp)
	//{{AFX_MSG_MAP(CVVMapEditorApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVVMapEditorApp construction

CVVMapEditorApp::CVVMapEditorApp():m_bShort(false)
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CVVMapEditorApp object

CVVMapEditorApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CVVMapEditorApp initialization

BOOL CVVMapEditorApp::InitInstance()
{
	const DWORD c_DefLCID = MAKELANGID( LANG_ENGLISH, SUBLANG_DEFAULT );
	DWORD dwLangID = c_DefLCID;
	CRegKey	reg;
	LONG res;
	res = reg.Open( HKEY_LOCAL_MACHINE, _T("SOFTWARE\\ELVEES\\Common"), KEY_READ ) ; 
	if( res == ERROR_SUCCESS )
	{
		res = reg.QueryDWORDValue( _T("LangLCID"), dwLangID );
		if( res != ERROR_SUCCESS )
		{
			dwLangID = c_DefLCID;
		}
	}
	SetThreadLocale( dwLangID );

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("ElVEES"));

	GetSettings().Load();
	int MRUNumber = GetSettings().GetProperty(  prop_MRU_Number );
	LoadStdProfileSettings(MRUNumber);  // Load standard INI file options (including MRU)


	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.
	m_pDocManager = new CDocManagerEx;

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CVVMapEditorDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CVVMapEditorView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	if (!ProcessShellCommand(cmdInfo)) 
		return FALSE;
	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

// CModuleVersion form MSJ April 1998

class CModuleVersion : public VS_FIXEDFILEINFO {
protected:
   BYTE* m_pVersionInfo;   // all version info

   struct TRANSLATION {
      WORD langID;         // language ID
      WORD charset;        // character set (code page)
   } m_translation;

public:
   CModuleVersion();
   virtual ~CModuleVersion();

   BOOL     GetFileVersionInfo(LPCTSTR modulename);
   CString  GetValue(LPCTSTR lpKeyName);
};

CModuleVersion::CModuleVersion()
{
   m_pVersionInfo = NULL;           // raw version info data 
}

//////////////////
// Destroy: delete version info
//
CModuleVersion::~CModuleVersion()
{
   delete [] m_pVersionInfo;
}

//////////////////
// Get file version info for a given module
// Allocates storage for all info, fills "this" with
// VS_FIXEDFILEINFO, and sets codepage.
//
BOOL CModuleVersion::GetFileVersionInfo(LPCTSTR modulename)
{
   m_translation.charset = 1252;    // default = ANSI code page
   memset((VS_FIXEDFILEINFO*)this, 0, sizeof(VS_FIXEDFILEINFO));

   // get module handle
   TCHAR filename[_MAX_PATH];
   HMODULE hModule = ::GetModuleHandle(modulename);
   if (hModule==NULL && modulename!=NULL)
      return FALSE;

   // get module file name
   DWORD len = GetModuleFileName(hModule, filename,
      sizeof(filename)/sizeof(filename[0]));
   if (len <= 0)
      return FALSE;

   // read file version info
   DWORD dwDummyHandle; // will always be set to zero
   len = GetFileVersionInfoSize(filename, &dwDummyHandle);
   if (len <= 0)
      return FALSE;

   if(m_pVersionInfo) delete[] m_pVersionInfo;
   m_pVersionInfo = new BYTE[len]; // allocate version info
   if (!::GetFileVersionInfo(filename, 0, len, m_pVersionInfo))
      return FALSE;

   LPVOID lpvi;
   UINT iLen;
   if (!VerQueryValue(m_pVersionInfo, _T("\\"), &lpvi, &iLen))
      return FALSE;

   // copy fixed info to myself, which am derived from VS_FIXEDFILEINFO
   *(VS_FIXEDFILEINFO*)this = *(VS_FIXEDFILEINFO*)lpvi;

   // Get translation info
   if (VerQueryValue(m_pVersionInfo,
      _T("\\VarFileInfo\\Translation"), &lpvi, &iLen) && iLen >= 4) {
      m_translation = *(TRANSLATION*)lpvi;
      TRACE( _T("code page = %d\n"), m_translation.charset);
   }

   return dwSignature == VS_FFI_SIGNATURE;
}

//////////////////
// Get string file info.
// Key name is something like "CompanyName".
// returns the value as a CString.
//
CString CModuleVersion::GetValue(LPCTSTR lpKeyName)
{
   CString sVal;
   if (m_pVersionInfo) {

      // To get a string value must pass query in the form
      //
      //    "\StringFileInfo\<langID><codepage>\keyname"
      //
      // where <langID><codepage> is the languageID concatenated with the
      // code page, in hex. Wow.
      //
      CString query;
      query.Format(_T("\\StringFileInfo\\%04x%04x\\%s"),
                   m_translation.langID,
                   m_translation.charset,
                   lpKeyName);

      LPCTSTR pVal;
      UINT iLenVal;
      if (VerQueryValue(m_pVersionInfo, (LPTSTR)(LPCTSTR)query,
          (LPVOID*)&pVal, &iLenVal)) {

         sVal = pVal;
      }
   }
   return sVal;
}


class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CVVMapEditorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CVVMapEditorApp message handlers

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition();
	CDocTemplate* pTmpl = AfxGetApp()->GetNextDocTemplate(pos);
	pos = pTmpl->GetFirstDocPosition();
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)pTmpl->GetNextDoc(pos);

	if(pDoc->m_VVMapTrans)
	{
		CString str(_T("VVMapInterface\t\t"));
		str += pDoc->m_VVMapTrans->GetInterfaceVersion();
		SetDlgItemText(IDC_VER_INT, str);
	}

	TCHAR buf[MAX_PATH];
	GetModuleFileName( AfxGetInstanceHandle(), buf, MAX_PATH ) ;

	CModuleVersion Ver;

	BOOL z = Ver.GetFileVersionInfo(buf);
	CString sz = Ver.GetValue(  _T("FileVersion"));
	
	CString str(_T("VVMapEditor\t\t"));
	str += sz;
	str.Replace(_T(','), _T('.'));
	str.Remove(_T(' '));
	SetDlgItemText(IDC_VER_EDITOR, str);
/*
	str = _T("SRSDll\t\t\t");
	z = Ver.GetFileVersionInfo( _T("srsdll.dll") );
	sz = Ver.GetValue(  _T("FileVersion"));

	str += sz;
	str.Replace(_T(','), _T('.'));
	str.Remove(_T(' '));
	SetDlgItemText(IDC_VER_SRS, str);*/

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int CVVMapEditorApp::Run() 
{
	// TODO: Add your specialized code here and/or call the base class
	try
	{
		return	 CWinApp::Run();	
	}
	catch(std::exception & ex)
	{
		AfxMessageBox( CString("Internal error::") + ex.what() );
		throw;
	}
	return -1;
}

int CVVMapEditorApp::ExitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class
	GetSettings().Save();
	return CWinApp::ExitInstance();
}
/*
void	CVVMapEditorApp::SaveMRU()
{
	
}*/

void	CVVMapEditorApp::ReloadMRU(int nMRUNumber)
{
	SaveStdProfileSettings();
	if (m_pRecentFileList != NULL) 
	{
		delete m_pRecentFileList;
		m_pRecentFileList = NULL;
	}
	LoadStdProfileSettings(nMRUNumber);
}

void CAboutDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnMouseMove(nFlags, point);
}
