

#ifndef _SAGEINTERFACE_ADA_H
#define _SAGEINTERFACE_ADA_H 1

#include "sage3basic.hhh"

namespace SageInterface
{
namespace Ada
{
  /// tests if this declaration defines a type that is completed
  ///   in a private section.
  bool declaresPrivateType(SgDeclarationStatement* dcl);
} // Ada
} // SageInterface

#endif /* _SAGEINTERFACE_ADA_H */
