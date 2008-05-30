// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Loc Per Function Analysis
// Author: Thomas Panas
// Date: 23-July-2007

#include "compass.h"
#include "compass_defUseAnalysis.h"


namespace CompassAnalyses
{ 
  namespace Compass_defUseAnalysis
  { 

    int Traversal::loc = 0;
    int Traversal::loc_actual = 0;

    const std::string checkerName      = "Compass_defUseAnalysis";

    bool Traversal::projectSet = false;
    DFAnalysis* Traversal::defuse = NULL;

    // Descriptions should not include the newline character "\n".
    std::string shortDescription = 
        "This function has too many lines of code :: LOC = " ;
    std::string longDescription  = 
    "This analysis detects functions with lines of code larger than a predefined value. The current value is ";
  } //End of namespace Compass_defUseAnalysis.
} //End of namespace CompassAnalyses.

CompassAnalyses::Compass_defUseAnalysis::
CheckerOutput::CheckerOutput ( SgNode* node )
  : OutputViolationBase(node,checkerName,shortDescription+
                        Traversal::getLOC_actual()+" > "+Traversal::getLOC())
{}

CompassAnalyses::Compass_defUseAnalysis::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
  : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
{
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["Compass_defUseAnalysis.YourParameter"]);

  //  loc = Compass::parseInteger(inputParameters["Compass_defUseAnalysis.Size"]);
  setShortDescription(shortDescription +  Traversal::getLOC_actual()+" > "+Traversal::getLOC());
  setLongDescription (longDescription +  Traversal::getLOC_actual()+" > "+Traversal::getLOC());
}


void
CompassAnalyses::Compass_defUseAnalysis::Traversal::
visit(SgNode* sgNode)
{ 
  if (projectSet==false) {
    SgProject* project = TransformationSupport::getProject(sgNode);
    defuse = new DefUseAnalysis(project);
    projectSet=true;
  }
  // Implement your traversal here.  
  if (isSgFunctionDefinition(sgNode)) {
    SgFunctionDefinition* funcDef = isSgFunctionDefinition(sgNode);
    int nrNodes = ((DefUseAnalysis*)defuse)->start_traversal_of_one_function(funcDef);
    output->addOutput(new CheckerOutput(funcDef));
  }

} //End of the visit function.
   
