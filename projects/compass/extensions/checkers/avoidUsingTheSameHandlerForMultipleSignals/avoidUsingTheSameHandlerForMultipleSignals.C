// Avoid Using The Same Handler For Multiple Signals
// Author: Mark Lewandowski, -422-3849
// Date: 26-November-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_AVOID_USING_THE_SAME_HANDLER_FOR_MULTIPLE_SIGNALS_H
#define COMPASS_AVOID_USING_THE_SAME_HANDLER_FOR_MULTIPLE_SIGNALS_H

namespace CompassAnalyses
   { 
     namespace AvoidUsingTheSameHandlerForMultipleSignals
        { 
        /*! \brief Avoid Using The Same Handler For Multiple Signals: Add your description here 
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

// COMPASS_AVOID_USING_THE_SAME_HANDLER_FOR_MULTIPLE_SIGNALS_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Avoid Using The Same Handler For Multiple Signals Analysis
// Author: Mark Lewandowski, -422-3849
// Date: 26-November-2007

#include "compass.h"
// #include "avoidUsingTheSameHandlerForMultipleSignals.h"

namespace CompassAnalyses
   { 
     namespace AvoidUsingTheSameHandlerForMultipleSignals
        { 
          const std::string checkerName      = "AvoidUsingTheSameHandlerForMultipleSignals";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Each signal handler should be registered to handle only one type of signal.";
          const std::string longDescription  = "It is possible to safely use the same handler for multiple signals, but doing so increases the likelihood of a security vulnerability. The delivered signal is masked and is not delivered until the registered signal handler exits. However, if this same handler is registered to handle a different signal, execution of the handler may be interrupted by this new signal. If a signal handler is constructed with the expectation that it cannot be interrupted, a vulnerability might exist. To eliminate this attack vector, each signal handler should be registered to handle only one type of signal.";
        } //End of namespace AvoidUsingTheSameHandlerForMultipleSignals.
   } //End of namespace CompassAnalyses.

CompassAnalyses::AvoidUsingTheSameHandlerForMultipleSignals::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::AvoidUsingTheSameHandlerForMultipleSignals::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["AvoidUsingTheSameHandlerForMultipleSignals.YourParameter"]);


   }

void
CompassAnalyses::AvoidUsingTheSameHandlerForMultipleSignals::Traversal::
visit(SgNode* node)
   { 
     SgFunctionCallExp* sig = isSgFunctionCallExp(node);
     if (!sig) return;

     /* Find all calls to the signal() function */
     SgFunctionRefExp* func = isSgFunctionRefExp(sig->get_function());
     
     if( !func ) return;

     if (func->get_symbol()->get_name().getString().compare("signal") == 0) {
       SgExpressionPtrList sig_args = sig->get_args()->get_expressions();

       SgExpressionPtrList::iterator args = ++(sig_args.begin());
       SgFunctionRefExp* sig_handler = isSgFunctionRefExp(*args);

         
       /* Store all assigned handlers, if signal is called again and references
        * one of these then we add output. */
       static SgExpressionPtrList handlers;

       for (SgExpressionPtrList::iterator i = handlers.begin(); i != handlers.end(); i++) {
         if (sig_handler->get_symbol()->get_declaration() == (isSgFunctionRefExp(*i))->get_symbol()->get_declaration()) {
           output->addOutput(new CheckerOutput(node));
           return;
         }
       }

       handlers.push_back(sig_handler);
     }
   } //End of the visit function.
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::AvoidUsingTheSameHandlerForMultipleSignals::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::AvoidUsingTheSameHandlerForMultipleSignals::Traversal(params, output);
}

extern const Compass::Checker* const avoidUsingTheSameHandlerForMultipleSignalsChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::AvoidUsingTheSameHandlerForMultipleSignals::checkerName,
        CompassAnalyses::AvoidUsingTheSameHandlerForMultipleSignals::shortDescription,
        CompassAnalyses::AvoidUsingTheSameHandlerForMultipleSignals::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
