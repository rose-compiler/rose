// No Template Usage
// Author: Andreas Saebjornsen
// Date: 25-July-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_NO_TEMPLATE_USAGE_H
#define COMPASS_NO_TEMPLATE_USAGE_H

namespace CompassAnalyses
   { 
     namespace NoTemplateUsage
        { 
        /*! \brief No Template Usage: Add your description here 
         */

          extern const std::string checkerName;
          extern const std::string shortDescription;
          extern const std::string longDescription;

       // Specification of Checker Output Implementation
          class CheckerOutput: public Compass::OutputViolationBase
             { 
               public:
                    CheckerOutput(SgNode* node);
             };

       // Specification of Checker Traversal Implementation

          class Traversal
             : public Compass::AstSimpleProcessingWithRunFunction
             {
            // Checker specific parameters should be allocated here.
               Compass::OutputObject* output;

               public:
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

                 // The implementation of the run function has to match the traversal being called.
                    void run(SgNode* n){ this->traverse(n, preorder); };

                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_NO_TEMPLATE_USAGE_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// No Template Usage Analysis
// Author: Andreas Saebjornsen
// Date: 25-July-2007

#include "compass.h"
// #include "noTemplateUsage.h"

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
   : output(output)
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
             output->addOutput(new CheckerOutput(node));
             break;
        default:
             break;
     }
   } //End of the visit function.
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::NoTemplateUsage::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::NoTemplateUsage::Traversal(params, output);
}

extern const Compass::Checker* const noTemplateUsageChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::NoTemplateUsage::checkerName,
        CompassAnalyses::NoTemplateUsage::shortDescription,
        CompassAnalyses::NoTemplateUsage::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
