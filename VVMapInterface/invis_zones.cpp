
#pragma warning (disable : 4786)

#include "stdafx.h"
#include "invis_zones.h"
#include "VVMErrors.h"

#include "memleak.h"

	void CInvZonesCont::remove_zone(long zone_index)
	{
		if (!is_indx_ok( zone_index)) return;
		zones.erase(zones.begin() + zone_index);

		refresh_index_map();
	}

	void CInvZonesCont::remove_master(long mID)
	{
		for (size_t i=0; i < zones.size();)
			if (zones[i].ID == mID) zones.erase(zones.begin() + i);
			else i++;

		refresh_index_map();
	}

	void CInvZonesCont::add_poly(long mID, const KPOINT * array, int count)
	{
		zones.push_back(CIZPoly());	
		CIZPoly& izp = (zones.back());
		izp.init( array, count);
		
		izp.ID = mID;
		izp.changed = true;
		index_map[mID].push_back(zones.size()-1);
	}


	bool CInvZonesCont::is_invis(long mID, KPOINT p) const
	{
		/*
		for (vect_iterc i = zones.begin(); i != zones.end(); ++i)
			if (i->is_inside(p)) return true;
		return false;
		*/
		map_iterc mi = index_map.find(mID);
		
		if (mi != index_map.end() )
		{
			const long_vect& iv = mi->second;
			
			for (long_iterc i = iv.begin(); i != iv.end(); ++i)
				
				if (zones[*i].is_inside(p)) return true;
		}

		return false;
	}

	bool CInvZonesCont::is_indx_ok(long zone_index) const
	{
		return (zone_index>=0) && (zone_index < (int)zones.size());
	}

	bool CInvZonesCont::is_master_id(long master_id) const
	{
		map_iterc mi = index_map.find(master_id);
		return  (mi != index_map.end() );
	}

	void CInvZonesCont::refresh_index_map()
	{
		index_map.clear();
		for (size_t i = 0; i < zones.size(); ++i)
		{
			long id = zones[i].ID;
			index_map[id].push_back(i);
		}
	}

	void CInvZonesCont::clear()
	{
		zones.clear();
		index_map.clear();
	}

	bool CInvZonesCont::is_changed_for_master(long master_id) const
	{
		bool res = false;
		map_iterc mi = index_map.find(master_id);
		if (mi != index_map.end() )
		{
			const long_vect& v = mi->second;
			for (long_iterc li = v.begin(); li != v.end(); ++li)
				res = res || zones[*li].changed;
			return res;
				
		}
		return false;
	}

//---------------------------------------------------------------------------------------

	long CInvZonesCont::IZ_Add(long Master, long NumPts, const POINT *pPoints, long *pN)
			//Добавить темную зону для мастера Master.
			//Зона задается выпуклым многоугольником с числом вершин NumPts
			//pPoints - массив вершин многоугольника
			//В pN получаем номер зоны
	{
		const KPOINT * p = static_cast <const KPOINT *> (pPoints);	
		add_poly(Master, p, NumPts);
		if (pN) *pN = zones.size() - 1;
		return 0;
	}

	long CInvZonesCont::IZ_SetList(long N, long *M, long *NP, const POINT *pPoints)
			//Задать список темных зон.
			//N - число зон, M - мастера, NP - число вершин в многоугольнике
			//pPoints - массив многоугольников.
			//Уничтожает ранее заданные зоны; номера после IZ_AddList идут подряд
	{
		clear();
		const KPOINT * p = static_cast <const KPOINT *> (pPoints);
		for (int i=0; i<N; ++i)
		{
			long n = NP[i];
			add_poly(M[i], p, n);
			p += n;
		}
		refresh_index_map();
		return 0;
	}

	
	long CInvZonesCont::IZ_Get(long N, long *pMaster, long *Np, POINT **ppPoints) 
		//Получить зону с номером N
	{
		if (is_indx_ok(N))
		{
			*pMaster = zones[N].ID;

			const POINT* pp = static_cast <const POINT *> (zones[N].data());
			*ppPoints = const_cast <POINT *> (pp); 

			*Np = zones[N].points_count();
			return 0;
		}
		else return ERR_INVALID_INDEX;
	}

	long CInvZonesCont::IZ_GetMaster(long Master, long *pN, long **ppNums) const
		//Получить номера всех темных зон для мастера Master
	{
		map_iterc mi = index_map.find(Master);
		
		if ( mi != index_map.end() && (!mi->second.empty()) )
		{
			*pN = mi->second.size();
			*ppNums = const_cast <long*> (&mi->second[0]);
		}
		else { *pN = 0;	*ppNums = 0; }

		return 0;
		
	}

	long CInvZonesCont::IZ_Remove(long N)
		//Удалить зону с номером N
	{
		if (is_indx_ok(N))
		{
			remove_zone(N);		return 0;
		}
		else return ERR_INVALID_INDEX;
	}

	long CInvZonesCont::IZ_RemoveMaster(long Master)
		//Удалить все темные зоны для мастера Master
	{
		remove_master(Master);
		return 0;
	}

	long CInvZonesCont::IZ_Clear()
		//Удалить все темных зоны
	{
		clear();
		return 0;
	}

	long CInvZonesCont::IZ_PreAllocate(long N)
		//Заранее выделить буфер на N темных зон
	{
		zones.reserve(N);
		return 0;
	}

	long CInvZonesCont::IZ_GetNumOfZones(long *pN) const
		//Получить число заданных темных зон
		//Непосредственно после загрузки файла - число зон равно 
		//максимальному номеру, т.е. номера идут подряд
	{
		if (pN) *pN = zones.size();
		return 0;
	}

	void CInvZonesCont::all_changed (bool value)
	{
		for (vect_iter i = zones.begin(); i != zones.end(); ++i)
			i->changed = value;
	}

	void CInvZonesCont::old_load( CBufferInput& Input)
	{
		long L;
		Input.check("Visibility zones=");
		Input.GetLong(&L);
		if(L)
		{
				IZ_Clear();
				IZ_PreAllocate((L/10+1)*10);
				for (long i=0; i<L; ++i)
				{
					long k = 0;
					long NP = 0; 
					long M = 0;
					Input.check("Zone# ");		Input.GetLong(&k);
					Input.check("Points=");		Input.GetLong(&NP);
					Input.check("Master=");		Input.GetLong(&M);
					Input.check("POLY");
					
					if (NP > 0)
					{
						std::vector <KPOINT> P(NP);
						
						KPOINT * p_rect = &P[0];
						PBYTE p_byte = reinterpret_cast <PBYTE> (p_rect);

						Input.GetBuffer( p_byte, P.size()*sizeof(KPOINT));
						
						long n = 0;
						IZ_Add ( M, NP, p_rect, &n);
					}
				
					//if (n!=k) _ERROR(ERR_INVALID_FORMAT);
				}
				refresh_index_map();

		}
	}