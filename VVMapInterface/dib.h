//#pragma once

#ifndef _DIB_H_INCLUDED_568756875
#define _DIB_H_INCLUDED_568756875

#include <windows.h>
#include "serializer3.h"
#include "bufferinput.h"

namespace masters
{

	class CDib
	{
		BITMAPINFO b_info;
		LPBYTE b_bits;
		std::tstring fname;

		zipper::CBuff save_buff;
	public:
		CDib();

		CDib(const BITMAPINFO& bi);
		CDib(const CDib& dib);

		void assign(const CDib& dib);
		void operator= (const CDib& dib);

		virtual ~CDib();

		void clear_dib();

		bool init(const BITMAPINFO& bi, const byte * src = 0);
		bool init(int w, int h);
		bool init(int w, int h, size_t im_size, int bpp, const byte * src);

		const PBITMAPINFO get_bi_header(void) const ;
									
		inline long w()				const { return b_info.bmiHeader.biWidth;		}
		inline long h()				const { return b_info.bmiHeader.biHeight;		}
		inline long bpl()			const 
		{ return (b_info.bmiHeader.biSizeImage/b_info.bmiHeader.biHeight);	}
		inline long bytes_pp()		const { return b_info.bmiHeader.biBitCount/8;	}
		inline long bpp()			const { return b_info.bmiHeader.biBitCount;		}
		inline long image_size()	const { return b_info.bmiHeader.biSizeImage;	}
		inline PBYTE bits()			const 
		{ 
			return b_bits;				
		}

		bool empty() const 
		{ 
			return !b_bits || !w() || !h();
		}
		virtual void ser(ser::CElement& e);

		unsigned get_pixel_index(int x, int y) const;
		
		void set_pixel			(int x, int y, unsigned value);
		void set_pixel_up		(int x, int y, unsigned value);
		void set_pixel_alpha	(int x, int y, unsigned value);
		void set_pixel_alpha_up	(int x, int y, unsigned value);
		
		void MakeBuff();

		void init_from_buff(const zipper::CBuff& buff);

		void get_image_fname(const _TCHAR * _fname);

		bool ready() const { return b_info.bmiHeader.biSize != 0;}
		bool is_all_black() const;

		
	};
}

#endif

