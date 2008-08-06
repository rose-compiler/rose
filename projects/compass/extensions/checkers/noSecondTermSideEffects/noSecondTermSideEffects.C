// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// No Second Term Side Effects Analysis
// Author: pants,,,
// Date: 24-August-2007

#include "compass.h"
#include "noSecondTermSideEffects.h"

namespace CompassAnalyses
   { 
     namespace NoSecondTermSideEffects
        { 
          const std::string checkerName      = "NoSecondTermSideEffects";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Finding any 'and' or 'or' with a side effect somewhere on the right hand side.";
          const std::string longDescription  = "Finds any and/or operator that has a statement with side effects in the right hand side.  Does not include function calls.";
        } //End of namespace NoSecondTermSideEffects.
   } //End of namespace CompassAnalyses.

CompassAnalyses::NoSecondTermSideEffects::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::NoSecondTermSideEffects::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["NoSecondTermSideEffects.YourParameter"]);


   }

void
CompassAnalyses::NoSecondTermSideEffects::Traversal::
visit(SgNode* node)
   { 
     SgAndOp *anAnd = isSgAndOp(node);
     SgOrOp *anOr = isSgOrOp(node);
     if (!anAnd and !anOr) return;
     SgExpression *rHand;
     if (anAnd)
       rHand = anAnd->get_rhs_operand_i ();
     else
       rHand = anOr->get_rhs_operand_i ();
     if (
         (NodeQuery::querySubTree(rHand,V_SgAssignOp)).size() or
         (NodeQuery::querySubTree(rHand,V_SgMinusAssignOp)).size() or
         (NodeQuery::querySubTree(rHand,V_SgPlusAssignOp)).size() or
         (NodeQuery::querySubTree(rHand,V_SgPlusPlusOp)).size() or
         (NodeQuery::querySubTree(rHand,V_SgMinusMinusOp)).size() or
         (NodeQuery::querySubTree(rHand,V_SgMultAssignOp)).size() or
         (NodeQuery::querySubTree(rHand,V_SgAndAssignOp)).size() or
         (NodeQuery::querySubTree(rHand,V_SgDivAssignOp)).size() or
         (NodeQuery::querySubTree(rHand,V_SgIorAssignOp)).size() or
         (NodeQuery::querySubTree(rHand,V_SgLshiftAssignOp)).size() or
         (NodeQuery::querySubTree(rHand,V_SgModAssignOp)).size() or
         (NodeQuery::querySubTree(rHand,V_SgRshiftAssignOp)).size() or
         (NodeQuery::querySubTree(rHand,V_SgXorAssignOp)).size()
        )
       {
              output->addOutput(new CheckerOutput(node));
       }
          
  // Implement your traversal here.  

   } //End of the visit function.
   
