#ifndef ROSE_BinaryAnalysis_CallingConvention_StoragePool_H
#define ROSE_BinaryAnalysis_CallingConvention_StoragePool_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/CallingConvention/BasicTypes.h>

#include <Rose/BinaryAnalysis/Address.h>
#include <Rose/BinaryAnalysis/Alignment.h>
#include <Rose/BinaryAnalysis/ByteOrder.h>
#include <Rose/BinaryAnalysis/ConcreteLocation.h>

#include <string>
#include <vector>

class SgAsmType;

namespace Rose {
namespace BinaryAnalysis {
namespace CallingConvention {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Base class for type predicates. */
class TypePredicate {
public:
    /** Shared-ownership pointer.
     *
     *  @{ */
    using Ptr = TypePredicatePtr;
    using ConstPtr = TypePredicateConstPtr;
    /** @} */

public:
    virtual ~TypePredicate();
protected:
    TypePredicate();

public:
    /** Evaluate this predicate on a type.
     *
     * @{ */
    virtual bool test(SgAsmType*) const = 0;
    bool operator()(SgAsmType*) const;
    /** @} */
};

template<class F>
class TypePredicateFunctor: public TypePredicate {
    F f;
public:
    TypePredicateFunctor(F f)
        : f(f) {}

    bool test(SgAsmType *type) const override {
        return f(type);
    }
};

/** Functor returning true for any non-null type. */
TypePredicate::Ptr isAnyType();

/** Functor returning true for floating-point types not wider than specified. */
TypePredicate::Ptr isFpNotWiderThan(size_t nBits);

/** Functor returning true for integer types not wider than specified. */
TypePredicate::Ptr isIntegerNotWiderThan(size_t nBits);

/** Functor returning true for pointers not wider than specified. */
TypePredicate::Ptr isPointerNotWiderThan(size_t nBits);

/** Functor returning true for non-floating-point types not wider than specified. */
TypePredicate::Ptr isNonFpNotWiderThan(size_t nBits);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Base class for pool of locations.
 *
 *  A storage pool contains an ordered sequence of locations and a predicate to determine whether the locations are able to hold a
 *  value of the specified type. */
class StoragePoolBase {
public:
    /** Shared-ownership pointer.
     *
     * @{ */
    using Ptr = StoragePoolBasePtr;
    using ConstPtr = StoragePoolBaseConstPtr;
    /** @} */

private:
    std::string name_;                                  // name for debugging
    TypePredicateConstPtr canStore_;                    // predicate for types that the locations can store

public:
    ~StoragePoolBase();
protected:
    StoragePoolBase() = delete;
    StoragePoolBase(const std::string &name, const TypePredicateConstPtr&);

public:
    /** Copy constructor.
     *
     *  Makes a new copy of this pool. */
    virtual Ptr copy() const = 0;

    /** Test whether locations can store type.
     *
     *  If this pool contains locations that are capable of storing an instance of the specified type, then this predicate returns
     *  true, else false. */
    virtual bool canStore(SgAsmType*) const;

    /** Reset allocation.
     *
     *  Resets this storage pool back to its initial state. For instance, if the storage pool contains a list of three explicit
     *  locations and some or all of them have been taken by @ref consume, then calling this function will make all three available
     *  again. */
    virtual void reset() = 0;

    /** Consume a storage location.
     *
     *  Given an argument type, return a location(s) if possible and remove that location from this pool. */
    virtual std::vector<ConcreteLocation> consume(SgAsmType*) = 0;

    /** List of constant storage locations.
     *
     *  A constant location is any location whose address is known and is a constant. For example, a @ref ConcreteLocation
     *  describing a particular register or memory location is a constant location, but a @ref ConcreteLocation that describes a
     *  memory address which is stored in a register or a memory address that is the sum of a register value and an offset is not
     *  constant.
     *
     *  The constant values are used by the calling convention @ref Analysis to check whether the callee reads from any of these
     *  locations without first writing to them.
     *
     *  The base implementation returns an empty vector. */
    virtual std::vector<ConcreteLocation> constantLocations() const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** A storage pool for explicit locations.
 *
 *  This pool holds an ordered sequence of explicit (usually register) locations. */
class StoragePoolEnumerated: public StoragePoolBase {
public:
    /** Shared-ownership pointer.
     *
     * @{ */
    using Ptr = StoragePoolEnumeratedPtr;
    using ConstPtr = StoragePoolEnumeratedConstPtr;
    /** @} */

private:
    std::vector<ConcreteLocation> locations_;
    size_t current_ = 0;                                // points to the next available location in `locations_`

public:
    ~StoragePoolEnumerated();
protected:
    StoragePoolEnumerated() = delete;
    StoragePoolEnumerated(const std::string &name, const TypePredicateConstPtr&);

public:
    /** Constructor.
     *
     *  Return a new storage pool that can potentially satisfy requests for types for which the predicate returns true. The returned
     *  storage pool has no storage (see @ref append). */
    static Ptr instance(const std::string &name, const TypePredicateConstPtr&);

    /** Append an additional location.
     *
     *  An exception is thrown if the new location already exists in this pool. It is the caller's responsibility to ensure that
     *  the location is capable of storing instances of all types for which @ref canStore returns true. */
    void append(const ConcreteLocation&);

public:
    StoragePoolBasePtr copy() const override;
    std::vector<ConcreteLocation> constantLocations() const override;
    std::vector<ConcreteLocation> consume(SgAsmType*) override;
    void reset() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** A storage pool for stack-based argument locations.
 *
 *  This pool holds an ordered sequence of locations on the stack. Some of the stack properties come from the associated calling
 *  convention definition. */
class StoragePoolStack: public StoragePoolBase {
public:
    /** Shared-ownership pointer.
     *
     * @{ */
    using Ptr = StoragePoolStackPtr;
    using ConstPtr = StoragePoolStackConstPtr;
    /** @} */

private:
    StackDirection stackDirection_ = StackDirection::GROWS_DOWN;
    size_t minimumValueSize_ = 1;
    Alignment valuePadding_;
    ByteOrder::Endianness valueJustification_ = ByteOrder::ORDER_LSB;
    Alignment alignment_;
    RegisterDescriptor baseRegister_;

    // Stack offsets are signed quantities measured from the perspective of the called function immediately after the "call"
    // instruction (whatever that might be for the architecture, such as `CALL` on x86). If the stack grows down then the storage
    // addresses are higher than the current stack pointer; or vice versa.
    int64_t initialOffset_ = 0;                         // offset from caller's SP to nearest storage location on the stack
    int64_t offset_ = 0;                                // adjusted by allocation: down if stack grows down, up if stack grows up

public:
    ~StoragePoolStack();
    StoragePoolStack() = delete;
    StoragePoolStack(const std::string &name, const TypePredicateConstPtr&,
                     const Rose::BinaryAnalysis::Architecture::BaseConstPtr&);

public:
    /** Constructor.
     *
     *  Construct a new storage pool for allocating arguments on the stack. */
    static Ptr instance(const std::string &name, const TypePredicateConstPtr&, const Architecture::BaseConstPtr&);

public:
    /** Property: Stack direction.
     *
     *  Determines whether the stack grows up or down when an item is pushed onto the stack. Downward-growing stacks are the usual
     *  convention, which means that a value is pushed onto the stack by placing it at the next lower stack address(es) in
     *  memory.
     *
     *  @{ */
    StackDirection stackDirection() const;
    void stackDirection(StackDirection);
    /** @} */

    /** Property: Initial stack offset.
     *
     *  Offset from the stack pointer to the first storage location.
     *
     *  @{ */
    int64_t initialOffset() const;
    void initialOffset(int64_t);
    /** @} */

    /** Property: Minimum value size.
     *
     *  When allocating space for a value of size `n` bytes, this pool will allocate at least `m` bytes.
     *
     *  @{ */
    size_t minimumValueSize() const;
    void minimumValueSize(size_t m);
    /** @} */

    /** Property: Aligned value size.
     *
     *  When allocating space for a value of size `n` bytes, this pool will pad the size to make it a multiple of `m` bytes.
     *
     * @{ */
    const Alignment& valuePadding() const;
    void valuePadding(const Alignment&);
    /** @} */

    /** Property: Value justification within allocated storage.
     *
     *  When allocating space for a value of of size `n` bytes in an allocation region of size `m` bytes where `m` is greater than
     *  `n`, this property determines whether the value is stored in the lowest-address bytes of the region (`ORDER_LSB`) or the
     *  highest bytes of the region (`ORDER_MSB`)
     *
     * @{ */
    ByteOrder::Endianness valueJustification() const;
    void valueJustification(ByteOrder::Endianness);
    /** @} */

    /** Property: Alignment of stack offsets.
     *
     *  After determining the padded size of the stack area that stores the value, the address on the stack is calculated as an
     *  offset from the stack pointer. The offset is then aligned according to this property.
     *
     * @{ */
    const Alignment& alignment() const;
    void alignment(const Alignment&);
    /** @} */

    /** Property: Base register.
     *
     *  This is the register that points to the top of the stack. The constructor initializes this with the stack pointer register
     *  obtained from the architecture description, but it can be overridden.
     *
     * @{ */
    RegisterDescriptor baseRegister() const;
    void baseRegister(RegisterDescriptor);
    /** @} */


public:
    StoragePoolBasePtr copy() const override;
    std::vector<ConcreteLocation> consume(SgAsmType*) override;
    void reset() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Allocator {
public:
    /** Shared-ownership pointer.
     *
     * @{ */
    using Ptr = AllocatorPtr;
    using ConstPtr = AllocatorConstPtr;
    /** @} */

private:
    std::vector<StoragePoolBaseConstPtr> initialPools_;

    // Initialized with copies of the `initialPools_` above when `reset` is called. That way, the methods that modify the state of
    // the pools (allocating space for arguments, etc) operate on copies without modifying the originals.
    std::vector<StoragePoolBasePtr> activePools_;

public:
    ~Allocator();
    Allocator();

public:
    /** Constructor.
     *
     *  Construct an allocator with empty storage pools. Such an allocator cannot allocate anything. */
    static Ptr instance();

    /** Add a storage pool to the allocator. */
    void append(const StoragePoolBaseConstPtr&);

    /** Reset the allocation algorithm.
     *
     *  This should be called before allocating arguments and return values for a new function declaration. */
    void reset();

    /** Allocate storage for a value.
     *
     *  Returns the location where this value would be stored. It's possible that some values are stored at more than one
     *  location. An empty result means the value cannot be allocated. */
    std::vector<ConcreteLocation> allocate(SgAsmType*);
};

} // namespace
} // namespace
} // namespace

#endif
#endif
