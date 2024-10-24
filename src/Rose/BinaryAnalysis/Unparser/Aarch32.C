#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_AARCH32
#include <Rose/BinaryAnalysis/Unparser/Aarch32.h>

#include <Rose/BinaryAnalysis/Architecture/Base.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

Aarch32::~Aarch32() {}

Aarch32::Aarch32(const Architecture::Base::ConstPtr &arch, const Aarch32Settings &settings)
    : Base(arch), settings_(settings) {}

Aarch32::Ptr
Aarch32::instance(const Architecture::Base::ConstPtr &arch, const Aarch32Settings &settings) {
    return Ptr(new Aarch32(arch, settings));
}

Base::Ptr
Aarch32::copy() const {
    return instance(architecture(), settings());
}

} // namespace
} // namespace
} // namespace

#endif
