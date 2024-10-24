#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Unparser/Mips.h>

#include <Rose/BinaryAnalysis/Architecture/Base.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

Mips::~Mips() {}

Mips::Mips(const Architecture::Base::ConstPtr &arch, const MipsSettings &settings)
    : Base(arch), settings_(settings) {}

Mips::Ptr
Mips::instance(const Architecture::Base::ConstPtr &arch, const MipsSettings &settings) {
    return Ptr(new Mips(arch, settings));
}

Base::Ptr
Mips::copy() const {
    return instance(architecture(), settings());
}

} // namespace
} // namespace
} // namespace

#endif
