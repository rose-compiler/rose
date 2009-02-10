// Operator Overloading
// Author: Han Suk  Kim
// Date: 24-July-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_OPERATOR_OVERLOADING_H
#define COMPASS_OPERATOR_OVERLOADING_H

namespace CompassAnalyses
   { 
     namespace OperatorOverloading
        { 
        /*! \brief Operator Overloading: Add your description here 
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

// COMPASS_OPERATOR_OVERLOADING_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Operator Overloading Analysis
// Author: Han Suk  Kim
// Date: 24-July-2007

#include "compass.h"
// #include "operatorOverloading.h"

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
   : output(output)
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
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::OperatorOverloading::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::OperatorOverloading::Traversal(params, output);
}

extern const Compass::Checker* const operatorOverloadingChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::OperatorOverloading::checkerName,
        CompassAnalyses::OperatorOverloading::shortDescription,
        CompassAnalyses::OperatorOverloading::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
