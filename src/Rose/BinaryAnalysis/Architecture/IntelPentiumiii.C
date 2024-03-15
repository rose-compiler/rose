#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Architecture/IntelPentiumiii.h>

#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherX86.h>
#include <Rose/BinaryAnalysis/Unparser/X86.h>

#include <SgAsmExecutableFileFormat.h>
#include <SgAsmGenericHeader.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

IntelPentiumiii::IntelPentiumiii()
    : X86("intel-pentiumiii", 4) {}

IntelPentiumiii::~IntelPentiumiii() {}

IntelPentiumiii::Ptr
IntelPentiumiii::instance() {
    return Ptr(new IntelPentiumiii);
}

RegisterDictionary::Ptr
IntelPentiumiii::registerDictionary() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    if (!registerDictionary_.isCached()) {
        auto regs = RegisterDictionary::instance(name());
        regs->insert(Architecture::findByName("intel-pentiumii").orThrow()->registerDictionary());

        regs->insert("xmm0", x86_regclass_xmm, 0, 0, 128);
        regs->insert("xmm1", x86_regclass_xmm, 1, 0, 128);
        regs->insert("xmm2", x86_regclass_xmm, 2, 0, 128);
        regs->insert("xmm3", x86_regclass_xmm, 3, 0, 128);
        regs->insert("xmm4", x86_regclass_xmm, 4, 0, 128);
        regs->insert("xmm5", x86_regclass_xmm, 5, 0, 128);
        regs->insert("xmm6", x86_regclass_xmm, 6, 0, 128);
        regs->insert("xmm7", x86_regclass_xmm, 7, 0, 128);

        /** SSE status and control register. */
        regs->insert("mxcsr",     x86_regclass_flags, x86_flags_mxcsr,  0, 32);
        regs->insert("mxcsr_ie",  x86_regclass_flags, x86_flags_mxcsr,  0,  1); // invalid operation flag
        regs->insert("mxcsr_de",  x86_regclass_flags, x86_flags_mxcsr,  1,  1); // denormal flag
        regs->insert("mxcsr_ze",  x86_regclass_flags, x86_flags_mxcsr,  2,  1); // divide by zero flag
        regs->insert("mxcsr_oe",  x86_regclass_flags, x86_flags_mxcsr,  3,  1); // overflow flag
        regs->insert("mxcsr_ue",  x86_regclass_flags, x86_flags_mxcsr,  4,  1); // underflow flag
        regs->insert("mxcsr_pe",  x86_regclass_flags, x86_flags_mxcsr,  5,  1); // precision flag
        regs->insert("mxcsr_daz", x86_regclass_flags, x86_flags_mxcsr,  6,  1); // denormals are zero
        regs->insert("mxcsr_im",  x86_regclass_flags, x86_flags_mxcsr,  7,  1); // invalid operation mask
        regs->insert("mxcsr_dm",  x86_regclass_flags, x86_flags_mxcsr,  8,  1); // denormal mask
        regs->insert("mxcsr_zm",  x86_regclass_flags, x86_flags_mxcsr,  9,  1); // divide by zero mask
        regs->insert("mxcsr_om",  x86_regclass_flags, x86_flags_mxcsr, 10,  1); // overflow mask
        regs->insert("mxcsr_um",  x86_regclass_flags, x86_flags_mxcsr, 11,  1); // underflow mask
        regs->insert("mxcsr_pm",  x86_regclass_flags, x86_flags_mxcsr, 12,  1); // precision mask
        regs->insert("mxcsr_r",   x86_regclass_flags, x86_flags_mxcsr, 13,  2); // rounding mode
        regs->insert("mxcsr_fz",  x86_regclass_flags, x86_flags_mxcsr, 15,  1); // flush to zero

        registerDictionary_ = regs;
    }

    return registerDictionary_.get();
}

bool
IntelPentiumiii::matchesHeader(SgAsmGenericHeader *header) const {
    ASSERT_not_null(header);
    const SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();
    return (isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_IA32_Family &&
        header->get_executableFormat()->get_wordSize() == bytesPerWord();
}

} // namespace
} // namespace
} // namespace

#endif
