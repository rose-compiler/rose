// Do Not Call Putenv With Auto Var
// Author: Mark Lewandowski, -422-3849
// Date: 30-August-2007

#include "compass.h"

#ifndef COMPASS_DO_NOT_CALL_PUTENV_WITH_AUTO_VAR_H
#define COMPASS_DO_NOT_CALL_PUTENV_WITH_AUTO_VAR_H

namespace CompassAnalyses
   { 
     namespace DoNotCallPutenvWithAutoVar
        { 
        /*! \brief Do Not Call Putenv With Auto Var: Add your description here 
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

// COMPASS_DO_NOT_CALL_PUTENV_WITH_AUTO_VAR_H
#endif 

