//========================= Copyright © 2004, Elvees ==========================
// Author: Eugene V. Gorbachev
// Purpose: Helper class for VVMapInterface error detecting
//
//=============================================================================


// VVError.h: interface for the VVError class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VVERROR_H__1A832D4E_6AA2_4CAB_AE79_6A9868EF7FD9__INCLUDED_)
#define AFX_VVERROR_H__1A832D4E_6AA2_4CAB_AE79_6A9868EF7FD9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Exception.h"

MACRO_EXCEPTION(VVMapTransException, CommonException)

class CVVMapTrans;

//-----------------------------------------------------------------------------
// Purpose: Throws VVMapTransException when error occured
//-----------------------------------------------------------------------------
class VVError  
{
	long			m_Error;
	CVVMapTrans*	m_VVInt;
public:
	VVError(CVVMapTrans* vv) : m_VVInt(vv), m_Error(0) {};

	long	GetError() const { return m_Error;}

	// @throw VVMapTransException
	void operator = (long res) ;

};

#endif // !defined(AFX_VVERROR_H__1A832D4E_6AA2_4CAB_AE79_6A9868EF7FD9__INCLUDED_)

