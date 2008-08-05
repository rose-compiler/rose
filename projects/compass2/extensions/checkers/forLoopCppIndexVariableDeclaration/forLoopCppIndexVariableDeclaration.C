// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// For Loop Cpp Index Variable Declaration Analysis
// Author: Gary M. Yuan
// Date: 24-July-2007

#include "compass.h"
#include "forLoopCppIndexVariableDeclaration.h"

namespace CompassAnalyses
   { 
     namespace ForLoopCppIndexVariableDeclaration
        { 
          const std::string checkerName      = "ForLoopCppIndexVariableDeclaration";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "This checker checks that the for loop index variable is always declared in the scope of the constructor for C++ code";
          const std::string longDescription  = "There are many reasons to declare for loop index variables as locally as possible. In C++ this means declaring the index variable inside the for loop initializer. One reason to do this is to minimize the scope of the index variable to the for loop. Another reason is to prevent confusion and re-using common counter variables such as \"i\" that may have an unknown value at the point of use.";
        } //End of namespace ForLoopCppIndexVariableDeclaration.
   } //End of namespace CompassAnalyses.

CompassAnalyses::ForLoopCppIndexVariableDeclaration::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::ForLoopCppIndexVariableDeclaration::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["ForLoopCppIndexVariableDeclaration.YourParameter"]);


   }


void
CompassAnalyses::ForLoopCppIndexVariableDeclaration::Traversal::
visit(SgNode* node)
   {
     SgDeclarationStatement *ainit = isSgDeclarationStatement(node);

     if( ainit == NULL && node != NULL )
     {
       SgNode *parent = node->get_parent();

       if( parent != NULL && 
           isSgForInitStatement(parent) != NULL )
       {
         if( node->get_file_info()->get_filenameString().find( ".c" ) !=
             std::string::npos ) return;

         output->addOutput( new CheckerOutput(parent) );
       } //if
     }//if
   
     return;
   } //End of the visit function.
