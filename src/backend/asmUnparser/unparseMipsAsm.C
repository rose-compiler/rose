#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"

#include "Registers.h"
#include "AsmUnparser.h"
#include "Diagnostics.h"

#include <iomanip>

using namespace Rose;
using namespace Rose::Diagnostics;
using namespace Rose::BinaryAnalysis;

/** Returns a string containing everthing before the first operand in a typical x86 assembly statement. */
std::string unparseMipsMnemonic(SgAsmMipsInstruction *insn) {
    ASSERT_not_null(insn);
    return insn->get_mnemonic();
}

/** Returns the name of a MIPS register.
 *
 * FIXME: This assumes MIPS32 */
std::string unparseMipsRegister(SgAsmInstruction *insn, RegisterDescriptor reg, const RegisterDictionary *registers) {
    using namespace StringUtility;
    if (!registers)
        registers = RegisterDictionary::dictionary_mips32();
    std::string name = registers->lookup(reg);
    if (name.empty())
        name = AsmUnparser::invalid_register(insn, reg, registers);
    return name;
}

static std::string mipsValToLabel(uint64_t val, const AsmUnparser::LabelMap *labels)
{
    if (!val || !labels)
        return "";
    
    AsmUnparser::LabelMap::const_iterator li = labels->find(val);
    if (li==labels->end())
        return "";

    return li->second;
}

static std::string mipsTypeToPtrName(SgAsmType* ty) {
    if (NULL==ty) {
        mlog[ERROR] <<"mipsTypeToPtrName: null type\n";
        return "BAD_TYPE";
    }

    if (SgAsmIntegerType *it = isSgAsmIntegerType(ty)) {
        switch (it->get_nBits()) {
            case 8: return "BYTE";
            case 16: return "HALFWORD";
            case 32: return "WORD";
            case 64: return "DOUBLEWORD";
        }
    }
    ASSERT_not_reachable("invalid MIPS type: " + ty->toString());
}


std::string unparseMipsExpression(SgAsmExpression *expr, const AsmUnparser::LabelMap *labels,
                                  const RegisterDictionary *registers) {
    std::string result = "";
    if (expr == NULL) return "BOGUS:NULL";

    switch (expr->variantT()) {
        case V_SgAsmBinaryAdd:
            result = unparseMipsExpression(isSgAsmBinaryExpression(expr)->get_lhs(), labels, registers) + " + " +
                     unparseMipsExpression(isSgAsmBinaryExpression(expr)->get_rhs(), labels, registers);
            break;

        case V_SgAsmMemoryReferenceExpression: {
            SgAsmMemoryReferenceExpression* mr = isSgAsmMemoryReferenceExpression(expr);
            result = mipsTypeToPtrName(mr->get_type()) + " [" +
                     unparseMipsExpression(mr->get_address(), labels, registers) + "]";
            break;
        }

        case V_SgAsmDirectRegisterExpression: {
            SgAsmInstruction *insn = SageInterface::getEnclosingNode<SgAsmInstruction>(expr);
            SgAsmDirectRegisterExpression* rr = isSgAsmDirectRegisterExpression(expr);
            result = unparseMipsRegister(insn, rr->get_descriptor(), registers);
            break;
        }

        case V_SgAsmIntegerValueExpression: {
            SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(expr);
            ASSERT_not_null(ival);
            uint64_t value = ival->get_absoluteValue(); // not sign extended
            result = StringUtility::signedToHex2(value, ival->get_significantBits());

            // Optional label.  Prefer a label supplied by the caller's LabelMap, but not for single-byte constants.  If
            // there's no caller-supplied label, then consider whether the value expression is relative to some other IR node.
            if (expr->get_comment().empty()) {
                std::string label;
                if (ival->get_significantBits()>8)
                    label = mipsValToLabel(value, labels);
                if (label.empty())
                    label = ival->get_label();
                result = StringUtility::appendAsmComment(result, label);
            }
            break;
        }

        default: {
            ASSERT_not_reachable("invalid MIPS expression: " + expr->class_name());
        }
    }

    result = StringUtility::appendAsmComment(result, expr->get_comment());
    return result;
}

#endif
