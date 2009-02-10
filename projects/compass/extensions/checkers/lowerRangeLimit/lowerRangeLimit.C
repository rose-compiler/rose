// Lower Range Limit
// Author: pants,,,
// Date: 14-August-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_LOWER_RANGE_LIMIT_H
#define COMPASS_LOWER_RANGE_LIMIT_H

namespace CompassAnalyses
   { 
     namespace LowerRangeLimit
        { 
        /*! \brief Lower Range Limit: Add your description here 
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

// COMPASS_LOWER_RANGE_LIMIT_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Lower Range Limit Analysis
// Author: pants,,,
// Date: 14-August-2007

#include "compass.h"
// #include "lowerRangeLimit.h"

namespace CompassAnalyses
   { 
     namespace LowerRangeLimit
        { 
          const std::string checkerName      = "LowerRangeLimit";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Finding examples of using lower range exclusive";
          const std::string longDescription  = "Searches for instances of using less intuitive lower range limit exclusive (strictly less than).";
        } //End of namespace LowerRangeLimit.
   } //End of namespace CompassAnalyses.

CompassAnalyses::LowerRangeLimit::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::LowerRangeLimit::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["LowerRangeLimit.YourParameter"]);


   }

void
CompassAnalyses::LowerRangeLimit::Traversal::
visit(SgNode* node)
   { 
     if (isSgLessThanOp(node)) output->addOutput(new CheckerOutput(node));
   } //End of the visit function.
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::LowerRangeLimit::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::LowerRangeLimit::Traversal(params, output);
}

extern const Compass::Checker* const lowerRangeLimitChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::LowerRangeLimit::checkerName,
        CompassAnalyses::LowerRangeLimit::shortDescription,
        CompassAnalyses::LowerRangeLimit::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
