#include "stdafx.h"
#include "opts_base.h"

namespace opts
{
//	using namespace opts;

	Itm::Itm()
	{
	}

	Itm::~Itm()
	{
		for (iter it = items.begin(); it != items.end(); ++it)
			if (*it) delete (*it);
	}

	void Itm::init()
	{
		_init();
		for (iter it = items.begin(); it != items.end(); ++it)
			if (*it) (*it)->init();
	}

	void Itm::read_value()
	{
		read_self_value();
		for (iter it = items.begin(); it != items.end(); ++it)
			if (*it) (*it)->read_value();
	}

	void Itm::set_value()
	{
		set_self_value();
		for (iter it = items.begin(); it != items.end(); ++it)
			if (*it) (*it)->set_value();	
	}

	void Itm::read_self_value()
	{
		if (sel) _read_value();
	}

	void Itm::set_self_value()
	{
		if (sel) _set_value();
	}

	void Itm::select( bool _sel)
	{	
		//if (sel && !_sel) 	_read_value();
		if (!sel && _sel) 	_set_value();
		sel = _sel;
		for (iter it = items.begin(); it != items.end(); ++it)
			if (*it) (*it)->select(_sel);
	}

	void Itm::sel_none()
	{
		for (iter it = items.begin(); it != items.end(); ++it)
			if (*it) (*it)->select(false);
	}
	void Itm::select_childs(my_win::CSelection selection)
	{
		my_win::CSelection sel2 = selection;
		if ( sel2.begin < 0 ) sel2.begin = 0;
		if ( sel2.end > (int) (items.size()-1) ) sel2.end = int (items.size()-1);
		sel_none();
		
		//Itm * it0 = items[i]
		for (int i = sel2.begin; i <= sel2.end; ++i)
				if (items[i]) 	items[i]->select(true);
	}
/*
	void Itm::init()
	{
		_init();
		for (iter it = items.begin(); it != items.end(); ++it)
			(*it)->init();
	}
*/	
	void Itm::show(bool vis) const
	{
		_show(vis);
		show_childs(vis);
	}

	void Itm::enabled(bool e) const
	{
		_enabled(e);
		enable_childs(e);
	}

	Itm * Itm::add_item(Itm * item)
	{
		items.push_back(item);
		return item;
	}
	void Itm::accept()
	{
		read_self_value();
		_accept();
		for (iter it = items.begin(); it != items.end(); ++it)
			if (*it) (*it)->accept();
	}

	void Itm::show_childs(bool vis) const
	{
		for (iter_c it = items.begin(); it != items.end(); ++it)
			(*it)->show(vis);
	}

	void Itm::enable_childs(bool e) const
	{
		for (iter_c it = items.begin(); it != items.end(); ++it)
			if (*it) (*it)->enabled(e);
	}
}
	