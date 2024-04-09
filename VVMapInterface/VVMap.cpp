#include "stdafx.h"
#include "VVMap.h"

//#include "ser_help/SerializeHelper.h"/

namespace masters
{
	
	CVisMap::CVisMap() :fw(0), fh(0), map(0), DoBring(false)
	{
	}


	CVisMap::~CVisMap()
	{
		release();
	}

	bool CVisMap::IsVis(int master_index, int x, int y) const 
	{
		if (!IsInside(x,y)) return false;
		
		return map[fw*y + x].visible(master_index) != 0;
	}


	void CVisMap::SetPoint(int master_index, int x, int y, bool value)
	{
		
		if (!IsInside(x,y)) return;

		int i = fw*y + x;
		
		if (value)
			map[i].add(master_index);
		else
			map[i].remove(master_index);
	}

	void CVisMap::SetPoint(int master_index, int x, int y, bool value, int p_size)
	{
		int d = p_size/2;
		for (int xi = -d; xi < d; ++xi)
		for (int yi = -d; yi < d; ++yi)
			SetPoint( master_index, x + xi, y + yi, value);
	}
		
	void CVisMap::ClearMaster(int master_index)
	{
		int x,y;
		for (x=0; x<fw; x++)
		for (y=0; y<fh; y++)
			map[fw*y + x].remove(master_index);
		
	}

	void CVisMap::ClearAll(void)
	{
		if (map)
			memset(map, 0, fw*fh*sizeof(TPointVisibility));
	}

	long CVisMap::SetSize(int w, int h)
	{
		if ((fw == w) && (fh == h) && (map))
										return 0;
		
		if (map) delete [] map;
		map = new TPointVisibility[w*h];
		
		if (!map) return ERR_OUT_OF_MEMORY;

		fw = w; fh = h;

		ClearAll();
		return 0;
	}


	void CVisMap::_FillMapRect(long *rx, long *ry, long m)
	{
		long minx = fw, maxx = 0, miny = fh, maxy = 0;
		
		for(int i=0;i<4;i++)
		{
			minx = (minx < rx[i]) ? minx : rx[i];
			miny = (miny < ry[i]) ? miny : ry[i];
			maxx = (maxx > rx[i]) ? maxx : rx[i];
			maxy = (maxy > ry[i]) ? maxy : ry[i];
		}
		if((minx == fw) || (miny==fh) || (maxx==0) || (maxy==0))return;

		if(minx<0)	minx = 0;	if(maxx>=fw)	maxx = fw-1;
		if(miny<0)	miny = 0;	if(maxy>=fh)	maxy = fh-1;

		//if((maxx-minx>200)||(maxy-miny>200))return;
		
		for(int ii = minx; ii<=maxx; ii++)
			for(int j=miny; j<=maxy; j++)
	
							map[j*fw+ii].add(m); 

			
	}
	///////////////////////////////////////////////////////////////////////



	bool CVisMap::BringToVis(int master_index, long *tx, long *ty) const
	{
	 // if (!DoBring) return false;

		DoBring = false;

		int new_x, new_y;

		if ((!tx)||(!ty)) return false;
		const int MAX_AREA_SEARCH = 300;
		for(int i=0; i<MAX_AREA_SEARCH; i++)
		{
			// т.к. область невидимости задана ломанной, 
			//первыми за пределы выйдут углы квадрата, 
			//поэтому делаем крест накрест.. х-образно
			
			
			new_x = *tx + i; new_y = *ty + i;
			
			if (IsInside(new_x, new_y))
				if (IsVis(master_index, new_x, new_y )) 
				{
					*tx = new_x; *ty = new_y;
					return true;
				}


			new_x = *tx - i; new_y = *ty + i;
			if (IsInside(new_x, new_y))
				if (IsVis(master_index, new_x, new_y )) 
				{
					*tx = new_x; *ty = new_y;
					return true;
				}
			
			new_x = *tx + i; new_y = *ty - i;
			if (IsInside(new_x, new_y))
				if (IsVis(master_index, new_x, new_y ))  
				{
					*tx = new_x; *ty = new_y;
					return true;
				}
				
			new_x = *tx - i; new_y = *ty - i;
			if (IsInside(new_x, new_y))
				if (IsVis(master_index, new_x, new_y ))  
				{
					*tx = new_x; *ty = new_y;
					return true;
				}
			}

		return false;
		
	}


	void CMap::old_load(CBufferInput * pInput)
	{
		CBufferInput& Input =  *pInput;
		Input.check("MAP");

	//	long L;
	//	double D;
		long w, h;
		Input.check("Height=");			Input.GetLong(&h);
		Input.check("Width=");			Input.GetLong(&w);

		set_size( w, h);

		/*

		if(Input.version>1)
		{
			double MapXScale = 0; 
			double MapYScale = 0;

			Input.check("XScale=");		Input.GetDouble(&MapXScale);
			Input.check("YScale=");		Input.GetDouble(&MapYScale);
		}

		if(Input.version>3)
		{
			long MapXOrig = 0;	long MapYOrig = 0;
			Input.check("XOrigin=");	Input.GetLong(&MapXOrig);
			Input.check("YOrigin=");	Input.GetLong(&MapYOrig);
			if(Input.version==4)
			{
				Input.check("Defining");

				Input.GetLong(&L);	
				Input.GetLong(&L);	
				Input.GetLong(&L);	
				Input.GetLong(&L);
			}
		}
		
		if(Input.version>10)
		{
			Input.check("Trans");
			for(int i=0; i<6; i++)
				//Input.GetDouble(MapTrans+i);
				Input.GetDouble(&D);
		}

		//NOTENOTE: we can use also a array of structs coz m_arrAlignMapPointNumbers.size() 
		//must be equal m_arrAlignMapPointCoords.size()
		//NOTENOTE: but since GetMapAlign & AlignMap use differnet array, it will be so
		//NOTENOTE: only the good use vectors, all the evil seems to be forever
		
		
			std::vector<long>		m_arrAlignMapPointNumbers;
			std::vector<long>		m_arrAlignMapPointCoords;

			//NOTENOTE: here we load sp - points for AlignMap
			SerializeHelper::AlignMapSaver	AMSaver;
			AMSaver.Load( Input.version, Input, m_arrAlignMapPointNumbers, m_arrAlignMapPointCoords );
		*/


		if (Input.find_down("AlignPoints")) 
								Input.Skip("AlignPoints");

		if(Input.version>7)
		{
			if(Input.find_down("BIH"))
			{
				Input.Skip("BIH");

				PBITMAPINFO PMap_bi = (PBITMAPINFO) (Input.Buffer + Input.Pointer);
				DWORD bmi_s = PMap_bi->bmiHeader.biSize;
				long im_size = PMap_bi->bmiHeader.biSizeImage;
				//Input.GetBuffer((PBYTE) &Map_bi,sizeof(Map_bi));
			
				Input.Skip(bmi_s);

				Input.check("BITS");
				if( Input.info->is_client() || Input.info->is_editor() )
				{
					//Input.GetBuffer( ib.ptr, Map_bi.biSizeImage);
					unsigned char * p = (unsigned char *) (Input.Buffer + Input.Pointer);
					set_image( *PMap_bi, p);
				}
				Input.Skip(im_size);
			}
			//else set_image( w, h);
		}
		else

			if(Input.find_down("Bitcount="))
			{
				long bpp; long im_size;

				Input.check("Bitcount=");	Input.GetLong(&bpp);
				
				Input.check("ImageSize=");	Input.GetLong(&im_size);
				
				Input.check("BITS");
				
				if ( Input.info->is_editor() || Input.info->is_client())
				{
					//zipper::CBuff ib (im_size);

					const byte * pbits = (byte *) (Input.Buffer + Input.Pointer);
					set_image( w, h, im_size, bpp, pbits);
				}
				Input.Skip(im_size);
				
			}
		//	else set_image( w, h);
	/////VISMAP////////////////////////////////////
		vmap.old_load( Input, w, h, get_bi_info());
	///////////////////////////////////////////////		
	}

////////////////////////////////////////////
	bool CVisMap::process_bm(const CDib& map_db, const CVisColors& clrs)
	{
		if (map_db.bits()) 
			vis_bm.assign(map_db);
		else
			vis_bm.init(fw, fh);

		int NM = clrs.get_num_of_master();

//		long bpl = vis_bm.bpl();// Map_bi.biSizeImage/Map_bi.biHeight;
//		long bpp = vis_bm.bpp();
		
//		int h = vis_bm.h();


		for(int j = 0; j < vis_bm.h(); j++)
		for(int i = 0; i < vis_bm.w(); i++)
		{
			int n=0;
			for(int m = 0; m < NM; m++)
			{
				if (IsVis(m,i,j))
				{
					vis_bm.set_pixel_alpha_up( i, j, clrs.get_color(m));
					n++;
				}
			}
			
			if(!n) vis_bm.set_pixel_alpha_up( i, j, clrs.get_color(NM));
		}
		return true;
	}

	void CVisMap::old_load(CBufferInput& Input, int width, int height, const PBITMAPINFO pbi)
	{
		if(Input.version>5)
		{
			if(Input.CheckString("VISMAP")!=WRONG_FORMAT)
			{
				SetSize( width, height);
				Input.GetBuffer( (byte*) map, bytes_size() );
			//	Input.Skip(bytes_size());
			}
		}

		if(Input.version>8)
		{
			if(Input.CheckString("PROCBITS")!=WRONG_FORMAT)
			{
				byte * pbits = (byte *) (Input.Buffer + Input.Pointer);
				vis_bm.init( *pbi, pbits );
				size_t s = width * height * sizeof(DWORD);
				Input.Skip(s);
			}
		}
	}
	

}
