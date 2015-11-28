#include "sage3basic.h"
#include <Partitioner2/Function.h>

#include <Partitioner2/Exception.h>
#include <Partitioner2/Utility.h>
#include <integerOps.h>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

bool
Function::insertDataBlock(const DataBlock::Ptr &dblock) {
    if (isFrozen_)
        throw Exception(printableName() + " is frozen or attached to the CFG/AUM");
    if (dblock==NULL)
        return false;
    bool retval = insertUnique(dblocks_, dblock, sortDataBlocks);
    if (retval)
        clearCache();
    return retval;
}

void
Function::eraseDataBlock(const DataBlock::Ptr &dblock) {
    if (dblock) {
        std::vector<DataBlock::Ptr>::iterator lb = std::lower_bound(dblocks_.begin(), dblocks_.end(), dblock, sortDataBlocks);
        if (lb!=dblocks_.end() && (*lb)==dblock) {
            dblocks_.erase(lb);
            clearCache();
        }
    }
}

std::string
Function::printableName() const {
    std::string s = "function " + StringUtility::addrToString(address());
    if (!name_.empty())
        s += " \"" + StringUtility::cEscape(name_) + "\"";
    return s;
}

bool
Function::isThunk() const {
    if (0 == (reasons_ & SgAsmFunction::FUNC_THUNK))
        return false;
    if (bblockVas_.size() != 1)
        return false;
    return true;
}

int64_t
Function::stackDeltaConcrete() const {
    BaseSemantics::SValuePtr v = stackDelta();
    if (v && v->is_number() && v->get_width() <= 64)
        return IntegerOps::signExtend2<uint64_t>(v->get_number(), v->get_width(), 64);
    return SgAsmInstruction::INVALID_STACK_DELTA;
}

} // namespace
} // namespace
} // namespace
