// Upper Range Limit
// Author: pants,,,
// Date: 14-August-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_UPPER_RANGE_LIMIT_H
#define COMPASS_UPPER_RANGE_LIMIT_H

namespace CompassAnalyses
   { 
     namespace UpperRangeLimit
        { 
        /*! \brief Upper Range Limit: Add your description here 
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

// COMPASS_UPPER_RANGE_LIMIT_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Upper Range Limit Analysis
// Author: pants,,,
// Date: 14-August-2007

#include "compass.h"
// #include "upperRangeLimit.h"

namespace CompassAnalyses
   { 
     namespace UpperRangeLimit
        { 
          const std::string checkerName      = "UpperRangeLimit";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Finding examples of using upper range inclusive";
          const std::string longDescription  = "Searches for instances of using the less intuitive"
          " inclusive upper range limit (greater than or equal to)";
        } //End of namespace UpperRangeLimit.
   } //End of namespace CompassAnalyses.

CompassAnalyses::UpperRangeLimit::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::UpperRangeLimit::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["UpperRangeLimit.YourParameter"]);


   }

void
CompassAnalyses::UpperRangeLimit::Traversal::
visit(SgNode* node)
   { 
     if (isSgGreaterOrEqualOp(node)) output->addOutput(new CheckerOutput(node));
   } //End of the visit function.
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::UpperRangeLimit::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::UpperRangeLimit::Traversal(params, output);
}

extern const Compass::Checker* const upperRangeLimitChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::UpperRangeLimit::checkerName,
        CompassAnalyses::UpperRangeLimit::shortDescription,
        CompassAnalyses::UpperRangeLimit::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
