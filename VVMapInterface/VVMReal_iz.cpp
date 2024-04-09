#include "stdafx.h"
#include "VVMReal1.h"


//////////////////////////////////////////////////////////////////////////

long CVVMReal::IZ_PreAllocate(long /*N*/)
{
	sys::CEnterCS cs_obj (CritSect);	
/*
	if(N<=0) 
			return ERR_CANNOT_ALLOCATE;
	
	if(NumOfZones>0)
			return ERR_CANNOT_ALLOCATE;
	if(N<=AllocNumOfZones)
			return 0;
	
	sys::CEnterCS cs_obj (CritSect);;
	
	SAFE_DELETE(Zones);
	AllocNumOfZones=N;
	Zones = new CInviZone[N];
	
	//LEAVE_CRIT_SECTION;*/
	return 0;
}
//////////////////////////////////////////////////////////////////////////

long CVVMReal::IZ_Clear()
{
	sys::CEnterCS cs_obj (CritSect);
	/*
	sys::CEnterCS cs_obj (CritSect);;
	NumOfZones=AllocNumOfZones=0;
	IZ_PreAllocate(10);
	//if(VisMap)ZeroMemory(VisMap,Map_bi.biWidth*Map_height()*sizeof(TPointVisibility));
	//LEAVE_CRIT_SECTION;
	*/
	return 0;
}
//////////////////////////////////////////////////////////////////////////

long CVVMReal::IZ_Add(long Master, long NumPts, POINT *pPoints, long *pN)
{
	sys::CEnterCS cs_obj (CritSect);	

	return p_src_data->iz_cont.IZ_Add ( Master, NumPts, pPoints, pN);

}

long CVVMReal::IZ_AddList(long N, long *M, long *NP, POINT *pPoints)
{
	sys::CEnterCS cs_obj (CritSect);	

	return p_src_data->iz_cont.IZ_SetList( N, M, NP, pPoints);
}

long CVVMReal::IZ_Get(long N, long *pMaster, long *Np, POINT **ppPoints)
{
	sys::CEnterCS cs_obj (CritSect);	
	
	return p_src_data->iz_cont.IZ_Get( N, pMaster, Np, ppPoints);

/*
	if(N>=AllocNumOfZones)return ERR_WRONG_ZONE;
	if(Zones[N].Poly==NULL)return ERR_WRONG_ZONE;

	if((NULL==pMaster)||(NULL==Np)||(NULL==ppPoints))return ERR_BAD_ADDRESS;
	
	sys::CEnterCS cs_obj (CritSect);;
	*pMaster=Zones[N].M;
	*Np=Zones[N].Num;
	*ppPoints=Zones[N].Poly;
	//LEAVE_CRIT_SECTION;
	return 0;
	*/

//	err_msg("IZ_Get");
//	return 100;

}

long CVVMReal::IZ_GetMaster(long Master, long *pN, long **ppNums)
{
	sys::CEnterCS cs_obj (CritSect);	

	return p_src_data->iz_cont.IZ_GetMaster( Master, pN, ppNums);
}

long CVVMReal::IZ_Remove(long N)
{
	sys::CEnterCS cs_obj (CritSect);
	
	return p_src_data->iz_cont.IZ_Remove(N);
}

long CVVMReal::IZ_RemoveMaster(long Master)
{
	sys::CEnterCS cs_obj (CritSect);
	return p_src_data->iz_cont.IZ_RemoveMaster(Master);
}

long CVVMReal::IZ_GetNumOfZones(long *pN)
{
	sys::CEnterCS cs_obj (CritSect);
	
	if(!pN)return ERR_BAD_ADDRESS;

	return p_src_data->iz_cont.IZ_GetNumOfZones(pN);

}
