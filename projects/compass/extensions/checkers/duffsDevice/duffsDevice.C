// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Duff's Device Analysis
// Author: Gergo Barany
// Date: 24-July-2007

#include "compass.h"
#include "duffsDevice.h"

namespace CompassAnalyses
   { 
     namespace DuffsDevice
        { 
          const std::string checkerName      = "DuffsDevice";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Case label inside loop inside switch.";
          const std::string longDescription  = "The infamous Duff's Device, a"
            " construct consisting of a case label within a loop within the"
            " enclosing switch statement, is encountered.";
        } //End of namespace DuffsDevice.
   } //End of namespace CompassAnalyses.

CompassAnalyses::DuffsDevice::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::DuffsDevice::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
   {
   }

void
CompassAnalyses::DuffsDevice::Traversal::
visit(SgNode* node)
   { 
     if (isSgCaseOptionStmt(node) || isSgDefaultOptionStmt(node))
     {
       SgNode *p = node->get_parent();
       while (p != NULL && !isSgSwitchStatement(p) && !isLoopStatement(p))
         p = p->get_parent();
       if (isLoopStatement(p))
         output->addOutput(new CheckerOutput(p)); // report the loop as Duff's Device
     }
   } //End of the visit function.
   
bool
CompassAnalyses::DuffsDevice::Traversal::
isLoopStatement(SgNode* n) const
{
  return isSgDoWhileStmt(n) || isSgForStatement(n) || isSgWhileStmt(n);
}
