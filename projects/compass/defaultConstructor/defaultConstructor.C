// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Default Constructor Analysis
// Author: Gary M. Yuan
// Date: 07-August-2007

#include "compass.h"
#include "defaultConstructor.h"

namespace CompassAnalyses
   { 
     namespace DefaultConstructor
        { 
          const std::string checkerName      = "DefaultConstructor";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "This checker checks that every class has a non-generated default constructor.";
          const std::string longDescription  = "Declare a default constructor for every class you create. Although some compilers may generate a more efficient implementation in some instances you should choose a default constructor because it is more clear.";
        } //End of namespace DefaultConstructor.
   } //End of namespace CompassAnalyses.

CompassAnalyses::DefaultConstructor::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::DefaultConstructor::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["DefaultConstructor.YourParameter"]);


   }

void
CompassAnalyses::DefaultConstructor::Traversal::
visit(SgNode* node)
   { 
     SgClassDefinition *sgcd = isSgClassDefinition(node);

     if( sgcd != NULL )
     {
       bool foundDefaultConstructor = false;
       const std::vector<SgNode*> children = 
         sgcd->get_traversalSuccessorContainer();

       for( std::vector<SgNode*>::const_iterator itr = children.begin(); 
            itr != children.end(); itr++ )
       {
         SgMemberFunctionDeclaration *sgmf = 
           isSgMemberFunctionDeclaration( *itr );

         if( sgmf != NULL )
         {
           if( sgmf->get_specialFunctionModifier().isConstructor() == true && 
               sgmf->get_functionModifier().isDefault() == true )
           {
             foundDefaultConstructor = true;
             break;
           } //sgmf->get_specialFunctionModifier().isConstructor() == true ... 
         } //if( sgmf != NULL )
       } //for, itr

       if( foundDefaultConstructor == false )
       {
         output->addOutput( new CheckerOutput( node ) );
       } //if( foundDefaultConstructor == false )
     } //if( sgcd != NULL )

     return;
   } //End of the visit function.
