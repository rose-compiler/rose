#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/Exception.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

Exception::Exception(const std::string &message)
    : Rose::Exception(message) {}

Exception::~Exception() {}

} // namespace
} // namespace
} // namespace

#endif
