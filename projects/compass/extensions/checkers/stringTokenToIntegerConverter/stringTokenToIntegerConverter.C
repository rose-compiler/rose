// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// String Token To Integer Converter Analysis
// Author: Han Suk  Kim
// Date: 04-September-2007

#include "compass.h"
#include "stringTokenToIntegerConverter.h"

namespace CompassAnalyses
   { 
     namespace StringTokenToIntegerConverter
        { 
          const std::string checkerName      = "StringTokenToIntegerConverter";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Use strtol() to convert a string token to an integer.";
          const std::string longDescription  = "Use strtol() or a related function to convert a string token to an integer. The strtol(), strtoll(), strtoul(), and strtoull() functions convert the initial portion of a string token to long int, long long int, unsigned long int, and unsigned long long int representation, respectively. These functions provide more robust error handling than alternative solutions.";
        } //End of namespace StringTokenToIntegerConverter.
   } //End of namespace CompassAnalyses.

CompassAnalyses::StringTokenToIntegerConverter::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::StringTokenToIntegerConverter::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["StringTokenToIntegerConverter.YourParameter"]);


   }

void
CompassAnalyses::StringTokenToIntegerConverter::Traversal::
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

           if(functionName == "atoi" || functionName == "atol" || 
               functionName == "atoll" || functionName == "sscanf" )
           {
             output->addOutput(new CheckerOutput(node));
           }
         }
       }
     }

   } //End of the visit function.
   
