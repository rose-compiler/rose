#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Unparser/M68k.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

M68k::~M68k() {}

M68k::M68k(const Architecture::Base::ConstPtr &arch, const M68kSettings &settings)
    : Base(arch), settings_(settings) {}

M68k::Ptr
M68k::instance(const Architecture::Base::ConstPtr &arch, const M68kSettings &settings) {
    return Ptr(new M68k(arch, settings));
}

Base::Ptr
M68k::copy() const {
    return instance(architecture(), settings());
}

} // namespace
} // namespace
} // namespace

#endif
