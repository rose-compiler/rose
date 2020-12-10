#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"

#include "Registers.h"
#include "AsmUnparser.h"
#include "Diagnostics.h"
#include "stringify.h"

#include <iomanip>

using namespace Rose;
using namespace Diagnostics;
using namespace BinaryAnalysis;

/** Returns a string containing everthing before the first operand in a typical x86 assembly statement. */
std::string unparseX86Mnemonic(SgAsmX86Instruction *insn) {
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

/** Returns the name of an X86 register.
 *
 *  We use the amd64 architecture if no register dictionary is specified because, since it's backward compatible with the 8086,
 *  it contains definitions for all the registers from older architectures. */
std::string unparseX86Register(SgAsmInstruction *insn, RegisterDescriptor reg, const RegisterDictionary *registers) {
    if (!registers)
        registers = RegisterDictionary::dictionary_amd64();
    std::string name = registers->lookup(reg);
    if (name.empty())
        name = AsmUnparser::invalid_register(insn, reg, registers);
    return name;
}

std::string unparseX86Register(RegisterDescriptor reg, const RegisterDictionary *registers) {
    return unparseX86Register(NULL, reg, registers);
}

static std::string x86ValToLabel(uint64_t val, const AsmUnparser::LabelMap *labels)
{
    if (!val || !labels)
        return "";
    
    AsmUnparser::LabelMap::const_iterator li = labels->find(val);
    if (li==labels->end())
        return "";

    return li->second;
}

static std::string x86TypeToPtrName(SgAsmType* ty) {
    if (NULL==ty) {
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
    } else if (ty == SageBuilderAsm::buildTypeVector(2, SageBuilderAsm::buildTypeU64())) {
        return "dqword";
    } else if (SgAsmVectorType *vt = isSgAsmVectorType(ty)) {
        return "v" + StringUtility::numberToString(vt->get_nElmts()) + x86TypeToPtrName(vt->get_elmtType());
    }
    ASSERT_not_reachable("unhandled type: " + ty->toString());
}

std::string unparseX86Expression(SgAsmExpression *expr, const AsmUnparser::LabelMap *labels,
                                 const RegisterDictionary *registers, bool leaMode) {
    std::string result = "";
    if (expr == NULL) return "BOGUS:NULL";

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
            SgAsmInstruction *insn = SageInterface::getEnclosingNode<SgAsmInstruction>(expr);
            SgAsmDirectRegisterExpression* rr = isSgAsmDirectRegisterExpression(expr);
            result = unparseX86Register(insn, rr->get_descriptor(), registers);
            break;
        }

        case V_SgAsmIndirectRegisterExpression: {
            SgAsmInstruction *insn = SageInterface::getEnclosingNode<SgAsmInstruction>(expr);
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
std::string unparseX86Expression(SgAsmExpression *expr, const AsmUnparser::LabelMap *labels,
                                 const RegisterDictionary *registers) {
    /* Find the instruction with which this expression is associated. */
    SgAsmX86Instruction *insn = NULL;
    for (SgNode *node=expr; !insn && node; node=node->get_parent()) {
        insn = isSgAsmX86Instruction(node);
    }
    ASSERT_not_null(insn);
    return unparseX86Expression(expr, labels, registers, insn->get_kind()==x86_lea);
}

#endif
