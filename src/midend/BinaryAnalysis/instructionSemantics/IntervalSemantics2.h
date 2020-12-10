#ifndef Rose_IntervalSemantics_H
#define Rose_IntervalSemantics_H
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include <stdint.h>

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>

#include "BaseSemantics2.h"
#include "integerOps.h"
#include "RegisterStateGeneric.h"
#include "MemoryCellList.h"

namespace Rose {
namespace BinaryAnalysis {              // documented elsewhere
namespace InstructionSemantics2 {       // documented elsewhere


/** An interval analysis semantic domain. */
namespace IntervalSemantics {

/* Single contiguous interval. */
typedef Sawyer::Container::Interval<uint64_t> Interval;

/** Set of intervals. */
typedef Sawyer::Container::IntervalSet<Interval> Intervals;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Semantic values
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Shared-ownership pointer to an interval semantic value. See @ref heap_object_shared_ownership. */
typedef Sawyer::SharedPointer<class SValue> SValuePtr;

/** Type of values manipulated by the IntervalSemantics domain. */
class SValue: public BaseSemantics::SValue {
protected:
    Intervals intervals_;
    bool isBottom_;

protected:
    // Protected constructors. See base class and public members for documentation
    explicit SValue(size_t nbits):
        BaseSemantics::SValue(nbits), isBottom_(false){
        intervals_.insert(Interval::hull(0, IntegerOps::genMask<uint64_t>(nbits)));
    }
    SValue(size_t nbits, uint64_t number)
        : BaseSemantics::SValue(nbits), isBottom_(false) {
        number &= IntegerOps::genMask<uint64_t>(nbits);
        intervals_.insert(number);
    }
    SValue(size_t nbits, uint64_t v1, uint64_t v2):
        BaseSemantics::SValue(nbits), isBottom_(false) {
        v1 &= IntegerOps::genMask<uint64_t>(nbits);
        v2 &= IntegerOps::genMask<uint64_t>(nbits);
        ASSERT_require(v1<=v2);
        intervals_.insert(Interval::hull(v1, v2));
    }
    SValue(size_t nbits, const Intervals &intervals):
        BaseSemantics::SValue(nbits), isBottom_(false) {
        ASSERT_require(!intervals.isEmpty());
        ASSERT_require((intervals.greatest() <= IntegerOps::genMask<uint64_t>(nbits)));
        intervals_ = intervals;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors
public:
    /** Instantiate a new prototypical value. Prototypical values are only used for their virtual constructors. */
    static SValuePtr instance() {
        return SValuePtr(new SValue(1));
    }

    /** Instantiate a new data-flow-bottom value of specified width. */
    static SValuePtr instance_bottom(size_t nbits) {
        SValue *self = new SValue(nbits);
        self->isBottom_ = true;
        return SValuePtr(self);
    }

    /** Instantiate a new undefined value of particular width.  Currently, there is no distinction between an unspecified
     *  value, an undefined value, and an interval that can represent any value of the specified size. */
    static SValuePtr instance_undefined(size_t nbits) {
        return SValuePtr(new SValue(nbits));
    }

    /** Instantiate a new unspecified value of specific width.
     *
     *  Currently, there is no distinction between an unspecified value, an undefined value, and an interval that can represent
     *  any value of the specified size. */
    static SValuePtr instance_unspecified(size_t nbits) {
        return SValuePtr(new SValue(nbits));
    }

    /** Instantiate a new concrete value of particular width. */
    static SValuePtr instance_integer(size_t nbits, uint64_t number) {
        return SValuePtr(new SValue(nbits, number));
    }

    /** Instantiate a new value from a set of intervals. */
    static SValuePtr instance_intervals(size_t nbits, const Intervals &intervals) {
        return SValuePtr(new SValue(nbits, intervals));
    }

    /** Instantiate a new value that's constrained to be between two unsigned values, inclusive. */
    static SValuePtr instance_hull(size_t nbits, uint64_t v1, uint64_t v2) {
        return SValuePtr(new SValue(nbits, v1, v2));
    }

    /** Instantiate a new copy of an existing value. */
    static SValuePtr instance_copy(const SValuePtr &other) {
        return SValuePtr(new SValue(*other));
    }
    
    /** Create a value from a set of possible bits. */
    static SValuePtr instance_from_bits(size_t nbits, uint64_t possible_bits);

    /** Promote a base value to an IntevalSemantics value. The value @p v must have an IntervalSemantics::SValue dynamic type. */
    static SValuePtr promote(const BaseSemantics::SValuePtr &v) { // hot
        SValuePtr retval = v.dynamicCast<SValue>();
        ASSERT_not_null(retval);
        return retval;
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual allocating constructors inherited from the super class
public:
    virtual BaseSemantics::SValuePtr bottom_(size_t nbits) const ROSE_OVERRIDE {
        return instance_bottom(nbits);
    }
    virtual BaseSemantics::SValuePtr undefined_(size_t nbits) const ROSE_OVERRIDE {
        return instance_undefined(nbits);
    }
    virtual BaseSemantics::SValuePtr unspecified_(size_t nbits) const ROSE_OVERRIDE {
        return instance_unspecified(nbits);
    }

    virtual BaseSemantics::SValuePtr number_(size_t nbits, uint64_t number) const ROSE_OVERRIDE {
        return instance_integer(nbits, number);
    }
    virtual BaseSemantics::SValuePtr copy(size_t new_width=0) const ROSE_OVERRIDE {
        SValuePtr retval(new SValue(*this));
        if (new_width!=0 && new_width!=retval->get_width())
            retval->set_width(new_width);
        return retval;
    }

    virtual Sawyer::Optional<BaseSemantics::SValuePtr>
    createOptionalMerge(const BaseSemantics::SValuePtr &other, const BaseSemantics::MergerPtr&,
                        const SmtSolverPtr&) const ROSE_OVERRIDE;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual allocating constructors first defined at this level of the class hierarchy
public:
    /** Construct a ValueType that's constrained to be between two unsigned values, inclusive. */
    virtual SValuePtr create(size_t nbits, uint64_t v1, uint64_t v2) {
        return instance_hull(nbits, v1, v2);
    }

    /** Construct a ValueType from a rangemap. Note that this does not truncate the rangemap to contain only values that would
     *  be possible for the ValueType size--see unsignedExtend() for that. */
    virtual SValuePtr create(size_t nbits, const Intervals &intervals) {
        return instance_intervals(nbits, intervals); 
    }

    /** Generate ranges from bits. Given the set of bits that could be set, generate a range.  We have to be careful here
     *  because we could end up generating very large rangemaps: a rangemap where the high 31 bits could be set but the zero
     *  bit must be cleared would create a rangemap with 2^31 singleton entries. */
    virtual SValuePtr create_from_bits(size_t nbits, uint64_t possible_bits) {
        return instance_from_bits(nbits, possible_bits);
    }
            

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Override virtual methods...
public:
    virtual bool may_equal(const BaseSemantics::SValuePtr &other,
                           const SmtSolverPtr &solver = SmtSolverPtr()) const ROSE_OVERRIDE;
    virtual bool must_equal(const BaseSemantics::SValuePtr &other,
                            const SmtSolverPtr &solver = SmtSolverPtr()) const ROSE_OVERRIDE;

    virtual bool isBottom() const ROSE_OVERRIDE {
        return isBottom_;
    }

    virtual bool is_number() const ROSE_OVERRIDE {
        return 1==intervals_.size();
    }
    
    virtual uint64_t get_number() const ROSE_OVERRIDE {
        ASSERT_require(1==intervals_.size());
        return intervals_.least();
    }

    virtual void print(std::ostream &output, BaseSemantics::Formatter&) const ROSE_OVERRIDE;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Additional methods introduced at this level of the class hierarchy
public:
    /** Returns the rangemap stored in this value. */
    const Intervals& get_intervals() const {
        return intervals_;
    }

    /** Changes the rangemap stored in the value. */
    void set_intervals(const Intervals &intervals) {
        intervals_ = intervals;
    }

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

/** Shared-ownership pointer to an interval memory state. See @ref heap_object_shared_ownership. */
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

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    MemoryState(const BaseSemantics::MemoryCellPtr &protocell)
        : BaseSemantics::MemoryCellList(protocell) {}

    MemoryState(const BaseSemantics::SValuePtr &addrProtoval, const BaseSemantics::SValuePtr &valProtoval)
        : BaseSemantics::MemoryCellList(addrProtoval, valProtoval) {}

    MemoryState(const MemoryState &other)
        : BaseSemantics::MemoryCellList(other) {}

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors
public:
    /** Instantiate a new memory state with specified prototypical cells and values. */
    static MemoryStatePtr instance(const BaseSemantics::MemoryCellPtr &protocell) {
        return MemoryStatePtr(new MemoryState(protocell));
    }

    /** Instantiate a new memory state with prototypical value. This constructor uses BaseSemantics::MemoryCell as the cell
     * type. The address protoval and value protoval are usually the same (or at least the same dynamic type). */
    static  MemoryStatePtr instance(const BaseSemantics::SValuePtr &addrProtoval, const BaseSemantics::SValuePtr &valProtoval) {
        return MemoryStatePtr(new MemoryState(addrProtoval, valProtoval));
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors
public:
    virtual BaseSemantics::MemoryStatePtr create(const BaseSemantics::MemoryCellPtr &protocell) const ROSE_OVERRIDE {
        return instance(protocell);
    }

    virtual BaseSemantics::MemoryStatePtr create(const BaseSemantics::SValuePtr &addrProtoval,
                                                 const BaseSemantics::SValuePtr &valProtoval) const ROSE_OVERRIDE {
        return instance(addrProtoval, valProtoval);
    }

    virtual BaseSemantics::MemoryStatePtr clone() const ROSE_OVERRIDE {
        return MemoryStatePtr(new MemoryState(*this));
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods we inherited
public:
    /** Read a byte from memory.
     *
     *  In order to read a multi-byte value, use RiscOperators::readMemory(). */
    virtual BaseSemantics::SValuePtr
    readMemory(const BaseSemantics::SValuePtr &addr, const BaseSemantics::SValuePtr &dflt,
               BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps) ROSE_OVERRIDE;

    /** Read a byte from memory without side effects.
     *
     *  In order to read a multi-byte value, use RiscOperators::peekMemory(). */
    virtual BaseSemantics::SValuePtr
    peekMemory(const BaseSemantics::SValuePtr &addr, const BaseSemantics::SValuePtr &dflt,
               BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps) ROSE_OVERRIDE;

    /** Write a byte to memory.
     *
     *  In order to write a multi-byte value, use RiscOperators::writeMemory(). */
    virtual void writeMemory(const BaseSemantics::SValuePtr &addr, const BaseSemantics::SValuePtr &value,
                             BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps) ROSE_OVERRIDE;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Complete state
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef BaseSemantics::State State;
typedef BaseSemantics::StatePtr StatePtr;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      RISC operators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Shared-ownership pointer to interval RISC operations. See @ref heap_object_shared_ownership. */
typedef boost::shared_ptr<class RiscOperators> RiscOperatorsPtr;

/** RISC operators for interval domains. */
class RiscOperators: public BaseSemantics::RiscOperators {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    explicit RiscOperators(const BaseSemantics::SValuePtr &protoval, const SmtSolverPtr &solver = SmtSolverPtr())
        : BaseSemantics::RiscOperators(protoval, solver) {
        name("Interval");
        (void) SValue::promote(protoval); // make sure its dynamic type is an IntervalSemantics::SValue or subclass thereof
    }

    explicit RiscOperators(const BaseSemantics::StatePtr &state, const SmtSolverPtr &solver = SmtSolverPtr())
        : BaseSemantics::RiscOperators(state, solver) {
        name("Interval");
        (void) SValue::promote(state->protoval());      // dynamic type must be IntervalSemantics::SValue or subclass thereof
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors
public:
    /** Instantiates a new RiscOperators object and configures it to use semantic values and states that are defaults for
     *  IntervalSemantics. */
    static RiscOperatorsPtr instance(const RegisterDictionary *regdict, const SmtSolverPtr &solver = SmtSolverPtr()) {
        BaseSemantics::SValuePtr protoval = SValue::instance();
        BaseSemantics::RegisterStatePtr registers = RegisterState::instance(protoval, regdict);
        BaseSemantics::MemoryStatePtr memory = MemoryState::instance(protoval, protoval);
        BaseSemantics::StatePtr state = State::instance(registers, memory);
        return RiscOperatorsPtr(new RiscOperators(state, solver));
    }

    /** Instantiates a new RiscOperators object with specified prototypical value. An SMT solver may be specified as the second
     *  argument for convenience. See @ref solver for details. */
    static RiscOperatorsPtr instance(const BaseSemantics::SValuePtr &protoval, const SmtSolverPtr &solver = SmtSolverPtr()) {
        return RiscOperatorsPtr(new RiscOperators(protoval, solver));
    }

    /** Instantiates a new RiscOperators with specified state. An SMT solver may be specified as the second argument for
     *  convenience. See @ref solver for details. */
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
    /** Run-time promotion of a base RiscOperators pointer to interval operators. This is a checked conversion--it
     *  will fail if @p from does not point to a IntervalSemantics::RiscOperators object. */
    static RiscOperatorsPtr promote(const BaseSemantics::RiscOperatorsPtr &x) {
        RiscOperatorsPtr retval = boost::dynamic_pointer_cast<RiscOperators>(x);
        ASSERT_not_null(retval);
        return retval;
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods first introduced at this level of the class hierarchy.
public:

    /** Create a new SValue from a set of possible bits.  This is just a convience function so that we don't have to
     *  see so many dynamic casts in the source code. */
    virtual SValuePtr svalue_from_bits(size_t nbits, uint64_t possible_bits) {
        return SValue::promote(protoval())->create_from_bits(nbits, possible_bits);
    }

    /** Create a new SValue from a set of intervals.  This is just a convience function so that we don't have to
     *  see so many dynamic casts in the source code. */
    virtual SValuePtr svalue_from_intervals(size_t nbits, const Intervals &intervals) {
        return SValue::promote(protoval())->create(nbits, intervals);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Override methods from base class.  These are the RISC operators that are invoked by a Dispatcher.
public:
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
    virtual BaseSemantics::SValuePtr unsignedExtend(const BaseSemantics::SValuePtr &a_, size_t new_width) ROSE_OVERRIDE;
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
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
