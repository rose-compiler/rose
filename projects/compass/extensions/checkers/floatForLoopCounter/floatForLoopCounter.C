// Float For Loop Counter
// Author: Gary M. Yuan
// Date: 11-September-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_FLOAT_FOR_LOOP_COUNTER_H
#define COMPASS_FLOAT_FOR_LOOP_COUNTER_H

namespace CompassAnalyses
   { 
     namespace FloatForLoopCounter
        { 
        /*! \brief Float For Loop Counter: Add your description here 
         */

          extern const std::string checkerName;
          extern const std::string shortDescription;
          extern const std::string longDescription;

       // Specification of Checker Output Implementation
          class CheckerOutput: public Compass::OutputViolationBase
             { 
               public:
                    CheckerOutput(SgNode* node);
             };

       // Specification of Checker Traversal Implementation

          class Traversal
             : public Compass::AstSimpleProcessingWithRunFunction
             {
            // Checker specific parameters should be allocated here.
               Compass::OutputObject* output;

               public:
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

                 // Change the implementation of this function if you are using inherited attributes.
                    void *initialInheritedAttribute() const { return NULL; }

                 // The implementation of the run function has to match the traversal being called.
                 // If you use inherited attributes, use the following definition:
                 // void run(SgNode* n){ this->traverse(n, initialInheritedAttribute()); }
                    void run(SgNode* n){ this->traverse(n, preorder); }

                 // Change this function if you are using a different type of traversal, e.g.
                 // void *evaluateInheritedAttribute(SgNode *, void *);
                 // for AstTopDownProcessing.
                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_FLOAT_FOR_LOOP_COUNTER_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Float For Loop Counter Analysis
// Author: Gary M. Yuan
// Date: 11-September-2007

#include "compass.h"
// #include "floatForLoopCounter.h"

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
   : output(output)
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

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::FloatForLoopCounter::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::FloatForLoopCounter::Traversal(params, output);
}

extern const Compass::Checker* const floatForLoopCounterChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::FloatForLoopCounter::checkerName,
        CompassAnalyses::FloatForLoopCounter::shortDescription,
        CompassAnalyses::FloatForLoopCounter::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
