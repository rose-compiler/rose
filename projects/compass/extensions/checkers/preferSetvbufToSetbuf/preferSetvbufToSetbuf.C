// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Prefer Setvbuf To Setbuf Analysis
// Author: Mark Lewandowski, -422-3849
// Date: 20-November-2007

#include "compass.h"
#include "preferSetvbufToSetbuf.h"

namespace CompassAnalyses
   { 
     namespace PreferSetvbufToSetbuf
        { 
          const std::string checkerName      = "PreferSetvbufToSetbuf";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "For added error checking, prefer using setvbuf() over setbuf()";
          const std::string longDescription  = "setvbuf() is equivalent to setbuf() with _IOFBF for mode and BUFSIZE for size (if buf is not NULL) or _IONBF for mode (if buf is NULL), except that it returns a nonzero value if the request could not be honored. For added error checking, prefer using setvbuf() over setbuf()";
        } //End of namespace PreferSetvbufToSetbuf.
   } //End of namespace CompassAnalyses.

CompassAnalyses::PreferSetvbufToSetbuf::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::PreferSetvbufToSetbuf::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["PreferSetvbufToSetbuf.YourParameter"]);


   }

void
CompassAnalyses::PreferSetvbufToSetbuf::Traversal::
visit(SgNode* node)
   { 
     SgFunctionRefExp* func= isSgFunctionRefExp(node);

     if (func) {
       if (func->get_symbol()->get_name().getString().compare("setbuf") == 0) {
         output->addOutput(new CheckerOutput(node));
       }
     }

   } //End of the visit function.
   
