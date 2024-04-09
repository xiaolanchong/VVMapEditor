#include <stdlib.h>

#include "zlib.h"
#include "ioapi.h"
#include "iomem.h"
#include <windows.h>



#if defined _DEBUG && defined __WIN32
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#ifdef USE_CRT_HEAP

typedef struct
{
    unsigned char*	m_pBuffer;
    uLong			m_ulSize;
	BOOL			m_bReadOnly;
	//NOTENOTE: current pointer for read/write
	uLong			m_ulCurrentOffset;
	int				m_iError;
} MemFile_t ;

typedef MemFile_t*	PMemFile_t;

#define MEM_BLOCK_SIZE	128 * 1024 * sizeof(unsigned char)	// 128k

BOOL	GrowBuffer( MemFile_t* pMemFile, uLong ulRequired )
{
	if( ulRequired > pMemFile->m_ulSize)
	{
		//NOTENOTE: double buffer size
		pMemFile->m_pBuffer = realloc( pMemFile->m_pBuffer,  pMemFile->m_ulSize * 2);
	}
	return pMemFile->m_pBuffer != 0;
}

///// some debug stuff

#ifdef _DEBUG
#define	DbgOutput(x)	OutputDebugStringA( x "\n" )
#define DbgBreak()		DebugBreak();
_inline void Assert(BOOL x) 
{ 
//	if(!x) _asm int 3; 
}
#else	
#define	DbgOutput(x) 
#define DbgBreak()		;
#endif
//#define CHECK_PTR(x)

//NOTENOTE: init our memory structure
voidpf ZCALLBACK mem_open_file_func
(
   voidpf opaque,				// our MemFile_t
   const char* filename,		// ignore
   int mode						// 
)
{
	MemFile_t* pMemFile;
	if( !opaque) return NULL;
	if( IsBadReadPtr( opaque, sizeof( MemFile_t ))  ) return NULL;

	pMemFile = (MemFile_t*) opaque; 

    if ( (mode & ZLIB_FILEFUNC_MODE_WRITE) && pMemFile->m_bReadOnly)
    {
		DbgOutput("Failed to open read-only buffer for write");
		return NULL;
    }
	else if( (mode & ZLIB_FILEFUNC_MODE_READ) && !pMemFile->m_bReadOnly )
	{
		DbgOutput("Failed to open write-only buffer for read");
	}
	else
	{
		DbgOutput("You must specify read or write flag for read");
		DbgBreak();
	}
	return (void*)pMemFile;
}

uLong ZCALLBACK mem_read_file_func (
									   voidpf opaque,
									   voidpf stream,
									   void* buf,
									   uLong size
								  )
{
	MemFile_t* pMemFile;
	uLong ret;
	if( !stream) return 0;
	if( IsBadReadPtr( stream, sizeof( MemFile_t ))  ) return 0;

	pMemFile = (MemFile_t*) stream; 
    
	if( !pMemFile->m_bReadOnly )
	{
		DbgOutput("Failed to read from write only memory");
		return 0;
	}

	ret = ( pMemFile->m_ulCurrentOffset + size >  pMemFile->m_ulSize ) ? 
			pMemFile->m_ulSize - pMemFile->m_ulCurrentOffset : 
			size;
	memcpy( buf, pMemFile->m_pBuffer +  pMemFile->m_ulCurrentOffset, ret );
	pMemFile->m_ulCurrentOffset += ret;
    return ret;
}


uLong ZCALLBACK mem_write_file_func (
   voidpf opaque,
   voidpf stream,
   const void* buf,
   uLong size
   )
{
 //   uLong ret;
	MemFile_t* pMemFile = (MemFile_t*) stream; 

	if( !pMemFile->m_bReadOnly) GrowBuffer(pMemFile, pMemFile->m_ulCurrentOffset + size) ;
	else return 0;

	memcpy( pMemFile->m_pBuffer +  pMemFile->m_ulCurrentOffset, buf, size );
	pMemFile->m_ulCurrentOffset += size;

	Assert(pMemFile->m_ulCurrentOffset < 100);
    return size;
}

long ZCALLBACK mem_tell_file_func (opaque, stream)
   voidpf opaque;
   voidpf stream;
{
    long ret;
    ret = ((MemFile_t *)stream)->m_ulCurrentOffset;
    return ret;
}

long ZCALLBACK mem_seek_file_func (opaque, stream, offset, origin)
   voidpf opaque;
   voidpf stream;
   uLong offset;
   int origin;
{
    int fseek_origin=0;
    long ret;
	MemFile_t* pMemFile = (MemFile_t*) stream;

	switch( origin )
	{
	case ZLIB_FILEFUNC_SEEK_END :
	{
		ret = ( offset > pMemFile->m_ulSize ) ? pMemFile->m_ulSize : offset;
		pMemFile->m_ulCurrentOffset = pMemFile->m_ulSize - ret;
		Assert(pMemFile->m_ulCurrentOffset < 100);
		break;
	}
	case  ZLIB_FILEFUNC_SEEK_SET:
	{
		if( !pMemFile->m_bReadOnly) GrowBuffer( pMemFile, offset );
//		ret = ( offset > pMemFile->m_ulSize ) ? pMemFile->m_ulSize : offset;
		ret = offset;
		pMemFile->m_ulCurrentOffset = ret;
		Assert(pMemFile->m_ulCurrentOffset < 100);
		break;
	}
	case ZLIB_FILEFUNC_SEEK_CUR:
	{
		if( !pMemFile->m_bReadOnly ) GrowBuffer( pMemFile, pMemFile->m_ulCurrentOffset + offset );
		pMemFile->m_ulCurrentOffset += offset;
		ret = offset;
		Assert(pMemFile->m_ulCurrentOffset < 100);
		break;
	}
	default: pMemFile->m_iError = -1; return -1;
	}
    return 0;
}

int ZCALLBACK mem_close_file_func (opaque, stream)
   voidpf opaque;
   voidpf stream;
{
    int ret = 0;
    return ret;
}

int ZCALLBACK mem_error_file_func (opaque, stream)
   voidpf opaque;
   voidpf stream;
{
  //  int ret;
    return ((MemFile_t *)stream)->m_iError;
 //   return ret;
}

void create_mem_read_filefunc ( zlib_filefunc_def* pzlib_filefunc_def, void* pBuffer, uLong ulSize )
{
	//NOTENOTE: for memory leak detection
	MemFile_t *pMemFile;
#ifdef _DEBUG
	_CrtSetDbgFlag( _CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF );
#endif	
    pzlib_filefunc_def->zopen_file	= mem_open_file_func;
    pzlib_filefunc_def->zread_file	= mem_read_file_func;
    pzlib_filefunc_def->zwrite_file = mem_write_file_func;
    pzlib_filefunc_def->ztell_file	= mem_tell_file_func;
    pzlib_filefunc_def->zseek_file	= mem_seek_file_func;
    pzlib_filefunc_def->zclose_file = mem_close_file_func;
    pzlib_filefunc_def->zerror_file = mem_error_file_func;

	pMemFile = malloc( sizeof(MemFile_t) );
	if( pMemFile )
	{
		pMemFile->m_pBuffer = malloc( MEM_BLOCK_SIZE );
		pMemFile->m_ulSize	= MEM_BLOCK_SIZE;
		pMemFile->m_iError	= 0;
		pMemFile->m_ulCurrentOffset = 0;
		pMemFile->m_bReadOnly = TRUE;
		if( !pMemFile->m_pBuffer  )
		{
			free(pMemFile);
			pMemFile = NULL;
		}
	}
    pzlib_filefunc_def->opaque		= pMemFile;	
}

void create_mem_write_filefunc ( zlib_filefunc_def* pzlib_filefunc_def )
{
	PMemFile_t pMemFile  ;
	//NOTENOTE: for memory leak detection
#ifdef _DEBUG
	_CrtSetDbgFlag( _CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF );
#endif	
    pzlib_filefunc_def->zopen_file	= mem_open_file_func;
    pzlib_filefunc_def->zread_file	= mem_read_file_func;
    pzlib_filefunc_def->zwrite_file = mem_write_file_func;
    pzlib_filefunc_def->ztell_file	= mem_tell_file_func;
    pzlib_filefunc_def->zseek_file	= mem_seek_file_func;
    pzlib_filefunc_def->zclose_file = mem_close_file_func;
    pzlib_filefunc_def->zerror_file = mem_error_file_func;

	pMemFile = malloc( sizeof(MemFile_t) );
	if( pMemFile )
	{
		pMemFile->m_pBuffer = malloc( MEM_BLOCK_SIZE );
		pMemFile->m_ulSize	= MEM_BLOCK_SIZE;
		pMemFile->m_iError	= 0;
		pMemFile->m_ulCurrentOffset = 0;
		pMemFile->m_bReadOnly = FALSE;
		if( !pMemFile->m_pBuffer  )
		{
			free(pMemFile);
			pMemFile = NULL;
		}
	}
    pzlib_filefunc_def->opaque		= pMemFile;
}

uLong get_mem_buffer_size(  zlib_filefunc_def* pzlib_filefunc_def )
{
	MemFile_t* pMemFile = (MemFile_t*) pzlib_filefunc_def->opaque;

	Assert(pMemFile->m_ulCurrentOffset < 100);
	return pMemFile->m_ulCurrentOffset;
}

void* get_mem_buffer_ptr(  zlib_filefunc_def* pzlib_filefunc_def )
{
	MemFile_t* pMemFile = (MemFile_t*) pzlib_filefunc_def->opaque;

	return pMemFile->m_pBuffer;
}

void	destroy_mem_filefunc(zlib_filefunc_def* pzlib_filefunc_def)
{
	MemFile_t* pMemFile = (MemFile_t*) pzlib_filefunc_def->opaque;
	if(pzlib_filefunc_def->opaque)
	{
		if( !pMemFile->m_bReadOnly) free(pMemFile->m_pBuffer);
		free(pzlib_filefunc_def->opaque);
		pzlib_filefunc_def->opaque = NULL;
	}
}

#else

typedef struct
{
    IStream*		m_pStream;		
	HRESULT			m_hError;
} MemFile_t ;

typedef MemFile_t*	PMemFile_t;

///// some debug stuff

#ifdef _DEBUG
#define	DbgOutput(x)	OutputDebugStringA( x "\n" )
#define DbgBreak()		DebugBreak();
_inline void Assert(BOOL x) 
{ 
//	if(!x) _asm int 3; 
}
#else	
#define	DbgOutput(x) 
#define DbgBreak()		;
#endif
//#define CHECK_PTR(x)

//NOTENOTE: init our memory structure
voidpf ZCALLBACK mem_open_file_func
(
   voidpf opaque,				// our MemFile_t
   const char* filename,		// ignore
   int mode						// 
)
{
/*
    if ( (mode & ZLIB_FILEFUNC_MODE_WRITE) )
    {
		DbgOutput("Failed to open read-only buffer for write");
		return NULL;
    }
	else if( (mode & ZLIB_FILEFUNC_MODE_READ)  )
	{
		DbgOutput("Failed to open write-only buffer for read");
	}
	else
	{
		DbgOutput("You must specify read or write flag for read");
		DbgBreak();
	}*/
	MemFile_t * pMemFile	= malloc( sizeof( MemFile_t ));
	pMemFile->m_pStream		= (IStream*) filename;
	pMemFile->m_pStream->lpVtbl->AddRef(pMemFile->m_pStream);
	pMemFile->m_hError		= S_OK;
	return (void*)pMemFile;
}

uLong ZCALLBACK mem_read_file_func (
									   voidpf opaque,
									   voidpf stream,
									   void* buf,
									   uLong size
								  )
{
	MemFile_t* pMemFile = ( MemFile_t* ) stream;
	
	ULONG cbRead;
	pMemFile->m_hError = pMemFile->m_pStream->lpVtbl->Read( pMemFile->m_pStream, buf, size, &cbRead );
    return cbRead;
}


uLong ZCALLBACK mem_write_file_func (
   voidpf opaque,
   voidpf stream,
   const void* buf,
   uLong size
   )
{
 //   uLong ret;
	MemFile_t* pMemFile = ( MemFile_t* ) stream;
	
	ULONG cbWritten;
	pMemFile->m_hError = pMemFile->m_pStream->lpVtbl->Write( pMemFile->m_pStream, buf, size, &cbWritten );
    return cbWritten;
}

long ZCALLBACK mem_tell_file_func (
   voidpf opaque,
   voidpf stream)
{
	MemFile_t* pMemFile = ( MemFile_t* ) stream;

	STATSTG sstg;
	pMemFile->m_hError = pMemFile->m_pStream->lpVtbl->Stat( pMemFile->m_pStream, &sstg, STATFLAG_NONAME  );
	return sstg.cbSize.LowPart;
}

long ZCALLBACK mem_seek_file_func (
   voidpf opaque,
   voidpf stream,
   uLong offset,
   int origin)
{
	MemFile_t* pMemFile = (MemFile_t*) stream;
	LARGE_INTEGER iPos ;
	STREAM_SEEK sseek;	
	iPos.HighPart	= 0;
	iPos.LowPart	= offset;

	switch( origin )
	{
	case ZLIB_FILEFUNC_SEEK_END :
	{
		sseek = STREAM_SEEK_END;
		break;
	}
	case  ZLIB_FILEFUNC_SEEK_SET:
	{
		sseek = STREAM_SEEK_SET;
		break;
	}
	case ZLIB_FILEFUNC_SEEK_CUR:
	{
		sseek = STREAM_SEEK_CUR;
		break;
	}
	default: pMemFile->m_hError = E_FAIL; return -1;
	}

//	pMemFile->m_pStream->Seek()
	pMemFile->m_hError = pMemFile->m_pStream->lpVtbl->Seek( pMemFile->m_pStream, iPos, sseek, 0 );
    return 0;// FAILED(pMemFile->m_hError);
}

int ZCALLBACK mem_close_file_func (
   voidpf opaque,
   voidpf stream)
{
	MemFile_t* pMemFile = (MemFile_t*) stream;
	STATSTG sstg;
	pMemFile->m_hError = pMemFile->m_pStream->lpVtbl->Stat( pMemFile->m_pStream, &sstg, STATFLAG_NONAME  );

	pMemFile->m_pStream->lpVtbl->Release(pMemFile->m_pStream);
	free( pMemFile );
    return 0;
}

int ZCALLBACK mem_error_file_func (
   voidpf opaque,
   voidpf stream)
{
  //  int ret;
    return (int)(((MemFile_t *)stream)->m_hError);
 //   return ret;
}

void	CreateOnStream( zlib_filefunc_def* pzlib_filefunc_def )
{
#if defined _DEBUG && defined __WIN32
	_CrtSetDbgFlag( _CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF );
#endif	
    pzlib_filefunc_def->zopen_file	= mem_open_file_func;
    pzlib_filefunc_def->zread_file	= mem_read_file_func;
    pzlib_filefunc_def->zwrite_file = mem_write_file_func;
    pzlib_filefunc_def->ztell_file	= mem_tell_file_func;
    pzlib_filefunc_def->zseek_file	= mem_seek_file_func;
    pzlib_filefunc_def->zclose_file = mem_close_file_func;
    pzlib_filefunc_def->zerror_file = mem_error_file_func;
    pzlib_filefunc_def->opaque		= NULL;		
}

#endif