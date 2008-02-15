// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Function Documentation Analysis
// Author: Thomas Panas
// Date: 23-July-2007

#include "compass.h"
#include "functionDocumentation.h"

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
  : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
{
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["FunctionDocumentation.YourParameter"]);
  

}

void
CompassAnalyses::FunctionDocumentation::Traversal::
visit(SgNode* sgNode)
{ 
  // Implement your traversal here.  
  if (isSgFunctionDeclaration(sgNode)) {
    SgFunctionDeclaration* funcDecl = isSgFunctionDeclaration(sgNode);
    bool skip = funcDecl->get_file_info()->isCompilerGenerated();
    if (skip)
      return;
    //                      SgFunctionDefinition* funcDef = funcDecl->get_definition();
    AttachedPreprocessingInfoType* comments = funcDecl->getAttachedPreprocessingInfo();
    bool isComment = false;
    if (comments!=NULL) {
      AttachedPreprocessingInfoType::iterator i;
      for (i=comments->begin(); i!= comments->end(); i++) {
        std::string commentStr = (*i)->getString().c_str() ;
        if (commentStr.find("//")!=std::string::npos ||
            commentStr.find("/*")!=std::string::npos ) {
          isComment=true;
        }
      }
    }
    if (!isComment) {
      std::string funcName = funcDecl->get_qualified_name();
      output->addOutput(new CheckerOutput(funcDecl, funcName));
    }
  }
} //End of the visit function.
   
