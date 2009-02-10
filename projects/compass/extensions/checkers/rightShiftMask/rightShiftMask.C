// Right Shift Mask
// Author: pants,,,
// Date: 31-August-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_RIGHT_SHIFT_MASK_H
#define COMPASS_RIGHT_SHIFT_MASK_H

namespace CompassAnalyses
   { 
     namespace RightShiftMask
        { 
        /*! \brief Right Shift Mask: Add your description here 
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

// COMPASS_RIGHT_SHIFT_MASK_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Right Shift Mask Analysis
// Author: pants,,,
// Date: 31-August-2007

#include "compass.h"
// #include "rightShiftMask.h"

namespace CompassAnalyses
   { 
     namespace RightShiftMask
        { 
          const std::string checkerName      = "RightShiftMask";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Finding a RightShift with no Bit Mask";
          const std::string longDescription  = "Makes sure that ";
        } //End of namespace RightShiftMask.
   } //End of namespace CompassAnalyses.

CompassAnalyses::RightShiftMask::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::RightShiftMask::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["RightShiftMask.YourParameter"]);


   }

void
CompassAnalyses::RightShiftMask::Traversal::
visit(SgNode* node)
   {
     if (not (isSgRshiftAssignOp(node) or isSgRshiftOp(node))) return;
     SgNode* p = node->get_parent();
     while (p)
       {
         if (isSgBitAndOp(p) or isSgAndAssignOp(p)) return;
         if (isSgBasicBlock(p))
           {
           output->addOutput(new CheckerOutput(node));
           return;
           }
         p = p->get_parent();
       }
     //what do we do? return error? this means we found a shift with
     //no and... hrm...
     output->addOutput(new CheckerOutput(node));
   } 
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::RightShiftMask::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::RightShiftMask::Traversal(params, output);
}

extern const Compass::Checker* const rightShiftMaskChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::RightShiftMask::checkerName,
        CompassAnalyses::RightShiftMask::shortDescription,
        CompassAnalyses::RightShiftMask::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
