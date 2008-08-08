// Bin Print Asm Instruction
// Author: Thomas Panas
// Date: 08-August-2008

#include "compass.h"

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
                    CheckerOutput(SgNode* node);
             };

       // Specification of Checker Traversal Implementation

          class Traversal
             : public AstSimpleProcessing, public Compass::TraversalBase
             {
            // Checker specific parameters should be allocated here.
	       std::string stringOutput;
               public:
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);
		    rose_hash::hash_map<std::string, int> instMap;

                 // Change the implementation of this function if you are using inherited attributes.
                    void *initialInheritedAttribute() const { return NULL; }

                 // The implementation of the run function has to match the traversal being called.
                 // If you use inherited attributes, use the following definition:
                 // void run(SgNode* n){ this->traverse(n, initialInheritedAttribute()); }
                    void run(SgNode* n){ this->traverse(n, preorder); 
		      std::cerr << "  running binPrintAsmInstuction checker... " << std::endl;
		      finish(n);
		      std::cerr << stringOutput << std::endl;
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

