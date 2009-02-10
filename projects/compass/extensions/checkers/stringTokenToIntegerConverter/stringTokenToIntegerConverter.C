// String Token To Integer Converter
// Author: Han Suk  Kim
// Date: 04-September-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_STRING_TOKEN_TO_INTEGER_CONVERTER_H
#define COMPASS_STRING_TOKEN_TO_INTEGER_CONVERTER_H

namespace CompassAnalyses
   { 
     namespace StringTokenToIntegerConverter
        { 
        /*! \brief String Token To Integer Converter: Add your description here 
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

// COMPASS_STRING_TOKEN_TO_INTEGER_CONVERTER_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// String Token To Integer Converter Analysis
// Author: Han Suk  Kim
// Date: 04-September-2007

#include "compass.h"
// #include "stringTokenToIntegerConverter.h"

namespace CompassAnalyses
   { 
     namespace StringTokenToIntegerConverter
        { 
          const std::string checkerName      = "StringTokenToIntegerConverter";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Use strtol() to convert a string token to an integer.";
          const std::string longDescription  = "Use strtol() or a related function to convert a string token to an integer. The strtol(), strtoll(), strtoul(), and strtoull() functions convert the initial portion of a string token to long int, long long int, unsigned long int, and unsigned long long int representation, respectively. These functions provide more robust error handling than alternative solutions.";
        } //End of namespace StringTokenToIntegerConverter.
   } //End of namespace CompassAnalyses.

CompassAnalyses::StringTokenToIntegerConverter::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::StringTokenToIntegerConverter::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["StringTokenToIntegerConverter.YourParameter"]);


   }

void
CompassAnalyses::StringTokenToIntegerConverter::Traversal::
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

           if(functionName == "atoi" || functionName == "atol" || 
               functionName == "atoll" || functionName == "sscanf" )
           {
             output->addOutput(new CheckerOutput(node));
           }
         }
       }
     }

   } //End of the visit function.
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::StringTokenToIntegerConverter::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::StringTokenToIntegerConverter::Traversal(params, output);
}

extern const Compass::Checker* const stringTokenToIntegerConverterChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::StringTokenToIntegerConverter::checkerName,
        CompassAnalyses::StringTokenToIntegerConverter::shortDescription,
        CompassAnalyses::StringTokenToIntegerConverter::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
