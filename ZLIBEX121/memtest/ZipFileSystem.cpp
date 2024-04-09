// ZipFileSystem.cpp: implementation of the ZipFileSystem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ZipFileSystem.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace ZipFileSystem  
{

ZipStorage::ZipFile::ZipFile( ZipStorage& Storage): 
	m_Storage(Storage)
{
		
}

bool	ZipStorage::ZipFile::Open(const char* Name)
{
	if(IsOpen()) Close();
	int err = zipOpenNewFileInZip( m_Storage.m_ZipFile, Name, 0, 0 ,0 , 0, 0, 0, Z_DEFLATED, 
									Z_BEST_SPEED );
	if( !err) m_Name = Name;	
	return !err;
}

void	ZipStorage::ZipFile::Close()
{
	bool bOldFileNameValid	= !m_Name.empty();
	if(  bOldFileNameValid )
	{
		zipCloseFileInZip( m_Storage. m_ZipFile );
		m_Name = ""; 
	}	
}

#pragma warning (disable : 4355)

ZipStorage::ZipStorage(const char* Name, unsigned int Mode) : m_ZipFile(0), m_File(*this)
{
	Open(Name, Mode);
}

ZipStorage::ZipStorage(): m_ZipFile(0), m_File(*this)
{
	
}

_inline CheckMode( unsigned int Mode, unsigned int Flags )
{
	return (Mode & Flags) == Flags;
}

bool	ZipStorage::Open( const char* Name, unsigned int Mode )
{
	if( IsOpen() ) 
		Close();
	int AppendMode = 0;
	if( CheckMode ( Mode,  ZF_WRITE | ZF_READ ) ) return false;
	if( CheckMode ( Mode , ZF_CREATE | ZF_READ )) return false;
	if( CheckMode ( Mode , ZF_APPEND | ZF_READ) ) return false;
	if( Mode & ZF_CREATE )
	{
		AppendMode |= APPEND_STATUS_CREATE;
	}
	if(  Mode & ZF_APPEND )
	{
		Mode |= APPEND_STATUS_ADDINZIP;
	}
/*	if( Mode & ZF_READ_ONLY )
	{
		
	}*/
	m_ZipFile = zipOpen2( Name , AppendMode , 0, 0);
	return m_ZipFile != 0;
}

void	ZipStorage::Close()
{
	m_File.Close();
	zipClose(m_ZipFile, 0);
	m_ZipFile = 0;
}

bool	ZipStorage::SetCurrentFile( const char* Name )
{
//	m_File.Close();
	return m_File.Open( Name );
}

size_t	ZipStorage::Write( const void* pBuffer, size_t Size )
{
	if(!m_File.IsOpen() ) return 0;
	return zipWriteInFileInZip( m_ZipFile, pBuffer, Size);
}

size_t	ZipStorage::Read( void* pBuffer, size_t Size )
{
	if(!m_File.IsOpen()) return 0;
	return 0;
}
	
ZipStorage::~ZipStorage()
{
	Close();
}

}
