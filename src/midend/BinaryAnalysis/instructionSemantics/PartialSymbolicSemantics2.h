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
#ifdef ROSE_HAVE_LIBGCRYPT
#include <gcrypt.h>
#endif

#include "BaseSemantics2.h"
#include "integerOps.h"
#include "MemoryMap.h"
#include "FormatRestorer.h"
#include "RegisterStateGeneric.h"
#include "MemoryCellList.h"

namespace Rose {
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

/** Shared-ownership pointer to a partial-symbolic semantic value. See @ref heap_object_shared_ownership. */
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
    virtual BaseSemantics::SValuePtr bottom_(size_t nbits) const ROSE_OVERRIDE {
        return instance(nbits);
    }
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

    virtual Sawyer::Optional<BaseSemantics::SValuePtr>
    createOptionalMerge(const BaseSemantics::SValuePtr &other,const BaseSemantics::MergerPtr&,
                        const SmtSolverPtr&) const ROSE_OVERRIDE;
    
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

    virtual bool may_equal(const BaseSemantics::SValuePtr &other,
                           const SmtSolverPtr &solver = SmtSolverPtr()) const ROSE_OVERRIDE;
    virtual bool must_equal(const BaseSemantics::SValuePtr &other,
                            const SmtSolverPtr &solver = SmtSolverPtr()) const ROSE_OVERRIDE;

    virtual void print(std::ostream&, BaseSemantics::Formatter&) const ROSE_OVERRIDE;

    virtual bool isBottom() const ROSE_OVERRIDE {
        return false;
    }
    
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

/** Shared-ownership pointer to partial symbolic semantics state. See @ref heap_object_shared_ownership. */
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
        (void) SValue::promote(registers->protoval());
        ASSERT_not_null(memory);
        (void) SValue::promote(memory->get_addr_protoval());
        (void) SValue::promote(memory->get_val_protoval());

        // This state should use a memory that is not byte restricted.
        MemoryStatePtr mcl = MemoryState::promote(memory);
        ASSERT_require(!mcl->byteRestricted());
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

/** Shared-ownership pointer to partial symbolic semantics RISC operations. See @ref heap_object_shared_ownership. */
typedef boost::shared_ptr<class RiscOperators> RiscOperatorsPtr;

/** Defines RISC operators for this semantic domain. */
class RiscOperators: public BaseSemantics::RiscOperators {
protected:
    MemoryMap::Ptr map;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    explicit RiscOperators(const BaseSemantics::SValuePtr &protoval, const SmtSolverPtr &solver = SmtSolverPtr())
        : BaseSemantics::RiscOperators(protoval, solver) {
        name("PartialSymbolic");
    }
    explicit RiscOperators(const BaseSemantics::StatePtr &state, const SmtSolverPtr &solver = SmtSolverPtr())
        : BaseSemantics::RiscOperators(state, solver) {
        name("PartialSymbolic");
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors
public:
    /** Instantiates a new RiscOperators object and configures it to use semantic values and states that are defaults for
     * PartialSymbolicSemantics. */
    static RiscOperatorsPtr instance(const RegisterDictionary *regdict);

    /** Instantiates a new RiscOperators object with specified prototypical values. */
    static RiscOperatorsPtr instance(const BaseSemantics::SValuePtr &protoval, const SmtSolverPtr &solver = SmtSolverPtr()) {
        return RiscOperatorsPtr(new RiscOperators(protoval, solver));
    }

    /** Instantiates a new RiscOperators with specified state. */
    static RiscOperatorsPtr instance(const BaseSemantics::StatePtr &state, const SmtSolverPtr &solver = SmtSolverPtr()) {
        return RiscOperatorsPtr(new RiscOperators(state, solver));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors
public:
    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::SValuePtr &protoval,
                                                   const SmtSolverPtr &solver = SmtSolverPtr()) const ROSE_OVERRIDE {
        return instance(protoval, solver);
    }

    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::StatePtr &state,
                                                   const SmtSolverPtr &solver = SmtSolverPtr()) const ROSE_OVERRIDE {
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
    const MemoryMap::Ptr get_memory_map() const { return map; }
    void set_memory_map(const MemoryMap::Ptr &m) { map = m; }
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
                                                    BaseSemantics::SValuePtr &carry_out/*out*/) ROSE_OVERRIDE;
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
    virtual BaseSemantics::SValuePtr readMemory(RegisterDescriptor segreg,
                                                const BaseSemantics::SValuePtr &addr,
                                                const BaseSemantics::SValuePtr &dflt,
                                                const BaseSemantics::SValuePtr &cond) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr peekMemory(RegisterDescriptor segreg,
                                                const BaseSemantics::SValuePtr &addr,
                                                const BaseSemantics::SValuePtr &dflt) ROSE_OVERRIDE;
    virtual void writeMemory(RegisterDescriptor segreg,
                             const BaseSemantics::SValuePtr &addr,
                             const BaseSemantics::SValuePtr &data,
                             const BaseSemantics::SValuePtr &cond) ROSE_OVERRIDE;

protected:
    BaseSemantics::SValuePtr readOrPeekMemory(RegisterDescriptor segreg, const BaseSemantics::SValuePtr &address,
                                              const BaseSemantics::SValuePtr &dflt_, bool allowSideEffects);
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
