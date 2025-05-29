#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Partitioner2/JumpTable.h>

#include <Rose/As.h>
#include <Rose/Affirm.h>
#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/DataBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/BinaryAnalysis/Partitioner2/Utility.h>
#include <Rose/StringUtility/NumberToString.h>

#include <SgAsmInstruction.h>
#include <SgAsmIntegerType.h>
#include <SgAsmVectorType.h>

using namespace Sawyer::Message::Common;
using Rose::StringUtility::addrToString;

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// JumpTable::Entries
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JumpTable::Entries::Entries() {}

JumpTable::Entries::Ptr
JumpTable::Entries::instance() {
    return Ptr(new JumpTable::Entries);
}

size_t
JumpTable::Entries::size() const {
    return targets_.size();
}

const std::vector<Address>&
JumpTable::Entries::targets() const {
    return targets_;
}

Address
JumpTable::Entries::parseEntry(const JumpTable *table, const std::vector<uint8_t> &bytes) const {
    ASSERT_not_null(table);
    Address target = 0;
    ASSERT_require(bytes.size() <= sizeof target);
    switch (table->partitioner()->architecture()->byteOrder()) {
        case ByteOrder::EL:
            for (size_t i = 0; i < bytes.size(); ++i)
                target = target | ((Address)bytes[i] << (i*8));
            break;
        case ByteOrder::BE:
            for (const uint8_t byte: bytes)
                target = (target << 8) | (Address)byte;
            break;
        default:
            ASSERT_not_reachable("invalid byte order");
    }

    target = BitOps::signExtend<Address>(target, 8*bytes.size());
    target += table->perEntryOffset();

    switch (table->entryType()) {
        case EntryType::ABSOLUTE:
            break;
        case EntryType::TABLE_RELATIVE:
            target += table->location().least();
            break;
    }

    target &= BitOps::lowMask<Address>(8*bytes.size());
    return target;
}

bool
JumpTable::Entries::isValidTarget(const JumpTable *table, const Address target) const {
    ASSERT_not_null(table);
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    if (table->showingDebug())
        debug.enable();

    SAWYER_MESG(debug) <<"    entry target = " <<addrToString(target) <<"\n";
    if (!table->targetLimits().contains(target)) {
        SAWYER_MESG(debug) <<"      target is not within " <<addrToString(table->targetLimits()) <<"\n";
        return false;
    } else if (!table->partitioner()->memoryMap()->at(target).require(MemoryMap::EXECUTABLE).exists()) {
        SAWYER_MESG(debug) <<"      target address is not executable\n";
        return false;
    } else {
        SgAsmInstruction *targetInsn = table->partitioner()->instructionProvider()[target];
        if (!targetInsn || table->partitioner()->architecture()->isUnknown(targetInsn)) {
            SAWYER_MESG(debug) <<"      target does not point to a valid instruction\n";
            return false;
        } else {
            const std::vector<SgAsmInstruction*> overlappingInsns = table->partitioner()->instructionsOverlapping(target);
            for (SgAsmInstruction *insn: overlappingInsns) {
                if (insn->get_address() != target) {
                    SAWYER_MESG(debug) <<"      target overlaps existing instruction\n";
                    return false;
                }
            }
        }
    }
    return true;
}

bool
JumpTable::Entries::maybeAppend(const MemoryTable *table_, const Address entryAddr, const std::vector<uint8_t> &bytes) {
    const auto table = as<const JumpTable>(table_);
    ASSERT_not_null(table);

    const Address target = parseEntry(table, bytes);
    if (isValidTarget(table, target)) {
        ASSERT_always_require(table->location().least() + targets_.size() * bytes.size() == entryAddr);
        targets_.push_back(target);
        return true;
    } else {
        return false;
    }
}

bool
JumpTable::Entries::maybePrepend(const MemoryTable *table_, const Address entryAddr, const std::vector<uint8_t> &bytes) {
    const auto table = as<const JumpTable>(table_);
    ASSERT_not_null(table);

    const Address target = parseEntry(table, bytes);
    if (isValidTarget(table, target)) {
        ASSERT_always_require(table->location().least() == entryAddr);
        targets_.insert(targets_.begin(), target);
        return true;
    } else {
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// JumpTable
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JumpTable::JumpTable(const Partitioner::ConstPtr &partitioner, const AddressInterval &tableLimits, size_t bytesPerEntry,
                     const Address perEntryOffset, const EntryType entryType)
    : MemoryTable(tableLimits, bytesPerEntry, Entries::instance()), partitioner_(notnull(partitioner)),
      perEntryOffset_(perEntryOffset), entryType_(entryType), targetLimits_(AddressInterval::whole()) {}

JumpTable::Ptr
JumpTable::instance(const Partitioner::ConstPtr &partitioner, const AddressInterval &tableLimits, size_t bytesPerEntry,
                    const Address perEntryOffset, const EntryType entryType) {
    return Ptr(new JumpTable(partitioner, tableLimits, bytesPerEntry, perEntryOffset, entryType));
}

Partitioner::ConstPtr
JumpTable::partitioner() const {
    return partitioner_;
}

Address
JumpTable::perEntryOffset() const {
    return perEntryOffset_;
}

JumpTable::EntryType
JumpTable::entryType() const {
    return entryType_;
}

AddressInterval
JumpTable::targetLimits() const {
    return targetLimits_;
}

void
JumpTable::targetLimits(const AddressInterval &limits) {
    targetLimits_ = limits;
}

void
JumpTable::refineLocationLimits(const BasicBlock::Ptr &bb, const Address probableTableAddr) {
    ASSERT_not_null(bb);

    const size_t wordSizeBytes = partitioner()->architecture()->bytesPerWord();
    const AddressInterval whole = AddressInterval::hull(0, BitOps::lowMask<Address>(8 * wordSizeBytes));
    AddressInterval tableLimits = this->tableLimits();
    SgAsmInstruction *lastInsn = bb->instructions().back();
    if (probableTableAddr > lastInsn->get_address()) {
        // table is after the end of the basic block instruction
        tableLimits = AddressInterval::hull(std::min(lastInsn->get_address() + lastInsn->get_size(), probableTableAddr),
                                            whole.greatest());
    } else {
        // table is before the end of the basic block
        tableLimits = AddressInterval::hull(0, lastInsn->get_address());
    }
    this->tableLimits(tableLimits);
}

void
JumpTable::refineTargetLimits(const BasicBlock::Ptr &bb) {
    ASSERT_not_null(bb);

    std::vector<Function::Ptr> functions = partitioner_->functions();
    {
        // If there's a function that precedes this basic block (possibly the one that will eventually own this block) then the
        // switch statement targets are almost certainly not before the beginning of that function.
        Function::Ptr needle = Function::instance(bb->address());
        std::vector<Function::Ptr>::iterator prevFunctionIter =
            std::lower_bound(functions.begin(), functions.end(), needle, sortFunctionsByAddress);
        if (prevFunctionIter != functions.end()) {
            while (prevFunctionIter != functions.begin() && (*prevFunctionIter)->address() > needle->address())
                --prevFunctionIter;
            Function::Ptr prevFunction = *prevFunctionIter;
            if (prevFunction->address() <= needle->address()) {
                if (prevFunction->address() <= targetLimits_.greatest()) {
                    targetLimits_ = targetLimits_ & AddressInterval::hull(prevFunction->address(), targetLimits_.greatest());
                } else {
                    targetLimits_ = AddressInterval();
                }
            } else {
                // No prior function, so assume that the "cases" are at higher addresses than the "switch"
                if (bb->fallthroughVa() <= targetLimits_.greatest()) {
                    targetLimits_ = AddressInterval::hull(bb->fallthroughVa(), targetLimits_.greatest());
                } else {
                    targetLimits_ = AddressInterval();
                }
            }
        }
    }

    {
        // If there's a function that follows this basic block then the switch targets are almost certainly not after the
        // beginning of that function.
        Function::Ptr needle = Function::instance(bb->fallthroughVa());
        std::vector<Function::Ptr>::iterator nextFunctionIter =
            std::lower_bound(functions.begin(), functions.end(), needle, sortFunctionsByAddress);
        if (nextFunctionIter != functions.end()) {
            Function::Ptr nextFunction = *nextFunctionIter;
            targetLimits_ = targetLimits_ & AddressInterval::hull(0, nextFunction->address() - 1);
        }
    }
}

const std::vector<Address>&
JumpTable::targets() const {
    return notnull(as<Entries>(entries()))->targets();
}

std::set<Address>
JumpTable::uniqueTargets() const {
    const auto list = targets();
    return std::set<Address>(list.begin(), list.end());
}

void
JumpTable::replaceBasicBlockSuccessors(const Partitioner::ConstPtr &partitioner, const BasicBlock::Ptr &bb) const {
    ASSERT_not_null(partitioner);
    ASSERT_not_null(bb);
    const size_t bitsPerWord = partitioner->architecture()->bitsPerWord();

    const std::set<Address> successors(targets().begin(), targets().end());
    bb->successors().clear();
    for (const Address successor: successors)
        bb->insertSuccessor(successor, bitsPerWord);
}

DataBlock::Ptr
JumpTable::createDataBlock() const {
    if (location()) {
        SgAsmType *entryType = SageBuilderAsm::buildTypeU(8 * bytesPerEntry());
        SgAsmType *type = SageBuilderAsm::buildTypeVector(entries()->size(), entryType);
        return DataBlock::instance(location().least(), type);
    } else {
        return {};
    }
}

void
JumpTable::attachTableToBasicBlock(const BasicBlock::Ptr &bb) const {
    ASSERT_not_null(bb);
    if (location()) {
        const Address addend = [this]() {
            const Address mask = BitOps::lowMask<Address>(partitioner_->architecture()->bitsPerWord());
            switch (entryType_) {
                case EntryType::ABSOLUTE:
                    return perEntryOffset_ & mask;
                case EntryType::TABLE_RELATIVE:
                    return (perEntryOffset_ + location().least()) & mask;
            }
            ASSERT_not_reachable("invalid entry type");
        }();

        DataBlock::Ptr data = createDataBlock();
        data->comment("jump table: addend = " + addrToString(addend));
        bb->insertDataBlock(data);
    }
}

} // namespace
} // namespace
} // namespace

#endif
