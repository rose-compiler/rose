// Non Virtual Redefinition
// Author: Gergo Barany
// Date: 31-July-2007

#include "compass.h"

#ifndef COMPASS_NON_VIRTUAL_REDEFINITION_H
#define COMPASS_NON_VIRTUAL_REDEFINITION_H

namespace CompassAnalyses
   { 
     namespace NonVirtualRedefinition
        { 
        /*! \brief Non Virtual Redefinition: Add your description here 
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
                 // Can a function of type a override a function of type b?
                 bool isOverridingType(SgFunctionType *a, SgFunctionType *b);
                 void checkBaseClasses(SgMemberFunctionDeclaration *mfd, SgClassDefinition *classdef);

               public:
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

                 // The implementation of the run function has to match the traversal being called.
                    void run(SgNode* n){ this->traverse(n, preorder); };

                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_NON_VIRTUAL_REDEFINITION_H
#endif 

