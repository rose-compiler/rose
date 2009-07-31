// Prefer Algorithms
// Author: Gergo  Barany
// Date: 29-August-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_PREFER_ALGORITHMS_H
#define COMPASS_PREFER_ALGORITHMS_H

namespace CompassAnalyses
   { 
     namespace PreferAlgorithms
        { 
        /*! \brief Prefer Algorithms: Add your description here 
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
                 bool checkForAssignment(SgStatement *stmt);
                 bool checkForComparison(SgExpression *expr);
                 bool checkForIncrementOrDecrement(SgExpression *expr);

               public:
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

                 // The implementation of the run function has to match the traversal being called.
                    void run(SgNode* n){ this->traverse(n, preorder); };

                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_PREFER_ALGORITHMS_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Prefer Algorithms Analysis
// Author: Gergo Barany
// Date: 29-August-2007

#include "compass.h"
// #include "preferAlgorithms.h"

namespace CompassAnalyses
   { 
     namespace PreferAlgorithms
        { 
          const std::string checkerName      = "PreferAlgorithms";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Consider using an STL algorithm instead of a loop.";
          const std::string longDescription  = "Consider using an STL algorithm instead of a loop.";
        } //End of namespace PreferAlgorithms.
   } //End of namespace CompassAnalyses.

CompassAnalyses::PreferAlgorithms::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::PreferAlgorithms::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
   }

void
CompassAnalyses::PreferAlgorithms::Traversal::
visit(SgNode* node)
   { 
     if (SgForStatement *fors = isSgForStatement(node))
     {
       bool assignmentInInit = false,
            comparisonInCondition = false,
            incrementOrDecrement = false;

       SgStatementPtrList &init = fors->get_for_init_stmt()->get_init_stmt();
       assignmentInInit = (init.size() == 1 ? checkForAssignment(init.front()) : false);

       SgExprStatement *cond = isSgExprStatement(fors->get_test());
       comparisonInCondition = (cond ? cond->get_expression() : false);

       incrementOrDecrement = checkForIncrementOrDecrement(fors->get_increment());

       if (assignmentInInit && comparisonInCondition && incrementOrDecrement)
         output->addOutput(new CheckerOutput(fors));
     }
   } //End of the visit function.

bool
CompassAnalyses::PreferAlgorithms::Traversal::
checkForAssignment(SgStatement *stmt)
{
  // this is an assignment if it is one of:
  // - an expression statement with an assignment op
  // - an expression statement with a function call to "operator="
  // - a variable declaration with an initializer
  // and the expression in question is not of integer or floating type
  SgExprStatement *exprStmt = isSgExprStatement(stmt);
  SgFunctionCallExp *call = NULL;
  SgVariableDeclaration *varDecl = isSgVariableDeclaration(stmt);

  if (exprStmt)
  {
    if (exprStmt->get_expression()->get_type()->isIntegerType())
      return false;
    if (exprStmt->get_expression()->get_type()->isFloatType())
      return false;
  }
  
  if (exprStmt && isSgAssignOp(exprStmt->get_expression()))
    return true;
  
  if (exprStmt && (call = isSgFunctionCallExp(exprStmt->get_expression())))
  {
    SgFunctionRefExp *fref = isSgFunctionRefExp(call->get_function());
    if (fref && std::string(fref->get_symbol()->get_name().str()) == "operator=")
      return true;
  }

  if (varDecl && varDecl->get_variables().size() == 1)
  {
    SgInitializedName *var = varDecl->get_variables().front();
    if (var->get_initializer() != NULL)
      return true;
  }

  return false;
}

bool
CompassAnalyses::PreferAlgorithms::Traversal::
checkForComparison(SgExpression *expr)
{
  // this is a comparison if it is one of:
  // - an expression with operator !=, <, <=, >, or >=
  // - an expression with a function call to "operator!="
  // and the expression in question is not of integer or floating type
  if (expr->get_type()->isIntegerType() || expr->get_type()->isFloatType())
    return false;

  if (isSgNotEqualOp(expr) || isSgLessThanOp(expr) || isSgLessOrEqualOp(expr)
      || isSgGreaterThanOp(expr) || isSgGreaterOrEqualOp(expr))
    return true;

  if (SgFunctionCallExp *call = isSgFunctionCallExp(expr))
  {
    SgFunctionRefExp *fref = isSgFunctionRefExp(call->get_function());
    if (fref && std::string(fref->get_symbol()->get_name().str()) == "operator!=")
      return true;
  }

  return false;
}

bool
CompassAnalyses::PreferAlgorithms::Traversal::
checkForIncrementOrDecrement(SgExpression *expr)
{
  // this is an increment or decrement if it is one of:
  // - an expression with operator ++ or --
  // - an expression with a member function call to "operator++" or "operator--"
  // and the expression in question is not of integer or floating type
  if (expr->get_type()->isIntegerType() || expr->get_type()->isFloatType())
    return false;

  if (isSgPlusPlusOp(expr) || isSgMinusMinusOp(expr))
    return true;

  if (SgFunctionCallExp *call = isSgFunctionCallExp(expr))
  {
    SgBinaryOp *dot = NULL;
    if (isSgDotExp(call->get_function()) || isSgArrowExp(call->get_function()))
      dot = isSgBinaryOp(call->get_function());

 // DQ (7/26/2009): For C code "dot" can be a NULL pointer and so we have to skip the following code.
    if (dot != NULL)
       {
         SgMemberFunctionRefExp *mfref = isSgMemberFunctionRefExp(dot->get_rhs_operand_i());
         if (mfref && std::string(mfref->get_symbol()->get_name().str()) == "operator++")
           return true;
         if (mfref && std::string(mfref->get_symbol()->get_name().str()) == "operator--")
           return true;
       }
  }

  return false;
}

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::PreferAlgorithms::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::PreferAlgorithms::Traversal(params, output);
}

extern const Compass::Checker* const preferAlgorithmsChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::PreferAlgorithms::checkerName,
        CompassAnalyses::PreferAlgorithms::shortDescription,
        CompassAnalyses::PreferAlgorithms::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
