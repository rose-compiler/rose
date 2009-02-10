// Prefer Fseek To Rewind
// Author: Han Suk  Kim
// Date: 04-September-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_PREFER_FSEEK_TO_REWIND_H
#define COMPASS_PREFER_FSEEK_TO_REWIND_H

namespace CompassAnalyses
   { 
     namespace PreferFseekToRewind
        { 
        /*! \brief Prefer Fseek To Rewind: Add your description here 
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

// COMPASS_PREFER_FSEEK_TO_REWIND_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Prefer Fseek To Rewind Analysis
// Author: Han Suk  Kim
// Date: 04-September-2007

#include "compass.h"
// #include "preferFseekToRewind.h"

namespace CompassAnalyses
   { 
     namespace PreferFseekToRewind
        { 
          const std::string checkerName      = "PreferFseekToRewind";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Prefer fseek() to rewind()";
          const std::string longDescription  = "rewind() sets the file position indicator for a stream to the beginning of that stream. However, rewind() is equivalent to fseek() with 0L for the offset and SEEK_SET for the mode with the error return value suppressed. Therefore, to validate that moving back to the beginning of a stream actually succeeded, fseek() should be used instead of rewind().";
        } //End of namespace PreferFseekToRewind.
   } //End of namespace CompassAnalyses.

CompassAnalyses::PreferFseekToRewind::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::PreferFseekToRewind::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["PreferFseekToRewind.YourParameter"]);


   }

void
CompassAnalyses::PreferFseekToRewind::Traversal::
visit(SgNode* node)
   { 
  // Implement your traversal here.  
     if(isSgFunctionCallExp(node))
     {
       SgFunctionCallExp* callSite = isSgFunctionCallExp(node);

       if(callSite->get_function() != NULL)
       {
         SgFunctionRefExp* functionRefExp = isSgFunctionRefExp(callSite->get_function());
         if(functionRefExp != NULL)
         {

           SgFunctionSymbol* functionSymbol = functionRefExp->get_symbol();
           ROSE_ASSERT(functionSymbol != NULL);

           std::string functionName = functionSymbol->get_name().getString();

           if(functionName == "rewind")
           {
             output->addOutput(new CheckerOutput(node));
           }
         }
       }
     }

   } //End of the visit function.
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::PreferFseekToRewind::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::PreferFseekToRewind::Traversal(params, output);
}

extern const Compass::Checker* const preferFseekToRewindChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::PreferFseekToRewind::checkerName,
        CompassAnalyses::PreferFseekToRewind::shortDescription,
        CompassAnalyses::PreferFseekToRewind::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
