// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Cyclomatic Complexity Analysis
// Author: Thomas Panas
// Date: 23-July-2007

#include "compass.h"
#include "cyclomaticComplexity.h"

namespace CompassAnalyses
{ 
  namespace CyclomaticComplexity
  { 
    /*namespace Traversal {
      int cc=0;
      int complexity=0;
    }*/
    //GMY 12/26/2007 changed namespace Traversal to explicit scope because of
    //error in EDG front-end processing.
    //These are static data member initializations of class Traversal
    int Traversal::cc = 0;
    int Traversal::complexity = 0;

    //12/26/2007 END

    const std::string checkerName      = "CyclomaticComplexity";

    // Descriptions should not include the newline character "\n".
    std::string shortDescription = 
    "This function is too complex : CC = ";
    std::string longDescription  = 
    "This analysis detects functions with lots of branch conditions. The analysis is implemented according to Mc Cabe's Cyclomatic Complexity. The CC value for this analysis  = ";
  } //End of namespace CyclomaticComplexity.
} //End of namespace CompassAnalyses.

CompassAnalyses::CyclomaticComplexity::
CheckerOutput::CheckerOutput ( SgNode* node )
  : OutputViolationBase(node,checkerName,shortDescription+
                        Traversal::getComplexity()+" > "+Traversal::getCC())
{}

CompassAnalyses::CyclomaticComplexity::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
  : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
{
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["CyclomaticComplexity.YourParameter"]);
  cc = Compass::parseInteger(inputParameters["CyclomaticComplexity.maxComplexity"]);
  setShortDescription(shortDescription + Traversal::getComplexity()+" > "+Traversal::getCC());
  setLongDescription (longDescription + Traversal::getComplexity()+" > "+Traversal::getCC());
}

void CompassAnalyses::CyclomaticComplexity::Traversal::
checkNode(SgNode* node) {
  if (isSgIfStmt(node) || isSgCaseOptionStmt(node) || isSgForStatement(node) || isSgDoWhileStmt(node) || isSgWhileStmt(node)) {
    complexity++;
  }
}


int CompassAnalyses::CyclomaticComplexity::Traversal::
checkDepth(std::vector<SgNode*> children) {
  int nrchildren=0;
  for (unsigned int i=0; i< children.size(); i++) {
    SgNode* node = children[i];
    if (node==NULL) {
      continue;
    }
    std::vector<SgNode*> childrenRec = node->get_traversalSuccessorContainer();
    if (childrenRec.size()>0) {
      nrchildren+=checkDepth(childrenRec);
    }
    nrchildren += 1;
    checkNode(node);
  }
  return nrchildren;
}


void
CompassAnalyses::CyclomaticComplexity::Traversal::
visit(SgNode* node)
{ 
  if (isSgFunctionDeclaration(node)) {
    complexity=0;
    // Implement your traversal here.  
    SgFunctionDeclaration* funcDecl = isSgFunctionDeclaration(node);
    // iterate the children and check for problems.
    std::vector<SgNode*> children = funcDecl->get_traversalSuccessorContainer();
    checkDepth(children);
    if (complexity>cc)
        output->addOutput(new CheckerOutput(funcDecl));
  }
} //End of the visit function.
   
