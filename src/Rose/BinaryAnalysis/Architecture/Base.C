#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/Base.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

const std::string&
Base::name() const {
    return name_;
}

void
Base::name(const std::string &s) {
    name_ = s;
}

} // namespace
} // namespace
} // namespace

#endif
