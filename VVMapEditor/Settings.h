// Settings.h: interface for the CSettings class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SETTINGS_H__24CF22F7_4100_4451_A14F_0A3988985925__INCLUDED_)
#define AFX_SETTINGS_H__24CF22F7_4100_4451_A14F_0A3988985925__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRegistry;

#include <map>
#include <vector>
#include "Exception.h"

MACRO_EXCEPTION( OptionException, CommonException )
MACRO_EXCEPTION( NoSuchOptionException, OptionException)

const DWORD	prop_AnchorColor		=	1;
const DWORD	prop_AnchorMapColor		=	2;
const DWORD	prop_RefPointColor		=	3;
const DWORD prop_ExtentColor		=	4;
const DWORD prop_PointNumberColor	=	5;

const DWORD	prop_MRU_Number			=	10;
const DWORD prop_PointFilter		=	11;
const DWORD	prop_PanoramaMaxX		=	12;
const DWORD	prop_PanoramaMaxY		=	13;
const DWORD	prop_CrossExtent		=	14;
const DWORD	prop_CrossThickness		=	15;	

const DWORD prop_OpenWindowNumber	=	16;
const DWORD prop_ShowRenumberDialog	=	17;

class CColorSettings
{
protected:
	typedef std::map<int, DWORD> ColorMap_t;

	ColorMap_t	m_Colors, m_ColorsBack;

	DWORD	GetDefColor( int Number) const;
public:
	void	Load( CWinApp* pApp, CString strSection );
	void	Save( CWinApp* pApp, CString strSection ) const;

	DWORD	GetColor(int Number) const;
	void	SetColor(int Number, DWORD dwColor) ;

	void	Transact()	{ m_ColorsBack = m_Colors;	}
	void	Rollback()	{ m_Colors = m_ColorsBack;	}
	void	Commit()	{}
};

template<typename T> class PropertyValue
{
	T		m_DefValue;
	T		m_Value;
	CString	m_strName;
public:
	T		GetDefValue()	const	{ return m_DefValue;	}
	T		GetValue()		const	{ return m_Value;		}
	void	SetValue(T	Value)		{ m_Value = Value;		}

	PropertyValue(T Value, CString strName) : 
		m_DefValue(Value), 
		m_Value(Value),
		m_strName(strName)
		{}
	PropertyValue(const PropertyValue<T>& ics) : 
		m_DefValue(ics.m_DefValue),
		m_Value(ics.m_Value),
		m_strName(ics.m_strName) {}
	
	PropertyValue&	operator = (const PropertyValue<T>& ics)
	{
		m_DefValue	= ics.m_DefValue;
		m_Value		= ics.m_Value;
		m_strName	= ics.m_strName;
	}

	void	Load(CWinApp*	pApp, CString strSection)
	{
		ASSERT( !strSection.IsEmpty()	);
		ASSERT( !m_strName.IsEmpty()	);
		ASSERT( pApp	);
		m_Value = pApp->GetProfileInt( strSection, m_strName, m_DefValue );
	}

	void	Save(CWinApp*	pApp, CString strSection) const
	{
		ASSERT( !strSection.IsEmpty()	);
		ASSERT( !m_strName.IsEmpty()	);
		ASSERT( pApp	);

		//NOTENOTE: save only non-default colors
		// it's bad idea to save only non-def values
		// e.g.  6 in regestry -> change to 2 in application and 2 is default -> so we have still 6 in regestry!!!
		if(  /*m_DefValue != m_Value */ true)
			pApp->WriteProfileInt( strSection, m_strName, m_Value );
	}

};

//NOTENOTE: this settings unchangable so have not a transaction stuff
class CMasterSettings
{
	bool				m_bUseStubList;
	std::vector<int>	m_MasterStubList;
public:
	CMasterSettings():m_bUseStubList(false){}
	void	Load(CWinApp* pApp, CString strSection);
	bool	UseMasterStubList() const { return m_bUseStubList; }
	std::vector<int>	GetMasterStubList() const { return m_MasterStubList; }
};



//-----------------------------------------------------------------------------
// Purpose: general aplication settings
//-----------------------------------------------------------------------------
/*

we can use several code strategies:
1) property map with std::map w/CString ( or DWORD) and boost::any
2) several different property  classes

*/
class CSettings  
{
public:
	enum Filter_t
	{
		FILTER_NONE,
		FILTER_WEAK,
		FILTER_STRONG
	};

//	static	DWORD
private:

/*	static	DWORD Filter2Number(  );
	static */
/*
	int			m_nRecentFiles;
	Filter_t	m_nFilter;
	CSize		m_sizePanoramaMax;
	CSize		m_sizeCross;*/
	

	CColorSettings		m_PointColors;
	CColorSettings		m_MasterColors;
	CMasterSettings		m_MasterStub;
	std::map< DWORD, PropertyValue<DWORD> >	m_DwordProperty, m_DwordPropertyBack;
/*
	static		Filter_t	GetDefFilter()			{ return FILTER_NONE;			}
	static		int			GetDefRecentFiles()		{ return _AFX_MRU_COUNT;		}
	static		CSize		GetDefPanoramaMaxSize()	{ return CSize( 1000, 1000);	}
	static		CSize		GetDefCrossSize()		{ return CSize( 15, 1);			}
*/
	bool	AddToMap	( DWORD dwNumber, DWORD dwColor, CString strName )
	{
		std::pair< std::map< DWORD, PropertyValue<DWORD> >::iterator, bool> 
			p = m_DwordProperty.insert( std::make_pair( dwNumber, PropertyValue<DWORD> ( dwColor, strName ) ) );
		return p.second;
	}
	void	SaveMap(CWinApp* pApp) const;
	void	LoadMap(CWinApp* pApp);
public:
	CSettings();
	CSettings( const CSettings& sets );
	CSettings& operator = ( const CSettings& sets);

	virtual		~CSettings();
	//NOTENOTE: may pass CWinApp* or get it from AfxGetApp()
	void		Load(/*CRegistry& rg*/);
	void		Save(/*CRegistry& rg*/) const;

	//NOTENOTE: for compatible do 
	Filter_t	GetFilter() const			{ return (Filter_t)GetProperty( prop_PointFilter );		}
	void		SetFilter(Filter_t flt)		{ SetProperty( prop_PointFilter, DWORD(flt) ); 			}

	
	//NOTENOTE: Incapsulalate color management
	DWORD		GetMasterColor(int Number) const { return m_MasterColors.GetColor(Number) ;}
	void		SetMasterColor(int Number, DWORD dwColor) { m_MasterColors.SetColor(Number, dwColor); }

	DWORD		GetPointColor(int Number) const { return m_PointColors.GetColor(Number) ;}
	void		SetPointColor(int Number, DWORD dwColor) { m_PointColors.SetColor(Number, dwColor); }

	//NOTENOTE: stub master list when hammer is down, use instead macro HAMMER_DOWN
	bool				UseMasterStubList() const { return m_MasterStub.UseMasterStubList();}
	std::vector<int>	GetMasterStubList() const { return m_MasterStub.GetMasterStubList(); }

	//NOTENOTE: 
	/// @throw( NoSuchOptionException )
	DWORD	GetProperty( DWORD	dwNumber) const;
	/// @throw( NoSuchOptionException )
	void	SetProperty( DWORD	dwNumber, DWORD dwColor);

	void	Transact();
	void	Rollback();
	void	Commit();
};

//-----------------------------------------------------------------------------
// Purpose: singleton handle
//-----------------------------------------------------------------------------
CSettings&	GetSettings();

#endif // !defined(AFX_SETTINGS_H__24CF22F7_4100_4451_A14F_0A3988985925__INCLUDED_)
