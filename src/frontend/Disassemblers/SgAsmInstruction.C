/* SgAsmInstruction member definitions.  Do not move them to src/ROSETTA/Grammar/BinaryInstruction.code (or any *.code file)
 * because then they won't get indexed/formatted/etc. by C-aware tools. */

#include "sage3basic.h"
#include "BinaryNoOperation.h"
#include "Diagnostics.h"
#include "Disassembler.h"

using namespace rose;
using namespace rose::Diagnostics;
using namespace rose::BinaryAnalysis;

/** Indicates concrete stack delta is not known or not calculated. */
const int64_t SgAsmInstruction::INVALID_STACK_DELTA = (uint64_t)1 << 63; // fairly arbitrary, but far from zero

void
SgAsmInstruction::appendSources(SgAsmInstruction *inst) {
    p_sources.push_back(inst);
}

SgAsmInstruction*
SgAsmInstruction::cfgBinFlowOutEdge(const VirtualBinCFG::AuxiliaryInformation *info) {
    if (!isAsmUnconditionalBranch(this)) {
        SgAsmInstruction* next = info->getInstructionAtAddress(this->get_address() + this->get_raw_bytes().size());
        return next;
    }
    return NULL;
}

std::vector<VirtualBinCFG::CFGEdge>
SgAsmInstruction::cfgBinOutEdges(const VirtualBinCFG::AuxiliaryInformation *info) {
    std::vector<VirtualBinCFG::CFGEdge> result;
    uint64_t nextAddr = this->get_address() + this->get_raw_bytes().size();

    if (isAsmBranch(this)) {
        uint64_t addr = 0;
        bool knownTarget = getBranchTarget(&addr);
        if (knownTarget) {
            SgAsmInstruction* tgt = info->getInstructionAtAddress(addr);
            if (tgt)
                makeEdge(this, tgt, info, result);
        } else {
            // Unknown target
            const std::set<uint64_t> successorAddrs = info->getPossibleSuccessors(this);
            for (std::set<uint64_t>::const_iterator i = successorAddrs.begin(); i != successorAddrs.end(); ++i) {
                SgAsmInstruction* tgt = info->getInstructionAtAddress(*i);
                if (tgt)
                    makeEdge(this, tgt, info, result);
            }
        }
    }

    if ((!isSgAsmX86Instruction(this) ||
         (isSgAsmX86Instruction(this)->get_kind() != x86_ret && isSgAsmX86Instruction(this)->get_kind() != x86_hlt))) {
        SgAsmInstruction* next = info->getInstructionAtAddress(nextAddr);
        if (next)
            makeEdge(this, next, info, result);
    }
    return result;
}

std::vector<VirtualBinCFG::CFGEdge>
SgAsmInstruction::cfgBinInEdges(const VirtualBinCFG::AuxiliaryInformation *info) {
    std::vector<VirtualBinCFG::CFGEdge> result;
    const std::set<uint64_t>& preds = info->getPossiblePredecessors(this);
    for (std::set<uint64_t>::const_iterator i = preds.begin(); i != preds.end(); ++i) {
        SgAsmInstruction* tgt = info->getInstructionAtAddress(*i);
        if (tgt)
            makeEdge(this, tgt, info, result);
    }
    return result;
}

unsigned
SgAsmInstruction::get_anyKind() const {
    // ROSETTA doesn't support pure virtual, so run-time errors is the best we can do.
    ASSERT_not_reachable("SgAsmInstruction::get_kind() should have been implemented in " + class_name());
}

std::set<rose_addr_t>
SgAsmInstruction::getSuccessors(bool *complete) {
    abort();
    // tps (12/9/2009) : MSC requires a return value
    std::set<rose_addr_t> t;
    return t;
}

std::set<rose_addr_t>
SgAsmInstruction::getSuccessors(const std::vector<SgAsmInstruction*>& basic_block, bool *complete/*out*/,
                                const MemoryMap *initial_memory/*=NULL*/)
{
    if (basic_block.size()==0) {
        if (complete) *complete = true;
        return std::set<rose_addr_t>();
    }
    return basic_block.back()->getSuccessors(complete);
}

bool
SgAsmInstruction::terminatesBasicBlock()
{
    abort();                                            // rosetta doesn't support pure virtual functions
#ifdef _MSC_VER
    return false;                                       // tps (12/9/2009) : MSC requires a return value
#endif
}

bool
SgAsmInstruction::isFunctionCallFast(const std::vector<SgAsmInstruction*>&, rose_addr_t *target, rose_addr_t *return_va)
{
    return false;
}

bool
SgAsmInstruction::isFunctionCallSlow(const std::vector<SgAsmInstruction*>&, rose_addr_t *target, rose_addr_t *return_va)
{
    return false;
}

bool
SgAsmInstruction::isFunctionReturnFast(const std::vector<SgAsmInstruction*>&)
{
    return false;
}

bool
SgAsmInstruction::isFunctionReturnSlow(const std::vector<SgAsmInstruction*>&)
{
    return false;
}

bool
SgAsmInstruction::isFirstInBlock()
{
    SgAsmBlock *bb = SageInterface::getEnclosingNode<SgAsmBlock>(this);
    if (bb) {
        const SgAsmStatementPtrList &stmts = bb->get_statementList();
        for (size_t i=0; i<stmts.size(); ++i) {
            if (SgAsmInstruction *insn = isSgAsmInstruction(stmts[i]))
                return insn==this;
        }
    }
    return false;
}

bool
SgAsmInstruction::isLastInBlock()
{
    SgAsmBlock *bb = SageInterface::getEnclosingNode<SgAsmBlock>(this);
    if (bb) {
        const SgAsmStatementPtrList &stmts = bb->get_statementList();
        for (size_t i=stmts.size(); i>0; --i) {
            if (SgAsmInstruction *insn = isSgAsmInstruction(stmts[i-1]))
                return insn==this;
        }
    }
    return false;
}

bool
SgAsmInstruction::getBranchTarget(rose_addr_t *target/*out*/) {
    return false;
}

bool
SgAsmInstruction::hasEffect()
{
    std::vector<SgAsmInstruction*> sequence;
    sequence.push_back(this);
    return hasEffect(sequence, false);
}

// Build analyzer for no-ops
static NoOperation
buildNopAnalyzer(SgAsmInterpretation *interp) {
    using namespace InstructionSemantics2;

    if (!interp) {
        static bool emitted = false;
        if (!emitted && InstructionSemantics2::mlog[WARN]) {
            InstructionSemantics2::mlog[WARN] <<"SgAsmInstruction::buildDispatcher: no binary interpretation\n";
            emitted = true;
        }
        return NoOperation(BaseSemantics::DispatcherPtr());
    }

    Disassembler *disassembler = Disassembler::lookup(interp);
    return NoOperation(disassembler);
}

bool
SgAsmInstruction::hasEffect(const std::vector<SgAsmInstruction*> &insns, bool allow_branch/*false*/,
                            bool relax_stack_semantics/*false*/)
{
    SgAsmInterpretation *interp = SageInterface::getEnclosingNode<SgAsmInterpretation>(insns.front());
    NoOperation analyzer = buildNopAnalyzer(interp);

    if (relax_stack_semantics) {
        static bool emitted = false;
        if (!emitted && InstructionSemantics2::mlog[WARN]) {
            InstructionSemantics2::mlog[WARN] <<"SgAsmX86Instruction::hasEffect: relax_stack_semantics not implemented\n";
            emitted = true;
        }
    }

    return !analyzer.isNoop(insns);
}

std::vector<std::pair<size_t,size_t> >
SgAsmInstruction::findNoopSubsequences(const std::vector<SgAsmInstruction*>& insns, bool allow_branch/*false*/, 
                                       bool relax_stack_semantics/*false*/)
{
    SgAsmInterpretation *interp = SageInterface::getEnclosingNode<SgAsmInterpretation>(insns.front());
    NoOperation analyzer = buildNopAnalyzer(interp);

    if (relax_stack_semantics) {
        static bool emitted = false;
        if (!emitted && InstructionSemantics2::mlog[WARN]) {
            InstructionSemantics2::mlog[WARN] <<"SgAsmX86Instruction::hasEffect: relax_stack_semantics not implemented\n";
            emitted = true;
        }
    }

    NoOperation::IndexIntervals indexes = analyzer.findNoopSubsequences(insns);
    std::vector<std::pair<size_t, size_t> > retval;
    BOOST_FOREACH (const NoOperation::IndexInterval &interval, indexes)
        retval.push_back(std::make_pair(interval.least(), interval.size()));
    return retval;
}

size_t
SgAsmInstruction::get_size() const
{
    return p_raw_bytes.size();
}

bool
SgAsmInstruction::isUnknown() const
{
    abort(); // too bad ROSETTA doesn't allow virtual base classes
    return false;
}
