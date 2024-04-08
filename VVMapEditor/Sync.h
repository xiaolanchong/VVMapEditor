//========================= Copyright © 2004, Elvees ==========================
// Author: Eugene V. Gorbachev
// Purpose: Helper class for CRITICAL_SECTION & TryEnterCriticalSection
//
//=============================================================================



// Sync.h: interface for the CSync class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYNC_H__BB9701F9_C033_4A17_9BA2_2E2B7DE77084__INCLUDED_)
#define AFX_SYNC_H__BB9701F9_C033_4A17_9BA2_2E2B7DE77084__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// CCriticalSection don't have TryEnter
// critical section lightweight( not kernel) sync object, so use for syncs

//#include <boost/utility.hpp>

//-----------------------------------------------------------------------------
// Purpose: For sync purpose like threading shedulers
//-----------------------------------------------------------------------------
class CSync /* : boost ::noncopyable*/
{
	CRITICAL_SECTION	m_cs;
public:
	
	CSync();
	~CSync();

	bool	Try() ;
	void	Lock();
	void	Unlock();
};

#endif // !defined(AFX_SYNC_H__BB9701F9_C033_4A17_9BA2_2E2B7DE77084__INCLUDED_)
