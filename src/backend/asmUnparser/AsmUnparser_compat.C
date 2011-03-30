/* These are backward compatibility functions now implemented in terms of AsmUnparser */
#include "sage3basic.h"
#include "AsmUnparser_compat.h"

/* FIXME: this should be a SgAsmInstruction class method. */
std::string unparseInstruction(SgAsmInstruction* insn, const AsmUnparser::LabelMap *labels) {
    /* Mnemonic */
    if (!insn) return "BOGUS:NULL";
    std::string result = unparseMnemonic(insn);
    result += std::string((result.size() >= 7 ? 1 : 7-result.size()), ' ');

    /* Operands */
    SgAsmOperandList* opList = insn->get_operandList();
    const SgAsmExpressionPtrList& operands = opList->get_operands();
    for (size_t i = 0; i < operands.size(); ++i) {
        if (i != 0) result += ", ";
        result += unparseExpression(operands[i], labels);
    }

    return result;
}

/* FIXME: This should be a SgAsmInstruction class method. */
std::string unparseInstructionWithAddress(SgAsmInstruction* insn, const AsmUnparser::LabelMap *labels) {
    if (!insn) return "BOGUS:NULL";
    return StringUtility::intToHex(insn->get_address()) + ":" + unparseInstruction(insn, labels);
}

/* FIXME: This should be a SgAsmInstruction class method. */
std::string unparseMnemonic(SgAsmInstruction *insn) {
    switch (insn->variantT()) {
        case V_SgAsmx86Instruction:
            return unparseX86Mnemonic(isSgAsmx86Instruction(insn));
        case V_SgAsmArmInstruction:
            return unparseArmMnemonic(isSgAsmArmInstruction(insn));
        case V_SgAsmPowerpcInstruction:
            return unparsePowerpcMnemonic(isSgAsmPowerpcInstruction(insn));
        default:
            std::cerr <<"Unhandled variant " <<insn->class_name() <<std::endl;
            abort();
    }
#ifdef _MSC_VER
    return "error in unparseMnemonic"; /*MSC doesn't know that abort() doesn't return*/
#endif
}

/* FIXME: This should be an SgAsmExpression class method */
std::string unparseExpression(SgAsmExpression *expr, const AsmUnparser::LabelMap *labels) {
    /* Find the instruction with which this expression is associated. */
    SgAsmInstruction *insn = NULL;
    for (SgNode *node=expr; !insn && node; node=node->get_parent()) {
        insn = isSgAsmInstruction(node);
    }

    /* The expression is possibly not linked into the tree yet. Assume x86 if that happens. */
    if (!insn)
        return unparseX86Expression(expr, labels, false);
        
    switch (insn->variantT()) {
        case V_SgAsmx86Instruction:
            return unparseX86Expression(expr, labels);
        case V_SgAsmArmInstruction:
            return unparseArmExpression(expr, labels);
        case V_SgAsmPowerpcInstruction:
            return unparsePowerpcExpression(expr, labels);
        default:
            std::cerr <<"Unhandled variant " <<insn->class_name() << std::endl;
            abort();
    }
#ifdef _MSC_VER
    return "ERROR in unparseExpression()"; /*MSC doesn't know that abort() doesn't return*/
#endif
}

/* FIXME: This should be an SgAsmStatement class method. */
std::string
unparseAsmStatement(SgAsmStatement* stmt)
{
    std::ostringstream s;
    AsmUnparser u;
    switch (stmt->variantT()) {
        case V_SgAsmx86Instruction:
        case V_SgAsmArmInstruction:
        case V_SgAsmPowerpcInstruction:
            u.unparse(s, isSgAsmInstruction(stmt));
            return s.str();
        case V_SgAsmBlock:
            u.unparse(s, isSgAsmBlock(stmt));
            return s.str();
        case V_SgAsmFunctionDeclaration:
            u.unparse(s, isSgAsmFunctionDeclaration(stmt));
            return s.str();
        default:
            std::cerr <<"Unhandled variant " <<stmt->class_name() <<std::endl;
            abort();
    }
#ifdef _MSC_VER
    return "ERROR in unparseAsmStatement()"; /*MSC doesn't know that abort() doesn't return*/
#endif
}

std::string
unparseAsmInterpretation(SgAsmInterpretation* interp)
{
    std::ostringstream s;
    AsmUnparser unparser;
    unparser.add_function_labels(interp);
    unparser.unparse(s, interp);
    return s.str();
}

void
unparseAsmStatementToFile(const std::string& filename, SgAsmStatement* stmt)
{
    ROSE_ASSERT (stmt != NULL);
    std::ofstream of(filename.c_str());
    of << unparseAsmStatement(stmt);
}
