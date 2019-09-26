#include <sage3basic.h>
#include <Partitioner2/Thunk.h>

#include <Partitioner2/ModulesX86.h>
#include <Partitioner2/Partitioner.h>

using namespace Sawyer::Message::Common;

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Individual thunk predicates
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ThunkDetection
isX86JmpMemThunk(const Partitioner &partitioner, const std::vector<SgAsmInstruction*> &insns) {
    if (insns.empty())
        return ThunkDetection();
    SgAsmX86Instruction *jmp = isSgAsmX86Instruction(insns[0]);
    if (!ModulesX86::matchJmpMem(partitioner, jmp))
        return ThunkDetection();
    return ThunkDetection(1, "JMP [address]");
}

ThunkDetection
isX86LeaJmpThunk(const Partitioner &partitioner, const std::vector<SgAsmInstruction*> &insns) {
    if (insns.size() < 2)
        return ThunkDetection();

    // LEA ECX, [EBP + constant]
    SgAsmX86Instruction *lea = isSgAsmX86Instruction(insns[0]);
    if (!ModulesX86::matchLeaCxMemBpConst(partitioner, lea))
        return ThunkDetection();

    // JMP address
    SgAsmX86Instruction *jmp = isSgAsmX86Instruction(insns[1]);
    if (!ModulesX86::matchJmpConst(partitioner, jmp))
        return ThunkDetection();

    return ThunkDetection(2, "LEA ECX, [EBP + constant]; JMP address");
}

ThunkDetection
isX86MovJmpThunk(const Partitioner &partitioner, const std::vector<SgAsmInstruction*> &insns) {
    if (insns.size() < 2)
        return ThunkDetection();

    // MOV reg1, [address]
    SgAsmX86Instruction *mov = isSgAsmX86Instruction(insns[0]);
    if (!mov || mov->get_kind() != x86_mov)
        return ThunkDetection();
    const SgAsmExpressionPtrList &movArgs = mov->get_operandList()->get_operands();
    if (movArgs.size() != 2)
        return ThunkDetection();
    SgAsmDirectRegisterExpression *movArg0 = isSgAsmDirectRegisterExpression(movArgs[0]);
    SgAsmMemoryReferenceExpression *movArg1 = isSgAsmMemoryReferenceExpression(movArgs[1]);
    if (!movArg0 || !movArg1)
        return ThunkDetection();

    // JMP reg1
    SgAsmX86Instruction *jmp = isSgAsmX86Instruction(insns[1]);
    if (!jmp || jmp->get_kind() != x86_jmp)
        return ThunkDetection();
    const SgAsmExpressionPtrList &jmpArgs = jmp->get_operandList()->get_operands();
    if (jmpArgs.size() != 1)
        return ThunkDetection();
    SgAsmDirectRegisterExpression *jmpArg0 = isSgAsmDirectRegisterExpression(jmpArgs[0]);
    if (!jmpArg0)
        return ThunkDetection();
    if (jmpArg0->get_descriptor() != movArg0->get_descriptor())
        return ThunkDetection();

    return ThunkDetection(2, "MOV reg1, [address]; JMP reg1");
}

ThunkDetection
isX86JmpImmThunk(const Partitioner &partitioner, const std::vector<SgAsmInstruction*> &insns) {
    if (insns.empty())
        return ThunkDetection();
    SgAsmX86Instruction *jmp = isSgAsmX86Instruction(insns[0]);
    if (!jmp || jmp->get_kind() != x86_jmp)
        return ThunkDetection();
    const SgAsmExpressionPtrList &jmpArgs = jmp->get_operandList()->get_operands();
    if (jmpArgs.size() != 1)
        return ThunkDetection();
    SgAsmIntegerValueExpression *jmpArg0 = isSgAsmIntegerValueExpression(jmpArgs[0]);
    if (!jmpArg0)
        return ThunkDetection();
    rose_addr_t targetVa = jmpArg0->get_absoluteValue();
    if (!partitioner.memoryMap()->require(MemoryMap::EXECUTABLE).at(targetVa).exists())
        return ThunkDetection();                        // target must be an executable address
    if (!partitioner.instructionExists(targetVa) && !partitioner.instructionsOverlapping(targetVa).empty())
        return ThunkDetection();                        // points to middle of some instruction
    return ThunkDetection(1, "JMP address");
}

ThunkDetection
isX86AddJmpThunk(const Partitioner &partitioner, const std::vector<SgAsmInstruction*> &insns) {
    if (insns.size() < 2)
        return ThunkDetection();
    SgAsmX86Instruction *add = isSgAsmX86Instruction(insns[0]);
    if (!add || add->get_kind() != x86_add)
        return ThunkDetection();
    const SgAsmExpressionPtrList &addArgs = add->get_operandList()->get_operands();
    if (addArgs.size() != 2)
        return ThunkDetection();
    SgAsmDirectRegisterExpression *addArg0 = isSgAsmDirectRegisterExpression(addArgs[0]);
    if (!addArg0 || addArg0->get_descriptor().majorNumber() != x86_regclass_gpr ||
        addArg0->get_descriptor().minorNumber() != x86_gpr_cx)
        return ThunkDetection();
    SgAsmIntegerValueExpression *addArg1 = isSgAsmIntegerValueExpression(addArgs[1]);
    if (!addArg1)
        return ThunkDetection();
    SgAsmX86Instruction *jmp = isSgAsmX86Instruction(insns[1]);
    if (!jmp || jmp->get_kind() != x86_jmp)
        return ThunkDetection();
    const SgAsmExpressionPtrList &jmpArgs = jmp->get_operandList()->get_operands();
    if (jmpArgs.size() != 1)
        return ThunkDetection();
    SgAsmIntegerValueExpression *jmpArg0 = isSgAsmIntegerValueExpression(jmpArgs[0]);
    if (!jmpArg0)
        return ThunkDetection();
    return ThunkDetection(2, "ADD r, constant; JMP address <where \"r\" is one of the cx registers>");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Collective thunk predicates
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// class method
ThunkPredicates::Ptr
ThunkPredicates::functionMatcherThunks() {
    Ptr retval = ThunkPredicates::instance();
    retval->predicates().push_back(isX86LeaJmpThunk);
    retval->predicates().push_back(isX86MovJmpThunk);
    retval->predicates().push_back(isX86JmpMemThunk);
    retval->predicates().push_back(isX86AddJmpThunk);

#if 0 // [Robb P. Matzke 2015-06-23]: disabled for now, but see splitThunkFunctions
    // This matcher is causing too many false positives. The problem is that when the partitioner fails to find some code of a
    // function and then starts searching for function prologues it's likely to find a "JMP imm" that just happens to be part
    // of the control flow in the missed code. It then tries to turn that JMP into its own function right in the middle of some
    // other function and the CG gets all messed up.
    retval->predicates().push_back(isX86JmpImmThunk);
#endif

    return retval;
}

ThunkPredicates::Ptr
ThunkPredicates::allThunks() {
    Ptr retval = ThunkPredicates::instance();
    retval->predicates().push_back(isX86LeaJmpThunk);
    retval->predicates().push_back(isX86MovJmpThunk);
    retval->predicates().push_back(isX86JmpMemThunk);
    retval->predicates().push_back(isX86AddJmpThunk);
    retval->predicates().push_back(isX86JmpImmThunk);
    return retval;
}

ThunkDetection
ThunkPredicates::isThunk(const Partitioner &partitioner, const std::vector<SgAsmInstruction*> &insns) const {
    BOOST_FOREACH (ThunkPredicate predicate, predicates_) {
        if (predicate) {
            if (ThunkDetection retval = (predicate)(partitioner, insns))
                return retval;
        }
    }
    return ThunkDetection();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Thunk utilities
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
splitThunkFunctions(Partitioner &partitioner, const ThunkPredicates::Ptr &thunkPredicates) {
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    debug <<"splitThunkFunctions\n";
    if (!thunkPredicates || thunkPredicates->predicates().empty()) {
        debug <<"  no predicates; nothing to do\n";
        return;
    }

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
        if (entryBlock == NULL)
            continue;                                   // can't split a block if we haven't discovered it yet

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
        ThunkDetection found = thunkPredicates->isThunk(partitioner, entryBlock->instructions());
        if (!found)
            continue;

        // Is the thunk pattern a proper subsequence of the entry block?
        bool thunkIsPrefix = found.nInsns < entryBlock->nInstructions();
        if (!thunkIsPrefix && candidate->basicBlockAddresses().size()==1) {
            // Function is only a thunk already, so make sure the FUNC_THUNK bit is set.
            candidate->insertReasons(SgAsmFunction::FUNC_THUNK);
            if (candidate->reasonComment().empty() && !found.name.empty())
                candidate->reasonComment("matched " + found.name);
            continue;                                   // function is only a thunk already
        }
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
            targetVertex = partitioner.truncateBasicBlock(entryVertex, entryBlock->instructions()[found.nInsns]);
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
        std::string reasonComment = found.name.empty() ? "" : "matched " + found.name;
        reasonComment = (reasonComment.empty() ? "" : " ") + std::string("split from ") + candidate->printableName();
        thunkFunction->reasonComment(reasonComment);
        SAWYER_MESG(debug) <<"    created thunk " <<thunkFunction->printableName() <<"\n";
        partitioner.attachFunction(thunkFunction);

        // Create the new target function, which has basically the same features as the original candidate function except a
        // different entry address.  The target might be indeterminate (e.g., "jmp [address]" where address is not mapped or
        // non-const), in which case we shouldn't create a function there (in fact, we can't since indeterminate has no
        // concrete address and functions need entry addresses). Since the partitioner supports shared basic blocks (basic
        // block owned by multiple functions), the target vertex might already be a function, in which case we shouldn't try to
        // create it.
        if (targetVertex->value().type() == V_BASIC_BLOCK && !partitioner.functionExists(targetVertex->value().address())) {
            unsigned newReasons = (candidate->reasons() & ~SgAsmFunction::FUNC_THUNK) | SgAsmFunction::FUNC_THUNK_TARGET;
            Function::Ptr newFunc = Function::instance(targetVertex->value().address(), candidate->name(), newReasons);
            newFunc->comment(candidate->comment());
            if (candidate->reasonComment().empty()) {
                newFunc->reasonComment("from thunk " + thunkFunction->printableName());
            } else {
                newFunc->reasonComment(candidate->reasonComment());
            }
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

} // namespace
} // namespace
} // namespace

