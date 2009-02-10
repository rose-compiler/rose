// Function Documentation
// Author: Thomas Panas
// Date: 23-July-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_FUNCTION_DOCUMENTATION_H
#define COMPASS_FUNCTION_DOCUMENTATION_H

namespace CompassAnalyses
   { 
     namespace FunctionDocumentation
        { 
        /*! \brief Function Documentation: Add your description here 
         */

          extern const std::string checkerName;
          extern  std::string shortDescription;
          extern const std::string longDescription;

       // Specification of Checker Output Implementation
          class CheckerOutput: public Compass::OutputViolationBase
             { 
               public:
                    CheckerOutput(SgNode* node, std::string funcname);
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

// COMPASS_FUNCTION_DOCUMENTATION_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Function Documentation Analysis
// Author: Thomas Panas
// Date: 23-July-2007

#include "compass.h"
// #include "functionDocumentation.h"

namespace CompassAnalyses
{ 
  namespace FunctionDocumentation
  { 
    const std::string checkerName      = "FunctionDocumentation";

    // Descriptions should not include the newline character "\n".
    std::string shortDescription       = "function is not documented: name = ";
    const std::string longDescription  = "This analysis identifies all functions that have not been documented above the function declaration.";
  } // End of namespace FunctionDocumentation.
} // End of namespace CompassAnalyses.

CompassAnalyses::FunctionDocumentation::
CheckerOutput::CheckerOutput ( SgNode* node , std::string funcname)
  : OutputViolationBase(node,checkerName,shortDescription+" "+funcname)
{}

CompassAnalyses::FunctionDocumentation::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
  : output(output)
{
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["FunctionDocumentation.YourParameter"]);
  

}


bool isDocumented(SgFunctionDeclaration* funcDecl)
{
  bool isComment = false;

  if(funcDecl == NULL )
    return isComment;

  AttachedPreprocessingInfoType* comments =
    funcDecl->getAttachedPreprocessingInfo();

  if (comments!=NULL) {
    AttachedPreprocessingInfoType::iterator i;
    for (i=comments->begin(); i!= comments->end(); i++) {
      PreprocessingInfo::DirectiveType dirType = (*i)->getTypeOfDirective();

      if( dirType == PreprocessingInfo::C_StyleComment || 
          dirType == PreprocessingInfo::CplusplusStyleComment )
      {
        isComment =true;
        break;
      }
    }
  }

  return isComment;
};

void
CompassAnalyses::FunctionDocumentation::Traversal::
visit(SgNode* sgNode)
{ 
  // Implement your traversal here.  
  if (isSgFunctionDeclaration(sgNode)) 
    {
      SgFunctionDeclaration* funcDecl = isSgFunctionDeclaration(sgNode);
      if (funcDecl->get_file_info()->isCompilerGenerated() 
          || funcDecl->get_definingDeclaration() != sgNode
          )
        return;


      if (!( isDocumented( funcDecl) || 
          isDocumented(isSgFunctionDeclaration(funcDecl->get_firstNondefiningDeclaration()))
           ) 
         ) {
        std::string funcName = funcDecl->get_qualified_name();
        output->addOutput(new CheckerOutput(funcDecl, funcName));
      }

    }




} //End of the visit function.
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::FunctionDocumentation::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::FunctionDocumentation::Traversal(params, output);
}

extern const Compass::Checker* const functionDocumentationChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::FunctionDocumentation::checkerName,
        CompassAnalyses::FunctionDocumentation::shortDescription,
        CompassAnalyses::FunctionDocumentation::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
