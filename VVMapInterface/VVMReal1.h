
// VVMReal1.h: interface for the CVVMReal class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _VVMREAL1_H_included_
#define _VVMREAL1_H_included_

#include <windows.h>
/*
#define min(a,b) ((a)<(b)?(a):(b));
#define max(a,b) ((b)<(a)?(a):(b));
#include <GdiPlus.h>
#undef min
#undef max
*/

#include "VVMInterface.h"


#define LOG_WINDOW

#include "Buffer.h"


//#include "log_win.h"
#include "zipper.h"

#include "points.h"
#include "Masters.h"
#include "VVMap.h"

#include "context.h"

#include "invis_zones.h"

#include "all_transes.h"

#include "get_same_obj.h"

#include "supp_points.h"


//NOTENOTE: we are using the stl & std cpp library
#pragma warning (disable : 4786)
#include <vector>
#include <map>





#define MAX_MASTERS 64	//64 не менять - на это число PointVisibility заточено!
#define MAXOBJECTS 100

struct CObjectRecord;	//Из SRS - структура описывает объект для GetSameObjects
class CSupportPoint;	//Класс опорной точки: координаты, пересчет абс-отн., коорд. на карте

class CRelativePoint;	//Класс точки, заданной расстояниями - расстояния, координаты


extern HMODULE hDllModule;


extern const char szMainFile[];


/// Главный класс 
class CVVMReal  
{
public:
	
	long GetHoriz(int master_index, const POINT **points, int * points_count);
	long RunOptionsDialog(HWND hParent);

	long GetPicCoords(long px, long py, double * X, double *  Y, double *  Z);
	long GetMapCoords(double X,double Y,double Z,long *px,long *py);

	long AlignMap(long N,long *SP_nums,long *MapCoords);
	long GetAlignMap( long* pNumber, const long** pIndex, const long** pPoint );

	long GetProcessedBitmap(PBITMAPINFO *pbi, PBYTE *pbits);
	long GetColors(long **ppColors);
	long ProcessMapBitmap(long *pColors=NULL);
	long SetColors(long *pColors);
	
	long RP_GetMapCoords(long N,long *pX,long *pY);
	long RP_GetAbsCoords(long N,double *pX,double *pY,double *pZ);
	long RP_Get(long N, long *ND, long **SP,double **R);
	long RP_GetNumOfPoints(long *pN);
	long RP_AddList(long N, long *ND, long *SP,double *R);
	
	long GetAbsCoordsFromDistances(double *R,double *pX,double *pY, double *pZ);
	long GetCoordsFromDistances(double *R,long *pX,long *pY);



	long GetMapOrigin(long *pX, long *pY);
	long SetMapOrigin(long X, long Y);
	long GetMasterSP(long /*Master*/,long * /*SP*/)		{ return ERR_NOT_IMPLEMENTED;}
	long SetMasterSP(long /*Master*/,long /*SP*/)		{ return ERR_NOT_IMPLEMENTED;}
	
	long IZ_AddList(long N, long *M, long *NP, POINT *pPoints);
	long IZ_GetNumOfZones(long *pN);
	long IZ_RemoveMaster(long Master);
	long IZ_Remove(long N);
	long IZ_GetMaster(long Master, long *pN,long **ppNums);
	long IZ_Get(long N, long *pMaster, long *Np, POINT **ppPoints);
	long IZ_Add(long Master,long NumPts, POINT *pPoints, long *pN);
	long IZ_Clear();
	long IZ_PreAllocate(long N);

	
	long GetMapScale(double *pX,double *pY);
	long SetMapScale(double sX,double sY);

	long SP_GetDefining(long **ppNums);
	long SP_AddList(long N, double *Coords, long *Calc);
	long SP_GetVisible(long /*SP*/,long /*Master*/, POINT * /*P*/)	{ return ERR_NOT_IMPLEMENTED;}
	long SP_SetVisible(long /*SP*/,long /*Master*/, POINT /*P*/)	{ return ERR_NOT_IMPLEMENTED;}
	long SP_SetDefining(long * /*pNums*/)							{ return ERR_NOT_IMPLEMENTED;}
	long SP_GetRelative(long N1,long N2,double *Ro,double *Phi,double *Psi);
	long SP_GetAbsolute(long N,double *X,double *Y,double *Z,long *C);
	long SP_GetNumOfPoints(long *pN);
	long SP_Remove(long N);
	long SP_AddRelative(long *pN,long P,double Ro,double Phi,double Psi);
	long SP_AddAbsolute(long *pN,double X,double Y,double Z);
	long SP_Clear();
	long SP_PreAllocate(long N);
	
	long GetSameObjects(long il,long ir, const CObjectRecord *Obj1, const CObjectRecord *Obj2,long N1,long N2,long **IDs);
	long GetTransQuality(long il,long ir,long *pQ);
	const char* GetInterfaceVersion();

	long StopProcess();
	long GetProgress(long *P);
	TCHAR * ErrString(long e);
	long LoadExtentTable(long masterIndex, void *bits, long N);
	long SaveExtentTable(long masterIndex, void **pbits, long *pN, BOOL Compact=TRUE);
	long GetExtentTable(long masterIndex, long **ptable, long *pSize);
	long CompileExtents();
	long GetExtentList(long masterIndex, long* pN, PRECT *prectArr);
	long SetExtentList(long masterIndex, long N, PRECT rectArr);
	long GetPoly(long ifrom, long ito, long *pN, POINT **pp);
	long Load(void *bits, long N);
	long Save(void **pbits, long *pN,BOOL Compact=FALSE);
	long CompileTrans();
	long GetXY(long indexfrom, long x1, long y1, long indexto, long *px2, long *py2, BOOL IgnoreMap=FALSE);
	long GetXYSeries(long num, long il, long ir, POINT *From, POINT *To);
	long DeleteMerge(long il,long ir);
	long CheckMerge(long il,long ir);
	long Merge(long indexleft, long indexright);
	long GetPointList(long* pN, long **parr);
	long SetPointList(long N, long *arr);
	long GetMapBitmap(PBITMAPINFO* ppbi, LPBYTE* pbits);
	long SetMapBitmap(PBITMAPINFO pbi, LPBYTE bits);
	long GetMasterBitmap(long i, PBITMAPINFO *ppbi, PBYTE *pbits);
	long SetMasterBitmap(long i, PBITMAPINFO pbi, PBYTE bits);
	long GetMasterID(long i,long *pID);
	long SetMasterID(long i, long ID);
	long GetNumOfMasters(long *pN);
	long SetNumOfMasters(long N);
	
	CVVMReal(long Opt = 1);
	~CVVMReal();

	long SetParameter(long param_id, long param);
	long GetParameter(long param_id);
	long SetMapSize (unsigned long width, unsigned long height);

	//long SaveShort(void **pbits, long *pN);
	virtual long GetMapBuffer	( const void ** pbuff_ptr,	size_t *	pbuff_size );
	virtual long PutMapBuffer	( const void * buff_ptr,	size_t		buff_size  );

private:
	TCHAR Err[MAX_PATH];

	void CVVMReal::SaveExternMapImage();



private:
			//Создает массив матриц для скоростного преобразования

	

	VVMInfo info;

	//Массив точек соответствия (см. интерфейс)
	
	//TPointVisibility *VisMap;			//карта видимости с различных мастеров (на каждый пиксель карты)
	
	KSrcDataContext   * p_src_data;

	trans::CTransCont * p_trans_cont;

///////////////////////////////////////////////////////////////////////////////////////
	
	double Corr(long il,long ir, POINT P1,POINT P2, long S,long c);
		//Вычисляет корреляцию кусков картинки на il и на ir, начиная
		//с Р1 и Р2 соотв, размера SxS, по компоненте с
	double FindCollPoint(POINT &P,long S,long il,long ir);
		//Находит точку на ir, по возможности соответствующую P на il
		//Размер зоны сравнения S

	long compile_progess;
		//Для длительных процессов - прогресс, 0-100
	long compile_stopped;
	
	sys::CCritSect CritSect;
		//Криты для всех объектов и для изменения progress

	get_same_object gso;

	supp_points::sp_man sp;
	

private:

	void Delete_All();
	void Init_All();

	zipper::CBuff m_SaveBuffer;
	zipper::CZipper zippr;
	zipper::CUnZipper unzippr;

	CPoly m_translated_poly;
	std::map < int, std::list < std::vector<POINT> > > m_translated_polygons;
	bool m_enable_extents, m_enable_vismap, m_enable_master_image, m_enable_map_image;

public:
	
	virtual long PutBuffer (const char * name, const void * buff_ptr, size_t buff_size);
	virtual long GetBuffer (const char * name, const void ** pbuff_ptr, size_t * pbuff_size);

	virtual long TranslatePolygons 
				(
					long indexfrom, 
					long indexto, 
					const std::vector<RECT>& rects,
					OUT std::list < std::vector<POINT> >& results
				);

	long InscribeRect(int master_index, long w, long h, RECT& result );
	

private:
	
///////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

	long old_Save(void **pbits, long *pN, BOOL Compact);
	long old_Load(void *bits, long N);

	/////////////////////////////////////////////////////////////////
	mutable std::tstring _err_msg;
	mutable std::tstring main_err_msg;

	void err_msg() const { _err_msg = std::tstring();}
	void err_msg(const char * msg) const { _err_msg = std::tstring(sys::T_str(msg));}
	//////////////////////////////////////////////////////////////
public:
	void ser(ser::CElement& e);
	void bind_opts(opts::KDialog * d, opts::Itm * i);

};
#endif 
