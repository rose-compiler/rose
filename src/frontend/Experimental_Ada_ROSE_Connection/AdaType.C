
#include "sage3basic.h"
#include "sageGeneric.h"

#include "AdaType.h"

#include "Ada_to_ROSE.h"
#include "AdaExpression.h"
#include "AdaMaker.h"

// turn on all GCC warnings after include files have been processed
#pragma GCC diagnostic warning "-Wall"
#pragma GCC diagnostic warning "-Wextra"


namespace sb = SageBuilder;


namespace Ada_ROSE_Translation
{

namespace
{
  struct MakeTyperef : sg::DispatchHandler<SgType*>
  {
    typedef sg::DispatchHandler<SgType*> base;

    MakeTyperef()
    : base()
    {}

    void handle(SgNode& n)               { SG_UNEXPECTED_NODE(n); }

    void handle(SgType& n)               { res = &n; }
    void handle(SgTypedefDeclaration& n) { res = &mkTypedefType(n); }
    void handle(SgClassDeclaration& n)   { res = &mkRecordType(n); }
    void handle(SgAdaTaskTypeDecl& n)    { res = &mkAdaTaskType(n); }
  };

  SgNode&
  getExprTypeID(Element_ID tyid, AstContext ctx);


  SgNode&
  getExprType(Expression_Struct& typeEx, AstContext ctx)
  {
    SgNode* res = nullptr;

    switch (typeEx.Expression_Kind)
    {
      case An_Identifier:
        {
          // is it a type?
          // typeEx.Corresponding_Name_Declaration ?
          res = findFirst(asisTypes(), typeEx.Corresponding_Name_Definition);

          if (!res)
          {
            // is it an exception?
            // typeEx.Corresponding_Name_Declaration ?
            res = findFirst(asisExcps(), typeEx.Corresponding_Name_Definition);
          }

          if (!res)
          {
            // is it a predefined Ada type?
            res = findFirst(adaTypes(), std::string{typeEx.Name_Image});
          }

          if (!res)
          {
            // what is it?
            logWarn() << "unknown type name: " << typeEx.Name_Image << std::endl;

            ROSE_ASSERT(!FAIL_ON_ERROR);
            res = sb::buildVoidType();
          }

          break /* counted in getExpr */;
        }

      case A_Selected_Component:
        {
          res = &getExprTypeID(typeEx.Selector, ctx);
          break /* counted in getExpr */;
        }

      default:
        logWarn() << "Unknown type expression: " << typeEx.Expression_Kind << std::endl;
        ROSE_ASSERT(!FAIL_ON_ERROR);
        res = sb::buildVoidType();
    }

    return SG_DEREF(res);
  }

  SgNode&
  getExprTypeID(Element_ID tyid, AstContext ctx)
  {
    Element_Struct& elem = retrieveAs<Element_Struct>(elemMap(), tyid);
    ROSE_ASSERT(elem.Element_Kind == An_Expression);

    return getExprType(elem.The_Union.Expression, ctx);
  }


  SgType&
  getAccessType(Definition_Struct& def, AstContext ctx)
  {
    ROSE_ASSERT(def.Definition_Kind == An_Access_Definition);

    Access_Definition_Struct& access = def.The_Union.The_Access_Definition;

    switch (access.Access_Definition_Kind)
    {
      case An_Anonymous_Access_To_Variable:            // [...] access subtype_mark
      case An_Anonymous_Access_To_Constant:            // [...] access constant subtype_mark
      case An_Anonymous_Access_To_Procedure:           // access procedure
      case An_Anonymous_Access_To_Protected_Procedure: // access protected procedure
      case An_Anonymous_Access_To_Function:            // access function
      case An_Anonymous_Access_To_Protected_Function:  // access protected function
        {
          logWarn() << "ak: " << access.Access_Definition_Kind << std::endl;
          break ; // not finished
        }

      case Not_An_Access_Definition: /* break; */ // An unexpected element
      default:
        logWarn() << "ak? " << access.Access_Definition_Kind << std::endl;
        ROSE_ASSERT(false);
    }

    ROSE_ASSERT(false);
    return SG_DEREF(sb::buildVoidType());
  }

  SgType&
  getDeclType(Element_Struct& elem, AstContext ctx)
  {
    if (elem.Element_Kind == An_Expression)
    {
      SgNode& basenode = getExprType(elem.The_Union.Expression, ctx);
      SgType* res      = sg::dispatch(MakeTyperef(), &basenode);

      return SG_DEREF(res);
    }

    ROSE_ASSERT(elem.Element_Kind == A_Definition);
    Definition_Struct& def = elem.The_Union.Definition;

    if (def.Definition_Kind == An_Access_Definition)
      return getAccessType(def, ctx);

    ROSE_ASSERT(false);
    return SG_DEREF(sb::buildVoidType());
  }

  SgAdaRangeConstraint&
  getRangeConstraint(Element_ID el, AstContext ctx)
  {
    if (isInvaldId(el))
    {
      logWarn() << "Uninitialized element [range constraint]" << std::endl;
      return mkAdaRangeConstraint(mkRangeExp());
    }

    SgAdaRangeConstraint* res = nullptr;
    Element_Struct&       elem = retrieveAs<Element_Struct>(elemMap(), el);
    ROSE_ASSERT(elem.Element_Kind == A_Definition);

    Definition_Struct&    def = elem.The_Union.Definition;
    ROSE_ASSERT(def.Definition_Kind == A_Constraint);

    Constraint_Struct&    constraint = def.The_Union.The_Constraint;

    switch (constraint.Constraint_Kind)
    {
      case A_Simple_Expression_Range:             // 3.2.2: 3.5(3)
        {
          SgExpression& lb       = getExprID(constraint.Lower_Bound, ctx);
          SgExpression& ub       = getExprID(constraint.Upper_Bound, ctx);
          SgRangeExp&   rangeExp = mkRangeExp(lb, ub);

          res = &mkAdaRangeConstraint(rangeExp);
          break;
        }

      case A_Range_Attribute_Reference:           // 3.5(2)
        {
          SgExpression& expr     = getExprID(constraint.Range_Attribute, ctx);
          SgRangeExp&   rangeExp = SG_DEREF(isSgRangeExp(&expr));

          res = &mkAdaRangeConstraint(rangeExp);
          break;
        }

      case Not_A_Constraint: /* break; */         // An unexpected element
      case A_Digits_Constraint:                   // 3.2.2: 3.5.9
      case A_Delta_Constraint:                    // 3.2.2: J.3
      case An_Index_Constraint:                   // 3.2.2: 3.6.1
      case A_Discriminant_Constraint:             // 3.2.2
      default:
        logWarn() << "Unhandled constraint: " << constraint.Constraint_Kind << std::endl;
        ROSE_ASSERT(!FAIL_ON_ERROR);
        res = &mkAdaRangeConstraint(mkRangeExp());
    }

    return SG_DEREF(res);
  }

  SgType&
  getDefinitionType(Definition_Struct& def, AstContext ctx)
  {
    SgType* res = nullptr;

    switch (def.Definition_Kind)
    {
      case A_Subtype_Indication:
        {
          Subtype_Indication_Struct& subtype   = def.The_Union.The_Subtype_Indication;

          res = &getDeclTypeID(subtype.Subtype_Mark, ctx);

          // \todo if there is no subtype constraint, shall we produce
          //       a subtype w/ NoConstraint, or leave the original type?
          if (subtype.Subtype_Constraint)
          {
            SgAdaTypeConstraint& range = getRangeConstraint(subtype.Subtype_Constraint, ctx);

            res = &mkAdaSubtype(SG_DEREF(res), range);
          }

          /* unused fields:
                bool       Has_Null_Exclusion;
          */
          break;
        }

      case A_Component_Definition:
        {
          Component_Definition_Struct& component = def.The_Union.The_Component_Definition;

          res = &getDefinitionTypeID(component.Component_Definition_View, ctx);
          break;
        }

      default:
        logWarn() << "Unhandled type definition: " << def.Definition_Kind << std::endl;
        res = sb::buildVoidType();
        ROSE_ASSERT(!FAIL_ON_ERROR);
    }

    return SG_DEREF(res);
  }

  SgClassDefinition&
  getRecordBody(Record_Definition_Struct& rec, AstContext ctx)
  {
    SgClassDefinition&        sgnode = SG_DEREF( sb::buildClassDefinition() );
    ElemIdRange               components = idRange(rec.Record_Components);
    //~ ElemIdRange               implicits  = idRange(rec.Implicit_Components);

    traverseIDs(components, elemMap(), ElemCreator{ctx.scope_npc(sgnode)});

    // how to represent implicit components
    //~ traverseIDs(implicits, elemMap(), ElemCreator{ctx.scope_npc(sgnode)});

    /* unused nodes:
         Record_Component_List Implicit_Components
    */
    return sgnode;
  }

  SgClassDefinition&
  getRecordBodyID(Element_ID recid, AstContext ctx)
  {
    Element_Struct&           elem = retrieveAs<Element_Struct>(elemMap(), recid);
    ROSE_ASSERT(elem.Element_Kind == A_Definition);

    Definition_Struct&        def = elem.The_Union.Definition;
    ROSE_ASSERT(def.Definition_Kind == A_Record_Definition);

    return getRecordBody(def.The_Union.The_Record_Definition, ctx);
  }

  SgClassDeclaration&
  getParentRecordDecl(Definition_Struct& def, AstContext ctx)
  {
    ROSE_ASSERT(def.Definition_Kind == A_Subtype_Indication);

    Subtype_Indication_Struct& subtype = def.The_Union.The_Subtype_Indication;
    ROSE_ASSERT (subtype.Subtype_Constraint == 0);

    Element_Struct&            subelem = retrieveAs<Element_Struct>(elemMap(), subtype.Subtype_Mark);
    ROSE_ASSERT(subelem.Element_Kind == An_Expression);

    SgNode*                    basenode = &getExprType(subelem.The_Union.Expression, ctx);
    SgClassDeclaration*        res = isSgClassDeclaration(basenode);

    return SG_DEREF(res);
  }

  SgClassDeclaration&
  getParentRecordDeclID(Element_ID defid, AstContext ctx)
  {
    Element_Struct&     elem = retrieveAs<Element_Struct>(elemMap(), defid);
    ROSE_ASSERT(elem.Element_Kind == A_Definition);

    return getParentRecordDecl(elem.The_Union.Definition, ctx);
  }
} // anonymous


SgType&
getDeclTypeID(Element_ID id, AstContext ctx)
{
  return getDeclType(retrieveAs<Element_Struct>(elemMap(), id), ctx);
}


SgType&
getDefinitionTypeID(Element_ID defid, AstContext ctx)
{
  if (isInvaldId(defid))
  {
    logWarn() << "undefined type id: " << defid << std::endl;
    return SG_DEREF(sb::buildVoidType());
  }

  Element_Struct&     elem = retrieveAs<Element_Struct>(elemMap(), defid);
  ROSE_ASSERT(elem.Element_Kind == A_Definition);

  return getDefinitionType(elem.The_Union.Definition, ctx);
}


TypeData
getTypeFoundation(Declaration_Struct& decl, AstContext ctx)
{
  ROSE_ASSERT( decl.Declaration_Kind == An_Ordinary_Type_Declaration );

  TypeData         res{nullptr, false, false, false};
  Element_Struct&         elem = retrieveAs<Element_Struct>(elemMap(), decl.Type_Declaration_View);
  ROSE_ASSERT(elem.Element_Kind == A_Definition);

  Definition_Struct&      def = elem.The_Union.Definition;
  ROSE_ASSERT(def.Definition_Kind == A_Type_Definition);

  /* unused fields:
     Definition_Struct
       bool                           Has_Null_Exclusion;
  */
  Type_Definition_Struct& typenode = def.The_Union.The_Type_Definition;

  switch (typenode.Type_Kind)
  {
    case A_Derived_Type_Definition:              // 3.4(2)     -> Trait_Kinds
      {
        /*
           unused fields: (derivedTypeDef)
              Declaration_List     Implicit_Inherited_Declarations;
        */

        res.n = &getDefinitionTypeID(typenode.Parent_Subtype_Indication, ctx);
        break;
      }

    case A_Derived_Record_Extension_Definition:  // 3.4(2)     -> Trait_Kinds
      {
        SgClassDefinition&  def    = getRecordBodyID(typenode.Record_Definition, ctx);
        SgClassDeclaration& basecl = getParentRecordDeclID(typenode.Parent_Subtype_Indication, ctx);
        SgBaseClass&        parent = mkRecordParent(basecl);

        def.append_inheritance(&parent);

        /*
        Declaration_List     Implicit_Inherited_Declarations;
        Declaration_List     Implicit_Inherited_Subprograms;
        Declaration          Corresponding_Parent_Subtype;
        Declaration          Corresponding_Root_Type;
        Declaration          Corresponding_Type_Structure;
        Expression_List      Definition_Interface_List;
        */
        res.n = &def;
        break;
      }

    case A_Floating_Point_Definition:            // 3.5.7(2)
      {
        SgExpression&         digits     = getExprID_opt(typenode.Digits_Expression, ctx);
        SgAdaRangeConstraint& constraint = getRangeConstraint(typenode.Real_Range_Constraint, ctx);

        res.n = &mkAdaFloatType(digits, constraint);
        break;
      }

    case An_Unconstrained_Array_Definition:      // 3.6(2)
      {
        ElemIdRange                indicesAsis = idRange(typenode.Index_Subtype_Definitions);
        std::vector<SgExpression*> indicesSeq  = traverseIDs(indicesAsis, elemMap(), ExprSeqCreator{ctx});
        SgExprListExp&             indicesAst  = SG_DEREF(sb::buildExprListExp(indicesSeq));
        SgType&                    compType    = getDefinitionTypeID(typenode.Array_Component_Definition, ctx);

        res.n = &mkArrayType(compType, indicesAst);
        /* unused fields:
        */
        break ;
      }

    case A_Tagged_Record_Type_Definition:        // 3.8(2)     -> Trait_Kinds
      {
        SgClassDefinition& def = getRecordBodyID(typenode.Record_Definition, ctx);

        //~ logInfo() << "tagged ? " << typenode.Has_Tagged << std::endl;

        /* unused fields:
              bool                 Has_Private;
              bool                 Has_Tagged;
              Declaration_List     Corresponding_Type_Operators;
        */
        res = TypeData{&def, typenode.Has_Abstract, typenode.Has_Limited, true};
        break;
      }


    case Not_A_Type_Definition: /* break; */     // An unexpected element
    case An_Enumeration_Type_Definition:         // 3.5.1(2)
    case A_Signed_Integer_Type_Definition:       // 3.5.4(3)
    case A_Modular_Type_Definition:              // 3.5.4(4)
    case A_Root_Type_Definition:                 // 3.5.4(14):  3.5.6(3)
    case An_Ordinary_Fixed_Point_Definition:     // 3.5.9(3)
    case A_Decimal_Fixed_Point_Definition:       // 3.5.9(6)
    case A_Constrained_Array_Definition:         // 3.6(2)
    case A_Record_Type_Definition:               // 3.8(2)     -> Trait_Kinds
    //  //|A2005 start
    case An_Interface_Type_Definition:           // 3.9.4      -> Interface_Kinds
    //  //|A2005 end
    case An_Access_Type_Definition:              // 3.10(2)    -> Access_Type_Kinds
    default:
      {
        logWarn() << "unhandled type kind " << typenode.Type_Kind << std::endl;
        ROSE_ASSERT(!FAIL_ON_ERROR);
        res.n = sb::buildVoidType();
      }
  }

  ROSE_ASSERT(res.n);
  return res;
}

void initializeAdaTypes(SgGlobal& global)
{
  SgAdaPackageSpec& hiddenScope = mkBareNode<SgAdaPackageSpec>();

  hiddenScope.set_parent(&global);

  adaTypes()[std::string{"Integer"}]   = sb::buildIntType();
  adaTypes()[std::string{"Character"}] = sb::buildCharType();

  // \todo items
  adaTypes()[std::string{"Float"}]     = sb::buildFloatType();  // Float is a subtype of Real
  adaTypes()[std::string{"Positive"}]  = sb::buildIntType();    // Positive is a subtype of int
  adaTypes()[std::string{"Natural"}]   = sb::buildIntType();    // Natural is a subtype of int
  adaTypes()[std::string{"Boolean"}]   = sb::buildBoolType();   // Boolean is an enumeration of True and False

  adaTypes()[std::string{"Exception"}] = sb::buildOpaqueType("Exception", &hiddenScope);
}



void ExHandlerTypeCreator::operator()(Element_Struct& elem)
{
  ROSE_ASSERT(elem.Element_Kind == An_Expression);

  Expression_Struct& asisexpr  = elem.The_Union.Expression;
  SgInitializedName* exception = isSgInitializedName(&getExprType(asisexpr, ctx));
  SgExpression&      exref     = mkExceptionRef(SG_DEREF(exception), ctx.scope());
  SgType&            extype    = mkExceptionType(exref);

  lst.push_back(&extype);
}

ExHandlerTypeCreator::operator SgType&() const
{
  ROSE_ASSERT(lst.size() > 0);

  if (lst.size() == 1)
    return SG_DEREF(lst[0]);

  return mkTypeUnion(lst);
}

}

