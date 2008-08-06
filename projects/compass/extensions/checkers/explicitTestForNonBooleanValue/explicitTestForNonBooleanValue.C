// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Explicit Test For Non Boolean Value Analysis
// Author: Han Suk  Kim
// Date: 24-July-2007

#include "compass.h"
#include "explicitTestForNonBooleanValue.h"

namespace CompassAnalyses
   { 
     namespace ExplicitTestForNonBooleanValue
        { 
          const std::string checkerName      = "ExplicitTestForNonBooleanValue";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Explicit test statements are required.";
          const std::string longDescription  = "This checker tests if boolean expressions in conditional tests call a function returning a non-boolean value.";
        } //End of namespace ExplicitTestForNonBooleanValue.
   } //End of namespace CompassAnalyses.

CompassAnalyses::ExplicitTestForNonBooleanValue::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::ExplicitTestForNonBooleanValue::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["ExplicitTestForNonBooleanValue.YourParameter"]);


   }

void
CompassAnalyses::ExplicitTestForNonBooleanValue::Traversal::
visit(SgNode* node)
   { 
  // Implement your traversal here.  
        // 1. conditional expression
        if(NULL != isSgBasicBlock(node))
        {
          Rose_STL_Container<SgNode*> conditionalExpList = NodeQuery::querySubTree(node, V_SgConditionalExp);
          for(Rose_STL_Container<SgNode*>::iterator i=conditionalExpList.begin(); i != conditionalExpList.end(); i++)
          {
            SgConditionalExp* conditionalExp = isSgConditionalExp(*i);
            //ROSE_ASSERT(conditionalExp != NULL);

            if(NULL != conditionalExp && NULL != isSgCastExp(conditionalExp->get_conditional_exp()))
            {
              output->addOutput(new CheckerOutput(conditionalExp));
            }
          }
        } else {

          SgExprStatement* exprStatement = NULL;

          // 2. test statement in a if statement
          SgIfStmt* ifStmt = isSgIfStmt(node);
          if(NULL != ifStmt)
            exprStatement = isSgExprStatement(ifStmt->get_conditional());

          // 3. test statement in a while statement
          SgWhileStmt* whileStmt = isSgWhileStmt(node);
          if(NULL != whileStmt)
            exprStatement = isSgExprStatement(whileStmt->get_condition());

          // 4. test statement in a do-while statement
          SgDoWhileStmt* doWhileStmt = isSgDoWhileStmt(node);
          if(NULL != doWhileStmt)
            exprStatement = isSgExprStatement(doWhileStmt->get_condition());

          // 5. test statement in a for statement
          SgForStatement* forStatement = isSgForStatement(node);
          if(NULL != forStatement)
            exprStatement = isSgExprStatement(forStatement->get_test());

          if(NULL != exprStatement && NULL != isSgCastExp(exprStatement->get_expression()))
          {
            output->addOutput(new CheckerOutput(node));
          }
        }

   } //End of the visit function.
   
