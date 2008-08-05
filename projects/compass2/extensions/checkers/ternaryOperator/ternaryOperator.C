// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Ternary Operator Analysis
// Author: Han Suk  Kim
// Date: 01-August-2007

#include "compass.h"
#include "ternaryOperator.h"

namespace CompassAnalyses
   { 
     namespace TernaryOperator
        { 
          const std::string checkerName      = "TernaryOperator";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Do not use the ternary operator(?:) in expressions";
          const std::string longDescription  = "Evaluation of a complex condition is best achieved through explicit conditional statements. Using the conditional operator invites errors during maintenance.";
        } //End of namespace TernaryOperator.
   } //End of namespace CompassAnalyses.

CompassAnalyses::TernaryOperator::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::TernaryOperator::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["TernaryOperator.YourParameter"]);


   }

void
CompassAnalyses::TernaryOperator::Traversal::
visit(SgNode* node)
   { 
  // Implement your traversal here.  

     if(isSgConditionalExp(node))
       output->addOutput(new CheckerOutput(node));


   } //End of the visit function.
   
