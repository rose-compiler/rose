#ifndef ROSE_BinaryAnalysis_InstructionSemantics2_MemoryCellMap_H
#define ROSE_BinaryAnalysis_InstructionSemantics2_MemoryCellMap_H

#include <BaseSemantics2.h>
#include <MemoryCell.h>
#include <Sawyer/Map.h>

namespace rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
namespace BaseSemantics {

/** Shared-ownership pointer for a MemoryCellMap. MemoryCellMap objects are always allocated on the heap, reference counted,
 *  and deleted automatically. */
typedef boost::shared_ptr<class MemoryCellMap> MemoryCellMapPtr;

/** Simple map-based memory state.
 *
 *  Memory cells (address + value pairs with additional data, @refMemoryCell) are stored in a map-like container so that a cell
 *  can be accessed in logarithmic time given its address.  The keys for the map are generated from the cell virtual addresses,
 *  either by using the address directly or by hashing it. The function that generates these keys, @ref generateCellKey, is
 *  pure virtual. */
class MemoryCellMap: public MemoryState {
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
    MemoryCellPtr protocell;
    CellMap cells;
    MemoryCellPtr latestWrittenCell_;                   // the cell whose value was most recently written to, if any

    explicit MemoryCellMap(const MemoryCellPtr &protocell)
        : MemoryState(protocell->get_address(), protocell->get_value()),
          protocell(protocell) {
        ASSERT_not_null(protocell);
        ASSERT_not_null(protocell->get_address());
        ASSERT_not_null(protocell->get_value());
    }

    explicit MemoryCellMap(const SValuePtr &addrProtoval, const SValuePtr &valProtoval)
        : MemoryState(addrProtoval, valProtoval), protocell(MemoryCell::instance(addrProtoval, valProtoval)) {
        ASSERT_not_null(addrProtoval);
        ASSERT_not_null(valProtoval);
    }

    MemoryCellMap(const MemoryCellMap &other)
        : MemoryState(other), protocell(other.protocell) {
        BOOST_FOREACH (const MemoryCellPtr &cell, cells.values())
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

    /** Visit each memory cell. */
    virtual void traverse(MemoryCell::Visitor &visitor);

    /** Property: Cell most recently written.
     *
     * @{ */
    virtual MemoryCellPtr latestWrittenCell() const {
        return latestWrittenCell_;
    }
    virtual void latestWrittenCell(const MemoryCellPtr &cell) {
        latestWrittenCell_ = cell;
    }
    /** @} */

public:
    virtual void clear() ROSE_OVERRIDE;

    virtual bool merge(const MemoryStatePtr &other, RiscOperators *addrOps, RiscOperators *valOps) ROSE_OVERRIDE;

    virtual SValuePtr readMemory(const SValuePtr &address, const SValuePtr &dflt,
                                 RiscOperators *addrOps, RiscOperators *valOps) ROSE_OVERRIDE;

    virtual void writeMemory(const SValuePtr &address, const SValuePtr &value,
                             RiscOperators *addrOps, RiscOperators *valOps) ROSE_OVERRIDE;

    virtual void print(std::ostream&, Formatter&) const ROSE_OVERRIDE;
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
