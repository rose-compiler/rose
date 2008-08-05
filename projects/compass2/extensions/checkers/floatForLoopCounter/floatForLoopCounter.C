// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Float For Loop Counter Analysis
// Author: Gary M. Yuan
// Date: 11-September-2007

#include "compass.h"
#include "floatForLoopCounter.h"

namespace CompassAnalyses
   { 
     namespace FloatForLoopCounter
        { 
          const std::string checkerName      = "FloatForLoopCounter";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "This checker checks that floating point variables are not used as for loop counters";
          const std::string longDescription  = "Floating point arithmetic is inexact and is subject to rounding errors. Hence, floating point variables should not be used as loop counters.";
        } //End of namespace FloatForLoopCounter.
   } //End of namespace CompassAnalyses.

CompassAnalyses::FloatForLoopCounter::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::FloatForLoopCounter::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["FloatForLoopCounter.YourParameter"]);


   }

void
CompassAnalyses::FloatForLoopCounter::Traversal::
visit(SgNode* node)
   {
     SgForInitStatement *f = isSgForInitStatement( node );

     if( f != NULL )
     {
       SgStatementPtrList list = f->get_init_stmt();

       for( SgStatementPtrList::const_iterator itr = list.begin(); 
            itr != list.end(); itr++ )
       {
         SgVariableDeclaration *d = isSgVariableDeclaration( *itr );

         if( d != NULL )
         {
           SgType *t = d->get_definition()->get_type();

           if( isSgTypeFloat(t) != NULL || isSgTypeDouble(t) != NULL )
             output->addOutput( new CheckerOutput(d) );

         } //if( d != NULL )

       } //for, itr
     } //if( f != NULL )

     return;
   } //End of the visit function.
