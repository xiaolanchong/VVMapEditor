#include "stdafx.h"

#include "supp_points.h"
#include "solver.h"
#include "VVMErrors.h"
#include "arithmetic.h"
#include "SerializeHelper.h"

namespace supp_points
{
	//static const double _pi = acos ( long double (-1));
	static const double _pi = 3.1415926535897932384626433832795;

	static double r2g (double r) { return 180*r/_pi;}
	static double g2r (double g) { return g*_pi/180;}

	//*************************************************************
	CRelativePoint::CRelativePoint() : OK(0)//NumDst(0),
	{
	}
	//-----------------

	CRelativePoint::~CRelativePoint()
	{
	}

	long CRelativePoint::CalcAbsCoords(const CSupportPoint *LookUp)
	{
		if(NumDst()<4)return 3;
		
		double res[4];
		std::vector<double> A (4*NumDst());
		std::vector<double> b (NumDst());
		
		for(long i=0; i<NumDst();i++)
		{
			if(LookUp[SP[i]].Removed) return 1;
			
			X =	A[i*4]		= -LookUp[SP[i]].X;
			Y =	A[i*4+1]	= -LookUp[SP[i]].Y;
			Z =	A[i*4+2]	= -LookUp[SP[i]].Z;
			A[i*4+3] = 0.5;
			double r_squared=X*X+Y*Y+Z*Z;
			b[i]=(R[i]*R[i]-r_squared)/2;
		}
		CLeastSquareSolver LSS;
		LSS.Reset(NumDst(),4, &A[0]);

		if(!LSS.Solve(res, &b[0])) return 2;
		
		X=res[0];
		Y=res[1];
		Z=res[2];
		return 0;
	}

	long CRelativePoint::CalcMapCoords(long mx, long my, double sx, double sy)
	{
		mX=(long)(X*sx + mx);
		mY=(long)(Y*sy + my);
		OK=1;
		return 0;
	}

	long CRelativePoint::Create(long N, long *pSP, double *pR)
	{
	//	NumDst=N;
		mX=mY=MINLONG;
		
		R.assign( pR, pR + N);
		SP.assign( pSP, pSP + N);

		return 0;
	}

	static unsigned skip_spaces(const char ** buf, unsigned max_lenth)
	{
		unsigned k = 0;
		while( (k < max_lenth) && (*(*buf + k) == ' ') ) 
			k++;
		return k;
	}

	static unsigned skip_non_spaces(const char ** buf, unsigned max_lenth)
	{
		unsigned k = 0;
		while( (k < max_lenth) && (*(*buf + k) != ' ') ) 
			k++;
		return k;
	}

	static long read_long(const char ** buf) 
	{
		const unsigned s = 20;		char tmp[s+1];
		unsigned k=0;

		//while( (k < s) && (*(*buf + k) != ' ') ) k++;
		* buf += skip_spaces( buf, s);
		k += skip_non_spaces( buf, s);

		memcpy( tmp, *buf, k);	tmp[k]=0;	*buf += k;
		return sys::my_atoi(tmp);
	}

	static double read_real(const char ** buf) 
	{
		const unsigned s = 20;		char tmp[s+1];
		unsigned k=0;

		//while( (k < s) && (*(*buf + k) != ' ') ) k++;

		* buf += skip_spaces( buf, s);
		k += skip_non_spaces( buf, s);

		memcpy( tmp, *buf, k);	tmp[k] = 0;		*buf += k;
		return atof(tmp);
	}

	long CRelativePoint::Load(const char *buf)
	{
		const char * pbuf = buf;
		long n = read_long(&pbuf);
		
		

		SP.resize(n);
		R.resize(n);

		for (long i=0; i < n;i++)
		{
			SP[i] = read_long(&pbuf);
		}

		while(*pbuf < ' ') pbuf++;

		for(long ii=0; ii < n; ii++)
		{
			R[ii] = read_real(&pbuf);
		}
		return long(pbuf - buf);
	}

	void CRelativePoint::ser(ser::CElement& e)
	{
		try
		{
			e.attr(_T("X"), X);
			e.attr(_T("Y"), Y);
			e.attr(_T("Z"), Z);

			e.attr(_T("mX"), mX);
			e.attr(_T("mY"), mY);
			
			e.attr(_T("Ok"), OK);

			e.ser_vect_ofnum(_T("SP_num"), SP);		//Номера оп точек
			e.ser_vect_ofnum(_T("R"), R);			//Расстояния
		}
		catch (...)
		{}
	}

	///////////////////////////////////////////////////////////////////////////////////
//----------------------------------


	CSupportPoint::CSupportPoint() :Calced (0),	Removed(true)
	{
	/*	for(long i=0;i<64;i++)
			Screen[i].x = Screen[i].y = MINLONG;*/
	}

	CSupportPoint::~CSupportPoint()
	{
		
	}

	long CSupportPoint::RelToAbs(double r, double phi, double psi, CSupportPoint* SP)
	{
		SP->Z = Z + r*sin(g2r(phi));
		SP->Y = Y + r*cos(g2r(phi))*cos(g2r(psi));
		SP->X = X + r*cos(g2r(phi))*sin(g2r(psi));
		return 0;
	}

	long CSupportPoint::AbsToRel(CSupportPoint &SP, double *r, double *phi, double *psi)
	{
		double x = SP.X - X;
		double y = SP.Y - Y; 
		double z = SP.Z - Z;

		*r = sqrt(x*x + y*y + z*z);
		if (0 == *r) return 1;
		*phi = r2g(asin(z/(*r)));
		*psi = r2g(asin((x*sqrt(x*x+y*y))/((*r)*(*r))));
		return 0;
	}

	void CSupportPoint::ser(ser::CElement& e)
	{
		e.attr(_T("X"), X);
		e.attr(_T("Y"), Y);
		e.attr(_T("Z"), Z);
		
		e.attr(_T("Calced"), Calced);
	}
////////////////////////////////////////////////////////////////////////////////////////////
//****************************************************************************************//
//****************************************************************************************//
//****************************************************************************************//
////////////////////////////////////////////////////////////////////////////////////////////


	bool sp_man::SP_PreAllocate(long N)

	{
		if (N<0) return false;
		
		SP_pts.reserve(N);
		return true;
	}

	void sp_man::SP_Clear()
	//Удалить все точки.
	{
		SP_pts.clear();

	}

	void sp_man::SP_AddAbsolute(long *pN, double X, double Y, double Z)
		//Добавляет опорную точку с измеренными координатами.
		//В pN получаем номер точки
	{
		long n = AddNewSP();
		if (pN) *pN = n;
		
		CSupportPoint &SP = SP_pts[n];
		SP.Calced = 0;
		SP.Removed = false;
		SP.X = X;
		SP.Y = Y;
		SP.Z = Z;
	}

	unsigned sp_man::AddNewSP()
	{
		for (unsigned i=0; i < SP_pts.size(); ++i)
				if (SP_pts[i].Removed)
					return i;

		SP_pts.push_back (CSupportPoint());

		return SP_pts.size() - 1;
	}

	long sp_man::SP_AddRelative(long *pN, long P, double Ro, double Phi, double Psi)
		//Добавляет опорную точку с координатами, измеренными относительно
		//уже заданной точки с номером P
		//В pN получаем номер точки
		//Угол Psi задается не относительно какого-то там выбранного 
		//направления, а относительно оси Y!
	{
		if(!pN)		return ERR_BAD_ADDRESS;
		if(!is_sp(P))	return ERR_WRONG_SP;
				
		*pN = AddNewSP();
		CSupportPoint &SP = SP_pts[*pN];
		SP.Calced = P + 1;
		SP.Removed = false;
		SP_pts[P].RelToAbs(Ro,Phi,Psi,&SP);	//Само посчитает!
		
		return 0;
	}

	long sp_man::SP_GetAbsolute(long N, double *X, double *Y, double *Z, long *C)
		//Получить абсолютные координаты опорной точки с номером N
		//Если С>=0, то координаты были не заданы, а вычислены относительно
		//точки с номером С
	{
		if(!is_sp(N))	return ERR_WRONG_SP;

		if( (!X) || (!Y) || (!Z) || (!C) )		return ERR_BAD_ADDRESS;
		
		*X = SP_pts[N].X;
		*Y = SP_pts[N].Y;
		*Z = SP_pts[N].Z;
		*C = SP_pts[N].Calced - 1;
		
		return 0;
	}

	long sp_man::SP_GetRelative(long N1, long N2, double *Ro, double *Phi, double *Psi)
	//Получить координаты точки N1 относительно N2.
	{
		if(!is_sp(N1) || !is_sp(N2))	return ERR_WRONG_SP;
		
		if ( !Ro || !Phi || !Psi )return ERR_BAD_ADDRESS;

		
		SP_pts[N1].AbsToRel(SP_pts[N2],Ro,Phi,Psi);
	
		return 0;	
	}

	void sp_man::CompactSP()
	{
		if (!SP_pts.empty())
		{
			std::vector <CSupportPoint> SPt(SP_pts.size());

			unsigned n = 0;
			for(unsigned i=0; i < SP_pts.size(); i++)
			{
				if(!SP_pts[i].Removed)	SPt[n] = SP_pts[i];

				for(unsigned j = 0; j<=i; j++)
						if(unsigned(SPt[j].Calced - 1) == i)	SPt[j].Calced = n;
			/*
				for(unsigned k = 0;j<NumOfMasters;j++)
						if(Masters[j].SP_Num==i)	Masters[j].SP_Num = n;
			*/

				n++;
			}

			std::vector <CSupportPoint> temp ( &SPt[0], &SPt[0] + n);

			SP_pts.swap(temp);
		}
	}

	long sp_man::SP_AddList(long N, double *Coords, long *Calc)
		//Задать список опорных точек.
		//N - число точек, Coords - координаты X,Y,Z для всех точек подряд
		//Calc - массив номеров - если Calc[i]>=0, то i-тая точка задана
		//относительными координатами, отн. точки Calc[i]
		//Уничтожает ранее заданные точки; номера после SP_AddList идут подряд
	{
		//SP_Clear();
		
		SP_pts.clear();

		if(!N)	return 0;

		if (!SP_PreAllocate(N)) return ERR_OUT_OF_MEMORY;
		long i;
		for(i=0;i<N;i++)
		{
			long e = 0;
			if(Calc[i]<0)
				SP_AddAbsolute(&e,Coords[i*3],Coords[i*3+1],Coords[i*3+2]);
			else
				SP_AddRelative(&e,Calc[i],Coords[i*3],Coords[i*3+1],Coords[i*3+2]);
			//if(e!=i) return ERR_WRONG_SP;
			//NumOfSP++;
		}
		return 0;
	} 


	long sp_man::RP_AddList(long N, long *ND, long *SP, double *R)
		//Устанавливает список точек, заданных расстояниями до опорных
		//N - число точек
		//ND[i] - число заданных расстояний для i-той точки
		//SP - массив номеров опорных точек, расстояних до которых и задают
		//наши точки. Сначала все для первой, потом все для второй итд
		//R - собственно расстояния, до соотв. точек
	{
		if ( !ND || !SP || !R) return ERR_BAD_ADDRESS;
		if(N<0)				return ERR_INVALID_SIZE;

		RP_pts.clear();
		RP_pts.resize(N);
				
		for(long i=0;i<N;i++)
		{
			RP_pts[i].Create(ND[i], SP, R);

			//if(!
				RP_pts[i].CalcAbsCoords(&SP_pts[0]);
			//)

				//RP_pts[i].CalcMapCoords(MapXOrig,MapYOrig,MapXScale,MapYScale);
			
			SP += ND[i];
			R  += ND[i];
		}

		return 0;
	}

	long sp_man::RP_Get(long N, long *ND, long **SP, double **R)
		//Получает информацию об N-той такой точке
		// ND - число заданных расстояний
		// SP - номера оп. точек, до которых эти расстояния
		// R - сами расстояния
	{
		if (N>=NumOfRP())	return ERR_WRONG_RP;
		if (!ND||!SP||!R)	return ERR_BAD_ADDRESS;
		

		*ND = RP_pts[N].NumDst();
		*SP = &(RP_pts[N].SP[0]);
		*R  = &(RP_pts[N].R[0]);

		
		return 0;
	}

	long sp_man::RP_GetAbsCoords(long N, double *pX, double *pY, double *pZ)
		//Получает абс. координаты точки, заданной расстояниями
		//N - номер точки
	{
		if (N>=NumOfRP())	return ERR_WRONG_RP;
		if (!pX || !pY || !pZ)	return ERR_BAD_ADDRESS;
		
		if(!RP_pts[N].OK)
			if(RP_pts[N].CalcAbsCoords(&SP_pts[0])) return ERR_COORDS_UNDEFINABLE;

		*pX = RP_pts[N].X;
		*pY = RP_pts[N].Y;
		*pZ = RP_pts[N].Z;
		
		return 0;
	}

	long sp_man::RP_GetMapCoords(long N, long *pX, long *pY)
		//Получает координаты точки на карте, заданной расстояниями
		//N - номер точки
	{
		if(N>=NumOfRP())	return ERR_WRONG_RP;

		if(!pX||!pY)	return ERR_BAD_ADDRESS;

		
		if(!RP_pts[N].OK)
		{
			if(RP_pts[N].CalcAbsCoords(&SP_pts[0])) 
							return ERR_COORDS_UNDEFINABLE;
			
		//	RP_pts[N].CalcMapCoords(MapXOrig,MapYOrig,MapXScale,MapYScale);
		}

		return (GetMapCoords(RP_pts[N].X,RP_pts[N].Y,RP_pts[N].Z,pX,pY));
	}

	bool sp_man::is_sp(long i) const 
	{ 
		return sp_ok_i(i) && !SP_pts[i].Removed;
	}

	long sp_man::GetMapCoords(double X, double Y, double Z, long *px, long *py)  const
	{
		if(!px||!py)
					return ERR_BAD_ADDRESS;

		double _x, _y;

		long res = GetMapCoords( X, Y, Z, &_x, &_y);

		*px = (long) _x;	*py = (long) _y;
		return res;
		
	}

	void sp_man::GetMap_2D(double X, double Y, double *px, double *py) const
	{
		
			*px = (long)(MapTrans[0]*X + MapTrans[1]*Y + MapTrans[2]);
			*py = (long)(MapTrans[3]*X + MapTrans[4]*Y + MapTrans[5]);
	}

	void sp_man::GetMap_3D(double X, double Y, double Z, double *px, double *py) const
	{
		
			*px=(long)(MapTrans3D[0]*X + MapTrans3D[1]*Y + MapTrans3D[2]*Z + MapTrans3D[3]);
			*py=(long)(MapTrans3D[4]*X + MapTrans3D[5]*Y + MapTrans3D[6]*Z + MapTrans3D[7]);
	}


	long sp_man::GetMapCoords(double X, double Y, double Z, double *px, double *py) const
	{

		if (use3DAlign && ready_3D)
		{
			GetMap_3D( X, Y, Z, px, py);	return ERR_NO_ERROR;
		}
		
		if (ready_2D)
		{
			GetMap_2D( X, Y, px, py);	return ERR_NO_ERROR;
		}
		
		return ERR_MAP_NOT_ALIGNED;
	}

	void sp_man::GetMap_3D_back(double x, double y, double * pX, double * pY, double * pZ) const
	{
		*pY = (MapTrans3DBack[0]*x + MapTrans3DBack[1]*y + MapTrans3DBack[2]);
		*pX = (MapTrans3DBack[3]*x + MapTrans3DBack[4]*y + MapTrans3DBack[5]); 
		*pZ = (MapTrans3DBack[6]*x + MapTrans3DBack[7]*y + MapTrans3DBack[8]);
	}

	void sp_man::GetMap_2D_back(double x, double y, double * pX, double * pY) const
	{
		*pX = (MapTransBack[0]*x + MapTransBack[1]*y + MapTransBack[2]);
		*pY = (MapTransBack[3]*x + MapTransBack[4]*y + MapTransBack[5]); 
	}

	long sp_man::GetPicCoords(long px, long py, double * X, double *  Y, double *  Z) const
	{
		
		if((!X)||(!Y)||(!Z))
					return ERR_BAD_ADDRESS;
		
		if (use3DAlign && ready_3D)
		{
			GetMap_3D_back( px, py, X, Y, Z);
			return ERR_NO_ERROR;
		}

		if (ready_2D)
		{
			GetMap_2D_back( px, py, X, Y);
			*Z = 0;

			return ERR_NO_ERROR;
		}
	
		return ERR_MAP_NOT_ALIGNED;
	}

	struct wrong_sp {};

	long sp_man::AlignMap(long N, long *SP_nums, long *MapCoords)
	{
		ready_3D = false;
		ready_2D = false;

		if(N<3)	return ERR_CANNOT_ALIGN;
		
		if(!SP_nums||!MapCoords)return ERR_BAD_ADDRESS;
	
		m_arrAlignMapPointNumbers.assign( SP_nums, SP_nums + N);
		m_arrAlignMapPointCoords.assign( MapCoords, MapCoords + N*2);
		
		try 
		{
			calc_2d_align();
			calc_3d_align();
		}
		catch (wrong_sp& )
		{
			return ERR_WRONG_SP;
		}
		catch (...)
		{
			return ERR_CANNOT_ALIGN;
		}

		if (!ready_2D && !(ready_3D && use3DAlign)) return ERR_CANNOT_ALIGN;
		
		return ERR_NO_ERROR;
		
	}

	void sp_man::clear()
	{
		SP_pts.clear();	//Точки, заданные расстояниями
		RP_pts.clear();
	}

	template <class T>
	size_t e_count (T array[])
	{
		return sizeof (array) / sizeof (array[0]);
	}

	void sp_man::ser(ser::CElement& e)
	{
		if (!e.is_loading()) CompactSP();
		
		//все в кучу, без разбору. лень чего-то изобретать
		
		e.attr(_T("use3DAlign"), use3DAlign);

		e.attr(_T("ready_2D"), ready_2D);
		e.attr(_T("ready_3D"), ready_3D);
		
		e.ser_vect (_T("Relative_point"),			RP_pts);
		e.ser_vect (_T("Support_point"),			SP_pts);

		e.ser_static_array(	_T("MapTrans"),			MapTrans,		sizeof (MapTrans) / sizeof (MapTrans[0]));
		e.ser_static_array(	_T("MapTrans3D"),		MapTrans3D,		sizeof (MapTrans3D) / sizeof (MapTrans3D[0]));
		e.ser_static_array(	_T("MapTrans3DBack"),	MapTrans3DBack, sizeof (MapTrans3DBack) / sizeof (MapTrans3DBack[0]));
		
		e.ser_vect_ofnum(_T("AlignMapPoint_Number"),		m_arrAlignMapPointNumbers);
		e.ser_vect_ofnum(_T("AlignMapPoint_coord"),			m_arrAlignMapPointCoords);

		if (e.is_loading()) 
		{
			set_all_removed(false);
			calc_2d_back_matrix();
		}
	}

	void sp_man::old_load(CBufferInput& Input)
	{
		ready_2D = false;
		ready_3D = false;

		if (Input.find_down("Support points="))
		{
			long L = 0;
			Input.check ("Support points=");	Input.GetLong(&L);

			if(L)
			{
				double X,Y,Z;
				long n,k;
				SP_Clear();
				SP_PreAllocate((L/10+1)*10);
				for(long i=0; i<L; i++)
				{
					long e;
					
					if (!Input.find_down("Point# ")) 
						throw std::string ("not enough support points");

					Input.check ("Point# ");	Input.GetLong(&k);
					Input.check ("Calced=");	Input.GetLong(&e);
					
					Input.GetDouble(&X);
					Input.GetDouble(&Y);
					Input.GetDouble(&Z);

					SP_AddAbsolute(&n,X,Y,Z);
					
					if (n!=k) throw std::string ("support points - (n!=k)");
					
					SP_pts[n].Calced=e;
					/*
					if (version>2)
					{
						Input.check ("POINTS");
						for(j=0;j<NumOfMasters;j++)
						{
							Input.GetLong( &SP_pts[i].Screen[j].x );
							Input.GetLong( &SP_pts[i].Screen[j].y );
						}
					}*/
					
				}
				ready_2D = true;
			}

			if ( (Input.version>4) && (Input.find_down("Special relative points=")) )
			{
				Input.check ("Special relative points=");
				Input.GetLong(&L);
				if(L)
				{
					RP_pts.resize(L);
				}
				//NOTENOTE: fix 10/08/04, when L==0 NumOfRP had previous value
				
							
				for(long i=0; i < L; i++)
				{
					Input.Pointer += RP_pts[i].Load(Input.pos());
					Input.skip();
				}
			}
		}
		
		calc_2d_back_matrix();
	}

	void sp_man::old_load_map_align(CBufferInput& Input)
		
	{
		long curr_ptr = Input.Pointer;
		try
		{
			if (!Input.find_down("MAP")) throw int();
			if (Input.version > 10)
			{
				if (!Input.find_down("Trans")) throw int();
				
				Input.Skip("Trans");
				for(unsigned i=0; i<6; i++)
						Input.GetDouble(&MapTrans[i]);
			}

			if (!Input.find_down("AlignPoints=")) 
												throw int();

			SerializeHelper::AlignMapSaver	AMSaver;
			AMSaver.Load( Input.version, Input, m_arrAlignMapPointNumbers, m_arrAlignMapPointCoords );
		}
		catch (int)
		{
			m_arrAlignMapPointNumbers.clear(); 
			m_arrAlignMapPointCoords.clear();
			ready_2D = false;
		}

		Input.Pointer = curr_ptr;
			
	}


	static bool ChooseAndSolve(unsigned N, unsigned M, const double * A, const double * B, double * X)
	{
		using namespace arithmetic;

		if (N < M)  return false;

		if (N == M)
			return SolveByGauss  ( M, A, B, X);
		else
			return SolveByMNK ( N, M, A, B, X);
	}


	bool sp_man::calc_2d_align()
	{
		

		size_t N = m_arrAlignMapPointNumbers.size();
		
		if (N < 3) return false;
		
		const std::vector<long>& SP_nums = m_arrAlignMapPointNumbers;
		const std::vector<long>& MapCoords = m_arrAlignMapPointCoords;

		std::vector <double> A(3*N);
		std::vector <double> bx(N), by(N);
						
		
	//----------------------------------------------
		{
			for(size_t i=0; i<N; i++)
			{
				if (!is_sp(SP_nums[i])) throw wrong_sp();

				const CSupportPoint &SP = SP_pts[SP_nums[i]];

				bx[i]		= MapCoords[i*2 + 0];
				by[i]		= MapCoords[i*2 + 1];
				
				A[i*3+0]	= SP.X;
				A[i*3+1]	= SP.Y;
				A[i*3+2]	= 1;
			}
		}
		
		if (!ChooseAndSolve( N, 3, &A[0], &bx[0], &MapTrans[0]))  return false;
		if (!ChooseAndSolve( N, 3, &A[0], &by[0], &MapTrans[3]))  return false;
		
		//------------------------------------------------------
		//				 обратное преобразование 
		//------------------------------------------------------

		if (!calc_2d_back_matrix()) return false;
/*
		{
			for(size_t i=0; i<N; i++)
			{
				if (!is_sp(SP_nums[i])) throw wrong_sp();

				const CSupportPoint &SP = SP_pts[SP_nums[i]];

				bx[i]		= SP.X;
				by[i]		= SP.Y;
				
				A[i*3+0]	= MapCoords[i*2 + 0];
				A[i*3+1]	= MapCoords[i*2 + 1];
				A[i*3+2]	= 1;
			}

			if (!ChooseAndSolve( N, 3, &A[0], &bx[0], &MapTransBack[0]))  return false;
			if (!ChooseAndSolve( N, 3, &A[0], &by[0], &MapTransBack[3]))  return false;
		}
*/


	//-----------------------------------------------

		ready_2D = true;
		return true;
	}

	bool sp_man::calc_3d_align()
	{	
		size_t N = m_arrAlignMapPointNumbers.size();

		if (N < 4) return false;
		
		const std::vector<long>& SP_nums = m_arrAlignMapPointNumbers;
		const std::vector<long>& MapCoords = m_arrAlignMapPointCoords;
		
		std::vector <double> A(4*N);
		std::vector <double> Bx(N), By(N), Bz(N);

		size_t i;
		

		for(i=0; i<8; i++)	MapTrans3D[i] = 0;

		for(i=0; i<9; i++)	MapTrans3DBack[i] = 0;


		//------------------------------------------------------

		for (i=0; i<N; i++)
		{
			
			if(!is_sp(SP_nums[i]))	throw wrong_sp();
			
			const CSupportPoint &SP = SP_pts[SP_nums[i]];
			
			A[i*4+0] = SP.X;
			A[i*4+1] = SP.Y;
			A[i*4+2] = SP.Z;
			A[i*4+3] = 1;

			Bx[i] = MapCoords[i*2 + 0];
			By[i] = MapCoords[i*2 + 1];
		}
		
		if (!ChooseAndSolve(N, 4, &A[0], &Bx[0], &MapTrans3D[0]))  return false;
		if (!ChooseAndSolve(N, 4, &A[0], &By[0], &MapTrans3D[4]))  return false;
		
		//------------------------------------------------------
		//				 обратное преобразование 
		//------------------------------------------------------

		for (i=0; i<N; i++)
		{
			const CSupportPoint &SP = SP_pts[SP_nums[i]];
		
			A[i*3+0] = MapCoords[i*2];
			A[i*3+1] = MapCoords[i*2+1];
			A[i*3+2] = 1;
			
			Bx[i] = SP.X; 
			By[i] = SP.Y; 
			Bz[i] = SP.Z; 
		}

		if ( !ChooseAndSolve( N, 3, &A[0], &Bx[0], &MapTrans3DBack[0])) return false;
		if ( !ChooseAndSolve( N, 3, &A[0], &By[0], &MapTrans3DBack[3])) return false;
		if ( !ChooseAndSolve( N, 3, &A[0], &Bz[0], &MapTrans3DBack[6])) return false;
		//------------------------------------------------------
		
		ready_3D = true;
		return true;
	}

//=================================================================================

	bool sp_man::calc_2d_back_matrix()
	{
		using namespace arithmetic;
		
		const double src[3][2] = { { 10.0, 0.0}, { 0.0, 10.0}, {10.0, 10.0} };
		
		double A[3][3];
		double Bx[3], By[3];

		for (unsigned i = 0; i < 3; ++i)
		{
			GetMap_2D( src[i][0], src[i][1], &A[i][0], &A[i][1]);

			A[i][2] = 1;
			
			Bx[i] = src[i][0];
			By[i] = src[i][1];
		}

		if ( !SolveByGauss( 3, &A[0][0], &Bx[0], &MapTransBack[0])) return false;
		if ( !SolveByGauss( 3, &A[0][0], &By[0], &MapTransBack[3])) return false;

		return true;

	}




} //namespace
