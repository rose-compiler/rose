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

using namespace rose;                                   // temporary until this API lives in the "rose" name space
using namespace rose::Diagnostics;

/****************************************************
 * resolve expression
 ****************************************************/
static std::string unparsePowerpcRegister(const RegisterDescriptor &rdesc, const RegisterDictionary *registers)
{
    if (!registers)
        registers = RegisterDictionary::dictionary_powerpc();
    std::string name = registers->lookup(rdesc);
    if (name.empty()) {
        static bool dumped_dict = false;
        Stream warn(Diagnostics::log[WARN]);
        warn <<"unparsePowerpcRegister(" <<rdesc <<"): register descriptor not found in dictionary.\n";
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
        using namespace StringUtility;
        return (std::string("BAD_REGISTER(") +
                numberToString(rdesc.get_major()) + "." +
                numberToString(rdesc.get_minor()) + "." +
                numberToString(rdesc.get_offset()) + "." +
                numberToString(rdesc.get_nbits()) + ")");
    }
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
        case V_SgAsmPowerpcRegisterReferenceExpression: {
            SgAsmPowerpcRegisterReferenceExpression* rr = isSgAsmPowerpcRegisterReferenceExpression(expr);
            result = unparsePowerpcRegister(rr->get_descriptor(), registers);
            break;
        }
        case V_SgAsmIntegerValueExpression: {
            uint64_t v = isSgAsmIntegerValueExpression(expr)->get_absolute_value();
            if (useHex) {
                result = StringUtility::intToHex(v);
            } else {
                result = StringUtility::numberToString(v);
            }
            if (labels) {
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
