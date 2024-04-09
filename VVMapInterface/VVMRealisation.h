// VVMRealisation.h: interface for the CVVMRealisation class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
//NOTENOTE: now in our folder instead SRS/SourceLibrary
#include "VVMInterface.h"

class CVVMReal;

//  ласс-жирова¤ прослойка.. соединительна¤ ткань.. 
class CVVMRealisation : public CVVMapTrans  
{
public:
	virtual long RunOptionsDialog(HWND hParent);
	virtual long GetCoordsFromMap(double *X,double *Y,double*Z,long x,long y);
	virtual long GetMapCoords(double X,double Y,double Z,long *px,long *py);
	
	virtual long AlignMap(long N,long *SP_nums,long *MapCoords);
	virtual long GetAlignMap( long* pNumber, const long** pIndex, const long** pPoint );

	virtual long GetProcessedBitmap(PBITMAPINFO *pbi,PBYTE *pbits);
	virtual long ProcessMapBitmap(long *pColors=NULL);
	virtual long GetColors(long **ppColors);
	virtual long SetColors(long *pColors);
	virtual long RP_GetMapCoords(long N,long *pX,long *pY);
	virtual long RP_GetAbsCoords(long N,double *pX,double *pY,double *pZ);
	virtual long RP_GetNumOfPoints(long *pN);
	virtual long RP_Get(long N, long *ND, long **SP,double **R);
	virtual long RP_AddList(long N, long *ND, long *SP,double *R);
	virtual long GetAbsCoordsFromDistances(double *R,double *pX,double *pY, double *pZ);
	virtual long SP_GetDefining(long **ppNums);
	virtual long IZ_AddList(long N, long *M, long *NP, POINT *pPoints);
	virtual long SP_AddList(long N, double *Coords, long *Calc);
	virtual long GetMapOrigin(long *pX,long *pY);
	virtual long SetMapOrigin(long X,long Y);
	CVVMRealisation();
	CVVMRealisation(long Opt);
	virtual ~CVVMRealisation();

	virtual char* GetErrorString(long ErrorID);
	
	virtual long _Release();
	virtual const char * GetInterfaceVersion();
	virtual long SetNumOfMasters(long N);
	virtual long GetNumOfMasters(long* pN);
	virtual long SetMasterID(long index, long ID);
	virtual long GetMasterID(long index, long* pID);
	virtual long SetMasterBitmap(long index, PBITMAPINFO pbi, LPBYTE bits);
	virtual long GetMasterBitmap(long index, PBITMAPINFO* ppbi, LPBYTE* pbits);
	virtual long SetMapBitmap(PBITMAPINFO pbi, LPBYTE bits);
	virtual long GetMapBitmap(PBITMAPINFO* ppbi, LPBYTE* pbits);
	virtual long SetPointList(long N, long *arr);
	virtual long GetPointList(long* pN, long **parr);
	virtual long Merge(long indexleft, long indexright);
	virtual long DeleteMerge(long indexleft, long indexright);
	virtual long CheckMerge(long indexleft, long indexright);
	virtual long GetXY(long indexfrom, long x1, long y1, long indexto, long *px2, long *py2);
	virtual long GetXYSeries(long num, long indexfrom, long indexto, POINT *From,POINT *To);
	virtual long GetTransQuality(long indexfrom,long indexto, long *pQ);
	virtual long Compile();
	virtual long Save(void **pbits, long *pN);
	virtual long SaveShort(void **pbits, long *pN);
	virtual long Load(void *bits, long N);
	virtual long GetMapPolygon(long masterIndex, long *pN, POINT **pp);
	virtual long GetMasterPolygon(long masterIndexFrom, long masterIndexTo, long *pN, POINT **pp);
	virtual long SetExtentList(long masterIndex, long N, PRECT rectArr);
	virtual long GetExtentList(long masterIndex, long* pN, PRECT *prectArr);
	virtual long SaveExtentTable(long masterIndex, void **pbits, long *pN);
	virtual long SaveExtentTableShort(long masterIndex, void **pbits, long *pN);
	virtual long LoadExtentTable(long masterIndex, void *bits, long N);
	virtual long GetExtentTable(long masterIndex, long **ptable, long *pSize);
	virtual long GetProgress(long *pP);
	virtual long StopProcess();
	virtual long GetSameObjects(long index1,long index2,CObjectRecord *Obj1,CObjectRecord *Obj2,long N1,long N2,long **IDs);
/////////////////////////
	
	virtual long SP_AddAbsolute(long *pN,double X,double Y,double Z);
	virtual long SP_AddRelative(long *pN,long P,double Ro,double Phi,double Psi);
	virtual long SP_GetAbsolute(long N,double *X,double *Y,double *Z,long *C);
	virtual long SP_GetRelative(long N1,long N2,double *Ro,double *Phi,double *Psi);
	virtual long SP_Remove(long N);
	virtual long SP_Clear();
	virtual long SP_PreAllocate(long N);
	virtual long SP_GetNumOfPoints(long *pN);

	virtual long SetMapScale(double sX,double sY);
	virtual long GetMapScale(double *pX,double *pY);
	virtual long SP_SetDefining(long *pNums);
	virtual long GetCoordsFromDistances(double *R,long *pX,long *pY);

	virtual long IZ_Add(long Master,long NumPts, POINT *pPoints, long *pN);
	virtual long IZ_Get(long N, long *pMaster, long *Np, POINT **ppPoints);
	virtual long IZ_GetMaster(long Master, long *pN,long **ppNums);
	virtual long IZ_Remove(long N);
	virtual long IZ_RemoveMaster(long Master);
	virtual long IZ_Clear();
	virtual long IZ_PreAllocate(long N);
	virtual long IZ_GetNumOfZones(long *pN);

	///////
	virtual	long SP_SetVisible(long SP,long Master,POINT P);
	virtual	long SP_GetVisible(long SP,long Master,POINT *P);
	virtual	long SetMasterSP(long Master,long SP);
	virtual	long GetMasterSP(long Master,long *SP);

	////////////////////////////////////////////////////
	virtual const wchar_t * GetErrorStringW(long ErrorID);

	virtual	long GetHoriz(int master_index, const POINT ** points, int * points_count);

	virtual long PutBuffer (const char * name, const void * buff_ptr, size_t buff_size);
	virtual long GetBuffer (const char * name, const void ** pbuff_ptr, size_t * pbuff_size);

	virtual long SetParameter(long param_id, long param);
	virtual long GetParameter(long param_id);
	
	virtual long SetMapSize (unsigned long width, unsigned long height);

	virtual long GetMapBuffer	( const void ** pbuff_ptr,	size_t *	pbuff_size );
	virtual long PutMapBuffer	( const void * buff_ptr,	size_t		buff_size  );
	
	virtual long TranslatePolygons
				(
					long indexfrom, 
					long indexto, 
					const std::vector<RECT>& rects,
					std::list < std::vector<POINT> >& results
				);

	virtual long CompileExtents();
	long InscribeRect(int master_index, long w, long h, RECT& result );
	
private:
	CVVMReal* VVMReal;
	char		m_szBuffer[MAX_PATH];
	wchar_t		m_szBufferW[MAX_PATH];
};
