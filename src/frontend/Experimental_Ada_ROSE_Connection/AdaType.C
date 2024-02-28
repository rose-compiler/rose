#include "sage3basic.h"

#include <limits>
#include <cmath>

#include <boost/algorithm/string.hpp>

#include "AdaType.h"

#include "sageGeneric.h"
#include "sageInterfaceAda.h"

#include "Ada_to_ROSE.h"
#include "AdaExpression.h"
#include "AdaStatement.h"
#include "AdaMaker.h"


// turn on all GCC warnings after include files have been processed
#pragma GCC diagnostic warning "-Wall"
#pragma GCC diagnostic warning "-Wextra"


// workaround: set internal variable before the frontend starts
namespace SageInterface
{
namespace Ada
{
extern SgAdaPackageSpecDecl* stdpkg;
}
}

namespace sb = SageBuilder;
namespace si = SageInterface;



namespace Ada_ROSE_Translation
{

namespace
{
  struct MakeTyperef : sg::DispatchHandler<SgType*>
  {
      using base = sg::DispatchHandler<SgType*>;

      MakeTyperef(Element_Struct& elem, AstContext astctx)
      : base(), el(elem), ctx(astctx)
      {}

      // checks whether this is a discriminated declaration and sets the type accordingly
      void handleDiscrDecl(SgDeclarationStatement& n, SgType* declaredType)
      {
        if (SgAdaDiscriminatedTypeDecl* discrDcl = si::Ada::getAdaDiscriminatedTypeDecl(n))
          declaredType = discrDcl->get_type();

        set(declaredType);
      }

      void set(SgType* ty)                       { ADA_ASSERT(ty); res = ty; }

      // error handler
      void handle(SgNode& n)                     { SG_UNEXPECTED_NODE(n); }

      // just use the type
      void handle(SgType& n)                     { set(&n); }
      void handle(SgAdaDiscriminatedTypeDecl& n) { set(n.get_type()); }

      // undecorated declarations

      // possibly decorated with an SgAdaDiscriminatedTypeDecl
      // \{
      void handle(SgAdaFormalTypeDecl& n)        { handleDiscrDecl(n, n.get_type()); }
      void handle(SgClassDeclaration& n)         { handleDiscrDecl(n, n.get_type()); }
      void handle(SgAdaTaskTypeDecl& n)          { handleDiscrDecl(n, n.get_type()); }
      void handle(SgAdaProtectedTypeDecl& n)     { handleDiscrDecl(n, n.get_type()); }
      void handle(SgEnumDeclaration& n)          { handleDiscrDecl(n, n.get_type()); }
      void handle(SgTypedefDeclaration& n)       { handleDiscrDecl(n, n.get_type()); }
      // \}

      // others
      void handle(SgInitializedName& n)          { set(&mkExprAsType(SG_DEREF(sb::buildVarRefExp(&n)))); }

      void handle(SgAdaAttributeExp& n)
      {
        attachSourceLocation(n, el, ctx); // \todo why is this not set where the node is made?
        set(&mkExprAsType(n));
      }


    private:
      Element_Struct& el;
      AstContext      ctx;
  };

  SgNode&
  getExprTypeID(Element_ID tyid, AstContext ctx);

  SgType* errorType(Expression_Struct& typeEx, AstContext ctx)
  {
    logError() << "unknown type name: " << typeEx.Name_Image
               << " / " << typeEx.Corresponding_Name_Definition
               << std::endl;

    ADA_ASSERT(!FAIL_ON_ERROR(ctx));
    return &mkUnresolvedType(typeEx.Name_Image);
  }

  // stop gap function
  // \todo remove after instantiations can be fully represented
  SgAdaGenericInstanceDecl*
  instantiationDeclID(Element_ID id, AstContext ctx)
  {
    Element_Struct*         elem = retrieveElemOpt(elemMap(), id);
    if (!elem || (elem->Element_Kind != An_Expression))
      return nullptr;

    Expression_Struct&      ex   = elem->The_Union.Expression;

    if (ex.Expression_Kind != An_Identifier)
      return nullptr;

    return isSgAdaGenericInstanceDecl(findFirst(asisDecls(), ex.Corresponding_Name_Definition));
  }

  template <class AsisDefinition>
  SgArrayType&
  createConstrainedArrayType(AsisDefinition& typenode, AstContext ctx)
  {
    ElemIdRange                indicesAsis = idRange(typenode.Discrete_Subtype_Definitions);
    std::vector<SgExpression*> indicesSeq  = traverseIDs(indicesAsis, elemMap(), ExprSeqCreator{ctx});
    SgExprListExp&             indicesAst  = mkExprListExp(indicesSeq);
    SgType&                    compType    = getDefinitionTypeID(typenode.Array_Component_Definition, ctx);

    return mkArrayType(compType, indicesAst, false /* constrained */);
  }

  template <class AsisDefinition>
  SgArrayType&
  createUnconstrainedArrayType(AsisDefinition& typenode, AstContext ctx)
  {
    ElemIdRange                indicesAsis = idRange(typenode.Index_Subtype_Definitions);
    std::vector<SgExpression*> indicesSeq  = traverseIDs(indicesAsis, elemMap(), ExprSeqCreator{ctx});
    SgExprListExp&             indicesAst  = mkExprListExp(indicesSeq);
    SgType&                    compType    = getDefinitionTypeID(typenode.Array_Component_Definition, ctx);

    return mkArrayType(compType, indicesAst, true /* unconstrained */);
  }

  struct FundamentalNodeExtractor : sg::DispatchHandler<SgNode*>
  {
    void handle(SgNode& n)            { SG_UNEXPECTED_NODE(n); }

    // expressions
    void handle(SgExpression& n)      { /*logInfo() << typeid(n).name() << std::endl;*/ }
    void handle(SgTypeExpression& n)  { res = extract(n.get_type()); }
    void handle(SgAdaAttributeExp& n) { res = &n; }

    // types
    void handle(SgType& n)            { res = &n; }
    void handle(SgNamedType& n)       { res = n.get_declaration(); }

    static
    SgNode* extract(SgNode* exp);
  };

  SgNode*
  FundamentalNodeExtractor::extract(SgNode* n)
  {
    return sg::dispatch(FundamentalNodeExtractor{}, n);
  }

  SgNode* extractFundamentalNode(SgExpression* exp)
  {
    SgNode* res = FundamentalNodeExtractor::extract(exp);

    //~ if (res) logWarn() << "i: " << typeid(*res).name() << std::endl;
    //~ else logWarn() << "i: nullptr" << std::endl;

    return res;
  }


  SgNode*
  queryArgumentTypeFromInstantiation(std::string tyname, AstContext ctx)
  {
    using ExprIterator = SgExpressionPtrList::const_iterator;
    using DeclIterator = SgDeclarationStatementPtrList::const_iterator;

    SgAdaGenericInstanceDecl* insdcl = ctx.instantiation();

    if (insdcl == nullptr)
      return nullptr;

    // first try to find an argument matching the name
    SgExprListExp&       args  = SG_DEREF(insdcl->get_actual_parameters());
    SgExpressionPtrList& exprs = args.get_expressions();
    //~ logError() << "instance " << exprs.size() << std::endl;
    ExprIterator         exbeg = exprs.begin();
    ExprIterator         exend = exprs.end();
    ExprIterator         expos = std::find_if( exbeg, exend,
                                               [&tyname](const SgExpression* exp) -> bool
                                               {
                                                 const SgActualArgumentExpression* act = isSgActualArgumentExpression(exp);

                                                 return act && boost::iequals(tyname, act->get_argument_name().getString());
                                               }
                                             );

    if (expos != exend)
      return extractFundamentalNode(isSgActualArgumentExpression(*expos)->get_expression());

    // match argument by position
    SgAdaGenericDecl& gendcl = si::Ada::getGenericDecl(*insdcl);
    SgAdaGenericDefn& gendfn = SG_DEREF(gendcl.get_definition());

    SgDeclarationStatementPtrList& genArglst = gendfn.get_declarations();
    DeclIterator      genbeg = genArglst.begin();
    DeclIterator      genend = genArglst.end();
    DeclIterator      genpos = std::find_if( genbeg, genend,
                                             [&tyname](const SgDeclarationStatement* prm) -> bool
                                             {
                                               if (const SgAdaDiscriminatedTypeDecl* discr = isSgAdaDiscriminatedTypeDecl(prm))
                                                 prm = discr->get_discriminatedDecl();

                                               const SgAdaFormalTypeDecl* formalParam = isSgAdaFormalTypeDecl(prm);

                                               return formalParam && boost::iequals(tyname, formalParam->get_name().getString());
                                             }
                                           );

    if (genpos != genend)
    {
      const int parmpos = std::distance(genbeg, genpos);

      ADA_ASSERT(parmpos < int(exprs.size()));
      return extractFundamentalNode(exprs[parmpos]);
    }

    return nullptr;
  }

  SgNode&
  getExprType(Expression_Struct& typeEx, AstContext ctx)
  {
    static constexpr bool findFirstOf = false;

    SgNode* res = nullptr;

    switch (typeEx.Expression_Kind)
    {
      case An_Identifier:
        {
          logKind("An_Identifier");

          // is it a type?
          // typeEx.Corresponding_Name_Declaration ?
          findFirstOf
          || (res = findFirst(asisTypes(), typeEx.Corresponding_Name_Definition))
          || (res = findFirst(asisExcps(), typeEx.Corresponding_Name_Definition))
          || (res = findFirst(asisVars(),  typeEx.Corresponding_Name_Definition)) /*+ , typeEx.Corresponding_Name_Declaration*/
          || (res = findFirst(adaTypes(),  AdaIdentifier{typeEx.Name_Image}))
          || (res = queryArgumentTypeFromInstantiation(typeEx.Name_Image, ctx))
          || (res = errorType(typeEx, ctx))
          ;

          break;
        }

      case A_Selected_Component:
        {
          logKind("A_Selected_Component");
          res = &getExprTypeID(typeEx.Selector, ctx);

/*
          /// temporary code to handle incomplete AST for generic instantiations
          /// \todo
          if (SgType* ty = isSgType(res))
          {
            if (SgAdaGenericInstanceDecl* gendecl = instantiationDeclID(typeEx.Prefix, ctx))
            {
              SgExpression& declref = mkAdaUnitRefExp(*gendecl);

              res = &mkQualifiedType(declref, *ty);
            }
          }
          /// end temporary code
*/
          break;
        }

      case An_Attribute_Reference:
        {
          logKind("An_Attribute_Reference");
          res = &getAttributeExpr(typeEx, ctx);
          break;
        }

      default:
        logWarn() << "Unknown type expression: " << typeEx.Expression_Kind << std::endl;
        ADA_ASSERT(!FAIL_ON_ERROR(ctx));
        res = &mkTypeUnknown();
    }

    return SG_DEREF(res);
  }

  SgNode&
  getExprTypeID(Element_ID tyid, AstContext ctx)
  {
    Element_Struct& elem = retrieveElem(elemMap(), tyid);
    ADA_ASSERT(elem.Element_Kind == An_Expression);

    return getExprType(elem.The_Union.Expression, ctx);
  }

  SgType&
  excludeNullIf(SgType& ty, bool exclNull, AstContext)
  {
    return exclNull ? mkNotNullType(ty) : ty;
  }

  SgType&
  getAnonymousAccessType(Definition_Struct& def, AstContext ctx)
  {
    ADA_ASSERT(def.Definition_Kind == An_Access_Definition);

    logKind("An_Access_Definition");

    SgType*                   underty = nullptr;
    Access_Definition_Struct& access  = def.The_Union.The_Access_Definition;
    Access_Definition_Kinds   access_type_kind = access.Access_Definition_Kind;

    switch (access_type_kind)
    {
      case An_Anonymous_Access_To_Constant:            // [...] access constant subtype_mark
      case An_Anonymous_Access_To_Variable:            // [...] access subtype_mark
      {
        const bool isConstant = access_type_kind == An_Anonymous_Access_To_Constant;
        logKind(isConstant ? "An_Anonymous_Access_To_Constant" : "An_Anonymous_Access_To_Variable");

        underty = &getDeclTypeID(access.Anonymous_Access_To_Object_Subtype_Mark, ctx);

        if (isConstant) underty = &mkConstType(*underty);
        break;
      }

      case An_Anonymous_Access_To_Procedure:           // access procedure
      case An_Anonymous_Access_To_Protected_Procedure: // access protected procedure
      case An_Anonymous_Access_To_Function:            // access function
      case An_Anonymous_Access_To_Protected_Function:  // access protected function
      {
        if (access_type_kind == An_Anonymous_Access_To_Procedure)
          logKind("An_Anonymous_Access_To_Procedure");
        else if (access_type_kind == An_Anonymous_Access_To_Protected_Procedure)
          logKind("An_Anonymous_Access_To_Protected_Procedure");
        else if (access_type_kind == An_Anonymous_Access_To_Function)
          logKind("An_Anonymous_Access_To_Function");
        else
          logKind("An_Anonymous_Access_To_Protected_Function");

        // these are functions, so we need to worry about return types
        const bool  isFuncAccess = (  (access_type_kind == An_Anonymous_Access_To_Function)
                                   || (access_type_kind == An_Anonymous_Access_To_Protected_Function)
                                   );
        const bool  isProtected  = (  (access_type_kind == An_Anonymous_Access_To_Protected_Function)
                                   || (access_type_kind == An_Anonymous_Access_To_Protected_Procedure)
                                   );

        ElemIdRange params  = idRange(access.Access_To_Subprogram_Parameter_Profile);
        SgType&     rettype = isFuncAccess ? getDeclTypeID(access.Access_To_Function_Result_Profile, ctx)
                                           : mkTypeVoid();
        underty = &mkAdaSubroutineType(rettype, ParameterCompletion{params, ctx}, ctx.scope(), isProtected);
        break;
      }

      case Not_An_Access_Definition: // An unexpected element
      default:
        logError() << "Unhandled anonymous access type kind: " << access_type_kind << std::endl;
        underty = &mkTypeUnknown();
        ADA_ASSERT(!FAIL_ON_ERROR(ctx));
    }

    SgType& res = mkAdaAccessType(SG_DEREF(underty), false /* general access */, true /* anonymous */);

    return excludeNullIf(res, access.Has_Null_Exclusion, ctx);
  }

  SgType&
  getAccessType(Access_Type_Struct& access_type, AstContext ctx)
  {
    auto access_type_kind = access_type.Access_Type_Kind;
    SgAdaAccessType* access_t = nullptr;

    switch (access_type_kind) {
      // variable access kinds
    case A_Pool_Specific_Access_To_Variable:
    case An_Access_To_Variable:
    case An_Access_To_Constant:
      {
        const bool isConstant = (access_type_kind == An_Access_To_Constant);
        const bool isVariable = (access_type_kind == An_Access_To_Variable);

        if (isConstant)
          logKind("An_Access_To_Constant");
        else if (isVariable)
          logKind("An_Access_To_Variable");
        else
          logKind("A_Pool_Specific_Access_To_Variable");

        SgType* ato = &getDefinitionTypeID(access_type.Access_To_Object_Definition, ctx);

        if (isConstant) ato = &mkConstType(*ato);
        access_t = &mkAdaAccessType(SG_DEREF(ato), isVariable /* general access */);

        break;
      }

      // subprogram access kinds
    case An_Access_To_Function:
    case An_Access_To_Protected_Function:
    case An_Access_To_Procedure:
    case An_Access_To_Protected_Procedure:
      {
        if (access_type_kind == An_Access_To_Procedure)
          logKind("An_Access_To_Procedure");
        else if (access_type_kind == An_Access_To_Protected_Procedure)
          logKind("An_Access_To_Protected_Procedure");
        else if (access_type_kind == An_Access_To_Function)
          logKind("An_Access_To_Function");
        else
          logKind("An_Access_To_Protected_Function");

        const bool isFuncAccess = (  (access_type_kind == An_Access_To_Function)
                                  || (access_type_kind == An_Access_To_Protected_Function)
                                  );
        const bool isProtected  = (  (access_type_kind == An_Access_To_Protected_Procedure)
                                  || (access_type_kind == An_Access_To_Protected_Function)
                                  );

        ElemIdRange          params  = idRange(access_type.Access_To_Subprogram_Parameter_Profile);
        SgType&              rettype = isFuncAccess ? getDeclTypeID(access_type.Access_To_Function_Result_Profile, ctx)
                                                    : mkTypeVoid();
        SgAdaSubroutineType& funty   = mkAdaSubroutineType(rettype, ParameterCompletion{params, ctx}, ctx.scope(), isProtected);

        access_t = &mkAdaAccessType(funty);
        break;
      }

    case Not_An_Access_Type_Definition:
    default:
      logError() << "Unhandled access type kind: " << access_type_kind << std::endl;
      access_t = &mkAdaAccessType(mkTypeUnknown());
      ADA_ASSERT(!FAIL_ON_ERROR(ctx));
    }

    return excludeNullIf(SG_DEREF(access_t), access_type.Has_Null_Exclusion, ctx);
  }


  SgType&
  getDeclType(Element_Struct& elem, AstContext ctx)
  {
    if (elem.Element_Kind == An_Expression)
    {
      SgNode& basenode = getExprType(elem.The_Union.Expression, ctx);
      SgType* res      = sg::dispatch(MakeTyperef(elem, ctx), &basenode);

      return SG_DEREF(res);
    }

    ADA_ASSERT(elem.Element_Kind == A_Definition);
    Definition_Struct& def = elem.The_Union.Definition;

    if (def.Definition_Kind == An_Access_Definition)
      return getAnonymousAccessType(def, ctx);

    logError() << "getDeclType: unhandled definition kind: " << def.Definition_Kind
               << std::endl;
    ADA_ASSERT(!FAIL_ON_ERROR(ctx));
    return mkTypeUnknown();
  }


  SgAdaTypeConstraint*
  getConstraintID_opt(Element_ID el, AstContext ctx)
  {
    return el ? &getConstraintID(el, ctx) : nullptr;
  }


  SgClassDefinition&
  getRecordBody(Record_Definition_Struct& rec, AstContext ctx)
  {
    SgClassDefinition&        sgnode     = mkRecordBody();
    ElemIdRange               components = idRange(rec.Record_Components);
    //~ ElemIdRange               implicits  = idRange(rec.Implicit_Components);

    sgnode.set_parent(&ctx.scope());

    // NOTE: the true parent is when the record is created; this is set to enable
    //       traversal from the record body to global scope.
    traverseIDs(components, elemMap(), ElemCreator{ctx.scope(sgnode)});
    //~ was: traverseIDs(components, elemMap(), ElemCreator{ctx.scope_npc(sgnode)});

    // how to represent implicit components
    //~ traverseIDs(implicits, elemMap(), ElemCreator{ctx.scope_npc(sgnode)});

    /* unused nodes:
         Record_Component_List Implicit_Components
    */

    markCompilerGenerated(sgnode);
    return sgnode;
  }

  SgClassDefinition&
  getRecordBodyID(Element_ID recid, AstContext ctx)
  {
    Element_Struct&           elem = retrieveElem(elemMap(), recid);
    ADA_ASSERT(elem.Element_Kind == A_Definition);

    Definition_Struct&        def = elem.The_Union.Definition;

    if (def.Definition_Kind == A_Null_Record_Definition)
    {
      logKind("A_Null_Record_Definition");
      SgClassDefinition&      sgdef = mkRecordBody();

      attachSourceLocation(sgdef, elem, ctx);
      return sgdef;
    }

    ADA_ASSERT(def.Definition_Kind == A_Record_Definition);

    logKind("A_Record_Definition");
    return getRecordBody(def.The_Union.The_Record_Definition, ctx);
  }

  struct EnumeratorCreator
  {
      EnumeratorCreator(SgEnumDeclaration& defn, AstContext astctx)
      : enumdef(defn), enumty(SG_DEREF(defn.get_type())), ctx(astctx)
      {
        ADA_ASSERT(enumdef.get_definingDeclaration() == &enumdef);
      }

      void operator()(Element_Struct& elem)
      {
        ADA_ASSERT(elem.Element_Kind == A_Declaration);
        logKind("A_Declaration");

        Declaration_Struct& decl = elem.The_Union.Declaration;
        ADA_ASSERT(decl.Declaration_Kind == An_Enumeration_Literal_Specification);
        logKind("An_Enumeration_Literal_Specification");

        NameData            name = singleName(decl, ctx);
        ADA_ASSERT(name.ident == name.fullName);

        // \todo name.ident could be a character literal, such as 'c'
        //       since SgEnumDeclaration only accepts SgInitializedName as enumerators
        //       SgInitializedName are created with the name 'c' instead of character constants.
        SgExpression&       repval = getEnumRepresentationValue(name.elem(), ctx);
        SgInitializedName&  sgnode = mkEnumeratorDecl(enumdef, name.ident, enumty, repval);

        attachSourceLocation(sgnode, elem, ctx);
        //~ sg::linkParentChild(enumdcl, sgnode, &SgEnumDeclaration::append_enumerator);
        enumdef.append_enumerator(&sgnode);
        ADA_ASSERT(sgnode.get_parent() == &enumdef);

        recordNode(asisVars(), name.id(), sgnode);
      }

    private:
      SgEnumDeclaration& enumdef;
      SgType&            enumty;
      AstContext         ctx;
  };

  SgAdaTypeConstraint*
  createDigitsConstraintIfNeeded(Expression_ID digitId, SgAdaTypeConstraint* sub, AstContext ctx)
  {
    if (digitId == 0) return sub;

    return &mkAdaDigitsConstraint(getExprID(digitId, ctx), sub);
  }

  SgAdaTypeConstraint*
  createDeltaConstraintIfNeeded(Expression_ID deltaId, bool isDecimal, SgAdaTypeConstraint* sub, AstContext ctx)
  {
    if (deltaId == 0) return sub;

    return &mkAdaDeltaConstraint(getExprID(deltaId, ctx), isDecimal, sub);
  }

  SgType&
  createSubtypeFromRootIfNeeded(SgType& basetype, SgAdaTypeConstraint* constraint, AstContext)
  {
    return constraint ? mkAdaSubtype(basetype, *constraint, true /* from root */) : basetype;
  }


  TypeData
  getTypeFoundation(const std::string& name, Definition_Struct& def, AstContext ctx)
  {
    ADA_ASSERT(def.Definition_Kind == A_Type_Definition);

    logKind("A_Type_Definition");

    Type_Definition_Struct& typenode = def.The_Union.The_Type_Definition;
    TypeData                res{&typenode, nullptr, false, false, false};

    /* unused fields:
       Definition_Struct
         bool                           Has_Null_Exclusion;
    */

    switch (typenode.Type_Kind)
    {
      case A_Derived_Type_Definition:              // 3.4(2)     -> Trait_Kinds
        {
          logKind("A_Derived_Type_Definition");
          /*
             unused fields: (derivedTypeDef)
                Declaration_List     Implicit_Inherited_Declarations;
          */
          SgType& basetype = getDefinitionTypeID(typenode.Parent_Subtype_Indication, ctx);

          //~ if (isSgEnumType(si::Ada::base)
          res.sageNode(mkAdaDerivedType(basetype));
          break;
        }

      case A_Derived_Record_Extension_Definition:  // 3.4(2)     -> Trait_Kinds
        {
          logKind("A_Derived_Record_Extension_Definition");

          SgClassDefinition&  def    = getRecordBodyID(typenode.Record_Definition, ctx);
          SgBaseClass&        parent = getParentTypeID(typenode.Parent_Subtype_Indication, ctx);

          sg::linkParentChild(def, parent, &SgClassDefinition::append_inheritance);

          /* unused fields:
          Declaration_List     Implicit_Inherited_Declarations;
          Declaration_List     Implicit_Inherited_Subprograms;
          Declaration          Corresponding_Parent_Subtype;
          Declaration          Corresponding_Root_Type;
          Declaration          Corresponding_Type_Structure;
          Expression_List      Definition_Interface_List;
          */
          res.sageNode(def);
          break;
        }

      case An_Enumeration_Type_Definition:         // 3.5.1(2)
        {
          ADA_ASSERT(name.size());

          logKind("An_Enumeration_Type_Definition");

          SgEnumDeclaration& sgnode = mkEnumDefn(name, ctx.scope());
          ElemIdRange        enums = idRange(typenode.Enumeration_Literal_Declarations);

          traverseIDs(enums, elemMap(), EnumeratorCreator{sgnode, ctx});
          /* unused fields:
           */
          res.sageNode(sgnode);
          break ;
        }

      case A_Signed_Integer_Type_Definition:       // 3.5.4(3)
        {
          logKind("A_Signed_Integer_Type_Definition");

          SgAdaTypeConstraint& constraint = getConstraintID(typenode.Integer_Constraint, ctx);
          SgType&              superty    = mkIntegralType();

          res.sageNode(mkAdaSubtype(superty, constraint, true /* from root */));
          /* unused fields:
           */
          break;
        }

      case A_Modular_Type_Definition:              // 3.5.4(4)
        {
          logKind("A_Modular_Type_Definition");

          SgExpression& modexpr = getExprID(typenode.Mod_Static_Expression, ctx);

          res.sageNode(mkAdaModularType(modexpr));
          /* unused fields:
           */
          break;
        }

      case A_Floating_Point_Definition:            // 3.5.7(2)
        {
          logKind("A_Floating_Point_Definition");

          SgType&               basetype   = SG_DEREF(sb::buildFloatType()); // \todo use mkRealType() ??
          SgAdaTypeConstraint*  constraint = getConstraintID_opt(typenode.Real_Range_Constraint, ctx);

          constraint = createDigitsConstraintIfNeeded(typenode.Digits_Expression, constraint, ctx);

          res.sageNode(createSubtypeFromRootIfNeeded(basetype, constraint, ctx));
          break;
        }

      case An_Ordinary_Fixed_Point_Definition:     // 3.5.9(3)
        {
          logKind("An_Ordinary_Fixed_Point_Definition");

          SgAdaTypeConstraint*  constraint = getConstraintID_opt(typenode.Real_Range_Constraint, ctx);

          constraint = createDeltaConstraintIfNeeded(typenode.Delta_Expression, false, constraint, ctx);

          res.sageNode(createSubtypeFromRootIfNeeded(mkFixedType(), constraint, ctx));
          break;
        }

      case A_Decimal_Fixed_Point_Definition:       // 3.5.9(6)
        {
          logKind("A_Decimal_Fixed_Point_Definition");

          SgAdaTypeConstraint*  constraint = getConstraintID_opt(typenode.Real_Range_Constraint, ctx);

          constraint = createDigitsConstraintIfNeeded(typenode.Digits_Expression, constraint, ctx);
          constraint = createDeltaConstraintIfNeeded(typenode.Delta_Expression, true, constraint, ctx);

          res.sageNode(createSubtypeFromRootIfNeeded(mkFixedType(), constraint, ctx));
          break;
        }

      case A_Constrained_Array_Definition:         // 3.6(2)
        {
          logKind("A_Constrained_Array_Definition");

          res.sageNode(createConstrainedArrayType(typenode, ctx));
          //~ ADA_ASSERT(indicesAst.get_parent());
          /* unused fields:
          */
          break ;
        }

      case An_Unconstrained_Array_Definition:      // 3.6(2)
        {
          logKind("An_Unconstrained_Array_Definition");

          res.sageNode(createUnconstrainedArrayType(typenode, ctx));
          //~ ADA_ASSERT(indicesAst.get_parent());
          /* unused fields:
          */
          break;
        }

      case A_Record_Type_Definition:               // 3.8(2)     -> Trait_Kinds
      case A_Tagged_Record_Type_Definition:        // 3.8(2)     -> Trait_Kinds
        {
          logKind(typenode.Type_Kind == A_Record_Type_Definition ? "A_Record_Type_Definition" : "A_Tagged_Record_Type_Definition");

          SgClassDefinition& def = getRecordBodyID(typenode.Record_Definition, ctx);

          // why not use Has_Tagged?
          res = TypeData{&typenode, &def, typenode.Has_Abstract, typenode.Has_Limited, typenode.Type_Kind == A_Tagged_Record_Type_Definition};
          /*
             unused fields (A_Record_Type_Definition):

             unused fields (A_Tagged_Record_Type_Definition):
                bool                 Has_Private;
                bool                 Has_Tagged;
                Declaration_List     Corresponding_Type_Operators;

             break;
          */
          break;
        }

      case An_Access_Type_Definition:              // 3.10(2)    -> Access_Type_Kinds
        {
          logKind("An_Access_Type_Definition");
          SgType& access_t = getAccessType(typenode.Access_Type, ctx);
          res.sageNode(access_t);
          break;
        }

      case Not_A_Type_Definition: /* break; */     // An unexpected element
      case A_Root_Type_Definition:                 // 3.5.4(14):  3.5.6(3)
      //  //|A2005 start
      case An_Interface_Type_Definition:           // 3.9.4      -> Interface_Kinds
      //  //|A2005 end
      default:
        {
          logWarn() << "unhandled type kind " << typenode.Type_Kind << std::endl;
          ADA_ASSERT(!FAIL_ON_ERROR(ctx));
          res.sageNode(mkTypeUnknown());
        }
    }

    ADA_ASSERT(&res.sageNode());
    return res;
  }

  SgType&
  getDefinitionType(Element_Struct& elem, AstContext ctx, bool forceSubtype = false)
  {
    ADA_ASSERT(elem.Element_Kind == A_Definition);

    Definition_Struct& def = elem.The_Union.Definition;
    SgType*            res = nullptr;

    switch (def.Definition_Kind)
    {
      case A_Type_Definition:
        {
          TypeData resdata = getTypeFoundation("", def, ctx);

          res = isSgType(&resdata.sageNode());
          ADA_ASSERT(res != nullptr);
          break;
        }

      case A_Subtype_Indication:
        {
          logKind("A_Subtype_Indication");

          Subtype_Indication_Struct& subtype = def.The_Union.The_Subtype_Indication;

          res = &getDeclTypeID(subtype.Subtype_Mark, ctx);

          // \todo if there is no subtype constraint, shall we produce
          //       a subtype w/ NoConstraint, or leave the original type?
          if (forceSubtype || subtype.Subtype_Constraint)
          {
            //~ SgAdaTypeConstraint& range = getConstraintID_opt(subtype.Subtype_Constraint, ctx);
            SgAdaTypeConstraint& range = getConstraintID(subtype.Subtype_Constraint, ctx);

            res = &mkAdaSubtype(SG_DEREF(res), range);
          }

          res = &excludeNullIf(SG_DEREF(res), subtype.Has_Null_Exclusion, ctx);
          break;
        }

      case A_Component_Definition:
        {
          logKind("A_Component_Definition");

          Component_Definition_Struct& component = def.The_Union.The_Component_Definition;

#if ADA_2005_OR_MORE_RECENT
          res = &getDefinitionTypeID(component.Component_Definition_View, ctx);
#else
          res = &getDefinitionTypeID(component.Component_Subtype_Indication, ctx);
#endif /* ADA_2005_OR_MORE_RECENT */

          if (component.Has_Aliased)
            res = &mkAliasedType(*res);

          /* unused fields:
          */
          break;
        }

      case A_Discrete_Subtype_Definition:
        {
          logKind("A_Discrete_Subtype_Definition");

          Discrete_Subtype_Definition_Struct& subdef = def.The_Union.The_Discrete_Subtype_Definition;

          // handle A_Discrete_Subtype_Indication here, because getDefinitionExpr would
          //   convert that into a type.
          res = (subdef.Discrete_Range_Kind == A_Discrete_Subtype_Indication)
                      ? &getDiscreteSubtypeID(subdef.Subtype_Mark, subdef.Subtype_Constraint, ctx)
                      : &mkExprAsType(getDiscreteSubtypeExpr(elem, def, ctx));

          break;
        }

      case An_Access_Definition:
        {
          logKind("An_Access_Definition");
          res = &getAnonymousAccessType(def, ctx);
          /* unused fields:
          */
          break;
        }

      default:
        logWarn() << "Unhandled type definition: " << def.Definition_Kind << std::endl;
        res = &mkTypeUnknown();
        ADA_ASSERT(!FAIL_ON_ERROR(ctx));
    }

    return SG_DEREF(res);
  }


  struct DiscriminantInstanceCreator
  {
      explicit
      DiscriminantInstanceCreator(AstContext astctx)
      : ctx(astctx), elems()
      {}

      DiscriminantInstanceCreator(DiscriminantInstanceCreator&&)                 = default;
      DiscriminantInstanceCreator& operator=(DiscriminantInstanceCreator&&)      = default;

      // \todo the following copying functions should be deleted post C++17
      // @{
      DiscriminantInstanceCreator(const DiscriminantInstanceCreator&)            = default;
      DiscriminantInstanceCreator& operator=(const DiscriminantInstanceCreator&) = default;
      // @}

      void operator()(Element_Struct& el);

      /// result read-out
      operator SgExpressionPtrList () &&
      {
        return std::move(elems);
      }

    private:
      AstContext          ctx;
      SgExpressionPtrList elems;

      DiscriminantInstanceCreator() = delete;
  };

  void DiscriminantInstanceCreator::operator()(Element_Struct& el)
  {
    using name_container = std::vector<NameData>;

    ADA_ASSERT(el.Element_Kind == An_Association);

    Association_Struct&        assoc = el.The_Union.Association;
    ADA_ASSERT(assoc.Association_Kind == A_Discriminant_Association);
    logKind("A_Discriminant_Association");

    ElemIdRange                nameRange = idRange(assoc.Discriminant_Selector_Names);
    name_container             names     = allNames(nameRange, ctx);
    SgExpression&              sgnode    = getExprID(assoc.Discriminant_Expression, ctx);

    attachSourceLocation(sgnode, el, ctx);

    if (names.empty())
    {
      elems.push_back(&sgnode);
    }
    else
    {
      NameData name = names.front();

      // \todo SgActualArgumentExpression only supports 1:1 mapping from name to an expression
      //       but not n:1.
      //       => create an entry for each name, and duplicate the expression
      elems.push_back(sb::buildActualArgumentExpression_nfi(name.ident, &sgnode));

      std::for_each( std::next(names.begin()), names.end(),
                     [&](NameData discrName) -> void
                     {
                       elems.push_back(sb::buildActualArgumentExpression_nfi(discrName.ident, si::deepCopy(&sgnode)));
                     }
                   );
    }
  }

  // PP: rewrote this code to create the SgAdaFormalTypeDecl together with the type
  FormalTypeData
  getFormalTypeFoundation( const std::string& name,
                           Definition_Struct& def,
                           SgScopeStatement& /*inheritedRoutineScope*/,
                           AstContext ctx
                         )
  {
    ADA_ASSERT(def.Definition_Kind == A_Formal_Type_Definition);
    logKind("A_Formal_Type_Definition");

    Formal_Type_Definition_Struct& typenode = def.The_Union.The_Formal_Type_Definition;
    SgAdaFormalTypeDecl&           sgnode = mkAdaFormalTypeDecl(name, ctx.scope());
    FormalTypeData                 res{&typenode, &sgnode};
    SgAdaFormalType&               formal = SG_DEREF(sgnode.get_type());
    SgType*                        formalBaseType = nullptr;

    switch (typenode.Formal_Type_Kind)
    {
      case A_Formal_Private_Type_Definition:         // 12.5.1(2)   -> Trait_Kinds
      case A_Formal_Tagged_Private_Type_Definition:  // 12.5.1(2)   -> Trait_Kinds
        {
          const bool tagged = (typenode.Formal_Type_Kind == A_Formal_Tagged_Private_Type_Definition);

          logKind(tagged ? "A_Formal_Tagged_Private_Type_Definition" : "A_Formal_Private_Type_Definition");

          res.setAbstract(typenode.Has_Abstract);
          res.setLimited(typenode.Has_Limited);
          res.setTagged(tagged && typenode.Has_Tagged);

          // NOTE: we use a private flag on the type instead of the privatize()
          // code used elsewhere since they currently denote different types of
          // private-ness.
          formal.set_is_private(typenode.Has_Private);

          // PP (2/16/22): to avoid null pointers in types
          formalBaseType = &mkOpaqueType();
          break;
        }

      case A_Formal_Derived_Type_Definition:         // 12.5.1(3)   -> Trait_Kinds
        {
          logKind("A_Formal_Derived_Type_Definition");

          res.setAbstract(typenode.Has_Abstract);
          res.setLimited(typenode.Has_Limited);

          SgType& undertype = getDeclTypeID(typenode.Subtype_Mark, ctx);

          formal.set_is_private(typenode.Has_Private);
          formalBaseType = &mkAdaDerivedType(undertype);

          res.inheritsRoutines(true);

          /* unused fields:
               bool                 Has_Synchronized
               Expression_List      Definition_Interface_List
           */
          break;
        }


      case A_Formal_Access_Type_Definition:          // 3.10(3),3.10(5)
        {
          logKind("A_Formal_Access_Type_Definition");
          formalBaseType = &getAccessType(typenode.Access_Type, ctx);
          /* unused fields:
           */
          break;
        }

      case A_Formal_Signed_Integer_Type_Definition:  // 12.5.2(3)
        {
          logKind("A_Formal_Signed_Integer_Type_Definition");
          formalBaseType = &mkIntegralType();
          /* unused fields:
           */
          break;
        }

      case A_Formal_Modular_Type_Definition:         // 12.5.2(4)
        {
          logKind("A_Formal_Modular_Type_Definition");
          formalBaseType = &mkAdaModularType(mkNullExpression());
          /* unused fields:
           */
          break;
        }

      case A_Formal_Floating_Point_Definition:       // 12.5.2(5)
        {
          logKind("A_Formal_Floating_Point_Definition");
          formalBaseType = &mkRealType();
          /* unused fields:
           */
          break;
        }

      case A_Formal_Discrete_Type_Definition:        // 12.5.2(2)
        {
          logKind("A_Formal_Discrete_Type_Definition");
          formalBaseType = &mkAdaDiscreteType();
          /* unused fields:
           */
          break;
        }

      case A_Formal_Ordinary_Fixed_Point_Definition: // 12.5.2(6)
        {
          logKind("A_Formal_Ordinary_Fixed_Point_Definition");
          formalBaseType = &mkFixedType();
          /* unused fields:
           */
          break;
        }

      case A_Formal_Decimal_Fixed_Point_Definition:  // 12.5.2(7)
        {
          logKind("A_Formal_Decimal_Fixed_Point_Definition");
          SgType&               fixed   = mkFixedType();
          SgAdaDeltaConstraint& decimal = mkAdaDeltaConstraint(mkNullExpression(), true, nullptr);

          formalBaseType = &mkAdaSubtype(fixed, decimal);
          break;
        }

      case A_Formal_Unconstrained_Array_Definition:  // 3.6(3)
        {
          logKind("A_Formal_Unconstrained_Array_Definition");
          formalBaseType = &createUnconstrainedArrayType(typenode, ctx);
          break;
        }

      case A_Formal_Constrained_Array_Definition:    // 3.6(5)
        {
          logKind("A_Formal_Constrained_Array_Definition");
          formalBaseType = &createConstrainedArrayType(typenode, ctx);
          break;
        }

      // MS: types to do later when we need them
        //|A2005 start
      case A_Formal_Interface_Type_Definition:       // 12.5.5(2) -> Interface_Kinds
        //|A2005 end

      default:
        logWarn() << "unhandled formal type kind " << typenode.Formal_Type_Kind << std::endl;
        // NOTE: temporarily create an AdaFormalType with the given name,
        //       but set no fields.  This is sufficient to pass some test cases but
        //       is not correct.
        ADA_ASSERT(!FAIL_ON_ERROR(ctx));
        formalBaseType = &mkTypeUnknown();
    }

    ADA_ASSERT(formalBaseType);

    formal.set_formal_type(formalBaseType);

/*
    if (inheritsDeclarationsAndSubprograms)
    {
      processInheritedSubroutines( formal,
                                   idRange(typenode.Implicit_Inherited_Subprograms),
                                   idRange(typenode.Implicit_Inherited_Declarations),
                                   ctx.scope(inheritedRoutineScope)
                                 );
    }
*/
    return res;
  }
} // anonymous

std::pair<SgInitializedName*, SgAdaRenamingDecl*>
getExceptionBase(Element_Struct& el, AstContext ctx)
{
  ADA_ASSERT(el.Element_Kind == An_Expression);

  NameData        name = getQualName(el, ctx);
  Element_Struct& elem = name.elem();

  ADA_ASSERT(elem.Element_Kind == An_Expression);
  Expression_Struct& ex  = elem.The_Union.Expression;

  //~ use this if package standard is included
  //~ return lookupNode(asisExcps(), ex.Corresponding_Name_Definition);

  // first try: look up in user defined exceptions
  if (SgInitializedName* ini = findFirst(asisExcps(), ex.Corresponding_Name_Definition))
    return std::make_pair(ini, nullptr);

  // second try: look up in renamed declarations
  if (SgDeclarationStatement* dcl = findFirst(asisDecls(), ex.Corresponding_Name_Definition))
  {
    SgAdaRenamingDecl& rendcl = SG_DEREF(isSgAdaRenamingDecl(dcl));

    return std::make_pair(nullptr, &rendcl);
  }

  // third try: look up in exceptions defined in standard
  if (SgInitializedName* ini = findFirst(adaExcps(), AdaIdentifier{ex.Name_Image}))
    return std::make_pair(ini, nullptr);

  // last resort: create a new initialized name representing the exception
  logError() << "Unknown exception: " << ex.Name_Image << std::endl;
  ADA_ASSERT(!FAIL_ON_ERROR(ctx));

  // \todo create an SgInitializedName if the exception was not found
  SgInitializedName& init = mkInitializedName(ex.Name_Image, lookupNode(adaTypes(), AdaIdentifier{"Exception"}), nullptr);

  init.set_scope(&ctx.scope());
  return std::make_pair(&init, nullptr);
}


SgType&
getDiscreteSubtypeID(Element_ID typeId, Element_ID constraintID, AstContext ctx)
{
  SgType*              res = &getDeclTypeID(typeId, ctx);
  SgAdaTypeConstraint* constraint = getConstraintID_opt(constraintID, ctx);

  ADA_ASSERT(res);
  return constraint ? mkAdaSubtype(*res, *constraint) : *res;
}

SgAdaTypeConstraint&
getConstraintID(Element_ID el, AstContext ctx)
{
  ADA_ASSERT(!isInvalidId(el));

  if (el == 0) return mkAdaNullConstraint();

  SgAdaTypeConstraint*  res = nullptr;
  Element_Struct&       elem = retrieveElem(elemMap(), el);
  ADA_ASSERT(elem.Element_Kind == A_Definition);

  Definition_Struct&    def = elem.The_Union.Definition;
  ADA_ASSERT(def.Definition_Kind == A_Constraint);

  logKind("A_Constraint");

  Constraint_Struct&    constraint = def.The_Union.The_Constraint;

  switch (constraint.Constraint_Kind)
  {
    case A_Simple_Expression_Range:             // 3.2.2: 3.5(3)
      {
        logKind("A_Simple_Expression_Range");

        SgExpression& lb       = getExprID(constraint.Lower_Bound, ctx);
        SgExpression& ub       = getExprID(constraint.Upper_Bound, ctx);
        SgRangeExp&   rangeExp = mkRangeExp(lb, ub);

        res = &mkAdaRangeConstraint(rangeExp);
        break;
      }

    case A_Range_Attribute_Reference:           // 3.5(2)
      {
        logKind("A_Range_Attribute_Reference");

        SgExpression& rangeExp = getExprID(constraint.Range_Attribute, ctx);

        res = &mkAdaRangeConstraint(rangeExp);
        break;
      }

    case An_Index_Constraint:                   // 3.2.2: 3.6.1
      {
        logKind("An_Index_Constraint");

        ElemIdRange         idxRanges = idRange(constraint.Discrete_Ranges);
        SgExpressionPtrList ranges = traverseIDs(idxRanges, elemMap(), RangeListCreator{ctx});

        res = &mkAdaIndexConstraint(std::move(ranges));
        break;
      }

    case A_Discriminant_Constraint:             // 3.2.2
      {
        logKind("A_Discriminant_Constraint");

        ElemIdRange         discrRange  = idRange(constraint.Discriminant_Associations);
        SgExpressionPtrList constraints = traverseIDs(discrRange, elemMap(), DiscriminantInstanceCreator{ctx});

        res = &mkAdaDiscriminantConstraint(std::move(constraints));
        break;
      }

    case A_Digits_Constraint:                   // 3.2.2: 3.5.9
      {
        SgAdaTypeConstraint* rangeConstr = getConstraintID_opt(constraint.Real_Range_Constraint, ctx);
        SgExpression&        digits = getExprID(constraint.Digits_Expression, ctx);

        res = &mkAdaDigitsConstraint(digits, rangeConstr);
        break;
      }

    case A_Delta_Constraint:                    // 3.2.2: J.3
      {
        SgAdaTypeConstraint* rangeConstr = getConstraintID_opt(constraint.Real_Range_Constraint, ctx);
        SgExpression&        delta = getExprID(constraint.Delta_Expression, ctx);

        // PP \todo can we get a decimal constraint, and how can we recognize it?
        res = &mkAdaDeltaConstraint(delta, false /* not a decimal constraint */, rangeConstr);
        break;
      }

    case Not_A_Constraint:                      // An unexpected element
    default:
      logError() << "Unhandled constraint: " << constraint.Constraint_Kind << std::endl;
      ROSE_ABORT();
  }

  attachSourceLocation(SG_DEREF(res), elem, ctx);
  return *res;
}


SgType&
getDeclTypeID(Element_ID id, AstContext ctx)
{
  return getDeclType(retrieveElem(elemMap(), id), ctx);
}


SgType&
getDefinitionTypeID(Element_ID defid, AstContext ctx, bool forceSubtype)
{
  return getDefinitionType(retrieveElem(elemMap(), defid), ctx, forceSubtype);
}

/// returns the ROSE type for an Asis definition \ref defid
/// iff defid is NULL, an SgTypeVoid is returned.
SgType&
getDefinitionTypeID_opt(Element_ID defid, AstContext ctx)
{
  // defid is null for example for an entry that does not specify a family type
  if (defid == 0)
    return mkTypeVoid();

  if (defid < 0)
  {
    logError() << "undefined type id: " << defid << std::endl;
    ADA_ASSERT(!FAIL_ON_ERROR(ctx));
    return mkTypeUnknown();
  }

  return getDefinitionTypeID(defid, ctx);
}

SgBaseClass&
getParentTypeID(Element_ID id, AstContext ctx)
{
  SgType& basety = getDefinitionTypeID(id, ctx);

  return mkRecordParent(basety);
}

FormalTypeData
getFormalTypeFoundation(const std::string& name, Declaration_Struct& decl, AstContext ctx)
{
  ADA_ASSERT( decl.Declaration_Kind == A_Formal_Type_Declaration );
  Element_Struct&         elem = retrieveElem(elemMap(), decl.Type_Declaration_View);
  ADA_ASSERT(elem.Element_Kind == A_Definition);
  Definition_Struct&      def = elem.The_Union.Definition;
  ADA_ASSERT(def.Definition_Kind == A_Formal_Type_Definition);

  if (SgAdaDiscriminatedTypeDecl* discr = createDiscriminatedDeclID_opt(decl.Discriminant_Part, 0, ctx))
  {
    SgScopeStatement&       genericScope = ctx.scope();
    SgScopeStatement&       discScope = SG_DEREF(discr->get_discriminantScope());
    FormalTypeData          res = getFormalTypeFoundation(name, def, genericScope, ctx.scope(discScope));
    SgDeclarationStatement& sgdecl = res.sageNode();

    sg::linkParentChild(*discr, sgdecl, &SgAdaDiscriminatedTypeDecl::set_discriminatedDecl);

    res.sageNode(*discr);
    return res;
  }

  return getFormalTypeFoundation(name, def, ctx.scope(), ctx);
}

TypeData
getTypeFoundation(const std::string& name, Declaration_Struct& decl, AstContext ctx)
{
  ADA_ASSERT( decl.Declaration_Kind == An_Ordinary_Type_Declaration );

  Element_Struct&         elem = retrieveElem(elemMap(), decl.Type_Declaration_View);
  ADA_ASSERT(elem.Element_Kind == A_Definition);

  Definition_Struct&      def = elem.The_Union.Definition;
  ADA_ASSERT(def.Definition_Kind == A_Type_Definition);

  return getTypeFoundation(name, def, ctx);
}


namespace
{
  SgTypedefDeclaration&
  declareIntSubtype(const std::string& name, int64_t lo, int64_t hi, SgAdaPackageSpec& scope)
  {
    SgType&               ty = mkIntegralType();
    SgLongLongIntVal&     lb = SG_DEREF(sb::buildLongLongIntVal(lo));
    ADA_ASSERT(lb.get_value() == lo);

    SgLongLongIntVal&     ub = SG_DEREF(sb::buildLongLongIntVal(hi));
    ADA_ASSERT(ub.get_value() == hi);

    SgRangeExp&           rng = mkRangeExp(lb, ub);
    SgAdaRangeConstraint& range = mkAdaRangeConstraint(rng);
    SgAdaSubtype&         subtype = mkAdaSubtype(ty, range, false /* from root*/ );
    SgTypedefDeclaration& sgnode = mkTypeDecl(name, subtype, scope);

    scope.append_statement(&sgnode);
    return sgnode;
  }

  template <class CxxType>
  SgTypedefDeclaration&
  declareIntSubtype(const std::string& name, SgAdaPackageSpec& scope)
  {
    using TypeLimits = std::numeric_limits<CxxType>;

    return declareIntSubtype(name, TypeLimits::min(), TypeLimits::max(), scope);
  }

  SgTypedefDeclaration&
  declareRealSubtype(const std::string& name, int ndigits, long double base, int exp, SgAdaPackageSpec& scope)
  {
    SgType&               ty = mkRealType();
    SgExpression&         lb = SG_DEREF(sb::buildFloat128Val(-std::pow(base, exp)));
    SgExpression&         ub = SG_DEREF(sb::buildFloat128Val(+std::pow(base, exp)));
    SgRangeExp&           rng = mkRangeExp(lb, ub);
    SgAdaRangeConstraint& range = mkAdaRangeConstraint(rng);
    SgExpression&         dig = SG_DEREF(sb::buildIntVal(ndigits));
    SgAdaTypeConstraint&  digits = mkAdaDigitsConstraint(dig, &range);
    SgAdaSubtype&         subtype = mkAdaSubtype(ty, digits);
    SgTypedefDeclaration& sgnode = mkTypeDecl(name, subtype, scope);

    scope.append_statement(&sgnode);
    return sgnode;
  }


  SgTypedefDeclaration&
  declareStringType(const std::string& name, SgType& positive, SgType& comp, SgAdaPackageSpec& scope)
  {
    SgExprListExp&        idx     = mkExprListExp({&mkTypeExpression(positive)});
    SgArrayType&          strtype = mkArrayType(comp, idx, true);
    SgTypedefDeclaration& sgnode  = mkTypeDecl(name, strtype, scope);

    scope.append_statement(&sgnode);
    return sgnode;
  }

  SgInitializedName&
  declareException(const std::string& name, SgType& base, SgAdaPackageSpec& scope)
  {
    SgInitializedName&              sgnode = mkInitializedName(name, base, nullptr);
    std::vector<SgInitializedName*> exdecl{ &sgnode };
    SgVariableDeclaration&          exvar = mkExceptionDecl(exdecl, scope);

    exvar.set_firstNondefiningDeclaration(&exvar);
    scope.append_statement(&exvar);
    return sgnode;
  }

  SgAdaPackageSpecDecl&
  declarePackage(const std::string& name, SgAdaPackageSpec& scope)
  {
    SgAdaPackageSpecDecl& sgnode = mkAdaPackageSpecDecl(name, scope);
    SgAdaPackageSpec&     pkgspec = SG_DEREF(sgnode.get_definition());

    scope.append_statement(&sgnode);
    sgnode.set_scope(&scope);

    markCompilerGenerated(pkgspec);
    markCompilerGenerated(sgnode);
    return sgnode;
  }

  // declares binary and unary operator declarations
  //   (all arguments are called left, right and right respectively)
  // see https://www.adaic.org/resources/add_content/standards/05rm/html/RM-A-1.html
  void declareOp(
                  map_t<OperatorKey, std::vector<OperatorDesc> >& fns,
                  //~ map_t<AdaIdentifier, std::vector<SgFunctionDeclaration*> >& fns,
                  const std::string& name,
                  SgType& retty,
                  std::vector<SgType*> params,
                  SgScopeStatement& scope
                )
  {
    ASSERT_require(params.size() > 0);

    std::string            fnname = si::Ada::roseOperatorPrefix + name;
    auto                   complete =
       [&params](SgFunctionParameterList& fnParmList, SgScopeStatement& scope)->void
       {
         static constexpr int MAX_PARAMS = 2;
         static const std::string parmNames[MAX_PARAMS] = { "Left", "Right" };

         int            parmNameIdx = MAX_PARAMS - params.size() - 1;
         SgTypeModifier defaultInMode;

         defaultInMode.setDefault();

         ADA_ASSERT(params.size() <= MAX_PARAMS);
         for (SgType* parmType : params)
         {
           const std::string&       parmName = parmNames[++parmNameIdx];
           SgInitializedName&       parmDecl = mkInitializedName(parmName, SG_DEREF(parmType), nullptr);
           SgInitializedNamePtrList parmList = {&parmDecl};
           /* SgVariableDeclaration&   pvDecl   =*/ mkParameter(parmList, defaultInMode, scope);

           parmDecl.set_parent(&fnParmList);
           fnParmList.get_args().push_back(&parmDecl);
         }
       };

    SgFunctionDeclaration& fndcl  = mkProcedureDecl_nondef(fnname, scope, retty, complete);

    fns[OperatorKey{&scope, name}].emplace_back(&fndcl, OperatorDesc::DECLARED_IN_STANDARD);
    //~ fns[].emplace_back(&fndcl);
  }

  template <class MapT>
  void declareCharConstants(MapT& m, const std::string& name, char ch, SgType& ty, SgScopeStatement& scope)
  {
    SgCharVal*             val = sb::buildCharVal(ch);
    markCompilerGenerated(SG_DEREF(val));

    SgInitializedName&     var = mkInitializedName(name, ty, val);
    SgVariableDeclaration& dcl = mkVarDecl(var, scope);

    scope.append_statement(&dcl);
    m[name] = &var;
  }

  void declareCharConstants(SgType& adaCharType, SgScopeStatement& asciispec)
  {
    char cval = 0;
    declareCharConstants(adaVars(), "nul",   cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "soh", ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "stx", ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "etx", ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "eot", ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "enq", ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "ack", ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "bel", ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "bs",  ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "ht",  ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "lf",  ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "vt",  ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "ff",  ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "cr",  ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "so",  ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "si",  ++cval, adaCharType, asciispec);

    declareCharConstants(adaVars(), "dle", ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "dc1", ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "dc2", ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "dc3", ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "dc4", ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "nak", ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "syn", ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "etb", ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "can", ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "em",  ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "sub", ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "esc", ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "fs",  ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "gs",  ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "rs",  ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "us",  ++cval, adaCharType, asciispec);
    ADA_ASSERT(cval == 31);

    declareCharConstants(adaVars(), "del",         127, adaCharType, asciispec);
    declareCharConstants(adaVars(), "exclam",      '!', adaCharType, asciispec);
    declareCharConstants(adaVars(), "quotation",   '"', adaCharType, asciispec);
    declareCharConstants(adaVars(), "sharp",       '#', adaCharType, asciispec);
    declareCharConstants(adaVars(), "dollar",      '$', adaCharType, asciispec);
    declareCharConstants(adaVars(), "percent",     '%', adaCharType, asciispec);
    declareCharConstants(adaVars(), "ampersand",   '&', adaCharType, asciispec);
    declareCharConstants(adaVars(), "colon",       ',', adaCharType, asciispec);
    declareCharConstants(adaVars(), "semicolon",   ';', adaCharType, asciispec);
    declareCharConstants(adaVars(), "query",       '?', adaCharType, asciispec);
    declareCharConstants(adaVars(), "at_sign",     '@', adaCharType, asciispec);
    declareCharConstants(adaVars(), "l_bracket",   '[', adaCharType, asciispec);
    declareCharConstants(adaVars(), "r_bracket",   ']', adaCharType, asciispec);
    declareCharConstants(adaVars(), "back_slash", '\\', adaCharType, asciispec);
    declareCharConstants(adaVars(), "circumflex",  '^', adaCharType, asciispec);
    declareCharConstants(adaVars(), "underline",   '_', adaCharType, asciispec);
    declareCharConstants(adaVars(), "grave",       '`', adaCharType, asciispec);
    declareCharConstants(adaVars(), "l_brace",     '{', adaCharType, asciispec);
    declareCharConstants(adaVars(), "r_brace",     '}', adaCharType, asciispec);
    declareCharConstants(adaVars(), "bar",         '_', adaCharType, asciispec);
    declareCharConstants(adaVars(), "tilde",       '~', adaCharType, asciispec);

    cval = 'a'-1;
    declareCharConstants(adaVars(), "lc_a", ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "lc_b", ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "lc_c", ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "lc_d", ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "lc_e", ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "lc_f", ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "lc_g", ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "lc_h", ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "lc_i", ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "lc_j", ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "lc_k", ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "lc_l", ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "lc_m", ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "lc_n", ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "lc_o", ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "lc_p", ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "lc_q", ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "lc_r", ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "lc_s", ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "lc_t", ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "lc_u", ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "lc_v", ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "lc_w", ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "lc_x", ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "lc_y", ++cval, adaCharType, asciispec);
    declareCharConstants(adaVars(), "lc_z", ++cval, adaCharType, asciispec);
    ADA_ASSERT(cval == 'z');
  }

  void buildAsciiPkg(SgType& adaCharType, SgAdaPackageSpec& stdspec)
  {
    SgAdaPackageSpecDecl& asciipkg    = declarePackage("Ascii", stdspec);
    SgAdaPackageSpec&     asciispec   = SG_DEREF(asciipkg.get_definition());

    adaPkgs()["STANDARD.ASCII"]       = &asciipkg;
    adaPkgs()["ASCII"]                = &asciipkg;
    declareCharConstants(adaCharType, asciispec);
  }
}


#if PREMAPPED_NUMERIC_TYPES
  // An earlier implementation used these type mappings in the AST.
  //   They were replaced with a newer, more Ada-like representation below.
  //   The original mappings are preserved for documentation, and in case
  //   the translation to C++ effort receives renewed interest.

  // FP types
  SgType& adaIntType                = mkIntegralType();
  SgType& intType                   = SG_DEREF(sb::buildIntType());
  SgType& adaCharType               = SG_DEREF(sb::buildCharType());
  SgType& adaWideCharType           = SG_DEREF(sb::buildChar16Type());
  SgType& adaWideWideCharType       = SG_DEREF(sb::buildChar32Type());

  adaTypes()["INTEGER"]             = &intType;
  adaTypes()["CHARACTER"]           = &adaCharType;
  adaTypes()["WIDE_CHARACTER"]      = &adaWideCharType;
  adaTypes()["WIDE_WIDE_CHARACTER"] = &adaWideWideCharType;
  adaTypes()["LONG_INTEGER"]        = sb::buildLongType(); // Long int
  adaTypes()["LONG_LONG_INTEGER"]   = sb::buildLongLongType(); // Long long int
  adaTypes()["SHORT_INTEGER"]       = sb::buildShortType(); // Long long int
  adaTypes()["SHORT_SHORT_INTEGER"] = declareIntSubtype("Short_Short_Integer", -(1 << 7), (1 << 7)-1, stdspec).get_type();

  SgType& adaRealType               = mkRealType();
  SgType& realType                  = SG_DEREF(sb::buildFloatType());
  adaTypes()["FLOAT"]               = &realType;
  adaTypes()["SHORT_FLOAT"]         = &realType;
  adaTypes()["LONG_FLOAT"]          = sb::buildDoubleType();
  adaTypes()["LONG_LONG_FLOAT"]     = sb::buildLongDoubleType();
#endif /* PREMAPPED_NUMERIC_TYPES */

void declareEnumItem(SgEnumDeclaration& enumdcl, const std::string& name, int repval)
{
  SgEnumType&         enumty = SG_DEREF(enumdcl.get_type());
  SgExpression&       sginit = SG_DEREF(sb::buildIntVal(repval));
  markCompilerGenerated(sginit);

  SgInitializedName&  sgnode = mkInitializedName(name, enumty, &sginit);

  sgnode.set_scope(enumdcl.get_scope());
  enumdcl.append_enumerator(&sgnode);
  ADA_ASSERT(sgnode.get_parent() == &enumdcl);
}

void initializePkgStandard(SgGlobal& global)
{
  // make available declarations from the package standard
  // https://www.adaic.org/resources/add_content/standards/05rm/html/RM-A-1.html

  constexpr auto ADAMAXINT = std::numeric_limits<std::int64_t>::max();

  SgAdaPackageSpecDecl& stdpkg  = mkAdaPackageSpecDecl(si::Ada::packageStandardName, global);
  SgAdaPackageSpec&     stdspec = SG_DEREF(stdpkg.get_definition());

  stdpkg.set_scope(&global);

  // \todo reconsider using a true Ada exception representation
  SgType&               exceptionType = SG_DEREF(sb::buildOpaqueType(si::Ada::exceptionName, &stdspec));

  adaTypes()["EXCEPTION"]           = &exceptionType;

  // boolean enum type
  AdaIdentifier         boolname{"BOOLEAN"};

  //~ SgType& adaBoolType               = SG_DEREF(sb::buildBoolType());
  SgEnumDeclaration&    boolDecl    = mkEnumDefn("BOOLEAN", stdspec);
  SgType&               adaBoolType = SG_DEREF(boolDecl.get_type());
  adaTypes()[boolname]              = &adaBoolType;

  declareEnumItem(boolDecl, "False", 0);
  declareEnumItem(boolDecl, "True",  1);

  // \todo reconsider adding a true Ada Duration type
  AdaIdentifier         durationName{si::Ada::durationTypeName};
  SgType&               adaDuration = SG_DEREF(sb::buildOpaqueType(durationName, &stdspec));

  adaTypes()[durationName]          = &adaDuration;

  // integral types
  SgType&               adaIntType  = mkIntegralType(); // the root integer type in ROSE

  adaTypes()["SHORT_SHORT_INTEGER"] = declareIntSubtype<std::int8_t> ("Short_Short_Integer", stdspec).get_type();
  adaTypes()["SHORT_INTEGER"]       = declareIntSubtype<std::int16_t>("Short_Integer",       stdspec).get_type();
  adaTypes()["INTEGER"]             = declareIntSubtype<std::int32_t>("Integer",             stdspec).get_type();
  adaTypes()["LONG_INTEGER"]        = declareIntSubtype<std::int64_t>("Long_Integer",        stdspec).get_type();
  adaTypes()["LONG_LONG_INTEGER"]   = declareIntSubtype<std::int64_t>("Long_Long_Integer",   stdspec).get_type();

  SgType& adaPositiveType           = SG_DEREF(declareIntSubtype("Positive", 1, ADAMAXINT, stdspec).get_type());
  SgType& adaNaturalType            = SG_DEREF(declareIntSubtype("Natural",  0, ADAMAXINT, stdspec).get_type());

  adaTypes()["POSITIVE"]            = &adaPositiveType;
  adaTypes()["NATURAL"]             = &adaNaturalType;

  // characters
  // \todo in Ada char, wide_char, and wide_wide_char are enums.
  //       Consider revising the ROSE representation.
  SgType& adaCharType               = SG_DEREF(sb::buildCharType());
  SgType& adaWideCharType           = SG_DEREF(sb::buildChar16Type());
  SgType& adaWideWideCharType       = SG_DEREF(sb::buildChar32Type());

  adaTypes()["CHARACTER"]           = &adaCharType;
  adaTypes()["WIDE_CHARACTER"]      = &adaWideCharType;
  adaTypes()["WIDE_WIDE_CHARACTER"] = &adaWideWideCharType;

  // from https://en.wikibooks.org/wiki/Ada_Programming/Libraries/Standard/GNAT
  // \todo consider using C++ min/max limits
  static constexpr long double VAL_S_FLOAT  = 3.40282;
  static constexpr int         EXP_S_FLOAT  = 38;
  static constexpr int         DIG_S_FLOAT  = 6;
  static constexpr long double VAL_FLOAT    = 3.40282;
  static constexpr int         EXP_FLOAT    = 38;
  static constexpr int         DIG_FLOAT    = 6;
  static constexpr long double VAL_L_FLOAT  = 1.79769313486232;
  static constexpr int         EXP_L_FLOAT  = 308;
  static constexpr int         DIG_L_FLOAT  = 15;
  static constexpr long double VAL_LL_FLOAT = 1.18973149535723177;
  static constexpr int         EXP_LL_FLOAT = 4932;
  static constexpr int         DIG_LL_FLOAT = 18;

  SgType& adaRealType               = mkRealType();
  adaTypes()["SHORT_FLOAT"]         = declareRealSubtype("Short_Float",     DIG_S_FLOAT,  VAL_S_FLOAT,  EXP_S_FLOAT,  stdspec).get_type();
  adaTypes()["FLOAT"]               = declareRealSubtype("Float",           DIG_FLOAT,    VAL_FLOAT,    EXP_FLOAT,    stdspec).get_type();
  adaTypes()["LONG_FLOAT"]          = declareRealSubtype("Long_Float",      DIG_L_FLOAT,  VAL_L_FLOAT,  EXP_L_FLOAT,  stdspec).get_type();
  adaTypes()["LONG_LONG_FLOAT"]     = declareRealSubtype("Long_Long_Float", DIG_LL_FLOAT, VAL_LL_FLOAT, EXP_LL_FLOAT, stdspec).get_type();

  // String types
  SgType& adaStringType             = SG_DEREF(declareStringType("String",           adaPositiveType, adaCharType,         stdspec).get_type());
  SgType& adaWideStringType         = SG_DEREF(declareStringType("Wide_String",      adaPositiveType, adaWideCharType,     stdspec).get_type());
  SgType& adaWideWideStringType     = SG_DEREF(declareStringType("Wide_Wide_String", adaPositiveType, adaWideWideCharType, stdspec).get_type());

  adaTypes()["STRING"]              = &adaStringType;
  adaTypes()["WIDE_STRING"]         = &adaWideStringType;
  adaTypes()["WIDE_WIDE_STRING"]    = &adaWideWideStringType;

  // Ada standard exceptions
  adaExcps()["CONSTRAINT_ERROR"]    = &declareException("Constraint_Error", exceptionType, stdspec);
  adaExcps()["PROGRAM_ERROR"]       = &declareException("Program_Error",    exceptionType, stdspec);
  adaExcps()["STORAGE_ERROR"]       = &declareException("Storage_Error",    exceptionType, stdspec);
  adaExcps()["TASKING_ERROR"]       = &declareException("Tasking_Error",    exceptionType, stdspec);

  // added packages
  adaPkgs()["STANDARD"]             = &stdpkg;

  // build ASCII package
  buildAsciiPkg(adaCharType, stdspec);

  //
  // build standard functions
  //~ map_t<OperatorKey, std::vector<OperatorDesc> >& opsMap = operatorSupport();
  auto& opsMap = operatorSupport();

  // bool
  declareOp(opsMap, "=",   adaBoolType, { &adaBoolType, &adaBoolType },    stdspec);
  declareOp(opsMap, "/=",  adaBoolType, { &adaBoolType, &adaBoolType },    stdspec);
  declareOp(opsMap, "<",   adaBoolType, { &adaBoolType, &adaBoolType },    stdspec);
  declareOp(opsMap, "<=",  adaBoolType, { &adaBoolType, &adaBoolType },    stdspec);
  declareOp(opsMap, ">",   adaBoolType, { &adaBoolType, &adaBoolType },    stdspec);
  declareOp(opsMap, ">=",  adaBoolType, { &adaBoolType, &adaBoolType },    stdspec);
  declareOp(opsMap, "and", adaBoolType, { &adaBoolType, &adaBoolType },    stdspec);
  declareOp(opsMap, "or",  adaBoolType, { &adaBoolType, &adaBoolType },    stdspec);
  declareOp(opsMap, "xor", adaBoolType, { &adaBoolType, &adaBoolType },    stdspec);
  declareOp(opsMap, "not", adaBoolType, { &adaBoolType }, /* unary */      stdspec);

  // integer
  declareOp(opsMap, "=",   adaBoolType, { &adaIntType,  &adaIntType },     stdspec);
  declareOp(opsMap, "/=",  adaBoolType, { &adaIntType,  &adaIntType },     stdspec);
  declareOp(opsMap, "<",   adaBoolType, { &adaIntType,  &adaIntType },     stdspec);
  declareOp(opsMap, "<=",  adaBoolType, { &adaIntType,  &adaIntType },     stdspec);
  declareOp(opsMap, ">",   adaBoolType, { &adaIntType,  &adaIntType },     stdspec);
  declareOp(opsMap, ">=",  adaBoolType, { &adaIntType,  &adaIntType },     stdspec);

  declareOp(opsMap, "+",   adaIntType,  { &adaIntType,  &adaIntType },     stdspec);
  declareOp(opsMap, "-",   adaIntType,  { &adaIntType,  &adaIntType },     stdspec);
  declareOp(opsMap, "*",   adaIntType,  { &adaIntType,  &adaIntType },     stdspec);
  declareOp(opsMap, "/",   adaIntType,  { &adaIntType,  &adaIntType },     stdspec);
  declareOp(opsMap, "rem", adaIntType,  { &adaIntType,  &adaIntType },     stdspec);
  declareOp(opsMap, "mod", adaIntType,  { &adaIntType,  &adaIntType },     stdspec);
  declareOp(opsMap, "**",  adaIntType,  { &adaIntType,  &adaNaturalType }, stdspec);

  declareOp(opsMap, "+",   adaIntType,  { &adaIntType }, /* unary */       stdspec);
  declareOp(opsMap, "-",   adaIntType,  { &adaIntType }, /* unary */       stdspec);
  declareOp(opsMap, "abs", adaIntType,  { &adaIntType }, /* unary */       stdspec);

  // float
  declareOp(opsMap, "=",   adaBoolType, { &adaRealType, &adaRealType },    stdspec);
  declareOp(opsMap, "/=",  adaBoolType, { &adaRealType, &adaRealType },    stdspec);
  declareOp(opsMap, "<",   adaBoolType, { &adaRealType, &adaRealType },    stdspec);
  declareOp(opsMap, "<=",  adaBoolType, { &adaRealType, &adaRealType },    stdspec);
  declareOp(opsMap, ">",   adaBoolType, { &adaRealType, &adaRealType },    stdspec);
  declareOp(opsMap, ">=",  adaBoolType, { &adaRealType, &adaRealType },    stdspec);

  declareOp(opsMap, "+",   adaRealType, { &adaRealType, &adaRealType },    stdspec);
  declareOp(opsMap, "-",   adaRealType, { &adaRealType, &adaRealType },    stdspec);
  declareOp(opsMap, "*",   adaRealType, { &adaRealType, &adaRealType },    stdspec);
  declareOp(opsMap, "/",   adaRealType, { &adaRealType, &adaRealType },    stdspec);
  declareOp(opsMap, "**",  adaRealType, { &adaRealType, &adaIntType },     stdspec);

  declareOp(opsMap, "+",   adaRealType, { &adaRealType }, /* unary */      stdspec);
  declareOp(opsMap, "-",   adaRealType, { &adaRealType }, /* unary */      stdspec);
  declareOp(opsMap, "abs", adaRealType, { &adaRealType }, /* unary */      stdspec);

  // mixed float and int
  declareOp(opsMap, "*",   adaRealType, { &adaIntType,  &adaRealType },    stdspec);
  declareOp(opsMap, "*",   adaRealType, { &adaRealType, &adaIntType },     stdspec);
  declareOp(opsMap, "/",   adaRealType, { &adaRealType, &adaIntType },     stdspec);

  // \todo what are built in fixed type operations??


  // operations on strings
  declareOp(opsMap, "=",   adaBoolType,           { &adaStringType,         &adaStringType },         stdspec);
  declareOp(opsMap, "/=",  adaBoolType,           { &adaStringType,         &adaStringType },         stdspec);
  declareOp(opsMap, "<",   adaBoolType,           { &adaStringType,         &adaStringType },         stdspec);
  declareOp(opsMap, "<=",  adaBoolType,           { &adaStringType,         &adaStringType },         stdspec);
  declareOp(opsMap, ">",   adaBoolType,           { &adaStringType,         &adaStringType },         stdspec);
  declareOp(opsMap, ">=",  adaBoolType,           { &adaStringType,         &adaStringType },         stdspec);
  declareOp(opsMap, "&",   adaStringType,         { &adaStringType,         &adaStringType },         stdspec);
  declareOp(opsMap, "&",   adaStringType,         { &adaCharType,           &adaStringType },         stdspec);
  declareOp(opsMap, "&",   adaStringType,         { &adaStringType,         &adaCharType   },         stdspec);
  declareOp(opsMap, "&",   adaStringType,         { &adaCharType,           &adaCharType   },         stdspec);

  declareOp(opsMap, "=",   adaBoolType,           { &adaWideStringType,     &adaWideStringType },     stdspec);
  declareOp(opsMap, "/=",  adaBoolType,           { &adaWideStringType,     &adaWideStringType },     stdspec);
  declareOp(opsMap, "<",   adaBoolType,           { &adaWideStringType,     &adaWideStringType },     stdspec);
  declareOp(opsMap, "<=",  adaBoolType,           { &adaWideStringType,     &adaWideStringType },     stdspec);
  declareOp(opsMap, ">",   adaBoolType,           { &adaWideStringType,     &adaWideStringType },     stdspec);
  declareOp(opsMap, ">=",  adaBoolType,           { &adaWideStringType,     &adaWideStringType },     stdspec);
  declareOp(opsMap, "&",   adaWideStringType,     { &adaWideStringType,     &adaWideStringType },     stdspec);
  declareOp(opsMap, "&",   adaWideStringType,     { &adaWideCharType,       &adaWideStringType },     stdspec);
  declareOp(opsMap, "&",   adaWideStringType,     { &adaWideStringType,     &adaWideCharType   },     stdspec);
  declareOp(opsMap, "&",   adaWideStringType,     { &adaWideCharType,       &adaWideCharType   },     stdspec);

  declareOp(opsMap, "=",   adaBoolType,           { &adaWideWideStringType, &adaWideWideStringType }, stdspec);
  declareOp(opsMap, "/=",  adaBoolType,           { &adaWideWideStringType, &adaWideWideStringType }, stdspec);
  declareOp(opsMap, "<",   adaBoolType,           { &adaWideWideStringType, &adaWideWideStringType }, stdspec);
  declareOp(opsMap, "<=",  adaBoolType,           { &adaWideWideStringType, &adaWideWideStringType }, stdspec);
  declareOp(opsMap, ">",   adaBoolType,           { &adaWideWideStringType, &adaWideWideStringType }, stdspec);
  declareOp(opsMap, ">=",  adaBoolType,           { &adaWideWideStringType, &adaWideWideStringType }, stdspec);
  declareOp(opsMap, "&",   adaWideWideStringType, { &adaWideWideStringType, &adaWideWideStringType }, stdspec);
  declareOp(opsMap, "&",   adaWideWideStringType, { &adaWideWideCharType,   &adaWideWideStringType }, stdspec);
  declareOp(opsMap, "&",   adaWideWideStringType, { &adaWideWideStringType, &adaWideWideCharType   }, stdspec);
  declareOp(opsMap, "&",   adaWideWideStringType, { &adaWideWideCharType,   &adaWideWideCharType   }, stdspec);

  // \todo operations on Duration

  // access types
  SgType& adaAccessType = SG_DEREF(sb::buildNullptrType());

  declareOp(opsMap, "=",   adaBoolType, { &adaAccessType, &adaAccessType   }, stdspec);
  declareOp(opsMap, "/=",  adaBoolType, { &adaAccessType, &adaAccessType   }, stdspec);

  // set the standard package in the SageInterface::ada namespace
  // \todo this should go away for a cleaner interface
  si::Ada::stdpkg = &stdpkg;
}


void ExHandlerTypeCreator::operator()(Element_Struct& elem)
{
  SgExpression* exceptExpr = nullptr;

  if (elem.Element_Kind == An_Expression)
  {
    auto              expair = getExceptionBase(elem, ctx);
    SgScopeStatement& scope = ctx.scope();

    exceptExpr = expair.first ? &mkExceptionRef(*expair.first, scope)
                              : &mkAdaRenamingRefExp(SG_DEREF(expair.second))
                              ;

    attachSourceLocation(SG_DEREF(exceptExpr), elem, ctx);
  }
  else if (elem.Element_Kind == A_Definition)
  {
    exceptExpr = &getDefinitionExpr(elem, ctx);
  }

  lst.push_back(&mkExceptionType(SG_DEREF(exceptExpr)));
}

ExHandlerTypeCreator::operator SgType&() &&
{
  ADA_ASSERT(lst.size() > 0);

  if (lst.size() == 1)
    return SG_DEREF(lst[0]);

  return mkTypeUnion(std::move(lst));
}

}



