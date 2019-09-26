#ifndef ROSE_Exception_H
#define ROSE_Exception_H

#include <stdexcept>

namespace Rose {

/** Base class for all ROSE exceptions. */
class Exception: public std::runtime_error {
public:
    /** Construct an exception with an error message. */
    explicit Exception(const std::string &s): std::runtime_error(s) {}
    ~Exception() throw() {}
};

} // namespace

#endif
