#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/Amd64.h>

#include <Rose/BinaryAnalysis/Disassembler/X86.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherX86.h>
#include <Rose/BinaryAnalysis/Unparser/X86.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

Amd64::Amd64()
    : X86("amd64", 8) {}

Amd64::~Amd64() {}

Amd64::Ptr
Amd64::instance() {
    return Ptr(new Amd64);
}

RegisterDictionary::Ptr
Amd64::registerDictionary() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    if (!registerDictionary_.isCached()) {
        auto regs = RegisterDictionary::instance(name());
        regs->insert(Architecture::findByName("intel-pentium4").orThrow()->registerDictionary());

        // Additional 64-bit (and hi-end 32-bit) registers
        regs->insert("rax", x86_regclass_gpr, x86_gpr_ax, 0, 64);
        regs->insert("rbx", x86_regclass_gpr, x86_gpr_bx, 0, 64);
        regs->insert("rcx", x86_regclass_gpr, x86_gpr_cx, 0, 64);
        regs->insert("rdx", x86_regclass_gpr, x86_gpr_dx, 0, 64);
        regs->insert("rsp", x86_regclass_gpr, x86_gpr_sp, 0, 64);
        regs->insert("rbp", x86_regclass_gpr, x86_gpr_bp, 0, 64);
        regs->insert("rsi", x86_regclass_gpr, x86_gpr_si, 0, 64);
        regs->insert("rdi", x86_regclass_gpr, x86_gpr_di, 0, 64);
        regs->insert("rip", x86_regclass_ip, 0, 0, 64);
        regs->insert("rflags", x86_regclass_flags, x86_flags_status, 0, 64);

        for (unsigned i=8; i<16; i++) {
            // New general purpose registers in various widths
            std::string name = "r" + StringUtility::numberToString(i);
            regs->insert(name,     x86_regclass_gpr, i, 0, 64);
            regs->insert(name+"b", x86_regclass_gpr, i, 0,  8);
            regs->insert(name+"w", x86_regclass_gpr, i, 0, 16);
            regs->insert(name+"d", x86_regclass_gpr, i, 0, 32);

            // New media XMM registers
            regs->insert(std::string("xmm")+StringUtility::numberToString(i),
                         x86_regclass_xmm, i, 0, 128);
        }

        // Additional flag bits with no official names
        for (unsigned i=32; i<64; ++i)
            regs->insert("f"+StringUtility::numberToString(i), x86_regclass_flags, x86_flags_status, i, 1);

        // Control registers become 64 bits, and cr8 is added
        regs->resize("cr0", 64);
        regs->resize("cr1", 64);
        regs->resize("cr2", 64);
        regs->resize("cr3", 64);
        regs->resize("cr4", 64);
        regs->insert("cr8", x86_regclass_cr, 8, 0, 64);

        // Debug registers become 64 bits
        regs->resize("dr0", 64);
        regs->resize("dr1", 64);
        regs->resize("dr2", 64);
        regs->resize("dr3", 64);                        // dr4 and dr5 are reserved
        regs->resize("dr6", 64);
        regs->resize("dr7", 64);

        // Special registers
        regs->instructionPointerRegister("rip");
        regs->stackPointerRegister("rsp");
        regs->stackFrameRegister("rbp");
        regs->stackSegmentRegister("ss");

        registerDictionary_ = regs;
    }

    return registerDictionary_.get();
}

bool
Amd64::matchesHeader(SgAsmGenericHeader *header) const {
    ASSERT_not_null(header);
    const SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();
    return (isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_X8664_Family;
}

} // namespace
} // namespace
} // namespace

#endif
