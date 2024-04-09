
#include "stdafx.h"
#include "serializer3.h"

namespace ser
{

	using namespace zipper;

		const _TCHAR * root_name = _T("root");

		CElement::CElement( const CElement& e, const _TCHAR * tag) 
			: owns_doc(false), pdoc(e.pdoc), buff_map(e.buff_map),
			 fname( e.fname), sdata(e.sdata)
		{
			if (is_loading()) 
			{ 
				if (e.is_finish()) throw no_tag();
				pelement = new XmlLite::XMLElement( *(e.ei) ); 
				ei = pelement->begin();
			}
			
			else pelement = new XmlLite::XMLElement( *(e.pdoc), tag );
			
			
		}

		void CElement::init()
		{
			if (is_loading())
			{				
				const void * p = 0; size_t s = 0;

				if ( buff_map->get_buff( fname, &p, &s) && s )
				{
					std::vector<byte> bin(s);
					memcpy( &bin[0], p, s);

					pdoc->LoadFromBinary(&bin[0], s);

					pelement = new XmlLite::XMLElement( pdoc->GetRoot());
					goto_begin();
				}
				else throw no_file(fname.c_str());
			}
			else 
			{
				pelement = new XmlLite::XMLElement( *pdoc, root_name);
				//(*pdoc) << (*pelement); 
			}
		}

		CElement::CElement( std::string _fname, CBuffMap * _buff_map)
			: owns_doc(true), pdoc( new XmlLite::XMLDocument() ),
			  buff_map(_buff_map), fname( _fname), pelement(0)
		{
			init();
		}
		
		CElement::CElement( const std::string _fname, const CElement& e)
			: owns_doc(true), pdoc( new XmlLite::XMLDocument ),
			 buff_map(e.buff_map), fname( _fname), pelement(0), 
			 sdata(e.sdata)
		{
			init();
		}

		CElement::~CElement() 
		{ 
			ei = XEI() ;
			if (pelement) { delete pelement; pelement = 0; }
			if (owns_doc && pdoc) 
			{ 
				delete pdoc; pdoc =0; 
			}
		}

		void CElement::Close()
		{
			if (!is_loading())
			{
				finish_doc();
				std::vector<byte> Arr;
				pdoc->SaveToBinary(Arr);
//				size_t s = Arr.size();
				//fman->Do_serialize(file_name, Arr);
				if (Arr.size())
					buff_map->put_buff ( fname, &Arr[0], Arr.size());
			}
		}

		void CElement::goto_begin() const
		{ 
			if (is_loading()) ( ei = pelement->begin() );
		}
		
		bool CElement::is_finish() const
		{ 
			return ! (ei != pelement->end());
		}

		void CElement::goto_tag(const _TCHAR * tag)  const
		{
			while (true)
			{
				if (is_finish()) throw no_tag();
				
				try 
				{
					XmlLite::XMLElement e(*ei);
					//std::tstring ts = e.Name();
					if (e.Name() == std::tstring(tag) )	return;
				}
				catch (XMLElementException) {}
				
				++ei;
			}
		}


		bool CElement::goto_next_tag() const
		{
			bool fin = is_finish();
			++ei;
			return !fin;
		}

		//---------------------------------------------------------------------------
		void CElement::get_attr_double(const _TCHAR *attr_name, double& value)
		{
				std::tstring ts;
				get_attr(attr_name, ts);
				value = atof(sys::ANSI_str(ts.c_str()).c_str());
		}
		//---------------------------------------------------------------------------
		void CElement::get_attr_double(const _TCHAR *attr_name, double& value, const double& def)
		{
			try
			{
				get_attr_double( attr_name, value);
			}
			catch (...)
			{
				value = def;
			}
		}

		//---------------------------------------------------------------------------
		
		void CElement::set_attr_str(const _TCHAR *attr_name, const std::tstring& str) 
		{
			set_attr( attr_name, str.c_str());
		}

		//---------------------------------------------------------------------------
		void CElement::set_attr_double(const _TCHAR *attr_name, const double& value) 
		{
			const size_t size = 32;
			_TCHAR tmp[size];		memset(tmp, 0, size);
			_sntprintf(tmp, size, _T(" %f"), value);

			set_attr( attr_name, tmp);
		}
		//-----------------------------------------
		//-----------------------------------------
		//-----------------------------------------
		void CElement::finish_doc() 
		{ 
			//XmlLite::XMLDocument * pd = const_cast<XmlLite::XMLDocument *> (&doc->doc);
			pdoc->SetRoot(*pelement);
		}

		//-----------------------------------------
		//-----------------------------------------
		//-----------------------------------------
		void CElement::attr( const _TCHAR *attr_name, double& value)
		{
			if (is_loading()) 
					get_attr_double( attr_name, value);
			else 
					set_attr( attr_name, value);
		}


		//-----------------------------------------
		void CElement::attr( const _TCHAR *attr_name, bool& value)
		{
			const _TCHAR t[] = _T("true");	const _TCHAR f[] = _T("false");
			if (is_loading()) 
			{
				std::tstring s;
				get_attr( attr_name, s);
				value = (s == std::tstring (t)); 
			}
			else
			{
				std::tstring s( (value) ? (t) : (f));
				set_attr_str( attr_name, s);
			}	
		}
		//-----------------------------------------
		void CElement::attr( const _TCHAR *attr_name, bool& value, const bool& def_value)
		{
			try
			{
				attr( attr_name, value);
			}
			catch (...)
			{
				if (is_loading()) value = def_value;
			}
		}
		//-----------------------------------------
		void CElement::attr(const _TCHAR *attr_name, std::tstring& value)
		{
			if (is_loading()) 
					get_attr( attr_name, value);
			else 
					set_attr_str( attr_name, value);
		}

		//-----------------------------------------
		void CElement::attr( const _TCHAR *attr_name, double& value, const double& def_value)
		{
			if (is_loading()) 
					get_attr_double( attr_name, value, def_value);
			else 
					set_attr( attr_name, value);
		}
		//-----------------------------------------
		void CElement::attr(const _TCHAR *attr_name, std::tstring& value, const std::tstring& def_value)
		{
			if (is_loading()) 
					get_attr_def( attr_name, value, def_value);
			else 
					set_attr_str( attr_name, value);
		}
}