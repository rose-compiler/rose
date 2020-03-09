#ifndef ROSE_DispatcherX86_H
#define ROSE_DispatcherX86_H
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include <BaseSemantics2.h>
#include <Registers.h>

#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/split_member.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Dispatcher
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Shared-ownership pointer to an x86 instruction dispatcher. See @ref heap_object_shared_ownership. */
typedef boost::shared_ptr<class DispatcherX86> DispatcherX86Ptr;

class DispatcherX86: public BaseSemantics::Dispatcher {
public:
    typedef BaseSemantics::Dispatcher Super;

protected:
    X86InstructionSize processorMode_;

public:
    /** Cached register. This register is cached so that there are not so many calls to Dispatcher::findRegister(). The
     *  register descriptor is updated only when the register dictionary is changed (see set_register_dictionary()).
     *
     *  Register names like REG_anyAX have sizes that depend on the architecture: 16 bits for 16-bit architectures, 32 bits for
     *  32-bit architectures, etc.  The other register names have specific sizes--such as REG_EAX being 32 bits--and are
     *  defined only on architectures that support them.
     *
     * @{ */
    RegisterDescriptor REG_anyAX, REG_anyBX, REG_anyCX, REG_anyDX;
    RegisterDescriptor REG_RAX,   REG_RBX,   REG_RCX,   REG_RDX;
    RegisterDescriptor REG_EAX,   REG_EBX,   REG_ECX,   REG_EDX;
    RegisterDescriptor REG_AX,    REG_BX,    REG_CX,    REG_DX;
    RegisterDescriptor REG_AL,    REG_BL,    REG_CL,    REG_DL;
    RegisterDescriptor REG_AH,    REG_BH,    REG_CH,    REG_DH;

    RegisterDescriptor REG_R8,    REG_R9,    REG_R10,   REG_R11;
    RegisterDescriptor REG_R12,   REG_R13,   REG_R14,   REG_R15;

    RegisterDescriptor REG_anyDI, REG_anySI, REG_anySP, REG_anyBP, REG_anyIP;
    RegisterDescriptor REG_RDI,   REG_RSI,   REG_RSP,   REG_RBP,   REG_RIP;
    RegisterDescriptor REG_EDI,   REG_ESI,   REG_ESP,   REG_EBP,   REG_EIP;
    RegisterDescriptor REG_DI,    REG_SI,    REG_SP,    REG_BP,    REG_IP;

    RegisterDescriptor REG_CS, REG_DS, REG_ES, REG_SS, REG_FS, REG_GS;

    RegisterDescriptor REG_anyFLAGS, REG_RFLAGS, REG_EFLAGS, REG_FLAGS;
    RegisterDescriptor REG_AF, REG_CF, REG_DF, REG_OF, REG_PF, REG_SF, REG_TF, REG_ZF;

    RegisterDescriptor REG_ST0, REG_FPSTATUS, REG_FPSTATUS_TOP, REG_FPCTL, REG_MXCSR;
    /** @}*/

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void save(S &s, const unsigned /*version*/) const {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Super);
        s & BOOST_SERIALIZATION_NVP(processorMode_);
    }
    
    template<class S>
    void load(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Super);
        s & BOOST_SERIALIZATION_NVP(processorMode_);
        regcache_init();
        iproc_init();
        memory_init();
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER();
#endif
    
protected:
    // Prototypical constructor
    DispatcherX86()
        : BaseSemantics::Dispatcher(32, SgAsmX86Instruction::registersForInstructionSize(x86_insnsize_32)),
          processorMode_(x86_insnsize_32) {}

    // Prototypical constructor
    DispatcherX86(size_t addrWidth, const RegisterDictionary *regs/*=NULL*/)
        : BaseSemantics::Dispatcher(addrWidth, regs ? regs : SgAsmX86Instruction::registersForWidth(addrWidth)),
          processorMode_(SgAsmX86Instruction::instructionSizeForWidth(addrWidth)) {}

    // Normal constructor
    DispatcherX86(const BaseSemantics::RiscOperatorsPtr &ops, size_t addrWidth, const RegisterDictionary *regs)
        : BaseSemantics::Dispatcher(ops, addrWidth, regs ? regs : SgAsmX86Instruction::registersForWidth(addrWidth)),
          processorMode_(SgAsmX86Instruction::instructionSizeForWidth(addrWidth)) {
        regcache_init();
        iproc_init();
        memory_init();
    }

public:
    /** Loads the iproc table with instruction processing functors. This normally happens from the constructor. */
    void iproc_init();

    /** Load the cached register descriptors.  This happens at construction and on set_register_dictionary() calls. */
    void regcache_init();

    /** Make sure memory is set up correctly. For instance, byte order should be little endian. */
    void memory_init();

public:
    /** Construct a prototypical dispatcher.  The only thing this dispatcher can be used for is to create another dispatcher
     *  with the virtual @ref create method. */
    static DispatcherX86Ptr instance() {
        return DispatcherX86Ptr(new DispatcherX86);
    }

    /** Construct a prototyipcal dispatcher. Construct a prototypical dispatcher with a specified address size. The only thing
     * this dispatcher can be used for is to create another dispatcher with the virtual @ref create method. */
    static DispatcherX86Ptr instance(size_t addrWidth, const RegisterDictionary *regs=NULL) {
        return DispatcherX86Ptr(new DispatcherX86(addrWidth, regs));
    }

    /** Constructor. */
    static DispatcherX86Ptr instance(const BaseSemantics::RiscOperatorsPtr &ops, size_t addrWidth,
                                     const RegisterDictionary *regs=NULL) {
        return DispatcherX86Ptr(new DispatcherX86(ops, addrWidth, regs));
    }

    /** Virtual constructor. */
    virtual BaseSemantics::DispatcherPtr create(const BaseSemantics::RiscOperatorsPtr &ops, size_t addrWidth=0,
                                                const RegisterDictionary *regs=NULL) const ROSE_OVERRIDE {
        if (0==addrWidth)
            addrWidth = addressWidth();
        if (NULL==regs)
            regs = get_register_dictionary();
        return instance(ops, addrWidth, regs);
    }

    /** Dynamic cast to a DispatcherX86Ptr with assertion. */
    static DispatcherX86Ptr promote(const BaseSemantics::DispatcherPtr &d) {
        DispatcherX86Ptr retval = boost::dynamic_pointer_cast<DispatcherX86>(d);
        assert(retval!=NULL);
        return retval;
    }

    /** CPU mode of operation.
     *
     * @{ */
    X86InstructionSize processorMode() const { return processorMode_; }
    void processorMode(X86InstructionSize m) { processorMode_ = m; }
    /** @} */

    virtual void set_register_dictionary(const RegisterDictionary *regdict) ROSE_OVERRIDE;

    /** Get list of common registers. Returns a list of non-overlapping registers composed of the largest registers except
     *  using individual flags for the fields of the FLAGS/EFLAGS register. */
    virtual RegisterDictionary::RegisterDescriptors get_usual_registers() const;

    virtual RegisterDescriptor instructionPointerRegister() const ROSE_OVERRIDE;
    virtual RegisterDescriptor stackPointerRegister() const ROSE_OVERRIDE;
    virtual RegisterDescriptor callReturnRegister() const ROSE_OVERRIDE;

    virtual int iproc_key(SgAsmInstruction *insn_) const ROSE_OVERRIDE {
        SgAsmX86Instruction *insn = isSgAsmX86Instruction(insn_);
        assert(insn!=NULL);
        return insn->get_kind();
    }

    virtual void write(SgAsmExpression *e, const BaseSemantics::SValuePtr &value, size_t addr_nbits=0) ROSE_OVERRIDE;

    enum AccessMode { READ_REGISTER, PEEK_REGISTER };

    /** Architecture-specific read from register.
     *
     *  Similar to RiscOperators::readRegister, but might do additional architecture-specific things. */
    virtual BaseSemantics::SValuePtr readRegister(RegisterDescriptor, AccessMode mode = READ_REGISTER);

    /** Architecture-specific write to register.
     *
     *  Similar to RiscOperators::writeRegister, but might do additional architecture-specific things. For instance, writing to
     *  a 32-bit GPR such as "eax" on x86-64 will write zeros to the upper half of "rax". */
    virtual void writeRegister(RegisterDescriptor, const BaseSemantics::SValuePtr &result);

    /** Set parity, sign, and zero flags appropriate for result value. */
    virtual void setFlagsForResult(const BaseSemantics::SValuePtr &result);

    /** Conditionally set parity, sign, and zero flags appropriate for result value. */
    virtual void setFlagsForResult(const BaseSemantics::SValuePtr &result, const BaseSemantics::SValuePtr &cond);

    /** Returns true if byte @p v has an even number of bits set; false for an odd number */
    virtual BaseSemantics::SValuePtr parity(const BaseSemantics::SValuePtr &v);

    /** Conditionally invert the bits of @p value.  The bits are inverted if @p maybe is true, otherwise @p value is returned. */
    virtual BaseSemantics::SValuePtr invertMaybe(const BaseSemantics::SValuePtr &value, bool maybe);

    /** Determines whether @p value is greater than or equal to ten. */
    virtual BaseSemantics::SValuePtr greaterOrEqualToTen(const BaseSemantics::SValuePtr &value);

    /** Return a Boolean for the specified flag combo for an instruction. */
    virtual BaseSemantics::SValuePtr flagsCombo(X86InstructionKind k);

    /** Enters a loop for a REP-, REPE-, or REPNE-prefixed instruction.  The return value is true if ECX is non-zero or the
     *  instruction doesn't have repeat prefix, and false otherwise. Use this in conjunction with repLeave(). */
    virtual BaseSemantics::SValuePtr repEnter(X86RepeatPrefix);

    /** Leave a loop for a REP-, REPE-, or REPNE-prefixed instruction.  The @p in_loop argument is the Boolean that indicates
     *  whether we just executed the instruction, and is usually the return value from the previous repEnter() call. If @p
     *  in_loop is false then this function is a no-op. Otherwise, the ECX register is decremented and, if it is non-zero and
     *  the repeat condition (true, equal, or not-equal) is satisified, then the EIP register is reset to the specified
     *  instruction address causing the instruction to be repeated. Use this in conjunction with repEnter().  The REP and REPE
     *  prefixes are shared, both represented by x86_repeat_repe, and we use the honorZeroFlag to decide whether the prefix is
     *  REP (false) or REPE (true). */
    virtual void repLeave(X86RepeatPrefix, const BaseSemantics::SValuePtr &in_loop, rose_addr_t insn_va, bool honorZeroFlag);

    /** Adds two values and adjusts flags.  This method can be used for subtraction if @p b is two's complement and @p
     *  invertCarries is set.  If @p cond is supplied, then the addition and flag adjustments are conditional.
     * @{ */
    virtual BaseSemantics::SValuePtr doAddOperation(BaseSemantics::SValuePtr a, BaseSemantics::SValuePtr b,
                                                    bool invertCarries, const BaseSemantics::SValuePtr &carryIn);
    virtual BaseSemantics::SValuePtr doAddOperation(BaseSemantics::SValuePtr a, BaseSemantics::SValuePtr b,
                                                    bool invertCarries, const BaseSemantics::SValuePtr &carryIn,
                                                    const BaseSemantics::SValuePtr &cond);
    /** @}*/

    /** Increments or decrements a value and adjusts flags.  If @p dec is set then the value is decremented instead of
     *  incremented. If @p setCarry is set then the CF flag is affected. */
    virtual BaseSemantics::SValuePtr doIncOperation(const BaseSemantics::SValuePtr &a, bool dec, bool setCarry);

    /** Implements the RCL, RCR, ROL, and ROR instructions for various operand sizes.  The rotate amount is always 8 bits wide
     * in the instruction, but the semantics mask off all but the low-order bits, keeping 5 bits in 32-bit mode and 6 bits in
     * 64-bit mode (indicated by the rotateSignificantBits argument). */
    virtual BaseSemantics::SValuePtr doRotateOperation(X86InstructionKind kind,
                                                       const BaseSemantics::SValuePtr &operand,
                                                       const BaseSemantics::SValuePtr &total_rotate,
                                                       size_t rotateSignificantBits);

    /** Implements the SHR, SAR, SHL, SAL, SHRD, and SHLD instructions for various operand sizes.  The shift amount is always 8
     *  bits wide in the instruction, but the semantics mask off all but the low-order bits, keeping 5 bits in 32-bit mode and
     *  7 bits in 64-bit mode (indicated by the @p shiftSignificantBits argument).  The semantics of SHL and SAL are
     *  identical (in fact, ROSE doesn't even define x86_sal). The @p source_bits argument contains the bits to be shifted into
     *  the result and is used only for SHRD and SHLD instructions. */
    virtual BaseSemantics::SValuePtr doShiftOperation(X86InstructionKind kind,
                                                      const BaseSemantics::SValuePtr &operand,
                                                      const BaseSemantics::SValuePtr &source_bits,
                                                      const BaseSemantics::SValuePtr &total_shift,
                                                      size_t shiftSignificantBits);

    /** Push floating-point value onto FP stack.  Pushes the specified value onto the floating-point circular stack.  The
     * current top-of-stack is the REG_ST register, but whose minor number is the value stored in the REG_ST_TOP register.  The
     * value in REG_ST_TOP (which must be concrete) is decremented modulo eight before being used. */
    virtual void pushFloatingPoint(const BaseSemantics::SValuePtr &valueToPush);

    /** Read a value from the floating point stack. */
    virtual BaseSemantics::SValuePtr readFloatingPointStack(size_t position);

    /** Pop the top item from the floating point stack. */
    virtual void popFloatingPoint();

    /** Extend or truncate value to propert memory address width. */
    virtual BaseSemantics::SValuePtr fixMemoryAddress(const BaseSemantics::SValuePtr &address) const;

    /** Convert a signed value to a narrower unsigned type.  Returns the truncated source value except when the value cannot
     *  be represented in the narrower type, in which case the closest unsigned value is returned (zero or all bits set). */
    virtual BaseSemantics::SValuePtr saturateSignedToUnsigned(const BaseSemantics::SValuePtr&, size_t narrowerWidth);

    /** Convert a signed value to a narrower signed type.  Returns the truncated source value except when the value cannot be
     * represented by the narrower type, in which case the closest signed value is returned. The closest signed value is either
     * 0b1000...0 (minimum signed value) or 0b0111...1 (maximum signed value). */
    virtual BaseSemantics::SValuePtr saturateSignedToSigned(const BaseSemantics::SValuePtr&, size_t narrowerWidth);

    /** Convert an unsigned value to a narrower unsigned type.  Returns the truncated source value except when the value cannot
     * be represented by the narrower type, in which case the closest unsigned value is returned. */
    virtual BaseSemantics::SValuePtr saturateUnsignedToUnsigned(const BaseSemantics::SValuePtr&, size_t narrowerWidth);
};
        
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Instruction processors
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace X86 {

/** Base class for all x86 instruction processors.
 *
 *  This class provides single-letter names for some types that are used in all instructions: D, I, A, and Ops for the
 *  dispatcher raw pointer, instruction pointer, argument list pointer, and RISC operators raw pointer.  It also takes care
 *  of advancing the instruction pointer prior to handing the instruction to the subclass, which by the way is done via
 *  @ref p method (short for "process").  See examples in DispatcherX86.C -- there are <em>lots</em> of them. */
class InsnProcessor: public BaseSemantics::InsnProcessor {
public:
    typedef DispatcherX86 *D;
    typedef BaseSemantics::RiscOperators *Ops;
    typedef SgAsmX86Instruction *I;
    typedef const SgAsmExpressionPtrList &A;
    virtual void p(D, Ops, I, A) = 0;
    virtual void process(const BaseSemantics::DispatcherPtr&, SgAsmInstruction*) ROSE_OVERRIDE;
    virtual void assert_args(I insn, A args, size_t nargs);
    void check_arg_width(D d, I insn, A args);
};

} // namespace

} // namespace
} // namespace
} // namespace

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
BOOST_CLASS_EXPORT_KEY(Rose::BinaryAnalysis::InstructionSemantics2::DispatcherX86);
#endif

#endif
#endif
