
#ifndef _CONTEXT_H_included
#define _CONTEXT_H_included

#include "masters.h"
#include "vvmap.h"
#include "invis_zones.h"
#include "opts_base.h"
#include "res/resource.h"

#include "points.h"
	
	// специально для того, чтобы передать все это в преобразования....
	struct KSrcDataContext
	{
		
		masters::CMastersCont	masters_cont;
		masters::CMapCont		maps_cont;
		CInvZonesCont			iz_cont;
		CPointDump				points_cont;
	///////////////////////////////////////////////////////////
		bool vismap_enabled;
		bool vispoly_enabled;
		bool saving_masters;
		bool saving_map;
		bool extents_enabled;
	///////////////////////////////////////////////////////////
		const VVMInfo * pinfo;
		
			KSrcDataContext (VVMInfo * _pinfo) : pinfo ( _pinfo), 
				
				vismap_enabled		(true), 
				vispoly_enabled		(true),
				saving_masters		(true),
				saving_map			(true),
				extents_enabled		(true)
		{
			/*
			masters_cont	= new masters::CMastersCont;
			maps_cont		= new masters::CMapsCont;
			iz_cont			= new CInvZonesCont;
			points_cont		= new CPointDump;
			*/
		}

		int correct_index(int index) const 
		{
			if (is_master(index)) return index;
			else return map_index();
		}

		inline bool is_master_index_ok(int index)
		{
			return (is_master(index) || is_map(index));
		}
		
		inline int map_index() const { return -1;}
		inline int master_begin() const { return -1;}
		inline int master_end() const { return masters_cont.Count();}

		inline bool is_map(int index) const { return (index == -1) || (index == int(masters_cont.Count()));}
		inline bool is_master(int index) const { return (index >= 0) && (index < int(masters_cont.Count()));}

		inline bool is_inside (long mi, KPOINT p) const { return get_master(mi)->inside(p.x, p.y);}
		inline bool is_map_vis (long mi, KPOINT p) const { return get_map()->is_vis(mi, p.x, p.y);}

		inline void is_editor() const;

		long corr_i(long mi) const 
		{ 
			long mc = (long) masters_cont.Count();
			if (mi == mc) return -1;
			else return mi;
		}

		long master_id(int index) const 
		{
			if ( is_master(index) ) return get_master(index)->getID();
			else return (-1);
		}

		masters::CFrame2D_pic * get_master(long index) 
		{
			if (masters_cont.is_indx_ok(index)) return &(masters_cont.items[index]);
			else return &(maps_cont.items[0]);
		}

		const masters::CFrame2D_pic * get_master(long index) const 
		{
			if (masters_cont.is_indx_ok(index)) return &(masters_cont.items[index]);
			else return &(maps_cont.items[0]);
		}

		masters::CMaster * get_master_ext(long index) 
		{
			if (is_master(index)) return &(masters_cont.items[index]);
			else return 0;
		}

		const masters::CMaster * get_master_ext(long index) const 
		{
			if (is_master(index)) return &(masters_cont.items[index]);
			else return 0;
		}

		masters::CMap * get_map() { return &(maps_cont.items[0]);}
		const masters::CMap * get_map() const { return &(maps_cont.items[0]);}

		void ser(ser::CElement& e)
		{
			e.attr(_T("vismap_enabled"),	vismap_enabled,		true);
			e.attr(_T("vispoly_enabled"),	vispoly_enabled,	true);

			e.attr(_T("saving_map"),		saving_map,			true);
			e.attr(_T("saving_masters"),	saving_masters,		true);
			if (e.is_loading())
			{
				e.sdata.saving_map = e.sdata.saving_map			&& saving_map;
				e.sdata.saving_masters = e.sdata.saving_masters && saving_masters;
				e.sdata.saving_vismap = e.sdata.saving_vismap	&& vismap_enabled;	
				//e.sdata.saving_extents = extents_enabled;
			}
			
			e.ser_obj( _T("points"), points_cont);
			e.ser_obj( _T("masters"), masters_cont);
			e.ser_obj( _T("maps"), maps_cont);
			e.ser_obj( _T("invi_zones"), iz_cont);

			if (e.is_loading() && vismap_enabled)
			{
				vismap_enabled = !get_map()->vmap.is_empty();
			}

		//	masters_cont.enable_extents (extents_enabled);
		}	

		void bind_opts(opts::KDialog * d, opts::Itm * i)
		{
			d->bind( i, IDC_VISMAP_ENABLED, &vismap_enabled);
			d->bind( i, IDC_VISPOLY_ENABLED, &vispoly_enabled);
		}

		long get_param(long param_id)
		{
			switch (param_id)
				{
				case GP_SAVE_MAP_IMAGE	:			//	сохранять изображение карты
					return (saving_map) ? (1) : (0);

				case GP_SAVE_MASTERS_IMAGES	:		//	сохранять изображения мастеров
					return (saving_masters) ? (1) : (0);

				case GP_ENABLE_VIS_MAP :
					return (vismap_enabled) ? (1) : (0);

				case GP_ENABLE_EXTENTS:
					return (extents_enabled) ? (1) : (0);
				};
				return GP_NO_PARAM;
		}

		bool set_param(long param_id, long value)
		{
				switch (param_id)
				{
				case GP_SAVE_MAP_IMAGE	:			//	сохранять изображение карты
					saving_map = (value) ? (true) : (false);
					return true;

				case GP_SAVE_MASTERS_IMAGES	:		//	сохранять изображения мастеров
					saving_masters = (value) ? (true) : (false);
					return true;

				case GP_ENABLE_VIS_MAP :
					vismap_enabled = (value) ? (true) : (false);
					return true;

				case GP_ENABLE_EXTENTS:
					extents_enabled = (value) ? (true) : (false);
					//this->masters_cont.enable_extents (extents_enabled);
					return true;
				};
				return false;
		}

	};

#endif