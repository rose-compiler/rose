// Default Case
// Author: pants,,,
// Date: 24-July-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_DEFAULT_CASE_H
#define COMPASS_DEFAULT_CASE_H

namespace CompassAnalyses
   { 
     namespace DefaultCase
        { 
        /*! \brief Default Case: Add your description here 
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

// COMPASS_DEFAULT_CASE_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Default Case Analysis
// Author: pants,,,
// Date: 24-July-2007

#include "compass.h"
// #include "defaultCase.h"

namespace CompassAnalyses
   { 
     namespace DefaultCase
        { 
          const std::string checkerName      = "DefaultCase";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Finding switch statements with no default";
          const std::string longDescription  = "Finds switch statments that do not have a default case in their basic block.";
        } //End of namespace DefaultCase.
   } //End of namespace CompassAnalyses.

CompassAnalyses::DefaultCase::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::DefaultCase::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["DefaultCase.YourParameter"]);


   }

void
CompassAnalyses::DefaultCase::Traversal::
visit(SgNode* node)
   {
     
        SgSwitchStatement* theSwitch = isSgSwitchStatement(node);
        if (!theSwitch) return;
        bool has_default = false;
        if (isSgBasicBlock(theSwitch->get_body())) {
          SgBasicBlock* BBlock = isSgBasicBlock(theSwitch->get_body());
          //I should maybe do more sanity checking for nulls here
          SgStatementPtrList BBlockStmts = BBlock->get_statements();
          for (Rose_STL_Container<SgStatement*>::iterator j = BBlockStmts.begin(); j != BBlockStmts.end(); j++)
            {
              if (isSgDefaultOptionStmt(*j)){
                has_default = true;
                break;
              }
            }
        } else {
          if (isSgDefaultOptionStmt(theSwitch->get_body())) {
            has_default = true;
          }
        }
        if (!has_default){
          output->addOutput(new CheckerOutput(node));
        }
  // Implement your traversal here.  

   } //End of the visit function.
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
     CompassAnalyses::DefaultCase::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::DefaultCase::Traversal(params, output);
}

extern const Compass::Checker* const defaultCaseChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::DefaultCase::checkerName,
        CompassAnalyses::DefaultCase::shortDescription,
        CompassAnalyses::DefaultCase::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
