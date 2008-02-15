// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// No Overload Ampersand Analysis
// Author: pants,,,
// Date: 31-August-2007

#include "compass.h"
#include "noOverloadAmpersand.h"

namespace CompassAnalyses
   { 
     namespace NoOverloadAmpersand
        { 
          const std::string checkerName      = "NoOverloadAmpersand";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Finding overloaded operator&";
          const std::string longDescription  = "Finds member functions of the name operator& and raises an alert.";
        } //End of namespace NoOverloadAmpersand.
   } //End of namespace CompassAnalyses.

CompassAnalyses::NoOverloadAmpersand::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::NoOverloadAmpersand::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["NoOverloadAmpersand.YourParameter"]);


   }

void
CompassAnalyses::NoOverloadAmpersand::Traversal::
visit(SgNode* node)
   { 
  // Implement your traversal here.  
     SgMemberFunctionDeclaration *mfn =  isSgMemberFunctionDeclaration(node);
     std::string op = "operator&";
     if (!mfn) return;
     std::string fName = mfn->get_name().str();
     if (op == fName)
       {
         output->addOutput(new CheckerOutput(node));
       }
   } //End of the visit function.
   
