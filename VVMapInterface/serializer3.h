/// Модуль сериализации данных в xml-формат
/// активно использует XmlLite

#ifndef _XML_SERIALIZER_H_456546456
#define _XML_SERIALIZER_H_456546456
/*
#ifndef _WIN32_WINNT
#define  _WIN32_WINNT  0x0500
#endif
*/
#include <windows.h>
#include <tchar.h>
#include "xmllite.h"
/*
#include <iostream>
#include <fstream>
*/
#include <iterator>
#include <string>

#include "sys.h"

#include "zipper.h"

namespace ser
{

	using namespace zipper;
	using namespace XmlLite;

	// класс исключения
	class missed_data
	{
		std::tstring s;
	public:
		missed_data(_TCHAR * c)  :s(c) {}
		const _TCHAR * text() const { return s.c_str();}
	};
	///////////////////////////////////////////////////////////////////
	
	struct some_data
	{
		bool saving_masters;
		bool saving_map;
		bool saving_vismap;
		bool saving_extents;
		some_data() 
			:saving_masters (true), saving_map(true), saving_vismap (true), saving_extents(true) {}
	};
	////////////////////////////////////////////////////////////////////////
	

	class no_file : public std::exception 
	{
	public:
		no_file(const char * s) : std::exception(s) {}
	};
	class no_tag : public std::exception {};

	///"Узел" XML-дерева. Представлет собой класс с шаблонными функциями, позволяющий работать с набором различных данных
	/// инкапсулирует XmlLite::XMLElement, позволяет также сохранять/загружать вектора и спски 
	class CElement
	{
	//	bool loading;
		bool owns_doc;

		XmlLite::XMLDocument * pdoc;
		XMLElement * pelement;
	//	XMLElement element;
		

		CBuffMap * buff_map;
		
		typedef XmlLite::XMLElement::iterator XEI;
		mutable XEI ei;

		std::string fname;
	public:
		some_data sdata;

		void init();
		CElement( const CElement& e, const _TCHAR * tag);
		CElement( std::string _fname, CBuffMap * _buff_map);
		CElement( const std::string _fname, const CElement& e);

		void Close();

		virtual ~CElement();

		inline bool is_loading() const { return buff_map->is_loading();}

	private:


		void goto_begin() const; //{ if (loading) ( ei = element.begin() );}
		bool is_finish()  const;  //{ return ! (ei != element.end());}

		void goto_tag(const _TCHAR * tag) const;
		bool goto_next_tag() const;
	


		//---------------------------------------------------------------------------
		template <class T> void get_attr(const _TCHAR *attr_name, T& value)
		{
			pelement->GetAttr( attr_name, value);
		}
		//---------------------------------------------------------------------------
		template <class T> void get_attr_def(const _TCHAR *attr_name, T& value, const T& def)
		{
			try
			{
				get_attr( attr_name, value);
			}
			catch(...)
			{
				value = def;
			}
		}

		//---------------------------------------------------------------------------
		void get_attr_double(const _TCHAR *attr_name, double& value);
		void get_attr_double(const _TCHAR *attr_name, double& value, const double& def);

		template <class T> void set_attr(const _TCHAR *attr_name, const T& value) 
		{
			(*pelement) << XmlLite::XMLAttribute( *pelement, attr_name, value);
		}
		//---------------------------------------------------------------------------
		
		void set_attr_str(const _TCHAR *attr_name, const std::tstring& str);
		void set_attr_double(const _TCHAR *attr_name, const double& value);
		//---------------------------------------------------------------------------
	public:
	
		//-----------------------------------------
		void finish_doc(); 
		

		template <class T> void attr(const _TCHAR *attr_name, T& value)
		{
			if (is_loading()) 
					get_attr( attr_name, value);
			else 
					set_attr( attr_name, value);
		}

		//-----------------------------------------

		template <class T> void attr( const _TCHAR *attr_name, T& value, const T& def_value)
		{
			if (is_loading()) 
					get_attr_def( attr_name, value, def_value);
			else 
					set_attr( attr_name, value);
			
		}
		//-----------------------------------------
		void attr( const _TCHAR *attr_name, double& value);
		void attr( const _TCHAR *attr_name, std::tstring& value);
		void attr(const  _TCHAR *attr_name, double& value, const double& def_value);
		void attr( const _TCHAR *attr_name, std::tstring& value, const std::tstring& def_value);
		void attr( const _TCHAR *attr_name, bool& value);
		void attr( const _TCHAR *attr_name, bool& value, const bool& def_value);
		//-----------------------------------------

		


		/// сериализация объекта. TObj должен иметь метод void ser (ser::CElement& e);
		template <class TObj> void ser_obj(const _TCHAR * obj_tag_name, TObj& o)
		{
			if (is_loading()) 
			{
					CElement e( *this, obj_tag_name);
					//e.sdata = sdata;
					o.ser(e);
					++ei;
			}
			else
			{
				CElement e( *this, obj_tag_name);
				//e.sdata = sdata;
				o.ser(e);
				(*pelement) << (*e.pelement);
			}
		}

	


		//-----------------------------------------
		/// сериализация вектора объектов. TObj должен иметь метод void ser (ser::CElement& e);
		template <class TObj> void ser_vect(const _TCHAR * items_tag, std::vector<TObj>& v)
		{
			const _TCHAR vect_tag[] = _T("vector");
			
			if (is_loading())
			{
				goto_tag(vect_tag);
				CElement e( *this, vect_tag);
				//e.sdata = sdata;

				unsigned tag_count = 0;

				e.goto_begin();
				while (e.goto_next_tag()) tag_count++;
				e.goto_begin();
				
				v.clear();
				v.reserve(tag_count);

				/*
				std::vector <TObj>::iterator it = v.begin();
				for (; it != v.end(); ++it)
							e.ser_obj( items_tag, (*it));
							*/
				for (unsigned i = 0; i < tag_count; ++i)
				{
					v.push_back (TObj());
					e.ser_obj( items_tag, v.back());
				}
				++ei;
				
			}
			else
			{
				CElement ee( *this, vect_tag);
				//ee.sdata = sdata;
				typename std::vector<TObj>::iterator vi = v.begin();
				for(; vi != v.end(); ++vi)
				{
					TObj& ro = *vi;		ee.ser_obj( items_tag, ro);
				}
				
				(*pelement) << (*ee.pelement);

			}
		}

		/// сериализация буффера (см. описание к CBuff)
		void ser_bin(const char * fname, CBuff * buff)
		{
			if (is_loading())
			{
				if (!buff_map->get_CBuff( fname, buff ) ) 
				{
					MyTrace ( std::string("Cannnot find ") + fname + "in archive.");
					throw no_file(fname);
				}
			}
			else
			{
				buff_map->put_CBuff( fname, buff) ;
			}
		}

	

		//-----------------------------------------


		/// сериализация вектора чисел 
		/**
			T - встроенный арифметический тип
			точно поддерживаются: int, long, float, double
			пераметры:
				(const _TCHAR *) value_tag - имя тега  
				(std::vector<T>&) v - массив данных
		*/
		template <class T> 
		void ser_vect_ofnum(const _TCHAR * value_tag, std::vector<T>& v )
		{
			const _TCHAR array_tag[] = _T("vector");
			
			if (is_loading())
			{
				goto_tag (array_tag);
				CElement e (*this, array_tag);

				size_t tag_count = 0;

				e.goto_begin();
				while (e.goto_next_tag()) tag_count++;
				e.goto_begin();
				
				
				v.resize(tag_count);

				for (size_t i=0; i < tag_count; ++i)
				{
					CElement val_e( e, value_tag);
					val_e.attr(_T("Value"), v[i]);
					e.goto_next_tag();
				}
				++ei;
			}
			else
			{
				CElement ee( *this, array_tag);
				
				for(unsigned i=0; i < v.size(); ++i)
				{
					CElement val_e( ee, value_tag);
					val_e.attr(_T("Value"), v[i]);
					(*ee.pelement) << (*val_e.pelement);
				}
				
				(*pelement) << (*ee.pelement);

			}
		}

		/// сериализация массива в стиле "С"
		/**
			T - встроенный арифметический тип
			точно поддерживаются: int, long, float, double
			пераметры:
				(const _TCHAR *) value_tag - (in) имя тега  
				(T ** ) pp - (in/out) указатель на указатель на массив. 
				(long *) psize - (in/out) указатель на размер массива
				При загрузке память выделяется оператором new[] и требует "ручного" освобождения
		*/
		template <class T> 
		void ser_array(const _TCHAR * value_tag, T ** pp, long * psize )
		{
			const _TCHAR array_tag[] = _T("array");
			
			if (is_loading())
			{
				goto_tag (array_tag);
				CElement e (*this, array_tag);

				int tag_count = 0;

				e.goto_begin();
				while (e.goto_next_tag()) tag_count++;
				e.goto_begin();
				
				*psize = tag_count;
				*pp = new T[tag_count];

				for (int i=0; i < tag_count; ++i)
				{
					CElement val_e( e, value_tag);
					val_e.attr(_T("Value"), (*pp)[i]);
					e.goto_next_tag();
				}
				++ei;
			}
			else
			{
				CElement ee( *this, array_tag);
				
				for(int i=0; i < *psize; ++i)
				{
					CElement val_e( ee, value_tag);
					val_e.attr(_T("Value"), (*pp)[i]);
					(*ee.pelement) << (*val_e.pelement);
				}
				
				(*pelement) << (*ee.pelement);

			}
		}
		
		/// сериализация массива в стиле "С" постоянного размера
		/**
			T - встроенный арифметический тип
			точно поддерживаются: int, long, float, double
			пераметры:
				(const _TCHAR *) value_tag - (in) имя тега  
				(T ** ) pp - (in/out) указатель на указатель на массив. 
				(long *) psize - (in/out) указатель на размер массива
				При загрузке память выделяется оператором new[] и требует "ручного" освобождения
		*/

		template <class T> 
		void ser_static_array(const _TCHAR * value_tag, T * p, size_t size)
		{
			const _TCHAR array_tag[] = _T("static_array");
			
			if (is_loading())
			{
				goto_tag (array_tag);
				CElement e (*this, array_tag);

				unsigned tag_count = 0;
			//	e.goto_tag(value_tag);

				e.goto_begin();
				
				while (e.goto_next_tag()) 
								tag_count++;
				e.goto_begin();

				size= tag_count;
				for (unsigned i = 0; i < size && p; ++i)
				{
					CElement val_e( e, value_tag);
					val_e.attr(_T("Value"), p[i]);	
				
					if (!e.goto_next_tag()) 
									throw no_tag();
				}
				++ei;
				
			}
			else
			{
				CElement ee( *this, array_tag);
				
				for (unsigned i = 0; i < size; ++i)
				{
					CElement val_e( ee, value_tag);
					val_e.attr(_T("Value"), p[i]);
					(*ee.pelement) << (*val_e.pelement);
				}
				
				(*pelement) << (*ee.pelement);

			}
		}

		/// сериализация массива в стиле "С" постоянного размера
		/**
			T - встроенный арифметический тип
			точно поддерживаются: int, long, float, double
			пераметры:
				(const _TCHAR *) value_tag - (in) имя тега  
				(T ** ) pp - (in/out) указатель на указатель на массив. 
				(long *) psize - (in/out) указатель на размер массива
				При загрузке память выделяется оператором new[] и требует "ручного" освобождения
		*/

		template <class TPtr, class TFab, class TObj> 
			void ser_map_int_ptr(TObj * dummy, const _TCHAR * items_tag, std::map<int, TPtr> & m, const TFab * fab)
		{
			const _TCHAR * map_tag = _T("MAP-int-ptr");
			const _TCHAR * key_attr = _T("key");
			const _TCHAR * type_attr = _T("type_str");
			
			
			if (is_loading())
			{
				goto_tag(map_tag);
				CElement e( *this, map_tag);

				e.goto_begin();
				
				while (!e.is_finish())
				{
					int key;
					std::tstring s;
					CElement ee( e, items_tag);
					ee.attr( key_attr, key);
					ee.attr( type_attr, s);
					
					TObj * o = fab->create(s);
					TPtr ptr (o);
					ptr.ser(ee);
					m[key] = ptr;
					++e.ei;
				}

				
			}
			else
			{
				CElement e( *this, map_tag);
				
				typename std::map<int, TPtr>::iterator mi = m.begin();
				for(; mi != m.end(); ++mi)
				{
					CElement ee( *this, items_tag);
					
					int key = mi->first; TPtr p (mi->second);
					std::tstring s (p.ptr->type_str());
					ee.attr(key_attr, key);
					ee.attr(type_attr, s);
					
					p.ser(ee);
					(*e.pelement) << (*ee.pelement);
				}
				
				(*pelement) << (*e.pelement);

			}
		}//*/

	};

		//-----------------------------------------


		/*

			//-----------------------------------------
		template <class TObj, class TFab> void ser_polymorf(TObj * o, TFab * fab)
		{
			const _TCHAR * type_tag = _T("type_id");
			
			if (is_loading()) 
			{
				goto_tag(TObj::tag_name());
				
				CElement e( *this, *ei);

				std::tstring ti;
				e.attr(type_tag, ti)
				o->ser(e);
				++ei;

			}
			else
			{
				CElement e( *this, TObj::tag_name());
				
				e.attr(type_tag, TObj::type_id());
				o->ser(e);

				element << e.element;
			}
		}

		//-----------------------------------------
		template <class TObj> void ser_vect_of_ptrs(std::vector<TObj*>& v)
		{
			const _TCHAR * vect_tag = _T("vector_of_ptrs");
			
			if (doc->loading)
			{
				goto_tag(vect_tag);
				CElement e( *this, *ei);

				while (!e.is_finish()) 
				{
					TObj * po = new TObj;
					e.ser_obj(*po);
					v.push_back(po);
				}
				
			}
			else
			{
				CElement e( *this, vect_tag);
				
				std::vector<TObj>::iterator vi = v.begin();
				for(; vi != v.end(); ++vi)
									e.ser_obj(*vi);

				element << e.element;
			}
		}
		//----------------------------------------------------------------------
		//-----------------------------------------
/*		template <class T> void ser_arr(CBuff)
		{
			const _TCHAR * the_tag = _T("array");
			
			if (doc->loading)
			{
				goto_tag(the_tag);
				CElement e( *this, *ei);

				std::vector <T> v;
				v.reserve(1000);

				while (!e.is_finish()) 
				{
					T t;
					e.attr(t);
					v.push_back(t);
				}

				if (v.size()) 
				{
					*
				}
				else 
				{
				}
				
			}
			else
			{
				CElement e( *this, vect_tag);
				
				std::vector<TObj>::iterator vi = v.begin();
				for(; vi != v.end(); ++vi)
									e.ser_obj(*vi);

				element << e.element;
			}
		}


		//-----------------------------------------
		template <class TObj, class TFab> void ser_vpolymorfs(std::vector<TObj*>& v, TFab * fab)
		{
			const _TCHAR * vect_tag = _T("vector_of_polymorfs");
			
					
			if (doc->loading)
			{
				goto_tag(vect_tag);
				CElement e( *this, *ei);
				
				while (!e.is_finish()) 
				{
					long tid;
					e.attr(type_tag, tid);

					TObj * po = fab->GetNew(tid);
					
					e.ser_obj(*po);
				
					//помещаем полученный указатель в вектор на место tid
					if (tid > v.size() - 1) v.resize(tid+1);

					v[tid] = po;
				}
				
			}
			else
			{
				CElement e( *this, vect_tag);
				
				std::vector<TObj>::iterator vi = v.begin();
				for(; vi != v.end(); ++vi)
				{

					e.ser_obj(*vi);
				}

				element << e.element;
			}
		}

	

	};
/*


	template <class TObj> bool ser_obj(CFileMan * fm, _TCHAR * file_name, TObj& obj)
	{
		try 
		{
			CDoc d(fm, file_name);
			CElement e(d);

			e.ser_obj( obj);
			
			e.finish_doc();
			d.Close();
		}
		catch(...)
		{
			return false;
		}

		return true;
	}*/
/*
	template <class TObj> bool save_obj(CFileMan * fm, _TCHAR * file_name, const TObj& obj)
	{
		TObj * o = const_cast <TObj*> (&obj);
		return ser_obj( fm, file_name, *o, false);
	}

	template <class TObj> long load_obj(CFileMan * fm, _TCHAR * file_name, TObj& obj)
	{
		return ser_obj( fm, file_name, obj, true);
	}
*/
}
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////





#endif



