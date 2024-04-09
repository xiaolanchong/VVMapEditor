
//#include "dib.h"
#include "stdafx.h"
#include "Masters.h"
#include "VVMErrors.h"

#include "memleak.h"

namespace masters
{

/*
	long CFrame2D::check_res_point(int* x, int* y, trans::CTrans * tr) const
	{
		if (!inside( *x, *y)) return ERR_POINT_OUT_OF_RECT;
		if (!is_vis( *x, *y)) return ERR_POINT_INVISIBLE;
		return 0;
	}
*/
	void CMaster::old_load(CBufferInput& Input)
	{
		long L;
		Input.check("Master#=");	Input.GetLong(&L);
		Input.check("ID=");			Input.GetLong(&L);		setID(L);

		long ww, hh;
		Input.check("Height=");		Input.GetLong(&hh);
		Input.check("Width=");		Input.GetLong(&ww);

		int_width = ww; int_height = hh;

		if(Input.version>2)
		{
			long _SP_Num;	
			Input.check("Anchor=");		Input.GetLong(&_SP_Num);
		}
		
		extents.old_load( Input, ww, hh);
					
		if(Input.CheckString("BIH")!=WRONG_FORMAT)
		{
			BITMAPINFO bi;
			memset(&(bi.bmiColors), 0, sizeof(bi.bmiColors));
			Input.GetBuffer((PBYTE) &(bi.bmiHeader),sizeof(bi.bmiHeader));
			if (Input.CheckString("BITS")!=WRONG_FORMAT)
			{
				if (Input.info->is_editor())
				{
					dib.init( bi );
						
					Input.GetBuffer(dib.bits(), dib.image_size());
				}
				else
				{
					Input.Skip(bi.bmiHeader.biSizeImage);
				}						
			}
		}
	}


}