// ChooseCompressor.h: interface for the CChooseCompressor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHOOSECOMPRESSOR_H__BDB8C1C3_F01A_4527_ACA8_EEB85B461A65__INCLUDED_)
#define AFX_CHOOSECOMPRESSOR_H__BDB8C1C3_F01A_4527_ACA8_EEB85B461A65__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vfw.h>

struct Compressor/* : COMPVARS*/
{
	DWORD		m_dwFourCC;

	Compressor(): m_dwFourCC(0){}

	void ChooseCompressor(HWND hwndParent) 
	{
		COMPVARS cv;
		memset( &cv, 0, sizeof( COMPVARS)  );
		if (!(cv.dwFlags & ICMF_COMPVARS_VALID)) 
		{
			memset(&cv, 0, sizeof( COMPVARS) );
			cv.dwFlags |= ICMF_COMPVARS_VALID;
			cv.lQ = 10000;
		}

		cv.cbSize = sizeof(COMPVARS);
		ChooseCompressor(hwndParent, &cv, 0); 

		m_dwFourCC = cv.fccHandler;

		FreeCompressor(&cv);
	};

private:
	void ChooseCompressor(HWND hwndParent, COMPVARS *lpCompVars, BITMAPINFOHEADER *bihInput);
	void FreeCompressor(COMPVARS *pCompVars);
};


#endif // !defined(AFX_CHOOSECOMPRESSOR_H__BDB8C1C3_F01A_4527_ACA8_EEB85B461A65__INCLUDED_)
