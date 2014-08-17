#ifndef ROSE_Partitioner2_OwnedDataBlock_H
#define ROSE_Partitioner2_OwnedDataBlock_H

#include <Partitioner2/BasicTypes.h>
#include <Partitioner2/DataBlock.h>
#include <Partitioner2/Function.h>

#include <sawyer/Map.h>

#include <vector>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

/** Shared reference to data block.  Data blocks can be owned by multiple functions, which is handled by using the @ref
 *  DataBlock::Ptr shared ownership pointers.  However, data blocks can also be owned by multiple functions that are attached
 *  to a CFG, in which case we need to keep track of the number of such owners so that the data block can be removed from the
 *  partitioner's bookkeeping when its last function is detached from the CFG.  The DataBlockReference objects associate a data
 *  block pointer with its owning, CFG-attached functions */
class OwnedDataBlock {
    DataBlock::Ptr dblock_;                             // the data block, non-null
    std::vector<Function::Ptr> owners_;                 // CFG-attached functions that own this block, sorted by address
public:
    /** Construct a new data block ownership record.  The data block is not owned by any function, so the @ref insert method
     *  must be called soon. The data block must not be null. */
    explicit OwnedDataBlock(const DataBlock::Ptr &dblock): dblock_(dblock) {
        ASSERT_not_null(dblock);
    }

    /** Construct a new data block ownership record.  The data block is marked so that it is owned by this one function.  The
     *  data block and owning function must neither be null pointers. */
    OwnedDataBlock(const DataBlock::Ptr &dblock, const Function::Ptr &owner)
        : dblock_(dblock), owners_(1, owner) {
        ASSERT_not_null(dblock);
        ASSERT_not_null(owner);
    }

    /** Add a function owner for this data block.  The specified function must not be null. If the function is already a member
     *  of the block owner list then this method does nothing. Returns the number of owners after adding the specified
     *  function. */
    size_t insert(const Function::Ptr&);

    /** Remove a function owner for this data block.  If the function is a null pointer or the function is not an owner of the
     *  data block then this method does nothing. Returns the number of owners after removing the specified function. */
    size_t erase(const Function::Ptr&);

    /** Returns the list of functions that own this data block. */
    const std::vector<Function::Ptr>& owningFunctions() const { return owners_; }

    /** Returns the number of functions that own this data block. */
    size_t nOwners() const { return owners_.size(); }

    /** Returns the data block for this ownership record. */
    DataBlock::Ptr dblock() const { return dblock_; }
};

/** Data blocks by starting address. */
typedef Sawyer::Container::Map<rose_addr_t, OwnedDataBlock> DataBlocks;

} // namespace
} // namespace
} // namespace

#endif
