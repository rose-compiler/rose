#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Architecture/IntelPentiumii.h>

#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherX86.h>
#include <Rose/BinaryAnalysis/Unparser/X86.h>

#include <SgAsmExecutableFileFormat.h>
#include <SgAsmGenericHeader.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

IntelPentiumii::IntelPentiumii()
    : X86("intel-pentiumii", 4) {}

IntelPentiumii::~IntelPentiumii() {}

IntelPentiumii::Ptr
IntelPentiumii::instance() {
    return Ptr(new IntelPentiumii);
}

RegisterDictionary::Ptr
IntelPentiumii::registerDictionary() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    if (!registerDictionary_.isCached()) {
        auto regs = RegisterDictionary::instance(name());
        regs->insert(Architecture::findByName("intel-pentium").orThrow()->registerDictionary());
        registerDictionary_ = regs;
    }

    return registerDictionary_.get();
}

bool
IntelPentiumii::matchesHeader(SgAsmGenericHeader *header) const {
    ASSERT_not_null(header);
    const SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();
    return (isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_IA32_Family &&
        header->get_executableFormat()->get_wordSize() == bytesPerWord();
}

} // namespace
} // namespace
} // namespace

#endif
