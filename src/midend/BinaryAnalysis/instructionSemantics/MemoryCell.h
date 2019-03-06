#ifndef ROSE_BinaryAnalysis_InstructionSemantics2_MemoryCell_H
#define ROSE_BinaryAnalysis_InstructionSemantics2_MemoryCell_H

#include <BaseSemantics2.h>
#include <Sawyer/Set.h>
#include <boost/serialization/access.hpp>
#include <boost/serialization/list.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
namespace BaseSemantics {

/** Shared-ownership pointer to a semantic memory cell. See @ref heap_object_shared_ownership. */
typedef boost::shared_ptr<class MemoryCell> MemoryCellPtr;

/** Represents one location in memory.
 *
 *  MemoryCell objects are used by the MemoryCellList to represent a memory state. Each memory cell has an address and a
 *  value. A cell also has an optional list of instruction addresses that wrote to that cell, and this list is manipulated by
 *  the @ref RiscOperators separately from updating cell addresses and values and according to settings in the
 *  RiscOperators. Cells written to by RiscOperators typically contain one writer address since each write operation creates a
 *  new cell; however, the result of a dataflow merge operation might produce cells that have multiple writers. */
class MemoryCell: public boost::enable_shared_from_this<MemoryCell> {
public:
    typedef Sawyer::Container::Set<rose_addr_t> AddressSet; /**< A set of concrete virtual addresses. */

    /** Visitor for traversing a cells. */
    class Visitor {
    public:
        virtual ~Visitor() {}
        virtual void operator()(MemoryCellPtr&) = 0;
    };

    /** Predicate for matching cells. */
    class Predicate {
    public:
        virtual ~Predicate() {};

        /** Invoked for some cell. The predicate must not modify the cell. */
        virtual bool operator()(const MemoryCellPtr&) const = 0;
    };

    /** Predicate that always returns true. */
    class AllCells: public Predicate {
    public:
        virtual bool operator()(const MemoryCellPtr&) const ROSE_OVERRIDE {
            return true;
        }
    };

    /** Predicate for non-written cells.
     *
     *  Returns true if a cell has no writers. */
    class NonWrittenCells: public Predicate {
    public:
        virtual bool operator()(const MemoryCellPtr&) const ROSE_OVERRIDE;
    };

private:
    SValuePtr address_;                                 // Address of memory cell.
    SValuePtr value_;                                   // Value stored at that address.
    AddressSet writers_;                                // Instructions that wrote to this cell
    InputOutputPropertySet ioProperties_;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Serialization
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_NVP(address_);
        s & BOOST_SERIALIZATION_NVP(value_);
        s & BOOST_SERIALIZATION_NVP(writers_);
        s & BOOST_SERIALIZATION_NVP(ioProperties_);
    }
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    MemoryCell() {}                                     // for serialization

    MemoryCell(const SValuePtr &address, const SValuePtr &value)
        : address_(address), value_(value) {
        ASSERT_not_null(address);
        ASSERT_not_null(value);
    }

    // deep-copy cell list so modifying this new one doesn't alter the existing one
    MemoryCell(const MemoryCell &other)
        : boost::enable_shared_from_this<MemoryCell>(other) {
        address_ = other.address_->copy();
        value_ = other.value_->copy();
        writers_ = other.writers_;
        ioProperties_ = other.ioProperties_;
    }

public:
    virtual ~MemoryCell() {}

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors
public:
    /** Instantiates a new memory cell object with the specified address and value. */
    static MemoryCellPtr instance(const SValuePtr &address, const SValuePtr &value) {
        return MemoryCellPtr(new MemoryCell(address, value));
    }

    /** Instantiates a new copy of an existing cell. */
    static MemoryCellPtr instance(const MemoryCellPtr &other) {
        return MemoryCellPtr(new MemoryCell(*other));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors
public:
    /** Creates a new memory cell object with the specified address and value. */
    virtual MemoryCellPtr create(const SValuePtr &address, const SValuePtr &value) {
        return instance(address, value);
    }

    /** Creates a new deep-copy of this memory cell. */
    virtual MemoryCellPtr clone() const {
        return MemoryCellPtr(new MemoryCell(*this));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casts. No-op since this is the base class.
public:
    static MemoryCellPtr promote(const MemoryCellPtr &x) {
        ASSERT_not_null(x);
        return x;
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods first declared at this level of the class hierarchy
public:
    /** Accessor for the memory cell address.
     * @{ */
    virtual SValuePtr get_address() const { return address_; }
    virtual void set_address(const SValuePtr &addr) {
        ASSERT_not_null(addr);
        address_ = addr;
    }
    /** @}*/

    /** Accessor for the value stored at a memory location.
     * @{ */
    virtual SValuePtr get_value() const { return value_; }
    virtual void set_value(const SValuePtr &v) {
        ASSERT_not_null(v);
        value_ = v;
    }
    /** @}*/

    /** Get writer information.
     *
     *  Returns all instruction addresses that have written to the specified memory address. */
    virtual const AddressSet& getWriters() const {
        return writers_;
    }

    /** Insert writer information.
     *
     *  Insert the specified instruction addresses as writers for this memory cell. Returns true if any address was inserted,
     *  false if they were all already members.
     *
     * @{ */
    bool insertWriter(rose_addr_t writerVa) /*final*/ { return writers_.insert(writerVa); }
    virtual bool insertWriters(const AddressSet &writerVas) { return writers_.insert(writerVas); }
    /** @} */

    /** Erase specified writers.
     *
     *  Removes the specified addresses from the set of writers for this memory cell. Returns true if none of the writer
     *  addresses existed, false if any were removed.
     *
     * @{ */
    bool eraseWriter(rose_addr_t writerVa) /*final*/ { return writers_.erase(writerVa); }
    virtual bool eraseWriters(const AddressSet &writerVas) { return writers_.erase(writerVas); }
    /** @} */

    /** Sets writer information.
     *
     *  Changes writer information to be excactly the specified address or set of addresses.
     *
     * @{ */
    void setWriter(rose_addr_t writerVa) /*final*/;
    virtual void setWriters(const AddressSet &writerVas) { writers_.insert(writerVas); }
    /** @} */

    /** Erase all writers.
     *
     *  Erases all writer information for this memory cell. */
    virtual void eraseWriters() { writers_.clear(); }

    /** Properties: Boolean property set.
     *
     *  This set holds Boolean input/output properties.  If a property is present in the set then it is considered to be true,
     *  otherwise false.
     *
     * @{ */
    const InputOutputPropertySet& ioProperties() const { return ioProperties_; }
    InputOutputPropertySet& ioProperties() { return ioProperties_; }
    /** @} */

    /** Determines whether two memory cells can alias one another.  Two cells may alias one another if it is possible that
     *  their addresses cause them to overlap.  For cells containing one-byte values, aliasing may occur if their two addresses
     *  may be equal; multi-byte cells will need to check ranges of addresses. */
    virtual bool may_alias(const MemoryCellPtr &other, RiscOperators *addrOps) const;

    /** Determines whether two memory cells must alias one another.  Two cells must alias one another when it can be proven
     * that their addresses cause them to overlap.  For cells containing one-byte values, aliasing must occur unless their
     * addresses can be different; multi-byte cells will need to check ranges of addresses. */
    virtual bool must_alias(const MemoryCellPtr &other, RiscOperators *addrOps) const;
    
    /** Print the memory cell on a single line.
     * @{ */
    void print(std::ostream &stream) const {
        Formatter fmt;
        print(stream, fmt);
    }
    virtual void print(std::ostream&, Formatter&) const;
    /** @} */

    /** State with formatter. See with_formatter(). */
    class WithFormatter {
        MemoryCellPtr obj;
        Formatter &fmt;
    public:
        WithFormatter(const MemoryCellPtr &obj, Formatter &fmt): obj(obj), fmt(fmt) {}
        void print(std::ostream &stream) const { obj->print(stream, fmt); }
    };

    /** Used for printing states with formatting. The usual way to use this is:
     * @code
     *  MemoryCellPtr obj = ...;
     *  Formatter fmt = ...;
     *  std::cout <<"The value is: " <<(*obj+fmt) <<"\n";
     * @endcode
     * @{ */
    WithFormatter with_format(Formatter &fmt) { return WithFormatter(shared_from_this(), fmt); }
    WithFormatter operator+(Formatter &fmt) { return with_format(fmt); }
    /** @} */
};


std::ostream& operator<<(std::ostream&, const MemoryCell&);
std::ostream& operator<<(std::ostream&, const MemoryCell::WithFormatter&);

} // namespace
} // namespace
} // namespace
} // namespace

#endif
