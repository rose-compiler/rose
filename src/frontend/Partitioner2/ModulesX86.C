#include "sage3basic.h"
#include <Partitioner2/ModulesX86.h>
#include <Partitioner2/Partitioner.h>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {
namespace ModulesX86 {

bool
MatchStandardPrologue::match(const Partitioner *partitioner, rose_addr_t anchor) {
    ASSERT_not_null(partitioner);
    const RegisterDescriptor bp(x86_regclass_gpr, x86_gpr_bp, 0,
                                partitioner->instructionProvider().instructionPointerRegister().get_nbits());
    const RegisterDescriptor sp(x86_regclass_gpr, x86_gpr_sp, 0, bp.get_nbits());

    // Look for PUSH EBP
    SgAsmx86Instruction *insn = NULL; 
    {
        rose_addr_t pushVa = anchor;
        if (partitioner->instructionExists(pushVa))
            return false;                               // already in the CFG/AUM
        insn = isSgAsmx86Instruction(partitioner->discoverInstruction(pushVa));
        if (!insn || insn->get_kind()!=x86_push)
            return false;
        const SgAsmExpressionPtrList &opands = insn->get_operandList()->get_operands();
        if (opands.size()!=1)
            return false;                               // crazy operands!
        SgAsmRegisterReferenceExpression *rre = isSgAsmRegisterReferenceExpression(opands[0]);
        if (!rre || rre->get_descriptor()!=bp)
            return false;
    }

    // Look for MOV RBP,RSP following the PUSH.
    {
        rose_addr_t moveVa = insn->get_address() + insn->get_size();
        if (partitioner->instructionExists(moveVa))
            return false;                               // already in the CFG/AUM
        insn = isSgAsmx86Instruction(partitioner->discoverInstruction(moveVa));
        if (!insn || insn->get_kind()!=x86_mov)
            return false;
        const SgAsmExpressionPtrList &opands = insn->get_operandList()->get_operands();
        if (opands.size()!=2)
            return false;                               // crazy operands!
        SgAsmRegisterReferenceExpression *rre = isSgAsmRegisterReferenceExpression(opands[0]);
        if (!rre || rre->get_descriptor()!=bp)
            return false;
        rre = isSgAsmRegisterReferenceExpression(opands[1]);
        if (!rre || rre->get_descriptor()!=sp)
            return false;
    }

    function_ = Function::instance(anchor, SgAsmFunction::FUNC_PATTERN);
    return true;
}

bool
MatchHotPatchPrologue::match(const Partitioner *partitioner, rose_addr_t anchor) {
    // Look for optional MOV EDI, EDI
    rose_addr_t moveVa = anchor;
    if (partitioner->instructionExists(moveVa))
        return false;                               // already in the CFG/AUM
    SgAsmx86Instruction *insn = isSgAsmx86Instruction(partitioner->discoverInstruction(moveVa));
    if (!insn || insn->get_kind()!=x86_mov)
        return false;
    const SgAsmExpressionPtrList &opands = insn->get_operandList()->get_operands();
    if (opands.size()!=2)
        return false;
    SgAsmDirectRegisterExpression *dst = isSgAsmDirectRegisterExpression(opands[0]);
    if (!dst ||
        dst->get_descriptor().get_major()!=x86_regclass_gpr ||
        dst->get_descriptor().get_minor()!=x86_gpr_di)
        return false;
    SgAsmDirectRegisterExpression *src = isSgAsmDirectRegisterExpression(opands[1]);
    if (!src || dst->get_descriptor()!=src->get_descriptor())
        return false;

    // Match a standard prologue immediately following the hot-patch
    if (!MatchStandardPrologue::match(partitioner, insn->get_address()+insn->get_size()))
        return false;

    function_ = Function::instance(anchor, SgAsmFunction::FUNC_PATTERN);
    return true;
}

bool
MatchEnterPrologue::match(const Partitioner *partitioner, rose_addr_t anchor) {
    ASSERT_not_null(partitioner);
    if (partitioner->instructionExists(anchor))
        return false;                                   // already in the CFG/AUM
    SgAsmx86Instruction *insn = isSgAsmx86Instruction(partitioner->discoverInstruction(anchor));
    if (!insn || insn->get_kind()!=x86_enter)
        return false;
    const SgAsmExpressionPtrList &args = insn->get_operandList()->get_operands();
    if (2!=args.size())
        return false;
    SgAsmIntegerValueExpression *arg = isSgAsmIntegerValueExpression(args[1]);
    if (!arg || 0!=arg->get_absoluteValue())
        return false;

    function_ = Function::instance(anchor, SgAsmFunction::FUNC_PATTERN);
    return true;
}

} // namespace
} // namespace
} // namespace
} // namespace
