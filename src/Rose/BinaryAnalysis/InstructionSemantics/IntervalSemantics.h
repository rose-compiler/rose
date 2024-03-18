#ifndef ROSE_BinaryAnalysis_InstructionSemantics_IntervalSemantics_H
#define ROSE_BinaryAnalysis_InstructionSemantics_IntervalSemantics_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/BasicTypes.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/MemoryCellList.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/SValue.h>

#include <Sawyer/IntervalSet.h>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {

/** An interval analysis semantic domain. */
namespace IntervalSemantics {

/* Single contiguous interval. */
typedef Sawyer::Container::Interval<uint64_t> Interval;

/** Set of intervals. */
typedef Sawyer::Container::IntervalSet<Interval> Intervals;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Semantic values
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Shared-ownership pointer to an interval semantic value. */
typedef Sawyer::SharedPointer<class SValue> SValuePtr;

/** Type of values manipulated by the IntervalSemantics domain. */
class SValue: public BaseSemantics::SValue {
public:
    /** Base type. */
    using Super = BaseSemantics::SValue;

    /** Shared-ownership pointer. */
    using Ptr = SValuePtr;

protected:
    Intervals intervals_;
    bool isBottom_;

protected:
    // Protected constructors. See base class and public members for documentation
    explicit SValue(size_t nbits);
    SValue(size_t nbits, uint64_t number);
    SValue(size_t nbits, uint64_t v1, uint64_t v2);
    SValue(size_t nbits, const Intervals &intervals);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors
public:
    /** Instantiate a new prototypical value. Prototypical values are only used for their virtual constructors. */
    static SValuePtr instance();

    /** Instantiate a new data-flow-bottom value of specified width. */
    static SValuePtr instance_bottom(size_t nbits);

    /** Instantiate a new undefined value of particular width.  Currently, there is no distinction between an unspecified
     *  value, an undefined value, and an interval that can represent any value of the specified size. */
    static SValuePtr instance_undefined(size_t nbits);

    /** Instantiate a new unspecified value of specific width.
     *
     *  Currently, there is no distinction between an unspecified value, an undefined value, and an interval that can represent
     *  any value of the specified size. */
    static SValuePtr instance_unspecified(size_t nbits);

    /** Instantiate a new concrete value of particular width. */
    static SValuePtr instance_integer(size_t nbits, uint64_t number);

    /** Instantiate a new value from a set of intervals. */
    static SValuePtr instance_intervals(size_t nbits, const Intervals &intervals);

    /** Instantiate a new value that's constrained to be between two unsigned values, inclusive. */
    static SValuePtr instance_hull(size_t nbits, uint64_t v1, uint64_t v2);

    /** Instantiate a new copy of an existing value. */
    static SValuePtr instance_copy(const SValuePtr&);

    /** Create a value from a set of possible bits. */
    static SValuePtr instance_from_bits(size_t nbits, uint64_t possible_bits);

    /** Promote a base value to an IntevalSemantics value. The value @p v must have an IntervalSemantics::SValue dynamic type. */
    static SValuePtr promote(const BaseSemantics::SValuePtr&);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual allocating constructors inherited from the super class
public:
    virtual BaseSemantics::SValuePtr bottom_(size_t nbits) const override;
    virtual BaseSemantics::SValuePtr undefined_(size_t nbits) const override;
    virtual BaseSemantics::SValuePtr unspecified_(size_t nbits) const override;
    virtual BaseSemantics::SValuePtr number_(size_t nbits, uint64_t number) const override;
    virtual BaseSemantics::SValuePtr copy(size_t new_width=0) const override;

    virtual Sawyer::Optional<BaseSemantics::SValuePtr>
    createOptionalMerge(const BaseSemantics::SValuePtr &other, const BaseSemantics::MergerPtr&,
                        const SmtSolverPtr&) const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual allocating constructors first defined at this level of the class hierarchy
public:
    /** Construct a ValueType that's constrained to be between two unsigned values, inclusive. */
    virtual SValuePtr create(size_t nbits, uint64_t v1, uint64_t v2);

    /** Construct a ValueType from a rangemap. Note that this does not truncate the rangemap to contain only values that would
     *  be possible for the ValueType size--see unsignedExtend() for that. */
    virtual SValuePtr create(size_t nbits, const Intervals&);

    /** Generate ranges from bits. Given the set of bits that could be set, generate a range.  We have to be careful here
     *  because we could end up generating very large rangemaps: a rangemap where the high 31 bits could be set but the zero
     *  bit must be cleared would create a rangemap with 2^31 singleton entries. */
    virtual SValuePtr create_from_bits(size_t nbits, uint64_t possible_bits);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Override virtual methods...
public:
    virtual void hash(Combinatorics::Hasher&) const override;
    virtual bool isBottom() const override;
    virtual void print(std::ostream &output, BaseSemantics::Formatter&) const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Override legacy methods. Override these, but always call the camelCase versions from BaseSemantics::SValue. These
    // snake_case names may eventually go away, so be sure to make good use of "override" in your own code.
public:
    // See mayEqual
    virtual bool may_equal(const BaseSemantics::SValuePtr &other,
                           const SmtSolverPtr &solver = SmtSolverPtr()) const override;

    // See mustEqual
    virtual bool must_equal(const BaseSemantics::SValuePtr &other,
                            const SmtSolverPtr &solver = SmtSolverPtr()) const override;

    // See isConcrete
    virtual bool is_number() const override;

    // See toUnsigned and toSigned
    virtual uint64_t get_number() const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Additional methods introduced at this level of the class hierarchy
public:
    /** Returns the rangemap stored in this value. */
    const Intervals& get_intervals() const;

    /** Changes the rangemap stored in the value. */
    void set_intervals(const Intervals&);

    /** Returns all possible bits that could be set. */
    uint64_t possible_bits() const;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Register state
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef BaseSemantics::RegisterStateGeneric RegisterState;
typedef BaseSemantics::RegisterStateGenericPtr RegisterStateGenericPtr;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Memory state
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Shared-ownership pointer to an interval memory state. */
typedef boost::shared_ptr<class MemoryState> MemoryStatePtr;

/** Byte-addressable memory.
 *
 *  This class represents an entire state of memory via a list of memory cells.  The memory cell list is sorted in reverse
 *  chronological order and addresses that satisfy a "must-alias" predicate are pruned so that only the must recent such memory
 *  cell is in the table.
 *
 *  A memory write operation prunes away any existing memory cell that must-alias the newly written address, then adds a new
 *  memory cell to the front of the memory cell list.
 *
 *  A memory read operation scans the memory cell list and returns the union of all possible matches. */
class MemoryState: public BaseSemantics::MemoryCellList {
public:
    /** Base type. */
    using Super = BaseSemantics::MemoryCellList;

    /** Shared-ownership pointer. */
    using Ptr = MemoryStatePtr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    MemoryState(const BaseSemantics::MemoryCellPtr &protocell);
    MemoryState(const BaseSemantics::SValuePtr &addrProtoval, const BaseSemantics::SValuePtr &valProtoval);
    MemoryState(const MemoryState &);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors
public:
    /** Instantiate a new memory state with specified prototypical cells and values. */
    static MemoryStatePtr instance(const BaseSemantics::MemoryCellPtr &protocell);

    /** Instantiate a new memory state with prototypical value. This constructor uses BaseSemantics::MemoryCell as the cell
     * type. The address protoval and value protoval are usually the same (or at least the same dynamic type). */
    static  MemoryStatePtr instance(const BaseSemantics::SValuePtr &addrProtoval, const BaseSemantics::SValuePtr &valProtoval);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors
public:
    virtual BaseSemantics::MemoryStatePtr create(const BaseSemantics::MemoryCellPtr &protocell) const override;
    virtual BaseSemantics::MemoryStatePtr create(const BaseSemantics::SValuePtr &addrProtoval,
                                                 const BaseSemantics::SValuePtr &valProtoval) const;
    virtual BaseSemantics::MemoryStatePtr clone() const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods we inherited
public:
    /** Read a byte from memory.
     *
     *  In order to read a multi-byte value, use RiscOperators::readMemory(). */
    virtual BaseSemantics::SValuePtr
    readMemory(const BaseSemantics::SValuePtr &addr, const BaseSemantics::SValuePtr &dflt,
               BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps) override;

    /** Read a byte from memory without side effects.
     *
     *  In order to read a multi-byte value, use RiscOperators::peekMemory(). */
    virtual BaseSemantics::SValuePtr
    peekMemory(const BaseSemantics::SValuePtr &addr, const BaseSemantics::SValuePtr &dflt,
               BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps) override;

    /** Write a byte to memory.
     *
     *  In order to write a multi-byte value, use RiscOperators::writeMemory(). */
    virtual void writeMemory(const BaseSemantics::SValuePtr &addr, const BaseSemantics::SValuePtr &value,
                             BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps) override;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Complete state
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef BaseSemantics::State State;
typedef BaseSemantics::StatePtr StatePtr;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      RISC operators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Shared-ownership pointer to interval RISC operations. */
typedef boost::shared_ptr<class RiscOperators> RiscOperatorsPtr;

/** RISC operators for interval domains. */
class RiscOperators: public BaseSemantics::RiscOperators {
public:
    /** Base type. */
    using Super = BaseSemantics::RiscOperators;

    /** Shared-ownership pointer. */
    using Ptr = RiscOperatorsPtr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    explicit RiscOperators(const BaseSemantics::SValuePtr &protoval, const SmtSolverPtr&);

    explicit RiscOperators(const BaseSemantics::StatePtr&, const SmtSolverPtr&);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors
public:
    /** Instantiates a new RiscOperators object and configures it to use semantic values and states that are defaults for
     *  IntervalSemantics. */
    static RiscOperatorsPtr instanceFromRegisters(const RegisterDictionaryPtr&, const SmtSolverPtr &solver = SmtSolverPtr());

    /** Instantiates a new RiscOperators object with specified prototypical value. An SMT solver may be specified as the second
     *  argument for convenience. See @ref solver for details. */
    static RiscOperatorsPtr instanceFromProtoval(const BaseSemantics::SValuePtr &protoval,
                                                 const SmtSolverPtr &solver = SmtSolverPtr());

    /** Instantiates a new RiscOperators with specified state. An SMT solver may be specified as the second argument for
     *  convenience. See @ref solver for details. */
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
    /** Run-time promotion of a base RiscOperators pointer to interval operators. This is a checked conversion--it
     *  will fail if @p from does not point to a IntervalSemantics::RiscOperators object. */
    static RiscOperatorsPtr promote(const BaseSemantics::RiscOperatorsPtr&);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods first introduced at this level of the class hierarchy.
public:

    /** Create a new SValue from a set of possible bits.  This is just a convience function so that we don't have to
     *  see so many dynamic casts in the source code. */
    virtual SValuePtr svalue_from_bits(size_t nbits, uint64_t possible_bits);

    /** Create a new SValue from a set of intervals.  This is just a convience function so that we don't have to
     *  see so many dynamic casts in the source code. */
    virtual SValuePtr svalue_from_intervals(size_t nbits, const Intervals &intervals);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Override methods from base class.  These are the RISC operators that are invoked by a Dispatcher.
public:
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
    virtual BaseSemantics::SValuePtr unsignedExtend(const BaseSemantics::SValuePtr &a_, size_t new_width) override;
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
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
