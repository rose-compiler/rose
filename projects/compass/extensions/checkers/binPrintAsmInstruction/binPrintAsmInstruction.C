// Bin Print Asm Instruction
// Author: Thomas Panas
// Date: 08-August-2008

#include "rose.h"
#include "compass.h"
#include "stringify.h"

#ifndef COMPASS_BIN_PRINT_ASM_INSTRUCTION_H
#define COMPASS_BIN_PRINT_ASM_INSTRUCTION_H

namespace CompassAnalyses
   { 
     namespace BinPrintAsmInstruction
        { 
        /*! \brief Bin Print Asm Instruction: Add your description here 
         */

          extern const std::string checkerName;
          extern const std::string shortDescription;
          extern const std::string longDescription;

       // Specification of Checker Output Implementation
          class CheckerOutput: public Compass::OutputViolationBase
             { 
               public:
	       CheckerOutput(SgNode* node,std::string s);
             };

       // Specification of Checker Traversal Implementation

          class Traversal
             : public Compass::AstSimpleProcessingWithRunFunction
             {
            // Checker specific parameters should be allocated here.
               Compass::OutputObject* output;
	       std::string stringOutput;
	       SgBinaryComposite* file;
               public:
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);
                    rose_hash::unordered_map<std::string, int> instMap;

                 // Change the implementation of this function if you are using inherited attributes.
                    void *initialInheritedAttribute() const { return NULL; }

                 // The implementation of the run function has to match the traversal being called.
                 // If you use inherited attributes, use the following definition:
                 // void run(SgNode* n){ this->traverse(n, initialInheritedAttribute()); }
                    void run(SgNode* n){ this->traverse(n, preorder); 
		      //std::cerr << "  running binPrintAsmInstuction checker... " << std::endl;
		      finish(n);
		      //std::cerr << stringOutput << std::endl;
		    }

		    void finish(SgNode* node);

                 // Change this function if you are using a different type of traversal, e.g.
                 // void *evaluateInheritedAttribute(SgNode *, void *);
                 // for AstTopDownProcessing.
                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_BIN_PRINT_ASM_INSTRUCTION_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Bin Print Asm Instruction Analysis
// Author: Thomas Panas
// Date: 08-August-2008

#include "compass.h"
// #include "binPrintAsmInstruction.h"

using namespace std;

namespace CompassAnalyses
   { 
     namespace BinPrintAsmInstruction
        { 
          const std::string checkerName      = "BinPrintAsmInstruction";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Statistics about the number of different instructions";
          const std::string longDescription  = "This analysis reports the number of different instructions in a binary in a sorted order";
        } //End of namespace BinPrintAsmInstruction.
   } //End of namespace CompassAnalyses.

CompassAnalyses::BinPrintAsmInstruction::
CheckerOutput::CheckerOutput ( SgNode* node, std::string output )
   : OutputViolationBase(node,checkerName,output)
   {}


void
CompassAnalyses::BinPrintAsmInstruction::Traversal::
finish(SgNode* n) {
  rose_hash::unordered_map<std::string, int>::const_iterator it = instMap.begin();
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

  if (file!=NULL)
    output->addOutput(new CheckerOutput(file, stringOutput));
}


CompassAnalyses::BinPrintAsmInstruction::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["BinPrintAsmInstruction.YourParameter"]);
     stringOutput = "";
     instMap.clear();
     file=NULL;
   }

void
CompassAnalyses::BinPrintAsmInstruction::Traversal::
visit(SgNode* n)
   { 

     if (isSgBinaryComposite(n) && file==NULL)
       file = isSgBinaryComposite(n);

  SgAsmx86Instruction* binInst = isSgAsmx86Instruction(n);
  if (binInst==NULL) return;
  ROSE_ASSERT(binInst);

  string className = stringifyX86InstructionKind(binInst->get_kind(), "x86_");
  int nr = 1;

//rose_hash::unordered_map<std::string, int>::const_iterator it = instMap.find(className);
  rose_hash::unordered_map<std::string, int>::const_iterator it = instMap.find(className);

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
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::BinPrintAsmInstruction::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::BinPrintAsmInstruction::Traversal(params, output);
}

extern const Compass::Checker* const binPrintAsmInstructionChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::BinPrintAsmInstruction::checkerName,
        CompassAnalyses::BinPrintAsmInstruction::shortDescription,
        CompassAnalyses::BinPrintAsmInstruction::longDescription,
        Compass::X86Assembly,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
