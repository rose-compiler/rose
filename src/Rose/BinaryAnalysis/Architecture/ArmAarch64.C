#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_AARCH64
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/ArmAarch64.h>

#include <Rose/BinaryAnalysis/Disassembler/Aarch64.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherAarch64.h>
#include <Rose/BinaryAnalysis/Unparser/Aarch64.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

ArmAarch64::ArmAarch64()
    : Base("arm-a64", 8, ByteOrder::ORDER_LSB) {}

ArmAarch64::~ArmAarch64() {}

ArmAarch64::Ptr
ArmAarch64::instance() {
    return Ptr(new ArmAarch64);
}

RegisterDictionary::Ptr
ArmAarch64::registerDictionary() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    if (!registerDictionary_.isCached()) {
        // References:
        //   [1] "Arm Instruction Set Version 1.0 Reference Guide" copyright 2018 Arm Limited.
        //   [2] "ARM Cortex-A Series Version 1.0 Programmer's Guide for ARMv8-A" copyright 2015 ARM.
        auto regs = RegisterDictionary::instance(name());

        // 31 64-bit general-purpose registers "x0" through "x30". The names "w0" through "w30" refer to the low-order 32 bits
        // of the corresponding "x" register in that read operations read only the low-order 32 bits and write operations write
        // to all 64 bits (always clearing the high-order 32 bits). [1, p. A3-72]
        //
        for (unsigned i = 0; i < 31; ++i) {
            regs->insert("x" + boost::lexical_cast<std::string>(i), aarch64_regclass_gpr, i, 0, 64);
            regs->insert("w" + boost::lexical_cast<std::string>(i), aarch64_regclass_gpr, i, 0, 32);
        }

        // Zero register. Also called "register 31" in the documentation ("sp", though distinct from the zero register, is also
        // called "register 31" in the documentation. [1, pp. A3-72, A3-76]
        regs->insert("xzr", aarch64_regclass_gpr, 31, 0, 64);
        regs->insert("wzr", aarch64_regclass_gpr, 31, 0, 32);

        // Another name for "x30" is "lr", the "procedure link register". [1. pp. A3-74, A3-76]
        regs->insert("lr", aarch64_regclass_gpr, 30, 0, 64);

        // Another name for "x29" is "fp", the frame pointer. [2, p 4-2]
        regs->insert("fp", aarch64_regclass_gpr, 29, 0, 64);

        // One program counter. Not a general purpose register. [1. p. A3-78]
        regs->insert("pc", aarch64_regclass_pc, 0, 0, 64);

        // Four stack pointer registers. [1. p. A3-72] These store the exception return address per exception level. The
        // "sp_el0" is an alias for the stack pointer "sp". [1. p. A3-75] The "sp" register is also referred to as "register 31"
        // in the documentation [1. p. A3-72] (along with "xzr" and "wzr", which is clearly a distinct register) although this
        // register is also claimed to be not general-purpose like all the other "x" and "w" registers. [1. p. A3-75].
        regs->insert("sp_el0", aarch64_regclass_sp, 0, 0, 64); // "sp_el0" is an alias for "sp" [1. p. A3-75]
        regs->insert("sp_el1", aarch64_regclass_sp, 1, 0, 64);
        regs->insert("sp_el2", aarch64_regclass_sp, 2, 0, 64);
        regs->insert("sp_el3", aarch64_regclass_sp, 3, 0, 64);
        regs->insert("sp",     aarch64_regclass_sp, 0, 0, 64);
        regs->insert("wsp",    aarch64_regclass_sp, 0, 0, 32); // [2, p. 4-3]

        // Three saved program status registers. [1. p. A3-72, A3-82]. These store the processor state fields per exception
        // level so they can be restored when the exception handler returns. The processor state registers have various fields
        // defined in [2, p. 4-5]. Unlike AArch32, there is no processor state register that's accessible directly from assembly
        // instructions, but ROSE still must define it because it does exist internally. ROSE gives it the minor number zero and
        // the base name "cpsr" (the same name as from AArch32).
        for (unsigned i = 0; i < 4; ++i) {
            std::string base = 0==i ? "cpsr" : "spsr_el" + boost::lexical_cast<std::string>(i);
            regs->insert(base, aarch64_regclass_system, aarch64_system_spsr+i, 0, 32);
            regs->insert(base+".n",  aarch64_regclass_system, aarch64_system_spsr+i, 31, 1);     // negative condition flag
            regs->insert(base+".z",  aarch64_regclass_system, aarch64_system_spsr+i, 30, 1);     // zero condition flag
            regs->insert(base+".c",  aarch64_regclass_system, aarch64_system_spsr+i, 29, 1);     // carry condition flag
            regs->insert(base+".v",  aarch64_regclass_system, aarch64_system_spsr+i, 28, 1);     // overflow condition flag
            regs->insert(base+".ss", aarch64_regclass_system, aarch64_system_spsr+i, 21, 1);     // software step bit
            regs->insert(base+".il", aarch64_regclass_system, aarch64_system_spsr+i, 20, 1);     // illegal excecution bit
            regs->insert(base+".d",  aarch64_regclass_system, aarch64_system_spsr+i, 9, 1);      // debug mask bit
            regs->insert(base+".a",  aarch64_regclass_system, aarch64_system_spsr+i, 8, 1);      // "SError" mask bit
            regs->insert(base+".i",  aarch64_regclass_system, aarch64_system_spsr+i, 7, 1);      // IRQ mask bit
            regs->insert(base+".f",  aarch64_regclass_system, aarch64_system_spsr+i, 6, 1);      // FIQ mask bit
            regs->insert(base+".m",  aarch64_regclass_system, aarch64_system_spsr+i, 0, 4);      // source of exception
        }

        // System registers [2, p. 4-7]
        for (unsigned i = 0; i < 4; ++i) {
            std::string n = boost::lexical_cast<std::string>(i);
            // FPCR?
            // FPSR?
            if (i == 0) {
                regs->insert("cntfrq_el"+n,    aarch64_regclass_system, aarch64_system_cntfrq+i,    0, 64); // ctr-timer frequency reg
                regs->insert("cntpct_el"+n,    aarch64_regclass_system, aarch64_system_cntpct+i,    0, 64); // ctr-timer phys count reg
                regs->insert("cntp_cval_el"+n, aarch64_regclass_system, aarch64_system_cntp_cval+i, 0, 64); // ctr-timer phys timer cmp
                regs->insert("cntp_ctl_el"+n,  aarch64_regclass_system, aarch64_system_cntp_ctl+i,  0, 64); // ctr-timer phys control reg
                regs->insert("ctr_el"+n,       aarch64_regclass_system, aarch64_system_ctr+i,       0, 64); // cache type register
                regs->insert("dczid_el"+n,     aarch64_regclass_system, aarch64_system_dczid+i,     0, 64); // data cache zero ID reg
                regs->insert("tpidrr0_el"+n,   aarch64_regclass_system, aarch64_system_tpidrr0+i,   0, 64); // user read-only thread ID reg
            }
            if (i == 1) {
                regs->insert("ccsidr_el"+n,  aarch64_regclass_system, aarch64_system_ccsidr+i,  0, 64); // current cache size ID register
                regs->insert("cntkctl_el"+n, aarch64_regclass_system, aarch64_system_cntkctl+i, 0, 64); // counter-timer kernel control reg
                regs->insert("cpacr_el"+n,   aarch64_regclass_system, aarch64_system_cpacr+i,   0, 64); // coprocessor access control reg
                regs->insert("csselr_el"+n,  aarch64_regclass_system, aarch64_system_csselr+i,  0, 64); // cache size selection register
                regs->insert("midr_el"+n,    aarch64_regclass_system, aarch64_system_midr+i,    0, 64); // main ID register
                regs->insert("mpidr_el"+n,   aarch64_regclass_system, aarch64_system_mpidr+i,   0, 64); // multiprocessor affinity reg
                regs->insert("ttbr1_el"+n,   aarch64_regclass_system, aarch64_system_ttbr1+i,   0, 64); // translation table base reg 1
            }
            if (i == 2) {
                regs->insert("hcr_el"+n,   aarch64_regclass_system, aarch64_system_hcr+i,   0, 64); // hypervisor configuration register
                regs->insert("vtcr_el"+n,  aarch64_regclass_system, aarch64_system_vtcr+i,  0, 64); // virtualization translation ctr reg
                regs->insert("vttbr_el"+n, aarch64_regclass_system, aarch64_system_vttbr+i, 0, 64); // virt translation table base reg
            }
            if (i == 3) {
                regs->insert("scr_el"+n,   aarch64_regclass_system, aarch64_system_scr+i,   0, 64); // secure configuration register

            }
            if (true) {
                regs->insert("sctlr_el"+n, aarch64_regclass_system, aarch64_system_sctlr+i, 0, 32); // system control register [2, p. 4-10]
                {
                    // Not all bits are available above EL1, but ROSE defines them across the board anyway. Also the
                    // documentation is confusing: it says in one place [2, p. 4-9] that this register is defined for EL0
                    // through EL3, but then in another place [2, p. 4-10] that the bit fields are only defined for EL1 through
                    // EL3.
                    regs->insert("sctlr_el"+n+".uci",     aarch64_regclass_system, aarch64_system_sctlr+i, 26, 1); // enable EL0 access
                    regs->insert("sctlr_el"+n+".ee",      aarch64_regclass_system, aarch64_system_sctlr+i, 25, 1); // exception endianness
                    regs->insert("sctlr_el"+n+".eoe",     aarch64_regclass_system, aarch64_system_sctlr+i, 24, 1); // endianness of explicit data accesses at EL0
                    regs->insert("sctlr_el"+n+".wxn",     aarch64_regclass_system, aarch64_system_sctlr+i, 19, 1); // write permission implies execute never
                    regs->insert("sctlr_el"+n+".ntwe",    aarch64_regclass_system, aarch64_system_sctlr+i, 18, 1); // not trap WFE
                    regs->insert("sctlr_el"+n+".ntwi",    aarch64_regclass_system, aarch64_system_sctlr+i, 16, 1); // not trap WFI
                    regs->insert("sctlr_el"+n+".uct",     aarch64_regclass_system, aarch64_system_sctlr+i, 15, 1); // enable EL0 access to CTR_EL0 register
                    regs->insert("sctlr_el"+n+".dze",     aarch64_regclass_system, aarch64_system_sctlr+i, 14, 1); // access to DC ZVA instruction at EL0
                    regs->insert("sctlr_el"+n+".uma",     aarch64_regclass_system, aarch64_system_sctlr+i,  9, 1); // user mask access
                    regs->insert("sctlr_el"+n+".sed",     aarch64_regclass_system, aarch64_system_sctlr+i,  8, 1); // SETEND disable
                    regs->insert("sctlr_el"+n+".itd",     aarch64_regclass_system, aarch64_system_sctlr+i,  7, 1); // IT disable
                    regs->insert("sctlr_el"+n+".cp15ben", aarch64_regclass_system, aarch64_system_sctlr+i,  5, 1); // CP15 barrier enable
                    regs->insert("sctlr_el"+n+".sa0",     aarch64_regclass_system, aarch64_system_sctlr+i,  4, 1); // stack alignment check enable for EL0
                    regs->insert("sctlr_el"+n+".sa",      aarch64_regclass_system, aarch64_system_sctlr+i,  3, 1); // stack alignment check enable
                    regs->insert("sctlr_el"+n+".c",       aarch64_regclass_system, aarch64_system_sctlr+i,  2, 1); // data enable cache
                    regs->insert("sctlr_el"+n+".a",       aarch64_regclass_system, aarch64_system_sctlr+i,  1, 1); // alignment check enable bit
                    regs->insert("sctlr_el"+n+".m",       aarch64_regclass_system, aarch64_system_sctlr+i,  0, 1); // enable the MMU
                }
                regs->insert("tpidr_el"+n, aarch64_regclass_system, aarch64_system_tpidr+i, 0, 64); // user read/write thread ID register
            }
            if (i >= 1) {
                regs->insert("actlr_el"+n, aarch64_regclass_system, aarch64_system_actlr+i, 0, 64); // auxiliary control registers
                regs->insert("clidr_el"+n, aarch64_regclass_system, aarch64_system_clidr+i, 0, 64); // cache level ID registers
                regs->insert("elr_el"+n,   aarch64_regclass_system, aarch64_system_elr+i,   0, 64); // exception link registers
                regs->insert("esr_el"+n,   aarch64_regclass_system, aarch64_system_esr+i,   0, 64); // exception syndrome registers
                regs->insert("far_el"+n,   aarch64_regclass_system, aarch64_system_far+i,   0, 64); // fault address register
                regs->insert("mair_el"+n,  aarch64_regclass_system, aarch64_system_mair+i,  0, 64); // memory attribute indirection register
                //gs->insert("spsr_el"+n,  aarch64_regclass_system, aarch64_system_spsr+i,  0, 64); // saved program status register (see above)
                regs->insert("tcr_el"+n,   aarch64_regclass_system, aarch64_system_tcr+i,   0, 64); // translation control register
                regs->insert("ttbr0_el"+n, aarch64_regclass_system, aarch64_system_ttbr0+i, 0, 64); // translation table base register 0
                regs->insert("vbar_el"+n,  aarch64_regclass_system, aarch64_system_vbar+i,  0, 64); // vector based address register
            }
        }


        // "Advanced SIMD" registers, which are also the floating-point registers. [1. p. A3-77]
        // also known as "NEON and floating-point registers" [2, p. 4-16]
        for (unsigned i = 0; i < 32; ++i) {
            // Parts of vector registers
            for (size_t j = 0; j < 16; ++j) {
                regs->insert("v" + boost::lexical_cast<std::string>(i) + ".b[" + boost::lexical_cast<std::string>(j) + "]",
                             aarch64_regclass_ext, i, 8 * j, 8);
            }
            for (size_t j = 0; j < 8; ++j) {
                regs->insert("v" + boost::lexical_cast<std::string>(i) + ".h[" + boost::lexical_cast<std::string>(j) + "]",
                             aarch64_regclass_ext, i, 16 * j, 16);
            }
            for (size_t j = 0; j < 4; ++j) {
                regs->insert("v" + boost::lexical_cast<std::string>(i) + ".s[" + boost::lexical_cast<std::string>(j) + "]",
                             aarch64_regclass_ext, i, 32 * j, 32);
            }
            for (size_t j = 0; j < 2; ++j) {
                regs->insert("v" + boost::lexical_cast<std::string>(i) + ".d[" + boost::lexical_cast<std::string>(j) + "]",
                             aarch64_regclass_ext, i, 64 * j, 64);
            }

            // Floating-point registers. 32 registers having names that access various parts of each register.
            regs->insert("v" + boost::lexical_cast<std::string>(i), aarch64_regclass_ext, i, 0, 128); // "quadword"
            regs->insert("d" + boost::lexical_cast<std::string>(i), aarch64_regclass_ext, i, 0, 64); // "doubleword"
            regs->insert("s" + boost::lexical_cast<std::string>(i), aarch64_regclass_ext, i, 0, 32); // "word"
            regs->insert("h" + boost::lexical_cast<std::string>(i), aarch64_regclass_ext, i, 0, 16); // "halfword"

            // Scalar registers are the same as the floating-point registers but use the name "q" to refer to the whole
            // register. It also uses "b" to refer to the least significant bit. Since the "q" and "v" registers are actually
            // the same register, and since ROSE identifies registers internally by their locations (RegisterDescriptor) rather
            // than their names, various parts of ROSE will report "q" instead of "v".
            regs->insert("q" + boost::lexical_cast<std::string>(i), aarch64_regclass_ext, i, 0, 128); // SIMD
            regs->insert("b" + boost::lexical_cast<std::string>(i), aarch64_regclass_ext, i, 0, 8); // SIMD
        }

        // Conditional execution registers. [1. p. A3-79]
        regs->insert("nzcv", aarch64_regclass_cc, 0, 0, 4);

        // Special registers
        regs->instructionPointerRegister("pc");
        regs->stackPointerRegister("sp");
        regs->stackFrameRegister("fp");
        regs->callReturnRegister("lr");

        registerDictionary_ = regs;
    }

    return registerDictionary_.get();
}

bool
ArmAarch64::matchesHeader(SgAsmGenericHeader *header) const {
    ASSERT_not_null(header);
    const SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();
    return (isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_ARM_Family &&
        header->get_executableFormat()->get_wordSize() == 8;
}

Disassembler::Base::Ptr
ArmAarch64::newInstructionDecoder() const {
    return Disassembler::Aarch64::instance(shared_from_this());
}

Unparser::Base::Ptr
ArmAarch64::newUnparser() const {
    return Unparser::Aarch64::instance(shared_from_this());
}

InstructionSemantics::BaseSemantics::DispatcherPtr
ArmAarch64::newInstructionDispatcher(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr &ops) const {
    return InstructionSemantics::DispatcherAarch64::instance(shared_from_this(), ops);
}

} // namespace
} // namespace
} // namespace

#endif
