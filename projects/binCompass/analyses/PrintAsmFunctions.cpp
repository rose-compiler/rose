

#include "PrintAsmFunctions.h"
#include "string.h"

using namespace std;


void
PrintAsmFunctions::visit(SgNode* n) {
  SgAsmInstruction* binInst = isSgAsmInstruction(n);
  SgAsmFunctionDeclaration* funcDecl = isSgAsmFunctionDeclaration(n);
  SgAsmBlock* block = isSgAsmBlock(n);
  if (funcDecl) {
    string name = funcDecl->get_name();
    cout << " ******** function : " << name << " ********** " << endl;
  }
  if (block) {
    cout << " ******** block  ********** " << endl;
  }
  if (binInst==NULL) return;
  ROSE_ASSERT(binInst);

  std::string str=binInst->get_raw_bytes();
  char *array=strdup(str.c_str());
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
  cout << address_str << "  " << size << "   " << hexcode << "\n";
}

extern "C" BC_AnalysisInterface* create() {
  return new PrintAsmFunctions();
}

