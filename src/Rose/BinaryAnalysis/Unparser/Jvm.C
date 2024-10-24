#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Unparser/Jvm.h>

#include <Rose/BinaryAnalysis/Architecture/Base.h>

#include <SgAsmBinaryAdd.h>
#include <SgAsmBinaryMultiply.h>
#include <SgAsmIntegerValueExpression.h>
#include <SgAsmJvmInstruction.h>
#include <SgAsmMemoryReferenceExpression.h>

#include <Cxx_GrammarDowncast.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

Jvm::~Jvm() {}

Jvm::Jvm(const Architecture::Base::ConstPtr &arch, const JvmSettings &settings)
    : Base(arch), settings_(settings) {}

Jvm::Ptr
Jvm::instance(const Architecture::Base::ConstPtr &arch, const JvmSettings &settings) {
    return Ptr(new Jvm(arch, settings));
}

Base::Ptr
Jvm::copy() const {
    return instance(architecture(), settings());
}

} // namespace
} // namespace
} // namespace

#endif
