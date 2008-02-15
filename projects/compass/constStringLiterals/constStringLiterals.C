// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Const String Literals Analysis
// Author: Gergo Barany
// Date: 07-September-2007

#include "compass.h"
#include "constStringLiterals.h"

namespace CompassAnalyses
   { 
     namespace ConstStringLiterals
        { 
          const std::string checkerName      = "ConstStringLiterals";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "String literals should be treated as const char *";
          const std::string longDescription  = "String literals should be treated as const char *";
        } //End of namespace ConstStringLiterals.
   } //End of namespace CompassAnalyses.

CompassAnalyses::ConstStringLiterals::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::ConstStringLiterals::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["ConstStringLiterals.YourParameter"]);


   }

void
CompassAnalyses::ConstStringLiterals::Traversal::
visit(SgNode* node)
   { 
     SgStringVal *str = isSgStringVal(node);
     if (str)
     {
       SgNode *parent = str->get_parent();
       SgCastExp *cast = isSgCastExp(parent);
       if (cast && !isSgCastExp(cast->get_parent()))
         output->addOutput(new CheckerOutput(cast->get_parent()));
     }
   } //End of the visit function.
   
