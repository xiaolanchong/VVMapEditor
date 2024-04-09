//======================= Copyright © 2004, Eugene V. Gorbachev =================
//
// Purpose: IO base function header for compress/uncompress .zip
//			  files using zlib + zip or unzip API
//			This IO API version uses memory read/write
//
//=============================================================================

#ifndef IOMEM_H
#define IOMEM_H
#ifdef _WIN32
#pragma once
#endif

#include <objbase.h>

#ifdef __cplusplus
extern "C" {
#endif
/*
// create zlib_filefunc_def stucture on the memory
void	create_mem_write_filefunc (zlib_filefunc_def* pzlib_filefunc_def);

void	create_mem_read_filefunc (zlib_filefunc_def* pzlib_filefunc_def, 
								void* pBuffer, 
								uLong ulSize);

void	destroy_mem_filefunc(zlib_filefunc_def* pzlib_filefunc_def);

// get buffer size
uLong	get_mem_buffer_size (zlib_filefunc_def* pzlib_filefunc_def);

// get buffer start pointer
void*	get_mem_buffer_ptr (zlib_filefunc_def* pzlib_filefunc_def);
*/


void	CreateOnStream( zlib_filefunc_def* pzlib_filefunc_def );	

#ifdef __cplusplus
}
#endif

#endif // IOMEM_H
