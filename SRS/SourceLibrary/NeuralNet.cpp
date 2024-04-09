// NeuralNet.cpp: implementation of the CNeuralNet class.
//
//////////////////////////////////////////////////////////////////////

#include <windows.h>

#ifdef DEBUG_NEW
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#endif //DEBUG_NEW

#include "NeuralNet.h"
#include "stdio.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNeuralNet::CNeuralNet()
{
	Init();
	
}

CNeuralNet::~CNeuralNet()
{
	Delete();
	
}

void CNeuralNet::Init()
{
	layers=NULL; NL=0; NEnt=0; NOut=0; XLearn=NULL; XTest=NULL; NLearn=0; NTest=0; RecLen=0; 
	Er=10.0; TestEr=0;
	step=0.5;
	cnt = 0;
}
long CNeuralNet::GetNumOfNeuronsInLayer(long index)
{
	return layers[index].NN;
}
void CNeuralNet::ClearXLearn(){	if (XLearn) delete[] XLearn;}
void CNeuralNet::ClearXTest() { if (XTest) delete[] XTest;}

void CNeuralNet::Delete()
{	
	ClearXTest();
	ClearXLearn();
	if (layers) delete[] layers;
	Init();
}

void CNeuralNet::Init(long NumOfLayers, long *list)
{
	Delete();
	NL=NumOfLayers;
	layers=new Layer[NL];
	long i;
	NEnt=list[0];
	for (i=0; i<NL; i++) {
		layers[i].Init(list[i+1],list[i]);		
	}
	NOut=list[NL];
	RecLen=NEnt+NOut;

	long ll,nn,ww;
	TotalNumOfWeights=0;
	for (ll=0; ll<NL; ll++) for (nn=0; nn<layers[ll].NN; nn++) for (ww=0; ww<layers[ll].neurons[nn].NW; ww++) {
		TotalNumOfWeights++;
	}
		
}

void CNeuralNet::GetResult(double *X)
{
	long i;
	layers[0].GetResult(X);
	for (i=1; i<NL; i++) {
		layers[i].GetResult(layers[i-1].Y);
	}
}

double CNeuralNet::Y(long index)
{
	return layers[NL-1].Y[index];
}

void CNeuralNet::SetXLearn(long N, double *X)
{
	ClearXLearn();
	NLearn=N;
	long size=N*RecLen*sizeof(double);
	XLearn=new double[size];
	CopyMemory(XLearn,X,size);
}
void CNeuralNet::SetXTest(long N, double *X)
{
	ClearXTest();
	NTest=N;
	long size=N*RecLen*sizeof(double);
	XTest=new double[size];
	CopyMemory(XTest,X,size);
	CalcTestErr();
}

void CNeuralNet::IterCleardE()
{
	long i,j;
	Layer *L;
	for (i=0; i<NL; i++) { 
		L=&layers[i];
		for (j=0; j<L->NN;j++)	L->neurons[j].CleardE();
	}	
}
void CNeuralNet::IterSetdY()
{
	long i,j;
	Layer *L;
	for (i=0; i<NL; i++) { 
		L=&layers[i];
		for (j=0; j<L->NN;j++)	L->neurons[j].SetdY();
	}	
}
void CNeuralNet::IterChangeW()
{
	long i,j;
	Layer *L;
	for (i=0; i<NL; i++) { 
		L=&layers[i];
		for (j=0; j<L->NN;j++)	L->neurons[j].ChangeW(step, 1.0/TotalNumOfWeights);
	}	
}
void CNeuralNet::IterInitLastLayer(double *YOut)
{
	Layer *L;
	long j;
	double Res;
	L=&layers[NL-1];
	double sendDiff;
	for (j=0; j<NOut; j++) {
		Res=Y(j);
		sendDiff=Res-YOut[j];
		L->neurons[j].SendZ(sendDiff*2/(NOut*CountOfLearning));
	}	
}
void CNeuralNet::NPush()	{ for (long i=0; i<NL; i++) { layers[i].Push();}}
void CNeuralNet::NPop()		{ for (long i=0; i<NL; i++) { layers[i].Pop();}}

void CNeuralNet::Iteration(double percent)
{
	if (NLearn==0) return;
	long i,j,j2,k, m, m2;
	Layer* L,*LNext;
	double *X=XLearn,*YOut=XLearn+NEnt;

	NPush();
	IterCleardE();
	double RealPrevEr=Er;
	CountOfLearning=long(NLearn*percent/100);
	for (m2=0; m2<NLearn/CountOfLearning; m2++) {
		for (m=0; m<CountOfLearning; m++) {
			k=(percent==100)?m:(long(NLearn*1.0*rand()/((long)RAND_MAX+1)));
			(percent==100)?0:((X=&XLearn[k*RecLen]),(YOut=X+NEnt));
				
			GetResult(X);
			IterSetdY();
			IterInitLastLayer(YOut);
			for (i=NL-2; i>=0; i--) { L=&layers[i]; LNext=&layers[i+1];
				for (j=0; j<L->NN;j++) {
					for (j2=0; j2<LNext->NN;j2++) {
						L->neurons[j].SendZ(LNext->neurons[j2].Z*LNext->neurons[j2].W[j]*LNext->neurons[j2].dY);
					}
				}
			}
			for (i=0; i<NL; i++) { L=&layers[i];
				for (j=0; j<L->NN;j++)	L->neurons[j].AdddE();
			}			
			X+=RecLen;
			YOut+=RecLen;
		}
		IterChangeW();
	}	
	
	
	Er=CalcEr(XLearn,NLearn);
	if (Er-1e-5<=RealPrevEr) {
		if (step<100000) step*=1.1;
	} else {
		NPop();
		Er=CalcEr(XLearn,NLearn);
		if (step>0.000001) step/=4;
	}
	
	srand(GetTickCount());
	long testRand=rand()%10;
	if (testRand==1) CalcTestErr();
	TestEr=step;
	
	/*
	static itNum=0;	itNum++;
	if(itNum%5000==1) {
		char str[1000];
		sprintf(str,"step=%f",step);
		MessageBoxA(NULL,str,"Info",MB_OK);
	}*/
	cnt++;
}


void CNeuralNet::CalcTestErr()
{
	TestEr = CalcEr(XTest,NTest);
}

double CNeuralNet::CalcEr(double *X, long N)
{
	if (N==0) return 0;
	double Err=0, Res;
	double *YOut=X+NEnt;
	long k,j;
	for (k=0; k<N; k++) {
		GetResult(X);
		for (j=0; j<NOut; j++) {
			Res=Y(j);
			Err+=(Res-YOut[j])*(Res-YOut[j]);
		}
		X+=RecLen;
		YOut+=RecLen;
	}
		
	long ll,nn,ww;
	for (ll=0; ll<NL; ll++) for (nn=0; nn<layers[ll].NN; nn++) for (ww=0; ww<layers[ll].neurons[nn].NW; ww++) {
		Err+=N*NOut*0.5*layers[ll].neurons[nn].W[ww]*layers[ll].neurons[nn].W[ww]/TotalNumOfWeights;
	}
	
	return Err/=(N*NOut);
	
}

void CNeuralNet::Load(void* data)
{
	long fileSize;
	long i,j;

	BYTE* seek=(BYTE*)(data);
	long addSeek;

	CopyMemory(&fileSize,seek,addSeek=sizeof(long)); seek+=addSeek;
	CopyMemory(&NL,seek,addSeek=sizeof(long)); seek+=addSeek;
	seek+=sizeof(long);//skiping NOut (it will be set in Init);
	Init(NL,(long*)seek); seek+=(NL+1)*sizeof(long);//skiping NEnt and NN array
	for (i=0;i<NL;i++) for (j=0;j<layers[i].NN;j++) {
		CopyMemory(layers[i].neurons[j].W,seek,addSeek=sizeof(double)*layers[i].neurons[j].NW); seek+=addSeek;
	}
}

long CNeuralNet::Save(void** data)
{
	long fileSize=0;
	long i,j;
	long NW=0;
	fileSize+=4*sizeof(long); // fileSize,NL,NOut,NEnt;
	NW+=(NEnt+1)*layers[0].NN;		
	fileSize+=NL*sizeof(long); //NN array;
	for (i=1;i<NL;i++) {
		NW+=(layers[i-1].NN+1)*layers[i].NN;
	}
	fileSize+=NW*sizeof(double); //W arrays

	*data=(void*) new BYTE[fileSize];
	
	BYTE* seek=(BYTE*)(*data);
	long addSeek;

	CopyMemory(seek,&fileSize,addSeek=sizeof(long)); seek+=addSeek;
	CopyMemory(seek,&NL,addSeek=sizeof(long)); seek+=addSeek;
	CopyMemory(seek,&NOut,addSeek=sizeof(long)); seek+=addSeek;
	CopyMemory(seek,&NEnt,addSeek=sizeof(long)); seek+=addSeek;
	for (i=0;i<NL;i++) {
		CopyMemory(seek,&(layers[i].NN),addSeek=sizeof(long)); seek+=addSeek;
	}
	for (i=0;i<NL;i++) for (j=0;j<layers[i].NN;j++) {
		CopyMemory(seek,layers[i].neurons[j].W,addSeek=sizeof(double)*layers[i].neurons[j].NW); seek+=addSeek;
	}

	return fileSize;
}
