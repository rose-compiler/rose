#ifndef _LIBADALANG_TYPE
#define _LIBADALANG_TYPE 1

/// Contains functions and classes for converting Ada expressions
///   from Libadalang to ROSE.


#include "Libadalang_to_ROSE.h"

#include "libadalang.h"

namespace Libadalang_ROSE_Translation {

/// represents a (partially) converted type
template <class libadalang_entity, class SageNode>
struct TypeDataT : std::tuple<libadalang_entity*, SageNode*, bool, bool, bool, bool>
{
  using base = std::tuple<libadalang_entity*, SageNode*, bool, bool, bool, bool>;
  // using base::base;

  TypeDataT( libadalang_entity* def = nullptr,
             SageNode* sgn = nullptr,
             bool abs = false,
             bool ltd = false,
             bool tag = false,
             bool inh = false
           )
  : base(def, sgn, abs, ltd, tag, inh)
  {}

  libadalang_entity*        definitionStruct()  const { return std::get<0>(*this); }
  SageNode&                 sageNode()          const { return SG_DEREF(std::get<1>(*this)); }
  bool                      isAbstract()        const { return std::get<2>(*this); }
  bool                      isLimited()         const { return std::get<3>(*this); }
  bool                      isTagged()          const { return std::get<4>(*this); }
  bool                      inheritsRoutines()  const { return std::get<5>(*this); }

  void                      sageNode(SageNode& n)     { std::get<1>(*this) = &n; }
  void                      setAbstract(bool b)       { std::get<2>(*this) = b; }
  void                      setLimited(bool b)        { std::get<3>(*this) = b; }
  void                      setTagged(bool b)         { std::get<4>(*this) = b; }
  void                      inheritsRoutines(bool b)  { std::get<5>(*this) = b; }
};

using TypeData       = TypeDataT<ada_base_entity, SgNode>;
using FormalTypeData = TypeDataT<ada_base_entity, SgDeclarationStatement>;

//Function to hash a unique int from a node using the node's kind and location.
//The kind and location can be provided, but if not they will be determined in the function
int hash_node(ada_base_entity *node, int kind = -1, std::string full_sloc = "");

/// initializes a standard package with built-in ada types
/// \todo this should disappear as soon as the Standard package is included in Asis
void initializePkgStandard(SgGlobal& global, ada_base_entity* lal_root);

/// returns the ROSE type for the Libadalang access type represented by \ref lal_element
SgType&
getAccessType(ada_base_entity* lal_element, AstContext ctx);

/// returns the ROSE type for the Libadalang type represented by \ref lal_id
SgType&
getDeclType(ada_base_entity* lal_id, AstContext ctx);

/// returns the ROSE type for a libadalang node \ref lal_def
/// \param lal_def      the libadalang node
/// \param ctx          the translation context
/// \param forceSubtype true, iff a subtype without constraint should be represented as such
SgType&
getDefinitionType(ada_base_entity* lal_def, AstContext ctx, bool forceSubtype = false);

/// returns the ROSE type for an libadalang node \ref lal_element
/// iff lal_element is NULL, an SgTypeVoid is returned.
SgType&
getDefinitionType_opt(ada_base_entity* lal_element, AstContext ctx);

/// looks up the record declaration associated with \ref lal_element
SgBaseClass&
getParentType(ada_base_entity* lal_element, AstContext ctx);

FormalTypeData
getFormalTypeFoundation(const std::string& name, ada_base_entity* lal_element, AstContext ctx);

/// returns a ROSE representation of the type represented by \ref lal_def
/// \post res.n is not NULL
TypeData
getTypeFoundation(const std::string& name, ada_base_entity* lal_def, AstContext ctx);

SgType&
getDiscreteSubtype(ada_base_entity* lal_type, ada_base_entity* lal_constraint, AstContext ctx);

/// creates a constraint node for \ref el
SgAdaTypeConstraint&
getConstraint(ada_base_entity* lal_constraint, AstContext ctx);

SgType&
createExHandlerType(ada_base_entity* lal_exception_choices, AstContext ctx);

/// Given the expr portion of an ada_number_decl, returns the type of said decl
SgType*
getNumberDeclType(ada_base_entity* lal_element);

/// returns a ROSE node representing the type indicated by \p lal_expr
SgNode&
getExprType(ada_base_entity* lal_expr, AstContext ctx);


} //end Libadlang_ROSE_Translation namespace


#endif //_LIBABLANG_TYPE_H
