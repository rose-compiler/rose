// No Vfork
// Author: Gary M. Yuan
// Date: 27-August-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_NO_VFORK_H
#define COMPASS_NO_VFORK_H

namespace CompassAnalyses
   { 
     namespace NoVfork
        { 
        /*! \brief No Vfork: Add your description here 
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

// COMPASS_NO_VFORK_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// No Vfork Analysis
// Author: Gary M. Yuan
// Date: 27-August-2007

#include "compass.h"
// #include "noVfork.h"

namespace CompassAnalyses
   { 
     namespace NoVfork
        { 
          const std::string checkerName      = "NoVfork";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "This checker checks that vfork() is not used.";
          const std::string longDescription  = "The vfork() function has the same effect as fork(), except that the behavior is undefined if the process created by vfork() either modifies any data other than a variable of type pid_t used to store the return value from vfork(), or returns from the function in which vfork() was called, or calls any other function before successfully calling _exit() or one of the exec family of functions.";
        } //End of namespace NoVfork.
   } //End of namespace CompassAnalyses.

CompassAnalyses::NoVfork::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::NoVfork::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["NoVfork.YourParameter"]);


   }

void
CompassAnalyses::NoVfork::Traversal::
visit(SgNode* node)
   {
     const int STRING_LEN_VFORK = 5;
     SgFunctionRefExp *sgFuncRef = isSgFunctionRefExp( node );

     if( sgFuncRef != NULL )
     {
       std::string fncName = sgFuncRef->get_symbol()->get_name().getString();

       if( fncName.find( "vfork", 0, STRING_LEN_VFORK ) != std::string::npos )
       {
         output->addOutput( new CheckerOutput( node ) );
       } //if(fncName.find("vfork", 0, STRING_LEN_VFORK) != std::string::npos )
     } //if( sgFuncRef != NULL )

     return;
   } //End of the visit function.
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::NoVfork::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::NoVfork::Traversal(params, output);
}

extern const Compass::Checker* const noVforkChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::NoVfork::checkerName,
        CompassAnalyses::NoVfork::shortDescription,
        CompassAnalyses::NoVfork::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
