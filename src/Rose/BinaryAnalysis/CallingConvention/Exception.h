#ifndef ROSE_BinaryAnalysis_CallingConvention_Exception_H
#define ROSE_BinaryAnalysis_CallingConvention_Exception_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/Exception.h>

#include <string>

namespace Rose {
namespace BinaryAnalysis {
namespace CallingConvention {

/** Error related to calling convention problems. */
class Exception: public Rose::Exception {
public:
    /** Construct error with specified message. */
    Exception(const std::string &mesg)
        : Rose::Exception(mesg) {}
    virtual ~Exception() throw() {}
};

/** Error occuring when parsing a declaration. */
class ParseError: public Exception {
public:
    /** Construct error with specified message. */
    ParseError(const std::string &mesg)
        : Exception(mesg) {}
    virtual ~ParseError() throw() {}
};

} // namespace
} // namespace
} // namespace

#endif
#endif
