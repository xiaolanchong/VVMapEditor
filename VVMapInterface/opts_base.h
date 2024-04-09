#ifndef _OPTS_BASE_46546
#define _OPTS_BASE_46546

#include <list>
#include "my_win.h"
#include "sys.h"


namespace opts
{
	using namespace my_win;

	template <class T>
	struct CheckVal
	{
		virtual bool check ( T& val, std::tstring * msg_str) = 0;
	};


	template <class T>
	class KVal
	{
		T value;
		T *pvalue;
		bool sel;
	public:
		KVal(T * _pvalue)
			:value(*_pvalue), pvalue( _pvalue), sel(false)
		{}
		void reload_val() { value = *pvalue; }
		void accept_val() { *pvalue = value; }
	
		void sval(const T& v) { value = v;}
		const T& gval() const { return value;}
		T& gval_nc() { return value;}
		//virtual 
	};

	class Itm
	{
		bool sel;
	protected:

		std::vector <Itm*> items;

		virtual void _init() = 0;
		virtual void _read_value() = 0;
		virtual void _set_value() = 0;

		virtual void _show(bool vis) const = 0;
		virtual void _enabled(bool e) const = 0;
		virtual void _accept() = 0;

	public:
		typedef std::vector <Itm*>::iterator iter;
		typedef std::vector <Itm*>::const_iterator iter_c;

		Itm();

		void init();
		void read_value();
		void set_value();

		virtual void read_self_value();
		virtual void set_self_value();

		void select( bool _sel);
		void show(bool vis) const;
		void enabled(bool e) const;
		bool selected() const { return sel;}
		Itm * add_item(Itm * item);
		void accept();
		void show_childs(bool vis) const;
		void enable_childs(bool e) const;

		void sel_none();
		void select_childs(CSelection selection);
		
		virtual ~Itm();
	};

	class bad_wnd {};

	class KChild :public Itm, protected CEvent
	{
	protected:
		virtual void _init() {}
		virtual CWnd * wnd() const = 0;	
		virtual void _show(bool vis) const { wnd()->Visible(vis); }
		virtual void _enabled(bool e) const { wnd()->Enabled(e); }
		virtual void OnEvent() { read_self_value(); }
	};

	template <class T>
	class KValue :protected KVal<T>, public KChild
	{
		CheckVal<T> * check_event;
	protected:
		virtual void _accept() { KVal<T>::accept_val(); }
		bool cmp(const Itm* i) const
		{
			const KValue<T>* p = dynamic_cast <const KValue<T>*> (i);
			if (!p) return false;
			return KVal<T>::gval() == p->gval();
		}
		
		virtual bool check_value(const T& val) 
		{
			if (wnd()->is_value_valid())
			{
				T _val = val;
				std::tstring msg;
				bool m = true;
				if (check_event)
				{
					m = check_event->check( _val, &msg);
					if (!m) wnd()->value_not_valid();
				}
				if (m)	sval(_val);
				if (msg.size()) wnd()->MsgBox( _T("<!>"), msg.c_str());
				return m;
			}
			return false;
		}

	public:
		KValue(T * pval, CheckVal<T> * _check_event) 
			: KVal<T> (pval), check_event(_check_event)
		{}
			
	};

	class KBool :public KValue<bool>
	{
		CCheckBox * const chbox;
		KBool(const KBool&);
		KBool& operator= (const KBool&);
	protected:
		virtual void _read_value() 
		{
			bool ch = chbox->IsChecked();
			sval(ch);		
			enable_childs(ch  && chbox->IsEnabled());
		}
		virtual void _set_value() 
		{
			chbox->Checked(gval());		
			enable_childs(gval() && chbox->IsEnabled());
		}
		virtual CWnd * wnd() const { return chbox;}	
	public:
		KBool (CCheckBox * _chbox, bool * pvalue)
			: chbox(_chbox), KValue<bool>(pvalue, 0)
		{ chbox->add_event(this); }
	};

	class KInt	:public KValue<int>
	{
		CEdit * const edit;
		
		KInt(const KInt&);
		KInt& operator= (const KInt&);
	protected:
		virtual CWnd * wnd() const { return edit;}
		virtual void _read_value() 
		{
			std::tstring s = edit->GetText();
			if (!s.size()) return;
			std::string as = sys::ANSI_str(s.c_str());
			if (!check_value( sys::my_atoi(as.c_str()) )) _set_value();
		}
		virtual void _set_value() 
		{
			const size_t s = 30;
			_TCHAR buff[s];
			_sntprintf( buff, s, _T(" %i"), gval());
			edit->SetText(buff);
		}
	public:
		KInt (CEdit * _edit, int * pvalue, CheckVal<int> * check_event)
			: edit(_edit), KValue<int>(pvalue, check_event)
		{ edit->add_event(this); }
	};

	class KDouble :public KValue<double>
	{
		CEdit * const edit;

		KDouble (const KDouble&);
		KDouble& operator= (const KDouble&);
	protected:
		virtual CWnd * wnd() const { return edit;}
		virtual void _read_value() 
		{
			std::tstring s = edit->GetText();
			if (!s.size()) return;
			std::string as = sys::ANSI_str(s.c_str());
			if (!check_value( atof(as.c_str()) )) _set_value();
		}
		virtual void _set_value() 
		{
			const size_t s = 30;
			_TCHAR buff[s];
			_sntprintf( buff, s, _T(" %f"), gval());
			edit->SetText(buff);
		}
	public:
		KDouble (CEdit * _edit, double * pvalue, CheckVal<double> * check_event)
			: edit(_edit), KValue<double>(pvalue, check_event)
		{ edit->add_event(this); }
	};

	class KLong :public KValue<long>
	{
		CEdit * const edit;
		KLong( const KLong&);
		KLong& operator= ( const KLong&);
	protected:
		virtual CWnd * wnd() const { return edit;}
		virtual void _read_value() 
		{
			std::tstring s = edit->GetText();
			if (!s.size()) return;
			std::string as = sys::ANSI_str(s.c_str());
			if (!check_value( sys::my_atoi(as.c_str()) )) _set_value();
		}
		virtual void _set_value() 
		{
			const size_t s = 30;
			_TCHAR buff[s];
			_sntprintf( buff, s, _T(" %i"), gval());
			edit->SetText(buff);
		}
	public:
		KLong (CEdit * _edit, long * pvalue, CheckVal<long> * check_event)
			: edit(_edit), KValue<long>(pvalue, check_event)
		{ edit->add_event(this); }
	};

	class KString :public KValue <std::tstring>
	{
		CEdit * const edit;
		
		KString(const KString&);
		KString& operator= (const KString&);
	protected:
		virtual CWnd * wnd() const { return edit;}
		virtual void _read_value() 
		{
			if (!check_value(edit->GetText())) _set_value();
		}
		virtual void _set_value() 
		{
			edit->SetText(gval().c_str());
		}
	public:
		KString (CEdit * _edit, std::tstring * pvalue, CheckVal<std::tstring> * check_event)
			: edit(_edit), KValue<std::tstring>(pvalue, check_event)
		{ edit->add_event(this); }
	};

	class KStatic :public KValue <std::tstring>
	{
		CStatic * const stat;

		KStatic(const KStatic&);
		KStatic& operator= (const KStatic&);
	protected:
		virtual CWnd * wnd() const { return stat;}
		virtual void _read_value() {}
		virtual void _set_value()  { stat->SetText(gval().c_str());}

		virtual void _accept()	   {}
	public:
		KStatic (CStatic * _stat, const std::tstring * t) 
			:stat(_stat), KValue <std::tstring> (const_cast <std::tstring *> (t), 0)
		{ stat->add_event(this); }
	};

	class KListBox :public KChild
	{
		CListBox * const lbox;

		KListBox(const KListBox&);
		KListBox& operator= (const KListBox&);
	protected:
		virtual CWnd * wnd() const { return lbox;}
		virtual void _read_value() {}
		virtual void _set_value()  {}
		virtual void _init() 
		{ 
			lbox->Select(CSelection( 0, 0) ); 
			select_childs(lbox->GetSelection());
		}

		virtual void OnEvent()
		{
			select_childs(lbox->GetSelection());
		}

		virtual void _accept()	   {}
	public:
		KListBox (CListBox * _lb) :lbox(_lb)
		{ lbox->add_event(this); }
	};
	
	class KDialog :public CDialog
	{
	protected:

	public:
		KDialog( HWND hpar, long _ID) :CDialog (hpar, _ID) {}

		Itm * text(Itm * parent, long res_ID, const std::tstring * pval)
		{
			CStatic * e = dynamic_cast <CStatic *> (get_from_id( en_static, res_ID));
			return parent->add_item( new KStatic( e, pval));
		}

		Itm * bind (Itm * parent, long res_ID, long * pval, CheckVal<long> * check_event = 0)
		{
			CEdit * e = dynamic_cast <CEdit *> (get_from_id( en_edit, res_ID));
			return parent->add_item( new KLong( e, pval, check_event));
		}

		
		Itm * bind (Itm * parent, long res_ID, double * pval, CheckVal<double> * check_event = 0)
		{
			CEdit * e = dynamic_cast <CEdit *> (get_from_id( en_edit, res_ID));
			return parent->add_item( new KDouble( e, pval, check_event));
		}
		
		Itm * bind (Itm * parent, long res_ID, bool * pval)
		{
			CCheckBox * e = dynamic_cast <CCheckBox *> (get_from_id( en_check, res_ID));
			return parent->add_item( new KBool( e, pval));
		}

		Itm * bind (Itm * parent, long res_ID, int * pval, CheckVal<int> * check_event = 0)
		{
			CEdit * e = dynamic_cast <CEdit *> (get_from_id( en_edit, res_ID));
			return parent->add_item( new KInt( e, pval, check_event));
		}

		Itm * bind (Itm * parent, long res_ID, std::tstring * pval, CheckVal<std::tstring> * check_event = 0)
		{
			CEdit * e = dynamic_cast <CEdit *> (get_from_id( en_edit, res_ID));
			return parent->add_item( new KString( e, pval, check_event));
		}

		template <class TObj>
			Itm * bind_vector (Itm * parent, long res_ID, std::vector<TObj> * vobj)
		{
			my_win::CListBox * lb = dynamic_cast <my_win::CListBox*> (get_from_id( my_win::en_lbox, res_ID));
			Itm * lb_item = parent->add_item( new KListBox( lb) );

			typename std::vector <TObj> ::iterator it = vobj->begin();
			for (; it != vobj->end(); ++it)
			{
				lb->AddString(it->ptr->LB_string());
				it->bind_opts( this, lb_item);
				//strtod
			}
			//lb_item->sel_none();
			return lb_item;
		}
	};

	template <class TObj>
	class KObjDialog :public KDialog, protected Itm
	{
		//KStatic root;
		TObj * obj;
	private:
		virtual void _init() {};
		virtual void _read_value() {};
		virtual void _set_value() {};

		virtual void _show(bool /*vis*/) const {};
		virtual void _enabled(bool /*e*/) const {};
		virtual void _accept(void) {}
	protected:
	
		virtual void OnStart()	
		{
			obj->bind_opts( this, this);

			init();
			enable_childs( true);
			set_value();
		};
		
		virtual void OnEnd() {}

		virtual void OnAccept()	
		{
			Itm::accept();
		};
	public:
		KObjDialog (HWND hpar, long _ID, TObj * o) :obj(o), KDialog( hpar, _ID)
		{}
	};





}

#endif