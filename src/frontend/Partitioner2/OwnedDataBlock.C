#include "sage3basic.h"
#include <Partitioner2/OwnedDataBlock.h>

#include <Partitioner2/Utility.h>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

size_t
OwnedDataBlock::insertOwner(const Function::Ptr &function) {
    ASSERT_require(isValid());
    if (function!=NULL)
        insertUnique(functions_, function, sortFunctionsByAddress);
    return nOwners();
}

size_t
OwnedDataBlock::insertOwner(const BasicBlock::Ptr &bblock) {
    ASSERT_require(isValid());
    if (bblock!=NULL)
        insertUnique(bblocks_, bblock, sortBasicBlocksByAddress);
    return nOwners();
}

size_t
OwnedDataBlock::eraseOwner(const Function::Ptr &function) {
    if (function!=NULL)
        eraseUnique(functions_, function, sortFunctionsByAddress);
    return nOwners();
}

size_t
OwnedDataBlock::eraseOwner(const BasicBlock::Ptr &bblock) {
    if (bblock!=NULL)
        eraseUnique(bblocks_, bblock, sortBasicBlocksByAddress);
    return nOwners();
}

} // namespace
} // namespace
} // namespace
