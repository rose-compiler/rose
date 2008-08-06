// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// No Side Effect In Sizeof Analysis
// Author: Han Suk  Kim
// Date: 24-August-2007

#include "compass.h"
#include "noSideEffectInSizeof.h"

namespace CompassAnalyses
   { 
     namespace NoSideEffectInSizeof
        { 
          const std::string checkerName      = "NoSideEffectInSizeof";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Operands to the sizeof operator should not contain side effects";
          const std::string longDescription  = "The sizeof operator yields the size (in bytes) of its operand, which may be an expression or the parenthesized name of a type. If the type of the operand is not a variable length array type the operand is not evaluated. Providing an expression that appears to produce side effects may be misleading to programmers who are not aware that these expressions are not evaluated. As a result, programmers may make invalid assumptions about program state leading to errors and possible software vulnerabilities.";
        } //End of namespace NoSideEffectInSizeof.
   } //End of namespace CompassAnalyses.

CompassAnalyses::NoSideEffectInSizeof::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::NoSideEffectInSizeof::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["NoSideEffectInSizeof.YourParameter"]);


   }

void
CompassAnalyses::NoSideEffectInSizeof::Traversal::
visit(SgNode* node)
   { 
  // Implement your traversal here.  
     if(isSgVariableDeclaration(node) || isSgExprStatement(node))
     {
       Rose_STL_Container<SgNode*> sizeOfOpList = NodeQuery::querySubTree(node, V_SgSizeOfOp);

       for(Rose_STL_Container<SgNode*>::iterator i = sizeOfOpList.begin(); i != sizeOfOpList.end(); i++)
       {
         SgSizeOfOp* op = isSgSizeOfOp(*i);
         ROSE_ASSERT(op != NULL);

         SgExpression* expr = op->get_operand_expr();

         if(isSgPlusPlusOp(expr) || isSgMinusMinusOp(expr) || isSgAssignOp(expr))
           output->addOutput(new CheckerOutput(op));
       }

     }
   } //End of the visit function.
   
