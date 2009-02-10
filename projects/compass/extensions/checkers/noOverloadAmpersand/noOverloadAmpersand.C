// No Overload Ampersand
// Author: pants,,,
// Date: 31-August-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_NO_OVERLOAD_AMPERSAND_H
#define COMPASS_NO_OVERLOAD_AMPERSAND_H

namespace CompassAnalyses
   { 
     namespace NoOverloadAmpersand
        { 
        /*! \brief No Overload Ampersand: Add your description here 
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

// COMPASS_NO_OVERLOAD_AMPERSAND_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// No Overload Ampersand Analysis
// Author: pants,,,
// Date: 31-August-2007

#include "compass.h"
// #include "noOverloadAmpersand.h"

namespace CompassAnalyses
   { 
     namespace NoOverloadAmpersand
        { 
          const std::string checkerName      = "NoOverloadAmpersand";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Finding overloaded operator&";
          const std::string longDescription  = "Finds member functions of the name operator& and raises an alert.";
        } //End of namespace NoOverloadAmpersand.
   } //End of namespace CompassAnalyses.

CompassAnalyses::NoOverloadAmpersand::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::NoOverloadAmpersand::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["NoOverloadAmpersand.YourParameter"]);


   }

void
CompassAnalyses::NoOverloadAmpersand::Traversal::
visit(SgNode* node)
   { 
  // Implement your traversal here.  
     SgMemberFunctionDeclaration *mfn =  isSgMemberFunctionDeclaration(node);
     std::string op = "operator&";
     if (!mfn) return;
     std::string fName = mfn->get_name().str();
     if (op == fName)
       {
         output->addOutput(new CheckerOutput(node));
       }
   } //End of the visit function.
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::NoOverloadAmpersand::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::NoOverloadAmpersand::Traversal(params, output);
}

extern const Compass::Checker* const noOverloadAmpersandChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::NoOverloadAmpersand::checkerName,
        CompassAnalyses::NoOverloadAmpersand::shortDescription,
        CompassAnalyses::NoOverloadAmpersand::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
