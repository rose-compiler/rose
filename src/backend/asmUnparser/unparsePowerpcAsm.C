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


/****************************************************
 * resolve expression
 ****************************************************/
static std::string unparsePowerpcRegister(const RegisterDescriptor &rdesc)
{
    const RegisterDictionary *rdict = RegisterDictionary::dictionary_powerpc();
    std::string name = rdict->lookup(rdesc);
    if (name.empty()) {
        std::cerr <<"unparsePowerpcRegister(" <<rdesc <<"): register descriptor not found in dictionary.\n";
        //std::cerr <<rdict;
        ROSE_ASSERT(!"register descriptor not found in dictionary");
    }
    return name;
}

/* Helper for unparsePowerpcExpression(SgAsmExpression*) */
static std::string unparsePowerpcExpression(SgAsmExpression* expr, const AsmUnparser::LabelMap *labels, bool useHex) {
    std::string result = "";
    if (expr == NULL) return "BOGUS:NULL";
    switch (expr->variantT()) {
        case V_SgAsmBinaryAdd:
            result = unparsePowerpcExpression(isSgAsmBinaryExpression(expr)->get_lhs(), labels, false) + " + " +
                     unparsePowerpcExpression(isSgAsmBinaryExpression(expr)->get_rhs(), labels, false);
            break;
        case V_SgAsmMemoryReferenceExpression: {
            SgAsmMemoryReferenceExpression* mr = isSgAsmMemoryReferenceExpression(expr);
            SgAsmExpression* addr = mr->get_address();
            switch (addr->variantT()) {
                case V_SgAsmBinaryAdd: {
                    SgAsmBinaryAdd* a = isSgAsmBinaryAdd(addr);
                    std::string lhs = unparsePowerpcExpression(a->get_lhs(), labels, false);
                    if (isSgAsmValueExpression(a->get_rhs())) {
                        // Sign-extend from 16 bits
                        result = boost::lexical_cast<std::string>(
                                   (int64_t)IntegerOps::signExtend<16, 64>(
                                      SageInterface::getAsmConstant(isSgAsmValueExpression(a->get_rhs()))));
                        result += "(" + lhs + ")";
                    } else {
                        result = lhs + ", " + unparsePowerpcExpression(a->get_rhs(), labels, false);
                    }
                    break;
                }
                default:
                    result = "(" + unparsePowerpcExpression(addr, labels, false) + ")";
                    break;
            }
            break;
        }
        case V_SgAsmPowerpcRegisterReferenceExpression: {
            SgAsmPowerpcRegisterReferenceExpression* rr = isSgAsmPowerpcRegisterReferenceExpression(expr);
            result = unparsePowerpcRegister(rr->get_descriptor());
            break;
        }
        case V_SgAsmByteValueExpression:
        case V_SgAsmWordValueExpression:
        case V_SgAsmDoubleWordValueExpression:
        case V_SgAsmQuadWordValueExpression: {
            uint64_t v = SageInterface::getAsmConstant(isSgAsmValueExpression(expr));
            if (useHex) {
                result = StringUtility::intToHex(v);
            } else {
                result = StringUtility::numberToString(v);
            }
            if (labels) {
                AsmUnparser::LabelMap::const_iterator li = labels->find(v);
                if (li!=labels->end())
                    result += "<" + li->second + ">";
            }
            break;
        }
        default: {
            std::cerr << "Unhandled expression kind " << expr->class_name() << std::endl;
            ROSE_ASSERT (false);
        }
    }
    if (expr->get_replacement() != "") {
        result += " <" + expr->get_replacement() + ">";
    }
    return result;
}

/** Returns a string for the part of the assembly instruction before the first operand. */
std::string unparsePowerpcMnemonic(SgAsmPowerpcInstruction *insn) {
    ROSE_ASSERT(insn!=NULL);
    return insn->get_mnemonic();
}

/** Returns the string representation of an instruction operand. */
std::string unparsePowerpcExpression(SgAsmExpression *expr, const AsmUnparser::LabelMap *labels) {
    /* Find the instruction with which this expression is associated. */
    SgAsmPowerpcInstruction *insn = NULL;
    for (SgNode *node=expr; !insn && node; node=node->get_parent()) {
        insn = isSgAsmPowerpcInstruction(node);
    }
    ROSE_ASSERT(insn!=NULL);

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
    return unparsePowerpcExpression(expr, labels, isBranchTarget);
}
