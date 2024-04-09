// Buffer.cpp: implementation of the CBuffer class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "buffer.h"
#include "memleak.h"

#ifdef UNIX_RELEASE
#define _snprintf snprintf
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define START_SIZE 0x10000

CBuffer::CBuffer()
{
	Size=START_SIZE;
	Buffer=new char[Size];
	NoMemory=(Buffer==NULL);
	Pointer=0;
}

CBuffer::~CBuffer()
{
	delete[]Buffer;
}

long CBuffer::AddString(char *S)
{
	return AddBuffer(S,strlen(S));
}

long CBuffer::AddBuffer(void *b, long n)
{
	if(NoMemory)return -1;
	while(Pointer+n>=Size)Enlarge();
	if(NoMemory)return -1;
	CopyMemory(Buffer+Pointer,b,n);
	Pointer+=n;
	return Pointer;
}

long CBuffer::NewLine()
{
	if(Pointer+2>=Size)Enlarge();
	Buffer[Pointer++]=0x0D;
	Buffer[Pointer++]=0x0A;
	return Pointer;
}

void CBuffer::Enlarge()
{
	Size*=2;
	char*tmp=new char[Size];
	NoMemory=(tmp==NULL);
	if(NoMemory)return;
	CopyMemory(tmp,Buffer,Pointer);
	delete[]Buffer;
	Buffer=tmp;
}

long CBuffer::AddLong(long n)
{
	char S[12];
	sprintf(S,"%d",n);
	return AddString(S);
}

long CBuffer::AddLongs(long *p, long n)
{
	char S[12];
	long i;
	for(i=0;i<n;i++){
		sprintf(S,"%d",p[i]);
		AddString(S);
		NewLine();
	}
	return Pointer;
}

long CBuffer::AddDouble(double d)
{
	const int buff_size = 25;
	char S[buff_size];
	_snprintf(S, buff_size, "%.6g",d);
	return AddString(S);
}

long CBuffer::AddDoubles(double *p, long n)
{
	const int buff_size = 25;
	char S[buff_size];
	long i;
	for(i=0;i<n;i++)
	{
		_snprintf(S, buff_size, "%.6g",p[i]);
		AddString(S);
		NewLine();
	}
	return Pointer;
}

long CBuffer::Line_long(char *s, long n)
{
	AddString(s);
	AddLong(n);
	return NewLine();
}

void CBuffer::Allocate(long size)
{
	while(Pointer+size>Size)Enlarge();
}
