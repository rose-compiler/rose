// Comma Operator
// Author: Gergo  Barany
// Date: 07-August-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_COMMA_OPERATOR_H
#define COMPASS_COMMA_OPERATOR_H

namespace CompassAnalyses
   { 
     namespace CommaOperator
        { 
        /*! \brief Comma Operator: Add your description here 
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

// COMPASS_COMMA_OPERATOR_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Comma Operator Analysis
// Author: Gergo  Barany
// Date: 07-August-2007

#include "compass.h"
// #include "commaOperator.h"
#include <cstring>

namespace CompassAnalyses
   { 
     namespace CommaOperator
        { 
          const std::string checkerName      = "CommaOperator";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "The comma operator should not be used.";
          const std::string longDescription  = "The comma operator should not be used.";
        } //End of namespace CommaOperator.
   } //End of namespace CompassAnalyses.

CompassAnalyses::CommaOperator::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::CommaOperator::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
   }

void
CompassAnalyses::CommaOperator::Traversal::
visit(SgNode* node)
   { 
     if (isSgCommaOpExp(node))
     {
       // simple case: built-in comma operator
       output->addOutput(new CheckerOutput(node));
     }
     else if (SgFunctionDeclaration *fd = isSgFunctionDeclaration(node))
     {
       if (std::strcmp(fd->get_name().str(), "operator,") == 0)
         output->addOutput(new CheckerOutput(node));
     }
   } //End of the visit function.
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::CommaOperator::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::CommaOperator::Traversal(params, output);
}

extern const Compass::Checker* const commaOperatorChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::CommaOperator::checkerName,
        CompassAnalyses::CommaOperator::shortDescription,
        CompassAnalyses::CommaOperator::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
