#include "sage3basic.h"
#include <Partitioner2/Semantics.h>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {
namespace Semantics {

namespace BaseSemantics = rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics;
namespace SymbolicSemantics = rose::BinaryAnalysis::InstructionSemantics2::SymbolicSemantics;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Risc Operators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RiscOperators::startInstruction(SgAsmInstruction *insn) {
    ASSERT_not_null(currentState());
    ASSERT_not_null(currentState()->memoryState());

    BaseSemantics::MemoryStatePtr mem = currentState()->memoryState();
    if (MemoryListStatePtr ml = boost::dynamic_pointer_cast<MemoryListState>(mem)) {
        ml->addressesRead().clear();
    } else if (MemoryMapStatePtr mm = boost::dynamic_pointer_cast<MemoryMapState>(mem)) {
        mm->addressesRead().clear();
    }
    SymbolicSemantics::RiscOperators::startInstruction(insn);
}

} // namespace
} // namespace
} // namespace
} // namespace
