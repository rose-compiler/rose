

#ifndef _SAGEINTERFACE_ADA_H
#define _SAGEINTERFACE_ADA_H 1

#include "sage3basic.hhh"

namespace SageInterface
{
namespace ada
{
  /// tests if the declaration \ref dcl defines a type that is completed
  ///   in a private section.
  /// \return true, iff dcl is completed in a private section
  /// \pre dcl is not null and points to a first-nondefining declaration
  bool withPrivateDefinition(const SgDeclarationStatement* dcl);

  /// flattens the representation of Ada array types
  /// \param   arrayType the type of the array to be flattened
  /// \param   skipWhat defines intermediate nodes that are skipped
  ///          for finding the base type of the array
  /// \return  a pair of a base type pointer, and a vector of index ranges.
  /// \pre     arrayType is not null
  std::pair<SgType*, std::vector<SgExpression*> >
  flattenArrayType(const SgArrayType* arrayType);
} // Ada
} // SageInterface

#endif /* _SAGEINTERFACE_ADA_H */
