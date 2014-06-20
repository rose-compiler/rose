#include "sage3basic.h"
#include "Registers.h"
#include "AsmUnparser.h"
#include "Diagnostics.h"
#include "stringify.h"

#include <iomanip>

using namespace rose;                                   // temporary until this API lives in the "rose" name space
using namespace rose::Diagnostics;

/** Returns a string containing everthing before the first operand in a typical x86 assembly statement. */
std::string unparseX86Mnemonic(SgAsmx86Instruction *insn) {
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
            ASSERT_not_reachable("bad x86 branch prediction: " + stringifyX86BranchPrediction(insn->get_branchPrediction()));
    }
    return result;
}

/** Returns the name of an X86 register.
 *
 *  We use the amd64 architecture if no register dictionary is specified because, since it's backward compatible with the 8086,
 *  it contains definitions for all the registers from older architectures. */
std::string unparseX86Register(const RegisterDescriptor &reg, const RegisterDictionary *registers) {
    using namespace StringUtility;
    if (!registers)
        registers = RegisterDictionary::dictionary_amd64();
    std::string name = registers->lookup(reg);
    if (name.empty()) {
        static bool dumped_dict = false;
        Stream warn(Diagnostics::log[WARN]);
        warn <<"unparseX86Register(" <<reg <<"): warning: register descriptor not found in dictionary.\n";
        if (!dumped_dict) {
            warn <<"  This warning is caused by instructions using registers that don't have names in the\n"
                 <<"  register dictionary.  The register dictionary used during unparsing comes from either\n"
                 <<"  the explicitly specified dictionary (see AsmUnparser::set_registers()) or the dictionary\n"
                 <<"  associated with the SgAsmInterpretation being unparsed.  The interpretation normally\n"
                 <<"  chooses a dictionary based on the architecture specified in the file header. For example,\n"
                 <<"  this warning may be caused by a file whose header specifies i386 but the instructions in\n"
                 <<"  the file are for the amd64 architecture.  The assembly listing will indicate unnamed\n"
                 <<"  registers with the notation \"BAD_REGISTER(a.b.c.d)\" where \"a\" and \"b\" are the major\n"
                 <<"  and minor numbers for the register, \"c\" is the bit offset within the underlying machine\n"
                 <<"  register, and \"d\" is the number of significant bits.\n";
            dumped_dict = true;
        }
        return (std::string("BAD_REGISTER(") +
                numberToString(reg.get_major()) + "." +
                numberToString(reg.get_minor()) + "." +
                numberToString(reg.get_offset()) + "." +
                numberToString(reg.get_nbits()) + ")");
    }
    return name;
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
        Diagnostics::log[ERROR] <<"x86TypeToPtrName: null type\n";
        return "BAD_TYPE";
    }

    switch (ty->variantT()) {
        case V_SgAsmTypeByte: return "BYTE";
        case V_SgAsmTypeWord: return "WORD";
        case V_SgAsmTypeDoubleWord: return "DWORD";
        case V_SgAsmTypeQuadWord: return "QWORD";
        case V_SgAsmTypeDoubleQuadWord: return "DQWORD";
        case V_SgAsmTypeSingleFloat: return "FLOAT";
        case V_SgAsmTypeDoubleFloat: return "DOUBLE";
        case V_SgAsmType80bitFloat: return "LDOUBLE";
        case V_SgAsmTypeVector: {
            SgAsmTypeVector* v = isSgAsmTypeVector(ty);
            return "V" + StringUtility::numberToString(v->get_elementCount()) + x86TypeToPtrName(v->get_elementType());
        }
        default: {
            ASSERT_not_reachable("bad class " + ty->class_name());
            return "error in x86TypeToPtrName()";// DQ (11/29/2009): Avoid MSVC warning.
        }
    }
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
                result += x86TypeToPtrName(mr->get_type()) + " PTR " +
                          (mr->get_segment() ? unparseX86Expression(mr->get_segment(), labels, registers, false) + ":" : "");
            }
            result += "[" + unparseX86Expression(mr->get_address(), labels, registers, false) + "]";
            break;
        }

        case V_SgAsmx86RegisterReferenceExpression: {
            SgAsmx86RegisterReferenceExpression* rr = isSgAsmx86RegisterReferenceExpression(expr);
            result = unparseX86Register(rr->get_descriptor(), registers);
            break;
        }

        case V_SgAsmIntegerValueExpression: {
            SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(expr);
            ASSERT_not_null(ival);
            uint64_t value = ival->get_absolute_value(); // not sign extended

            // If the value looks like it might be an address, then don't bother showing the decimal form.
            if ((32==ival->get_significant_bits() || 64==ival->get_significant_bits()) &&
                value > 0x0000ffff && value < 0xffff0000) {
                result = StringUtility::addrToString(value, ival->get_significant_bits());
            } else {
                result = StringUtility::signedToHex2(value, ival->get_significant_bits());
            }

            // Optional label.  Prefer a label supplied by the caller's LabelMap, but not for single-byte constants.  If
            // there's no caller-supplied label, then consider whether the value expression is relative to some other IR node.
            std::string label;
            if (ival->get_significant_bits()>8)
                label =x86ValToLabel(value, labels);
            if (label.empty())
                label = ival->get_label();
            result = StringUtility::appendAsmComment(result, label);
            break;
        }

        default: {
            ASSERT_not_reachable("invalid x86 expression: " + expr->class_name());
        }
    }

    result = StringUtility::appendAsmComment(result, expr->get_replacement());
    return result;
}

/** Returns a string containing the specified operand. */
std::string unparseX86Expression(SgAsmExpression *expr, const AsmUnparser::LabelMap *labels,
                                 const RegisterDictionary *registers) {
    /* Find the instruction with which this expression is associated. */
    SgAsmx86Instruction *insn = NULL;
    for (SgNode *node=expr; !insn && node; node=node->get_parent()) {
        insn = isSgAsmx86Instruction(node);
    }
    ASSERT_not_null(insn);
    return unparseX86Expression(expr, labels, registers, insn->get_kind()==x86_lea);
}
