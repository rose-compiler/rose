#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Unparser/Null.h>

#include <Rose/BinaryAnalysis/Architecture/Base.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

Null::~Null() {}

Null::Null(const Architecture::Base::ConstPtr &arch, const NullSettings &settings)
    : Base(arch), settings_(settings) {}

Null::Ptr
Null::instance(const Architecture::Base::ConstPtr &arch, const NullSettings &settings) {
    return Ptr(new Null(arch, settings));
}

Base::Ptr
Null::copy() const {
    return instance(architecture(), settings());
}

} // namespace
} // namespace
} // namespace

#endif
