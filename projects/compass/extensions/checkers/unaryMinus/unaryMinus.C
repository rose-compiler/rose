// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Unary Minus Analysis
// Author: Gergo  Barany
// Date: 07-August-2007

#include "compass.h"
#include "unaryMinus.h"

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
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
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
   
