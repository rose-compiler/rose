#include <sage3basic.h>
#include <MemoryCellMap.h>

namespace rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
namespace BaseSemantics {

void
MemoryCellMap::clear() {
    cells.clear();
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

    BOOST_FOREACH (const MemoryCellPtr &otherCell, other->cells.values()) {
        bool thisCellChanged = false;
        CellKey key = generateCellKey(otherCell->get_address());
        if (const MemoryCellPtr &thisCell = cells.getOrDefault(key)) {
            SValuePtr otherValue = otherCell->get_value();
            SValuePtr thisValue = thisCell->get_value();
            SValuePtr newValue = thisValue->createOptionalMerge(otherValue, merger(), valOps->get_solver()).orDefault();
            if (newValue)
                thisCellChanged = true;

            MemoryCell::AddressSet otherWriters = otherCell->getWriters();
            MemoryCell::AddressSet thisWriters = thisCell->getWriters();
            MemoryCell::AddressSet newWriters = otherWriters | thisWriters;
            if (newWriters != thisWriters)
                thisCellChanged = true;

            InputOutputPropertySet otherProps = otherCell->ioProperties();
            InputOutputPropertySet thisProps = thisCell->ioProperties();
            InputOutputPropertySet newProps = otherProps | thisProps;
            if (newProps != thisProps)
                thisCellChanged = true;

            if (thisCellChanged) {
                if (!newValue)
                    newValue = thisValue->copy();
                writeMemory(thisCell->get_address(), newValue, addrOps, valOps);
                latestWrittenCell_->setWriters(newWriters);
                latestWrittenCell_->ioProperties() = newProps;
                changed = true;
            }
        }
    }
    return changed;
}
    
void
MemoryCellMap::print(std::ostream &out, Formatter &fmt) const {
    BOOST_FOREACH (const MemoryCellPtr &cell, cells.values())
        out <<fmt.get_line_prefix() <<(*cell+fmt) <<"\n";
}
    



} // namespace
} // namespace
} // namespace
} // namespace
