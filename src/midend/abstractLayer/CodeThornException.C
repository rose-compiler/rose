#include <exception>
#include <string>
#include "CodeThornException.h"

namespace CodeThorn {
  Exception::Exception(std::string text) {
    this->text=text;
  }
  const char* Exception::what() const throw() {
    return text.c_str();
  }
  Exception::~Exception() throw() {
  }
  
  // schroder3 (2016-08-26): Created this exception class to distinguish
  //  exceptions that occur because of a missing normalization from other
  //  exceptions.
  NormalizationRequiredException::NormalizationRequiredException(std::string text)
    : Exception("Normalization required: " + text) {
  }
  
  NormalizationRequiredException::~NormalizationRequiredException() throw() {
  }
} // end of namespace CodeThorn
