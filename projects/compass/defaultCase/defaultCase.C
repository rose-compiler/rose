// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Default Case Analysis
// Author: pants,,,
// Date: 24-July-2007

#include "compass.h"
#include "defaultCase.h"

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
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
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
        SgBasicBlock* BBlock = theSwitch->get_body();
        //I should maybe do more sanity checking for nulls here
        SgStatementPtrList BBlockStmts = BBlock->get_statements();
        bool has_default = false;
        for (Rose_STL_Container<SgStatement*>::iterator j = BBlockStmts.begin(); j != BBlockStmts.end(); j++)
          {
            if (isSgDefaultOptionStmt(*j)){
              has_default = true;
              break;
            }
          }
        if (!has_default){
          output->addOutput(new CheckerOutput(node));
        }
  // Implement your traversal here.  

   } //End of the visit function.
   
