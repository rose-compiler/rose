// Explicit Copy
// Author: Valentin  David
// Date: 02-August-2007

#include "rose.h"
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
      : public Compass::AstSimpleProcessingWithRunFunction {
      private:
      Compass::OutputObject* output;
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

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Explicit Copy Analysis
// Author: Valentin  David
// Date: 02-August-2007

#include "compass.h"
// #include "explicitCopy.h"

namespace CompassAnalyses {
  namespace ExplicitCopy {
    const std::string checkerName      = "ExplicitCopy";

    const std::string shortDescription = "Check for copy constructor and "
                                         "operator.";
    const std::string longDescription  = "This checker enforce the "
    "declaration of a copy constructor and a copy operator";
  } //End of namespace ExplicitCopy.
} //End of namespace CompassAnalyses.

CompassAnalyses::ExplicitCopy::
CheckerOutput::CheckerOutput (SgNode* node, bool hascons,
                              bool hasop)
  : OutputViolationBase(node, checkerName,
                        hascons?"Class definition misses a copy operator.":
                        (hasop?"Class definition misses a copy constructor.":
                         "Class definition misses copy constructor and "
                         "operator."))
{}

CompassAnalyses::ExplicitCopy::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
  : output(output)
{
}

void
CompassAnalyses::ExplicitCopy::Traversal::
visit(SgNode* n)
{
  SgClassDefinition *def = isSgClassDefinition(n);
  if (def == NULL)
    return ;

  SgClassType *myself = def->get_declaration()->get_type();
  SgDeclarationStatementPtrList members = def->get_members();
  bool	hascons	= false;
  bool	hasop	= false;
  use_default(def->get_declaration(), hascons, hasop);
  use_default(def, hascons, hasop);
  for (SgDeclarationStatementPtrList::iterator i = members.begin();
       i != members.end();
       ++i) {
    use_default(*i, hascons, hasop);
    bool isconstr;
    if (is_copy(*i, myself, isconstr)) {
      if (isconstr) {
        if (hasop)
          return ;
        hascons = true;
        continue ;
      }
      else {
        if (hascons)
          return ;
        hasop = true;
        continue ;
      }
    }
  }

  if ((hasop) && (hascons))
    return ;

  output->addOutput(new CheckerOutput(n, hascons, hasop));
} //End of the visit function.


void
CompassAnalyses::ExplicitCopy::Traversal::
use_default(SgLocatedNode* ln, bool& constr, bool& op)
{
  AttachedPreprocessingInfoType* comments =
    ln->getAttachedPreprocessingInfo();
  if (comments != NULL) {
    AttachedPreprocessingInfoType::iterator k;
    for (k = comments->begin(); k != comments->end(); k++) {
      if ((*k)->getString().find("use default copy operator")
          != std::string::npos) {
        op = true;
      }
      if ((*k)->getString().find("use default copy constructor")
          != std::string::npos) {
        constr = true;
      }
    }
  }
}

bool
CompassAnalyses::ExplicitCopy::Traversal::
is_copy(SgDeclarationStatement* stmt,
        SgClassType *myself,
        bool& isconstr)
{
  SgMemberFunctionDeclaration *decl =
    isSgMemberFunctionDeclaration(stmt);
  if (decl == NULL)
    return false;
  if (decl->isTemplateFunction())
    return false;
  isconstr = false;
  if (decl->get_name() == myself->get_name()) {
    isconstr = true;
  }
  else {
    if (std::string(decl->get_name()) != std::string("operator="))
      //The member is not a constructor nor a assginment operator.
      return false;
  }
  //12.8.2: First argument is one of those:
  //T&, const T&, const volatile T&, volatile T&
  //others have a default value;
  //12.8.9: For an copy operator, there cannot be other parameters.
  //(Anyway, imagine an assignment operator with other parameters...)
  SgInitializedNamePtrList args = decl->get_args();
  SgInitializedNamePtrList::iterator j;

  j = args.begin();
  if (j == args.end()) {
    //Default constructor.
    return false;
  }
  SgType	 *type  = (*j)->get_type();
  SgReferenceType *rtype =  isSgReferenceType(type);
  if (rtype == NULL)
    return false;
  type = rtype->get_base_type();
  SgModifierType *mtype;
  do {
    mtype =  isSgModifierType(type);
    if (mtype != NULL)
      type = mtype->get_base_type();
  } while (mtype != NULL);
  SgClassType *ctype =  isSgClassType(type);
  if (ctype == NULL)
    return false;

  //FIXME: ctype and myself sometimes does not share the same
  //       address.
  if (ctype->get_qualified_name() != myself->get_qualified_name()) {
    return false;
  }

  ++j;
  for (; j != args.end(); ++j) {
    if (!isconstr)
      return false;
    if ((*j)->get_initializer() == NULL) {
      //There is a non default'd parameter.
      //It is not a copy constructor.
      return false;
    }
  }
  return true;
}


static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::ExplicitCopy::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::ExplicitCopy::Traversal(params, output);
}

extern const Compass::Checker* const explicitCopyChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::ExplicitCopy::checkerName,
        CompassAnalyses::ExplicitCopy::shortDescription,
        CompassAnalyses::ExplicitCopy::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
