// No Exceptions
// Author: Andreas Saebjornsen
// Date: 25-July-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_NO_EXCEPTIONS_H
#define COMPASS_NO_EXCEPTIONS_H

namespace CompassAnalyses
   { 
     namespace NoExceptions
        { 
        /*! \brief No Exceptions: Add your description here 
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

// COMPASS_NO_EXCEPTIONS_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// No Exceptions Analysis
// Author: Andreas Saebjornsen
// Date: 25-July-2007

#include "compass.h"
// #include "noExceptions.h"

namespace CompassAnalyses
   { 
     namespace NoExceptions
        { 
          const std::string checkerName      = "NoExceptions";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "C++ exception handling should not be used.";
          const std::string longDescription  = "C++ exception handling should not be used.";
        } //End of namespace NoExceptions.
   } //End of namespace CompassAnalyses.

CompassAnalyses::NoExceptions::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::NoExceptions::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["NoExceptions.YourParameter"]);


   }

void
CompassAnalyses::NoExceptions::Traversal::
visit(SgNode* node)
   {
     //Detect all places where C++ exceptions are used.
     switch(node->variantT()){
      case V_SgThrowOp:
      case V_SgCatchOptionStmt:
      case V_SgTryStmt:
         output->addOutput(new CheckerOutput(node));
         break;
      default:
         break;
    }
   } //End of the visit function.
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::NoExceptions::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::NoExceptions::Traversal(params, output);
}

extern const Compass::Checker* const noExceptionsChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::NoExceptions::checkerName,
        CompassAnalyses::NoExceptions::shortDescription,
        CompassAnalyses::NoExceptions::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
