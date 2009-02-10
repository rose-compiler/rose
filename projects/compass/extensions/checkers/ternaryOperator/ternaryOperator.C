// Ternary Operator
// Author: Han Suk  Kim
// Date: 01-August-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_TERNARY_OPERATOR_H
#define COMPASS_TERNARY_OPERATOR_H

namespace CompassAnalyses
   { 
     namespace TernaryOperator
        { 
        /*! \brief Ternary Operator: Add your description here 
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
             : public Compass::AstSimpleProcessingWithRunFunction
             {
            // Checker specific parameters should be allocated here.
               Compass::OutputObject* output;

               public:
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

                 // The implementation of the run function has to match the traversal being called.
                    void run(SgNode* n){ this->traverse(n, preorder); };

                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_TERNARY_OPERATOR_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Ternary Operator Analysis
// Author: Han Suk  Kim
// Date: 01-August-2007

#include "compass.h"
// #include "ternaryOperator.h"

namespace CompassAnalyses
   { 
     namespace TernaryOperator
        { 
          const std::string checkerName      = "TernaryOperator";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Do not use the ternary operator(?:) in expressions";
          const std::string longDescription  = "Evaluation of a complex condition is best achieved through explicit conditional statements. Using the conditional operator invites errors during maintenance.";
        } //End of namespace TernaryOperator.
   } //End of namespace CompassAnalyses.

CompassAnalyses::TernaryOperator::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::TernaryOperator::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["TernaryOperator.YourParameter"]);


   }

void
CompassAnalyses::TernaryOperator::Traversal::
visit(SgNode* node)
   { 
  // Implement your traversal here.  

     if(isSgConditionalExp(node))
       output->addOutput(new CheckerOutput(node));


   } //End of the visit function.
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::TernaryOperator::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::TernaryOperator::Traversal(params, output);
}

extern const Compass::Checker* const ternaryOperatorChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::TernaryOperator::checkerName,
        CompassAnalyses::TernaryOperator::shortDescription,
        CompassAnalyses::TernaryOperator::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
