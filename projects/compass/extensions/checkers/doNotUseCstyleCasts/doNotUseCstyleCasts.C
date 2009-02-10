// Do Not Use C-style Casts
// Author: Mark Lewandowski, -422-3849
// Date: 18-December-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_DO_NOT_USE_CSTYLE_CASTS_H
#define COMPASS_DO_NOT_USE_CSTYLE_CASTS_H

namespace CompassAnalyses
   { 
     namespace DoNotUseCstyleCasts
        { 
        /*! \brief Do Not Use C-style Casts: Add your description here 
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

                 // Change the implementation of this function if you are using inherited attributes.
                    void *initialInheritedAttribute() const { return NULL; }

                 // The implementation of the run function has to match the traversal being called.
                 // If you use inherited attributes, use the following definition:
                 // void run(SgNode* n){ this->traverse(n, initialInheritedAttribute()); }
                    void run(SgNode* n){ this->traverse(n, preorder); }

                 // Change this function if you are using a different type of traversal, e.g.
                 // void *evaluateInheritedAttribute(SgNode *, void *);
                 // for AstTopDownProcessing.
                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_DO_NOT_USE_CSTYLE_CASTS_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Do Not Use C-style Casts Analysis
// Author: Mark Lewandowski, -422-3849
// Date: 18-December-2007

#include "compass.h"
// #include "doNotUseCstyleCasts.h"

namespace CompassAnalyses
   { 
     namespace DoNotUseCstyleCasts
        { 
          const std::string checkerName      = "DoNotUseCstyleCasts";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "C++ casts allow for more compiler checking and are easier to find in source code (either by tools or by human readers).";
          const std::string longDescription  = "C++ allows C-style casts, although it has introduced its own casts:\n\t* static_cast<type>(expression)\n\t* const_cast<type>(expression)\n\t* dynamic_cast<type>(expression)\n\t* reinterpret_cast<type>(expression)\nC++ casts allow for more compiler checking and are easier to find in source code (either by tools or by human readers).";
        } //End of namespace DoNotUseCstyleCasts.
   } //End of namespace CompassAnalyses.

CompassAnalyses::DoNotUseCstyleCasts::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::DoNotUseCstyleCasts::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["DoNotUseCstyleCasts.YourParameter"]);


   }

void
CompassAnalyses::DoNotUseCstyleCasts::Traversal::
visit(SgNode* node)
   {
     SgCastExp* cast = isSgCastExp(node);

     if (!cast) {
       return;
     }

     if (cast->get_cast_type() == SgCastExp::e_C_style_cast) {
       output->addOutput(new CheckerOutput(node));
     }
   } //End of the visit function.
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::DoNotUseCstyleCasts::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::DoNotUseCstyleCasts::Traversal(params, output);
}

extern const Compass::Checker* const doNotUseCstyleCastsChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::DoNotUseCstyleCasts::checkerName,
        CompassAnalyses::DoNotUseCstyleCasts::shortDescription,
        CompassAnalyses::DoNotUseCstyleCasts::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
