// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Sub Expression Evaluation Order Analysis
// Author: Han Suk  Kim
// Date: 24-July-2007

#include "compass.h"
#include "subExpressionEvaluationOrder.h"

namespace CompassAnalyses
   { 
     namespace SubExpressionEvaluationOrder
        { 
          const std::string checkerName      = "SubExpressionEvaluationOrder";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "The order of evaluation of sub-expressions is not guaranteed to be left-to-right.";
          const std::string longDescription  = "Tests if there is a function call that takes more than two parameters and if the parameters are being updated by sub-expressions such as ++ operations.";
        } //End of namespace SubExpressionEvaluationOrder.
   } //End of namespace CompassAnalyses.

CompassAnalyses::SubExpressionEvaluationOrder::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::SubExpressionEvaluationOrder::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["SubExpressionEvaluationOrder.YourParameter"]);


   }

void
CompassAnalyses::SubExpressionEvaluationOrder::Traversal::
visit(SgNode* node)
   { 
  // Implement your traversal here.  
        Rose_STL_Container<std::string> variableNameList;

        if(NULL != isSgFunctionCallExp(node))
        {
          SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(node);

          Rose_STL_Container<SgExpression*> parameterList = functionCallExp->get_args()->get_expressions();

          // for each parameter in a function call
          for(Rose_STL_Container<SgExpression*>::iterator i = parameterList.begin(); i != parameterList.end(); i++)
          {
            bool duplicatedVariable = false;
            std::string variableName;
            SgVarRefExp* varRefExp = NULL;

            if(isSgPlusPlusOp(*i) != NULL)
              varRefExp = isSgVarRefExp(isSgPlusPlusOp(*i)->get_operand_i());

            else if(isSgMinusMinusOp(*i) != NULL)
              varRefExp = isSgVarRefExp(isSgMinusMinusOp(*i)->get_operand_i());

            else if (isSgAssignOp(*i) != NULL)
              varRefExp = isSgVarRefExp(isSgAssignOp(*i)->get_lhs_operand_i());

            if(varRefExp != NULL)
            {
              // check if there is an expression previously updating the parameter
              variableName = varRefExp->get_symbol()->get_name().getString();

              duplicatedVariable = false;
              for(Rose_STL_Container<std::string>::iterator j = variableNameList.begin(); j != variableNameList.end(); j++)
              {
                if(*j == variableName)
                {
                  output->addOutput(new CheckerOutput(*i));
                  duplicatedVariable = true;
                }
              }
              if(duplicatedVariable == false)
                variableNameList.push_back(variableName);
            }
          }
        }

   } //End of the visit function.
   
