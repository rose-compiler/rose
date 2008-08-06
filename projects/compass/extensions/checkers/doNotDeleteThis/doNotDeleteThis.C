// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Do Not Delete This Analysis
// Author: Gary M. Yuan
// Date: 14-September-2007

#include "compass.h"
#include "doNotDeleteThis.h"

namespace CompassAnalyses
   { 
     namespace DoNotDeleteThis
        { 
          const std::string checkerName      = "DoNotDeleteThis";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "This checker checks that 'this' is not the argument to 'delete'";
          const std::string longDescription  = "Deleting this leaves it as a \"dangling\" pointer, which leads to undefined behavior if it is accessed.";
        } //End of namespace DoNotDeleteThis.
   } //End of namespace CompassAnalyses.

CompassAnalyses::DoNotDeleteThis::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::DoNotDeleteThis::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["DoNotDeleteThis.YourParameter"]);


   }

void
CompassAnalyses::DoNotDeleteThis::Traversal::
visit(SgNode* node)
   {
     SgDeleteExp *d = isSgDeleteExp( node );

     if( d != NULL )
     {
       SgThisExp *t = isSgThisExp( d->get_variable() );

       if( t != NULL )
         output->addOutput( new CheckerOutput( d ) );
     } //if( d != NULL )

     return;
   } //End of the visit function.
