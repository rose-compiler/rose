/* SgAsmInstruction member definitions.  Do not move them to src/ROSETTA/Grammar/BinaryInstruction.code (or any *.code file)
 * because then they won't get indexed/formatted/etc. by C-aware tools. */
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"

#include "BinaryNoOperation.h"
#include "Diagnostics.h"
#include "Disassembler.h"
#include "AsmUnparser_compat.h"

using namespace Rose;
using namespace Rose::Diagnostics;
using namespace Rose::BinaryAnalysis;

// Indicates concrete stack delta is not known or not calculated.
const int64_t SgAsmInstruction::INVALID_STACK_DELTA = (uint64_t)1 << 63; // fairly arbitrary, but far from zero

size_t
SgAsmInstruction::nOperands() const {
    if (!get_operandList())
        return 0;
    return get_operandList()->get_operands().size();
}

SgAsmExpression*
SgAsmInstruction::operand(size_t i) const {
    return i < nOperands() ? get_operandList()->get_operands()[i] : NULL;
}

void
SgAsmInstruction::appendSources(SgAsmInstruction *inst) {
    p_sources.push_back(inst);
}

unsigned
SgAsmInstruction::get_anyKind() const {
    // ROSETTA doesn't support pure virtual, so run-time errors is the best we can do.
    ASSERT_not_reachable("SgAsmInstruction::get_kind() should have been implemented in " + class_name());
}

AddressSet
SgAsmInstruction::getSuccessors(bool *complete) {
    abort();
    // tps (12/9/2009) : MSC requires a return value
    return AddressSet();
}

AddressSet
SgAsmInstruction::getSuccessors(const std::vector<SgAsmInstruction*>& basic_block, bool *complete/*out*/,
                                const MemoryMap::Ptr &initial_memory/*=NULL*/)
{
    if (basic_block.size()==0) {
        if (complete) *complete = true;
        return AddressSet();
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
            InstructionSemantics2::mlog[WARN] <<"SgAsmInstruction::hasEffect: relax_stack_semantics not implemented\n";
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
            InstructionSemantics2::mlog[WARN] <<"SgAsmInstruction::hasEffect: relax_stack_semantics not implemented\n";
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

std::string
SgAsmInstruction::toString() const {
    SgAsmInstruction *insn = const_cast<SgAsmInstruction*>(this); // old API doesn't use 'const'
    std::string retval = StringUtility::addrToString(get_address()) + ": " + unparseMnemonic(insn);
    if (SgAsmOperandList *opList = insn->get_operandList()) {
        const SgAsmExpressionPtrList &operands = opList->get_operands();
        for (size_t i = 0; i < operands.size(); ++i) {
            retval += i == 0 ? " " : ", ";
            retval += StringUtility::trim(unparseExpression(operands[i], NULL, NULL));
        }
    }
    return retval;
}

std::set<rose_addr_t>
SgAsmInstruction::explicitConstants() const {
    struct T1: AstSimpleProcessing {
        std::set<rose_addr_t> values;
        void visit(SgNode *node) {
            if (SgAsmIntegerValueExpression *ive = isSgAsmIntegerValueExpression(node))
                values.insert(ive->get_absoluteValue());
        }
    } t1;
#if 0 // [Robb Matzke 2019-02-06]: ROSE API deficiency: cannot traverse a const AST
    t1.traverse(this, preorder);
#else
    t1.traverse(const_cast<SgAsmInstruction*>(this), preorder);
#endif
    return t1.values;
}


static SAWYER_THREAD_TRAITS::Mutex semanticFailureMutex;

size_t
SgAsmInstruction::semanticFailure() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(semanticFailureMutex);
    return semanticFailure_.n;
}

void
SgAsmInstruction::semanticFailure(size_t n) {
    SAWYER_THREAD_TRAITS::LockGuard lock(semanticFailureMutex);
    semanticFailure_.n = n;
}

void
SgAsmInstruction::incrementSemanticFailure() {
    SAWYER_THREAD_TRAITS::LockGuard lock(semanticFailureMutex);
    ++semanticFailure_.n;
}

#endif
