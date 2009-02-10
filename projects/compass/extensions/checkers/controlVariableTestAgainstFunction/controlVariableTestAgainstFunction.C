// Control Variable Test Against Function
// Author: Han Suk  Kim
// Date: 24-July-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_CONTROL_VARIABLE_TEST_AGAINST_FUNCTION_H
#define COMPASS_CONTROL_VARIABLE_TEST_AGAINST_FUNCTION_H

namespace CompassAnalyses
   { 
     namespace ControlVariableTestAgainstFunction
        { 
        /*! \brief Control Variable Test Against Function: Add your description here 
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

// COMPASS_CONTROL_VARIABLE_TEST_AGAINST_FUNCTION_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Control Variable Test Against Function Analysis
// Author: Han Suk  Kim
// Date: 24-July-2007

#include "compass.h"
// #include "controlVariableTestAgainstFunction.h"

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
   : output(output)
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
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::ControlVariableTestAgainstFunction::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::ControlVariableTestAgainstFunction::Traversal(params, output);
}

extern const Compass::Checker* const controlVariableTestAgainstFunctionChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::ControlVariableTestAgainstFunction::checkerName,
        CompassAnalyses::ControlVariableTestAgainstFunction::shortDescription,
        CompassAnalyses::ControlVariableTestAgainstFunction::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
