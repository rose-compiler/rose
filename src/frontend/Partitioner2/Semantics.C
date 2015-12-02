#include "sage3basic.h"
#include <Partitioner2/Semantics.h>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {
namespace Semantics {

namespace BaseSemantics = rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics;
namespace SymbolicSemantics = rose::BinaryAnalysis::InstructionSemantics2::SymbolicSemantics;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Memory State
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const MemoryMap*
MemoryState::memoryMap() const {
    return map_;
}

BaseSemantics::SValuePtr
MemoryState::readMemory(const BaseSemantics::SValuePtr &addr, const BaseSemantics::SValuePtr &dflt,
                        BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps) {
    if (!enabled_)
        return dflt->copy();

    addressesRead_.push_back(SValue::promote(addr));
    if (map_ && addr->is_number()) {
        ASSERT_require2(8==dflt->get_width(), "multi-byte reads should have been handled above this call");
        rose_addr_t va = addr->get_number();
        bool isModifiable = map_->at(va).require(MemoryMap::WRITABLE).exists();
        bool isInitialized = map_->at(va).require(MemoryMap::INITIALIZED).exists();
        if (!isModifiable || isInitialized) {
            uint8_t byte;
            if (1 == map_->at(va).limit(1).read(&byte).size()) {
                unsigned flags = isModifiable ? SymbolicExpr::Node::INDETERMINATE : 0;
                SymbolicExpr::Ptr expr = SymbolicExpr::makeInteger(8, byte, "", flags);
                SymbolicSemantics::SValuePtr val = SymbolicSemantics::SValue::promote(valOps->undefined_(8));
                val->set_expression(expr);
                return val;
            }
        }
    }
    return SymbolicSemantics::MemoryState::readMemory(addr, dflt, addrOps, valOps);
}
        
void
MemoryState::writeMemory(const BaseSemantics::SValuePtr &addr, const BaseSemantics::SValuePtr &value,
                         BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps) {
    if (!enabled_)
        return;
    SymbolicSemantics::MemoryState::writeMemory(addr, value, addrOps, valOps);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Risc Operators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RiscOperators::startInstruction(SgAsmInstruction *insn) {
    ASSERT_not_null(get_state());
    ASSERT_not_null(get_state()->get_memory_state());
    MemoryState::promote(get_state()->get_memory_state())->addressesRead().clear();
    SymbolicSemantics::RiscOperators::startInstruction(insn);
}

} // namespace
} // namespace
} // namespace
} // namespace
