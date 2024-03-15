#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Debugger/Exception.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Debugger {

Exception::Exception(const std::string &mesg)
    : Rose::Exception(mesg) {}

Exception::~Exception() throw () {}

} // namespace
} // namespace
} // namespace

#endif
