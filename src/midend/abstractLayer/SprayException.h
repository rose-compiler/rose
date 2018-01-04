#ifndef SPRAY_EXCEPTION
#define SPRAY_EXCEPTION

#include <exception>
#include <string>

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

  // schroder3 (2016-08-26): Created this exception class to distinguish
  //  exceptions that occur because of a missing normalization from other
  //  exceptions.
  class NormalizationRequiredException : public Exception {
  public:
    NormalizationRequiredException(std::string text)
        : Exception("Normalization required: " + text) { }

    ~NormalizationRequiredException() throw() { }
  };
} // end of namespace SPRAY
#endif
