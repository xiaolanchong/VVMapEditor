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
//NOTENOTE: if you specified pBuffer && bReadOnly != TRUE, so allocate it with pAlloc!!!
//NOTENOTE: or on Realloc we 'll have big troubles!!!
IMemStream*	CreateStreamOnMalloc(	IMalloc* pAlloc,	// memory allocation stuff
									UINT  uMemoryBlock, // memory allocation block size
									BYTE* pBuffer,	// start on valid buffer /when read/
									UINT uSize,		// size of start buffer
									BOOL bReadOnly);	// 


#endif // MEMSTREAM_H
