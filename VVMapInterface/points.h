#ifndef _POINTS_H_INCLUDED__
#define _POINTS_H_INCLUDED__


#include <vector>
#include "serializer3.h"
#include "BufferInput.h"

#include "kpoint.h"

//namespace points
//{
	/*
	struct Vect2i
	{
		int x; 		int y;		bool changed;

		Vect2i():x(-1), y(-1), changed(true) {}
		Vect2i( int _x, int _y) : x(_x), y(_y), changed(true) {}

		void set( int _x, int _y) 
		{ 
			changed = !((x == _x) && (y == _y)); x = _x; y = _y;
		}

		void ser(ser::CElement e)
		{
			e.attr(_T("x"), x);
			e.attr(_T("y"), y);
		}

		static const _TCHAR * tag_name() { return _T("point2i");}

	};

	struct Vect2Dd
	{
		double x;	double y;		bool changed;
		Vect2Dd():x(0), y(0), changed(true) {}
		Vect2Dd( double _x, double _y) : x(_x), y(_y), changed(true) {}

		void ser(ser::CElement e)
		{
			e.attr(_T("x"), x);
			e.attr(_T("y"), y);
		}

		static const _TCHAR * tag_name() { return _T("point2d");}
	};

	struct Vect3Di
	{
		int x; 	int y;	int z;
		bool changed;
		Vect3Di():x(0), y(0), z(0), changed(true) {}
		Vect3Di( int _x, int _y, int _z) : x(_x), y(_y), z(_z), changed(true) {}
		Vect3Di( int _x, int _y) : x(_x), y(_y), z(0), changed(true) {}

		void set( int _x, int _y, int _z) 
		{ 
			changed = !((x == _x) && (y == _y) && (z == _z)); x = _x; y = _y; z = _z;
		}


		void ser(ser::CElement e)
		{
			e.attr(_T("index"), index);
			e.attr(_T("x"), x);
			e.attr(_T("y"), y);
			e.attr(_T("z"), z);
		}

		static const _TCHAR * tag_name() { return _T("point3i");}


	};

	struct Vect3Dd
	{
		double x;	double y;	double z;
		bool changed;
		Vect3Dd():x(0), y(0), z(0), changed(true) {}
		Vect3Dd( double _x, double _y, double _z) : x(_x), y(_y), z(_z), changed(true) {}
		Vect3Dd ( double _x, double _y) : x(_x), y(_y), z(0), changed(true) {}

		void ser(ser::CElement e)
		{
			
			e.attr(_T("x"), x);
			e.attr(_T("y"), y);
			e.attr(_T("z"), z);
		}

		static const _TCHAR * tag_name() { return _T("point3d");}
	};

	struct Extent
	{
		int index;		long ID;
		int left;		int top;		int right;		int bottom;
		void ser(ser::CElement e)
		{
			e.attr(_T("left")	, left);
			e.attr(_T("top")	, top);
			e.attr(_T("right")	, right);
			e.attr(_T("bottom")	, bottom);
		}
	};*/
/*
	class CContVect2i :public CCont<Vect2i>
	{
	public:
		void LoadFromIntArray(long * arr, int master_index, int nMasters, int nPoints);
		bool is_interpolation(const Vect2i& p);
		virtual long type_id() const { return Points2Di_Cont;}
	};

	class CContPrim :public CContPtr<CContainer>
	{
	public:
		void LoadFromIntArray(long * arr, int master_index, int nMasters, int nPoints);
		bool is_interpolation(const Vect2i& p);
		virtual long type_id() const { return PrimitiveCont;}
	};
*/
	class point_long
	{
		long x; long y;
		point_long() :x(0), y(0) {}
		point_long(int _x, int _y) :x(_x), y(_y) {}
	};

	/**
		массив из long
	*/

	class CPointDump
	{
		int master_count;		int points_count;
		std::vector <KPOINT> cont;
		std::vector <bool> changes;
		//--------------------------------
		typedef std::vector <KPOINT>::iterator points_iter;
		typedef std::vector <KPOINT>::const_iterator points_iterc;

		typedef std::vector <bool>::iterator bool_iter;
		typedef std::vector <bool>::const_iterator bool_iterc;
		//--------------------------------		
		inline int indx ( int master_index, int point_index) const; 
		inline int back_master_indx ( int index) const;
		inline int back_point_indx ( int index) const;
		//--------------------------------
		inline int correct_master_index ( int master_index) const;
		
	public:

		//--------------------------------
		CPointDump();
		CPointDump(int _master_count, int _points_count);
		//--------------------------------
		void set_master_count(int NM) { config( NM, points_count);} 
		void config(int _master_count, int _points_count);
		void resize();
		bool copy_from(const long * ptr, size_t longs_count);
		size_t get_buff(const KPOINT ** pptr) const;
		//--------------------------------
		const KPOINT& get_point(int master_index, int point_index) const;
		bool put_point(int master_index, int point_index, KPOINT p);
		//--------------------------------
		bool is_point_common(int m1,  int m2, int point_index) const;
		bool is_master_changed(int master_index) const;
		bool is_point_changed(int master_index, int point_index) const;
		bool is_pair_changed(int mi_1, int mi_2) const;
		//--------------------------------
		point_vect_t GetPoints(int from, int to) const;
		//--------------------------------
	
		inline int PointsCount() const { return points_count;}
	
		inline int MastersCount() const { return master_count;}

		void all_changed(bool value);
		void reset_points();

		int common_points_count( int master_index1, int master_index2) const;

		void set_changes(int master_index, int point_index, bool value);

		virtual void ser(ser::CElement& e);

		void old_load( CBufferInput * bi );
		//----------------------------------------------------------------
		unsigned find_valid( unsigned pi, int mi) const;
		unsigned find_valid( unsigned pi, int mi_1, int mi_2) const;
		bool find_valid( unsigned * pi, int mi) const;
		bool find_valid( unsigned * pi, int mi_1, int mi_2) const;
		unsigned valid_count(int mi_1, int mi_2) const;
		bool end(unsigned pi) const;
		//----------------------------------------------------------------
		bool is_interpolation_area(int mfrom, int mto, KPOINT point) const;
		//----------------------------------------------------------------
	};

#endif