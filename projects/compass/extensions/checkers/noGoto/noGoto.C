// No Goto
// Author: Valentin  David
// Date: 02-August-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_NO_GOTO_H
# define COMPASS_NO_GOTO_H

namespace CompassAnalyses {
  namespace NoGoto {
    /*! \brief No Goto: detects uses of \c goto statements.
     */

    extern const std::string checkerName;
    extern const std::string shortDescription;
    extern const std::string longDescription;

    // Specification of Checker Output Implementation
    class CheckerOutput: public Compass::OutputViolationBase {
    public:
      CheckerOutput(SgNode* node);
    };

    // Specification of Checker Traversal Implementation

    class Traversal
      : public Compass::AstSimpleProcessingWithRunFunction {

      Compass::OutputObject* output;

      public:
      Traversal(Compass::Parameters inputParameters,
		Compass::OutputObject* output);
      void run(SgNode* n) {
	this->traverse(n, preorder);
      }
      void visit(SgNode* n);
    };
  }
}

// COMPASS_NO_GOTO_H
#endif
// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// No Goto Analysis
// Author: Valentin  David
// Date: 02-August-2007

#include "compass.h"
// #include "noGoto.h"

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
  : output(output)
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

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::NoGoto::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::NoGoto::Traversal(params, output);
}

extern const Compass::Checker* const noGotoChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::NoGoto::checkerName,
        CompassAnalyses::NoGoto::shortDescription,
        CompassAnalyses::NoGoto::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
