#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/InstructionSemantics/StaticSemantics.h>

#include <Rose/As.h>
#include <Rose/AST/Traversal.h>
#include <Rose/AST/Utility.h>
#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>

#include <SgAsmDirectRegisterExpression.h>
#include <SgAsmInstruction.h>
#include <SgAsmIntegerType.h>
#include <SgAsmIntegerValueExpression.h>
#include <SgAsmExprListExp.h>

#include <Cxx_GrammarDowncast.h>
#include <SageBuilderAsm.h>
#include "stringify.h"

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {
namespace StaticSemantics {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      User-level supporting functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void attachInstructionSemantics(SgNode *ast, const Architecture::Base::ConstPtr &arch) {
    ASSERT_not_null(ast);
    ASSERT_not_null(arch);
    RiscOperators::Ptr ops = RiscOperators::instanceFromProtoval(SValue::instance(), SmtSolver::Ptr());
    BaseSemantics::Dispatcher::Ptr cpu = arch->newInstructionDispatcher(ops);
    if (!cpu)
        throw BaseSemantics::Exception("no instruction semantics for architecture", NULL);
    attachInstructionSemantics(ast, cpu);
}

void attachInstructionSemantics(SgNode *ast, const BaseSemantics::Dispatcher::Ptr &cpu) {
    using namespace Rose::AST::Traversal;
    std::vector<SgAsmInstruction*> insns;
    forwardPre<SgAsmInstruction>(ast, [&insns](SgAsmInstruction *node) {
        insns.push_back(node);
    });

    for (SgAsmInstruction *insn: insns) {
        if (insn->get_semantics()==NULL) {
            try {
                // Side effect is that semanics are attached to the instruction.
                cpu->processInstruction(insn);
            } catch (const BaseSemantics::Exception&) {
                // FIXME[Robb P. Matzke 2015-06-06]: semantics subtree should be deleted, but delete mechanism seems to be
                // broken.
                if (SgAsmNode *tmp = insn->get_semantics()) {
                    insn->set_semantics(nullptr);
                    tmp->set_parent(nullptr);
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Supporting functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RiscOperators::RiscOperators(const BaseSemantics::SValue::Ptr &protoval, const SmtSolver::Ptr &solver)
    : BaseSemantics::RiscOperators(protoval, solver) {
    name("StaticSemantics");
    (void) SValue::promote(protoval); // make sure its dynamic type is a StaticSemantics::SValue
}

RiscOperators::RiscOperators(const BaseSemantics::State::Ptr &state, const SmtSolver::Ptr &solver)
    : BaseSemantics::RiscOperators(state, solver) {
    name("StaticSemantics");
    (void) SValue::promote(state->protoval()); // values must have StaticSemantics::SValue dynamic type
}

RiscOperators::~RiscOperators() {}

RiscOperators::Ptr
RiscOperators::instanceFromRegisters(const RegisterDictionary::Ptr &regdict, const SmtSolver::Ptr &solver) {
    BaseSemantics::SValue::Ptr protoval = SValue::instance();
    BaseSemantics::RegisterState::Ptr registers = RegisterState::instance(protoval, regdict);
    BaseSemantics::MemoryState::Ptr memory = MemoryState::instance(protoval, protoval);
    BaseSemantics::State::Ptr state = State::instance(registers, memory);
    return Ptr(new RiscOperators(state, solver));
}

RiscOperators::Ptr
RiscOperators::instanceFromProtoval(const BaseSemantics::SValue::Ptr &protoval, const SmtSolver::Ptr &solver) {
    return Ptr(new RiscOperators(protoval, solver));
}

RiscOperators::Ptr
RiscOperators::instanceFromState(const BaseSemantics::State::Ptr &state, const SmtSolver::Ptr &solver) {
    return Ptr(new RiscOperators(state, solver));
}

BaseSemantics::RiscOperators::Ptr
RiscOperators::create(const BaseSemantics::SValue::Ptr &protoval, const SmtSolver::Ptr &solver) const {
    return instanceFromProtoval(protoval, solver);
}

BaseSemantics::RiscOperators::Ptr
RiscOperators::create(const BaseSemantics::State::Ptr &state, const SmtSolver::Ptr &solver) const {
    return instanceFromState(state, solver);
}

RiscOperators::Ptr
RiscOperators::promote(const BaseSemantics::RiscOperators::Ptr &x) {
    Ptr retval = as<RiscOperators>(x);
    ASSERT_not_null(retval);
    return retval;
}

// Make an SValue for an arbitrary SgAsmExpression subtree
BaseSemantics::SValue::Ptr
RiscOperators::makeSValue(size_t nbits, SgAsmExpression *expr) {
    ASSERT_not_null(expr);
    SValue::Ptr v = SValue::promote(protoval()->undefined_(nbits));
    v->ast(expr);
    return v;
}

// Make an SValue for a RISC operator that takes no arguments. The width in bits is therefore required.
BaseSemantics::SValue::Ptr
RiscOperators::makeSValue(size_t nbits, SgAsmRiscOperation::RiscOperator op) {
    SValue::Ptr v = SValue::promote(protoval()->undefined_(nbits));
    v->ast(SageBuilderAsm::buildRiscOperation(op));
    return v;
}

// RISC operator that takes one operand.
BaseSemantics::SValue::Ptr
RiscOperators::makeSValue(size_t nbits, SgAsmRiscOperation::RiscOperator op, const BaseSemantics::SValue::Ptr &a_) {
    SValue::Ptr v = SValue::promote(protoval()->undefined_(nbits));
    SValue::Ptr a = SValue::promote(a_);
    v->ast(SageBuilderAsm::buildRiscOperation(op, a->ast()));
    return v;
}

// RISC operator that takes two operands.
BaseSemantics::SValue::Ptr
RiscOperators::makeSValue(size_t nbits, SgAsmRiscOperation::RiscOperator op, const BaseSemantics::SValue::Ptr &a_,
                          const BaseSemantics::SValue::Ptr &b_) {
    SValue::Ptr v = SValue::promote(protoval()->undefined_(nbits));
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    v->ast(SageBuilderAsm::buildRiscOperation(op, a->ast(), b->ast()));
    return v;
}

// RISC operator that takes three operands.
BaseSemantics::SValue::Ptr
RiscOperators::makeSValue(size_t nbits, SgAsmRiscOperation::RiscOperator op, const BaseSemantics::SValue::Ptr &a_,
                          const BaseSemantics::SValue::Ptr &b_, const BaseSemantics::SValue::Ptr &c_) {
    SValue::Ptr v = SValue::promote(protoval()->undefined_(nbits));
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    SValue::Ptr c = SValue::promote(c_);
    v->ast(SageBuilderAsm::buildRiscOperation(op, a->ast(), b->ast(), c->ast()));
    return v;
}

// RISC operator that takes four operands.
BaseSemantics::SValue::Ptr
RiscOperators::makeSValue(size_t nbits, SgAsmRiscOperation::RiscOperator op, const BaseSemantics::SValue::Ptr &a_,
                          const BaseSemantics::SValue::Ptr &b_, const BaseSemantics::SValue::Ptr &c_,
                          const BaseSemantics::SValue::Ptr &d_) {
    SValue::Ptr v = SValue::promote(protoval()->undefined_(nbits));
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    SValue::Ptr c = SValue::promote(c_);
    SValue::Ptr d = SValue::promote(d_);
    v->ast(SageBuilderAsm::buildRiscOperation(op, a->ast(), b->ast(), c->ast(), d->ast()));
    return v;
}

// Save the semantic effect in the current instruction.
void
RiscOperators::saveSemanticEffect(const BaseSemantics::SValue::Ptr &a_) {
    if (SgAsmInstruction *insn = currentInstruction()) {
        SValue::Ptr a = SValue::promote(a_);
        ASSERT_not_null(a->ast());

        // Ensure that the instruction has a place to store semantics
        SgAsmExprListExp *semantics = insn->get_semantics();
        if (NULL == semantics) {
            semantics = new SgAsmExprListExp;
            insn->set_semantics(semantics);
            semantics->set_parent(insn);
        }

        // We're about to make a copy of the a->ast(), so we need to ensure all the parent pointers are correct.
        AST::Utility::repairParentPointers(a->ast());
        AST::Utility::checkParentPointers(a->ast());

        // Deep-copy the SValue's AST because we're about to link it into the real AST.
        SgTreeCopy deep;
        SgAsmExpression *copied = isSgAsmExpression(a->ast()->copy(deep));
        ASSERT_not_null(copied);
        semantics->get_expressions().push_back(copied);
        copied->set_parent(semantics);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      RiscOperators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


SValue::SValue(size_t nbits, SgAsmRiscOperation::RiscOperator op)
    : BaseSemantics::SValue(nbits) {
    static uint64_t nVars = 0;
    ast_ = SageBuilderAsm::buildRiscOperation(op, SageBuilderAsm::buildValueU64(nVars++));
}

SValue::SValue(size_t nbits, uint64_t number)
    : BaseSemantics::SValue(nbits) {
    SgAsmType *type = SgAsmType::registerOrDelete(new SgAsmIntegerType(ByteOrder::ORDER_LSB, nbits, false /*unsigned*/));
    ast_ = SageBuilderAsm::buildValueInteger(number, type);
}

SValue::SValue(const SValue &other)
    : BaseSemantics::SValue(other) {
    SgTreeCopy deep;
    SgNode *copied = other.ast_->copy(deep);
    ASSERT_require(isSgAsmExpression(copied));
    ast_ = isSgAsmExpression(copied);
}

SValue::Ptr
SValue::instance() {
    return SValue::Ptr(new SValue(1, SgAsmRiscOperation::OP_undefined));
}

SValue::Ptr
SValue::instance_bottom(size_t nbits) {
    return SValue::Ptr(new SValue(nbits, SgAsmRiscOperation::OP_bottom));
}

SValue::Ptr
SValue::instance_undefined(size_t nbits) {
    return SValue::Ptr(new SValue(nbits, SgAsmRiscOperation::OP_undefined));
}

SValue::Ptr
SValue::instance_unspecified(size_t nbits) {
    return SValue::Ptr(new SValue(nbits, SgAsmRiscOperation::OP_unspecified));
}

SValue::Ptr
SValue::instance_integer(size_t nbits, uint64_t value) {
    return SValue::Ptr(new SValue(nbits, value));
}

BaseSemantics::SValue::Ptr
SValue::bottom_(size_t nbits) const {
    return instance_bottom(nbits);
}

BaseSemantics::SValue::Ptr
SValue::undefined_(size_t nbits) const {
    return instance_undefined(nbits);
}

BaseSemantics::SValue::Ptr
SValue::unspecified_(size_t nbits) const {
    return instance_unspecified(nbits);
}

BaseSemantics::SValue::Ptr
SValue::number_(size_t nbits, uint64_t value) const {
    return instance_integer(nbits, value);
}

BaseSemantics::SValue::Ptr
SValue::boolean_(bool value) const {
    return instance_integer(1, value ? 1 : 0);
}

BaseSemantics::SValue::Ptr
SValue::copy(size_t new_width) const {
    SValue::Ptr retval(new SValue(*this));
    if (new_width!=0 && new_width!=retval->nBits())
        retval->set_width(new_width);
    return retval;
}

Sawyer::Optional<BaseSemantics::SValuePtr>
SValue::createOptionalMerge(const BaseSemantics::SValue::Ptr&, const BaseSemantics::Merger::Ptr&, const SmtSolver::Ptr&) const {
    throw BaseSemantics::NotImplemented("StaticSemantics is not suitable for dataflow analysis", NULL);
}

SValue::Ptr
SValue::promote(const BaseSemantics::SValue::Ptr &v) {
    SValue::Ptr retval = as<SValue>(v);
    ASSERT_not_null(retval);
    return retval;
}

bool
SValue::may_equal(const BaseSemantics::SValue::Ptr &/*other*/, const SmtSolver::Ptr&) const {
    ASSERT_not_reachable("no implementation necessary");
}

bool
SValue::must_equal(const BaseSemantics::SValue::Ptr &/*other*/, const SmtSolver::Ptr&) const {
    ASSERT_not_reachable("no implementation necessary");
}

void
SValue::set_width(size_t /*nbits*/) {
    ASSERT_not_reachable("no implementation necessary");
}

bool
SValue::isBottom() const {
    return false;
}

bool
SValue::is_number() const {
    return false;
}

uint64_t
SValue::get_number() const {
    ASSERT_not_reachable("no implementation necessary");
}

void
SValue::hash(Combinatorics::Hasher&) const {
    ASSERT_not_reachable("no implementation necessary");
}

void
SValue::print(std::ostream &out, BaseSemantics::Formatter&) const {
    if (ast_) {
        AST::Traversal::forward<SgNode>(ast_, [&out](SgNode *node, const AST::Traversal::Order order) {
            if (AST::Traversal::Order::PRE == order) {
                out <<" (";
                if (SgAsmRiscOperation *riscOp = isSgAsmRiscOperation(node)) {
                    out <<stringifySgAsmRiscOperationRiscOperator(riscOp->get_riscOperator(), "OP_");
                } else {
                    out <<node->class_name();
                }
            } else {
                out <<")";
            }
        });
    } else {
        out <<" null";
    }
}

SgAsmExpression*
SValue::ast() {
    return ast_;
}

void
SValue::ast(SgAsmExpression *x) {
    ASSERT_not_null(x);
    ASSERT_require(x->get_parent() == NULL);
    ast_ = x;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      RiscOperators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RiscOperators::startInstruction(SgAsmInstruction *insn) {
    // Make sure any previous semantics are erased before we start.
    if (SgAsmExprListExp *semantics = insn->get_semantics()) {
        for (size_t i = 0; i < semantics->get_expressions().size(); ++i) {
            if (SgNode *tmp = semantics->get_expressions()[i]) {
                // FIXME[Robb P. Matzke 2015-06-06]: delete subtree mechanism is broken; this is a memory leak
                semantics->get_expressions()[i] = nullptr;
                tmp->set_parent(nullptr);
            }
        }
        semantics->get_expressions().clear();
    }
    BaseSemantics::RiscOperators::startInstruction(insn);
}

BaseSemantics::SValue::Ptr
RiscOperators::filterCallTarget(const BaseSemantics::SValue::Ptr &a) {
    return makeSValue(a->nBits(), SgAsmRiscOperation::OP_filterCallTarget, a);
}

BaseSemantics::SValue::Ptr
RiscOperators::filterReturnTarget(const BaseSemantics::SValue::Ptr &a) {
    return makeSValue(a->nBits(), SgAsmRiscOperation::OP_filterReturnTarget, a);
}

BaseSemantics::SValue::Ptr
RiscOperators::filterIndirectJumpTarget(const BaseSemantics::SValue::Ptr &a) {
    return makeSValue(a->nBits(), SgAsmRiscOperation::OP_filterIndirectJumpTarget, a);
}

void
RiscOperators::hlt() {
    saveSemanticEffect(makeSValue(1 /*arbitrary*/, SgAsmRiscOperation::OP_hlt));
}

void
RiscOperators::cpuid() {
    saveSemanticEffect(makeSValue(1 /*arbitrary*/, SgAsmRiscOperation::OP_cpuid));
}

BaseSemantics::SValue::Ptr
RiscOperators::rdtsc() {
    return makeSValue(64, SgAsmRiscOperation::OP_rdtsc);
}

BaseSemantics::SValue::Ptr
RiscOperators::and_(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b) {
    return makeSValue(a->nBits(), SgAsmRiscOperation::OP_and_, a, b);
}

BaseSemantics::SValue::Ptr
RiscOperators::or_(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b) {
    return makeSValue(a->nBits(), SgAsmRiscOperation::OP_or_, a, b);
}

BaseSemantics::SValue::Ptr
RiscOperators::xor_(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b) {
    return makeSValue(a->nBits(), SgAsmRiscOperation::OP_xor_, a, b);
}

BaseSemantics::SValue::Ptr
RiscOperators::invert(const BaseSemantics::SValue::Ptr &a) {
    return makeSValue(a->nBits(), SgAsmRiscOperation::OP_invert, a);
}

BaseSemantics::SValue::Ptr
RiscOperators::extract(const BaseSemantics::SValue::Ptr &a, size_t begin_bit, size_t end_bit) {
    ASSERT_require(end_bit < 256);
    ASSERT_require(begin_bit < end_bit);
    BaseSemantics::SValue::Ptr beginExpr = makeSValue(8, SageBuilderAsm::buildValueU8(begin_bit));
    BaseSemantics::SValue::Ptr endExpr   = makeSValue(8, SageBuilderAsm::buildValueU8(end_bit));
    return makeSValue(end_bit-begin_bit, SgAsmRiscOperation::OP_extract, a, beginExpr, endExpr);
}

BaseSemantics::SValue::Ptr
RiscOperators::concat(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b) {
    return makeSValue(a->nBits()+b->nBits(), SgAsmRiscOperation::OP_concat, a, b);
}

BaseSemantics::SValue::Ptr
RiscOperators::leastSignificantSetBit(const BaseSemantics::SValue::Ptr &a) {
    return makeSValue(a->nBits(), SgAsmRiscOperation::OP_leastSignificantSetBit, a);
}

BaseSemantics::SValue::Ptr
RiscOperators::mostSignificantSetBit(const BaseSemantics::SValue::Ptr &a) {
    return makeSValue(a->nBits(), SgAsmRiscOperation::OP_mostSignificantSetBit, a);
}

BaseSemantics::SValue::Ptr
RiscOperators::rotateLeft(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &sa) {
    return makeSValue(a->nBits(), SgAsmRiscOperation::OP_rotateLeft, a, sa);
}

BaseSemantics::SValue::Ptr
RiscOperators::rotateRight(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &sa) {
    return makeSValue(a->nBits(), SgAsmRiscOperation::OP_rotateRight, a, sa);
}

BaseSemantics::SValue::Ptr
RiscOperators::shiftLeft(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &sa) {
    return makeSValue(a->nBits(), SgAsmRiscOperation::OP_shiftLeft, a, sa);
}

BaseSemantics::SValue::Ptr
RiscOperators::shiftRight(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &sa) {
    return makeSValue(a->nBits(), SgAsmRiscOperation::OP_shiftRight, a, sa);
}

BaseSemantics::SValue::Ptr
RiscOperators::shiftRightArithmetic(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &sa) {
    return makeSValue(a->nBits(), SgAsmRiscOperation::OP_shiftRightArithmetic, a, sa);
}

BaseSemantics::SValue::Ptr
RiscOperators::equalToZero(const BaseSemantics::SValue::Ptr &a) {
    return makeSValue(1, SgAsmRiscOperation::OP_equalToZero, a);
}

BaseSemantics::SValue::Ptr
RiscOperators::iteWithStatus(const BaseSemantics::SValue::Ptr &sel, const BaseSemantics::SValue::Ptr &a,
                             const BaseSemantics::SValue::Ptr &b, IteStatus &status) {
    status = IteStatus::BOTH;
    return makeSValue(a->nBits(), SgAsmRiscOperation::OP_ite, sel, a, b);
}

BaseSemantics::SValue::Ptr
RiscOperators::isEqual(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b) {
    return makeSValue(1, SgAsmRiscOperation::OP_isEqual, a, b);
}

BaseSemantics::SValue::Ptr
RiscOperators::isNotEqual(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b) {
    return makeSValue(1, SgAsmRiscOperation::OP_isNotEqual, a, b);
}

BaseSemantics::SValue::Ptr
RiscOperators::isUnsignedLessThan(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b) {
    return makeSValue(1, SgAsmRiscOperation::OP_isUnsignedLessThan, a, b);
}

BaseSemantics::SValue::Ptr
RiscOperators::isUnsignedLessThanOrEqual(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b) {
    return makeSValue(1, SgAsmRiscOperation::OP_isUnsignedLessThanOrEqual, a, b);
}

BaseSemantics::SValue::Ptr
RiscOperators::isUnsignedGreaterThan(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b) {
    return makeSValue(1, SgAsmRiscOperation::OP_isUnsignedGreaterThan, a, b);
}

BaseSemantics::SValue::Ptr
RiscOperators::isUnsignedGreaterThanOrEqual(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b) {
    return makeSValue(1, SgAsmRiscOperation::OP_isUnsignedGreaterThanOrEqual, a, b);
}

BaseSemantics::SValue::Ptr
RiscOperators::isSignedLessThan(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b) {
    return makeSValue(1, SgAsmRiscOperation::OP_isSignedLessThan, a, b);
}

BaseSemantics::SValue::Ptr
RiscOperators::isSignedLessThanOrEqual(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b) {
    return makeSValue(1, SgAsmRiscOperation::OP_isSignedLessThanOrEqual, a, b);
}

BaseSemantics::SValue::Ptr
RiscOperators::isSignedGreaterThan(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b) {
    return makeSValue(1, SgAsmRiscOperation::OP_isSignedGreaterThan, a, b);
}

BaseSemantics::SValue::Ptr
RiscOperators::isSignedGreaterThanOrEqual(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b) {
    return makeSValue(1, SgAsmRiscOperation::OP_isSignedGreaterThanOrEqual, a, b);
}

BaseSemantics::SValue::Ptr
RiscOperators::unsignedExtend(const BaseSemantics::SValue::Ptr &a, size_t new_width) {
    ASSERT_require(new_width < 256);
    BaseSemantics::SValue::Ptr widthExpr = makeSValue(8, SageBuilderAsm::buildValueU8(new_width));
    return makeSValue(new_width, SgAsmRiscOperation::OP_unsignedExtend, a, widthExpr);
}

BaseSemantics::SValue::Ptr
RiscOperators::signExtend(const BaseSemantics::SValue::Ptr &a, size_t new_width) {
    ASSERT_require(new_width < 256);
    BaseSemantics::SValue::Ptr widthExpr = makeSValue(8, SageBuilderAsm::buildValueU8(new_width));
    return makeSValue(new_width, SgAsmRiscOperation::OP_signExtend, a, widthExpr);
}

BaseSemantics::SValue::Ptr
RiscOperators::add(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b) {
    return makeSValue(a->nBits(), SgAsmRiscOperation::OP_add, a, b);
}

BaseSemantics::SValue::Ptr
RiscOperators::addWithCarries(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b,
                              const BaseSemantics::SValue::Ptr &c, BaseSemantics::SValue::Ptr &carry_out/*out*/) {
    carry_out = makeSValue(a->nBits(), SgAsmRiscOperation::OP_addCarries, a, b, c);
    return makeSValue(a->nBits(), SgAsmRiscOperation::OP_add, a, b, c);
}

BaseSemantics::SValue::Ptr
RiscOperators::subtract(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b) {
    return makeSValue(a->nBits(), SgAsmRiscOperation::OP_subtract, a, b);
}

BaseSemantics::SValue::Ptr
RiscOperators::negate(const BaseSemantics::SValue::Ptr &a) {
    return makeSValue(a->nBits(), SgAsmRiscOperation::OP_negate, a);
}

BaseSemantics::SValue::Ptr
RiscOperators::signedDivide(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b) {
    return makeSValue(a->nBits(), SgAsmRiscOperation::OP_signedDivide, a, b);
}

BaseSemantics::SValue::Ptr
RiscOperators::signedModulo(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b) {
    return makeSValue(b->nBits(), SgAsmRiscOperation::OP_signedModulo, a, b);
}

BaseSemantics::SValue::Ptr
RiscOperators::signedMultiply(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b) {
    return makeSValue(a->nBits() + b->nBits(), SgAsmRiscOperation::OP_signedMultiply, a, b);
}

BaseSemantics::SValue::Ptr
RiscOperators::unsignedDivide(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b) {
    return makeSValue(a->nBits(), SgAsmRiscOperation::OP_unsignedDivide, a, b);
}

BaseSemantics::SValue::Ptr
RiscOperators::unsignedModulo(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b) {
    return makeSValue(b->nBits(), SgAsmRiscOperation::OP_unsignedModulo, a, b);
}

BaseSemantics::SValue::Ptr
RiscOperators::unsignedMultiply(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b) {
    return makeSValue(a->nBits() + b->nBits(), SgAsmRiscOperation::OP_unsignedMultiply, a, b);
}

void
RiscOperators::interrupt(int majr, int minr) {
    BaseSemantics::SValue::Ptr majrExpr = makeSValue(32, SageBuilderAsm::buildValueU32(majr));
    BaseSemantics::SValue::Ptr minrExpr = makeSValue(32, SageBuilderAsm::buildValueU32(minr));
    saveSemanticEffect(makeSValue(1 /*arbitrary*/, SgAsmRiscOperation::OP_interrupt, majrExpr, minrExpr));
}

BaseSemantics::SValue::Ptr
RiscOperators::readRegister(RegisterDescriptor reg, const BaseSemantics::SValue::Ptr &/*dflt*/) {
    BaseSemantics::SValue::Ptr regExpr = makeSValue(reg.nBits(), new SgAsmDirectRegisterExpression(reg));
    return makeSValue(reg.nBits(), SgAsmRiscOperation::OP_readRegister, regExpr);
}

BaseSemantics::SValue::Ptr
RiscOperators::peekRegister(RegisterDescriptor reg, const BaseSemantics::SValue::Ptr &/*dflt*/) {
    BaseSemantics::SValue::Ptr regExpr = makeSValue(reg.nBits(), new SgAsmDirectRegisterExpression(reg));
    return makeSValue(reg.nBits(), SgAsmRiscOperation::OP_peekRegister, regExpr);
}

void
RiscOperators::writeRegister(RegisterDescriptor reg, const BaseSemantics::SValue::Ptr &a) {
    BaseSemantics::SValue::Ptr regExpr = makeSValue(reg.nBits(), new SgAsmDirectRegisterExpression(reg));
    saveSemanticEffect(makeSValue(1 /*arbitrary*/, SgAsmRiscOperation::OP_writeRegister, regExpr, a)); 
}
        
BaseSemantics::SValue::Ptr
RiscOperators::readMemory(RegisterDescriptor segreg, const BaseSemantics::SValue::Ptr &address,
                          const BaseSemantics::SValue::Ptr &dflt, const BaseSemantics::SValue::Ptr &cond) {
    if (!segreg.isEmpty()) {
        BaseSemantics::SValue::Ptr segRegExpr = makeSValue(segreg.nBits(), new SgAsmDirectRegisterExpression(segreg));
        return makeSValue(dflt->nBits(), SgAsmRiscOperation::OP_readMemory, segRegExpr, address, dflt, cond);
    } else {
        return makeSValue(dflt->nBits(), SgAsmRiscOperation::OP_readMemory,             address, dflt, cond);
    }
}

BaseSemantics::SValue::Ptr
RiscOperators::peekMemory(RegisterDescriptor segreg, const BaseSemantics::SValue::Ptr &address,
                          const BaseSemantics::SValue::Ptr &dflt) {
    if (!segreg.isEmpty()) {
        BaseSemantics::SValue::Ptr segRegExpr = makeSValue(segreg.nBits(), new SgAsmDirectRegisterExpression(segreg));
        return makeSValue(dflt->nBits(), SgAsmRiscOperation::OP_peekMemory, segRegExpr, address, dflt);
    } else {
        return makeSValue(dflt->nBits(), SgAsmRiscOperation::OP_peekMemory,             address, dflt);
    }
}

void
RiscOperators::writeMemory(RegisterDescriptor segreg, const BaseSemantics::SValue::Ptr &address,
                           const BaseSemantics::SValue::Ptr &value, const BaseSemantics::SValue::Ptr &cond) {
    if (!segreg.isEmpty()) {
        BaseSemantics::SValue::Ptr segRegExpr = makeSValue(segreg.nBits(), new SgAsmDirectRegisterExpression(segreg));
        saveSemanticEffect(makeSValue(1 /*arbitrary*/, SgAsmRiscOperation::OP_writeMemory, segRegExpr, address, value, cond));
    } else {
        saveSemanticEffect(makeSValue(1 /*arbitrary*/, SgAsmRiscOperation::OP_writeMemory,             address, value, cond));
    }
}

} // namespace
} // namespace
} // namespace
} // namespace

#endif
