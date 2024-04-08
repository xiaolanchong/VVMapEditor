//======================================================================================//
//                                       Orwell2K                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Database accessors for the "OrwellDB" database
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 13.04.2005
//                                                                                      //
//======================================================================================//
#ifndef _D_B_ACCESSOR_7983257548527842_
#define _D_B_ACCESSOR_7983257548527842_

#include <atldbcli.h>

//======================================================================================//
//                                   class DBAccessor                                   //
//======================================================================================//

struct DBA_Camera
{
	int		m_nCameraID;
	int		m_nCameraType;

	BEGIN_ACCESSOR_MAP( DBA_Camera, 1 )
		BEGIN_ACCESSOR( 0, true)
			COLUMN_ENTRY( 1, m_nCameraID);
			COLUMN_ENTRY( 2, m_nCameraType);
		END_ACCESSOR()
	END_ACCESSOR_MAP()
	DEFINE_COMMAND_EX(DBA_Camera, _T("SELECT CameraID, CameraType FROM Camera"))
};

struct DBA_Global_Vme
{
	ISequentialStream*	m_BLOBDATA;

	ULONG m_BLOBDATA_LENGTH;
	ULONG m_BLOBDATA_STATUS;

	BEGIN_COLUMN_MAP(DBA_Global_Vme)
		BLOB_ENTRY_LENGTH_STATUS(1, IID_ISequentialStream, STGM_READ, m_BLOBDATA, m_BLOBDATA_LENGTH, m_BLOBDATA_STATUS);
	END_COLUMN_MAP()
	DEFINE_COMMAND_EX(DBA_Global_Vme, _T("SELECT Intersection_TransFile FROM Globals"))
};

struct DBA_Global_VmeHash
{
	char m_szVMEHash[100];

	BEGIN_ACCESSOR_MAP( DBA_Global_VmeHash, 1 )
		BEGIN_ACCESSOR( 0, true)
		COLUMN_ENTRY( 1, m_szVMEHash);
		END_ACCESSOR()
	END_ACCESSOR_MAP()
	DEFINE_COMMAND_EX(DBA_Global_VmeHash, _T("SELECT VMEHash FROM Globals"))
};

#endif // _D_B_ACCESSOR_7983257548527842_