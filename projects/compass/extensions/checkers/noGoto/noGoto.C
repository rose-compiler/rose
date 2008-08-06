// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// No Goto Analysis
// Author: Valentin  David
// Date: 02-August-2007

#include "compass.h"
#include "noGoto.h"

namespace CompassAnalyses {
  namespace NoGoto {
    const std::string checkerName      = "NoGoto";

    const std::string shortDescription = "Check for goto statements";
    const std::string longDescription  = "This checker looks for gotos.";
  } //End of namespace NoGoto.
} //End of namespace CompassAnalyses.

CompassAnalyses::NoGoto::
CheckerOutput::CheckerOutput(SgNode* node)
  : OutputViolationBase(node,checkerName,"goto found")
{}

CompassAnalyses::NoGoto::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
  : Compass::TraversalBase(output, checkerName,
                           shortDescription, longDescription)
{
}

void
CompassAnalyses::NoGoto::Traversal::
visit(SgNode* node)
{
  SgGotoStatement* gto = isSgGotoStatement(node);
  if (gto == NULL)
    return ;
  output->addOutput(new CheckerOutput(node));
} //End of the visit function.
