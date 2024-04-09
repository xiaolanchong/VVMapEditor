#ifndef thread_safe_global_h_included
#define thread_safe_global_h_included

#include "sys.h"

/// потокобезопасная обертка для глобальных переменных (если уж все-таки приспичило их использовать:) )
/**
	идея: содержит список значений для всех потоков, каждый поток получает доступ к своей переменной.
	параметр шаблона - тип глобальной переменной.
*/

template <class T>
struct ThreadSafeGlobal
{
	/// Получить значение
	T get()
	{
		sys::CEnterCS cs(m_section);
		return get_pair().value;
	}

	/// Записать значение
	void write(T value)
	{
		sys::CEnterCS cs(m_section);
		get_pair().value = value;
	}

	/// максимальное число потоков, для которых резервируется память.
	/**
		Необходимо, чтобы зарезервировать память. 
	*/
	enum { max_thread_count = 50 };

	ThreadSafeGlobal() { m_data.reserve(max_thread_count); }
private:

	struct Pair
	{
		DWORD thread_id;
		T value;
		Pair() {}
		Pair(DWORD h) : thread_id(h), value(T()) {}
		bool operator == (const Pair& p) const { return p.thread_id == thread_id; }
	};

	typedef std::vector<Pair> items_vector;

	Pair& get_pair()
	{
		DWORD curr_thread_id = ::GetCurrentThreadId();
		items_vector::iterator i = std::find ( m_data.begin(), m_data.end(), Pair(curr_thread_id));
		if (i != m_data.end())
			return *i;
		
		m_data.push_back (Pair(curr_thread_id));
		return m_data.back();
	}

	items_vector m_data;
	sys::CCritSect m_section;
};

#endif