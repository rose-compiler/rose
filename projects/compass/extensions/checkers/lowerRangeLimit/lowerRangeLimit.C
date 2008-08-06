// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Lower Range Limit Analysis
// Author: pants,,,
// Date: 14-August-2007

#include "compass.h"
#include "lowerRangeLimit.h"

namespace CompassAnalyses
   { 
     namespace LowerRangeLimit
        { 
          const std::string checkerName      = "LowerRangeLimit";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Finding examples of using lower range exclusive";
          const std::string longDescription  = "Searches for instances of using less intuitive lower range limit exclusive (strictly less than).";
        } //End of namespace LowerRangeLimit.
   } //End of namespace CompassAnalyses.

CompassAnalyses::LowerRangeLimit::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::LowerRangeLimit::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["LowerRangeLimit.YourParameter"]);


   }

void
CompassAnalyses::LowerRangeLimit::Traversal::
visit(SgNode* node)
   { 
     if (isSgLessThanOp(node)) output->addOutput(new CheckerOutput(node));
   } //End of the visit function.
   
