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
    return true;
}

    

size_t
isJmpMemThunk(const Partitioner &partitioner, const std::vector<SgAsmInstruction*> &insns) {
    if (insns.empty())
        return 0;
    SgAsmX86Instruction *jmp = isSgAsmX86Instruction(insns[0]);
    if (!matchJmpMem(partitioner, jmp))
        return 0;
    return 1;
}

size_t
isLeaJmpThunk(const Partitioner &partitioner, const std::vector<SgAsmInstruction*> &insns) {
    if (insns.size() < 2)
        return 0;
    
    // LEA ECX, [EBP + constant]
    SgAsmX86Instruction *lea = isSgAsmX86Instruction(insns[0]);
    if (!matchLeaCxMemBpConst(partitioner, lea))
        return 0;

    // JMP address
    SgAsmX86Instruction *jmp = isSgAsmX86Instruction(insns[1]);
    if (!matchJmpConst(partitioner, jmp))
        return 0;

    return 2;
}

size_t
isMovJmpThunk(const Partitioner &partitioner, const std::vector<SgAsmInstruction*> &insns) {
    if (insns.size() < 2)
        return 0;

    // MOV reg1 [address]
    SgAsmX86Instruction *mov = isSgAsmX86Instruction(insns[0]);
    if (!mov || mov->get_kind() != x86_mov)
        return 0;
    const SgAsmExpressionPtrList &movArgs = mov->get_operandList()->get_operands();
    if (movArgs.size() != 2)
        return 0;
    SgAsmDirectRegisterExpression *movArg0 = isSgAsmDirectRegisterExpression(movArgs[0]);
    SgAsmMemoryReferenceExpression *movArg1 = isSgAsmMemoryReferenceExpression(movArgs[1]);
    if (!movArg0 || !movArg1)
        return 0;
    
    // JMP reg1
    SgAsmX86Instruction *jmp = isSgAsmX86Instruction(insns[1]);
    if (!jmp || jmp->get_kind() != x86_jmp)
        return 0;
    const SgAsmExpressionPtrList &jmpArgs = jmp->get_operandList()->get_operands();
    if (jmpArgs.size() != 1)
        return 0;
    SgAsmDirectRegisterExpression *jmpArg0 = isSgAsmDirectRegisterExpression(jmpArgs[0]);
    if (!jmpArg0)
        return 0;
    if (jmpArg0->get_descriptor() != movArg0->get_descriptor())
        return 0;

    return 2;
}

size_t
isJmpImmThunk(const Partitioner &partitioner, const std::vector<SgAsmInstruction*> &insns) {
    if (insns.empty())
        return 0;
    SgAsmX86Instruction *jmp = isSgAsmX86Instruction(insns[0]);
    if (!jmp || jmp->get_kind() != x86_jmp)
        return 0;
    const SgAsmExpressionPtrList &jmpArgs = jmp->get_operandList()->get_operands();
    if (jmpArgs.size() != 1)
        return 0;
    SgAsmIntegerValueExpression *jmpArg0 = isSgAsmIntegerValueExpression(jmpArgs[0]);
    if (!jmpArg0)
        return 0;
    rose_addr_t targetVa = jmpArg0->get_absoluteValue();
    if (!partitioner.memoryMap().require(MemoryMap::EXECUTABLE).at(targetVa).exists())
        return 0;                                       // target must be an executable address
    if (!partitioner.instructionExists(targetVa) && !partitioner.instructionsOverlapping(targetVa).empty())
        return 0;                                       // points to middle of some instruction
    return 1;
}

size_t
isThunk(const Partitioner &partitioner, const std::vector<SgAsmInstruction*> &insns) {
    // Longer patterns must be before shorter patterns if they could both match
    if (size_t n = isLeaJmpThunk(partitioner, insns))
        return n;
    if (size_t n = isMovJmpThunk(partitioner, insns))
        return n;
    if (size_t n = isJmpMemThunk(partitioner, insns))
        return n;
#if 0 // [Robb P. Matzke 2015-06-23]: disabled for now, but see splitThunkFunctions
    // This matcher is causing too many false positives. The problem is that when the partitioner fails to find some code of a
    // function and then starts searching for function prologues it's likely to find a "JMP imm" that just happens to be part
    // of the control flow in the missed code. It then tries to turn that JMP into its own function right in the middle of some
    // other function and the CG gets all messed up.
    if (size_t n = isJmpImmThunk(partitioner, insns))
        return n;
#endif
    return 0;
}

void
splitThunkFunctions(Partitioner &partitioner) {
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    debug <<"splitThunkFunctions\n";
    std::vector<Function::Ptr> workList = partitioner.functions();
    while (!workList.empty()) {
        Function::Ptr candidate = workList.back();
        workList.pop_back();
        SAWYER_MESG(debug) <<"  considering " <<candidate->printableName() <<"\n";

        // Get the entry vertex in the CFG and the entry basic block.
        ControlFlowGraph::ConstVertexIterator entryVertex = partitioner.findPlaceholder(candidate->address());
        ASSERT_require(partitioner.cfg().isValidVertex(entryVertex));
        if (entryVertex->value().type() != V_BASIC_BLOCK)
            continue;
        BasicBlock::Ptr entryBlock = entryVertex->value().bblock();
        ASSERT_not_null(entryBlock);

        // All incoming edges must be function calls, function transfers, etc. We cannot split the thunk from the beginning of
        // the entry block if the entry block is a successor of some other non-call block in the same function (e.g., the top
        // of a loop).  Recursive calls (other than optimized tail recursion) should be fine.
        bool hasIntraFunctionEdge = false;
        BOOST_FOREACH (const ControlFlowGraph::Edge &edge, entryVertex->inEdges()) {
            if (edge.value().type() == E_NORMAL) {
                hasIntraFunctionEdge = true;
                break;
            }
        }
        if (hasIntraFunctionEdge)
            continue;

        // Does the function appear to start with a thunk pattern of instructions?
        size_t thunkSize = isThunk(partitioner, entryBlock->instructions());
#if 1 // [Robb P. Matzke 2015-06-26]: this case is commented out in isThunk(), so handle it here
        if (0 == thunkSize)
            thunkSize = isJmpImmThunk(partitioner, entryBlock->instructions());
#endif
        if (0 == thunkSize)
            continue;

        // Is the thunk pattern a proper subsequence of the entry block?
        bool thunkIsPrefix = thunkSize < entryBlock->nInstructions();
        if (!thunkIsPrefix && candidate->basicBlockAddresses().size()==1)
            continue;                                   // function is only a thunk already
        if (!thunkIsPrefix && entryVertex->nOutEdges() != 1)
            continue;                                   // thunks have only one outgoing edge

        // FIXME[Robb P. Matzke 2015-07-09]: The basic-block splitting part could be its own function
        // FIXME[Robb P. Matzke 2015-07-09]: The function splitting part could be its own function

        // By now we've determined that there is indeed a thunk that must be split off from the big candidate function. We
        // can't just remove the thunk's basic block from the candidate function because the thunk is the candidate function's
        // entry block. Therefore detach the big function from the CFG to make room for new thunk and target functions.
        SAWYER_MESG(debug) <<"    " <<candidate->printableName() <<" starts with a thunk\n";
        partitioner.detachFunction(candidate);

        // If the thunk is a proper prefix of the candidate function's entry block then split the entry block in two.
        BasicBlock::Ptr origEntryBlock = entryBlock;
        ControlFlowGraph::ConstVertexIterator targetVertex = partitioner.cfg().vertices().end();
        if (thunkIsPrefix) {
            SAWYER_MESG(debug) <<"    splitting entry " <<origEntryBlock->printableName() <<"\n";
            targetVertex = partitioner.truncateBasicBlock(entryVertex, entryBlock->instructions()[thunkSize]);
            entryBlock = entryVertex->value().bblock();
            SAWYER_MESG(debug) <<"    new entry is " <<entryBlock->printableName() <<"\n";
            ASSERT_require(entryBlock != origEntryBlock); // we need the original block for its analysis results below
            ASSERT_require(entryBlock->nInstructions() < origEntryBlock->nInstructions());
        } else {
            targetVertex = entryVertex->outEdges().begin()->target();
            SAWYER_MESG(debug) <<"    eliding entry block; new entry is " <<partitioner.vertexName(targetVertex) <<"\n";
        }
        ASSERT_require(partitioner.cfg().isValidVertex(targetVertex));

        // Create the new thunk function.
        Function::Ptr thunkFunction = Function::instance(candidate->address(), SgAsmFunction::FUNC_THUNK);
        SAWYER_MESG(debug) <<"    created thunk " <<thunkFunction->printableName() <<"\n";
        partitioner.attachFunction(thunkFunction);

        // Create the new target function, which has basically the same features as the original candidate function except a
        // different entry address.  The target might be indeterminate (e.g., "jmp [address]" where address is not mapped or
        // non-const), in which case we shouldn't create a function there (in fact, we can't since indeterminate has no
        // concrete address and functions need entry addresses). Since the partitioner supports shared basic blocks (basic
        // block owned by multiple functions), the target vertex might already be a function, in which case we shouldn't try to
        // create it.
        if (targetVertex->value().type() == V_BASIC_BLOCK && !partitioner.functionExists(targetVertex->value().address())) {
            unsigned newReasons = (candidate->reasons() & ~SgAsmFunction::FUNC_THUNK) | SgAsmFunction::FUNC_GRAPH;
            Function::Ptr newFunc = Function::instance(targetVertex->value().address(), candidate->name(), newReasons);
            newFunc->comment(candidate->comment());
            BOOST_FOREACH (rose_addr_t va, candidate->basicBlockAddresses()) {
                if (va != thunkFunction->address())
                    newFunc->insertBasicBlock(va);
            }
            BOOST_FOREACH (const DataBlock::Ptr &db, candidate->dataBlocks())
                newFunc->insertDataBlock(db);
            partitioner.attachFunction(newFunc);
            workList.push_back(newFunc);                // new function might have more thunks to split off yet.

            if (origEntryBlock != entryBlock) {         // original entry block was split
                // Discover the new function's entry block. This new block is much like the original block since conceptually
                // the thunk and this new block are really a single basic block.  However, discover thunk might not have been
                // able to resolve things like opaque predicates and the new block might therefore be shorter.  Consider:
                //     mov eax, global      ; part of thunk
                //     jmp [eax]            ; part of thunk
                //
                //     cmp eax, global
                //     jne foo              ; opaque predicate when thunk was attached to this block
                //     nop                  ; originally part of this block, but now will start a new block
                BasicBlock::Ptr targetBlock = partitioner.discoverBasicBlock(newFunc->address());
                if (targetBlock->nInstructions() + entryBlock->nInstructions() == origEntryBlock->nInstructions()) {
                    BOOST_FOREACH (const DataBlock::Ptr &db, origEntryBlock->dataBlocks())
                        targetBlock->insertDataBlock(db);
                    targetBlock->copyCache(origEntryBlock);
                }

                partitioner.attachBasicBlock(targetBlock);
            }
        }

        // Fix edge types between the thunk and the target function
        for (ControlFlowGraph::ConstEdgeIterator ei=entryVertex->outEdges().begin(); ei!=entryVertex->outEdges().end(); ++ei)
            partitioner.fixInterFunctionEdge(ei);
    }
}

bool
MatchThunk::match(const Partitioner &partitioner, rose_addr_t anchor) {
    // Disassemble the next few undiscovered instructions
    static const size_t maxInsns = 2;                   // max length of a thunk
    std::vector<SgAsmInstruction*> insns;
    rose_addr_t va = anchor;
    for (size_t i=0; i<maxInsns; ++i) {
        if (partitioner.instructionExists(va))
            break;                                      // look only for undiscovered instructions
        SgAsmInstruction *insn = partitioner.discoverInstruction(va);
        if (!insn)
            break;
        insns.push_back(insn);
        va += insn->get_size();
    }
    if (insns.empty())
        return false;

    functions_.clear();
    size_t thunkSize = isThunk(partitioner, insns);
    if (0 == thunkSize)
        return false;

    // This is a thunk
    functions_.push_back(Function::instance(anchor, SgAsmFunction::FUNC_THUNK));

    // Do we know the successors?  They would be the function(s) to which the thunk branches.
    BasicBlock::Ptr bb = BasicBlock::instance(anchor, &partitioner);
    for (size_t i=0; i<thunkSize; ++i)
        bb->append(insns[i]);
    BOOST_FOREACH (const BasicBlock::Successor &successor, partitioner.basicBlockSuccessors(bb)) {
        if (successor.expr()->is_number()) {
            rose_addr_t targetVa = successor.expr()->get_number();
            if (!partitioner.functionExists(targetVa))
                insertUnique(functions_, Function::instance(targetVa, SgAsmFunction::FUNC_GRAPH), sortFunctionsByAddress);
        }
    }

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
    if (!jmp || jmp->get_kind()!=x86_jmp || jmp->get_operandList()->get_operands().size()!=1)
        return false;                                   // not a JMP instruction
    SgAsmMemoryReferenceExpression *mre = isSgAsmMemoryReferenceExpression(jmp->get_operandList()->get_operands()[0]);
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
                                partitioner.instructionProvider().instructionPointerRegister().get_nbits());
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
                                partitioner.instructionProvider().stackPointerRegister().get_nbits());
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
    const RegisterDescriptor BP(x86_regclass_gpr, x86_gpr_bp, 0, SP.get_nbits());
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
                                partitioner.instructionProvider().instructionPointerRegister().get_nbits());
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
                                partitioner.instructionProvider().stackPointerRegister().get_nbits());
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
                                partitioner.instructionProvider().instructionPointerRegister().get_nbits());
    SgAsmRegisterReferenceExpression *rre = isSgAsmRegisterReferenceExpression(opands[0]);
    if (!rre || rre->get_descriptor()!=SI)
        return false;

    return true;
}

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
    size_t wordSize = args.partitioner.instructionProvider().instructionPointerRegister().get_nbits() / 8;
    AddressInterval whole = AddressInterval::hull(0, IntegerOps::genMask<rose_addr_t>(8*wordSize));
    AddressInterval tableLimits = AddressInterval::hull(tableVa, whole.greatest());

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
    std::vector<rose_addr_t> tableEntries = scanCodeAddressTable(args.partitioner, tableLimits /*in,out*/,
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
