// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Buffer Overflow Functions Analysis
// Author: Thomas Panas
// Date: 23-July-2007

#include "compass.h"
#include "bufferOverflowFunctions.h"

namespace CompassAnalyses
{ 
  namespace BufferOverflowFunctions
  { 
    const std::string checkerName      = "BufferOverflowFunctions";

    // Descriptions should not include the newline character "\n".
    const std::string shortDescription = "Detects functions that may cause a buffer overflow.";
    const std::string longDescription  = "This checker detects functions that may cause an buffer overflow if used inpropriate. For more information, check the manual.";
  } //End of namespace BufferOverflowFunctions.
} //End of namespace CompassAnalyses.

CompassAnalyses::BufferOverflowFunctions::
CheckerOutput::CheckerOutput ( std::string problem, SgNode* node )
  : OutputViolationBase(node,checkerName,shortDescription+" "+problem)
{}

CompassAnalyses::BufferOverflowFunctions::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
  : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
{
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["BufferOverflowFunctions.YourParameter"]);

}

void
CompassAnalyses::BufferOverflowFunctions::Traversal::
visit(SgNode* node)
{ 
  // Implement your traversal here.  
  if (isSgFunctionCallExp(node)) {
    SgFunctionCallExp* fcexp = isSgFunctionCallExp( node);
    std::vector<SgNode*> sgExprListVec = fcexp->get_traversalSuccessorContainer();
    for (unsigned int i=0; i< sgExprListVec.size() ; i++) {
      if (isSgFunctionRefExp(sgExprListVec[i])) {
        SgFunctionRefExp* funcRefExp = (SgFunctionRefExp*) sgExprListVec[i];
        SgFunctionSymbol* funcSymbol = funcRefExp->get_symbol();
        std::string name = funcSymbol->get_name().str();
        //                  std::cout << " --- found SgFunctionRefExp: " << name << std::endl;
        if (name.compare("sprintf")==0) {
          std::string problem = "found sprintf(). Use snprintf instead. ";
          output->addOutput(new CheckerOutput(problem, node));
        }
        if (name.compare("scanf")==0) {
          std::string problem = "found scanf().";
          output->addOutput(new CheckerOutput(problem, node));
        }
        if (name.compare("sscanf")==0) {
          std::string problem = "found sscanf().";
          output->addOutput(new CheckerOutput(problem, node));
        }
        if (name.compare("gets")==0) {
          std::string problem = "found gets(). Use fgets() instead.";
          output->addOutput(new CheckerOutput(problem, node));
        }
        if (name.compare("strcpy")==0) {
          std::string problem = "found strcpy(). Use strncpy() instead.";
          output->addOutput(new CheckerOutput(problem, node));
        }
        if (name.compare("_mbscpy")==0) {
          std::string problem = "found _mbscpy().";
          output->addOutput(new CheckerOutput(problem, node));
        }
        if (name.compare("lstrcat")==0) {
          std::string problem = "found lstrcat().";
          output->addOutput(new CheckerOutput(problem, node));
        }
        if (name.compare("memcpy")==0) {
          std::string problem = "found memcpy(). Check for buffer overflow when copying to destination.";
          output->addOutput(new CheckerOutput(problem, node));
        }
        if (name.compare("strcat")==0) {
          std::string problem = "found strcat(). Use strncat() instead.";
          output->addOutput(new CheckerOutput(problem, node));
        }
      }
    }
  }

} //End of the visit function.
   
