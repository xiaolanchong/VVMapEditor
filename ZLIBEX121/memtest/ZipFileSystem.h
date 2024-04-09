// ZipFileSystem.h: interface for the ZipFileSystem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZIPFILESYSTEM_H__BE15326C_6B30_4F49_ABD1_1D141F7F6CB5__INCLUDED_)
#define AFX_ZIPFILESYSTEM_H__BE15326C_6B30_4F49_ABD1_1D141F7F6CB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../zlibex/zip.h"
#include "../zlibex/unzip.h"

#include <vector>

namespace ZipFileSystem  
{

class	ZipStorage;
//struct	ZipFile;

enum ZipOpenFlag_t
{
	ZF_READ			=	1 << 1,
	ZF_WRITE		=	1 << 2,

	ZF_CREATE		=	1 << 3,
	ZF_APPEND		=	1 << 4,
};

class	ZipStorage
{	
	
struct ZipFile
{	
	ZipFile( ZipStorage& Storage );

	ZipStorage&	m_Storage;
	std::string m_Name;
	bool	IsOpen() { return !m_Name.empty();}
	bool	Open(const char* Name);
	void	Close();
	
}	m_File;

	friend struct ZipFile;
	zlib_filefunc_def	m_FileFunc;
	zipFile				m_ZipFile;

public:
	ZipStorage( const char* Name, unsigned int Mode);
	ZipStorage();
	bool	Open( const char* Name, unsigned int Mode );
	bool	IsOpen() const { return m_ZipFile != 0; }
	void	Close();

	bool	SetCurrentFile( const char* Name );
	size_t	Write( const void* pBuffer, size_t Size );
	size_t	Read( void* pBuffer, size_t Size );

	virtual ~ZipStorage();
};
/*
class	ZipFile
{
	friend class ZipStorage;
	ZipStorage&		m_Storage;
	std::string		m_Name;
public:	
	ZipFile(ZipStorage& Storage, const char* Name);
	virtual ~ZipFile();	
}
*/
};

#endif // !defined(AFX_ZIPFILESYSTEM_H__BE15326C_6B30_4F49_ABD1_1D141F7F6CB5__INCLUDED_)
