//========================= Copyright © 2004, Elvees ==========================
// Author: Eugene V. Gorbachev
// Purpose: Main data class of editor
//
//=============================================================================

// VVMapEditorDoc.h : interface of the CVVMapEditorDoc class
//
/////////////////////////////////////////////////////////////////////////////

/************************************
  Revised on 25.08.2004 18:17:11
  Comments: Добавлено отображение для высот
  Revised on 25.08.2004 10:48:24
  Comments: Изменен процесс получения полигонов - если преобразования нет полигон стирается
  Revised on 24.08.2004 18:42:27
  Comments: Добавлена возможность изменение соответствия в реалтайме
  пока Undo/Redo на оставшихся не сохраняется
 ************************************/


#if !defined(AFX_VVMAPEDITORDOC_H__70AC22E3_260B_44AE_A992_FA780CC23EC4__INCLUDED_)
#define AFX_VVMAPEDITORDOC_H__70AC22E3_260B_44AE_A992_FA780CC23EC4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <algorithm>
#include <memory>

#include "PointList.h"
#include "ChooseCompressor.h"
#include "VVError.h"
#include "Settings.h"

#include <utility>

const int	c_nMapIndex = -1;

MACRO_EXCEPTION(PropertyException, CommonException)
MACRO_EXCEPTION(UndefinedPropertyException, PropertyException)
MACRO_EXCEPTION(AssignPropertyException, PropertyException)

template< typename T> class Property
{
	bool		m_bSet;
	typename T	m_t;
public:
	Property(const Property<T>& prop) : m_bSet(prop.m_bSet), m_t(prop.m_t){ /* if(int(prop.m_bSet) > 1 ) throw 0;*/  }
	Property(T prop) : m_bSet(true), m_t(prop){}
	Property() : m_bSet(false){}

	bool		IsSet() const	{ return m_bSet;	};
	void		Reset() { m_bSet = false;}
	T	Get() const	
	{ 
		if(!m_bSet) throw UndefinedPropertyException("Property undefined"); 
		return m_t;		
	}

	T operator	= (T t)			{ m_bSet = true; return this->m_t = t;}
	Property<T>& operator	= (const Property<T>& prop)			
	{ 
		if(&prop != this) 
		{
			if( !prop.IsSet() ) throw AssignPropertyException("Assign undefined property");
			t.m_t = prop.m_t; 
			t.m_bSet = prop.m_bSet;
		}
		return *this;
	}
};

template<class T> class UndoStack
{
	std::vector<T>						m_Store;
	typename std::vector<T>::iterator	m_CurPos;
public:
	UndoStack()
	{
		m_Store.reserve(100);
		m_CurPos = m_Store.end();
	}
	bool	CanRedo() const 
	{ 
		bool z = !m_Store.empty() && m_CurPos != m_Store.end() - 1;
		return z;
	}

	bool		CanUndo() const
	{
		return m_CurPos != m_Store.begin() ;
	}

	void		Redo()
	{
		++m_CurPos;
		CString s;
		s.Format(_T("Redo=================\n"));
		OutputDebugString(s);
	}
	void		Undo()
	{
		--m_CurPos;
		CString s;
		s.Format(_T("Undo=================\n"));
		OutputDebugString(s);
	}
	void		Backup()
	{
		OutputDebugString(_T("Backup=================\n"));

		if(  m_CurPos != m_Store.end() && m_CurPos != m_Store.end() -1)
			m_Store.erase(  m_CurPos +1 , m_Store.end()  );
		m_Store.push_back( m_Store.back());
		m_CurPos = m_Store.end() - 1;
//		ASSERT( m_UndoPos != -1 );
	}
	void		Erase()
	{
		CString s;
//		s.Format(_T("Erase=================\n"));
//		OutputDebugString(s);
		m_Store.clear();
		m_CurPos = m_Store.end();
	}
	const T& Top() const
	{
		return *( m_CurPos);
	}
	T& Top() 
	{
		return *( m_CurPos);
	}

	void	Resize() 
	{
		m_Store.resize(1);
		m_CurPos = m_Store.begin();
	}
};

class	CFilterCache
{
	friend class CVVMapEditorDoc;

	std::set<int>	m_FilterMasters;
	std::set<int>	m_FilterGroups;

	const CVVMapEditorDoc*	m_pDoc;

	CFilterCache(	const CVVMapEditorDoc* pDoc, 
					const std::set<int>& Ids, 
					const std::set<int>& Groups 	
					) : m_pDoc(pDoc), m_FilterMasters( Ids ), m_FilterGroups(Groups) {}

public:
	bool	IsGroupValid(int nGroupIndex) const;
	bool	IsPointValid( int nGroupIndex, int nMasterIndex ) const;

};


class CCameraView;
class CMergeView;
class CExtentView;
class CVVMapEditorView;
class CAnchorView;
class CMapScaleView;
class CAllMapView;
class CBitmapBackground;

class CVVMapEditorDoc : public CDocument
{
protected: // create from serialization only
	CVVMapEditorDoc();
	DECLARE_DYNCREATE(CVVMapEditorDoc)

// Attributes
public:
	enum State
	{
		usual,
		compiled,
		edit_points,
		edit_extents,
		edit_map,
		edit_iz,
		edit_map_origin,
		edit_anchor_points
		//NOTENOTE: we unite two states
//		edit_ref_points
	};

//	CSettings	m_Settings;

//-----------------------------------------------------------------------------
// Purpose: for edit extents (HitTest-like states)
//-----------------------------------------------------------------------------
	enum Part
	{
		topleft		= 0,
		top		,
		topright, 
		right, 
		bottomright, 
		bottom, 
		bottomleft, 
		left
	};

	State	m_State;

//-----------------------------------------------------------------------------
// Purpose: Video stream play state , for masters
//-----------------------------------------------------------------------------
	enum PlayerState
	{
		stop,
		play
	};
//-----------------------------------------------------------------------------
// Purpose: Video stream record state , for masters
//-----------------------------------------------------------------------------
	enum RecordState
	{
		record_play,
		record_stop
	};

	PlayerState m_ps;
	RecordState m_rs;
//-----------------------------------------------------------------------------
// Purpose: Handles of compiling & merging threads
//-----------------------------------------------------------------------------
	HANDLE	m_hMerge;
	HANDLE	m_hCompile;

	void	EndCompile() { CloseHandle( m_hCompile );}
	void	EndMerge() { CloseHandle( m_hMerge );}
//-----------------------------------------------------------------------------
// Purpose: results when threads ended
//-----------------------------------------------------------------------------
	long	m_MergedID[2];

	long	m_nThreadRes;

//-----------------------------------------------------------------------------
// Purpose: Create correspondence
// Input  : CamerIDs - masters in correspondence
//			bMap - with map?
//-----------------------------------------------------------------------------
	void	CreateCommonArea(const std::vector<int>& CamerIDs, bool bMap);

//-----------------------------------------------------------------------------
// Purpose: Function for detecting existence of masters & map in correspondence
//-----------------------------------------------------------------------------
	bool	IsCurrent() const { return !m_Ids.empty(); }
	bool	IsCameraInCurrent(int Number) 
	{
		return std::find(m_Ids.begin(), m_Ids.end(), Number) != m_Ids.end();
	}
	bool	IsMapInCurrent()  const
	{
		return m_bMap;
	}

	bool	IsCompiling() const { return m_hCompile != 0 || m_hMerge != 0;}
	bool	CanCompile() const;

//-----------------------------------------------------------------------------
// Purpose: Convert master ID (from DB) <-> correspondence id
//-----------------------------------------------------------------------------
	long	IdToIndex(int Id) const;
	int		IndexToId(long Index) const;

	bool	Compile();
	bool	Merge(int FirstID, int SecondID);

	void	CreatePolygon(bool bWithMap);

private:
	bool	SetData();
	void	ResetInterfaceSettings();

//	void	ChangeID(int Oldid,  int  NewId);
public:
	void	FillGroupCameraMenu(CMenu& Menu, int nIndex) const;
	void	FillGroupMapMenu(CMenu& Menu) const;
private:
//-----------------------------------------------------------------------------
// Purpose: for serializing data
//-----------------------------------------------------------------------------
	// throw (VVMapTransException)
	void	Load();
	// throw (VVMapTransException)
	void	Save();
//NOTENOTE: not only in Save() & Load()
	int		LoadIZ();
	void	SaveIZ() const;	
	int		LoadAnchorPoints();
	void	SaveAnchorPoints() const;
	void	LoadMapOrigin();
	void	SaveMapOrigin() const;
	void	LoadMapScale();
	void	SaveMapScale() const;
	
	int		LoadRefPoints();
	bool	SaveRefPoints() const;

	void	LoadMapAlign();	

//	std::pair<CSize, CRect> GetMergeRect(int nIndex);

public:
	void	Fill(PointList& pl);
	void	Backup();

	void	Redo();
	void	Undo();

	bool	CanUndo() const;
	bool	CanRedo() const;

	CRect	GetMergeRect(int nIndex);
	CRect	GetMapPanoramaRect(CSize sizeMap);

	void	SetMastersID( const std::vector<int> &Ids );

	Bitmap*	CreateBitmap( int nIndex, CRect rcArea  );
	Bitmap* CreateMapBitmap( );

	std::pair<int, Part>		PtInExtent(int Index, CPoint pt);
	void	CreateTable();
	void	Renumber(const std::vector<int>& Current, const std::vector<int>& Need);


//-----------------------------------------------------------------------------
// Purpose:	get views
// Output : map or pointer of the correspondence view
//-----------------------------------------------------------------------------
	std::map<int, CCameraView*>		GetCameraViews();
	std::map<int, CMergeView*>		GetMergeViews();
	std::map<int, CExtentView*>		GetExtentViews();
	CVVMapEditorView*				GetMapView();
	CAnchorView*					GetAnchorView();
	CMapScaleView*					GetMapScaleView();	
	CAllMapView*					GetAllMapView();	

	Compressor&		GetCompressor() { return m_Compressor;}

	void	ExportMapPolygon( CString FileName ) const;
//	int		MinPoints() const;

	int		GetMaxGroup() const ;
	std::set<int>	GetAllValidGroups() const;

	bool	HasMapPoints() const;
	void	DeleteMapPoints() ;
	void	ScaleMap(float Scale);

	void	DeleteGroup( int Number );
	void	DeletePoint( int Group, int Master , bool Backup = true);

	void	AddPointToGroup( int nGroup, int nIndex, PointF point  );

//NOTENOTE: after CMainFrame::OnEditOptions invoked
	void	OnOptionChange();

//-----------------------------------------------------------------------------
// Purpose: Draw through getdc function
//-----------------------------------------------------------------------------
	void	DrawGetPoint();
	void	DrawHeight();
	void	DrawSelectPoint	(int GroupNumber);
	void	DrawAnchorPoint	(int nPoint		);
	void	DrawRefPoint	(int nPoint		);

//-----------------------------------------------------------------------------
// Purpose: Clear draw cache
//-----------------------------------------------------------------------------
	void	InvalidateSelectPoint();
	void	InvalidateHeight();
	void	InvalidateGetPoint();
	void	InvalidateAnchorPoint();
	void	InvalidateRefPoint();

	//NOTENOTE: Now in CSettings
//	static	DWORD GetColor(int Number);
	static	DWORD __stdcall MergeProc(void*);
	static	DWORD __stdcall CompileProc(void*);

	enum { IZ_INVALID_MASTER = -1};

//NOTENOTE: 
	std::vector<int>	FindWhereCorrPointExist( int nIndex ) const;

//-----------------------------------------------------------------------------
// Purpose: For current master index when edit iz
//-----------------------------------------------------------------------------
	int		GetIZMaster() const { return m_nIZMaster;}
	void	SetIZMaster(int nMaster = IZ_INVALID_MASTER )		{ m_nIZMaster = nMaster; }

//-----------------------------------------------------------------------------
// Purpose: IZ polygon to current master
// return:	IZ Number
//-----------------------------------------------------------------------------
	int		AddIZPolygon(const Polygon_t& Polygon );
	void	RemoveIZPolygon( int Number );

//-----------------------------------------------------------------------------
// Purpose: Functions dealing with ID_IZ_VIEW menu on ID_VIEW_IZ tool button
// Input  : nIndex - Index of master in correspondence
//-----------------------------------------------------------------------------
	void	AddIZView(int nIndex)		{ m_IZViewMasters.second.insert( nIndex ); }
	void	RemoveIZView(int nIndex)	{ m_IZViewMasters.second.erase( nIndex );	}
	void	ToggleIZView(int nIndex)	{ IsIZView(nIndex) ? RemoveIZView(nIndex) : AddIZView(nIndex); }
	void	ClearIZView()				{ m_IZViewMasters.second.clear();			}
	bool	IsIZView(int nIndex) const	{ return m_IZViewMasters.second.find(nIndex) != m_IZViewMasters.second.end();	}
//	const std::set<int>&	GetIZView() const					{ return m_IZViewMasters.second;};

//-----------------------------------------------------------------------------
// Purpose: deals with ID_VIEW_IZ button tool's state
//-----------------------------------------------------------------------------
	bool	IsShowIZ() const			{ return m_IZViewMasters.first;}
	void	ToggleShowIZ()				{ m_IZViewMasters.first = !m_IZViewMasters.first;}
	void	SetShowIZ()					{ m_IZViewMasters.first = true; }
//-----------------------------------------------------------------------------
// Purpose: for map oringin location
//-----------------------------------------------------------------------------	
	bool	IsSetMapOrigin() const		{ return m_MapOrigin.IsSet();	}
	void	SetMapOrigin(PointF point)	{ m_MapOrigin = point;	SaveMapOrigin();	}
	PointF	GetMapOrigin()		const	{ return m_MapOrigin.Get();		}
//-----------------------------------------------------------------------------
// Purpose: anchor point add/remove/change stuff
//-----------------------------------------------------------------------------	
	void	AddAnchorPoint(double x, double y, double z, int Relative = AnchorPoint_t::NO_DEPENDENCE);
	void	RemoveAnchorPoint( int Number );
	void	ChangeAnchorPoint( int Number, const AnchorPoint_t& Anchor );
	std::pair<bool, PointF>	GetAnchorPointOnMap( int nPoint) const;

	void	SetAnchorPointOnMap( int Number, long x, long y );
	void	RemoveAnchorPointFromMap( int Number);
//-----------------------------------------------------------------------------
// Purpose: map scale add/remove stuff (x & y axis )
//			we can add here some check or smth else
//-----------------------------------------------------------------------------	
	void	SetScaleX(double x);
	void	SetScaleY(double y);
	void	InvalidateScaleX()	{ m_ScaleX .first = false; }
	void	InvalidateScaleY()	{ m_ScaleY .first = false; }
	std::pair<bool, double>		GetScaleX() const { return m_ScaleX; }
	std::pair<bool, double>		GetScaleY() const { return m_ScaleY; }
	PointF	ScaleMapPoint(PointF ptfMap) const;

//-----------------------------------------------------------------------------
// Purpose: find active inv zone polygon 
// Input  : point - map point
//-----------------------------------------------------------------------------
	int		FindActiveIZ(PointF point) const ;
	void	SetActiveIZ( int Number = -1)	{ m_nActiveIZ = Number; }
	int		GetActiveIZ () const			{ return m_nActiveIZ;	}

//-----------------------------------------------------------------------------
// Purpose: Reference point add/remove/change stuff
//-----------------------------------------------------------------------------
	bool	AddRefPoint();
	bool	AddRefPointDistance(int nRefPoint, int Number, double fDist);
	bool	RemoveRefPoint(int nRefPoint);
	bool	RemoveRefPointDistance(int nRefPoint, int Number);
	bool	ChangeRefPointPoint(int nRefPoint, int Number, int nPoint);
	bool	ChangeRefPointDistance(int nRefPoint, int Number, double fDistance);
	bool	ChangeRefPoint( int nRefPoint, int Number, int nPoint, double fDistance );
	bool	IsValidRefPoint ( int nRefNumber) const;

	std::unique_ptr<CBitmapBackground>	CreateViewBitmap(  ) const;


//	bool	IsPointValid( int nGroupIndex ) ;

	std::unique_ptr<CFilterCache>	GetFilterCache();

	void	OnShowMaster();
	void	OnCloseMaster();

	const Polygon_t&	GetHoryzon(int nMasterIndex) const;
//	Bitmap*	GetViewBitmap() const { return m_ViewBitmap.get();}
private:
	
//	std::set<int>	m_FilterMasters;
	
//-----------------------------------------------------------------------------
// Purpose: iterative function
// Input  : Anchors - set of dependent points
//-----------------------------------------------------------------------------
	bool	FillRelativeAnchorPoints(std::set<int>& Anchors);
	void	RemoveDependentRefPoints( std::set<int>& Anchors );

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
	void	UndoStackResize();
	void	UndoStackErase();
public:
//-----------------------------------------------------------------------------
// Purpose: main interface
//-----------------------------------------------------------------------------	
	CVVMapTrans*						m_VVMapTrans;
//-----------------------------------------------------------------------------
// Purpose: current correspondence masters array
//-----------------------------------------------------------------------------	
	std::vector<int>					m_Ids;
//-----------------------------------------------------------------------------
// Purpose: Is map in correspondence
//-----------------------------------------------------------------------------	
	bool								m_bMap;
//-----------------------------------------------------------------------------
// Purpose: current map & masters polygons. Warnings!!! they are mutable
//-----------------------------------------------------------------------------	
	PolygonMap_t								m_Polygons;	
	PolygonArr_t								m_MapPolygons;
	
private:
	UndoStack<ExtentMap_t>						m_UndoExtents;
	UndoStack<PointList>						m_UndoPoints;
	UndoStack<InvZoneMap_t>						m_UndoIZPolygons;
	UndoStack< std::pair<AnchorPointMap_t, 
						 RefPointMap_t> >		m_UndoAncRefPoints;

	Compressor									m_Compressor;

	PolygonArr_t								m_Horyzons;

	void	DumpPoints( const long* Pts, int Number ) const;
	void	DumpPolygon( int from, int to, const POINT* Pts, int Number ) const;
	
	void	ResizeGroup(int nGroups);
	void	CreateQualityWnd();
	void	CreateHoryzon();
	void	PrepareData(int Number);

	//NOTENOTE: Index for edit IZ master
	int											m_nIZMaster;	
	//NOTENOTE: Indices for view IZ masters
	std::pair< bool, std::set<int> >			m_IZViewMasters;
	std::pair<bool, double>						m_ScaleX;
	std::pair<bool, double>						m_ScaleY;

	int											m_nActiveIZ;

//	std::vector<BYTE>							m_MapCache;
	bool										m_bSetMapSize;
public:

//-----------------------------------------------------------------------------
// Purpose: Function for undo/redo stack's current statte
//-----------------------------------------------------------------------------
	PointList&									Points()				{ return m_UndoPoints.Top();}
	const PointList&							Points() const			{ return m_UndoPoints.Top();}

	ExtentMap_t&								Extents()				{ return m_UndoExtents.Top();}
	const ExtentMap_t&							Extents() const			{ return m_UndoExtents.Top();}

	InvZoneMap_t&								IZPolygons()			{ return m_UndoIZPolygons.Top();}
	const InvZoneMap_t&							IZPolygons() const		{ return m_UndoIZPolygons.Top();}
/*
	AnchorPointMap_t&							AnchorPoints()			{ return m_UndoAnchorPoints.Top();}
	const AnchorPointMap_t&						AnchorPoints() const	{ return m_UndoAnchorPoints.Top();}

	RefPointMap_t&								RefPoints()				{ return m_UndoRefPoints.Top();}
	const	RefPointMap_t&						RefPoints()	const		{ return m_UndoRefPoints.Top();}*/
	AnchorPointMap_t&							AnchorPoints()			{ return m_UndoAncRefPoints.Top().first;}
	const AnchorPointMap_t&						AnchorPoints() const	{ return m_UndoAncRefPoints.Top().first;}

	RefPointMap_t&								RefPoints()				{ return m_UndoAncRefPoints.Top().second;}
	const	RefPointMap_t&						RefPoints()	const		{ return m_UndoAncRefPoints.Top().second;}

	void										SetMapCache( const std::vector<BYTE>& Arr);
//	const std::vector<BYTE>&					GetMapCache( ) const { return m_MapCache; }
//-----------------------------------------------------------------------------
// Purpose: Current masters & point for GetXY & GetExtent on other masters & map
//-----------------------------------------------------------------------------
	enum
	{
		COR_INVALID_MASTER = -1
	};
	std::pair<int, CPoint>						m_GetPoint;
	std::pair<int, CPoint>						m_HeightPoint;
	std::pair<int, CPoint>						m_AnchorPoint;
	std::pair<int, CPoint>						m_RefPoint;

	ExtentTable_t								m_ExtentTable;
//-----------------------------------------------------------------------------
// Purpose: triggers for showing (toggle on toolbar or menu)
//-----------------------------------------------------------------------------
	bool										m_bShowCommonPoints;
	bool										m_bShowGetPoints;
	bool										m_bShowHeights;
	bool										m_bShowExtents;
	bool										m_bShowPolygons;
	bool										m_bShowPointNumber;
	
private:
	//NOTENOTE: it's not container !!!
//	UndoStack<bool>								m_RefBackupHistory;			
//-----------------------------------------------------------------------------
// Purpose: origin of anchor coordinates
//-----------------------------------------------------------------------------
	Property<PointF>							m_MapOrigin;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVVMapEditorDoc)
	public:
	virtual void Serialize(CArchive& ar);
	virtual void DeleteContents();
	virtual void OnCloseDocument();
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CVVMapEditorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CVVMapEditorDoc)
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSaveAs(CCmdUI* pCmdUI);
	afx_msg void OnFileSavetable();
	afx_msg void OnUpdateFileSavetable(CCmdUI* pCmdUI);
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);

	afx_msg void OnFileOpenDB();
	afx_msg void OnFileSaveDB();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VVMAPEDITORDOC_H__70AC22E3_260B_44AE_A992_FA780CC23EC4__INCLUDED_)
