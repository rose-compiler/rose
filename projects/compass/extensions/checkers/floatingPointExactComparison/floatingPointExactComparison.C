// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Floating Point Exact Comparison Analysis
// Author: Han Suk  Kim
// Date: 24-July-2007

#include "compass.h"
#include "floatingPointExactComparison.h"

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
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
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
   
