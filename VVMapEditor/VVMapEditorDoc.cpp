// VVMapEditorDoc.cpp : implementation of the CVVMapEditorDoc class
//

/************************************
  REVISION LOG ENTRY
  Revision By: Eugene V. Gorbachev
  Revised on 24.08.2004 15:12:49
  Comments: ...
 ************************************/


#include "stdafx.h"
#include "VVMapEditor.h"

#include "VVMapEditorDoc.h"
#include "Exception.h"


#include "CameraView.h"
#include "VVMapEditorView.h"
#include "AnchorView.h"
#include "MapScaleView.h"
#include "AllMapView.h"

#include "MainFrm.h"
#include "Array2D.h"
#include "MapPolygonExport.h"
#include "res/resource.h"
#include "db/DBSerializer.h"

#include <limits>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVVMapEditorDoc

IMPLEMENT_DYNCREATE(CVVMapEditorDoc, CDocument)

BEGIN_MESSAGE_MAP(CVVMapEditorDoc, CDocument)
	//{{AFX_MSG_MAP(CVVMapEditorDoc)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateFileSaveAs)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)

	ON_COMMAND( ID_FILE_SAVE_DB, OnFileSaveDB )
	ON_COMMAND( ID_FILE_OPEN_DB, OnFileOpenDB )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVVMapEditorDoc construction/destruction

CVVMapEditorDoc::CVVMapEditorDoc():
	m_VVMapTrans(0), 
	m_hMerge(0),
	m_hCompile(0),
	m_State(State::usual),
	m_bShowHeights(false),
	m_bShowCommonPoints(true),
	m_bShowExtents(true),
	m_bShowGetPoints(false),
	m_bShowPolygons(true),
	m_bShowPointNumber(true),
	m_GetPoint(		std::make_pair	( (int)COR_INVALID_MASTER, CPoint(0,0) ) ),
	m_HeightPoint(	std::make_pair	( (int)COR_INVALID_MASTER, CPoint(0,0) ) ),
	m_AnchorPoint(	std::make_pair	( (int)COR_INVALID_MASTER, CPoint(0,0) ) ),
	m_RefPoint(		std::make_pair	( (int)COR_INVALID_MASTER, CPoint(0,0) ) ),
	m_rs(RecordState::record_stop),
	m_ps(PlayerState::play),
	m_nIZMaster(IZ_INVALID_MASTER),
	m_ScaleX( std::make_pair( true, 1.0 )),
	m_ScaleY( std::make_pair( true, 1.0 )),
	m_nActiveIZ(-1),
	m_bSetMapSize(false)
{
	// TODO: add one-time construction code here
	GetVVMInterface(&m_VVMapTrans, VVMINT_EDITOR);
	ASSERT(m_VVMapTrans);

	UndoStackResize();

	m_IZViewMasters.first = true;
#ifdef USE_MAPSCALE
	SaveMapScale();
#endif

}

CVVMapEditorDoc::~CVVMapEditorDoc()
{
	if(m_VVMapTrans) m_VVMapTrans->_Release();
}



static CMainFrame* GetMainWnd() 
{
	return dynamic_cast<CMainFrame*>( AfxGetMainWnd() );
}


/////////////////////////////////////////////////////////////////////////////
// CVVMapEditorDoc serialization

void CVVMapEditorDoc::Serialize(CArchive& ar)
{
	CFile* pFile = ar.GetFile();
	if (ar.IsStoring())
	{
		// TODO: add storing code here

		try
		{

			VVError err(m_VVMapTrans);
			void* pInfo;
			long nSize;
			Save();
			err =  static_cast<CVVMapEditorApp*>(AfxGetApp())->m_bShort ?
					m_VVMapTrans->SaveShort( &pInfo, &nSize ):
					m_VVMapTrans->Save( &pInfo, &nSize );

			
			pFile->Write( pInfo, nSize );
		}
		catch(VVMapTransException& ex)
		{
			AfxMessageBox( Helper::Convert(ex.what()) );
			AfxThrowUserException();
		}

	}
	else
	{
		// TODO: add loading code here
		try
		{
			DWORD dwLength = pFile->GetLength();
			std::vector<BYTE> v;
			v.resize( dwLength );
			pFile->Read( &v[0], dwLength );


			VVError err(m_VVMapTrans);
			err = m_VVMapTrans->Load( &v[0], dwLength );

			GetMainWnd()->CloseMergeExtentWindows();
			Load();

		}
		catch(VVMapTransException& ex)
		{
			AfxMessageBox( Helper::Convert( ex.what() ));
			AfxThrowUserException();
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CVVMapEditorDoc diagnostics

#ifdef _DEBUG
void CVVMapEditorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CVVMapEditorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CVVMapEditorDoc commands



void	CVVMapEditorDoc::CreateCommonArea(const std::vector<int>& CameraIDs, bool bMap)
{
	PointList pl;
	

	PointMap_t	LastPoints		= Points().m_Points;
	PointArr_t	LastMapPoints	= Points().m_MapPoints;
	ExtentMap_t	LastExtents		= Extents();

	ClearIZView();
	m_Polygons.clear();
	m_MapPolygons.clear();
	m_ExtentTable.clear();
	
	UndoStackErase();
	UndoStackResize();

	int num = CameraIDs.size();
	
	m_Polygons.resize( num );
	m_MapPolygons.resize(num);
	m_ExtentTable.resize(num);
	m_Horyzons.resize( num );
	Extents().resize( num );
	Points().m_Points.resize( num );
	if(bMap) Points().m_MapPoints.resize( num );

	for(int i = 0; i < CameraIDs.size(); ++i)
	{
		std::vector<int>::const_iterator itr = std::find( m_Ids.begin(), m_Ids.end(), CameraIDs[i] );
		if( itr  != m_Ids.end() )
		{
			int Num = itr - m_Ids.begin();
			Points().m_Points[i] = LastPoints[Num];
			Extents()[i] = LastExtents[Num];
		}
	}

	if(bMap && m_bMap)
	{
		Points().m_MapPoints = LastMapPoints;
	}

	m_Ids = CameraIDs;

	for( /*int*/ i=0; i < num; ++i )
	{
		m_Polygons[i].resize( m_Ids.size() );
		m_Horyzons[i].clear();
	}
	GetMainWnd()->CreateUniteWndMenu(m_Ids);


	VVError Res( m_VVMapTrans );
	Res = m_VVMapTrans->SetNumOfMasters( m_Ids.size() );
	for(  i=0; i < m_Ids.size(); ++i )
	{
		m_VVMapTrans->SetMasterID( i, m_Ids[i] );
		//NOTENOTE: We want all views on start
		AddIZView( i );
	}

	m_bMap = bMap;

			int nMaxGroup = GetMaxGroup();			
			ResizeGroup(nMaxGroup + 1);

	UpdateAllViews(0);
}


struct FindMap
{
	bool operator()(const bool& p) const
	{
		return p == false;
	}
};

MACRO_EXCEPTION(CompileException, CommonException)


DWORD GetBmpLenght(LPBITMAP pBITMAP)
{
	if(pBITMAP->bmWidth <= 0 || pBITMAP->bmHeight <= 0)
		return 0;//to left bitmap

	return ((pBITMAP->bmWidth * pBITMAP->bmPlanes * pBITMAP->bmBitsPixel + 31) & ~31) /8 * pBITMAP->bmHeight;
}

bool CVVMapEditorDoc::Compile() 
{
	if( !IsCurrent() ) return false;
	m_nThreadRes = 0;
/*	if(Points().m_Points.size() < 2) 
	{
		AfxMessageBox("Wrong number of masters");
		return false;
	}*/

	try
	{
		SetData();
		m_hCompile = CreateThread( 0, 0, CompileProc, this, 0, 0 );
//		ThreadProc(this);
	}
	catch( VVMapTransException& ex)
	{
		CString err( _T("VVMapTrans - "));
		err += ex.what();
		AfxMessageBox(err, MB_OK|MB_ICONERROR);
		return false;
	}
	return true;
}


bool CVVMapEditorDoc::Merge(int FirstID, int SecondID) 
{
	PointList pl;
	VVError res(m_VVMapTrans) ;
	m_nThreadRes = 0;

	POSITION pos = GetFirstViewPosition();
	
//	CView * pView;
	std::map<int ,CCameraView*> Views = GetCameraViews();
	try
	{
		int nID[] = {	FirstID, SecondID	};
		for( int ind = 0 ; ind < 2 ; ++ind )
		{

			int z = nID[ind];
			int Index = IdToIndex( z );
			m_MergedID[ind] = Index;
			CSize sizeView = Views[z]->GetBackground()->GetSize();
			CDC dc;
			

			Bitmap		BackBmp(sizeView.cx, sizeView.cy);
			Graphics	BackBuffer(&BackBmp);

			Views[nID[ind]]->GetBackground()->Draw( BackBuffer, CRectF(0, 0, sizeView.cx, sizeView.cy));
			HBITMAP hBitmap = 0;
			BackBmp.GetHBITMAP( Color::Black, &hBitmap );
			CBitmap bmp;
			bmp.Attach( hBitmap );

			BITMAP bm;
			bmp.GetBitmap( &bm );

			BITMAPINFO	bi;
			memset(&bi, 0, sizeof(BITMAPINFO));
			bi.bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
			bi.bmiHeader.biWidth = bm.bmWidth;
			bi.bmiHeader.biHeight = bm.bmHeight;
			bi.bmiHeader.biPlanes = bm.bmPlanes;
			bi.bmiHeader.biBitCount = bm.bmBitsPixel;
			bi.bmiHeader.biCompression = BI_RGB;
			bi.bmiHeader.biSizeImage = GetBmpLenght( &bm );
/*

		CDC* pDC = AfxGetMainWnd()->GetDC();

		SetDIBitsToDevice(pDC->GetSafeHdc(), 0 + ind *355, 0, sizeView.cx, sizeView.cy, 0, 0, 0, sizeView.cy, bm.bmBits , &bi, DIB_RGB_COLORS);
		AfxGetMainWnd()->ReleaseDC(pDC);
*/
			res = m_VVMapTrans->SetMasterBitmap( Index, &bi, (BYTE*)bm.bmBits );

			bmp.DeleteObject();
		}
		m_hMerge = CreateThread( 0, 0, MergeProc, this, 0, 0 );
	}
	catch( VVMapTransException& ex)
	{
		CString err( _T("VVMapTrans - "));
		err += ex.what();
		AfxMessageBox(err, MB_OK|MB_ICONERROR);
		return false;
	}
	return true;
}

bool CVVMapEditorDoc::SetData()
{
	PointMap_t::const_iterator itr = Points().m_Points.begin();
	VVError res(m_VVMapTrans) ;

	std::map<int ,CCameraView*> Views = GetCameraViews();

	std::vector<long> CoordArr;
	

	
	DWORD pt =  GetMaxGroup();
	
	DWORD dw =  pt * (m_Ids.size() + 1) ;
	CoordArr.resize( dw * 2, -1L );

	POINT*	Coord = (POINT*)&CoordArr[0];

	CString s;
	s.Format( _T("========= points=%d\n =========\n"), pt);
	OutputDebugString( s );

	itr = Points().m_Points.begin();
	for( int Index = 0; itr != Points().m_Points.end() ; ++itr , ++Index)
	{
		int Id = IndexToId(Index);
		CSize sizeView = Views[Id]->GetBackground()->GetSize();
		CDC dc;
		

		Bitmap		BackBmp(sizeView.cx, sizeView.cy);
		Graphics	BackBuffer(&BackBmp);

		Views[Id]->GetBackground()->Draw( BackBuffer, CRectF(0, 0, sizeView.cx, sizeView.cy) );
		HBITMAP hBitmap = 0;
		BackBmp.GetHBITMAP( Color::Black, &hBitmap );
		CBitmap bmp;
		bmp.Attach( hBitmap );

		BITMAP bm;
		bmp.GetBitmap( &bm );

		BITMAPINFO	bi;
		memset(&bi, 0, sizeof(BITMAPINFO));
        bi.bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
        bi.bmiHeader.biWidth = bm.bmWidth;
        bi.bmiHeader.biHeight = bm.bmHeight;
        bi.bmiHeader.biPlanes = bm.bmPlanes;
        bi.bmiHeader.biBitCount = bm.bmBitsPixel;
		bi.bmiHeader.biCompression = BI_RGB;
		bi.bmiHeader.biSizeImage = GetBmpLenght( &bm );
		BYTE* pBytes = (BYTE*)bm.bmBits;
		res = m_VVMapTrans->SetMasterBitmap( Index, &bi, pBytes );
/*
		CDC* pDC = AfxGetMainWnd()->GetDC();

		SetDIBitsToDevice(pDC->GetSafeHdc(), 0 + i *355, 0, rc.Width(), rc.Height(), 0, 0, 0, rc.Height(), bm.bmBits , &bi, DIB_RGB_COLORS);
		AfxGetMainWnd()->ReleaseDC(pDC);
*/
		bmp.DeleteObject();
		
		
		for(int j = 0; j < pt; ++j)
		{
			const std::pair<bool, PointF>& ptz = (*itr)[j]; 
			if( ptz.first )
			{
				int PtIndex =  j * ( m_Ids.size() + 1) + Index + 1;
				PointF pf= ptz.second;
				Coord[ PtIndex ].x = pf.X;
				Coord[ PtIndex ].y = pf.Y;
			}
		}

		ExtentMap_t::const_iterator itrExt = Extents().begin( ) + Index;
		if( itrExt ->size())
		{
			int size = itrExt->size();
			std::vector<CRect> Rects( size );
			for( int i = 0; i < size; ++ i)
			{
				Rects[ i ] = (*itrExt)[i].ToCRect();
				CString str;
				str.Format(_T("Rect#%d=%d %d %d %d\n"), i, Rects[i].left, Rects[i].top, Rects[i].right, Rects[i].bottom );
				OutputDebugString(str);
			}
			m_VVMapTrans->SetExtentList( Index, size, &Rects[0] );
		}
		else
		{
			m_VVMapTrans->SetExtentList( Index, 0, (RECT*)1 );
		}
	}
	
	if( m_bMap )
	{
		for(int j = 0; j < pt; ++j)
		{
			int Ind =  j * (m_Ids.size()+1) ;
			std::pair<bool, PointF>& ptz = Points().m_MapPoints[j]; 
			if( ptz.first)
			{
				PointF pf= ptz.second;
				Coord[ Ind + 0].x = pf.X;
				Coord[ Ind + 0].y = pf.Y;
			}
			else
			{
				Coord[ Ind + 0].x = -1;
				Coord[ Ind + 0].y = -1;
			}
		}

        CRect rc;
		POSITION pos = GetFirstViewPosition();
		CVVMapEditorView* pView = dynamic_cast<CVVMapEditorView*>(GetNextView(pos));
		pView->GetClientRect(&rc);
		CDC dc;
		
		if(pView->m_Background.get()) 
		{
			CSize size = pView->m_Background->GetSize();
			rc.SetRect(0, 0, size.cx, size.cy);
		}
#if 0
		Bitmap		BackBmp(rc.Width(), rc.Height(), PixelFormat24bppRGB);
		Graphics	BackBuffer(&BackBmp);

		if(pView->m_Background.get()) 
		{
			pView->m_Background->Draw( BackBuffer, CRectF(0, 0, rc.Width(), rc.Height()) );
		}
		HBITMAP hBitmap = 0;
		BackBmp.GetHBITMAP( Color::Black, &hBitmap );
		CBitmap bmp;
		bmp.Attach( hBitmap );

		BITMAP bm;
		bmp.GetBitmap( &bm );

		BITMAPINFO	bi;
		memset(&bi, 0, sizeof(BITMAPINFO));
        bi.bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
        bi.bmiHeader.biWidth = bm.bmWidth;
        bi.bmiHeader.biHeight = bm.bmHeight;
        bi.bmiHeader.biPlanes = bm.bmPlanes;
        bi.bmiHeader.biBitCount = bm.bmBitsPixel;
		bi.bmiHeader.biCompression = BI_RGB;
		bi.bmiHeader.biSizeImage = GetBmpLenght( &bm );

		res = m_VVMapTrans->SetMapBitmap( &bi, (BYTE*)bm.bmBits );
		bmp.DeleteObject();
#else
		if(m_bSetMapSize)
			m_VVMapTrans->SetMapSize( rc.Width(), rc.Height() );
#endif
	}
	DumpPoints( &CoordArr[0], CoordArr.size());

	long * zzz = &CoordArr[0]; 
	if(!zzz) zzz = (long*)1;
	res = m_VVMapTrans->SetPointList( pt,  zzz );

	return true;
}

DWORD  __stdcall CVVMapEditorDoc::MergeProc(void* pParam)
{
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)pParam;
	long res ;
	res = pDoc->m_VVMapTrans->Merge(pDoc->m_MergedID[0], pDoc->m_MergedID[1]);
	if(res)
	{
		pDoc->m_nThreadRes = res;
		return 11;
	}
	return 10;
}


DWORD  __stdcall CVVMapEditorDoc::CompileProc(void* pParam)
{
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)pParam;
	long res ;
	res = pDoc->m_VVMapTrans->Compile();
	if(res)
	{
		pDoc->m_nThreadRes = res;
		return 11;
	}
	return 10;
}


void	CVVMapEditorDoc::CreatePolygon(bool bWithMap)
{
//	VVError err(m_VVMapTrans);
	long err = 0;
	try
	{
	
	std::map<int ,CCameraView*> Views = GetCameraViews();

	long	num;
	POINT*	buf;
	
	GetMainWnd()->SetMerged();

	for( int i = 0 ; i < m_Ids.size(); ++i  )
	{
		for( int j = 0 ; j < m_Ids.size(); ++j)
		{
			if( i != j)
			{
				err = m_VVMapTrans->GetMasterPolygon( j , i, &num, &buf );
				CString sss; sss.Format(_T("Failed to create polygon %d->%d\n"), j, i );
				OutputDebugString(sss);
				if(err != 0)
				{
					m_Polygons[ i ][j].clear();
					continue;
				}
				m_Polygons[ i ] [ j ].resize( num );
				for(int k = 0 ; k < num; ++k)
				{
					m_Polygons[ i ][ j ] [ k ] =  PointF(buf[k].x, buf[k].y) ;
				}
				DumpPolygon( j, i, buf, num );
			}
		}
	}
	
	if(bWithMap)
	{
		if( m_bMap)
		for( i = 0 ; i < m_Ids.size(); ++i)
		{
			err = m_VVMapTrans->GetMapPolygon( i, &num, &buf );
				CString sss; sss.Format(_T("Failed to create polygon %d->map\n"), i );
				OutputDebugString(sss);
			if(err != 0)
			{
				m_MapPolygons[ i ].clear();
				continue;
			}
			m_MapPolygons[ i ] .resize( num );
			for(int k = 0 ; k < num; ++k)
			{
				m_MapPolygons[ i ] [ k ] =  PointF(buf[k].x, buf[k].y) ;
			}
			DumpPolygon( -1, i, buf, num );
		}
	}

	CreateTable();

	CreateQualityWnd();

	CreateHoryzon();

	CString str;
	str.Format( _T("%d"), num);
	OutputDebugString(str);

	GetAllMapView()->UpdateBitmap();
	UpdateAllViews(0);
	}
	catch(VVMapTransException& ex)
	{
		CString str;
//		str.Format( /*"%d,*/"%s",/* ex.GetError(),*/ ex.what() );
		AfxMessageBox( Helper::Convert( ex.what() )  );
	}
}

void CVVMapEditorDoc::DeleteContents() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	m_Polygons.clear();
	m_MapPolygons.clear();
	m_Ids.clear();
	m_ExtentTable.clear();
#if 0
	Extents().clear();
	Points().m_Points.clear();
	Points().m_MapPoints.clear();
#endif
	m_bMap = false;

	AnchorPoints().clear();
	RefPoints().clear();

	UndoStackErase();
	
	UndoStackResize();
	ResetInterfaceSettings();

	m_bSetMapSize = false;

	CDocument::DeleteContents();
}

bool	CVVMapEditorDoc::CanCompile() const
{
/*	const int MinPointNumber = 4;
	if(IsCurrent())
	{
		PointMap_t::const_iterator itr = m_Points.m_Points.begin();
		for(; itr != m_Points.m_Points.end(); ++itr)
		{
			if( itr->size() < MinPointNumber) return false;
		}
		return true;
	}
	else return false;*/ return IsCurrent();
}

long	CVVMapEditorDoc::IdToIndex(int Id) const throw (VVMapTransException)
{	
/*	CString z;
	z.Format(_T("%d"), Id);
	AfxMessageBox(z);*/
	std::vector<int>::const_iterator itr =  std::find( m_Ids.begin(), m_Ids.end(), Id );
	if( itr != m_Ids.end())
		return itr - m_Ids.begin();
	else
		throw VVMapTransException("Index not found");
	return 0;
}

int		CVVMapEditorDoc::IndexToId(long Index) const throw (VVMapTransException)
{
/*	VVError res(m_VVMapTrans);
	long ind;
	res = m_VVMapTrans->GetMasterID( Index, &ind );*/
	if( Index >= m_Ids.size() ) throw VVMapTransException( "Wrong index" );
	return m_Ids[Index];
}

void	CVVMapEditorDoc::Load() throw (VVMapTransException)
{
	VVError res(m_VVMapTrans);

	long Number;
	res = m_VVMapTrans->GetNumOfMasters(&Number);

	m_bMap = false;

	UndoStackErase();
	UndoStackResize();

	PrepareData(Number);

	SetIZMaster(0);

	for( int  i=0; i < Number; ++i )
	{
		long id;
		m_VVMapTrans->GetMasterID( i, &id );
		//NOTENOTE: we want see all IZ
		m_IZViewMasters.second.insert( i );

		m_Ids[i] = id;

		long	NumberRect;
		RECT*	Rects = 0;
		long NotExcRes = m_VVMapTrans->GetExtentList( i, &NumberRect, (RECT**)&Rects );
		if( !NotExcRes )
		{
			Extents()[i].resize(NumberRect, CRectF(0, 0, 0, 0));
			for(int j =0; j < NumberRect; ++j)
			{
				Extents()[i][j] = CRectF( Rects[j] );
			}
		}
	}

	GetMainWnd()->Renumber(m_Ids);
	GetMainWnd()->SetVisible(m_Ids);

	GetMainWnd()->CreateUniteWndMenu(m_Ids);



	CPoint*	Points;
	long	PointNumber;
	res = m_VVMapTrans->GetPointList(  &PointNumber, (long**)&Points );

	for( i = 0; i < PointNumber; ++i  )
	{
		int ind = (Number+1)*i;
		if(Points[ind]  != CPoint(-1,-1))
		{
			CVVMapEditorDoc::Points().m_MapPoints.push_back( 
				std::make_pair( true, PointF( Points[ ind ].x, Points[ind].y ) ) );
			m_bMap = true;
		}
		else
		{
			CVVMapEditorDoc::Points().m_MapPoints.push_back( 
				std::make_pair( false, PointF( -1.0f, -1.0f ) ) );		// no such point in group
		}
		for( int j = 1; j < Number+1; ++j )
		{
			ind = j + (Number+1) * i; 
			long id;
			m_VVMapTrans->GetMasterID( j-1, &id );
			CPoint& z = Points[ind] ;
			if( Points[ind]  != CPoint(-1,-1))
			{
				CVVMapEditorDoc::Points().m_Points[j-1].push_back(  
					std::make_pair( true, PointF( Points[ind].x, Points[ind].y ) ));
			}
			else	// no such point in group
			{
				CVVMapEditorDoc::Points().m_Points[j-1].push_back(  
					std::make_pair( false, PointF( -1.0f, -1.0f )  ));
			}
		}
	}

	long err;
		POINT*	buf;

	for(  i = 0 ; i < m_Ids.size(); ++i  )
	{
		for( int j = 0 ; j < m_Ids.size(); ++j)
		{
			if( i != j)
			{
				err = m_VVMapTrans->GetMasterPolygon( j , i, &Number, &buf );
				if(err) continue;
				m_Polygons[ i ] [ j ].resize( Number );
				for(int k = 0 ; k < Number; ++k)
				{
					m_Polygons[ i ][ j ] [ k ] =  PointF(buf[k].x, buf[k].y) ;
				}
			}
		}
	}
	
	if(true)
	{
		if( m_bMap)
		for( i = 0 ; i < m_Ids.size(); ++i)
		{
			err = m_VVMapTrans->GetMapPolygon( i, &Number, &buf );
			if(err) continue;
			m_MapPolygons[ i ] .resize( Number );
			for(int k = 0 ; k < Number; ++k)
			{
				m_MapPolygons[ i ] [ k ] =  PointF(buf[k].x, buf[k].y) ;
			}
		}
	}

	long	nZoneNumber;
	res = m_VVMapTrans->IZ_GetNumOfZones(&nZoneNumber);
	for(i =0; i < nZoneNumber; ++i)
	{
		long	nMaster; long nPointNumber;
		POINT*	pptInvZone;
		res = m_VVMapTrans->IZ_Get( i, &nMaster, &nPointNumber , &pptInvZone);
		Polygon_t IZPolygon( nPointNumber );
		for(int j=0; j < nPointNumber; ++j)
		{
			IZPolygon[j].X = pptInvZone[j].x;
			IZPolygon[j].Y = pptInvZone[j].y;
		}
		IZPolygons().push_back( InvZone_t(IZPolygon, i, nMaster ));
	}

	CreateTable();

	int NumberIZ = LoadIZ();
	int NumberAP = LoadAnchorPoints();
	int NumberRP = LoadRefPoints();
	//NOTENOTE: if we save IZ or Anchors we set map in correspondece to
	if(  !m_bMap) m_bMap = ( NumberAP  || NumberIZ || NumberRP);
#ifdef USE_MAPSCALE
	LoadMapOrigin();
	LoadMapScale();
#endif
	LoadMapAlign();
	GetMainWnd()->SetMerged();
	CreateQualityWnd();
/*
	BITMAPINFO * pbi; BYTE* pbits;  
	m_VVMapTrans->GetMasterBitmap(0, &pbi, &pbits);*/
}

void	CVVMapEditorDoc::Save()  throw(VVMapTransException)
{
	SetData();
}

void	CVVMapEditorDoc::Fill(PointList& pl)  throw(VVMapTransException)
{
	VVError err(m_VVMapTrans);

	try
	{
	m_bMap = true;
	std::vector<int> nID = m_Ids;

	long	num;
	POINT*	buf;
	
	for( int i = 0 ; i < nID.size(); ++i  )
	{
		for( int j = 0 ; j < nID.size(); ++j)
		{
			if( i != j)
			{
				err = m_VVMapTrans->GetMasterPolygon( j , i, &num, &buf );
				(m_Polygons[ i ] [ j ]).resize( num );
				for(int k = 0 ; k < num; ++k)
				{
					m_Polygons[ i ][ j ] [ k ] =  PointF(buf[k].x, buf[k].y) ;
				}
			}
		}
	}
	
	m_bMap = false;
	for( i = 0 ; i < nID.size(); ++i)
	{
		err = m_VVMapTrans->GetMapPolygon( i, &num, &buf );
		m_MapPolygons[ nID[i] ] .resize( num );
		for(int k = 0 ; k < num; ++k)
		{
			m_bMap = true;
			m_MapPolygons[ i] [ k ] =  PointF(buf[k].x, buf[k].y) ;
		}
	}	

	}
	catch(VVMapTransException&)
	{
	}
}

void CVVMapEditorDoc::OnUpdateFileSave(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if( !IsCurrent() ) pCmdUI->Enable(FALSE);
}

void CVVMapEditorDoc::OnUpdateFileSaveAs(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
}

void CVVMapEditorDoc::OnFileSavetable() 
{
	// TODO: Add your command handler code here
/*	CFileDialogEx dlgFile( FALSE, "*.vmt", 0, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter );

	if( dlgFile.DoModal() == IDOK )
	{

	}*/
}

void CVVMapEditorDoc::OnUpdateFileSavetable(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
}


bool	CVVMapEditorDoc::CanUndo() const
{
	switch( m_State )
	{
	case State::edit_extents:
				return m_UndoExtents.CanUndo();
	case State::edit_points : 
				return m_UndoPoints.CanUndo();
	case State::edit_iz : 
				return m_UndoIZPolygons.CanUndo();
	case State::edit_anchor_points: 
/*				return m_UndoAnchorPoints.CanUndo();
	case State::edit_ref_points: 
				return m_UndoRefPoints.CanUndo();*/
				return m_UndoAncRefPoints.CanUndo();
	}
	return false;
}

bool	CVVMapEditorDoc::CanRedo() const
{
	
	switch( m_State )
	{
	case State::edit_extents:
				return m_UndoExtents.CanRedo();
	case State::edit_points : 
				return m_UndoPoints.CanRedo();
	case State::edit_iz : 
				return m_UndoIZPolygons.CanRedo();
	case State::edit_anchor_points: 
/*				return m_UndoAnchorPoints.CanRedo();
	case State::edit_ref_points: 
				return m_UndoRefPoints.CanRedo();*/
				return m_UndoAncRefPoints.CanRedo();
	}
	return false;
}

void CVVMapEditorDoc::OnEditUndo() 
{
	// TODO: Add your command handler code here
	switch( m_State )
	{
	case State::edit_extents:
				m_UndoExtents.Undo();
				SetModifiedFlag();
				break;
	case State::edit_points : 
				m_UndoPoints.Undo();
				SetModifiedFlag();
				break;
	case State::edit_iz : 
				m_UndoIZPolygons.Undo();
				//NOTENOTE: Save coz IZ_AddList not in Compile()
				SaveIZ();
				SetActiveIZ();
				SetModifiedFlag();
				break;
	case State::edit_anchor_points:
/*				m_UndoAnchorPoints.Undo();
				SetModifiedFlag();
				SaveAnchorPoints();
				GetAnchorView()->UpdateList();
				break;
	case State::edit_ref_points:
				m_UndoRefPoints.Undo();
				SetModifiedFlag();
				SaveRefPoints();
				GetMapScaleView()->UpdateTree();*/
				m_UndoAncRefPoints.Undo();
				SetModifiedFlag();
				SaveAnchorPoints();
				SaveRefPoints();
				GetMapScaleView()->UpdateTree();
				GetAnchorView()->UpdateList();				
				break;
	}

	UpdateAllViews(0);
}

void CVVMapEditorDoc::OnUpdateEditUndo(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if( !CanUndo()) pCmdUI->Enable( FALSE);
}

void CVVMapEditorDoc::Backup() 
{
	switch( m_State )
	{
	case State::edit_extents:
				m_UndoExtents.Backup();
				SetModifiedFlag();
				break;
	case State::edit_points : 
				m_UndoPoints.Backup(  );
				SetModifiedFlag();
				break;
	case State::edit_iz : 
				m_UndoIZPolygons.Backup(  );
				SetModifiedFlag();
	case State::edit_anchor_points : 
/*				m_UndoAnchorPoints.Backup(  );
				SetModifiedFlag();
				break;
	case State::edit_ref_points:
				m_UndoRefPoints.Backup();
				SetModifiedFlag();
				break;*/
				m_UndoAncRefPoints.Backup();
				SetModifiedFlag();
				break;
	}
	
}

void CVVMapEditorDoc::OnEditRedo() 
{
	// TODO: Add your command handler code here


	switch( m_State )
	{
	case State::edit_extents:
				m_UndoExtents.Redo();
//				m_Extents = m_UndoExtents.Top();
				SetModifiedFlag();
				break;
	case State::edit_points : 
				m_UndoPoints.Redo();
//				m_Points = m_UndoPoints.Top();
				SetModifiedFlag();
				break;
	case State::edit_iz : 
				m_UndoIZPolygons.Redo();
				//NOTENOTE: Save coz IZ_AddList not in Compile()
				SaveIZ();
				SetActiveIZ();
				SetModifiedFlag();
				break;
	case State::edit_anchor_points:
/*				m_UndoAnchorPoints.Redo();
				//NOTENOTE: Save coz SP_AddList not in Compile()
				SaveAnchorPoints();
				GetAnchorView()->UpdateList();
				SetModifiedFlag();
				break;
	case State::edit_ref_points:
				m_UndoRefPoints.Redo();
				//NOTENOTE: Save coz RP_AddList not in Compile()
				SaveRefPoints();
				//NOTENOTE: explicit tree updating
				GetMapScaleView()->UpdateTree();
				SetModifiedFlag();
				break;*/
				SetModifiedFlag();
				m_UndoAncRefPoints.Redo();
				SaveAnchorPoints();
				SaveRefPoints();
				GetAnchorView()->UpdateList();
				GetMapScaleView()->UpdateTree();
				break;
	}
	UpdateAllViews(0);
}

void CVVMapEditorDoc::OnUpdateEditRedo(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if( !CanRedo()) pCmdUI->Enable( FALSE);
}

CRect CVVMapEditorDoc::GetMergeRect(int nIndex) throw (VVMapTransException)
{
	std::map<int ,CCameraView*> Views = GetCameraViews();

	CSize sizeClient = Views[ IndexToId( nIndex ) ]->GetBackground()->GetSize();
	CRect	rcArea(CPoint (0,0) , sizeClient);
//	CRect	szArea( sizeClient );
		
	long	xto, yto;

	long res;
	for( int i=0; i < m_Ids.size(); ++i)
	{
		if( i == nIndex  ) continue;
		CRect rcClient;
		Views[ IndexToId(nIndex) ]->GetClientRect(&rcClient);
		for( long x = 0; x < sizeClient.cx; x++)
			for( long y = 0; y < sizeClient.cy;y++)
			{
				res = m_VVMapTrans->GetXY( i, x, y, nIndex, &xto, &yto );

				if(res) continue;
				if( rcArea.left > xto) rcArea.left = xto;
				else if( rcArea.right < xto) rcArea.right = xto;
				if( rcArea.top > yto) rcArea.top = yto;
				else if( rcArea.bottom < yto) rcArea.bottom = yto;
			}
	}
	rcArea.InflateRect(1,1,1,1);

		CSize sizeMax ( GetSettings().GetProperty( prop_PanoramaMaxX ) , 
						GetSettings().GetProperty( prop_PanoramaMaxY ) );
		
		CSize sizeWindow = sizeClient;
		if( rcArea.Width() > sizeMax.cx )
		{
			rcArea.left = ( - sizeMax.cx + sizeWindow.cx)/2;
			rcArea.right = ( sizeMax.cx + sizeWindow.cx)/2;
		}
		if( rcArea.Height() > sizeMax.cy )
		{
			rcArea.top = ( - sizeMax.cy + sizeWindow.cy)/2;
			rcArea.bottom = ( sizeMax.cy + sizeWindow.cy)/2;
		}
	

	return rcArea;
}


std::pair<int,CVVMapEditorDoc::Part>		CVVMapEditorDoc::PtInExtent(int Index, CPoint pt)
{
	ASSERT( Index < Extents().size() );
	const std::vector<CRectF>& Arr = Extents()[Index];
	PointF ptf(pt.x, pt.y);
	const float c_fExtent = 5.0f;
//	const float c_fExtentY = 5.0f;

	for( int i=0; i < Arr.size(); ++i)
	{
		const CRectF& rc = Arr[i];
/*		CRectF rc1(rc), rc2(rc);

		RectF tl( rc.Left(), rc.Top(), 3,3 );
		RectF tr( rc.Right(), rc.Top(), 3,3 );
		RectF br( rc.Right(), rc.Bottom(), 3,3 );
		RectF bl( rc.Left(), rc.Bottom(), 3,3 );

		rc1.Inflate(2,2,2,2);
		rc2.Deflate(2,2,2,2);

		if( !rc1.Contains( ptf ) || rc2.Contains(ptf)) continue;

		if( tl.Contains(ptf )) return std::make_pair( i, Part::topleft );
		else if( tr.Contains(ptf )) return std::make_pair( i, Part::topright );
		else if( br.Contains(ptf )) return std::make_pair( i, Part::bottomright );
		else if( bl.Contains(ptf )) return std::make_pair( i, Part::bottomleft );
		else if( ptf.X < rc.X ) return std::make_pair( i, Part::left );
		else if( ptf.X > rc.X ) return std::make_pair( i, Part::right );
		else if( ptf.Y < rc.Y ) return std::make_pair( i, Part::top );
		else if( ptf.Y > rc.Y ) return std::make_pair( i, Part::bottom );*/
		RectF Areas[8] = 
		{
			RectF( rc.X - c_fExtent, rc.Y - c_fExtent, 2 * c_fExtent, 2 * c_fExtent ),
			RectF( rc.X + c_fExtent, rc.Y - c_fExtent, rc.Width - 2 * c_fExtent, 2 * c_fExtent ),
			RectF( rc.X + rc.Width - c_fExtent, rc.Y - c_fExtent, 2 * c_fExtent, 2 * c_fExtent ),

			RectF( rc.X + rc.Width - c_fExtent, rc.Y + c_fExtent, 2 * c_fExtent, rc.Height - 2 * c_fExtent ),
			RectF( rc.X + rc.Width - c_fExtent, rc.Y + rc.Height - c_fExtent, 2 * c_fExtent, 2 * c_fExtent ),

			RectF( rc.X + c_fExtent, rc.Y + rc.Height - c_fExtent, rc.Width - 2 * c_fExtent, 2 * c_fExtent ),
			RectF( rc.X - c_fExtent, rc.Y + rc.Height - c_fExtent, 2 * c_fExtent, 2 * c_fExtent ),

			RectF( rc.X - c_fExtent, rc.Y + c_fExtent, 2 * c_fExtent, rc.Height - 2 * c_fExtent ),
		};

		for( int j = 0; j < 8 ; ++j)
		{
			if( Areas[j].Contains( ptf ) ) return std::make_pair( i, Part(j) );
		}
	}
	return std::make_pair( -1, Part::top );
}

std::map<int , CCameraView*> CVVMapEditorDoc::GetCameraViews()
{
	POSITION pos = GetFirstViewPosition();
	
	CView * pView;
	std::map<int ,CCameraView*> Views;

	pView = GetNextView( pos );	// CVVMapEditorView
	for( ; pos != NULL; )
	{
		CCameraView *pView = dynamic_cast<CCameraView *>(GetNextView( pos ));
		if( pView ) Views.insert( std::make_pair( pView->m_Number, pView ) );
	}
	return Views;
}

std::map<int, CExtentView*> CVVMapEditorDoc::GetExtentViews()
{
	POSITION pos = GetFirstViewPosition();
	
	CView * pView;
	std::map<int ,CExtentView*> Views;

	pView = GetNextView( pos );	// CVVMapEditorView
	for( ; pos != NULL; )
	{
		CExtentView *pView = dynamic_cast<CExtentView *>(GetNextView( pos ));
		if( pView ) Views.insert( std::make_pair( pView->m_Index, pView ) );
	}
	return Views;
}


CVVMapEditorView* CVVMapEditorDoc::GetMapView()
{
	POSITION pos = GetFirstViewPosition();
//NOTENOTE: currently CVVMapEditorView is the first view class in the document's array, but check until last
	for( ; pos != NULL; )
	{
		CView* pView = GetNextView( pos );
		CVVMapEditorView *pMapView = dynamic_cast<CVVMapEditorView *>(pView);
		if( pMapView ) return pMapView;
	}
	return 0;	
}

CAnchorView* CVVMapEditorDoc::GetAnchorView()
{
	POSITION pos = GetFirstViewPosition();
	for( ; pos != NULL; )
	{
		CView* pView = GetNextView( pos );
		CAnchorView *pMapView = dynamic_cast<CAnchorView *>(pView);
		if( pMapView ) return pMapView;
	}
	return 0;	
}

CMapScaleView* CVVMapEditorDoc::GetMapScaleView()
{
	POSITION pos = GetFirstViewPosition();
	for( ; pos != NULL; )
	{
		CView* pView = GetNextView( pos );
		CMapScaleView *pMapView = dynamic_cast<CMapScaleView *>(pView);
		if( pMapView ) return pMapView;
	}
	return 0;	
}

CAllMapView*	CVVMapEditorDoc::GetAllMapView()
{
	POSITION pos = GetFirstViewPosition();
	for( ; pos != NULL; )
	{
		CView* pView = GetNextView( pos );
		CAllMapView *pMapView = dynamic_cast<CAllMapView *>(pView);
		if( pMapView ) return pMapView;
	}
	return 0;
}

Bitmap*	CVVMapEditorDoc::CreateBitmap( int nIndex, CRect rcArea )
{
	std::map<int ,CCameraView*> Views = GetCameraViews();

	CSize sizeClient = Views[ IndexToId( nIndex ) ]->GetBackground()->GetSize();

/*	CRect */ rcArea = GetMergeRect(nIndex);

	CPoint ofs = CPoint( -rcArea.left, -rcArea.top );
	CRect rcClient(CPoint(0,0), sizeClient );

	Bitmap* Bmp = ::new Bitmap( rcArea.Width(), rcArea.Height() );
	Graphics	gr(Bmp);
	gr.Clear( Color::LightGray );
	gr.TranslateTransform( ofs.x, ofs.y );
	Views[IndexToId( nIndex )]->GetBackground()->Draw( gr, CRectF(0, 0, sizeClient.cx, sizeClient.cy) );
	gr.TranslateTransform( 0, 0 );

	BitmapData BmpData;
	Bmp->LockBits( &Rect(0, 0, rcArea.Width(), rcArea.Height()), ImageLockModeWrite , PixelFormat32bppARGB, &BmpData );
	
	Array2D<  Color > BmpArr( (Color*)BmpData.Scan0, BmpData.Width, BmpData.Height, BmpData.Stride );

	std::vector< Bitmap* > MasterBmp ( m_Ids.size(), 0  ) ;
	std::vector< BitmapData > MasterBitmapData( m_Ids.size() );
	std::vector< CRect >	 MasterSizes( m_Ids.size() );

	for(int i=0; i < m_Ids.size(); ++i)
	{
	
		int nID = IndexToId(i) ;
		CSize sizeView = Views[ nID ]->GetBackground()->GetSize();
	
		MasterBmp[i] = ::new Bitmap( sizeView.cx, sizeView.cy );
		Graphics gr( MasterBmp[i] );
		Views[ nID ]->GetBackground()->Draw( gr , CRectF(0, 0, sizeView.cx, sizeView.cy) );
		MasterBmp[i]->LockBits( &Rect(0, 0, sizeView.cx, sizeView.cy), ImageLockModeRead , PixelFormat32bppARGB, &MasterBitmapData[i] );
		MasterSizes[i] = CRect( CPoint ( 0, 0) , sizeView ) ;
	}
	///////////////////////////////////////////////////////////
	CPoint ptOffset( -rcArea.left, -rcArea.top); 
	CSize szAll( rcArea.Width(), rcArea.Height() );
	CRect rcCamera(  MasterSizes[nIndex] );
	CSize szSource = rcCamera.Size();
	CRect rcSource( ptOffset, szSource );

	long err, xto, yto;
	for( int x = 0; x < szAll.cx ; ++x)
	{
		for( int y = 0; y < szAll.cy; ++y)
		{
			//NOTENOTE: if we are not in source rect then try
#if 0
			if( !rcSource.PtInRect( CPoint( x, y ) ) )
#endif
			for(int i = 0; i < m_Ids.size(); ++i)
			{
				if( i != nIndex )
				{
					long xfrom = x - ptOffset.x;
					long yfrom = y - ptOffset.y;
					err = m_VVMapTrans->GetXY( nIndex, xfrom, yfrom, i, &xto, &yto );
					if(!err &&  MasterSizes[i].PtInRect( CPoint ( xto, yto) ) )
					{
						Array2D<  Color > ViewArr( (Color*)MasterBitmapData[i].Scan0, 
													MasterBitmapData[i].Width, 
													MasterBitmapData[i].Height, 
													MasterBitmapData[i].Stride );

						BmpArr(x, y) = ViewArr(xto, yto);
						//NOTENOTE: find first master
						break;
					}
				}
			}
		}
	}

	for( i=0; i < m_Ids.size(); ++i)
	{
		MasterBmp[i]->UnlockBits( &MasterBitmapData[i] );
		::delete MasterBmp[i];
	}

	Bmp->UnlockBits(&BmpData);
	return Bmp;
}

void	CVVMapEditorDoc::CreateTable()
{
	//NOTENOTE: comment out for the debug purpose
//#define _DUMP_EXTENT_
	long*	Table;
	long	size;
//	m_ExtentTable.clear();
	for( int i=0; i< m_Ids.size(); ++i)
	{
		long res = m_VVMapTrans->GetExtentTable( i, &Table, &size );
		if( res ) continue;
		long cx = Table[0];
		long cy = Table[1];
		ASSERT( cx * cy == size );
		m_ExtentTable[i].Resize( cx, cy );
#ifdef _DUMP_EXTENT_
		CFile fileDump(_T("_dump_extent.txt"), CFile::modeCreate | CFile::modeWrite);
		memset(  &m_ExtentTable[i](0,0), 0xCC, cx * cy * sizeof(long));
#endif _DUMP_EXTENT_
		for( int k = 0; k < cy; ++k)
		{
			for( int l = 0; l < cx; ++l  )
			{
				m_ExtentTable[i](l, k) = 20000/ abs( Table[ 2 + cx * k + l ] );
			#ifdef _DUMP_EXTENT_
				CString str;
				str.Format(_T("%04x "), m_ExtentTable[i](l, k) );
				fileDump.Write(str, str.GetLength()  );
			#endif _DUMP_EXTENT_
			}
		#ifdef _DUMP_EXTENT_
			const TCHAR* z = "\n";
			fileDump.Write( z, sizeof(TCHAR));
		#endif _DUMP_EXTENT_
		}
	}
}

void	CVVMapEditorDoc::Renumber(const std::vector<int>& Current, const std::vector<int>& Need)
{
	std::vector<int > NewIds;
	NewIds.reserve( m_Ids.size() );
	for( int i=0; i < m_Ids.size(); ++i)
	{
//		if( Current[i] != Need[i])
//		{
			std::vector<int>::const_iterator itr = std::find(Current.begin(), Current.end(), m_Ids[i]);
			int z = itr - Current.begin();
			if( itr != m_Ids.end() )
			{
				NewIds.push_back( Need[z] );
			}
//		}
	}
	m_Ids = NewIds;
	for(  i=0; i < m_Ids.size(); ++i)
	{
		m_VVMapTrans->SetMasterID( i, m_Ids[i]  );
	}
}

//NOTENOTE: obsolete function since DkClient use our VME files
void	CVVMapEditorDoc::ExportMapPolygon( CString FileName ) const
{
	CFile file( FileName, CFile::modeCreate | CFile::modeWrite );

	for( int i=0; i < m_Ids.size(); ++i)
	{
		if( !m_MapPolygons[i].empty() )
		{
			Elvees::InfoCamera infc;
			infc.dwType = 1;
			infc.dwSize = sizeof( Elvees::InfoHeader ) + 2 * sizeof( DWORD ) + sizeof(POINT) * m_MapPolygons[i].size();
			infc.dwCameraID = m_Ids[i];
			infc.dwPointNumber = m_MapPolygons[i].size();
			
			file.Write( &infc, sizeof( Elvees::InfoCamera )  - sizeof(POINT) );
			file.Write( &m_MapPolygons[i][0], m_MapPolygons[i].size() * sizeof(POINT) );
		}

	}
}
/*

int		CVVMapEditorDoc::MinPoints() const
{
	DWORD Min = 0x7FFFFFFF;
	for( int i = 0 ; i < m_Ids.size(); ++i )
	{
		Min = std::min<int>( Min, (int)Points().m_Points[i].size() );
	}
	if( m_bMap )
	Min = std::min( Min, (DWORD)Points().m_MapPoints.size() );
	return Min;
}*/

bool	CVVMapEditorDoc::HasMapPoints() const
{
	return  !Points().m_MapPoints.empty();
}

void	CVVMapEditorDoc::DeleteMapPoints() 
{

		Points().m_MapPoints.clear();
}
/*
void	CVVMapEditorDoc::ScaleMap(float Scale)
{
	for( int i = 0; i < m_MapPolygons.size(); i++)
	{
		for( int j = 0 ;  j < m_MapPolygons[i].size(); ++j )
		{
			m_MapPolygons[i][j].X *= Scale;
			m_MapPolygons[i][j].Y *= Scale;
		}
	}
}*/

void	CVVMapEditorDoc::DeleteGroup( int Group )
{
	m_UndoPoints.Backup(  );
	SetModifiedFlag();

	for( int i = 0; i < Points().m_Points.size(); i++)
	{
		DeletePoint(Group, i, false);
	}

	if(  m_bMap /* Points().m_MapPoints.size() > Group */)
	{
		DeletePoint(Group, -1, false);
	}

	UpdateAllViews(0);
}

void	CVVMapEditorDoc::DeletePoint( int Group, int Master, bool Backup )
{
	if(Backup) m_UndoPoints.Backup(  );
	SetModifiedFlag();

	if( Master == -1 )
	{
		PointArr_t::iterator itr =  Points().m_MapPoints.begin() + Group;
		itr->first = false;
	}
	else
	{
//		int z = m_Ids[Master];
		PointArr_t::iterator itr =  Points().m_Points[ Master ].begin() + Group;
		itr->first = false;

		int zz = Points().m_Points[ Master ].size();
	}

	UpdateAllViews(0);
}

void	CVVMapEditorDoc::SetMastersID( const std::vector<int>& Ids )
{
	VVError res(m_VVMapTrans) ;
	res = m_VVMapTrans->SetNumOfMasters(Ids.size());
	for( int ind = 0 ; ind < Ids.size() ; ++ind )
	{	
		int z = Ids[ind];
		res = m_VVMapTrans->SetMasterID(ind, z);
	}	
}

struct FindLastTrue
{
	bool operator () ( const std::pair< bool, PointF >& p ) const
	{
		return !p.first;
	}
};

int		CVVMapEditorDoc::GetMaxGroup() const 
{
	int MaxGroupNumber = 0;
	for( int i = 0; i < Points().m_Points.size(); ++i )
	{
		const PointArr_t&  pa =  Points().m_Points[i];
		int nLastTrue = 0;
		for( int j = 0; j < pa.size(); ++j )
		{
			if ( pa[j].first ) nLastTrue = j + 1; 
		}
/*
		PointArr_t::const_reverse_iterator itr = std::find_if( pa.rbegin(), pa.rend(), FindLastTrue() );
		int z = 0;
		if( itr != pa.rend())
			z = itr - pa.rbegin() + pa.size();*/
		MaxGroupNumber = std::max<int>( MaxGroupNumber, nLastTrue );
	}
	if( m_bMap )
	{
		const PointArr_t&  pa =  Points().m_MapPoints;
		int nLastTrue = 0;
		for( int j = 0; j < pa.size(); ++j )
		{
			if ( pa[j].first ) nLastTrue = j + 1; 
		}
		MaxGroupNumber = std::max<int>( MaxGroupNumber, nLastTrue );
	}

	return MaxGroupNumber;
}

std::set<int>	CVVMapEditorDoc::GetAllValidGroups() const
{
	std::set<int> AllGroups;
	for( int i = 0; i < Points().m_Points.size(); ++i )
	{
		const PointArr_t&  pa =  Points().m_Points[i];
//		int nLastTrue = 0;
		for( int j = 0; j < pa.size(); ++j )
		{
			if ( pa[j].first ) 
			{
				AllGroups.insert( j );
			}
		}
	}
	if( IsMapInCurrent() )
	{
		const PointArr_t&  pa =  Points().m_MapPoints;
		for( int j = 0; j < pa.size(); ++j )
		{
			if ( pa[j].first ) AllGroups.insert( j ); 
		}
	}

	return AllGroups;
}

void	CVVMapEditorDoc::FillGroupCameraMenu(CMenu& Menu, int nIndex) const
{
	CString str;
	str.LoadString( IDS_MENU_NEW );
	Menu.AppendMenu( MF_STRING, ID_MENU_GROUP,  str );

	const PointArr_t& PtArr = Points().m_Points[nIndex];

	int MaxGroupNumber = GetMaxGroup();
	for ( int i=0; i<MaxGroupNumber; ++i )
	{
		if( (i < PtArr.size( ) && !PtArr[i].first)  ||
			 i >= PtArr.size( ))
		{
			CString str;
			str.Format( IDS_FMT_GROUP, i );
			Menu.AppendMenu( MF_STRING, ID_MENU_GROUP + 1 + i,  str );
		}
	}

}

void	CVVMapEditorDoc::FillGroupMapMenu(CMenu& Menu) const
{

	const PointArr_t& PtArr = Points().m_MapPoints;

	CString str;
	str.LoadString( IDS_MENU_NEW );
	Menu.AppendMenu( MF_STRING, ID_MENU_GROUP ,  str );

	int MaxGroupNumber = GetMaxGroup();
	for ( int i=0; i<MaxGroupNumber; ++i )
	{
		if( (i < PtArr.size( ) && !PtArr[i].first)  ||
			 i >= PtArr.size( ))
		{
			CString str;
			str.Format( IDS_FMT_GROUP, i);
			Menu.AppendMenu( MF_STRING, ID_MENU_GROUP + i + 1,  str );
		}
	}

}

void	CVVMapEditorDoc::AddPointToGroup( int nGroup, int nIndex, PointF point )
{
	//FIXME: rewrite on named constants
	bool bNewGroup = (nGroup == -1);
	bool bInMap = (nIndex == -1);


	Backup();

	if(! bInMap )
	{
		if( bNewGroup )
		{
			int nMaxGroup = GetMaxGroup();			
			ResizeGroup(nMaxGroup + 1);
			Points().m_Points[nIndex][nMaxGroup] = std::make_pair(true, point);
		}
		else
		{
			Points().m_Points[nIndex][nGroup] = std::make_pair( true, point );
		}
	}
	else
	{
		if( bNewGroup )
		{
			int nMaxGroup = GetMaxGroup();
			ResizeGroup(nMaxGroup + 1);
			Points().m_MapPoints[nMaxGroup] = std::make_pair(true, point);
		}
		else
		{
			Points().m_MapPoints[nGroup] = std::make_pair( true, point );
		}
	}
	
			
	UpdateAllViews(0);
	GetMainWnd()->GetPointTree()->Refresh();
	
}

void CVVMapEditorDoc::ResizeGroup( int nGroups )
{
	for( int i = 0; i < m_Ids.size(); ++i )
	{
		Points().m_Points[i].resize( nGroups , std::make_pair( false, PointF( -1.0f, -1.0f ) ) );
	}
	if(m_bMap)	Points().m_MapPoints.resize(nGroups , std::make_pair( false, PointF( -1.0f, -1.0f ) )  );
}

void	CVVMapEditorDoc::DumpPoints( const long* Pts, int Number ) const
{
#ifdef _DEBUG
	for( int i=0; i< Number; i += 2)
	{
		CString str;
		str.Format(_T("Point #%d = (%d,%d)\n"), i/2, Pts[i], Pts[i+1] );
		OutputDebugString( str );
	}
#endif
}

void	CVVMapEditorDoc::DumpPolygon( int from, int to, const POINT* Pts, int Number ) const
{
#ifdef _DEBUG
	CString str;
	str.Format(_T("Polygon %d->%d\n"), from, to );
	OutputDebugString( str );
	for( int i=0; i< Number; i ++)
	{
		str.Format(_T("\tPoint #%d = (%d,%d)"), i, Pts[i].x, Pts[i].y );
		OutputDebugString( str );
	}
	OutputDebugString( _T("\n") );
#endif	
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void	CVVMapEditorDoc::DrawGetPoint()
{
	std::map<int, CCameraView*> Views = GetCameraViews();
	std::map<int, CCameraView*>::iterator itr = Views.begin();
	for(; itr != Views.end(); ++itr)
	{
		itr->second->DrawGetPoint();
	}
	CVVMapEditorView* pView = GetMapView();
	pView->DrawGetPoint();

	CAllMapView* pViewMap = GetAllMapView();
	pViewMap->DrawGetPoint();
}

void	CVVMapEditorDoc::DrawHeight()
{
	std::map<int, CCameraView*> Views = GetCameraViews();
	std::map<int, CCameraView*>::iterator itr = Views.begin();
	for(; itr != Views.end(); ++itr)
	{
		itr->second->DrawHeight();
	}
	std::map<int, CExtentView*> ExtViews = GetExtentViews();
	std::map<int, CExtentView*>::iterator itrEx = ExtViews.begin();
	for(; itrEx != ExtViews.end(); ++itrEx)
	{
		itrEx->second->DrawHeight();
	}
//	CVVMapEditorView* pView = GetMapView();
//	pView->DrawGetPoint();
}


//FIXME: HACK on singleton, bring to CVVMapEditorDoc
bool	bInvertSelectPoint = false;
bool	bInvertAnchorPoint = false;
bool	bInvertRefPoint		= false;

void	CVVMapEditorDoc::DrawSelectPoint(int GroupNumber)
{
	std::map<int, CCameraView*> Views = GetCameraViews();
	std::map<int, CCameraView*>::iterator itr = Views.begin();
	bInvertSelectPoint = !bInvertSelectPoint;
	for(; itr != Views.end(); ++itr)
	{
		if(bInvertSelectPoint)
		{
			itr->second->InvalidateSelectPoint();
		}
		else
		{
			itr->second->DrawSelectPoint(GroupNumber);
		}
	}
	GetMainWnd()->GetPointTree()->InvertGroup( GroupNumber );

	CVVMapEditorView* pView = GetMapView();
	if(bInvertSelectPoint)
	{
		pView->InvalidateSelectPoint();
	}
	else
	{
		pView->DrawSelectPoint(GroupNumber);
	}
}

void	CVVMapEditorDoc::DrawAnchorPoint(int nPoint)
{
	bInvertAnchorPoint = !bInvertAnchorPoint;
	CVVMapEditorView* pView = GetMapView();
	if(bInvertAnchorPoint)
	{
		pView->InvalidateAnchorPoint();
		GetAnchorView()->InvalidateSelectItem( );
	}
	else
	{
		pView->DrawAnchorPoint(nPoint);
		GetAnchorView()->SelectItem( nPoint );
	}
}

void	CVVMapEditorDoc::DrawRefPoint(int nPoint)
{
//	bInvertSelectPoint = !bInvertSelectPoint;
}

//-----------------------------------------------------------------------------
// Purpose: invalidate functions
//-----------------------------------------------------------------------------
void	CVVMapEditorDoc::InvalidateHeight()
{
	std::map<int, CCameraView*> Views = GetCameraViews();
	std::map<int, CCameraView*>::iterator itr = Views.begin();
	for(; itr != Views.end(); ++itr)
	{
		itr->second->InvalidateHeight();
	}
	std::map<int, CExtentView*> ExtViews = GetExtentViews();
	std::map<int, CExtentView*>::iterator itrEx = ExtViews.begin();
	for(; itrEx != ExtViews.end(); ++itrEx)
	{
		itrEx->second->InvalidateHeight();
	}
//	CVVMapEditorView* pView = GetMapView();
//	pView->DrawGetPoint();
}


void	CVVMapEditorDoc::InvalidateSelectPoint()
{
	std::map<int, CCameraView*> Views = GetCameraViews();
	std::map<int, CCameraView*>::iterator itr = Views.begin();
	for(; itr != Views.end(); ++itr)
	{
		itr->second->InvalidateSelectPoint();
	}
	bInvertSelectPoint = false;
	CVVMapEditorView* pView = GetMapView();
	pView->InvalidateSelectPoint();	

	GetMainWnd()->GetPointTree()->InvalidateInvertGroup(  );
}

void	CVVMapEditorDoc::InvalidateAnchorPoint()
{
	bInvertAnchorPoint = false;
	CVVMapEditorView* pView = GetMapView();
	pView->InvalidateAnchorPoint();	
	GetAnchorView()->InvalidateSelectItem();
}

void	CVVMapEditorDoc::InvalidateRefPoint()
{
	bInvertRefPoint = false;
	CVVMapEditorView* pView = GetMapView();
	pView->InvalidateRefPoint();	
}
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void	CVVMapEditorDoc::CreateQualityWnd()
{
	std::vector< MergeQuality > Arr;
	Arr.reserve(10);
	for(int i = 0; i < m_Ids.size(); ++i)
	{
		for(int j = 0;  j < m_Ids.size(); ++j)
		{
			if( i != j )
			{
				int		From = m_Ids[i];
				int		To = m_Ids[j];
				long	Quality;
				long err = m_VVMapTrans->GetTransQuality( i, j, &Quality );
				if( !err )
				{
					Arr.push_back( MergeQuality(From, To, Quality) );
				}
			}
		}
	}

	if(m_bMap)
	{
		for(int i = 0;  i < m_Ids.size(); ++i)
		{

				int		From = m_Ids[i];
				int		To = m_Ids.size(); // Map
				long	Quality;
				long err = m_VVMapTrans->GetTransQuality( i, To, &Quality );
				if( !err )
				{
					Arr.push_back( MergeQuality(From, -1, Quality) );
				}
		}
	}

	GetMainWnd()->GetQualityBar()->Refresh(Arr);
}


void CVVMapEditorDoc::OnCloseDocument() 
{
	// TODO: Add your specialized code here and/or call the base class
	//TODO: Need purge all controls
	if(GetMainWnd())
	{
		GetMainWnd()->ResetColors();
	}
	CDocument::OnCloseDocument();
}

int	CVVMapEditorDoc::AddIZPolygon( const Polygon_t& Polygon )
{
	int nIndex = GetIZMaster();
	std::vector<CPoint>	ArrPoints;
	for( int i=0 ; i< Polygon.size(); ++i)
	{
		ArrPoints.push_back( CPoint( int(Polygon[i].X), int(Polygon[i].Y) ) );
	}
	Backup();
	IZPolygons().push_back( InvZone_t(Polygon, /*Number,*/ InvZone_t::ERROR_NUMBER, nIndex ));

	SaveIZ();
	//NOTENOTE: last IZ
	return IZPolygons().size()-1;
}


//-----------------------------------------------------------------------------
// Purpose: anchor point stuff
// Input  : x,y,z - coords
//			Relative - absolute or relative point?
//-----------------------------------------------------------------------------
void	CVVMapEditorDoc::AddAnchorPoint(double x, double y, double z, int Relative)
{
	Backup();
	AnchorPoints().push_back( AnchorPoint_t( x, y, z, Relative ) );
	SaveAnchorPoints();
	SetModifiedFlag();
	UpdateAllViews(0);
}

//-----------------------------------------------------------------------------
// Purpose: Remove from array of AP
// Input  : Number - index in array
//-----------------------------------------------------------------------------
/*
template<class T> struct RemoveIfInSet
{
	const std::set<T>&	m_Set;
	RemoveIfInSet( const std::set<T>& Set ) : m_Set(Set){}
	RemoveIfInSet( const RemoveIfInSet<T>& rhs ) : m_Set(rhs.m_Set){}
	bool operator ()(const Anchor& Value) const
	{
		return m_Set.find(VALUE) != m_Set.end();
	};
};*/

void	CVVMapEditorDoc::RemoveAnchorPoint( int Number )
{
	Backup();
	std::set<int> RelativePoints;
	AnchorPointMap_t& apm = AnchorPoints();

	RelativePoints.insert( Number );
	while ( FillRelativeAnchorPoints(RelativePoints) ) {};
	std::set<int>::const_iterator itr;
	for(int i=0; i < apm.size(); ++i)
	{
		if( apm[i].m_Relative != AnchorPoint_t::NO_DEPENDENCE &&
			RelativePoints.find( i ) == RelativePoints.end()
			)
		{
		//	std::set<int>::const_iterator itr2;
			int& Rel=  apm[i].m_Relative;
			itr= RelativePoints.upper_bound(Rel );
			int nDist = std::distance< std::set<int>::const_iterator >  ( RelativePoints.begin(), itr);
			apm[i].m_Relative -= nDist;
		}
	}

	RemoveDependentRefPoints(RelativePoints);


	itr = RelativePoints.begin();

	for( int ind = 0; itr != RelativePoints.end(); ++itr, ++ind )
	{
		//NOTENOTE: vector reduce its size by one on each cycle
		int Number  = *itr - ind;
		apm.erase( apm.begin() + Number );
	}

	SaveAnchorPoints();
	SaveRefPoints();
	GetMapScaleView()->UpdateTree();
	SetModifiedFlag();
	UpdateAllViews(0);
}

//-----------------------------------------------------------------------------
// Purpose: Load IZ from interface, called from Load
// Return:	Number of loaded IZ
//-----------------------------------------------------------------------------
int		CVVMapEditorDoc::LoadIZ()
{
	
	long IZNumber;
	long err;
	long MasterID;
	long PointNumber;
	POINT*	IZPoints;
	err = m_VVMapTrans->IZ_GetNumOfZones(&IZNumber);
	ASSERT(!err);
	
	IZPolygons().clear();
	IZPolygons().resize(IZNumber);
	for(int i=0; i < IZNumber; ++i)
	{
		err = m_VVMapTrans->IZ_Get( i, &MasterID, &PointNumber, &IZPoints );
		ASSERT(!err);
		IZPolygons()[i].m_Polygon.resize(PointNumber);
		IZPolygons()[i].m_nMasterNumber = MasterID;
		for(int j=0; j < PointNumber; ++j)
		{
			IZPolygons()[i].m_Polygon[j] = PointF( IZPoints[j].x, IZPoints[j].y );	
		}
	}
//NOTENOTE: invalidate active zone
	SetActiveIZ(-1);
	return IZNumber;
}

//-----------------------------------------------------------------------------
// Purpose: Save IZ to interface, called from Save& Undo& Redo
//-----------------------------------------------------------------------------
void	CVVMapEditorDoc::SaveIZ() const
{
	std::vector<long>	MasterIDs;
	std::vector<long>	PointNumbers;
	std::vector<CPoint>	IZPoints;
	long err;

	const InvZoneMap_t& izp = IZPolygons();
	for(int i = 0; i < izp.size(); i++)
	{
		MasterIDs.push_back( izp[i].m_nMasterNumber );
		PointNumbers.push_back( izp[i].m_Polygon.size() );
		for(int j=0; j < izp[i].m_Polygon.size(); ++j)
		{
			const PointF& ptf = izp[i].m_Polygon[j];
			IZPoints.push_back( CPoint(ptf.X, ptf.Y ) );
		}
	}
	err = m_VVMapTrans->IZ_AddList( izp.size(), &MasterIDs[0], &PointNumbers[0], &IZPoints[0] );
	ASSERT(!err);
#ifdef _DEBUG
//	for( int i=0; i< Number; i += 2)
	{
		CString str;
		str.Format(_T("IZ #%d\n"), izp.size() );
		OutputDebugString( str );
	}
#endif
}

//-----------------------------------------------------------------------------
// Purpose: Load anchor point with VVMinterface stuff
// Return : Number of loaded points
//-----------------------------------------------------------------------------
int		CVVMapEditorDoc::LoadAnchorPoints()
{
	AnchorPointMap_t&	ap = AnchorPoints();
	long err; long PointNumber;
	err = m_VVMapTrans->SP_GetNumOfPoints(&PointNumber);
	ASSERT(!err);
	AnchorPoints().clear();
	AnchorPoints().resize(PointNumber);
	double x,y,z;
	long lCalc;
	for(int i=0; i<PointNumber; ++i)
	{
		AnchorPoint_t& ap = AnchorPoints()[i];
		m_VVMapTrans->SP_GetAbsolute(i,&x, &y, &z, &lCalc);
		if( lCalc < 0 )
		{
			ap.m_x = x;
			ap.m_y = y;
			ap.m_z = z;
		}
		else
		{
			long Anchor_Point = lCalc;
			m_VVMapTrans->SP_GetRelative( i, Anchor_Point, &x, &y, &z );
			ap.m_x = x;
			ap.m_y = y;
			ap.m_z = z;
			ap.m_Relative = Anchor_Point;
		}
	}
	return PointNumber;
}

void	CVVMapEditorDoc::SaveAnchorPoints() const
{
	const AnchorPointMap_t&	ap = AnchorPoints();
	const AnchorPoint_t& z =  ap[0];
	//NOTENOTE: raw array of x,y,z
	std::vector<double>		Coord (ap.size() * 3);
	//NOTENOTE: raw array of dependences
	std::vector<long>		Calc(ap.size(), -1);
	//NOTENOTE: raw array of anchor map indices
	std::vector<long>		MapCoords;
	//NOTENOTE: raw array of map coords
	std::vector<long>		AnchorOnMap;
	for(int i =0; i <ap.size(); ++i)
	{
		double 
		x = ap[i].m_x,
		y = ap[i].m_y,
		z = ap[i].m_z;
		Coord[3 * i + 0] = x;
		Coord[3 * i + 1] = y;
		Coord[3 * i + 2] = z;
		Calc[i] = ap[i].m_Relative;

		if( ap[i].IsOnMap() )
		{
			std::pair<long, long> pt = ap[i].GetMapCoord();
			AnchorOnMap.push_back( i );
			MapCoords.push_back( pt.first );
			MapCoords.push_back( pt.second );
		}
	}
	long err;
	err = m_VVMapTrans->SP_AddList( ap.size(), &Coord[0], &Calc[0] );
	err = m_VVMapTrans->AlignMap( AnchorOnMap.size(), &AnchorOnMap[0], &MapCoords[0] );
}

void	CVVMapEditorDoc::LoadMapOrigin()
{
	long err, x, y;
	err = m_VVMapTrans->GetMapOrigin(&x, &y);
	//NOTENOTE: there was a bug in map origin loading code, currently we may comment out assert
//	ASSERT(!err);
	if( !err )
		SetMapOrigin( PointF(x ,y ) );
}

void	CVVMapEditorDoc::SaveMapOrigin() const
{
	if( IsSetMapOrigin() )
	{
		long err;
		PointF ptf = GetMapOrigin();
		err = m_VVMapTrans->SetMapOrigin( ptf.X, ptf.Y);
		ASSERT(!err);
	}
}

void	CVVMapEditorDoc::LoadMapScale()
{
	long err;
	double x, y;
	err = m_VVMapTrans->GetMapScale(&x, &y);
	ASSERT(!err);
	SetScaleX( x );
	SetScaleY( y );
}

void	CVVMapEditorDoc::SaveMapScale() const
{
	if( GetScaleX().first	&&
		GetScaleY().second	)
	{
		long err;
		err = m_VVMapTrans->SetMapScale( GetScaleX().second , GetScaleY().second );
		ASSERT(!err);
	}
}

int		CVVMapEditorDoc::FindActiveIZ(PointF point) const 
{
	const InvZoneMap_t & zones = IZPolygons();
	//FIXME: Hack, must be VVMapEditorView::EditorState::ERR_POLY but not -1 on return !!!
	for(int i=0; i < zones.size(); ++i)
	{
		const Polygon_t& poly = zones[i].m_Polygon;
		std::vector<CPoint>	PolyPoints(poly.size());
		for(int j=0; j<poly.size(); ++j)
		{
			PolyPoints [j] = CPoint( poly[j].X , poly[j].Y) ;
		}
		CRgn rgn;
		rgn.CreatePolygonRgn( &PolyPoints[0], PolyPoints.size(), ALTERNATE );
		BOOL res = rgn.PtInRegion( CPoint(point.X, point.Y) );
		if(res) return i;
	}
	return -1;
}

void	CVVMapEditorDoc::RemoveIZPolygon(int Number)
{
	//FIXME: to enum or const, same as prior
	if( Number != -1 )
	{
		Backup();
		IZPolygons().erase( IZPolygons().begin() + Number );
		SaveIZ();
	}
}

void	CVVMapEditorDoc::ChangeAnchorPoint( int Number, const AnchorPoint_t& Anchor )
{
	ASSERT( AnchorPoints().size() > Number );
	AnchorPoint_t& Current = AnchorPoints()[Number];
	//NOTENOTE: we can override operator == instead
	if( Current.m_x != Anchor.m_x || 
		Current.m_y != Anchor.m_y ||
		Current.m_z != Anchor.m_z ||
		Current.m_Relative != Anchor.m_Relative)
	{
		Backup();
		//NOTENOTE: Current point is into anchor buffer, so get new reference from top
		AnchorPoint_t& CurrentAnchor = AnchorPoints()[Number];
		CurrentAnchor = Anchor;
		SaveAnchorPoints();
		SetModifiedFlag();
		UpdateAllViews(0);
	}
}

std::pair<bool, PointF>	CVVMapEditorDoc::GetAnchorPointOnMap( int nPoint) const
{
	ASSERT( AnchorPoints().size() > nPoint );

	double x,y,z ;long lCalc;
	m_VVMapTrans->SP_GetAbsolute( nPoint, &x, &y, &z, &lCalc );
	//NOTENOTE: old code
#if 0	
	return ScaleMapPoint( PointF(x, y) );
#endif
	const AnchorPoint_t& ap = AnchorPoints()[nPoint];
	long err, MapX, MapY;
	err = m_VVMapTrans->GetMapCoords( x, y, z, &MapX, &MapY );
	if( err == 0)
	{
		return std::make_pair( true, PointF( MapX, MapY ) );
	}
	else if( ap.IsOnMap() )
	{
		std::pair<long, long> p = ap.GetMapCoord();
		return std::make_pair( true, PointF( p.first, p.second ) );
	}
	return std::make_pair( false, PointF( -1, -1) );
}

void	CVVMapEditorDoc::UndoStackResize()
{
	m_UndoExtents.Resize();
	m_UndoPoints.Resize();
	m_UndoIZPolygons.Resize();
//	m_UndoAnchorPoints.Resize();
//	m_UndoRefPoints.Resize();
	m_UndoAncRefPoints.Resize();
}

void	CVVMapEditorDoc::UndoStackErase()
{
	m_UndoExtents.Erase();
	m_UndoPoints.Erase();
	m_UndoIZPolygons.Erase();
//	m_UndoAnchorPoints.Erase();
//	m_UndoRefPoints.Erase();
	m_UndoAncRefPoints.Resize();
}

//-----------------------------------------------------------------------------
// Purpose: Refernce points
//-----------------------------------------------------------------------------
bool	CVVMapEditorDoc::AddRefPoint()
{
	Backup();
	RefPoints().push_back( RefPoint_t()  );
	bool res = SaveRefPoints();
	UpdateAllViews(0);
	return res;
}

bool	CVVMapEditorDoc::AddRefPointDistance(int nRefPoint,int Number, double fDist)
{
	ASSERT( RefPoints().size() > nRefPoint );
	Backup();
	RefPoint_t& rp = RefPoints()[nRefPoint];
	rp.push_back( std::make_pair( Number, fDist ) );
	bool res = SaveRefPoints();
	UpdateAllViews(0);
	return res;
}

bool	CVVMapEditorDoc::RemoveRefPoint(int nRefPoint)
{
	ASSERT( RefPoints().size() > nRefPoint );
	Backup();
	RefPoints().erase( RefPoints().begin() + nRefPoint );
	bool res = SaveRefPoints();
	UpdateAllViews(0);
	return res;
}

bool	CVVMapEditorDoc::RemoveRefPointDistance(int nRefPoint, int Number)
{
	ASSERT( RefPoints().size() > nRefPoint );
	Backup();
	RefPoint_t& rp = RefPoints()[nRefPoint];
	rp.erase( rp.begin() + Number );
	UpdateAllViews(0);
	bool res = SaveRefPoints();
	UpdateAllViews(0);
	return res;
}


bool	CVVMapEditorDoc::ChangeRefPointDistance(int nRefPoint, int Number, double fDistance)
{
	ASSERT( RefPoints().size() > nRefPoint );
	Backup();
	SetModifiedFlag();

	RefPoint_t& rp = RefPoints()[nRefPoint];
	rp[Number].second = fDistance ;
	bool res = SaveRefPoints();
	UpdateAllViews(0);
	return res;
}

bool	CVVMapEditorDoc::ChangeRefPointPoint(int nRefPoint, int Number, int nPoint)
{
	ASSERT( RefPoints().size() > nRefPoint );
	Backup();
	SetModifiedFlag();

	RefPoint_t& rp = RefPoints()[nRefPoint];
	rp[Number].first = nPoint ;
	bool res = SaveRefPoints();
	UpdateAllViews(0);	
	return res;
}

bool	CVVMapEditorDoc::ChangeRefPoint( int nRefPoint, int Number, int nPoint, double fDistance )
{
	ASSERT( RefPoints().size() > nRefPoint );
	Backup();
	SetModifiedFlag();

	RefPoint_t& rp = RefPoints()[nRefPoint];
	rp[Number] = std::make_pair( nPoint, fDistance) ;
	bool res = SaveRefPoints();
	UpdateAllViews(0);	
	return res;
}

int	CVVMapEditorDoc::LoadRefPoints()
{
	long err, PointNumber;
	long * Anchors; double* Distances;

	err = m_VVMapTrans->RP_GetNumOfPoints( &PointNumber );
	RefPoints().resize( PointNumber );
	for(int i = 0 ; i < PointNumber; ++i)
	{
		long Number;
		err = m_VVMapTrans->RP_Get( i, &Number, &Anchors, &Distances );
		RefPoint_t& rp = RefPoints()[i];
		for(int j=0; j< Number; ++j)
		{
			rp.push_back( std::make_pair( Anchors[j], Distances[j] ) );
		}
	}

//	GetMapScaleView()->UpdateTree();
	return PointNumber;
}

bool	CVVMapEditorDoc::SaveRefPoints() const
{
	const RefPointMap_t& rpm = RefPoints();
	std::vector<long>	PointNumber;
	std::vector<long>	Anchors;
	std::vector<double>	Distances;
	for( int i=0; i < rpm.size(); ++i)
	{
		const RefPoint_t &rp = rpm[i];
		PointNumber.push_back( rp.size() );
		for(int j=0 ; j < rp.size(); ++j)
		{
			const std::pair<int, double>& pr = rp[j];
			Anchors.push_back( pr.first );
			Distances.push_back( pr.second );
		}
	}
	long err;
	err = m_VVMapTrans->RP_AddList( rpm.size(), &PointNumber[0], &Anchors[0], &Distances[0] );
//	ASSERT( !err );
	return !err;
}

void	CVVMapEditorDoc::SetScaleX(double x)	
{ 
	m_ScaleX = std::make_pair( true, x); 
	SaveMapScale();
	SetModifiedFlag();
}

void	CVVMapEditorDoc::SetScaleY(double y) 
{ 
	m_ScaleY = std::make_pair( true, y); 
	SaveMapScale();
	SetModifiedFlag();
}

PointF	CVVMapEditorDoc::ScaleMapPoint(PointF ptfMap) const
{
	PointF z;
	z.X = ptfMap.X * GetScaleX().second + GetMapOrigin().X;
	z.Y = ptfMap.Y * GetScaleY().second + GetMapOrigin().Y;
	return z;
}

//-----------------------------------------------------------------------------
// Purpose: is data enough for point rendering
// Input  : nRefNumber - Number of refernce point 
// Output : view can render point
//-----------------------------------------------------------------------------
bool	CVVMapEditorDoc::IsValidRefPoint ( int nRefNumber) const
{
	long x,y;
	return m_VVMapTrans->RP_GetMapCoords( nRefNumber, &x, &y ) == 0;
}

//-----------------------------------------------------------------------------
// Purpose: determine which point should delete from deleted set
// Input  : Anchors - set of deleted point
//-----------------------------------------------------------------------------
bool	CVVMapEditorDoc::FillRelativeAnchorPoints(std::set<int>& Anchors)
{
	const AnchorPointMap_t& apm = AnchorPoints();
	bool bAddPoint = false;
	for(int i=0; i < apm.size(); ++i)
	{
		int nRelative = apm[i].m_Relative;
		if( nRelative != AnchorPoint_t::NO_DEPENDENCE )
		{
			std::set<int>::iterator	itrDep	= Anchors.find( nRelative ),
									itrSelf = Anchors.find( i );
			if( itrDep != Anchors.end() && itrSelf == Anchors.end() ) 
			{
				bAddPoint = true;
				Anchors.insert( i );
			}
		}
	}
	return bAddPoint;
}

void	CVVMapEditorDoc::RemoveDependentRefPoints( std::set<int>& Anchors )
{
	RefPointMap_t& rpm = RefPoints();
	AnchorPointMap_t& apm = AnchorPoints();
/*
//NOTENOTE: may use this algoritm in RemoveAnchorPoint
1)	get deleted points
2)	get image of anchor state after deleting
	example : 0 1 2 3 4 5 -> delete 1 & 3 -> 0 -1 1 -1 2 3
3)	iterate & change index
*/
	std::vector<int>	AnchorIndex;
	const int c_nWrongAnchor = -1;

	for(int i= 0, nDeleted = 0; i < apm.size() ; ++i)
	{
		bool bDelete = ( Anchors.find( i ) != Anchors.end() );
		if(bDelete)
		{
			AnchorIndex.push_back( c_nWrongAnchor );
			++nDeleted;
		}
		else
		{
			AnchorIndex.push_back( i - nDeleted );
		}
	}

	//NOTENOTE: should use remove_if/erase paradigm
	for(  i=0; i < rpm.size(); ++i )
	{
		RefPoint_t & rp = rpm[i];
		RefPoint_t::iterator itr = rp.begin();
		while( itr != rp.end() )
		{
			int& nPoint = itr->first;
			if( AnchorIndex[ nPoint] == c_nWrongAnchor )
			{
				itr = rp.erase( itr);
			}
			else
			{
				nPoint = AnchorIndex[ nPoint ];
				++itr;
			}
		}
	}
}
/*
template<typename T>  void global_delete(T* x)
{
	::delete x;
}*/

void global_delete (Bitmap*x) { ::delete x;}

std::auto_ptr<CBitmapBackground >	CVVMapEditorDoc::CreateViewBitmap() const
{
	long err;
	std::vector<DWORD>	Colors;
	Colors.reserve( (m_Ids.size()+1 ) * 4);
	for(int i=0 ; i < m_Ids.size(); ++i)
	{
		Color dwColor = GetSettings().GetMasterColor( m_Ids[i] );
		long r = dwColor.GetR();
		long g = dwColor.GetG();
		long b = dwColor.GetB();
		long a = dwColor.GetA();
#ifdef BUG_IN_PROCESS_BITMAP
		Colors.push_back( b );
		Colors.push_back( g );
		Colors.push_back( r );
#else
		Colors.push_back( r );
		Colors.push_back( g );
		Colors.push_back( b );
#endif
		Colors.push_back( /*a */0x80);
	}
	Color dwColor = Color::Black;
		long r = dwColor.GetR();
		long g = dwColor.GetG();
		long b = dwColor.GetB();
		long a = dwColor.GetA();
	Colors.push_back( r );
	Colors.push_back( g );
	Colors.push_back( b );
	Colors.push_back( /*a*/0x0 );


	err = m_VVMapTrans->SetColors( (long*)&Colors[0] );
	err = m_VVMapTrans->ProcessMapBitmap(0);
	BITMAPINFO* pBmpInfo;
	BYTE*		pBits;
	err = m_VVMapTrans->GetProcessedBitmap( &pBmpInfo, &pBits);
	if(err || !pBmpInfo || !pBits ) return std::auto_ptr<CBitmapBackground > (0);

	BITMAPINFO	bi;
/*	memset(&bi, 0, sizeof(BITMAPINFO));
	const BITMAPINFOHEADER& bih = pBmpInfo->bmiHeader;
    bi.bmiHeader.biSize			= sizeof( BITMAPINFOHEADER );
    bi.bmiHeader.biWidth		= bih.biWidth;
    bi.bmiHeader.biHeight		= bih.biHeight;
    bi.bmiHeader.biPlanes		= 1;
    bi.bmiHeader.biBitCount		= bih.biBitCount;
	bi.bmiHeader.biCompression	= BI_RGB;
	bi.bmiHeader.biSizeImage	= bih.biSizeImage;*/
	memcpy(&bi, pBmpInfo, sizeof(BITMAPINFO));
	try
	{
		return std::auto_ptr<CBitmapBackground > (  new CBitmapBackground (&bi, pBits) );
	}
	catch( ImageFileException )
	{
		return std::auto_ptr<CBitmapBackground > (  );
	}
}

void CVVMapEditorDoc::ResetInterfaceSettings()
{
	m_VVMapTrans->SetPointList( 0, (long*)1 );
//	m_VVMapTrans->SetExtentList(  )
}

BOOL CVVMapEditorDoc::OnNewDocument() 
{
	// TODO: Add your specialized code here and/or call the base class

	return CDocument::OnNewDocument();
}

void	CVVMapEditorDoc::OnOptionChange()
{
	//NOTENOTE: when option dialog closed redraw all windows
	UpdateAllViews(0);
}

void	CVVMapEditorDoc::SetAnchorPointOnMap( int Number, long x, long y )
{
	Backup();
	AnchorPointMap_t&	ap = AnchorPoints();
	ASSERT( ap.size() > Number );
	ap[ Number ].SetMapCoord( x, y );
	SaveAnchorPoints();
	GetAnchorView()->UpdateList();
	UpdateAllViews(0);
}

void	CVVMapEditorDoc::RemoveAnchorPointFromMap( int Number)
{
	Backup();
	AnchorPointMap_t&	ap = AnchorPoints();
	ASSERT( ap.size() > Number );
	ap[ Number ].RemoveFromMap( );
	SaveAnchorPoints();	
	UpdateAllViews(0);
}

std::vector<int>	CVVMapEditorDoc::FindWhereCorrPointExist( int nIndex ) const
{
	const int nMapID = -1;
	std::vector<int> Masters;
	for( int i = 0; i < Points().m_Points.size(); ++i )
	{
		const PointArr_t&  pa =  Points().m_Points[i];
		if ( pa.size() > nIndex && pa[nIndex].first ) 
		{
			Masters .push_back( i );
		}
	}
	if( IsMapInCurrent() )
	{
		const PointArr_t&  pa =  Points().m_MapPoints;
		if ( pa.size() > nIndex &&  pa[nIndex].first ) 
		{
			Masters .push_back( nMapID );
		}
	}
	return Masters;
}

void	CVVMapEditorDoc::LoadMapAlign()
{	
	long lNumber; 
	const long*	pNumber;
	const long*	pCoord;

	AnchorPointMap_t&	apm = AnchorPoints();
	m_VVMapTrans->GetAlignMap( &lNumber, &pNumber, &pCoord );
	for( int i=0 ; i < lNumber; ++i)
	{
		int nIndex = pNumber[i];
		ASSERT( apm.size() > nIndex );
		long	MapX = pCoord[ 2 * i + 0],
				MapY = pCoord[ 2 * i + 1];
		apm[i].SetMapCoord( MapX, MapY );
		
	}
}

///////////////////// Filter API ///////////////////////////////////////

inline int GetMinCount( CSettings::Filter_t Filter )
{
	switch( Filter )
	{
	case CSettings::FILTER_WEAK :	return 1;
	case CSettings::FILTER_STRONG : return 2;
	case CSettings::FILTER_NONE:	return 0;
	default: return 0;
	}
}

std::auto_ptr<CFilterCache>	CVVMapEditorDoc::GetFilterCache()
{
	CSettings::Filter_t Filter = GetSettings().GetFilter();

	std::set<int> ValidGroups;
	std::map<int, CCameraView*> Views = GetCameraViews();
	if( Filter == CSettings::FILTER_NONE )
	{
		 ValidGroups = GetAllValidGroups();
	}
	else
	{
		std::vector<int> AllGroups( GetMaxGroup(), 0 );
		const PointList& pl = Points();
		
		int nCount = 0;
		for( int i = 0; i < pl.m_Points.size(); ++i )
		{
			const PointArr_t& pa = pl.m_Points[i];
			int nNumber = IndexToId( i );
			CCameraView* pView = Views[nNumber];
			CControlBar* pWnd = (CControlBar*)pView->GetParent();
			ASSERT(pWnd);
			for( int j =0; j < pa.size(); ++j )
			{
				if( pWnd->IsVisible() && pa[j].first )
				{
					++AllGroups[j] ;
				}
			}
		}

		for( i = 0; i < AllGroups.size(); ++i)
		{
			if( AllGroups[i] >= GetMinCount(Filter) )
			{
				ValidGroups.insert( i );
			}
		}
	}

	std::set<int> VisibleCams;
	for( int i=0; i < m_Ids.size(); ++i )
	{
		int Number = IndexToId( i );
		CCameraView* pView = Views[Number];
		ASSERT(pView);
		CControlBar* pWnd = (CControlBar*)pView->GetParent();
		ASSERT(pWnd);
		if( Filter == CSettings::FILTER_NONE	|| 
			(Filter != CSettings::FILTER_NONE  && pWnd->IsVisible() ) )
				VisibleCams.insert( i );
	}

	return std::auto_ptr<CFilterCache>( new CFilterCache( this, VisibleCams, ValidGroups  ) );
}

void	CVVMapEditorDoc::OnShowMaster()
{
	UpdateAllViews(0);
}

void	CVVMapEditorDoc::OnCloseMaster()
{
	UpdateAllViews(0);
}

bool CFilterCache::IsGroupValid(int nGroupIndex) const
{
	return m_FilterGroups.find( nGroupIndex ) != m_FilterGroups.end();
}

bool CFilterCache::IsPointValid(int nGroupIndex, int nMasterIndex) const
{
	if( nMasterIndex == c_nMapIndex )
	{
		//NOTENOTE: assume map is always visible
		const PointArr_t& pa = m_pDoc->Points().m_MapPoints;
		ASSERT( nGroupIndex < pa.size() );
		return	pa[nGroupIndex].first		&&
				IsGroupValid(nGroupIndex);
	}
	else
	{
		const PointMap_t& pm = m_pDoc->Points().m_Points;
		ASSERT( pm.size() > nMasterIndex);
		const PointArr_t& pa = m_pDoc->Points().m_Points[nMasterIndex];
		ASSERT( nGroupIndex < pa.size() );
		return	pa[nGroupIndex].first		&&
				IsGroupValid(nGroupIndex)	&&
				(m_FilterMasters.find( nMasterIndex ) != m_FilterMasters.end() );
	}
}

const Polygon_t&	CVVMapEditorDoc::GetHoryzon(int nMasterIndex) const 
{ 
	ASSERT(nMasterIndex < m_Ids.size() );
	return m_Horyzons[nMasterIndex];
};

void	CVVMapEditorDoc::CreateHoryzon()
{
	m_Horyzons.resize( m_Ids.size() );
	long			err;
	const POINT*	pHorPoints;
	int				PointNumber;
	for( int i = 0; i < m_Ids.size(); ++i )
	{
		err = m_VVMapTrans->GetHoriz( i, &pHorPoints, &PointNumber );
		m_Horyzons[i].clear();
		if( err == ERR_NO_ERROR )
		{
			m_Horyzons[i].resize( PointNumber);
			for( int j=0; j < PointNumber; ++j)
			{
				m_Horyzons[i][j] = PointF( pHorPoints[j].x, pHorPoints[j].y );
			}
		}
	}
}

void	CVVMapEditorDoc::PrepareData( int Number)
{
	//TODO: PrepareData !!!
	m_Ids.resize( Number );
	m_Polygons.resize( Number );
	Extents().resize( Number );
	Points().m_Points.resize( Number );
	m_MapPolygons.resize(Number);
	m_ExtentTable.resize(Number);
	m_Horyzons.resize( Number );

	for( int i=0; i < Number; ++i )
	{
		m_Polygons[i].resize( m_Ids.size() );
		m_Horyzons[i].clear();
	}

}

#undef max
#undef min

CRect	CVVMapEditorDoc::GetMapPanoramaRect(CSize sizeMap)
{
	std::map<int ,CCameraView*> Views = GetCameraViews();

//	CSize sizeClient = Views[ IndexToId( nIndex ) ]->GetBackground()->GetSize();
//	CRect	rcArea(CPoint (0,0) , sizeClient);

	CRect	rcArea ( CPoint(0, 0), sizeMap);

	CRect	rcMapArea(	std::numeric_limits<int>::max(), 
						std::numeric_limits<int>::max(), 
						std::numeric_limits<int>::min(),
						std::numeric_limits<int>::min());

	long	xto, yto;

	long res;

	int nIndex = m_Ids.size(); // map
	for( int i=0; i < m_Ids.size(); ++i)
	{
		CRect rcClient;
		Views[ IndexToId(i) ]->GetClientRect(&rcClient);
		CSize sizeClient = rcClient.Size();
		for( long x = 0; x < sizeClient.cx; x++)
			for( long y = 0; y < sizeClient.cy;y++)
			{
				res = m_VVMapTrans->GetXY( i, x, y, nIndex, &xto, &yto );

				if(res) continue;
				if( rcArea.PtInRect( CPoint(xto, yto) )  )
				{
					rcMapArea.left	= std::_cpp_min( rcMapArea.left, xto );
					rcMapArea.right	= std::_cpp_max( rcMapArea.right, xto );
					rcMapArea.top	= std::_cpp_min( rcMapArea.top, yto );
					rcMapArea.bottom= std::_cpp_max( rcMapArea.bottom, yto );
				}
			}
	}
	rcMapArea.InflateRect(1,1,1,1);
/*
		CSize sizeMax ( GetSettings().GetProperty( prop_PanoramaMaxX ) , 
						GetSettings().GetProperty( prop_PanoramaMaxY ) );
		
		CSize sizeWindow(0,0);
		if( rcArea.Width() > sizeMax.cx )
		{
			rcArea.left = ( - sizeMax.cx + sizeWindow.cx)/2;
			rcArea.right = ( sizeMax.cx + sizeWindow.cx)/2;
		}
		if( rcArea.Height() > sizeMax.cy )
		{
			rcArea.top = ( - sizeMax.cy + sizeWindow.cy)/2;
			rcArea.bottom = ( sizeMax.cy + sizeWindow.cy)/2;
		}
	
*/
	return rcMapArea;
}



Bitmap* CVVMapEditorDoc::CreateMapBitmap( )
{
	
	if( !GetMapView()->GetBackground() ) return 0;
	CSize sizeMap = GetMapView()->GetBackground()->GetSize();
	CRect rcProbableArea = GetMapPanoramaRect( sizeMap );
	CRect rcArea (	std::_cpp_max<int>( 0, rcProbableArea.left)  , 
				   	std::_cpp_max<int>( 0, rcProbableArea.top)  ,
					std::_cpp_min<int>( sizeMap.cx, rcProbableArea.right)  ,
					std::_cpp_min<int>( sizeMap.cy, rcProbableArea.bottom)  ); 

	std::map<int ,CCameraView*> Views = GetCameraViews();

//	CSize sizeClient = Views[ IndexToId( nIndex ) ]->GetBackground()->GetSize();


	CPoint ofs = CPoint( -rcArea.left, -rcArea.top );
//	CRect rcClient(CPoint(0,0), sizeClient );

	Bitmap* Bmp = ::new Bitmap( rcArea.Width(), rcArea.Height() );
	Graphics	gr(Bmp);
	gr.Clear( Color::LightGray );

	BitmapData BmpData;
	Bmp->LockBits( &Rect(0, 0, rcArea.Width(), rcArea.Height()), ImageLockModeWrite , PixelFormat32bppARGB, &BmpData );
	
	Array2D<  Color > BmpArr( (Color*)BmpData.Scan0, BmpData.Width, BmpData.Height, BmpData.Stride );

	std::vector< Bitmap* > MasterBmp ( m_Ids.size(), 0  ) ;
	std::vector< BitmapData > MasterBitmapData( m_Ids.size() );
	std::vector< CRect >	 MasterSizes( m_Ids.size() );

	for(int i=0; i < m_Ids.size(); ++i)
	{
	
		int nID = IndexToId(i) ;
		CSize sizeView = Views[ nID ]->GetBackground()->GetSize();
	
		MasterBmp[i] = ::new Bitmap( sizeView.cx, sizeView.cy );
		Graphics gr( MasterBmp[i] );
		Views[ nID ]->GetBackground()->Draw( gr , CRectF(0, 0, sizeView.cx, sizeView.cy) );
		MasterBmp[i]->LockBits( &Rect(0, 0, sizeView.cx, sizeView.cy), ImageLockModeRead , PixelFormat32bppARGB, &MasterBitmapData[i] );
		MasterSizes[i] = CRect( CPoint ( 0, 0) , sizeView ) ;
	}
	///////////////////////////////////////////////////////////
	CPoint ptOffset( -rcArea.left, -rcArea.top); 
	CSize szAll( rcArea.Width(), rcArea.Height() );
//	CRect rcCamera(  MasterSizes[nIndex] );
//	CSize szSource = rcCamera.Size();
//	CRect rcSource( ptOffset, szSource );

	int nIndex = m_Ids.size();
	long err, xto, yto;
	for( int x = 0; x < szAll.cx ; ++x)
	{
		for( int y = 0; y < szAll.cy; ++y)
		{
			//NOTENOTE: if we are not in source rect then try
//			if( !rcSource.PtInRect( CPoint( x, y ) ) )
			for(int i = 0; i < m_Ids.size(); ++i)
			{

					long xfrom = x - ptOffset.x;
					long yfrom = y - ptOffset.y;
					err = m_VVMapTrans->GetXY( nIndex, xfrom, yfrom, i, &xto, &yto );
					if(!err &&  MasterSizes[i].PtInRect( CPoint ( xto, yto) ) )
					{
						Array2D<  Color > ViewArr( (Color*)MasterBitmapData[i].Scan0, 
													MasterBitmapData[i].Width, 
													MasterBitmapData[i].Height, 
													MasterBitmapData[i].Stride );

						BmpArr(x, y) = ViewArr(xto, yto);
						//NOTENOTE: find first master
						break;
					}
			}
		}
	}

	for( i=0; i < m_Ids.size(); ++i)
	{
		MasterBmp[i]->UnlockBits( &MasterBitmapData[i] );
		::delete MasterBmp[i];
	}

	Bmp->UnlockBits(&BmpData);
	return Bmp;
}

void	CVVMapEditorDoc::SetMapCache( const std::vector<BYTE>& Arr)
{
	m_bSetMapSize = true;
	long res = m_VVMapTrans->PutMapBuffer( &Arr[0], Arr.size() );
	//ASSERT( res == 0);
}

void	CVVMapEditorDoc::OnFileSaveDB()
try
{

	VVError err(m_VVMapTrans);
	void* pInfo;
	long nSize;
	Save();
	err =  /*static_cast<CVVMapEditorApp*>(AfxGetApp())->m_bShort ?
		m_VVMapTrans->SaveShort( &pInfo, &nSize ):*/
	m_VVMapTrans->Save( &pInfo, &nSize );
	std::vector<BYTE> Data( (const BYTE*)pInfo, (const BYTE*)pInfo + nSize );

	DBSerializer dbs;
	dbs.SaveFile( Data);
	SetModifiedFlag(FALSE);
}
catch(VVMapTransException& ex)
{
	AfxMessageBox( Helper::Convert(ex.what()), MB_OK|MB_ICONERROR );
}
catch ( DBSaveDataException ) 
{
	AfxMessageBox( IDS_DB_SAVE, MB_OK|MB_ICONERROR );
}
catch( DatabaseException )
{
	AfxMessageBox( IDS_DB_CONNECTION, MB_OK|MB_ICONERROR );
};


void	CVVMapEditorDoc::OnFileOpenDB()
try
{
	BOOL res = SaveModified();
	if( !res ) return;
	DeleteContents();
	SetModifiedFlag( FALSE );

	std::vector<BYTE> v;
	DBSerializer dbs;
	dbs.LoadFile( v );


	VVError err(m_VVMapTrans);
	err = m_VVMapTrans->Load( &v[0], v.size() );

	GetMainWnd()->CloseMergeExtentWindows();
	Load();

//	OnOpenDocument();
	SendInitialUpdate();
	UpdateAllViews(NULL);
}
catch(VVMapTransException& ex)
{
	AfxMessageBox( Helper::Convert( ex.what() ), MB_OK|MB_ICONERROR);
}
catch ( DBNoDataException ) 
{
	AfxMessageBox( IDS_NO_DATA, MB_OK|MB_ICONERROR );
}
catch( DatabaseException )
{
	AfxMessageBox( IDS_DB_CONNECTION, MB_OK|MB_ICONERROR );
}