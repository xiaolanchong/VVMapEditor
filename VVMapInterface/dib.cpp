
#include "stdafx.h"
#include "dib.h"

#include "memleak.h"
		//int images_saved = 0;
namespace masters
{

		CDib::CDib() :b_bits(0) 
		{
			memset(&b_info, 0, sizeof(b_info));
		//	b_info.bmiHeader.biSize = sizeof(b_info.bmiHeader);
		}

		CDib::CDib(const BITMAPINFO& bi) :b_bits(0) { init(bi);}
		
		CDib::CDib(const CDib& dib) :b_bits(0)
		{
			//memset(&b_info, 0, sizeof(b_info));
			assign(dib);
		}

		void CDib::operator= (const CDib& dib)
		{
			assign(dib);
			return;
		}

		void CDib::assign(const CDib& dib)
		{
			clear_dib();
			if (!dib.ready()) return;
			init (dib.b_info, dib.b_bits);
		}

		CDib::~CDib() 
		{	
			clear_dib();
		}

		void CDib::clear_dib() 
		{
			if (b_bits) 
					delete (b_bits);	
			b_bits = 0;
			memset(&b_info, 0, sizeof(b_info));
			b_info.bmiHeader.biSize = sizeof(b_info.bmiHeader);
			save_buff.release();
		}

		bool CDib::init(const BITMAPINFO& bi, const byte * src)
		{
			try
			{
				if (b_bits) clear_dib();
				memcpy (&b_info, &bi, sizeof(b_info));
				
				size_t s = image_size();
			
				b_bits = new BYTE [s];
				if (!b_bits) throw;
				if (src) memcpy(b_bits, src, s);
				else memset(b_bits, 0, s);
				
				return true;
			}
			catch (...)
			{
				return false;
			}//*/
		}

		bool CDib::init(int w, int h)
		{
			try
			{
				clear_dib();
				
				size_t s = w*h*sizeof(DWORD);

				b_info.bmiHeader.biWidth = w;
				b_info.bmiHeader.biHeight = h;
				b_info.bmiHeader.biPlanes = 1;
				b_info.bmiHeader.biCompression = BI_RGB;
				b_info.bmiHeader.biSizeImage = s;
				b_info.bmiHeader.biBitCount = 32;
			
				b_bits = new BYTE [s];
				if (!b_bits) throw;
				memset(b_bits, 0xff, s);
				
				return true;
			}
			catch (...)
			{
				return false;
			}
		}

		bool CDib::init(int w, int h, size_t im_size, int bpp, const byte * src)
		{
			try
			{
				clear_dib();
				
				b_info.bmiHeader.biWidth = w;
				b_info.bmiHeader.biHeight = h;
				b_info.bmiHeader.biPlanes = 1;
				b_info.bmiHeader.biCompression = BI_RGB;
				b_info.bmiHeader.biSizeImage = im_size;
		
				b_info.bmiHeader.biBitCount = WORD(bpp);
			
				b_bits = new BYTE [im_size];
				if (!b_bits) throw;
				memcpy(b_bits, src, im_size);
				
				return true;
			}
			catch (...)
			{
				return false;
			}
		}

		const PBITMAPINFO CDib::get_bi_header(void) const 
		{ 
			return (PBITMAPINFO) &b_info;
		}
									

		void CDib::ser(ser::CElement& e)
		{

			e.attr ( _T("image_file_name"), fname);
			
			try {
				zipper::CBuff bu;
				if (e.is_loading())
				{
					clear_dib();
					
					e.ser_bin( sys::ANSI_str(fname.c_str()).c_str(), &bu);
					
					init_from_buff(bu);
				}
				else
				{
					MakeBuff();

					e.ser_bin(  sys::ANSI_str(fname.c_str()).c_str(), &save_buff);
					save_buff.release();
				}
			}
			catch (ser::no_file)
			{
				MyTrace ("CDib::ser caught no_file");//clear_dib();
			}

		}

		unsigned CDib::get_pixel_index(int x, int y) const
		{ 
			if ( (x<0) || (x>=w()) || (y<0) || (y>=h()) ) return 0;

			int Delta = (w() * bpp() + 31) / 32 * 4;
			BYTE * pPixel = b_bits + y * Delta;

			switch (bpp())
			{
			case 24: 
				pPixel += x*3;
				return (pPixel[0]) | (pPixel[1] << 8) | (pPixel[2] << 16);
			case 32:
				return (unsigned)  ((DWORD *)pPixel)[x];
			}
			
			return 0;
		}

		void CDib::set_pixel(int x, int y, unsigned value)
		{
			if ( (x<0) || (x>=w()) || (y<0) || (y>=h()) ) return;

			int Delta = (w() * bpp() + 31) / 32 * 4;
			BYTE * pPixel = b_bits + y * Delta;

			switch (bpp())
			{
			case 24: 
				pPixel += x*3;
				pPixel[0] = BYTE(value & 0xff);
				pPixel[1] = BYTE((value & 0xff00)   >> 8); 
				pPixel[2] = BYTE((value & 0xff0000) >> 16);

				return;
			case 32:
				((DWORD *)pPixel)[x] = (DWORD) value;
				return;
			}
		}

		void CDib::set_pixel_up(int x, int y, unsigned value)
		{
			set_pixel( x, h()-1-y, value);
		}

		inline unsigned char apply_alpha( unsigned c1, unsigned c2, unsigned char alpha)
		{
			return (unsigned char)((( (c1 & 0xff) *alpha + (0xff - alpha) * (c2 & 0xff)) >> 8) & 0xff);
		}

		void CDib::set_pixel_alpha	(int x, int y, unsigned value)
		{
			unsigned alpha = (value >> 24) & 0xff;

			unsigned c0 = get_pixel_index( x, y);
			unsigned c1 = apply_alpha( value, c0, (unsigned char)(alpha));
			unsigned c2 = apply_alpha( value >> 8,  c0 >>  8, (unsigned char)(alpha));
			unsigned c3 = apply_alpha( value >> 16, c0 >> 16, (unsigned char)(alpha));
			set_pixel( x, y, (c3 << 16) + (c2 << 8) + c1);
		}
		void CDib::set_pixel_alpha_up	(int x, int y, unsigned value)
		{
			set_pixel_alpha ( x, h()-1-y, value);
		}

		void CDib::MakeBuff()
		{
			save_buff.release();
			save_buff.copy_from((void*) &b_info, sizeof(b_info));
			
			zipper::CAutoBuff b2;
			b2.copy_from( bits(), image_size());
			
			save_buff.concat( b2);
		}

		void CDib::init_from_buff(const zipper::CBuff& buff)
		{
			const size_t s = sizeof(b_info);
			PBITMAPINFO bi = (PBITMAPINFO) buff.ptr;
			init(*bi, buff.ptr + s);
		}

		void CDib::get_image_fname(const _TCHAR * _fname) 
		{
			fname = std::tstring (_fname);
		}
		
		bool CDib::is_all_black() const 
		{ 
			size_t is = image_size();
			for (size_t i=0; i < is; ++i)
				if (!b_bits[i]) return false;
			
			return true;
		}

}