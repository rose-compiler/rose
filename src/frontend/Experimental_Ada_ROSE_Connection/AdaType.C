#include "sage3basic.h"

#include <limits>

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
        if (SgAdaDiscriminatedTypeDecl* discrDcl = si::ada::getAdaDiscriminatedTypeDecl(n))
          declaredType = discrDcl->get_type();

        set(declaredType);
      }

      void set(SgType* ty)                   { ADA_ASSERT(ty); res = ty; }

      // error handler
      void handle(SgNode& n)                 { SG_UNEXPECTED_NODE(n); }

      // just use the type
      void handle(SgType& n)                 { set(&n); }

      // undecorated declarations
      void handle(SgAdaFormalTypeDecl& n)    { set(n.get_type()); }

      // possibly decorated with an SgAdaDiscriminatedTypeDecl
      // \{
      void handle(SgClassDeclaration& n)     { handleDiscrDecl(n, n.get_type()); }
      void handle(SgAdaTaskTypeDecl& n)      { handleDiscrDecl(n, n.get_type()); }
      void handle(SgAdaProtectedTypeDecl& n) { handleDiscrDecl(n, n.get_type()); }
      void handle(SgEnumDeclaration& n)      { handleDiscrDecl(n, n.get_type()); }
      void handle(SgTypedefDeclaration& n)   { handleDiscrDecl(n, n.get_type()); }
      // \}

      // others
      void handle(SgAdaAttributeExp& n)
      {
        attachSourceLocation(n, el, ctx); // \todo why is this not set where the node is made?
        set(&mkAttributeType(n));
      }

    private:
      Element_Struct& el;
      AstContext      ctx;
  };

  SgNode&
  getExprTypeID(Element_ID tyid, AstContext ctx);

  namespace
  {
    SgType* errorType(Expression_Struct& typeEx, AstContext ctx)
    {
      logError() << "unknown type name: " << typeEx.Name_Image
                 << " / " << typeEx.Corresponding_Name_Definition
                 << std::endl;

      ADA_ASSERT(!FAIL_ON_ERROR(ctx));
      return sb::buildVoidType();
    }
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
          || (res = findFirst(adaTypes(),  AdaIdentifier{typeEx.Name_Image}))
          || (res = errorType(typeEx, ctx))
          ;

          break;
        }

      case A_Selected_Component:
        {
          logKind("A_Selected_Component");
          res = &getExprTypeID(typeEx.Selector, ctx);
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
        res = sb::buildVoidType();
    }

    return SG_DEREF(res);
  }

  SgNode&
  getExprTypeID(Element_ID tyid, AstContext ctx)
  {
    Element_Struct& elem = retrieveAs(elemMap(), tyid);
    ADA_ASSERT(elem.Element_Kind == An_Expression);

    return getExprType(elem.The_Union.Expression, ctx);
  }



  SgType&
  getAccessType(Definition_Struct& def, AstContext ctx)
  {
    ADA_ASSERT(def.Definition_Kind == An_Access_Definition);

    logKind("An_Access_Definition");

    SgType*                   res = nullptr;
    Access_Definition_Struct& access = def.The_Union.The_Access_Definition;
    const auto                access_type_kind = access.Access_Definition_Kind;

    switch (access_type_kind)
    {
      case An_Anonymous_Access_To_Constant:            // [...] access constant subtype_mark
      case An_Anonymous_Access_To_Variable:            // [...] access subtype_mark
      {
        const bool isConstant = access_type_kind == An_Anonymous_Access_To_Constant;
        logKind(isConstant ? "An_Anonymous_Access_To_Constant" : "An_Anonymous_Access_To_Variable");

        SgType&          ty = getDeclTypeID(access.Anonymous_Access_To_Object_Subtype_Mark, ctx);
        SgAdaAccessType& access_t = mkAdaAccessType(ty);

        // PP(2/2/22)
        // cmp. getAccessTypeDefinition
        // \todo consider making a const qualifier
        // \todo what is general_access?
        if (isConstant)
          access_t.set_is_constant(true);
        //~ else
          //~ access_t.set_is_general_access(true);

        res = &access_t;
        /** unused fields:
               bool                         Has_Null_Exclusion;
         */
        break;
      }

      case An_Anonymous_Access_To_Procedure:           // access procedure
      case An_Anonymous_Access_To_Protected_Procedure: // access protected procedure
      case An_Anonymous_Access_To_Function:            // access function
      case An_Anonymous_Access_To_Protected_Function:  // access protected function
      {
        logWarn() << "subprogram access type support incomplete" << std::endl;

        // these are functions, so we need to worry about return types
        const bool isFuncAccess = (  (access_type_kind == An_Anonymous_Access_To_Function)
                                  || (access_type_kind == An_Anonymous_Access_To_Protected_Function)
                                  );

        if (access.Access_To_Subprogram_Parameter_Profile.Length > 0) {
          logWarn() << "subprogram access types with parameter profiles not supported." << std::endl;
          /*
            ElemIdRange range = idRange(access_type.Access_To_Subprogram_Parameter_Profile);

            SgFunctionParameterList& lst   = mkFunctionParameterList();
            SgFunctionParameterScope& psc  = mkLocatedNode<SgFunctionParameterScope>(&mkFileInfo());
            ParameterCompletion{range,ctx}(lst, ctx);

            ((SgAdaAccessType*)res.n)->set_subprogram_profile(&lst);
          */
        }

        SgType& retType = isFuncAccess ? getDeclTypeID(access.Access_To_Function_Result_Profile, ctx)
                                       : SG_DEREF(sb::buildVoidType());
        SgFunctionType& funty = mkFunctionType(retType /* \todo add paramter profile */);
        SgAdaAccessType& access_t = mkAdaAccessType(funty);

        access_t.set_is_object_type(false);

        // if protected, set the flag
        if (access_type_kind == An_Anonymous_Access_To_Protected_Function ||
            access_type_kind == An_Anonymous_Access_To_Protected_Procedure) {
          access_t.set_is_protected(true);
        }

        res = &access_t;
        break;
      }

      case Not_An_Access_Definition: /* break; */ // An unexpected element
      default:
        logWarn() << "adk? " << access_type_kind << std::endl;
        res = &mkAdaAccessType(SG_DEREF(sb::buildVoidType()));
        ADA_ASSERT(!FAIL_ON_ERROR(ctx));
    }

    return SG_DEREF(res);
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
      return getAccessType(def, ctx);

    logError() << "getDeclType: unhandled definition kind: " << def.Definition_Kind
               << std::endl;
    ADA_ASSERT(!FAIL_ON_ERROR(ctx));
    return SG_DEREF(sb::buildVoidType());
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

    traverseIDs(components, elemMap(), ElemCreator{ctx.scope_npc(sgnode)});

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
    Element_Struct&           elem = retrieveAs(elemMap(), recid);
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

  SgClassDeclaration&
  getParentRecordDecl(Definition_Struct& def, AstContext ctx)
  {
    ADA_ASSERT(def.Definition_Kind == A_Subtype_Indication);

    logKind("A_Subtype_Indication");

    Subtype_Indication_Struct& subtype = def.The_Union.The_Subtype_Indication;
    ADA_ASSERT (subtype.Subtype_Constraint == 0);

    Element_Struct&            subelem = retrieveAs(elemMap(), subtype.Subtype_Mark);
    ADA_ASSERT(subelem.Element_Kind == An_Expression);

    SgNode*                    basenode = &getExprType(subelem.The_Union.Expression, ctx);
    SgClassDeclaration*        res = isSgClassDeclaration(basenode);

    if (res == nullptr)
    {
      logError() << "getParentRecordDecl: " << typeid(*basenode).name() << std::endl;
      ROSE_ABORT();
    }

    return SG_DEREF(res);
  }


  struct EnumeratorCreator
  {
      EnumeratorCreator(SgEnumDeclaration& n, AstContext astctx)
      : enumdcl(n), enumty(SG_DEREF(n.get_type())), ctx(astctx)
      {}

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
        SgInitializedName&  sgnode = mkInitializedName(name.ident, enumty, &repval);

        sgnode.set_scope(enumdcl.get_scope());
        attachSourceLocation(sgnode, elem, ctx);
        //~ sg::linkParentChild(enumdcl, sgnode, &SgEnumDeclaration::append_enumerator);
        enumdcl.append_enumerator(&sgnode);
        ADA_ASSERT(sgnode.get_parent() == &enumdcl);

        recordNode(asisVars(), name.id(), sgnode);
      }

    private:
      SgEnumDeclaration& enumdcl;
      SgType&            enumty;
      AstContext         ctx;
  };

  SgAdaAccessType*
  getAccessTypeDefinition(Access_Type_Struct& access_type, AstContext ctx)
  {
    auto access_type_kind = access_type.Access_Type_Kind;
    bool isFuncAccess = false;
    SgAdaAccessType* access_t = nullptr;

    switch (access_type_kind) {
      // variable access kinds
    case A_Pool_Specific_Access_To_Variable:
    case An_Access_To_Variable:
    case An_Access_To_Constant:
      {
        SgType& ato = getDefinitionTypeID(access_type.Access_To_Object_Definition, ctx);
        access_t = &mkAdaAccessType(ato);

        // handle cases for ALL or CONSTANT general access modifiers
        switch (access_type_kind) {
        case An_Access_To_Variable:
          access_t->set_is_general_access(true);
          break;
        case An_Access_To_Constant:
          access_t->set_is_constant(true);
          break;
        default:
          break;
        }

        break;
      }

      // subprogram access kinds
    case An_Access_To_Function:
    case An_Access_To_Protected_Function:
    case An_Access_To_Procedure:
    case An_Access_To_Protected_Procedure:
      {
        logWarn() << "subprogram access type support incomplete" << std::endl;

        if (access_type_kind == An_Access_To_Function ||
            access_type_kind == An_Access_To_Protected_Function) {
          // these are functions, so we need to worry about return types
          isFuncAccess = true;
        }

        if (access_type.Access_To_Subprogram_Parameter_Profile.Length > 0) {
          logWarn() << "subprogram access types with parameter profiles not supported." << std::endl;
          /*
            ElemIdRange range = idRange(access_type.Access_To_Subprogram_Parameter_Profile);

            SgFunctionParameterList& lst   = mkFunctionParameterList();
            SgFunctionParameterScope& psc  = mkLocatedNode<SgFunctionParameterScope>(&mkFileInfo());
            ParameterCompletion{range,ctx}(lst, ctx);

            ((SgAdaAccessType*)res.n)->set_subprogram_profile(&lst);
          */
        }

        SgType& retType = isFuncAccess ? getDeclTypeID(access_type.Access_To_Function_Result_Profile, ctx)
                                       : SG_DEREF(sb::buildVoidType());
        SgFunctionType& funty = mkFunctionType(retType /* \todo add paramter profile */);

        access_t = &mkAdaAccessType(funty);
        access_t->set_is_object_type(false);


        // if protected, set the flag
        if (access_type_kind == An_Access_To_Protected_Procedure ||
            access_type_kind == An_Access_To_Protected_Function) {
          access_t->set_is_protected(true);
        }

        break;
      }
    default:
      logWarn() << "Unhandled access type kind: " << access_type_kind << std::endl;
      access_t = &mkAdaAccessType(SG_DEREF(sb::buildVoidType()));
      ADA_ASSERT(!FAIL_ON_ERROR(ctx));
    }

    ROSE_ASSERT(access_t);
    return access_t;
  }


  // PP: rewrote this code to create the SgAdaFormalTypeDecl together with the type
  TypeData
  getFormalTypeFoundation(const std::string& name, Definition_Struct& def, AstContext ctx)
  {
    ADA_ASSERT(def.Definition_Kind == A_Formal_Type_Definition);
    logKind("A_Formal_Type_Definition");

    Formal_Type_Definition_Struct& typenode = def.The_Union.The_Formal_Type_Definition;
    TypeData                       res{nullptr, nullptr, false, false, false};

    switch (typenode.Formal_Type_Kind)
      {
        // MS: types relevant right now

      case A_Formal_Private_Type_Definition:         // 12.5.1(2)   -> Trait_Kinds
        {
          logKind("A_Formal_Private_Type_Definition");
          SgAdaFormalTypeDecl& dcl =mkAdaFormalTypeDecl(name, ctx.scope());
          SgAdaFormalType&     t = SG_DEREF(dcl.get_type());
          res.setAbstract(typenode.Has_Abstract);
          res.setLimited(typenode.Has_Limited);
          res.setTagged(typenode.Has_Tagged);

          // NOTE: we use a private flag on the type instead of the privatize()
          // code used elsewhere since they currently denote different types of
          // private-ness.
          if (typenode.Has_Private) {
            t.set_is_private(true);
          }

          res.sageNode(dcl);
          break;
        }

      case A_Formal_Access_Type_Definition:          // 3.10(3),3.10(5)
        {
          logKind("A_Formal_Access_Type_Definition");

          SgAdaFormalTypeDecl& dcl =mkAdaFormalTypeDecl(name, ctx.scope());
          SgAdaFormalType&     t = SG_DEREF(dcl.get_type());
          SgAdaAccessType* access_t = getAccessTypeDefinition(typenode.Access_Type, ctx);
          t.set_formal_type(access_t);
          res.sageNode(dcl);
          break;
        }

        // MS: types to do later when we need them
      case A_Formal_Tagged_Private_Type_Definition:  // 12.5.1(2)   -> Trait_Kinds
      case A_Formal_Derived_Type_Definition:         // 12.5.1(3)   -> Trait_Kinds
      case A_Formal_Discrete_Type_Definition:        // 12.5.2(2)
      case A_Formal_Signed_Integer_Type_Definition:  // 12.5.2(3)
      case A_Formal_Modular_Type_Definition:         // 12.5.2(4)
      case A_Formal_Floating_Point_Definition:       // 12.5.2(5)
      case A_Formal_Ordinary_Fixed_Point_Definition: // 12.5.2(6)
      case A_Formal_Decimal_Fixed_Point_Definition:  // 12.5.2(7)

        //|A2005 start
      case A_Formal_Interface_Type_Definition:       // 12.5.5(2) -> Interface_Kinds
        //|A2005 end

      case A_Formal_Unconstrained_Array_Definition:  // 3.6(3)
      case A_Formal_Constrained_Array_Definition:    // 3.6(5)

      default:
        logWarn() << "unhandled formal type kind " << typenode.Formal_Type_Kind << std::endl;
        // NOTE: temporarily create an AdaFormalType with the given name,
        //       but set no fields.  This is sufficient to pass some test cases but
        //       is not correct.
        ADA_ASSERT(!FAIL_ON_ERROR(ctx));
        SgAdaFormalTypeDecl& dcl =mkAdaFormalTypeDecl(name, ctx.scope());
        res.sageNode(dcl);
      }

    return res;
  }

  SgAdaTypeConstraint*
  createDigitsConstraintIfNeeded(Expression_ID digitId, SgAdaTypeConstraint* sub, AstContext ctx)
  {
    if (digitId == 0) return sub;

    return &mkAdaDigitsConstraint(getExprID(digitId, ctx), sub);
  }

  SgAdaTypeConstraint*
  createDeltaConstraintIfNeeded(Expression_ID deltaId, SgAdaTypeConstraint* sub, AstContext ctx)
  {
    if (deltaId == 0) return sub;

    return &mkAdaDeltaConstraint(getExprID(deltaId, ctx), sub);
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

          res.sageNode(mkAdaDerivedType(basetype));
          break;
        }

      case A_Derived_Record_Extension_Definition:  // 3.4(2)     -> Trait_Kinds
        {
          logKind("A_Derived_Record_Extension_Definition");

          SgClassDefinition&  def    = getRecordBodyID(typenode.Record_Definition, ctx);
          SgClassDeclaration& basecl = getParentRecordDeclID(typenode.Parent_Subtype_Indication, ctx);
          SgBaseClass&        parent = mkRecordParent(basecl);

          sg::linkParentChild(def, parent, &SgClassDefinition::append_inheritance);

          /*
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
          SgTypeInt&           superty    = SG_DEREF(sb::buildIntType());

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

          constraint = createDeltaConstraintIfNeeded(typenode.Delta_Expression, constraint, ctx);

          res.sageNode(createSubtypeFromRootIfNeeded(mkFixedType(), constraint, ctx));
          break;
        }

      case A_Decimal_Fixed_Point_Definition:       // 3.5.9(6)
        {
          logKind("A_Decimal_Fixed_Point_Definition");

          SgAdaTypeConstraint*  constraint = getConstraintID_opt(typenode.Real_Range_Constraint, ctx);

          constraint = createDigitsConstraintIfNeeded(typenode.Digits_Expression, constraint, ctx);
          constraint = createDeltaConstraintIfNeeded(typenode.Delta_Expression, constraint, ctx);

          res.sageNode(createSubtypeFromRootIfNeeded(mkFixedType(), constraint, ctx));
          break;
        }

      case A_Constrained_Array_Definition:         // 3.6(2)
        {
          logKind("A_Constrained_Array_Definition");

          ElemIdRange                indicesAsis = idRange(typenode.Discrete_Subtype_Definitions);
          std::vector<SgExpression*> indicesSeq  = traverseIDs(indicesAsis, elemMap(), ExprSeqCreator{ctx});
          SgExprListExp&             indicesAst  = mkExprListExp(indicesSeq);
          SgType&                    compType    = getDefinitionTypeID(typenode.Array_Component_Definition, ctx);

          res.sageNode(mkArrayType(compType, indicesAst, false /* constrained */));
          ADA_ASSERT(indicesAst.get_parent());
          /* unused fields:
          */
          break ;
        }

      case An_Unconstrained_Array_Definition:      // 3.6(2)
        {
          logKind("An_Unconstrained_Array_Definition");

          ElemIdRange                indicesAsis = idRange(typenode.Index_Subtype_Definitions);
          std::vector<SgExpression*> indicesSeq  = traverseIDs(indicesAsis, elemMap(), ExprSeqCreator{ctx});
          SgExprListExp&             indicesAst  = mkExprListExp(indicesSeq);
          SgType&                    compType    = getDefinitionTypeID(typenode.Array_Component_Definition, ctx);

          res.sageNode(mkArrayType(compType, indicesAst, true /* unconstrained */));
          ADA_ASSERT(indicesAst.get_parent());
          /* unused fields:
          */
          break;
        }

      case A_Record_Type_Definition:               // 3.8(2)     -> Trait_Kinds
      case A_Tagged_Record_Type_Definition:        // 3.8(2)     -> Trait_Kinds
        {
          logKind(typenode.Type_Kind == A_Record_Type_Definition ? "A_Record_Type_Definition" : "A_Tagged_Record_Type_Definition");

          SgClassDefinition& def = getRecordBodyID(typenode.Record_Definition, ctx);

          (typenode.Has_Tagged ? logWarn() : logTrace())
             << "Type_Definition_Struct::tagged set ? " << typenode.Has_Tagged
             << std::endl;

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
          SgAdaAccessType* access_t = getAccessTypeDefinition(typenode.Access_Type, ctx);
          res.sageNode(SG_DEREF(access_t));
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
          res.sageNode(SG_DEREF(sb::buildVoidType()));
        }
    }

    ADA_ASSERT(&res.sageNode());
    return res;
  }

  SgType&
  getDefinitionType(Definition_Struct& def, AstContext ctx)
  {
    SgType* res = nullptr;

    switch (def.Definition_Kind)
    {
      case A_Type_Definition:
        {
          TypeData resdata = getTypeFoundation("", def, ctx);

          res = isSgType(&resdata.sageNode());
          ADA_ASSERT(res);
          break;
        }

      case A_Subtype_Indication:
        {
          logKind("A_Subtype_Indication");

          Subtype_Indication_Struct& subtype = def.The_Union.The_Subtype_Indication;

          res = &getDeclTypeID(subtype.Subtype_Mark, ctx);

          // \todo if there is no subtype constraint, shall we produce
          //       a subtype w/ NoConstraint, or leave the original type?
          if (subtype.Subtype_Constraint)
          {
            SgAdaTypeConstraint& range = getConstraintID(subtype.Subtype_Constraint, ctx);

            res = &mkAdaSubtype(SG_DEREF(res), range);
          }

          /* unused fields:
                bool       Has_Null_Exclusion;
          */
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

      case An_Access_Definition:
        {
          res = &getAccessType(def, ctx);

          /* unused fields:
          */
          break;
        }

      default:
        logWarn() << "Unhandled type definition: " << def.Definition_Kind << std::endl;
        res = sb::buildVoidType();
        ADA_ASSERT(!FAIL_ON_ERROR(ctx));
    }

    return SG_DEREF(res);
  }

  SgTypedefDeclaration&
  declareIntSubtype(const std::string& name, int64_t lo, int64_t hi, SgAdaPackageSpec& scope)
  {
    SgTypeInt&            ty = SG_DEREF(sb::buildIntType());
    SgIntVal&             lb = SG_DEREF(sb::buildIntVal(lo));
    SgIntVal&             ub = SG_DEREF(sb::buildIntVal(hi));
    SgRangeExp&           range = mkRangeExp(lb, ub);
    SgAdaRangeConstraint& constraint = mkAdaRangeConstraint(range);
    SgAdaSubtype&         subtype = mkAdaSubtype(ty, constraint);
    SgTypedefDeclaration& sgnode = mkTypeDecl(name, subtype, scope);

    scope.append_statement(&sgnode);
    return sgnode;
  }

  SgTypedefDeclaration&
  declareStringType(const std::string& name, SgType& positive, SgType& comp, SgAdaPackageSpec& scope)
  {
    SgExprListExp&        idx     = mkExprListExp({sb::buildTypeExpression(&positive)});
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

  struct DiscriminantCreator
  {
      explicit
      DiscriminantCreator(AstContext astctx)
      : ctx(astctx), elems()
      {}

      DiscriminantCreator(DiscriminantCreator&&)                 = default;
      DiscriminantCreator& operator=(DiscriminantCreator&&)      = default;

      // \todo the following copying functions should be deleted post C++17
      // @{
      DiscriminantCreator(const DiscriminantCreator&)            = default;
      DiscriminantCreator& operator=(const DiscriminantCreator&) = default;
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

      DiscriminantCreator() = delete;
  };

  void DiscriminantCreator::operator()(Element_Struct& el)
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
      elems.push_back(sb::buildActualArgumentExpression(name.ident, &sgnode));

      std::for_each( std::next(names.begin()), names.end(),
                     [&](NameData discrName) -> void
                     {
                       elems.push_back(sb::buildActualArgumentExpression(discrName.ident, si::deepCopy(&sgnode)));
                     }
                   );
    }
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


SgAdaTypeConstraint&
getConstraintID(Element_ID el, AstContext ctx)
{
  if (isInvalidId(el))
  {
    logWarn() << "Uninitialized element [range constraint]" << std::endl;
    return mkAdaRangeConstraint(mkRangeExp());
  }

  SgAdaTypeConstraint*  res = nullptr;
  Element_Struct&       elem = retrieveAs(elemMap(), el);
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
        SgExpressionPtrList constraints = traverseIDs(discrRange, elemMap(), DiscriminantCreator{ctx});

        res = &mkAdaDiscriminantConstraint(std::move(constraints));
        break;
      }

    case A_Digits_Constraint:                   // 3.2.2: 3.5.9
      {
        SgExpression&         digits = getExprID(constraint.Digits_Expression, ctx);
        SgAdaTypeConstraint*  rngConstr = getConstraintID_opt(constraint.Real_Range_Constraint, ctx);
        SgAdaRangeConstraint* range = isSgAdaRangeConstraint(rngConstr);
        ADA_ASSERT(!rngConstr || range);

        res = &mkAdaDigitsConstraint(digits, range);
        break;
      }

    case Not_A_Constraint: /* break; */         // An unexpected element
    case A_Delta_Constraint:                    // 3.2.2: J.3
    default:
      logWarn() << "Unhandled constraint: " << constraint.Constraint_Kind << std::endl;
      ADA_ASSERT(!FAIL_ON_ERROR(ctx));
      res = &mkAdaRangeConstraint(mkRangeExp());
  }

  attachSourceLocation(SG_DEREF(res), elem, ctx);
  return *res;
}


SgType&
getDeclTypeID(Element_ID id, AstContext ctx)
{
  return getDeclType(retrieveAs(elemMap(), id), ctx);
}


SgType&
getDefinitionTypeID(Element_ID defid, AstContext ctx)
{
  if (isInvalidId(defid))
  {
    logWarn() << "undefined type id: " << defid << std::endl;
    return SG_DEREF(sb::buildVoidType());
  }

  Element_Struct&     elem = retrieveAs(elemMap(), defid);
  ADA_ASSERT(elem.Element_Kind == A_Definition);

  return getDefinitionType(elem.The_Union.Definition, ctx);
}

SgClassDeclaration&
getParentRecordDeclID(Element_ID defid, AstContext ctx)
{
  Element_Struct&     elem = retrieveAs(elemMap(), defid);
  ADA_ASSERT(elem.Element_Kind == A_Definition);

  return getParentRecordDecl(elem.The_Union.Definition, ctx);
}

TypeData
getFormalTypeFoundation(const std::string& name, Declaration_Struct& decl, AstContext ctx)
{
  ADA_ASSERT( decl.Declaration_Kind == A_Formal_Type_Declaration );
  Element_Struct&         elem = retrieveAs(elemMap(), decl.Type_Declaration_View);
  ADA_ASSERT(elem.Element_Kind == A_Definition);
  Definition_Struct&      def = elem.The_Union.Definition;
  ADA_ASSERT(def.Definition_Kind == A_Formal_Type_Definition);
  return getFormalTypeFoundation(name, def, ctx);
}

TypeData
getTypeFoundation(const std::string& name, Declaration_Struct& decl, AstContext ctx)
{
  ADA_ASSERT( decl.Declaration_Kind == An_Ordinary_Type_Declaration );

  Element_Struct&         elem = retrieveAs(elemMap(), decl.Type_Declaration_View);
  ADA_ASSERT(elem.Element_Kind == A_Definition);

  Definition_Struct&      def = elem.The_Union.Definition;
  ADA_ASSERT(def.Definition_Kind == A_Type_Definition);

  return getTypeFoundation(name, def, ctx);
}

void initializePkgStandard(SgGlobal& global)
{
  // make available declarations from the package standard
  // https://www.adaic.org/resources/add_content/standards/05rm/html/RM-A-1.html

  constexpr auto ADAMAXINT = std::numeric_limits<int>::max();

  SgAdaPackageSpecDecl& stddecl = mkAdaPackageSpecDecl("Standard", global);
  SgAdaPackageSpec&     stdspec = SG_DEREF(stddecl.get_definition());

  stddecl.set_scope(&global);

  // \todo reconsider using a true Ada exception representation
  SgType&               exceptionType = SG_DEREF(sb::buildOpaqueType("Exception", &stdspec));

  adaTypes()["EXCEPTION"]           = &exceptionType;

  // \todo reconsider modeling Boolean as an enumeration of True and False
  adaTypes()["BOOLEAN"]             = sb::buildBoolType();

  // \todo reconsider adding a true Ada Duration type
  adaTypes()["DURATION"]            = sb::buildOpaqueType("Duration", &stdspec);

  // integral types
  SgType& intType                   = SG_DEREF(sb::buildIntType());
  SgType& characterType             = SG_DEREF(sb::buildCharType());
  SgType& wideCharacterType         = SG_DEREF(sb::buildChar16Type());
  SgType& wideWideCharacterType     = SG_DEREF(sb::buildChar32Type());

  adaTypes()["INTEGER"]             = &intType;
  adaTypes()["CHARACTER"]           = &characterType;
  adaTypes()["WIDE_CHARACTER"]      = &wideCharacterType;
  adaTypes()["WIDE_WIDE_CHARACTER"] = &wideWideCharacterType;
  adaTypes()["LONG_INTEGER"]        = sb::buildLongType(); // Long int
  adaTypes()["LONG_LONG_INTEGER"]   = sb::buildLongLongType(); // Long long int
  adaTypes()["SHORT_INTEGER"]       = sb::buildShortType(); // Long long int
  adaTypes()["SHORT_SHORT_INTEGER"] = declareIntSubtype("Short_Short_Integer", -(1 << 7), (1 << 7)-1, stdspec).get_type();

  // \todo floating point types
  adaTypes()["FLOAT"]               = sb::buildFloatType();  // Float is a subtype of Real
  adaTypes()["SHORT_FLOAT"]         = sb::buildFloatType();  // Float is a subtype of Real
  adaTypes()["LONG_FLOAT"]          = sb::buildDoubleType(); // Float is a subtype of Real
  adaTypes()["LONG_LONG_FLOAT"]     = sb::buildLongDoubleType(); // Long long Double?

  // int subtypes
  SgType& positiveType              = SG_DEREF(declareIntSubtype("Positive", 1, ADAMAXINT, stdspec).get_type());

  adaTypes()["POSITIVE"]            = &positiveType;
  adaTypes()["NATURAL"]             = declareIntSubtype("Natural",  0, ADAMAXINT, stdspec).get_type();


  // String types
  adaTypes()["STRING"]              = declareStringType("String",           positiveType, characterType,         stdspec).get_type();
  adaTypes()["WIDE_STRING"]         = declareStringType("Wide_String",      positiveType, wideCharacterType,     stdspec).get_type();
  adaTypes()["WIDE_WIDE_STRING"]    = declareStringType("Wide_Wide_String", positiveType, wideWideCharacterType, stdspec).get_type();

  // Ada standard exceptions
  adaExcps()["CONSTRAINT_ERROR"]    = &declareException("Constraint_Error", exceptionType, stdspec);
  adaExcps()["PROGRAM_ERROR"]       = &declareException("Program_Error",    exceptionType, stdspec);
  adaExcps()["STORAGE_ERROR"]       = &declareException("Storage_Error",    exceptionType, stdspec);
  adaExcps()["TASKING_ERROR"]       = &declareException("Tasking_Error",    exceptionType, stdspec);

  // added packages
  adaPkgs()["STANDARD"]             = &stddecl;
  adaPkgs()["STANDARD.ASCII"]       = &declarePackage("Ascii", stdspec);
  adaPkgs()["ASCII"]                = adaPkgs()["STANDARD.ASCII"];
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
