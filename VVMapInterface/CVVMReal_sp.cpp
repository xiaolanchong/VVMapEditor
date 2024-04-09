#pragma warning (disable : 4786)

#include "stdafx.h"
#include "VVMReal1.h"

#include "memleak.h"

long CVVMReal::SP_PreAllocate(long N)
{
	sys::CEnterCS cs_obj (CritSect);
	return sp.SP_PreAllocate(N);
}

long CVVMReal::SP_Clear()
{
	sys::CEnterCS cs_obj (CritSect);
	sp.SP_Clear();
	return 0;
 
}

long CVVMReal::SP_AddAbsolute(long *pN, double X, double Y, double Z)
{
	sys::CEnterCS cs_obj (CritSect);
	sp.SP_AddAbsolute( pN, X, Y, Z);

	return 0;
}


long CVVMReal::SP_AddRelative(long *pN, long P, double Ro, double Phi, double Psi)
{
	sys::CEnterCS cs_obj (CritSect);

	return sp.SP_AddRelative( pN, P, Ro, Phi, Psi);
}

long CVVMReal::SP_Remove(long N)
{
	sys::CEnterCS cs_obj (CritSect);

	sp.SP_Remove(N);
	return 0;
}

long CVVMReal::SP_GetNumOfPoints(long *pN)
{
	if(pN==NULL)return ERR_BAD_ADDRESS;
	
	sys::CEnterCS cs_obj (CritSect);
	*pN = sp.NumOfSP();
	
	return 0;
}

long CVVMReal::SP_GetAbsolute(long N, double *X, double *Y, double *Z, long *C)
{
	sys::CEnterCS cs_obj (CritSect);
	
	return sp.SP_GetAbsolute( N, X, Y, Z, C);
}

long CVVMReal::SP_GetRelative(long N1, long N2, double *Ro, double *Phi, double *Psi)
{
	sys::CEnterCS cs_obj (CritSect);
	
	return sp.SP_GetRelative (N1, N2, Ro, Phi, Psi);
}

long CVVMReal::SetMapScale(double sX, double sY)
{
	sys::CEnterCS cs_obj (CritSect);
	
	return sp.SetMapScale( sX, sY);
}

long CVVMReal::GetMapScale(double *pX, double *pY)
{
	sys::CEnterCS cs_obj (CritSect);
	
	return sp.GetMapScale( *pX, *pY);
}
//////////////////////////////////////////////////////////////////////////

long CVVMReal::AlignMap(long N, long *SP_nums, long *MapCoords)
{
	sys::CEnterCS cs_obj (CritSect);
	return sp.AlignMap(N, SP_nums, MapCoords);
}


long CVVMReal::GetMapCoords(double X, double Y, double Z, long *px, long *py)
{
	sys::CEnterCS cs_obj (CritSect);
	return sp.GetMapCoords( X, Y, Z, px, py);
}


long CVVMReal::GetPicCoords(long px, long py, double * X, double *  Y, double *  Z)
{
	sys::CEnterCS cs_obj (CritSect);
	return sp.GetPicCoords( px, py, X, Y, Z);
}


long CVVMReal::SetMapOrigin(long /*X*/, long /*Y*/)
{
	return 100;
}

long CVVMReal::GetMapOrigin(long *pX, long *pY)
{
	*pX = 0;	*pY = 0;
	return 100;
}

long CVVMReal::SP_AddList(long N, double *Coords, long *Calc)
{
	sys::CEnterCS cs_obj (CritSect);

	return sp.SP_AddList( N, Coords, Calc);
}


long CVVMReal::SP_GetDefining(long **ppNums)
{
	*ppNums=0 ;
	return 100;
}

long CVVMReal::GetAbsCoordsFromDistances(double *R, double *pX, double *pY, double *pZ)
{
	*R = 0; *pX =0; *pY = 0; *pZ = 0;
	return 100;
}

long CVVMReal::RP_AddList(long N, long *ND, long *SP, double *R)
{
	sys::CEnterCS cs_obj (CritSect);
	return sp.RP_AddList( N, ND, SP, R);
}

long CVVMReal::RP_GetNumOfPoints(long *pN)
{
	sys::CEnterCS cs_obj (CritSect);
	
	*pN = sp.NumOfRP();

	return 0;
}

long CVVMReal::RP_Get(long N, long *ND, long **SP, double **R)
{
	sys::CEnterCS cs_obj (CritSect);
	return sp.RP_Get( N, ND, SP, R);
}

long CVVMReal::RP_GetAbsCoords(long N, double *pX, double *pY, double *pZ)
{
	sys::CEnterCS cs_obj (CritSect);
	return sp.RP_GetAbsCoords( N, pX, pY, pZ);
}

long CVVMReal::RP_GetMapCoords(long N, long *pX, long *pY)
{
	sys::CEnterCS cs_obj (CritSect);
	return sp.RP_GetMapCoords( N, pX, pY);
}