// Dynamic Cast
// Author: Andreas Saebjoernsen
// Date: 23-July-2007

#include "compass.h"

#ifndef COMPASS_DYNAMIC_CAST_H
#define COMPASS_DYNAMIC_CAST_H

namespace CompassAnalyses
   { 
     namespace DynamicCast
        { 
        /*! \brief Dynamic Cast: Add your description here 
         */

          extern const std::string checkerName;
          extern const std::string shortDescription;
          extern const std::string longDescription;

       // Specification of Checker Output Implementation
          class CheckerOutput: public Compass::OutputViolationBase
             {
                    SgType* toType;
                    SgType* fromType;
                    SgCastExp* IRnode;
               public:
                    CheckerOutput(SgType* to, SgType* from,
                              SgCastExp* node);

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

// COMPASS_DYNAMIC_CAST_H
#endif 

