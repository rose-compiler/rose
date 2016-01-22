#include <sage3basic.h>
#include <MemoryCellMap.h>

namespace rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
namespace BaseSemantics {

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
        retval = cell->get_value();
    } else {
        retval = dflt->copy();
        cell = protocell->create(address, retval);
        cell->ioProperties().insert(IO_READ);
        cell->ioProperties().insert(IO_READ_BEFORE_WRITE);
        cell->ioProperties().insert(IO_READ_UNINITIALIZED);
        cells.insert(key, cell);
    }
    return retval;
}

void
MemoryCellMap::writeMemory(const SValuePtr &address, const SValuePtr &value, RiscOperators *addrOps, RiscOperators *valOps) {
    ASSERT_not_null(address);
    ASSERT_require(!byteRestricted() || value->get_width() == 8);
    MemoryCellPtr newCell = protocell->create(address, value);
    if (addrOps->get_insn() || valOps->get_insn()) {
        newCell->ioProperties().insert(IO_WRITE);
    } else {
        newCell->ioProperties().insert(IO_INIT);
    }

    CellKey key = generateCellKey(address);
    cells.insert(key, newCell);
    latestWrittenCell_ = newCell;
}

bool
MemoryCellMap::merge(const MemoryStatePtr &other_, RiscOperators *addrOps, RiscOperators *valOps) {
    MemoryCellMapPtr other = boost::dynamic_pointer_cast<MemoryCellMap>(other_);
    ASSERT_not_null(other);
    bool changed = false;

    std::set<CellKey> allKeys;                          // union of cell keys from "this" and "other"
    BOOST_FOREACH (const CellKey &key, cells.keys())
        allKeys.insert(key);
    BOOST_FOREACH (const CellKey &key, other->cells.keys())
        allKeys.insert(key);

    BOOST_FOREACH (const CellKey &key, allKeys) {
        const MemoryCellPtr &thisCell  = cells.getOrDefault(key);
        const MemoryCellPtr &otherCell = other->cells.getOrDefault(key);
        bool thisCellChanged = false;

        ASSERT_require(thisCell != NULL || otherCell != NULL);
        SValuePtr thisValue  = thisCell  ? thisCell->get_value()  : valOps->undefined_(otherCell->get_value()->get_width());
        SValuePtr otherValue = otherCell ? otherCell->get_value() : valOps->undefined_(thisCell->get_value()->get_width());
        SValuePtr newValue   = thisValue->createOptionalMerge(otherValue, merger(), valOps->solver()).orDefault();
        if (newValue)
            thisCellChanged = true;

        MemoryCell::AddressSet thisWriters  = thisCell  ? thisCell->getWriters()  : MemoryCell::AddressSet();
        MemoryCell::AddressSet otherWriters = otherCell ? otherCell->getWriters() : MemoryCell::AddressSet();
        MemoryCell::AddressSet newWriters = otherWriters | thisWriters;
        if (newWriters != thisWriters)
            thisCellChanged = true;

        InputOutputPropertySet thisProps  = thisCell  ? thisCell->ioProperties()  : InputOutputPropertySet();
        InputOutputPropertySet otherProps = otherCell ? otherCell->ioProperties() : InputOutputPropertySet();
        InputOutputPropertySet newProps = otherProps | thisProps;
        if (newProps != thisProps)
            thisCellChanged = true;

        if (thisCellChanged) {
            if (!newValue)
                newValue = thisValue->copy();
            SValuePtr address = thisCell ? thisCell->get_address() : otherCell->get_address();
            writeMemory(address, newValue, addrOps, valOps);
            latestWrittenCell_->setWriters(newWriters);
            latestWrittenCell_->ioProperties() = newProps;
            changed = true;
        }
    }
    return changed;
}
    
void
MemoryCellMap::print(std::ostream &out, Formatter &fmt) const {
    BOOST_FOREACH (const MemoryCellPtr &cell, cells.values())
        out <<fmt.get_line_prefix() <<(*cell+fmt) <<"\n";
}

void
MemoryCellMap::traverse(MemoryCell::Visitor &visitor) {
    CellMap newMap;
    BOOST_FOREACH (MemoryCellPtr &cell, cells.values()) {
        (visitor)(cell);
        newMap.insert(generateCellKey(cell->get_address()), cell);
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

} // namespace
} // namespace
} // namespace
} // namespace
