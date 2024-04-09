#ifndef __EXCEPT_ALL_H__
#define __EXCEPT_ALL_H__

#include <exception>
#include <string>

#define MACRO_EXCEPTION(Child, Parent) \
class Child : public Parent \
{ \
public: \
	Child(const std::string& msg): \
		Parent(msg){}  \
	Child(const char* msg): \
		Parent(msg){} \
	Child(const Child& e): Parent(e){} \
};


class CommonException : public std::exception
{
	std::string m_what;
public:
	CommonException(const std::string& type, const std::string& cause):
	  m_what( type + (cause.empty()?"":" - ") + cause )
	  {
#ifdef EXCEPTION_INT3 
		  _asm int 3
#endif
	  }
	  CommonException(const std::string& msg):m_what(msg){}
	  const char* what() const throw() { return m_what.c_str();}
	  ~CommonException() throw() {}
};

#endif //__EXCEPT_ALL_H__