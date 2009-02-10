// Empty Instead Of Size
// Author: Gergo  Barany
// Date: 29-August-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_EMPTY_INSTEAD_OF_SIZE_H
#define COMPASS_EMPTY_INSTEAD_OF_SIZE_H

namespace CompassAnalyses
   { 
     namespace EmptyInsteadOfSize
        { 
        /*! \brief Empty Instead Of Size: Add your description here 
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

// COMPASS_EMPTY_INSTEAD_OF_SIZE_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Empty Instead Of Size Analysis
// Author: Gergo Barany
// Date: 29-August-2007

#include "compass.h"
// #include "emptyInsteadOfSize.h"

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
   : output(output)
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

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::EmptyInsteadOfSize::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::EmptyInsteadOfSize::Traversal(params, output);
}

extern const Compass::Checker* const emptyInsteadOfSizeChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::EmptyInsteadOfSize::checkerName,
        CompassAnalyses::EmptyInsteadOfSize::shortDescription,
        CompassAnalyses::EmptyInsteadOfSize::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
