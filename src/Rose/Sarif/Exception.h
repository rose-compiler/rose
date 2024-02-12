#ifndef ROSE_Sarif_Exception_H
#define ROSE_Sarif_Exception_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_SARIF
#include <Rose/Exception.h>

namespace Rose {
namespace Sarif {

/** Base class for SARIF exceptions. */
class Exception: public Rose::Exception {
public:
    /** Construct an exception with a message. */
    Exception(const std::string &mesg)
        : Rose::Exception(mesg) {}

    ~Exception() throw() {}
};

} // namespace
} // namespace

#endif
#endif
