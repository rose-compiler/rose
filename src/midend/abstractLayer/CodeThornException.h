#ifndef CODETHORN_EXCEPTION
#define CODETHORN_EXCEPTION

#include <exception>
#include <string>

namespace CodeThorn {
  class Exception : public std::exception {
  public:
    Exception(std::string text);
    const char* what() const throw();
    ~Exception() throw();
  private:
    std::string text;
  };

  // schroder3 (2016-08-26): Created this exception class to distinguish
  //  exceptions that occur because of a missing normalization from other
  //  exceptions.
  class NormalizationRequiredException : public CodeThorn::Exception {
  public:
    NormalizationRequiredException(std::string text);
    ~NormalizationRequiredException() throw();
  };
} // end of namespace CodeThorn

// backward compatibility
namespace SPRAY = CodeThorn;

#endif
