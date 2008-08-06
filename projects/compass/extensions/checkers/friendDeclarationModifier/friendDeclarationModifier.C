// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Friend Declaration Modifier Analysis
// Author: Gary M. Yuan
// Date: 30-July-2007

#include "compass.h"
#include "friendDeclarationModifier.h"

namespace CompassAnalyses
   { 
     namespace FriendDeclarationModifier
        { 
          const std::string checkerName      = "FriendDeclarationModifier";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "This checker checks if a declarations statement contains the \"friend\" keyword modifier ";
          const std::string longDescription  = "Avoid using the \"friend\" keyword modifier because they bypass access restrictions and hide dependencies between classes and functions. Also \"friends\" are often indicative of poor design.";
        } //End of namespace FriendDeclarationModifier.
   } //End of namespace CompassAnalyses.

CompassAnalyses::FriendDeclarationModifier::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::FriendDeclarationModifier::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["FriendDeclarationModifier.YourParameter"]);


   }

void
CompassAnalyses::FriendDeclarationModifier::Traversal::
visit(SgNode* node)
   { 
     SgDeclarationStatement *ds = isSgDeclarationStatement(node);

     if( ds != NULL )
     {
       SgDeclarationModifier dsm = ds->get_declarationModifier();

       if( dsm.isFriend() == true )
       {
         output->addOutput( new CheckerOutput(node) );
       } //if( dsm.isFriend() == true )
     } // if( ds != NULL )

     return;
   } //End of the visit function.
