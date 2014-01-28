#include "sage3basic.h"
#include "Registers.h"
#include "AsmUnparser.h"
#include "Diagnostics.h"

#include <iomanip>

using namespace rose;                                   // temporary until this API lives in the "rose" name space
using namespace rose::Diagnostics;

/** Returns a string containing everthing before the first operand in a typical x86 assembly statement. */
std::string unparseMipsMnemonic(SgAsmMipsInstruction *insn) {
    ASSERT_not_null(insn);
    return insn->get_mnemonic();
}

/** Returns the name of a MIPS register.
 *
 * FIXME: This assumes MIPS32 */
std::string unparseMipsRegister(const RegisterDescriptor &reg, const RegisterDictionary *registers) {
    using namespace StringUtility;
    if (!registers)
        registers = RegisterDictionary::dictionary_mips32();
    std::string name = registers->lookup(reg);
    if (name.empty()) {
        Stream warn(Diagnostics::log[WARN]);
        static bool dumped_dict = false;
        warn <<"unparseMipsRegister(" <<reg <<"): register descriptor not found in dictionary.\n";
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
        Diagnostics::log[ERROR] <<"mipsTypeToPtrName: null type\n";
        return "BAD_TYPE";
    }

    switch (ty->variantT()) {
        case V_SgAsmTypeByte: return "BYTE";
        case V_SgAsmTypeWord: return "HALFWORD";
        case V_SgAsmTypeDoubleWord: return "WORD";
        case V_SgAsmTypeQuadWord: return "DOUBLEWORD";
        default: {
            ASSERT_not_reachable("invalid MIPS type: " + ty->class_name());
        }
    }
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
            result = mipsTypeToPtrName(mr->get_type()) + " PTR [" +
                     unparseMipsExpression(mr->get_address(), labels, registers) + "]";
            break;
        }

        case V_SgAsmMipsRegisterReferenceExpression: {
            SgAsmMipsRegisterReferenceExpression* rr = isSgAsmMipsRegisterReferenceExpression(expr);
            result = unparseMipsRegister(rr->get_descriptor(), registers);
            break;
        }

        case V_SgAsmByteValueExpression:
        case V_SgAsmWordValueExpression:
        case V_SgAsmDoubleWordValueExpression:
        case V_SgAsmQuadWordValueExpression:
        case V_SgAsmIntegerValueExpression: {
            SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(expr);
            ASSERT_not_null(ival);
            uint64_t value = ival->get_absolute_value(); // not sign extended
            result = StringUtility::signedToHex2(value, ival->get_significant_bits());

            // Optional label.  Prefer a label supplied by the caller's LabelMap, but not for single-byte constants.  If
            // there's no caller-supplied label, then consider whether the value expression is relative to some other IR node.
            std::string label;
            if (ival->get_significant_bits()>8)
                label =mipsValToLabel(value, labels);
            if (label.empty())
                label = ival->get_label();
            result = StringUtility::appendAsmComment(result, label);
            break;
        }

        default: {
            ASSERT_not_reachable("invalid MIPS expression: " + expr->class_name());
        }
    }

    result = StringUtility::appendAsmComment(result, expr->get_replacement());
    return result;
}
