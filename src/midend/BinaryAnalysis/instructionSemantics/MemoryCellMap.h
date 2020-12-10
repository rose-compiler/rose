#ifndef ROSE_BinaryAnalysis_InstructionSemantics2_MemoryCellMap_H
#define ROSE_BinaryAnalysis_InstructionSemantics2_MemoryCellMap_H
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include <BaseSemantics2.h>
#include <boost/foreach.hpp>
#include <MemoryCellState.h>
#include <Sawyer/Map.h>

#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
namespace BaseSemantics {

/** Shared-ownership pointer to a map-based memory state. See @ref heap_object_shared_ownership. */
typedef boost::shared_ptr<class MemoryCellMap> MemoryCellMapPtr;

/** Simple map-based memory state.
 *
 *  Memory cells (address + value pairs with additional data, @refMemoryCell) are stored in a map-like container so that a cell
 *  can be accessed in logarithmic time given its address.  The keys for the map are generated from the cell virtual addresses,
 *  either by using the address directly or by hashing it. The function that generates these keys, @ref generateCellKey, is
 *  pure virtual. */
class MemoryCellMap: public MemoryCellState {
public:
    /** Key used to look up memory cells.
     *
     *  The key is generated from the cell's virtual address either by using the address directly or by hashing it. For
     *  instance, a concrete domain might use the address directly while a symbolic domain will probably hash the symbolic
     *  address expression. */
    typedef uint64_t CellKey;

    /** Map of memory cells indexed by cell keys. */
    typedef Sawyer::Container::Map<CellKey, MemoryCellPtr> CellMap;

protected:
    CellMap cells;

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(MemoryCellState);
        s & BOOST_SERIALIZATION_NVP(cells);
    }
#endif
    
protected:
    MemoryCellMap() {}                                  // for serialization

    explicit MemoryCellMap(const MemoryCellPtr &protocell)
        : MemoryCellState(protocell) {}

    MemoryCellMap(const SValuePtr &addrProtoval, const SValuePtr &valProtoval)
        : MemoryCellState(addrProtoval, valProtoval) {}

    MemoryCellMap(const MemoryCellMap &other)
        : MemoryCellState(other) {
        BOOST_FOREACH (const MemoryCellPtr &cell, other.cells.values())
            cells.insert(other.generateCellKey(cell->get_address()), cell->clone());
    }

private:
    MemoryCellMap& operator=(MemoryCellMap&) /*delete*/;

public:
    /** Shared-ownership pointer for a memory cell map object. */
    typedef MemoryCellMap Ptr;

public:
    /** Promote a base memory state pointer to a MemoryCellMap pointer. The memory state, @p x, must have a MemoryCellMap
     *  dynamic type. */
    static MemoryCellMapPtr promote(const MemoryStatePtr &x) {
        MemoryCellMapPtr retval = boost::dynamic_pointer_cast<MemoryCellMap>(x);
        ASSERT_not_null(retval);
        return retval;
    }

public:
    /** Generate a cell lookup key.
     *
     *  Generates a key from a virtual address. The key is used to look up the cell in a map-based container. */
    virtual CellKey generateCellKey(const SValuePtr &address) const = 0;

    /** Look up memory cell for address.
     *
     *  Returns the memory cell for the specified address, or a null pointer if the cell does not exist.  The address is used
     *  to look up the cell in logirithmic time. This is just a convenience wrapper around @ref matchingCells that returns
     *  either the (single) cell found by that function or a null pointer. */
    virtual MemoryCellPtr findCell(const SValuePtr &addr) const;

    /** Predicate to determine whether all bytes are present.
     *
     *  Returns true if bytes at the specified address and the following consecutive addresses are all present in this
     *  memory state. */
    virtual bool isAllPresent(const SValuePtr &address, size_t nBytes, RiscOperators *addrOps) const;

public:
    virtual void clear() ROSE_OVERRIDE;
    virtual bool merge(const MemoryStatePtr &other, RiscOperators *addrOps, RiscOperators *valOps) ROSE_OVERRIDE;
    virtual SValuePtr readMemory(const SValuePtr &address, const SValuePtr &dflt,
                                 RiscOperators *addrOps, RiscOperators *valOps) ROSE_OVERRIDE;
    virtual SValuePtr peekMemory(const SValuePtr &address, const SValuePtr &dflt,
                                 RiscOperators *addrOps, RiscOperators *valOps) ROSE_OVERRIDE;
    virtual void writeMemory(const SValuePtr &address, const SValuePtr &value,
                             RiscOperators *addrOps, RiscOperators *valOps) ROSE_OVERRIDE;
    virtual void print(std::ostream&, Formatter&) const ROSE_OVERRIDE;
    virtual std::vector<MemoryCellPtr> matchingCells(const MemoryCell::Predicate&) const ROSE_OVERRIDE;
    virtual std::vector<MemoryCellPtr> leadingCells(const MemoryCell::Predicate&) const ROSE_OVERRIDE;
    virtual void eraseMatchingCells(const MemoryCell::Predicate&) ROSE_OVERRIDE;
    virtual void eraseLeadingCells(const MemoryCell::Predicate&) ROSE_OVERRIDE;
    virtual void traverse(MemoryCell::Visitor&) ROSE_OVERRIDE;
    virtual MemoryCell::AddressSet getWritersUnion(const SValuePtr &addr, size_t nBits, RiscOperators *addrOps,
                                                   RiscOperators *valOps) ROSE_OVERRIDE;
    virtual MemoryCell::AddressSet getWritersIntersection(const SValuePtr &addr, size_t nBits, RiscOperators *addrOps,
                                                          RiscOperators *valOps) ROSE_OVERRIDE;
};

} // namespace
} // namespace
} // namespace
} // namespace

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
BOOST_CLASS_EXPORT_KEY(Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::MemoryCellMap);
#endif

#endif
#endif
