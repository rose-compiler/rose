// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Void Star Analysis
// Author: Valentin  David
// Date: 03-August-2007

#include "compass.h"
#include "voidStar.h"

namespace CompassAnalyses {
  namespace VoidStar {
    const std::string checkerName      = "VoidStar";

    const std::string shortDescription = "defines public methods accepting or returning void.";
    const std::string longDescription  = "Tests if classes defines public methods accepting or returning void.";
  } //End of namespace VoidStar.
} //End of namespace CompassAnalyses.

CompassAnalyses::VoidStar::
CheckerOutput::CheckerOutput(SgMemberFunctionDeclaration* node)
  : OutputViolationBase(node, checkerName,
                        "Public method " + std::string(node->get_name()) +
                        " uses void* parameters or return type.")
{}

CompassAnalyses::VoidStar::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
  : Compass::TraversalBase(output, checkerName,
                           shortDescription, longDescription)
{
}


static
SgType *ignoreModifiers(SgType *type) {
  SgModifierType *mod = NULL;
  do {
    SgModifierType *mod = isSgModifierType(type);
    if (mod != NULL)
      type = mod->get_base_type();
  } while (mod != NULL);
  return type;
}

static
bool isVoidStar(SgType* type) {
  SgPointerType *ptr = isSgPointerType(ignoreModifiers(type));
  if (ptr == NULL)
    return false;
  return (isSgTypeVoid(ignoreModifiers(ptr->get_base_type())) != NULL);
}

void
CompassAnalyses::VoidStar::Traversal::
visit(SgNode* n)
{
  SgMemberFunctionDeclaration *decl = isSgMemberFunctionDeclaration(n);
  if (decl != NULL) {
    if (decl->get_declarationModifier().get_accessModifier().isPublic()) {
      if (isVoidStar(decl->get_type()->get_return_type())) {
        output->addOutput(new CheckerOutput(decl));
        return ;
      }
      const SgTypePtrList& args = decl->get_type()->get_arguments();
      for (SgTypePtrList::const_iterator i = args.begin();
           i != args.end();
           ++i) {
        if (isVoidStar(*i)) {
          output->addOutput(new CheckerOutput(decl));
          return ;
        }
      }
    }
  } //End of the visit function.
}
