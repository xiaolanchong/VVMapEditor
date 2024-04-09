#ifndef _MY_WIN_23435
#define _MY_WIN_23435

//все вызовы WinApi - только здесь



namespace std
{
	#undef tstring
	
	#if (defined(_UNICODE) || defined (UNICODE) )
		typedef  wstring tstring;
	#else	
		typedef string tstring;
	#endif
}

namespace my_win
{	
//	typedef void* HWND;

	class CEvent
	{
	public:
		virtual void OnEvent() = 0;
	};
	/**
		интерфейс для окна
	*/
	class CWnd
	{
		mutable bool _valid_value;
		mutable bool _allow_showing_warn;
		std::list <CEvent*> on_change;
	protected:
		virtual void _MsgBox( const _TCHAR * caption, const _TCHAR * text) const = 0;
	public:
		CWnd() : _valid_value(true), _allow_showing_warn(true) {}
		bool is_value_valid() const { return _valid_value; }
		bool is_warnings_to_show() const { return _allow_showing_warn; }

		void value_not_valid() { _valid_value = false; } 
		//void dont_show_warns() { _valid_value = false; } 

		void add_event( CEvent * ev) { on_change.push_back(ev); }

		void OnChange();
		void ClearEvents() { on_change.clear();}
		//-------------------------
		virtual void SetSize( int w, int h) = 0;
		virtual void SetPos( int x, int y) = 0;
		virtual void SetW( int width) = 0;
		virtual void SetH( int height) = 0;

		virtual void SetText(const _TCHAR * text) = 0;
		virtual void Visible(bool vis) = 0;
		virtual void Enabled(bool en) = 0;

		virtual int GetW() const = 0;
		virtual int GetH() const = 0;
		virtual int GetX() const = 0;
		virtual int GetY() const = 0;

		virtual std::tstring GetText() const = 0;
		virtual bool IsVisible() const = 0;
		virtual bool IsEnabled() const = 0;

		void MsgBox( const _TCHAR * caption, const _TCHAR * text) const
		{
			if (_allow_showing_warn) _MsgBox( caption, text);
			CWnd * w = const_cast <CWnd *> (this);
			w->_allow_showing_warn = false;
		}
	};

	class CRefCounter
	{
		int ref_count;
	public:
		CRefCounter() : ref_count(0) {}
		virtual int add_ref() { return ++ref_count;}
		virtual int release() { return --ref_count;}	
		virtual bool is_null() const { return ref_count == 0;}
	};

	class CDialog;

	class CMSWnd :public CWnd, protected CRefCounter
	{
		HWND hwnd;
		HWND par_hwnd;
		void set_user_data();
		friend class CDialog;
		
	protected:
		
		RECT GetRect() const;
		virtual int OnCmd(int /*code*/, unsigned /*ID*/) {return 0;}

		void init(CMSWnd * owner, const _TCHAR * class_name, long style, const _TCHAR * text);
		void init(CMSWnd * owner, long /*_ID*/);
	//	void init(HWND _hwnd);

		virtual void _MsgBox( const _TCHAR * caption, const _TCHAR * text) const;

	public:
		
		inline HWND handle() const { return hwnd;}

		CMSWnd(CMSWnd * owner, const _TCHAR * class_name, long style, const _TCHAR * text);
		CMSWnd(CMSWnd * owner, long _ID);
		CMSWnd(HWND _hwnd);
		CMSWnd();
		virtual ~CMSWnd();

		virtual void SetSize( int w, int h);
		virtual void SetPos( int x, int y);
		virtual void SetW( int width);
		virtual void SetH( int height);

		virtual void SetText(const _TCHAR * text);
		virtual void Visible(bool vis);
		virtual void Enabled(bool en);

		virtual int GetW() const;
		virtual int GetH() const;
		virtual int GetX() const;
		virtual int GetY() const;

		virtual std::tstring GetText() const;
		virtual bool IsVisible() const;
		virtual bool IsEnabled() const;
	};

	class CStatic :public CMSWnd
	{
	protected:
		virtual int OnCmd( int code, unsigned ID);
	public:
		CStatic(CMSWnd * owner, const _TCHAR * txt);
		CStatic(CMSWnd * owner, long _ID);
	};

	class CEdit :public CMSWnd
	{
	protected:
		virtual int OnCmd(int code, unsigned ID);
		bool is_changed() const;		
	public:
		CEdit(CMSWnd * owner, const _TCHAR * txt);
		CEdit(CMSWnd * owner, long _ID);
	};

	class CCheckBox :public CMSWnd
	{
	protected:
		virtual int OnCmd(int code, unsigned ID);
	public:
		CCheckBox( CMSWnd * owner, const _TCHAR * txt, bool _checked);
		CCheckBox(CMSWnd * owner, long _ID, bool _checked);

		void Checked( bool _checked);
		bool IsChecked() const;
	};

	struct CSelection
	{ 
		int begin;
		int end;
		CSelection(int _begin, int _end) : begin(_begin), end(_end) 
		{}
		inline bool selected() const { return !( (begin < 0) || (end < 0) );}
	};

	class CListWnd
	{
	public:
		virtual int ItemsCount() const = 0;
		virtual bool IsMultiple() const = 0;
		virtual void AddString(const _TCHAR * text) = 0;
		virtual void DelString(int index) = 0;
		virtual void InsString(int index, const _TCHAR * text) = 0;
		virtual std::tstring GetString(int index) const = 0;
		virtual void Clear() = 0;
		virtual void Sel1(int index) = 0;
		virtual void Select(const  CSelection& selection) = 0;
		virtual CSelection GetSelection() const = 0;
		virtual int GetSel1() const = 0;

	};

	class CListBox :public CMSWnd, public CListWnd
	{
	protected:
		virtual int OnCmd(int code, unsigned ID);
	public:
		CListBox( CMSWnd * owner,  const _TCHAR * txt, bool multi = false);
		CListBox(CMSWnd * owner, long _ID );

		virtual int ItemsCount() const;
		virtual void AddString(const _TCHAR * text);
		virtual void DelString(int index);
		virtual void InsString(int index, const _TCHAR * text);
		virtual std::tstring GetString(int index) const;
		virtual void Clear();
		virtual void Sel1(int index);
		virtual void Select(const  CSelection& selection);

		virtual CSelection GetSelection() const;
		virtual int GetSel1() const;
		virtual bool IsMultiple() const;
	};
/*
	class CFrame :public CMSWnd
	{

	};
*/
	class WndList
	{
		std::list <CWnd*> items;
	public:
		CMSWnd * push(CMSWnd* w) { items.push_back(w); return w; }
		virtual ~WndList ();
	};

	enum 
	{
		en_check, en_edit, en_static, en_button, en_lbox, en_lbox_multi
	};

	class CDialog :public WndList
	{
		CEvent * OnAcceptEv;
		CEvent * OnCancelEv;
		CEvent * OnStartEv;
		CEvent * OnFinishEv;

		CMSWnd * wnd;

		HWND hparent;
		long ID;
		static LRESULT CALLBACK DlgProc( HWND hwndDlg, UINT uMsg, 
										WPARAM wParam, LPARAM lParam);

		int OnCommand(unsigned ID, int code, HWND hw);
		int WndProc( HWND hw, unsigned message, WPARAM wparam, LPARAM lparam);
	protected:

		virtual void OnStart()	{};
		virtual void OnEnd()	{};
		virtual void OnAccept()	{};
		virtual void OnCancel()	{};
		
		inline CMSWnd * parent_wnd() const { return wnd;}

	public:
		CDialog(HWND _hparent, long _ID);
		virtual ~CDialog();

		void SetOnAcceptEvent( CEvent * _OnAccept);
		void SetOnCancelEvent( CEvent * _OnCancel);
		void SetOnStartEvent( CEvent * _OnStart);
		void SetOnFinishEvent( CEvent * _OnFinish);

		CMSWnd * get_wnd() const { return wnd;}
		
		virtual int Run();
		
		CMSWnd * get_from_id(int ctrl_type, unsigned ID);
	};

		
}

#endif