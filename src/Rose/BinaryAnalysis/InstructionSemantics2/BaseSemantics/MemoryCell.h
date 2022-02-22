#ifndef ROSE_BinaryAnalysis_InstructionSemantics2_BaseSemantics_MemoryCell_H
#define ROSE_BinaryAnalysis_InstructionSemantics2_BaseSemantics_MemoryCell_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/InstructionSemantics2/BaseSemantics/Types.h>

#include <Sawyer/Set.h>

#include <boost/enable_shared_from_this.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/list.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
namespace BaseSemantics {

/** Represents one location in memory.
 *
 *  MemoryCell objects are used by the MemoryCellList to represent a memory state. Each memory cell has an address and a
 *  value. A cell also has an optional list of instruction addresses that wrote to that cell, and this list is manipulated by
 *  the @ref RiscOperators separately from updating cell addresses and values and according to settings in the
 *  RiscOperators. Cells written to by RiscOperators typically contain one writer address since each write operation creates a
 *  new cell; however, the result of a dataflow merge operation might produce cells that have multiple writers. */
class MemoryCell: public boost::enable_shared_from_this<MemoryCell> {
public:
    /** Shared-ownership pointer. */
    using Ptr = MemoryCellPtr;

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
        virtual bool operator()(const MemoryCellPtr&) const override {
            return true;
        }
    };

    /** Predicate for non-written cells.
     *
     *  Returns true if a cell has no writers. */
    class NonWrittenCells: public Predicate {
    public:
        virtual bool operator()(const MemoryCellPtr&) const override;
    };

private:
    SValuePtr address_;                                 // Address of memory cell.
    SValuePtr value_;                                   // Value stored at that address.
    AddressSet writers_;                                // Instructions that wrote to this cell
    InputOutputPropertySet ioProperties_;
    unsigned position_ = 0;                             // position when printing a memory state

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Serialization
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned version) {
        s & BOOST_SERIALIZATION_NVP(address_);
        s & BOOST_SERIALIZATION_NVP(value_);
        s & BOOST_SERIALIZATION_NVP(writers_);
        s & BOOST_SERIALIZATION_NVP(ioProperties_);
        if (version >= 1)
            s & BOOST_SERIALIZATION_NVP(position_);
    }
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    MemoryCell();                                       // for serialization

    MemoryCell(const SValuePtr &address, const SValuePtr &value);

    // deep-copy cell list so modifying this new one doesn't alter the existing one
    MemoryCell(const MemoryCell &other);

public:
    virtual ~MemoryCell();

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
    /** Property: Memory cell address.
     *
     *  This property holds the address of the memory cell. The address must not be null.
     *
     *  This method should not be reimplemented (yet) in subclasses. Instead, it calls @ref get_address and @ref set_address to
     *  do its work.
     *
     * @{ */
    SValuePtr address() const /*final*/ {
        return get_address();
    }
    void address(const SValuePtr &addr) /*final*/ {
        set_address(addr);
    }
    /** @} */

    /** Property: Memory cell value.
     *
     *  This property holds the value of the memory cell. It must not be null.
     *
     *  This method should not be reimplemented (yet) in subclasses. Instead, it calls @ref get_value and @ref set_value to do
     *  its work.
     *
     *  @{ */
    SValuePtr value() const /*final*/ {
        return get_value();
    }
    void value(const SValuePtr &v) /*final*/ {
        set_value(v);
    }
    /** @} */

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

    /** Property: Position in listings.
     *
     *  For memory states that support it, this property holds an integer position of the cell relative to the other cells. For
     *  instance, a map-based memory state that uses address hashes would normally list the memory cells in order of their
     *  hashes, which makes it confusing for a human to look at because the bytes for a single multi-byte word will be
     *  scattered throughout the listing.  By setting a position, the user can control the order that the four bytes are listed
     *  with respect to each other and with respect to the other cells in the memory state.
     *
     * @{ */
    unsigned position() const { return position_; }
    void position(unsigned p) { position_ = p; }
    /** @} */

    /** Test whether two memory cells can alias one another.
     *
     *  Two cells may alias one another if it is possible that their addresses cause them to overlap.  For cells containing
     *  one-byte values, aliasing may occur if their two addresses may be equal; multi-byte cells will need to check ranges of
     *  addresses.
     *
     *  For now, this just calls the old virtual @ref may_alias method, but eventually we'll remove that method.
     */
    bool mayAlias(const MemoryCellPtr &other, RiscOperators *addrOps) const /*final*/ {
        return may_alias(other, addrOps);
    }

    // [Robb Matzke 2021-03-18]: deprecated, but not marked as such because we need to still call it in order to support
    // user-defined subclasses that might have reimplemented it. Users: be sure to mark them as "override" so you'll notice
    // when we switch to just "mayAlias".
    virtual bool may_alias(const MemoryCellPtr &other, RiscOperators *addrOps) const;

    /** Test whether two memory cells must alias one another.
     *
     *  Two cells must alias one another when it can be proven that their addresses cause them to overlap.  For cells
     *  containing one-byte values, aliasing must occur unless their addresses can be different; multi-byte cells will need to
     *  check ranges of addresses.
     *
     *  For now, this just calls the old virtual @ref must_alias method, but eventually we'll remove that method. */
    bool mustAlias(const MemoryCellPtr &other, RiscOperators *addrOps) const /*final*/ {
        return must_alias(other, addrOps);
    }

    // [Robb Matzke 2021-03-18]: deprecated, but not marked as such because we need to still call it in order to support
    // user-defined subclasses that might have reimplemented it. Users: be sure to mark them as "override" so you'll notice
    // when we switch to just "mustAlias".
    virtual bool must_alias(const MemoryCellPtr &other, RiscOperators *addrOps) const;
    
    /** Hash the address and value.
     *
     *  This hashes the address and value for the cell, but not any other properties. The goal is that an analysis that
     *  encounters the same state twice might be able to eliminate some work, and comparing hashes is a fast first test of
     *  equality: different hashes mean the states are different. */
    virtual void hash(Combinatorics::Hasher&) const;

    /** Print the memory cell on a single line.
     * @{ */
    void print(std::ostream &stream) const;
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
    WithFormatter with_format(Formatter&);
    WithFormatter operator+(Formatter&);
    WithFormatter operator+(const std::string &linePrefix);
    /** @} */

    // [Robb Matzke 2021-03-18]: deprecated.
    // Virtual functions for the address property. These will eventually be removed, so it's in your best interest to use C++11
    // "override" in your subclasses. We cannot mark these as ROSE_DEPRECATED because the value property needs to call them.
    virtual SValuePtr get_address() const {
        return address_;
    }
    virtual void set_address(const SValuePtr &addr) {
        ASSERT_not_null(addr);
        address_ = addr;
    }

    // [Robb Matzke 2021-03-18]: deprecated
    // Virtual functions for the value property. These will eventually be removed, so it's in your best interest to use C++11
    // "override" in your subclasses. We cannot mark these as ROSE_DEPRECATED because the value property needs to call them.
    virtual SValuePtr get_value() const {
        return value_;
    }
    virtual void set_value(const SValuePtr &v) {
        ASSERT_not_null(v);
        value_ = v;
    }
};


std::ostream& operator<<(std::ostream&, const MemoryCell&);
std::ostream& operator<<(std::ostream&, const MemoryCell::WithFormatter&);

} // namespace
} // namespace
} // namespace
} // namespace

BOOST_CLASS_EXPORT_KEY(Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::MemoryCell);
BOOST_CLASS_VERSION(Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::MemoryCell, 1);

#endif
#endif
