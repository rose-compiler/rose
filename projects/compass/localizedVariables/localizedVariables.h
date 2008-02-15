// Localized Variables
// Author: Valentin  David
// Date: 02-August-2007

#include "compass.h"

#ifndef COMPASS_LOCALIZED_VARIABLES_H
#define COMPASS_LOCALIZED_VARIABLES_H

namespace CompassAnalyses {
  namespace LocalizedVariables {
    /*! \brief Localized Variables: Add your description here
     */

    extern const std::string checkerName;
    extern const std::string shortDescription;
    extern const std::string longDescription;

    // Specification of Checker Output Implementation
    class CheckerOutput: public Compass::OutputViolationBase {
    public:
      CheckerOutput(SgNode* node, std::string msg);
    };

    // Specification of Checker Traversal Implementation

    class Traversal:
      public AstTopDownBottomUpProcessing<void*,void*>,
      public Compass::TraversalBase {
      private:
      struct Var {
	bool is_used_in_scope;
	bool is_used_close;
	bool is_far;
	bool was_used;
	bool is_const;
	Var(bool is_const):
	  is_used_in_scope(false),
	  is_used_close(false),
	  is_far(false),
	  was_used(false),
	  is_const(is_const) {}
      };

      typedef std::map<SgInitializedName*, Var*> map;
      class Stack: public std::list<map*> {
	// using list for front insertion
      public:
	Stack(): std::list<map*>() {}

	Var* operator[](SgInitializedName* n) {
	  for (iterator i = begin();
	       i != end();
	       ++i) {
	    Var* v = (*(*i))[n];
	    if (v != NULL)
	      return v;
	  }
	  return NULL;
	}

	void pop() {
	  pop_front();
	}

	map *top() {
	  if (empty())
	    return NULL;
	  return *(begin());
	}

	void push(map* m) {
	  push_front(m);
	}
      } stack;

      public:
      Traversal(Compass::Parameters inputParameters,
		Compass::OutputObject* output);

      void run(SgNode* n) {
        this->traverse(n, NULL);
      }

      virtual void* evaluateInheritedAttribute(SgNode* n, void*);
      virtual void* evaluateSynthesizedAttribute(SgNode* n, void*,
						 SynthesizedAttributesList);
    };
  }
}

// COMPASS_LOCALIZED_VARIABLES_H
#endif

