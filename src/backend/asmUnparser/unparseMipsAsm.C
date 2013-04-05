#include "sage3basic.h"
#include "Registers.h"
#include "AsmUnparser.h"

#include <iomanip>

/** Returns a string containing everthing before the first operand in a typical x86 assembly statement. */
std::string unparseMipsMnemonic(SgAsmMipsInstruction *insn) {
    ROSE_ASSERT(insn!=NULL);
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
        std::cerr <<"unparseMipsRegister(" <<reg <<"): register descriptor not found in dictionary.\n";
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
        std::cerr <<"mipsTypeToPtrName: null type" <<std::endl;
        return "BAD_TYPE";
    }

    switch (ty->variantT()) {
        case V_SgAsmTypeByte: return "BYTE";
        case V_SgAsmTypeWord: return "HALFWORD";
        case V_SgAsmTypeDoubleWord: return "WORD";
        case V_SgAsmTypeQuadWord: return "DOUBLEWORD";
        default: {
            std::cerr << "MipsTypeToPtrName: Bad class " << ty->class_name() << std::endl;
            assert(!"bad type class");
            abort();
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
            assert(ival!=NULL);
            uint64_t value = ival->get_absolute_value(); // not sign extended
            result = StringUtility::addrToString(value, ival->get_significant_bits(), true/*signed*/);

            // Optional label.  Prefer a label supplied by the caller's LabelMap, but not for single-byte constants.  If
            // there's no caller-supplied label, then consider whether the value expression is relative to some other IR node.
            std::string label;
            if (ival->get_significant_bits()>8)
                label =mipsValToLabel(value, labels);
            if (label.empty())
                label = ival->get_label();
            if (!label.empty())
                result += "<" + label + ">";
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
#if 0
    if (expr->get_bit_size()>0) {
        result += " <@" + StringUtility::numberToString(expr->get_bit_offset()) +
                  "+" + StringUtility::numberToString(expr->get_bit_size()) + ">";
    }
#endif
    return result;
}
