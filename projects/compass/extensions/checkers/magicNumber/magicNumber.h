// Magic Number
// Author: Gergo Barany
// Date: 19-July-2007

#include "compass.h"

#ifndef COMPASS_MAGIC_NUMBER_H
#define COMPASS_MAGIC_NUMBER_H

namespace CompassAnalyses
   { 
     namespace MagicNumber
        { 
        /*! \brief Magic Number: Checks for integer or floating point
         *         literals occurring outside of initializer expressions. The
         *         user may choose to ignore some such constants (0 and 1 are
         *         likely candidates), this can be configured via the
         *         parameters.
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
                 std::vector<int> allowedIntegers;
                 std::vector<double> allowedFloats;

               public:
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

                 // The implementation of the run function has to match the traversal being called.
                    void run(SgNode* n){ this->traverse(n, preorder); };

                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_MAGIC_NUMBER_H
#endif 

