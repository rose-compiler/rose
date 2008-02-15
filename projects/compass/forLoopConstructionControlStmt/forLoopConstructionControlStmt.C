// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// For Loop Construction Control Stmt Analysis
// Author: Gary M. Yuan
// Date: 23-July-2007

#include "compass.h"
#include "forLoopConstructionControlStmt.h"

namespace CompassAnalyses
   { 
     namespace ForLoopConstructionControlStmt
        { 
          const std::string checkerName      = "ForLoopConstructionControlStmt";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "This checker checks that only loop control expressions appear in the for loop constructor block";

          const std::string longDescription  = "Unlike while loop constructs, non-control statements in for loop constructors tend to appear beyond the test expression. Often these statements may be moved in to the body of the for loop construct easily and results in greater clarity in the for loop construction code.";
        } //End of namespace ForLoopConstructionControlStmt.
   } //End of namespace CompassAnalyses.

CompassAnalyses::ForLoopConstructionControlStmt::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::ForLoopConstructionControlStmt::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["ForLoopConstructionControlStmt.YourParameter"]);


   }

void
CompassAnalyses::ForLoopConstructionControlStmt::Traversal::
visit(SgNode* node)
   { 
     SgCommaOpExp *commaStmt = isSgCommaOpExp( node );

     if( commaStmt != NULL )
     {
       SgNode *parent = node->get_parent();

       if( parent != NULL && isSgForStatement(parent) != NULL )
         output->addOutput( new CheckerOutput( parent ) );
     } //if( commaStmt != NULL )

     return;
   } //End of the visit function.
