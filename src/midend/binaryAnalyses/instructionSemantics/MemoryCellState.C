#include <sage3basic.h>

#include <MemoryCellState.h>

namespace rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
namespace BaseSemantics {

void
MemoryCellState::clear() {
    latestWrittenCell_ = MemoryCellPtr();
}

} // namespace
} // namespace
} // namespace
} // namespace
