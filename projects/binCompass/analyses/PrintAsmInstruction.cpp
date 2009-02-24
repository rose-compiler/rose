#include "rose.h"

#include "PrintAsmInstruction.h"
#include "string.h"

using namespace std;

void
PrintAsmInstruction::init(SgNode* n) {
  output="";
}

void
PrintAsmInstruction::finish(SgNode* n) {
  rose_hash::hash_map<std::string, int>::const_iterator it = instMap.begin();
  multimap<int, string> s;
  for (;it!=instMap.end();it++) {
    string instType = it->first;
    int nr = it->second;
    s.insert(pair<int, string>(nr,instType));
  }
  multimap<int, string>::reverse_iterator it2 = s.rbegin();
  for (;it2!=s.rend();it2++) {
    string instType = it2->second;
    int nr = it2->first;
    output += instType + ":" + RoseBin_support::ToString(nr) + ": \n";
  }
}

void
PrintAsmInstruction::visit(SgNode* n) {
  
  SgAsmInstruction* binInst = isSgAsmInstruction(n);
  if (binInst==NULL) return;
  ROSE_ASSERT(binInst);

  string className = binInst->class_name();
  int nr = 1;
  rose_hash::hash_map<std::string, int>::const_iterator it = instMap.find(className);
  if (it!=instMap.end()) {
    nr = it->second;
    nr++;
  }
  instMap[className]=nr;

  unsigned int address = binInst->get_address();
  ostringstream addrhex;
  addrhex << hex << setw(8) << address ;
  string address_str = addrhex.str();
  // print the instruction
  //cout << address_str << "  " << unparser->unparseInstruction(binInst) << 
  //  "    className " << binInst->class_name() << "  occured: " << nr << endl;
  
}

extern "C" BC_AnalysisInterface* create() {
  return new PrintAsmInstruction();
}

