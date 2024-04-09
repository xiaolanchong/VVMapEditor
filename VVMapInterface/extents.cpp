#include "stdafx.h"
#include "extents.h"
#include "VVMErrors.h"

#include "memleak.h"
#include "triangulation.h"

/*
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif
*/
namespace masters
{
		//////////////////////////////////////////////////////
		void CExtentCont::load_from_array(unsigned array_size, const KRECT * array)
		{
			//the_table.clear();
			items.assign( array, array + array_size);
		}
		//////////////////////////////////////////////////////
		zipper::CBuff CExtentCont::save_from_buff()
		{
			unsigned char * p = reinterpret_cast <unsigned char *> (GetPtr(the_table));
			const size_t s = the_table.size()*sizeof(long);
			return zipper::CBuff ( p, s);
		}
		void CExtentCont::load_from_buff(zipper::CBuff b)
		{
			const long * p = reinterpret_cast <long *> (b.ptr);
			size_t sl = b.size / sizeof(long);
			the_table.assign( p, p + sl);
		}
		//////////////////////////////////////////////////////

		void CExtentCont::set_val(unsigned x, unsigned y, long val)
		{
			the_table[ indx( x, y) ] = val;
		}
		
		void CExtentCont::clear()
		{
			the_table.clear();
		}


		long CExtentCont::get_val(unsigned x, unsigned y) const
		{
			return the_table[ indx( x, y) ];
		}

		void CExtentCont::old_load(CBufferInput& Input, int _w, int _h)
		{
			w = _w;	h = _h; 
			if(Input.CheckString("Rects=")!=WRONG_FORMAT)
			{
				if(h<0)		throw std::string ("master height < 0");
				
				if(Input.info->is_editor())
				{
					long NumRects, dummy;
					Input.GetLong(&NumRects);

					items.resize(NumRects);

					for(int j=0; j<NumRects; j++)
					{
						Input.GetLong(&dummy);		items[j].left	= dummy;
						Input.GetLong(&dummy);		items[j].top	= dummy;
						Input.GetLong(&dummy);		items[j].right	= dummy;
						Input.GetLong(&dummy);		items[j].bottom = dummy;
					}
				}
				else
				{
					long e;		long L;
					Input.GetLong(&L);
					for(int j=0; j<L*4; j++)
							Input.GetLong(&e);
				}
			}
				
			if(Input.CheckString("EXTENTS")!=WRONG_FORMAT)
			{
				
				long s = w*height()+2;
				if(Input.info->is_commutator())
				{
					Input.Skip(s*sizeof(long));
				}
				else
				{
					the_table.resize(s);
					PBYTE p = reinterpret_cast <PBYTE> (&(the_table[0]));
					if (s>0)	
						Input.GetBuffer( p, s*sizeof(long));
				}
			}
		}


		void CExtentCont::old_load_table(CBufferInput& /*Input*/)
		{

		}

		void CExtentCont::old_save_table(CBufferInput& /*Input*/)
		{
		}


		//////////////////////////////////////////////////////
		void CExtentCont::ser(ser::CElement& e)
		{
			int mi = master_id;
			e.attr ( _T("master_id"), mi);
						
			if (e.is_loading()) set_mastert_id(mi);
			
			const char * the_path = "extent_tables/";
			std::string fname = std::string(the_path) + sys::ANSI_str(tag_str.c_str()) + std::string(".bin");

			e.ser_vect( _T("RECT"), items);
			
			try
			{
				if (e.is_loading())
				{
					zipper::CBuff b;
					e.ser_bin( fname.c_str(), &b);
					load_from_buff(b);
				}
				else
				{
					zipper::CBuff b = save_from_buff();
					e.ser_bin( fname.c_str(), &b);
				}
			}
			catch (const ser::no_file& e) 
			{ 
				the_table.clear();
				MyTrace ("VVMInterface/CExtentCont::ser catched ser::no_file " + std::string(e.what()));
			}
		}

		inline unsigned CExtentCont::indx(unsigned x, unsigned y) const
		{
			return y*w + x + 2;
		}

		void CExtentCont::set_mastert_id(long mid)
		{
			master_id = mid;
			const size_t s = 30; _TCHAR b[s]; int _i = mid;
			_sntprintf ( b, s, _T("extent_id_%i"), _i);
			tag_str = std::tstring (b);
		}
#if 0
		long CExtentCont::compile (int _w, int _h)

		{
			try
			{
					w = _w,		h = _h;
					//long W = w; 
					long H = h;
					long Total = items.size();
					bool Done = false;
					long WH = w*h;
					
					long i,j,t,k;
									
					
			
					std::vector <long> x(Total);
					std::vector <long> y(Total);
					std::vector <long> L(Total);
					std::vector <long> STr(WH);
						
					std::vector <long> Tr[3];
					for ( k=0; k<3; k++) 
						Tr[k].resize(WH);

					long x1,x2,x3,y1,y2,y3;
					long L1,L2,L3,t1=0,t2=0,t3=0;
					long ST,maxST, argMaxST[3], minL, maxL;
					long miny,maxy, minx, maxx;

					for ( i=0; i<WH; i++) 
					{
						for ( k=0; k<3; k++) 
							Tr[k][i]=-1;
						STr[i]=MAXLONG;
					}

					minL=MAXLONG; maxL=MINLONG;

					for (t=0; t<Total; t++) 
					{
						x[t] = (items[t].left + items[t].right)/2;
						y[t] = items[t].bottom - 1;
						if (items[t].bottom-items[t].top+1<=0) return ERR_INVALID_RECT;
						L[t] = 20000/(items[t].bottom - items[t].top + 1);
						minL = min(minL,L[t]); maxL = max(maxL,L[t]);
					}

					maxST = MINLONG;
					double mul,a1,a2;

					for (t1=0;		t1<Total-2;		t1++) 
					for (t2=t1+1;	t2<Total-1;		t2++) 
					for (t3=t2+1;	t3<Total;		t3++) 
					{
					
						x1=x[t1]; x2=x[t2]; x3=x[t3]; 	y1=y[t1]; y2=y[t2]; y3=y[t3];
								//В x1,x2,x3 и y - коорд треугольника
						
						mul = (x1 - x3)*(x2 - x3) + (y1 - y3)*(y2 - y3); mul = mul*mul;
						a1 = (x1 - x3) * (x1 - x3) + (y1 - y3) * (y1 - y3);	
						a2 = (x2 - x3) * (x2 - x3) + (y2 - y3) * (y2 - y3);
						if (a1*a2==0) continue;
						mul /= (a1*a2);
						if (mul > 0.9) continue;
								//Если слишком узкий (скалярное произв. сторон)

						miny = min( y1, min(y2,y3)); maxy = max(y1,max(y2,y3));
						minx = min( x1, min(x2,x3)); maxx = max(x1,max(x2,x3));

						ST = max( maxx - minx, maxy - miny );
								//ST - "Размер" треугольника

						if (ST>maxST) 
						{
							Done=true;
							maxST=ST;
							argMaxST[0]=t1;	argMaxST[1]=t2;	argMaxST[2]=t3;
						}			
						
						for (j=miny; j<=maxy; j++) 
						{
							maxx=MINLONG; minx=MAXLONG;
							for (k=0; k<3; k++) 
							{
								switch (k) 
								{
								case 0: x1=x[t1]; x2=x[t3]; y1=y[t1]; y2=y[t3]; break;						
								case 1: x1=x[t3]; x2=x[t2]; y1=y[t3]; y2=y[t2]; break;
								case 2: x1=x[t1]; x2=x[t2]; y1=y[t1]; y2=y[t2]; break;
								}
								if ((j - y1)*(j - y2)<=0)  
								{
									if (y1 == y2) 
									{
										minx = min( minx, min (x1,x2));
										maxx = max( maxx, max (x1,x2));
									} else 
									{
										i = (j - y1)*(x2 - x1)/(y2 - y1) + x1;
										minx = min( minx, i + ((i - x1)*(y2 - y1)!=(j - y1)*(x2 - x1)));
										maxx = max( maxx,i);
									}
								}
							}				
								//Находим длину отрезка при y=j. k - три возм. пары сторон
							
							for (i=minx; i<=maxx; i++) 
							{	//По всем точкам
								if (ST<STr[j*w+i]) 
								{
									STr[j*w+i]=ST;		//размер интерп. треугольника
									Tr[0][j*w+i]=t1; Tr[1][j*w+i]=t2; Tr[2][j*w+i]=t3;	//Сам треугольник
								}
							}				
						}
					}
						//В каждой точке задан треугольник

					bool flag;
					
					double A,B,C,D;
				
					if (!Done) return ERR_COMPILE_ERROR;

		
					the_table.resize( WH + 2);
					
					the_table[0] = long (w);
					the_table[1] = long (h);
					
					//size_t table_pos = 2;
					
					for (i=0; i<w; i++) 
					for (j=0; j<H; j++) 
					{

						if (STr[j*w+i]<WH*10) 
						{		//Если треугольник задан
							t1=Tr[0][j*w+i];
							t2=Tr[1][j*w+i];
							t3=Tr[2][j*w+i];
							
							flag=false;
						} 
						else 
						{
							flag=true;
							if (Total/4>5)  
							{
								t1=-1;
								
								std::vector <long> label (Total); 
															

								long r, minR, argMin;

								for (k=0; k<Total/4; k++) 
								{
									minR=MAXLONG; argMin=-1;
									for (t=0; t<Total; t++) 
									{
										if (label[t]) continue;
										r=(x[t]-i)*(x[t]-i)+(y[t]-j)*(y[t]-j);
										(r<minR)?((minR=r),(argMin=t)):0;
									}
									if (argMin>=0) label[argMin]=1;
								}	//Набираем Total/4 точек, ближайших к (i,j)

								double X,Y,XX,YY,XY,ZX,ZY,Z; X=Y=XX=YY=XY=ZX=ZY=Z=0;
								long N=0;
								
								for (t=0; t<Total; t++) 
								{
									if (label[t]==0) continue;
									X+=x[t]; Y+=y[t]; XX+=x[t]*x[t]; YY+=y[t]*y[t];
									XY+=x[t]*y[t]; ZX+=L[t]*x[t]; ZY+=L[t]*y[t]; Z+=L[t];
									N++;
								}
								
								X/=N;Y/=N; XX/=N; YY/=N; XY/=N; ZX/=N; ZY/=N; Z/=N;
								
								D = XX*YY + 2*XY*Y*X - X*X*YY - Y*Y*XX - XY*XY;
								
								if (D==0) return (ERR_COMPILE_ERROR);
								A=ZX*YY+ZY*X*Y+XY*Y*Z-Z*X*YY-ZX*Y*Y-XY*ZY; A/=D;
								B=XX*ZY+X*Y*ZX+X*Z*XY-X*X*ZY-Y*Z*XX-ZX*XY; B/=D;
								C=XX*YY*Z+X*ZY*XY+ZX*Y*XY-ZX*YY*X-XX*Y*ZY-Z*XY*XY; C/=D;

								//Строим плоскость по этим точкам по МНК
								
							} else 
							{
								t1 = argMaxST[0];	t2 = argMaxST[1];	t3 = argMaxST[2];	
								//Если точек мало, выбираем самый большой треугольник
							}
						}
						if (t1>=0) 
						{	//Если найден треугольник, найти интерп плоскость
							x1=x[t1]; x2=x[t2]; x3=x[t3]; 	y1=y[t1]; y2=y[t2]; y3=y[t3]; L1=L[t1]; L2=L[t2]; L3=L[t3];				
							
							D=x1*y2+x2*y3+x3*y1-x1*y3-x2*y1-x3*y2;
							if(D==0) return (ERR_COMPILE_ERROR);
							C=x1*y2*L3+x2*y3*L1+x3*y1*L2-x1*y3*L2-x2*y1*L3-x3*y2*L1; C/=D;
							A=L1*y2+L2*y3+L3*y1-L1*y3-L2*y1-L3*y2; A/=D;
							B=x1*L2+x2*L3+x3*L1-x1*L3-x2*L1-x3*L2; B/=D;
						}
						
						set_val( i, j, long(A*i+B*j+C) );
						/////////////
					//	if(M->Extents[j*W+i]>10000)MessageBox(NULL,"!","!",MB_OK);
						if (get_val(i,j) <= 0) 
								set_val ( i, j, 1);			
						set_val (i, j, get_val(i,j) * (flag?(-1):1));
					}

				//const size_t ss = the_table.size();								
				return 0;
			}
			catch (std::bad_alloc)
			{
				return ERR_OUT_OF_MEMORY;
			}
		}

#endif

	static long _round(double x) { return long(x+0.5);}

	PointF GetDelta ( KPOINT cm, KPOINT p0, KPOINT p1 )
	{
		PointF res = p1 - p0;	
		MyFloat t = res.y; res.y = - res.x; res.x = t;
		res = res * MyFloat(1.0 / distance (res, PointF()));
		res = res * sign(res*(cm-p0));
		return res;
	}
	//////////////////////////////////////////////////////////

	KPOINT ClosestPoint ( KPOINT p, KPOINT p0, KPOINT p1, double  * alpha = 0 )
	{	
		long u = p1.x - p0.x, v = p1.y - p0.y;
		double t = -double (u*(p0.x - p.x) + v*(p0.y - p.y)) / (sqr(u) + sqr(v));
		if (alpha) *alpha = std::max( 0.0, std::min ( 1.0, t) );
		if (t < 0.0) return p0;
		if (t > 1.0) return p1;
		
		return KPOINT ( p0.x + _round(u*t), p0.y + _round(v*t) );
	}

	struct HPoint : KPOINT
	{
		typedef KPOINT::coord_type coord_type;
		HPoint() : height(1) {}
		HPoint(KPOINT p) : KPOINT (p), height(-1) {}
		HPoint(int x, int y) : KPOINT(x,y), height(1) {}
		int height;
		
		
	};

	long CExtentCont::compile (int width, int height)
	{
		w = width, h = height;
		using namespace triangulation;
		
		point_vect_t points;		points.reserve (20);
		int_vect heights;			heights.reserve (20);

		// получаем массив точек
		for (krect_vect_t::const_iterator ri = items.begin(); ri != items.end(); ++ri)
		{
			points.push_back ( KPOINT ( ri->left, ri->bottom) ) ;
			heights.push_back( abs(ri->bottom - ri->top) );

			points.push_back ( KPOINT ( ri->right, ri->bottom) ) ;
			heights.push_back( abs(ri->bottom - ri->top) );
		}

		// устанавлиаем размер таблицы
		the_table.resize ( width * height  + 2);
		std::fill ( ALL (the_table), 1 );
		the_table[0] = width;	the_table[1] = height; 

		// строим триангуляцию
		Triangulation tr;
		try
		{
			tr.build_all ( points );
		}
		catch (not_enough_points&)
		{
			return 0;
		}
		// выпуклая оболочка
		typedef std::vector<HPoint> hpoint_vect_t;
		hpoint_vect_t hpoints ( points.size() );
		for (size_t n = 0; n < hpoints.size(); ++n)
		{
			hpoints[n].x = points[n].x; hpoints[n].y = points[n].y; 
			hpoints[n].height = heights[n];
		}

		hpoint_vect_t convex_shell = GetConvexShell(hpoints);
				
		
		// строим афинное преобразование
		for (triang_vect_t::iterator ti = tr.triang_vect().begin(); ti != tr.triang_vect().end(); ++ti)
		{
			KPOINT XY[3], H[3];
			for (int n=0; n<3; ++n)
			{
				int i = ti->index[n];
				XY[n] = points[i];
				H[n] = KPOINT ( 0, heights[i]);
			}
			ti->trans_matr.calc( XY, H );
		}
		
		// заполняем таблицу
		for (int y=0; y < height; ++y)
		for (int x=0; x < width; ++x)
		{
			Matr3x2 m;
			KPOINT p = KPOINT (x,y);

			if (tr.GetMatrForTriang ( p, &m) )
			{
				// область интерполяции
				set_val( x,y, _round(20000.0 / m.mult (p).y/*( tr.TriangNum(p)+2)*/) );
			}
			else
			{
				// вне ее
				if (convex_shell.size() < 3) continue;
				
				HPoint p0 = convex_shell.front(), p1 = convex_shell.back();
				double alpha = 0;
				double min_distance = p.distance( ClosestPoint ( p, p0, p1, &alpha ) );
								
				for (size_t n = 1; n < convex_shell.size(); ++n)
				{
					HPoint tp0 = convex_shell[n-1], tp1 = convex_shell[n];
					
					double t = 0;
					KPOINT tp = ClosestPoint ( p, tp0, tp1, &t );
					double d = tp.distance( p );
					if (d < min_distance)
					{
						min_distance = d; 
						alpha = t;
						p0 = tp0, p1 = tp1;
					}
				}
				
				assert (0.0 <= alpha && alpha <= 1.0);
				double value = p0.height*(1.0-alpha) + p1.height*alpha;
				set_val( x,y, _round(20000.0 / value) );
				
			}
		}
		return 0;
	}
	

}


