#ifndef ROSE_Partitioner2_OwnedDataBlock_H
#define ROSE_Partitioner2_OwnedDataBlock_H

#include <Partitioner2/BasicBlock.h>
#include <Partitioner2/BasicTypes.h>
#include <Partitioner2/DataBlock.h>
#include <Partitioner2/Function.h>

#include <sawyer/Map.h>

#include <vector>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

/** Shared reference to data block.
 *
 *  A data block can be referenced from (owned by) multiple functions and/or basic blocks, where each owner has a
 *  shared-ownership pointer to the data block. When a data block and (some of) its owners are attached to the CFG/AUM, we need
 *  to be able to quickly find the owners when given the data block.  For instance, we can use the AUM to find all data blocks
 *  that overlap a certain address interval, and then use the data block's owner list to find the functions that necessarily
 *  also overlap that interval.  The OwnedDataBlock objects keeps track of the child-to-parent ownership links by containing a
 *  pointer to a data block and pointers to the functions and/or basic blocks that own it. */
class OwnedDataBlock {
    DataBlock::Ptr dblock_;                             // the data block, non-null
    std::vector<Function::Ptr> functions_;              // functions that own this data block, sorted
    std::vector<BasicBlock::Ptr> bblocks_;              // basic blocks that own this data block, sorted
public:
    /** Default constructed null ownership.
     *
     *  A default constructed data block ownership record does not point to any data block, and its @ref isValid method returns
     *  false. All other methods for constructing an ownership record require a non-null data block and the @ref isValid method
     *  would return true for them. */
    OwnedDataBlock() {}

    /** Construct a new data block ownership record.
     *
     *  The data block must not be a null pointer. The lists of owning functions and basic blocks are empty after this call. */
    explicit OwnedDataBlock(const DataBlock::Ptr &dblock): dblock_(dblock) {
        ASSERT_not_null(dblock);
    }

    /** Construct a new data block ownership record.
     *
     *  The data block is marked so that it is owned by the specified function.  Neither the data block nor the function may
     *  be null pointers. */
    OwnedDataBlock(const DataBlock::Ptr &dblock, const Function::Ptr &owner)
        : dblock_(dblock), functions_(1, owner) {
        ASSERT_not_null(dblock);
        ASSERT_not_null(owner);
    }

    /** Construct a new data block ownership record.
     *
     *  The data block is marked so that it is owned by this one basic block.  Neither the data block nor the basic block may
     *  be null pointers. */
    OwnedDataBlock(const DataBlock::Ptr &dblock, const BasicBlock::Ptr &owner)
        : dblock_(dblock), bblocks_(1, owner) {
        ASSERT_not_null(dblock);
        ASSERT_not_null(owner);
    }

    /** Determines whether an ownership record is valid.
     *
     *  Returns true if and only if this ownership record points to a non-null data block.  This will return true for all
     *  ownership records except those that are default constructed. */
    bool isValid() const { return dblock_!=NULL; }

    /** Returns the data block for this ownership record.
     *
     *  The return value is guaranteed to be non-null if and only if @ref isValid returns true. */
    DataBlock::Ptr dataBlock() const { return dblock_; }

    /** Add a function owner for this data block.
     *
     *  The specified function must not be null and this must be a valid ownership record. If the function is already in the
     *  function owners list then this method does nothing. Returns the number of owners after adding the specified
     *  function. */
    size_t insertOwner(const Function::Ptr&);

    /** Add a basic block owner for this data block.
     *
     *  The specified basic block must not be null and this must be a valid ownership record. If the basic block is already in
     *  the basic block owners list then this method does nothing. Returns the number of owners after adding the specified
     *  basic block. */
    size_t insertOwner(const BasicBlock::Ptr&);

    /** Remove a function owner for this data block.
     *
     *  If the function is a null pointer or the function is not an owner of the data block then this method does
     *  nothing. Returns the number of owners after removing the specified function.  */
    size_t eraseOwner(const Function::Ptr&);

    /** Remove a basic block owner for this data block.
     *
     *  If the basic block is a null pointer or the basic block is not an owner of the data block then this method does
     *  nothing. Returns the number of owners after removing the specified basic block. */
    size_t eraseOwner(const BasicBlock::Ptr&);

    /** Returns the list of functions that own this data block.
     *
     *  The returned list contains distinct functions sorted by entry address.  The returned list will always be empty for
     *  ownership records for which @ref isValid returns false. */
    const std::vector<Function::Ptr>& owningFunctions() const { return functions_; }

    /** Returns the list of basic blocks that own this data block.
     *
     *  The returned list contains distinct basic blocks sorted by starting address.  The returned list will always be empty
     *  for ownership records for which @ref isValid returns false.*/
    const std::vector<BasicBlock::Ptr>& owningBasicBlocks() const { return bblocks_; }

    /** Returns the number of functions that own this data block.  The return value will always be zero for ownership records
     *  for which @ref isValid returns false. */
    size_t nFunctionOwners() const { return functions_.size(); }

    /** Returns the number basic blocks that own this data block.  The return value will always be zero for ownership records
     *  for which @ref isValid returns false. */
    size_t nBasicBlockOwners() const { return bblocks_.size(); }

    /** Returns the number of owners for this data block.
     *
     *  The total number of owners is the sum of the number of function owners and the number of basic block owners.  The
     *  return value will always be zero for ownership records for which @ref isValid returns false. */
    size_t nOwners() const { return functions_.size() + bblocks_.size(); }

    /** Returns true if this data block is owned.
     *
     *  Returns true if this data block has any function owners or basic block owners, and false otherwise.  The return value
     *  will always be false for ownership records for which @ref isValid returns false. */
    bool isOwned() const { return !functions_.empty() || !bblocks_.empty(); }
};

} // namespace
} // namespace
} // namespace

#endif
