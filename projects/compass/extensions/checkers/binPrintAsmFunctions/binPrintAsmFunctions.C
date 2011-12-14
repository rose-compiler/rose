// Bin Print Asm Functions
// Author: Thomas Panas
// Date: 08-August-2008

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_BIN_PRINT_ASM_FUNCTIONS_H
#define COMPASS_BIN_PRINT_ASM_FUNCTIONS_H

namespace CompassAnalyses
   { 
     namespace BinPrintAsmFunctions
        { 
        /*! \brief Bin Print Asm Functions: Add your description here 
         */

          extern const std::string checkerName;
          extern const std::string shortDescription;
          extern const std::string longDescription;

       // Specification of Checker Output Implementation
          class CheckerOutput: public Compass::OutputViolationBase
             { 
               public:
	       CheckerOutput(SgNode* node, std::string value);
             };

       // Specification of Checker Traversal Implementation

          class Traversal
             : public Compass::AstSimpleProcessingWithRunFunction
             {
            // Checker specific parameters should be allocated here.
               Compass::OutputObject* output;
	       std::string stringOutput;
	       //SgAsmFunction* project;
	       SgBinaryComposite* file;
               public:
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

                 // Change the implementation of this function if you are using inherited attributes.
                    void *initialInheritedAttribute() const { return NULL; }

                 // The implementation of the run function has to match the traversal being called.
                 // If you use inherited attributes, use the following definition:
                 // void run(SgNode* n){ this->traverse(n, initialInheritedAttribute()); }
                    void run(SgNode* n){ this->traverse(n, preorder); 
		      //std::cerr << " running binary checker: binPrintAsmFunctions " << std::endl;
		      finalize();
		    }

		    void finalize();

                 // Change this function if you are using a different type of traversal, e.g.
                 // void *evaluateInheritedAttribute(SgNode *, void *);
                 // for AstTopDownProcessing.
                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_BIN_PRINT_ASM_FUNCTIONS_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Bin Print Asm Functions Analysis
// Author: Thomas Panas
// Date: 08-August-2008

#include "compass.h"
// #include "binPrintAsmFunctions.h"

using namespace std;

namespace CompassAnalyses
   { 
     namespace BinPrintAsmFunctions
        { 
          const std::string checkerName      = "BinPrintAsmFunctions";
          
       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Simple Binary Unparser Test";
          const std::string longDescription  = "Simple Binary Unparser Test";
        } //End of namespace BinPrintAsmFunctions.
   } //End of namespace CompassAnalyses.

CompassAnalyses::BinPrintAsmFunctions::
CheckerOutput::CheckerOutput ( SgNode* node, std::string value )
   : OutputViolationBase(node,checkerName,value)
   {}

CompassAnalyses::BinPrintAsmFunctions::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["BinPrintAsmFunctions.YourParameter"]);
     stringOutput="";
     file=NULL;
   }

void
CompassAnalyses::BinPrintAsmFunctions::Traversal::
finalize() {
  //std::cerr << stringOutput << std::endl;
  if (file!=NULL)
    output->addOutput(new CheckerOutput(file, stringOutput));
}

void
CompassAnalyses::BinPrintAsmFunctions::Traversal::
visit(SgNode* n)
   { 
     // mark the first Function as the output object
     //     if (isSgAsmFunction(n) && file==NULL)
     //file = isSgAsmFunction(n);
     if (isSgBinaryComposite(n) && file==NULL)
       file = isSgBinaryComposite(n);
  SgAsmInstruction* binInst = isSgAsmInstruction(n);
  SgAsmFunction* funcDecl = isSgAsmFunction(n);
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
  stringOutput += ""+address_str + "  " + RoseBin_support::ToString(size) + "   " + hexcode;
  if (size==0) stringOutput += "\t\t\t\t";
  if (size>0 && size<=3) stringOutput +=  "\t\t\t";
  if (size>3 && size<=5) stringOutput += "\t\t";
  if (size>5 ) stringOutput += "\t";

  stringOutput +=  unparseInstruction(binInst) +"\n";

   } //End of the visit function.
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::BinPrintAsmFunctions::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::BinPrintAsmFunctions::Traversal(params, output);
}

extern const Compass::Checker* const binPrintAsmFunctionsChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::BinPrintAsmFunctions::checkerName,
        CompassAnalyses::BinPrintAsmFunctions::shortDescription,
        CompassAnalyses::BinPrintAsmFunctions::longDescription,
        Compass::X86Assembly,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
