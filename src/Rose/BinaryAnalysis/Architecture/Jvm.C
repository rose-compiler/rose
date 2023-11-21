#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/Jvm.h>

#include <Rose/BinaryAnalysis/Disassembler/Jvm.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

Jvm::Jvm()
    : Base("jvm", 1, ByteOrder::ORDER_LSB) {}

Jvm::~Jvm() {}

Jvm::Ptr
Jvm::instance() {
    return Ptr(new Jvm);
}

RegisterDictionary::Ptr
Jvm::registerDictionary() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);
    if (!registerDictionary_.isCached())
        registerDictionary_ = RegisterDictionary::instanceJvm();
    return registerDictionary_.get();
}

bool
Jvm::matchesHeader(SgAsmGenericHeader *header) const {
    ASSERT_not_null(header);
    const SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();
    return (isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_JVM;
}

Disassembler::Base::Ptr
Jvm::newInstructionDecoder() const {
    return Disassembler::Jvm::instance(shared_from_this());
}

} // namespace
} // namespace
} // namespace

#endif
