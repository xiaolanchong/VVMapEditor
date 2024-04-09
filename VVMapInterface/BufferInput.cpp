// BufferInput.cpp: implementation of the CBufferInput class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"



#include "BufferInput.h"
#include "memleak.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBufferInput::CBufferInput( const VVMInfo * _pinfo) :Buffer (0), Pointer (0), Size(0), info (_pinfo)
{
}

CBufferInput::~CBufferInput()
{

}

CBufferInput::CBufferInput(char* p,long s, const VVMInfo * _pinfo) 
	:Buffer (p), Pointer (0), Size(s), info (_pinfo) 
{
}

long CBufferInput::CheckString(const char *S)
{
	skip();
	long L=strlen(S);
	long i;
	for(i=0;i<L;i++)
		if ((Pointer+i>=Size) || (Buffer[Pointer+i]!=S[i]))
													return WRONG_FORMAT;
	Pointer+=L;
	return Pointer;
}

long CBufferInput::GetNumString(char **S)
{
	skip();
	long L=0;
	while((
		(Buffer[Pointer]=='-')||
		(Buffer[Pointer]=='e')||
		(Buffer[Pointer]=='+')||
		(Buffer[Pointer]=='.')||
		((Buffer[Pointer]>='0')&&(Buffer[Pointer]<='9'))
		)&&(Pointer<Size)){
		Pointer++;
		L++;
	}
	char *s;
	s=new char[L+1];
	s[L]=0;
	CopyMemory(s,Buffer+Pointer-L,L);
	*S=s;
	return 0;
}

void CBufferInput::skip()
{
	while((Pointer<Size)&&(Buffer[Pointer]<=0x0f))Pointer++;
}

long CBufferInput::GetLong(long *p)
{
	char *S;
	GetNumString(&S);
	if(strlen(S)==0)return WRONG_FORMAT;
	long err=sscanf(S,"%d",p);
	delete[] S;
	if(err!=1)return WRONG_FORMAT;
	else return Pointer;
}

long CBufferInput::GetDouble(double *p)
{
	char*S;
	GetNumString(&S);
	if(strlen(S)==0)return WRONG_FORMAT;
	*p=atof(S);
	delete[] S;
	/*if(err!=1)return WRONG_FORMAT;
	else*/ return Pointer;
}

long CBufferInput::GetBuffer(PBYTE b, long n)
{
	CopyMemory(b,Buffer+Pointer,n);
	Pointer+=n;
	return Pointer;
}

void CBufferInput::Skip(char *S)
{
	Pointer+=strlen(S);
}

void CBufferInput::Skip(long S)
{
	Pointer+=S;
}

static unsigned my_strcmp (const char * what, const char * where, unsigned size_of_where)
// возвращает число совпавших символов 
{
	unsigned n = 0;
	while ( (n < size_of_where) && what[n] && where[n] && (what[n] == where[n]) )  n++; 
	return n;
		
}

void CBufferInput::goto_text(const char * text)
{
	unsigned slen = strlen( text );

	while (!is_end())
	{
		const char * t = Buffer + Pointer;
		
		unsigned rlen = Size - Pointer;
		if (rlen < slen) break;
		
		unsigned nn = my_strcmp( text, t, rlen);
		if (nn == slen) return; //найдено
		
		Pointer += nn+1;
	}
	throw std::string(text);
}

bool CBufferInput::find(const char * text)
{
	long p = Pointer;
	Pointer = 0;
	try
	{
		goto_text(text);
		return true;
	}
	catch (std::string&)
	{
		Pointer = p; 
		return false;
	}
}

bool CBufferInput::find_down(const char * text)
{
	long p = Pointer;
	try
	{
		goto_text(text);
		return true;
	}
	catch (std::string&)
	{
		Pointer = p;
		return false;
	}
}
