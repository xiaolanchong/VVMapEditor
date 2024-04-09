#include "stdafx.h"

#include "VVMReal1.h"
//////////////////////////////////////////////////////////////////////////////////////




long CVVMReal::old_Load(void *bits, long N)
{
	//Options=VVMINT_MASTER;
	sys::CEnterCS cs_obj (CritSect);;

	try
	{

	CBufferInput Input((char*)bits,N, &info);

	

	Input.Buffer	=	(char*) bits;
	Input.Size		=	N;
	Input.Pointer	=	0;			


//	long L;
//	long version;

	//// версии:
	//		1			Ваще без опорных точек, зон и тыпы
	//		2			Сохраняет опорные точки и темные зоны
	//		3			Сохраняет координаты камер и опорных точек на камерах
	//		4			Сохраняет координаты origin на карте и точки SP_def
	//		5			Новая версия сохраняет список RP
	//		6			Сохраняет цвета для генерации карты видимости и VisMap
	//		7			Сохраняет суперпреобразования при помощи Serialize
	//		8			Исправлен баг при сохранении Map_bi
	//		9			Сохраняет карту видимости (не то она генерится долго)
	//		10			Исправлен баг сохранения SuperTransition
	//		11			Сохраняет преобразование карта-физические и настройки
	//		12			heuz : теперь сохраним и настройки координатной сетки
	//		13			heuz : Теперь сжимаем данные
	//		14			Сохраняем 3D align-преобразования (АГ)
	////	15			сохраняем опции по-новому
	//		16			для каждого CTransition отдельные опции

	Input.check("VVMAPInterface saved info. v");

 	Input.GetLong( &(Input.version));

	if(Input.version<0) return(ERR_INVALID_FORMAT);

/////Masters count//////////////////////////
	p_src_data->masters_cont.old_read_count( Input);
///////////////////////////////////////////

/////////////////Masters colors////////////////////////////
	p_src_data->maps_cont.colors.old_load( Input, p_src_data->masters_cont.Count());


///////////////////////////Masters////////////////////////////
	p_src_data->masters_cont.old_load(Input);
//////////////////////////////////////////////////////////////

/////////////////MAP Align///////////////////////////
	sp.old_load_map_align(Input);
////////////////////////////////////////////////////

	
/////////MAP////////////	
	p_src_data->maps_cont.old_load( &Input);
	
/////////POINTS//////////
	p_src_data->points_cont.set_master_count( p_src_data->masters_cont.Count() );
	p_src_data->points_cont.old_load( &Input);
//////////////////////////////////////////////////////////////	
//////////пропускаем все преобразования нах///////////////////
//////////////////////////////////////////////////////////////
	if (Input.version>1) 
	{
		Input.goto_text("Support points=");
		sp.old_load(Input);

		Input.goto_text("Visibility zones=");
		p_src_data->iz_cont.old_load(Input);
	}
	
	
//////////////////////////////////////////////////////////////	
	p_trans_cont->reserve_transes(true);

	/*
	//-----------

	if(Input.version>=14)
	{
		

		__CHECK("3D Align=");
		Input.GetLong( (long*) &DefaultOpts.use3DAlign);
		
		int i;
		for (i=0;i<8;i++)
			Input.GetDouble( &MapTrans3D[i] );
		for (i=0;i<9;i++)
			Input.GetDouble( &MapTrans3DBack[i] );
	}

	//----------
	*/
	

	p_trans_cont->load_old_opts(Input);

	
	//-----------
	/*
	if(Input.version>=16) 
	{
		for (i=0; i<CompNum; i++)
		{
			L = Comp[i].Opts.LoadSave((char*)Input.Buffer+Input.Pointer,0, false);
			
			Input.Pointer+=L;
			Input.skip();
		}
	}
	*/	
	//---------------
	/*	for (i=0; i<CompNum; i++)
		{
			if ((Comp[i].il<0) || (Comp[i].il == NumOfMasters))		Comp[i].l_id = -1; 
			else		Comp[i].l_id = Masters[Comp[i].il].ID; 
				
			if ((Comp[i].ir<0) || (Comp[i].ir == NumOfMasters))		Comp[i].r_id = -1; 
			else		Comp[i].r_id = Masters[Comp[i].ir].ID;
		}
	*/
	//---------------

//	AllPointsChanged(false);
//	FirstCompile = true;
		p_trans_cont->shadow_compile_trans();
	
	}
	catch (std::string s)
	{
		err_msg(s.c_str());
		//return ERR_INVALID_FORMAT;
		throw ser::no_tag();
	}

	return 0;
}
