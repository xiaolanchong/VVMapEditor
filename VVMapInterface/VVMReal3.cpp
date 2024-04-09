#include "stdafx.h"

#include "VVMErrors.h"
#include "VVMReal1.h"

#pragma warning (disable : 4786)

	const char main_file[] = "data/main.xml";
	const char trans_file[] = "data/transitions.xml";
	const char sp_file[] = "data/sp.xml";
	const char szMainFile[]	= "Main.vme";

	extern const char * map_file_name;

	void CVVMReal::SaveExternMapImage()
	{
		const void * map_image = 0;	size_t image_size = 0;
		const void * dummy1 = 0;	size_t dummy2 = 0;

		if ((GetMapBuffer( &map_image, &image_size ) == ERR_NO_ERROR) 
			&& !zippr.get_buff( map_file_name, &dummy1, &dummy2 ))
		{
			PutMapBuffer ( map_image, image_size);
		}
	}

	long CVVMReal::Save(void **pbits, long *pN, BOOL Compact)
	{

		try{		
			ser::CElement e ( std::string(main_file), &zippr);

			if (Compact)
			{
				e.sdata.saving_map		= false;
				e.sdata.saving_masters	= false;
				e.sdata.saving_vismap	= false;
			}
			else
			{
				e.sdata.saving_map		=	p_src_data->saving_map;
				e.sdata.saving_masters	=	p_src_data->saving_masters;
				e.sdata.saving_vismap	=	p_src_data->vismap_enabled;

				SaveExternMapImage();
				
			}

			e.ser_obj( _T("CVVMReal"), *this);

			e.Close();

			m_SaveBuffer.release();
			zippr.ZipTo(m_SaveBuffer);
		
			*pbits	= m_SaveBuffer.ptr;
			*pN		= m_SaveBuffer.size;
			zippr.map_clear();
			return ERR_OK;
		}
		catch (...)
		{
			zippr.map_clear();
			return ERR_UNKNOWN;
		}
	}



	long CVVMReal::Load(void *bits, long N)
	{
		try{
			const void * p = 0;	size_t s = 0;
			
			Init_All();

			unzippr.map_clear();
			zipper::CBuff lbuff(bits, N);
		//	zipper::CAutoBuff unzipped_buff;
			//m_SaveBuffer.assign( bits, N);
			
			bool is_zip = unzippr.UnzipFrom( lbuff, szMainFile);
			bool is_old = unzippr.get_buff( szMainFile, &p, &s);

			if (!is_old && !is_zip)
			{
				MyTrace ("File not old and not packed!");
				throw ser::no_file("not old and not zipped");//никогда не случится
			}

			if (is_old) 
				old_Load( (void*) p, s);
			else
			{
				ser::CElement e (std::string(main_file), &unzippr);
				e.sdata.saving_extents = m_enable_extents;
				e.sdata.saving_vismap = m_enable_vismap; 
				e.sdata.saving_masters = m_enable_master_image; 
				e.sdata.saving_map = m_enable_map_image;
				e.ser_obj( _T("CVVMReal"), *this);

			}
			// p_trans_cont->reserve_transes(true);
			return ERR_OK;
		}
		catch (ser::no_tag)
		{
			Init_All();
			unzippr.map_clear();
			return ERR_INVALID_FORMAT;
		}
		
		catch (ser::no_file& e)
		{
			Init_All();
			unzippr.map_clear();
			MyTrace ("VVMInterface::Load caught ser::no_file " + std::string(e.what()));
			return ERR_FILE_NOT_FOUND_IN_ZIP;
		}
		catch (std::bad_alloc)
		{
			unzippr.map_clear();
			return ERR_OUT_OF_MEMORY;
		}
		catch (...)
		{
 			Init_All();
			unzippr.map_clear();
			return ERR_UNKNOWN;
		}
	}

	//--------------------------------------------------------

	struct sp_dummy
	{
		supp_points::sp_man& sp;
		
		sp_dummy(supp_points::sp_man& _sp) :sp(_sp) {}
		
		void ser(ser::CElement& e)
		{
			e.ser_obj( _T("SP_data"), sp);
		}
	};

	void CVVMReal::ser(ser::CElement& e)
	{
		//e.sdata.saving_extents = m_enable_extents;
		e.ser_obj( _T("SOURCE_DATA") , *p_src_data);
		
		try 
		{
			ser::CElement ee (std::string(trans_file), e);
			ee.ser_obj( _T("CVVMREAL_TRANSITIONS"), *p_trans_cont);
			ee.Close();
		}
		catch (ser::no_file& e) 
		{
			MyTrace ("CVVMReal::ser caught ser::no_file " + std::string(e.what()) + " - transitions not loaded");
		}

		try 
		{
			sp_dummy sd(sp);
			ser::CElement ee (std::string(sp_file), e);
			ee.ser_obj( _T("SP"), sd);
			ee.Close();
		}
		catch (ser::no_file& e) 
		{
			MyTrace ("CVVMReal::ser caught ser::no_file " + std::string(e.what()) + " - supports points not loaded");
		}
	}

	void CVVMReal::bind_opts(opts::KDialog * d, opts::Itm * i)
	{
		std::tstring s (_T("opts"));
		opts::Itm * i0 = d->text( i, IDC_DIALOG_CAPTION, &s);
		
		//d->bind( i, IDC_VISPOLY_ENABLED, &vispoly_enabled);
		p_src_data->bind_opts( d, i0);
		p_trans_cont->bind_opts( d, i0);
	}
