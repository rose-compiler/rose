// Unary Minus
// Author: Gergo  Barany
// Date: 07-August-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_UNARY_MINUS_H
#define COMPASS_UNARY_MINUS_H

namespace CompassAnalyses
   { 
     namespace UnaryMinus
        { 
        /*! \brief Unary Minus: Add your description here 
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

// COMPASS_UNARY_MINUS_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Unary Minus Analysis
// Author: Gergo  Barany
// Date: 07-August-2007

#include "compass.h"
// #include "unaryMinus.h"

namespace CompassAnalyses
   { 
     namespace UnaryMinus
        { 
          const std::string checkerName      = "UnaryMinus";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Unary minus should only be used with signed types.";
          const std::string longDescription  = "The unary minus operator "
            "should only be used with operands of signed type.";
        } //End of namespace UnaryMinus.
   } //End of namespace CompassAnalyses.

CompassAnalyses::UnaryMinus::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::UnaryMinus::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
   }

void
CompassAnalyses::UnaryMinus::Traversal::
visit(SgNode* node)
   { 
     if (SgMinusOp *minus = isSgMinusOp(node))
     {
       SgType *t = minus->get_operand()->get_type();
       if (t->isUnsignedType())
         output->addOutput(new CheckerOutput(minus));
     }
   } //End of the visit function.
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::UnaryMinus::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::UnaryMinus::Traversal(params, output);
}

extern const Compass::Checker* const unaryMinusChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::UnaryMinus::checkerName,
        CompassAnalyses::UnaryMinus::shortDescription,
        CompassAnalyses::UnaryMinus::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
