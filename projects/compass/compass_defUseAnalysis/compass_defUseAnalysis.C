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
    /*namespace Traversal {
      int loc=0;
      int loc_actual=0;
    }*/
    //GMY 12/26/2007 changed namespace Traversal to explicit scope because of
    //error in EDG front-end processing.
    //These are static data member initializations of class Traversal

    int Traversal::loc = 0;
    int Traversal::loc_actual = 0;
    //12/26/2007 END

    const std::string checkerName      = "Compass_defUseAnalysis";

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

  loc = Compass::parseInteger(inputParameters["Compass_defUseAnalysis.Size"]);
  setShortDescription(shortDescription +  Traversal::getLOC_actual()+" > "+Traversal::getLOC());
  setLongDescription (longDescription +  Traversal::getLOC_actual()+" > "+Traversal::getLOC());
}


void
CompassAnalyses::Compass_defUseAnalysis::Traversal::
visit(SgNode* sgNode)
{ 
  // Implement your traversal here.  
  if (isSgFunctionDeclaration(sgNode)) {
    SgFunctionDeclaration* funcDecl = isSgFunctionDeclaration(sgNode);
    SgFunctionDefinition* funcDef = funcDecl->get_definition();
    if (funcDef) {
      Sg_File_Info* start = funcDef->get_body()->get_startOfConstruct();
      Sg_File_Info* end = funcDef->get_body()->get_endOfConstruct();
      ROSE_ASSERT(start);
      ROSE_ASSERT(end);
      int lineS = start->get_line();
      int lineE = end->get_line();
      loc_actual = lineE-lineS;
      if (loc_actual>loc) {
        output->addOutput(new CheckerOutput(funcDef));
      }
    }
  }

} //End of the visit function.
   
