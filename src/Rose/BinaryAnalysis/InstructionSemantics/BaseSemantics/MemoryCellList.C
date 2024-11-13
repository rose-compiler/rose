#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/MemoryCellList.h>

#include <Rose/As.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Formatter.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Merger.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/RiscOperators.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/SValue.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/Utility.h>
#include <Rose/StringUtility/Diagnostics.h>

#include <boost/range/adaptor/reversed.hpp>

using namespace Sawyer::Message::Common;

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {
namespace BaseSemantics {

void
MemoryCellList::clear() {
    cells.clear();
    MemoryCellState::clear();
}

SValue::Ptr
MemoryCellList::readMemory(const SValue::Ptr &addr, const SValue::Ptr &dflt, RiscOperators *addrOps, RiscOperators *valOps) {
    CellList::iterator cursor = get_cells().begin();
    CellList cells = scan(cursor /*in,out*/, addr, dflt->nBits(), addrOps, valOps);
    SValue::Ptr retval = mergeCellValues(cells, dflt, addrOps, valOps);
    updateReadProperties(cells);
    if (cells.empty()) {
        // No matching cells
        insertReadCell(addr, retval);
    } else if (cursor == get_cells().end()) {
        // No must_equal match and at least one may_equal match. We must merge the default into the return value and save the
        // result back into the cell list.
        retval = retval->createMerged(dflt, merger(), valOps->solver());
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

// identical to readMemory but without side effects
SValue::Ptr
MemoryCellList::peekMemory(const SValue::Ptr &addr, const SValue::Ptr &dflt, RiscOperators *addrOps, RiscOperators *valOps) {
    CellList::iterator cursor = get_cells().begin();
    CellList cells = scan(cursor /*in,out*/, addr, dflt->nBits(), addrOps, valOps);
    SValue::Ptr retval = mergeCellValues(cells, dflt, addrOps, valOps);

    // If there's no must_equal match and at least one may_equal match, then merge the default into the return value.
    if (!cells.empty() && cursor == get_cells().end())
        retval = retval->createMerged(dflt, merger(), valOps->solver());

    return retval;
}

void
MemoryCellList::writeMemory(const SValue::Ptr &addr, const SValue::Ptr &value, RiscOperators *addrOps, RiscOperators *valOps) {
    ASSERT_not_null(addr);
    ASSERT_require(!byteRestricted() || value->nBits() == 8);
    MemoryCell::Ptr newCell = protocell->create(addr, value);

    // Update I/O properties
    InputOutputPropertySet wprops;
    if (addrOps->currentInstruction() || valOps->currentInstruction()) {
        wprops.insert(IO_WRITE);
    } else {
        wprops.insert(IO_INIT);
    }
    updateWriteProperties(CellList{newCell}, wprops);

    // Prune away all cells that must-alias this new one since they will be occluded by this new one.
    if (occlusionsErased_) {
        for (CellList::iterator cli=cells.begin(); cli!=cells.end(); /*void*/) {
            MemoryCell::Ptr oldCell = *cli;
            if (newCell->mustAlias(oldCell, addrOps)) {
                cli = cells.erase(cli);
            } else {
                ++cli;
            }
        }
    }

    // Insert the new cell
    cells.push_front(newCell);
    latestWrittenCell_ = newCell;
}

bool
MemoryCellList::isAllPresent(const SValue::Ptr &address, size_t nBytes, RiscOperators *addrOps, RiscOperators *valOps) const {
    ASSERT_not_null(addrOps);
    ASSERT_not_null(valOps);
    for (size_t offset = 0; offset < nBytes; ++offset) {
        SValue::Ptr byteAddress = 0==offset ? address : addrOps->add(address, addrOps->number_(address->nBits(), offset));
        CellList::const_iterator cursor = get_cells().begin();
        if (scan(cursor/*in,out*/, byteAddress, 8, addrOps, valOps).empty())
            return false;
    }
    return true;
}

bool
MemoryCellList::merge(const MemoryState::Ptr &other, RiscOperators *addrOps, RiscOperators *valOps) {
    if (!merger() || merger()->memoryAddressesMayAlias()) {
        return mergeWithAliasing(other, addrOps, valOps);
    } else {
        return mergeNoAliasing(other, addrOps, valOps);
    }
}

bool
MemoryCellList::mergeWithAliasing(const MemoryState::Ptr &other_, RiscOperators *addrOps, RiscOperators *valOps) {
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    debug.enable(debug.enabled() && merger() && merger()->memoryMergeDebugging());

    MemoryCellList::Ptr other = as<MemoryCellList>(other_);
    ASSERT_not_null(other);
    bool changed = false;

    if (debug) {
        debug <<"MemoryCellList::mergeWithAliasing\n";
        debug <<"  merge into:\n";
        for (const MemoryCell::Ptr &cell: get_cells())
            debug <<"    addr=" <<*cell->address() <<" value=" <<*cell->value() <<"\n";
        debug <<"  merging from:\n";
        for (const MemoryCell::Ptr &cell: other->get_cells())
            debug <<"    addr=" <<*cell->address() <<" value=" <<*cell->value() <<"\n";
    }

    for (const MemoryCell::Ptr &otherCell: boost::adaptors::reverse(other->get_cells())) {
        SAWYER_MESG(debug) <<"  merging from cell"
                           <<" addr=" <<*otherCell->address()
                           <<" value=" <<*otherCell->value() <<"\n";

        // Is there some later-in-time (earlier-in-list) cell that occludes this one? If so, then we don't need to process this
        // cell.
        bool isOccluded = false;
        for (const MemoryCell::Ptr &cell: other->get_cells()) {
            if (cell == otherCell) {
                break;
            } else if (otherCell->address()->mustEqual(cell->address(), addrOps->solver())) {
                isOccluded = true;
            }
        }
        if (isOccluded) {
            SAWYER_MESG(debug) <<"    occluded by earlier cell (skipping)\n";
            continue;
        }

        // Read the value, writers, and properties without disturbing the states
        SValue::Ptr address = otherCell->address();

        CellList::iterator otherCursor = other->get_cells().begin();
        CellList otherCells = other->scan(otherCursor /*in,out*/, address, 8, addrOps, valOps);
        SValue::Ptr otherValue = mergeCellValues(otherCells, valOps->undefined_(8), addrOps, valOps);
        AddressSet otherWriters = mergeCellWriters(otherCells);
        InputOutputPropertySet otherProps = mergeCellProperties(otherCells);
        SAWYER_MESG(debug) <<"    scan found " <<StringUtility::plural(otherCells.size(), "cells") <<"\n"
                           <<"    condensed scan value=" <<*otherValue <<"\n";

        CellList::iterator thisCursor = get_cells().begin();
        CellList thisCells = scan(thisCursor /*in,out*/, address, 8, addrOps, valOps);

        // Merge cell values
        if (thisCells.empty()) {
            SAWYER_MESG(debug) <<"    no matching values in destination\n"
                               <<"    writing source cell to destination state\n";
            writeMemory(address, otherValue, addrOps, valOps);
            latestWrittenCell_->setWriters(otherWriters);
            latestWrittenCell_->ioProperties() = otherProps;
            changed = true;
        } else {
            bool cellChanged = false;
            SValue::Ptr thisValue = mergeCellValues(thisCells, valOps->undefined_(8), addrOps, valOps);
            SValue::Ptr mergedValue = thisValue->createOptionalMerge(otherValue, merger(), valOps->solver()).orDefault();
            SAWYER_MESG(debug) <<"    " <<StringUtility::plural(thisCells.size(), "matching values") <<" in destination\n"
                               <<"    matching values condensed to " <<*thisValue <<"\n";
            if (mergedValue) {
                SAWYER_MESG(debug) <<"    merged source and destination value=" <<*mergedValue <<"\n";
                cellChanged = true;
            } else {
                SAWYER_MESG(debug) <<"    destination value unchanged\n";
            }

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
                latestWrittenCell_->setWriters(mergedWriters);
                latestWrittenCell_->ioProperties() = mergedProps;
                changed = true;
            }

            if (debug) {
                debug <<"    new destination state:\n";
                for (const MemoryCell::Ptr &cell: get_cells())
                    debug <<"      addr=" <<*cell->address() <<" value=" <<*cell->value() <<"\n";
            }
        }
    }
    return changed;
}

bool
MemoryCellList::mergeNoAliasing(const MemoryState::Ptr &other_, RiscOperators *addrOps, RiscOperators *valOps) {
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    debug.enable(debug.enabled() && merger() && merger()->memoryMergeDebugging());

    MemoryCellList::Ptr other = as<MemoryCellList>(other_);
    ASSERT_not_null(other);
    bool changed = false;

    if (debug) {
        debug <<"MemoryCellList::mergeNoAliasing:\n"
                    <<"  merging into:\n";
        for (const MemoryCell::Ptr &cell: get_cells())
            debug <<"    addr=" <<*cell->address() <<" value=" <<*cell->value() <<"\n";
        debug <<"  merging from:\n";
        for (const MemoryCell::Ptr &cell: other->get_cells())
            debug <<"    addr=" <<*cell->address() <<" value=" <<*cell->value() <<"\n";
    }
    
    for (const MemoryCell::Ptr &otherCell: boost::adaptors::reverse(other->get_cells())) {
        // Read the value, writers, and properties without disturbing the states
        SValue::Ptr otherAddress = otherCell->address();
        SValue::Ptr otherValue = otherCell->value();
        AddressSet otherWriters = otherCell->getWriters();
        InputOutputPropertySet otherProps = otherCell->ioProperties();
        SAWYER_MESG(debug) <<"  merging from cell addr=" <<*otherAddress <<" value=" <<*otherValue <<"\n";

        // Is there some later-in-time (earlier-in-list) cell that occludes this one? If so, then we don't need to process this
        // cell.
        bool isOccluded = false;
        for (const MemoryCell::Ptr &cell: other->get_cells()) {
            if (cell == otherCell) {
                break;
            } else if (otherAddress->mustEqual(cell->address(), addrOps->solver())) {
                isOccluded = true;
            }
        }
        if (isOccluded) {
            SAWYER_MESG(debug) <<"    occluded by earlier cell (skipping)\n";
            continue;
        }

        // If otherAddress is must_equal to something in the destination state, modify the destination state.
        SAWYER_MESG(debug) <<"    looking for must_equal match in destination state\n";
        bool foundExactMatchingAddress = false;
        for (const MemoryCell::Ptr &thisCell: get_cells()) {
            SValue::Ptr thisAddress = thisCell->address();
            SValue::Ptr thisValue = thisCell->value();
            AddressSet thisWriters = otherCell->getWriters();
            InputOutputPropertySet thisProps = thisCell->ioProperties();
            SAWYER_MESG(debug) <<"      destination cell addr=" <<*thisAddress <<" value=" <<*thisValue <<"\n";

            if (otherAddress->mustEqual(thisCell->address(), addrOps->solver())) {
                bool cellChanged = false;
                SValue::Ptr mergedValue = thisValue->createOptionalMerge(otherValue, merger(), valOps->solver()).orDefault();
                if (mergedValue)
                    cellChanged = true;
                AddressSet mergedWriters = otherWriters | thisWriters;
                if (mergedWriters != thisWriters)
                    cellChanged = true;
                InputOutputPropertySet mergedProps = otherProps | thisProps;
                if (mergedProps != thisProps)
                    cellChanged = true;

                if (cellChanged) {
                    if (mergedValue)
                        thisCell->value(mergedValue);
                    thisCell->setWriters(mergedWriters);
                    thisCell->ioProperties() = mergedProps;
                    changed = true;
                }

                if (debug) {
                    debug <<"      address is an exact match\n";
                    if (mergedValue) {
                        debug <<"      merged value=" <<*mergedValue <<"\n";
                    } else {
                        debug <<"      values are equal (no change)\n";
                    }
                    debug <<"      new destination state:\n";
                    for (const MemoryCell::Ptr &cell: get_cells())
                        debug <<"        addr=" <<*cell->address() <<" value=" <<*cell->value() <<"\n";
                }
                
                foundExactMatchingAddress = true;
                break;                                  // don't need to search for any more matches in destination state
            }
        }
        if (foundExactMatchingAddress)
            continue;                                   // process the next source cell

        // We didn't find an exact match of the source address in the destination state.
        SAWYER_MESG(debug) <<"    no exact match found\n"
                                 <<"    inserting source cell into destination state\n";
        writeMemory(otherAddress, otherValue->copy(), addrOps, valOps);
        latestWrittenCell_->setWriters(otherWriters);
        latestWrittenCell_->ioProperties() = otherProps;
        changed = true;
        if (debug) {
            debug <<"    new destination state:\n";
            for (const MemoryCell::Ptr &cell: get_cells())
            debug <<"      addr=" <<*cell->address() <<" value=" <<*cell->value() <<"\n";
        }
    }
    return changed;
}

SValue::Ptr
MemoryCellList::mergeCellValues(const CellList &cells, const SValue::Ptr &dflt, RiscOperators */*addrOps*/, RiscOperators *valOps) {
    SValue::Ptr retval;
    for (const MemoryCell::Ptr &cell: cells) {
        // Get the cell's value. If the cell value is not the same width as the desired return value then we've go more work to
        // do. This isn't implemented yet. [Robb P. Matzke 2015-08-17]
        SValue::Ptr cellValue = valOps->unsignedExtend(cell->value(), dflt->nBits());

        if (!retval) {
            retval = cellValue;
        } else {
            retval = retval->createMerged(cellValue, merger(), valOps->solver());
        }
    }

    return retval ? retval : dflt;
}

AddressSet
MemoryCellList::mergeCellWriters(const CellList &cells) {
    AddressSet writers;
    for (const MemoryCell::Ptr &cell: cells)
        writers |= cell->getWriters();
    return writers;
}

InputOutputPropertySet
MemoryCellList::mergeCellProperties(const CellList &cells) {
    InputOutputPropertySet props;
    for (const MemoryCell::Ptr &cell: cells)
        props |= cell->ioProperties();
    return props;
}

MemoryCell::Ptr
MemoryCellList::insertReadCell(const SValue::Ptr &addr, const SValue::Ptr &value) {
    MemoryCell::Ptr cell = protocell->create(addr, value);
    cell->ioProperties().insert(IO_READ);
    cell->ioProperties().insert(IO_READ_BEFORE_WRITE);
    cell->ioProperties().insert(IO_READ_UNINITIALIZED);
    cells.push_front(cell);
    return cell;
}

MemoryCell::Ptr
MemoryCellList::insertReadCell(const SValue::Ptr &addr, const SValue::Ptr &value,
                               const AddressSet &writers, const InputOutputPropertySet &props) {
    MemoryCell::Ptr cell = protocell->create(addr, value);
    cell->setWriters(writers);
    cell->ioProperties() = props;
    cells.push_front(cell);
    return cell;
}

AddressSet
MemoryCellList::getWritersUnion(const SValue::Ptr &addr, size_t nBits, RiscOperators *addrOps, RiscOperators *valOps) {
    AddressSet retval;
    CellList::iterator cursor = get_cells().begin();
    for (const MemoryCell::Ptr &cell: scan(cursor, addr, nBits, addrOps, valOps))
        retval |= cell->getWriters();
    return retval;
}

AddressSet
MemoryCellList::getWritersIntersection(const SValue::Ptr &addr, size_t nBits, RiscOperators *addrOps, RiscOperators *valOps) {
    AddressSet retval;
    CellList::iterator cursor = get_cells().begin();
    size_t nCells = 0;
    for (const MemoryCell::Ptr &cell: scan(cursor, addr, nBits, addrOps, valOps)) {
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

void
MemoryCellList::hash(Combinatorics::Hasher &hasher, RiscOperators*/*addrOps*/, RiscOperators*/*valOps*/) const {
    // This could be improved substantially. For instance, the list
    //   {(0,x), (0,y)}
    // could be given the same hash as
    //   {(0,x)},
    //   {(0,x), (0,z)},
    //   {(0,x), (0,y), (0,z)},
    //   etc.
    // because a more recent concrete address (zero in this case) occludes the older cells with the same address. But in
    // order to do this properly, we have to do a better comparison than just checking concrete addresses for equality.
    for (const MemoryCell::Ptr &cell: cells)
        cell->hash(hasher);
}

void
MemoryCellList::print(std::ostream &stream, Formatter &fmt) const
{
    for (CellList::const_iterator ci=cells.begin(); ci!=cells.end(); ++ci)
        stream <<fmt.get_line_prefix() <<(**ci+fmt) <<"\n";
}

std::vector<MemoryCell::Ptr>
MemoryCellList::matchingCells(MemoryCell::Predicate &p) const {
    std::vector<MemoryCell::Ptr> retval;
    for (const MemoryCell::Ptr &cell: cells) {
        if (p(cell))
            retval.push_back(cell);
    }
    return retval;
}

std::vector<MemoryCell::Ptr>
MemoryCellList::leadingCells(MemoryCell::Predicate &p) const {
    std::vector<MemoryCell::Ptr> retval;
    for (const MemoryCell::Ptr &cell: cells) {
        if (!p(cell))
            break;
        retval.push_back(cell);
    }
    return retval;
}

void
MemoryCellList::eraseMatchingCells(MemoryCell::Predicate &p) {
    CellList::iterator ci = cells.begin();
    while (ci != cells.end()) {
        if (p(*ci)) {
            ci = cells.erase(ci);
        } else {
            ++ci;
        }
    }
}

void
MemoryCellList::eraseLeadingCells(MemoryCell::Predicate &p) {
    CellList::iterator ci = cells.begin();
    while (ci != cells.end()) {
        if (p(*ci)) {
            ci = cells.erase(ci);
        } else {
            return;
        }
    }
}

void
MemoryCellList::traverse(MemoryCell::Visitor &v) {
    for (MemoryCell::Ptr &cell: cells)
        v(cell);
}

} // namespace
} // namespace
} // namespace
} // namespace

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics::MemoryCellList);
#endif

#endif
