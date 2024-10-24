#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Unparser/X86.h>

#include <Rose/AST/Traversal.h>
#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <stringify.h>

#include <SgAsmFloatType.h>
#include <SgAsmIndirectRegisterExpression.h>
#include <SgAsmIntegerType.h>
#include <SgAsmMemoryReferenceExpression.h>
#include <SgAsmVectorType.h>
#include <SgAsmX86Instruction.h>
#include <Cxx_GrammarDowncast.h>

#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <sstream>

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

X86::~X86() {}

X86::X86(const Architecture::Base::ConstPtr &arch, const X86Settings &settings)
    : Base(arch), settings_(settings) {}

X86::Ptr
X86::instance(const Architecture::Base::ConstPtr &arch, const X86Settings &settings) {
    return Ptr(new X86(arch, settings));
}

Base::Ptr
X86::copy() const {
    return instance(architecture(), settings());
}

void
X86::emitInstructionMnemonic(std::ostream &out, SgAsmInstruction *insn_, State &state) const {
    SgAsmX86Instruction *insn = isSgAsmX86Instruction(insn_);
    ASSERT_not_null2(insn, "not an x86 instruction");

    if (insn->get_lockPrefix())
        out <<"lock ";

    out <<insn->get_mnemonic();

    switch (insn->get_branchPrediction()) {
        case x86_branch_prediction_none: break;
        case x86_branch_prediction_taken: out <<",pt"; break;
        case x86_branch_prediction_not_taken: out <<",pn"; break;
        default:
            ASSERT_not_reachable("bad x86 branch prediction: " +
                                 stringifyBinaryAnalysisX86BranchPrediction(insn->get_branchPrediction()));
    }

    if (insn->semanticFailure() > 0) {
        StyleGuard style(state.styleStack(), settings().insn.mnemonic.semanticFailureStyle);
        out <<style.render() <<settings().insn.mnemonic.semanticFailureMarker <<style.restore();
    }
}

void
X86::emitTypeName(std::ostream &out, SgAsmType *type, State &state) const {
    if (!type)
        return;

    if (SgAsmIntegerType *it = isSgAsmIntegerType(type)) {
        switch (it->get_nBits()) {
            case 8: out <<"byte"; break;
            case 16: out <<"word"; break;
            case 32: out <<"dword"; break;
            case 64: out <<"qword"; break;
        }
    } else if (SgAsmFloatType *ft = isSgAsmFloatType(type)) {
        switch (ft->get_nBits()) {
            case 32: out <<"float"; break;
            case 64: out <<"double"; break;
            case 80: out <<"ldouble"; break;
        }
    } else if (SgAsmVectorType *vt = isSgAsmVectorType(type)) {
        if (2 == vt->get_nElmts() && isSgAsmIntegerType(vt->get_elmtType()) && vt->get_elmtType()->get_nBits() == 64) {
            out <<"dqword";
        } else {
            out <<"v" <<vt->get_nElmts();
            state.frontUnparser().emitTypeName(out, vt->get_elmtType(), state);
        }
    } else {
        ASSERT_not_reachable("unhandled type: " + type->toString());
    }
}

void
X86::emitMemoryReferenceExpression(std::ostream &out, SgAsmMemoryReferenceExpression *expr, State &state) const {
    ASSERT_not_null(expr);
    if (nextUnparser()) {
        nextUnparser()->emitMemoryReferenceExpression(out, expr, state);
    } else {
        SgAsmX86Instruction *insn = AST::Traversal::findParentTyped<SgAsmX86Instruction>(expr);
        const bool isLea = insn && insn->get_kind() == x86_lea;

        if (!isLea) {
            state.frontUnparser().emitTypeName(out, expr->get_type(), state);
            out <<" ";
        }
        if (expr->get_segment()) {
            emitExpression(out, expr->get_segment(), state);
            out <<":";
        }

        out <<"[";
        emitExpression(out, expr->get_address(), state);
        out <<"]";
    }
}

void
X86::emitIndirectRegisterExpression(std::ostream &out, SgAsmIndirectRegisterExpression *expr, State &state) const {
    ASSERT_not_null(expr);
    if (nextUnparser()) {
        nextUnparser()->emitIndirectRegisterExpression(out, expr, state);
    } else {
        std::ostringstream ss;
        state.frontUnparser().emitRegister(ss, expr->get_descriptor(), state);
        std::string s = ss.str();
        if (boost::ends_with(s, "0"))
            boost::erase_tail(s, 1);
        out <<s <<"(" <<expr->get_index() <<")";
    }
}

} // namespace
} // namespace
} // namespace

#endif
