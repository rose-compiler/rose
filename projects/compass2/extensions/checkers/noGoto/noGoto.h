// No Goto
// Author: Valentin  David
// Date: 02-August-2007

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
      : public AstSimpleProcessing, public Compass::TraversalBase {

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
