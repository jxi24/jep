#ifndef jep_exception_h
#define jep_exception_h

#include <exception>
#include <string>
#include <sstream>

namespace jep {

class jepex : public std::exception {
  private:
    std::ostringstream msg;

  public:
    jepex(const char* fcn, unsigned line, const char* class_name=0) throw()
    : std::exception() {
      msg << "<jep::";
      if (class_name) msg << class_name << "::";
      msg << fcn << ":" << line << ">: ";
    }
    jepex(const jepex& other) throw() { msg << other.what(); }
    jepex& operator= (const jepex& other) throw()
    { msg << other.what(); return *this; }
    virtual ~jepex() throw() { }
    virtual const char* what() const throw() { return msg.str().c_str(); }
    template <class T> jepex& operator<<(const T& rhs) throw()
    { msg << rhs; return *this; }
};

}

#endif
