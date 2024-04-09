
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

		//��������� ID � �������

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
			//�������� ������ ���� ��� ������� Master.
			//���� �������� �������� ��������������� � ������ ������ NumPts
			//pPoints - ������ ������ ��������������
			//� pN �������� ����� ����

	long IZ_SetList(long N, long *M, long *NP, const POINT *pPoints);
			//������ ������ ������ ���.
			//N - ����� ���, M - �������, NP - ����� ������ � ��������������
			//pPoints - ������ ���������������.
			//���������� ����� �������� ����; ������ ����� IZ_AddList ���� ������
	
	long IZ_Get(long N, long *pMaster, long *Np, POINT **ppPoints);
		//�������� ���� � ������� N
	
	long IZ_GetMaster(long Master, long *pN,long **ppNums) const;
		//�������� ������ ���� ������ ��� ��� ������� Master
	
	long IZ_Remove(long N);
		//������� ���� � ������� N
	
	long IZ_RemoveMaster(long Master);
		//������� ��� ������ ���� ��� ������� Master
	
	long IZ_Clear();
		//������� ��� ������ ����

	long IZ_PreAllocate(long N);
		//������� �������� ����� �� N ������ ���


	long IZ_GetNumOfZones(long *pN) const;
		//�������� ����� �������� ������ ���
		//��������������� ����� �������� ����� - ����� ��� ����� 
		//������������� ������, �.�. ������ ���� ������

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