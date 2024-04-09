
#ifndef _FRAMES_H_
#define _FRAMES_H_


#include "kpoint.h"
//#include "../config_enums.h"
//#include "../transitions/trans.h"

//#include "../serializer/serializer3.h"



/*
namespace trans
{
	class CTrans;
}
*/


namespace masters
{

	/// мастера и карты

	/**соджержит контейнеры точек, различных примитивов
	*/

	class CFrame 
	{
		long ID;
		public:
			//long index;
			virtual long getID() const { return ID;}
			virtual void setID(long _ID) { ID = _ID;}
	
			CFrame() : ID(0) {}
			virtual ~CFrame() {};		
			virtual void ser(ser::CElement& e)
			{
				e.attr(_T("ID"), ID);
			}
			virtual long type_id() const = 0;
			virtual const _TCHAR * type_str() const = 0;
			//virtual const char * name() const = 0;
	};

	/**
		двумерный "мастер" без картинки
		* массив точек
		* содержит "двумерные" примитивы. массив точек
		* размеры ширина/высота.
		* проверка видимости (= 0)
		* 
	*/
	class CFrame2D :public CFrame
	{
		//размеры ширина/высота
		//
	public:
		virtual int width()		const = 0;
		virtual int height()	const = 0;

		virtual bool inside(int x, int y) const { return (x>=0) && (x<width()) && (y>=0) && (y<height());}
		virtual bool is_vis(long /*mi*/, int /*x*/, int /*y*/) const { return true;}
		
		virtual KPOINT DockPoint() const { return KPOINT ( -50/*width()/2*/, -50/*height()/2)*/); }
		virtual void ser(ser::CElement& e)
		{
			CFrame::ser(e);
		}

		virtual ~CFrame2D() {}
	};

}


#endif