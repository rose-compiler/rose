#include <sage3basic.h>
#include <AsmUnparser_compat.h>
#include <BinaryCodeInserter.h>
#include <BinaryUnparserBase.h>
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
        mlog.comment("inserting code into an existing specimen");
        Diagnostics::mfacilities.insertAndAdjust(mlog);
    }
}

void
CodeInserter::chunkAllocationAlignment(size_t n) {
    chunkAllocationAlignment_ = std::min((size_t)1, n);
}

CodeInserter::InstructionInfoMap
CodeInserter::computeInstructionInfoMap(const P2::BasicBlock::Ptr &bb, size_t startIdx, size_t nDeleted) {
    ASSERT_not_null(bb);
    ASSERT_require(startIdx < bb->nInstructions());
    ASSERT_require(startIdx + nDeleted <= bb->nInstructions());
    InstructionInfoMap retval;

    // Instructions before the insertion point
    for (size_t i = 0; i < startIdx; ++i) {
        int relativeIndex = (int)i - (int)startIdx;
        retval.insert(relativeIndex, InstructionInfo(bb->instructions()[i]));
    }

    // Instructions after the deletion (or insertion point)
    for (size_t i = startIdx + nDeleted; i < bb->nInstructions(); ++i) {
        int relativeIndex = (int)i - (int)(startIdx + nDeleted);
        retval.insert(relativeIndex, InstructionInfo(bb->instructions()[i]));
    }

    return retval;
}

bool
CodeInserter::replaceBlockInsns(const P2::BasicBlock::Ptr &bb, size_t index, size_t nInsns,
                                std::vector<uint8_t> replacement, const std::vector<Relocation> &relocations) {
    ASSERT_not_null(bb);
    ASSERT_require(index + nInsns <= bb->nInstructions());
    if (0 == nInsns && replacement.empty())
        return true;

    const size_t insertAt = index;
    const size_t nDeletes = nInsns;
    Sawyer::Message::Stream debug(mlog[DEBUG]);

    // Where are the instructions located that are to be removed?  Be careful: the instructions of a basic block need not be in
    // address order, and need not be contiguous in memory.
    InstructionInfoMap insnInfoMap = computeInstructionInfoMap(bb, insertAt, nDeletes);
    std::vector<SgAsmInstruction*> toReplace(bb->instructions().begin() + index, bb->instructions().begin() + index + nInsns);
    AddressIntervalSet toReplaceVas = instructionLocations(toReplace);

    if (debug) {
        debug <<"replaceBlockInsns:\n";
        debug <<"  basic block with insertion point and deletions:\n";
        if (Unparser::Base::Ptr unparser = partitioner_.unparser()) {
            unparser->settings().function.cg.showing = false;
            unparser->settings().insn.stackDelta.showing = false;
            for (size_t i=0; i<bb->nInstructions(); ++i) {
                debug <<"    " <<(i==index ? "--> " : "    ") <<(i>=index && i<index+nInsns ? " X " : "   ");
                unparser->unparse(debug, partitioner_, bb->instructions()[i]);
                debug <<"\n";
            }
        }
        debug <<"  replacing " <<StringUtility::plural(nInsns, "instructions") <<" with [";
        BOOST_FOREACH (uint8_t byte, replacement)
            Diagnostics::mfprintf(debug)(" 0x%02x", byte);
        debug <<" ], " <<StringUtility::plural(replacement.size(), "bytes") <<"\n";
    }

    // Try to insert the replacement, enlarging the replacement set with subsequent then previous instructions.
    size_t relocStart = 0;
    while (true) {
        if (!toReplace.empty()) {
            // Find largest contiguous region of to-be-replaced instructions starting at to-be-replaced entry instruction.
            rose_addr_t toReplaceVa = toReplace[0]->get_address();
            AddressIntervalSet::ConstIntervalIterator found = toReplaceVas.findFirstOverlap(toReplaceVa);
            ASSERT_require(found != toReplaceVas.intervals().end());
            AddressInterval entryInterval = *found & AddressInterval::hull(toReplaceVa, AddressInterval::whole().greatest());

            // Try to replace
            SAWYER_MESG(debug) <<"  trying replacement using overwrite and/or transfer methodologies...\n";
            if (replaceByOverwrite(toReplaceVas, entryInterval, replacement, relocations, relocStart, insnInfoMap) ||
                replaceByTransfer(toReplaceVas, entryInterval, toReplace, replacement, relocations, relocStart, insnInfoMap)) {
                SAWYER_MESG(debug) <<"  replaceBlockInsns succeeded\n";
                return true;
            }
        }

        if ((aggregationDirection_ & AGGREGATE_SUCCESSORS) != 0 && index + nInsns < bb->nInstructions()) {
            // Enlarge to-be-replaced by appending one of the block's instructions to the replacement
            size_t insnIdx = index + nInsns++;
            int relIdx = (int)insnIdx - (int)(insertAt + nDeletes); // relative to one past last deleted instruction
            SgAsmInstruction *nextInsn = bb->instructions()[insnIdx];

            insnInfoMap[relIdx].newVaOffset = replacement.size();

            toReplace.push_back(nextInsn);
            toReplaceVas |= AddressInterval::baseSize(nextInsn->get_address(), nextInsn->get_size());
            std::vector<uint8_t> bytes = nextInsn->get_raw_bytes();
            replacement.insert(replacement.end(), bytes.begin(), bytes.end());

            SAWYER_MESG(debug) <<"  enlarged area by redirecting a successor:\n"
                               <<"    " <<nextInsn->toString() <<"\n";

        } else if ((aggregationDirection_ & AGGREGATE_PREDECESSORS) != 0 && index > 0) {
            // Enlarge to-be-replaced by prepending one of the block's instructions to the replacement
            size_t insnIdx = --index;
            int relIdx = (int)insnIdx - (int)insertAt;  // relative to insertion point
            SgAsmInstruction *prevInsn = bb->instructions()[insnIdx];

            BOOST_FOREACH (InstructionInfo &info, insnInfoMap.values()) {
                if (info.newVaOffset)
                    info.newVaOffset = info.newVaOffset.get() + prevInsn->get_size();
            }
            insnInfoMap[relIdx].newVaOffset = 0;

            toReplace.insert(toReplace.begin(), prevInsn);
            toReplaceVas |= AddressInterval::baseSize(prevInsn->get_address(), prevInsn->get_size());
            std::vector<uint8_t> bytes = prevInsn->get_raw_bytes();
            replacement.insert(replacement.begin(), bytes.begin(), bytes.end());
            relocStart += bytes.size();

            SAWYER_MESG(debug) <<"  enlarged area by redirecting a predecessor:\n"
                               <<"    " <<prevInsn->toString() <<"\n";
        } else {
            break;
        }
    }
    SAWYER_MESG(debug) <<"  replaceBlockInsns was unable to insert new code\n";
    return false;
}

bool
CodeInserter::replaceInsns(const std::vector<SgAsmInstruction*> &toReplace, const std::vector<uint8_t> &replacement,
                           const std::vector<Relocation> &relocations) {
    ASSERT_forbid(toReplace.empty());
    Sawyer::Message::Stream debug(mlog[DEBUG]);

    // Find the largest contiguous region of to-be-replaced instructions starting with the first one. By "first one", I
    // mean the first one executed, which isn't necessarily the lowest address.
    AddressIntervalSet toReplaceVas = instructionLocations(toReplace);
    rose_addr_t toReplaceVa = toReplace[0]->get_address();
    AddressIntervalSet::ConstIntervalIterator found = toReplaceVas.findFirstOverlap(toReplaceVa);
    ASSERT_require(found != toReplaceVas.intervals().end());
    AddressInterval entryInterval = *found & AddressInterval::hull(toReplaceVa, AddressInterval::whole().greatest());

    if (debug) {
        debug <<"replaceInsns:\n";
        debug <<"  instructions to replace:\n";
        BOOST_FOREACH (SgAsmInstruction *insn, toReplace)
            debug <<"    " <<partitioner_.unparse(insn) <<"\n";
        debug <<"  replacement = [";
        BOOST_FOREACH (uint8_t byte, replacement)
            Diagnostics::mfprintf(debug)(" 0x%02x", byte);
        debug <<" ], " <<StringUtility::plural(replacement.size(), "bytes") <<"\n";
    }

    // Try to replace
    SAWYER_MESG(debug) <<"  trying to replace using overwrite and/or transfer methodologies...\n";
    return (replaceByOverwrite(toReplaceVas, entryInterval, replacement, relocations, 0, InstructionInfoMap()) ||
            replaceByTransfer(toReplaceVas, entryInterval, toReplace, replacement, relocations, 0, InstructionInfoMap()));
}

void
CodeInserter::fillWithNops(const AddressIntervalSet &where) {
    Sawyer::Message::Stream debug(mlog[DEBUG]);

    std::string isa = partitioner_.instructionProvider().disassembler()->name();
    BOOST_FOREACH (const AddressInterval &interval, where.intervals()) {
        SAWYER_MESG(debug) <<"filling " <<StringUtility::addrToString(interval) <<" with no-op instructions\n";

        // Create the vector containing the encoded instructions
        std::vector<uint8_t> nops;
        nops.reserve(interval.size());
        if ("i386" == isa || "amd64" == isa) {
            nops.resize(interval.size(), 0x90);

        } else if ("coldfire" == isa) {
            // Although coldfire instructions are normally a multiple of 2 bytes, there is one exception: a bad instruction can
            // be an odd number of bytes. Therefore we could be asked to fill an odd number of bytes with NOP instructions. If
            // that happens, we fill even addresses with the first half of the NOP and odd addresses with the second half, this
            // way preserving the usual m68k instruction alignment.
            for (size_t i = 0; i < where.size(); ++i)
                nops.push_back(i % 2 == 0 ? 0x4e : 0x71);

        } else {
            TODO("no-op insertion for " + isa + " is not implemented yet");
        }

        // Write the vector to memory
        if (partitioner_.memoryMap()->at(interval.least()).write(nops).size() != nops.size()) {
            mlog[ERROR] <<"short write of " <<interval.size() <<"-byte NOP sequence at "
                        <<StringUtility::addrToString(interval.least()) <<"\n";
        }
    }
}

void
CodeInserter::fillWithRandom(const AddressIntervalSet &where) {
    Sawyer::Message::Stream debug(mlog[DEBUG]);

    std::string isa = partitioner_.instructionProvider().disassembler()->name();
    BOOST_FOREACH (const AddressInterval &interval, where.intervals()) {
        SAWYER_MESG(debug) <<"filling " <<StringUtility::addrToString(interval) <<" with random data\n";
        std::vector<uint8_t> data;
        data.reserve(interval.size());
        for (size_t i=0; i<interval.size(); ++i)
            data.push_back(Sawyer::fastRandomIndex(256));
        if (partitioner_.memoryMap()->at(interval.least()).write(data).size() != data.size()) {
            mlog[ERROR] <<"short write of " <<interval.size() <<"-byte random sequence at "
                        <<StringUtility::addrToString(interval) <<"\n";
        }
    }
}

std::vector<uint8_t>
CodeInserter::encodeJump(rose_addr_t srcVa, rose_addr_t tgtVa) {
    Sawyer::Message::Stream debug(mlog[DEBUG]);
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

    } else if ("coldfire" == isa) {
        rose_addr_t delta = tgtVa - (srcVa + 2);
        retval.push_back(0x60);                         // bra.l
        retval.push_back(0xff);
        retval.push_back((delta >> 24) & 0xff);
        retval.push_back((delta >> 16) & 0xff);
        retval.push_back((delta >>  8) & 0xff);
        retval.push_back((delta >>  0) & 0xff);

    } else {
        TODO("jump encoding for " + isa + " is not implemented yet");
    }

    if (debug) {
        debug <<"  jump from " <<StringUtility::addrToString(srcVa) <<" to " <<StringUtility::addrToString(tgtVa)
              <<" is encoded as [";
        BOOST_FOREACH (uint8_t byte, retval)
            Diagnostics::mfprintf(debug)(" %02x", byte);
        debug <<" ]\n";
    }

    return retval;
}

std::vector<uint8_t>
CodeInserter::applyRelocations(rose_addr_t va, std::vector<uint8_t> replacement, const std::vector<Relocation> &relocations,
                               size_t relocStart, const InstructionInfoMap &insnInfoMap) {
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    if (debug) {
        debug <<"applying relocations at va " <<StringUtility::addrToString(va) <<"\n"
              <<"  relocStart = " <<relocStart <<"\n"
              <<"  reloc input =  [";
        BOOST_FOREACH (uint8_t byte, replacement)
            Diagnostics::mfprintf(debug)(" 0x%02x", byte);
        debug <<" ], " <<StringUtility::plural(replacement.size(), "bytes") <<"\n";
    }

    BOOST_FOREACH (const Relocation &reloc, relocations) {
        ASSERT_require(reloc.offset < replacement.size());
        rose_addr_t value = 0;
        SAWYER_MESG(debug) <<"  at offset " <<(relocStart + reloc.offset) <<": ";
        switch (reloc.type) {
            case RELOC_INDEX_ABS_LE32: {
                // Virtual address of one of the input bytes.
                SAWYER_MESG(debug) <<"index_abs_le32(" <<StringUtility::addrToString(reloc.value) <<")";

                value = va + relocStart + reloc.value;

                ASSERT_require(relocStart + reloc.offset + 4 <= replacement.size());
                replacement[relocStart + reloc.offset + 0] = (value >>  0) & 0xff;
                replacement[relocStart + reloc.offset + 1] = (value >>  8) & 0xff;
                replacement[relocStart + reloc.offset + 2] = (value >> 16) & 0xff;
                replacement[relocStart + reloc.offset + 3] = (value >> 24) & 0xff;
                break;
            }

            case RELOC_INDEX_ABS_BE32: {
                // Virtual address of one of the input bytes.
                SAWYER_MESG(debug) <<"index_abs_be32(" <<StringUtility::addrToString(reloc.value) <<")";

                value = va + relocStart + reloc.value;

                ASSERT_require(relocStart + reloc.offset + 4 <= replacement.size());
                replacement[relocStart + reloc.offset + 0] = (value >> 24) & 0xff;
                replacement[relocStart + reloc.offset + 1] = (value >> 16) & 0xff;
                replacement[relocStart + reloc.offset + 2] = (value >>  8) & 0xff;
                replacement[relocStart + reloc.offset + 3] = (value >>  0) & 0xff;
                break;
            }

            case RELOC_INDEX_ABS_LE32HI: {
                // Virtual address of one of the input bytes (but only the high 32 bits)
                SAWYER_MESG(debug) <<"index_abs_le32hi(" <<StringUtility::addrToString(reloc.value) <<")";

                value = (va + relocStart + reloc.value) >> 32;

                ASSERT_require(relocStart + reloc.offset + 4 <= replacement.size());
                replacement[relocStart + reloc.offset + 0] = (value >> 0) & 0xff;
                replacement[relocStart + reloc.offset + 1] = (value >> 8) & 0xff;
                replacement[relocStart + reloc.offset + 2] = (value >> 16) & 0xff;
                replacement[relocStart + reloc.offset + 3] = (value >> 24) & 0xff;
                break;
            }

            case RELOC_ADDR_REL_LE32: {
                // Offset from the input byte's virtual address to a specified virtual address, adjusted by adding the value
                // originally stored in the input.
                SAWYER_MESG(debug) <<"index_rel_le32(" <<StringUtility::addrToString(reloc.value);

                ASSERT_require(relocStart + reloc.offset + 4 <= replacement.size());
                rose_addr_t addend = replacement[relocStart + reloc.offset + 0];
                addend |= replacement[relocStart + reloc.offset + 1] << 8;
                addend |= replacement[relocStart + reloc.offset + 1] << 16;
                addend |= replacement[relocStart + reloc.offset + 1] << 24;
                addend = IntegerOps::signExtend2(addend, 32, 64);

                SAWYER_MESG(debug) <<", addend=" <<StringUtility::toHex2(addend, 32) <<")";

                value = reloc.value - (va + relocStart + reloc.offset) + addend;

                replacement[relocStart + reloc.offset + 0] = (value >>  0) & 0xff;
                replacement[relocStart + reloc.offset + 1] = (value >>  8) & 0xff;
                replacement[relocStart + reloc.offset + 2] = (value >> 16) & 0xff;
                replacement[relocStart + reloc.offset + 3] = (value >> 24) & 0xff;
                break;
            }

            case RELOC_ADDR_REL_BE32: {
                // Offset from the input byte's virtual address to a specified virtual address, adjusted by adding the value
                // originally stored in the input.
                SAWYER_MESG(debug) <<"index_rel_be32(" <<StringUtility::addrToString(reloc.value);

                ASSERT_require(relocStart + reloc.offset + 4 <= replacement.size());
                rose_addr_t addend = replacement[relocStart + reloc.offset + 0] << 24;
                addend |= replacement[relocStart + reloc.offset + 1] << 16;
                addend |= replacement[relocStart + reloc.offset + 1] <<  8;
                addend |= replacement[relocStart + reloc.offset + 1] <<  0;
                addend = IntegerOps::signExtend2(addend, 32, 64);

                SAWYER_MESG(debug) <<", addend=" <<StringUtility::toHex2(addend, 32) <<")";

                value = reloc.value - (va + relocStart + reloc.offset) + addend;

                replacement[relocStart + reloc.offset + 0] = (value >> 24) & 0xff;
                replacement[relocStart + reloc.offset + 1] = (value >> 16) & 0xff;
                replacement[relocStart + reloc.offset + 2] = (value >>  8) & 0xff;
                replacement[relocStart + reloc.offset + 3] = (value >>  0) & 0xff;
                break;
            }

            case RELOC_INSN_ABS_LE32: {
                // Virtual address of some possibly moved instruction in the original basic block. The instruction is
                // indicated by its index relative to the instructions that were removed by this insertion.
                int relIdx = (int)reloc.value;
                SAWYER_MESG(debug) <<"insn_abs_le32(" <<relIdx;
                ASSERT_require(insnInfoMap.exists(relIdx));

                const InstructionInfo &info = insnInfoMap[relIdx];
                rose_addr_t value = info.newVaOffset ? va + info.newVaOffset.get() : info.originalVa;

                ASSERT_require(relocStart + reloc.offset + 4 <= replacement.size());
                replacement[relocStart + reloc.offset + 0] = (value >>  0) & 0xff;
                replacement[relocStart + reloc.offset + 1] = (value >>  8) & 0xff;
                replacement[relocStart + reloc.offset + 2] = (value >> 16) & 0xff;
                replacement[relocStart + reloc.offset + 3] = (value >> 24) & 0xff;
                break;
            }

            case RELOC_INSN_REL_LE32: {
                // Offset from the input byte's virtual address to the virtual address of some possibly moved instruction of
                // the original basic block, adjusted by adding the value originally stored in the input. The instruction is
                // indicated by its index relative to the instructions that were removed by this insertion.
                int relIdx = (int)reloc.value;
                SAWYER_MESG(debug) <<"insn_rel_le32(" <<relIdx;
                ASSERT_require(insnInfoMap.exists(relIdx));

                ASSERT_require(relocStart + reloc.offset + 4 <= replacement.size());
                rose_addr_t addend = replacement[relocStart + reloc.offset + 0];
                addend |= replacement[relocStart + reloc.offset + 1] << 8;
                addend |= replacement[relocStart + reloc.offset + 1] << 16;
                addend |= replacement[relocStart + reloc.offset + 1] << 24;
                addend = IntegerOps::signExtend2(addend, 32, 64);
                SAWYER_MESG(debug) <<", addend=" <<StringUtility::toHex2(addend, 32) <<")";

                const InstructionInfo &info = insnInfoMap[relIdx];
                rose_addr_t targetVa = info.newVaOffset ? va + info.newVaOffset.get() : info.originalVa;
                SAWYER_MESG(debug) <<" [target=" <<StringUtility::addrToString(targetVa) <<"]";
                value = targetVa - (va + relocStart + reloc.offset) + addend;

                replacement[relocStart + reloc.offset + 0] = (value >>  0) & 0xff;
                replacement[relocStart + reloc.offset + 1] = (value >>  8) & 0xff;
                replacement[relocStart + reloc.offset + 2] = (value >> 16) & 0xff;
                replacement[relocStart + reloc.offset + 3] = (value >> 24) & 0xff;
                break;
            }

            case RELOC_INSN_REL_BE32: {
                // Offset from the input byte's virtual address to the virtual address of some possibly moved instruction of
                // the original basic block, adjusted by adding the value originally stored in the input. The instruction is
                // indicated by its index relative to the instructions that were removed by this insertion.
                int relIdx = (int)reloc.value;
                SAWYER_MESG(debug) <<"insn_rel_be32(" <<relIdx;
                ASSERT_require(insnInfoMap.exists(relIdx));

                ASSERT_require(relocStart + reloc.offset + 4 <= replacement.size());
                rose_addr_t addend = replacement[relocStart + reloc.offset + 0] << 24;
                addend |= replacement[relocStart + reloc.offset + 1] << 16;
                addend |= replacement[relocStart + reloc.offset + 1] << 8;
                addend |= replacement[relocStart + reloc.offset + 1] << 0;
                addend = IntegerOps::signExtend2(addend, 32, 64);
                SAWYER_MESG(debug) <<", addend=" <<StringUtility::toHex2(addend, 32) <<")";

                const InstructionInfo &info = insnInfoMap[relIdx];
                rose_addr_t targetVa = info.newVaOffset ? va + info.newVaOffset.get() : info.originalVa;
                SAWYER_MESG(debug) <<" [target=" <<StringUtility::addrToString(targetVa) <<"]";
                value = targetVa - (va + relocStart + reloc.offset) + addend;

                replacement[relocStart + reloc.offset + 0] = (value >> 24) & 0xff;
                replacement[relocStart + reloc.offset + 1] = (value >> 16) & 0xff;
                replacement[relocStart + reloc.offset + 2] = (value >>  8) & 0xff;
                replacement[relocStart + reloc.offset + 3] = (value >>  0) & 0xff;
                break;
            }
        }
        SAWYER_MESG(debug)
                           << " = " <<StringUtility::addrToString(value)
                           <<" written to " <<StringUtility::addrToString(va)
                           <<StringUtility::addrToString(va + relocStart + reloc.offset) <<"\n";
    }

    if (debug) {
        debug <<"  reloc output = [";
        BOOST_FOREACH (uint8_t byte, replacement)
            Diagnostics::mfprintf(debug)(" 0x%02x", byte);
        debug <<" ], " <<StringUtility::plural(replacement.size(), "bytes") <<"\n";
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
                                 const std::vector<uint8_t> &replacement, const std::vector<Relocation> &relocations,
                                 size_t relocStart, const InstructionInfoMap &insnInfoMap) {
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    if (debug) {
        debug <<"replaceByOverwrite:\n";
        debug <<"  toReplaceVas = " <<StringUtility::addrToString(toReplaceVas) <<", "
              <<StringUtility::plural(toReplaceVas.size(), "bytes") <<"\n";
        debug <<"  entryInterval = " <<StringUtility::addrToString(entryInterval)
              <<", " <<StringUtility::plural(entryInterval.size(), "bytes") <<"\n";
        debug <<"  replacement [";
        BOOST_FOREACH (uint8_t byte, replacement)
            Diagnostics::mfprintf(debug)(" 0x%02x", byte);
        debug <<" ], " <<StringUtility::plural(replacement.size(), "bytes") <<"\n";
    }

    // Figure out where the replacement should go
    if (replacement.size() > entryInterval.size()) {
        SAWYER_MESG(debug) <<"  replaceByOverwrite failed: replacement is larger than entryInterval\n";
        return false;
    }
    rose_addr_t replacementVa = entryInterval.least();
    if (replacement.size() < entryInterval.size()) {
        switch (nopPadding_) {
            case PAD_NOP_FRONT:
                replacementVa += entryInterval.size() - replacement.size();
                break;
            case PAD_NOP_BACK:
            case PAD_RANDOM_BACK:
                break;
        }
    }
    SAWYER_MESG(debug) <<"  trying to insert replacement at " <<StringUtility::addrToString(replacementVa) <<"\n";
    AddressInterval replacementVas = AddressInterval::baseSize(replacementVa, replacement.size());

    // Write the replacement to memory
    std::vector<uint8_t> patched = applyRelocations(replacementVa, replacement, relocations, relocStart, insnInfoMap);
    if (partitioner_.memoryMap()->at(replacementVas).write(patched).size() != patched.size()) {
        SAWYER_MESG(debug) <<"  replaceByOverwrite failed: short write to memory map at "
                           <<StringUtility::addrToString(replacementVas) <<"\n";
        return false;
    }

    // Fill everything else with no-ops, etc.
    switch (nopPadding_) {
        case PAD_NOP_FRONT:
        case PAD_NOP_BACK:
            fillWithNops(toReplaceVas - replacementVas);
            break;
        case PAD_RANDOM_BACK:
            fillWithRandom(toReplaceVas - replacementVas);
            break;
    }

    SAWYER_MESG(debug) <<"  replaceByOverwrite succeeded\n";
    return true;
}

bool
CodeInserter::replaceByTransfer(const AddressIntervalSet &toReplaceVas, const AddressInterval &entryInterval,
                                const std::vector<SgAsmInstruction*> &toReplace, const std::vector<uint8_t> &replacement,
                                const std::vector<Relocation> &relocations, size_t relocStart,
                                const InstructionInfoMap &insnInfoMap) {
    ASSERT_forbid(toReplace.empty());
    ASSERT_forbid(toReplaceVas.isEmpty());
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    if (debug) {
        debug <<"replaceByTransfer:\n";
        debug <<"  toReplaceVas = " <<StringUtility::addrToString(toReplaceVas)
              <<", " <<StringUtility::plural(toReplaceVas.size(), "bytes") <<"\n";
        debug <<"  entryInterval = " <<StringUtility::addrToString(entryInterval)
              <<", " <<StringUtility::plural(entryInterval.size(), "bytes") <<"\n";
        debug <<"  instructions to be moved:\n";
        BOOST_FOREACH (SgAsmInstruction *insn, toReplace)
            debug <<"    " <<partitioner_.unparse(insn) <<"\n";
        debug <<"  replacement = [";
        BOOST_FOREACH (uint8_t byte, replacement)
            Diagnostics::mfprintf(debug)(" 0x%02x", byte);
        debug <<" ], " <<StringUtility::plural(replacement.size(), "bytes") <<"\n";
    }

    // Allocate memory for the replacement, plus enough to add a branch back to the first instruction after those being
    // replaced.
    rose_addr_t toReplaceFallThroughVa = toReplace.back()->get_address() + toReplace.back()->get_size();
    AddressInterval replacementVas = allocateMemory(replacement.size(), toReplaceFallThroughVa, Commit::NO);
    if (replacementVas.isEmpty()) {
        SAWYER_MESG(debug) <<"  replaceByTransfer failed: cannot allocate space in memory map\n";
        return false;                                   // failure to allocate memory
    }
    SAWYER_MESG(debug) <<"  memory for moved instructions and returning branch is "
                       <<StringUtility::addrToString(replacementVas) <<"\n";

    // Insert the replacement
    std::vector<uint8_t> patched = applyRelocations(replacementVas.least(), replacement, relocations, relocStart, insnInfoMap);
    if (partitioner_.memoryMap()->at(replacementVas).write(patched).size() != patched.size()) {
        SAWYER_MESG(debug) <<"  replaceByTransfer failed: short write to memory map at "
                           <<StringUtility::addrToString(replacementVas) <<"\n";
        return false;                               // write failed
    }

    // Insert the jump back from the replacement
    rose_addr_t replacementFallThroughVa = replacementVas.least() + replacement.size();
    std::vector<uint8_t> jmpFrom = encodeJump(replacementFallThroughVa, toReplaceFallThroughVa);
    if (partitioner_.memoryMap()->at(replacementFallThroughVa).write(jmpFrom).size() != jmpFrom.size()) {
        SAWYER_MESG(debug) <<"  replaceByTransfer failed: short write to memory map for returning branch at "
                           <<StringUtility::addrToString(replacementFallThroughVa) <<"\n";
        return false;                               // write failed
    }

    // Insert the jump to the replacement code.  If we're nop-padding before the jump this becomes a bit complicated because
    // the size of the jump could change based on its address.  If that happens, give up.
    rose_addr_t jmpToSite = toReplace[0]->get_address();
    std::vector<uint8_t> jmpTo = encodeJump(jmpToSite, replacementVas.least());
    if (jmpTo.size() > entryInterval.size()) {
        SAWYER_MESG(debug) <<"  replaceByTransfer failed: not enough room for the branch to moved code at "
                           <<StringUtility::addrToString(jmpToSite) <<"\n";
        return false;                                   // not enough room for the branch-to-replacement instruction
    }
    if (jmpTo.size() < entryInterval.size() && PAD_NOP_FRONT == nopPadding_) {
        jmpToSite += entryInterval.size() - jmpTo.size();
        std::vector<uint8_t> newJmpTo = encodeJump(jmpToSite, replacementVas.least());
        if (newJmpTo.size() != jmpTo.size()) {
            SAWYER_MESG(debug) <<"  replaceByTransfer failed: not enough room for re-encoded branch to moved code at "
                               <<StringUtility::addrToString(jmpToSite) <<"\n";
            return false;                               // padded, re-encoded jump no longer fits
        }
        jmpTo = newJmpTo;
    }
    AddressInterval jmpToVas = AddressInterval::baseSize(jmpToSite, jmpTo.size());
    if (partitioner_.memoryMap()->at(jmpToVas).write(jmpTo).size() != jmpTo.size()) {
        SAWYER_MESG(debug) <<"  replaceByTransfer failed: short write to memory map for branch to moved code at "
                           <<StringUtility::addrToString(jmpToVas) <<"\n";
        return false;                               // write failed
    }

    // Nop-fill
    fillWithNops(toReplaceVas - jmpToVas);
    commitAllocation(replacementVas);
    SAWYER_MESG(debug) <<"  replaceByTransfer succeeded\n";
    return true;
}

} // namespace
} // namespace
