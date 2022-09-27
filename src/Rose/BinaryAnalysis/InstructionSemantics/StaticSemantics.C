#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"
#include <Rose/BinaryAnalysis/InstructionSemantics/StaticSemantics.h>

#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include "stringify.h"

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {
namespace StaticSemantics {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      User-level supporting functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void attachInstructionSemantics(SgNode *ast, const Disassembler::Base::Ptr &disassembler) {
    ASSERT_not_null(ast);
    ASSERT_not_null(disassembler);
    RiscOperators::Ptr ops = RiscOperators::instanceFromProtoval(SValue::instance(), SmtSolver::Ptr());
    BaseSemantics::Dispatcher::Ptr cpu = disassembler->dispatcher();
    if (!cpu)
        throw BaseSemantics::Exception("no instruction semantics for architecture", NULL);
    cpu = cpu->create(ops, 0, RegisterDictionary::Ptr());
    attachInstructionSemantics(ast, cpu);
}

void attachInstructionSemantics(SgNode *ast, const BaseSemantics::Dispatcher::Ptr &cpu) {
    // We cannot use an AST traversal because we'd be modifying the AST while traversing, which is bad.
    std::vector<SgAsmInstruction*> insns = SageInterface::querySubTree<SgAsmInstruction>(ast);
    for (SgAsmInstruction *insn: insns) {
        if (insn->get_semantics()==NULL) {
            try {
                // Side effect is that semanics are attached to the instruction.
                cpu->processInstruction(insn);
            } catch (const BaseSemantics::Exception&) {
                // FIXME[Robb P. Matzke 2015-06-06]: semantics subtree should be deleted, but delete mechanism seems to be
                // broken.
                if (insn->get_semantics())
                    insn->get_semantics()->set_parent(NULL);
                insn->set_semantics(NULL);
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
    Ptr retval = boost::dynamic_pointer_cast<RiscOperators>(x);
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
            semantics->set_parent(insn);
            insn->set_semantics(semantics);
        }

        // We're about to make a copy of the a->ast(), so we need to ensure all the parent pointers are correct.
        struct T1: AstPrePostProcessing {
            std::vector<SgNode*> path;
            void preOrderVisit(SgNode *node) override {
                if (!path.empty())
                    node->set_parent(path.back());
                path.push_back(node);
            }
            void postOrderVisit(SgNode *node) override {
                ASSERT_require(!path.empty());
                ASSERT_require(path.back() == node);
                path.pop_back();
            }
        } t1;
        t1.traverse(a->ast());

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

void
SValue::print(std::ostream &out, BaseSemantics::Formatter &fmt) const {
    if (ast_) {
        struct T1: AstPrePostProcessing {
            std::ostream &out;

            T1(std::ostream &out): out(out) {}
            
            void preOrderVisit(SgNode *node) {
                out <<" (";
                if (SgAsmRiscOperation *riscOp = isSgAsmRiscOperation(node)) {
                    out <<stringifySgAsmRiscOperationRiscOperator(riscOp->get_riscOperator(), "OP_");
                } else {
                    out <<node->class_name();
                }
            }

            void postOrderVisit(SgNode *node) {
                out <<")";
            }
        } t1(out);
        t1.traverse(ast_);
    } else {
        out <<" null";
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      RiscOperators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RiscOperators::startInstruction(SgAsmInstruction *insn) {
    // Make sure any previous semantics are erased before we start.
    if (SgAsmExprListExp *semantics = insn->get_semantics()) {
        for (SgAsmExpression *expr: semantics->get_expressions()) {
            // FIXME[Robb P. Matzke 2015-06-06]: delete subtree mechanism is broken; this is a memory leak
            expr->set_parent(NULL);
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
RiscOperators::readRegister(RegisterDescriptor reg, const BaseSemantics::SValue::Ptr &dflt) {
    BaseSemantics::SValue::Ptr regExpr = makeSValue(reg.nBits(), new SgAsmDirectRegisterExpression(reg));
    return makeSValue(reg.nBits(), SgAsmRiscOperation::OP_readRegister, regExpr);
}

BaseSemantics::SValue::Ptr
RiscOperators::peekRegister(RegisterDescriptor reg, const BaseSemantics::SValue::Ptr &dflt) {
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
