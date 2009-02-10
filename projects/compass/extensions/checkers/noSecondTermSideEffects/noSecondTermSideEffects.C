// No Second Term Side Effects
// Author: pants,,,
// Date: 24-August-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_NO_SECOND_TERM_SIDE_EFFECTS_H
#define COMPASS_NO_SECOND_TERM_SIDE_EFFECTS_H

namespace CompassAnalyses
   { 
     namespace NoSecondTermSideEffects
        { 
        /*! \brief No Second Term Side Effects: Add your description here 
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

// COMPASS_NO_SECOND_TERM_SIDE_EFFECTS_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// No Second Term Side Effects Analysis
// Author: pants,,,
// Date: 24-August-2007

#include "compass.h"
// #include "noSecondTermSideEffects.h"

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
   : output(output)
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
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::NoSecondTermSideEffects::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::NoSecondTermSideEffects::Traversal(params, output);
}

extern const Compass::Checker* const noSecondTermSideEffectsChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::NoSecondTermSideEffects::checkerName,
        CompassAnalyses::NoSecondTermSideEffects::shortDescription,
        CompassAnalyses::NoSecondTermSideEffects::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
