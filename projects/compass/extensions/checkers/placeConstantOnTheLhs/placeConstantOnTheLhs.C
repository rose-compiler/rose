// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Place Constant On The Lhs Analysis
// Author: Han Suk  Kim
// Date: 24-July-2007

#include "compass.h"
#include "placeConstantOnTheLhs.h"

namespace CompassAnalyses
   { 
     namespace PlaceConstantOnTheLhs
        { 
          const std::string checkerName      = "PlaceConstantOnTheLhs";

       // Descriptions should not include the newline character "\n".

          const std::string shortDescription = "Place the constant on the left hand side in this comparison!";
          const std::string longDescription  = "This checker detects a test clause whether or not it contains a constant on the left hand side when comparing a varialbe and the constant for equality. By putting the constant on the left hand side, the compiler can prevent programmers from making mistake to write '=' for '=='.";
        } //End of namespace PlaceConstantOnTheLhs.
   } //End of namespace CompassAnalyses.

CompassAnalyses::PlaceConstantOnTheLhs::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::PlaceConstantOnTheLhs::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["PlaceConstantOnTheLhs.YourParameter"]);


   }

void
CompassAnalyses::PlaceConstantOnTheLhs::Traversal::
visit(SgNode* node)
   { 
  // Implement your traversal here.  
			SgExprStatement* comparisonExpr = NULL;

			if(isSgIfStmt(node) != NULL)
			{
				SgIfStmt* ifStatement = isSgIfStmt(node);
				ROSE_ASSERT(ifStatement != NULL);

				comparisonExpr = isSgExprStatement(ifStatement->get_conditional());
			} 
			else if(isSgWhileStmt(node) != NULL)
			{
				SgWhileStmt* whileStatement = isSgWhileStmt(node);
				ROSE_ASSERT(whileStatement != NULL);

				comparisonExpr = isSgExprStatement(whileStatement->get_condition());
			}
			else if(isSgDoWhileStmt(node) != NULL)
			{
				SgDoWhileStmt* doWhileStmt = isSgDoWhileStmt(node);
				ROSE_ASSERT(doWhileStmt != NULL);

				comparisonExpr = isSgExprStatement(doWhileStmt->get_condition());
			}
			else if(isSgForStatement(node) != NULL)
			{
				SgForStatement* forStatement = isSgForStatement(node);
				ROSE_ASSERT(forStatement != NULL);

				comparisonExpr = isSgExprStatement(forStatement->get_test());
			}

			if(comparisonExpr != NULL)
			{
        Rose_STL_Container<SgNode*> comparisonList =  NodeQuery::querySubTree(comparisonExpr, V_SgEqualityOp);
				
				for(Rose_STL_Container<SgNode*>::iterator comparisonListItr = comparisonList.begin(); comparisonListItr != comparisonList.end(); comparisonListItr++)
				{
					SgEqualityOp* equalityExpression = isSgEqualityOp(*comparisonListItr);
					ROSE_ASSERT(equalityExpression != NULL);

					if(equalityExpression->get_lhs_operand() != NULL 
							&& isSgVarRefExp(equalityExpression->get_lhs_operand()) != NULL)
					{
            output->addOutput(new CheckerOutput(comparisonExpr));

						//output->addOutput(new PlaceConstantsOnTheLhsCheckerOutput(equalityExpression->get_startOfConstruct()));
					}
				}
			}


   } //End of the visit function.
   
