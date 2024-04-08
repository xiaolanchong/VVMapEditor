#ifndef __DBHELPER_H__
#define __DBHELPER_H__

#if 0
#include "CinemaDB.h"
#include "DBRows.h"
#endif

#include <atldbcli.h>
#define STRSAFE_NO_CB_FUNCTIONS
#include <strsafe.h>

#include <atlconv.h>
#include <vector>

namespace ISSHelper
{
	struct MemStreamRead :  ISequentialStream
	{
		const std::vector<unsigned char>& m_Buffer;
		std::vector<unsigned char>::const_iterator m_Current;
		MemStreamRead(const std::vector<unsigned char>& Arr) : m_Buffer(Arr),
			m_Current ( m_Buffer.begin())
		{
		}

		virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
			/* [in] */ REFIID riid,
			/* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject) 
		{
			if( IID_ISequentialStream == riid )
			{
				*ppvObject = static_cast<ISequentialStream*>(this);
				return S_OK;
			} else if( IID_IUnknown == riid )
			{
				*ppvObject = static_cast<IUnknown*>(this);
				return S_OK;
			}
			return E_NOTIMPL;
		}
		virtual ULONG STDMETHODCALLTYPE AddRef( void) {return 1;}
		virtual ULONG STDMETHODCALLTYPE Release( void) { return 1;}
		virtual HRESULT STDMETHODCALLTYPE Read( void *pv, ULONG cb, ULONG *pcbRead) 
		{ 
			ULONG Remain = ULONG(m_Buffer.size() - ( m_Current - m_Buffer.begin() ));
			if(cb > Remain) cb = Remain;
			memcpy( pv, &(*m_Current), cb );
			m_Current += cb;
			if( pcbRead ) { *pcbRead = cb;}
			return S_OK;
		}
		virtual HRESULT STDMETHODCALLTYPE Write( const void *pv, ULONG cb, ULONG *pcbWritten)
		{
			UNREFERENCED_PARAMETER(pv);
			UNREFERENCED_PARAMETER(cb);
			UNREFERENCED_PARAMETER(pcbWritten);
			return E_NOTIMPL;
		}	
	} ;

	inline bool ReadBlobToArray( std::vector<BYTE> & Arr, ISequentialStream* pSeqStream, size_t size)
	{
		//		const ULONG dwSize = size;
		ULONG dwRead;
		Arr.resize( size );
		HRESULT hr = pSeqStream->Read( &Arr[0], ULONG( size ), &dwRead );
		return SUCCEEDED(hr);
	}
}

template <class T> bool		WriteBlobData(CSession& ses, LPCWSTR szQuery, const std::vector<BYTE>& Data)
{
	if( !ses.m_spOpenRowset ) return false;

	HRESULT hr;

	CDBPropSet  propset(DBPROPSET_ROWSET);
	propset.AddProperty(DBPROP_IRowsetChange, true);
	propset.AddProperty(DBPROP_UPDATABILITY, DBPROPVAL_UP_CHANGE );

	CCommand<CAccessor<T> > A_Table;
	hr = A_Table.Open(ses, szQuery, &propset);
	if(FAILED( hr ) ) 
	{
//		AtlTraceError(hr);
		return false;
	}
	// Get data from the rowset
	ISSHelper::MemStreamRead msr( Data );
	bool res = false;
	if (A_Table.MoveNext() == S_OK)
	{
		if( A_Table.m_BLOBDATA_STATUS ==  DBSTATUS_S_OK)
		{
			// field is not NULL
			hr = A_Table.m_BLOBDATA->Release();
		}
		A_Table.m_BLOBDATA_STATUS	= DBSTATUS_S_OK;
		A_Table.m_BLOBDATA			= &msr;
		A_Table.m_BLOBDATA_LENGTH	= (ULONG)Data.size();
		hr = A_Table.SetData();
		if( FAILED(hr) ) 
		{
			
			res = false;
			AtlTraceErrorRecords( hr );
			throw SaveDataException();
		}
		else
			res = true;
		A_Table.FreeRecordMemory();

	}	
	return res;
}

struct NoDataException
{
};

struct SaveDataException
{
};

template <class T> bool		ReadBlobData(CSession& ses,LPCWSTR szQuery, std::vector<BYTE>& Data)
{
	HRESULT hr;
	CCommand<CAccessor<T> > A_Table;
	hr = A_Table.Open(ses, szQuery);
	if(FAILED( hr ) ) 	return false;
	// Get data from the rowset
	Data.clear();
	bool res = false;
	if (A_Table.MoveNext() == S_OK)
	{
		if( A_Table.m_BLOBDATA_STATUS ==  DBSTATUS_S_OK)
		{
			// field is not NULL
			size_t nSize					= A_Table.m_BLOBDATA_LENGTH;
			ISequentialStream* pSeqStream	= A_Table.m_BLOBDATA ;
			res = ISSHelper::ReadBlobToArray( Data, pSeqStream, nSize );	
		}
		else if(A_Table.m_BLOBDATA_STATUS ==  DBSTATUS_S_ISNULL)
		{
			throw NoDataException();
		}
		else return false;
		A_Table.FreeRecordMemory();
	}	
	return res;
}


inline void SystemTimeToDBTime( const SYSTEMTIME& st, DBTIMESTAMP& Time )
{
	Time.year	= st.wYear;
	Time.month	= st.wMonth;
	Time.day	= st.wDay;
	Time.hour	= st.wHour;
	Time.minute = st.wMinute;
	Time.second = st.wSecond;
	Time.fraction = 0;
}

inline void DBTimeToSystemTime( const DBTIMESTAMP& Time, SYSTEMTIME& st )
{
	st.wYear	= Time.year;
	st.wMonth	= Time.month;
	st.wDay	= Time.day;
	st.wHour	= Time.hour;
	st.wMinute = Time.minute;
	st.wSecond = Time.second;
	//Time.fraction = 0;
}

//from atlbase.h since we can hardly use <oledb.h>
inline CTime ToCTime( const DBTIMESTAMP& dbts, int nDST = -1)
{
	struct tm atm;
	atm.tm_sec = dbts.second;
	atm.tm_min = dbts.minute;
	atm.tm_hour = dbts.hour;
	atm.tm_mday = dbts.day;
	atm.tm_mon = dbts.month - 1;        // tm_mon is 0 based
	ATLASSERT(dbts.year >= 1900);
	atm.tm_year = dbts.year - 1900;     // tm_year is 1900 based
	atm.tm_isdst = nDST;
	__time64_t m_time = _mktime64(&atm);
	ATLASSERT(m_time != -1);       // indicates an illegal input time
	return CTime( m_time );
}

#endif //__DBHELPER_H__