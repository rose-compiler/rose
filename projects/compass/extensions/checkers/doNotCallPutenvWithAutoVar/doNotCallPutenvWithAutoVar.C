// Do Not Call Putenv With Auto Var
// Author: Mark Lewandowski, -422-3849
// Date: 30-August-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_DO_NOT_CALL_PUTENV_WITH_AUTO_VAR_H
#define COMPASS_DO_NOT_CALL_PUTENV_WITH_AUTO_VAR_H

namespace CompassAnalyses
   { 
     namespace DoNotCallPutenvWithAutoVar
        { 
        /*! \brief Do Not Call Putenv With Auto Var: Add your description here 
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

// COMPASS_DO_NOT_CALL_PUTENV_WITH_AUTO_VAR_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2:tw=80

// Do Not Call Putenv With Auto Var Analysis
// Author: Mark Lewandowski, -422-3849
// Date: 30-August-2007

#include "compass.h"
// #include "doNotCallPutenvWithAutoVar.h"

namespace CompassAnalyses
   { 
     namespace DoNotCallPutenvWithAutoVar
        { 
          const std::string checkerName      = "DoNotCallPutenvWithAutoVar";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = 
            "Do not call putenv() with an automatic variable as an argument.  Try using setenv() instead.";
          
          const std::string longDescription  = 
            "The POSIX function putenv() is used to set environment variable values. The putenv() function does not create a copy of the string supplied to it as a parameter, rather it inserts a pointer to the string into the environment array. If an automatic variable is supplied as a parameter to putenv(), the memory allocated for that variable may be overwritten when the containing function returns and stack memory is recycled. ";
        } //End of namespace DoNotCallPutenvWithAutoVar.
   } //End of namespace CompassAnalyses.

CompassAnalyses::DoNotCallPutenvWithAutoVar::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::DoNotCallPutenvWithAutoVar::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["DoNotCallPutenvWithAutoVar.YourParameter"]);


   }

void
CompassAnalyses::DoNotCallPutenvWithAutoVar::Traversal::
visit(SgNode* node)
   { 
     /* This checker will ban the use of putenv() since setenv() can be used to
      * accomplish the same thing, without any of the implementation drawbacks. */
     SgGlobal* global = isSgGlobal(node);
     static SgScopeStatement* g_scope;
     if(global) {
       g_scope = isSgScopeStatement(global);
       return;
     }
     
     SgFunctionCallExp* fcall = isSgFunctionCallExp(node);
     if(!(fcall = isSgFunctionCallExp(node))) {
       return;
     }
     
     SgFunctionRefExp* func = isSgFunctionRefExp(fcall->get_function());

     if( !func ) return;
     if( func->get_symbol()->get_name().getString().compare("putenv") != 0 ) {
       return;
     }

     SgVarRefExp* var = isSgVarRefExp(fcall->get_args()->get_expressions().front());
     SgScopeStatement* var_scope = var->get_symbol()->get_declaration()->get_scope();

     if (var_scope != g_scope) {
       output->addOutput(new CheckerOutput(node));
     }

   } //End of the visit function.
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::DoNotCallPutenvWithAutoVar::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::DoNotCallPutenvWithAutoVar::Traversal(params, output);
}

extern const Compass::Checker* const doNotCallPutenvWithAutoVarChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::DoNotCallPutenvWithAutoVar::checkerName,
        CompassAnalyses::DoNotCallPutenvWithAutoVar::shortDescription,
        CompassAnalyses::DoNotCallPutenvWithAutoVar::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
