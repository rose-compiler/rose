

#include "PrintAsmInstruction.h"
#include "string.h"

using namespace std;


void
PrintAsmInstruction::visit(SgNode* n) {
  
  SgAsmInstruction* binInst = isSgAsmInstruction(n);
  if (binInst==NULL) return;
  ROSE_ASSERT(binInst);
  

  unsigned int address = binInst->get_address();
  ostringstream addrhex;
  addrhex << hex << setw(8) << address ;
  string address_str = addrhex.str();
  // print the instruction
  cout << address_str << "  " << unparser->unparseInstruction(binInst) << "\n";
  
}

extern "C" BC_AnalysisInterface* create() {
  return new PrintAsmInstruction();
}

