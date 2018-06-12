#include <sage3basic.h>
#include <BinaryCodeInserter.h>
#include <MemoryMap.h>
#include <StringUtility.h>

using namespace Sawyer::Message::Common;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

namespace Rose {
namespace BinaryAnalysis {

Sawyer::Message::Facility CodeInserter::mlog;

// class method
void
CodeInserter::initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        mlog = Sawyer::Message::Facility("Rose::BinaryAnalysis::CodeInserter", Diagnostics::destination);
        Diagnostics::mfacilities.insertAndAdjust(mlog);
    }
}

void
CodeInserter::chunkAllocationAlignment(size_t n) {
    chunkAllocationAlignment_ = std::min((size_t)1, n);
}

bool
CodeInserter::replaceBlockInsns(const P2::BasicBlock::Ptr &bb, size_t index, size_t nInsns,
                                std::vector<uint8_t> replacement, const std::vector<Relocation> &relocations) {
    ASSERT_not_null(bb);
    ASSERT_require(index + nInsns <= bb->nInstructions());

    // Where are the instructions located that are to be removed?  Be careful: the instructions of a basic block need not be in
    // address order, and need not be contiguous in memory.
    std::vector<SgAsmInstruction*> toReplace(bb->instructions().begin() + index, bb->instructions().begin() + index + nInsns);
    AddressIntervalSet toReplaceVas = instructionLocations(toReplace);

    // Try to insert the replacement, enlarging the replacement set with subsequent then previous instructions.
    while (true) {
        if (!toReplace.empty()) {
            // Find largest contiguous region of to-be-replaced instructions starting at to-be-replaced entry instruction.
            rose_addr_t toReplaceVa = toReplace[0]->get_address();
            AddressIntervalSet::ConstIntervalIterator found = toReplaceVas.findFirstOverlap(toReplaceVa);
            ASSERT_require(found != toReplaceVas.intervals().end());
            AddressInterval entryInterval = *found & AddressInterval::hull(toReplaceVa, AddressInterval::whole().greatest());

            // Try to replace
            if (replaceByOverwrite(toReplaceVas, entryInterval, replacement, relocations) ||
                replaceByTransfer(toReplaceVas, entryInterval, toReplace, replacement, relocations))
                return true;
        }

        if ((aggregationDirection_ & AGGREGATE_SUCCESSORS) != 0 && index + nInsns < bb->nInstructions()) {
            // Enlarge to-be-replaced by appending one of the block's instructions to the replacement
            SgAsmInstruction *nextInsn = bb->instructions()[index + nInsns++];
            toReplace.push_back(nextInsn);
            toReplaceVas |= AddressInterval::baseSize(nextInsn->get_address(), nextInsn->get_size());
            std::vector<uint8_t> bytes = nextInsn->get_raw_bytes();
            replacement.insert(replacement.end(), bytes.begin(), bytes.end());
        } else if ((aggregationDirection_ & AGGREGATE_PREDECESSORS) != 0 && index > 0) {
            // Enlarge to-be-replaced by prepending one of the block's instructions to the replacement
            SgAsmInstruction *prevInsn = bb->instructions()[--index];
            toReplace.insert(toReplace.begin(), prevInsn);
            toReplaceVas |= AddressInterval::baseSize(prevInsn->get_address(), prevInsn->get_size());
            std::vector<uint8_t> bytes = prevInsn->get_raw_bytes();
            replacement.insert(replacement.begin(), bytes.begin(), bytes.end());
        } else {
            break;
        }
    }
    return false;
}

bool
CodeInserter::replaceInsns(const std::vector<SgAsmInstruction*> &toReplace, const std::vector<uint8_t> &replacement,
                           const std::vector<Relocation> &relocations) {
    ASSERT_forbid(toReplace.empty());

    // Find the largest contiguous region of to-be-replaced instructions starting with the first one. By "first one", I
    // mean the first one executed, which isn't necessarily the lowest address.
    AddressIntervalSet toReplaceVas = instructionLocations(toReplace);
    rose_addr_t toReplaceVa = toReplace[0]->get_address();
    AddressIntervalSet::ConstIntervalIterator found = toReplaceVas.findFirstOverlap(toReplaceVa);
    ASSERT_require(found != toReplaceVas.intervals().end());
    AddressInterval entryInterval = *found & AddressInterval::hull(toReplaceVa, AddressInterval::whole().greatest());

    // Try to replace
    return (replaceByOverwrite(toReplaceVas, entryInterval, replacement, relocations) ||
            replaceByTransfer(toReplaceVas, entryInterval, toReplace, replacement, relocations));
}

void
CodeInserter::fillWithNops(const AddressIntervalSet &where) {
    std::string isa = partitioner_.instructionProvider().disassembler()->name();
    BOOST_FOREACH (const AddressInterval &interval, where.intervals()) {
        if ("i386" == isa || "amd64" == isa) {
            std::vector<uint8_t> nops(interval.size(), 0x90);
            if (partitioner_.memoryMap()->at(interval.least()).write(nops).size() != nops.size()) {
                mlog[ERROR] <<"short write of " <<interval.size() <<"-byte NOP sequence at "
                            <<StringUtility::addrToString(interval.least()) <<"\n";
            }
        } else {
            TODO("no-op insertion for " + isa + " is not implemented yet");
        }
    }
}

std::vector<uint8_t>
CodeInserter::encodeJump(rose_addr_t srcVa, rose_addr_t tgtVa) {
    std::vector<uint8_t> retval;
    std::string isa = partitioner_.instructionProvider().disassembler()->name();
    if ("i386" == isa || "amd64" == isa) {
        // For now, just use a jump with a 4-byte operand.
        rose_addr_t delta = tgtVa - (srcVa + 5);
        retval.push_back(0xe9);
        retval.push_back((delta >>  0) & 0xff);
        retval.push_back((delta >>  8) & 0xff);
        retval.push_back((delta >> 16) & 0xff);
        retval.push_back((delta >> 24) & 0xff);
    } else {
        TODO("jump encoding for " + isa + " is not implemented yet");
    }
    return retval;
}

std::vector<uint8_t>
CodeInserter::applyRelocations(rose_addr_t va, std::vector<uint8_t> replacement, const std::vector<Relocation> &relocations) {
    BOOST_FOREACH (const Relocation &reloc, relocations) {
        ASSERT_require(reloc.offset < replacement.size());
        switch (reloc.type) {
            case RELOC_REL_LE32: {
                ASSERT_require(reloc.offset + 4 <= replacement.size());
                rose_addr_t value = reloc.value - (va + reloc.offset);
                replacement[reloc.offset + 0] = (value >>  0) & 0xff;
                replacement[reloc.offset + 1] = (value >>  8) & 0xff;
                replacement[reloc.offset + 2] = (value >> 16) & 0xff;
                replacement[reloc.offset + 3] = (value >> 24) & 0xff;
                break;
            }

            case RELOC_IDXABS_LE32: {
                ASSERT_require(reloc.offset + 4 <= replacement.size());
                rose_addr_t value = va + reloc.value;
                replacement[reloc.offset + 0] = (value >>  0) & 0xff;
                replacement[reloc.offset + 1] = (value >>  8) & 0xff;
                replacement[reloc.offset + 2] = (value >> 16) & 0xff;
                replacement[reloc.offset + 3] = (value >> 24) & 0xff;
                break;
            }

            case RELOC_IDXABSHI_LE32: {
                ASSERT_require(reloc.offset + 4 <= replacement.size());
                rose_addr_t value = va + reloc.value;
                replacement[reloc.offset + 0] = (value >> 32) & 0xff;
                replacement[reloc.offset + 1] = (value >> 40) & 0xff;
                replacement[reloc.offset + 2] = (value >> 48) & 0xff;
                replacement[reloc.offset + 3] = (value >> 54) & 0xff;
                break;
            }
        }
    }
    return replacement;
}

AddressInterval
CodeInserter::allocateMemory(size_t nBytes, rose_addr_t jmpTargetVa, Commit::Boolean commit) {
    // Find the first interval that's large enough, not considering the jump.  Then add the jump (whose size might depend
    // on the address) and see if there's still enough free space. If not, continue searching through the free space list.
    AddressIntervalSet::ConstIntervalIterator found = freeSpace_.intervals().begin();
    while (found != freeSpace_.intervals().end()) {
        found = freeSpace_.firstFit(nBytes, found);
        if (found != freeSpace_.intervals().end()) {
            size_t totalBytes = nBytes + encodeJump(found->least() + nBytes, jmpTargetVa).size();
            if (totalBytes <= found->size()) {
                AddressInterval retval = AddressInterval::baseSize(found->least(), totalBytes);
                commitAllocation(retval, commit);
                return retval;
            } else {
                ++found;
            }
        }
    }

    // If we didn't find enough free space in segments that are already mapped, then map big chunk and use the first part
    // of it to satisfy the request.
    static const size_t maxJmpEncodedSize = 16; // size of largest unconditional jump on any architecture
    //AddressInterval chunkArea = AddressInterval::hull(0x80000000, 0xbfffffff); // restrinctions on chunk locations
    size_t chunkSize = std::max(nBytes + maxJmpEncodedSize, minChunkAllocationSize_);
    rose_addr_t chunkVa = 0;
    if (!partitioner_.memoryMap()->findFreeSpace(chunkSize, chunkAllocationAlignment_, chunkAllocationRegion_)
        .assignTo(chunkVa))
        return AddressInterval();               // no virtual address space available

    // Create the chunk and map it
    AddressInterval chunkVas = AddressInterval::baseSize(chunkVa, chunkSize);
    partitioner_.memoryMap()->insert(chunkVas,
                                     MemoryMap::Segment(MemoryMap::AllocatingBuffer::instance(chunkSize),
                                                        0, MemoryMap::READ_EXECUTE, chunkAllocationName_));
    freeSpace_ |= chunkVas;
    allocatedChunks_ |= chunkVas;

    // Use the first part of the new chunk
    size_t totalBytes = nBytes + encodeJump(chunkVa + nBytes, jmpTargetVa).size();
    ASSERT_require2(totalBytes <= chunkSize, "maxJmpEncodedSize is not large enough");
    AddressInterval retval = AddressInterval::baseSize(chunkVa, totalBytes);
    commitAllocation(retval, commit);
    return retval;
}

void
CodeInserter::commitAllocation(const AddressInterval &where, Commit::Boolean commit) {
    if (Commit::YES == commit)
        freeSpace_ -= where;
}

AddressIntervalSet
CodeInserter::instructionLocations(const std::vector<SgAsmInstruction*> &insns) {
    AddressIntervalSet retval;
    BOOST_FOREACH (SgAsmInstruction *insn, insns)
        retval |= AddressInterval::baseSize(insn->get_address(), insn->get_size());
    return retval;
}

bool
CodeInserter::replaceByOverwrite(const AddressIntervalSet &toReplaceVas, const AddressInterval &entryInterval,
                                 const std::vector<uint8_t> &replacement, const std::vector<Relocation> &relocations) {
    // Figure out where the replacement should go
    if (replacement.size() > entryInterval.size())
        return false;
    rose_addr_t replacementVa = entryInterval.least();
    if (replacement.size() < entryInterval.size() && PAD_NOP_FRONT == nopPadding_)
        replacementVa += entryInterval.size() - replacement.size();
    AddressInterval replacementVas = AddressInterval::baseSize(replacementVa, replacement.size());

    // Write the replacement to memory
    std::vector<uint8_t> patched = applyRelocations(replacementVa, replacement, relocations);
    if (partitioner_.memoryMap()->at(replacementVas).write(patched).size() != patched.size())
        return false;

    // Fill everything else with no-ops
    fillWithNops(toReplaceVas - replacementVas);
    return true;
}

bool
CodeInserter::replaceByTransfer(const AddressIntervalSet &toReplaceVas, const AddressInterval &entryInterval,
                                const std::vector<SgAsmInstruction*> &toReplace, const std::vector<uint8_t> &replacement,
                                const std::vector<Relocation> &relocations) {
    ASSERT_forbid(toReplace.empty());
    ASSERT_forbid(toReplaceVas.isEmpty());

    // Allocate memory for the replacement, plus enough to add a bracn back to the first instruction after those being
    // replaced.
    rose_addr_t toReplaceFallThroughVa = toReplace.back()->get_address() + toReplace.back()->get_size();
    AddressInterval replacementVas = allocateMemory(replacement.size(), toReplaceFallThroughVa, Commit::NO);
    if (replacementVas.isEmpty())
        return false;                                   // failure to allocate memory

    // Insert the replacement
    std::vector<uint8_t> patched = applyRelocations(replacementVas.least(), replacement, relocations);
    if (partitioner_.memoryMap()->at(replacementVas).write(patched).size() != patched.size())
        return false;                               // write failed

    // Insert the jump back from the replacement
    rose_addr_t replacementFallThroughVa = replacementVas.least() + replacement.size();
    std::vector<uint8_t> jmpFrom = encodeJump(replacementFallThroughVa, toReplaceFallThroughVa);
    if (partitioner_.memoryMap()->at(replacementFallThroughVa).write(jmpFrom).size() != jmpFrom.size())
        return false;                               // write failed

    // Insert the jump to the replacement code.  If we're nop-padding before the jump this becomes a bit complicated because
    // the size of the jump could change based on its address.  If that happens, give up.
    rose_addr_t jmpToSite = toReplace[0]->get_address();
    std::vector<uint8_t> jmpTo = encodeJump(jmpToSite, replacementVas.least());
    if (jmpTo.size() > entryInterval.size())
        return false;                                   // not enough room for the branch-to-replacement instruction
    if (jmpTo.size() < entryInterval.size() && PAD_NOP_FRONT == nopPadding_) {
        jmpToSite += entryInterval.size() - jmpTo.size();
        std::vector<uint8_t> newJmpTo = encodeJump(jmpToSite, replacementVas.least());
        if (newJmpTo.size() != jmpTo.size())
            return false;                               // padded, re-encoded jump no longer fits
        jmpTo = newJmpTo;
    }
    AddressInterval jmpToVas = AddressInterval::baseSize(jmpToSite, jmpTo.size());
    if (partitioner_.memoryMap()->at(jmpToVas).write(jmpTo).size() != jmpTo.size())
        return false;                               // write failed

    // Nop-fill
    fillWithNops(toReplaceVas - jmpToVas);
    commitAllocation(replacementVas);
    return true;
}

} // namespace
} // namespace
