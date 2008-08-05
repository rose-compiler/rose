// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Protect Virtual Methods Analysis
// Author: Gary M. Yuan
// Date: 07-August-2007

#include "compass.h"
#include "protectVirtualMethods.h"

namespace CompassAnalyses
   { 
     namespace ProtectVirtualMethods
        { 
          const std::string checkerName      = "ProtectVirtualMethods";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "This checker checks that virtual member function are not public. Virtual member functions should be protected with public member accessor functions.";
          const std::string longDescription  = "Declare virtual member functions as protected and call them from public accessor member functions. This ensures that the virtual member function is not exposed to the public interface of the class.";
        } //End of namespace ProtectVirtualMethods.
   } //End of namespace CompassAnalyses.

CompassAnalyses::ProtectVirtualMethods::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::ProtectVirtualMethods::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["ProtectVirtualMethods.YourParameter"]);


   }

void
CompassAnalyses::ProtectVirtualMethods::Traversal::
visit(SgNode* node)
   { 
     SgMemberFunctionDeclaration *sgmf = isSgMemberFunctionDeclaration(node);

     if( sgmf != NULL )
     {
       SgFunctionModifier sgfm = sgmf->get_functionModifier();
       SgAccessModifier sgam = 
         sgmf->get_declarationModifier().get_accessModifier();

       if( sgfm.isVirtual() == true && sgam.isPublic() == true )
       {
         output->addOutput( new CheckerOutput( node ) );
       } //if( sgfm.isVirtual() == true && sgam.isPublic() == true )
     } //if( sgmf != NULL )

     return;
   } //End of the visit function.
