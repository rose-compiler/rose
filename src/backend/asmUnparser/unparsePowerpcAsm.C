/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 5Apr07
 * Decription : unparser
 ****************************************************/

// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "AsmUnparser.h"

#include <iomanip>
#include <boost/lexical_cast.hpp>
#include "integerOps.h"
#include "powerpcInstructionProperties.h"
#include "Registers.h"
#include "Diagnostics.h"

using namespace rose;
using namespace rose::Diagnostics;
using namespace rose::BinaryAnalysis;

/****************************************************
 * resolve expression
 ****************************************************/
static std::string unparsePowerpcRegister(SgAsmInstruction *insn, const RegisterDescriptor &rdesc,
                                          const RegisterDictionary *registers)
{
    if (!registers)
        registers = RegisterDictionary::dictionary_powerpc();
    std::string name = registers->lookup(rdesc);
    if (name.empty())
        name = AsmUnparser::invalid_register(insn, rdesc, registers);
    return name;
}

/* Helper for unparsePowerpcExpression(SgAsmExpression*) */
static std::string unparsePowerpcExpression(SgAsmExpression* expr, const AsmUnparser::LabelMap *labels,
                                            const RegisterDictionary *registers, bool useHex) {
    std::string result = "";
    if (expr == NULL) return "BOGUS:NULL";
    switch (expr->variantT()) {
        case V_SgAsmBinaryAdd:
            result = unparsePowerpcExpression(isSgAsmBinaryExpression(expr)->get_lhs(), labels, registers, false) + " + " +
                     unparsePowerpcExpression(isSgAsmBinaryExpression(expr)->get_rhs(), labels, registers, false);
            break;
        case V_SgAsmMemoryReferenceExpression: {
            SgAsmMemoryReferenceExpression* mr = isSgAsmMemoryReferenceExpression(expr);
            SgAsmExpression* addr = mr->get_address();
            switch (addr->variantT()) {
                case V_SgAsmBinaryAdd: {
                    SgAsmBinaryAdd* a = isSgAsmBinaryAdd(addr);
                    std::string lhs = unparsePowerpcExpression(a->get_lhs(), labels, registers, false);
                    if (isSgAsmValueExpression(a->get_rhs())) {
                        // Sign-extend from 16 bits
                        SgAsmValueExpression *ve = isSgAsmValueExpression(a->get_rhs());
                        ASSERT_not_null(ve);
                        result = boost::lexical_cast<std::string>(
                                   (int64_t)IntegerOps::signExtend<16, 64>(SageInterface::getAsmConstant(ve)));
                        result += "(" + lhs + ")";
                    } else {
                        result = lhs + ", " + unparsePowerpcExpression(a->get_rhs(), labels, registers, false);
                    }
                    break;
                }
                default:
                    result = "(" + unparsePowerpcExpression(addr, labels, registers, false) + ")";
                    break;
            }
            break;
        }
        case V_SgAsmDirectRegisterExpression: {
            SgAsmInstruction *insn = SageInterface::getEnclosingNode<SgAsmInstruction>(expr);
            SgAsmDirectRegisterExpression* rr = isSgAsmDirectRegisterExpression(expr);
            result = unparsePowerpcRegister(insn, rr->get_descriptor(), registers);
            break;
        }
        case V_SgAsmIntegerValueExpression: {
            uint64_t v = isSgAsmIntegerValueExpression(expr)->get_absoluteValue();
            if (useHex) {
                result = StringUtility::intToHex(v);
            } else {
                result = StringUtility::numberToString(v);
            }
            if (expr->get_comment().empty() && labels) {
                AsmUnparser::LabelMap::const_iterator li = labels->find(v);
                if (li!=labels->end())
                    result = StringUtility::appendAsmComment(result, li->second);
            }
            break;
        }
        default: {
            ASSERT_not_reachable("invalid PowerPC expression: " + expr->class_name());
        }
    }
    result = StringUtility::appendAsmComment(result, expr->get_replacement());
    result = StringUtility::appendAsmComment(result, expr->get_comment());
    return result;
}

/** Returns a string for the part of the assembly instruction before the first operand. */
std::string unparsePowerpcMnemonic(SgAsmPowerpcInstruction *insn) {
    ASSERT_not_null(insn);
    return insn->get_mnemonic();
}

/** Returns the string representation of an instruction operand. */
std::string unparsePowerpcExpression(SgAsmExpression *expr, const AsmUnparser::LabelMap *labels,
                                     const RegisterDictionary *registers) {
    /* Find the instruction with which this expression is associated. */
    SgAsmPowerpcInstruction *insn = NULL;
    for (SgNode *node=expr; !insn && node; node=node->get_parent()) {
        insn = isSgAsmPowerpcInstruction(node);
    }
    ASSERT_not_null(insn);

    PowerpcInstructionKind kind = insn->get_kind();
    bool isBranchTarget = (((kind == powerpc_b ||
                             kind == powerpc_bl ||
                             kind == powerpc_ba ||
                             kind == powerpc_bla) &&
                            expr==insn->get_operandList()->get_operands()[0]) ||
                           ((kind == powerpc_bc ||
                             kind == powerpc_bcl ||
                             kind == powerpc_bca ||
                             kind == powerpc_bcla) &&
                            insn->get_operandList()->get_operands().size()>=3 &&
                            expr==insn->get_operandList()->get_operands()[2]));
    return unparsePowerpcExpression(expr, labels, registers, isBranchTarget);
}
