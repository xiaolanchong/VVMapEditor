

#include "stdafx.h"
//#include <wdm.h>

#include "sys.h"

namespace sys
{


	void ExchangeLongs( long& out, const long& in)
	{
		InterlockedExchange( &out, in);
	}

	CCritSect::CCritSect ()
	{
		InitializeCriticalSection(&cs);
	}

	CCritSect::~CCritSect ()
	{
		DeleteCriticalSection(&cs);
	}


	
	CEnterCS::CEnterCS(CCritSect& _csc) : csc(_csc)
	{
		EnterCriticalSection(&csc.cs);
	}
	
	CEnterCS::~CEnterCS()
	{
		LeaveCriticalSection(&csc.cs);
	}

	CTryEnterCS::CTryEnterCS(CCritSect& _csc) : csc(_csc)
	{
		if (!TryEnterCriticalSection(&csc.cs)) throw crit_sect_occuped();
	}

	CTryEnterCS::~CTryEnterCS()
	{
		LeaveCriticalSection(&csc.cs);
	}
	
}