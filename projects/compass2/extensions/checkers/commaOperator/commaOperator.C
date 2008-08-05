// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Comma Operator Analysis
// Author: Gergo  Barany
// Date: 07-August-2007

#include "compass.h"
#include "commaOperator.h"
#include <cstring>

namespace CompassAnalyses
   { 
     namespace CommaOperator
        { 
          const std::string checkerName      = "CommaOperator";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "The comma operator should not be used.";
          const std::string longDescription  = "The comma operator should not be used.";
        } //End of namespace CommaOperator.
   } //End of namespace CompassAnalyses.

CompassAnalyses::CommaOperator::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::CommaOperator::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
   {
   }

void
CompassAnalyses::CommaOperator::Traversal::
visit(SgNode* node)
   { 
     if (isSgCommaOpExp(node))
     {
       // simple case: built-in comma operator
       output->addOutput(new CheckerOutput(node));
     }
     else if (SgFunctionDeclaration *fd = isSgFunctionDeclaration(node))
     {
       if (std::strcmp(fd->get_name().str(), "operator,") == 0)
         output->addOutput(new CheckerOutput(node));
     }
   } //End of the visit function.
   
