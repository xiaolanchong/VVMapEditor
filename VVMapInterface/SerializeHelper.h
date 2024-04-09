// SerializeHelper.h: interface for the SerializeHelper class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _SER_HELPER_H_included
#define _SER_HELPER_H_included


#include <vector>
#include <windows.h>

class CBuffer;
class CBufferInput;

namespace SerializeHelper  
{


class AlignMapSaver
{
	static const char*	szAlignmapString;
public:
	AlignMapSaver() {};
	virtual ~AlignMapSaver() {};

	void	Save(	CBuffer& Buffer, 
					const std::vector<long>&	PointNumners,
					const std::vector<long>&	PointCoords
				);
	bool	Load(	long				FileVersion,
					CBufferInput&		Buffer,
					std::vector<long>&	PointNumners,
					std::vector<long>&	PointCoords
				);

};

//const int n
/*
class InvalidVersion : public std::exception
{
public:
	const char* what() const { return "Invalid version";}
};

class SerializeBuffer
{
//	CBufferInput&	m_Buffer;	
	DWORD		m_dwVersion;
public:
	SerializeBuffer( DWORD dwVersion) : m_dwVersion(dwVersion){}

	bool	IsZipVersion(void *pBuffer, size_t Size) const throw(InvalidVersion);

	bool	ZipBuffer( const void * pBuffer, size_t Size, std::vector<unsigned char>& FilledBuffer  );
	bool	UnzipBuffer( const void * pBuffer, size_t Size, std::vector<unsigned char>& FilledBuffer );
};*/

}

#endif // !defined(AFX_SERIALIZEHELPER_H__30D90BC1_432D_426C_B37F_F5F3B4DCF409__INCLUDED_)
