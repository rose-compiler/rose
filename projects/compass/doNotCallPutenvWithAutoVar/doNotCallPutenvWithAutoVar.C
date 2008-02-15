// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2:tw=80

// Do Not Call Putenv With Auto Var Analysis
// Author: Mark Lewandowski, -422-3849
// Date: 30-August-2007

#include "compass.h"
#include "doNotCallPutenvWithAutoVar.h"

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
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
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
   
