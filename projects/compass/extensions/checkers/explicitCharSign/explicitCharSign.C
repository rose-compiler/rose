// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Explicit Char Sign Analysis
// Author: Gary M. Yuan
// Date: 1-December-2007

#include "compass.h"
#include "explicitCharSign.h"

namespace CompassAnalyses
   { 
     namespace ExplicitCharSign
        { 
          const std::string checkerName      = "ExplicitCharSign";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "This checker checks that all char declarations initialized with type int are explitly signed or unsigned.";
          const std::string longDescription  = "The three types char, signed char, and unsigned char are collectively called the character types. Compilers have the latitude to define char to have the same range, representation, and behavior as either signed char or unsigned char. Irrespective of the choice made, char is a separate type from the other two and is not compatible with either.";
        } //End of namespace ExplicitCharSign.
   } //End of namespace CompassAnalyses.

CompassAnalyses::ExplicitCharSign::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::ExplicitCharSign::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["ExplicitCharSign.YourParameter"]);


   }

void
CompassAnalyses::ExplicitCharSign::Traversal::
visit(SgNode* node)
   {
     SgAssignInitializer *sgAssign = isSgAssignInitializer(node);

     if( sgAssign && isSgTypeChar(sgAssign->get_type()) )
     {
       SgCastExp *sgCast = isSgCastExp(sgAssign->get_operand());
       SgCharVal *sgChar = isSgCharVal(sgAssign->get_operand());

       if( (sgCast && isSgTypeInt(sgCast->get_operand()->get_type())) ||
           (sgChar && (int)sgChar->get_value() < 0) )
       {
         output->addOutput( new CheckerOutput( node ) );
       } //if
     } //if( sgAssign && isSgTypeChar(sgAssign->get_type()) )

     return;
   } //End of the visit function. 
