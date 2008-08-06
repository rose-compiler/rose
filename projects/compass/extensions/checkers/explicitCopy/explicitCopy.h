// Explicit Copy
// Author: Valentin  David
// Date: 02-August-2007

#include "compass.h"

#ifndef COMPASS_EXPLICIT_COPY_H
#define COMPASS_EXPLICIT_COPY_H

namespace CompassAnalyses {
  namespace ExplicitCopy {
    /*! \brief Explicit copy: this checker enforce the declaration of
     *     a copy constructor and a copy operator.
     */

    extern const std::string checkerName;
    extern const std::string shortDescription;
    extern const std::string longDescription;

    // Specification of Checker Output Implementation
    class CheckerOutput: public Compass::OutputViolationBase {
    public:
      CheckerOutput(SgNode* node, bool hascons, bool hasop);
    };

    // Specification of Checker Traversal Implementation

    class Traversal
      : public AstSimpleProcessing, public Compass::TraversalBase {
      private:
      void use_default(SgLocatedNode* ln, bool& constr, bool& op);
      bool is_copy(SgDeclarationStatement* stmt,
		   SgClassType *myself,
		   bool& isconstr);

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

// COMPASS_EXPLICIT_COPY_H
#endif

