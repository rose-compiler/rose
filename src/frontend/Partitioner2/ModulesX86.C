#include "sage3basic.h"
#include "AsmUnparser_compat.h"
#include "Diagnostics.h"

#include <boost/format.hpp>
#include <Partitioner2/ModulesX86.h>
#include <Partitioner2/Partitioner.h>
#include <Partitioner2/Utility.h>

using namespace Rose::Diagnostics;

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {
namespace ModulesX86 {

bool
MatchStandardPrologue::match(const Partitioner &partitioner, rose_addr_t anchor) {
    // Look for PUSH EBP
    SgAsmX86Instruction *insn = NULL;
    rose_addr_t pushVa = anchor;
    if (partitioner.instructionExists(pushVa))
        return false;                                   // already in the CFG/AUM
    insn = isSgAsmX86Instruction(partitioner.discoverInstruction(pushVa));
    if (!matchPushBp(partitioner, insn))
        return false;

    // Look for MOV RBP,RSP following the PUSH.
    rose_addr_t moveVa = insn->get_address() + insn->get_size();
    if (partitioner.instructionExists(moveVa))
        return false;                                   // already in the CFG/AUM
    insn = isSgAsmX86Instruction(partitioner.discoverInstruction(moveVa));
    if (!matchMovBpSp(partitioner, insn))
        return false;

    function_ = Function::instance(anchor, SgAsmFunction::FUNC_PATTERN);
    function_->reasonComment("matched PUSH <bp>; MOV <bp>, <sp>");
    return true;
}

bool
MatchHotPatchPrologue::match(const Partitioner &partitioner, rose_addr_t anchor) {
    // Match MOV EDI, EDI
    rose_addr_t moveVa = anchor;
    if (partitioner.instructionExists(moveVa))
        return false;                               // already in the CFG/AUM
    SgAsmX86Instruction *insn = isSgAsmX86Instruction(partitioner.discoverInstruction(moveVa));
    if (!matchMovDiDi(partitioner, insn))
        return false;

    // Match a standard prologue immediately following the hot-patch
    if (!MatchStandardPrologue::match(partitioner, insn->get_address()+insn->get_size()))
        return false;

    function_ = Function::instance(anchor, SgAsmFunction::FUNC_PATTERN);
    function_->reasonComment("matched MOV <di>, <di>; PUSH <bp>; MOV <bp>, <sp>");
    return true;
}

// Example function pattern matcher: matches x86 "MOV EDI, EDI; PUSH ESI" as a function prologue.
bool
MatchAbbreviatedPrologue::match(const Partitioner &partitioner, rose_addr_t anchor) {
    // Look for MOV EDI, EDI
    rose_addr_t movVa = anchor;
    if (partitioner.instructionExists(movVa))
        return false;                                   // already in the CFG/AUM
    SgAsmX86Instruction *insn = isSgAsmX86Instruction(partitioner.discoverInstruction(movVa));
    if (!matchMovDiDi(partitioner, insn))
        return false;

    // Look for PUSH ESI
    rose_addr_t pushVa = insn->get_address() + insn->get_size();
    if (partitioner.instructionExists(pushVa))
        return false;                                   // already in the CFG/AUM
    insn = isSgAsmX86Instruction(partitioner.discoverInstruction(pushVa));
    if (!matchPushSi(partitioner, insn))
        return false;

    // Seems good!
    function_ = Function::instance(anchor, SgAsmFunction::FUNC_PATTERN);
    function_->reasonComment("matched MOV <di>, <di>; PUSH <si>");
    return true;
}

bool
MatchEnterPrologue::match(const Partitioner &partitioner, rose_addr_t anchor) {
    if (partitioner.instructionExists(anchor))
        return false;                                   // already in the CFG/AUM
    SgAsmX86Instruction *insn = isSgAsmX86Instruction(partitioner.discoverInstruction(anchor));
    if (!matchEnterAnyZero(partitioner, insn))
        return false;
    function_ = Function::instance(anchor, SgAsmFunction::FUNC_PATTERN);
    function_->reasonComment("matched ENTER <x>, 0");
    return true;
}

bool
MatchRetPadPush::match(const Partitioner &partitioner, rose_addr_t anchor) {
    // RET (prior to anchor) must already exist in the CFG/AUM
    // The RET instruction can be 1 or 3 bytes.
    SgAsmX86Instruction *ret = NULL;
    if (partitioner.instructionExists(anchor-1) &&
        (ret = isSgAsmX86Instruction(partitioner.discoverInstruction(anchor-1))) &&
        ret->get_kind() == x86_ret && ret->get_size()==1) {
        // found RET
    } else if (partitioner.instructionExists(anchor-3) &&
               (ret = isSgAsmX86Instruction(partitioner.discoverInstruction(anchor-3))) &&
               ret->get_kind() == x86_ret && ret->get_size()==3) {
        // found RET x
    } else {
        return false;
    }

    // Optional padding (NOP; or INT3; or MOV EDI,EDI)
    rose_addr_t padVa = anchor;
    if (partitioner.instructionExists(padVa))
        return false;
    SgAsmX86Instruction *pad = isSgAsmX86Instruction(partitioner.discoverInstruction(padVa));
    if (!pad)
        return false;
    if (pad->get_kind() != x86_nop && pad->get_kind() != x86_int3 && !matchMovDiDi(partitioner, pad))
        pad = NULL;

    // PUSH x
    rose_addr_t pushVa = padVa + (pad ? pad->get_size() : 0);
    if (partitioner.instructionExists(pushVa))
        return false;
    SgAsmX86Instruction *push = isSgAsmX86Instruction(partitioner.discoverInstruction(pushVa));
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
        std::vector<rose_addr_t> concreteSuccessors = args.partitioner.basicBlockConcreteSuccessors(args.bblock, &isComplete);
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
matchEnterAnyZero(const Partitioner &partitioner, SgAsmX86Instruction *enter) {
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

Sawyer::Optional<rose_addr_t>
matchJmpConst(const Partitioner &partitioner, SgAsmX86Instruction *jmp) {
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
matchJmpMem(const Partitioner &partitioner, SgAsmX86Instruction *jmp) {
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
matchLeaCxMemBpConst(const Partitioner &partitioner, SgAsmX86Instruction *lea) {
    if (!lea || lea->get_kind()!=x86_lea)
        return false;

    const SgAsmExpressionPtrList &leaArgs = lea->get_operandList()->get_operands();
    if (2!=leaArgs.size())
        return false;

    const RegisterDescriptor CX(x86_regclass_gpr, x86_gpr_cx, 0,
                                partitioner.instructionProvider().instructionPointerRegister().nBits());
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
                                partitioner.instructionProvider().stackPointerRegister().nBits());
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
matchMovBpSp(const Partitioner &partitioner, SgAsmX86Instruction *mov) {
    if (!mov || mov->get_kind()!=x86_mov)
        return false;

    const SgAsmExpressionPtrList &opands = mov->get_operandList()->get_operands();
    if (opands.size()!=2)
        return false;                                   // crazy operands!

    const RegisterDescriptor SP = partitioner.instructionProvider().stackPointerRegister();
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
matchMovDiDi(const Partitioner &partitioner, SgAsmX86Instruction *mov) {
    if (!mov || mov->get_kind()!=x86_mov)
        return false;

    const SgAsmExpressionPtrList &opands = mov->get_operandList()->get_operands();
    if (opands.size()!=2)
        return false;

    const RegisterDescriptor DI(x86_regclass_gpr, x86_gpr_di, 0,
                                partitioner.instructionProvider().instructionPointerRegister().nBits());
    SgAsmDirectRegisterExpression *dst = isSgAsmDirectRegisterExpression(opands[0]);
    if (!dst || dst->get_descriptor()!=DI)
        return false;

    SgAsmDirectRegisterExpression *src = isSgAsmDirectRegisterExpression(opands[1]);
    if (!src || src->get_descriptor()!=DI)
        return false;

    return true;
}

bool
matchPushBp(const Partitioner &partitioner, SgAsmX86Instruction *push) {
    if (!push || push->get_kind()!=x86_push)
        return false;

    const SgAsmExpressionPtrList &opands = push->get_operandList()->get_operands();
    if (opands.size()!=1)
        return false;                                   // crazy operands!

    const RegisterDescriptor BP(x86_regclass_gpr, x86_gpr_bp, 0,
                                partitioner.instructionProvider().stackPointerRegister().nBits());
    SgAsmDirectRegisterExpression *rre = isSgAsmDirectRegisterExpression(opands[0]);
    if (!rre || rre->get_descriptor()!=BP)
        return false;

    return true;
}

bool
matchPushSi(const Partitioner &partitioner, SgAsmX86Instruction *push) {
    if (!push || push->get_kind()!=x86_push)
        return false;

    const SgAsmExpressionPtrList &opands = push->get_operandList()->get_operands();
    if (opands.size()!=1)
        return false;                                   // crazy operands!

    const RegisterDescriptor SI(x86_regclass_gpr, x86_gpr_si, 0,
                                partitioner.instructionProvider().instructionPointerRegister().nBits());
    SgAsmRegisterReferenceExpression *rre = isSgAsmRegisterReferenceExpression(opands[0]);
    if (!rre || rre->get_descriptor()!=SI)
        return false;

    return true;
}

std::vector<rose_addr_t>
scanCodeAddressTable(const Partitioner &partitioner, AddressInterval &tableLimits /*in,out*/,
                     const AddressInterval &targetLimits, size_t tableEntrySize,
                     Sawyer::Optional<rose_addr_t> probableStartVa, size_t nSkippable) {
    ASSERT_require(tableEntrySize > 0 && tableEntrySize <= sizeof(rose_addr_t));
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    SAWYER_MESG(debug) <<"scanCodeAddressTable: tableLimits = " <<StringUtility::addrToString(tableLimits)
                       <<", targetLimits = " <<StringUtility::addrToString(targetLimits)
                       <<", tableEntrySize = " <<StringUtility::plural(tableEntrySize, "bytes")
                       <<(probableStartVa ? ", probable start = " + StringUtility::addrToString(*probableStartVa) : "")
                       <<", nSkippable = " <<nSkippable <<"\n";

    std::vector<rose_addr_t> tableEntries;              // decoded entries in the table

    if (tableLimits.isEmpty() || targetLimits.isEmpty())
        return tableEntries;
    MemoryMap::Ptr map = partitioner.memoryMap();

    // Look forward from the probable start address, but possibly allow the table to start with some addresses that are out of
    // range (which are skipped and not officially part of the table).
    rose_addr_t actualStartVa = probableStartVa.orElse(tableLimits.least()); // adjusted later if entries are skipped
    size_t nSkippedEntries = 0;
    while (1) {
        // Read table entry to get target address
        uint8_t bytes[sizeof(rose_addr_t)];
        rose_addr_t tableEntryVa = actualStartVa + tableEntries.size() * tableEntrySize;
        if (!tableLimits.isContaining(AddressInterval::baseSize(tableEntryVa, tableEntrySize))) {
            SAWYER_MESG(debug) <<"  entry at " <<StringUtility::addrToString(tableEntryVa) <<" falls outside table boundary\n";
            break;
        }
        if (tableEntrySize != (map->at(tableEntryVa).limit(tableEntrySize)
                               .require(MemoryMap::READABLE).prohibit(MemoryMap::WRITABLE).read(bytes).size())) {
            SAWYER_MESG(debug) <<"  entry at " <<StringUtility::addrToString(tableEntryVa) <<" is not read-only\n";
            break;
        }
        rose_addr_t target = 0;
        for (size_t i=0; i<tableEntrySize; ++i)
            target |= bytes[i] << (8*i);                // x86 is little endian

        // Save or skip the table entry
        if (targetLimits.isContaining(target) && map->at(target).require(MemoryMap::EXECUTABLE).exists()) {
            tableEntries.push_back(target);
        } else if (tableEntries.empty() && nSkippedEntries < nSkippable) {
            ++nSkippedEntries;
            actualStartVa += tableEntrySize;
            SAWYER_MESG(debug) <<"  entry at " <<StringUtility::addrToString(tableEntryVa) <<" is skipped"
                               <<": value is " <<StringUtility::addrToString(target) <<"\n";
        } else {
            SAWYER_MESG(debug) <<"  entry at " <<StringUtility::addrToString(tableEntryVa) <<" is invalid"
                               <<": value is " <<StringUtility::addrToString(target) <<"\n";
            break;
        }
    }

    // Look backward from the start address to see if there are valid table entries at negative indexes, even if we didn't find
    // any by looking forward from the probable start address. Be careful of over/under flows.
    size_t nBackwardEntries = 0;
    if (0 == nSkippedEntries) {
        while (actualStartVa >= tableEntrySize &&
               tableLimits.isContaining(AddressInterval::baseSize(actualStartVa-tableEntrySize, tableEntrySize))) {
            uint8_t bytes[sizeof(rose_addr_t)];
            rose_addr_t tableEntryVa = actualStartVa - tableEntrySize;
            if (tableEntrySize != (map->at(tableEntryVa).limit(tableEntrySize)
                                   .require(MemoryMap::READABLE).prohibit(MemoryMap::WRITABLE).read(bytes).size()))
                break;
            rose_addr_t target = 0;
            for (size_t i=0; i<tableEntrySize; ++i)
                target |= bytes[i] << (8*i);            // x86 is little endian

            // Save entry if valid, otherwise we've reached the beginning of the table
            if (targetLimits.isContaining(target) && map->at(target).require(MemoryMap::EXECUTABLE).exists()) {
                tableEntries.insert(tableEntries.begin(), target);
                actualStartVa -= tableEntrySize;
                ++nBackwardEntries;
                SAWYER_MESG(debug) <<"  entry at " <<StringUtility::addrToString(tableEntryVa) <<" found by backward search\n";
            } else {
                break;
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
            BOOST_FOREACH (rose_addr_t target, tableEntries) {
                debug <<"    entry[" <<boost::format("%4d") % idx <<"]"
                      <<" at " <<StringUtility::addrToString(actualStartVa + idx * tableEntrySize)
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
        rose_addr_t indexArrayStartVa = actualStartVa + tableEntries.size() * tableEntrySize;
        rose_addr_t indexArrayCurrentVa = indexArrayStartVa;
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
    AddressInterval actualTableLocation = AddressInterval::baseSize(actualStartVa, tableEntries.size() * tableEntrySize + nIndexes);
    ASSERT_require(tableLimits.isContaining(actualTableLocation));
    tableLimits = actualTableLocation;
    SAWYER_MESG(debug) <<"  actual table location = " <<StringUtility::addrToString(actualTableLocation) <<"\n";
    return tableEntries;
}

Sawyer::Optional<rose_addr_t>
findTableBase(SgAsmExpression *expr) {
    ASSERT_not_null(expr);
    rose_addr_t baseVa(-1);

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

// A "switch" statement is a computed jump consisting of a base address and a register offset.
bool
SwitchSuccessors::operator()(bool chain, const Args &args) {
    ASSERT_not_null(args.bblock);
    static const rose_addr_t NO_ADDR(-1);
    if (!chain)
        return false;
    size_t nInsns = args.bblock->nInstructions();
    if (nInsns < 1)
        return chain;

    // Block always ends with JMP
    SgAsmX86Instruction *jmp = isSgAsmX86Instruction(args.bblock->instructions()[nInsns-1]);
    if (!jmp || jmp->get_kind()!=x86_jmp)
        return chain;
    const SgAsmExpressionPtrList &jmpArgs = jmp->get_operandList()->get_operands();
    if (jmpArgs.size()!=1)
        return chain;

    // Try to match a pattern
    rose_addr_t tableVa = NO_ADDR;
    do {
        // Pattern 1: JMP [offset + reg * size]
        if (findTableBase(jmpArgs[0]).assignTo(tableVa))
            break;

        // Other patterns are: MOV reg, ...; JMP reg
        if (nInsns < 2)
            return chain;
        SgAsmX86Instruction *mov = isSgAsmX86Instruction(args.bblock->instructions()[nInsns-2]);
        if (!mov || mov->get_kind()!=x86_mov)
            return chain;
        const SgAsmExpressionPtrList &movArgs = mov->get_operandList()->get_operands();
        if (movArgs.size()!=2)
            return chain;

        // First arg of MOV must be the same register as the first arg for JMP
        SgAsmDirectRegisterExpression *reg1 = isSgAsmDirectRegisterExpression(jmpArgs[0]);
        SgAsmDirectRegisterExpression *reg2 = isSgAsmDirectRegisterExpression(movArgs[0]);
        if (!reg1 || !reg2 || reg1->get_descriptor()!=reg2->get_descriptor())
            return chain;

        // Pattern 2: MOV reg2, [offset + reg1 * size]; JMP reg2
        if (findTableBase(movArgs[1]).assignTo(tableVa))
            break;

        // No match
        return chain;
    } while (0);
    ASSERT_forbid(tableVa == NO_ADDR);

    // Set some limits on the location of the target address table, besides those restrictions that will be imposed during the
    // table-reading loop (like table is mapped read-only).
    size_t wordSizeBytes = args.partitioner.instructionProvider().instructionPointerRegister().nBits() / 8;
    AddressInterval whole = AddressInterval::hull(0, IntegerOps::genMask<rose_addr_t>(8*wordSizeBytes));
    AddressInterval tableLimits;
    if (tableVa > jmp->get_address()) {
        // table is after the jmp instruction
        tableLimits = AddressInterval::hull(std::min(jmp->get_address() + jmp->get_size(), tableVa), whole.greatest());
    } else {
        // table is before the jmp instruction
        tableLimits = AddressInterval::hull(0, jmp->get_address());
    }

    // Set some limits on allowable target addresses contained in the table, besides those restrictions that will be imposed
    // during the table-reading loop (like targets must be mapped with execute permission).
    AddressInterval targetLimits = AddressInterval::hull(args.bblock->fallthroughVa(), whole.greatest());

    // If there's a function that follows us then the switch targets are almost certainly not after the beginning of that
    // function.
    {
        Function::Ptr needle = Function::instance(args.bblock->fallthroughVa());
        std::vector<Function::Ptr> functions = args.partitioner.functions();
        std::vector<Function::Ptr>::iterator nextFunctionIter = std::lower_bound(functions.begin(), functions.end(),
                                                                                 needle, sortFunctionsByAddress);
        if (nextFunctionIter != functions.end()) {
            Function::Ptr nextFunction = *nextFunctionIter;
            if (args.bblock->fallthroughVa() == nextFunction->address())
                return chain;                           // not even room for one case label
            targetLimits = AddressInterval::hull(targetLimits.least(), nextFunction->address()-1);
        }
    }

    // Read the table
    static const size_t maxSkippable = 1;               // max number of invalid table entries to skip; arbitrary
    std::vector<rose_addr_t> tableEntries = scanCodeAddressTable(args.partitioner, tableLimits /*in,out*/,
                                                                 targetLimits, wordSizeBytes, tableVa, maxSkippable);
    if (tableEntries.empty())
        return chain;

    // Replace basic block's successors with the new ones we found.
    std::set<rose_addr_t> successors(tableEntries.begin(), tableEntries.end());
    args.bblock->successors().clear();
    BOOST_FOREACH (rose_addr_t va, successors)
        args.bblock->insertSuccessor(va, wordSizeBytes*8);

    // Create a data block for the offset table and attach it to the basic block
    size_t nTableEntries = tableLimits.size() / wordSizeBytes;
    SgAsmType *tableEntryType = SageBuilderAsm::buildTypeU(8*wordSizeBytes);
    SgAsmType *tableType = SageBuilderAsm::buildTypeVector(nTableEntries, tableEntryType);
    DataBlock::Ptr addressTable = DataBlock::instance(tableLimits.least(), tableType);
    addressTable->comment("x86 'switch' statement's 'case' address table");
    args.bblock->insertDataBlock(addressTable);

    // Debugging
    if (mlog[DEBUG]) {
        using namespace StringUtility;
        mlog[DEBUG] <<"ModulesX86::SwitchSuccessors: found \"switch\" statement\n";
        mlog[DEBUG] <<"  basic block: " <<addrToString(args.bblock->address()) <<"\n";
        mlog[DEBUG] <<"  instruction: " <<args.bblock->instructions()[nInsns-1]->toString() <<"\n";
        mlog[DEBUG] <<"  table va:    " <<addrToString(tableLimits.least()) <<"\n";
        mlog[DEBUG] <<"  table size:  " <<plural(tableEntries.size(), "entries")
                    <<", " <<plural(tableLimits.size(), "bytes") <<"\n";
        mlog[DEBUG] <<"  successors:  " <<plural(successors.size(), "distinct addresses") <<"\n";
        mlog[DEBUG] <<"   ";
        BOOST_FOREACH (rose_addr_t va, successors)
            mlog[DEBUG] <<" " <<addrToString(va);
        mlog[DEBUG] <<"\n";
    }

    return chain;
}

} // namespace
} // namespace
} // namespace
} // namespace
