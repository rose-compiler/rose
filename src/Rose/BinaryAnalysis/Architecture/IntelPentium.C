#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Architecture/IntelPentium.h>

#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherX86.h>
#include <Rose/BinaryAnalysis/Unparser/X86.h>

#include <SgAsmExecutableFileFormat.h>
#include <SgAsmGenericHeader.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

IntelPentium::IntelPentium()
    : X86("intel-pentium", 4) {}

IntelPentium::~IntelPentium() {}

IntelPentium::Ptr
IntelPentium::instance() {
    return Ptr(new IntelPentium);
}

RegisterDictionary::Ptr
IntelPentium::registerDictionary() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    if (!registerDictionary_.isCached()) {
        auto regs = RegisterDictionary::instance(name());
        regs->insert(Architecture::findByName("intel-i486").orThrow()->registerDictionary());

        // Additional flags
        regs->insert("vif", x86_regclass_flags, x86_flags_status, 19, 1); // virtual interrupt flag
        regs->insert("vip", x86_regclass_flags, x86_flags_status, 20, 1); // virt interrupt pending
        regs->insert("id",  x86_regclass_flags, x86_flags_status, 21, 1); // ident system flag

        // The MMi registers are aliases for the ST(i) registers but are absolute rather than relative to the top of the
        // stack. We're creating the static definitions, so MMi will point to the same storage as ST(i) for 0<=i<=7. Note that a
        // write to one of the 64-bit MMi registers causes the high-order 16 bits of the corresponding ST(j) register to be set to
        // all ones to indicate a NaN value.
        regs->insert("mm0", x86_regclass_st, x86_st_0, 0, 64);
        regs->insert("mm1", x86_regclass_st, x86_st_1, 0, 64);
        regs->insert("mm2", x86_regclass_st, x86_st_2, 0, 64);
        regs->insert("mm3", x86_regclass_st, x86_st_3, 0, 64);
        regs->insert("mm4", x86_regclass_st, x86_st_4, 0, 64);
        regs->insert("mm5", x86_regclass_st, x86_st_5, 0, 64);
        regs->insert("mm6", x86_regclass_st, x86_st_6, 0, 64);
        regs->insert("mm7", x86_regclass_st, x86_st_7, 0, 64);

        registerDictionary_ = regs;
    }

    return registerDictionary_.get();
}

bool
IntelPentium::matchesHeader(SgAsmGenericHeader *header) const {
    ASSERT_not_null(header);
    const SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();
    return (isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_IA32_Family &&
        header->get_executableFormat()->get_wordSize() == bytesPerWord();
}

} // namespace
} // namespace
} // namespace

#endif
