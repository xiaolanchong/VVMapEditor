// get_same_object_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "../../../Supervisor/trunk/supervisorex/batch_stuff/utils.h"
#include "../../../trunk/SRSMULTICAM/MultiCamGlobals/messages.h"

const char * dll = "VVMInterface.dll";
const char * func_name = "GetVVMInterface";
//const char * vme_file = "d:\\data\\airport\\untitled.vme";
const char * vme_file = "D:\\Data\\exhebition\\untitled.vme";

typedef long  (__cdecl *func_ptr) (CVVMapTrans **vvmi, DWORD options);

void do_test(CVVMapTrans * ptr);

int _tmain(int argc, _TCHAR* argv[])
{
	srand( 300 );
	if (HMODULE h = ::LoadLibrary (dll))
	{
		std::cout << "Loaded " << std::endl;
		if (func_ptr f = reinterpret_cast <func_ptr> (::GetProcAddress ( h, func_name)) )
		{
			CVVMapTrans * ptr = 0;
			f(&ptr, VVMINT_EDITOR);
			if (ptr) 
			{
				std::cout << "Created! " << std::endl;
				// loading vme
				byte_buff data;
				if (read_file ( vme_file, data))
				{
					std::cout << "vme loaded! (" << vme_file << ")" << std::endl;

					long err = ptr->Load ( &data[0], long (data.size()) );
					if (err == ERR_OK)
					{
						std::cout << "starting test!" << std::endl;
						do_test(ptr);
					}
					else
						std::cout << "CVVMapTrans::Load error (" << err << ")" << std::endl;
				}		
				else
					std::cout << "Cannot load vme: " << vme_file << std::endl;
				
				ptr->_Release();
			}
			else 
				std::cout << "Cannot created the Interface" << std::endl;
		}
		else
			std::cout << "Cannot get the \"" << func_name << "\" proc" << std::endl;

		::FreeLibrary (h);
	} 
	else
		std::cout << "Cannot load " << dll << std::endl;

	std::cout << "Finished!" << std::endl;
	getch();

	return 0;
}

////////////////////////////////////////////////////////////

std::ostream& operator << (std::ostream& os, const CObjectRecord& or )
{
	os << "( realID: " << or.realID << " xmin: " << or.xmin << " xmax: " << or.xmax;
	os << " ymin: " << or.ymin << " ymax: " << or.ymax <<")";
		/*
	long realID;
//	long dbID;
	WORD xmin,ymin,xmax,ymax;
	float Vx,Vy;
	signed char flags; //бит 5 - followObj, бит 4 - left object, биты 0-3 - Smoothness
	signed char classID;
	*/
	return os;
}

CObjectRecord create_obj (long id)
{
	CObjectRecord o;
	memset( &o, 0, sizeof(o));
	o.realID = id;
	return o;
};
int _rand(int min = 0)
{
	static max = 300;
	
	return min + int ((max - min)*double(rand())/RAND_MAX + 0.5);
}

CObjectRecord create_rand_obj (long id)
{
	
	CObjectRecord o = create_obj(id);
	o.xmin = _rand();
	o.xmax = _rand(o.xmin);
	o.ymin = _rand();
	o.ymax = _rand(o.ymin);
	return o;
	
};

void fill_obj_arr(CObjectRecord * arr, int N)
{
	for (int n = 0; n < N; ++n)
		arr[n] = create_rand_obj(n);
}

void print_obj_arr(CObjectRecord * arr, int N)
{

	std::cout << "object array, count = " << N << std::endl;
	for (int n = 0; n < N; ++n)
		std::cout << arr[n] << std::endl;
	std::cout << std::endl;
}

void exeb_obj_arr(CObjectRecord * arr, int N)
{
	for (int n = 0; n < N; ++n)
		arr[n] = create_rand_obj(n);
}

void do_test(CVVMapTrans * ptr)
{
	static const int N1 = 5, N2 = 10;
	static const int n1 = 3, n2 = 2;
	static const int from = 1, to = 2;

	static const long shift_x = 10, shift_y = 5;

	
	
	std::vector <CObjectRecord> objs1(N1), objs2(N2);

	fill_obj_arr(&objs1[0], N1);
	fill_obj_arr(&objs2[0], N2);

	if (N1 > n1)
	{
		objs1[n1].xmin = 20;
		objs1[n1].xmax = 80;
		objs1[n1].ymin = 50;
		objs1[n1].ymax = 150;
	}

	if (N2 > n2)
	{
		long x0, y0, x1, y1;
		long err1 = ptr->GetXY ( from, objs1[n1].xmin, objs1[n1].ymin, to, &x0, &y0);
		long err2 = ptr->GetXY ( from, objs1[n1].xmax, objs1[n1].ymax, to, &x1, &y1);

		if ((err1 == ERR_OK) && (err2 == ERR_OK))
		{
			objs2[n2].xmin = WORD(x0 + shift_x); 
			objs2[n2].xmax = WORD(x1 + shift_x);
			objs2[n2].ymin = WORD(y0 + shift_y);
			objs2[n2].ymax = WORD(y1 + shift_y);
		}
	}

	print_obj_arr(&objs1[0], N1);
	print_obj_arr(&objs2[0], N2);

	long * id_arr = 0;
	try
	{

		long err = ptr->GetSameObjects ( from, to, &objs1[0], &objs2[0], N1, N2, &id_arr);

		if (err == ERR_OK)
		{
			std::cout << "GetSameObjects result is: \n\t";
			
			if (N1 > 0)
				std::cout << id_arr[0];

			for (int n = 1; n < N1; n++)
				std::cout << "," << id_arr[n];
			std::cout << std::endl;
		}
		else
			std::cout << "GetSameObjects returned error: " << err << std::endl;
		
		
	}
	catch (const char* msg )	{ std::cout << " GetSameObjects Error : " << msg << std::endl; }
	catch (...)	{ std::cout << " GetSameObjects Error : Unknown!" << std::endl; }

	

}

