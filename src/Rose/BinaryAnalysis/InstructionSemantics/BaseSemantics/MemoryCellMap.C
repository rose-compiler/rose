#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/MemoryCellMap.h>

#include <Rose/As.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Formatter.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Merger.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/RiscOperators.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/SValue.h>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {
namespace BaseSemantics {

MemoryCellMap::~MemoryCellMap() {}

MemoryCellMap::MemoryCellMap() {}

MemoryCellMap::MemoryCellMap(const MemoryCell::Ptr &protocell)
    : MemoryCellState(protocell) {}

MemoryCellMap::MemoryCellMap(const SValue::Ptr &addrProtoval, const SValue::Ptr &valProtoval)
    : MemoryCellState(addrProtoval, valProtoval) {}

MemoryCellMap::MemoryCellMap(const MemoryCellMap &other)
    : MemoryCellState(other) {
    for (const MemoryCell::Ptr &cell: other.cells.values()) {
        cells.insert(other.generateCellKey(cell->address()), cell->clone());
        lastPosition_ = std::max(lastPosition_, cell->position());
    }
}

MemoryCellMap::Ptr
MemoryCellMap::promote(const AddressSpace::Ptr &x) {
    Ptr retval = as<MemoryCellMap>(x);
    ASSERT_not_null(retval);
    return retval;
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

SValue::Ptr
MemoryCellMap::readMemory(const SValue::Ptr &address, const SValue::Ptr &dflt,
                          RiscOperators */*addrOps*/, RiscOperators */*valOps*/) {
    SValue::Ptr retval;
    CellKey key = generateCellKey(address);
    MemoryCell::Ptr cell = cells.getOrDefault(key);
    if (cell) {
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

    ASSERT_not_null(cell);
    CellList cellList;
    cellList.push_back(cell);
    updateReadProperties(cellList);
    return retval;
}

SValue::Ptr
MemoryCellMap::peekMemory(const SValue::Ptr &address, const SValue::Ptr &dflt,
                          RiscOperators */*addrOps*/, RiscOperators */*valOps*/) {
    // Just like readMemory except no side effects
    SValue::Ptr retval;
    CellKey key = generateCellKey(address);
    if (MemoryCell::Ptr cell = cells.getOrDefault(key)) {
        retval = cell->value();
    } else {
        retval = dflt->copy();
    }
    return retval;
}

void
MemoryCellMap::writeMemory(const SValue::Ptr &address, const SValue::Ptr &value, RiscOperators *addrOps, RiscOperators *valOps) {
    ASSERT_not_null(address);
    ASSERT_require(!byteRestricted() || value->nBits() == 8);
    MemoryCell::Ptr newCell = protocell->create(address, value);
    newCell->position(nextPosition());

    // Update I/O properties
    InputOutputPropertySet wprops;
    if (addrOps->currentInstruction() || valOps->currentInstruction()) {
        wprops.insert(IO_WRITE);
    } else {
        wprops.insert(IO_INIT);
    }
    updateWriteProperties(CellList{newCell}, wprops);

    CellKey key = generateCellKey(address);
    cells.insert(key, newCell);
    latestWrittenCell_ = newCell;
}

bool
MemoryCellMap::isAllPresent(const SValue::Ptr &address, size_t nBytes, RiscOperators *addrOps) const {
    ASSERT_not_null(addrOps);
    for (size_t offset = 0; offset < nBytes; ++offset) {
        SValue::Ptr byteAddress = 0==offset ? address : addrOps->add(address, addrOps->number_(address->nBits(), offset));
        CellKey key = generateCellKey(byteAddress);
        if (!cells.exists(key))
            return false;
    }
    return true;
}

bool
MemoryCellMap::merge(const AddressSpace::Ptr &other_, RiscOperators *addrOps, RiscOperators *valOps) {
    MemoryCellMap::Ptr other = as<MemoryCellMap>(other_);
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
        const MemoryCell::Ptr &thisCell  = cells.getOrDefault(key);
        const MemoryCell::Ptr &otherCell = other->cells.getOrDefault(key);
        bool thisCellChanged = false;

        ASSERT_require(thisCell != NULL || otherCell != NULL);
        SValue::Ptr thisValue  = thisCell  ? thisCell->value()  : valOps->undefined_(otherCell->value()->nBits());
        SValue::Ptr otherValue = otherCell ? otherCell->value() : valOps->undefined_(thisCell->value()->nBits());
        SValue::Ptr newValue   = thisValue->createOptionalMerge(otherValue, merger(), valOps->solver()).orDefault();
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
            SValue::Ptr address = thisCell ? thisCell->address() : otherCell->address();
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
    std::vector<MemoryCell::Ptr> sorted;
    sorted.reserve(cells.size());
    for (const MemoryCell::Ptr &cell: cells.values())
        sorted.push_back(cell);
    std::sort(sorted.begin(), sorted.end(), [](const MemoryCell::Ptr &a, const MemoryCell::Ptr &b) {
            return a->position() > b->position();
        });

    for (const MemoryCell::Ptr &cell: sorted)
        out <<fmt.get_line_prefix() <<(*cell+fmt) <<"\n";
}

void
MemoryCellMap::traverse(MemoryCell::Visitor &visitor) {
    CellMap newMap;
    for (MemoryCell::Ptr &cell: cells.values()) {
        (visitor)(cell);
        newMap.insert(generateCellKey(cell->address()), cell);
    }
    cells = newMap;
}
    
std::vector<MemoryCell::Ptr>
MemoryCellMap::matchingCells(MemoryCell::Predicate &p) const {
    std::vector<MemoryCell::Ptr> retval;
    for (const MemoryCell::Ptr &cell: cells.values()) {
        if (p(cell))
            retval.push_back(cell);
    }
    return retval;
}

std::vector<MemoryCell::Ptr>
MemoryCellMap::leadingCells(MemoryCell::Predicate &p) const {
    std::vector<MemoryCell::Ptr> retval;
    for (const MemoryCell::Ptr &cell: cells.values()) {
        if (!p(cell))
            break;
        retval.push_back(cell);
    }
    return retval;
}

void
MemoryCellMap::eraseMatchingCells(MemoryCell::Predicate &p) {
    CellMap tmp = cells;
    for (const CellMap::Node &cell: tmp.nodes()) {
        if (p(cell.value()))
            cells.erase(cell.key());
    }
}

void
MemoryCellMap::eraseLeadingCells(MemoryCell::Predicate &p) {
    CellMap tmp = cells;
    for (const CellMap::Node &cell: tmp.nodes()) {
        if (!p(cell.value()))
            break;
        cells.erase(cell.key());
    }
}

MemoryCell::Ptr
MemoryCellMap::findCell(const SValue::Ptr &addr) const {
    return cells.getOrDefault(generateCellKey(addr));
}

AddressSet
MemoryCellMap::getWritersUnion(const SValue::Ptr &addr, size_t /*nBits*/, RiscOperators */*addrOps*/, RiscOperators */*valOps*/) {
    AddressSet retval;
    CellKey key = generateCellKey(addr);
    if (MemoryCell::Ptr cell = cells.getOrDefault(key))
        retval = cell->getWriters();
    return retval;
}

AddressSet
MemoryCellMap::getWritersIntersection(const SValue::Ptr &addr, size_t /*nBits*/,
                                      RiscOperators */*addrOps*/, RiscOperators */*valOps*/) {
    AddressSet retval;
    CellKey key = generateCellKey(addr);
    if (MemoryCell::Ptr cell = cells.getOrDefault(key))
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
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics::MemoryCellMap);
#endif

#endif
