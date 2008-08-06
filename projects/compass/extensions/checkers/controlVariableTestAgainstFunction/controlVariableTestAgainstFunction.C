// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Control Variable Test Against Function Analysis
// Author: Han Suk  Kim
// Date: 24-July-2007

#include "compass.h"
#include "controlVariableTestAgainstFunction.h"

namespace CompassAnalyses
   { 
     namespace ControlVariableTestAgainstFunction
        { 
          const std::string checkerName      = "ControlVariableTestAgainstFunction";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "The control variable is tested against a function.";
          const std::string longDescription  = "If a control variable is tested against a function, then the function is computed every time the test is invoked. Assigning the return value of a function to a variable and using the variable may increase the performance.";
        } //End of namespace ControlVariableTestAgainstFunction.
   } //End of namespace CompassAnalyses.

CompassAnalyses::ControlVariableTestAgainstFunction::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::ControlVariableTestAgainstFunction::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["ControlVariableTestAgainstFunction.YourParameter"]);


   }

void
CompassAnalyses::ControlVariableTestAgainstFunction::Traversal::
visit(SgNode* node)
   { 
  // Implement your traversal here.  

        if(NULL != isSgForStatement(node))
        {
          SgForStatement* forStatement = isSgForStatement(node);

          Rose_STL_Container<SgNode*> functionCallExpList = NodeQuery::querySubTree(forStatement->get_test(), V_SgFunctionCallExp);

          for(Rose_STL_Container<SgNode*>::iterator i = functionCallExpList.begin(); i != functionCallExpList.end(); i++)
          {
            output->addOutput(new CheckerOutput(*i));
          }
        }


   } //End of the visit function.
   
