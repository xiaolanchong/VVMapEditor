// VVError.cpp: implementation of the VVError class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VVMapEditor.h"
#include "VVError.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void VVError::operator = (long res) throw(VVMapTransException)
{
	if( res )
	{
		m_Error = res;
		throw VVMapTransException( m_VVInt->GetErrorString( res ) );
	}
}