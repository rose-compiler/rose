// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// File Read Only Access Analysis
// Author: Gary Yuan
// Date: 03-January-2008

#include "compass.h"
#include "fileReadOnlyAccess.h"

namespace CompassAnalyses
   { 
     namespace FileReadOnlyAccess
        { 
          const std::string checkerName      = "FileReadOnlyAccess";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "This checker checks that all standard C/C++ file I/O is read-only.";
          const std::string longDescription  = "Long description not written yet!";
        } //End of namespace FileReadOnlyAccess.
   } //End of namespace CompassAnalyses.

CompassAnalyses::FileReadOnlyAccess::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::FileReadOnlyAccess::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["FileReadOnlyAccess.YourParameter"]);


   }

void
CompassAnalyses::FileReadOnlyAccess::Traversal::
visit(SgNode* node)
   {
     SgFunctionCallExp *fcall = isSgFunctionCallExp(node);
     SgVariableDeclaration *vdecl = isSgVariableDeclaration(node);

     if( fcall != NULL )
     {
       SgFunctionRefExp *fref = isSgFunctionRefExp(fcall->get_function());

       if( fref != NULL )
       {
         if( fref->get_symbol()->get_name().getString() == "fopen" )
         {
           const SgExpressionPtrList arguments = fcall->get_args()->get_expressions();
           ROSE_ASSERT( arguments.size() == 2 );

           SgExpressionPtrList::const_iterator itr = arguments.begin(); itr++;

           SgStringVal *mode = isSgStringVal(*itr);

           ROSE_ASSERT( mode != NULL );

           if( mode->get_value() != "r" && mode->get_value() != "rb" )
           {
             output->addOutput( new CheckerOutput(fcall) );
           } //if( mode->get_value() != "r" || mode->get_value() != "rb" )
         } //if( fref->get_symbol()->get_name().getString() == "fopen" )
       } //if( fref != NULL )
     } //if( fcall != NULL )

     if( vdecl != NULL )
     {
       SgVariableDefinition *vdef = vdecl->get_definition();
       ROSE_ASSERT( vdef != NULL );

       SgNamedType *vname = isSgNamedType( vdef->get_type() );

       if( vname != NULL )
       {
         std::string vnameString( vname->get_name().getString() );

         if( vnameString == "fstream" || vnameString == "ofstream" )
         {
           output->addOutput( new CheckerOutput(vdecl) );
         } //if( vnameString == "fstream" || vnameString == "ofstream" )
       } //if( vname != NULL )
     } //if( vdecl != NULL )

     return;
   } //End of the visit function.
   
