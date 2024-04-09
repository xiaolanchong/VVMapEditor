// VVMRealisation.cpp: implementation of the CVVMRealisation class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VVMRealisation.h"
#include "VVMInterface.h"

#include "VVMReal1.h"

#include "res\resource.h"

#include "memleak.h"
//////////////////////////////////////////////////////////////////////
// CVVMRealisation
//////////////////////////////////////////////////////////////////////

CVVMRealisation::CVVMRealisation(){
	VVMReal=new CVVMReal(VVMINT_EDITOR);
}

CVVMRealisation::CVVMRealisation(long Opt){
	VVMReal=new CVVMReal(Opt);
}

CVVMRealisation::~CVVMRealisation(){
	delete VVMReal;	
}

char* CVVMRealisation::GetErrorString(long ErrorID)
{
	//NOTENOTE: since VVMInterface returns char* we copy string in our internal buffer
//	static char minLongError[]="Method Not Implemented";
	
	TCHAR *res=NULL;
	if(ErrorID==MINLONG) 
//		res=minLongError;
		res=VVMReal->ErrString( ERR_NOT_IMPLEMENTED  );
	else 
		res=VVMReal->ErrString(ErrorID);
#if defined(UNICODE) || defined (_UNICODE) 
	wcstombs( m_szBuffer, res, MAX_PATH );
#else
	strncpy( m_szBuffer, res, MAX_PATH);	
#endif
	return m_szBuffer;
}

const wchar_t * CVVMRealisation::GetErrorStringW(long ErrorID)
{
	TCHAR *res=NULL;
	if(ErrorID==MINLONG) 
//		res=minLongError;
		res=VVMReal->ErrString( ERR_NOT_IMPLEMENTED  );

	else 
		res = VVMReal->ErrString(ErrorID);
	#if defined(UNICODE) || defined (_UNICODE) 
		wcsncpy( m_szBufferW, res, MAX_PATH);
	#else
		mbstowcs( m_szBufferW, res, MAX_PATH );
	#endif

	return m_szBufferW;
}

long CVVMRealisation::_Release() {
	delete this;
	return 0;
}

long CVVMRealisation::RunOptionsDialog(HWND hParent){return VVMReal->RunOptionsDialog(hParent);}

const char* CVVMRealisation::GetInterfaceVersion()
{
	return VVMReal->GetInterfaceVersion();
}
long CVVMRealisation::SetNumOfMasters(long N) { return VVMReal->SetNumOfMasters(N); }
long CVVMRealisation::GetNumOfMasters(long* pN) { return VVMReal->GetNumOfMasters(pN); }

long CVVMRealisation::SetMasterID(long index, long ID) { return VVMReal->SetMasterID(index,ID); }
long CVVMRealisation::GetMasterID(long index, long* pID) { return VVMReal->GetMasterID(index, pID); }
long CVVMRealisation::SetMasterBitmap(long index, PBITMAPINFO pbi, LPBYTE bits) { return VVMReal->SetMasterBitmap(index,pbi,bits); }
long CVVMRealisation::GetMasterBitmap(long index, PBITMAPINFO* ppbi, LPBYTE* pbits) { return VVMReal->GetMasterBitmap(index,ppbi,pbits); }
long CVVMRealisation::SetMapBitmap(PBITMAPINFO pbi, LPBYTE bits) { return VVMReal->SetMapBitmap(pbi, bits); }
long CVVMRealisation::GetMapBitmap(PBITMAPINFO* ppbi, LPBYTE* pbits) { return VVMReal->GetMapBitmap(ppbi,pbits); }
long CVVMRealisation::SetPointList(long N, long *arr) { return VVMReal->SetPointList(N,arr); }
long CVVMRealisation::GetPointList(long* pN, long **parr) { return VVMReal->GetPointList(pN,parr); }
long CVVMRealisation::Merge(long indexleft, long indexright) { return VVMReal->Merge(indexleft,indexright); }
long CVVMRealisation::DeleteMerge(long indexleft, long indexright) { return VVMReal->DeleteMerge(indexleft,indexright); }
long CVVMRealisation::CheckMerge(long indexleft, long indexright) { return VVMReal->CheckMerge(indexleft,indexright); }
long CVVMRealisation::GetTransQuality(long indexfrom, long indexto, long *pQ){return VVMReal->GetTransQuality(indexfrom,indexto,pQ);}
long CVVMRealisation::GetXY(long indexfrom, long x1, long y1, long indexto, long *px2, long *py2) { return VVMReal->GetXY(indexfrom,x1,y1,indexto,px2,py2); }
long CVVMRealisation::GetXYSeries(long num, long indexfrom, long indexto, POINT *From, POINT *To){return VVMReal->GetXYSeries(num,indexfrom,indexto,From,To);}
long CVVMRealisation::Compile() 
{ 
	long e=VVMReal->CompileTrans(),e1=VVMReal->CompileExtents(); 
	//if((e)&&(e!=ERR_NO_POINTS))return e; else 
	return e?e:e1;
}
long CVVMRealisation::Save(void **pbits, long *pN) { return VVMReal->Save(pbits, pN); }
long CVVMRealisation::SaveShort(void **pbits, long *pN) 
{ 
	return VVMReal->Save(pbits,pN,TRUE); 
}

long CVVMRealisation::Load(void *bits, long N) { return VVMReal->Load(bits,N); }
long CVVMRealisation::GetMapPolygon(long masterIndex, long *pN, POINT **pp) { return VVMReal->GetPoly(masterIndex,-1,pN,pp); }
long CVVMRealisation::GetMasterPolygon(long masterIndexFrom, long masterIndexTo, long *pN, POINT **pp) { return VVMReal->GetPoly(masterIndexFrom,masterIndexTo,pN,pp); }
long CVVMRealisation::SetExtentList(long masterIndex, long N, PRECT rectArr) { return VVMReal->SetExtentList(masterIndex,N,rectArr); }
long CVVMRealisation::GetExtentList(long masterIndex, long* pN, PRECT *prectArr) { return VVMReal->GetExtentList(masterIndex,pN,prectArr); }
long CVVMRealisation::SaveExtentTable(long masterIndex, void **pbits, long *pN) { return VVMReal->SaveExtentTable(masterIndex,pbits,pN,FALSE); }
long CVVMRealisation::SaveExtentTableShort(long masterIndex, void **pbits, long *pN) { return VVMReal->SaveExtentTable(masterIndex,pbits,pN,TRUE); }
long CVVMRealisation::LoadExtentTable(long masterIndex, void *bits, long N) { return VVMReal->LoadExtentTable(masterIndex,bits,N); }
long CVVMRealisation::GetExtentTable(long masterIndex, long **ptable, long *pSize) { return VVMReal->GetExtentTable(masterIndex,ptable,pSize); }
long CVVMRealisation::GetProgress(long *pP){return VVMReal->GetProgress(pP);}
long CVVMRealisation::StopProcess(){return VVMReal->StopProcess();}
long CVVMRealisation::GetSameObjects(long index1, long index2, CObjectRecord *Obj1, CObjectRecord *Obj2, long N1, long N2, long **IDs){return VVMReal->GetSameObjects(index1,index2,Obj1,Obj2,N1,N2,IDs);}

long CVVMRealisation::SP_AddAbsolute(long *pN,double X,double Y,double Z){return VVMReal->SP_AddAbsolute(pN,X,Y,Z);}
long CVVMRealisation::SP_AddList(long N, double *Coords, long *Calc){return VVMReal->SP_AddList(N,Coords,Calc);}
long CVVMRealisation::SP_AddRelative(long *pN,long P,double Ro,double Phi,double Psi){return VVMReal->SP_AddRelative(pN,P,Ro,Phi,Psi);}
long CVVMRealisation::SP_GetAbsolute(long N,double *X,double *Y,double *Z,long *C){return VVMReal->SP_GetAbsolute(N,X,Y,Z,C);}
long CVVMRealisation::SP_GetRelative(long N1,long N2,double *Ro,double *Phi,double *Psi){return VVMReal->SP_GetRelative(N1,N2,Ro,Phi,Psi);}
long CVVMRealisation::SP_Remove(long N){return VVMReal->SP_Remove(N);}
long CVVMRealisation::SP_Clear(){return VVMReal->SP_Clear();}
long CVVMRealisation::SP_PreAllocate(long N){return VVMReal->SP_PreAllocate(N);}
long CVVMRealisation::SP_GetNumOfPoints(long *pN){return VVMReal->SP_GetNumOfPoints(pN);}

long CVVMRealisation::SetMapScale(double sX,double sY){return VVMReal->SetMapScale(sX,sY);}
long CVVMRealisation::GetMapScale(double *pX,double *pY){return VVMReal->GetMapScale(pX,pY);}
long CVVMRealisation::SetMapOrigin(long X, long Y){return VVMReal->SetMapOrigin(X,Y);}
long CVVMRealisation::GetMapOrigin(long *pX, long *pY){return VVMReal->GetMapOrigin(pX,pY);}
long CVVMRealisation::AlignMap(long N, long *SP_nums, long *MapCoords){return VVMReal->AlignMap(N,SP_nums,MapCoords);}
long CVVMRealisation::GetMapCoords(double X, double Y, double Z, long *px, long *py){return VVMReal->GetMapCoords(X,Y,Z,px,py);}
long CVVMRealisation::GetCoordsFromMap(double *X, double *Y, double *Z, long x, long y){return VVMReal->GetPicCoords(x,y,X,Y,Z);}

long CVVMRealisation::SP_SetDefining(long *pNums){return VVMReal->SP_SetDefining(pNums);}
long CVVMRealisation::SP_GetDefining(long **ppNums){return VVMReal->SP_GetDefining(ppNums);}
long CVVMRealisation::GetAbsCoordsFromDistances(double *R, double *pX, double *pY, double *pZ){return VVMReal->GetAbsCoordsFromDistances(R,pX,pY,pZ);}
long CVVMRealisation::GetCoordsFromDistances(double *R,long *pX,long *pY){return VVMReal->GetCoordsFromDistances(R,pX,pY);}

long CVVMRealisation::RP_AddList(long N, long *ND, long *SP, double *R){return VVMReal->RP_AddList(N,ND,SP,R);}
long CVVMRealisation::RP_Get(long N, long *ND, long **SP, double **R){return VVMReal->RP_Get(N, ND, SP,R);}
long CVVMRealisation::RP_GetNumOfPoints(long *pN){return VVMReal->RP_GetNumOfPoints(pN);}
long CVVMRealisation::RP_GetAbsCoords(long N, double *pX, double *pY, double *pZ){return VVMReal->RP_GetAbsCoords(N,pX,pY,pZ);}
long CVVMRealisation::RP_GetMapCoords(long N, long *pX, long *pY){return VVMReal->RP_GetMapCoords(N,pX,pY);}

long CVVMRealisation::IZ_Add(long Master,long NumPts, POINT *pPoints, long *pN)
{
	return VVMReal->IZ_Add(Master,NumPts,pPoints,pN);
}

long CVVMRealisation::IZ_AddList(long N, long *M, long *NP, POINT *pPoints)
{
	return VVMReal->IZ_AddList(N,M,NP,pPoints);
}

long CVVMRealisation::IZ_Get(long N, long *pMaster, long *Np, POINT **ppPoints)
{
	return VVMReal->IZ_Get(N,pMaster,Np,ppPoints);
}

long CVVMRealisation::IZ_GetMaster(long Master, long *pN,long **ppNums)
{
	return VVMReal->IZ_GetMaster(Master,pN,ppNums);
}

long CVVMRealisation::IZ_Remove(long N)
{
	return VVMReal->IZ_Remove(N);
}

long CVVMRealisation::IZ_RemoveMaster(long Master)
{
	return VVMReal->IZ_RemoveMaster(Master);
}

long CVVMRealisation::IZ_Clear()
{
	return VVMReal->IZ_Clear();
}

long CVVMRealisation::IZ_PreAllocate(long N)
{
	return VVMReal->IZ_PreAllocate(N);
}

long CVVMRealisation::IZ_GetNumOfZones(long *pN)
{
	return VVMReal->IZ_GetNumOfZones(pN);
}

long CVVMRealisation::SP_SetVisible(long SP,long Master,POINT P)
{
	return VVMReal->SP_SetVisible(SP,Master,P);
}

long CVVMRealisation::SP_GetVisible(long SP,long Master,POINT *P)
{
	return VVMReal->SP_GetVisible(SP,Master,P);
}

long CVVMRealisation::SetMasterSP(long Master,long SP)
{
	return VVMReal->SetMasterSP(Master,SP);
}

long CVVMRealisation::GetMasterSP(long Master,long *SP)
{
	return VVMReal->GetMasterSP(Master,SP);
}

long CVVMRealisation::SetColors(long *pColors)
{
	return VVMReal->SetColors(pColors);
}

long CVVMRealisation::GetColors(long **ppColors)
{
	return VVMReal->GetColors(ppColors);
}

long CVVMRealisation::ProcessMapBitmap(long *pColors/*=NULL*/)
{
	return VVMReal->ProcessMapBitmap(pColors);
}

long CVVMRealisation::GetProcessedBitmap(PBITMAPINFO *pbi, PBYTE *pbits)
{
	return VVMReal->GetProcessedBitmap(pbi,pbits);
}

long CVVMRealisation::GetAlignMap( long* pNumber, const long** pIndex, const long** pPoint )
{
	return VVMReal->GetAlignMap( pNumber, pIndex, pPoint);
}

long CVVMRealisation::GetHoriz(int master_index, const POINT **points, int * points_count)
{
	return VVMReal->GetHoriz(master_index, points, points_count);
}


long CVVMRealisation::PutBuffer (const char * name, const void * buff_ptr, size_t buff_size)
{
	return VVMReal->PutBuffer( name, buff_ptr, buff_size);
}

long CVVMRealisation::GetBuffer (const char * name, const void ** pbuff_ptr, size_t * pbuff_size)
{
	return VVMReal->GetBuffer( name, pbuff_ptr, pbuff_size);
}

long CVVMRealisation::SetParameter(long param_id, long param)
{
	return VVMReal->SetParameter( param_id, param);
}

long CVVMRealisation::GetParameter(long param_id)
{
	return VVMReal->GetParameter( param_id);
}
	
long CVVMRealisation::SetMapSize (unsigned long width, unsigned long height)
{
	return VVMReal->SetMapSize ( width, height);
}

long CVVMRealisation::GetMapBuffer	( const void ** pbuff_ptr,	size_t * pbuff_size )
{
	return VVMReal->GetMapBuffer ( pbuff_ptr, pbuff_size );
}

long CVVMRealisation::PutMapBuffer	( const void * buff_ptr,	size_t buff_size )
{
	return VVMReal->PutMapBuffer ( buff_ptr,	buff_size );
}

long CVVMRealisation::TranslatePolygons
				(
					long indexfrom, 
					long indexto, 
					const std::vector<RECT>& rects,
					std::list < std::vector<POINT> >& results
				)
{
	return VVMReal->TranslatePolygons ( indexfrom, indexto, rects, results );
}


long CVVMRealisation::CompileExtents()
{
	return VVMReal->CompileExtents();
}

long CVVMRealisation::InscribeRect(int master_index,  long w, long h, RECT& result )
{
	return VVMReal->InscribeRect ( master_index, w, h, result );
}

/////////////////////////////////////////////////////////////////////////////////////////
long __cdecl GetVVMInterface (CVVMapTrans **vvmi, DWORD options) 
{
	*vvmi=new CVVMRealisation(options);
	return 0;
}


