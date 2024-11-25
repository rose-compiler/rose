#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/MemoryCellState.h>

#include <Rose/As.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Merger.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/SValue.h>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {
namespace BaseSemantics {

MemoryCellState::MemoryCellState() {}

MemoryCellState::MemoryCellState(const MemoryCell::Ptr &protocell)
    : MemoryState(protocell->address(), protocell->value()), protocell(protocell) {}

MemoryCellState::MemoryCellState(const SValue::Ptr &addrProtoval, const SValue::Ptr &valProtoval)
    : MemoryState(addrProtoval, valProtoval), protocell(MemoryCell::instance(addrProtoval, valProtoval)) {}

MemoryCellState::MemoryCellState(const MemoryCellState &other)
    : MemoryState(other), protocell(other.protocell) {} // latestWrittenCell_ is cleared

MemoryCellState::~MemoryCellState() {}

MemoryCellState::Ptr
MemoryCellState::promote(const BaseSemantics::MemoryState::Ptr &m) {
    Ptr retval = as<MemoryCellState>(m);
    ASSERT_not_null(retval);
    return retval;
}

void
MemoryCellState::clear() {
    latestWrittenCell_ = MemoryCell::Ptr();
}

MemoryCell::Ptr
MemoryCellState::latestWrittenCell() const {
    return latestWrittenCell_;
}

void
MemoryCellState::latestWrittenCell(const MemoryCell::Ptr &cell) {
    latestWrittenCell_ = cell;
}

void
MemoryCellState::updateReadProperties(const CellList &cells) {
    for (const MemoryCell::Ptr &cell: cells) {
        cell->ioProperties().insert(IO_READ);
        if (cell->ioProperties().exists(IO_WRITE)) {
            cell->ioProperties().insert(IO_READ_AFTER_WRITE);
        } else {
            cell->ioProperties().insert(IO_READ_BEFORE_WRITE);
        }
        if (!cell->ioProperties().exists(IO_INIT))
            cell->ioProperties().insert(IO_READ_UNINITIALIZED);
    }
}

void
MemoryCellState::updateWriteProperties(const CellList &cells, InputOutputPropertySet properties) {
    for (const MemoryCell::Ptr &cell: cells)
        cell->ioProperties().insert(properties);
}

void
MemoryCellState::eraseNonWritten() {
    MemoryCell::NonWrittenCells p;
    eraseMatchingCells(p);
}

std::vector<MemoryCell::Ptr>
MemoryCellState::allCells() const {
    MemoryCell::AllCells p;
    return matchingCells(p);
}

} // namespace
} // namespace
} // namespace
} // namespace

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics::MemoryCellState);
#endif

#endif
