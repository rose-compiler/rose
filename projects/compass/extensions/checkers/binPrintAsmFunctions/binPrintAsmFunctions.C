// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Bin Print Asm Functions Analysis
// Author: Thomas Panas
// Date: 08-August-2008

#include "compass.h"
#include "binPrintAsmFunctions.h"

using namespace std;

namespace CompassAnalyses
   { 
     namespace BinPrintAsmFunctions
        { 
          const std::string checkerName      = "BinPrintAsmFunctions";
          
       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Short description not written yet!";
          const std::string longDescription  = "Long description not written yet!";
        } //End of namespace BinPrintAsmFunctions.
   } //End of namespace CompassAnalyses.

CompassAnalyses::BinPrintAsmFunctions::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::BinPrintAsmFunctions::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["BinPrintAsmFunctions.YourParameter"]);
     stringOutput="";

   }

void
CompassAnalyses::BinPrintAsmFunctions::Traversal::
finalize() {
  std::cerr << stringOutput << std::endl;
}

void
CompassAnalyses::BinPrintAsmFunctions::Traversal::
visit(SgNode* n)
   { 
  SgAsmInstruction* binInst = isSgAsmInstruction(n);
  SgAsmFunctionDeclaration* funcDecl = isSgAsmFunctionDeclaration(n);
  SgAsmBlock* block = isSgAsmBlock(n);
  if (funcDecl) {
    string name = funcDecl->get_name();
    stringOutput +=  " ******** function : " + name + " ********** \n" ;
  }
  if (block) {
    stringOutput += " ******** block  ********** \n" ;
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
  stringOutput += ""+address_str + "  " + RoseBin_support::ToString(size) + "   " + hexcode;
  if (size==0) stringOutput += "\t\t\t\t";
  if (size>0 && size<=3) stringOutput +=  "\t\t\t";
  if (size>3 && size<=5) stringOutput += "\t\t";
  if (size>5 ) stringOutput += "\t";

  stringOutput +=  unparseInstruction(binInst) +"\n";

   } //End of the visit function.
   
