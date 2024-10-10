#ifndef ROSE_BinaryAnalysis_InstructionSemantics_DispatcherAarch32_H
#define ROSE_BinaryAnalysis_InstructionSemantics_DispatcherAarch32_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_AARCH32

#include <Rose/BinaryAnalysis/BasicTypes.h>
#include <Rose/BinaryAnalysis/InstructionEnumsAarch32.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics.h>

#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/split_member.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {

/** Shared-ownership pointer to an A32/T32 instruction dispatcher. */
using DispatcherAarch32Ptr = boost::shared_ptr<class DispatcherAarch32>;

class DispatcherAarch32: public BaseSemantics::Dispatcher {
public:
    /** Base type. */
    using Super = BaseSemantics::Dispatcher;

    /** Shared-ownership pointer. */
    using Ptr = DispatcherAarch32Ptr;

public:
    /** Cached register.
     *
     *  This register is cached so that there are not so amny calls to @ref BaseSemantics::Dispatcher::findRegister. Changing
     *  the @ref BaseSemantics::Dispatcher::registerDictionary property updates all entries of this cache.
     *
     * @{ */
    RegisterDescriptor REG_PC, REG_SP, REG_LR;
    RegisterDescriptor REG_PSTATE_N, REG_PSTATE_Z, REG_PSTATE_C, REG_PSTATE_V, REG_PSTATE_T; // parts of CPSR
    RegisterDescriptor REG_PSTATE_E, REG_PSTATE_Q, REG_PSTATE_GE;
    RegisterDescriptor REG_PSTATE_NZCV;                 // the CPSR N, Z, C, and V bits
    RegisterDescriptor REG_SPSR, REG_CPSR;
    RegisterDescriptor REG_DTRTX;                       // debug registers
    RegisterDescriptor REG_UNKNOWN;                     // special ROSE register
    /** @} */

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void save(S &s, const unsigned /*version*/) const {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Super);
    }

    template<class S>
    void load(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Super);
        initializeRegisterDescriptors();
        initializeInsnDispatchTable();
        initializeMemory();
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER();
#endif

private:
    DispatcherAarch32();                                // used only by boost::serialization

protected:
    // prototypical constructor
    DispatcherAarch32(const Architecture::BaseConstPtr&);

    DispatcherAarch32(const Architecture::BaseConstPtr&, const BaseSemantics::RiscOperatorsPtr&);

public:
    ~DispatcherAarch32();

    /** Construct a prototypical dispatcher.
     *
     *  The only thing this dispatcher can be used for is to create another dispatcher with the virtual @ref create method. */
    static DispatcherAarch32Ptr instance(const Architecture::BaseConstPtr&);

    /** Allocating constructor. */
    static DispatcherAarch32Ptr instance(const Architecture::BaseConstPtr&, const BaseSemantics::RiscOperatorsPtr&);

    /** Virtual constructor. */
    virtual BaseSemantics::DispatcherPtr create(const BaseSemantics::RiscOperatorsPtr&) const override;

    /** Dynamic cast to DispatcherAarch32 with assertion. */
    static DispatcherAarch32Ptr promote(const BaseSemantics::DispatcherPtr&);

protected:
    /** Initialized cached register descriptors from the register dictionary. */
    void initializeRegisterDescriptors();

    /** Initializes the instruction dispatch table.
     *
     *  This is called from the constructor. */
    void initializeInsnDispatchTable();

    /** Make sure memory is configured correctly, such as setting the byte order. */
    void initializeMemory();

protected:
    int iprocKey(SgAsmInstruction*) const override;
    RegisterDescriptor instructionPointerRegister() const override;
    RegisterDescriptor stackPointerRegister() const override;
    RegisterDescriptor callReturnRegister() const override;

public:
    BaseSemantics::SValuePtr read(SgAsmExpression*, size_t value_nbits=0, size_t addr_nbits=0) override;
    void write(SgAsmExpression*, const BaseSemantics::SValuePtr&, size_t addr_nbits=0) override;

    // Instruction condition
    BaseSemantics::SValuePtr conditionHolds(Aarch32InstructionCondition);

    // True if the expression is the program counter register
    bool isIpRegister(SgAsmExpression*);

    // Read the instruction pointer (PC) register, which is handled in a special way.
    BaseSemantics::SValuePtr readIpRegister(SgAsmInstruction*);

    // Is the processor in T32 mode? This is based just on the instruction being executed.
    BaseSemantics::SValuePtr isT32Mode();

    // Is the processor in A32 mode? This is based just on the instruction being executed.
    BaseSemantics::SValuePtr isA32Mode();

    // Set T32 mode on or off.
    void setThumbMode(SgAsmAarch32Instruction*);        // on if instructions is 2 bytes; off otherwise
    void setThumbMode(const BaseSemantics::SValuePtr &state);
    void setThumbMode(const BaseSemantics::SValuePtr &enabled, const BaseSemantics::SValuePtr &state);

    // Conditionally write a value to a register
    void maybeWriteRegister(const BaseSemantics::SValuePtr &enabled, RegisterDescriptor, const BaseSemantics::SValuePtr &value);

    // Conditionally write a value to memory
    void maybeWriteMemory(const BaseSemantics::SValuePtr &enabled, const BaseSemantics::SValuePtr &address,
                          const BaseSemantics::SValuePtr &value);

    // Conditionally write the the destination described the by ROSE expression, which must also be readable.
    void maybeWrite(const BaseSemantics::SValuePtr &enabled, SgAsmExpression *destination, const BaseSemantics::SValuePtr &value);

    // Returns the most significant bit.
    BaseSemantics::SValuePtr signBit(const BaseSemantics::SValuePtr&);

    // Return the register that's being directly read or written.
    RegisterDescriptor accessedRegister(SgAsmExpression*);

    // Returns true if the specified value is a constant true, false in all other cases
    bool mustBeSet(const BaseSemantics::SValuePtr&);

    //----------------------------------------------------------------------------------------------------------------
    // The following functions are more or less from ARM documentation and named similarly. They are generally not
    // further documented here or by ARM.
    //----------------------------------------------------------------------------------------------------------------
    using TwoValues = std::tuple<BaseSemantics::SValuePtr, BaseSemantics::SValuePtr>;

    enum class SrType { LSL, LSR, ASR, ROR, RRX };      // SRType
    enum class BranchType {                             // BranchType
        DIRCALL,                                        // direct branch with link
        DIR,                                            // undocumented but used by B instruction (maybe the same as DIRCALL?)
        INDCALL,                                        // indirect branch with link
        ERET,                                           // exception return (indirect)
        DBGEXIT,                                        // exit from debug state
        RET,                                            // indirect branch with function return hint
        INDIR,                                          // indicrect branch
        EXCEPTION,                                      // exception entry
        RESET,                                          // reset
        UNKNOWN                                         // other
    };

    BaseSemantics::SValuePtr part(const BaseSemantics::SValuePtr&, size_t maxBit, size_t minBit); // X<m,n>
    BaseSemantics::SValuePtr part(const BaseSemantics::SValuePtr&, size_t bitNumber); // X<n>
    BaseSemantics::SValuePtr join(const BaseSemantics::SValuePtr &highBits, const BaseSemantics::SValuePtr &lowBits); // X:Y
    BaseSemantics::SValuePtr zeroExtend(const BaseSemantics::SValuePtr&, size_t); // ZeroExtend
    BaseSemantics::SValuePtr makeZeros(size_t);         // Zeros
    TwoValues a32ExpandImmC(const BaseSemantics::SValuePtr&); // A32ExpandImm_C
    TwoValues shiftC(const BaseSemantics::SValuePtr&, SrType, int amount, const BaseSemantics::SValuePtr &carry); // Shift_C
    TwoValues lslC(const BaseSemantics::SValuePtr&, size_t shift); // LSL_C
    TwoValues lsrC(const BaseSemantics::SValuePtr&, size_t shift); // LSR_C
    TwoValues asrC(const BaseSemantics::SValuePtr&, size_t shift); // ASR_C
    TwoValues rorC(const BaseSemantics::SValuePtr&, int shift);    // ROR_C
    TwoValues rrxC(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr &carry); // RRX_C
    BaseSemantics::SValuePtr lsr(const BaseSemantics::SValuePtr&, size_t shift); // LSR
    BaseSemantics::SValuePtr lsl(const BaseSemantics::SValuePtr&, size_t shift); // LSL
    BaseSemantics::SValuePtr signExtend(const BaseSemantics::SValuePtr&, size_t); // SignExtend
    void aluExceptionReturn(const BaseSemantics::SValuePtr &enabled,
                            const BaseSemantics::SValuePtr &address); // ALUExceptionReturn
    void aluWritePc(const BaseSemantics::SValuePtr &enabled, const BaseSemantics::SValuePtr &address); // ALUWritePC
    void bxWritePc(const BaseSemantics::SValuePtr &enabled, const BaseSemantics::SValuePtr &address, BranchType); // BXWritePC
    void branchWritePc(const BaseSemantics::SValuePtr &enabled, const BaseSemantics::SValuePtr &address,
                       BranchType branchType); // BranchWritePC
    void branchTo(const BaseSemantics::SValuePtr &enabled, const BaseSemantics::SValuePtr &alignedAddress,
                  BranchType);                          // BranchTo
    BaseSemantics::SValuePtr align(const BaseSemantics::SValuePtr&, unsigned); // Align
    BaseSemantics::SValuePtr pc();                      // PC. Returns address of current instruction plus eight (not sure why).
    BaseSemantics::SValuePtr pcStoreValue();            // PCStoreValue (which doesn't store anything at all)
    void loadWritePc(const BaseSemantics::SValuePtr &enabled, const BaseSemantics::SValuePtr &address); // LoadWritePC
    TwoValues addWithCarry(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&,
                           const BaseSemantics::SValuePtr&); // AddWithCarry
    BaseSemantics::SValuePtr spsr();                    // SPSR
    void aarch32ExceptionReturn(const BaseSemantics::SValuePtr &enabled, const BaseSemantics::SValuePtr &address,
                                const BaseSemantics::SValuePtr &spsr); // AArch32.ExceptionReturn
    void dbgdtrEl0(const BaseSemantics::SValuePtr &enabled, const BaseSemantics::SValuePtr &value); // DBGDTR_EL0
    BaseSemantics::SValuePtr dbgdtrEl0();               // DBGDTR_EL0
    BaseSemantics::SValuePtr bigEndian();               // BigEndian
    BaseSemantics::SValuePtr signedSat(const BaseSemantics::SValuePtr&, size_t); // SignedSat
    TwoValues signedSatQ(const BaseSemantics::SValuePtr&, size_t); // SignedSatQ
    BaseSemantics::SValuePtr unsignedSat(const BaseSemantics::SValuePtr&, size_t); // UnsignedSat
    TwoValues unsignedSatQ(const BaseSemantics::SValuePtr&, size_t); // UnsignedSatQ
    BaseSemantics::SValuePtr abs(const BaseSemantics::SValuePtr&); // Abs
    BaseSemantics::SValuePtr countLeadingZeroBits(const BaseSemantics::SValuePtr&); // CountLeadingZeroBits
    void aarch32CallHypervisor(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&); // AArch32.CallHypervisor
};

} // namespace
} // namespace
} // namespace

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
BOOST_CLASS_EXPORT_KEY(Rose::BinaryAnalysis::InstructionSemantics::DispatcherAarch32);
#endif

#endif
#endif
