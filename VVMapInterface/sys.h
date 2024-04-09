#ifndef _SYS_H_SYS_H_
#define _SYS_H_SYS_H_


//#include "../StdAfx.h"

//#include <list>
#include <tchar.h>
//#include <iostream>

//#include "win.h"

#include <string>

#if (defined(_UNICODE) || defined (UNICODE) )
	#define tstring wstring
#else	
	#define tstring string
#endif

extern HMODULE hDllModule;
extern HINSTANCE hInstance;
//одно и то же

namespace sys
{

	std::tstring get_res_string(unsigned str_ID);
	std::string ANSI_str(const _TCHAR * t);
	std::wstring WIDE_str(const  char * t);
	std::wstring WIDE_str(const WCHAR * t);
	std::tstring T_str(const char * t);


	class crit_sect_occuped {};
	
	class CEnterCS;
	class CTryEnterCS;

	/// класс Критческой секции 
	/**
		для того, чтобы войти в критическую секцию, необходимо создать CEnterCS (либо CTryEnterCS),
		передав в конструктор ссылку на критическую секцию. (см. описания CEnterCS и CTryEnterCS).
		Реализация в sys3.cpp
	*/
	
	class CCritSect
	{
		CRITICAL_SECTION cs;
		friend class CEnterCS;
		friend class CTryEnterCS;
	public:
		CCritSect ();
		virtual ~CCritSect ();
	};

	/// Входит в критическую секцию 
	/**
		При создании экземпляра этого класса входим в критическую секцию (в конструкторе). 
		При разрушении класса - выходим (в деструкторе).
		Удобный класс, позволяет следовать стратегии "Выделение ресурсов есть инициализация"
		и писать код безопасный для исключений. 
		(т.е. LeaveCriticalSection будет вызван даже если произошло исключение)
		Реализация в sys3.cpp
	*/
	class CEnterCS
	{
		CCritSect& csc;
		CEnterCS(const CEnterCS&);
		CEnterCS& operator= (const CEnterCS&);
	public:
		/// конструктор
		/**
			передаем ссылку на критическую секцию, в которую хотим войти
		*/
		CEnterCS(CCritSect& _csc);
		virtual ~CEnterCS();
	};

	/// Входит в критическую секцию с проверкой
	/**
		если критич секция занята, выбрасывается исключение sys::crit_sect_occuped
	*/
	class CTryEnterCS
	{
		CCritSect& csc;
		CTryEnterCS(const CTryEnterCS&);
		CTryEnterCS& operator= (const CTryEnterCS&);
	public:
		/// конструктор
		/**
			передаем ссылку на критическую секцию, в которую хотим войти
			если критич секция занята, выбрасывается исключение sys::crit_sect_occuped
		*/
		CTryEnterCS(CCritSect& _csc);
		~CTryEnterCS();
	};
	
	/// потокобезопасно присваивает in out (см. в SDK InterlockedExchange)
	void ExchangeLongs( long& out, const long& in);

	int my_atoi(const char * string);

}

#endif