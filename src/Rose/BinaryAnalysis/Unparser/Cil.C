#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Unparser/Cil.h>

#include <Rose/BinaryAnalysis/Architecture/Base.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

Cil::~Cil() {}

Cil::Cil(const Architecture::Base::ConstPtr &arch, const CilSettings &settings)
    : Base(arch), settings_(settings) {}

Cil::Ptr
Cil::instance(const Architecture::Base::ConstPtr &arch, const CilSettings &settings) {
    return Ptr(new Cil(arch, settings));
}

Base::Ptr
Cil::copy() const {
    return instance(architecture(), settings());
}

} // namespace
} // namespace
} // namespace

#endif
