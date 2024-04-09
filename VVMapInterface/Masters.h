#ifndef _MASTERS_H_included
#define _MASTERS_H_included


#include "serializer3.h"
#include "BufferInput.h"

#include "extents.h"

#include "frames.h"
#include "dib.h"

namespace masters
{
	/*
		class CIZCont
		{
			std::vector <CPoly> zones;
		public:
			push

		};
		*/

	//	class CExtentsFrame;
	//	class CPicsFrame;

//-------------------------------------------------------------------------------
		class CFrame2D_pic :public CFrame2D
		{
		protected:
			CDib dib;
			int int_width;	int int_height;
			// дублируется на случай, если картинка не загружена
		
		protected:
			void set_size( int w, int h) { int_width = w;	int_height = h; }
			//size_t image_size()

			virtual std::tstring gen_dib_fname() const
			{
				const size_t s = 30;	_TCHAR buff[s];
				_sntprintf( buff, s, _T("images/master_%i.dib"), getID());
				return std::tstring(buff);
			}

		public:

			

			const CDib& the_dib() const 
			{ 
				return dib;
			}
			CFrame2D_pic() :int_width(0),	int_height(0) {}
			virtual ~CFrame2D_pic() {}
			
			virtual int width()		const {return int_width;}
			virtual int height()	const {return int_height;}

			virtual unsigned get_pixel(int x, int y) const { return dib.get_pixel_index(x,y);}
				
			virtual bool set_image(const BITMAPINFO& bi, const PBYTE src)
			{
				int_width = 0;		int_height = 0;
				if (!dib.init(bi, src)) return false;
				int_width = dib.w();		int_height = dib.h();
				return true;
			}
			
			virtual bool set_image(int w, int h) 
			{ 
				int_width = w; int_height = h; 
				return dib.init( w, h);
			}
			
			virtual bool set_image(int w, int h, size_t im_size, int bpp, const unsigned char * src)
			{
				int_width = w;		int_height = w;
				if (!dib.init( w, h, im_size, bpp, src)) return false;
				int_width = dib.w();		int_height = dib.h();
				
				return true;
			}

			virtual const PBITMAPINFO get_bi_info() { return dib.get_bi_header();}
			virtual const PBYTE get_bits() { return dib.bits();}
		
			virtual void ser(ser::CElement& e)
			{
				if (!e.is_loading())	
				{
					std::tstring st (gen_dib_fname());
					dib.get_image_fname (st.c_str());
				}
								
				CFrame2D::ser(e);

				e.attr(_T("width"), int_width);
				e.attr(_T("height"), int_height);
				
				try
				{
					bool load_or_not = true;

					if ((type_id() == 501) && !e.sdata.saving_masters) 
							load_or_not = false;

					if ((type_id() == 502) && !e.sdata.saving_map) 
							load_or_not = false;

					if (load_or_not && int_width && int_height)
										e.ser_obj( _T("dib"), dib);
				}
				catch (ser::no_file&)
				{
					MyTrace ("CDib::ser caught no_file");//clear_dib();
					// давим ser::no_file
				}
			}
			
	};
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
	class CMaster :public CFrame2D_pic
	{
	public:
		CExtentCont extents;

		CMaster(){}
		
		long compile_extents()
		{
			return extents.compile( width(), height() );
		}

		virtual void ser(ser::CElement& e)
		{
			CFrame2D_pic::ser(e);
			try
			{ 
				if (!e.is_loading())	extents.set_mastert_id(getID());
				if (e.sdata.saving_extents)
					e.ser_obj( _T("Extents"), extents); 
			}
			catch (... ) {}
		}

		void old_load( CBufferInput& Input);
		
		enum {type_id_num = 501};

		
		virtual long type_id() const { return type_id_num;}
		virtual const _TCHAR * type_str() const { return _T("MASTER");};
		

	};

//-------------------------------------------------------------------------------
	struct CMastersCont
	{
		std::vector<CMaster> items;

		typedef std::vector<CMaster>::iterator iter;
		typedef std::vector<CMaster>::const_iterator iterc;

		virtual void ser(ser::CElement& e)
		{
			//e.sdata.saving_extents = m_extents_enabled;
			e.ser_vect( _T("master"), items);
		}

		void old_load(CBufferInput& Input)
		{
			
			for (iter i = items.begin(); i != items.end(); ++i)
				i->old_load(Input);
		}

		void old_read_count(CBufferInput& Input)
		{
			long L = -1;
			Input.Pointer++;
			Input.check("Masters=");
			Input.GetLong(&L);
			set_count(L);
		}

		size_t Count() const { return items.size();}
		void set_count(size_t _count) 
		{ 
			c_changed = false;
			if (_count != Count()) 
			{
				c_changed = true; 
				items.clear();
				items.resize(_count);
			}
		}

		bool is_indx_ok(int i) const { return (i>=0) && (i < (int)Count()); }
		bool is_masters() const { return !items.empty();}
		
		bool count_changed() const { return c_changed;}

		void on_compile() 
		{  
			//compile_extents();
			c_changed = false; 
		}

		void compile_extents()
		{
			for ( iter i = items.begin(); i != items.end(); ++i)
			{
				i->compile_extents();
			}
		}

		CMastersCont() : c_changed(true) {}
	private:
		bool c_changed;

	
	};
//-------------------------------------------------------------------------------
}

#endif