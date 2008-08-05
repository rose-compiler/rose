// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// No Template Usage Analysis
// Author: Andreas Saebjornsen
// Date: 25-July-2007

#include "compass.h"
#include "noTemplateUsage.h"

namespace CompassAnalyses
   { 
     namespace NoTemplateUsage
        { 
          const std::string checkerName      = "NoTemplateUsage";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Templates should not be used.";
          const std::string longDescription  = "Templates should not be used.";
        } //End of namespace NoTemplateUsage.
   } //End of namespace CompassAnalyses.

CompassAnalyses::NoTemplateUsage::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::NoTemplateUsage::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["NoTemplateUsage.YourParameter"]);


   }

void
CompassAnalyses::NoTemplateUsage::Traversal::
visit(SgNode* node)
   {
     //Detect all usages of C++ templates 
     switch(node->variantT()){
        case V_SgTemplateInstantiationDecl:
        case V_SgTemplateInstantiationMemberFunctionDecl:
        case V_SgTemplateInstantiationFunctionDecl:
        case V_SgTemplateInstantiationDefn:
             getOutput()->addOutput(new CheckerOutput(node));
             break;
        default:
             break;
     }
   } //End of the visit function.
   
