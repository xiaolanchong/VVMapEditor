// memtest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../zlibex/zip.h"
#include "../zlibex/iomem.h"

#include "ZipFileSystem.h"
#include "MemStream.h"

#include <fstream>
#include <time.h>
/*
const char szTest[] = "Test string!!!";*/
/*
const char szFile0[] = "test_mem.zip";
const char szFile1[] = "test_file.zip";*/
/*
const char szInnerFile1[] = "DirEx2\\NewFile3ex.txt";
const char szInnerFile2[] = "NewFile3ex.txt";*/

class CrtAlloc : public IMalloc
{
    STDMETHOD (QueryInterface)(REFIID riid, void **ppvObj)
	{
	    if (IsEqualIID(riid, IID_IMalloc)	|| 
			IsEqualIID(riid, IID_IUnknown) )
	    {
	        *ppvObj=this;
	        return S_OK;
	    }
		else
		{
			*ppvObj = NULL;
			return E_NOINTERFACE;
		}
	}
	
    STDMETHOD_(ULONG, AddRef)() { return 2;};
    STDMETHOD_(ULONG, Release)() { return 1;};
	virtual SIZE_T STDMETHODCALLTYPE GetSize(  void *pv)  { return 0;}
        
	virtual int STDMETHODCALLTYPE DidAlloc( void *pv) { return 0;}
        
	virtual void STDMETHODCALLTYPE HeapMinimize( void) {}

	
	virtual void* STDMETHODCALLTYPE	Alloc( ULONG uSize )	
	{
		return malloc( uSize );
	}
	virtual   void* STDMETHODCALLTYPE	Realloc( void* pOldBuffer, ULONG uNewSize)
	{
		return realloc( pOldBuffer, uNewSize);
	}
	virtual   void STDMETHODCALLTYPE	Free( void* pBuffer )
	{
		free(pBuffer);
	}
};

void WriteToMemory( const char* szFileName )
{
	zlib_filefunc_def filefunc;
	zipFile hFile;
	HGLOBAL hGlobal = GlobalAlloc( GHND, 0 );
	int size;
	const char* ptr;

	ULONG	nRef = 0;
	CrtAlloc Alloc;
	IMemStream* pStream = CreateStreamOnMalloc( &Alloc, 0, 0  );
	CreateOnStream( &filefunc );
#if 0
	hFile = zipOpen2( (const char*)pStream, APPEND_STATUS_CREATE | 0, 0, &filefunc);
	zipOpenNewFileInZip( hFile, szInnerFile1, 0, 0 ,0 , 0, 0, 0, Z_DEFLATED, Z_DEFAULT_COMPRESSION );

	zipWriteInFileInZip( hFile, szTest, strlen(szTest) );
	zipCloseFileInZip( hFile );
	zipClose(hFile, 0);

	STATSTG sstg;
	pStream->Stat( &sstg, 0 );
	size = sstg.cbSize.LowPart;
	pStream->GetPtr( (void**)&ptr );

	std::ofstream ZipStream( szFileName , std::ios_base::out );
	ZipStream.write( ptr, size );
#endif
	nRef = pStream->Release();
}

void WriteToFile(const char* szFileName)
{
	using namespace ZipFileSystem;
	ZipStorage Storage( szFileName , ZF_WRITE|ZF_APPEND);
/*	Storage.SetCurrentFile( szInnerFile1 );
	Storage.Write( szTest, strlen( szTest ) );
	Storage.SetCurrentFile( szInnerFile2 );
	Storage.Write( szTest, strlen( szTest ) );*/

	const int nFiles = 1;
	srand( time( 0) );
	int z = rand( ) ;

	time_t long_time;
	time( &long_time ); 

	const int c_nBufferSize = 5 * 1024 * 1024;
	std::vector<char>	TempBuffer( c_nBufferSize );
	//const char* szTime = asctime( &long_time );
	std::string FileName;
	char buf[200];

	for( int i =0 ; i < TempBuffer.size(); ++i)
	{
		TempBuffer[i] = (rand()%('Z' - 'A')) + 'A';
	}
	for( i = 0 ; i < nFiles; ++i)
	{
		int rnd = rand()%500;
		_snprintf( buf, 200, "file_%d.txt", rnd );
		FileName = buf;
		Storage.SetCurrentFile( FileName.c_str() );
		Storage.Write( &TempBuffer[0], TempBuffer.size() );
	}
}

int main(int argc, char* argv[])
{
	WriteToFile("test_file.zip");
	WriteToMemory("test_mem.zip");

	return 0;
}
