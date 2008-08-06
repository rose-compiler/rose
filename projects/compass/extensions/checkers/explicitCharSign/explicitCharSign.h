// Explicit Char Sign
// Author: Gary M. Yuan
// Date: 24-August-2007

#include "compass.h"

#ifndef COMPASS_EXPLICIT_CHAR_SIGN_H
#define COMPASS_EXPLICIT_CHAR_SIGN_H

namespace CompassAnalyses
   { 
     namespace ExplicitCharSign
        { 
        /*! \brief Explicit Char Sign: Add your description here 
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

// COMPASS_EXPLICIT_CHAR_SIGN_H
#endif 

