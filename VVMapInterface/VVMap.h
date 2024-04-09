 
#ifndef _MAP_H_INCLUDED_238956237
#define _MAP_H_INCLUDED_238956237

#include "Masters.h"
#include "kpoint.h"

#include <vector>

namespace masters
{
//----------------------------------------------------------------------------------------------------------
		class CVisColors
		{
			int num_of_masters;
			std::vector <long> colors;
		public:
			virtual void SetColors(long *pColors, int _num_of_masters)
			
			//Устанавливает массив цветов, которыми заполнять карту
			//цвета - четверки RGBA, числом NumOfMasters+1
			//Последняя четверка - для точек, невидимых ниоткуда.
			{
				num_of_masters = _num_of_masters;
				size_t s = (num_of_masters + 1)*4;
				colors.assign( pColors, pColors + s);
			}

			int get_num_of_master() const { return num_of_masters; }
			bool is_empty() const { return colors.empty();}
			//----------------------------------------------------
			virtual void GetColors(const long **ppColors, size_t * psize)
			{
				*ppColors = GetPtr(colors);		*psize = colors.size();
			}

			inline int indx(int mi) const { return mi*4;}

			inline unsigned get_color(int m_index) const 
			{
				if (is_empty()) return 0;

				int n = indx(m_index);
				int a = (colors[n + 3] & 0xff) << 24;
				int b = (colors[n + 2] & 0xff) << 0;
				int g = (colors[n + 1] & 0xff) << 8;
				int r = (colors[n + 0] & 0xff) << 16;
				return  r + g + b + a;
				//return unsigned(colors[m_index]);

			}
			//---------------------------------------------------
			inline const long * get_color_ptr(int mi) const
			{
				return &(colors[indx(mi)]);
			}
			//---------------------------------------------
			void old_load(CBufferInput& Input, int masters_count)
			{
				if(Input.version>5)
				{
					if(Input.CheckString("Map Colors")!=WRONG_FORMAT)
					{
						num_of_masters = masters_count;

						unsigned s = 4*(masters_count + 1);
						colors.resize(s);
					
						for(unsigned i=0; i<s; i++)
						{
							long L = 0;	Input.GetLong(&L);	colors[i] = L;
						}
					}
				}
			}
		};
//----------------------------------------------------------------------------------------------------------		
		class empty_map_bitmap	{};
//----------------------------------------------------------------------------------------------------------	
		class CVisMap
		{
		private:
			std::tstring fname;
			int fw, fh;

			CDib vis_bm;
			struct TPointVisibility //Структурка хранит 64 бита видимости точки с 64х мастеров
				{	
					BYTE v[8];			
					inline BOOL visible(long M)		{	return (v[M>>3]&(1<<(M&7)));}
					inline void remove(long M)	{v[M/8]&=~(1<<(M%8));}
					inline void add(long M)		{v[M/8]|=(1<<(M%8));}

					TPointVisibility(){ZeroMemory(v,8);}
				} * map;

			mutable bool DoBring;

		protected:
			
			void _FillMapRect(long *rx, long *ry, long m);

			//--------------------------------------------
		
			zipper::CBuff get_buff() const
			{
				unsigned char * p = (unsigned char *) map;
				return zipper::CBuff( p, bytes_size() );
			}

			void load_from_buff(zipper::CBuff the_buff)
			{
				release();
				if ( the_buff.size != bytes_size() ) throw size_t(the_buff.size);
				map = new TPointVisibility[fw*fh];
				memcpy ( map, the_buff.ptr, the_buff.size);
			}		

			TPointVisibility * getBuffPtr() const {return map;}
		
		public:
			CVisMap();
			virtual ~CVisMap();

			void release() { if (map) delete (map); map = 0;}

			void bring_to_vis_next_time()  const { DoBring = true;}
				
			
			bool BringToVis(int master_index, long *tx, long *ty) const;
			
			bool IsVis(int master_index, int x, int y) const;
			bool IsInside(int x, int y) const	{ return ((x>=0) && (x<fw) && (y>=0) && (y<fh));}
			
			void ClearMaster(int master_index);
			void ClearAll(void);
			
			void SetPoint(int master_index, int x, int y, bool value, int p_size);
			void SetPoint(int master_index, int x, int y, bool value);

			void PutRect(int master_index, KPOINT p[4])
			{
				long x[4];	long y[4];
				for (int i=0; i<4; ++i)
				{
					x[i] = p[i].x;	y[i] = p[i].y;
				}
				_FillMapRect( x, y, master_index);
			}

			size_t bytes_size() const { return fw*fh*sizeof(TPointVisibility); }

			long SetSize(int w, int h);
			
			bool process_bm(const CDib& map_db, const CVisColors& clrs);
			//--------------------------------------------
			virtual void ser(ser::CElement& e)
			{
				if (!e.is_loading() && !map) return;

				e.attr( _T("vismap_file"), fname);
				e.attr( _T("width"), fw);
				e.attr( _T("height"), fh);
				
				std::string s = sys::ANSI_str(fname.c_str());
				const char * t = s.c_str();
				if (e.is_loading()) 
				{
					zipper::CBuff b;
					
					e.ser_bin( t, &b);
					load_from_buff(b);
				}
				else
				{
					if (!e.sdata.saving_vismap) return;
					zipper::CBuff b = get_buff();
					e.ser_bin( t, &b);
				}
			}

			void set_fname(const _TCHAR * _fname)
			{
				fname = std::tstring(_fname);
			}

			bool is_empty() const { return !map;}

			const CDib& processed_dib() const { return vis_bm;}

			void assign_bitmap( const CDib& bm) 
			{
				if (map)
					vis_bm.assign(bm);
			}

			void old_load(CBufferInput& Input, int width, int height, const PBITMAPINFO pbi);

			friend class map_filler;
		};
//----------------------------------------------------------------------------------------------------------
		class map_filler
		{
			
			CVisMap& vmap;
			typedef std::vector <KPOINT> vect;
			typedef std::vector <bool> bool_vect;
			vect line[2];
			bool_vect bool_line[2];
			unsigned first; unsigned second;
			unsigned xi;	unsigned yi;
			long master_w; long master_h;
			long master_ind;

		public:
			long new_line()
			{ 
				unsigned t = first; first = second; second = t; xi = 0; return ++yi;
			}

			bool end() const { return yi >= unsigned(master_h);} 
			
			map_filler(CVisMap& _vmap, long _master_w, long _master_h, long master_index) 
				:vmap(_vmap), first (0), second(1), xi(0), yi(0), 
				 master_w(_master_w), master_h(_master_h), master_ind(master_index)
			{
				line[first].resize(master_w);
				line[second].resize(master_w);

				bool_line[first].resize(master_w);
				bool_line[second].resize(master_w);

				//vmap.ClearAll();
				vmap.ClearMaster(master_index);
			}
			
			KPOINT cur_pos() const { return KPOINT( xi, yi);}

			long put_point(bool put, KPOINT p)
			{
			/*	if (put)
					vmap.SetPoint( master_ind, p.x, p.y, true);*/
			
				bool_line[first][xi] = put;
				line[first][xi] = p;

				if ((xi>0) && (yi>0))
				{
			
					bool ok = true;
					KPOINT& p1 = line[second][xi];			ok = ok && bool_line[second][xi];
					KPOINT& p2 = line[second][xi-1];		ok = ok && bool_line[second][xi-1];
					KPOINT& p3 = line[first][xi-1];			ok = ok && bool_line[first][xi-1];
					KPOINT& p4 = line[first][xi];			ok = ok && bool_line[first][xi];

					if (ok)
					{
						long x[4]; long y[4];
						x[0] = p1.x;	y[0] = p1.y;
						x[1] = p2.x;	y[1] = p2.y;
						x[2] = p3.x;	y[2] = p3.y;
						x[3] = p4.x;	y[3] = p4.y;
					
						vmap._FillMapRect( x, y, master_ind);
				
					}
				//	if (put) vmap.SetPoint(master_ind, p.x, p.y, true, 5);
				}
			//	*/				
				++xi;
				if (xi >= unsigned(master_w)) new_line();
				return xi;
			}

		};
//----------------------------------------------------------------------------------------------------------
		class CMap :public CFrame2D_pic//, public CVisMap
		{
		public:
			CVisMap vmap;			
		private:
			POINT res_point;
		protected:
			
			virtual std::tstring gen_dib_fname() const
			{
				const size_t s = 30;	_TCHAR buff[s];
				_sntprintf( buff, s, _T("images/map_%i.dib"), getID());
				return std::tstring(buff);
			}
			
			virtual std::tstring gen_vismap_fname() const
			{
				const size_t s = 30;	_TCHAR buff[s];
				_sntprintf( buff, s, _T("vismap/vismap%i.bin"), getID());
				return std::tstring(buff);
			}

		public:	
			
			long ProcessBitmap();
			
			//const CDib * GetVisMapBM() const {return &bm;}
			long ProcessVisBM(const CVisColors& clrs)
			{
				if (vmap.process_bm( the_dib(), clrs)) 
					return ERR_NO_ERROR;
				else
					return ERR_MAP_NOT_COMPILED;
			}

			void clear_vismap()
			{
				vmap.ClearAll();
				vmap.assign_bitmap(the_dib());
			}

			virtual void ser(ser::CElement& e)
			{
				CFrame2D_pic::ser(e);

				if (!e.is_loading()) 
				{
					std::tstring st (gen_vismap_fname());
					vmap.set_fname( st.c_str() );
				}

				try
				{
					if (width() && height() && e.sdata.saving_vismap)
						e.ser_obj(_T("vis_map"), vmap);
				}
				catch(...)
				{}
			}

			virtual bool is_vis(long mi, int x, int y) const 
			{
				return vmap.IsVis( mi, x, y);
			}

			void set_size(int w, int h)
			{
				int_width = w;	
				int_height = h;
				//init_vis_map();
			}

			void init_vis_map()
			{
				vmap.SetSize( width(), height() );
			//	vmap.ClearAll();
			}
			
			virtual bool is_master_vis(int master_index, int x, int y) const 
			{ 
				return vmap.IsVis( master_index, x, y);
			}

			void old_load(CBufferInput * pInput);

			long type_id(void) const {return 502;}
			virtual const _TCHAR * type_str() const { return _T("MAP");};

			
			//virtual long check_res_point(int* x, int* y, trans::CTrans * tr) const;
			//virtual POINT get_res_point() const { return res_point;}
		};


		struct CMapCont //:public CCont<CMaster>
		{
			std::vector <CMap> items;
			CVisColors colors;

			typedef std::vector <CMap>::iterator iter;
			typedef std::vector <CMap>::const_iterator iterc;

			CMapCont() {items.resize(1);}

			virtual void ser(ser::CElement& e)
			{
				e.ser_vect(_T("map"), items);
			}

			void old_load(CBufferInput * pInput)
			{
				if (!items.size()) items.resize(1);

				items[0].old_load( pInput);
			}
		};
}

#endif