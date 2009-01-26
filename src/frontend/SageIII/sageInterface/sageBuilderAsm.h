#ifndef ROSE_SAGEBUILDERASM_H
#define ROSE_SAGEBUILDERASM_H

#include "../Disassemblers/disassemblers.h" /* For defn of DisassemblerCommon::FunctionStarts */

namespace PutInstructionsIntoBasicBlocks { // From src/frontend/Disassemblers/putInstructionsIntoBasicBlocks.C
    SgAsmBlock* putInstructionsIntoBasicBlocks(const std::map<uint64_t, SgAsmBlock*> &basicBlocks,
                                               const std::map<uint64_t, SgAsmInstruction*> &insns);
    SgAsmBlock* putInstructionsIntoFunctions(SgAsmBlock* top, const DisassemblerCommon::FunctionStarts &functionStarts);
}

namespace SageBuilderAsm {
  SgAsmByteValueExpression* makeByteValue(uint8_t val);
  SgAsmWordValueExpression* makeWordValue(uint16_t val);
  SgAsmDoubleWordValueExpression* makeDWordValue(uint32_t val);
  SgAsmQuadWordValueExpression* makeQWordValue(uint64_t val);
  SgAsmMemoryReferenceExpression* makeMemoryReference(SgAsmExpression* addr, SgAsmExpression* segment = NULL, SgAsmType* t = NULL);
  SgAsmBinaryAdd* makeAdd(SgAsmExpression* lhs, SgAsmExpression* rhs);
  SgAsmBinarySubtract* makeSubtract(SgAsmExpression* lhs, SgAsmExpression* rhs);
  SgAsmBinaryAddPreupdate* makeAddPreupdate(SgAsmExpression* lhs, SgAsmExpression* rhs);
  SgAsmBinarySubtractPreupdate* makeSubtractPreupdate(SgAsmExpression* lhs, SgAsmExpression* rhs);
  SgAsmBinaryAddPostupdate* makeAddPostupdate(SgAsmExpression* lhs, SgAsmExpression* rhs);
  SgAsmBinarySubtractPostupdate* makeSubtractPostupdate(SgAsmExpression* lhs, SgAsmExpression* rhs);
  SgAsmBinaryMultiply* makeMul(SgAsmExpression* lhs, SgAsmExpression* rhs);
  SgAsmBinaryLsl* makeLsl(SgAsmExpression* lhs, SgAsmExpression* rhs);
  SgAsmBinaryLsr* makeLsr(SgAsmExpression* lhs, SgAsmExpression* rhs);
  SgAsmBinaryAsr* makeAsr(SgAsmExpression* lhs, SgAsmExpression* rhs);
  SgAsmBinaryRor* makeRor(SgAsmExpression* lhs, SgAsmExpression* rhs);
  SgAsmUnaryRrx* makeRrx(SgAsmExpression* lhs);
  SgAsmUnaryArmSpecialRegisterList* makeArmSpecialRegisterList(SgAsmExpression* lhs);
  SgAsmExprListExp* makeExprListExp();

  template <typename Insn> // For correct subtype to be returned
  inline Insn* appendOperand(Insn* insn, SgAsmExpression* op) {
    SgAsmOperandList* operands = insn->get_operandList();
    operands->get_operands().push_back(op);
    op->set_parent(operands);
    return insn; // For chaining this operation
  }

}

#endif // ROSE_SAGEBUILDERASM_H
