// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Right Shift Mask Analysis
// Author: pants,,,
// Date: 31-August-2007

#include "compass.h"
#include "rightShiftMask.h"

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
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
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
   
