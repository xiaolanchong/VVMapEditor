//======================================================================================//
//                                       Orwell2K                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	database serializer for dep file & db insertion
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 13.04.2005
//                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "DBAccessor.h"
#include "DBSerializer.h"

#include "md5/md5.h"

//======================================================================================//
//                                  class DBSerializer                                  //
//======================================================================================//
DBSerializer::DBSerializer()
{
}

DBSerializer::~DBSerializer()
{
}

void	DBSerializer::SaveFile( const std::vector<BYTE>& Data)
try
{
	Connect();
	bool res = WriteBlobData<DBA_Global_Vme>( m_dbSession, NULL, Data );
	if( !res ) throw DatabaseException("Failed to save vme file");

	CCommand< CAccessor< DBA_Global_VmeHash > > A_VMEHash;


	if( !m_dbSession.m_spOpenRowset || !CheckConnection())
	{
		throw SaveDBFileException("Statistics table connection error");
	}

	HRESULT hr;
	CDBPropSet  propset(DBPROPSET_ROWSET);
	propset.AddProperty(DBPROP_IRowsetChange, true);
	propset.AddProperty(DBPROP_UPDATABILITY, DBPROPVAL_UP_CHANGE );

	hr = A_VMEHash.Open(m_dbSession, NULL, &propset);
	if(FAILED( hr ) ) 
	{
		TraceError(hr);
		throw CameraListException("table connection error");
	}

	if( A_VMEHash.MoveNext() == S_OK ) 
	{
		memset( A_VMEHash.m_szVMEHash, 0, sizeof( A_VMEHash.m_szVMEHash ) );
		md5_string( (const char*)&Data[0], A_VMEHash.m_szVMEHash, Data.size()* sizeof(BYTE)/sizeof(char) );
		A_VMEHash.SetData(0);
	}

	Close();
}
catch ( SaveDataException ) 
{
	Close();
	throw DBSaveDataException("zzz");
}
catch( DatabaseException )
{
	Close();
	throw;
};

void	DBSerializer::LoadFile( std::vector<BYTE>& BinArr )
try
{
	Connect();
	bool res = ReadBlobData<DBA_Global_Vme>( m_dbSession, NULL, BinArr );
	if( !res ) throw DatabaseException("Failed to load vme file");
	Close();
}
catch ( NoDataException ) 
{
	throw DBNoDataException("No data");
}
catch( DatabaseException )
{
	Close();
	throw;
};

void	DBSerializer::Connect()
{
#if 0
	TCHAR szBuf[1024];
	DWORD dwSize = sizeof( szBuf );
	DWORD dwType = REG_SZ;
	DWORD res = SHGetValue( HKEY_LOCAL_MACHINE, _T("Software\\ElVEES\\Common"), _T("DbInitString"), &dwType, szBuf, &dwSize );
	if( res != ERROR_SUCCESS  ) throw  DBConnectionException("Invalid connection string");
	std::tstring sCS (szBuf);
#if 1
	sCS += _T(";User ID=OrwellSAdmin;Password=t*e_AVafrEzUtaTha2rEmUJE@ew#xaze"); 
#endif
	Open( sCS, false );
#else
	DWORD dwSize = 0;
	long res = GetO2KDBString( NULL, &dwSize, true);
	if(res != DBB_S_OK)
	{
		throw DBConnectionException("failed to get the connection string");
	}
	std::vector<WCHAR> Buf(dwSize);
	res = GetO2KDBString( &Buf[0], &dwSize, true);
	if(res != DBB_S_OK)
	{
		throw DBConnectionException("failed to get the connection string");
	}
	std::wstring sCS( Buf.begin(), Buf.end() );
	Open( sCS, false );
#endif
}

void	DBSerializer::GetCameraList( std::vector<int>& CameraArr ) 
try
{
	Connect();
	const int c_nMaxCamera = 1000000;
	CCommand<CAccessor<DBA_Camera> > A_Camera;
	if( !m_dbSession.m_spOpenRowset || !CheckConnection())
	{
		throw CameraListException("Statistics table connection error");
	}

	HRESULT hr;

	hr = A_Camera.Open(m_dbSession, NULL);
	if(FAILED( hr ) ) 
	{
		TraceError(hr);
		throw CameraListException("table connection error");
	}

	while( A_Camera.MoveNext() == S_OK ) 
	{
		int nCameraID = A_Camera.m_nCameraID;
		if( nCameraID > 0 && 
			nCameraID < c_nMaxCamera &&
			A_Camera.m_nCameraType == 0) CameraArr.push_back( nCameraID );
	}
}
catch( DatabaseException )
{
	CameraArr.clear();
};