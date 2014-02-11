#include "sage3basic.h"
#include "Registers.h"
#include "AsmUnparser.h"

/** Returns a string containing everything before the first operand in a typical m68k statement. */
std::string
unparseM68kMnemonic(SgAsmM68kInstruction *insn)
{
    assert(insn!=NULL);
    return insn->get_mnemonic();
}

std::string
unparseM68kExpression(SgAsmExpression *expr, const AsmUnparser::LabelMap *labels, const RegisterDictionary *registers)
{
    RegisterNames name_of(registers);
    std::string result = "";
    if (expr==NULL)
        return "BUGUS:NULL";

    if (SgAsmM68kRegisterReferenceExpression *rre = isSgAsmM68kRegisterReferenceExpression(expr)) {
        int adjustment = rre->get_adjustment();
        if (adjustment < 0)
            result = "--";
        result += name_of(rre->get_descriptor());
        if (adjustment > 0)
            result += "++";
    } else if (SgAsmMemoryReferenceExpression *mre = isSgAsmMemoryReferenceExpression(expr)) {
        result = "[" + unparseM68kExpression(mre->get_address(), labels, registers) + "]";
    } else if (SgAsmBinaryAdd *add = isSgAsmBinaryAdd(expr)) {
        result = unparseM68kExpression(add->get_lhs(), labels, registers) + "+" +
                 unparseM68kExpression(add->get_rhs(), labels, registers);
    } else if (SgAsmBinaryMultiply *mul = isSgAsmBinaryMultiply(expr)) {
        result = unparseM68kExpression(mul->get_lhs(), labels, registers) + "*" +
                 unparseM68kExpression(mul->get_rhs(), labels, registers);
    } else if (SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(expr)) {
        assert(ival!=NULL);
        uint64_t value = ival->get_absolute_value(); // not sign extended
        result = StringUtility::addrToString(value, ival->get_significant_bits(), true/*signed*/);

        // Optional label.  Prefer a label supplied by the caller's LabelMap, but not for single-byte constants.  If
        // there's no caller-supplied label, then consider whether the value expression is relative to some other IR node.
        std::string label;
        if (ival->get_significant_bits()>8) {
            if (0!=value && labels!=NULL) {
                AsmUnparser::LabelMap::const_iterator li = labels->find(value);
                if (li!=labels->end())
                    label = li->second;
            }
        }
        if (label.empty())
            label = ival->get_label();
        if (!label.empty())
            result += "<" + label + ">";
    } else {
        result = "<UNHANDLED_EXPRESSION type=" + expr->class_name() + ">";
    }
    if (expr->get_replacement() != "") {
        result += " <" + expr->get_replacement() + ">";
    }
    return result;
}
