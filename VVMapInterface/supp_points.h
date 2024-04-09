
#ifndef _SUPP_POINTS_H_included_
#define _SUPP_POINTS_H_included_

#pragma warning (disable : 4786)

#include "kpoint.h"
#include "BufferInput.h"
#include <vector>


namespace supp_points
{
/*
	struct supp_point
	{
		double x, y, z;

		supp_point() : x(0), y(0), z(0) {}
	};
*/
	class CSupportPoint  
	{
	public:
		long AbsToRel(CSupportPoint& SP,double *r,double *phi, double *psi);
		long RelToAbs(double r, double phi,double psi, CSupportPoint* SP);
		double X,Y,Z;		//Координаты
		int Calced;		//Вычислена по относительным координатам
		bool Removed;		//Удалена
	//	KPOINT Screen[64];	//Кооржинаты данной точки на изображении с мастера

		CSupportPoint();
		virtual ~CSupportPoint();
		void ser(ser::CElement& e);
	};
/*
	struct rel_point : supp_point
		//задается 1 расстоянием + 2 eukfvb
	{
		double r, phi, psi;

		rel_point (const supp_point& p, double _r, double _phi, double _psi)
			: r(_r), phi(_phi), psi(_psi) 
		{
			x = p.x + r * cos(phi) * cos(psi);
			y = p.y + r * sin(phi) * cos(psi);
			z = p.z + r * cos(psi);
		}

		rel_point () : r(0), phi(0), psi(0) {}
	};

	struct ref_point : supp_point
		//задается по расстояниям
	{
		std::vector <double> ;
	};
*/
	//*************************************************************
	class CRelativePoint //: public CSupportPoint
	{
	public:
		
		long Load(const char *buf);
		long Create(long N,long *pSP,double *pR);
		long CalcMapCoords(long mx,long my,double sx,double sy);
		long CalcAbsCoords(const CSupportPoint *LookUp);

		long NumDst() const { return R.size();}	//Число расстояний до оп. точек
		std::vector <long> SP;		//Номера оп точек
		std::vector <double> R;		//Расстояния
		double X,Y,Z;	//Вычисленные трехмерные координаты
		long mX,mY;		//Вычисленные координаты на карте

		bool OK;

		void ser(ser::CElement& e);
		
		CRelativePoint();
		virtual ~CRelativePoint();
	private:
		
	};

//*****************************************************************************
//*****************************************************************************

	class sp_man
	{
		
		double MapTrans[6];			//Коэффициенты аффинного преобразования
									//Физ. координаты -> Координаты на карте в пикселях
		
	
		double MapTransBack[6];			//Коэффициенты обратного аффинного преобразования
									//Физ. координаты <- Координаты на карте в пикселях
	
		double MapTrans3D[8];		//Коэффициенты аффинного преобразования
								//Физ. координаты (X,Y,Z)<-> Координаты на карте в пикселях (X,Y)

		double MapTrans3DBack[9];   //Коэффициенты аффинного преобразования
								//Физ. координаты (X,Y,Z)<-> Координаты на карте в пикселях (X,Y)
	
		bool ready_2D;
		bool ready_3D;

		bool use3DAlign;
	
		std::vector <CRelativePoint> RP_pts;	//Точки, заданные расстояниями
		
		std::vector <CSupportPoint> SP_pts;		//Опрные точки

		typedef std::vector <CSupportPoint>::iterator sp_iter;
		typedef std::vector <CSupportPoint>::const_iterator sp_iterc;

		std::vector<long>		m_arrAlignMapPointNumbers;
		std::vector<long>		m_arrAlignMapPointCoords;
	
		
		bool sp_ok_i(long i) const {return (i>=0) && (i < (int)SP_pts.size());}
		bool is_sp(long i) const; 
/*
		unsigned num_of_removed_sp() const
		{
			unsigned res = 0;

			for (sp_iterc i = SP_pts.begin(); i != SP_pts.end();  ++i)
				if (i->Removed) res++;
			
			return res;

		}
*/
		unsigned sp_man::AddNewSP();
		void set_all_removed(bool value)
		{
			for (sp_iter i = SP_pts.begin(); i != SP_pts.end(); ++i)
				i->Removed = value;
		}

		bool calc_2d_align();
		bool calc_3d_align();
		bool calc_2d_back_matrix();

		void sp_man::GetMap_3D(double X, double Y, double Z, double *px, double *py) const;
		void sp_man::GetMap_2D(double X, double Y, double *px, double *py) const;
		void sp_man::GetMap_3D_back(double x, double y, double * pX, double * pY, double * pZ) const;
		void sp_man::GetMap_2D_back(double x, double y, double * pX, double * pY) const;
	public:
		
		sp_man() : ready_2D(false), ready_3D(false), use3DAlign(true) {}
		long NumOfRP()  { return RP_pts.size();}				//И их количество.
		long NumOfSP()  { return SP_pts.size()/* - num_of_removed_sp()*/;}
		//----------------------------------------------------------------------------
		bool sp_man::SP_PreAllocate(long N);

		void sp_man::SP_Clear();
		void sp_man::SP_AddAbsolute(long *pN, double X, double Y, double Z);
		long sp_man::SP_AddRelative(long *pN, long P, double Ro, double Phi, double Psi);
		long sp_man::SP_AddList(long N,  double *Coords,  long *Calc);

		void sp_man::SP_Remove(long /*N*/) {}//{ return 100;}

		long sp_man::SP_GetAbsolute(long N, double *X, double *Y, double *Z, long *C) ;
		long sp_man::SP_GetRelative(long N1, long N2, double *Ro, double *Phi, double *Psi) ;

		//-----------------------------------------------------------
		long sp_man::SetMapScale(double /*sX*/, double /*sY*/)	 { return 100; }
		long sp_man::GetMapScale(double /**pX*/, double /**pY*/) { return 100; }
		void sp_man::CompactSP();
		long sp_man::SetMapOrigin(long /*X*/, long /*Y*/)		{ return 100; }
		long sp_man::GetMapOrigin(long /**pX*/, long /**pY*/)	{ return 100; }
		
		
		//-----------------------------------------------------------
		long sp_man::RP_AddList(long N, long *ND, long *SP, double *R);
		
		long sp_man::RP_Get(long N, long *ND, long **SP, double **R) ;
		long sp_man::RP_GetAbsCoords(long N, double *pX, double *pY, double *pZ) ;
		long sp_man::RP_GetMapCoords(long N, long *pX, long *pY) ;
		//-----------------------------------------------------------
		long sp_man::GetMapCoords(double X, double Y, double Z, double *px, double *py) const;
		long sp_man::GetMapCoords(double X, double Y, double Z, long *px, long *py)  const;
		long sp_man::GetPicCoords(long px, long py, double * X, double *  Y, double *  Z)   const;
		//-----------------------------------------------------------
		long sp_man::AlignMap(long N, long *SP_nums, long *MapCoords);


		//-----------------------------------------------------------
		void clear();
		virtual void ser(ser::CElement& e);

		void sp_man::old_load(CBufferInput& Input);
		void sp_man::old_load_map_align(CBufferInput& Input);
	};



}

#endif