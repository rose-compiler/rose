#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Architecture/IntelI486.h>

#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherX86.h>
#include <Rose/BinaryAnalysis/Unparser/X86.h>

#include <SgAsmExecutableFileFormat.h>
#include <SgAsmGenericHeader.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

IntelI486::IntelI486()
    : X86("intel-i486", 4) {}

IntelI486::~IntelI486() {}

IntelI486::Ptr
IntelI486::instance() {
    return Ptr(new IntelI486);
}

RegisterDictionary::Ptr
IntelI486::registerDictionary() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    if (!registerDictionary_.isCached()) {
        auto regs = RegisterDictionary::instance(name());
        regs->insert(Architecture::findByName("intel-i386").orThrow()->registerDictionary());
        regs->insert("ac", x86_regclass_flags, x86_flags_status, 18, 1); /* alignment check system flag */
        registerDictionary_ = regs;
    }

    return registerDictionary_.get();
}

bool
IntelI486::matchesHeader(SgAsmGenericHeader *header) const {
    ASSERT_not_null(header);
    const SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();
    return (isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_IA32_Family &&
        header->get_executableFormat()->get_wordSize() == bytesPerWord();
}

} // namespace
} // namespace
} // namespace

#endif
