// No Side Effect In Sizeof
// Author: Han Suk  Kim
// Date: 24-August-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_NO_SIDE_EFFECT_IN_SIZEOF_H
#define COMPASS_NO_SIDE_EFFECT_IN_SIZEOF_H

namespace CompassAnalyses
   { 
     namespace NoSideEffectInSizeof
        { 
        /*! \brief No Side Effect In Sizeof: Add your description here 
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

// COMPASS_NO_SIDE_EFFECT_IN_SIZEOF_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// No Side Effect In Sizeof Analysis
// Author: Han Suk  Kim
// Date: 24-August-2007

#include "compass.h"
// #include "noSideEffectInSizeof.h"

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
   : output(output)
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
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::NoSideEffectInSizeof::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::NoSideEffectInSizeof::Traversal(params, output);
}

extern const Compass::Checker* const noSideEffectInSizeofChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::NoSideEffectInSizeof::checkerName,
        CompassAnalyses::NoSideEffectInSizeof::shortDescription,
        CompassAnalyses::NoSideEffectInSizeof::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
