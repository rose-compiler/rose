#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"

#include "Registers.h"
#include "AsmUnparser.h"
#include <boost/foreach.hpp>

using namespace Rose;
using namespace Rose::BinaryAnalysis;

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
    if (!registers)
        registers = RegisterDictionary::dictionary_coldfire_emac();
    RegisterNames name_of(registers);
    std::string result = "";
    if (expr==NULL)
        return "BUGUS:NULL";

    if (SgAsmRegisterReferenceExpression *rre = isSgAsmRegisterReferenceExpression(expr)) {
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
        uint64_t value = ival->get_absoluteValue(); // not sign extended
        result = StringUtility::signedToHex2(value, ival->get_significantBits());

        // Optional label.  Prefer a label supplied by the caller's LabelMap, but not for single-byte constants.  If
        // there's no caller-supplied label, then consider whether the value expression is relative to some other IR node.
        if (expr->get_comment().empty()) {
            std::string label;
            if (ival->get_significantBits()>8) {
                if (0!=value && labels!=NULL) {
                    AsmUnparser::LabelMap::const_iterator li = labels->find(value);
                    if (li!=labels->end())
                        label = li->second;
                }
            }
            if (label.empty())
                label = ival->get_label();
            result = StringUtility::appendAsmComment(result, label);
        }

    } else if (SgAsmRegisterNames *regs = isSgAsmRegisterNames(expr)) {
        // The usual assembly is to show only an integer register mask.  That's not very friendly, especially since the meaning
        // of the bits is dependent on the addressing mode of the other instruction.  So we show the register names instead in
        // curly braces.
        int nregs = 0;
        result = "{";
        BOOST_FOREACH (SgAsmRegisterReferenceExpression *rre, regs->get_registers())
            result += (nregs++ ? ", " : "") + unparseM68kExpression(rre, labels, registers);
        result += "}";
        if (regs->get_mask()!=0)
            result += "<" + StringUtility::toHex2(regs->get_mask(), 16, false, false) + ">";
    } else {
        result = "<UNHANDLED_EXPRESSION type=" + expr->class_name() + ">";
    }

    result = StringUtility::appendAsmComment(result, expr->get_comment());
    return result;
}

#endif
