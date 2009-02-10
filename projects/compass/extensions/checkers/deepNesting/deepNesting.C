// Deep Nesting
// Author: Gergo Barany
// Date: 24-July-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_DEEP_NESTING_H
#define COMPASS_DEEP_NESTING_H

namespace CompassAnalyses
   { 
     namespace DeepNesting
        { 
        /*! \brief Deep Nesting: Add your description here 
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
                 int maximumNestedScopes;

               public:
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

                 // The implementation of the run function has to match the traversal being called.
                    void run(SgNode* n){ this->traverse(n, preorder); };

                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_DEEP_NESTING_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Deep Nesting Analysis
// Author: Gergo Barany
// Date: 24-July-2007

#include "compass.h"
// #include "deepNesting.h"

namespace CompassAnalyses
   { 
     namespace DeepNesting
        { 
          const std::string checkerName      = "DeepNesting";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Scope is nested deeper than allowed.";
          const std::string longDescription  = "The nesting of scope"
            " statements (blocks, loops, if, switch) is deeper than the "
            " maximum specified in the parameters.";
        } //End of namespace DeepNesting.
   } //End of namespace CompassAnalyses.

CompassAnalyses::DeepNesting::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::DeepNesting::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
     maximumNestedScopes = Compass::parseInteger(inputParameters["DeepNestingChecker.maximumNestedScopes"]);
   }

void
CompassAnalyses::DeepNesting::Traversal::
visit(SgNode* n)
   { 
      if (isSgScopeStatement(n) && !isSgBasicBlock(n)
       || isSgBasicBlock(n) && isSgBasicBlock(n->get_parent()))
      {
        // Follow parent pointers, counting enclosing scopes.
          int scopes = 0;
          SgNode *p = n;
          while ((p = p->get_parent()) != NULL && !isSgFunctionDefinition(p))
          {
            // Count scope statements that are not basic blocks; count basic
            // blocks that have basic blocks as their parents. This is meant
            // to make sure that while (...) { ... } is not counted as *two*
            // scopes.
              if (isSgScopeStatement(p) && !isSgBasicBlock(p))
                scopes++;
              else if (isSgBasicBlock(p) && isSgBasicBlock(p->get_parent()))
                scopes++;
          }
          if (scopes == maximumNestedScopes)
              output->addOutput(new CheckerOutput(n));
      }
   } //End of the visit function.

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::DeepNesting::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::DeepNesting::Traversal(params, output);
}

extern const Compass::Checker* const deepNestingChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::DeepNesting::checkerName,
        CompassAnalyses::DeepNesting::shortDescription,
        CompassAnalyses::DeepNesting::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
