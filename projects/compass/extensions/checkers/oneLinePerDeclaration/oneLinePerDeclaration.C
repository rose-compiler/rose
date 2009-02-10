// One Line Per Declaration
// Author: Gary M. Yuan
// Date: 24-August-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_ONE_LINE_PER_DECLARATION_H
#define COMPASS_ONE_LINE_PER_DECLARATION_H

#include <set>

namespace CompassAnalyses
   { 
     namespace OneLinePerDeclaration
        { 
        /*! \brief One Line Per Declaration: Add your description here 
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
               Compass::OutputObject* output;
               std::set<int> lineNums;
               std::string currFileName;

               public:
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

                 // The implementation of the run function has to match the traversal being called.
                    void run(SgNode* n){ this->traverse(n, preorder); };

                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_ONE_LINE_PER_DECLARATION_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// One Line Per Declaration Analysis
// Author: Gary M. Yuan
// Date: 24-August-2007

#include "compass.h"
// #include "oneLinePerDeclaration.h"

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
   : output(output)
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
#if ROSE_GCC_OMP
#pragma omp critical (oneLinePDclear) 
#endif
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
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::OneLinePerDeclaration::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::OneLinePerDeclaration::Traversal(params, output);
}

extern const Compass::Checker* const oneLinePerDeclarationChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::OneLinePerDeclaration::checkerName,
        CompassAnalyses::OneLinePerDeclaration::shortDescription,
        CompassAnalyses::OneLinePerDeclaration::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
