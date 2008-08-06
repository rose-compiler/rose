// Push Back
// Author: Valentin  David
// Date: 02-August-2007

#include "compass.h"

#ifndef COMPASS_PUSH_BACK_H
#define COMPASS_PUSH_BACK_H

namespace CompassAnalyses {
  namespace PushBack {
    /*! \brief Push Back: detect possibly inefficient insertions in
     *  STL containers.
     */

    extern const std::string checkerName;
    extern const std::string shortDescription;
    extern const std::string longDescription;

    // Specification of Checker Output Implementation
    class CheckerOutput: public Compass::OutputViolationBase  {
    public:
      CheckerOutput(SgNode* node,
		    const std::string& what,
		    bool begin = false);
    };

    // Specification of Checker Traversal Implementation

    class Traversal
      : public AstSimpleProcessing, public Compass::TraversalBase {
      private:
      static SgVariableSymbol* get_obj(SgFunctionCallExp* call);
      static std::string get_method(SgFunctionCallExp* call);
      static bool is_begin(SgExpression* arg, SgVariableSymbol* obj);
      static bool is_end(SgExpression* arg, SgVariableSymbol* obj);
      static bool is_call(std::string what,
			  SgExpression* arg, SgVariableSymbol* obj);
      static bool is_inc_size(SgExpression* arg, SgVariableSymbol* obj);
      public:

      Traversal(Compass::Parameters inputParameters,
		Compass::OutputObject* output);

      void run(SgNode* n) { this->traverse(n, preorder); };
      void visit(SgNode* n);
    };
  }
}

// COMPASS_PUSH_BACK_H
#endif
