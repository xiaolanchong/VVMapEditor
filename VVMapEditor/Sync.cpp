// Sync.cpp: implementation of the CSync class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Sync.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CSync::CSync()
{
	::InitializeCriticalSection(&m_cs);
}

CSync::~CSync()
{
	::DeleteCriticalSection(&m_cs);
}


bool	CSync  ::Try() 
{
	return ::TryEnterCriticalSection(&m_cs) == TRUE;
//	return false;
}

void	CSync  ::Lock()
{
	::EnterCriticalSection(&m_cs);
}

void	CSync  ::Unlock()
{
	::LeaveCriticalSection(&m_cs);
}
