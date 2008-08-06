// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Function Call Allocates Multiple Resources Analysis
// Author: Gary Yuan
// Date: 21-December-2007

#include "compass.h"
#include "functionCallAllocatesMultipleResources.h"

namespace CompassAnalyses
   { 
     namespace FunctionCallAllocatesMultipleResources
        { 
          const std::string checkerName      = "FunctionCallAllocatesMultipleResources";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "This checker checks that multiple resources are not allocated in a single function call expression.";
          const std::string longDescription  = "Allocating more than one resource in a single statement could result in a memory leak, and this could lead to a denial-of-service attack. This checker checks that multiple resources are not allocated in a single function call expression.";
        } //End of namespace FunctionCallAllocatesMultipleResources.
   } //End of namespace CompassAnalyses.

CompassAnalyses::FunctionCallAllocatesMultipleResources::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::FunctionCallAllocatesMultipleResources::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["FunctionCallAllocatesMultipleResources.YourParameter"]);


   }

void
CompassAnalyses::FunctionCallAllocatesMultipleResources::Traversal::
visit(SgNode* node)
   { 
     SgFunctionCallExp *fcall = isSgFunctionCallExp(node);

     if( fcall != NULL )
     {
       int count = 0;
       const SgExpressionPtrList & args = fcall->get_args()->get_expressions();

       for( std::vector<SgExpression*>::const_iterator itr = args.begin();
            itr != args.end(); itr++ )
       {
         if( isSgNewExp(*itr) != NULL  )
           count++;
       } //for

       if( count > 1 )
         output->addOutput( new CheckerOutput(fcall) );
     } //if( fcall != NULL )
   } //End of the visit function.
