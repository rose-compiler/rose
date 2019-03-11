// Other Argument
// Author: Valentin  David
// Date: 03-August-2007

#include "rose.h"
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
      : public Compass::AstSimpleProcessingWithRunFunction {
      // Checker specific parameters should be allocated here.
               Compass::OutputObject* output;

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

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Other Argument Analysis
// Author: Valentin  David
// Date: 03-August-2007

#include "compass.h"
// #include "otherArgument.h"

namespace CompassAnalyses {
  namespace OtherArgument {
    const std::string checkerName      = "OtherArgument";

    const std::string shortDescription = "Enforce copy argument called "
    "\"other\"";
    const std::string longDescription  = "This checker looks at all main "
    "arguments of copy constructors and copy operators. The main argument "
    "should be called \"other\".";
  } //End of namespace OtherArgument.
} //End of namespace CompassAnalyses.

CompassAnalyses::OtherArgument::
CheckerOutput::CheckerOutput(SgNode* node)
  : OutputViolationBase(node,checkerName,
                        "A copy operations should have its main argument "
                        "named \"other\".")
{}

CompassAnalyses::OtherArgument::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
  : output(output)
{}

static std::string lower_camel_case(const std::string& from) {
  std::string out;
  std::string::size_type cur = 0;
  std::string::size_type s;
  do {
    s = from.find('_', cur);
    if (s != cur) {
      std::string word = from.substr(cur, s);
      if (cur != 0)
        *(word.begin()) = toupper(*(word.begin()));
      else
        *(word.begin()) = tolower(*(word.begin()));
      out += word;
    }
    if (s != std::string::npos)
      cur = s + 1;
  } while (s != std::string::npos);
  return out;
}

static bool is_copy(SgMemberFunctionDeclaration *decl)
{
  if (decl == NULL)
    return false;
  if (decl->isTemplateFunction())
    return false;

  bool isconstr = false;

  SgClassDefinition * class_defn = isSgClassDefinition(decl->get_class_scope());
  ROSE_ASSERT(class_defn != NULL);
  if (decl->get_name() == class_defn->get_declaration()->get_name()) {
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
  if (ctype->get_qualified_name() != class_defn
      ->get_declaration()->get_qualified_name()) {
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


void
CompassAnalyses::OtherArgument::Traversal::
visit(SgNode* n) {
  SgMemberFunctionDeclaration *decl =
    isSgMemberFunctionDeclaration(n);
  if (decl != NULL) {
    if (is_copy(decl)) {
      SgClassDefinition * class_defn = isSgClassDefinition(decl->get_class_scope());
      ROSE_ASSERT(class_defn != NULL);
      SgInitializedNamePtrList args = decl->get_args();
      SgInitializedName *name = *(args.begin());
      if ((name->get_name().str() != std::string(""))
          && (name->get_name().str() != std::string("other"))
          && (name->get_name().str() != std::string("that"))
          && (name->get_name().str() !=
              lower_camel_case(class_defn
                               ->get_declaration()
                               ->get_name().str()))) {
        output->addOutput(new CheckerOutput(decl));
      }
    }
  }
} //End of the visit function.

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::OtherArgument::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::OtherArgument::Traversal(params, output);
}

extern const Compass::Checker* const otherArgumentChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::OtherArgument::checkerName,
        CompassAnalyses::OtherArgument::shortDescription,
        CompassAnalyses::OtherArgument::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
