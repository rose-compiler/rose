// Place Constant On The Lhs
// Author: Han Suk  Kim
// Date: 24-July-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_PLACE_CONSTANT_ON_THE_LHS_H
#define COMPASS_PLACE_CONSTANT_ON_THE_LHS_H

namespace CompassAnalyses
   { 
     namespace PlaceConstantOnTheLhs
        { 
        /*! \brief Place Constant On The Lhs: Add your description here 
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

// COMPASS_PLACE_CONSTANT_ON_THE_LHS_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Place Constant On The Lhs Analysis
// Author: Han Suk  Kim
// Date: 24-July-2007

#include "compass.h"
// #include "placeConstantOnTheLhs.h"

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
   : output(output)
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
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::PlaceConstantOnTheLhs::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::PlaceConstantOnTheLhs::Traversal(params, output);
}

extern const Compass::Checker* const placeConstantOnTheLhsChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::PlaceConstantOnTheLhs::checkerName,
        CompassAnalyses::PlaceConstantOnTheLhs::shortDescription,
        CompassAnalyses::PlaceConstantOnTheLhs::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
