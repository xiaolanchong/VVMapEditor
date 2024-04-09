// VVMOptions.cpp: implementation of the CVVMOptions class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "old_classes.h"
#include "memleak.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace trans
{

	CVVMOptions::CVVMOptions():
		MAXBASIS(3),
		log_win(false),
		bas(3), maxbas(11), ppf(5), UseSuper(1)
	{
		use3DAlign = false;	type = trProjective;
		DoAdj = false;
		RadOfAdj = 10;

		UseWT = false;
		UseProj = true;
		WT_inside = true;

		UseNeural = false;
		Neur_inside = true;

		DrawMap = false;
		col_lim = 128;
		
	}

	CVVMOptions::~CVVMOptions()
	{

	}

	long read_long( const char ** pbuff)
	{	
		while (! **pbuff) (*pbuff)++;

		unsigned len = strlen(*pbuff);

		long res = atol(*pbuff);	
		
		(*pbuff) += len + 1;

		return res;
	
	}

	double read_double( const char ** pbuff)
	{	
		while (! **pbuff) (*pbuff)++;

		unsigned len = strlen(*pbuff);

		double res = atof(*pbuff);	
		
		(*pbuff) += len + 1;

		return res;
	
	}

	long CVVMOptions::old_load(const char *buf)

	{
		const char * pbuf = buf;
		long data_len = read_long(&pbuf);	
		
		type = (TransType) read_long(&pbuf);
		//Îïöèè SuperTransition
			
		UseSuper = (read_long(&pbuf) != 0);//1
		//
		bas = read_long(&pbuf);//2

		maxbas = read_long(&pbuf); //3

		ppf = read_double(&pbuf);//4
		
		/////Neural options

		LearnRatio = read_double(&pbuf);//5
		
		////Support Points OPTIONS ////////////////////////////////////////////////
		use3DAlign = (read_long(&pbuf) != 0);
			
		////////////////////////////////////////////////////////
		DoAdj = (read_long(&pbuf) != 0 );

		RadOfAdj = read_double(&pbuf);

		return data_len;				
	
	}


	#define _READLONG(var) {k=0;while(buf[k++]!=' ');CopyMemory(tmp,buf,k);tmp[k]=0;var=sys::my_atoi(tmp);buf+=k;}
	#define _READREAL(var) {k=0;while(buf[k++]!=' ');CopyMemory(tmp,buf,k);tmp[k]=0;var=atof(tmp);buf+=k;}
	long CVVMOptions::old_old_load(const char *pbuf)
	{
			
		long k=0;
		long L;
		const char * buf = pbuf;
		char tmp[20];
		const char * s=buf;
		_READLONG(bas);
		_READLONG(maxbas);
		_READREAL(ppf);
		_READLONG(L);	UseSuper = (L != 0);
/*
		_READLONG(ll);
		_READLONG(ll);
		_READREAL(dd);
		_READLONG(ll);
*/
		return buf-s;
	}
	#undef _READREAL
	#undef _READLONG

	CVVMOptions& CVVMOptions::operator= (const CVVMOptions& o)
	{
		memcpy (this, &o, sizeof(CVVMOptions));
		return *this;
	}


}