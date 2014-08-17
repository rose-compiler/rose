#include "sage3basic.h"
#include <Partitioner2/DataBlock.h>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

std::string
DataBlock::printableName() const {
    return "data block " + StringUtility::addrToString(address());
}

} // namespace
} // namespace
} // namespace
