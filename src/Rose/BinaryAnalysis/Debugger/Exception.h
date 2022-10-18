#ifndef ROSE_BinaryAnalysis_Debugger_Exception_H
#define ROSE_BinaryAnalysis_Debugger_Exception_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/Exception.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Debugger {

/** Exceptions for debugging. */
class Exception: public Rose::Exception {
public:
    /** Construct new exception with message. */
    Exception(const std::string &mesg);

    ~Exception() throw();
};

} // namespace
} // namespace
} // namespace

#endif
#endif
