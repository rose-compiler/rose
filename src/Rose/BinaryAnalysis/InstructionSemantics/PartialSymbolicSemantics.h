#ifndef ROSE_BinaryAnalysis_InstructionSemantics_PartialSymbolicSemantics_H
#define ROSE_BinaryAnalysis_InstructionSemantics_PartialSymbolicSemantics_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/BasicTypes.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Formatter.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/MemoryCellList.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/State.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/SValue.h>

#ifdef ROSE_HAVE_LIBGCRYPT
#include <gcrypt.h>
#endif

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {

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

/** Shared-ownership pointer to a partial-symbolic semantic value. */
typedef Sawyer::SharedPointer<class SValue> SValuePtr;

/** Type of values manipulated by the PartialSymbolicSemantics domain. A value is either known or unknown. Unknown values have
 *  a base name (unique ID number), offset, and sign. */
class SValue: public BaseSemantics::SValue {
public:
    /** Base type. */
    using Super = BaseSemantics::SValue;

    /** Shared-ownership pointer. */
    using Ptr = SValuePtr;

public:
    uint64_t name;              /**< Zero for constants; non-zero ID number for everything else. */
    uint64_t offset;            /**< The constant (if name==0) or an offset w.r.t. an unknown (named) base value. */
    bool negate;                /**< Switch between name+offset and (-name)+offset; should be false for constants. */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    explicit SValue(size_t nbits);
    SValue(size_t nbits, uint64_t number);
    SValue(size_t nbits, uint64_t name, uint64_t offset, bool negate);

public:
    /** Returns the next available name.
     *
     *  Returns the next available name and increments it.
     *
     *  Thread safety: This function is thread safe. */
    static uint64_t nextName();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors
public:
    /** Instantiate a new prototypical value. Prototypical values are only used for their virtual constructors. */
    static SValuePtr instance();

    /** Instantiate a new undefined value of specified width. */
    static SValuePtr instance(size_t nbits);

    /** Instantiate a new concrete value. */
    static SValuePtr instance(size_t nbits, uint64_t value);

    /** Insantiate a new value with all the necessary parts. */
    static SValuePtr instance(size_t nbits, uint64_t name, uint64_t offset, bool negate);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors
public:
    virtual BaseSemantics::SValuePtr bottom_(size_t nbits) const override;
    virtual BaseSemantics::SValuePtr undefined_(size_t nbits) const override;
    virtual BaseSemantics::SValuePtr unspecified_(size_t nbits) const override;
    virtual BaseSemantics::SValuePtr number_(size_t nbits, uint64_t value) const override;
    virtual BaseSemantics::SValuePtr copy(size_t new_width=0) const override;

    virtual Sawyer::Optional<BaseSemantics::SValuePtr>
    createOptionalMerge(const BaseSemantics::SValuePtr &other,const BaseSemantics::MergerPtr&,
                        const SmtSolverPtr&) const override;
    
    /** Virtual allocating constructor. Creates a new semantic value with full control over all aspects of the value. */
    virtual BaseSemantics::SValuePtr create(size_t nbits, uint64_t name, uint64_t offset, bool negate) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casts
public:
    /** Promote a base value to a PartialSymbolicSemantics value. The value @p v must have a
     *  PartialSymbolicSemantics::SValue dynamic type. */
    static SValuePtr promote(const BaseSemantics::SValuePtr&);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Other stuff we inherited from the super class
public:
    virtual void hash(Combinatorics::Hasher&) const override;
    virtual void print(std::ostream&, BaseSemantics::Formatter&) const override;

    virtual bool isBottom() const override {
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Override legacy members. These have similar non-virtual camelCase names in the base class, and eventually we'll remoe
    // these snake_case names and change the camelCase names to be the virtual functions. Therefore, be sure to use "override"
    // in your own code so you know when we make this change.
public:
    // See nBits
    virtual void set_width(size_t nbits) override;

    // See mayEqual
    virtual bool may_equal(const BaseSemantics::SValuePtr &other,
                           const SmtSolverPtr &solver = SmtSolverPtr()) const override;

    // See mustEqual
    virtual bool must_equal(const BaseSemantics::SValuePtr &other,
                            const SmtSolverPtr &solver = SmtSolverPtr()) const override;

    // See isConcrete
    virtual bool is_number() const override {
        return 0==name;
    }

    // See toUnsigned and toSigned.
    virtual uint64_t get_number() const override {
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

/** Shared-ownership pointer to partial symbolic semantics state. */
typedef boost::shared_ptr<class State> StatePtr;

/** Represents the entire state of the machine. This state expects to use a subclass of BaseSemantics::MemoryCellList as
 *  its memory state, and does not expect that MemoryCellList to be byte-restricted. */
class State: public BaseSemantics::State {
public:
    /** Base type. */
    using Super = BaseSemantics::State;

    /** Shared-ownership pointer. */
    using Ptr = StatePtr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    State(const BaseSemantics::RegisterStatePtr &registers, const BaseSemantics::MemoryStatePtr &memory);
    State(const State &other);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors
public:
    /** Instantiates a new instance of memory state with specified register and memory states. */
    static StatePtr instance(const BaseSemantics::RegisterStatePtr &registers, const BaseSemantics::MemoryStatePtr &memory);

    /** Instantiates a new copy of an existing state. */
    static StatePtr instance(const StatePtr &other);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors
public:
    virtual BaseSemantics::StatePtr create(const BaseSemantics::RegisterStatePtr &registers,
                                           const BaseSemantics::MemoryStatePtr &memory) const override;
    virtual BaseSemantics::StatePtr clone() const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casts
public:
    static StatePtr promote(const BaseSemantics::StatePtr&);

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

/** Shared-ownership pointer to partial symbolic semantics RISC operations. */
typedef boost::shared_ptr<class RiscOperators> RiscOperatorsPtr;

/** Defines RISC operators for this semantic domain. */
class RiscOperators: public BaseSemantics::RiscOperators {
public:
    /** Base type. */
    using Super = BaseSemantics::RiscOperators;

    /** Shared-ownership pointer. */
    using Ptr = RiscOperatorsPtr;

protected:
    MemoryMapPtr map;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    explicit RiscOperators(const BaseSemantics::SValuePtr &protoval, const SmtSolverPtr&);

    explicit RiscOperators(const BaseSemantics::StatePtr&, const SmtSolverPtr&);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors
public:
    ~RiscOperators();

    /** Instantiates a new RiscOperators object and configures it to use semantic values and states that are defaults for
     * PartialSymbolicSemantics. */
    static RiscOperatorsPtr instanceFromRegisters(const RegisterDictionaryPtr&);

    /** Instantiates a new RiscOperators object with specified prototypical values. */
    static RiscOperatorsPtr instanceFromProtoval(const BaseSemantics::SValuePtr &protoval,
                                                 const SmtSolverPtr &solver = SmtSolverPtr());

    /** Instantiates a new RiscOperators with specified state. */
    static RiscOperatorsPtr instanceFromState(const BaseSemantics::StatePtr&, const SmtSolverPtr &solver = SmtSolverPtr());

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors
public:
    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::SValuePtr &protoval,
                                                   const SmtSolverPtr &solver = SmtSolverPtr()) const override;

    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::StatePtr&,
                                                   const SmtSolverPtr &solver = SmtSolverPtr()) const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casts
public:
    /** Run-time promotion of a base RiscOperators pointer to partial symbolic operators. This is a checked conversion--it will
     *  fail if @p from does not point to a PartialSymbolicSemantics::RiscOperators object. */
    static RiscOperatorsPtr promote(const BaseSemantics::RiscOperatorsPtr&);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods first declared at this level of the class hierarchy
public:
    /** A memory map can be used to provide default values for memory cells that are read before being written. Usually one
     *  would initialize the memory map to contain all the non-writable addresses.  The byte-order property of the memory
     *  map is used when reading the value.
     * @{ */
    const MemoryMapPtr get_memory_map() const;
    void set_memory_map(const MemoryMapPtr&);
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Risc operators inherited
public:
    virtual void interrupt(int majr, int minr) override;
    virtual BaseSemantics::SValuePtr and_(const BaseSemantics::SValuePtr &a_,
                                          const BaseSemantics::SValuePtr &b_) override;
    virtual BaseSemantics::SValuePtr or_(const BaseSemantics::SValuePtr &a_,
                                         const BaseSemantics::SValuePtr &b_) override;
    virtual BaseSemantics::SValuePtr xor_(const BaseSemantics::SValuePtr &a_,
                                          const BaseSemantics::SValuePtr &b_) override;
    virtual BaseSemantics::SValuePtr invert(const BaseSemantics::SValuePtr &a_) override;
    virtual BaseSemantics::SValuePtr extract(const BaseSemantics::SValuePtr &a_,
                                             size_t begin_bit, size_t end_bit) override;
    virtual BaseSemantics::SValuePtr concat(const BaseSemantics::SValuePtr &a_,
                                            const BaseSemantics::SValuePtr &b_) override;
    virtual BaseSemantics::SValuePtr leastSignificantSetBit(const BaseSemantics::SValuePtr &a_) override;
    virtual BaseSemantics::SValuePtr mostSignificantSetBit(const BaseSemantics::SValuePtr &a_) override;
    virtual BaseSemantics::SValuePtr rotateLeft(const BaseSemantics::SValuePtr &a_,
                                                const BaseSemantics::SValuePtr &sa_) override;
    virtual BaseSemantics::SValuePtr rotateRight(const BaseSemantics::SValuePtr &a_,
                                                 const BaseSemantics::SValuePtr &sa_) override;
    virtual BaseSemantics::SValuePtr shiftLeft(const BaseSemantics::SValuePtr &a_,
                                               const BaseSemantics::SValuePtr &sa_) override;
    virtual BaseSemantics::SValuePtr shiftRight(const BaseSemantics::SValuePtr &a_,
                                                const BaseSemantics::SValuePtr &sa_) override;
    virtual BaseSemantics::SValuePtr shiftRightArithmetic(const BaseSemantics::SValuePtr &a_,
                                                          const BaseSemantics::SValuePtr &sa_) override;
    virtual BaseSemantics::SValuePtr equalToZero(const BaseSemantics::SValuePtr &a_) override;
    virtual BaseSemantics::SValuePtr iteWithStatus(const BaseSemantics::SValuePtr &sel_,
                                                   const BaseSemantics::SValuePtr &a_,
                                                   const BaseSemantics::SValuePtr &b_,
                                                   IteStatus&) override;
    virtual BaseSemantics::SValuePtr signExtend(const BaseSemantics::SValuePtr &a_, size_t new_width) override;
    virtual BaseSemantics::SValuePtr add(const BaseSemantics::SValuePtr &a_,
                                         const BaseSemantics::SValuePtr &b_) override;
    virtual BaseSemantics::SValuePtr addWithCarries(const BaseSemantics::SValuePtr &a_,
                                                    const BaseSemantics::SValuePtr &b_,
                                                    const BaseSemantics::SValuePtr &c_,
                                                    BaseSemantics::SValuePtr &carry_out/*out*/) override;
    virtual BaseSemantics::SValuePtr negate(const BaseSemantics::SValuePtr &a_) override;
    virtual BaseSemantics::SValuePtr signedDivide(const BaseSemantics::SValuePtr &a_,
                                                  const BaseSemantics::SValuePtr &b_) override;
    virtual BaseSemantics::SValuePtr signedModulo(const BaseSemantics::SValuePtr &a_,
                                                  const BaseSemantics::SValuePtr &b_) override;
    virtual BaseSemantics::SValuePtr signedMultiply(const BaseSemantics::SValuePtr &a_,
                                                    const BaseSemantics::SValuePtr &b_) override;
    virtual BaseSemantics::SValuePtr unsignedDivide(const BaseSemantics::SValuePtr &a_,
                                                    const BaseSemantics::SValuePtr &b_) override;
    virtual BaseSemantics::SValuePtr unsignedModulo(const BaseSemantics::SValuePtr &a_,
                                                    const BaseSemantics::SValuePtr &b_) override;
    virtual BaseSemantics::SValuePtr unsignedMultiply(const BaseSemantics::SValuePtr &a_,
                                                      const BaseSemantics::SValuePtr &b_) override;
    virtual BaseSemantics::SValuePtr readMemory(RegisterDescriptor segreg,
                                                const BaseSemantics::SValuePtr &addr,
                                                const BaseSemantics::SValuePtr &dflt,
                                                const BaseSemantics::SValuePtr &cond) override;
    virtual BaseSemantics::SValuePtr peekMemory(RegisterDescriptor segreg,
                                                const BaseSemantics::SValuePtr &addr,
                                                const BaseSemantics::SValuePtr &dflt) override;
    virtual void writeMemory(RegisterDescriptor segreg,
                             const BaseSemantics::SValuePtr &addr,
                             const BaseSemantics::SValuePtr &data,
                             const BaseSemantics::SValuePtr &cond) override;

protected:
    BaseSemantics::SValuePtr readOrPeekMemory(RegisterDescriptor segreg, const BaseSemantics::SValuePtr &address,
                                              const BaseSemantics::SValuePtr &dflt_, bool allowSideEffects);
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
