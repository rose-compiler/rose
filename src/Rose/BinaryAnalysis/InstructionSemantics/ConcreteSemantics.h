#ifndef ROSE_BinaryAnalysis_InstructionSemantics_ConcreteSemantics_H
#define ROSE_BinaryAnalysis_InstructionSemantics_ConcreteSemantics_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/BasicTypes.h>

#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics.h>
#include <Rose/BinaryAnalysis/MemoryMap.h>

#include <Sawyer/BitVector.h>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {

/** A concrete semantic domain.
 *
 *  Semantics in a concrete domain, where all values are actual known bits.  Since the symbolic domain does constant folding,
 *  it also can be used as a concrete domain, although using ConcreteSemantics is probably somewhat faster. */
namespace ConcreteSemantics {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Value type
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Smart-ownership pointer to a concrete semantic value. */
typedef Sawyer::SharedPointer<class SValue> SValuePtr;

/** Formatter for symbolic values. */
typedef BaseSemantics::Formatter Formatter;             // we might extend this in the future

/** Type of values manipulated by the concrete domain.
 *
 *  Values of type type are used whenever a value needs to be stored, such as memory addresses, the values stored at those
 *  addresses, the values stored in registers, the operands for RISC operations, and the results of those operations.  Each
 *  value has a known size and known bits.  All RISC operations on these values will produce a new known value. This type is
 *  not capable of storing an undefined value, and any attempt to create an undefined value will create a value with all bits
 *  cleared instead. */
class SValue: public BaseSemantics::SValue {
public:
    /** Base type. */
    using Super = BaseSemantics::SValue;

    /** Shared-ownership pointer. */
    using Ptr = SValuePtr;

protected:
    Sawyer::Container::BitVector bits_;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    explicit SValue(size_t nbits);
    SValue(size_t nbits, uint64_t number);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors
public:
    /** Instantiate a new prototypical value. Prototypical values are only used for their virtual constructors. */
    static SValuePtr instance();

    /** Instantiate a new undefined value of specified width.
     *
     *  This semantic domain has no representation for an undefined values--all values are defined. Therefore any attempt to
     *  create an undefined value will result in all bits being set to zero instead. */
    static SValuePtr instance(size_t nbits);

    /** Instantiate a new concrete value. */
    static SValuePtr instance(size_t nbits, uint64_t value);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual allocating constructors
public:
    virtual BaseSemantics::SValuePtr undefined_(size_t nbits) const override;
    virtual BaseSemantics::SValuePtr unspecified_(size_t nbits) const override;
    virtual BaseSemantics::SValuePtr bottom_(size_t nbits) const override;
    virtual BaseSemantics::SValuePtr number_(size_t nbits, uint64_t value) const override;
    virtual BaseSemantics::SValuePtr boolean_(bool value) const override;
    virtual BaseSemantics::SValuePtr copy(size_t new_width=0) const override;

    virtual Sawyer::Optional<BaseSemantics::SValuePtr>
    createOptionalMerge(const BaseSemantics::SValuePtr &other, const BaseSemantics::MergerPtr&,
                        const SmtSolverPtr&) const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casts
public:
    /** Promote a base value to a SymbolicSemantics value.  The value @p v must have a SymbolicSemantics::SValue dynamic type. */
    static SValuePtr promote(const BaseSemantics::SValuePtr&);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Override virtual methods...
public:
    virtual void hash(Combinatorics::Hasher&) const override;
    virtual bool isBottom() const override;
    virtual void print(std::ostream&, BaseSemantics::Formatter&) const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Override legacy virtual methods. These snake_case names may eventually go away, but for now they're the ones you should
    // override. Be sure to use "override" in your own code in order to be notified when we finally remove these.
public:
    // See mayEqual
    virtual bool may_equal(const BaseSemantics::SValuePtr &other,
                           const SmtSolverPtr &solver = SmtSolverPtr()) const override;

    // See mustEqual
    virtual bool must_equal(const BaseSemantics::SValuePtr &other,
                            const SmtSolverPtr &solver = SmtSolverPtr()) const override;

    // See nBits
    virtual void set_width(size_t nbits) override;

    // See isConcrete
    virtual bool is_number() const override {
        return true;
    }

    // See toUnsigned and toSigned
    virtual uint64_t get_number() const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Additional methods first declared in this class...
public:
    /** Returns the bit vector storing the concrete value.
     *
     * @{ */
    virtual const Sawyer::Container::BitVector& bits() const { return bits_; }
    virtual void bits(const Sawyer::Container::BitVector&);
    /** @} */
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Register State
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef BaseSemantics::RegisterStateGeneric RegisterState;
typedef BaseSemantics::RegisterStateGenericPtr RegisterStatePtr;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Memory State
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Shared-ownership pointer to a concrete memory state. */
typedef boost::shared_ptr<class MemoryState> MemoryStatePtr;

/** Byte-addressable memory.
 *
 *  This class represents an entire state of memory via MemoryMap, allocating new memory in units of pages (the size of a page
 *  is configurable. */
class MemoryState: public BaseSemantics::MemoryState {
public:
    /** Base type. */
    using Super = BaseSemantics::MemoryState;

    /** Shared-ownership pointer. */
    using Ptr = MemoryStatePtr;

private:
    MemoryMap::Ptr map_;
    Address pageSize_;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    explicit MemoryState(const BaseSemantics::SValuePtr &addrProtoval, const BaseSemantics::SValuePtr &valProtoval);
    MemoryState(const MemoryState &other);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors
public:
    /** Instantiates a new memory state having specified prototypical value.
     *
     *  The @p addrProtoval and @p valProtoval must both be of ConcreteSemantics::SValue type or derived classes. */
    static  MemoryStatePtr instance(const BaseSemantics::SValuePtr &addrProtoval, const BaseSemantics::SValuePtr &valProtoval);

    /** Instantiates a new deep copy of an existing state.
     *
     *  For efficiency purposes, the data buffers are not copied immediately but rather marked as copy-on-write. However, the
     *  newly constructed memory map will have its own segments, which hold the segment names, access permissions, etc. */
    static MemoryStatePtr instance(const MemoryStatePtr &other);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors
public:
    /** Virtual constructor.
     *
     *  Creates a memory state having specified prototypical value, which should be of type ConcreteSemantics::SValue or
     *  subclasses. */
    virtual BaseSemantics::MemoryStatePtr create(const BaseSemantics::SValuePtr &addrProtoval,
                                                 const BaseSemantics::SValuePtr &valProtoval) const override;

    /** Virtual copy constructor.
     *
     *  Creates a new deep copy of this memory state. For efficiency purposes, the data buffers are not copied immediately but
     *  rather marked as copy-on-write.  However, the newly constructed memory map will have its own segments, which hold the
     *  segment names, access permissions, etc. */
    virtual BaseSemantics::AddressSpacePtr clone() const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casts
public:
    /** Recasts a base pointer to a concrete memory state. This is a checked cast that will fail if the specified pointer does
     *  not have a run-time type that is a ConcreteSemantics::MemoryState or subclass thereof. */
    static MemoryStatePtr promote(const BaseSemantics::AddressSpacePtr&);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods we inherited
public:
    virtual void clear() override;

    virtual void hash(Combinatorics::Hasher&, BaseSemantics::RiscOperators *addrOps,
                      BaseSemantics::RiscOperators *valOps) const override;

    virtual void print(std::ostream&, Formatter&) const override;

    virtual BaseSemantics::SValuePtr readMemory(const BaseSemantics::SValuePtr &addr, const BaseSemantics::SValuePtr &dflt,
                                                BaseSemantics::RiscOperators *addrOps,
                                                BaseSemantics::RiscOperators *valOps) override;

    virtual BaseSemantics::SValuePtr peekMemory(const BaseSemantics::SValuePtr &addr, const BaseSemantics::SValuePtr &dflt,
                                                BaseSemantics::RiscOperators *addrOps,
                                                BaseSemantics::RiscOperators *valOps) override;

    virtual void writeMemory(const BaseSemantics::SValuePtr &addr, const BaseSemantics::SValuePtr &value,
                             BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps) override;

    virtual bool merge(const BaseSemantics::AddressSpacePtr &other, BaseSemantics::RiscOperators *addrOps,
                       BaseSemantics::RiscOperators *valOps) override;

protected:
    BaseSemantics::SValuePtr readOrPeekMemory(const BaseSemantics::SValuePtr &addr,
                                              const BaseSemantics::SValuePtr &dflt,
                                              BaseSemantics::RiscOperators *addrOps,
                                              BaseSemantics::RiscOperators *valOps,
                                              bool allowSideEffects);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods first declared in this class
public:
    /** Returns the memory map. */
    const MemoryMap::Ptr memoryMap() const { return map_; }

    /** Set memory map.
     *
     *  If the specified map's areas are not in units of pages then padding segments will be added to this memory state. The
     *  padding segments will either have the accessibility specified by @p padAccess, or will have the same accessibility as
     *  the memory region being padded.  All padding segments will be named "padding". */
    void memoryMap(const MemoryMap::Ptr&, Sawyer::Optional<unsigned> padAccess = Sawyer::Nothing());

    /** Size of each page of memory.
     *
     *  Memory is allocated in units of the page size and aligned on page-size boundaries.  The page size cannot be changed
     *  once the map contains data.
     *
     * @{ */
    Address pageSize() const { return pageSize_; }
    void pageSize(Address nBytes);
    /** @} */

    /** Allocate a page of memory.
     *
     *  The specified address will be contained in the page, which is aligned on a page boundary. Do not call this if the page
     *  is already allocated unless: it will replace the allocated page with a new one containing all zeros. */
    void allocatePage(Address va);

};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Complete semantic state
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef BaseSemantics::State State;
typedef BaseSemantics::StatePtr StatePtr;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      RISC operators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Shared-ownership pointer to concrete RISC operations. */
typedef boost::shared_ptr<class RiscOperators> RiscOperatorsPtr;

/** Defines RISC operators for the ConcreteSemantics domain.
 *
 *  These RISC operators depend on functionality introduced into the SValue class hierarchy at the ConcreteSemantics::SValue
 *  level. Therefore, the prototypical value supplied to the constructor or present in the supplied state object must have a
 *  dynamic type which is a ConcreteSemantics::SValue (or subclass).
 *
 *  Each RISC operator should return a newly allocated semantic value rather than trying to re-use an existing one. This will
 *  allow the caller to change the value stored there without affecting any of the input arguments. For example, a no-op that
 *  returns its argument should be implemented like this:
 *
 * @code
 *  BaseSemantics::SValuePtr noop(const BaseSemantics::SValuePtr &arg) {
 *      return arg->copy();     //correct
 *      return arg;             //incorrect
 *  }
 * @endcode
 */
class RiscOperators: public BaseSemantics::RiscOperators {
public:
    /** Base type. */
    using Super = BaseSemantics::RiscOperators;

    /** Shared-ownership pointer. */
    using Ptr = RiscOperatorsPtr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    RiscOperators(const BaseSemantics::SValuePtr &protoval, const SmtSolverPtr&);

    RiscOperators(const BaseSemantics::StatePtr&, const SmtSolverPtr&);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors
public:
    ~RiscOperators();

    /** Allocating constructor.
     *
     *  Instantiates a new object and configures it to use semantic values and states that are defaults for @ref
     *  ConcreteSemantics. */
    static RiscOperatorsPtr instanceFromRegisters(const RegisterDictionaryPtr&, const SmtSolverPtr &solver = SmtSolverPtr());

    /** Allocating constructor.
     *
     *  Instantiates a new object with specified prototypical values. An SMT solver may be specified as the second argument
     *  because the base class expects one, but it is not used for @ref ConcreteSemantics. See @ref SmtSolver for details. */
    static RiscOperatorsPtr instanceFromProtoval(const BaseSemantics::SValuePtr &protoval, const SmtSolverPtr &solver = SmtSolverPtr());

    /** Allocating constructor.
     *
     *  Instantiates a new RiscOperators object with specified state.  An SMT solver may be specified as the second argument
     *  because the base class expects one, but it is not used for concrete semantics. See @ref solver for details. */
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
    /** Run-time promotion of a base RiscOperators pointer to concrete operators. This is a checked conversion--it
     *  will fail if @p x does not point to a ConcreteSemantics::RiscOperators object. */
    static RiscOperatorsPtr promote(const BaseSemantics::RiscOperatorsPtr&);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // New methods for constructing values, so we don't have to write so many SValue::promote calls in the RiscOperators
    // implementations.
protected:
    SValuePtr svalueNumber(size_t nbits, uint64_t value);
    SValuePtr svalueNumber(const Sawyer::Container::BitVector&);
    SValuePtr svalueBoolean(bool b);
    SValuePtr svalueZero(size_t nbits);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Override methods from base class.  These are the RISC operators that are invoked by a Dispatcher.
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

    virtual BaseSemantics::SValuePtr fpFromInteger(const BaseSemantics::SValuePtr &intValue, SgAsmFloatType*) override;
    virtual BaseSemantics::SValuePtr fpToInteger(const BaseSemantics::SValuePtr &fpValue, SgAsmFloatType *fpType,
                                                 const BaseSemantics::SValuePtr &dflt) override;
    virtual BaseSemantics::SValuePtr fpAdd(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b,
                                           SgAsmFloatType*) override;
    virtual BaseSemantics::SValuePtr fpSubtract(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b,
                                                SgAsmFloatType*) override;
    virtual BaseSemantics::SValuePtr fpMultiply(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b,
                                                SgAsmFloatType*) override;
    virtual BaseSemantics::SValuePtr fpRoundTowardZero(const BaseSemantics::SValuePtr &a, SgAsmFloatType*) override;

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
    // handles readMemory and peekMemory
    BaseSemantics::SValuePtr readOrPeekMemory(RegisterDescriptor segreg, const BaseSemantics::SValuePtr &address,
                                              const BaseSemantics::SValuePtr &dflt, bool allowSideEffects);

    // Convert expression to double
    double exprToDouble(const BaseSemantics::SValuePtr &expr, SgAsmFloatType*);

    // Convert double to expression
    BaseSemantics::SValuePtr doubleToExpr(double d, SgAsmFloatType*);
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
