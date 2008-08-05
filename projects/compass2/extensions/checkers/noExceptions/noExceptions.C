// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// No Exceptions Analysis
// Author: Andreas Saebjornsen
// Date: 25-July-2007

#include "compass.h"
#include "noExceptions.h"

namespace CompassAnalyses
   { 
     namespace NoExceptions
        { 
          const std::string checkerName      = "NoExceptions";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "C++ exception handling should not be used.";
          const std::string longDescription  = "C++ exception handling should not be used.";
        } //End of namespace NoExceptions.
   } //End of namespace CompassAnalyses.

CompassAnalyses::NoExceptions::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::NoExceptions::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["NoExceptions.YourParameter"]);


   }

void
CompassAnalyses::NoExceptions::Traversal::
visit(SgNode* node)
   {
     //Detect all places where C++ exceptions are used.
     switch(node->variantT()){
      case V_SgThrowOp:
      case V_SgCatchOptionStmt:
      case V_SgTryStmt:
         getOutput()->addOutput(new CheckerOutput(node));
         break;
      default:
         break;
    }
   } //End of the visit function.
   
