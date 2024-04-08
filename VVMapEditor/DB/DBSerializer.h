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
#ifndef _D_B_SERIALIZER_3409536366849194_
#define _D_B_SERIALIZER_3409536366849194_

#include "OleDBConnection.h"

MACRO_EXCEPTION( SaveDBFileException, DatabaseException)
MACRO_EXCEPTION( LoadDBFileException, DatabaseException)
MACRO_EXCEPTION( CameraListException, DatabaseException)
//======================================================================================//
//                                  class DBSerializer                                  //
//======================================================================================//

class DBSerializer : protected COleDBConnection
{
	void	Connect();

public:
	DBSerializer();
	virtual ~DBSerializer();

	void	SaveFile( const std::vector<BYTE>& Data);
	void	LoadFile( std::vector<BYTE>& BinArr ); 
	void	GetCameraList( std::vector<int>& CameraArr ); 
};

#endif // _D_B_SERIALIZER_3409536366849194_