// Floating Point Exact Comparison
// Author: Han Suk  Kim
// Date: 24-July-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_FLOATING_POINT_EXACT_COMPARISON_H
#define COMPASS_FLOATING_POINT_EXACT_COMPARISON_H

namespace CompassAnalyses
   { 
     namespace FloatingPointExactComparison
        { 
        /*! \brief Floating Point Exact Comparison: Add your description here 
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

// COMPASS_FLOATING_POINT_EXACT_COMPARISON_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Floating Point Exact Comparison Analysis
// Author: Han Suk  Kim
// Date: 24-July-2007

#include "compass.h"
// #include "floatingPointExactComparison.h"

namespace CompassAnalyses
   { 
     namespace FloatingPointExactComparison
        { 
          const std::string checkerName      = "FloatingPointExactComparison";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Do not write code that expects floating point calculations to yield exact results.";
          const std::string longDescription  = "Equivalence tests for floating point values should use <, <=, >, >=, and not use == or !=. Floating point representations are platform dependent, so it is necessary to avoid exact comparisons.";
        } //End of namespace FloatingPointExactComparison.
   } //End of namespace CompassAnalyses.

CompassAnalyses::FloatingPointExactComparison::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::FloatingPointExactComparison::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["FloatingPointExactComparison.YourParameter"]);
   }

void
CompassAnalyses::FloatingPointExactComparison::Traversal::
visit(SgNode* node)
   { 
  // Implement your traversal here.  

     SgExprStatement* exprStatement = NULL;

     SgIfStmt* ifStmt = isSgIfStmt(node);
     if(NULL != ifStmt)
       exprStatement = isSgExprStatement(ifStmt->get_conditional());

     SgWhileStmt* whileStmt = isSgWhileStmt(node);
     if(NULL != whileStmt)
       exprStatement = isSgExprStatement(whileStmt->get_condition());

     SgDoWhileStmt* doWhileStmt = isSgDoWhileStmt(node);
     if(NULL != doWhileStmt)
       exprStatement = isSgExprStatement(doWhileStmt->get_condition());

     SgForStatement* forStatement = isSgForStatement(node);
     if(NULL != forStatement)
       exprStatement = isSgExprStatement(forStatement->get_test());

     if(NULL != exprStatement && NULL != isSgNotEqualOp(exprStatement->get_expression()))
     {
       SgNotEqualOp* comparison = isSgNotEqualOp(exprStatement->get_expression());
       if((comparison->get_lhs_operand_i() != NULL &&
           isSgDoubleVal(comparison->get_lhs_operand_i()) != NULL) ||
          (comparison->get_rhs_operand_i() != NULL &&
           isSgDoubleVal(comparison->get_rhs_operand_i()) != NULL))
       {
         output->addOutput(new CheckerOutput(comparison));
       }
     }

     if(NULL != exprStatement && NULL != isSgEqualityOp(exprStatement->get_expression()))
     {
       SgEqualityOp* comparison = isSgEqualityOp(exprStatement->get_expression());
       if((comparison->get_lhs_operand_i() != NULL &&
           isSgDoubleVal(comparison->get_lhs_operand_i()) != NULL) ||
          (comparison->get_rhs_operand_i() != NULL &&
           isSgDoubleVal(comparison->get_rhs_operand_i()) != NULL))
       {
         output->addOutput(new CheckerOutput(comparison));
       }
     }

   } //End of the visit function.
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::FloatingPointExactComparison::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::FloatingPointExactComparison::Traversal(params, output);
}

extern const Compass::Checker* const floatingPointExactComparisonChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::FloatingPointExactComparison::checkerName,
        CompassAnalyses::FloatingPointExactComparison::shortDescription,
        CompassAnalyses::FloatingPointExactComparison::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
