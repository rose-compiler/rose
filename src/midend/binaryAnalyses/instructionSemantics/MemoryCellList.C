#include <sage3basic.h>
#include <MemoryCellList.h>

namespace rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
namespace BaseSemantics {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      MemoryCell
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool
MemoryCell::may_alias(const MemoryCellPtr &other, RiscOperators *addrOps) const
{
    // Check for the easy case:  two one-byte cells may alias one another if their addresses may be equal.
    if (8==value_->get_width() && 8==other->get_value()->get_width())
        return address_->may_equal(other->get_address(), addrOps->get_solver());

    size_t addr_nbits = address_->get_width();
    ASSERT_require(other->get_address()->get_width()==addr_nbits);

    ASSERT_require(value_->get_width() % 8 == 0);       // memory is byte addressable, so values must be multiples of a byte
    SValuePtr lo1 = address_;
    SValuePtr hi1 = addrOps->add(lo1, addrOps->number_(lo1->get_width(), value_->get_width() / 8));

    ASSERT_require(other->get_value()->get_width() % 8 == 0);
    SValuePtr lo2 = other->get_address();
    SValuePtr hi2 = addrOps->add(lo2, addrOps->number_(lo2->get_width(), other->get_value()->get_width() / 8));

    // Two cells may_alias iff we can prove that they are not disjoint.  The two cells are disjoint iff lo2 >= hi1 or lo1 >=
    // hi2. Two things complicate this: first, the values might not be known quantities, depending on the semantic domain.
    // Second, the RiscOperators does not define a greater-than-or-equal operation, so we need to write it in terms of a
    // subtraction. See x86 CMP and JG instructions for examples. ("sf" is sign flag, "of" is overflow flag.)
    SValuePtr carries;
    SValuePtr diff = addrOps->addWithCarries(lo2, addrOps->invert(hi1), addrOps->boolean_(true), carries/*out*/);
    SValuePtr sf = addrOps->extract(diff, addr_nbits-1, addr_nbits);
    SValuePtr of = addrOps->xor_(addrOps->extract(carries, addr_nbits-1, addr_nbits),
                                 addrOps->extract(carries, addr_nbits-2, addr_nbits-1));
    SValuePtr cond1 = addrOps->invert(addrOps->xor_(sf, of));
    diff = addrOps->addWithCarries(lo1, addrOps->invert(hi2), addrOps->boolean_(true), carries/*out*/);
    sf = addrOps->extract(diff, addr_nbits-1, addr_nbits);
    of = addrOps->xor_(addrOps->extract(carries, addr_nbits-1, addr_nbits),
                       addrOps->extract(carries, addr_nbits-2, addr_nbits-1));
    SValuePtr cond2 = addrOps->invert(addrOps->xor_(sf, of));
    SValuePtr disjoint = addrOps->or_(cond1, cond2);
    if (disjoint->is_number() && disjoint->get_number()!=0)
        return false;
    return true;
}

bool
MemoryCell::must_alias(const MemoryCellPtr &other, RiscOperators *addrOps) const
{
    // Check the easy case: two one-byte cells must alias one another if their address must be equal.
    if (8==value_->get_width() && 8==other->get_value()->get_width())
        return address_->must_equal(other->get_address(), addrOps->get_solver());

    size_t addr_nbits = address_->get_width();
    ASSERT_require(other->get_address()->get_width()==addr_nbits);

    ASSERT_require(value_->get_width() % 8 == 0);
    SValuePtr lo1 = address_;
    SValuePtr hi1 = addrOps->add(lo1, addrOps->number_(lo1->get_width(), value_->get_width() / 8));

    ASSERT_require(other->get_value()->get_width() % 8 == 0);
    SValuePtr lo2 = other->get_address();
    SValuePtr hi2 = addrOps->add(lo2, addrOps->number_(lo2->get_width(), other->get_value()->get_width() / 8));

    // Two cells must_alias iff hi2 >= lo1 and hi1 >= lo2. Two things complicate this: first, the values might not be known
    // quantities, depending on the semantic domain.  Second, the RiscOperators does not define a greater-than-or-equal
    // operation, so we need to write it in terms of a subtraction. See x86 CMP and JG instructions for examples. ("sf" is sign
    // flag, "of" is overflow flag.)
    SValuePtr carries;
    SValuePtr diff = addrOps->addWithCarries(hi2, addrOps->invert(lo1), addrOps->boolean_(true), carries/*out*/);
    SValuePtr sf = addrOps->extract(diff, addr_nbits-1, addr_nbits);
    SValuePtr of = addrOps->xor_(addrOps->extract(carries, addr_nbits-1, addr_nbits),
                                 addrOps->extract(carries, addr_nbits-2, addr_nbits-1));
    SValuePtr cond1 = addrOps->invert(addrOps->xor_(sf, of));
    diff = addrOps->addWithCarries(hi1, addrOps->invert(lo2), addrOps->boolean_(true), carries/*out*/);
    sf = addrOps->extract(diff, addr_nbits-1, addr_nbits);
    of = addrOps->xor_(addrOps->extract(carries, addr_nbits-1, addr_nbits),
                       addrOps->extract(carries, addr_nbits-2, addr_nbits-1));
    SValuePtr cond2 = addrOps->invert(addrOps->xor_(sf, of));
    SValuePtr overlap = addrOps->and_(cond1, cond2);
    if (overlap->is_number() && overlap->get_number()!=0)
        return true;
    return false;
}

void
MemoryCell::print(std::ostream &stream, Formatter &fmt) const
{
    stream <<"addr=" <<(*address_+fmt);

    if (fmt.get_show_latest_writers()) {
        const AddressSet &writers = getWriters();
        if (writers.isEmpty()) {
            // nothing to show
        } else if (writers.size() == 1) {
            stream <<" writer=" <<StringUtility::addrToString(*writers.values().begin());
        } else {
            stream <<" writers=[";
            for (AddressSet::ConstIterator iter=writers.values().begin(); iter!=writers.values().end(); ++iter) {
                stream <<(iter==writers.values().begin() ? "" : ", ")
                       <<StringUtility::addrToString(*iter);
            }
            stream <<"]";
        }
    }

    // FIXME[Robb P. Matzke 2015-08-12]: This doesn't take into account all combinations of properties -- just a few of the
    // more common ones.
    if (fmt.get_show_properties()) {
        if (ioProperties_.exists(IO_READ_BEFORE_WRITE)) {
            stream <<" read-before-write";
        } else if (ioProperties_.exists(IO_WRITE) && ioProperties_.exists(IO_READ)) {
            // nothing
        } else if (ioProperties_.exists(IO_READ)) {
            stream <<" read-only";
        } else if (ioProperties_.exists(IO_WRITE)) {
            stream <<" write-only";
        }
    }

    stream <<" value=" <<(*value_+fmt);
}

void
MemoryCell::setWriter(rose_addr_t writerVa) {
    eraseWriters();
    writers_.insert(writerVa);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      MemoryCellList
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SValuePtr
MemoryCellList::readMemory(const SValuePtr &addr, const SValuePtr &dflt, RiscOperators *addrOps, RiscOperators *valOps) {
    CellList::iterator cursor = get_cells().begin();
    CellList cells = scan(cursor /*in,out*/, addr, dflt->get_width(), addrOps, valOps);
    SValuePtr retval = mergeCellValues(cells, dflt, addrOps, valOps);
    updateReadProperties(cells);
    if (cells.empty()) {
        // No matching cells
        insertReadCell(addr, retval);
    } else if (cursor == get_cells().end()) {
        // No must_equal match and at least one may_equal match. We must merge the default into the return value and save the
        // result back into the cell list.
        retval = retval->createMerged(dflt, merger(), valOps->get_solver());
        AddressSet writers = mergeCellWriters(cells);
        InputOutputPropertySet props = mergeCellProperties(cells);
        insertReadCell(addr, retval, writers, props);
    } else if (cells.size() == 1) {
        // Exactly one must_equal match (no additional may_equal matches)
    } else {
        // One or more may_equal matches with a final must_equal match.
        AddressSet writers = mergeCellWriters(cells);
        InputOutputPropertySet props = mergeCellProperties(cells);
        insertReadCell(addr, retval, writers, props);
    }
    return retval;
}

void
MemoryCellList::writeMemory(const SValuePtr &addr, const SValuePtr &value, RiscOperators *addrOps, RiscOperators *valOps)
{
    ASSERT_not_null(addr);
    ASSERT_require(!byte_restricted || value->get_width() == 8);
    MemoryCellPtr newCell = protocell->create(addr, value);

    if (addrOps->get_insn() || valOps->get_insn()) {
        newCell->ioProperties().insert(IO_WRITE);
    } else {
        newCell->ioProperties().insert(IO_INIT);
    }

    // Prune away all cells that must-alias this new one since they will be occluded by this new one.
    if (occlusionsErased_) {
        for (CellList::iterator cli=cells.begin(); cli!=cells.end(); /*void*/) {
            MemoryCellPtr oldCell = *cli;
            if (newCell->must_alias(oldCell, addrOps)) {
                cli = cells.erase(cli);
            } else {
                ++cli;
            }
        }
    }

    // Insert the new cell
    cells.push_front(newCell);
    latest_written_cell = newCell;
}

bool
MemoryCellList::merge(const MemoryStatePtr &other_, RiscOperators *addrOps, RiscOperators *valOps) {
    MemoryCellListPtr other = boost::dynamic_pointer_cast<MemoryCellList>(other_);
    ASSERT_not_null(other);
    bool changed = false;

    BOOST_REVERSE_FOREACH (const MemoryCellPtr &otherCell, other->get_cells()) {
        // Is there some later-in-time (earlier-in-list) cell that occludes this one? If so, then we don't need to process this
        // cell.
        bool isOccluded = false;
        BOOST_FOREACH (const MemoryCellPtr &cell, other->get_cells()) {
            if (cell == otherCell) {
                break;
            } else if (otherCell->get_address()->must_equal(cell->get_address(), addrOps->get_solver())) {
                isOccluded = true;
            }
        }
        if (isOccluded)
            continue;

        // Read the value, writers, and properties without disturbing the states
        SValuePtr address = otherCell->get_address();

        CellList::iterator otherCursor = other->get_cells().begin();
        CellList otherCells = other->scan(otherCursor /*in,out*/, address, 8, addrOps, valOps);
        SValuePtr otherValue = mergeCellValues(otherCells, valOps->undefined_(8), addrOps, valOps);
        AddressSet otherWriters = mergeCellWriters(otherCells);
        InputOutputPropertySet otherProps = mergeCellProperties(otherCells);

        CellList::iterator thisCursor = get_cells().begin();
        CellList thisCells = scan(thisCursor /*in,out*/, address, 8, addrOps, valOps);

        // Merge cell values
        if (thisCells.empty()) {
            writeMemory(address, otherValue, addrOps, valOps);
            latest_written_cell->setWriters(otherWriters);
            latest_written_cell->ioProperties() = otherProps;
            changed = true;
        } else {
            bool cellChanged = false;
            SValuePtr thisValue = mergeCellValues(thisCells, valOps->undefined_(8), addrOps, valOps);
            SValuePtr mergedValue = thisValue->createOptionalMerge(otherValue, merger(), valOps->get_solver()).orDefault();
            if (mergedValue)
                cellChanged = true;

            AddressSet thisWriters = mergeCellWriters(thisCells);
            AddressSet mergedWriters = otherWriters | thisWriters;
            if (mergedWriters != thisWriters)
                cellChanged = true;

            InputOutputPropertySet thisProps = mergeCellProperties(thisCells);
            InputOutputPropertySet mergedProps = otherProps | thisProps;
            if (mergedProps != thisProps)
                cellChanged = true;

            if (cellChanged) {
                if (!mergedValue)
                    mergedValue = thisValue->copy();
                writeMemory(address, mergedValue, addrOps, valOps);
                latest_written_cell->setWriters(mergedWriters);
                latest_written_cell->ioProperties() = mergedProps;
                changed = true;
            }
        }
    }
    return changed;
}

void
MemoryCellList::clearNonWritten() {
    for (CellList::iterator ci=cells.begin(); ci!=cells.end(); ++ci) {
        if ((*ci)->getWriters().isEmpty())
            *ci = MemoryCellPtr();
    }
    cells.erase(std::remove(cells.begin(), cells.end(), MemoryCellPtr()), cells.end());
}

SValuePtr
MemoryCellList::mergeCellValues(const CellList &cells, const SValuePtr &dflt, RiscOperators *addrOps, RiscOperators *valOps) {
    SValuePtr retval;
    BOOST_FOREACH (const MemoryCellPtr &cell, cells) {
        // Get the cell's value. If the cell value is not the same width as the desired return value then we've go more work to
        // do. This isn't implemented yet. [Robb P. Matzke 2015-08-17]
        SValuePtr cellValue = valOps->unsignedExtend(cell->get_value(), dflt->get_width());

        if (!retval) {
            retval = cellValue;
        } else {
            retval = retval->createMerged(cellValue, merger(), valOps->get_solver());
        }
    }

    return retval ? retval : dflt;
}

MemoryCellList::AddressSet
MemoryCellList::mergeCellWriters(const CellList &cells) {
    AddressSet writers;
    BOOST_FOREACH (const MemoryCellPtr &cell, cells)
        writers |= cell->getWriters();
    return writers;
}

InputOutputPropertySet
MemoryCellList::mergeCellProperties(const CellList &cells) {
    InputOutputPropertySet props;
    BOOST_FOREACH (const MemoryCellPtr &cell, cells)
        props |= cell->ioProperties();
    return props;
}

void
MemoryCellList::updateReadProperties(CellList &cells) {
    BOOST_FOREACH (MemoryCellPtr &cell, cells) {
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

MemoryCellPtr
MemoryCellList::insertReadCell(const SValuePtr &addr, const SValuePtr &value) {
    MemoryCellPtr cell = protocell->create(addr, value);
    cell->ioProperties().insert(IO_READ);
    cell->ioProperties().insert(IO_READ_BEFORE_WRITE);
    cell->ioProperties().insert(IO_READ_UNINITIALIZED);
    cells.push_front(cell);
    return cell;
}

MemoryCellPtr
MemoryCellList::insertReadCell(const SValuePtr &addr, const SValuePtr &value,
                               const AddressSet &writers, const InputOutputPropertySet &props) {
    MemoryCellPtr cell = protocell->create(addr, value);
    cell->setWriters(writers);
    cell->ioProperties() = props;
    cells.push_front(cell);
    return cell;
}

MemoryCellList::AddressSet
MemoryCellList::getWritersUnion(const SValuePtr &addr, size_t nBits, RiscOperators *addrOps, RiscOperators *valOps) {
    AddressSet retval;
    CellList::iterator cursor = get_cells().begin();
    BOOST_FOREACH (const MemoryCellPtr &cell, scan(cursor, addr, nBits, addrOps, valOps))
        retval |= cell->getWriters();
    return retval;
}

MemoryCellList::AddressSet
MemoryCellList::getWritersIntersection(const SValuePtr &addr, size_t nBits, RiscOperators *addrOps, RiscOperators *valOps) {
    AddressSet retval;
    CellList::iterator cursor = get_cells().begin();
    size_t nCells = 0;
    BOOST_FOREACH (const MemoryCellPtr &cell, scan(cursor, addr, nBits, addrOps, valOps)) {
        if (1 == ++nCells) {
            retval = cell->getWriters();
        } else {
            retval &= cell->getWriters();
        }
        if (retval.isEmpty())
            break;
    }
    return retval;
}

// [Robb P. Matzke 2015-08-10]: deprecated; use getWritersUnion instead.
std::set<rose_addr_t>
MemoryCellList::get_latest_writers(const SValuePtr &addr, size_t nbits, RiscOperators *addrOps, RiscOperators *valOps) {
    AddressSet writers = getWritersUnion(addr, nbits, addrOps, valOps);
    std::set<rose_addr_t> retval;
    BOOST_FOREACH (rose_addr_t va, writers.values())
        retval.insert(va);
    return retval;
}

void
MemoryCellList::print(std::ostream &stream, Formatter &fmt) const
{
    for (CellList::const_iterator ci=cells.begin(); ci!=cells.end(); ++ci)
        stream <<fmt.get_line_prefix() <<(**ci+fmt) <<"\n";
}

// [Robb P. Matzke 2015-08-18]: deprecated
MemoryCellList::CellList
MemoryCellList::scan(const BaseSemantics::SValuePtr &addr, size_t nbits, RiscOperators *addrOps, RiscOperators *valOps,
                     bool &short_circuited/*out*/) const {
    CellList::const_iterator cursor = get_cells().begin();
    CellList retval = scan(cursor, addr, nbits, addrOps, valOps);
    short_circuited = cursor != get_cells().end();
    return retval;
}

void
MemoryCellList::traverse(Visitor &visitor)
{
    for (CellList::iterator ci=cells.begin(); ci!=cells.end(); ++ci)
        (visitor)(*ci);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Free functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream &o, const MemoryCell &x) {
    x.print(o);
    return o;
}

std::ostream& operator<<(std::ostream &o, const MemoryCell::WithFormatter &x)
{
    x.print(o);
    return o;
}

} // namespace
} // namespace
} // namespace
} // namespace
