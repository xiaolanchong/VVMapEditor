#ifndef _MAPPOLYGONEXPORT_H_
#define _MAPPOLYGONEXPORT_H_

namespace Elvees
{

struct	InfoHeader
{
	DWORD	dwType;		// тип структуры
	DWORD	dwSize;		// размер структуры(общий)
};

// dwType = 1
struct InfoCamera : InfoHeader
{
	DWORD	dwCameraID;	// id камеры, полигон  которой в структуре	
	DWORD	dwPointNumber;	// кол-во точек в полигоне
	POINT	ptPolygon[1];	// массив точек полигона
};

}

#endif // _MAPPOLYGONEXPORT_H_