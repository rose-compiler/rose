#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Architecture/Intel80286.h>

#include <Rose/BinaryAnalysis/Disassembler/X86.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherX86.h>
#include <Rose/BinaryAnalysis/Unparser/X86.h>

#include <SgAsmExecutableFileFormat.h>
#include <SgAsmGenericHeader.h>
#include <Cxx_GrammarDowncast.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

Intel80286::Intel80286()
    : X86("intel-80286", 2) {}

Intel80286::~Intel80286() {}

Intel80286::Ptr
Intel80286::instance() {
    return Ptr(new Intel80286);
}

RegisterDictionary::Ptr
Intel80286::registerDictionary() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    if (!registerDictionary_.isCached()) {
        auto regs = RegisterDictionary::instance(name());
        regs->insert(Architecture::findByName("intel-8088").orThrow()->registerDictionary());
        regs->insert("iopl", x86_regclass_flags, x86_flags_status, 12, 2); /*  I/O privilege level flag */
        regs->insert("nt",   x86_regclass_flags, x86_flags_status, 14, 1); /*  nested task system flag */
        registerDictionary_ = regs;
    }

    return registerDictionary_.get();
}

bool
Intel80286::matchesHeader(SgAsmGenericHeader *header) const {
    ASSERT_not_null(header);
    const SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();
    return isSgAsmDOSFileHeader(header) &&
        (isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_IA32_Family &&
        header->get_executableFormat()->get_wordSize() == bytesPerWord();
}

} // namespace
} // namespace
} // namespace

#endif
