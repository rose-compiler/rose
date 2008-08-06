// Other Argument
// Author: Valentin  David
// Date: 03-August-2007

#include "compass.h"

#ifndef COMPASS_OTHER_ARGUMENT_H
#define COMPASS_OTHER_ARGUMENT_H

namespace CompassAnalyses {
  namespace OtherArgument {
    /*! \brief Other Argument: Check that arguments of copy are called
     *  "other".
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
      // Checker specific parameters should be allocated here.

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

// COMPASS_OTHER_ARGUMENT_H
#endif

