// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// One Line Per Declaration Analysis
// Author: Gary M. Yuan
// Date: 24-August-2007

#include "compass.h"
#include "oneLinePerDeclaration.h"

namespace CompassAnalyses
   { 
     namespace OneLinePerDeclaration
        { 
          const std::string checkerName      = "OneLinePerDeclaration";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "This checker checks that only one variable declaration occurs per line.";
          const std::string longDescription  = "Declaring multiple variables on a single line of code can cause confusion regarding the types of the variables and their initial values. If more than one variable is declared on a line, care must be taken that the actual type and initialized value of the variable is known. To avoid confusion, more than one type of variable should not be declared on the same line.";
        } //End of namespace OneLinePerDeclaration.
   } //End of namespace CompassAnalyses.

CompassAnalyses::OneLinePerDeclaration::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::OneLinePerDeclaration::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["OneLinePerDeclaration.YourParameter"]);


   }

void
CompassAnalyses::OneLinePerDeclaration::Traversal::
visit(SgNode* node)
   {
     SgVariableDeclaration *sgVarDecl = isSgVariableDeclaration( node );

     if( sgVarDecl != NULL )
     {
       if( node->get_file_info()->isCompilerGenerated() == true )
         return;
     }
#pragma omp critical (oneLinePDclear) 
     if( sgVarDecl != NULL )
     {
       std::string file = node->get_file_info()->get_filenameString();
       int line = node->get_file_info()->get_line();

       if( currFileName.compare( file ) != 0 )
       {
         currFileName.assign( file );
         lineNums.clear();
       } 

       if( lineNums.find( line ) != lineNums.end() )
         output->addOutput( new CheckerOutput( node ) );
       else {
         lineNums.insert( line );
       }
     } //if( sgVarDecl != NULL )

     return;
   } //End of the visit function.
   
