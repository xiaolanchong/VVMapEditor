// Settings.cpp: implementation of the CSettings class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VVMapEditor.h"
#include "Settings.h"
#include <shlwapi.h>
#include <algorithm>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define REG_KEY_SETTINGS			_T("Settings")
#define REG_KEY_MASTERCOLOR			_T("MasterColor")
#define REG_KEY_POINTCOLOR			_T("PointColor")
#define REG_KEY_CUSTOMSETTINGS		_T("CustomSettings")

#define REG_STR_MASTER_STUB			_T("MasterStub")

#define REG_STR_MRU_NUMBER			_T("MRUNumber")
#define	REG_STR_POINT_FILTER		_T("PointFilter")
#define	REG_STR_PANORAMA_X			_T("PanoramaMaxX")
#define	REG_STR_PANORAMA_Y			_T("PanoramaMaxY")
#define REG_STR_CROSS_THICKNESS		_T("CrossThickness")
#define REG_STR_CROSS_EXTENT		_T("CrossExtent")

CSettings::CSettings()
{
	AddToMap( prop_AnchorColor,			Color::Green,		_T("AnchorColor"		));
	AddToMap( prop_AnchorMapColor,		Color::Blue,		_T("AnchorMapColor"		)) ;
	AddToMap( prop_RefPointColor,		Color::Red,			_T("RefPointColor"		)) ;
	AddToMap( prop_ExtentColor,			Color::Red,			_T("ExtentColor"		));
	AddToMap( prop_PointNumberColor,	Color::Red,			_T("PointNumberColor"	));

	AddToMap( prop_MRU_Number,			_AFX_MRU_COUNT,		REG_STR_MRU_NUMBER);
	AddToMap( prop_PointFilter,			DWORD(FILTER_NONE),	REG_STR_POINT_FILTER);
	AddToMap( prop_PanoramaMaxX,		1000,				REG_STR_PANORAMA_X	);
	AddToMap( prop_PanoramaMaxY,		1000,				REG_STR_PANORAMA_Y	);
	AddToMap( prop_CrossExtent,			15,					REG_STR_CROSS_EXTENT);
	AddToMap( prop_CrossThickness,		1,					REG_STR_CROSS_THICKNESS);

	AddToMap( prop_OpenWindowNumber,	2,					_T("OpenWindowNumber"	));
	AddToMap( prop_ShowRenumberDialog,	0,					_T("ShowRenumberDialog"	));
}

CSettings::~CSettings()
{

}

CSettings::CSettings( const CSettings& sets )
{

}

CSettings& CSettings::operator = ( const CSettings& sets)
{
/*	m_nRecentFiles = sets.m_nRecentFiles;
	m_nFilter = sets.m_nFilter;*/
	return *this;
}

void		CSettings::Load()
{
	CString strSection (REG_KEY_SETTINGS);
	CWinApp* pApp = AfxGetApp();
	
	m_PointColors.Load( pApp,  REG_KEY_POINTCOLOR);
	m_MasterColors.Load( pApp,  REG_KEY_MASTERCOLOR);
	m_MasterStub.Load( pApp, REG_STR_MASTER_STUB );
	LoadMap(pApp);
}

void		CSettings::Save() const
{
	CString strSection (REG_KEY_SETTINGS);
	CWinApp* pApp = AfxGetApp();

	m_PointColors.Save( pApp,  REG_KEY_POINTCOLOR);
	m_MasterColors.Save( pApp,  REG_KEY_MASTERCOLOR);
	SaveMap(pApp);
}

DWORD	CSettings::GetProperty( DWORD	dwNumber) const throw( NoSuchOptionException )
{
	std::map< DWORD, PropertyValue<DWORD> >::const_iterator itr = m_DwordProperty.find(dwNumber);
	
	if( itr == m_DwordProperty.end() ) throw NoSuchOptionException( "No such property" );
	return itr->second.GetValue();
}

void	CSettings::SetProperty( DWORD	dwNumber, DWORD dwColor) throw( NoSuchOptionException )
{
	std::map< DWORD, PropertyValue<DWORD> >::iterator itr = m_DwordProperty.find(dwNumber);
	
	if( itr == m_DwordProperty.end() ) throw NoSuchOptionException( "No such property" );
	itr->second.SetValue(dwColor);
}

void	CSettings::SaveMap(CWinApp* pApp) const
{
	std::map< DWORD, PropertyValue<DWORD> >::const_iterator itr = m_DwordProperty.begin();
	
	for(; itr != m_DwordProperty.end(); ++itr )
	{
		itr->second.Save( pApp, REG_KEY_SETTINGS );
	}
}

void	CSettings::LoadMap(CWinApp* pApp)
{
	std::map< DWORD, PropertyValue<DWORD> >::iterator itr = m_DwordProperty.begin();

	for(; itr != m_DwordProperty.end(); ++itr )
	{
		itr->second.Load( pApp, REG_KEY_SETTINGS );
	}
}


void	CSettings::Transact()	
{ 
	m_PointColors.Transact();
	m_MasterColors.Transact();
	m_DwordPropertyBack = m_DwordProperty;
	
}
void	CSettings::Rollback()	
{ 
	m_PointColors.Rollback();
	m_MasterColors.Rollback();
	m_DwordProperty = m_DwordPropertyBack;
}

void	CSettings::Commit()	
{

}

CSettings&	GetSettings()
{
	static CSettings sets;
	return sets;
}


/////////////////////////////////////

DWORD	CColorSettings::GetDefColor( int Number) const
{
	//NOTENOTE: we cyclic see at the array of colors
/*	static Colors[] = 
	{
		Color::Red,
		Color::Green,
		Color::Blue,
		Color::Chocolate,
		Color::Chartreuse,
		Color::LightSalmon,
		Color::DimGray
	};*/
	static Colors[]=
	{
		Color::DarkRed,
		Color::DarkGreen,
		0xFF808000,			// mustartd
		Color::DarkBlue,
		Color::Purple,
		Color::DarkTurquoise,
//		Color::Gray,			// bad as white too
//		Color::DarkGray,
		Color::Red,
		Color::Green,
		Color::Yellow,
		Color::Blue,
		Color::Pink,
		Color::Cyan,
//		Color::White		White is bad coz our background mostly white
	};
	return Colors[ Number% (sizeof(Colors)/sizeof(Color))  ];	
}

void	CColorSettings::Load( CWinApp* pApp, CString strSection )
{
	DWORD dwIndex = 0;
	TCHAR	szValueName [MAX_PATH];
    DWORD	cbData,	dwType, cchValueName;
    BYTE	pData[MAX_PATH];

	HKEY hSecKey = pApp->GetSectionKey(strSection);
	while ( true)
	{
		//NOTENOTE: iterate through regesty key and search DWORD value with number name	
		cbData = MAX_PATH;
		cchValueName = MAX_PATH;
		//NOTENOTE: since we use only win2k & later we don't differ RegEnumValueEx & SHEnumValue, comment out if it's not true
	//	DWORD dwRes = SHEnumValue( hSecKey, dwIndex, szValueName, &cchValueName, &dwType, pData, &cbData);
		DWORD dwRes = RegEnumValue(hSecKey, dwIndex, szValueName, &cchValueName, NULL, &dwType, pData, &cbData);
		if ( dwRes != ERROR_SUCCESS) break;
		int nMaster;
		if( dwType == REG_DWORD )
		{
			//NOTENOTE: use shlw api, but can use also _tcstod
			BOOL res = StrToIntEx( szValueName ,STIF_DEFAULT , &nMaster); 
			if( res )
			{
				DWORD dwColor;
				memcpy( &dwColor, pData, sizeof(DWORD) );
				m_Colors.insert( std::make_pair( nMaster, dwColor ) );
			}
		}
		++dwIndex;
	} 
}

void	CColorSettings::Save( CWinApp* pApp, CString strSection ) const
{
	ColorMap_t::const_iterator itr = m_Colors.begin( );
	CString strEntry;
	for (  ; itr != m_Colors.end() ; ++itr)
	{
		DWORD dwColor = itr ->second;
		//NOTE: some hack - we store only not default colors
		if( dwColor != GetDefColor( itr->first ) )
		{
			strEntry.Format( _T("%d" ), itr->first );
			pApp->WriteProfileInt( strSection, strEntry, dwColor );
		}
	}
}

DWORD	CColorSettings::GetColor(int Number) const
{
	ColorMap_t::const_iterator itr = m_Colors.find( Number );
	if( itr == m_Colors.end() )
	{
		return GetDefColor( Number );
	}
	else
	{
		return itr->second;
	}
}

void	CColorSettings::SetColor(int Number, DWORD dwColor) 
{
	ColorMap_t::iterator itr = m_Colors.find( Number );
	if( itr == m_Colors.end() )
	{
		m_Colors.insert( std::make_pair( Number, dwColor ) );
	}
	else
	{
		itr->second = dwColor;
	}
}

std::vector<int>	ParseString( CString strText );

void	CMasterSettings::Load( CWinApp* pApp, CString strSection )
{
	DWORD dwIndex = 0;
//	TCHAR	szValueName [MAX_PATH];
    DWORD	cbData,	dwType;
    TCHAR	pData[MAX_PATH];
	cbData = (MAX_PATH - 1) * sizeof(TCHAR);
	HKEY hSecKey = pApp->GetSectionKey(REG_KEY_SETTINGS);	
	LONG lRes = RegQueryValueEx(hSecKey, strSection, NULL, &dwType, (BYTE*)pData, &cbData);
	if( lRes == ERROR_SUCCESS && dwType == REG_SZ)
	{
		m_bUseStubList = true;
		if( pData[ cbData -1] != _T('\0') ) pData[ cbData ] = _T('\0');
		CString z( pData );
		m_MasterStubList = ParseString( z );
	}
}

//-----------------------------------------------------------------------------
// Purpose: parse string to the array of integer numbers 
// Input  : strText - numbers delimited by space: 1 2 12 4 5
//-----------------------------------------------------------------------------
std::vector<int>	ParseString( CString strText )
{
	std::vector<int> Numbers;

	while(true)
	{
		int nMaster;
		int Num = strText.Find( _T(' ') );
		if( Num == -1 )
		{
			BOOL res = StrToIntEx( strText, STIF_DEFAULT , &nMaster );
			if( res) Numbers.push_back( nMaster );
			break;
		}
		else
		{
			CString sz = strText.Left( Num );
			strText.Delete( 0, Num + 1 );
			BOOL res = StrToIntEx( sz, STIF_DEFAULT , &nMaster );
			if( res) Numbers.push_back( nMaster );
		}
	}
	std::sort( Numbers.begin(), Numbers.end());
	std::unique( Numbers.begin(), Numbers.end() );
	return Numbers;
}

/////////////////////////////////////////////////////////////////////////////////////////
