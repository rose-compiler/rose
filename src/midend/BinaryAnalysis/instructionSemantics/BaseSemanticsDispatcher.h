#ifndef ROSE_BinaryAnalysis_InstructionSemantics2_BaseSemantics_Dispatcher_H
#define ROSE_BinaryAnalysis_InstructionSemantics2_BaseSemantics_Dispatcher_H
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include <BaseSemanticsTypes.h>
#include <Registers.h>

#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/shared_ptr.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
namespace BaseSemantics {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Instruction Dispatcher
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Functor that knows how to dispatch a single kind of instruction. */
class InsnProcessor {
public:
    virtual ~InsnProcessor() {}
    virtual void process(const DispatcherPtr &dispatcher, SgAsmInstruction *insn) = 0;
};
    
/** Dispatches instructions through the RISC layer.
 *
 *  The dispatcher is the instruction semantics entity that translates a high-level architecture-dependent instruction into a
 *  sequence of RISC operators whose interface is defined by ROSE. These classes are the key in ROSE's ability to connect a
 *  variety of instruction set architectures to a variety of semantic domains.
 *
 *  Each dispatcher contains a table indexed by the machine instruction "kind" (e.g., SgAsmMipsInstruction::get_kind()). The
 *  table stores functors derived from the abstract InsnProcessor class.  (FIXME: The functors are not currently reference
 *  counted; they are owned by the dispatcher and deleted when the dispatcher is destroyed. [Robb Matzke 2013-03-04])
 *
 *  Dispatcher objects are allocated on the heap and reference counted.  The BaseSemantics::Dispatcher is an abstract class
 *  that defines the interface.  See the Rose::BinaryAnalysis::InstructionSemantics2 namespace for an overview of how the parts
 *  fit together. */
class Dispatcher: public boost::enable_shared_from_this<Dispatcher> {
protected:
    RiscOperatorsPtr operators;
    const RegisterDictionary *regdict;                  /**< See set_register_dictionary(). */
    size_t addrWidth_;                                  /**< Width of memory addresses in bits. */
    bool autoResetInstructionPointer_;                  /**< Reset instruction pointer register for each instruction. */

    // Dispatchers keep a table of all the kinds of instructions they can handle.  The lookup key is typically some sort of
    // instruction identifier, such as from SgAsmX86Instruction::get_kind(), and comes from the iproc_key() virtual method.
    typedef std::vector<InsnProcessor*> InsnProcessors;
    InsnProcessors iproc_table;

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_NVP(operators);
        s & BOOST_SERIALIZATION_NVP(regdict);
        s & BOOST_SERIALIZATION_NVP(addrWidth_);
        s & BOOST_SERIALIZATION_NVP(autoResetInstructionPointer_);
        //s & iproc_table; -- not saved
    }
#endif
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    // Prototypical constructor
    Dispatcher(): regdict(NULL), addrWidth_(0), autoResetInstructionPointer_(true) {}

    // Prototypical constructor
    Dispatcher(size_t addrWidth, const RegisterDictionary *regs)
        : regdict(regs), addrWidth_(addrWidth), autoResetInstructionPointer_(true) {}

    Dispatcher(const RiscOperatorsPtr &ops, size_t addrWidth, const RegisterDictionary *regs)
        : operators(ops), regdict(regs), addrWidth_(addrWidth), autoResetInstructionPointer_(true) {
        ASSERT_not_null(operators);
        ASSERT_not_null(regs);
    }

public:
    /** Shared-ownership pointer for a @ref Dispatcher object. See @ref heap_object_shared_ownership. */
    typedef DispatcherPtr Ptr;

public:
    virtual ~Dispatcher() {
        for (InsnProcessors::iterator iter=iproc_table.begin(); iter!=iproc_table.end(); ++iter)
            delete *iter;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors. None since this is an abstract class


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors
public:
    /** Virtual constructor. */
    virtual DispatcherPtr create(const RiscOperatorsPtr &ops, size_t addrWidth=0, const RegisterDictionary *regs=NULL) const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods to process instructions
public:
    /** Process a single instruction. */
    virtual void processInstruction(SgAsmInstruction *insn);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Instruction processor table operations
public:
    /** Lookup the processor for an instruction.  Looks up the functor that has been registered to process the given
     *  instruction. Returns the null pointer if the instruction cannot be processed. Instruction processor objects are
     *  managed by the caller; the instruction itself is only used for the duration of this call. */
    virtual InsnProcessor *iproc_lookup(SgAsmInstruction *insn);

    /** Replace an instruction processor with another.  The processor for the specified instruction is replaced with the
     *  specified processor, which may be the null pointer.  Instruction processor objects are managed by the caller; the
     *  instruction itself is only used for the duration of this call. */
    virtual void iproc_replace(SgAsmInstruction *insn, InsnProcessor *iproc);    

    /** Given an instruction, return the InsnProcessor key that can be used as an index into the iproc_table. */
    virtual int iproc_key(SgAsmInstruction*) const = 0;

    /** Set an iproc table entry to the specified value.
     *
     *  The @p iproc object will become owned by this dispatcher and deleted when this dispatcher is destroyed. */
    virtual void iproc_set(int key, InsnProcessor *iproc);

    /** Obtain an iproc table entry for the specified key. */
    virtual InsnProcessor *iproc_get(int key);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Convenience methods that defer the call to some member object
public:
    /** Get a pointer to the RISC operators object. */
    virtual RiscOperatorsPtr get_operators() const { return operators; }

    /** Get a pointer to the state object. The state is stored in the RISC operators object, so this is just here for
     *  convenience. */
    virtual StatePtr currentState() const;

    /** Return the prototypical value.  The prototypical value comes from the RISC operators object. */
    virtual SValuePtr protoval() const;

    /** Returns the instruction that is being processed.
     *
     *  The instruction comes from the @ref RiscOperators::currentInstruction "currentInstruction" method of the RiscOperators
     *  object. */
    virtual SgAsmInstruction* currentInstruction() const;

    /** Return a new undefined semantic value.
     *
     * @{ */
    virtual SValuePtr undefined_(size_t nbits) const;
    virtual SValuePtr unspecified_(size_t nbits) const;
    /** @} */

    /** Return a semantic value representing a number. */
    virtual SValuePtr number_(size_t nbits, uint64_t number) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods related to registers
public:
    /** Access the register dictionary.  The register dictionary defines the set of registers over which the RISC operators may
     *  operate. This should be same registers (or superset thereof) whose values are stored in the machine state(s).  This
     *  dictionary is used by the Dispatcher class to translate register names to register descriptors.  For instance, to read
     *  from the "eax" register, the dispatcher will look up "eax" in its register dictionary and then pass that descriptor to
     *  the @ref RiscOperators::readRegister operation.  Register descriptors are also stored in instructions when the
     *  instruction is disassembled, so the dispatcher should probably be using the same registers as the disassembler, or a
     *  superset thereof.
     *
     *  The register dictionary should not be changed after a dispatcher is instantiated because the dispatcher's constructor
     *  may query the dictionary and cache the resultant register descriptors.
     * @{ */
    virtual const RegisterDictionary *get_register_dictionary() const {
        return regdict;
    }
    virtual void set_register_dictionary(const RegisterDictionary *regdict) {
        this->regdict = regdict;
    }
    /** @} */

    /** Lookup a register by name.  This dispatcher's register dictionary is consulted and the specified register is located by
     *  name.  If a bit width is specified (@p nbits) then it must match the size of register that was found.  If a valid
     *  register cannot be found then either an exception is thrown or an invalid register is returned depending on whether
     *  @p allowMissing is false or true, respectively. */
    virtual RegisterDescriptor findRegister(const std::string &regname, size_t nbits=0, bool allowMissing=false) const;

    /** Property: Width of memory addresses.
     *
     *  This property defines the width of memory addresses. All memory reads and writes (and any other defined memory
     *  operations) should pass address expressions that are this width.  The address width cannot be changed once it's set.
     *
     * @{ */
    size_t addressWidth() const { return addrWidth_; }
    void addressWidth(size_t nbits);
    /** @} */

    /** Returns the instruction pointer register. */
    virtual RegisterDescriptor instructionPointerRegister() const = 0;

    /** Returns the stack pointer register. */
    virtual RegisterDescriptor stackPointerRegister() const = 0;

    /** Returns the function call return address register. */
    virtual RegisterDescriptor callReturnRegister() const = 0;

    /** Property: Reset instruction pointer register for each instruction.
     *
     *  If this property is set, then each time an instruction is processed, the first thing that happens is that the
     *  instruction pointer register is reset to the concrete address of the instruction.
     *
     * @{ */
    bool autoResetInstructionPointer() const { return autoResetInstructionPointer_; }
    void autoResetInstructionPointer(bool b) { autoResetInstructionPointer_ = b; }
    /** @} */
    
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Miscellaneous methods that tend to be the same for most dispatchers
public:

    /** Update the instruction pointer register.
     *
     *  Causes the instruction pointer register to point to the address following the specified instruction.  Since every
     *  instruction has a concrete address, we could simply set the instruction pointer to that concrete address. However, some
     *  analyses depend on having an instruction pointer value that's built up by processing one instruction after
     *  another.  Therefore, if we can recognize the register state implementation and determine that the instruction pointer
     *  registers' value is already stored, we'll increment that value, which might result in a concrete value depending on the
     *  semantic domain. Otherwise we just explicitly assign a new concrete value to that register. */
    virtual void advanceInstructionPointer(SgAsmInstruction*);

    /** Returns a register descriptor for the segment part of a memory reference expression.  Many architectures don't use
     *  segment registers (they have a flat virtual address space), in which case the returned register descriptor's is_valid()
     *  method returns false. */
    virtual RegisterDescriptor segmentRegister(SgAsmMemoryReferenceExpression*);

    /** Increment all auto-increment registers in the expression.  This method traverses the expression and increments each
     *  the register of each register reference expression that has a positive adjustment value.  If the same register is
     *  encountered multiple times then it is incremented multiple times. */
    virtual void incrementRegisters(SgAsmExpression*);

    /** Decrement all auto-decrement registers in the expression.  This method traverses the expression and increments each
     *  the register of each register reference expression that has a negative adjustment value.  If the same register is
     *  encountered multiple times then it is decremented multiple times. */
    virtual void decrementRegisters(SgAsmExpression*);

    /** Update registers for pre-add expressions.
     *
     *  For each SgAsmBinaryAddPreupdate, add the lhs and rhs operands and assign the sum to the lhs, which must be a register
     *  reference expression. */
    virtual void preUpdate(SgAsmExpression*);

    /** Update registers for post-add expressions.
     *
     *  For each SgAsmBinaryAddPostupdate, add the lhs and rhs operands and assign the sum to the lhs, which must be a register
     *  reference expression. */
    virtual void postUpdate(SgAsmExpression*);

    /** Returns a memory address by evaluating the address expression.  The address expression can be either a constant or an
     *  expression containing operators and constants.  If @p nbits is non-zero then the result is sign extended or truncated
     *  to the specified width, otherwise the returned SValue is the natural width of the expression. */
    virtual SValuePtr effectiveAddress(SgAsmExpression*, size_t nbits=0);

    /** Reads an R-value expression.  The expression can be a constant, register reference, or memory reference.  The width of
     *  the returned value is specified by the @p value_nbits argument, and if this argument is zero then the width of the
     *  expression type is used.  The width of the address passed to lower-level memory access functions is specified by @p
     *  addr_nbits.  If @p addr_nbits is zero then the natural width of the effective address is passed to lower level
     *  functions. */
    virtual SValuePtr read(SgAsmExpression*, size_t value_nbits=0, size_t addr_nbits=0);

    /** Writes to an L-value expression. The expression can be a register or memory reference.  The width of the address passed
     *  to lower-level memory access functions is specified by @p addr_nbits.  If @p addr_nbits is zero then the natural width
     *  of the effective address is passed to lower level functions. */
    virtual void write(SgAsmExpression*, const SValuePtr &value, size_t addr_nbits=0);
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
