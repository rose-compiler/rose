#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/MemoryCellState.h>

#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Merger.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/SValue.h>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {
namespace BaseSemantics {

MemoryCellState::MemoryCellState(const MemoryCellPtr &protocell)
    : MemoryState(protocell->address(), protocell->value()), protocell(protocell) {}

MemoryCellState::MemoryCellState(const SValuePtr &addrProtoval, const SValuePtr &valProtoval)
    : MemoryState(addrProtoval, valProtoval), protocell(MemoryCell::instance(addrProtoval, valProtoval)) {}

MemoryCellState::MemoryCellState(const MemoryCellState &other)
    : MemoryState(other), protocell(other.protocell) {} // latestWrittenCell_ is cleared

MemoryCellState::~MemoryCellState() {}

void
MemoryCellState::clear() {
    latestWrittenCell_ = MemoryCellPtr();
}

} // namespace
} // namespace
} // namespace
} // namespace

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics::MemoryCellState);
#endif

#endif
