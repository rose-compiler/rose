#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Unparser/Powerpc.h>

#include <Rose/BinaryAnalysis/Architecture/Base.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

Powerpc::~Powerpc() {}

Powerpc::Powerpc(const Architecture::Base::ConstPtr &arch, const PowerpcSettings &settings)
    : Base(arch), settings_(settings) {}

Powerpc::Ptr
Powerpc::instance(const Architecture::Base::ConstPtr &arch, const PowerpcSettings &settings) {
    return Ptr(new Powerpc(arch, settings));
}

Base::Ptr
Powerpc::copy() const {
    return instance(architecture(), settings());
}

} // namespace
} // namespace
} // namespace

#endif
