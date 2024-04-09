// SerializeHelper.cpp: implementation of the SerializeHelper class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "SerializeHelper.h"
#include "Buffer.h"
#include "BufferInput.h"
//#include <exception>
/*#include "../../3rd_party/zlibex121/zlibex/zip.h"
#include "../../3rd_party/zlibex121/zlibex/ioapi.h"
#include "../../3rd_party/zlibex121/zlibex/iomem.h"
#include "ZipFileSystem.h"
#include "CrtMalloc.h"
#include "MemStream.h"*/
//#include <fstream>

#include "memleak.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace SerializeHelper
{

class VVMapSerializeAssign
{
public:
	void operator = ( long res)
	{
		if( !res) throw res;
	}
};

const char*	AlignMapSaver::szAlignmapString = "AlignPoints=";
const int	c_nAlignMapVersion				= 12;

void	AlignMapSaver::Save(	CBuffer& Buffer, 
								const std::vector<long>&	PointNumbers,
								const std::vector<long>&	PointCoords)
{
	Buffer.Line_long(const_cast<char*>(szAlignmapString)		, PointNumbers.size());
	Buffer.AddLongs( const_cast<long*>(GetPtr(PointNumbers))		, PointNumbers.size());
	Buffer.AddLongs( const_cast<long*>(GetPtr(PointCoords))	,	PointCoords.size() );
}

bool	AlignMapSaver::Load(	long				FileVersion,
								CBufferInput&		Buffer,
								std::vector<long>&	PointNumbers,
								std::vector<long>&	PointCoords
							)
{
	try
	{
		if( c_nAlignMapVersion > FileVersion)
		{
			//NOTENOTE: just clear all and exit
			PointNumbers.clear();
			PointCoords.clear();
			return true;
		}
		long lPointNumber;
		VVMapSerializeAssign res;
		res = Buffer.CheckString(const_cast<char*>(szAlignmapString));
		res = Buffer.GetLong(&lPointNumber);
		const int c_nCoordsNumberInPoint = 2;
		PointNumbers.resize( lPointNumber);
		PointCoords.resize( lPointNumber * c_nCoordsNumberInPoint  );
		int i;
		for( i= 0; i < lPointNumber; ++i)
		{
			res = Buffer.GetLong( &PointNumbers[i] );
		}

		for( i= 0; i < lPointNumber; ++i)
		{
			res = Buffer.GetLong( &PointCoords[i * c_nCoordsNumberInPoint + 0] );
//			Buffer.Skip(2);
			res = Buffer.GetLong( &PointCoords[i * c_nCoordsNumberInPoint + 1] );
//			Buffer.Skip(2);
		}
	}
	catch( long/* err*/)
	{
		//NOTENOTE: data is invalid, clear all
		PointNumbers.clear();
		PointCoords.clear();
		return false;
	}
	return true;
}
/*
static const char szVersion[]			= "VVMAPInterface saved info. v";
static const char szZipHeader[]			= "PK\x3\x4";
static const char szNewVersionInfo[]	= "version=%d";
static const char szMainFile[]			= "Main.vme";
static const char szManifestFile[]		= "Manifest";
static const char szComment[]			= "VVMapInterface saved info. v%d";

#define SIZEOF(x) (sizeof(x)/sizeof(x[0] ))

bool SerializeBuffer::IsZipVersion(void *pBuffer, size_t Size) const throw(InvalidVersion)
{
	if( !pBuffer ) throw InvalidVersion();
	if( Size <= strlen(szZipHeader) ) throw InvalidVersion();

	const char *z = (LPCSTR)pBuffer;
	if( !memcmp( z, szZipHeader, strlen(szZipHeader) ) )
	{
		return true;
	}
	else if( Size > strlen(szVersion) && !memcmp( pBuffer, szVersion, strlen(szVersion) ) )
	{
//		Buffer.Skip( SIZEOF(szVersion) );
		return false;
	}
	throw InvalidVersion();
	return false;
}

bool	SerializeBuffer::ZipBuffer( const void * pBuffer, size_t Size, std::vector<unsigned char>& FilledBuffer  )
{
	DWORD				dwArcSize;
	BYTE*				pCompBuf;
#define ZIP_BUFFER
#ifdef ZIP_BUFFER
	zlib_filefunc_def	filefunc;
	zipFile				hFile;

	ULONG	nRef = 0;
	CrtAlloc Alloc;
	UINT c_nMemoryBlock = 5 * 1024 * 1024 ;// 5MB
	IMemStream* pStream = CreateStreamOnMalloc( &Alloc, c_nMemoryBlock, 0, 0, FALSE );
	CreateOnStream( &filefunc );

	hFile = zipOpen2( (const char*)pStream, APPEND_STATUS_CREATE, 0, &filefunc);
	zipOpenNewFileInZip( hFile, szMainFile, 0, 0 ,0 , 0, 0, 0, Z_DEFLATED, Z_DEFAULT_COMPRESSION );
	zipWriteInFileInZip( hFile, pBuffer, Size );
	zipCloseFileInZip( hFile );

	char buf[40];
	_snprintf( buf, 40, szNewVersionInfo, m_dwVersion);
	zipOpenNewFileInZip( hFile, szManifestFile, 0, 0 ,0 , 0, 0, 0, Z_DEFLATED, Z_DEFAULT_COMPRESSION );
	
	zipWriteInFileInZip( hFile, buf, strlen(buf) );

	_snprintf( buf, 40, szComment, m_dwVersion);
	zipCloseFileInZip( hFile  );	
	zipClose(hFile, buf);

	STATSTG sstg;
	pStream->Stat( &sstg, 0 );
	dwArcSize = sstg.cbSize.LowPart;
	pStream->GetPtr( (void**)&pCompBuf );

	FilledBuffer.assign( pCompBuf, pCompBuf + dwArcSize );

	pStream->Release();
#else
	pCompBuf = (BYTE*) pBuffer;
	FilledBuffer.assign( pCompBuf, pCompBuf + Size );
#endif
	return true;
}

bool	SerializeBuffer::UnzipBuffer( const void * pBuffer, size_t Size, std::vector<unsigned char>& FilledBuffer )
{
	zlib_filefunc_def	filefunc;
	unzFile				hFile;
//	DWORD				dwUncompSize;
//	BYTE*				pUncompBuf;
	
	ULONG	nRef = 0;
	CrtAlloc Alloc;
	UINT c_nMemoryBlock = 5 * 1024 * 1024 ;// 5MB
	IMemStream* pStream = CreateStreamOnMalloc( &Alloc, c_nMemoryBlock , 
												(BYTE*)pBuffer, Size,
												TRUE);
	CreateOnStream( &filefunc );

	int err;
	hFile = unzOpen2((const char*)pStream, &filefunc );

	err = unzLocateFile ( hFile, szMainFile, 0);
	unz_file_info FileInfo;
	err = unzGetCurrentFileInfo( hFile, &FileInfo, 0, 0, 0, 0, 0, 0);
	err = unzOpenCurrentFile(hFile);
	FilledBuffer.resize( FileInfo.uncompressed_size );
	unzReadCurrentFile( hFile, &FilledBuffer[0], FileInfo.uncompressed_size );
/*
	std::ofstream z("dump.bin", std::ios::binary);
	z.write( (const char*)&FilledBuffer[0], FileInfo.uncompressed_size );
	z.close();
	unzCloseCurrentFile(hFile);
	unzClose( hFile );
	
	pStream->Release();
	return false;
}*/
	
}
