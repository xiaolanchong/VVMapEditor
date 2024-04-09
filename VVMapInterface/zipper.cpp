#include "stdafx.h"
#include "zipper.h"

#include "../zlibex121/zlibex/zip.h"
#include "../zlibex121/zlibex/unzip.h"
#include "../zlibex121/zlibex/ioapi.h"
#include "../zlibex121/zlibex/iomem.h"

#include "memstream.h"
#include "crtmalloc.h"

#include "tchar.h"

int cbuff_count = 0;

namespace zipper
{


	

		CBuff::CBuff() : ptr(0), size(0) {}
		CBuff::CBuff(void * _ptr, size_t _size) : ptr( static_cast <unsigned char *> (_ptr)), size(_size) {} 
		CBuff::CBuff(unsigned char * _ptr, size_t _size) : ptr(_ptr), size(_size) {}
		CBuff::CBuff(size_t _size) : ptr( new unsigned char [_size]), size(_size) {cbuff_count++;}

		CBuff::~CBuff()
		{
			//release();
		}

		void CBuff::assign(void * _ptr, size_t _size)
		{
			ptr = (unsigned char *) _ptr;	size = _size;
		}

		void CBuff::copy_from(const void * _ptr, size_t _size)
		{
			ptr = new unsigned char [_size];	cbuff_count++;
			memcpy( ptr, _ptr, _size);
			size = _size;
			
		}

		void CBuff::release() 
		{
			if (ptr)  
			{ 
				delete [] ptr;	
				cbuff_count--;
			}
			ptr = 0;	size = 0;
		}

		void CBuff::resize(size_t new_size)
		{
			unsigned char * temp = new unsigned char [new_size];	cbuff_count++;
			
			if (ptr && size)
			{
				size_t size_to_copy = (new_size < size) ? (new_size) : (size);
				
				memcpy(temp, ptr, size_to_copy);
				
				if (size_to_copy < new_size) 
							memset(&temp[size_to_copy], 0, new_size - size_to_copy);
				release();
			}

			ptr = temp;			size = new_size;
		}

		void CBuff::concat(CBuff another_buff)
		{
			size_t new_size = size + another_buff.size;
			unsigned char * new_ptr = new unsigned char [new_size];

			if (ptr && size) 
					memcpy( new_ptr, ptr, size);
				
			if (another_buff.ptr && another_buff.size) 
					memcpy( new_ptr + size, another_buff.ptr, another_buff.size);
			
			release();
			ptr = new_ptr;	size = new_size;

		}

	
	//--------------------------------------------------------------------------
	class CZipLoader //:public CLoader
	{
		unzFile	hFile;
		CrtAlloc Alloc;
		IMemStream* pStream;
		
		const CBuff& src_buff;
	public:
		bool IsZip() const;
	
		
		CZipLoader(const CBuff& _src_buff);
		
		virtual bool Do_serialize(const char * fname, /*out*/ CBuff& data);

		bool ReadCurrentFile(/*out*/ CBuff& data);
		std::string GetCurrFileName();
		bool FirstFile();
		bool NextFile();
		
		virtual ~CZipLoader();
	};

	//--------------------------------------------------------------------------

	class CZipSaver //:public CSaver
	{
		zipFile	hFile;
		CrtAlloc Alloc;
		IMemStream* pStream;
		CBuff& dest_buff;

		CZipSaver(const CZipSaver&);
		CZipSaver& operator= (const CZipSaver&);
	public:
		CZipSaver(CBuff& _dest_buff);
		virtual bool Do_serialize(const char * fname, const CBuff& data);
		//virtual bool Save(const _TCHAR * fname, const std::vector<byte>& data);
		virtual ~CZipSaver();
	};
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////

	CZipLoader::CZipLoader(const CBuff& _src_buff) : src_buff(_src_buff)
	{
		zlib_filefunc_def	filefunc;
		
		UINT c_nMemoryBlock = 1 * 1024 * 1024;// 1MB
		pStream = CreateStreamOnMalloc( &Alloc, c_nMemoryBlock , 
										src_buff.ptr, src_buff.size,
								  		TRUE);
		CreateOnStream( &filefunc );
		hFile = unzOpen2((const char*)pStream, &filefunc );
	}
	
	//--------------------------------------------------------------------------

	bool CZipLoader::IsZip() const
	{
		const char szZipHeader[]	= "PK\x3\x4";
		if( src_buff.size <= strlen(szZipHeader) ) //throw InvalidVersion();
														return false;

		const char *z =  (char *) src_buff.ptr;
		if( !memcmp( z, szZipHeader, strlen(szZipHeader) ) )
		{
			return true;
		}
		else 
			return false;
	}

	//--------------------------------------------------------------------------

	CZipLoader::~CZipLoader()
	{
		unzClose( hFile );
		pStream->Release();
	}
	//--------------------------------------------------------------------------
	//bool CZipLoader::Load(const _TCHAR * fname, std::vector<byte>& data)
	bool CZipLoader::Do_serialize(const char * fname, /*out*/ CBuff& data)
	{
		try
		{
			std::string s(fname);// = sys::ANSI_str(fname);
			if (UNZ_OK != unzLocateFile ( hFile, s.c_str(), 0)) throw;

			return ReadCurrentFile(data);
		}
		catch(...)
		{
			return false;
		}
	}

	bool CZipLoader::ReadCurrentFile(/*out*/ CBuff& data)
	{
		try
		{
			unz_file_info FileInfo;
			if (UNZ_OK != unzGetCurrentFileInfo( hFile, &FileInfo, 0, 0, 0, 0, 0, 0) ) throw; 
			if (UNZ_OK != unzOpenCurrentFile(hFile) ) throw;
			data.resize( FileInfo.uncompressed_size );
			unzReadCurrentFile( hFile, data.ptr, FileInfo.uncompressed_size );

			unzCloseCurrentFile(hFile);
			return true;
		}
		catch(...)
		{
			return false;
		}
	}

	std::string CZipLoader::GetCurrFileName()
	{
		const size_t size = 30;
		char buff[size];
		unz_file_info FileInfo;
		if (UNZ_OK == unzGetCurrentFileInfo( hFile, &FileInfo, buff, size, 0, 0, 0, 0) )
			return std::string(buff);

		return std::string();
	}


	bool CZipLoader::FirstFile()
	{
		return (UNZ_OK == unzGoToFirstFile (hFile) );
	}


	bool CZipLoader::NextFile()
	{
		return (UNZ_OK == unzGoToNextFile (hFile) );
	}

	//--------------------------------------------------------------
	//**************************************************************

	CZipSaver::CZipSaver(CBuff& _dest_buff) : dest_buff(_dest_buff) //:Alloc()
	{
		zlib_filefunc_def	filefunc;
		
		//ULONG	nRef = 0;
		
		UINT c_nMemoryBlock = 5 * 1024;// 5MB
		pStream = CreateStreamOnMalloc( &Alloc, c_nMemoryBlock, 0, 0, FALSE );
		CreateOnStream( &filefunc );

		hFile = zipOpen2( (const char*)pStream, APPEND_STATUS_CREATE, 0, &filefunc);
	}
	//bool CZipSaver::Save(const _TCHAR * fname, const std::vector<byte>& data)
	//--------------------------------------------------------------------------
	
	bool CZipSaver::Do_serialize(const char * fname, const CBuff& data)
	{
		try
		{
			//std::string s = sys::ANSI_str(fname);
			std::string s(fname);
			zipOpenNewFileInZip( hFile, s.c_str(), 0, 0 ,0 , 0, 0, 0, Z_DEFLATED, Z_DEFAULT_COMPRESSION );
			//size_t t = data.size;
			zipWriteInFileInZip( hFile, data.ptr, data.size );
			zipCloseFileInZip( hFile );
			return true;
		}
		catch(...)
		{
			return false;
		}
		
	}
	//--------------------------------------------------------------------------
	CZipSaver::~CZipSaver()
	{
		zipClose(hFile, 0);//"my zip file");

		STATSTG sstg;
		pStream->Stat( &sstg, 0 );
		unsigned long dwArcSize = sstg.cbSize.LowPart;
		
		void * p = 0;
		pStream->GetPtr( &p );
		dest_buff.copy_from( p,  dwArcSize );

		pStream->Release();
	}
	//--------------------------------------------------------------------------
	bool CZipper::ZipTo(/*out*/CBuff& buff)
	{
		CZipSaver zzz(buff);

		bool res = true;

		std::map < std::string, CBuff >::const_iterator mi = begin();
		for (; mi != end(); ++mi)
		{
			CBuff * v = const_cast <CBuff *> (& mi->second);
			res = zzz.Do_serialize(mi->first.c_str(), *v) && res;
		}
		
		return res;

	}
		
	bool CUnZipper::UnzipFrom(const CBuff& buff, const std::string def_file)
	{
		CZipLoader zzz( buff);

		if (!zzz.IsZip())
		{
			CBuff& data = operator[](def_file);
			data.copy_from(buff.ptr, buff.size);
			return false;
		}

		if ( zzz.FirstFile() )
		{
			do
			{
				std::string s = zzz.GetCurrFileName();
			 	if (s.size())
				{
					CBuff data;
					zzz.ReadCurrentFile(data);
					operator[](s) = data;
				}

			} while (zzz.NextFile());
			return true;
		}
		return false;
	}
	
	//////////////////////////////////////////////////////////////////

	bool CBuffMap::get_buff ( std::string name, const void ** pbuff, size_t * psize) const
	{
		iter_c mmi = find(name);

		if (mmi != end())
		{
			*pbuff = mmi->second.ptr;
			*psize = mmi->second.size;
			return true;
		}
		else return false;
	}

	void CBuffMap::put_buff ( std::string name, const void * pbuff, size_t size)
	{
		iter mmi = find(name);
		if (mmi != end()) mmi->second.release();
		
		CBuff& rbuff = operator[](name);
		rbuff.copy_from( pbuff, size);
	}

	void CBuffMap::map_clear()
	{
		iter mi = begin();
		for(; mi != end(); ++mi)
			mi->second.release();
		clear();
	}
	
	CBuffMap::~CBuffMap()
	{
		map_clear();
	}


}