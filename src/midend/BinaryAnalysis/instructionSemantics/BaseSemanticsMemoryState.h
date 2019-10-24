#ifndef ROSE_BinaryAnalysis_InstructionSemantics2_BaseSemantics_MemoryState_H
#define ROSE_BinaryAnalysis_InstructionSemantics2_BaseSemantics_MemoryState_H

#include <BaseSemanticsTypes.h>

#include <boost/enable_shared_from_this.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/shared_ptr.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
namespace BaseSemantics {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Memory State
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Represents all memory in the state. MemoryState objects are allocated on the heap and reference counted.  The
 *  BaseSemantics::MemoryState is an abstract class that defines the interface.  See the
 *  Rose::BinaryAnalysis::InstructionSemantics2 namespace for an overview of how the parts fit together.*/
class MemoryState: public boost::enable_shared_from_this<MemoryState> {
    SValuePtr addrProtoval_;                            /**< Prototypical value for addresses. */
    SValuePtr valProtoval_;                             /**< Prototypical value for values. */
    ByteOrder::Endianness byteOrder_;                   /**< Memory byte order. */
    MergerPtr merger_;
    bool byteRestricted_;                               // are cell values all exactly one byte wide?

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Serialization
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_NVP(addrProtoval_);
        s & BOOST_SERIALIZATION_NVP(valProtoval_);
        s & BOOST_SERIALIZATION_NVP(byteOrder_);
        //s & merger_ -- not saved
        s & BOOST_SERIALIZATION_NVP(byteRestricted_);
    }
#endif


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    MemoryState();                                      // for serialization
    MemoryState(const SValuePtr &addrProtoval, const SValuePtr &valProtoval);
    MemoryState(const MemoryStatePtr &other);

public:
    /** Shared-ownership pointer for a @ref MemoryState. See @ref heap_object_shared_ownership. */
    typedef MemoryStatePtr Ptr;

public:
    virtual ~MemoryState();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors. None needed since this class is abstract

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors
public:
    /** Virtual allocating constructor.
     *
     *  Allocates and constructs a new MemoryState object having the same dynamic type as this object. A prototypical SValue
     *  must be supplied and will be used to construct any additional SValue objects needed during the operation of a
     *  MemoryState.  Two prototypical values are supplied, one for addresses and another for values stored at those addresses,
     *  although they will almost always be the same. */
    virtual MemoryStatePtr create(const SValuePtr &addrProtoval, const SValuePtr &valProtoval) const = 0;

    /** Virtual allocating copy constructor. Creates a new MemoryState object which is a copy of this object. */
    virtual MemoryStatePtr clone() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casts.  No-op since this is the base class.
public:
    static MemoryStatePtr promote(const MemoryStatePtr &x) {
        ASSERT_not_null(x);
        return x;
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods first declared at this level of the class hierarchy
public:
    /** Property: Merger.
     *
     *  This property is optional details about how to merge two states. It is passed down to the register and memory state
     *  merge operation and to the semantic value merge operation.  Users can subclass this to hold whatever information is
     *  necessary for merging.  Unless the user overrides merge functions to do something else, all merging will use the same
     *  merger object -- the one set for this property.
     *
     * @{ */
    MergerPtr merger() const;
    void merger(const MergerPtr&);
    /** @} */

    /** Return the address protoval.  The address protoval is used to construct other memory addresses via its virtual
     *  constructors. */
    SValuePtr get_addr_protoval() const { return addrProtoval_; }

    /** Return the value protoval.  The value protoval is used to construct other stored values via its virtual
     *  constructors. */
    SValuePtr get_val_protoval() const { return valProtoval_; }

    /** Clear memory. Removes all memory cells from this memory state. */
    virtual void clear() = 0;

    /** Indicates whether memory cell values are required to be eight bits wide.
     *
     *  The default is true since this simplifies the calculations for whether two memory cells are alias and how to combine
     *  the value from two or more aliasing cells. A memory that contains only eight-bit values requires that the caller
     *  concatenate/extract individual bytes when reading/writing multi-byte values.
     *
     * @{ */
    bool byteRestricted() const { return byteRestricted_; }
    void byteRestricted(bool b) { byteRestricted_ = b; }
    /** @} */

    /** Memory byte order.
     *  @{ */
    ByteOrder::Endianness get_byteOrder() const { return byteOrder_; }
    void set_byteOrder(ByteOrder::Endianness bo) { byteOrder_ = bo; }
    /** @} */

    /** Merge memory states for data flow analysis.
     *
     *  Merges the @p other state into this state, returning true if this state changed. */
    virtual bool merge(const MemoryStatePtr &other, RiscOperators *addrOps, RiscOperators *valOps) = 0;

    /** Read a value from memory.
     *
     *  Consults the memory represented by this MemoryState object and returns a semantic value. Depending on the semantic
     *  domain, the value can be a value that is already stored in the memory state, a supplied default value, a new value
     *  constructed from some combination of existing values and/or the default value, or anything else.  For instance, in a
     *  symbolic domain the @p address could alias multiple existing memory locations and the implementation may choose to
     *  return a McCarthy expression.  Additional data (such as SMT solvers) may be passed via the RiscOperators argument.
     *
     *  The size of the value being read does not necessarily need to be equal to the size of values stored in the memory
     *  state, though it typically is(1). For instance, an implementation may allow reading a 32-bit little endian value from a
     *  memory state that stores only bytes.  A RiscOperators object is provided for use in these situations.
     *
     *  In order to support cases where an address does not match any existing location, the @p dflt value can be used to
     *  initialize a new memory location.  The manner in which the default is used depends on the implementation.  In any case,
     *  the width of the @p dflt value determines how much to read.
     *
     *  Footnote 1: A MemoryState::readMemory() call is the last in a sequence of delegations starting with
     *  RiscOperators::readMemory().  The designers of the MemoryState, State, and RiscOperators subclasses will need to
     *  coordinate to decide which layer should handle concatenating values from individual memory locations. */
    virtual SValuePtr readMemory(const SValuePtr &address, const SValuePtr &dflt,
                                 RiscOperators *addrOps, RiscOperators *valOps) = 0;

    /** Read a value from memory without side effects.
     *
     *  This is similar to @ref readMemory except there are no side effects. The memory state is not modified by this
     *  function. */
    virtual SValuePtr peekMemory(const SValuePtr &address, const SValuePtr &dflt,
                                 RiscOperators *addrOps, RiscOperators *valOps) = 0;

    /** Write a value to memory.
     *
     *  Consults the memory represented by this MemoryState object and possibly inserts the specified value.  The details of
     *  how a value is inserted into a memory state depends entirely on the implementation in a subclass and will probably be
     *  different for each semantic domain.
     *
     *  A MemoryState::writeMemory() call is the last in a sequence of delegations starting with
     *  RiscOperators::writeMemory(). The designers of the MemoryState, State, and RiscOperators will need to coordinate to
     *  decide which layer (if any) should handle splitting a multi-byte value into multiple memory locations. */
    virtual void writeMemory(const SValuePtr &addr, const SValuePtr &value,
                             RiscOperators *addrOps, RiscOperators *valOps) = 0;

    /** Print a memory state to more than one line of output.
     * @{ */
    void print(std::ostream&, const std::string prefix = "") const;
    virtual void print(std::ostream&, Formatter&) const = 0;
    /** @} */

    /** MemoryState with formatter. See with_formatter(). */
    class WithFormatter {
        MemoryStatePtr obj;
        Formatter &fmt;
    public:
        WithFormatter(const MemoryStatePtr &obj, Formatter &fmt): obj(obj), fmt(fmt) {}
        void print(std::ostream &stream) const { obj->print(stream, fmt); }
    };

    /** Used for printing memory states with formatting. The usual way to use this is:
     * @code
     *  MemoryStatePtr obj = ...;
     *  Formatter fmt = ...;
     *  std::cout <<"The value is: " <<(*obj+fmt) <<"\n";
     * @endcode
     * @{ */
    WithFormatter with_format(Formatter &fmt) { return WithFormatter(shared_from_this(), fmt); }
    WithFormatter operator+(Formatter &fmt) { return with_format(fmt); }
    /** @} */
};

std::ostream& operator<<(std::ostream&, const MemoryState&);
std::ostream& operator<<(std::ostream&, const MemoryState::WithFormatter&);

} // namespace
} // namespace
} // namespace
} // namespace

#endif
