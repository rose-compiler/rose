#ifndef BINQSUPPORT_H
#define BINQSUPPORT_H
#include "rose.h"

SgAsmExpressionPtrList& getOperands(SgAsmInstruction* insn);


class BinQSupport
{
  public:
  BinQSupport(){};
    ~BinQSupport();
    SgNode* disassembleFile(std::string tsv_directory, std::string& sourceFile);
    std::string normalizeInstructionsToHTML(std::vector<SgAsmx86Instruction*>::iterator beg, 
					    std::vector<SgAsmx86Instruction*>::iterator end);

    std::pair<std::string,std::string> getAddressFromVectorsTable(uint64_t function_id, uint64_t index);
    std::string unparseX86InstructionToHTMLWithAddress(SgAsmx86Instruction* insn);

  private:
    std::string relativePathPrefix;
    std::string ToUpper(std::string myString);   
}; //class BinQGUI

#endif
