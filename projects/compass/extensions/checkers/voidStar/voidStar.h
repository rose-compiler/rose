// Void Star
// Author: Valentin  David
// Date: 03-August-2007

#include "compass.h"

#ifndef COMPASS_VOID_STAR_H
#define COMPASS_VOID_STAR_H

namespace CompassAnalyses {
  namespace VoidStar {
    /*! \brief Void Star: Tests if classes defines public methods
      accepting or returning void.
    */

    extern const std::string checkerName;
    extern const std::string shortDescription;
    extern const std::string longDescription;

    // Specification of Checker Output Implementation
    class CheckerOutput: public Compass::OutputViolationBase {
    public:
      CheckerOutput(SgMemberFunctionDeclaration* node);
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

// COMPASS_VOID_STAR_H
#endif
