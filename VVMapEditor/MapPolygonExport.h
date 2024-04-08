#ifndef _MAPPOLYGONEXPORT_H_
#define _MAPPOLYGONEXPORT_H_

namespace Elvees
{

struct	InfoHeader
{
	DWORD	dwType;		// ��� ���������
	DWORD	dwSize;		// ������ ���������(�����)
};

// dwType = 1
struct InfoCamera : InfoHeader
{
	DWORD	dwCameraID;	// id ������, �������  ������� � ���������	
	DWORD	dwPointNumber;	// ���-�� ����� � ��������
	POINT	ptPolygon[1];	// ������ ����� ��������
};

}

#endif // _MAPPOLYGONEXPORT_H_