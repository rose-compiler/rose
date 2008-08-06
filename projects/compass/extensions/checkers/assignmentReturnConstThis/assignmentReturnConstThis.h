// Assignment Return Const This
// Author: pants,,,
// Date: 24-July-2007

#include "compass.h"

#ifndef COMPASS_ASSIGNMENT_RETURN_CONST_THIS_H
#define COMPASS_ASSIGNMENT_RETURN_CONST_THIS_H

namespace CompassAnalyses
   { 
     namespace AssignmentReturnConstThis
        { 
        /*! \brief Assignment Return Const This: Add your description here 
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

               public:
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

                 // The implementation of the run function has to match the traversal being called.
                    void run(SgNode* n){ this->traverse(n, preorder); };

                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_ASSIGNMENT_RETURN_CONST_THIS_H
#endif 

