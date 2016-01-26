#ifndef ROSE_BinaryAnalysis_InstructionSemantics2_MemoryCellState_H
#define ROSE_BinaryAnalysis_InstructionSemantics2_MemoryCellState_H

#include <BaseSemantics2.h>
#include <MemoryCell.h>

namespace rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
namespace BaseSemantics {

/** Smart pointer to a MemoryCellState object.  MemoryCellState objects are reference counted and should not be explicitly
 *  deleted. */
typedef boost::shared_ptr<class MemoryCellState> MemoryCellStatePtr;

/** Cell-based memory state.
 *
 *  This is the base class for cell-based memory states. */
class MemoryCellState: public MemoryState {
protected:
    MemoryCellPtr protocell;                            // prototypical memory cell used for its virtual constructors
    MemoryCellPtr latestWrittenCell_;                   // the cell whose value was most recently written to, if any

protected:
    explicit MemoryCellState(const MemoryCellPtr &protocell)
        : MemoryState(protocell->get_address(), protocell->get_value()), protocell(protocell) {}

    MemoryCellState(const SValuePtr &addrProtoval, const SValuePtr &valProtoval)
        : MemoryState(addrProtoval, valProtoval), protocell(MemoryCell::instance(addrProtoval, valProtoval)) {}

    MemoryCellState(const MemoryCellState &other)
        : MemoryState(other), protocell(other.protocell) {} // latestWrittenCell_ is cleared

public:
    /** Promote a base memory state pointer to a BaseSemantics::MemoryCellState pointer.  The memory state @p m must have a
     *  BaseSemantics::MemoryCellState dynamic type. */
    static MemoryCellStatePtr promote(const BaseSemantics::MemoryStatePtr &m) {
        MemoryCellStatePtr retval = boost::dynamic_pointer_cast<MemoryCellState>(m);
        ASSERT_not_null(retval);
        return retval;
    }

public:
    virtual void clear() ROSE_OVERRIDE;

public:
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

    /** Find all matching cells.
     *
     *  Returns a vector of cells for which the @p predicate returns true. */
    virtual std::vector<MemoryCellPtr> matchingCells(const MemoryCell::Predicate&) const = 0;

    /** Find leading matching cells.
     *
     *  Returns the vector of cells obtained by invoking the predicate on each cell and returning those leading cells for which
     *  the predicate is true.  The first cell for which the predicate is false terminates the traversal and does not appear
     *  in the return value. */
    virtual std::vector<MemoryCellPtr> leadingCells(const MemoryCell::Predicate&) const = 0;
    
    /** Remove all matching cells.
     *
     *  Traverses the memory cells and removes those for which the predicate returns true. */
    virtual void eraseMatchingCells(const MemoryCell::Predicate&) = 0;

    /** Remove leading matching cells.
     *
     *  Traverses the memory cells and removes those for which the predicate returns true. The traversal is terminated the
     *  first time the predicate returns false. */
    virtual void eraseLeadingCells(const MemoryCell::Predicate&) = 0;

    /** Traverse and modify cells.
     *
     *  Traverse cells and optionally modify them. */
    virtual void traverse(MemoryCell::Visitor&) = 0;

    /** Erase cells that have no writers. */
    void eraseNonWritten() {
        eraseMatchingCells(MemoryCell::NonWrittenCells());
    }

    /** All cells. */
    std::vector<MemoryCellPtr> allCells() const {
        return matchingCells(MemoryCell::AllCells());
    }
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
