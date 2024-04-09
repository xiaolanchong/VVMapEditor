// SRSDll.cpp : Defines the entry point for the DLL application.
//



#include "windows.h"
#define VVMDLL_API __declspec(dllexport)
//NOTENOTE: now VVMInterface folder instead SRS/SourceLibrary
#include "VVMInterface.h"
#include "memleak.h"

HMODULE hDllModule;
HINSTANCE hInstance;

BOOL APIENTRY DllMain( HANDLE hModule, 
					  DWORD  ul_reason_for_call, 
					  LPVOID /*lpReserved*/
					  )
{

    switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
#if defined _DEBUG && !defined _WIN32_WCE
		_CrtSetDbgFlag( _CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF );
#endif
		hDllModule=(HMODULE)hModule;
		hInstance =(HINSTANCE)hModule;
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
    }
    return TRUE;
}