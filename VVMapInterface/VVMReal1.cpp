// VVMReal1.cpp: implementation of the CVVMReal class.
//
//////////////////////////////////////////////////


#include "stdafx.h"
//NOTENOTE: we move it to res
#include "res/resource.h"
#include "VVMInterface.h"
#include "Buffer.h"



#include "VVMReal1.h"
#include "VVMap.h"

#include "../SRS/sourcelibrary/fourier/FourierTransformer.h"

#include "Bufferinput.h"
#include "../MultiCamGlobals/messages.h"

#include "serializer3.h"



#include "VVMInterface.h"

#include <math.h>
#include "arithmetic.h"

#include "opts_base.h"
#include "ModuleVersion.h"


#ifdef _DEBUG
	#include <assert.h>
#endif

#include "memleak.h"

#define VVMINT_EDITOR		1
#define VVMINT_COMMUTATOR	2
#define VVMINT_CLIENT		3
#define VVMINT_MASTER		4

#define DEF_INTERFACE_VER_STR( MajorVer, MinorVer, SubVer)	#MajorVer "." #MinorVer "." #SubVer

#define	INTERFACE_VER		DEF_INTERFACE_VER_STR( 4 , 2 , 3.0 )

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVVMReal::CVVMReal(long Opt):
	info(Opt), p_src_data(0), p_trans_cont(0)
		
{
	m_enable_extents = true;
	m_enable_vismap = true; 
	m_enable_master_image = true; 
	m_enable_map_image = true;

	Init_All();
}

void CVVMReal::Init_All()
{
	Delete_All();
	p_src_data = new KSrcDataContext(&info);
	p_trans_cont = new trans::CTransCont(*p_src_data);
	
}

extern int cbuff_count;

CVVMReal::~CVVMReal()
{	
	Delete_All();
}

long CVVMReal::SetNumOfMasters(long N)
{
	//if(Options!=VVMINT_EDITOR)return ERR_WRONG_OPTION;
	if(N<=0)return ERR_INVALID_MASTERS;
	if(N>MAX_MASTERS)return ERR_INVALID_MASTERS;
	
	sys::CEnterCS cs_obj (CritSect);

	p_src_data->masters_cont.set_count(N);
	p_src_data->points_cont.set_master_count(N);
	p_src_data->get_map()->clear_vismap();
	
	p_trans_cont->reserve_transes(true);
	
	return 0;
}

long CVVMReal::GetNumOfMasters(long *pN)
{
	if (!pN) return ERR_BAD_ADDRESS;

	sys::CEnterCS cs_obj (CritSect);

	long res = (long) p_src_data->masters_cont.Count();
	*pN = res;
	
	if (!res) return ERR_MASTERS_UNDEFINED;
	return 0;
}

long CVVMReal::SetMasterID(long i, long ID)
{
	sys::CEnterCS cs_obj(CritSect);
	
	if (!info.can_edit()) return ERR_WRONG_OPTION;
	// (Options!=VVMINT_EDITOR&&Options!=VVMINT_COMMUTATOR) 
	if (!p_src_data->masters_cont.is_masters()) return ERR_INVALID_MASTERS;
	if (!p_src_data->masters_cont.is_indx_ok(i)) return ERR_INVALID_INDEX;
	
	p_src_data->masters_cont.items[i].setID(ID);
	p_trans_cont->reserve_transes();

	return 0;
}

long CVVMReal::GetMasterID(long i, long *pID)
{
	if (!p_src_data->masters_cont.is_masters()) return ERR_INVALID_MASTERS;
	if (!p_src_data->masters_cont.is_indx_ok(i)) return ERR_INVALID_INDEX;
	
	if (!pID) return ERR_BAD_ADDRESS;
	
	sys::CEnterCS cs_obj (CritSect);
	
	*pID = p_src_data->masters_cont.items[i].getID();

	return 0;
}

long CVVMReal::SetMasterBitmap(long i, PBITMAPINFO pbi, PBYTE bits)
{
	if (!info.is_editor())		return ERR_WRONG_OPTION;
	if (!p_src_data->masters_cont.is_masters())	return ERR_INVALID_MASTERS;
	if (!p_src_data->masters_cont.is_indx_ok(i)) return ERR_INVALID_INDEX;
	
	if ( !pbi || !bits)			return ERR_BAD_ADDRESS;
	if ((pbi->bmiHeader.biBitCount!=24) && (pbi->bmiHeader.biBitCount!=32))
								return ERR_INVALID_BITMAP;

	sys::CEnterCS cs_obj (CritSect);
	
	bool res = p_src_data->masters_cont.items[i].set_image( *pbi, bits);
	if (!res) return ERR_OUT_OF_MEMORY;

	return 0;
}

long CVVMReal::GetMasterBitmap(long i, PBITMAPINFO *ppbi, PBYTE *pbits)
{
	if (!info.is_editor())			return ERR_WRONG_OPTION;
	if (!p_src_data->masters_cont.is_masters())		return ERR_INVALID_MASTERS;
	if (!p_src_data->masters_cont.is_indx_ok(i))		return ERR_INVALID_INDEX;
	if (!ppbi || !pbits)			return ERR_BAD_ADDRESS;

	*ppbi = 0;	*pbits = 0;

	sys::CEnterCS cs_obj (CritSect);

	if (p_src_data->masters_cont.items[i].the_dib().empty()) return ERR_NO_ERROR;

	*ppbi = p_src_data->masters_cont.items[i].get_bi_info();
	*pbits = p_src_data->masters_cont.items[i].get_bits();

	return 0;
}

long CVVMReal::SetMapBitmap(PBITMAPINFO pbi, LPBYTE bits)
{
	if (!info.is_editor())		return ERR_WRONG_OPTION;
	if (pbi==NULL||bits==NULL)	return ERR_BAD_ADDRESS;
	if (pbi->bmiHeader.biBitCount!=24&&pbi->bmiHeader.biBitCount!=32)return ERR_INVALID_BITMAP;

	sys::CEnterCS cs_obj (CritSect);

	//Если изменились размеры карты - обнулить карту видимости
	//Вообще-то надо в любом случае обнулять, но editor зачем-то вызывает
	//SetMapBitmap постоянно, так что не получается.

	bool res = p_src_data->maps_cont.items[0].set_image( *pbi, bits);
	if (!res) return ERR_OUT_OF_MEMORY;

//	SetMapSize( pbi->bmiHeader.biWidth, pbi->bmiHeader.biHeight );

	return 0;
}

long CVVMReal::GetMapBitmap(PBITMAPINFO *ppbi, LPBYTE *pbits)
{
	//if(Options!=VVMINT_EDITOR&&Options!=VVMINT_CLIENT)return ERR_WRONG_OPTION;

	if(ppbi==NULL||pbits==NULL)return ERR_BAD_ADDRESS;

	*ppbi = 0;		*pbits = 0;
	
	sys::CEnterCS cs_obj(CritSect);

	if (p_src_data->maps_cont.items[0].the_dib().empty()) return ERR_NO_ERROR;

	*ppbi  = p_src_data->maps_cont.items[0].get_bi_info();
	*pbits = p_src_data->maps_cont.items[0].get_bits();

	return ERR_NO_ERROR;
}

long CVVMReal::SetPointList(long N, long *arr)
{
	sys::CEnterCS cs_obj (CritSect);
	if (!p_src_data) return 0;
	
	if (!info.is_editor())	return ERR_WRONG_OPTION;

	if (!p_src_data->masters_cont.Count())	return ERR_INVALID_MASTERS;
	if (!arr)				return ERR_BAD_ADDRESS;
	if (N<0)				return ERR_INVALID_SIZE;

	
	p_src_data->points_cont.config( p_src_data->masters_cont.Count(), N);
	p_src_data->points_cont.copy_from( arr, N*(p_src_data->masters_cont.Count() + 1)*2);
	p_trans_cont->reserve_transes();
	
	return 0;

}

long CVVMReal::GetPointList(long *pN, long **parr)
{
	//if (Options!=VVMINT_EDITOR)return ERR_WRONG_OPTION;
	if (!p_src_data->masters_cont.Count())return ERR_INVALID_MASTERS;
	if (!parr || !pN)	return ERR_BAD_ADDRESS;
	
	sys::CEnterCS cs_obj (CritSect);

	
		
	const KPOINT * p = 0;
	p_src_data->points_cont.get_buff( &p);

	const long * _parr = reinterpret_cast <const long *> (p);
	

	* pN = p_src_data->points_cont.PointsCount();
	* parr = const_cast <long *> (_parr);

	return 0;
}

long CVVMReal::Merge(long /*indexleft*/, long /*indexright*/)
{
/*
	//if(Options!=VVMINT_EDITOR)return ERR_WRONG_OPTION;
	if(NumOfMasters<=0)return ERR_INVALID_MASTERS;
	if(indexleft<0||indexleft>=NumOfMasters)return ERR_INVALID_INDEX;
	if(indexright<0||indexright>=NumOfMasters)return ERR_INVALID_INDEX;

	if(Masters[indexleft].bits==NULL||Masters[indexright].bits==NULL)
		return ERR_BITMAP_REQUIRED;

	if	(	Masters[indexleft].height()!=Masters[indexright].height()||
			Masters[indexleft].width()!=Masters[indexright].width()||
			Masters[indexleft].bpp() != Masters[indexright].bpp()
		)	
			return ERR_MERGE_DONT_MATCH;
	
	sys::CEnterCS cs_obj (CritSect);;

	Progress=1;

//	long t1=GetTickCount(),t2;

	int i,j,m,S,NUM;
	long w=Masters[indexleft].width();
	long h=Masters[indexleft].height();
	RECT Rect;SetRect(&Rect,0,0,w,h);
	double c,C;
	POINT P;
	struct tagOI{
		POINT P1[MAX_COLL_POINTS],P2[MAX_COLL_POINTS];
		long N;
	}TempOI;	//Это будут все найденные точки соответствия
	double Cs[MAX_COLL_POINTS];	//А это их "хорошесть" в лице корреляции (см. Corr)

	TempOI.N=0;
	OverlapCalc.N=0;

	S=32;	//Размер зоны сравнения

	NUM=0;

	long PMax=((w/2)/S)*(h/S);	//Всего точек, для которых мы ищем соотв.

	for(i=w/2;i<w-S;i+=S)for(j=0;j<h-S;j+=S){	//По всем точкам в правой половине левой картинки
		P=CP(i,j);
		c=(long)FindCollPoint(P,S,indexleft,indexright);	//Ищем соответствие
		NUM++;	

		if(Progress<0){		//Это мы даем о себе знать внешнему миру
			_ERROR(ERR_PROCESS_STOPPED);
		}
		Progress=1+49*NUM/PMax;

		if((c<1500)&&(c>50)&&(P.x>0)){	
			//Если нашли точку, и хорошесть не слишком ольшая, и не слишком маленькая
			//(а иначе будет находить неверные точки в однотонных областях)
			
			TempOI.P1[TempOI.N]=CP(i,j);	//Точку записали
			TempOI.P2[TempOI.N]=P;
			Cs[TempOI.N]=c;
			if(TempOI.N<MAX_COLL_POINTS-1)	//И если уже и так много - вышли нах
				TempOI.N++;
			else 
				break;
		}
	}
	int k[6],k_opt[6]={0,1,2,3,4,5};	//Здесь будут номера тех 6 точек, по которым выходит 
						//лучшее преобразование
	S=TempOI.N;
	
	if(S<4){			//Ну если их всего 3 - делать нечего
		//LEAVE_CRIT_SECTION;
		return ERR_MERGE_UNABLE;
	}
	//t1=(t2=GetTickCount())-t1;
	
	m=0;
	if(S>20){			//...а если до хрена - выбираем 20 с наилучшей корреляцией
		for(i=0;i<20;i++){
			C=1501;
			for(j=0;j<S;j++)if(Cs[j]<C){
				C=(long)Cs[j];
				m=j;
			}
			OverlapCalc.P1[i]=TempOI.P1[m];	//OverlapCalc используем в какчестве буфера
			OverlapCalc.P2[i]=TempOI.P2[m];
			Cs[m]=1600;
		}
		for(i=0;i<20;i++){
			TempOI.P1[i]=OverlapCalc.P1[i];
			TempOI.P2[i]=OverlapCalc.P2[i];
		}
		S=20;
	}
	long l,L=0;
//	OverlapCalc.Init(6);
	CTransition T;
	if(S>6){	//Если теперь у нас больше 6, выберем 6 лучших
		PMax=(S)*(S-1)*(S-2)*(S-3)*(S-4)*(S-5)/720;
		OverlapCalc.N=6;
		NUM=0;
		for(k[0]=0;k[0]<S;k[0]++)for(k[1]=k[0]+1;k[1]<S;k[1]++){
			for(k[2]=k[1]+1;k[2]<S;k[2]++) for(k[3]=k[2]+1;k[3]<S;k[3]++)for(k[4]=k[3]+1;k[4]<S;k[4]++)for(k[5]=k[4]+1;k[5]<S;k[5]++){
					//перебираем все 6-ки
				for(i=0;i<6;i++){
					OverlapCalc.P1[i]=TempOI.P1[k[i]];				
					OverlapCalc.P2[i]=TempOI.P2[k[i]];
				}
					//и вычисляем преобразование
					//Быыла еще тут ускоряющая проверка, типа если левый угол левой картинки
					//преобразовался - значит лажа.
					//Но теперь нет, опять забыл почему
				if((OverlapCalc.CalcTrans(T)<3)/*&&
					!_PtInRect(OverlapCalc.Trans(CP(0,0)),Rect)&&
					!_PtInRect(OverlapCalc.Trans(CP(w,0),FALSE),Rect)***){
//					T.SetMatrix((double*)&OverlapCalc.B21,(double*)&OverlapCalc.B12);
					GetTrQ_Int(indexleft,indexright,&l,1,&T);
					//Вычисляем какчество преобразования, и если пока лучшее - запомнили
					if(L<l){
						L=l;
						for(i=0;i<6;i++)k_opt[i]=k[i];
					}
				}
				NUM++;
			}
			S=6;
		}
		if(Progress<0){
			_ERROR(ERR_PROCESS_STOPPED);
		}
		Progress=50+50*NUM/PMax;
	}
	for(i=0;i<S;i++){
		OverlapCalc.P1[i]=TempOI.P1[k_opt[i]];				
		OverlapCalc.P2[i]=TempOI.P2[k_opt[i]];
	}
	OverlapCalc.N=S;
	//Ну а теперьб таки вычисляем преобразование, которое

	OverlapCalc.CalcTrans(T);

//	t2=GetTickCount()-t2;
	//Это была проверка на отколнение от проективности, которая почему-то не работала
//	if(OverlapCalc.AvgDiscr()>2)_ERROR(ERR_MERGE_UNABLE);

	j=-1;
	//Ищем, если уже раньше было преобразование
	for(i=0;i<AutoNum;i++)
		if(Auto[i].Set()&&(Auto[i].il==indexleft)&&(Auto[i].ir==indexright))
			{j=i;break;}
	if(j>=0){
		//...то переписать
		Auto[j].Copy(T);
	}else{
		//А иначе - увеличить буфер и добавить новое
		//Здесь может быть ошибка, но все равно автоматическим объединением нкто не пользуется...
		j=AutoNum;
		CTransition *tmp=Auto;
		AutoNum++;
		Auto=new CTransition[AutoNum];
		
		if(Auto==NULL)	
		{
			SAFE_DELETE(tmp);
			//LEAVE_CRIT_SECTION;
			return ERR_OUT_OF_MEMORY;
		}

		if(tmp)CopyMemory(Auto,tmp,(AutoNum-1)*sizeof(CTransition));
		Auto[j].Copy(T);
		Auto[j].il=indexleft;
		Auto[j].ir=indexright;
	}
	Progress=101;

/*	t1/=1000;t2/=1000; //А это мы пытались ускорить, но забили, потому что не вышло
	char Str[128];sprintf(Str,"1st part %d\n2nd part %d\nAll %d",t1,t2,t1+t2);
	MessageBox(NULL,Str,"Profiling",MB_OK);***
	
	//LEAVE_CRIT_SECTION;*/
	return 100;
}


double CVVMReal::FindCollPoint(POINT & /*P*/, long /*S*/, long /*il*/, long /*ir*/)
{
/*	CFourierTransformer FFT;
	BITMAPINFOHEADER bi = *Masters[ir].get_bi_header();;
	int w = bi.biWidth; int h = bi.biHeight;
	RECT Rect;SetRect(&Rect,0,0,w,h);
	long dx,dy;
	int i,j,k,D,Sx,Sy;
	double c,C=999999;
	int X=-1,Y=-1;
	double M[3]={0,0,0},Dp[3]={0,0,0};

	//Сначала считаем дисперсию по всем компонентам в квадрате SxS
	D=S/3;
	for(i=P.x;i<P.x+S;i++)for(j=P.y;j<P.y+S;j++){
		for(k=0;k<3;k++)M[k]+=Masters[il].Point(i,j,k);
	}
	for(k=0;k<3;k++)M[k]/=(S*S);

	for(i=P.x;i<P.x+S;i++)for(j=P.y;j<P.y+S;j++){
		for(k=0;k<3;k++)Dp[k]+=pow(M[k]-Masters[il].Point(i,j,k),2);
	}
	for(k=0;k<3;k++)Dp[k]/=(S*S);

	//И если дисперсия мала, выходим не найдя точки - потому что когда
	//квадрат почти равномерный, все равно нифига не найдем.
	if((Dp[0]<100)&&(Dp[1]<100)&&(Dp[2]<100)){
		P.x=-1;P.y=-1;
		return Dp[0]+Dp[1]+Dp[2];
	}

	Sx=P.x;
	Sy=h-P.y-S;
	//Теперь с шагом S/3 проверяем все квадраты на втором мастере
	for(i=0;i<w/2;i+=D)for(j=0;j<h-S;j+=D){
		for(k=0;k<3;k++)if(Dp[k]>100){
			//Это мы получаем сдвиг между квадратами
			FFT.GetDisplacement(Masters[il].bits(),Masters[ir].bits(),
				(PBITMAPINFOHEADER) Masters[il].get_bi_header(), 
				k,Sx,Sy,S,Sx-i,j+Sy+S-h,&dx,&dy);
			//Если он небольшой, то считаем реальную корреляцию
			if(_PtInRect(CP(i+dx,j-dy),Rect)&&_PtInRect(CP(i+dx+S,j-dy+S),Rect)&&(abs(dx)<=D)&&(abs(dy)<=D)){
				//ищем квадрат с наилучшей корреляцией
				if((c=Corr(il,ir,P,CP(i+dx,j-dy),S,k))<C){
					C=c;X=i+dx;Y=j-dy;
				}
			}
		}
	}
	//Чего нашли возвращаем
	P.x=X;
	P.y=Y;
	return C;*/

	return 0;
}

double CVVMReal::Corr(long /*il*/, long /*ir*/, POINT /*P1*/, POINT /*P2*/, long /*S*/, long /*c*/)
{
//Тут была попытка вычислять честно линейную корреляцию,
//вместо тупой средней разности квадратов. Не прошла.
/*	double l_mean[3]={0,0,0},r_mean[3]={0,0,0};
	double lr_mean[3]={0,0,0},ll_mean[3]={0,0,0};
	double rr_mean[3]={0,0,0};
	BYTE p1,p2;
	for(j=0;j<S;j+=2)for(i=0;i<S;i+=2){
		for(k=0;k<3;k++){
			p1=Masters[il].Point(i+P1.x,j+P1.y,k);
			p2=Masters[ir].Point(i+P2.x,j+P2.y,k);
			l_mean[k]+=p1;ll_mean[k]+=p1*p1;
			r_mean[k]+=p2;rr_mean[k]+=p2*p2;
			lr_mean[k]+=p1*p2;
		}
		N++;
	}
	double res[3];
	for(k=0;k<3;k++){
		l_mean[k]/=N;r_mean[k]/=N;
		ll_mean[k]/=N;rr_mean[k]/=N;
		lr_mean[k]/=N;
		res[k]=(lr_mean[k]-l_mean[k]*r_mean[k])/
			sqrt((ll_mean[k]-l_mean[k]*l_mean[k])*(rr_mean[k]-r_mean[k]*r_mean[k]));
		if(res[k]<0)res[k]=0.001;
		if(res[k]<C)C=res[k];
	}
	if(0==C)C=0.001;
	return 1./C;*/
/*
	int i,j,N=0;
	double C=0;
	for(i=0;i<S;i++)for(j=0;j<S;j++){
		C+=pow((double)(Masters[il].Point(i+P1.x,j+P1.y,c)-Masters[ir].Point(i+P2.x,j+P2.y,c)),2);
		N++;
	}
	C/=N;
	return C;*/
	return 0;
}

long CVVMReal::CheckMerge(long il, long ir)
{
	//if(Options!=VVMINT_EDITOR)return ERR_WRONG_OPTION;

	if (!p_src_data->masters_cont.Count())return ERR_INVALID_MASTERS;
	if (!p_src_data->masters_cont.is_indx_ok(il) || 
		!p_src_data->masters_cont.is_indx_ok(ir))
												return ERR_INVALID_INDEX;
/*
	if (AutoNum==0)return ERR_NO_MERGE;

	sys::CEnterCS cs_obj (CritSect);;
	long i;
	for(i=0;i<AutoNum;i++)
		if(Auto[i].Set()&&(Auto[i].il==il)&&(Auto[i].ir==ir)){
			//LEAVE_CRIT_SECTION;
			return 0;
		}

	//LEAVE_CRIT_SECTION;*/
	return ERR_NO_MERGE; 
}

long CVVMReal::DeleteMerge(long il, long ir)
{
	if (info.is_editor())return ERR_WRONG_OPTION;
	
	if( !p_src_data->masters_cont.Count() )return ERR_INVALID_MASTERS;
	if( !p_src_data->masters_cont.is_indx_ok(il) || !p_src_data->masters_cont.is_indx_ok(ir) ) return ERR_INVALID_INDEX;

	return 100;
}

long CVVMReal::CompileTrans()
{
	sys::CEnterCS cs_obj (CritSect);
	
	compile_stopped = 0;
	compile_progess = 0;
	trans::CProgress p( compile_progess, compile_stopped);
		
	//trans::CTransCont * _temp = new trans::CTransCont(*p_trans_cont);

	long res = p_trans_cont->compile(&p);
/*
	if (res != ERR_PROCESS_STOPPED)
	{
		delete (p_trans_cont);
		p_trans_cont = _temp;

	}*/
	//p_src_data->masters_cont.compile_extents();

	compile_stopped = 1;
	return res;
}


long CVVMReal::GetPoly(long ifrom, long ito, long *pN, POINT **pp)
{
	//if( info.Options==VVMINT_MASTER)	return ERR_WRONG_OPTION;
	if (!p_src_data->masters_cont.is_masters())	return ERR_INVALID_MASTERS;
		
	if(!pN || !pp)
					return ERR_BAD_ADDRESS;

	trans::CTransClip * tc = p_trans_cont->find( ifrom, ito);
	if (!tc) return ERR_TRANSITION_UNDEFINED;

	const KPOINT * pb = 0;
	size_t bs = 0;

	tc->get_vis_poly_buff(&pb,&bs);

	*pp = const_cast <KPOINT *> (pb);
	*pN = bs;

	return 0;
}

long CVVMReal::SetExtentList(long masterIndex, long N, PRECT rectArr)
{
	if (!p_src_data->masters_cont.Count())	return ERR_INVALID_MASTERS;
	if (!p_src_data->is_master(masterIndex))	return ERR_INVALID_INDEX;
	
	
	sys::CEnterCS cs_obj (CritSect);

	if (N<0)		return ERR_INVALID_SIZE;

	if (!rectArr)	return ERR_BAD_ADDRESS;

	const KRECT * p = static_cast <KRECT *> (rectArr);

	masters::CMaster * f = p_src_data->get_master_ext (masterIndex);
		
	if(f->height()<0)return ERR_BITMAPINFO_REQUIRED;

	f->extents.load_from_array( N, p);
	
	
	return 0;
}

long CVVMReal::GetExtentList(long masterIndex, long *pN, PRECT *prectArr)
{
	
	if ( !p_src_data->masters_cont.Count() )	return ERR_INVALID_MASTERS;
	if ( !p_src_data->is_master(masterIndex))	return ERR_INVALID_INDEX;

	if( !prectArr || !pN )	return ERR_BAD_ADDRESS;

	sys::CEnterCS cs_obj (CritSect);

	* pN = 0;
	* prectArr = 0;

	masters::CMaster * f = p_src_data->get_master_ext (masterIndex);
	
	if(f->extents.size())
	{
		* pN = f->extents.size();
		* prectArr = const_cast <KRECT*> (f->extents.ptr());
		 
		return ERR_NO_ERROR;
	}
	else return ERR_NO_LIST;
}

long CVVMReal::CompileExtents()
{
	if (!p_src_data->extents_enabled) return 0;
	sys::CEnterCS cs_obj (CritSect);

	unsigned num_of_masters = p_src_data->masters_cont.Count();
	if (!num_of_masters) return ERR_INVALID_MASTERS;

	
	compile_progess = 95;
	compile_stopped = 0;
	for ( unsigned i=0; i < num_of_masters; ++i)
	{
		if (compile_stopped)	return ERR_PROCESS_STOPPED;
		masters::CMaster * f = p_src_data->get_master_ext (i);
		f->compile_extents();
	}
	compile_stopped = 1; 
	
	
	return 0;

}

long CVVMReal::GetExtentTable(long masterIndex, long **ptable, long *pSize)
{
	if ( !p_src_data->masters_cont.Count() )	return ERR_INVALID_MASTERS;
	if ( !p_src_data->is_master(masterIndex))	return ERR_INVALID_INDEX;

	if( !ptable || !pSize )return ERR_BAD_ADDRESS;

	sys::CEnterCS cs_obj (CritSect);

	masters::CMaster * f = p_src_data->get_master_ext (masterIndex);
	
	if(f->extents.table_size())
	{
		* pSize= f->extents.table_size() - 2;
		* ptable = const_cast <long *> (f->extents.table_ptr());
		 
		return ERR_NO_ERROR;
	}
	else return ERR_NO_LIST;
}

long CVVMReal::SaveExtentTable(long /*masterIndex*/, void ** /*pbits*/, long * /*pN*/, BOOL /*Compact*/)
{
/*	if(!info.is_editor())return ERR_WRONG_OPTION;
	if(NumOfMasters<=0)return ERR_INVALID_MASTERS;
	if(masterIndex<0||masterIndex>NumOfMasters)return ERR_INVALID_INDEX;
	if(pN==NULL||pbits==NULL)return ERR_BAD_ADDRESS;
	if(Masters[masterIndex].Rects==NULL)return ERR_NO_LIST;

	sys::CEnterCS cs_obj (CritSect);

	SaveBuffer.Pointer=0;

	CMasterInfo *M=&Masters[masterIndex];

	SaveBuffer.AddString("VVMAPInterface extents info. v1.\n");	
	SaveBuffer.Line_long("ID=",M->ID);
	SaveBuffer.Line_long("Height=",M->height());
	SaveBuffer.Line_long("Width=",M->width());
	SaveBuffer.Line_long("Rects=",M->NumRects);

	SaveBuffer.AddLongs((long*)M->Rects,M->NumRects*4);
	if(SaveBuffer.NoMemory)_ERROR(ERR_OUT_OF_MEMORY);

	if(!Compact && M->bits())
	{
		SaveBuffer.Line_long("Bitcount=",M->bpp());
		SaveBuffer.Line_long("ImageSize=",M->ImageSize());
		SaveBuffer.AddBuffer(M->bits(),M->ImageSize());
		if(SaveBuffer.NoMemory)_ERROR(ERR_OUT_OF_MEMORY);
	}

	if(M->Extents){
		SaveBuffer.AddString("EXTENTS");
		SaveBuffer.AddBuffer(M->Extents,
			M->width()*M->height()*sizeof(long));
		if(SaveBuffer.NoMemory)_ERROR(ERR_OUT_OF_MEMORY);
	}

	*pbits=SaveBuffer.Buffer;
	*pN=SaveBuffer.Pointer;

	//LEAVE_CRIT_SECTION;
*/
	err_msg("SaveExtentTable");
	return 100;
}

long CVVMReal::LoadExtentTable(long /*masterIndex*/, void * /*bits*/, long /*N*/)
{
/*	if(info.is_commutator())return ERR_WRONG_OPTION;
	if(NumOfMasters<=0)return ERR_INVALID_MASTERS;
	if(masterIndex<-1||masterIndex>NumOfMasters)return ERR_INVALID_INDEX;
	if(bits==NULL)return ERR_BAD_ADDRESS;

	sys::CEnterCS cs_obj (CritSect);;

#define __CHECK(str) if(Input.CheckString(str)==WRONG_FORMAT)return ERR_INVALID_FORMAT;

	CBufferInput Input((char*)bits, N, &info);
	CMasterInfo *M=NULL;
	if(masterIndex>=0)M=&(Masters[masterIndex]);

	long i;

	__CHECK("VVMAPInterface extents info. v1.");

	__CHECK("ID=");Input.GetLong(&i);
	if(masterIndex>=0)	M->ID=i;
	else
	{
		long j;
		for(j=0;j<NumOfMasters;j++)
					if(Masters[j].ID==i) M = &Masters[j];
		if(M==NULL)_ERROR(ERR_ID_NOT_PRESENT);
	}

	long w, h, bc, is; 
	__CHECK("Height=");Input.GetLong(&h);
	__CHECK("Width=");Input.GetLong(&w);
	__CHECK("Rects=");Input.GetLong(&M->NumRects);
	if(M->height()<0||M->NumRects<=0)_ERROR(ERR_INVALID_FORMAT);

	for(i=0;i<M->NumRects*4;i++)
		Input.GetLong((long*)(&M->Rects)+i);
	
	if(Input.CheckString("Bitcount=")!=WRONG_FORMAT)
	{
		
		
		Input.GetLong(&i);		bc = (BYTE)i;
		__CHECK("ImageSize=");	Input.GetLong((long*)&is);
		__CHECK("BITS");

		//SAFE_DELETE(M->bits);
		//M->bits=new BYTE[M->bi.biSizeImage];
		
		
		if (!M->SetImInfo(w, h, bc, is)) _ERROR(ERR_OUT_OF_MEMORY);
		
		Input.GetBuffer(M->bits(),M->ImageSize());
	}

	if(Input.CheckString("EXTENTS")!=WRONG_FORMAT){
		long s=M->width()*M->height();
		
		SAFE_DELETE(M->Extents);
		M->Extents=new long[s];
		if(M->Extents==NULL)_ERROR(ERR_OUT_OF_MEMORY);

		Input.GetBuffer((PBYTE)(M->Extents),s*sizeof(long));
	}
#undef __CHECK

	//LEAVE_CRIT_SECTION;
	return 0;*/

	err_msg("LoadExtentTable");
	return 100;
}
///#define ABS(e) ( (e<0) ? (-e) : e)

_TCHAR * CVVMReal::ErrString(long e)
{
	int err_id;
	
	if (!e)		
				err_id = ERR_OK_ID;
	else
				err_id = abs(e) + 1000;

	LoadString(hDllModule, err_id, Err, MAX_PATH);

	main_err_msg = ( std::tstring(Err) + std::tstring(_T(" (")) + _err_msg + std::tstring(_T(")")) ); 
	_err_msg = std::tstring();
	
	/* );
	_TCHAR * ps = const_cast <_TCHAR *> (main_err_msg.c_str());
	return ps;
	*/

	return Err;

}


long CVVMReal::GetProgress(long *P)
{
	if( !info.is_editor() )		return ERR_WRONG_OPTION;
	if (compile_stopped)		return ERR_NO_PROCESS_RUNNING;
//	
	long _progress_;
	sys::ExchangeLongs( _progress_, compile_progess);
	
	if(_progress_>=0)
	{
		*P = _progress_;
		return 0;
	}
	else
	{
		return ERR_NO_PROCESS_RUNNING;

	}
}

long CVVMReal::StopProcess()
{
	if(!info.is_editor())	return ERR_WRONG_OPTION;

	if (compile_stopped)	return ERR_NO_PROCESS_RUNNING;

	long _stop_ = 1;
	sys::ExchangeLongs( compile_stopped, _stop_);
	
	return 0;
}

const char *CVVMReal::GetInterfaceVersion()
{
	//NOTENOTE: we 'll be add after every bugfix & enhencement 

	//return /*"3.3.6"*/ INTERFACE_VER;
	CModuleVersion mv;
	_TCHAR buf[MAX_PATH];
	GetModuleFileName ( hDllModule, buf, MAX_PATH);
	if (mv.GetFileVersionInfoFromFullPath ( buf ))
	{
		CString str = mv.GetValue (_T("ProductVersion"));
		static char ver_buf[MAX_PATH];	
		USES_CONVERSION;
		CT2CA z(str);
		//ANSI_str(LPCTSTR(str));
		strncpy ( ver_buf, z, MAX_PATH);
		return ver_buf;
	}
	
	return "";
}

long CVVMReal::GetTransQuality(long il, long ir, long *pQ)
{

	if (!p_src_data->master_end())	return ERR_INVALID_MASTERS;

	sys::CEnterCS cs_obj (CritSect);

	long _il = p_src_data->correct_index(il);	
	long _ir = p_src_data->correct_index(ir);


	if ( const trans::CTransClip * t = p_trans_cont->find( _il, _ir) ) 
	{
		if (pQ) *pQ = long (t->quality());
		return ERR_NO_ERROR;
	}
	else 
		return ERR_TRANSITION_UNDEFINED;
}



//////////////////////////////////////////////////////////////////////////

long CVVMReal::GetCoordsFromDistances(double * /*R*/, long * /*pX*/, long * /*pY*/)
{
	/*
	if(!R||!pX||!pY)return ERR_BAD_ADDRESS;
	if(-1==MapXScale)return ERR_MAP_NOT_ALIGNED;
	if(MINLONG==MapXOrig)return ERR_MAP_NOT_ALIGNED;

	sys::CEnterCS cs_obj (CritSect);;
	long e;
	double X,Y,Z;
	if(e=GetAbsCoordsFromDistances(R,&X,&Y,&Z))_ERROR(e);
	*pX=(long)(X*MapXScale+MapXOrig);
	*pY=(long)(Y*MapYScale+MapYOrig);
	//LEAVE_CRIT_SECTION;*/
	return 0;
}

long CVVMReal::GetXYSeries(long /*num*/, long /*il*/, long /*ir*/, POINT * /*From*/, POINT * /*To*/)
{
/*	if(!info.is_commutator())return ERR_WRONG_OPTION;
	if(NumOfMasters<=0)return ERR_INVALID_MASTERS;
	if(il<0||il>=NumOfMasters)return ERR_INVALID_INDEX;
	if(ir<0||ir>=NumOfMasters)return ERR_INVALID_INDEX;

	sys::CEnterCS cs_obj (CritSect);;

	if(il==ir){
		CopyMemory(To,From,num*sizeof(POINT));
		_ERROR(0);
	}

	double L;
	long j;
		if(il==NumOfMasters)il=-1;
		if(ir==NumOfMasters)ir=-1;
		BOOL D;
		CTransition *T;
		if(FindTransition(il,ir,&D,&T))_ERROR(ERR_TRANSITION_UNDEFINED);
		for(j=0;j<num;j++)
			T->Trans(From[j].x,From[j].y,&To[j].x,&To[j].y,L,D);

	//LEAVE_CRIT_SECTION;*/
	return 100;
}

long CVVMReal::SetColors(long *pColors)
{
	if(!pColors)return ERR_BAD_ADDRESS;
	
	sys::CEnterCS cs_obj (CritSect);
	int m_num = p_src_data->masters_cont.Count();
	p_src_data->maps_cont.colors.SetColors( pColors, m_num);
	//////CopyMemory(Colors,pColors,(NumOfMasters+1)*4*sizeof(long));
	return 0;
}

long CVVMReal::ProcessMapBitmap(long *pColors)
{
	sys::CEnterCS cs_obj (CritSect);
	
	if (!p_src_data->vismap_enabled) 
	{
		p_src_data->maps_cont.items[0].vmap.release();			
		return ERR_NO_ERROR;
	}

	if (p_src_data->maps_cont.items[0].vmap.is_empty()) return ERR_MAP_NOT_COMPILED;
	if (!pColors)
	{
		//masters::CVisColors c& = ;
		return p_src_data->maps_cont.items[0].ProcessVisBM(p_src_data->maps_cont.colors);
	}
	else
	{
		int n = p_src_data->masters_cont.Count();
		masters::CVisColors c; c.SetColors(pColors,n);
		return p_src_data->maps_cont.items[0].ProcessVisBM(c);
	}
}

long CVVMReal::GetColors(long **ppColors)
{
	if (!ppColors) return ERR_BAD_ADDRESS;
	sys::CEnterCS cs_obj (CritSect);
	if (p_src_data->maps_cont.colors.is_empty()) return ERR_COLORS_NOT_SET;
	const long *pColors = 0; 
	size_t size = 0;

	p_src_data->maps_cont.colors.GetColors( &pColors, &size);
	*ppColors = const_cast <long*> (pColors);

	return 0;
}

long CVVMReal::GetProcessedBitmap(PBITMAPINFO *pbi, PBYTE *pbits)
{
	if (!pbi || !pbits) return ERR_BAD_ADDRESS;

	*pbi = 0; 	*pbits = 0;

	sys::CEnterCS cs_obj (CritSect);
	
	if (!p_src_data->vismap_enabled) 
	{
		p_src_data->maps_cont.items[0].vmap.release();
		return ERR_NO_ERROR;
	}

	//if (p_src_data->maps_cont.items[0].the_dib().empty()) return ERR_NO_ERROR;

	if (p_src_data->maps_cont.items[0].vmap.is_empty()) return ERR_MAP_NOT_COMPILED;
	
	
	
	const masters::CDib& r = p_src_data->maps_cont.items[0].vmap.processed_dib();
	
	//if (! ( (r.w() > 0) && (r.h() > 0) ) ) return ERR_NO_ERROR;

	*pbi = const_cast <PBITMAPINFO> (r.get_bi_header());
	*pbits = const_cast <PBYTE> (r.bits());
	return 0;
}

long CVVMReal::RunOptionsDialog(HWND hParent)
{
#ifndef _WIN32_WCE
	sys::CEnterCS cs_obj (CritSect);
	opts::KObjDialog<CVVMReal> dlg(hParent, IDD_OPTIONS_DLG, this);//trans::CTransCont p_trans_cont
	if (dlg.Run()) p_trans_cont->compile_all_next_time();

	return 0;
#else
	return 100;
#endif
}


long CVVMReal::GetAlignMap( long* pNumber, const long ** pIndex, const long** /*pPoint*/ )
{
//	return ERR_NOT_IMPLEMENTED;
/*
	*pNumber	= m_arrAlignMapPointNumbers.size();
	*pIndex		= &m_arrAlignMapPointNumbers[0];
	*pPoint		= &m_arrAlignMapPointCoords[0];
	return ERR_NO_ERROR;
*/
	*pNumber	= 0;
	*pIndex		= 0;
	return 100;
}

struct the_word
{
	KPOINT p[16];

	the_word()
	{
		p[0] = KPOINT (20, 20);
		p[1] = KPOINT (70, 70);

		p[2] = KPOINT (70, 20);
		p[3] = KPOINT (20, 70);
	}

	unsigned count() const { return 4;}
};

//the_word word;

long CVVMReal::GetHoriz(int master_index, const POINT **points, int * points_count)
{
	if ( !points || !points_count ) return ERR_BAD_ADDRESS;
	sys::CEnterCS cs_obj (CritSect);

	*points = 0;
	*points_count = 0;

	int map_i = p_src_data->map_index();

//	long master_ID = p_src_data->master_id(master_index);
	

	if (const trans::CTransClip * t = p_trans_cont->find( master_index, map_i))
	{
		const KPOINT * p = t->horiz.get_data_ptr();
		const size_t s = t->horiz.points_count();

		*points = p;		//	word.p;
		*points_count = s;	//	word.count();
							//	return ERR_NO_ERROR;

		if (s) return ERR_NO_ERROR;
	}
	
	return ERR_HORIZONT_NOT_FOUND;
}

void CVVMReal::Delete_All()
{
	sys::CEnterCS cs_obj (CritSect);

	if (p_src_data)		delete p_src_data;
	if (p_trans_cont)	delete p_trans_cont;

	sp.clear();
	
	p_src_data = 0;		p_trans_cont = 0;
	m_SaveBuffer.release();
}

long CVVMReal::GetXY(long indexfrom, long x1, long y1, long indexto, long *px2, long *py2, BOOL /*IgnoreMap*/)
{
	if (!px2 || !py2) return ERR_BAD_ADDRESS;

	*px2 = 0;		*py2 = 0;
	
	try
	{
		sys::CTryEnterCS cs_obj (CritSect);

		KPOINT res_point = p_src_data->get_master(indexto)->DockPoint();
		long result = 
			p_trans_cont->get_xy(
					p_src_data->corr_i(indexfrom), KPOINT(x1,y1), 
					p_src_data->corr_i(indexto), &res_point
								);
		
		*px2 = res_point.x;		*py2 = res_point.y;

		return result;
	}
	catch (sys::crit_sect_occuped)
	{
		return ERR_CRITSECTION_OCCUPIED;
	}
	
}

long CVVMReal::PutBuffer (const char * name, const void * buff_ptr, size_t buff_size)
{
		
	if (!name || !buff_ptr ) return ERR_BAD_ADDRESS;
	if (!strlen(name)) return ERR_INVALID_ARGUMENT;
	sys::CEnterCS cs_obj (CritSect);

		zippr.put_buff( std::string(name), buff_ptr, buff_size);
	return ERR_OK;
}

long CVVMReal::GetBuffer (const char * name, const void ** pbuff_ptr, size_t * pbuff_size)
{
	if (!name || !pbuff_ptr || !pbuff_size) return ERR_BAD_ADDRESS;
	if (!strlen(name)) return ERR_INVALID_ARGUMENT;
	sys::CEnterCS cs_obj (CritSect);

	if (unzippr.get_buff( std::string(name), pbuff_ptr, pbuff_size))
		return ERR_OK;
	else
		return ERR_FILE_NOT_FOUND_IN_ZIP;
}
//----------------------------------------------------------------------

long CVVMReal::GetSameObjects
				(
				long il, long ir, 
				
				const CObjectRecord *Obj1,  const CObjectRecord *Obj2, 
				
				long N1, long N2, 
				
				long **IDs
				)

{
//	if	(Options != VVMINT_EDITOR && Options != VVMINT_COMMUTATOR)	return ERR_WRONG_OPTION;

	if (!IDs) return ERR_BAD_ADDRESS;

	sys::CEnterCS cs_obj (CritSect);

	if	(!p_src_data->master_end())									return ERR_INVALID_MASTERS;
	
	if	(!p_src_data->is_master(il) || !p_src_data->is_master(ir))	return ERR_INVALID_INDEX;
	
	if	((p_src_data->get_master(il)->height()<=0) || 
		 (p_src_data->get_master(ir)->height()<=0) )				return ERR_BITMAPINFO_REQUIRED;
	
	if	(N1 > get_same_object::max_objects)							return ERR_TOO_MANY_OBJECTS;


	gso.do_it ( p_trans_cont, il, ir, Obj1, Obj2, N1, N2);
	
	*IDs = const_cast <long * > (gso.ptr());

	return 0;
}


long CVVMReal::SetParameter(long param_id, long param)
{

	p_src_data->set_param(param_id, param);
	bool old_val = param != 0;
	switch (param_id)
	{
	case GP_ENABLE_EXTENTS:			std::swap(m_enable_extents, old_val);		break;
	case GP_ENABLE_VIS_MAP:			std::swap(m_enable_vismap, old_val);		break;
	case GP_SAVE_MAP_IMAGE:			std::swap(m_enable_map_image, old_val);	break;
	case GP_SAVE_MASTERS_IMAGES:	std::swap(m_enable_master_image, old_val);		break;

	default:
		return -1;
	}
	return old_val ? 1 : 0;
}

long CVVMReal::GetParameter(long param_id)
{
	return p_src_data->get_param(param_id);
}

long CVVMReal::SetMapSize (unsigned long width, unsigned long height)
{
	p_src_data->saving_map		= false;
//	p_src_data->vismap_enabled	= false;

	p_src_data->get_map()->set_size(width, height);
	return 0;
}
//-------------------------------------------------------------------------------------
const char * map_file_name = "extern_images/map.image";

long CVVMReal::GetMapBuffer	( const void ** pbuff_ptr,	size_t *	pbuff_size )
{
	return GetBuffer( map_file_name, pbuff_ptr, pbuff_size);
}

long CVVMReal::PutMapBuffer	( const void * buff_ptr,	size_t		buff_size  )
{
	return PutBuffer( map_file_name, buff_ptr, buff_size);
}
