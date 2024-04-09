


#include "stdafx.h"

#include "sys.h"



namespace sys
{
	

	long cur_ID = 5000;

	std::tstring get_res_string(unsigned str_ID)
	{
#ifndef UNIX_RELEASE
		const size_t s = MAX_PATH; _TCHAR b[s];
		if (::LoadString( hDllModule, str_ID, b, s))
			return std::tstring(b);
		else 
#endif
			return std::tstring();
	}
/*
	int get_new_ID()
	{
		return cur_ID++;
	}
	
	RECT get_wnd_rect(HWND hw)
	{
		RECT r;	
		r.left = 0; r.top = 0, r.right = 0; r.bottom = 0;
#ifndef UNIX_RELEASE
		::GetWindowRect( hw, &r);
#endif
		return r;
	}

	int get_width(HWND hw)
	{
		RECT r = get_wnd_rect(hw);
		return (r.right - r.left);
	}

	int get_height(HWND hw)
	{
		RECT r = get_wnd_rect(hw);
		return (r.bottom - r.top);
	}

	void set_width(HWND hw, int w)
	{
		int h = get_height(hw);
		wnd_size( hw, w, h);
	}

	void set_height(HWND hw, int h)
	{
		int w = get_width(hw);
		wnd_size( hw, w, h);
	}

	SIZE text_size(HWND hw, const _TCHAR * text)
	{
		SIZE ss;	ss.cx = 0; ss.cy = 0;

#ifndef UNIX_RELEASE
		HDC dc = GetDC(hw);
			
		GetTextExtentPoint32(dc, (LPCTSTR) text, (int) _tcslen(text), &ss);
		
		ReleaseDC(hw, dc);
#endif
		return ss;
	}

	void set_text(HWND hw, const _TCHAR * text)
	{
#ifndef UNIX_RELEASE
		::SetWindowText(hw, text);
#endif
	}

	int text_h(HWND hw, const _TCHAR * text)
	{
		return text_size(hw, text).cy;
	}

	int text_w(HWND hw, const _TCHAR * text)
	{
		return text_size(hw, text).cx;
	}

	void wnd_pos(HWND hw, int x, int y)
	{
#ifndef UNIX_RELEASE
		::SetWindowPos(hw, 0, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
#endif
	}
	void wnd_size(HWND hw, int w, int h)
	{
#ifndef UNIX_RELEASE
		::SetWindowPos(hw, 0, 0, 0, w, h, SWP_NOZORDER | SWP_NOMOVE);
#endif
	}
	/////////////////////////////////////////////////////////////////////////
	
	KWnd::KWnd( const _TCHAR * class_name, HWND parent, long flags, long _ID) : phwnd(parent), ID(_ID)
	{
#ifndef UNIX_RELEASE
		const _TCHAR title[] = _T("title");
		hwnd = ::CreateWindowEx(0, class_name, title,  flags,
						0, 0, 1, 1,
						parent, 0, hDllModule, 0);
		if (_ID > 0)
			::SetWindowLong(hwnd, GWL_ID, ID);
#endif
	}
	//----------------------------------------------------------------------
	void KWnd::vis(bool v) const
	{
#ifndef UNIX_RELEASE
		::ShowWindow(hwnd, v);
#endif
	}
	//----------------------------------------------------------------------
	void KWnd::enable(bool e) const 
	{
#ifndef UNIX_RELEASE
		::EnableWindow(hwnd, e);
#endif
	}
	//----------------------------------------------------------------------
	void KWnd::pos(int x, int y) const 
	{
		wnd_pos( hwnd, x, y);
	}
	//----------------------------------------------------------------------
	void KWnd::size(int w, int h) const
	{
		wnd_size( hwnd, w, h);
	}
	//----------------------------------------------------------------------
	void KWnd::text(const _TCHAR * t) const
	{
		set_text(hwnd, t);
	}
*/
}

