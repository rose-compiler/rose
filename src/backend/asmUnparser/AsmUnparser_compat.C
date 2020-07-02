/* These are backward compatibility functions now implemented in terms of AsmUnparser */
#include <featureTests.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"
#include "AsmUnparser_compat.h"

#include "BinaryControlFlow.h"
#include "Diagnostics.h"
#include "Disassembler.h"

using namespace Rose;
using namespace Rose::BinaryAnalysis;

/* FIXME: this should be a SgAsmInstruction class method. */
std::string unparseInstruction(SgAsmInstruction* insn, const AsmUnparser::LabelMap *labels, const RegisterDictionary *registers) {
    /* Mnemonic */
    if (!insn) return "BOGUS:NULL";
    std::string result = unparseMnemonic(insn);
    result += std::string((result.size() >= 7 ? 1 : 7-result.size()), ' ');

    /* Operands */
    SgAsmOperandList* opList = insn->get_operandList();
    const SgAsmExpressionPtrList& operands = opList->get_operands();
    for (size_t i = 0; i < operands.size(); ++i) {
        if (i != 0) result += ", ";
        result += unparseExpression(operands[i], labels, registers);
    }

    return result;
}

/* FIXME: This should be a SgAsmInstruction class method. */
std::string unparseInstructionWithAddress(SgAsmInstruction* insn, const AsmUnparser::LabelMap *labels,
                                          const RegisterDictionary *registers) {
    if (!insn) return "BOGUS:NULL";
    return StringUtility::addrToString(insn->get_address()) + ": " + unparseInstruction(insn, labels, registers);
}

/* FIXME: This should be a SgAsmInstruction class method. */
std::string unparseMnemonic(SgAsmInstruction *insn) {
    switch (insn->variantT()) {
        case V_SgAsmX86Instruction:
            return unparseX86Mnemonic(isSgAsmX86Instruction(insn));
#ifdef ROSE_ENABLE_ASM_A64
        case V_SgAsmA64Instruction:
            return unparseA64Mnemonic(isSgAsmA64Instruction(insn));
#endif
        case V_SgAsmPowerpcInstruction:
            return unparsePowerpcMnemonic(isSgAsmPowerpcInstruction(insn));
        case V_SgAsmMipsInstruction:
            return unparseMipsMnemonic(isSgAsmMipsInstruction(insn));
        case V_SgAsmM68kInstruction:
            return unparseM68kMnemonic(isSgAsmM68kInstruction(insn));
        default:
            ASSERT_not_reachable("unhandled variant: " + insn->class_name());
    }
#ifdef _MSC_VER
    return "error in unparseMnemonic"; /*MSC doesn't know that abort() doesn't return*/
#endif
}

/* FIXME: This should be an SgAsmExpression class method */
std::string unparseExpression(SgAsmExpression *expr, const AsmUnparser::LabelMap *labels, const RegisterDictionary *registers) {
    // Find the instruction with which this expression is associated. If we go through the instruction's p_semantics member to
    // get there then don't unparse this (it's static semantics, not instruction arguments).
    SgAsmInstruction *insn = NULL;
    for (SgNode *node=expr; !insn && node; node=node->get_parent()) {
        if (node->get_parent()!=NULL && isSgAsmInstruction(node->get_parent()) &&
            node == isSgAsmInstruction(node->get_parent())->get_semantics())
            return "";
        insn = isSgAsmInstruction(node);
    }

    /* The expression is possibly not linked into the tree yet. Assume x86 if that happens. */
    if (!insn)
        return unparseX86Expression(expr, labels, registers, false);
        
    switch (insn->variantT()) {
        case V_SgAsmX86Instruction:
            return unparseX86Expression(expr, labels, registers);
#ifdef ROSE_ENABLE_ASM_A64
        case V_SgAsmA64Instruction:
            return unparseA64Expression(expr, labels, registers);
#endif
        case V_SgAsmPowerpcInstruction:
            return unparsePowerpcExpression(expr, labels, registers);
        case V_SgAsmMipsInstruction:
            return unparseMipsExpression(expr, labels, registers);
        case V_SgAsmM68kInstruction:
            return unparseM68kExpression(expr, labels, registers);
        default:
            ASSERT_not_reachable("unhandled variant: " + insn->class_name());
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
        case V_SgAsmX86Instruction:
#ifdef ROSE_ENABLE_ASM_A64
        case V_SgAsmA64Instruction:
#endif
        case V_SgAsmPowerpcInstruction:
        case V_SgAsmMipsInstruction:
        case V_SgAsmM68kInstruction:
            u.unparse(s, isSgAsmInstruction(stmt));
            return s.str();
        case V_SgAsmBlock:
            u.unparse(s, isSgAsmBlock(stmt));
            return s.str();
        case V_SgAsmFunction:
            u.unparse(s, isSgAsmFunction(stmt));
            return s.str();
        default:
            ASSERT_not_reachable("unhandled variant: " + stmt->class_name());
    }
#ifdef _MSC_VER
    return "ERROR in unparseAsmStatement()"; /*MSC doesn't know that abort() doesn't return*/
#endif
}

std::string
unparseAsmInterpretation(SgAsmInterpretation* interp)
{
    AsmUnparser unparser;

    // Build a control flow graph, but exclude all the basic blocks that are marked as disassembly leftovers.
    struct NoLeftovers: public Rose::BinaryAnalysis::ControlFlow::VertexFilter {
        virtual bool operator()(Rose::BinaryAnalysis::ControlFlow*, SgAsmNode *node) {
            SgAsmFunction *func = SageInterface::getEnclosingNode<SgAsmFunction>(node);
            return func && 0==(func->get_reason() & SgAsmFunction::FUNC_LEFTOVERS);
        }
    } vertex_filter;
    Rose::BinaryAnalysis::ControlFlow cfg_analyzer;
    cfg_analyzer.set_vertex_filter(&vertex_filter);
    Rose::BinaryAnalysis::ControlFlow::Graph cfg;
    cfg_analyzer.build_block_cfg_from_ast(interp, cfg/*out*/);

    // We will try to disassemble static data blocks (i.e., disassembling data as instructions), but we need to choose an
    // appropriate disassembler.  We don't have available the disassembler that was originally used, so we'll obtain a default
    // disassembler based on the interpretation's first file header (if it has one).
    Disassembler *disassembler = Disassembler::lookup(interp);
    if (disassembler) {
        disassembler = disassembler->clone();
        unparser.staticDataDisassembler.init(disassembler);
    }

    // Unparse the interpretation to a string.
    std::ostringstream s;
    unparser.add_function_labels(interp);
    unparser.add_control_flow_graph(cfg);
    unparser.unparse(s, interp);
    delete disassembler;
    return s.str();
}

void
unparseAsmStatementToFile(const std::string& filename, SgAsmStatement* stmt)
{
    ASSERT_not_null(stmt);
    std::ofstream of(filename.c_str());
    of << unparseAsmStatement(stmt);
}

#endif
