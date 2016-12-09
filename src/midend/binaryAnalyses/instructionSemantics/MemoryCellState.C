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

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
BOOST_CLASS_EXPORT_IMPLEMENT(rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::MemoryCellState);
#endif
