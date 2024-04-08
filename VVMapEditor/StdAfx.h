// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__6137657D_3BEA_4762_B263_34D2E51B5344__INCLUDED_)
#define AFX_STDAFX_H__6137657D_3BEA_4762_B263_34D2E51B5344__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#pragma warning ( disable : 4786)
//

#ifdef _DEBUG
#define GDIPLUS_NO_AUTO_INIT
#include "GdiplusH.h"
#else
//#define GDIPLUS_USE_GDIPLUS_MEM
#include <gdiplus.h>
#endif

using namespace Gdiplus;

#define TODOSTRINGIZE(L) #L
#define TODOMAKESTRING(M,L) M(L)
#define TODOLINE TODOMAKESTRING( TODOSTRINGIZE, __LINE__) 
#define TODO(_msg) message(__FILE__ "(" TODOLINE ") : TODO : " _msg)
#define BUG(_msg) message(__FILE__ "(" TODOLINE ") : BUG : " _msg)

#include "res/resource.h"

#ifdef ORWELL_DB
#include "../../DBBridge/trunk/DBBridge.h"
#endif
#include "../../SRS/VVMapInterface/trunk/VVMInterface.h"
#ifdef ORWELL_CAM
#include "../../Mapimg/trunk/mapimg.h"
#endif

//#define SWITCH_OFF_ALL_FEATURES
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__6137657D_3BEA_4762_B263_34D2E51B5344__INCLUDED_)
