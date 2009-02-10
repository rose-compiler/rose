// For Loop Cpp Index Variable Declaration
// Author: Gary M. Yuan
// Date: 24-July-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_FOR_LOOP_CPP_INDEX_VARIABLE_DECLARATION_H
#define COMPASS_FOR_LOOP_CPP_INDEX_VARIABLE_DECLARATION_H

#include <set>

namespace CompassAnalyses
   { 
     namespace ForLoopCppIndexVariableDeclaration
        { 
          /// \brief checkerName is a std::string containing the name of this
          /// checker.
          extern const std::string checkerName;
          /// \brief shortDescription is a std::string with a short description
          /// of this checker's pattern
          extern const std::string shortDescription;
          /// \brief longDescription is a std::string with a detailed
          /// description of this checker's pattern and purpose.
          extern const std::string longDescription;

       // Specification of Checker Output Implementation
          //////////////////////////////////////////////////////////////////////          /// The CheckerOutput class implements the violation output for this
          /// checker
          //////////////////////////////////////////////////////////////////////
          class CheckerOutput: public Compass::OutputViolationBase
             { 
               public:
                 /// The constructor
                 /// \param node is a SgNode*
                 CheckerOutput(SgNode* node);
             };

       // Specification of Checker Traversal Implementation
          class Traversal
             : public Compass::AstSimpleProcessingWithRunFunction
             {
            // Checker specific parameters should be allocated here.
               Compass::OutputObject* output;
               std::set<SgNode*> seen;

               public:
                    /// The constructor
                    /// \param out is a Compass::OutputObject*
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

                 // The implementation of the run function has to match the traversal being called.
                    /// run function
                    /// \param n is a SgNode*
                    void run(SgNode* n){ this->traverse(n, preorder); };

                    /// visit function
                    /// \param n is a SgNode
                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_FOR_LOOP_CPP_INDEX_VARIABLE_DECLARATION_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// For Loop Cpp Index Variable Declaration Analysis
// Author: Gary M. Yuan
// Date: 24-July-2007

#include "compass.h"
// #include "forLoopCppIndexVariableDeclaration.h"

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
   : output(output)
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

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::ForLoopCppIndexVariableDeclaration::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::ForLoopCppIndexVariableDeclaration::Traversal(params, output);
}

extern const Compass::Checker* const forLoopCppIndexVariableDeclarationChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::ForLoopCppIndexVariableDeclaration::checkerName,
        CompassAnalyses::ForLoopCppIndexVariableDeclaration::shortDescription,
        CompassAnalyses::ForLoopCppIndexVariableDeclaration::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
