// Name All Parameters
// Author: Valentin  David
// Date: 03-August-2007

#include "compass.h"

#ifndef COMPASS_NAME_ALL_PARAMETERS_H
#define COMPASS_NAME_ALL_PARAMETERS_H

namespace CompassAnalyses {
  namespace NameAllParameters {
    /*! \brief Name All Parameters: Add your description here
     */

     extern const std::string checkerName;
     extern const std::string shortDescription;
     extern const std::string longDescription;

     class CheckerOutput: public Compass::OutputViolationBase
        {
          public:
               CheckerOutput(SgInitializedName* arg, SgFunctionDeclaration* fun);
        };

    // Specification of Checker Traversal Implementation
     class Traversal : public AstSimpleProcessing, public Compass::TraversalBase
        {
          public:
               Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

               void run(SgNode* n)
                  {
                    this->traverse(n, preorder);
                  }

               void visit(SgNode* n);
        };
  }
}

// COMPASS_NAME_ALL_PARAMETERS_H
#endif
