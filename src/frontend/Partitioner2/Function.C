#include "sage3basic.h"
#include <Partitioner2/Function.h>

#include <Partitioner2/Exception.h>
#include <Partitioner2/Utility.h>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

void
Function::insertDataBlock(const DataBlock::Ptr &dblock) {
    if (isFrozen_)
        throw Exception(printableName() + " is frozen or attached to the CFG/AUM");
    if (dblock==NULL)
        return;

    std::vector<DataBlock::Ptr>::iterator lb = std::lower_bound(dblocks_.begin(), dblocks_.end(), dblock,
                                                                sortDataBlocksByAddress);
    if (lb != dblocks_.end() && (*lb)->address()==dblock->address()) {
        *lb = dblock;
    } else {
        dblocks_.push_back(dblock);
    }
    ASSERT_require(isSorted(dblocks_, sortDataBlocksByAddress, true));
}

void
Function::eraseDataBlock(const DataBlock::Ptr &dblock) {
    if (dblock) {
        std::vector<DataBlock::Ptr>::iterator lb = std::lower_bound(dblocks_.begin(), dblocks_.end(), dblock,
                                                                    sortDataBlocksByAddress);
        if (lb!=dblocks_.end() && (*lb)==dblock)
            dblocks_.erase(lb);
    }
}

std::string
Function::printableName() const {
    return "function " + StringUtility::addrToString(address());
}


} // namespace
} // namespace
} // namespace
