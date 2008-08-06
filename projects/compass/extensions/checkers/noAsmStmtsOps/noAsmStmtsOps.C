// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// No Asm Stmts Ops Analysis
// Author: Gary Yuan
// Date: 19-December-2007

#include "compass.h"
#include "noAsmStmtsOps.h"

namespace CompassAnalyses
   { 
     namespace NoAsmStmtsOps
        { 
          const std::string checkerName      = "NoAsmStmtsOps";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "This checker checks that no C assembly statements or ops occur.";
          const std::string longDescription  = "This checker checks that no C assembly statements or ops occur.";
        } //End of namespace NoAsmStmtsOps.
   } //End of namespace CompassAnalyses.

CompassAnalyses::NoAsmStmtsOps::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::NoAsmStmtsOps::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["NoAsmStmtsOps.YourParameter"]);


   }

void
CompassAnalyses::NoAsmStmtsOps::Traversal::
visit(SgNode* node)
   {
     SgAsmStmt *stmt = isSgAsmStmt(node);
     SgAsmOp *op = isSgAsmOp(node);

     if( stmt || op )
       output->addOutput( new CheckerOutput(node) );

     return;
   } //End of the visit function.
