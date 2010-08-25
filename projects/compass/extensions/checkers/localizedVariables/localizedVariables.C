// Localized Variables
// Author: Valentin  David
// Date: 02-August-2007

#include "rose.h"
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
      public AstTopDownBottomUpProcessing<void*,void*> {
      private:
      Compass::OutputObject* output;
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

      void visit(SgNode* n) {
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

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Localized Variables Analysis
// Author: Valentin  David
// Date: 02-August-2007

#include "compass.h"
// #include "localizedVariables.h"

namespace CompassAnalyses {
  namespace LocalizedVariables {
    const std::string checkerName      = "LocalizedVariables";
    const std::string shortDescription = "Look for variables with too "
                                         "wide scope";
    const std::string longDescription  = "Tests if variable declared could "
                                         "have been declared in an inner "
                                         "scope or later on in the same "
                                         "scope.";

    class OutputWide: public CheckerOutput {
    public:
      OutputWide(SgInitializedName* iname):
	CheckerOutput(iname,
                      "Variable " + std::string(iname->get_name()) +
                      " does not seem to be used widely in that scope."
                      " Try to move it to an inner scope.") {}
    };

    class OutputNever: public CheckerOutput {
    public:
      OutputNever(SgInitializedName* iname):
	CheckerOutput(iname,
                      "Variable " + std::string(iname->get_name()) +
                      " does not seem to be used.") {}
    };

    class OutputFar: public CheckerOutput {
    public:
      OutputFar(SgInitializedName* iname)
        : CheckerOutput(iname,
                        "Variable " + iname->get_name() +
                        " does not seem to be used right after"
                        " its declaration."
                        " Try to move it down in the code.") {}
    };

  } //End of namespace LocalizedVariables.
} //End of namespace CompassAnalyses.

CompassAnalyses::LocalizedVariables::
CheckerOutput::CheckerOutput(SgNode* node, std::string msg)
   : OutputViolationBase(node,checkerName,msg)
   {}

CompassAnalyses::LocalizedVariables::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
  : output(output)
{
}


void*
CompassAnalyses::LocalizedVariables::Traversal::
evaluateInheritedAttribute(SgNode* n, void*)
{
  //Entering a new scope
  if (isSgScopeStatement(n) != NULL) {
    stack.push(new map());
  }

  //Using a variable
  SgVarRefExp *e = isSgVarRefExp(n);
  if (e != NULL) {
    Var* first = (*stack.top())[e->get_symbol()->get_declaration()];
    if (first != NULL) {
      //In this case we are using the variable in its own scope
      first->is_used_in_scope = true;
      first->was_used = true;
    }
    else {
      //otherwise we just mark as used
      Var* v = stack[e->get_symbol()->get_declaration()];
      if (v != NULL)
        v->was_used = true;
    }
  }

  //Declaring new variables
  SgVariableDeclaration* decl = isSgVariableDeclaration(n);
  if (decl != NULL) {
    for (SgInitializedNamePtrList::iterator i
           = decl->get_variables().begin();
         i != decl->get_variables().end();
         ++i) {
      //Is it a constant?
      SgModifierType* mod = isSgModifierType((*i)->get_type());
      bool cons = false;
      if (mod != NULL)
        if (mod->get_typeModifier().get_constVolatileModifier()
            .isConst())
          cons = true;
      (*stack.top())[*i] = new Var(cons);
    }
  }

  return NULL;
}

void*
CompassAnalyses::LocalizedVariables::Traversal::
evaluateSynthesizedAttribute(SgNode* n, void*,
                             SynthesizedAttributesList)
{
  //Quitting a scope, we get the report of usage of each variable
  //declared in the scope we are quitting and then clean the
  //environment.
  if (isSgScopeStatement(n) != NULL) {
    map* top = stack.top();
    for (map::iterator i = top->begin();
         i != top->end(); ++i) {
      if ((*i).second != NULL) {
        if (!((*i).second->was_used)) {
          //We have never used the variable.
          output
            ->addOutput(new OutputNever((*i).first));
        } else {
          if (!((*i).second->is_const)) {
            //If it is a constant, we accept that the variable is
            //user far from its declaration.
            if (!((*i).second->is_used_in_scope)) {
              output
                ->addOutput(new OutputWide((*i).first));
            } else if (!((*i).second->is_used_close)) {
              output
                ->addOutput(new OutputFar((*i).first));
            }
          }
        }
        delete (*i).second;
      }
    }
    delete top;
    stack.pop();
  }

  //Quitting a new statement. If it is not a declaration, we need
  //to mark all variables that were not used as far. It is like
  //having done one step.
  if (isSgStatement(n) != NULL) {
    //We only care of the variable of the current scope.
    //In the other case, the full scope itself is just one step only.
    map* scope = stack.top();
    if (scope != NULL) {
      for (map::iterator j = scope->begin();
           j != scope->end();
           ++j) {
        Var* v = (*j).second;
        if (v != NULL) {
          if ((v->was_used) && (!v->is_far))
            v->is_used_close = true;
          //We want to consider declarations by blocks.
          //A declaration should not make previous declaration
          //to be far.
          if (isSgDeclarationStatement(n) == NULL)
            v->is_far = true;
        }
      }
    }
  }

  return NULL;
}

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::LocalizedVariables::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

extern const Compass::Checker* const localizedVariablesChecker =
  new Compass::Checker(
        CompassAnalyses::LocalizedVariables::checkerName,
        CompassAnalyses::LocalizedVariables::shortDescription,
        CompassAnalyses::LocalizedVariables::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run);
