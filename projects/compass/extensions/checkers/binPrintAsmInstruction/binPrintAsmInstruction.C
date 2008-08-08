// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Bin Print Asm Instruction Analysis
// Author: Thomas Panas
// Date: 08-August-2008

#include "compass.h"
#include "binPrintAsmInstruction.h"

using namespace std;

namespace CompassAnalyses
   { 
     namespace BinPrintAsmInstruction
        { 
          const std::string checkerName      = "BinPrintAsmInstruction";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Short description not written yet!";
          const std::string longDescription  = "Long description not written yet!";
        } //End of namespace BinPrintAsmInstruction.
   } //End of namespace CompassAnalyses.

CompassAnalyses::BinPrintAsmInstruction::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}


void
CompassAnalyses::BinPrintAsmInstruction::Traversal::
finish(SgNode* n) {
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
    stringOutput += instType + ":" + RoseBin_support::ToString(nr) + " \n";
  }
}


CompassAnalyses::BinPrintAsmInstruction::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["BinPrintAsmInstruction.YourParameter"]);
     stringOutput = "";
     instMap.clear();
   }

void
CompassAnalyses::BinPrintAsmInstruction::Traversal::
visit(SgNode* n)
   { 
  SgAsmx86Instruction* binInst = isSgAsmx86Instruction(n);
  if (binInst==NULL) return;
  ROSE_ASSERT(binInst);

  string className = toString(binInst->get_kind());
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

   } //End of the visit function.
   
