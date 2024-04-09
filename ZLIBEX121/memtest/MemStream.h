//======================= Copyright © 2004, Eugene V. Gorbachev =================
//
// Purpose: creates stream on IMalloc memory allocation
//
//=============================================================================

#ifndef MEMSTREAM_H
#define MEMSTREAM_H
#ifdef _WIN32
#pragma once
#endif

#include <objbase.h>

struct IMemStream : public IStream
{
	//NOTENOTE: returns pointer to internal memory buffer
    virtual HRESULT STDMETHODCALLTYPE GetPtr( void** ppBuffer ) = 0;
};

// IMalloc::Alloc, IMalloc::Realloc, IMalloc::Free required, all other ignore
IMemStream*	CreateStreamOnMalloc(	IMalloc* pAlloc,	// memory allocation stuff
									BYTE* pBuffer = 0,	// start on valid buffer /when read/
									UINT uSize = 0 );	// size of valid buffer

#endif // MEMSTREAM_H
