// BufferInput.h: interface for the CBufferInput class.
//
//////////////////////////////////////////////////////////////////////

#ifndef AFX_BUFFERINPUT_H__INCLUDED_
#define AFX_BUFFERINPUT_H__INCLUDED_

#ifndef __CHECK
#define __CHECK(str) if(Input.CheckString(str)==WRONG_FORMAT)_ERROR(ERR_INVALID_FORMAT);
#endif

#define WRONG_FORMAT (-99)
#include <string>

#include "VVMSettings.h"


class CBufferInput  
{
public:
	void Skip(long S);
	void Skip(char *S);
	void skip();

	long GetBuffer(PBYTE b,long n);
	long GetDouble(double *p);
	long GetLong(long *p);
	
	long GetNumString(char **S);
	long CheckString(const char* S);
	
	long Pointer;
	char * Buffer;
	long Size;

	CBufferInput(char* p,long s, const VVMInfo * _pinfo);
	CBufferInput( const VVMInfo * _pinfo);

	virtual ~CBufferInput();

	void check(std::string str)
	{
		if (CheckString(str.c_str())==WRONG_FORMAT) throw std::string(str);
	}

	long version;
	const VVMInfo * info;

	void goto_text(const char * text);
	bool find(const char * text);
	bool find_down(const char * text);

	bool is_end () const 
	{ 
		return (Pointer >= Size) || (Pointer < 0);
	}

	const char * pos() const { return Buffer + Pointer; }

};

#endif 
