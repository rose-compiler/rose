// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Empty Instead Of Size Analysis
// Author: Gergo Barany
// Date: 29-August-2007

#include "compass.h"
#include "emptyInsteadOfSize.h"

namespace CompassAnalyses
   { 
     namespace EmptyInsteadOfSize
        { 
          const std::string checkerName      = "EmptyInsteadOfSize";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Use empty() member function "
            "instead of comparing the result of size() against 0.";
          const std::string longDescription  = "The empty() member function "
            "(if available) should be used instead of comparing the result of "
            "size() against 0.";
        } //End of namespace EmptyInsteadOfSize.
   } //End of namespace CompassAnalyses.

CompassAnalyses::EmptyInsteadOfSize::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::EmptyInsteadOfSize::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
   {
   }

void
CompassAnalyses::EmptyInsteadOfSize::Traversal::
visit(SgNode* node)
   { 
     if (SgFunctionCallExp *call = isSgFunctionCallExp(node))
     {
       SgNode *op = call->get_parent();
       // note that we do not check for all unequalities, only those that make
       // sense in this context
       if (isSgEqualityOp(op) || isSgGreaterThanOp(op) || isSgNotEqualOp(op))
       {
         SgValueExp *val = isSgValueExp(isSgBinaryOp(op)->get_rhs_operand_i());
         // if the rhs isn't a constant, maybe the lhs is?
         if (val == NULL)
           val = isSgValueExp(isSgBinaryOp(op)->get_lhs_operand_i());
         if (val != NULL)
         {
           std::string representation = val->unparseToString();
           if (representation == "0" || representation == "(0)")
           {
             if (isSgDotExp(call->get_function()) || isSgArrowExp(call->get_function()))
             {
               if (SgMemberFunctionRefExp *mfr = isSgMemberFunctionRefExp(isSgBinaryOp(call->get_function())->get_rhs_operand_i()))
               {
                 if (std::string(mfr->get_symbol()->get_name().str()) == "size")
                   output->addOutput(new CheckerOutput(op));
               }
             }
           }
         }
       }
     }
   } //End of the visit function.
