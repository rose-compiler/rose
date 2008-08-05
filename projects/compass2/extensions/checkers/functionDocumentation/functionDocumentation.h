// Function Documentation
// Author: Thomas Panas
// Date: 23-July-2007

#include "compass.h"

#ifndef COMPASS_FUNCTION_DOCUMENTATION_H
#define COMPASS_FUNCTION_DOCUMENTATION_H

namespace CompassAnalyses
   { 
     namespace FunctionDocumentation
        { 
        /*! \brief Function Documentation: Add your description here 
         */

          extern const std::string checkerName;
          extern  std::string shortDescription;
          extern const std::string longDescription;

       // Specification of Checker Output Implementation
          class CheckerOutput: public Compass::OutputViolationBase
             { 
               public:
                    CheckerOutput(SgNode* node, std::string funcname);
             };

       // Specification of Checker Traversal Implementation

          class Traversal
             : public AstSimpleProcessing, public Compass::TraversalBase
             {
            // Checker specific parameters should be allocated here.
               public:
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

                 // The implementation of the run function has to match the traversal being called.
                    void run(SgNode* n){ this->traverse(n, preorder); };
                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_FUNCTION_DOCUMENTATION_H
#endif 

