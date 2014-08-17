#include "sage3basic.h"
#include <Partitioner2/OwnedDataBlock.h>

#include <Partitioner2/Utility.h>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

size_t
OwnedDataBlock::insert(const Function::Ptr &function) {
    std::vector<Function::Ptr>::iterator lb = std::lower_bound(owners_.begin(), owners_.end(), function,
                                                               sortFunctionsByAddress);
    if (lb!=owners_.end() && function->address()==(*lb)->address()) {
        ASSERT_require2(function==(*lb), "two functions with the same address must be the same function");
    } else {
        owners_.insert(lb, function);
        ASSERT_require(isSorted(owners_, sortFunctionsByAddress, true));
    }
    return owners_.size();
}

size_t
OwnedDataBlock::erase(const Function::Ptr &function) {
    if (function) {
        std::vector<Function::Ptr>::iterator lb = std::lower_bound(owners_.begin(), owners_.end(), function,
                                                                   sortFunctionsByAddress);
        if (lb!=owners_.end())
            owners_.erase(lb);
    }
    return owners_.size();
}

} // namespace
} // namespace
} // namespace
