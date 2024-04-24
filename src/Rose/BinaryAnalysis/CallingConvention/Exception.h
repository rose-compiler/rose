#ifndef ROSE_BinaryAnalysis_CallingConvention_Exception_H
#define ROSE_BinaryAnalysis_CallingConvention_Exception_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/Exception.h>

#include <string>

class SgAsmType;

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

/** Error allocating argument or return value storage. */
class AllocationError: public Exception {
public:
    /** Construct error about allocating storage for something. */
    AllocationError(const std::string &mesg)
        : Exception(mesg) {}

    /** Construct error about allocating an argument. */
    static AllocationError cannotAllocateArgument(int argIndex, const std::string &argName, SgAsmType*, const std::string &decl);

    /** Construct error about allocating return value. */
    static AllocationError cannotAllocateReturn(SgAsmType*, const std::string &decl);

    virtual ~AllocationError() throw() {}
};

} // namespace
} // namespace
} // namespace

#endif
#endif
