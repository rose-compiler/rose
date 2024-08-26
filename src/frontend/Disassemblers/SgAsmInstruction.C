// SgAsmInstruction member definitions.
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Dispatcher.h>
#include <Rose/BinaryAnalysis/NoOperation.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/Diagnostics.h>

#include "AsmUnparser_compat.h"

using namespace Rose;
using namespace Rose::Diagnostics;
using namespace Rose::BinaryAnalysis;

// Indicates concrete stack delta is not known or not calculated.
const int64_t SgAsmInstruction::INVALID_STACK_DELTA = (uint64_t)1 << 63; // fairly arbitrary, but far from zero

std::string
SgAsmInstruction::architectureIdSerialize(uint8_t id) const {
    return Architecture::name(id);
}

uint8_t
SgAsmInstruction::architectureIdDeserialize(const std::string &name) const {
    return boost::numeric_cast<uint8_t>(*Architecture::findByName(name).orThrow()->registrationId());
}

Architecture::Base::ConstPtr
SgAsmInstruction::architecture() const {
    return Architecture::findById(get_architectureId()).orThrow();
}

std::string
SgAsmInstruction::get_mnemonic() const {
    return architecture()->instructionMnemonic(this);
}

// [Robb Matzke 2023-12-04]: deprecated
std::string
SgAsmInstruction::description() const {
    return architecture()->instructionDescription(this);
}

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

unsigned
SgAsmInstruction::get_anyKind() const {
    // ROSETTA doesn't support pure virtual, so run-time errors is the best we can do.
    ASSERT_not_reachable("SgAsmInstruction::get_kind() should have been implemented in " + class_name());
}

// [Robb Matzke 2023-12-04]: deprecated
AddressSet
SgAsmInstruction::getSuccessors(bool &complete) {
    return architecture()->getSuccessors(this, complete);
}

// [Robb Matzke 2023-12-04]: deprecated
AddressSet
SgAsmInstruction::getSuccessors(const std::vector<SgAsmInstruction*>& basic_block, bool &complete,
                                const MemoryMap::Ptr &initial_memory) {
    return architecture()->getSuccessors(basic_block, complete /*out*/, initial_memory);
}

// [Robb Matzke 2023-12-04]: deprecated
bool
SgAsmInstruction::terminatesBasicBlock() {
    return architecture()->terminatesBasicBlock(this);
}

// [Robb Matzke 2023-12-04]: deprecated
bool
SgAsmInstruction::isFunctionCallFast(const std::vector<SgAsmInstruction*> &insns, rose_addr_t *target, rose_addr_t *return_va) {
    return architecture()->isFunctionCallFast(insns, target, return_va);
}

// [Robb Matzke 2023-12-04]: deprecated
bool
SgAsmInstruction::isFunctionCallSlow(const std::vector<SgAsmInstruction*> &insns, rose_addr_t *target, rose_addr_t *return_va) {
    return architecture()->isFunctionCallSlow(insns, target, return_va);
}

// [Robb Matzke 2023-12-04]: deprecated
bool
SgAsmInstruction::isFunctionReturnFast(const std::vector<SgAsmInstruction*> &insns) {
    return architecture()->isFunctionReturnFast(insns);
}

// [Robb Matzke 2023-12-04]: deprecated
bool
SgAsmInstruction::isFunctionReturnSlow(const std::vector<SgAsmInstruction*> &insns) {
    return architecture()->isFunctionReturnSlow(insns);
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

// [Robb Matzke 2023-12-04]: deprecated
Sawyer::Optional<rose_addr_t>
SgAsmInstruction::branchTarget() {
    return architecture()->branchTarget(this);
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
    using namespace InstructionSemantics;

    if (!interp) {
        static bool emitted = false;
        if (!emitted && InstructionSemantics::mlog[WARN]) {
            InstructionSemantics::mlog[WARN] <<"SgAsmInstruction::buildDispatcher: no binary interpretation\n";
            emitted = true;
        }
        return NoOperation(BaseSemantics::Dispatcher::Ptr());
    }

    Architecture::Base::Ptr arch = Architecture::findByInterpretation(interp).orThrow();
    Disassembler::Base::Ptr disassembler = arch->newInstructionDecoder();
    return NoOperation(disassembler);
}

bool
SgAsmInstruction::hasEffect(const std::vector<SgAsmInstruction*> &insns, bool /*allow_branch=false*/,
                            bool relax_stack_semantics/*false*/)
{
    SgAsmInterpretation *interp = SageInterface::getEnclosingNode<SgAsmInterpretation>(insns.front());
    NoOperation analyzer = buildNopAnalyzer(interp);

    if (relax_stack_semantics) {
        static bool emitted = false;
        if (!emitted && InstructionSemantics::mlog[WARN]) {
            InstructionSemantics::mlog[WARN] <<"SgAsmInstruction::hasEffect: relax_stack_semantics not implemented\n";
            emitted = true;
        }
    }

    return !analyzer.isNoop(insns);
}

std::vector<std::pair<size_t,size_t> >
SgAsmInstruction::findNoopSubsequences(const std::vector<SgAsmInstruction*>& insns, bool /*allow_branch=false*/,
                                       bool relax_stack_semantics/*false*/)
{
    SgAsmInterpretation *interp = SageInterface::getEnclosingNode<SgAsmInterpretation>(insns.front());
    NoOperation analyzer = buildNopAnalyzer(interp);

    if (relax_stack_semantics) {
        static bool emitted = false;
        if (!emitted && InstructionSemantics::mlog[WARN]) {
            InstructionSemantics::mlog[WARN] <<"SgAsmInstruction::hasEffect: relax_stack_semantics not implemented\n";
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
    return get_rawBytes().size();
}

// [Robb Matzke 2023-12-05]: deprecated
bool SgAsmInstruction::isUnknown() const {
    return architecture()->isUnknown(this);
}

std::string
SgAsmInstruction::toString() const {
    if (isSgAsmUserInstruction(this))
        return architecture()->toString(this);

    // Old backward-compatible stuff that we'd like to eventually remove
    SgAsmInstruction *insn = const_cast<SgAsmInstruction*>(this); // old API doesn't use 'const'
    std::string retval = StringUtility::addrToString(get_address()) + ": " + unparseMnemonic(insn);
    if (SgAsmOperandList *opList = insn->get_operandList()) {
        const SgAsmExpressionPtrList &operands = opList->get_operands();
        for (size_t i = 0; i < operands.size(); ++i) {
            retval += i == 0 ? " " : ", ";
            retval += StringUtility::trim(unparseExpression(operands[i], NULL, RegisterDictionary::Ptr()));
        }
    }
    return retval;
}

std::string
SgAsmInstruction::toStringNoAddr() const {
    if (isSgAsmUserInstruction(this))
        return architecture()->toStringNoAddr(this);

    // Old backward-compatible stuff that we'd like to eventually remove
    SgAsmInstruction *insn = const_cast<SgAsmInstruction*>(this); // old API doesn't use 'const'
    std::string retval = unparseMnemonic(insn);
    if (SgAsmOperandList *opList = insn->get_operandList()) {
        const SgAsmExpressionPtrList &operands = opList->get_operands();
        for (size_t i = 0; i < operands.size(); ++i) {
            retval += i == 0 ? " " : ", ";
            retval += StringUtility::trim(unparseExpression(operands[i], NULL, RegisterDictionary::Ptr()));
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

bool
SgAsmInstruction::normalizeOperands() {
    bool changed = false;
    for (size_t i = 0; i < nOperands(); ++i) {
        // match (+ (+ reg1 (* reg2 sz)) offset)
        if (auto mre = isSgAsmMemoryReferenceExpression(operand(i))) {
            if (auto add1 = isSgAsmBinaryAdd(mre->get_address())) {
                if (auto add2 = isSgAsmBinaryAdd(add1->get_lhs())) {
                    if (/*auto reg1 = */isSgAsmDirectRegisterExpression(add2->get_lhs())) {
                        if (auto mul = isSgAsmBinaryMultiply(add2->get_rhs())) {
                            if (/*auto reg2 = */isSgAsmDirectRegisterExpression(mul->get_lhs())) {
                                if (/*auto sz = */isSgAsmIntegerValueExpression(mul->get_rhs())) {
                                    if (auto offset = isSgAsmIntegerValueExpression(add1->get_rhs())) {
                                        // Swap the offset and multiplication to result in
                                        //   (+ (+ reg1 offset) (* reg2 sz))

                                        // Remove offset and mul from the tree
                                        add2->set_rhs(nullptr);
                                        add1->set_rhs(nullptr);

                                        // Insert them back in the correct order
                                        add2->set_rhs(offset);
                                        offset->set_parent(add2);
                                        add1->set_rhs(mul);
                                        mul->set_parent(add1);
                                        changed = true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return changed;
}

const SgUnsignedCharList&
SgAsmInstruction::get_raw_bytes() const {
    return get_rawBytes();
}

void
SgAsmInstruction::set_raw_bytes(const SgUnsignedCharList &x) {
    set_rawBytes(x);
}

#endif
