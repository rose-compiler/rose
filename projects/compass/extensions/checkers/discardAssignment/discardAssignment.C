// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Discard Assignment Analysis
// Author: Gergo Barany
// Date: 07-August-2007

#include "compass.h"
#include "discardAssignment.h"
#include <cstring>

namespace CompassAnalyses
   { 
     namespace DiscardAssignment
        { 
          const std::string checkerName      = "DiscardAssignment";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Result of assignment should be discarded.";
          const std::string longDescription  = "Always discard the result of "
            "the assignment operator to avoid confusion with the comparison "
            "operator.";
        } //End of namespace DiscardAssignment.
   } //End of namespace CompassAnalyses.

CompassAnalyses::DiscardAssignment::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::DiscardAssignment::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
   {
   }

void
CompassAnalyses::DiscardAssignment::Traversal::
visit(SgNode* node)
   { 
     if (isSgAssignOp(node))
     {
       // simple case: the parent of a "real" assignment op must be an
       // expression statement
       if (!isSgExprStatement(node->get_parent()))
       {
         output->addOutput(new CheckerOutput(node));
       }
       else
       {
         // not so simple case: the parent is an expression statement, but if
         // that statement is an if/loop condition, we still want to complain
         SgNode *assignment = node->get_parent();
         SgNode *p = assignment->get_parent();
         SgDoWhileStmt *dws;
         SgForStatement *fs;
         SgIfStmt *is;
         SgSwitchStatement *ss;
         SgWhileStmt *ws;

         if ((dws = isSgDoWhileStmt(p)) && dws->get_condition() == assignment)
         {
           output->addOutput(new CheckerOutput(node));
         }
         else if ((fs = isSgForStatement(p)) && fs->get_test() == assignment)
         {
           output->addOutput(new CheckerOutput(node));
         }
         else if ((is = isSgIfStmt(p)) && is->get_conditional() == assignment)
         {
           output->addOutput(new CheckerOutput(node));
         }
         else if ((ss = isSgSwitchStatement(p)) && ss->get_item_selector() == assignment)
         {
           output->addOutput(new CheckerOutput(node));
         }
         else if ((ws = isSgWhileStmt(p)) && ws->get_condition() == assignment)
         {
           output->addOutput(new CheckerOutput(node));
         }
       }
     }
     else if (SgMemberFunctionRefExp *mf = isSgMemberFunctionRefExp(node))
     {
       // not so simple case: call to operator= member function that is not an
       // expression statement by itself
       SgFunctionCallExp *call = isSgFunctionCallExp(mf->get_parent()->get_parent());
       if (call && !isSgExprStatement(call->get_parent())
        && mf->get_parent() == call->get_function()
        && std::strcmp(mf->get_symbol()->get_name().str(), "operator=") == 0)
       {
         output->addOutput(new CheckerOutput(call));
       }
     }
   } //End of the visit function.
