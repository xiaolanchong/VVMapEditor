

#ifndef _ZIPPER_H_235235
#define _ZIPPER_H_235235

#pragma warning (disable : 4786)


#include <map>
#include <vector>
#include <string>

class CVVMReal;
namespace zipper
{
	struct CBuff
	{

		unsigned char * ptr;
		size_t size;
	public:
		CBuff();
		CBuff(unsigned char * _ptr, size_t _size); //не копирует, а присваивает
		CBuff(void * _ptr, size_t _size); 
		CBuff(size_t _size);

		virtual ~CBuff();

		void assign(void * _ptr, size_t _size); //не копирует, а присваивает
		void copy_from(const void * _ptr, size_t _size); //копирует в себя из _ptr[]
		void release(); //вызывает if (ptr) delete [] ptr;
		void resize(size_t size);

		void concat(CBuff another_buff);
/*
		unsigned char * get_ptr() const { return ptr;}
		const unsigned char * get_ptr_const() const { return const_cast <const unsigned char *> (ptr);}
		void * get_ptr_void() const { return const_cast <void *> (ptr);}
		const void * get_ptr_const_void() const 
		{ 
			return 
				const_cast <const void *> 
				(reinterpret_cast <void*> (ptr) );
		}

		size_t get_size() const { return size;}
*/
	};

	struct CAutoBuff : CBuff
	{
		virtual ~CAutoBuff() { release();}
	};

	class CBuffMap :protected std::map < std::string, CBuff > 
	{
	public:

		typedef std::map < std::string, CBuff >::const_iterator iter_c;
		typedef std::map < std::string, CBuff >::iterator iter;

		void map_clear();
		virtual bool is_loading() const = 0;
		virtual bool get_buff ( std::string name, const void ** pbuff, size_t * psize) const;
		virtual void put_buff ( std::string name, const void * pbuff, size_t size);
	//--------------------------------------
		virtual bool get_buffc ( std::string name, const unsigned char ** pbuff, size_t * psize) const
		{
			return get_buff ( name, reinterpret_cast <const void **> (pbuff), psize);
		}

		virtual void put_buffc ( std::string name, const unsigned char * pbuff, size_t size)
		{
			put_buff ( name, reinterpret_cast <const void *> (pbuff), size);
		}

		virtual bool get_CBuff ( std::string name, CBuff * buff) const
		{
			const void * p = 0;
			bool res =  get_buff ( name, &p, &buff->size);
			buff->ptr =  reinterpret_cast <unsigned char *> (const_cast <void *> (p) );
			return res;
		}

		virtual void put_CBuff ( std::string name, CBuff * buff)
		{
			const void * p = const_cast <const void *> ( reinterpret_cast <void *>(buff->ptr) );
			put_buff ( name, p, buff->size);
		}

		virtual ~CBuffMap();

	};

	class CUnZipper :public CBuffMap
	{
	public:
		bool UnzipFrom(const CBuff& buff, const std::string def_file);

		virtual bool is_loading() const { return true;}
	};

	class CZipper :public CBuffMap
	{
	public:
		
		virtual bool is_loading() const { return false;}

		bool ZipTo(CBuff& buff);
	};


}


#endif
