#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"
#include "StaticSemantics2.h"

#include "Disassembler.h"
#include "stringify.h"

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
namespace StaticSemantics {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      User-level supporting functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void attachInstructionSemantics(SgNode *ast, Disassembler *disassembler) {
    ASSERT_not_null(ast);
    ASSERT_not_null(disassembler);
    RiscOperatorsPtr ops = RiscOperators::instance(SValue::instance(), SmtSolverPtr());
    BaseSemantics::DispatcherPtr cpu = disassembler->dispatcher();
    if (!cpu)
        throw BaseSemantics::Exception("no instruction semantics for architecture", NULL);
    cpu = cpu->create(ops);
    attachInstructionSemantics(ast, cpu);
}

void attachInstructionSemantics(SgNode *ast, const BaseSemantics::DispatcherPtr &cpu) {
    // We cannot use an AST traversal because we'd be modifying the AST while traversing, which is bad.
    std::vector<SgAsmInstruction*> insns = SageInterface::querySubTree<SgAsmInstruction>(ast);
    BOOST_FOREACH (SgAsmInstruction *insn, insns) {
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

// Make an SValue for an arbitrary SgAsmExpression subtree
BaseSemantics::SValuePtr
RiscOperators::makeSValue(size_t nbits, SgAsmExpression *expr) {
    ASSERT_not_null(expr);
    SValuePtr v = SValue::promote(protoval()->undefined_(nbits));
    v->ast(expr);
    return v;
}

// Make an SValue for a RISC operator that takes no arguments. The width in bits is therefore required.
BaseSemantics::SValuePtr
RiscOperators::makeSValue(size_t nbits, SgAsmRiscOperation::RiscOperator op) {
    SValuePtr v = SValue::promote(protoval()->undefined_(nbits));
    v->ast(SageBuilderAsm::buildRiscOperation(op));
    return v;
}

// RISC operator that takes one operand.
BaseSemantics::SValuePtr
RiscOperators::makeSValue(size_t nbits, SgAsmRiscOperation::RiscOperator op, const BaseSemantics::SValuePtr &a_) {
    SValuePtr v = SValue::promote(protoval()->undefined_(nbits));
    SValuePtr a = SValue::promote(a_);
    v->ast(SageBuilderAsm::buildRiscOperation(op, a->ast()));
    return v;
}

// RISC operator that takes two operands.
BaseSemantics::SValuePtr
RiscOperators::makeSValue(size_t nbits, SgAsmRiscOperation::RiscOperator op, const BaseSemantics::SValuePtr &a_,
                          const BaseSemantics::SValuePtr &b_) {
    SValuePtr v = SValue::promote(protoval()->undefined_(nbits));
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    v->ast(SageBuilderAsm::buildRiscOperation(op, a->ast(), b->ast()));
    return v;
}

// RISC operator that takes three operands.
BaseSemantics::SValuePtr
RiscOperators::makeSValue(size_t nbits, SgAsmRiscOperation::RiscOperator op, const BaseSemantics::SValuePtr &a_,
                          const BaseSemantics::SValuePtr &b_, const BaseSemantics::SValuePtr &c_) {
    SValuePtr v = SValue::promote(protoval()->undefined_(nbits));
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    SValuePtr c = SValue::promote(c_);
    v->ast(SageBuilderAsm::buildRiscOperation(op, a->ast(), b->ast(), c->ast()));
    return v;
}

// RISC operator that takes four operands.
BaseSemantics::SValuePtr
RiscOperators::makeSValue(size_t nbits, SgAsmRiscOperation::RiscOperator op, const BaseSemantics::SValuePtr &a_,
                          const BaseSemantics::SValuePtr &b_, const BaseSemantics::SValuePtr &c_,
                          const BaseSemantics::SValuePtr &d_) {
    SValuePtr v = SValue::promote(protoval()->undefined_(nbits));
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    SValuePtr c = SValue::promote(c_);
    SValuePtr d = SValue::promote(d_);
    v->ast(SageBuilderAsm::buildRiscOperation(op, a->ast(), b->ast(), c->ast(), d->ast()));
    return v;
}

// Save the semantic effect in the current instruction.
void
RiscOperators::saveSemanticEffect(const BaseSemantics::SValuePtr &a_) {
    if (SgAsmInstruction *insn = currentInstruction()) {
        SValuePtr a = SValue::promote(a_);
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
            void preOrderVisit(SgNode *node) ROSE_OVERRIDE {
                if (!path.empty())
                    node->set_parent(path.back());
                path.push_back(node);
            }
            void postOrderVisit(SgNode *node) ROSE_OVERRIDE {
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
        BOOST_FOREACH (SgAsmExpression *expr, semantics->get_expressions()) {
            // FIXME[Robb P. Matzke 2015-06-06]: delete subtree mechanism is broken; this is a memory leak
            expr->set_parent(NULL);
        }
        semantics->get_expressions().clear();
    }
    BaseSemantics::RiscOperators::startInstruction(insn);
}

BaseSemantics::SValuePtr
RiscOperators::filterCallTarget(const BaseSemantics::SValuePtr &a) {
    return makeSValue(a->get_width(), SgAsmRiscOperation::OP_filterCallTarget, a);
}

BaseSemantics::SValuePtr
RiscOperators::filterReturnTarget(const BaseSemantics::SValuePtr &a) {
    return makeSValue(a->get_width(), SgAsmRiscOperation::OP_filterReturnTarget, a);
}

BaseSemantics::SValuePtr
RiscOperators::filterIndirectJumpTarget(const BaseSemantics::SValuePtr &a) {
    return makeSValue(a->get_width(), SgAsmRiscOperation::OP_filterIndirectJumpTarget, a);
}

void
RiscOperators::hlt() {
    saveSemanticEffect(makeSValue(1 /*arbitrary*/, SgAsmRiscOperation::OP_hlt));
}

void
RiscOperators::cpuid() {
    saveSemanticEffect(makeSValue(1 /*arbitrary*/, SgAsmRiscOperation::OP_cpuid));
}

BaseSemantics::SValuePtr
RiscOperators::rdtsc() {
    return makeSValue(64, SgAsmRiscOperation::OP_rdtsc);
}

BaseSemantics::SValuePtr
RiscOperators::and_(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
    return makeSValue(a->get_width(), SgAsmRiscOperation::OP_and_, a, b);
}

BaseSemantics::SValuePtr
RiscOperators::or_(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
    return makeSValue(a->get_width(), SgAsmRiscOperation::OP_or_, a, b);
}

BaseSemantics::SValuePtr
RiscOperators::xor_(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
    return makeSValue(a->get_width(), SgAsmRiscOperation::OP_xor_, a, b);
}

BaseSemantics::SValuePtr
RiscOperators::invert(const BaseSemantics::SValuePtr &a) {
    return makeSValue(a->get_width(), SgAsmRiscOperation::OP_invert, a);
}

BaseSemantics::SValuePtr
RiscOperators::extract(const BaseSemantics::SValuePtr &a, size_t begin_bit, size_t end_bit) {
    ASSERT_require(end_bit < 256);
    ASSERT_require(begin_bit < end_bit);
    BaseSemantics::SValuePtr beginExpr = makeSValue(8, SageBuilderAsm::buildValueU8(begin_bit));
    BaseSemantics::SValuePtr endExpr   = makeSValue(8, SageBuilderAsm::buildValueU8(end_bit));
    return makeSValue(end_bit-begin_bit, SgAsmRiscOperation::OP_extract, a, beginExpr, endExpr);
}

BaseSemantics::SValuePtr
RiscOperators::concat(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
    return makeSValue(a->get_width()+b->get_width(), SgAsmRiscOperation::OP_concat, a, b);
}

BaseSemantics::SValuePtr
RiscOperators::leastSignificantSetBit(const BaseSemantics::SValuePtr &a) {
    return makeSValue(a->get_width(), SgAsmRiscOperation::OP_leastSignificantSetBit, a);
}

BaseSemantics::SValuePtr
RiscOperators::mostSignificantSetBit(const BaseSemantics::SValuePtr &a) {
    return makeSValue(a->get_width(), SgAsmRiscOperation::OP_mostSignificantSetBit, a);
}

BaseSemantics::SValuePtr
RiscOperators::rotateLeft(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &sa) {
    return makeSValue(a->get_width(), SgAsmRiscOperation::OP_rotateLeft, a, sa);
}

BaseSemantics::SValuePtr
RiscOperators::rotateRight(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &sa) {
    return makeSValue(a->get_width(), SgAsmRiscOperation::OP_rotateRight, a, sa);
}

BaseSemantics::SValuePtr
RiscOperators::shiftLeft(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &sa) {
    return makeSValue(a->get_width(), SgAsmRiscOperation::OP_shiftLeft, a, sa);
}

BaseSemantics::SValuePtr
RiscOperators::shiftRight(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &sa) {
    return makeSValue(a->get_width(), SgAsmRiscOperation::OP_shiftRight, a, sa);
}

BaseSemantics::SValuePtr
RiscOperators::shiftRightArithmetic(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &sa) {
    return makeSValue(a->get_width(), SgAsmRiscOperation::OP_shiftRightArithmetic, a, sa);
}

BaseSemantics::SValuePtr
RiscOperators::equalToZero(const BaseSemantics::SValuePtr &a) {
    return makeSValue(1, SgAsmRiscOperation::OP_equalToZero, a);
}

BaseSemantics::SValuePtr
RiscOperators::ite(const BaseSemantics::SValuePtr &sel, const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
    return makeSValue(a->get_width(), SgAsmRiscOperation::OP_ite, sel, a, b);
}

BaseSemantics::SValuePtr
RiscOperators::isEqual(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
    return makeSValue(1, SgAsmRiscOperation::OP_isEqual, a, b);
}

BaseSemantics::SValuePtr
RiscOperators::isNotEqual(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
    return makeSValue(1, SgAsmRiscOperation::OP_isNotEqual, a, b);
}

BaseSemantics::SValuePtr
RiscOperators::isUnsignedLessThan(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
    return makeSValue(1, SgAsmRiscOperation::OP_isUnsignedLessThan, a, b);
}

BaseSemantics::SValuePtr
RiscOperators::isUnsignedLessThanOrEqual(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
    return makeSValue(1, SgAsmRiscOperation::OP_isUnsignedLessThanOrEqual, a, b);
}

BaseSemantics::SValuePtr
RiscOperators::isUnsignedGreaterThan(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
    return makeSValue(1, SgAsmRiscOperation::OP_isUnsignedGreaterThan, a, b);
}

BaseSemantics::SValuePtr
RiscOperators::isUnsignedGreaterThanOrEqual(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
    return makeSValue(1, SgAsmRiscOperation::OP_isUnsignedGreaterThanOrEqual, a, b);
}

BaseSemantics::SValuePtr
RiscOperators::isSignedLessThan(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
    return makeSValue(1, SgAsmRiscOperation::OP_isSignedLessThan, a, b);
}

BaseSemantics::SValuePtr
RiscOperators::isSignedLessThanOrEqual(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
    return makeSValue(1, SgAsmRiscOperation::OP_isSignedLessThanOrEqual, a, b);
}

BaseSemantics::SValuePtr
RiscOperators::isSignedGreaterThan(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
    return makeSValue(1, SgAsmRiscOperation::OP_isSignedGreaterThan, a, b);
}

BaseSemantics::SValuePtr
RiscOperators::isSignedGreaterThanOrEqual(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
    return makeSValue(1, SgAsmRiscOperation::OP_isSignedGreaterThanOrEqual, a, b);
}

BaseSemantics::SValuePtr
RiscOperators::unsignedExtend(const BaseSemantics::SValuePtr &a, size_t new_width) {
    ASSERT_require(new_width < 256);
    BaseSemantics::SValuePtr widthExpr = makeSValue(8, SageBuilderAsm::buildValueU8(new_width));
    return makeSValue(new_width, SgAsmRiscOperation::OP_unsignedExtend, a, widthExpr);
}

BaseSemantics::SValuePtr
RiscOperators::signExtend(const BaseSemantics::SValuePtr &a, size_t new_width) {
    ASSERT_require(new_width < 256);
    BaseSemantics::SValuePtr widthExpr = makeSValue(8, SageBuilderAsm::buildValueU8(new_width));
    return makeSValue(new_width, SgAsmRiscOperation::OP_signExtend, a, widthExpr);
}

BaseSemantics::SValuePtr
RiscOperators::add(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
    return makeSValue(a->get_width(), SgAsmRiscOperation::OP_add, a, b);
}

BaseSemantics::SValuePtr
RiscOperators::addWithCarries(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b,
                              const BaseSemantics::SValuePtr &c, BaseSemantics::SValuePtr &carry_out/*out*/) {
    carry_out = makeSValue(a->get_width(), SgAsmRiscOperation::OP_addCarries, a, b, c);
    return makeSValue(a->get_width(), SgAsmRiscOperation::OP_add, a, b, c);
}

BaseSemantics::SValuePtr
RiscOperators::subtract(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
    return makeSValue(a->get_width(), SgAsmRiscOperation::OP_subtract, a, b);
}

BaseSemantics::SValuePtr
RiscOperators::negate(const BaseSemantics::SValuePtr &a) {
    return makeSValue(a->get_width(), SgAsmRiscOperation::OP_negate, a);
}

BaseSemantics::SValuePtr
RiscOperators::signedDivide(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
    return makeSValue(a->get_width(), SgAsmRiscOperation::OP_signedDivide, a, b);
}

BaseSemantics::SValuePtr
RiscOperators::signedModulo(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
    return makeSValue(b->get_width(), SgAsmRiscOperation::OP_signedModulo, a, b);
}

BaseSemantics::SValuePtr
RiscOperators::signedMultiply(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
    return makeSValue(a->get_width() + b->get_width(), SgAsmRiscOperation::OP_signedMultiply, a, b);
}

BaseSemantics::SValuePtr
RiscOperators::unsignedDivide(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
    return makeSValue(a->get_width(), SgAsmRiscOperation::OP_unsignedDivide, a, b);
}

BaseSemantics::SValuePtr
RiscOperators::unsignedModulo(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
    return makeSValue(b->get_width(), SgAsmRiscOperation::OP_unsignedModulo, a, b);
}

BaseSemantics::SValuePtr
RiscOperators::unsignedMultiply(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
    return makeSValue(a->get_width() + b->get_width(), SgAsmRiscOperation::OP_unsignedMultiply, a, b);
}

void
RiscOperators::interrupt(int majr, int minr) {
    BaseSemantics::SValuePtr majrExpr = makeSValue(32, SageBuilderAsm::buildValueU32(majr));
    BaseSemantics::SValuePtr minrExpr = makeSValue(32, SageBuilderAsm::buildValueU32(minr));
    saveSemanticEffect(makeSValue(1 /*arbitrary*/, SgAsmRiscOperation::OP_interrupt, majrExpr, minrExpr));
}

BaseSemantics::SValuePtr
RiscOperators::readRegister(RegisterDescriptor reg, const BaseSemantics::SValuePtr &dflt) {
    BaseSemantics::SValuePtr regExpr = makeSValue(reg.nBits(), new SgAsmDirectRegisterExpression(reg));
    return makeSValue(reg.nBits(), SgAsmRiscOperation::OP_readRegister, regExpr);
}

BaseSemantics::SValuePtr
RiscOperators::peekRegister(RegisterDescriptor reg, const BaseSemantics::SValuePtr &dflt) {
    BaseSemantics::SValuePtr regExpr = makeSValue(reg.nBits(), new SgAsmDirectRegisterExpression(reg));
    return makeSValue(reg.nBits(), SgAsmRiscOperation::OP_peekRegister, regExpr);
}

void
RiscOperators::writeRegister(RegisterDescriptor reg, const BaseSemantics::SValuePtr &a) {
    BaseSemantics::SValuePtr regExpr = makeSValue(reg.nBits(), new SgAsmDirectRegisterExpression(reg));
    saveSemanticEffect(makeSValue(1 /*arbitrary*/, SgAsmRiscOperation::OP_writeRegister, regExpr, a)); 
}
        
BaseSemantics::SValuePtr
RiscOperators::readMemory(RegisterDescriptor segreg, const BaseSemantics::SValuePtr &address,
                          const BaseSemantics::SValuePtr &dflt, const BaseSemantics::SValuePtr &cond) {
    if (!segreg.isEmpty()) {
        BaseSemantics::SValuePtr segRegExpr = makeSValue(segreg.nBits(), new SgAsmDirectRegisterExpression(segreg));
        return makeSValue(dflt->get_width(), SgAsmRiscOperation::OP_readMemory, segRegExpr, address, dflt, cond);
    } else {
        return makeSValue(dflt->get_width(), SgAsmRiscOperation::OP_readMemory,             address, dflt, cond);
    }
}

BaseSemantics::SValuePtr
RiscOperators::peekMemory(RegisterDescriptor segreg, const BaseSemantics::SValuePtr &address,
                          const BaseSemantics::SValuePtr &dflt) {
    if (!segreg.isEmpty()) {
        BaseSemantics::SValuePtr segRegExpr = makeSValue(segreg.nBits(), new SgAsmDirectRegisterExpression(segreg));
        return makeSValue(dflt->get_width(), SgAsmRiscOperation::OP_peekMemory, segRegExpr, address, dflt);
    } else {
        return makeSValue(dflt->get_width(), SgAsmRiscOperation::OP_peekMemory,             address, dflt);
    }
}

void
RiscOperators::writeMemory(RegisterDescriptor segreg, const BaseSemantics::SValuePtr &address,
                           const BaseSemantics::SValuePtr &value, const BaseSemantics::SValuePtr &cond) {
    if (!segreg.isEmpty()) {
        BaseSemantics::SValuePtr segRegExpr = makeSValue(segreg.nBits(), new SgAsmDirectRegisterExpression(segreg));
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
