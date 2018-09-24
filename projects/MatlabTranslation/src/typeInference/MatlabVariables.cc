#include "MatlabVariables.h"

#include <iostream>

// #include "sageGeneric.h"
#include "sageBuilder.h"
#include "TypeAttribute.h"

namespace sb = SageBuilder;

namespace MatlabVariables
{
  /// \brief Traverses the AST and finds variables with known types.
  // \todo \pp could we also do the same for builtin functions right here?
  struct VarRefModifier : AstSimpleProcessing
  {
    virtual void visit(SgNode* n);
  };

  // \todo replace with a more declarative implementation
  //       that maps names to types...
  struct VarRefTypeSetter
  {
    void handle(SgNode&) {}

    void handle(SgVarRefExp& n)
    {
      if (n.unparseToString() == "nargin")
      {
        TypeAttribute* tattr = TypeAttribute::buildAttribute(sb::buildIntType());

        tattr->attach_to(&n);
      }
    }
  };

  void VarRefModifier::visit(SgNode* n)
  {
    if (isSgVarRefExp(n))
    {
      VarRefTypeSetter vrts;

      vrts.handle(*isSgVarRefExp(n));
    }
  }

  void setTypes(SgProject* project)
  {
    VarRefModifier vrm;

    vrm.traverse(project, preorder);
  }
}
