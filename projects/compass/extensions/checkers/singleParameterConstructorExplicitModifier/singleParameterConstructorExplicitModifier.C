// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Single Parameter Constructor Explicit Modifier Analysis
// Author: Gary M. Yuan
// Date: 23-July-2007

#include "compass.h"
#include "singleParameterConstructorExplicitModifier.h"

namespace CompassAnalyses
   { 
     namespace SingleParameterConstructorExplicitModifier
        { 
          const std::string checkerName      = "SingleParameterConstructorExplicitModifier";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "This checker checks that single paramater constructors are declared using the explicit keyword.";
          const std::string longDescription  = "Declare single parameter constructors as explicit to avoid unexpected type conversion. A compiler can use a single paramter constructor for type conversions. While this is natural in some situations; it may be unexpected in others.";
        } //End of namespace SingleParameterConstructorExplicitModifier.
   } //End of namespace CompassAnalyses.

CompassAnalyses::SingleParameterConstructorExplicitModifier::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::SingleParameterConstructorExplicitModifier::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["SingleParameterConstructorExplicitModifier.YourParameter"]);


   }

void
CompassAnalyses::SingleParameterConstructorExplicitModifier::Traversal::
visit(SgNode* node)
   { 
     SgFunctionDeclaration *fd = isSgFunctionDeclaration(node);

     if( fd != NULL )
     {
       SgSpecialFunctionModifier sfm = fd->get_specialFunctionModifier();

       if( sfm.isConstructor() == true )
       {
         int argc = fd->get_parameterList()->get_args().size();

         if( argc == 1 )
         {
           if( fd->get_functionModifier().isExplicit() == false )
           {
             output->addOutput( new CheckerOutput( node ) );
           } //if( fd->get_functionModifier().isExplicit() == false )
         } //if( argc == 1 )
       } //if( sfm.isConstructor() == true )
     } //if( fd != NULL )

     return;
   } //End of the visit function.
   
