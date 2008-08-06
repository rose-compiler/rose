// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Do Not Use C-style Casts Analysis
// Author: Mark Lewandowski, -422-3849
// Date: 18-December-2007

#include "compass.h"
#include "doNotUseCstyleCasts.h"

namespace CompassAnalyses
   { 
     namespace DoNotUseCstyleCasts
        { 
          const std::string checkerName      = "DoNotUseCstyleCasts";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "C++ casts allow for more compiler checking and are easier to find in source code (either by tools or by human readers).";
          const std::string longDescription  = "C++ allows C-style casts, although it has introduced its own casts:\n\t* static_cast<type>(expression)\n\t* const_cast<type>(expression)\n\t* dynamic_cast<type>(expression)\n\t* reinterpret_cast<type>(expression)\nC++ casts allow for more compiler checking and are easier to find in source code (either by tools or by human readers).";
        } //End of namespace DoNotUseCstyleCasts.
   } //End of namespace CompassAnalyses.

CompassAnalyses::DoNotUseCstyleCasts::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::DoNotUseCstyleCasts::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["DoNotUseCstyleCasts.YourParameter"]);


   }

void
CompassAnalyses::DoNotUseCstyleCasts::Traversal::
visit(SgNode* node)
   {
     SgCastExp* cast = isSgCastExp(node);

     if (!cast) {
       return;
     }

     if (cast->get_cast_type() == SgCastExp::e_C_style_cast) {
       output->addOutput(new CheckerOutput(node));
     }
   } //End of the visit function.
   
