// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Upper Range Limit Analysis
// Author: pants,,,
// Date: 14-August-2007

#include "compass.h"
#include "upperRangeLimit.h"

namespace CompassAnalyses
   { 
     namespace UpperRangeLimit
        { 
          const std::string checkerName      = "UpperRangeLimit";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Finding examples of using upper range inclusive";
          const std::string longDescription  = "Searches for instances of using the less intuitive"
          " inclusive upper range limit (greater than or equal to)";
        } //End of namespace UpperRangeLimit.
   } //End of namespace CompassAnalyses.

CompassAnalyses::UpperRangeLimit::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::UpperRangeLimit::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["UpperRangeLimit.YourParameter"]);


   }

void
CompassAnalyses::UpperRangeLimit::Traversal::
visit(SgNode* node)
   { 
     if (isSgGreaterOrEqualOp(node)) output->addOutput(new CheckerOutput(node));
   } //End of the visit function.
   
