
#include "stdafx.h"
#include "sys.h"

namespace sys
{
	std::string ANSI_str(const  _TCHAR * t)
		{
	#ifdef _UNICODE
		
			USES_CONVERSION;
			return std::string (W2A(t));
	#else
			return std::string (t);
	#endif
		}


	std::wstring WIDE_str(const WCHAR * t)
		{
			return t;
		}

	std::wstring WIDE_str(const char * t)
	{
		USES_CONVERSION;
		return std::wstring (A2W(t));
	}

	std::tstring T_str(const char * t)
		{
	#ifdef _UNICODE

			USES_CONVERSION;
			return std::tstring (A2W(t));
	#else
			return std::tstring (t);
	#endif
		}


	int sys::my_atoi(const char * string)
	{
		#ifdef _WIN32_WCE
			return _ttoi( T_str(string).c_str() );
		#else
			return atoi(string);
		#endif 	
	}
}