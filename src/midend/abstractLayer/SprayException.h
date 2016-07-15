#ifndef SPRAY_EXCEPTION
#define SPRAY_EXCEPTION

#include <exception>

namespace SPRAY {
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
} // end of namespace SPRAY
#endif
