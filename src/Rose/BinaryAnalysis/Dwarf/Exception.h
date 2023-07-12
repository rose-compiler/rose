#ifndef ROSE_BinaryAnalysis_Dwarf_Exception_H
#define ROSE_BinaryAnalysis_Dwarf_Exception_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/Exception.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Dwarf {

/** Exceptions for DWARF parsing. */
class Exception: public Rose::Exception {
public:
    Exception(const std::string &mesg)
        : Rose::Exception(mesg) {}
    ~Exception() throw () {}
};

} // namespace
} // namespace
} // namespace

#endif
#endif
