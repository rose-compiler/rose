#ifndef BINQSUPPORT_H
#define BINQSUPPORT_H
//#include "rose.h"

SgAsmExpressionPtrList& getOperands(SgAsmInstruction* insn);


class BinQSupport
{
  public:
  BinQSupport(){};
    ~BinQSupport();
    SgNode* disassembleFile(std::string tsv_directory, std::string& sourceFile);

    static rose_addr_t evaluateMemoryExpression(SgAsmx86Instruction* destInst,
					 SgAsmMemoryReferenceExpression* mem);
    static std::string resolveValue(SgAsmValueExpression* leftVal);

    static bool 
      memoryExpressionContainsRegister(X86RegisterClass cl, int registerNumber,
				       SgAsmMemoryReferenceExpression* mem);

  private:
    std::string relativePathPrefix;
    std::string ToUpper(std::string myString);   
}; //class BinQGUI

#endif
