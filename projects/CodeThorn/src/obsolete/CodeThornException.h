#ifndef CODETHORN_EXCEPTION
#define CODETHORN_EXCEPTION

#include <exception>
#include <string>

namespace CodeThorn {
  class Exception : public std::exception {
  public:
    Exception(std::string text) {
      this->text=text;
    }
    const char* what() const throw() {
      return text.c_str();
    }
    ~Exception() throw() {
    }
  private:
    std::string text;
  };
} // end of namespace CodeThorn
#endif
