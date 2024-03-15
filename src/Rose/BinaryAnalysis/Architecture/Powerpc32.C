#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Architecture/Powerpc32.h>

#include <Rose/BinaryAnalysis/Disassembler/Powerpc.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherPowerpc.h>
#include <Rose/BinaryAnalysis/Unparser/Powerpc.h>

#include <SgAsmExecutableFileFormat.h>
#include <SgAsmGenericHeader.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

Powerpc32::Powerpc32(ByteOrder::Endianness byteOrder)
    : Powerpc(4, byteOrder) {}

Powerpc32::~Powerpc32() {}

Powerpc32::Ptr
Powerpc32::instance(ByteOrder::Endianness byteOrder) {
    return Ptr(new Powerpc32(byteOrder));
}

RegisterDictionary::Ptr
Powerpc32::registerDictionary() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    if (!registerDictionary_.isCached()) {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // WARNING: PowerPC documentation numbers register bits in reverse of their power-of-two position. ROSE numbers bits
        //          according to their power of two, so that the bit corresponding to 2^i is said to be at position i in the
        //          register.  In PowerPC documentation, the bit for 2^i is at N - (i+1) where N is the total number of bits in the
        //          register.  All PowerPC bit position numbers need to be converted to the ROSE numbering when they appear here.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        auto regs = RegisterDictionary::instance(name());

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // General purpose and floating point registers
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        for (unsigned i=0; i<32; i++) {
            regs->insert("r"+StringUtility::numberToString(i), powerpc_regclass_gpr, i, 0, 32);
            regs->insert("f"+StringUtility::numberToString(i), powerpc_regclass_fpr, i, 0, 64);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // State, status, condition, control registers
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // Machine state register
        regs->insert("msr", powerpc_regclass_msr, 0, 0, 32);

        // Floating point status and control register
        regs->insert("fpscr", powerpc_regclass_fpscr, 0, 0, 32);

        // Condition Register. This register is grouped into eight fields, where each field is 4 bits. Many PowerPC instructions
        // define the least significant bit of the instruction encoding as the Rc bit, and some instructions imply an Rc value equal
        // to 1. When Rc is equal to 1 for integer operations, the CR0 field is set to reflect the result of the instruction's
        // operation: Less than zero (LT), greater than zero (GT), equal to zero (EQ), and summary overflow (SO). When Rc is equal
        // to 1 for floating-point operations, the CR1 field is set to reflect the state of the exception status bits in the FPSCR:
        // FX, FEX, VX, and OX. Any CR field can be the target of an integer or floating-point comparison instruction. The CR0 field
        // is also set to reflect the result of a conditional store instruction (stwcx or stdcx). There is also a set of
        // instructions that can manipulate a specific CR bit, a specific CR field, or the entire CR, usually to combine several
        // conditions into a single bit for testing.
        regs->insert("cr",  powerpc_regclass_cr, 0,  0, 32);

        regs->insert("cr0", powerpc_regclass_cr, 0, 28,  4);
        regs->insert("cr0.lt", powerpc_regclass_cr, 0, 31, 1);
        regs->insert("cr0.gt", powerpc_regclass_cr, 0, 30, 1);
        regs->insert("cr0.eq", powerpc_regclass_cr, 0, 29, 1);
        regs->insert("cr0.so", powerpc_regclass_cr, 0, 28, 1);

        regs->insert("cr1", powerpc_regclass_cr, 0, 24,  4);
        regs->insert("cr1.lt", powerpc_regclass_cr, 0, 27, 1);
        regs->insert("cr1.gt", powerpc_regclass_cr, 0, 26, 1);
        regs->insert("cr1.eq", powerpc_regclass_cr, 0, 25, 1);
        regs->insert("cr1.so", powerpc_regclass_cr, 0, 24, 1);

        regs->insert("cr2", powerpc_regclass_cr, 0, 20,  4);
        regs->insert("cr2.lt", powerpc_regclass_cr, 0, 23, 1);
        regs->insert("cr2.gt", powerpc_regclass_cr, 0, 22, 1);
        regs->insert("cr2.eq", powerpc_regclass_cr, 0, 21, 1);
        regs->insert("cr2.so", powerpc_regclass_cr, 0, 20, 1);

        regs->insert("cr3", powerpc_regclass_cr, 0, 16,  4);
        regs->insert("cr3.lt", powerpc_regclass_cr, 0, 19, 1);
        regs->insert("cr3.gt", powerpc_regclass_cr, 0, 18, 1);
        regs->insert("cr3.eq", powerpc_regclass_cr, 0, 17, 1);
        regs->insert("cr3.so", powerpc_regclass_cr, 0, 16, 1);

        regs->insert("cr4", powerpc_regclass_cr, 0, 12,  4);
        regs->insert("cr4.lt", powerpc_regclass_cr, 0, 15, 1);
        regs->insert("cr4.gt", powerpc_regclass_cr, 0, 14, 1);
        regs->insert("cr4.eq", powerpc_regclass_cr, 0, 13, 1);
        regs->insert("cr4.so", powerpc_regclass_cr, 0, 12, 1);

        regs->insert("cr5", powerpc_regclass_cr, 0,  8,  4);
        regs->insert("cr5.lt", powerpc_regclass_cr, 0, 11, 1);
        regs->insert("cr5.gt", powerpc_regclass_cr, 0, 10, 1);
        regs->insert("cr5.eq", powerpc_regclass_cr, 0,  9, 1);
        regs->insert("cr5.so", powerpc_regclass_cr, 0,  8, 1);

        regs->insert("cr6", powerpc_regclass_cr, 0,  4,  4);
        regs->insert("cr6.lt", powerpc_regclass_cr, 0,  7, 1);
        regs->insert("cr6.gt", powerpc_regclass_cr, 0,  6, 1);
        regs->insert("cr6.eq", powerpc_regclass_cr, 0,  5, 1);
        regs->insert("cr6.so", powerpc_regclass_cr, 0,  4, 1);

        regs->insert("cr7", powerpc_regclass_cr, 0,  0,  4);
        regs->insert("cr7.lt", powerpc_regclass_cr, 0,  3, 1);
        regs->insert("cr7.gt", powerpc_regclass_cr, 0,  2, 1);
        regs->insert("cr7.eq", powerpc_regclass_cr, 0,  1, 1);
        regs->insert("cr7.so", powerpc_regclass_cr, 0,  0, 1);

        // The processor version register is a 32-bit read-only register that identifies the version and revision level of the
        // processor. Processor versions are assigned by the PowerPC architecture process. Revision levels are implementation
        // defined. Access to the register is privileged, so that an application program can determine the processor version only
        // with the help of an operating system function. */
        regs->insert("pvr", powerpc_regclass_pvr, 0, 0, 32);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // The instruction address register is a pseudo register. It is not directly available to the user other than through a
        // "branch and link" instruction. It is primarily used by debuggers to show the next instruction to be executed.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        regs->insert("iar", powerpc_regclass_iar, 0, 0, 32);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Special purpose registers. There are 1024 of these, some of which have special names.  We name all 1024 consistently and
        // create aliases for the special ones. This allows the disassembler to look them up generically.  Because the special names
        // appear after the generic names, a reverse lookup will return the special name.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // Generic names for them all
        for (unsigned i=0; i<1024; i++)
            regs->insert("spr"+StringUtility::numberToString(i), powerpc_regclass_spr, i, 0, 32);

        // The link register contains the address to return to at the end of a function call.  Each branch instruction encoding has
        // an LK bit. If the LK bit is 1, the branch instruction moves the program counter to the link register. Also, the
        // conditional branch instruction BCLR branches to the value in the link register.
        regs->insert("lr", powerpc_regclass_spr, powerpc_spr_lr, 0, 32);

        // The fixed-point exception register contains carry and overflow information from integer arithmetic operations. It also
        // contains carry input to certain integer arithmetic operations and the number of bytes to transfer during load and store
        // string instructions, lswx and stswx.
        regs->insert("xer", powerpc_regclass_spr, powerpc_spr_xer, 0, 32);
        regs->insert("xer_so", powerpc_regclass_spr, powerpc_spr_xer, 31, 1); // summary overflow
        regs->insert("xer_ov", powerpc_regclass_spr, powerpc_spr_xer, 30, 1); // overflow
        regs->insert("xer_ca", powerpc_regclass_spr, powerpc_spr_xer, 29, 1); // carry

        // The count register contains a loop counter that is decremented on certain branch operations. Also, the conditional branch
        // instruction bcctr branches to the value in the CTR. */
        regs->insert("ctr", powerpc_regclass_spr, powerpc_spr_ctr, 0, 32);

        // Other special purpose registers.
        regs->insert("dsisr", powerpc_regclass_spr, powerpc_spr_dsisr, 0, 32);
        regs->insert("dar", powerpc_regclass_spr, powerpc_spr_dar, 0, 32);
        regs->insert("dec", powerpc_regclass_spr, powerpc_spr_dec, 0, 32);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Time base registers. There are 1024 of these, some of which have special names. We name all 1024 consistently and create
        // aliases for the special ones. This allows the disassembler to look them up generically.  Because the special names appear
        // after the generic names, a reverse lookup will return the special name.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        for (unsigned i=0; i<1024; i++)
            regs->insert("tbr"+StringUtility::numberToString(i), powerpc_regclass_tbr, i, 0, 32);

        regs->insert("tbl", powerpc_regclass_tbr, powerpc_tbr_tbl, 0, 32);      /* time base lower */
        regs->insert("tbu", powerpc_regclass_tbr, powerpc_tbr_tbu, 0, 32);      /* time base upper */

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Special registers
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        regs->instructionPointerRegister("iar");
        regs->stackPointerRegister("r1");
        regs->stackFrameRegister("r31");
        regs->callReturnRegister("lr");

        registerDictionary_ = regs;
    }

    return registerDictionary_.get();
}

bool
Powerpc32::matchesHeader(SgAsmGenericHeader *header) const {
    ASSERT_not_null(header);
    const SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();
    SgAsmGenericFormat *fmt = header->get_executableFormat();
    ASSERT_not_null(fmt);
    return SgAsmExecutableFileFormat::ISA_PowerPC == isa && fmt->get_sex() == byteOrder();
}

} // namespace
} // namespace
} // namespace

#endif
