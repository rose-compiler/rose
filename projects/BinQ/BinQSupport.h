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

    static SgAsmInstruction* getNextStmt(SgAsmInstruction* inst);
    static rose_addr_t checkIfValidAddress(rose_addr_t next_addr, SgAsmInstruction* inst);
    static SgAsmInstruction* getPrevStmt(SgAsmInstruction* inst);
    // this one is not precise because it only gives you one edge
    // along the CFG up. Unfortunately, the get_predecessors algo from
    // Jeremiah doesnt work on IDA yet
    static SgAsmx86Instruction* checkIfValidPredecessor(rose_addr_t next_addr, SgAsmInstruction* inst);

  private:
    std::string relativePathPrefix;
    std::string ToUpper(std::string myString);   
}; //class BinQGUI

#endif
