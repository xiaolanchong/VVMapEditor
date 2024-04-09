// Buffer.h: interface for the CBuffer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BUFFER_H__CA6AC0F0_B438_452A_A3E5_2FCA50A18BD7__INCLUDED_)
#define AFX_BUFFER_H__CA6AC0F0_B438_452A_A3E5_2FCA50A18BD7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <stdio.h>

class CBuffer  
{
public:
	void Allocate(long size);
	BOOL NoMemory;
	long Line_long(char *s,long n);
	long AddDoubles(double *p,long n);
	long AddDouble(double d);
	long AddLongs(long *p,long n);
	long AddLong(long n);
	void Enlarge();
	long NewLine();
	long AddBuffer(void *b,long n);
	long AddString(char *S);
	long Pointer;
	long Size;
	char *Buffer;
	CBuffer();
	virtual ~CBuffer();

};

#endif // !defined(AFX_BUFFER_H__CA6AC0F0_B438_452A_A3E5_2FCA50A18BD7__INCLUDED_)
