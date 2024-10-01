#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Unparser/X86.h>

#include <Rose/AST/Traversal.h>
#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>

#include <SgAsmBinaryAdd.h>
#include <SgAsmBinaryMultiply.h>
#include <SgAsmBinarySubtract.h>
#include <SgAsmDirectRegisterExpression.h>
#include <SgAsmFloatType.h>
#include <SgAsmIndirectRegisterExpression.h>
#include <SgAsmIntegerType.h>
#include <SgAsmIntegerValueExpression.h>
#include <SgAsmMemoryReferenceExpression.h>
#include <SgAsmVectorType.h>
#include <SgAsmX86Instruction.h>

#include <Cxx_GrammarDowncast.h>

#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <SageBuilderAsm.h>
#include <stringify.h>

using namespace Sawyer::Message::Common;

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Supporting functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string
unparseX86Mnemonic(SgAsmX86Instruction *insn) {
    ASSERT_not_null(insn);
    std::string result;
    if (insn->get_lockPrefix())
        result += "lock ";
    result += insn->get_mnemonic();
    switch (insn->get_branchPrediction()) {
        case x86_branch_prediction_none: break;
        case x86_branch_prediction_taken: result += ",pt"; break;
        case x86_branch_prediction_not_taken: result += ",pn"; break;
        default:
            ASSERT_not_reachable("bad x86 branch prediction: " +
                                 stringifyBinaryAnalysisX86BranchPrediction(insn->get_branchPrediction()));
    }
    return result;
}

// We use the amd64 architecture if no register dictionary is specified because, since it's backward compatible with the 8086,
// it contains definitions for all the registers from older architectures.
std::string
unparseX86Register(SgAsmInstruction *insn, RegisterDescriptor reg, RegisterDictionary::Ptr registers) {
    if (!registers)
        registers = Architecture::findByName("amd64").orThrow()->registerDictionary();
    std::string name = registers->lookup(reg);
    if (name.empty())
        name = invalidRegister(insn, reg, registers);
    return name;
}

std::string
unparseX86Register(RegisterDescriptor reg, const RegisterDictionary::Ptr &registers) {
    return unparseX86Register(nullptr, reg, registers);
}

static std::string
x86ValToLabel(uint64_t val, const LabelMap *labels) {
    if (!val || !labels)
        return "";

    LabelMap::const_iterator li = labels->find(val);
    if (li==labels->end())
        return "";

    return li->second;
}

static std::string
x86TypeToPtrName(SgAsmType* ty) {
    if (nullptr==ty) {
        mlog[ERROR] <<"x86TypeToPtrName: null type\n";
        return "BAD_TYPE";
    }

    if (SgAsmIntegerType *it = isSgAsmIntegerType(ty)) {
        switch (it->get_nBits()) {
            case 8: return "byte";
            case 16: return "word";
            case 32: return "dword";
            case 64: return "qword";
        }
    } else if (SgAsmFloatType *ft = isSgAsmFloatType(ty)) {
        switch (ft->get_nBits()) {
            case 32: return "float";
            case 64: return "double";
            case 80: return "ldouble";
        }
    } else if (SgAsmVectorType *vt = isSgAsmVectorType(ty)) {
        if (2 == vt->get_nElmts() && isSgAsmIntegerType(vt->get_elmtType()) && vt->get_elmtType()->get_nBits() == 64) {
            return "dqword";
        } else {
            return "v" + StringUtility::numberToString(vt->get_nElmts()) + x86TypeToPtrName(vt->get_elmtType());
        }
    }
    ASSERT_not_reachable("unhandled type: " + ty->toString());
}

std::string
unparseX86Expression(SgAsmExpression *expr, const LabelMap *labels, const RegisterDictionary::Ptr &registers, bool leaMode) {
    std::string result = "";
    if (expr == nullptr) return "BOGUS:NULL";

    switch (expr->variantT()) {
        case V_SgAsmBinaryAdd:
            result = unparseX86Expression(isSgAsmBinaryExpression(expr)->get_lhs(), labels, registers, false) + " + " +
                     unparseX86Expression(isSgAsmBinaryExpression(expr)->get_rhs(), labels, registers, false);
            break;

        case V_SgAsmBinarySubtract:
            result = unparseX86Expression(isSgAsmBinaryExpression(expr)->get_lhs(), labels, registers, false) + " - " +
                     unparseX86Expression(isSgAsmBinaryExpression(expr)->get_rhs(), labels, registers, false);
            break;

        case V_SgAsmBinaryMultiply:
            result = unparseX86Expression(isSgAsmBinaryExpression(expr)->get_lhs(), labels, registers, false) + "*" +
                     unparseX86Expression(isSgAsmBinaryExpression(expr)->get_rhs(), labels, registers, false);
            break;

        case V_SgAsmMemoryReferenceExpression: {
            SgAsmMemoryReferenceExpression* mr = isSgAsmMemoryReferenceExpression(expr);
            if (!leaMode) {
                result += x86TypeToPtrName(mr->get_type()) + " " +
                          (mr->get_segment() ? unparseX86Expression(mr->get_segment(), labels, registers, false) + ":" : "");
            }
            result += "[" + unparseX86Expression(mr->get_address(), labels, registers, false) + "]";
            break;
        }

        case V_SgAsmDirectRegisterExpression: {
            SgAsmInstruction *insn = AST::Traversal::findParentTyped<SgAsmInstruction>(expr);
            SgAsmDirectRegisterExpression* rr = isSgAsmDirectRegisterExpression(expr);
            result = unparseX86Register(insn, rr->get_descriptor(), registers);
            break;
        }

        case V_SgAsmIndirectRegisterExpression: {
            SgAsmInstruction *insn = AST::Traversal::findParentTyped<SgAsmInstruction>(expr);
            SgAsmIndirectRegisterExpression* rr = isSgAsmIndirectRegisterExpression(expr);
            result = unparseX86Register(insn, rr->get_descriptor(), registers);
            if (!result.empty() && '0'==result[result.size()-1])
                result = result.substr(0, result.size()-1);
            result += "(" + StringUtility::numberToString(rr->get_index()) + ")";
            break;
        }

        case V_SgAsmIntegerValueExpression: {
            SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(expr);
            ASSERT_not_null(ival);
            uint64_t value = ival->get_absoluteValue(); // not sign extended

            // If the value looks like it might be an address, then don't bother showing the decimal form.
            if ((32==ival->get_significantBits() || 64==ival->get_significantBits()) &&
                value > 0x0000ffff && value < 0xffff0000) {
                result = StringUtility::addrToString(value, ival->get_significantBits());
            } else {
                result = StringUtility::signedToHex2(value, ival->get_significantBits());
            }

            // Optional label.  Prefer a label supplied by the caller's LabelMap, but not for single-byte constants.  If
            // there's no caller-supplied label, then consider whether the value expression is relative to some other IR node.
            if (expr->get_comment().empty()) {
                std::string label;
                if (label.empty() && ival->get_significantBits()>8)
                    label =x86ValToLabel(value, labels);
                if (label.empty())
                    label = ival->get_label();
                result = StringUtility::appendAsmComment(result, label);
            }
            break;
        }

        default: {
            ASSERT_not_reachable("invalid x86 expression: " + expr->class_name());
        }
    }

    result = StringUtility::appendAsmComment(result, expr->get_comment());
    return result;
}

/** Returns a string containing the specified operand. */
std::string
unparseX86Expression(SgAsmExpression *expr, const LabelMap *labels, const RegisterDictionary::Ptr &registers) {
    /* Find the instruction with which this expression is associated. */
    SgAsmX86Instruction *insn = nullptr;
    for (SgNode *node=expr; !insn && node; node=node->get_parent()) {
        insn = isSgAsmX86Instruction(node);
    }
    ASSERT_not_null(insn);
    return unparseX86Expression(expr, labels, registers, insn->get_kind()==x86_lea);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// X86
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
X86::emitInstructionMnemonic(std::ostream &out, SgAsmInstruction *insn_, State&) const {
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
X86::outputExpr(std::ostream &out, SgAsmExpression *expr, State &state) const {
    ASSERT_not_null(expr);
    std::vector<std::string> comments;

    if (SgAsmBinaryExpression *bin = isSgAsmBinaryExpression(expr)) {
        outputExpr(out, bin->get_lhs(), state);
        switch (expr->variantT()) {
            case V_SgAsmBinaryAdd:      out <<" + "; break;
            case V_SgAsmBinarySubtract: out <<" - "; break;
            case V_SgAsmBinaryMultiply: out <<" * "; break;
            default:
                ASSERT_not_implemented(expr->class_name());
        }
        outputExpr(out, bin->get_rhs(), state);

    } else if (SgAsmMemoryReferenceExpression *mr = isSgAsmMemoryReferenceExpression(expr)) {
        SgAsmX86Instruction *insn = AST::Traversal::findParentTyped<SgAsmX86Instruction>(expr);
        bool isLea = insn != nullptr && insn->get_kind() == x86_lea;
        if (!isLea) {
            state.frontUnparser().emitTypeName(out, mr->get_type(), state);
            out <<" ";
        }
        if (mr->get_segment()) {
            outputExpr(out, mr->get_segment(), state);
            out <<":";
        }
        out <<"[";
        outputExpr(out, mr->get_address(), state);
        out <<"]";

    } else if (SgAsmDirectRegisterExpression *rr = isSgAsmDirectRegisterExpression(expr)) {
        state.frontUnparser().emitRegister(out, rr->get_descriptor(), state);

    } else if (SgAsmIndirectRegisterExpression *rr = isSgAsmIndirectRegisterExpression(expr)) {
        std::ostringstream ss;
        state.frontUnparser().emitRegister(ss, rr->get_descriptor(), state);
        std::string s = ss.str();
        if (boost::ends_with(s, "0"))
            boost::erase_tail(s, 1);
        out <<s <<"(" <<rr->get_index() <<")";

    } else if (SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(expr)) {
        comments = state.frontUnparser().emitSignedInteger(out, ival->get_bitVector(), state);

    } else {
        ASSERT_not_implemented("invalid x86 expression: " + expr->class_name());
    }

    if (!expr->get_comment().empty())
        comments.push_back(expr->get_comment());
    if (!comments.empty())
        out <<"<" + boost::join(comments, ",") <<">";
}

void
X86::emitOperandBody(std::ostream &out, SgAsmExpression *expr, State &state) const {
    outputExpr(out, expr, state);
}

} // namespace
} // namespace
} // namespace

#endif
