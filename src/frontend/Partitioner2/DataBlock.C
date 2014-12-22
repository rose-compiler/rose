#include "sage3basic.h"
#include <Partitioner2/DataBlock.h>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

void
DataBlock::size(size_t nBytes) {
    ASSERT_forbid(isFrozen());
    ASSERT_require(nBytes>0);
    size_ = nBytes;
}

size_t
DataBlock::incrementOwnerCount() {
    return ++nAttachedOwners_;
}

size_t
DataBlock::decrementOwnerCount() {
    ASSERT_require(nAttachedOwners_ > 0);
    return --nAttachedOwners_;
}

std::string
DataBlock::printableName() const {
    return "data block " + StringUtility::addrToString(address());
}

} // namespace
} // namespace
} // namespace
