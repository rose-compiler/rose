#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/InstructionSemantics2/BaseSemantics/MemoryCellMap.h>

#include <Rose/BinaryAnalysis/InstructionSemantics2/BaseSemantics/Formatter.h>
#include <Rose/BinaryAnalysis/InstructionSemantics2/BaseSemantics/Merger.h>
#include <Rose/BinaryAnalysis/InstructionSemantics2/BaseSemantics/RiscOperators.h>
#include <Rose/BinaryAnalysis/InstructionSemantics2/BaseSemantics/SValue.h>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
namespace BaseSemantics {

MemoryCellMap::MemoryCellMap(const MemoryCellMap &other)
    : MemoryCellState(other) {
    for (const MemoryCellPtr &cell: other.cells.values()) {
        cells.insert(other.generateCellKey(cell->address()), cell->clone());
        lastPosition_ = std::max(lastPosition_, cell->position());
    }
}

unsigned
MemoryCellMap::nextPosition() {
    return ++lastPosition_;
}

unsigned
MemoryCellMap::lastPosition() const {
    return lastPosition_;
}

void
MemoryCellMap::lastPosition(unsigned p) {
    lastPosition_ = p;
}

void
MemoryCellMap::clear() {
    cells.clear();
    MemoryCellState::clear();
}

SValuePtr
MemoryCellMap::readMemory(const SValuePtr &address, const SValuePtr &dflt, RiscOperators *addrOps, RiscOperators *valOps) {
    SValuePtr retval;
    CellKey key = generateCellKey(address);
    if (MemoryCellPtr cell = cells.getOrDefault(key)) {
        cell->position(nextPosition());
        retval = cell->value();
    } else {
        retval = dflt->copy();
        cell = protocell->create(address, retval);
        cell->ioProperties().insert(IO_READ);
        cell->ioProperties().insert(IO_READ_BEFORE_WRITE);
        cell->ioProperties().insert(IO_READ_UNINITIALIZED);
        cell->position(nextPosition());
        cells.insert(key, cell);
    }
    return retval;
}

SValuePtr
MemoryCellMap::peekMemory(const SValuePtr &address, const SValuePtr &dflt, RiscOperators *addrOps, RiscOperators *valOps) {
    // Just like readMemory except no side effects
    SValuePtr retval;
    CellKey key = generateCellKey(address);
    if (MemoryCellPtr cell = cells.getOrDefault(key)) {
        retval = cell->value();
    } else {
        retval = dflt->copy();
    }
    return retval;
}

void
MemoryCellMap::writeMemory(const SValuePtr &address, const SValuePtr &value, RiscOperators *addrOps, RiscOperators *valOps) {
    ASSERT_not_null(address);
    ASSERT_require(!byteRestricted() || value->nBits() == 8);
    MemoryCellPtr newCell = protocell->create(address, value);
    if (addrOps->currentInstruction() || valOps->currentInstruction()) {
        newCell->ioProperties().insert(IO_WRITE);
    } else {
        newCell->ioProperties().insert(IO_INIT);
    }
    newCell->position(nextPosition());

    CellKey key = generateCellKey(address);
    cells.insert(key, newCell);
    latestWrittenCell_ = newCell;
}

bool
MemoryCellMap::isAllPresent(const SValuePtr &address, size_t nBytes, RiscOperators *addrOps) const {
    ASSERT_not_null(addrOps);
    for (size_t offset = 0; offset < nBytes; ++offset) {
        SValuePtr byteAddress = 0==offset ? address : addrOps->add(address, addrOps->number_(address->nBits(), offset));
        CellKey key = generateCellKey(byteAddress);
        if (!cells.exists(key))
            return false;
    }
    return true;
}

bool
MemoryCellMap::merge(const MemoryStatePtr &other_, RiscOperators *addrOps, RiscOperators *valOps) {
    MemoryCellMapPtr other = boost::dynamic_pointer_cast<MemoryCellMap>(other_);
    ASSERT_not_null(other);
    bool changed = false;
    unsigned otherBasePosition = this->lastPosition();
    unsigned maxPosition = 0;

    std::set<CellKey> allKeys;                          // union of cell keys from "this" and "other"
    for (const CellKey &key: cells.keys())
        allKeys.insert(key);
    for (const CellKey &key: other->cells.keys())
        allKeys.insert(key);

    for (const CellKey &key: allKeys) {
        const MemoryCellPtr &thisCell  = cells.getOrDefault(key);
        const MemoryCellPtr &otherCell = other->cells.getOrDefault(key);
        bool thisCellChanged = false;

        ASSERT_require(thisCell != NULL || otherCell != NULL);
        SValuePtr thisValue  = thisCell  ? thisCell->value()  : valOps->undefined_(otherCell->value()->nBits());
        SValuePtr otherValue = otherCell ? otherCell->value() : valOps->undefined_(thisCell->value()->nBits());
        SValuePtr newValue   = thisValue->createOptionalMerge(otherValue, merger(), valOps->solver()).orDefault();
        if (newValue)
            thisCellChanged = true;

        AddressSet thisWriters  = thisCell  ? thisCell->getWriters()  : AddressSet();
        AddressSet otherWriters = otherCell ? otherCell->getWriters() : AddressSet();
        AddressSet newWriters = otherWriters | thisWriters;
        if (newWriters != thisWriters)
            thisCellChanged = true;

        InputOutputPropertySet thisProps  = thisCell  ? thisCell->ioProperties()  : InputOutputPropertySet();
        InputOutputPropertySet otherProps = otherCell ? otherCell->ioProperties() : InputOutputPropertySet();
        InputOutputPropertySet newProps = otherProps | thisProps;
        if (newProps != thisProps)
            thisCellChanged = true;

        unsigned position = thisCell ? thisCell->position() : otherBasePosition + otherCell->position();
        maxPosition = std::max(maxPosition, position);

        if (thisCellChanged) {
            if (!newValue)
                newValue = thisValue->copy();
            SValuePtr address = thisCell ? thisCell->address() : otherCell->address();
            writeMemory(address, newValue, addrOps, valOps);
            latestWrittenCell_->setWriters(newWriters);
            latestWrittenCell_->ioProperties() = newProps;
            latestWrittenCell_->position(position);
            changed = true;
        }
    }

    if (changed)
        this->lastPosition(maxPosition);

    return changed;
}
    
void
MemoryCellMap::print(std::ostream &out, Formatter &fmt) const {
    // For better human readability, print the cells in order of descending position. This generally corresponds to reverse
    // chronological order.
    std::vector<MemoryCellPtr> sorted;
    sorted.reserve(cells.size());
    for (const MemoryCellPtr &cell: cells.values())
        sorted.push_back(cell);
    std::sort(sorted.begin(), sorted.end(), [](const MemoryCellPtr &a, const MemoryCellPtr &b) {
            return a->position() > b->position();
        });

    for (const MemoryCellPtr &cell: sorted)
        out <<fmt.get_line_prefix() <<(*cell+fmt) <<"\n";
}

void
MemoryCellMap::traverse(MemoryCell::Visitor &visitor) {
    CellMap newMap;
    for (MemoryCellPtr &cell: cells.values()) {
        (visitor)(cell);
        newMap.insert(generateCellKey(cell->address()), cell);
    }
    cells = newMap;
}
    
std::vector<MemoryCellPtr>
MemoryCellMap::matchingCells(const MemoryCell::Predicate &p) const {
    std::vector<MemoryCellPtr> retval;
    BOOST_FOREACH (const MemoryCellPtr &cell, cells.values()) {
        if (p(cell))
            retval.push_back(cell);
    }
    return retval;
}

std::vector<MemoryCellPtr>
MemoryCellMap::leadingCells(const MemoryCell::Predicate &p) const {
    std::vector<MemoryCellPtr> retval;
    BOOST_FOREACH (const MemoryCellPtr &cell, cells.values()) {
        if (!p(cell))
            break;
        retval.push_back(cell);
    }
    return retval;
}

void
MemoryCellMap::eraseMatchingCells(const MemoryCell::Predicate &p) {
    CellMap tmp = cells;
    BOOST_FOREACH (const CellMap::Node &cell, tmp.nodes()) {
        if (p(cell.value()))
            cells.erase(cell.key());
    }
}

void
MemoryCellMap::eraseLeadingCells(const MemoryCell::Predicate &p) {
    CellMap tmp = cells;
    BOOST_FOREACH (const CellMap::Node &cell, tmp.nodes()) {
        if (!p(cell.value()))
            break;
        cells.erase(cell.key());
    }
}

MemoryCellPtr
MemoryCellMap::findCell(const SValuePtr &addr) const {
    return cells.getOrDefault(generateCellKey(addr));
}

AddressSet
MemoryCellMap::getWritersUnion(const SValuePtr &addr, size_t nBits, RiscOperators *addrOps, RiscOperators *valOps) {
    AddressSet retval;
    CellKey key = generateCellKey(addr);
    if (MemoryCellPtr cell = cells.getOrDefault(key))
        retval = cell->getWriters();
    return retval;
}

AddressSet
MemoryCellMap::getWritersIntersection(const SValuePtr &addr, size_t nBits, RiscOperators *addrOps, RiscOperators *valOps) {
    AddressSet retval;
    CellKey key = generateCellKey(addr);
    if (MemoryCellPtr cell = cells.getOrDefault(key))
        retval = cell->getWriters();
    return retval;
}

void
MemoryCellMap::hash(Combinatorics::Hasher &hasher, RiscOperators*/*addrOps*/, RiscOperators*/*valOps*/) const {
    for (const CellMap::Node &node: cells.nodes()) {
        hasher.insert(node.key());
        node.value()->hash(hasher);
    }
}

} // namespace
} // namespace
} // namespace
} // namespace

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::MemoryCellMap);
#endif

#endif
