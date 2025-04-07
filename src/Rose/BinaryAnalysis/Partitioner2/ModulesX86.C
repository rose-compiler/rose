#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Partitioner2/ModulesX86.h>

#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/DataBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/Function.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/BinaryAnalysis/Partitioner2/Utility.h>
#include <Rose/BitOps.h>
#include <Rose/Diagnostics.h>

#include <SgAsmBinaryAdd.h>
#include <SgAsmBinaryMultiply.h>
#include <SgAsmDirectRegisterExpression.h>
#include <SgAsmFunction.h>
#include <SgAsmIntegerType.h>
#include <SgAsmIntegerValueExpression.h>
#include <SgAsmMemoryReferenceExpression.h>
#include <SgAsmType.h>
#include <SgAsmVectorType.h>
#include <SgAsmX86Instruction.h>

#include <Cxx_GrammarDowncast.h>

#include <boost/format.hpp>
#include <stringify.h>

using namespace Rose::Diagnostics;

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {
namespace ModulesX86 {

MatchStandardPrologue::MatchStandardPrologue() {}

MatchStandardPrologue::~MatchStandardPrologue() {}

MatchStandardPrologue::Ptr
MatchStandardPrologue::instance() {
    return Ptr(new MatchStandardPrologue);
}

std::vector<Function::Ptr>
MatchStandardPrologue::functions() const {
    return std::vector<Function::Ptr>(1, function_);
}

bool
MatchStandardPrologue::match(const Partitioner::ConstPtr &partitioner, Address anchor) {
    ASSERT_not_null(partitioner);

    // Look for PUSH EBP
    SgAsmX86Instruction *insn = NULL;
    Address pushVa = anchor;
    if (partitioner->instructionExists(pushVa))
        return false;                                   // already in the CFG/AUM
    insn = isSgAsmX86Instruction(partitioner->discoverInstruction(pushVa));
    if (!matchPushBp(partitioner, insn))
        return false;

    // Look for MOV RBP,RSP following the PUSH.
    Address moveVa = insn->get_address() + insn->get_size();
    if (partitioner->instructionExists(moveVa))
        return false;                                   // already in the CFG/AUM
    insn = isSgAsmX86Instruction(partitioner->discoverInstruction(moveVa));
    if (!matchMovBpSp(partitioner, insn))
        return false;

    function_ = Function::instance(anchor, SgAsmFunction::FUNC_PATTERN);
    function_->reasonComment("matched PUSH <bp>; MOV <bp>, <sp>");
    return true;
}

bool
MatchHotPatchPrologue::match(const Partitioner::ConstPtr &partitioner, Address anchor) {
    ASSERT_not_null(partitioner);

    // Match MOV EDI, EDI
    Address moveVa = anchor;
    if (partitioner->instructionExists(moveVa))
        return false;                               // already in the CFG/AUM
    SgAsmX86Instruction *insn = isSgAsmX86Instruction(partitioner->discoverInstruction(moveVa));
    if (!matchMovDiDi(partitioner, insn))
        return false;

    // Match a standard prologue immediately following the hot-patch
    if (!MatchStandardPrologue::match(partitioner, insn->get_address()+insn->get_size()))
        return false;

    function_ = Function::instance(anchor, SgAsmFunction::FUNC_PATTERN);
    function_->reasonComment("matched MOV <di>, <di>; PUSH <bp>; MOV <bp>, <sp>");
    return true;
}

MatchAbbreviatedPrologue::MatchAbbreviatedPrologue() {}

MatchAbbreviatedPrologue::~MatchAbbreviatedPrologue() {}

MatchAbbreviatedPrologue::Ptr
MatchAbbreviatedPrologue::instance() {
    return Ptr(new MatchAbbreviatedPrologue);
}

std::vector<Function::Ptr>
MatchAbbreviatedPrologue::functions() const {
    return std::vector<Function::Ptr>(1, function_);
}

// Example function pattern matcher: matches x86 "MOV EDI, EDI; PUSH ESI" as a function prologue.
bool
MatchAbbreviatedPrologue::match(const Partitioner::ConstPtr &partitioner, Address anchor) {
    ASSERT_not_null(partitioner);

    // Look for MOV EDI, EDI
    Address movVa = anchor;
    if (partitioner->instructionExists(movVa))
        return false;                                   // already in the CFG/AUM
    SgAsmX86Instruction *insn = isSgAsmX86Instruction(partitioner->discoverInstruction(movVa));
    if (!matchMovDiDi(partitioner, insn))
        return false;

    // Look for PUSH ESI
    Address pushVa = insn->get_address() + insn->get_size();
    if (partitioner->instructionExists(pushVa))
        return false;                                   // already in the CFG/AUM
    insn = isSgAsmX86Instruction(partitioner->discoverInstruction(pushVa));
    if (!matchPushSi(partitioner, insn))
        return false;

    // Seems good!
    function_ = Function::instance(anchor, SgAsmFunction::FUNC_PATTERN);
    function_->reasonComment("matched MOV <di>, <di>; PUSH <si>");
    return true;
}

bool
MatchEnterPrologue::match(const Partitioner::ConstPtr &partitioner, Address anchor) {
    ASSERT_not_null(partitioner);
    if (partitioner->instructionExists(anchor))
        return false;                                   // already in the CFG/AUM
    SgAsmX86Instruction *insn = isSgAsmX86Instruction(partitioner->discoverInstruction(anchor));
    if (!matchEnterAnyZero(partitioner, insn))
        return false;
    function_ = Function::instance(anchor, SgAsmFunction::FUNC_PATTERN);
    function_->reasonComment("matched ENTER <x>, 0");
    return true;
}

bool
MatchRetPadPush::match(const Partitioner::ConstPtr &partitioner, Address anchor) {
    ASSERT_not_null(partitioner);

    // RET (prior to anchor) must already exist in the CFG/AUM
    // The RET instruction can be 1 or 3 bytes.
    SgAsmX86Instruction *ret = NULL;
    if (partitioner->instructionExists(anchor-1) &&
        (ret = isSgAsmX86Instruction(partitioner->discoverInstruction(anchor-1))) &&
        ret->get_kind() == x86_ret && ret->get_size()==1) {
        // found RET
    } else if (partitioner->instructionExists(anchor-3) &&
               (ret = isSgAsmX86Instruction(partitioner->discoverInstruction(anchor-3))) &&
               ret->get_kind() == x86_ret && ret->get_size()==3) {
        // found RET x
    } else {
        return false;
    }

    // Optional padding (NOP; or INT3; or MOV EDI,EDI)
    Address padVa = anchor;
    if (partitioner->instructionExists(padVa))
        return false;
    SgAsmX86Instruction *pad = isSgAsmX86Instruction(partitioner->discoverInstruction(padVa));
    if (!pad)
        return false;
    if (pad->get_kind() != x86_nop && pad->get_kind() != x86_int3 && !matchMovDiDi(partitioner, pad))
        pad = NULL;

    // PUSH x
    Address pushVa = padVa + (pad ? pad->get_size() : 0);
    if (partitioner->instructionExists(pushVa))
        return false;
    SgAsmX86Instruction *push = isSgAsmX86Instruction(partitioner->discoverInstruction(pushVa));
    if (!push || push->get_kind()!=x86_push)
        return false;

    // Looks good
    function_ = Function::instance(pushVa, SgAsmFunction::FUNC_PATTERN);
    function_->reasonComment("matched RET [x]; <padding>; PUSH <y>");
    return true;
}

bool
FunctionReturnDetector::operator()(bool chain, const Args &args) {
    if (chain) {
        if (args.bblock->isFunctionReturn().isCached()) // property is already computed?
            return chain;
        if (args.bblock->isEmpty()) {
            args.bblock->isFunctionReturn() = false;    // empty blocks are never considered returns
            return chain;
        }
        SgAsmX86Instruction *lastInsn = isSgAsmX86Instruction(args.bblock->instructions().back());
        if (NULL==lastInsn)
            return chain;                               // defer if not x86
        if (lastInsn->get_kind()!=x86_ret && lastInsn->get_kind()!=x86_retf)
            return chain;                               // defer if not a return instruction

        // A RET/RETF that has a single successor that is concrete probably isn't a real function return. Sometimes these
        // instructions are used to hide unconditional branches, like "PUSH label; RET".
        bool isComplete = false;
        std::vector<Address> concreteSuccessors = args.partitioner->basicBlockConcreteSuccessors(args.bblock, &isComplete);
        if (1==concreteSuccessors.size() && isComplete) {
            args.bblock->isFunctionReturn() = false;
            return chain;
        }

        // Must be a function return
        args.bblock->isFunctionReturn() = true;
    }
    return chain;
};

bool
matchEnterAnyZero(const Partitioner::ConstPtr&, SgAsmX86Instruction *enter) {
#if 1 // FIXME[Robb Matzke 2015-12-17]
    // This matcher looks at only two bytes of input (0xc8, 0x??, 0x??, 0x00) and thus gets too many false positives. A better
    // approach ight be to look at the entire block starting at the ENTER instruction and measure how reasonable it looks
    // before deciding this is a function entry point.  For now I'll just disable this. The effect of disabling is that
    // functions that start with this instruction will not be detected by this mechanism, although they will still be detected
    // by other mechanisms (call targets, symbols, etc). [Robb Matzke 2015-12-17]
    return false;
#endif

    if (!enter || enter->get_kind()!=x86_enter)
        return false;

    const SgAsmExpressionPtrList &args = enter->get_operandList()->get_operands();
    if (2!=args.size())
        return false;

    SgAsmIntegerValueExpression *arg = isSgAsmIntegerValueExpression(args[1]);
    if (!arg || 0!=arg->get_absoluteValue())
        return false;

    return true;
}

Sawyer::Optional<Address>
matchJmpConst(const Partitioner::ConstPtr&, SgAsmX86Instruction *jmp) {
    if (!jmp || jmp->get_kind()!=x86_jmp)
        return Sawyer::Nothing();

    const SgAsmExpressionPtrList &jmpArgs = jmp->get_operandList()->get_operands();
    if (1!=jmpArgs.size())
        return Sawyer::Nothing();

    SgAsmIntegerValueExpression *target = isSgAsmIntegerValueExpression(jmpArgs[0]);
    if (!target)
        return Sawyer::Nothing();

    return target->get_absoluteValue();
}

bool
matchJmpMem(const Partitioner::ConstPtr&, SgAsmX86Instruction *jmp) {
    if (!jmp || jmp->get_kind()!=x86_jmp || jmp->nOperands() != 1)
        return false;                                   // not a JMP instruction
    SgAsmMemoryReferenceExpression *mre = isSgAsmMemoryReferenceExpression(jmp->operand(0));
    if (!mre)
        return false;                                   // JMP is not through memory
    ASSERT_not_null2(mre->get_type(), "all binary expressions have a type");
    size_t nBytes = mre->get_type()->get_nBytes();
    if (nBytes != 4 && nBytes != 8)
        return false;                                   // wrong size for indirection
    SgAsmIntegerValueExpression *ive = isSgAsmIntegerValueExpression(mre->get_address());
    if (!ive)
        return false;                                   // JMP operand is not "[address]"
    return true;
}

bool
matchLeaCxMemBpConst(const Partitioner::ConstPtr &partitioner, SgAsmX86Instruction *lea) {
    ASSERT_not_null(partitioner);
    if (!lea || lea->get_kind()!=x86_lea)
        return false;

    const SgAsmExpressionPtrList &leaArgs = lea->get_operandList()->get_operands();
    if (2!=leaArgs.size())
        return false;

    const RegisterDescriptor CX(x86_regclass_gpr, x86_gpr_cx, 0,
                                partitioner->instructionProvider().instructionPointerRegister().nBits());
    SgAsmDirectRegisterExpression *cxReg = isSgAsmDirectRegisterExpression(leaArgs[0]);
    if (!cxReg || cxReg->get_descriptor()!=CX)
        return false;

    SgAsmMemoryReferenceExpression *mre = isSgAsmMemoryReferenceExpression(leaArgs[1]);
    if (!mre)
        return false;

    SgAsmBinaryAdd *sum = isSgAsmBinaryAdd(mre->get_address());
    if (!sum)
        return false;

    const RegisterDescriptor BP(x86_regclass_gpr, x86_gpr_bp, 0,
                                partitioner->instructionProvider().stackPointerRegister().nBits());
    SgAsmDirectRegisterExpression *bpReg = isSgAsmDirectRegisterExpression(sum->get_lhs());
    if (!bpReg || bpReg->get_descriptor()!=BP)
        return false;

    SgAsmIntegerValueExpression *offset = isSgAsmIntegerValueExpression(sum->get_rhs());
    if (!offset)
        return false;
    if (offset->get_signedValue() > 0)
        return false;

    return true;
}

bool
matchMovBpSp(const Partitioner::ConstPtr &partitioner, SgAsmX86Instruction *mov) {
    ASSERT_not_null(partitioner);
    if (!mov || mov->get_kind()!=x86_mov)
        return false;

    const SgAsmExpressionPtrList &opands = mov->get_operandList()->get_operands();
    if (opands.size()!=2)
        return false;                                   // crazy operands!

    const RegisterDescriptor SP = partitioner->instructionProvider().stackPointerRegister();
    const RegisterDescriptor BP(x86_regclass_gpr, x86_gpr_bp, 0, SP.nBits());
    SgAsmDirectRegisterExpression *rre = isSgAsmDirectRegisterExpression(opands[0]);
    if (!rre || rre->get_descriptor()!=BP)
        return false;

    rre = isSgAsmDirectRegisterExpression(opands[1]);
    if (!rre || rre->get_descriptor()!=SP)
        return false;

    return true;
}

bool
matchMovDiDi(const Partitioner::ConstPtr &partitioner, SgAsmX86Instruction *mov) {
    ASSERT_not_null(partitioner);
    if (!mov || mov->get_kind()!=x86_mov)
        return false;

    const SgAsmExpressionPtrList &opands = mov->get_operandList()->get_operands();
    if (opands.size()!=2)
        return false;

    const RegisterDescriptor DI(x86_regclass_gpr, x86_gpr_di, 0,
                                partitioner->instructionProvider().instructionPointerRegister().nBits());
    SgAsmDirectRegisterExpression *dst = isSgAsmDirectRegisterExpression(opands[0]);
    if (!dst || dst->get_descriptor()!=DI)
        return false;

    SgAsmDirectRegisterExpression *src = isSgAsmDirectRegisterExpression(opands[1]);
    if (!src || src->get_descriptor()!=DI)
        return false;

    return true;
}

bool
matchPushBp(const Partitioner::ConstPtr &partitioner, SgAsmX86Instruction *push) {
    ASSERT_not_null(partitioner);
    if (!push || push->get_kind()!=x86_push)
        return false;

    const SgAsmExpressionPtrList &opands = push->get_operandList()->get_operands();
    if (opands.size()!=1)
        return false;                                   // crazy operands!

    const RegisterDescriptor BP(x86_regclass_gpr, x86_gpr_bp, 0,
                                partitioner->instructionProvider().stackPointerRegister().nBits());
    SgAsmDirectRegisterExpression *rre = isSgAsmDirectRegisterExpression(opands[0]);
    if (!rre || rre->get_descriptor()!=BP)
        return false;

    return true;
}

bool
matchPushSi(const Partitioner::ConstPtr &partitioner, SgAsmX86Instruction *push) {
    ASSERT_not_null(partitioner);
    if (!push || push->get_kind()!=x86_push)
        return false;

    const SgAsmExpressionPtrList &opands = push->get_operandList()->get_operands();
    if (opands.size()!=1)
        return false;                                   // crazy operands!

    const RegisterDescriptor SI(x86_regclass_gpr, x86_gpr_si, 0,
                                partitioner->instructionProvider().instructionPointerRegister().nBits());
    SgAsmRegisterReferenceExpression *rre = isSgAsmRegisterReferenceExpression(opands[0]);
    if (!rre || rre->get_descriptor()!=SI)
        return false;

    return true;
}

// Is this table entry a valid target? If valid, returns nullptr, otherwise a reason for being invalid.
static const char*
isInvalidTarget(Address target, const AddressInterval &targetLimits, const Partitioner::ConstPtr &partitioner) {
    ASSERT_not_null(partitioner);
    if (!targetLimits.contains(target)) {
        return "target is not within target limits";
    } else if (!partitioner->memoryMap()->at(target).require(MemoryMap::EXECUTABLE).exists()) {
        return "target is not in executable memory";
    } else {
        SgAsmInstruction *targetInsn = partitioner->instructionProvider()[target];
        if (!targetInsn || partitioner->architecture()->isUnknown(targetInsn)) {
            return "target does not point to a valid instruction";
        } else {
            std::vector<SgAsmInstruction*> overlappingInsns = partitioner->instructionsOverlapping(target);
            for (SgAsmInstruction *insn: overlappingInsns) {
                if (insn->get_address() != target)
                    return "target overlaps existing instruction";
            }
        }
    }
    return nullptr;
}

std::vector<Address>
scanCodeAddressTable(const Partitioner::ConstPtr &partitioner, AddressInterval &tableLimits /*in,out*/,
                     const AddressInterval &targetLimits, const SwitchSuccessors::EntryType tableEntryType,
                     const size_t tableEntrySizeBytes, const Sawyer::Optional<Address> probableStartVa, const size_t nSkippable) {
    ASSERT_require(tableEntrySizeBytes > 0 && tableEntrySizeBytes <= sizeof(Address));
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    if (debug) {
        namespace Str = stringify::Rose::BinaryAnalysis::Partitioner2::ModulesX86::SwitchSuccessors;
        debug <<"scanCodeAddressTable: tableLimits = " <<StringUtility::addrToString(tableLimits)
              <<", targetLimits = " <<StringUtility::addrToString(targetLimits)
              <<", entry size = " <<StringUtility::plural(tableEntrySizeBytes, "bytes")
              <<", entry type " <<Str::EntryType(tableEntryType)
              <<(probableStartVa ? ", probable start = " + StringUtility::addrToString(*probableStartVa) : "")
              <<", nSkippable = " <<nSkippable <<"\n";
    }

    std::vector<Address> tableEntries;                  // decoded entries in the table

    if (tableLimits.isEmpty() || targetLimits.isEmpty())
        return tableEntries;
    const MemoryMap::Ptr map = partitioner->memoryMap();

    // Look forward from the probable start address, but possibly allow the table to start with some addresses that are out of
    // range (which are skipped and not officially part of the table).
    Address actualStartVa = probableStartVa.orElse(tableLimits.least()); // adjusted later if entries are skipped
    size_t nSkippedEntries = 0;
    while (1) {
        // Read table entry to get target address
        uint8_t bytes[sizeof(Address)];
        Address tableEntryVa = actualStartVa + tableEntries.size() * tableEntrySizeBytes;
        if (!tableLimits.contains(AddressInterval::baseSize(tableEntryVa, tableEntrySizeBytes))) {
            SAWYER_MESG(debug) <<"  entry at " <<StringUtility::addrToString(tableEntryVa) <<" falls outside table boundary\n";
            break;
        }
        if (tableEntrySizeBytes != (map->at(tableEntryVa).limit(tableEntrySizeBytes)
                                    .require(MemoryMap::READABLE).prohibit(MemoryMap::WRITABLE).read(bytes).size())) {
            SAWYER_MESG(debug) <<"  entry at " <<StringUtility::addrToString(tableEntryVa) <<" is not read-only\n";
            break;
        }
        Address target = 0;
        for (size_t i=0; i<tableEntrySizeBytes; ++i)
            target |= bytes[i] << (8*i);                // x86 is little endian
        switch (tableEntryType) {
            case SwitchSuccessors::ABSOLUTE:
                break;
            case SwitchSuccessors::RELATIVE:
                // target is the table entry plus the table address, careful for overflow
                SAWYER_MESG(debug) <<"  entry at " <<StringUtility::addrToString(tableEntryVa)
                                   <<" raw value = " <<StringUtility::addrToString(target) <<"\n";
                target = (BitOps::signExtend<Address>(target, 8*tableEntrySizeBytes) + actualStartVa) &
                         BitOps::lowMask<Address>(partitioner->instructionProvider().instructionPointerRegister().nBits());
                break;
        }

        // Save or skip the table entry
        const char *invalidReason = isInvalidTarget(target, targetLimits, partitioner);
        if (!invalidReason) {
            tableEntries.push_back(target);
        } else if (tableEntries.empty() && nSkippedEntries < nSkippable) {
            ++nSkippedEntries;
            actualStartVa += tableEntrySizeBytes;
            SAWYER_MESG(debug) <<"  entry at " <<StringUtility::addrToString(tableEntryVa) <<" is skipped"
                               <<": target is " <<StringUtility::addrToString(target)
                               <<" (" <<invalidReason <<")\n";
        } else {
            SAWYER_MESG(debug) <<"  entry at " <<StringUtility::addrToString(tableEntryVa) <<" is invalid"
                               <<": target is " <<StringUtility::addrToString(target)
                               <<" (" <<invalidReason <<")\n";
            break;
        }
    }

    // Look backward from the start address to see if there are valid table entries at negative indexes, even if we didn't find
    // any by looking forward from the probable start address. Be careful of over/under flows.
    size_t nBackwardEntries = 0;
    if (0 == nSkippedEntries) {
        while (actualStartVa >= tableEntrySizeBytes &&
               tableLimits.contains(AddressInterval::baseSize(actualStartVa-tableEntrySizeBytes, tableEntrySizeBytes))) {
            uint8_t bytes[sizeof(Address)];
            Address tableEntryVa = actualStartVa - tableEntrySizeBytes;
            if (tableEntrySizeBytes != (map->at(tableEntryVa).limit(tableEntrySizeBytes)
                                        .require(MemoryMap::READABLE).prohibit(MemoryMap::WRITABLE).read(bytes).size()))
                break;
            Address target = 0;
            for (size_t i=0; i<tableEntrySizeBytes; ++i)
                target |= bytes[i] << (8*i);            // x86 is little endian

            // Save entry if valid, otherwise we've reached the beginning of the table
            const char *invalidReason = isInvalidTarget(target, targetLimits, partitioner);
            if (invalidReason) {
                SAWYER_MESG(debug) <<"  entry at " <<StringUtility::addrToString(tableEntryVa) <<" is invalid"
                                   <<": target is " <<StringUtility::addrToString(target)
                                   <<" (" <<invalidReason <<")\n";
                break;
            } else {
                tableEntries.insert(tableEntries.begin(), target);
                actualStartVa -= tableEntrySizeBytes;
                ++nBackwardEntries;
                SAWYER_MESG(debug) <<"  entry at " <<StringUtility::addrToString(tableEntryVa) <<" found by backward search\n";
            }
        }
    }

    if (debug) {
        if (tableEntries.empty()) {
            debug <<"  no valid table entries found\n";
        } else {
            if (nSkippedEntries > 0)
                debug <<"  skipped " <<StringUtility::plural(nSkippedEntries, "leading table entries") <<"\n";
            if (tableEntries.size() > nBackwardEntries) {
                debug <<"  found " <<StringUtility::plural(tableEntries.size() - nBackwardEntries, "entries")
                      <<" by scanning forward\n";
            }
            if (nBackwardEntries > 0)
                debug <<"  found " <<StringUtility::plural(nBackwardEntries, "entries") <<" by scanning backward\n";
            debug <<"  total entries found: " <<tableEntries.size() <<"\n";
            int idx = -nBackwardEntries;
            for (Address target: tableEntries) {
                debug <<"    entry[" <<boost::format("%4d") % idx <<"]"
                      <<" at " <<StringUtility::addrToString(actualStartVa + idx * tableEntrySizeBytes)
                      <<" = " <<StringUtility::addrToString(target) <<"\n";
                ++idx;
            }
        }
    }

    // Sometimes the jump table is followed by 1-byte offsets into the jump table, and we should read those offsets as part of
    // the table.  For an example, look at tetris.exe compiled with MSVC 2010 (md5sum 30f1442a16d0275c2db4f52e9c78b5cd): eax is
    // the zero-origin value of the switch expression, which is looked up in the byte array at 0x00401670, which in turn is
    // used to index into the jump address array at 0x004165c.  Only do this for small tables, otherwise it will eat up all
    // kinds of stuff.
    //     0x0040150a: 3d a2 00 00 00          |=....   |   cmp    eax, 0x000000a2
    //     0x0040150f: 0f 87 86 00 00 00       |......  |   ja     0x0040159b
    //     0x00401515: 0f b6 90 70 16 40 00    |...p.@. |   movzx  edx, BYTE PTR ds:[eax + 0x00401670]
    //     0x0040151c: ff 24 95 5c 16 40 00    |.$.\.@. |   jmp    DWORD PTR ds:[0x0040165c + edx*0x04]
    //
    //     [0x0040165c,0x0040166f]: uint32_t addresses[5] = { <target addresses> };
    //
    //     [0x00401670,0x00401712]: uint8_t index[0xa3] = { <values 0..4> };
    //
    // Let's hope that the compiler doesn't combine the offset table technique with negative offsets.
    size_t nIndexes = 0;
    if (!tableEntries.empty() && 0 == nBackwardEntries) {
        Address indexArrayStartVa = actualStartVa + tableEntries.size() * tableEntrySizeBytes;
        Address indexArrayCurrentVa = indexArrayStartVa;
        if (tableEntries.size() <= 16 /*arbitrarily small tables*/) {
            while (indexArrayCurrentVa <= tableLimits.greatest()) {
                uint8_t byte;
                if (!map->at(indexArrayCurrentVa).limit(1).require(MemoryMap::READABLE).prohibit(MemoryMap::WRITABLE).read(&byte))
                    break;
                if (byte >= tableEntries.size())
                    break;
                if (indexArrayCurrentVa == tableLimits.greatest())
                    break;                                  // avoid overflow
                ++indexArrayCurrentVa;
                ++nIndexes;
            }
        }
        if (nIndexes > 0)
            SAWYER_MESG(debug) <<"  found " <<StringUtility::plural(nIndexes, "post table indexes") <<"\n";
    }

    // Return values
    AddressInterval actualTableLocation =
        AddressInterval::baseSize(actualStartVa, tableEntries.size() * tableEntrySizeBytes + nIndexes);
    ASSERT_require(tableLimits.contains(actualTableLocation));
    tableLimits = actualTableLocation;
    SAWYER_MESG(debug) <<"  actual table location = " <<StringUtility::addrToString(actualTableLocation) <<"\n";
    return tableEntries;
}

Sawyer::Optional<Address>
findTableBase(SgAsmExpression *expr) {
    ASSERT_not_null(expr);
    Address baseVa(-1);

    // Strip of optional memory reference
    if (SgAsmMemoryReferenceExpression *mre = isSgAsmMemoryReferenceExpression(expr))
        expr = mre->get_address();

    // We need to have a sum whose operands are a base address and some kind of register expression
    if (SgAsmBinaryAdd *sum = isSgAsmBinaryAdd(expr)) {
        // Find the integer base for the sum; "expr" will be the other operand
        SgAsmIntegerValueExpression *value = isSgAsmIntegerValueExpression(sum->get_lhs());
        if (value) {
            expr = sum->get_rhs();
        } else if ((value = isSgAsmIntegerValueExpression(sum->get_rhs()))) {
            expr = sum->get_lhs();
        } else {
            return Sawyer::Nothing();
        }
        baseVa = value->get_absoluteValue();

        // Look at the other addend (the one that should have a register)
        if (SgAsmBinaryMultiply *product = isSgAsmBinaryMultiply(expr)) {
            // Register multiplied by a constant?
            SgAsmDirectRegisterExpression *reg = isSgAsmDirectRegisterExpression(product->get_lhs());
            if (reg) {
                expr = product->get_rhs();
            } else if ((reg = isSgAsmDirectRegisterExpression(product->get_rhs()))) {
                expr = product->get_lhs();
            } else {
                return Sawyer::Nothing();               // no register
            }
            if (!isSgAsmIntegerValueExpression(expr))
                return Sawyer::Nothing();
        } else if (isSgAsmDirectRegisterExpression(expr)) {
            // Bare register
        } else {
            return Sawyer::Nothing();
        }
    } else {
        return Sawyer::Nothing();
    }

    return baseVa;
}

// Matches:
//   JMP [const1 + ...]  where const1 is the address of the table
bool
SwitchSuccessors::matchPattern1(SgAsmExpression *jmpArg) {
    ASSERT_not_null(jmpArg);
    if ((tableVa_ = findTableBase(jmpArg))) {
        entryType_ = ABSOLUTE;
        entrySizeBytes_ = jmpArg->get_type()->get_nBytes();
        return true;
    } else {
        return false;
    }
}

// Matches:
//   MOV reg1, [const1 + ...] where const1 is the address of the table
//   JMP reg1
bool
SwitchSuccessors::matchPattern2(const BasicBlock::Ptr &bb, SgAsmInstruction *jmp) {
    ASSERT_not_null(bb);
    ASSERT_not_null(jmp);
    size_t nInsns = bb->nInstructions();
    if (nInsns < 2)
        return false;

    // MOV reg, [base + ...]
    SgAsmX86Instruction *mov = isSgAsmX86Instruction(bb->instructions()[nInsns-2]);
    if (!mov || mov->get_kind() != x86_mov)
        return false;
    const SgAsmExpressionPtrList &movArgs = mov->get_operandList()->get_operands();
    if (movArgs.size() != 2)
        return false;

    // First arg of MOV must be the same register as the first arg for JMP
    const SgAsmExpressionPtrList &jmpArgs = jmp->get_operandList()->get_operands();
    SgAsmDirectRegisterExpression *reg1 = isSgAsmDirectRegisterExpression(jmpArgs[0]);
    SgAsmDirectRegisterExpression *reg2 = isSgAsmDirectRegisterExpression(movArgs[0]);
    if (!reg1 || !reg2 || reg1->get_descriptor()!=reg2->get_descriptor())
        return false;

    // Second argument of move must be [base + ...]
    if ((tableVa_ = findTableBase(movArgs[1]))) {
        entryType_ = ABSOLUTE;
        entrySizeBytes_ = movArgs[1]->get_type()->get_nBytes();
        return true;
    } else {
        return false;
    }
}

// Matches:
//   LEA reg1, [RIP+const1] where RIP+const1 is the address of the table
//   MOV EDI, EDI or other optional no-op
//   MOVSXD reg2, [reg1 + ...]
//   ADD reg2, reg1
//   JMP reg2
//
// This pattern is used for position independent AMD64 code. The table, starting at base, contains offsets from the start of
// the table to the target basic block. Representative specimen: wget2 email-20201209 df4ea624fc0d2a890c5cf6d0344ece52
bool
SwitchSuccessors::matchPattern3(const Partitioner::ConstPtr &partitioner, const BasicBlock::Ptr &bb, SgAsmInstruction *jmp) {
    ASSERT_not_null(bb);
    ASSERT_always_not_null(jmp);
    size_t nInsns = bb->nInstructions();
    if (nInsns < 4)
        return false;

    // ADD reg2, reg1
    SgAsmX86Instruction *add = isSgAsmX86Instruction(bb->instructions()[nInsns-2]);
    if (!add || add->get_kind() != x86_add)
        return false;
    const SgAsmExpressionPtrList &addArgs = add->get_operandList()->get_operands();
    if (addArgs.size() != 2)
        return false;
    SgAsmDirectRegisterExpression *addDst = isSgAsmDirectRegisterExpression(addArgs[0]);
    SgAsmDirectRegisterExpression *addSrc = isSgAsmDirectRegisterExpression(addArgs[1]);
    if (!addDst || !addSrc)
        return false;

    // MOVSXD reg2, [reg1 + ...]
    SgAsmX86Instruction *movsxd = isSgAsmX86Instruction(bb->instructions()[nInsns-3]);
    if (!movsxd || movsxd->get_kind() != x86_movsxd)
        return false;
    const SgAsmExpressionPtrList &movsxdArgs = movsxd->get_operandList()->get_operands();
    if (movsxdArgs.size() != 2)
        return false;
    SgAsmDirectRegisterExpression *movsxdDst = isSgAsmDirectRegisterExpression(movsxdArgs[0]);
    SgAsmMemoryReferenceExpression *movsxdSrc = isSgAsmMemoryReferenceExpression(movsxdArgs[1]);
    if (!movsxdDst || !movsxdSrc || movsxdDst->get_descriptor() != addDst->get_descriptor())
        return false;
    SgAsmBinaryAdd *movsxdAddr = isSgAsmBinaryAdd(movsxdSrc->get_address());
    if (!movsxdAddr)
        return false;
    SgAsmDirectRegisterExpression *movsxdSrcReg = isSgAsmDirectRegisterExpression(movsxdAddr->get_lhs());
    if (!movsxdSrcReg)
        movsxdSrcReg = isSgAsmDirectRegisterExpression(movsxdAddr->get_rhs());
    if (!movsxdSrcReg || movsxdSrcReg->get_descriptor() != addSrc->get_descriptor())
        return false;

    // MOV EDI EDI (optional)
    bool foundNop = false;
    do {
        SgAsmX86Instruction *mov = isSgAsmX86Instruction(bb->instructions()[nInsns-4]);
        if (!mov || mov->get_kind() != x86_mov)
            break;
        const SgAsmExpressionPtrList &movArgs = mov->get_operandList()->get_operands();
        if (movArgs.size() != 2)
            break;
        SgAsmDirectRegisterExpression *movDst = isSgAsmDirectRegisterExpression(movArgs[0]);
        SgAsmDirectRegisterExpression *movSrc = isSgAsmDirectRegisterExpression(movArgs[1]);
        if (!movDst || !movSrc)
            break;
        if (movDst->get_descriptor() != movSrc->get_descriptor())
            break;
        foundNop = true;
    } while (false);

    // LEA reg1 [RIP + const1]
    if (foundNop && nInsns < 5)
        return false;
    SgAsmX86Instruction *lea = isSgAsmX86Instruction(bb->instructions()[nInsns - (foundNop ? 5 : 4)]);
    if (!lea || lea->get_kind() != x86_lea)
        return false;
    const SgAsmExpressionPtrList &leaArgs = lea->get_operandList()->get_operands();
    if (leaArgs.size() != 2)
        return false;
    SgAsmDirectRegisterExpression *leaDst = isSgAsmDirectRegisterExpression(leaArgs[0]);
    if (!leaDst || leaDst->get_descriptor() != addSrc->get_descriptor())
        return false;
    SgAsmMemoryReferenceExpression *leaSrc = isSgAsmMemoryReferenceExpression(leaArgs[1]);
    if (!leaSrc)
        return false;
    SgAsmBinaryAdd *leaSrcAdd = isSgAsmBinaryAdd(leaSrc->get_address());
    SgAsmDirectRegisterExpression *leaSrcRip = isSgAsmDirectRegisterExpression(leaSrcAdd->get_lhs());
    SgAsmIntegerValueExpression *leaSrcOffset = NULL;
    if (leaSrcRip && leaSrcRip->get_descriptor() == partitioner->instructionProvider().instructionPointerRegister()) {
        leaSrcOffset = isSgAsmIntegerValueExpression(leaSrcAdd->get_rhs());
    } else {
        leaSrcRip = isSgAsmDirectRegisterExpression(leaSrcAdd->get_rhs());
        if (!leaSrcRip || leaSrcRip->get_descriptor() != partitioner->instructionProvider().instructionPointerRegister())
            return false;
        leaSrcOffset = isSgAsmIntegerValueExpression(leaSrcAdd->get_lhs());
    }
    if (!leaSrcOffset)
        return false;

    tableVa_ = (lea->get_address() + lea->get_size() + leaSrcOffset->get_absoluteValue()) &
               BitOps::lowMask<Address>(partitioner->instructionProvider().instructionPointerRegister().nBits());
    if (!tableVa_)
        return false;

    entryType_ = RELATIVE;
    entrySizeBytes_ = 4;
    return true;
}

// Match a basic block whose final instruction pointer has a symbolic value matching:
//     (add[u64] (signextend[u64] 64, v1[u32]) c1[u64])
// where
//     v1 is any symbolic variable
//     c1 is a constant that is inside a section of memory mapped read-only no-execute no-write (typically named ".rodata")
bool
SwitchSuccessors::matchPattern4(const Partitioner::ConstPtr &partitioner, const BasicBlock::Ptr &bblock) {
    ASSERT_not_null(partitioner);
    ASSERT_not_null(bblock);
    using namespace SymbolicExpression;

    const BasicBlockSemantics &semantics = bblock->semantics();
    const InstructionSemantics::BaseSemantics::State::Ptr state = semantics.finalState();
    if (!semantics.operators || !state)
        return false;                                   // only works when instruction semantics are enabled

    const RegisterDescriptor IP = partitioner->architecture()->registerDictionary()->instructionPointerRegister();
    const auto ip = semantics.operators->peekRegister(IP);
    const auto add = InstructionSemantics::SymbolicSemantics::SValue::promote(ip)->get_expression();

    if (!add || !add->isOperator(OP_ADD) || add->nBits() != 64 || !add->isIntegerExpr())
        return false;

    const auto sext = add->child(0);
    if (!sext || !sext->isOperator(OP_SEXTEND) || sext->nBits() != 64)
        return false;

    const auto sixtyfour = sext->child(0);
    if (!sixtyfour || sixtyfour->toUnsigned().orElse(0) != 64)
        return false;

    const auto v1 = sext->child(1);
    if (!v1 || !v1->isIntegerVariable() || v1->nBits() != 32)
        return false;

    const auto c1 = add->child(1);
    if (!c1 || !c1->isIntegerConstant() || c1->nBits() != 64)
        return false;
    Address tableAddr = *c1->toUnsigned();

    auto segments = partitioner->memoryMap()->at(tableAddr)
                    .require(MemoryMap::READABLE)
                    .prohibit(MemoryMap::WRITABLE | MemoryMap::EXECUTABLE)
                    .segments();
    if (segments.begin() == segments.end())
        return false;                                   // table addr is not mapped, or not read-only

    // Matched
    tableVa_ = tableAddr;
    entryType_ = RELATIVE;
    entrySizeBytes_ = 4;
    return true;
}

bool
SwitchSuccessors::matchPatterns(const Partitioner::ConstPtr &partitioner, const BasicBlock::Ptr &bblock) {
    ASSERT_not_null(bblock);

    // Block always ends with JMP with one argument.
    size_t nInsns = bblock->nInstructions();
    SgAsmX86Instruction *jmp = isSgAsmX86Instruction(bblock->instructions()[nInsns-1]);
    if (!jmp || jmp->get_kind() != x86_jmp)
        return false;
    const SgAsmExpressionPtrList &jmpArgs = jmp->get_operandList()->get_operands();
    if (jmpArgs.size() != 1)
        return false;

    // JMP [const1 + ...]   where base is assumed to be the table address
    if (matchPattern1(jmpArgs[0]))
        return true;

    // MOV reg1, ...
    // JMP reg1
    if (matchPattern2(bblock, jmp))
        return true;

    //   LEA reg1, [RIP+const1] where RIP+const1 is the address of the table
    //   MOV EDI, EDI or other optional no-op
    //   MOVSXD reg2, [reg1 + ...]
    //   ADD reg2, reg1
    //   JMP reg2
    if (matchPattern3(partitioner, bblock, jmp))
        return true;

    // Symbolic expression for instruction pointer:
    //   (add[u64] (signextend[u64] 0x40[u32] v1[u32]) c1[u64])
    if (matchPattern4(partitioner, bblock))
        return true;

    // no matching pattern
    return false;
}

// A "switch" statement is a computed jump consisting of a base address and a register offset.
bool
SwitchSuccessors::operator()(const bool chain, const Args &args) {
    ASSERT_not_null(args.bblock);
    if (!chain)
        return false;
    const size_t nInsns = args.bblock->nInstructions();
    if (nInsns < 1)
        return chain;
    if (!matchPatterns(args.partitioner, args.bblock))
        return chain;
    ASSERT_require(tableVa_);
    Sawyer::Message::Stream debug(mlog[DEBUG]);

    // Set some limits on the location of the target address table, besides those restrictions that will be imposed during the
    // table-reading loop (like table is mapped read-only).
    const size_t wordSizeBytes = args.partitioner->architecture()->bytesPerWord();
    const AddressInterval whole = AddressInterval::hull(0, BitOps::lowMask<Address>(8*wordSizeBytes));
    AddressInterval tableLimits;
    SgAsmInstruction *lastInsn = args.bblock->instructions().back();
    if (*tableVa_ > lastInsn->get_address()) {
        // table is after the jmp instruction
        tableLimits = AddressInterval::hull(std::min(lastInsn->get_address() + lastInsn->get_size(), *tableVa_), whole.greatest());
    } else {
        // table is before the jmp instruction
        tableLimits = AddressInterval::hull(0, lastInsn->get_address());
    }

    // Set some limits on allowable target addresses contained in the table, besides those restrictions that will be imposed
    // during the table-reading loop (like targets must be mapped with execute permission).
    AddressInterval targetLimits = AddressInterval::whole();
    std::vector<Function::Ptr> functions = args.partitioner->functions();
    {
        // If there's a function that precedes this basic block (possibly the one that will eventually own this block) then the
        // switch statement targets are almost certainly not before the beginning of that function.
        Function::Ptr needle = Function::instance(args.bblock->address());
        std::vector<Function::Ptr>::iterator prevFunctionIter = std::lower_bound(functions.begin(), functions.end(),
                                                                                 needle, sortFunctionsByAddress);
        if (prevFunctionIter != functions.end()) {
            while (prevFunctionIter != functions.begin() && (*prevFunctionIter)->address() > needle->address())
                --prevFunctionIter;
            Function::Ptr prevFunction = *prevFunctionIter;
            if (prevFunction->address() <= needle->address()) {
                targetLimits = targetLimits & AddressInterval::hull(prevFunction->address(), targetLimits.greatest());
            } else {
                // no prior function, so assume that the "cases" are at higher addresses than the "switch"
                targetLimits = AddressInterval::hull(args.bblock->fallthroughVa(), AddressInterval::whole().greatest());
            }
        }
    }
    {
        // If there's a function that follows this basic block then the switch targets are almost certainly not after the
        // beginning of that function.
        Function::Ptr needle = Function::instance(args.bblock->fallthroughVa());
        std::vector<Function::Ptr>::iterator nextFunctionIter = std::lower_bound(functions.begin(), functions.end(),
                                                                                 needle, sortFunctionsByAddress);
        if (nextFunctionIter != functions.end()) {
            Function::Ptr nextFunction = *nextFunctionIter;
            targetLimits = targetLimits & AddressInterval::hull(0, nextFunction->address()-1);
        }
    }
    SAWYER_MESG(debug) <<"switch table target limits: " <<StringUtility::addrToString(targetLimits) <<"\n";
    if (targetLimits.isEmpty())
        return chain;                                   // not even room for one case label

    // Read the table
    static const size_t maxSkippable = 1;               // max number of invalid table entries to skip; arbitrary
    std::vector<Address> tableEntries = scanCodeAddressTable(args.partitioner, tableLimits /*in,out*/,
                                                             targetLimits, entryType_, entrySizeBytes_,
                                                             *tableVa_, maxSkippable);
    if (tableEntries.empty())
        return chain;

    // Replace basic block's successors with the new ones we found.
    std::set<Address> successors(tableEntries.begin(), tableEntries.end());
    args.bblock->successors().clear();
    for (Address va: successors)
        args.bblock->insertSuccessor(va, wordSizeBytes*8);

    // Create a data block for the offset table and attach it to the basic block
    size_t nTableEntries = tableLimits.size() / entrySizeBytes_;
    SgAsmType *tableEntryType = SageBuilderAsm::buildTypeU(8*entrySizeBytes_);
    SgAsmType *tableType = SageBuilderAsm::buildTypeVector(nTableEntries, tableEntryType);
    DataBlock::Ptr addressTable = DataBlock::instance(tableLimits.least(), tableType);
    addressTable->comment("x86 'switch' statement's 'case' address table");
    args.bblock->insertDataBlock(addressTable);

    // Debugging
    if (debug) {
        using namespace StringUtility;
        debug <<"ModulesX86::SwitchSuccessors: found \"switch\" statement\n";
        debug <<"  basic block: " <<addrToString(args.bblock->address()) <<"\n";
        debug <<"  instruction: " <<args.bblock->instructions()[nInsns-1]->toString() <<"\n";
        debug <<"  table va:    " <<addrToString(tableLimits.least()) <<"\n";
        debug <<"  table size:  " <<plural(tableEntries.size(), "entries")
              <<", " <<plural(tableLimits.size(), "bytes") <<"\n";
        debug <<"  successors:  " <<plural(successors.size(), "distinct addresses") <<"\n";
        debug <<"   ";
        for (Address va: successors)
            debug <<" " <<addrToString(va);
        debug <<"\n";
    }

    return chain;
}

} // namespace
} // namespace
} // namespace
} // namespace

#endif
