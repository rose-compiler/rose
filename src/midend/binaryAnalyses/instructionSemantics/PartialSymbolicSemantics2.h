#ifndef Rose_PartialSymbolicSemantics2_H
#define Rose_PartialSymbolicSemantics2_H


#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>

#include <map>
#include <stdint.h>
#include <vector>

#include "rosePublicConfig.h"
#ifdef ROSE_HAVE_GCRYPT_H
#include <gcrypt.h>
#endif

#include "x86InstructionSemantics.h"
#include "BaseSemantics2.h"
#include "MemoryMap.h"
#include "FormatRestorer.h"
#include "RegisterStateGeneric.h"

namespace rose {
namespace BinaryAnalysis {              // documented elsewhere
namespace InstructionSemantics2 {       // documented elsewhere

/** A fast, partially symbolic semantic domain.
 *
 *  This policy can be used to emulate the execution of a single basic block of instructions.  It is similar in nature to the
 *  FindConstantsPolicy (deprecated) except much simpler, much faster, and much more memory-lean.  The main classes are:
 *
 *  <ul>
 *    <li>SValue: the values stored in registers and memory and used for memory addresses.</li>
 *    <li>State: represents the state of the virtual machine, its registers and memory.</li>
 *    <li>RiscOperators: the low-level operators called by instruction dispatchers (e.g., DispatcherX86).</li>
 *  </ul>
 *
 *  Each value is either a known value or an unknown value. An unknown value consists of a base name and offset and whether the
 *  value is negated. */
namespace PartialSymbolicSemantics {

extern uint64_t name_counter;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Print formatter
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Formatter that renames variables on the fly.  When this formatter is used, named variables are renamed using lower
 *  numbers. This is useful for human-readable output because variable names tend to get very large (like "v904885611"). */
class Formatter: public BaseSemantics::Formatter {
protected:
    typedef std::map<uint64_t, uint64_t> Map;
    Map renames;
    size_t next_name;
public:
    Formatter(): next_name(1) {}
    uint64_t rename(uint64_t orig_name);
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Semantic values
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Smart pointer to an SValue object.  SValue objects are reference counted and should not be explicitly deleted. */
typedef Sawyer::SharedPointer<class SValue> SValuePtr;

/** Type of values manipulated by the PartialSymbolicSemantics domain. A value is either known or unknown. Unknown values have
 *  a base name (unique ID number), offset, and sign. */
class SValue: public BaseSemantics::SValue {
public:
    uint64_t name;              /**< Zero for constants; non-zero ID number for everything else. */
    uint64_t offset;            /**< The constant (if name==0) or an offset w.r.t. an unknown (named) base value. */
    bool negate;                /**< Switch between name+offset and (-name)+offset; should be false for constants. */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    explicit SValue(size_t nbits)
        : BaseSemantics::SValue(nbits), name(++name_counter), offset(0), negate(false) {}

    SValue(size_t nbits, uint64_t number)
        : BaseSemantics::SValue(nbits), name(0), offset(number), negate(false) {
        if (nbits <= 64) {
            this->offset &= IntegerOps::genMask<uint64_t>(nbits);
        } else {
            name = ++name_counter;
            offset = 0;
        }
    }

    SValue(size_t nbits, uint64_t name, uint64_t offset, bool negate)
        : BaseSemantics::SValue(nbits), name(name), offset(offset), negate(negate) {
        this->offset &= IntegerOps::genMask<uint64_t>(nbits);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors
public:
    /** Instantiate a new prototypical value. Prototypical values are only used for their virtual constructors. */
    static SValuePtr instance() {
        return SValuePtr(new SValue(1));
    }

    /** Instantiate a new undefined value of specified width. */
    static SValuePtr instance(size_t nbits) {
        return SValuePtr(new SValue(nbits));
    }

    /** Instantiate a new concrete value. */
    static SValuePtr instance(size_t nbits, uint64_t value) {
        return SValuePtr(new SValue(nbits, value));
    }
    
    /** Insantiate a new value with all the necessary parts. */
    static SValuePtr instance(size_t nbits, uint64_t name, uint64_t offset, bool negate) {
        return SValuePtr(new SValue(nbits, name, offset, negate));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors
public:
    virtual BaseSemantics::SValuePtr undefined_(size_t nbits) const ROSE_OVERRIDE {
        return instance(nbits);
    }
    virtual BaseSemantics::SValuePtr unspecified_(size_t nbits) const ROSE_OVERRIDE {
        return instance(nbits);
    }

    virtual BaseSemantics::SValuePtr number_(size_t nbits, uint64_t value) const ROSE_OVERRIDE {
        return instance(nbits, value);
    }

    virtual BaseSemantics::SValuePtr copy(size_t new_width=0) const ROSE_OVERRIDE {
        SValuePtr retval(new SValue(*this));
        if (new_width!=0 && new_width!=retval->get_width())
            retval->set_width(new_width);
        return retval;
    }

    virtual BaseSemantics::SValuePtr merge(const BaseSemantics::SValuePtr &other, SMTSolver*) const ROSE_OVERRIDE;
    
    /** Virtual allocating constructor. Creates a new semantic value with full control over all aspects of the value. */
    virtual BaseSemantics::SValuePtr create(size_t nbits, uint64_t name, uint64_t offset, bool negate) const {
        return instance(nbits, name, offset, negate);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casts
public:
    /** Promote a base value to a PartialSymbolicSemantics value. The value @p v must have a
     *  PartialSymbolicSemantics::SValue dynamic type. */
    static SValuePtr promote(const BaseSemantics::SValuePtr &v) {
        SValuePtr retval = v.dynamicCast<SValue>();
        ASSERT_not_null(retval);
        return retval;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Other stuff we inherited from the super class
public:
    virtual void set_width(size_t nbits) ROSE_OVERRIDE {
        BaseSemantics::SValue::set_width(nbits);
        offset &= IntegerOps::genMask<uint64_t>(nbits);
    }

    virtual bool may_equal(const BaseSemantics::SValuePtr &other, SMTSolver *solver=NULL) const ROSE_OVERRIDE;
    virtual bool must_equal(const BaseSemantics::SValuePtr &other, SMTSolver *solver=NULL) const ROSE_OVERRIDE;

    virtual void print(std::ostream&, BaseSemantics::Formatter&) const ROSE_OVERRIDE;
    
    virtual bool is_number() const ROSE_OVERRIDE {
        return 0==name;
    }

    virtual uint64_t get_number() const ROSE_OVERRIDE {
        ASSERT_require(is_number());
        return offset;
    }
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Register state
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef BaseSemantics::RegisterStateGeneric RegisterState;
typedef BaseSemantics::RegisterStateGenericPtr RegisterStatePtr;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Memory state
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// PartialSymbolicSemantics uses BaseSemantics::MemoryCellList (or subclass) as its memory state, and does not expect the
// MemoryCellList to be byte-restricted (i.e., the cells can store multi-byte values).

typedef BaseSemantics::MemoryCellList MemoryState;
typedef BaseSemantics::MemoryCellListPtr MemoryStatePtr;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Complete state
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Smart pointer to a State object.  State objects are reference counted and should not be explicitly deleted. */
typedef boost::shared_ptr<class State> StatePtr;

/** Represents the entire state of the machine. This state expects to use a subclass of BaseSemantics::MemoryCellList as
 *  its memory state, and does not expect that MemoryCellList to be byte-restricted. */
class State: public BaseSemantics::State {

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    State(const BaseSemantics::RegisterStatePtr &registers,
          const BaseSemantics::MemoryStatePtr &memory)
        : BaseSemantics::State(registers, memory) {
        // This state should use PartialSymbolicSemantics values (or subclasses thereof)
        ASSERT_not_null(registers);
        (void) SValue::promote(registers->get_protoval());
        ASSERT_not_null(memory);
        (void) SValue::promote(memory->get_addr_protoval());
        (void) SValue::promote(memory->get_val_protoval());

        // This state should use a memory that is not byte restricted.
        MemoryStatePtr mcl = MemoryState::promote(memory);
        ASSERT_require(!mcl->get_byte_restricted());
    }

    State(const State &other): BaseSemantics::State(other) {}

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors
public:
    /** Instantiates a new instance of memory state with specified register and memory states. */
    static StatePtr instance(const BaseSemantics::RegisterStatePtr &registers,
                             const BaseSemantics::MemoryStatePtr &memory) {
        return StatePtr(new State(registers, memory));
    }

    /** Instantiates a new copy of an existing state. */
    static StatePtr instance(const StatePtr &other) {
        return StatePtr(new State(*other));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors
public:
    virtual BaseSemantics::StatePtr create(const BaseSemantics::RegisterStatePtr &registers,
                                           const BaseSemantics::MemoryStatePtr &memory) const ROSE_OVERRIDE {
        return instance(registers, memory);
    }

    virtual BaseSemantics::StatePtr clone() const ROSE_OVERRIDE {
        StatePtr self = promote(boost::const_pointer_cast<BaseSemantics::State>(shared_from_this()));
        return instance(self);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casts
public:
    static StatePtr promote(const BaseSemantics::StatePtr &x) {
        StatePtr retval = boost::dynamic_pointer_cast<State>(x);
        ASSERT_not_null(x);
        return retval;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods first declared at this level of the class hierarchy
public:
    /** Print info about how registers differ.  If a RenameMap is specified then named values will be renamed to
     *  have a shorter name.  */
    virtual void print_diff_registers(std::ostream&, const StatePtr &other_state, Formatter&) const;

    /** Tests registers of two states for equality. */
    virtual bool equal_registers(const StatePtr &other) const;

    /** Removes from memory those values at addresses below the current stack pointer. This is automatically called
     *  after each instruction if the policy's p_discard_popped_memory property is set. */
    virtual void discard_popped_memory();
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      RISC operators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Smart pointer to a RiscOperators object.  RiscOperators objects are reference counted and should not be explicitly
 *  deleted. */
typedef boost::shared_ptr<class RiscOperators> RiscOperatorsPtr;

/** Defines RISC operators for this semantic domain. */
class RiscOperators: public BaseSemantics::RiscOperators {
protected:
    const MemoryMap *map;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    explicit RiscOperators(const BaseSemantics::SValuePtr &protoval, SMTSolver *solver=NULL)
        : BaseSemantics::RiscOperators(protoval, solver), map(NULL) {
        set_name("PartialSymbolic");
    }
    explicit RiscOperators(const BaseSemantics::StatePtr &state, SMTSolver *solver=NULL)
        : BaseSemantics::RiscOperators(state, solver), map(NULL) {
        set_name("PartialSymbolic");
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors
public:
    /** Instantiates a new RiscOperators object and configures it to use semantic values and states that are defaults for
     * PartialSymbolicSemantics. */
    static RiscOperatorsPtr instance(const RegisterDictionary *regdict);

    /** Instantiates a new RiscOperators object with specified prototypical values. */
    static RiscOperatorsPtr instance(const BaseSemantics::SValuePtr &protoval, SMTSolver *solver=NULL) {
        return RiscOperatorsPtr(new RiscOperators(protoval, solver));
    }

    /** Instantiates a new RiscOperators with specified state. */
    static RiscOperatorsPtr instance(const BaseSemantics::StatePtr &state, SMTSolver *solver=NULL) {
        return RiscOperatorsPtr(new RiscOperators(state, solver));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors
public:
    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::SValuePtr &protoval,
                                                   SMTSolver *solver=NULL) const ROSE_OVERRIDE {
        return instance(protoval, solver);
    }

    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::StatePtr &state,
                                                   SMTSolver *solver=NULL) const ROSE_OVERRIDE {
        return instance(state, solver);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casts
public:
    /** Run-time promotion of a base RiscOperators pointer to partial symbolic operators. This is a checked conversion--it will
     *  fail if @p from does not point to a PartialSymbolicSemantics::RiscOperators object. */
    static RiscOperatorsPtr promote(const BaseSemantics::RiscOperatorsPtr &x) {
        RiscOperatorsPtr retval = boost::dynamic_pointer_cast<RiscOperators>(x);
        ASSERT_not_null(retval);
        return retval;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods first declared at this level of the class hierarchy
public:
    /** A memory map can be used to provide default values for memory cells that are read before being written. Usually one
     *  would initialize the memory map to contain all the non-writable addresses.  The byte-order property of the memory
     *  map is used when reading the value.
     * @{ */
    const MemoryMap *get_memory_map() const { return map; }
    void set_memory_map(const MemoryMap *m) { map = m; }
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Risc operators inherited
public:
    virtual void interrupt(int majr, int minr) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr and_(const BaseSemantics::SValuePtr &a_,
                                          const BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr or_(const BaseSemantics::SValuePtr &a_,
                                         const BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr xor_(const BaseSemantics::SValuePtr &a_,
                                          const BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr invert(const BaseSemantics::SValuePtr &a_) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr extract(const BaseSemantics::SValuePtr &a_,
                                             size_t begin_bit, size_t end_bit) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr concat(const BaseSemantics::SValuePtr &a_,
                                            const BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr leastSignificantSetBit(const BaseSemantics::SValuePtr &a_) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr mostSignificantSetBit(const BaseSemantics::SValuePtr &a_) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr rotateLeft(const BaseSemantics::SValuePtr &a_,
                                                const BaseSemantics::SValuePtr &sa_) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr rotateRight(const BaseSemantics::SValuePtr &a_,
                                                 const BaseSemantics::SValuePtr &sa_) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr shiftLeft(const BaseSemantics::SValuePtr &a_,
                                               const BaseSemantics::SValuePtr &sa_) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr shiftRight(const BaseSemantics::SValuePtr &a_,
                                                const BaseSemantics::SValuePtr &sa_) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr shiftRightArithmetic(const BaseSemantics::SValuePtr &a_,
                                                          const BaseSemantics::SValuePtr &sa_) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr equalToZero(const BaseSemantics::SValuePtr &a_) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr ite(const BaseSemantics::SValuePtr &sel_,
                                         const BaseSemantics::SValuePtr &a_,
                                         const BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr signExtend(const BaseSemantics::SValuePtr &a_, size_t new_width) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr add(const BaseSemantics::SValuePtr &a_,
                                         const BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr addWithCarries(const BaseSemantics::SValuePtr &a_,
                                                    const BaseSemantics::SValuePtr &b_,
                                                    const BaseSemantics::SValuePtr &c_,
                                                    BaseSemantics::SValuePtr &carry_out/*out*/);
    virtual BaseSemantics::SValuePtr negate(const BaseSemantics::SValuePtr &a_) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr signedDivide(const BaseSemantics::SValuePtr &a_,
                                                  const BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr signedModulo(const BaseSemantics::SValuePtr &a_,
                                                  const BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr signedMultiply(const BaseSemantics::SValuePtr &a_,
                                                    const BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr unsignedDivide(const BaseSemantics::SValuePtr &a_,
                                                    const BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr unsignedModulo(const BaseSemantics::SValuePtr &a_,
                                                    const BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr unsignedMultiply(const BaseSemantics::SValuePtr &a_,
                                                      const BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr readMemory(const RegisterDescriptor &segreg,
                                                const BaseSemantics::SValuePtr &addr,
                                                const BaseSemantics::SValuePtr &dflt,
                                                const BaseSemantics::SValuePtr &cond) ROSE_OVERRIDE;
    virtual void writeMemory(const RegisterDescriptor &segreg,
                             const BaseSemantics::SValuePtr &addr,
                             const BaseSemantics::SValuePtr &data,
                             const BaseSemantics::SValuePtr &cond) ROSE_OVERRIDE;


#if 0

protected:
                mutable State<SValue> orig_state;/**< Original machine state, initialized by constructor and mem_write. This
                                                     *   data member is mutable because a mem_read() operation, although
                                                     *   conceptually const, may cache the value that was read so that
                                                     *   subsquent reads from the same address will return the same value. This
                                                     *   member is initialized by the first call to startInstruction() (as
                                                     *   called by X86InstructionSemantics::processInstruction()) which allows
                                                     *   the user to initialize the original conditions using the same
                                                     *   interface that's used to process instructions.  In other words, if one
                                                     *   wants the stack pointer to contain a specific original value, then one
                                                     *   may initialize the stack pointer by calling writeGPR() before
                                                     *   processing the first instruction. */
                bool p_discard_popped_memory;       /**< Property that determines how the stack behaves.  When set, any time
                                                     * the stack pointer is adjusted, memory below the stack pointer and having
                                                     * the same address name as the stack pointer is removed (the memory
                                                     * location becomes undefined). The default is false, that is, no special
                                                     * treatment for the stack. */
                MemoryMap *map;                     /**< Initial known memory values for known addresses. */

            public:
                typedef State<SValue> StateType;

                Policy(): cur_insn(NULL), p_discard_popped_memory(false), ninsns(0), map(NULL) {
                    /* So that named values are identical in both; reinitialized by first call to startInstruction(). */
                    set_register_dictionary(RegisterDictionary::dictionary_pentium4());
                    orig_state = cur_state;
                }

                /** Returns the original state.  The original state is initialized to be equal to the current state twice: once
                 *  by the constructor, and then again when the first instruction is processed. */
                const State<SValue>& get_orig_state() const { return orig_state; }
                State<SValue>& get_orig_state() { return orig_state; }

                /** Returns a copy of the state after removing memory that is not pertinent to an equal_states() comparison. */
                typename State<SValue>::Memory memory_for_equality(const State<SValue>&) const;

                /** Returns a copy of the current state after removing memory that is not pertinent to an equal_states()
                 *  comparison. */
                typename State<SValue>::Memory memory_for_equality() const { return memory_for_equality(cur_state); }

                /** Compares two states for equality. The comarison looks at all register values and the memory locations that
                 *  are different than their original value (but excluding differences due to clobbering). It does not compare
                 *  memory that has only been read. */
                bool equal_states(const State<SValue>&, const State<SValue>&) const;

                /** Returns true if the specified value exists in memory and is provably at or above the stack pointer.  The
                 *  stack pointer need not have a known value. */
                bool on_stack(const SValue<32> &value) const;

                /** Changes how the policy treats the stack.  See the p_discard_popped_memory property data member for
                 *  details. */
                void set_discard_popped_memory(bool b) {
                    p_discard_popped_memory = b;
                }

                /** Returns the current setting for the property that determines how the stack behaves. See the
                 *  p_set_discard_popped_memory property data member for details. */
                bool get_discard_popped_memory() const {
                    return p_discard_popped_memory;
                }

                /** Print only the differences between two states.  If a rename map is specified then named values will be
                 *  renamed to have a shorter name.  See the SValue<>::rename() method for details. */
                void print_diff(std::ostream&, const State<SValue>&, const State<SValue>&, RenameMap *rmap=NULL) const ;

                /** Print the difference between a state and the initial state.  If a rename map is specified then named values
                 *  will be renamed to have a shorter name.  See the SValue<>::rename() method for details. */
                void print_diff(std::ostream &o, const State<SValue> &state, RenameMap *rmap=NULL) const {
                    print_diff(o, orig_state, state, rmap);
                }

                /** Print the difference between the current state and the initial state.  If a rename map is specified then
                 *  named values will be renamed to have a shorter name.  See the SValue<>::rename() method for details. */
                void print_diff(std::ostream &o, RenameMap *rmap=NULL) const {
                    print_diff(o, orig_state, cur_state, rmap);
                }

                /** Returns the SHA1 hash of the difference between the current state and the original state.  If libgcrypt is
                 *  not available then the return value will be an empty string. */
                std::string SHA1() const;

                /** Obtains the binary SHA1 hash of the difference between the current state and the original state.  The hash
                 *  is returned through the @p digest argument, which must be at least 20 bytes long.  Returns true if the hash
                 *  can be computed. If libgcrypt is not available then the first 20 bytes of @p digest will be set to zero and
                 *  a false value is returned. */
                bool SHA1(unsigned char *digest) const;

                /** See memory_reference_type(). */
                enum MemRefType { MRT_STACK_PTR, MRT_FRAME_PTR, MRT_OTHER_PTR };

                /** Determines if the specified address is related to the current stack or frame pointer. This is used by
                 *  mem_write() when we're operating under the assumption that memory written via stack pointer is different
                 *  than memory written via frame pointer, and that memory written by either pointer is different than all
                 *  other memory. */
                MemRefType memory_reference_type(const State<SValue> &state, const SValue<32> &addr) const {
                    if (addr.name) {
                        if (addr.name==state.registers.gpr[x86_gpr_sp].name) return MRT_STACK_PTR;
                        if (addr.name==state.registers.gpr[x86_gpr_bp].name) return MRT_FRAME_PTR;
                        return MRT_OTHER_PTR;
                    }
                    if (addr==state.registers.gpr[x86_gpr_sp]) return MRT_STACK_PTR;
                    if (addr==state.registers.gpr[x86_gpr_bp]) return MRT_FRAME_PTR;
                    return MRT_OTHER_PTR;
                }




                /*************************************************************************************************************
                 * Functions invoked by the X86InstructionSemantics class for data access operations
                 *************************************************************************************************************/

#include "ReadWriteRegisterFragment.h"

                /** See NullSemantics::Policy::readMemory() */
                template <size_t Len> SValue<Len>
                readMemory(X86SegmentRegister segreg, const SValue<32> &addr, const SValue<1> &cond) const {
                    return mem_read<Len>(cur_state, addr);
                }

                /** See NullSemantics::Policy::writeMemory() */
                template <size_t Len> void
                writeMemory(X86SegmentRegister segreg, const SValue<32> &addr, const SValue<Len> &data,
                            const SValue<1> &cond) {
                    mem_write<Len>(cur_state, addr, data);
                }
#endif

            };


#if 0
            /*************************************************************************************************************
             *                              MemoryCell template functions
             *************************************************************************************************************/

            template<template <size_t> class SValue>
            bool
            MemoryCell<SValue>::may_alias(const MemoryCell &other) const {
                const SValue<32> &addr1 = this->address;
                const SValue<32> &addr2 = other.address;
                if (addr1.name != addr2.name) return true;

                /* Same unknown values but inverses (any offset). */
                if (addr1.name && addr1.negate!=addr2.negate) return true;

                /* If they have the same base values (or are both constant) then check the offsets. The 32-bit casts are
                 * purportedly necessary to wrap propertly, but I'm not sure this will work for addresses (LatticeElements)
                 * that have a length other than 32 bits. [FIXME RPM 2009-02-03]. */
                uint32_t offsetDiff = (uint32_t)(addr1.offset - addr2.offset);
                if (offsetDiff < this->nbytes || offsetDiff > (uint32_t)(-other.nbytes))
                    return true;
                return false;
            }

            template<template<size_t> class SValue>
            bool
            MemoryCell<SValue>::must_alias(const MemoryCell<SValue> &other) const {
                return this->address == other.address;
            }
#endif

#if 0
            /*****************************************************************************************************************
             *                              State template functions
             *****************************************************************************************************************/

            template<template<size_t> class SValue>
            void
            State<SValue>::print_diff_registers(std::ostream &o, const State &other, RenameMap *rmap/*=NULL*/) const
            {
#ifndef CXX_IS_ROSE_ANALYSIS
                // DQ (5/22/2010): This code does not compile using ROSE, it needs to be investigated to be reduced to an bug
                // report.

                std::string prefix = "    ";

                for (size_t i=0; i<this->registers.n_gprs; ++i) {
                    if (this->registers.gpr[i]!=other.registers.gpr[i]) {
                        o <<prefix <<gprToString((X86GeneralPurposeRegister)i) <<": "
                          <<this->registers.gpr[i].rename(rmap) <<" -> " <<other.registers.gpr[i].rename(rmap) <<"\n";
                    }
                }
                for (size_t i=0; i<this->registers.n_segregs; ++i) {
                    if (this->registers.segreg[i]!=other.registers.segreg[i]) {
                        o <<prefix <<segregToString((X86SegmentRegister)i) <<": "
                          <<this->registers.segreg[i].rename(rmap) <<" -> " <<other.registers.segreg[i].rename(rmap) <<"\n";
                    }
                }
                for (size_t i=0; i<this->registers.n_flags; ++i) {
                    if (this->registers.flag[i]!=other.registers.flag[i]) {
                        o <<prefix <<flagToString((X86Flag)i) <<": "
                          <<this->registers.flag[i].rename(rmap) <<" -> " <<other.registers.flag[i].rename(rmap) <<"\n";
                    }
                }
                if (this->registers.ip!=other.registers.ip) {
                    o <<prefix <<"ip: " <<this->registers.ip.rename(rmap) <<" -> " <<other.registers.ip.rename(rmap) <<"\n";
                }
#endif
            }

            template<template<size_t> class SValue>
            bool
            State<SValue>::equal_registers(const State &other) const
            {
#ifndef CXX_IS_ROSE_ANALYSIS
                for (size_t i=0; i<this->registers.n_gprs; ++i)
                    if (this->registers.gpr[i]!=other.registers.gpr[i]) return false;
                for (size_t i=0; i<this->registers.n_segregs; ++i)
                    if (this->registers.segreg[i]!=other.registers.segreg[i]) return false;
                for (size_t i=0; i<this->registers.n_flags; ++i)
                    if (this->registers.flag[i]!=other.registers.flag[i]) return false;
                if (this->registers.ip!=other.registers.ip) return false;
#endif
                return true;
            }

            template<template<size_t> class SValue>
            void
            State<SValue>::discard_popped_memory()
            {
                Memory new_mem;
                const SValue<32> &sp = this->registers.gpr[x86_gpr_sp];
                for (typename Memory::const_iterator mi=this->memory.begin(); mi!=this->memory.end(); ++mi) {
                    const SValue<32> &addr = mi->get_address();
                    if (addr.name!=sp.name || addr.negate!=sp.negate || (int32_t)addr.offset>=(int32_t)sp.offset)
                        new_mem.push_back(*mi);
                }
                this->memory = new_mem;
            }
#endif

#if 0
            /*****************************************************************************************************************
             *                              Policy template functions
             *****************************************************************************************************************/

            /* Returns memory that needs to be compared by equal_states() */
            template<
                template <template <size_t> class SValue> class State,
                template<size_t> class SValue>
            typename State<SValue>::Memory
            Policy<State, SValue>::memory_for_equality(const State<SValue> &state) const
            {
                State<SValue> tmp_state = state;
                typename State<SValue>::Memory retval;
#ifndef CXX_IS_ROSE_ANALYSIS
                for (typename State<SValue>::Memory::const_iterator mi=state.memory.begin(); mi!=state.memory.end(); ++mi) {
                    if (mi->is_written() && mi->get_data()!=mem_read<32>(orig_state, mi->get_address()))
                        retval.push_back(*mi);
                }
#endif
                return retval;
            }

            template<
                template <template <size_t> class SValue> class State,
                template<size_t> class SValue>
            bool
            Policy<State, SValue>::equal_states(const State<SValue> &s1, const State<SValue> &s2) const
            {
#ifndef CXX_IS_ROSE_ANALYSIS
                if (!s1.equal_registers(s2))
                    return false;
                typename State<SValue>::Memory m1 = memory_for_equality(s1);
                typename State<SValue>::Memory m2 = memory_for_equality(s2);
                std::vector<typename State<SValue>::Memory::value_type> v1(m1.begin(), m1.end());
                std::vector<typename State<SValue>::Memory::value_type> v2(m1.begin(), m1.end());
                if (v1.size()!=v2.size())
                    return false;
                std::sort(v1.begin(), v1.end());
                std::sort(v2.begin(), v2.end());
                for (size_t i=0; i<v1.size(); ++i) {
                    if (v1[i].get_nbytes() != v2[i].get_nbytes() ||
                        v1[i].get_address()!= v2[i].get_address() ||
                        v1[i].get_data()   != v2[i].get_data())
                        return false;
                }
#endif
                return true;
            }

            template<
                template <template <size_t> class SValue> class State,
                template<size_t> class SValue>
            void
            Policy<State, SValue>::print(std::ostream &o, RenameMap *rmap/*=NULL*/) const
            {
                cur_state.print(o, "", rmap);
            }

            template<
                template <template <size_t> class SValue> class State,
                template<size_t> class SValue>
            void
            Policy<State, SValue>::print_diff(std::ostream &o, const State<SValue> &s1,
                                                 const State<SValue> &s2, RenameMap *rmap/*=NULL*/) const
            {
#ifndef CXX_IS_ROSE_ANALYSIS
                s1.print_diff_registers(o, s2, rmap);

                /* Get all addresses that have been written and are not currently clobbered. */
                std::set<SValue<32> > addresses;
                for (typename State<SValue>::Memory::const_iterator mi=s1.memory.begin(); mi!=s1.memory.end(); ++mi) {
                    if (!mi->is_clobbered() && mi->is_written())
                        addresses.insert(mi->get_address());
                }
                for (typename State<SValue>::Memory::const_iterator mi=s2.memory.begin(); mi!=s2.memory.end(); ++mi) {
                    if (!mi->is_clobbered() && mi->is_written())
                        addresses.insert(mi->get_address());
                }

                State<SValue> tmp_s1 = s1;
                State<SValue> tmp_s2 = s2;
                size_t nmemdiff = 0;
                for (typename std::set<SValue<32> >::const_iterator ai=addresses.begin(); ai!=addresses.end(); ++ai) {
                    SValue<32> v1 = mem_read<32>(tmp_s1, *ai);
                    SValue<32> v2 = mem_read<32>(tmp_s2, *ai);
                    if (v1 != v2) {
                        if (0==nmemdiff++) o <<"    memory:\n";
                        o <<"      " <<(*ai).rename(rmap) <<": " <<v1.rename(rmap) <<" -> " <<v2.rename(rmap) <<"\n";
                    }
                }
#endif
            }

            template<
                template <template <size_t> class SValue> class State,
                template<size_t> class SValue>
            bool
            Policy<State, SValue>::on_stack(const SValue<32> &value) const
            {
#ifndef CXX_IS_ROSE_ANALYSIS
                const SValue<32> sp_inverted = invert(cur_state.registers.gpr[x86_gpr_sp]);
                for (typename State<SValue>::Memory::const_iterator mi=cur_state.memory.begin();
                     mi!=cur_state.memory.end();
                     ++mi) {
                    if (mi->get_nbytes()!=4 || !(mi->get_data()==value)) continue;
                    const SValue<32> &addr = mi->get_address();

                    /* Is addr >= sp? */
                    SValue<32> carries = 0;
                    SValue<32> diff = addWithCarries(addr, sp_inverted, true_(), carries/*out*/);
                    SValue<1> sf = extract<31,32>(diff);
                    SValue<1> of = xor_(extract<31,32>(carries), extract<30,31>(carries));
                    if (sf==of) return true;
                }
#endif
                return false;
            }

            template<
                template <template <size_t> class SValue> class State,
                template<size_t> class SValue>
            bool
            Policy<State, SValue>::SHA1(unsigned char digest[20]) const
            {
#ifdef ROSE_HAVE_GCRYPT_H
                /* libgcrypt requires gcry_check_version() to be called "before any other function in the library", but doesn't
                 * include an API function for determining if this has already been performed. It also doesn't indicate what
                 * happens when it's called more than once, or how expensive the call is.  Therefore, instead of calling it
                 * every time through this function, we'll just call it the first time. */
                static bool initialized = false;
                if (!initialized) {
                    gcry_check_version(NULL);
                    initialized = true;
                }

                std::stringstream s;
                RenameMap rmap;
                print_diff(s, &rmap);
                ROSE_ASSERT(gcry_md_get_algo_dlen(GCRY_MD_SHA1)==20);
                gcry_md_hash_buffer(GCRY_MD_SHA1, digest, s.str().c_str(), s.str().size());
                return true;
#else
                memset(digest, 0, 20);
                return false;
#endif
            }

            template<
                template <template <size_t> class SValue> class State,
                template<size_t> class SValue>
            std::string
            Policy<State, SValue>::SHA1() const
            {
                std::string digest_str;
                unsigned char digest[20];
                if (SHA1(digest)) {
                    for (size_t i=sizeof digest; i>0; --i) {
                        digest_str += "0123456789abcdef"[(digest[i-1] >> 4) & 0xf];
                        digest_str += "0123456789abcdef"[digest[i-1] & 0xf];
                    }
                }
                return digest_str;
            }
#endif

} // namespace
} // namespace
} // namespace
} // namespace

#endif
