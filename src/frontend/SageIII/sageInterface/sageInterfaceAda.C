

#ifndef _SAGEINTERFACE_ADA_H
#define _SAGEINTERFACE_ADA_H 1

#include "sageInterfaceAda.h"
#include "sageGeneric.h"


namespace
{
  bool definitionIsPrivate(const SgDeclarationStatement& n)
  {
    return n.get_declarationModifier().get_accessModifier().isPrivate()
  }
}

namespace SageInterface
{
namespace Ada
{
  bool declaresPrivateType(const SgDeclarationStatement* dcl)
  {
    ROSE_ASSERT(dcl);

    // \todo check that dcl is a type
    return definitionIsPrivate(SG_DEREF(dcl->get_definingDeclaration()));
  }
} // Ada
} // SageInterface

#endif /* _SAGEINTERFACE_ADA_H */
