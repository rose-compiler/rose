// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Prefer Fseek To Rewind Analysis
// Author: Han Suk  Kim
// Date: 04-September-2007

#include "compass.h"
#include "preferFseekToRewind.h"

namespace CompassAnalyses
   { 
     namespace PreferFseekToRewind
        { 
          const std::string checkerName      = "PreferFseekToRewind";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Prefer fseek() to rewind()";
          const std::string longDescription  = "rewind() sets the file position indicator for a stream to the beginning of that stream. However, rewind() is equivalent to fseek() with 0L for the offset and SEEK_SET for the mode with the error return value suppressed. Therefore, to validate that moving back to the beginning of a stream actually succeeded, fseek() should be used instead of rewind().";
        } //End of namespace PreferFseekToRewind.
   } //End of namespace CompassAnalyses.

CompassAnalyses::PreferFseekToRewind::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::PreferFseekToRewind::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["PreferFseekToRewind.YourParameter"]);


   }

void
CompassAnalyses::PreferFseekToRewind::Traversal::
visit(SgNode* node)
   { 
  // Implement your traversal here.  
     if(isSgFunctionCallExp(node))
     {
       SgFunctionCallExp* callSite = isSgFunctionCallExp(node);

       if(callSite->get_function() != NULL)
       {
         SgFunctionRefExp* functionRefExp = isSgFunctionRefExp(callSite->get_function());
         if(functionRefExp != NULL)
         {

           SgFunctionSymbol* functionSymbol = functionRefExp->get_symbol();
           ROSE_ASSERT(functionSymbol != NULL);

           std::string functionName = functionSymbol->get_name().getString();

           if(functionName == "rewind")
           {
             output->addOutput(new CheckerOutput(node));
           }
         }
       }
     }

   } //End of the visit function.
   
