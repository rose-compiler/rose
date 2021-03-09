

#ifndef _SAGEINTERFACE_ADA_H
#define _SAGEINTERFACE_ADA_H 1

#include "sage3basic.hhh"

namespace SageInterface
{
namespace ada
{
  typedef std::pair<SgArrayType*, std::vector<SgExpression*> > FlatArrayType;

  /// tests if the declaration \ref dcl defines a type that is completed
  ///   in a private section.
  /// \return true, iff dcl is completed in a private section
  /// \pre dcl is not null and points to a first-nondefining declaration
  bool withPrivateDefinition(const SgDeclarationStatement* dcl);

  /// flattens the representation of Ada array types
  /// \param   atype the type of the array to be flattened
  /// \return  iff \ref is not an arraytype, a pair <nullptr, empty vector> is returned
  ///          otherwise a pair of a array pointer, and a vector of index ranges.
  ///          (the expressions are part of the AST and MUST NOT BE DELETED.
  /// \pre     atype is not null
  FlatArrayType
  flattenArrayType(SgType* atype);
} // Ada
} // SageInterface

#endif /* _SAGEINTERFACE_ADA_H */
