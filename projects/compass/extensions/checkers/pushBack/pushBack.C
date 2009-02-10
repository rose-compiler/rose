// Push Back
// Author: Valentin  David
// Date: 02-August-2007

#include "rose.h"
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
      : public Compass::AstSimpleProcessingWithRunFunction {
      private:
      Compass::OutputObject* output;
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
// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Push Back Analysis
// Author: Valentin  David
// Date: 02-August-2007

#include "compass.h"
// #include "pushBack.h"

namespace CompassAnalyses {
  namespace PushBack {
    const std::string checkerName      = "PushBack";

    const std::string shortDescription = "Detects possibly ineffecient "
                                         "insertion in STL containers";
    const std::string longDescription  = "This checker looks typically for "
                                         "insertions using for example "
                                         "l.insert(o, l.end()) where a "
                                         "call to push_back would have been "
                                         "better.";
  } //End of namespace PushBack.
} //End of namespace CompassAnalyses.

CompassAnalyses::PushBack::
CheckerOutput::CheckerOutput(SgNode* node, const std::string& what,
                             bool begin)
  : OutputViolationBase(node,checkerName, "call to " + what + " where " +
                        (begin?"push_front":"push_back")
                        + " is supposed to be faster.") {}

CompassAnalyses::PushBack::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
  : output(output)
{
}

void
CompassAnalyses::PushBack::Traversal::
visit(SgNode* n)
{
  SgFunctionCallExp* call = isSgFunctionCallExp(n);
  if (call != NULL) {
    std::string method = get_method(call);
    SgVariableSymbol* obj = get_obj(call);
    if (obj == NULL)
      return ;
    if ((method == "::std::vector::resize") ||
        (method == "::std::deque::resize") ||
        (method == "::std::list::resize")) {
      //resize(vec.size()+1, ...)
      SgExpressionPtrList args = call->get_args()->get_expressions();
      if (args.size() != 2)
        return ;
      if (is_inc_size(*(args.begin()), obj))
        output->addOutput(new CheckerOutput(call, method));
      return ;
    }
    if ((method == "::std::vector::insert") ||
        (method == "::std::deque::insert") ||
        (method == "::std::list::insert")) {
      //insert(vec.end(), ..)
      SgExpressionPtrList args = call->get_args()->get_expressions();
      if (args.size() != 2)
        return ;
      if (is_end(*(args.begin()), obj))
        output->addOutput(new CheckerOutput(call, method, false));
    }
    if ((method == "::std::slist::insert") ||
        (method == "::std::deque::insert") ||
        (method == "::std::list::insert")) {
      //insert(vec.begin(), ..)
      SgExpressionPtrList args = call->get_args()->get_expressions();
      if (args.size() != 2)
        return ;
      if (is_begin(*(args.begin()), obj))
        output->addOutput(new CheckerOutput(call, method, true));
    }
  }
} //End of the visit function.


SgVariableSymbol*
CompassAnalyses::PushBack::Traversal::
get_obj(SgFunctionCallExp* call) {
  if (call != NULL) {
    SgExpression* function = call->get_function();
    if (function != NULL) {
      SgDotExp* dotf = isSgDotExp(function);
      SgArrowExp* arrf = isSgArrowExp(function);
      SgBinaryOp* be = dotf;
      if (dotf == NULL)
        be = arrf;
      if (be != NULL) {
        SgExpression* obj = be->get_lhs_operand();
        SgVarRefExp* ref = isSgVarRefExp(obj);
        if (ref != NULL) {
          return ref->get_symbol();
        }
      }
    }
  }
  return NULL;
}

std::string
CompassAnalyses::PushBack::Traversal::
get_method(SgFunctionCallExp* call) {
  if (call != NULL) {
    SgExpression* function = call->get_function();
    if (function != NULL) {
      SgDotExp* dotf = isSgDotExp(function);
      SgArrowExp* arrf = isSgArrowExp(function);
      SgBinaryOp* be = dotf;
      if (dotf == NULL)
        be = arrf;
      if (be != NULL) {
        SgExpression* method = be->get_rhs_operand();
        SgMemberFunctionRefExp* mref = isSgMemberFunctionRefExp(method);
        if (mref != NULL) {
          SgTemplateInstantiationMemberFunctionDecl* member =
            isSgTemplateInstantiationMemberFunctionDecl(
                                                        mref->get_symbol()->get_declaration());
          if (member != NULL) {
            SgClassDefinition* cldef =
              isSgClassDefinition(member->get_scope());
            if (cldef != NULL) {
              SgTemplateInstantiationDecl* instdecl =
                isSgTemplateInstantiationDecl(cldef->get_declaration());
              if (instdecl != NULL) {
                SgNamespaceDefinitionStatement* scope =
                  isSgNamespaceDefinitionStatement(instdecl
                                                   ->get_scope());
                if (scope != NULL) {
                  return std::string(scope->get_qualified_name())
                    + "::" + std::string(instdecl->get_templateName())
                    + "::" + std::string(member->get_name());
                }
              }
            }
          }
        }
      }
    }
  }
  return std::string("ERROR");
}

bool
CompassAnalyses::PushBack::Traversal::
is_begin(SgExpression* arg, SgVariableSymbol* obj) {
  return is_call("begin", arg, obj);
}

bool
CompassAnalyses::PushBack::Traversal::
is_end(SgExpression* arg, SgVariableSymbol* obj) {
  return is_call("end", arg, obj);
}

bool
CompassAnalyses::PushBack::Traversal::
is_call(std::string what,
        SgExpression* arg, SgVariableSymbol* obj) {
  SgFunctionCallExp* call = isSgFunctionCallExp(arg);
  if (call != NULL) {
    SgExpression* function = call->get_function();
    SgDotExp* dotf = isSgDotExp(function);
    SgArrowExp* arrf = isSgArrowExp(function);
    SgBinaryOp* be = dotf;

    if (dotf == NULL)
      be = arrf;
    if (be != NULL) {
      SgVarRefExp* ref = isSgVarRefExp(be->get_lhs_operand());
      if (ref == NULL)
        return false;
      if (ref->get_symbol() != obj)
        return false;
      SgMemberFunctionRefExp* method =
        isSgMemberFunctionRefExp(be->get_rhs_operand());
      if (method == NULL)
        return false;
      if (std::string(method->get_symbol()->get_name()) == what)
        return true;
      else
        return false;
    }
  }
  return false;
}

bool
CompassAnalyses::PushBack::Traversal::
is_inc_size(SgExpression* arg, SgVariableSymbol* obj) {
  bool hassize = false;
  SgAddOp *addop = isSgAddOp(arg);
  if (addop == NULL)
    return false;
  int value = -1;
  int i = 0;
  for (SgExpression* e = addop->get_lhs_operand();
       i < 2;
       e = addop->get_rhs_operand(), i++) {
    SgValueExp* v = isSgValueExp(e);
    if (v != NULL) {
      std::string s(v->unparseToString());
      s = s.substr(1, s.size()-2);
      value = atoi(s.c_str());
    } else {
      if (hassize)
        return false;
      SgFunctionCallExp* call = isSgFunctionCallExp(e);
      if (call != NULL) {
        SgExpression* function = call->get_function();
        SgDotExp* dotf = isSgDotExp(function);
        SgArrowExp* arrf = isSgArrowExp(function);
        SgBinaryOp* be = dotf;

        if (dotf == NULL)
          be = arrf;
        if (be != NULL) {
          SgVarRefExp* ref = isSgVarRefExp(be->get_lhs_operand());
          if (ref == NULL)
            return false;
          if (ref->get_symbol() != obj)
            return false;
          SgMemberFunctionRefExp* method =
            isSgMemberFunctionRefExp(be->get_rhs_operand());
          if (method == NULL)
            return false;
          if (std::string(method->get_symbol()->get_name()) == "size") {
            hassize = true;
          }
          else
            return false;
        }
      }
    }
  }
  return hassize && (value == 1);
}

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::PushBack::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::PushBack::Traversal(params, output);
}

extern const Compass::Checker* const pushBackChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::PushBack::checkerName,
        CompassAnalyses::PushBack::shortDescription,
        CompassAnalyses::PushBack::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
