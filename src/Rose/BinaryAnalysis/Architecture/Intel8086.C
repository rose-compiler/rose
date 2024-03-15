#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Architecture/Intel8086.h>

#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherX86.h>
#include <Rose/BinaryAnalysis/Unparser/X86.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

Intel8086::Intel8086()
    : X86("intel-8086", 2) {}

Intel8086::~Intel8086() {}

Intel8086::Ptr
Intel8086::instance() {
    return Ptr(new Intel8086);
}

RegisterDictionary::Ptr
Intel8086::registerDictionary() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    if (!registerDictionary_.isCached()) {
        auto regs = RegisterDictionary::instance(name());

        // 16-bit general purpose registers. Each has three names depending on which bytes are reference.
        regs->insert("al", x86_regclass_gpr, x86_gpr_ax, 0, 8);
        regs->insert("ah", x86_regclass_gpr, x86_gpr_ax, 8, 8);
        regs->insert("ax", x86_regclass_gpr, x86_gpr_ax, 0, 16);

        regs->insert("bl", x86_regclass_gpr, x86_gpr_bx, 0, 8);
        regs->insert("bh", x86_regclass_gpr, x86_gpr_bx, 8, 8);
        regs->insert("bx", x86_regclass_gpr, x86_gpr_bx, 0, 16);

        regs->insert("cl", x86_regclass_gpr, x86_gpr_cx, 0, 8);
        regs->insert("ch", x86_regclass_gpr, x86_gpr_cx, 8, 8);
        regs->insert("cx", x86_regclass_gpr, x86_gpr_cx, 0, 16);

        regs->insert("dl", x86_regclass_gpr, x86_gpr_dx, 0, 8);
        regs->insert("dh", x86_regclass_gpr, x86_gpr_dx, 8, 8);
        regs->insert("dx", x86_regclass_gpr, x86_gpr_dx, 0, 16);

        // 16-bit segment registers
        regs->insert("cs", x86_regclass_segment, x86_segreg_cs, 0, 16);
        regs->insert("ds", x86_regclass_segment, x86_segreg_ds, 0, 16);
        regs->insert("ss", x86_regclass_segment, x86_segreg_ss, 0, 16);
        regs->insert("es", x86_regclass_segment, x86_segreg_es, 0, 16);

        // 16-bit pointer registers
        regs->insert("sp", x86_regclass_gpr, x86_gpr_sp, 0, 16); // stack pointer
        regs->insert("spl", x86_regclass_gpr, x86_gpr_sp, 0, 8);

        regs->insert("bp", x86_regclass_gpr, x86_gpr_bp, 0, 16); // base pointer
        regs->insert("bpl", x86_regclass_gpr, x86_gpr_bp, 0, 8);

        regs->insert("ip", x86_regclass_ip, 0, 0, 16);  // instruction pointer
        regs->insert("ipl", x86_regclass_ip, 0, 0, 8);

        // Array indexing registers
        regs->insert("si", x86_regclass_gpr, x86_gpr_si, 0, 16);
        regs->insert("sil", x86_regclass_gpr, x86_gpr_si, 0, 8);

        regs->insert("di", x86_regclass_gpr, x86_gpr_di, 0, 16);
        regs->insert("dil", x86_regclass_gpr, x86_gpr_di, 0, 8);

        // Flags with official names.
        regs->insert("flags", x86_regclass_flags, x86_flags_status,  0, 16); // all flags
        regs->insert("cf",    x86_regclass_flags, x86_flags_status,  0,  1); // carry status flag
        regs->insert("pf",    x86_regclass_flags, x86_flags_status,  2,  1); // parity status flag
        regs->insert("af",    x86_regclass_flags, x86_flags_status,  4,  1); // adjust status flag
        regs->insert("zf",    x86_regclass_flags, x86_flags_status,  6,  1); // zero status flag
        regs->insert("sf",    x86_regclass_flags, x86_flags_status,  7,  1); // sign status flag
        regs->insert("tf",    x86_regclass_flags, x86_flags_status,  8,  1); // trap system flag
        regs->insert("if",    x86_regclass_flags, x86_flags_status,  9,  1); // interrupt enable system flag
        regs->insert("df",    x86_regclass_flags, x86_flags_status, 10,  1); // direction control flag
        regs->insert("of",    x86_regclass_flags, x86_flags_status, 11,  1); // overflow status flag
        regs->insert("nt",    x86_regclass_flags, x86_flags_status, 14,  1); // nested task system flag

        // Flags without names
        regs->insert("f1",    x86_regclass_flags, x86_flags_status,  1,  1);
        regs->insert("f3",    x86_regclass_flags, x86_flags_status,  3,  1);
        regs->insert("f5",    x86_regclass_flags, x86_flags_status,  5,  1);
        regs->insert("f12",   x86_regclass_flags, x86_flags_status, 12,  1);
        regs->insert("f13",   x86_regclass_flags, x86_flags_status, 13,  1);
        regs->insert("f15",   x86_regclass_flags, x86_flags_status, 15,  1);

        // Special registers
        regs->instructionPointerRegister("ip");
        regs->stackPointerRegister("sp");
        regs->stackFrameRegister("bp");
        regs->stackSegmentRegister("ss");

        registerDictionary_ = regs;
    }

    return registerDictionary_.get();
}

bool
Intel8086::matchesHeader(SgAsmGenericHeader*) const {
    return false;
}

} // namespace
} // namespace
} // namespace

#endif
