#ifndef _ADA_UTIL_H
#define _ADA_UTIL_H 1

//~ #include "sage3basic.h"

#include <sstream>

namespace Ada_ROSE_Translation
{
  extern Sawyer::Message::Facility adalogger;

  /// initializes the logger facilities
  void logInit();
}

namespace
{
  /// converts a value of type V to a value of type U via streaming
  /// \tparam V input value type
  /// \tparam U return value type
  /// \param val the value to be converted
  template <class U, class V>
  inline
  U conv(const V& val)
  {
    U                 res;
    std::stringstream buf;

    buf << val;
    buf >> res;

    return res;
  }

  /// upcasts an object of type Derived to an object of type Base
  /// \note useful mainly in the context of overloaded functions
  template <class Base, class Derived>
  inline
  Base& as(Derived& obj)
  {
    Base& res = obj;

    return res;
  }

  //
  // loggers

#ifndef ALTERNATIVE_LOGGER

  inline
  auto logTrace() -> decltype(Ada_ROSE_Translation::adalogger[Sawyer::Message::TRACE])
  {
    return Ada_ROSE_Translation::adalogger[Sawyer::Message::TRACE];
  }

  inline
  auto logInfo() -> decltype(Ada_ROSE_Translation::adalogger[Sawyer::Message::INFO])
  {
    return Ada_ROSE_Translation::adalogger[Sawyer::Message::INFO];
  }

  inline
  auto logWarn() -> decltype(Ada_ROSE_Translation::adalogger[Sawyer::Message::WARN])
  {
    return Ada_ROSE_Translation::adalogger[Sawyer::Message::WARN];
  }

  inline
  auto logError() -> decltype(Ada_ROSE_Translation::adalogger[Sawyer::Message::ERROR])
  {
    return Ada_ROSE_Translation::adalogger[Sawyer::Message::ERROR];
  }

  inline
  auto logFatal() -> decltype(Ada_ROSE_Translation::adalogger[Sawyer::Message::FATAL])
  {
    return Ada_ROSE_Translation::adalogger[Sawyer::Message::FATAL];
  }


#else /* ALTERNATIVE_LOGGER */

  inline
  std::ostream& logTrace()
  {
    return std::cerr << "[TRACE] ";
  }

  inline
  std::ostream logInfo()
  {
    return std::cerr << "[INFO] ";
  }

  inline
  std::ostream& logWarn()
  {
    return std::cerr << "[WARN] ";
  }

  inline
  std::ostream& logError()
  {
    return std::cerr << "[ERROR] ";
  }

  inline
  std::ostream& logFatal()
  {
    return std::cerr << "[FATAL] ";
  }

  void logInit() {}
#endif /* ALTERNATIVE_LOGGER */

}
#endif /* _ADA_UTIL_H */
