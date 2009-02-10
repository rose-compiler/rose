// Function Call Allocates Multiple Resources
// Author: Gary Yuan
// Date: 21-December-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_FUNCTION_CALL_ALLOCATES_MULTIPLE_RESOURCES_H
#define COMPASS_FUNCTION_CALL_ALLOCATES_MULTIPLE_RESOURCES_H

namespace CompassAnalyses
   { 
     namespace FunctionCallAllocatesMultipleResources
        { 
        /*! \brief Function Call Allocates Multiple Resources: Add your description here 
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

// COMPASS_FUNCTION_CALL_ALLOCATES_MULTIPLE_RESOURCES_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Function Call Allocates Multiple Resources Analysis
// Author: Gary Yuan
// Date: 21-December-2007

#include "compass.h"
// #include "functionCallAllocatesMultipleResources.h"

namespace CompassAnalyses
   { 
     namespace FunctionCallAllocatesMultipleResources
        { 
          const std::string checkerName      = "FunctionCallAllocatesMultipleResources";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "This checker checks that multiple resources are not allocated in a single function call expression.";
          const std::string longDescription  = "Allocating more than one resource in a single statement could result in a memory leak, and this could lead to a denial-of-service attack. This checker checks that multiple resources are not allocated in a single function call expression.";
        } //End of namespace FunctionCallAllocatesMultipleResources.
   } //End of namespace CompassAnalyses.

CompassAnalyses::FunctionCallAllocatesMultipleResources::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::FunctionCallAllocatesMultipleResources::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["FunctionCallAllocatesMultipleResources.YourParameter"]);


   }

void
CompassAnalyses::FunctionCallAllocatesMultipleResources::Traversal::
visit(SgNode* node)
   { 
     SgFunctionCallExp *fcall = isSgFunctionCallExp(node);

     if( fcall != NULL )
     {
       int count = 0;
       const SgExpressionPtrList & args = fcall->get_args()->get_expressions();

       for( std::vector<SgExpression*>::const_iterator itr = args.begin();
            itr != args.end(); itr++ )
       {
         if( isSgNewExp(*itr) != NULL  )
           count++;
       } //for

       if( count > 1 )
         output->addOutput( new CheckerOutput(fcall) );
     } //if( fcall != NULL )
   } //End of the visit function.

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::FunctionCallAllocatesMultipleResources::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::FunctionCallAllocatesMultipleResources::Traversal(params, output);
}

extern const Compass::Checker* const functionCallAllocatesMultipleResourcesChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::FunctionCallAllocatesMultipleResources::checkerName,
        CompassAnalyses::FunctionCallAllocatesMultipleResources::shortDescription,
        CompassAnalyses::FunctionCallAllocatesMultipleResources::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
