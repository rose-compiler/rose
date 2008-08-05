// Variable Name Equals Database Name
// Author: Andreas Saebjoernsen,,,
// Date: 12-August-2007

#include "compass.h"

#ifndef COMPASS_VARIABLE_NAME_EQUALS_DATABASE_NAME_H
#define COMPASS_VARIABLE_NAME_EQUALS_DATABASE_NAME_H

namespace CompassAnalyses
   { 
     namespace VariableNameEqualsDatabaseName
        { 
        /*! \brief Variable Name Equals Database Name: Add your description here 
         */

          extern const std::string checkerName;
          extern const std::string shortDescription;
          extern const std::string longDescription;

       // Specification of Checker Output Implementation
          class CheckerOutput: public Compass::OutputViolationBase
             { 
                    SgNode* violatingDeclaration;
               public:
                    CheckerOutput(SgNode* node);
             };

       // Specification of Checker Traversal Implementation

          class Traversal
             : public AstSimpleProcessing, public Compass::TraversalBase
             {
            // Checker specific parameters should be allocated here.
                    std::string classToLookFor;
                    std::string memberFunctionToLookFor;
                    SgNode* assignExp;

               public:
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

                 // The implementation of the run function has to match the traversal being called.
                    void run(SgNode* n){ this->traverse(n, preorder); };

                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_VARIABLE_NAME_EQUALS_DATABASE_NAME_H
#endif 

