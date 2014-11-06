#include "sage3basic.h"
#include "AsmUnparser_compat.h"
#include "Diagnostics.h"

#include <Partitioner2/ModulesX86.h>
#include <Partitioner2/Partitioner.h>
#include <Partitioner2/Utility.h>

using namespace rose::Diagnostics;

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {
namespace ModulesX86 {

bool
MatchStandardPrologue::match(const Partitioner *partitioner, rose_addr_t anchor) {
    ASSERT_not_null(partitioner);
    const RegisterDescriptor bp(x86_regclass_gpr, x86_gpr_bp, 0,
                                partitioner->instructionProvider().instructionPointerRegister().get_nbits());
    const RegisterDescriptor sp(x86_regclass_gpr, x86_gpr_sp, 0, bp.get_nbits());

    // Look for PUSH EBP
    SgAsmX86Instruction *insn = NULL; 
    {
        rose_addr_t pushVa = anchor;
        if (partitioner->instructionExists(pushVa))
            return false;                               // already in the CFG/AUM
        insn = isSgAsmX86Instruction(partitioner->discoverInstruction(pushVa));
        if (!insn || insn->get_kind()!=x86_push)
            return false;
        const SgAsmExpressionPtrList &opands = insn->get_operandList()->get_operands();
        if (opands.size()!=1)
            return false;                               // crazy operands!
        SgAsmRegisterReferenceExpression *rre = isSgAsmRegisterReferenceExpression(opands[0]);
        if (!rre || rre->get_descriptor()!=bp)
            return false;
    }

    // Look for MOV RBP,RSP following the PUSH.
    {
        rose_addr_t moveVa = insn->get_address() + insn->get_size();
        if (partitioner->instructionExists(moveVa))
            return false;                               // already in the CFG/AUM
        insn = isSgAsmX86Instruction(partitioner->discoverInstruction(moveVa));
        if (!insn || insn->get_kind()!=x86_mov)
            return false;
        const SgAsmExpressionPtrList &opands = insn->get_operandList()->get_operands();
        if (opands.size()!=2)
            return false;                               // crazy operands!
        SgAsmRegisterReferenceExpression *rre = isSgAsmRegisterReferenceExpression(opands[0]);
        if (!rre || rre->get_descriptor()!=bp)
            return false;
        rre = isSgAsmRegisterReferenceExpression(opands[1]);
        if (!rre || rre->get_descriptor()!=sp)
            return false;
    }

    function_ = Function::instance(anchor, SgAsmFunction::FUNC_PATTERN);
    return true;
}

bool
MatchHotPatchPrologue::match(const Partitioner *partitioner, rose_addr_t anchor) {
    // Look for optional MOV EDI, EDI
    rose_addr_t moveVa = anchor;
    if (partitioner->instructionExists(moveVa))
        return false;                               // already in the CFG/AUM
    SgAsmX86Instruction *insn = isSgAsmX86Instruction(partitioner->discoverInstruction(moveVa));
    if (!insn || insn->get_kind()!=x86_mov)
        return false;
    const SgAsmExpressionPtrList &opands = insn->get_operandList()->get_operands();
    if (opands.size()!=2)
        return false;
    SgAsmDirectRegisterExpression *dst = isSgAsmDirectRegisterExpression(opands[0]);
    if (!dst ||
        dst->get_descriptor().get_major()!=x86_regclass_gpr ||
        dst->get_descriptor().get_minor()!=x86_gpr_di)
        return false;
    SgAsmDirectRegisterExpression *src = isSgAsmDirectRegisterExpression(opands[1]);
    if (!src || dst->get_descriptor()!=src->get_descriptor())
        return false;

    // Match a standard prologue immediately following the hot-patch
    if (!MatchStandardPrologue::match(partitioner, insn->get_address()+insn->get_size()))
        return false;

    function_ = Function::instance(anchor, SgAsmFunction::FUNC_PATTERN);
    return true;
}

// Example function pattern matcher: matches x86 "MOV EDI, EDI; PUSH ESI" as a function prologue.
bool
MatchAbbreviatedPrologue::match(const Partitioner *partitioner, rose_addr_t anchor) {
    SgAsmX86Instruction *insn = NULL;
    // Look for MOV EDI, EDI
    {
        static const RegisterDescriptor REG_EDI(x86_regclass_gpr, x86_gpr_di, 0, 32);
        rose_addr_t moveVa = anchor;
        if (partitioner->instructionExists(moveVa))
            return false;                               // already in the CFG/AUM
        insn = isSgAsmX86Instruction(partitioner->discoverInstruction(moveVa));
        if (!insn || insn->get_kind()!=x86_mov)
            return false;
        const SgAsmExpressionPtrList &opands = insn->get_operandList()->get_operands();
        if (opands.size()!=2)
            return false;
        SgAsmDirectRegisterExpression *dst = isSgAsmDirectRegisterExpression(opands[0]);
        if (!dst || dst->get_descriptor()!=REG_EDI)
            return false;
        SgAsmDirectRegisterExpression *src = isSgAsmDirectRegisterExpression(opands[1]);
        if (!src || dst->get_descriptor()!=src->get_descriptor())
            return false;
    }

    // Look for PUSH ESI
    {
        static const RegisterDescriptor REG_ESI(x86_regclass_gpr, x86_gpr_si, 0, 32);
        rose_addr_t pushVa = insn->get_address() + insn->get_size();
        insn = isSgAsmX86Instruction(partitioner->discoverInstruction(pushVa));
        if (partitioner->instructionExists(pushVa))
            return false;                               // already in the CFG/AUM
        if (!insn || insn->get_kind()!=x86_push)
            return false;
        const SgAsmExpressionPtrList &opands = insn->get_operandList()->get_operands();
        if (opands.size()!=1)
            return false;                               // crazy operands!
        SgAsmRegisterReferenceExpression *rre = isSgAsmRegisterReferenceExpression(opands[0]);
        if (!rre || rre->get_descriptor()!=REG_ESI)
            return false;
    }

    // Seems good!
    function_ = Function::instance(anchor, SgAsmFunction::FUNC_PATTERN);
    return true;
}

bool
MatchEnterPrologue::match(const Partitioner *partitioner, rose_addr_t anchor) {
    ASSERT_not_null(partitioner);
    if (partitioner->instructionExists(anchor))
        return false;                                   // already in the CFG/AUM
    SgAsmX86Instruction *insn = isSgAsmX86Instruction(partitioner->discoverInstruction(anchor));
    if (!insn || insn->get_kind()!=x86_enter)
        return false;
    const SgAsmExpressionPtrList &args = insn->get_operandList()->get_operands();
    if (2!=args.size())
        return false;
    SgAsmIntegerValueExpression *arg = isSgAsmIntegerValueExpression(args[1]);
    if (!arg || 0!=arg->get_absoluteValue())
        return false;

    function_ = Function::instance(anchor, SgAsmFunction::FUNC_PATTERN);
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
        std::vector<rose_addr_t> concreteSuccessors = args.partitioner->basicBlockConcreteSuccessors(args.bblock, &isComplete);
        if (1==concreteSuccessors.size() && isComplete) {
            args.bblock->isFunctionReturn() = false;
            return chain;
        }

        // Must be a function return
        args.bblock->isFunctionReturn() = true;
    }
    return chain;
};

std::vector<rose_addr_t>
scanCodeAddressTable(const Partitioner &partitioner, AddressInterval &tableLimits /*in,out*/,
                     const AddressInterval &targetLimits, size_t tableEntrySize) {
    ASSERT_require(tableEntrySize>0 && tableEntrySize<=sizeof(rose_addr_t));

    std::vector<rose_addr_t> successors;
    if (tableLimits.isEmpty() || targetLimits.isEmpty())
        return successors;

    const MemoryMap &map = partitioner.memoryMap();
    while (1) {
        // Read table entry to get target address
        uint8_t bytes[sizeof(rose_addr_t)];
        rose_addr_t tableEntryVa = tableLimits.least() + successors.size() * tableEntrySize;
        if (!tableLimits.isContaining(AddressInterval::baseSize(tableEntryVa, tableEntrySize)))
            break;                                      // table entry is outside of table boundary
        if (tableEntrySize != (map.at(tableEntryVa).limit(tableEntrySize)
                               .require(MemoryMap::READABLE).prohibit(MemoryMap::WRITABLE).read(bytes).size()))
            break;                                      // table entry must be readable but not writable
        rose_addr_t target = 0;
        for (size_t i=0; i<tableEntrySize; ++i)
            target |= bytes[i] << (8*i);

        // Check target validity
        if (!targetLimits.isContaining(target))
            break;                                      // target is outside allowed interval
        if (!map.at(target).require(MemoryMap::EXECUTABLE).exists())
            break;                                      // target address is not executable

        successors.push_back(target);
    }
    if (successors.empty()) {
        tableLimits = AddressInterval();
        return successors;
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
    rose_addr_t indexArrayStartVa = tableLimits.least() + successors.size()*tableEntrySize;
    rose_addr_t indexArrayCurrentVa = indexArrayStartVa;
    if (successors.size() <= 16 /*arbitrarily small tables*/) {
        while (indexArrayCurrentVa <= tableLimits.greatest()) {
            uint8_t byte;
            if (!map.at(indexArrayCurrentVa).limit(1).require(MemoryMap::READABLE).prohibit(MemoryMap::WRITABLE).read(&byte))
                break;
            if (byte >= successors.size())
                break;
            if (indexArrayCurrentVa == tableLimits.greatest())
                break;                                  // avoid overflow
            ++indexArrayCurrentVa;
        }
    }
    
    // Return values
    tableLimits = AddressInterval::hull(tableLimits.least(), indexArrayCurrentVa-1);
    return successors;
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
    ASSERT_not_null(args.partitioner);
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
    size_t wordSize = args.partitioner->instructionProvider().instructionPointerRegister().get_nbits() / 8;
    AddressInterval whole = AddressInterval::hull(0, IntegerOps::genMask<rose_addr_t>(8*wordSize));
    AddressInterval tableLimits = AddressInterval::hull(tableVa, whole.greatest());

    // Set some limits on allowable target addresses contained in the table, besides those restrictions that will be imposed
    // during the table-reading loop (like targets must be mapped with execute permission).
    AddressInterval targetLimits = AddressInterval::hull(args.bblock->fallthroughVa(), whole.greatest());
    
    // If there's a function that follows us then the switch targets are almost certainly not after the beginning of that
    // function.
    {
        Function::Ptr needle = Function::instance(args.bblock->fallthroughVa());
        std::vector<Function::Ptr> functions = args.partitioner->functions();
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
    std::vector<rose_addr_t> tableEntries = scanCodeAddressTable(*args.partitioner, tableLimits /*in,out*/,
                                                                 targetLimits, wordSize);
    if (tableEntries.empty())
        return chain;

    // Replace basic block's successors with the new ones we found.
    std::set<rose_addr_t> successors(tableEntries.begin(), tableEntries.end());
    args.bblock->successors().clear();
    BOOST_FOREACH (rose_addr_t va, successors)
        args.bblock->insertSuccessor(va, wordSize*8);

    // Create a data block for the offset table and attach it to the basic block
    DataBlock::Ptr addressTable = DataBlock::instance(tableLimits.least(), tableLimits.size());
    args.bblock->insertDataBlock(addressTable);

    // Debugging
    if (mlog[DEBUG]) {
        using namespace StringUtility;
        mlog[DEBUG] <<"ModulesX86::SwitchSuccessors: found \"switch\" statement\n";
        mlog[DEBUG] <<"  basic block: " <<addrToString(args.bblock->address()) <<"\n";
        mlog[DEBUG] <<"  instruction: " <<unparseInstructionWithAddress(args.bblock->instructions()[nInsns-1]) <<"\n";
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
