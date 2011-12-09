#include "rose.h"

#include "PrintAsmFunctions.h"
#include "string.h"

using namespace std;

void
PrintAsmFunctions::init(SgNode* n) {
  output="";
}

void
PrintAsmFunctions::visit(SgNode* n) {
  SgAsmInstruction* binInst = isSgAsmInstruction(n);
  SgAsmFunction* funcDecl = isSgAsmFunction(n);
  SgAsmBlock* block = isSgAsmBlock(n);
  if (funcDecl) {
    string name = funcDecl->get_name();
    output +=  " ******** function : " + name + " ********** \n" ;
  }
  if (block) {
    output += " ******** block  ********** \n" ;
  }
  if (binInst==NULL) return;
  ROSE_ASSERT(binInst);

// DQ (8/31/2008): IR nodes now uses SgUnsignedCharList instead of string
// std::string str=binInst->get_raw_bytes();
// char *array=strdup(str.c_str());
  SgUnsignedCharList array = binInst->get_raw_bytes();
  int size=0;
  string hexcode ="";
  for(int i=0; array[i]; ++i) {
    size++;
    //    std::cerr << std::bitset<sizeof(char)*8>((int)array[i]);
    const char* table = "0123456789abcdef";
    unsigned char c = (char)array[i];
    string S;
    S = table[c>>4];
    S += table[c & 0x0F];
    hexcode = hexcode + " "+S;
  }
  //  std::cerr << " size: " << size << endl; 

  unsigned int address = binInst->get_address();
  ostringstream addrhex;
  addrhex << hex << setw(8) << address ;
  string address_str = addrhex.str();
  // print the instruction
  output += ""+address_str + "  " + RoseBin_support::ToString(size) + "   " + hexcode;
  if (size==0) output += "\t\t\t\t";
  if (size>0 && size<=3) output +=  "\t\t\t";
  if (size>3 && size<=5) output += "\t\t";
  if (size>5 ) output += "\t";

  output +=  unparseInstruction(binInst) +"\n";
}

extern "C" BC_AnalysisInterface* create() {
  return new PrintAsmFunctions();
}

