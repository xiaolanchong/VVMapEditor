#include <string>

#ifndef tstring
	#if (defined(_UNICODE) || defined (UNICODE) )
		#define tstring wstring
	#else	
		#define tstring string
	#endif
#endif	

