
#ifndef _INVIS_ZONES_H_included
#define _INVIS_ZONES_H_included

#include "poly.h"
//#include "../Masters/frames.h"

#include "BufferInput.h"

struct CIZPoly : CPoly
{
	CIZPoly() :ID(-1), changed (true) {}
	long ID;
	bool changed;
	virtual void ser (ser::CElement& e)
	{
		e.attr(_T("master_ID"), ID);
		CPoly::ser(e);
		
		if (e.is_loading())		changed = true;
	}
};

class CInvZonesCont
{
	std::vector <CIZPoly> zones;
	std::map  < long, std::vector <long> > index_map;

	typedef std::vector <CIZPoly>::iterator			vect_iter;
	typedef std::vector <CIZPoly>::const_iterator	vect_iterc;

	typedef std::vector <long> long_vect;
	typedef std::vector <long>::iterator			long_iter;
	typedef std::vector <long>::const_iterator		long_iterc;

	typedef std::map  < long, std::vector <long> >::iterator		map_iter;
	typedef std::map  < long, std::vector <long> >::const_iterator	map_iterc;

		//переводит ID в индексы

	void remove_zone(long zone_index);
	void remove_master(long mID);

	void add_poly(long mID, const KPOINT * array, int count);

	bool is_indx_ok(long zone_index) const;

	bool is_master_id(long master_id) const;

	void refresh_index_map();

	void clear();

public:

	bool is_invis(long mID, KPOINT p) const;
	const CIZPoly& get( int zone_index) const { return zones[zone_index];}

	long IZ_Add(long Master,long NumPts, const POINT *pPoints, long *pN);
			//Добавить темную зону для мастера Master.
			//Зона задается выпуклым многоугольником с числом вершин NumPts
			//pPoints - массив вершин многоугольника
			//В pN получаем номер зоны

	long IZ_SetList(long N, long *M, long *NP, const POINT *pPoints);
			//Задать список темных зон.
			//N - число зон, M - мастера, NP - число вершин в многоугольнике
			//pPoints - массив многоугольников.
			//Уничтожает ранее заданные зоны; номера после IZ_AddList идут подряд
	
	long IZ_Get(long N, long *pMaster, long *Np, POINT **ppPoints);
		//Получить зону с номером N
	
	long IZ_GetMaster(long Master, long *pN,long **ppNums) const;
		//Получить номера всех темных зон для мастера Master
	
	long IZ_Remove(long N);
		//Удалить зону с номером N
	
	long IZ_RemoveMaster(long Master);
		//Удалить все темные зоны для мастера Master
	
	long IZ_Clear();
		//Удалить все темных зоны

	long IZ_PreAllocate(long N);
		//Заранее выделить буфер на N темных зон


	long IZ_GetNumOfZones(long *pN) const;
		//Получить число заданных темных зон
		//Непосредственно после загрузки файла - число зон равно 
		//максимальному номеру, т.е. номера идут подряд

	void all_changed (bool value);

	bool is_changed_for_master(long master_id) const;

	//////////////////////////////////////////////////////////////////////////

	void ser(ser::CElement& e)
	{
		e.ser_vect( _T("IZ_POLY"), zones);
		
		if (e.is_loading()) refresh_index_map();
	}

	void old_load( CBufferInput& Input);


};

#endif