// PicBackground.h: interface for the CPicBackground class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PICBACKGROUND_H__276E5C2A_999D_4FFA_9E8C_6FACE7074484__INCLUDED_)
#define AFX_PICBACKGROUND_H__276E5C2A_999D_4FFA_9E8C_6FACE7074484__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Background.h"
#include "Exception.h"
#include <string>
#include <boost/scoped_array.hpp>

MACRO_EXCEPTION(ImageException, CommonException);
MACRO_EXCEPTION(ImageFileException, ImageException);
MACRO_EXCEPTION(ImageSizeException, ImageException);

class CPicBackground : public CBackground
{
};

class CBitmapBackground : public CPicBackground  
{
	template<class T> struct Copy
	{
		Copy( T* dst, const T* src, int size )
		{
			memcpy( dst, src, size* sizeof(T) );
		}
		Copy(){}
	};

	BITMAPINFO					m_bi;
	boost::scoped_array<BYTE>	m_bits;	
	Copy<BYTE>					m_copy;

	Bitmap			m_Image;


	
public:
	CBitmapBackground(const std::wstring& FileName);
	CBitmapBackground(BITMAPINFO* pbi, void * pbits);
	virtual ~CBitmapBackground();

	virtual void	Draw(Graphics& gr, const CRectF& rc) const;
			CSize	GetSize() const;			
};

#endif // !defined(AFX_PICBACKGROUND_H__276E5C2A_999D_4FFA_9E8C_6FACE7074484__INCLUDED_)
