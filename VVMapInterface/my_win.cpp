
//#include "res/resource.h"
#include "stdafx.h"
#include "my_win.h"


#include "memleak.h"

extern HINSTANCE hInstance;

namespace my_win
{
/*
#ifdef _WIN32_WCE
	#error WINCE!!
#endif
*/
#define MY_WM_SHOW_TEXT		WM_USER + 333
	
	void CWnd::OnChange()
	{
#ifndef UNIX_RELEASE
		_allow_showing_warn = true;
		_valid_value		= true;
		std::list <CEvent*>::iterator it = on_change.begin();
		for (; it != on_change.end(); ++it)
			if (*it) (*it)->OnEvent();
#endif
	}
	
	void CMSWnd::set_user_data()
	{
#ifndef UNIX_RELEASE
#ifndef _WIN32_WCE
		::SetWindowLongPtr( handle(), GWLP_USERDATA, (LONG_PTR) this);
#endif
#endif
	}

	void CMSWnd::init(CMSWnd * owner, const _TCHAR * class_name, long style, const _TCHAR * text)
	{
#ifndef UNIX_RELEASE
		const int x = 0;
		const int y = 0;
		const int w = 20;
		const int h = 10;

		par_hwnd = owner->handle();
		hwnd = ::CreateWindowEx( 0, class_name, text, style, x, y, w, h, 
			par_hwnd, 0, hInstance, 0);
		set_user_data();
#endif
	}
	void CMSWnd::init(CMSWnd * owner, long _ID)
	{
#ifndef UNIX_RELEASE
		par_hwnd = owner->handle();
		hwnd = ::GetDlgItem( owner->handle(), _ID);
		set_user_data();
#endif
	}
/*	void CMSWnd::init(HWND _hwnd)
	{
		hwnd = _hwnd;
	}
*/

	CMSWnd::CMSWnd(CMSWnd * owner, const _TCHAR * class_name, long style, const _TCHAR * text)
	{
		init(owner, class_name, style, text);
	}

	CMSWnd::CMSWnd(CMSWnd * owner, long _ID)
	{
		init( owner, _ID);
	}

	CMSWnd::CMSWnd(HWND _hwnd) :hwnd(_hwnd)
	{
		//set_user_data();
	}

	CMSWnd::CMSWnd() :hwnd(0)
	{
	}

	CMSWnd::~CMSWnd()
	{
#ifndef UNIX_RELEASE
#ifndef _WIN32_WCE
		::SetWindowLongPtr( handle(), GWLP_USERDATA, 0);
#endif
#endif
	}

	RECT CMSWnd::GetRect() const
	{
		RECT r = {0,0,0,0};
#ifndef UNIX_RELEASE
		::GetWindowRect(handle(), &r);
#endif
		return r;
	}

	void CMSWnd::SetSize( int w, int h)
	{
#ifndef UNIX_RELEASE
		::SetWindowPos( handle(), 0, 0, 0, w, h, SWP_NOZORDER | SWP_NOMOVE);
#endif
	}
	
	void CMSWnd::SetPos( int x, int y)
	{
#ifndef UNIX_RELEASE
		::SetWindowPos( handle(), 0, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
#endif
	}

	void CMSWnd::SetW( int width)
	{
#ifndef UNIX_RELEASE
		SetSize( width, GetH());
#endif
	}

	void CMSWnd::SetH( int height)
	{
#ifndef UNIX_RELEASE
		SetSize( GetW(), height);
#endif
	}

	void CMSWnd::SetText(const _TCHAR * text)
	{
#ifndef UNIX_RELEASE
		::SetWindowText( handle(), text);
#endif
	}

	void CMSWnd::Visible(bool vis)
	{
#ifndef UNIX_RELEASE
		::ShowWindow( handle(), (vis) ? (SW_SHOW) : (SW_HIDE) );
#endif
	}
	
	void CMSWnd::Enabled(bool en)
	{
#ifndef UNIX_RELEASE
		::EnableWindow( handle(), en);
#endif
	}

	int CMSWnd::GetW() const
	{
		RECT r = GetRect();
		return r.right - r.left;
	}
	
	int CMSWnd::GetH() const
	{
		RECT r = GetRect();
		return r.bottom - r.top;
	}

	int CMSWnd::GetX() const
	{
		RECT r = GetRect();
		return r.left;
	}

	int CMSWnd::GetY() const
	{
		RECT r = GetRect();
		return r.top;
	}
	
	std::tstring CMSWnd::GetText() const
	{
		const size_t size = 128;
		_TCHAR buff[size];
#ifndef UNIX_RELEASE
		::GetWindowText( handle(), buff, size - 1);
#else
		buff[0] = _T('\0');
#endif
		return std::tstring(buff);
	}

	bool CMSWnd::IsVisible() const
	{
#ifndef UNIX_RELEASE
		return (::IsWindowVisible(handle()) != 0);
#else
		return false;
#endif
	}

	bool CMSWnd::IsEnabled() const
	{
#ifndef UNIX_RELEASE
		return (::IsWindowEnabled(handle()) != 0);
#else
		return false;
#endif
	}

	void CMSWnd::_MsgBox( const _TCHAR * caption, const _TCHAR * text) const
	{
#ifndef UNIX_RELEASE
		//::MessageBox( par_hwnd, text, caption, MB_OK);
		std::tstring * ts = new std::tstring(text);
		std::tstring * cs = new std::tstring(caption);

		LONG_PTR tl = reinterpret_cast <LONG_PTR> (ts);
		LONG_PTR cl = reinterpret_cast <LONG_PTR> (cs);
		
		BOOL res = ::PostMessage ( par_hwnd, MY_WM_SHOW_TEXT, tl, cl);

		if (!res) 
		{
			if (ts) delete ts;
			if (cs) delete cs;
		}
#endif
	}
///////////////////////////////////////////////////////////////////////////	
	CStatic::CStatic(CMSWnd * owner, const _TCHAR * txt) 
#ifndef UNIX_RELEASE
			:CMSWnd (owner, _T("STATIC"), (WS_VISIBLE | WS_CHILD), txt)
#endif
	{
	}

	CStatic::CStatic(CMSWnd * owner, long _ID)  :CMSWnd (owner, _ID)
	{
	}

	int CStatic::OnCmd(int /*code*/, unsigned /*ID*/)
	{
		return 0;
	}
	
///////////////////////////////////////////////////////////////////////////
	int CEdit::OnCmd(int code, unsigned /*ID*/)
	{
#ifndef UNIX_RELEASE
		if (code == EN_KILLFOCUS)// EN_CHANGE
			if (is_changed()) OnChange();
#endif
		return 0;
	}

	bool CEdit::is_changed() const
	{
#ifndef UNIX_RELEASE
		LONG_PTR v = ::SendMessage( handle(), EM_GETMODIFY, 0, 0);
		return (v != 0);
#else
		return false;
#endif
	}
	
	CEdit::CEdit(CMSWnd * owner, const _TCHAR * txt) 
#ifndef UNIX_RELEASE
			:CMSWnd (owner, _T("EDIT"), 
			(WS_CHILD | WS_VISIBLE | ES_RIGHT),
			txt)
#endif
	{
	}
	
	CEdit::CEdit(CMSWnd * owner, long _ID)  :CMSWnd (owner, _ID)
	{
	}
	
///////////////////////////////////////////////////////////////////////////
	int CCheckBox::OnCmd(int code, unsigned /*ID*/)
	{
#ifndef UNIX_RELEASE
		if (code == BN_CLICKED)		
							OnChange();
#endif
		return 0;
	}

	CCheckBox::CCheckBox( CMSWnd * owner, const _TCHAR * txt, bool _checked) 
#ifndef UNIX_RELEASE
			:CMSWnd (owner, _T("BUTTON"),	
			(WS_VISIBLE | WS_CHILD | BS_CHECKBOX | BS_TEXT), txt)
#endif
	{ 
		Checked( _checked);
	}

	CCheckBox::CCheckBox(CMSWnd * owner, long _ID, bool _checked)  :CMSWnd (owner, _ID)
	{ 
		Checked( _checked);
	}

	void CCheckBox::Checked( bool _checked)
	{
#ifndef UNIX_RELEASE
		int iv = (_checked) ? (BST_CHECKED) : (BST_UNCHECKED);
		::SendMessage(handle(), BM_SETCHECK, iv, 0);
#endif
	}

	bool CCheckBox::IsChecked() const
	{
#ifndef UNIX_RELEASE
		return ( ::SendMessage( handle(), BM_GETCHECK, 0, 0) == BST_CHECKED );
#else
		return false;
#endif
	}
	///////////////////////////////////////////////////////////////////////////

	WndList::~WndList ()
	{
#ifndef UNIX_RELEASE
		std::list <CWnd*>::iterator it = items.begin();
		for (; it != items.end(); ++it)
			if (*it) delete (*it);
#endif
	}

	int CListBox::OnCmd( int code, unsigned /*ID*/)
	{
#ifndef UNIX_RELEASE
		if (code == LBN_SELCHANGE)
		{
			/*if (IsMultiple())
			{
				Select(GetSelection());
			}*/
			OnChange();
		}
#endif
		return 0;
	}
	
	CListBox::CListBox( CMSWnd * owner, const _TCHAR * txt, bool multi) 
#ifndef UNIX_RELEASE
		:CMSWnd (owner, _T("LISTBOX"),	
		(
			  WS_CHILD | WS_VISIBLE | LBS_STANDARD | LBS_NOTIFY | 
			  ((multi) ? (LBS_EXTENDEDSEL) : (0)) 
		),
		txt)
#endif
	{
	}

	CListBox::CListBox(CMSWnd * owner, long _ID)  :CMSWnd (owner, _ID)
	{
	}

	int CListBox::ItemsCount() const
	{
#ifndef UNIX_RELEASE
		INT_PTR res = ::SendMessage ( handle(), LB_GETCOUNT, 0, 0);
		if (res != LB_ERR) return (int) res;
		else return 0;
#else
		return 0;
#endif
	}

	 void CListBox::AddString(const _TCHAR * text)
	 {
#ifndef UNIX_RELEASE
		 LPARAM lparam = (LPARAM) text;
		 ::SendMessage( handle(), LB_ADDSTRING, 0, lparam);
#endif
	 }

	 void CListBox::DelString(int index)
	 {
#ifndef UNIX_RELEASE
		 ::SendMessage( handle(), LB_DELETESTRING, index, 0);
#endif
	 }
     
	 void CListBox::InsString(int index, const _TCHAR * text)
	 {
#ifndef UNIX_RELEASE
		LPARAM lparam = (LPARAM) text;
		::SendMessage( handle(), LB_INSERTSTRING, index, lparam);
#endif
	 }
		
	 std::tstring CListBox::GetString(int index) const
	 {
#ifndef UNIX_RELEASE
		INT_PTR size = ::SendMessage( handle(), LB_GETTEXTLEN, index, 0);
		if (size != LB_ERR)
		{
			std::vector <_TCHAR> buff(size+1);
			
			::SendMessage( handle(), LB_GETTEXT, index, (LPARAM) &buff[0]);
			return std::tstring(&buff[0]);
		}
		else return std::tstring();
#else
		return std::tstring();
#endif
	 }

	 void CListBox::Clear()
	 {
#ifndef UNIX_RELEASE
		 ::SendMessage( handle(), LB_RESETCONTENT, 0, 0);
#endif
	 }

	 void CListBox::Sel1(int index)
	 {
#ifndef UNIX_RELEASE
		 ::SendMessage( handle(), LB_SETCURSEL, index, 0);
#endif
	 }

	void CListBox::Select(const CSelection& selection)
	{
#ifndef UNIX_RELEASE
		if (IsMultiple())
		{
			::SendMessage( handle(), LB_SELITEMRANGE, TRUE, MAKELPARAM(selection.begin, selection.end));
		}
		else Sel1(selection.begin);
#endif
	}

	CSelection CListBox::GetSelection() const
	{
#ifndef UNIX_RELEASE
		if (IsMultiple())
		{
			INT_PTR c = ::SendMessage( handle(), LB_GETSELCOUNT, 0, 0);
			if (c != LB_ERR)
			{
				std::vector<int> sel_buff(c);
				LPARAM lparam = (LPARAM) (GetPtr(sel_buff));
				if (::SendMessage( handle(), LB_GETSELITEMS, c, lparam) != LB_ERR)
				{
					std::vector<int>::const_iterator it = sel_buff.begin();
					CSelection sel(100000, -1);
					for(; it != sel_buff.end(); ++it)
					{
						if (*it > sel.end) sel.end = *it;
						if (*it < sel.begin) sel.begin = *it;
					}
					return sel;
				}
			}
		}
		else
		{
			int sel = GetSel1();
			return CSelection( sel, sel);
		}
#endif
		return CSelection( -1, -1);
	}

	int CListBox::GetSel1() const
	{
#ifndef UNIX_RELEASE
		return (int) ::SendMessage( handle(), LB_GETCURSEL, 0, 0);
#else
		return 0;
#endif
	}

	bool CListBox::IsMultiple() const
	{
#ifndef UNIX_RELEASE
		long style = ::GetWindowLong( handle(), GWL_STYLE);
		return (style & LBS_EXTENDEDSEL) != 0;
#else
		return false;
#endif
	}

	///////////////////////////////////////////////////////////////////////////
	void * g_this = 0;
	///////////////////////////////////////////////////////////////////////////

	CDialog::CDialog(HWND _hparent, long _ID) 
		:hparent(_hparent), ID(_ID), wnd(0),
		OnAcceptEv(0), OnCancelEv(0), OnStartEv(0), OnFinishEv(0)
	{
	}

	CDialog::~CDialog()
	{
		if (wnd) delete wnd;
	}

	void CDialog::SetOnAcceptEvent( CEvent * _OnAccept)
	{
		OnAcceptEv = _OnAccept;
	}
	void CDialog::SetOnCancelEvent( CEvent * _OnCancel)
	{
		OnCancelEv = _OnCancel;
	}

	void CDialog::SetOnStartEvent( CEvent * _OnStart)
	{
		OnStartEv =_OnStart;
	}

	void CDialog::SetOnFinishEvent( CEvent * _OnFinish)
	{
		OnFinishEv =_OnFinish;
	}
		
	int CDialog::Run()
	{
#ifndef UNIX_RELEASE
		g_this = this;
		return (int) ::DialogBox(hInstance, MAKEINTRESOURCE(ID), hparent,(DLGPROC) DlgProc);
#else
		return 0;
#endif
	}

	void show_msg( HWND h, LONG_PTR p_msg, LONG_PTR p_caption)
	{
#ifndef UNIX_RELEASE
		std::tstring * m = reinterpret_cast <std::tstring *> (p_msg);
		std::tstring * c = reinterpret_cast <std::tstring *> (p_caption);

		const _TCHAR * cc = _T("<!>");
		if (c) cc = c->c_str();

		const _TCHAR * mm = _T("...");
		if (m) mm = m->c_str();
		
		::MessageBox ( h, mm, cc, MB_OK); 
		
		if (c) delete c;
		if (m) delete m;
		
		
#endif
	}

	int CDialog::WndProc( HWND hw, unsigned message, WPARAM wparam, LPARAM lparam)
	{
#ifndef UNIX_RELEASE
		switch (message) 
		{
		case WM_COMMAND:
			{
				return OnCommand(LOWORD(wparam), HIWORD(wparam), (HWND) lparam);
			}
		case WM_SIZE:
			{
				//OnResize();
				return FALSE;
			}
		case MY_WM_SHOW_TEXT:
			{
				show_msg( hw, wparam, lparam);
				return TRUE;
			}
		}
#endif
		return FALSE;
	}

	int CDialog::OnCommand(unsigned ID, int code, HWND hw)
	{
#ifndef UNIX_RELEASE
#ifndef _WIN32_WCE
		switch (ID)
		{
		case IDOK:
					if (OnAcceptEv) OnAcceptEv->OnEvent(); OnAccept();
					if (OnFinishEv) OnFinishEv->OnEvent(); OnEnd();
					::EndDialog(wnd->handle(), 1);
					return TRUE;
		case IDCANCEL:	
					if (OnCancelEv) OnCancelEv->OnEvent(); OnCancel();
					if (OnFinishEv) OnFinishEv->OnEvent(); OnEnd();
					::EndDialog(wnd->handle(), 0);
					return TRUE;
		default:
			{	


				int i = (wnd->handle() == hw) ? (DWLP_USER) : (GWLP_USERDATA);
				CMSWnd * w = (CMSWnd *) GetWindowLongPtr( hw, i);

				if (w)	return w->OnCmd(code, ID);
			}
			
		}
#endif
#endif
		return FALSE; 
	}

 
	LRESULT CALLBACK CDialog::DlgProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
#ifndef UNIX_RELEASE
#ifndef _WIN32_WCE		
		if (uMsg == WM_INITDIALOG)
		{
			LONG_PTR nn = (LONG_PTR) (g_this);
			::SetWindowLongPtr( hwndDlg, DWLP_USER, nn);

			CDialog * d = reinterpret_cast <CDialog*> (nn);
			
			if (d->wnd) delete d->wnd;
			d->wnd = new CMSWnd(hwndDlg);
			if (d->OnStartEv) d->OnStartEv->OnEvent(); d->OnStart();
			
			g_this = 0;
			return FALSE;
		}
		else
		{
			LONG_PTR n = GetWindowLongPtr(hwndDlg, DWLP_USER);
			if (n)
			{
				CDialog * d = reinterpret_cast <CDialog*> (n);
				return d->WndProc( hwndDlg, uMsg, wParam, lParam);
			}
			else return FALSE;
		}
#else
		return FALSE;
#endif
#else
		return FALSE;
#endif
	}	

	CMSWnd * CDialog::get_from_id(int ctrl_type, unsigned ID)
		{

#ifndef UNIX_RELEASE
#ifndef _WIN32_WCE
			if (HWND h = ::GetDlgItem( wnd->handle(), ID))
			{
				if (LONG_PTR p = ::GetWindowLongPtr( h, GWLP_USERDATA))
				{
					CMSWnd * w = reinterpret_cast <CMSWnd *> (p);
					w->add_ref();
					return w;
				}
				else
				{
					CMSWnd * cp = 0;
					switch (ctrl_type)
					{
					case en_check: 
						cp = new CCheckBox( wnd, ID, false);
						break;
					case en_edit:
						cp = new CEdit( wnd, ID);
						break;
					case en_static:
						cp = new CStatic( wnd, ID);
						break;
					case en_lbox:
						cp = new CListBox( wnd, ID);
						break;
					/*case en_lbox_multi:
						cp = new CListBox( wnd, ID, true);
						break;*/
					default : return 0;
					};
					return push(cp);
				}
			}
			else return 0;
#else 
			return 0;
#endif
#else
			return 0;
#endif

		}


}

