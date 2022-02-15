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
struct TypeData : std::tuple<Type_Definition_Struct*, SgNode*, bool, bool, bool>
{
  using base = std::tuple<Type_Definition_Struct*, SgNode*, bool, bool, bool>;
  using base::base;

  Type_Definition_Struct& definitionStruct()  const { return SG_DEREF(std::get<0>(*this)); }
  SgNode&                 sageNode()          const { return SG_DEREF(std::get<1>(*this)); }
  bool                    isAbstract()        const { return std::get<2>(*this); }
  bool                    isLimited()         const { return std::get<3>(*this); }
  bool                    isTagged()          const { return std::get<4>(*this); }

  void                    sageNode(SgNode& n)       { std::get<1>(*this) = &n; }
  void                    setAbstract(bool b)       { std::get<2>(*this) = b; }
  void                    setLimited(bool b)        { std::get<3>(*this) = b; }
  void                    setTagged(bool b)         { std::get<4>(*this) = b; }
};

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
SgType&
getDefinitionTypeID(Element_ID defid, AstContext ctx);

/// returns the ROSE type for an Asis definition \ref defid
/// iff defid is NULL, an SgTypeVoid is returned.
SgType&
getDefinitionTypeID_opt(Element_ID defid, AstContext ctx);

/// creates a constraint node for \ref el
SgAdaTypeConstraint&
getConstraintID(Element_ID el, AstContext ctx);

/// looks up the record declaration associated with \ref defid
SgClassDeclaration&
getParentRecordDeclID(Element_ID defid, AstContext ctx);

TypeData
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
