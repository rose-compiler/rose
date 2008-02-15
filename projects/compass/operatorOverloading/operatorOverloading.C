// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Operator Overloading Analysis
// Author: Han Suk  Kim
// Date: 24-July-2007

#include "compass.h"
#include "operatorOverloading.h"

namespace CompassAnalyses
   { 
     namespace OperatorOverloading
        { 
          const std::string checkerName      = "OperatorOverloading";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Overloading operator&&, operator||, or operator,(comma) may cause subtle bugs!";
          const std::string longDescription  = "If any of operator&&, operator|| or operator,(comma) is overloaded and if the expression (expr1 && expr2) is used, both expr1 and expr2 are evaluated. But this is not the case for normal test expressions due to short-curcuiting. In addition, the order of evaluation is not specified for these two expressions, which may cause another problem.";
        } //End of namespace OperatorOverloading.
   } //End of namespace CompassAnalyses.

CompassAnalyses::OperatorOverloading::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::OperatorOverloading::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["OperatorOverloading.YourParameter"]);


   }

void
CompassAnalyses::OperatorOverloading::Traversal::
visit(SgNode* node)
   { 
  // Implement your traversal here.  

        if(NULL != isSgFile(node))
        {
          Rose_STL_Container<SgNode*> functionDeclarationList = NodeQuery::querySubTree(node, V_SgFunctionDeclaration);

          for(Rose_STL_Container<SgNode*>::iterator i = functionDeclarationList.begin(); i != functionDeclarationList.end(); i++)
          {
            SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(*i);

            // checker
            std::string funcName = functionDeclaration->get_name().str();
            if(funcName == "operator&&" || funcName == "operator||" || funcName == "operator,")
            {
              output->addOutput(new CheckerOutput(functionDeclaration));
            }
          }
        }


   } //End of the visit function.
   
