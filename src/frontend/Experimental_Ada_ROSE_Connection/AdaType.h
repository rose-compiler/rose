#ifndef _ADA_TYPE
#define _ADA_TYPE 1

/// Contains functions and classes for converting Ada expressions
///   from Asis to ROSE.


#include "Ada_to_ROSE.h"
#include "a_nodes.h"

namespace Ada_ROSE_Translation
{

/// initializes a standard package with built-in ada types
/// \todo this should disappear as soon as the Standard package is included in Asis
void initializePkgStandard(SgGlobal& global);

/// represents a (partially) converted type
template <class AsisTypeDefinitionStruct, class SageNode>
struct TypeDataT : std::tuple<AsisTypeDefinitionStruct*, SageNode*, bool, bool, bool, bool>
{
  using base = std::tuple<AsisTypeDefinitionStruct*, SageNode*, bool, bool, bool, bool>;
  // using base::base;

  TypeDataT( AsisTypeDefinitionStruct* def = nullptr,
             SageNode* sgn = nullptr,
             bool abs = false,
             bool ltd = false,
             bool tag = false,
             bool inh = false
           )
  : base(def, sgn, abs, ltd, tag, inh)
  {}

  AsisTypeDefinitionStruct& definitionStruct()  const { return SG_DEREF(std::get<0>(*this)); }
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

using TypeData       = TypeDataT<Type_Definition_Struct, SgNode>;
using FormalTypeData = TypeDataT<Formal_Type_Definition_Struct, SgDeclarationStatement>;

/// traverses over a list of types and creates a joint type (single or unioned)
/// for the exception handler.
struct ExHandlerTypeCreator
{
    explicit
    ExHandlerTypeCreator(AstContext astctx)
    : ctx(astctx)
    {}

    void operator()(Element_Struct& elem);

    /// conversion to return (and generate) the type
    operator SgType&() &&;

  private:
    AstContext           ctx;
    std::vector<SgType*> lst;

    ExHandlerTypeCreator() = delete;
};


/// returns the ROSE type for the Asis type represented by \ref id
SgType&
getDeclTypeID(Element_ID id, AstContext ctx);


/// returns the ROSE type for an Asis definition \ref defid
/// \param defid        the Asis element ID
/// \param ctx          the translation context
/// \param forceSubtype true, iff a subtype without constraint should be represented as such
SgType&
getDefinitionTypeID(Element_ID defid, AstContext ctx, bool forceSubtype = false);

/// returns the ROSE type for an Asis definition \ref defid
/// iff defid is NULL, an SgTypeVoid is returned.
SgType&
getDefinitionTypeID_opt(Element_ID defid, AstContext ctx);

/// creates a constraint node for \ref el
SgAdaTypeConstraint&
getConstraintID(Element_ID el, AstContext ctx);

/// creates a discrete subtype from super type indicated by \ref typeId
///   and constraint \ref constraintID
SgType&
getDiscreteSubtypeID(Element_ID typeId, Element_ID constraintID, AstContext ctx);


/// looks up the record declaration associated with \ref defid
SgBaseClass&
getParentTypeID(Element_ID defid, AstContext ctx);

FormalTypeData
getFormalTypeFoundation(const std::string& name, Declaration_Struct& decl, AstContext ctx);

/// returns a ROSE representation of the type represented by \ref decl
/// \post res.n is not NULL
TypeData
getTypeFoundation(const std::string& name, Declaration_Struct& decl, AstContext ctx);

/// returns the base of a referenced exception
/// \result either an SgInitializedName or an SgAdaRenamingDecl
std::pair<SgInitializedName*, SgAdaRenamingDecl*>
getExceptionBase(Element_Struct& el, AstContext ctx);

}

#endif /* _ADA_TYPE */
