#include "sage3basic.h"
#include "sageBuilder.h"
#include "sageInterfaceAda.h"
#include "sageGeneric.h"

#include <numeric>

#include "AdaStatement.h"

#include "Ada_to_ROSE.h"
#include "AdaExpression.h"
#include "AdaMaker.h"
#include "AdaType.h"

// turn on all GCC warnings after include files have been processed
#pragma GCC diagnostic warning "-Wall"
#pragma GCC diagnostic warning "-Wextra"

namespace sb = SageBuilder;
namespace si = SageInterface;

namespace Ada_ROSE_Translation
{

namespace
{
  /// creates a vector of NameData objects from a sequence of Asis names.
  struct NameCreator
  {
      typedef NameData              result_type;
      typedef std::vector<NameData> result_container;

      explicit
      NameCreator(AstContext astctx)
      : ctx(astctx)
      {}

      void operator()(Element_Struct& elem)
      {
        names.push_back(getName(elem, ctx));
      }

      operator result_container() && { return std::move(names); }

    private:
      AstContext       ctx;
      result_container names;

      NameCreator() = delete;
  };

  /// converts an Asis exception handler \ref elem to ROSE
  /// and adds it to the try block \ref tryStmt.
  void handleExceptionHandler(Element_Struct& elem, SgTryStmt& tryStmt, AstContext ctx);


  /// functor to convert exception handlers
  /// \details
  ///    the created exception handlers are added to a provided
  ///    try-statement (SgTryStmt).
  struct ExHandlerCreator
  {
      ExHandlerCreator(AstContext astctx, SgTryStmt& tryStatement)
      : ctx(astctx), tryStmt(tryStatement)
      {}

      void operator()(Element_Struct& elem)
      {
        handleExceptionHandler(elem, tryStmt, ctx);
      }

    private:
      AstContext ctx;
      SgTryStmt& tryStmt;
  };



  /// retrieves the scope of a declaration node
  struct ScopeQuery : sg::DispatchHandler<SgScopeStatement*>
  {
    template <class SageDecl>
    void def(SageDecl& n) { res = n.get_definition(); }

    void handle(SgNode& n)               { SG_UNEXPECTED_NODE(n); }
    void handle(SgAdaPackageSpecDecl& n) { def(n); }
    void handle(SgAdaPackageBodyDecl& n) { def(n); }
    void handle(SgAdaGenericDecl& n)     { def(n); }
    // \todo add handlers as needed
  };


  SgScopeStatement&
  getScopeID(Element_ID& el, AstContext ctx);


  /// returns the ROSE scope of an already converted Asis element \ref elem.
  SgScopeStatement&
  getScope(Element_Struct& elem, AstContext ctx)
  {
    ADA_ASSERT (elem.Element_Kind == An_Expression);

    Expression_Struct& expr = elem.The_Union.Expression;

    if (expr.Expression_Kind == A_Selected_Component)
      //~ return getScopeID(expr.Prefix, ctx);
      return getScopeID(expr.Selector, ctx);

    if (expr.Expression_Kind != An_Identifier)
      logError() << "unexpected identifier" << expr.Expression_Kind;

    ADA_ASSERT (expr.Expression_Kind == An_Identifier);
    logKind("An_Identifier");

    SgDeclarationStatement* dcl = getDecl_opt(expr, ctx);

    if (dcl == nullptr)
    {
      logError() << "Unable to find scope/declaration for " << expr.Name_Image
                 << std::endl;
      ROSE_ABORT();
    }

    SgScopeStatement*       res = sg::dispatch(ScopeQuery(), dcl);

    return SG_DEREF(res);
  }

  SgScopeStatement&
  getScopeID(Element_ID& el, AstContext ctx)
  {
    return getScope(retrieveAs(elemMap(), el), ctx);
  }


  /// if \ref isPrivate \ref dcl's accessibility is set to private;
  /// otherwise nothing.
  void
  privatize(SgDeclarationStatement& dcl, bool isPrivate)
  {
    if (!isPrivate) return;

    dcl.get_declarationModifier().get_accessModifier().setPrivate();
  }

  SgLabelStatement&
  labelStmt(SgStatement& stmt, std::string lblname, Defining_Name_ID lblid, AstContext ctx)
  {
    ADA_ASSERT (lblid > 0);

    SgNode&           parent  = SG_DEREF(stmt.get_parent());
    SgLabelStatement& sgn     = mkLabelStmt(lblname, stmt, ctx.scope());
    Element_Struct&   lblelem = retrieveAs(elemMap(), lblid);

    //~ copyFileInfo(stmt, sgn);
    attachSourceLocation(sgn, lblelem, ctx);
    sgn.set_parent(&parent);
    ctx.labelsAndLoops().label(lblid, sgn);

    ADA_ASSERT (stmt.get_parent() == &sgn);
    return sgn;
  }

  std::string
  getLabelName(Element_ID el, AstContext ctx)
  {
    return getNameID(el, ctx).fullName;
  }

  /// labels a statement with a block label or a sequence of labels (if needed)
  /// @{
  template <class SageAdaStmt>
  SgStatement&
  labelIfNeeded(SageAdaStmt& stmt, Defining_Name_ID lblid, AstContext ctx)
  {
    ADA_ASSERT (lblid >= 0);

    if (!lblid) return stmt;

    std::string lblname = getLabelName(lblid, ctx);

    stmt.set_string_label(lblname);
    return labelStmt(stmt, lblname, lblid, ctx);
  }

  SgStatement&
  labelIfNeeded(SgStatement& stmt, Statement_Struct& adastmt, AstContext ctx)
  {
    typedef NameCreator::result_container name_container;

    ElemIdRange    range  = idRange(adastmt.Label_Names);
    name_container names  = allNames(range, ctx);
    SgStatement*   sgnode = std::accumulate( names.rbegin(), names.rend(),
                                             &stmt,
                                             [&](SgStatement* labeled, const NameData& el) -> SgStatement*
                                             {
                                               ADA_ASSERT (el.fullName == el.ident);
                                               return &labelStmt(SG_DEREF(labeled), el.fullName, el.id(), ctx);
                                             }
                                           );

    return SG_DEREF(sgnode);
  }

  /// @}


  /// completes statements by setting source locations, parent node,
  /// adding labels (if needed)...
  /// @{
  void
  completeStmt(SgStatement& sgnode, Element_Struct& elem, AstContext ctx)
  {
    ADA_ASSERT (elem.Element_Kind == A_Statement);

    attachSourceLocation(sgnode, elem, ctx);
    sgnode.set_parent(&ctx.scope());

    Statement_Struct& stmt = elem.The_Union.Statement;
    SgStatement&      sgn  = labelIfNeeded(sgnode, stmt, ctx);

    ctx.scope().append_statement(&sgn);
    ADA_ASSERT (sgn.get_parent() == &ctx.scope());
  }

  template <class SageScopeStmt>
  void
  completeStmt(SageScopeStmt& sgnode, Element_Struct& elem, AstContext ctx, Defining_Name_ID lblid)
  {
    ADA_ASSERT (elem.Element_Kind == A_Statement);

    attachSourceLocation(sgnode, elem, ctx);
    sgnode.set_parent(&ctx.scope());

    SgStatement&      sgn0 = labelIfNeeded(sgnode, lblid, ctx);
    Statement_Struct& stmt = elem.The_Union.Statement;
    SgStatement&      sgn  = labelIfNeeded(sgn0, stmt, ctx);

    ctx.scope().append_statement(&sgn);
    ADA_ASSERT (sgn.get_parent() == &ctx.scope());
  }

  /// @}

  /// gets the body of a function declaration \ref defdcl
  /// \pre defdcl is the defining declaration
  SgBasicBlock& getFunctionBody(SgFunctionDeclaration& defdcl)
  {
    SgFunctionDefinition* def = isSgFunctionDefinition(defdcl.get_definition());

    return SG_DEREF(SG_DEREF(def).get_body());
  }

  /// sets the override flag in \ref sgmod
  void setOverride(SgDeclarationModifier& sgmod, bool isOverride)
  {
    if (isOverride)
      sgmod.setOverride();
    else
      sgmod.unsetOverride();
  }

  /// creates a deep-copyW of \ref exp
  /// if \ref exp is null, null will be returned
  SgExpression*
  cloneIfNeeded(SgExpression* exp, bool required)
  {
    if (!required) return exp;

    exp = si::deepCopy(exp);

    // \todo use a traversal to set all children nodes to compiler generated
    markCompilerGenerated(SG_DEREF(exp));
    return exp;
  }


  /// creates a sequence of initialized names for all names \ref names.
  /// \param m        a map that maintains mappings between Asis IDs and variables/parameters
  /// \param names    the list of Asis names
  /// \param dcltype  the type of all initialized name
  /// \param initexpr the initializer (if it exists) that will be cloned for each
  ///                 of the initialized names.
  SgInitializedNamePtrList
  constructInitializedNamePtrList( AstContext ctx,
                                   std::map<int, SgInitializedName*>& m,
                                   const NameCreator::result_container& names,
                                   SgType& dcltype,
                                   SgExpression* initexpr
                                 )
  {
    SgInitializedNamePtrList lst;
    int                      num      = names.size();

    for (int i = 0; i < num; ++i)
    {
      const NameCreator::result_container::value_type obj = names.at(i);

      ADA_ASSERT (obj.fullName == obj.ident);

      const std::string& name = obj.fullName;
      Element_ID         id   = obj.id();
      SgInitializedName& dcl  = mkInitializedName(name, dcltype, cloneIfNeeded(initexpr, initexpr && (i != 0)));

      attachSourceLocation(dcl, retrieveAs(elemMap(), id), ctx);

      lst.push_back(&dcl);
      recordNonUniqueNode(m, id, dcl, true /* overwrite existing entries if needed */);
    }

    return lst;
  }


  /// converts a parameter mode to its ROSE representation
  SgTypeModifier
  getMode(Mode_Kinds asisKind)
  {
    SgTypeModifier res;

    switch (asisKind)
    {
      case A_Default_In_Mode:
        logKind("A_Default_In_Mode");
        res.setDefault();
        break;

      case An_In_Mode:
        logKind("An_In_Mode");
        res.setIntent_in();
        break;

      case An_Out_Mode:
        logKind("An_Out_Mode");
        res.setIntent_out();
        break;

      case An_In_Out_Mode:
        logKind("An_In_Out_Mode");
        res.setIntent_inout();
        break;

      case Not_A_Mode: /* break; */
      default:
        ROSE_ABORT();
    }

    return res;
  }



  /// creates a ROSE expression for an Asis declaration's initializer expression
  /// returns null, if no declaration exists.
  /// \param expectedType a type that is carried over from a lhs constant declaration
  SgExpression*
  getVarInit(Declaration_Struct& decl, SgType* /*expectedType*/, AstContext ctx)
  {
    if (decl.Declaration_Kind == A_Deferred_Constant_Declaration)
      return nullptr;

    ADA_ASSERT (  decl.Declaration_Kind == A_Variable_Declaration
               || decl.Declaration_Kind == A_Constant_Declaration
               || decl.Declaration_Kind == A_Parameter_Specification
               || decl.Declaration_Kind == A_Real_Number_Declaration
               || decl.Declaration_Kind == An_Integer_Number_Declaration
               || decl.Declaration_Kind == A_Component_Declaration
               || decl.Declaration_Kind == A_Discriminant_Specification
               || decl.Declaration_Kind == A_Discriminant_Specification
               || decl.Declaration_Kind == A_Formal_Object_Declaration
               );

    //~ logWarn() << "decl.Initialization_Expression = " << decl.Initialization_Expression << std::endl;
    if (decl.Initialization_Expression == 0)
      return nullptr;

    SgExpression& res = getExprID_opt(decl.Initialization_Expression, ctx);

    //  \todo this fails ...
    //        discuss with DQ expression types
    //~ if (expectedType) res.set_explicitly_stored_type(expectedType);
    return &res;
  }

  SgType&
  getVarType(Declaration_Struct& decl, AstContext ctx)
  {
    ADA_ASSERT (  decl.Declaration_Kind == A_Variable_Declaration
               || decl.Declaration_Kind == A_Constant_Declaration
               || decl.Declaration_Kind == A_Component_Declaration
               || decl.Declaration_Kind == A_Deferred_Constant_Declaration
               );

    return getDefinitionTypeID(decl.Object_Declaration_View, ctx);
  }

  Element_ID getLabelRef(Element_ID id, AstContext ctx)
  {
    Element_Struct&    labelref = retrieveAs(elemMap(), id);
    ADA_ASSERT (labelref.Element_Kind == An_Expression);

    Expression_Struct& labelexp = labelref.The_Union.Expression;
    ADA_ASSERT (labelexp.Expression_Kind == An_Identifier);

    logKind("An_Identifier");
    return labelexp.Corresponding_Name_Definition;
  }


  /// converts an Asis parameter declaration to a ROSE paramter (i.e., variable)
  ///   declaration.
  SgVariableDeclaration&
  getParm(Element_Struct& elem, AstContext ctx)
  {
    typedef NameCreator::result_container name_container;

    ADA_ASSERT (elem.Element_Kind == A_Declaration);

    Declaration_Struct&      asisDecl = elem.The_Union.Declaration;
    ADA_ASSERT (  asisDecl.Declaration_Kind == A_Parameter_Specification
              || asisDecl.Declaration_Kind == A_Formal_Object_Declaration
              );

    ElemIdRange              range    = idRange(asisDecl.Names);
    name_container           names    = allNames(range, ctx);
    SgType&                  basety   = getDeclTypeID(asisDecl.Object_Declaration_View, ctx);
    const bool               aliased  = (asisDecl.Declaration_Kind == A_Parameter_Specification) && asisDecl.Has_Aliased;
    SgType&                  parmtype = aliased ? mkAliasedType(basety) : basety;
    SgInitializedNamePtrList dclnames = constructInitializedNamePtrList( ctx, asisVars(), names,
                                                                         parmtype, getVarInit(asisDecl, &parmtype, ctx)
                                                                       );
    SgVariableDeclaration&   sgnode   = mkParameter(dclnames, getMode(asisDecl.Mode_Kind), ctx.scope());

    attachSourceLocation(sgnode, elem, ctx);
    /* unused fields:
         bool                           Has_Null_Exclusion;
    */
    return sgnode;
  }


  /// adds all parameters from a sequence of Asis parameters to a
  ///   parameter list (either function, procedure, or accept)
  struct ParmlistCreator
  {
      ParmlistCreator(SgFunctionParameterList& parms, AstContext astctx)
      : parmlist(parms), ctx(astctx)
      {}

      // CallableDeclaration is either derived from function declaration,
      // or an accept statement.
      template <class CallableDeclaration>
      ParmlistCreator(CallableDeclaration& callable, AstContext astctx)
      : ParmlistCreator(SG_DEREF(callable.get_parameterList()), astctx)
      {}

      void operator()(Element_Struct& elem)
      {
        SgVariableDeclaration&    decl = getParm(elem, ctx);
        SgInitializedNamePtrList& args = parmlist.get_args();

        // in Ada multiple parameters can be declared
        //   within a single declaration.
        for (SgInitializedName* parm : decl.get_variables())
        {
          parm->set_parent(&parmlist);
          args.push_back(parm);
        }
      }

    private:
      SgFunctionParameterList& parmlist;
      AstContext               ctx;
  };

  /// converts an Asis parameter declaration to a ROSE paramter (i.e., variable)
  ///   declaration.
  SgVariableDeclaration&
  getDiscriminant(Element_Struct& elem, AstContext ctx)
  {
    typedef NameCreator::result_container name_container;

    ADA_ASSERT (elem.Element_Kind == A_Declaration);

    Declaration_Struct&      asisDecl = elem.The_Union.Declaration;
    ADA_ASSERT (asisDecl.Declaration_Kind == A_Discriminant_Specification);

    // SgType&                   dcltype = tyModifier(getVarType(decl, ctx));
    ElemIdRange              range    = idRange(asisDecl.Names);
    name_container           names    = allNames(range, ctx);
    SgType&                  basety   = getDeclTypeID(asisDecl.Object_Declaration_View, ctx);
    SgInitializedNamePtrList dclnames = constructInitializedNamePtrList( ctx, asisVars(), names,
                                                                         basety, getVarInit(asisDecl, &basety, ctx)
                                                                       );
    SgVariableDeclaration&   sgnode   = mkVarDecl(dclnames, ctx.scope());

    attachSourceLocation(sgnode, elem, ctx);
    /* unused fields:
         bool                           Has_Null_Exclusion;
    */
    return sgnode;
  }


  struct DiscriminantCreator
  {
      DiscriminantCreator(SgAdaDiscriminatedTypeDecl& discrNode, AstContext astctx)
      : sgnode(discrNode), ctx(astctx)
      {}

      void operator()(Element_Struct& elem)
      {
        SgVariableDeclaration&    decl = getDiscriminant(elem, ctx);

        // SgDeclarationScope does not hold a list of declarations..
        // ctx.scope().append_statement(&decl);
        // alternative: store them with the discriminated decl node

        SgInitializedNamePtrList& args = sgnode.get_discriminants();

        // in Ada multiple parameters can be declared
        //   within a single declaration.
        for (SgInitializedName* discriminant : decl.get_variables())
        {
          args.push_back(discriminant);
        }
      }

    private:
      SgAdaDiscriminatedTypeDecl& sgnode;
      AstContext                  ctx;

      DiscriminantCreator() = delete;
  };

  struct VariantCreator
  {
      VariantCreator(AstContext astctx)
      : ctx(astctx)
      {}

      void operator()(Element_Struct& elem)
      {
        ADA_ASSERT (elem.Element_Kind == A_Definition);
        Definition_Struct& def = elem.The_Union.Definition;

        ADA_ASSERT (def.Definition_Kind == A_Variant);
        Variant_Struct&    variant = def.The_Union.The_Variant;
        ElemIdRange        range = idRange(variant.Record_Components);

        traverseIDs(range, elemMap(), ElemCreator{ ctx.variantChoice(variant.Variant_Choices) });

        /* unused fields:
           Record_Component_List Implicit_Components
        */
      }

    private:
      AstContext ctx;

      VariantCreator() = delete;
  };

  SgClassDeclaration&
  createRecordDecl( const std::string& name,
                    SgClassDefinition& def,
                    SgScopeStatement& scope,
                    SgDeclarationStatement* nondefdcl
                  )
  {
    if (SgClassDeclaration* recdcl = isSgClassDeclaration(nondefdcl))
      return mkRecordDecl(*recdcl, def, scope);

    // if nondefdcl is set, it must be a SgClassDeclaration
    if (nondefdcl)
      logError() << name << " " << typeid(*nondefdcl).name() << std::endl;

    ADA_ASSERT (!nondefdcl);
    return mkRecordDecl(name, def, scope);
  }

  void
  setModifiers(SgDeclarationStatement& dcl, bool abstract, bool limited, bool tagged)
  {
    SgDeclarationModifier& mod = dcl.get_declarationModifier();

    if (abstract) mod.setAdaAbstract();
    if (limited)  mod.setAdaLimited();
    if (tagged)   mod.setAdaTagged();

    //~ logError() << typeid(dcl).name() << " " << abstract << limited << tagged
               //~ << std::endl;
  }

  void
  setModifiers(SgDeclarationStatement& dcl, const TypeData& info)
  {
    setModifiers(dcl, info.isAbstract(), info.isLimited(), info.isTagged());
  }

  template <class AsisStruct>
  void
  setModifiers(SgDeclarationStatement& dcl, const AsisStruct& info)
  {
    setModifiers(dcl, info.Has_Abstract, info.Has_Limited, info.Has_Tagged);
  }

  template <class AsisStruct>
  void
  setModifiersUntagged(SgDeclarationStatement& dcl, const AsisStruct& info)
  {
    setModifiers(dcl, info.Has_Abstract, info.Has_Limited, false);
  }

  /// creates a ROSE declaration depending on the provided type/definition
  struct TypeDeclMaker : sg::DispatchHandler<SgDeclarationStatement*>
  {
      typedef sg::DispatchHandler<SgDeclarationStatement*> base;

      TypeDeclMaker( const std::string& name,
                     SgScopeStatement& scope,
                     TypeData basis,
                     SgDeclarationStatement* incompl
                   )
      : base(nullptr), dclname(name), dclscope(scope), foundation(basis), incomplDecl(incompl)
      {}

      void handle(SgNode& n) { SG_UNEXPECTED_NODE(n); }

      void handle(SgAdaFormalTypeDecl& n)
      {
        ADA_ASSERT (n.get_scope() == &dclscope);

        res = &n;
      }

      void handle(SgType& n)
      {
        res = &mkTypeDecl(dclname, n, dclscope);

        if (incomplDecl)
        {
          //~ logError() << dclname << " " << typeid(*res).name() << typeid(*incomplDecl).name()
                     //~ << std::endl;
          res->set_firstNondefiningDeclaration(incomplDecl);
          res->set_definingDeclaration(res);
          incomplDecl->set_definingDeclaration(res);
        }
      }

      void handle(SgAdaDerivedType& n)
      {
        SgEnumDeclaration* enmdcl = isSgEnumDeclaration(si::ada::baseDeclaration(n));

        if (enmdcl == nullptr)
        {
          handle(sg::asBaseType(n));
          return;
        }

        SgEnumDeclaration& derivedEnum = mkEnumDefn(dclname, dclscope);

        derivedEnum.set_adaParentType(&n);
        res = &derivedEnum;
      }

      void handle(SgEnumDeclaration& n)
      {
        ADA_ASSERT (n.get_scope() == &dclscope);

        res = &n;
      }

      void handle(SgClassDefinition& n)
      {
        SgClassDeclaration& rec = createRecordDecl(dclname, n, dclscope, incomplDecl);

        setModifiers(rec, foundation);
        res = &rec;
      }

      operator SgDeclarationStatement& () const
      {
        return SG_DEREF(res);
      }

    private:
      std::string             dclname;
      SgScopeStatement&       dclscope;
      TypeData                foundation;
      SgDeclarationStatement* incomplDecl;
  };


  using TypeModifierFn = std::function<SgType&(SgType&)>;

  SgType& tyIdentity(SgType& ty) { return ty; }

  //
  // helper function for combined handling of variables and constant declarations

  using VarMakerFn = std::function<SgVariableDeclaration&(const SgInitializedNamePtrList&, SgScopeStatement&)>;

  SgVariableDeclaration&
  varMaker_default(const SgInitializedNamePtrList& vars, SgScopeStatement& scope)
  {
    return mkVarDecl(vars, scope);
  }

  void
  handleNumVarCstDecl( Element_Struct& elem,
                       Declaration_Struct& decl,
                       AstContext ctx,
                       bool isPrivate,
                       SgType& dclType,
                       SgType* expectedType = nullptr,
                       VarMakerFn varMaker = varMaker_default
                     )
  {
    typedef NameCreator::result_container name_container;

    ElemIdRange              range    = idRange(decl.Names);
    name_container           names    = allNames(range, ctx);
    SgScopeStatement&        scope    = ctx.scope();
    SgInitializedNamePtrList dclnames = constructInitializedNamePtrList( ctx, asisVars(), names,
                                                                         dclType, getVarInit(decl, expectedType, ctx)
                                                                       );
    SgVariableDeclaration&   vardcl   = varMaker(dclnames, scope);

    attachSourceLocation(vardcl, elem, ctx);
    privatize(vardcl, isPrivate);
    scope.append_statement(&vardcl);

    ADA_ASSERT (vardcl.get_parent() == &scope);
  }

  void
  handleNumberDecl(Element_Struct& elem, Declaration_Struct& decl, AstContext ctx, bool isPrivate, SgType& numty, SgType& expctty)
  {
    SgType& cstty = SG_DEREF( sb::buildConstType(&numty) );

    handleNumVarCstDecl(elem, decl, ctx, isPrivate, cstty, &expctty);
  }

  void
  handleVarCstDecl( Element_Struct& elem,
                    Declaration_Struct& dcl,
                    AstContext ctx,
                    bool isPrivate,
                    TypeModifierFn constMaker,
                    VarMakerFn varMaker = varMaker_default
                  )
  {
    SgType& basety = constMaker(getVarType(dcl, ctx));
    SgType& varty  = dcl.Has_Aliased ? mkAliasedType(basety) : basety;

    handleNumVarCstDecl(elem, dcl, ctx, isPrivate, varty, nullptr, varMaker);
  }

  SgDeclarationStatement&
  getAliasedID(Element_ID declid, AstContext ctx);

  SgDeclarationStatement&
  getAliased(Expression_Struct& expr, AstContext ctx)
  {
    if (expr.Expression_Kind == An_Identifier)
    {
      logKind("An_Identifier");
      return SG_DEREF(getDecl_opt(expr, ctx));
    }

    ADA_ASSERT (expr.Expression_Kind == A_Selected_Component);
    logKind("A_Selected_Component");
    return getAliasedID(expr.Selector, ctx);
  }

  SgDeclarationStatement&
  getAliasedID(Element_ID declid, AstContext ctx)
  {
    Element_Struct& elem = retrieveAs(elemMap(), declid);
    ADA_ASSERT (elem.Element_Kind == An_Expression);

    return getAliased(elem.The_Union.Expression, ctx);
  }


  std::pair<SgAdaTaskSpec*, DeferredBodyCompletion>
  getTaskSpec(Element_Struct& elem, AstContext ctx)
  {
    ADA_ASSERT (elem.Element_Kind == A_Definition);
    logKind("A_Definition");

    Definition_Struct&      def = elem.The_Union.Definition;
    ADA_ASSERT (def.Definition_Kind == A_Task_Definition);

    logKind("A_Task_Definition");

    Task_Definition_Struct* tasknode = &def.The_Union.The_Task_Definition;
    SgAdaTaskSpec&          sgnode   = mkAdaTaskSpec();
    SgAdaTaskSpec*          nodePtr  = &sgnode;

    sgnode.set_hasMembers(true);
    sgnode.set_hasPrivate(tasknode->Is_Private_Present);

    auto deferred = [ctx,nodePtr,tasknode]() -> void
                    {
                      // visible items
                      {
                        ElemIdRange range = idRange(tasknode->Visible_Part_Items);

                        traverseIDs(range, elemMap(), ElemCreator{ctx.scope(*nodePtr)});
                      }

                      // private items
                      {
                        ElemIdRange range = idRange(tasknode->Private_Part_Items);
                        ADA_ASSERT ((!range.empty()) == tasknode->Is_Private_Present);

                        traverseIDs(range, elemMap(), ElemCreator{ctx.scope(*nodePtr), true /* private items */});
                      }
                    };

    /* unused fields: (Task_Definition_Struct)
         bool                  Has_Task;
         Declaration_List      Corresponding_Type_Operators;
    */
    return std::make_pair(&sgnode, deferred);
  }

  std::pair<SgAdaTaskSpec*, DeferredBodyCompletion>
  getTaskSpecID_opt(Element_ID id, AstContext ctx)
  {
    if (id == 0)
      return std::make_pair(&mkAdaTaskSpec(), []()->void {}); // nothing to complete

    return getTaskSpec(retrieveAs(elemMap(), id), ctx);
  }

  std::pair<SgAdaTaskSpec*, DeferredBodyCompletion>
  getTaskSpecForTaskType(Declaration_Struct& decl, AstContext ctx)
  {
    ADA_ASSERT (decl.Declaration_Kind == A_Task_Type_Declaration);

    return getTaskSpecID_opt(decl.Type_Declaration_View, ctx);
  }

  std::pair<SgAdaTaskSpec*, DeferredBodyCompletion>
  getTaskSpecForSingleTask(Declaration_Struct& decl, AstContext ctx)
  {
    ADA_ASSERT (decl.Declaration_Kind == A_Single_Task_Declaration);

    return getTaskSpecID_opt(decl.Object_Declaration_View, ctx);
  }

  //
  // protected objects
  //
  // protected objects are represented as ClassDeclaration and ClassDefinition

  std::pair<SgAdaProtectedSpec*, DeferredBodyCompletion>
  getProtectedSpec(Element_Struct& elem, AstContext ctx)
  {

    ADA_ASSERT (elem.Element_Kind == A_Definition);
    logKind("A_Definition");

    Definition_Struct&      def = elem.The_Union.Definition;
    ADA_ASSERT (def.Definition_Kind == A_Protected_Definition);

    logKind("A_Protected_Definition");

    Protected_Definition_Struct* protectedNode = &def.The_Union.The_Protected_Definition;
    SgAdaProtectedSpec&     sgnode = mkAdaProtectedSpec();
    SgAdaProtectedSpec*     nodePtr  = &sgnode;

    sgnode.set_hasPrivate(protectedNode->Is_Private_Present);

    auto deferred = [ctx,nodePtr,protectedNode]() -> void
                    {
                      // visible items
                      {
                        ElemIdRange range = idRange(protectedNode->Visible_Part_Items);

                        traverseIDs(range, elemMap(), ElemCreator{ctx.scope(*nodePtr)});
                      }

                      // private items
                      {
                        ElemIdRange range = idRange(protectedNode->Private_Part_Items);
                        ADA_ASSERT ((!range.empty()) == protectedNode->Is_Private_Present);

                        traverseIDs(range, elemMap(), ElemCreator{ctx.scope(*nodePtr), true /* private items */});
                      }
                    };

    /* unused fields: (Protected_Definition_Struct)
         bool                  Has_Protected;
         Declaration_List      Corresponding_Type_Operators;
    */
    return std::make_pair(&sgnode, deferred);
  }

  std::pair<SgAdaProtectedSpec*, DeferredBodyCompletion>
  getProtectedSpecID(Element_ID id, AstContext ctx)
  {
    return getProtectedSpec(retrieveAs(elemMap(), id), ctx);
  }

  std::pair<SgAdaProtectedSpec*, DeferredBodyCompletion>
  getProtectedSpecForProtectedType(Declaration_Struct& decl, AstContext ctx)
  {
    ADA_ASSERT (decl.Declaration_Kind == A_Protected_Type_Declaration);

    if (decl.Type_Declaration_View == 0)
      return std::make_pair(&mkAdaProtectedSpec(), []()->void {}); // nothing to complete

    return getProtectedSpecID(decl.Type_Declaration_View, ctx);
  }


  std::pair<SgAdaProtectedSpec*, DeferredBodyCompletion>
  getProtectedSpecForSingleProtected(Declaration_Struct& decl, AstContext ctx)
  {
    ADA_ASSERT (decl.Declaration_Kind == A_Single_Protected_Declaration);

    return getProtectedSpecID(decl.Object_Declaration_View, ctx);
  }


  struct CaseStmtCreator
  {
      typedef std::pair<SgExpression*, SgStatement*> branch_type;
      typedef std::vector<branch_type>               branch_container;

      CaseStmtCreator(AstContext astctx, SgSwitchStatement& caseStmt)
      : ctx(astctx), caseNode(caseStmt)
      {}

      bool isWhenOthers(ElemIdRange choices)
      {
        ADA_ASSERT (!choices.empty());

        if (choices.size() > 1) return false;

        Element_Struct& el = retrieveAs(elemMap(), *choices.first);

        return (  el.Element_Kind == A_Definition
               && el.The_Union.Definition.Definition_Kind == An_Others_Choice
               );
      }

      void operator()(Element_Struct& elem)
      {
        Path_Struct& path = elem.The_Union.Path;

        switch (path.Path_Kind)
        {
          case A_Case_Path:
            {
              logKind("A_Case_Path");

              SgStatement*  sgnode      = nullptr;
              SgBasicBlock& block       = mkBasicBlock();
              ElemIdRange   caseChoices = idRange(path.Case_Path_Alternative_Choices);

              if (isWhenOthers(caseChoices))
              {
                logKind("An_Others_Choice");

                SgDefaultOptionStmt* whenOthersNode = &mkWhenOthersPath(block);

                caseNode.append_default(whenOthersNode);
                sgnode = whenOthersNode;
              }
              else
              {
                std::vector<SgExpression*> choices   = traverseIDs(caseChoices, elemMap(), ExprSeqCreator{ctx});

                // \todo reconsider the "reuse" of SgCommaOp
                //   SgCommaOp is only used to separate discrete choices in case-when
                ADA_ASSERT (choices.size());
                SgExpression&              caseCond  = mkChoiceExpIfNeeded(std::move(choices));
                SgCaseOptionStmt*          whenNode  = &mkWhenPath(caseCond, block);

                caseNode.append_case(whenNode);
                sgnode = whenNode;
              }

              ADA_ASSERT (sgnode);
              ElemIdRange caseBlock = idRange(path.Sequence_Of_Statements);

              traverseIDs(caseBlock, elemMap(), StmtCreator{ctx.scope(block)});
              attachSourceLocation(*sgnode, elem, ctx);
              break;
            }

          default:
            ROSE_ABORT();
        }
      }

    private:
      AstContext         ctx;
      SgSwitchStatement& caseNode; // \todo consider introducing SgAdaCaseStmt
  };

  struct IfStmtCreator
  {
      IfStmtCreator(SgIfStmt& sgnode, AstContext astctx)
      : ifStmt(&sgnode), ctx(astctx)
      {}

      void commonBranch(Path_Struct& path, void (SgIfStmt::*branchSetter)(SgStatement*))
      {
        ElemIdRange   thenStmts = idRange(path.Sequence_Of_Statements);
        SgBasicBlock& block     = mkBasicBlock();

        sg::linkParentChild(SG_DEREF(ifStmt), static_cast<SgStatement&>(block), branchSetter);
        traverseIDs(thenStmts, elemMap(), StmtCreator{ctx.scope(block)});
      }

      void conditionedBranch(Path_Struct& path)
      {
        SgExpression& condExpr = getExprID(path.Condition_Expression, ctx);
        SgStatement&  condStmt = mkExprStatement(condExpr);

        sg::linkParentChild(SG_DEREF(ifStmt), condStmt, &SgIfStmt::set_conditional);
        commonBranch(path, &SgIfStmt::set_true_body);
      }

      void operator()(Element_Struct& elem)
      {
        Path_Struct& path = elem.The_Union.Path;

        switch (path.Path_Kind)
        {
          case An_If_Path:
            {
              logKind("An_If_Path");
              ADA_ASSERT (ifStmt);
              conditionedBranch(path);
              break;
            }

          case An_Elsif_Path:
            {
              logKind("An_Elsif_Path");
              ADA_ASSERT (ifStmt);

              SgIfStmt& cascadingIf = mkIfStmt();

              sg::linkParentChild( SG_DEREF(ifStmt),
                                   static_cast<SgStatement&>(cascadingIf),
                                   &SgIfStmt::set_false_body
                                 );
              ifStmt = &cascadingIf;
              conditionedBranch(path);
              break;
            }

          case An_Else_Path:
            {
              logKind("An_Else_Path");
              ADA_ASSERT (ifStmt);
              commonBranch(path, &SgIfStmt::set_false_body);
              break;
            }

          default:
            ROSE_ABORT();
        }
      }

    private:
      SgIfStmt*        ifStmt;
      AstContext       ctx;

      IfStmtCreator() = delete;
  };

  // MS 11/17/2020 : builders not in sageBuilder (yet)
  SgAdaSelectAlternativeStmt* buildAdaSelectAlternativeStmt(SgExpression *guard,
                                                            SgBasicBlock *body)
  {
    ADA_ASSERT (body);

    SgAdaSelectAlternativeStmt *stmt =
      new SgAdaSelectAlternativeStmt();
    ADA_ASSERT (stmt);

    stmt->set_guard(guard);
    stmt->set_body(body);

    body->set_parent(stmt);
    guard->set_parent(stmt);

    markCompilerGenerated(*stmt);
    return stmt;
  }

  SgAdaSelectStmt& mkAdaSelectStmt(SgAdaSelectStmt::select_type_enum select_type)
  {
    SgAdaSelectStmt *stmt = new SgAdaSelectStmt;
    ADA_ASSERT (stmt);

    stmt->set_select_type(select_type);

    markCompilerGenerated(*stmt);
    return *stmt;
  }

  // MS 11/17/2020 : SelectStmtCreator modeled on IfStmtCreator
  // PP 10/12/2021 : modified code to eliminate the need for using scope_npc.
  //                 A block will only be populated after the new node has been connected
  //                 to the AST. This is achieved by returning a lambda function w/o parameters
  //                 (DeferredBodyCompletion) that is invoked after the node has been connected
  //                 to the AST.
  struct SelectStmtCreator
  {
    typedef SgAdaSelectAlternativeStmt*  alternative;
    typedef std::vector<alternative>     alternative_container;

    SelectStmtCreator(SgAdaSelectStmt& sgn, AstContext astctx)
      : sgnode(&sgn), ctx(astctx)
    {}

    std::pair<SgAdaSelectAlternativeStmt*, DeferredBodyCompletion>
    commonAltStmt(Path_Struct& path) {
      // create body of alternative
      SgBasicBlock* block = &mkBasicBlock();

      // create guard
      SgExpression* guard = &getExprID_opt(path.Guard, ctx);

      // instantiate SgAdaSelectAlternativeStmt node and return it
      SgAdaSelectAlternativeStmt* stmt = buildAdaSelectAlternativeStmt(guard, block);
      Path_Struct* pathptr = &path;
      AstContext   astctx{ctx};

      auto completion = [pathptr, block, astctx]() -> void
                        {
                          ElemIdRange altStmts = idRange(pathptr->Sequence_Of_Statements);

                          traverseIDs(altStmts, elemMap(), StmtCreator{astctx.scope(*block)});
                        };

      return std::make_pair(stmt, completion);
    }

    std::pair<SgBasicBlock*, DeferredBodyCompletion>
    commonMakeBlock(Path_Struct& path) {
      SgBasicBlock* block   = &mkBasicBlock();
      Path_Struct*  pathptr = &path;
      AstContext    astctx{ctx};

      auto completion = [pathptr, block, astctx]() -> void
                        {
                          ElemIdRange altStmts = idRange(pathptr->Sequence_Of_Statements);

                          traverseIDs(altStmts, elemMap(), StmtCreator{astctx.scope(*block)});
                        };
      return std::make_pair(block, completion);
    }

    void orAlternative(Path_Struct& path)
    {
      ADA_ASSERT (path.Path_Kind == An_Or_Path);

      auto alt = commonAltStmt(path);

      if (currOrPath == nullptr)
      {
        ADA_ASSERT (  (sgnode->get_select_type() == SgAdaSelectStmt::e_selective_accept)
                  || (sgnode->get_select_type() == SgAdaSelectStmt::e_timed_entry)
                  );

        sg::linkParentChild(*sgnode, *(alt.first), &SgAdaSelectStmt::set_or_path);
      }
      else
      {
        ADA_ASSERT (sgnode->get_select_type() == SgAdaSelectStmt::e_selective_accept);

        sg::linkParentChild(*currOrPath, *(alt.first), &SgAdaSelectAlternativeStmt::set_next);
      }

      currOrPath = alt.first;
      alt.second();
    }

    void selectAlternative(Path_Struct& path)
    {
      ADA_ASSERT (path.Path_Kind == A_Select_Path);
      auto alt = commonAltStmt(path);

      sg::linkParentChild(*sgnode, *(alt.first), &SgAdaSelectStmt::set_select_path);
      alt.second();
    }

    void elseAlternative(Path_Struct& path)
    {
      ADA_ASSERT (path.Path_Kind == An_Else_Path);
      ADA_ASSERT (  (sgnode->get_select_type() == SgAdaSelectStmt::e_selective_accept)
                || (sgnode->get_select_type() == SgAdaSelectStmt::e_conditional_entry)
                );

      auto alt = commonMakeBlock(path);

      sg::linkParentChild(*sgnode, *(alt.first), &SgAdaSelectStmt::set_else_path);
      alt.second();
    }

    void abortAlternative(Path_Struct& path)
    {
      ADA_ASSERT (path.Path_Kind == A_Then_Abort_Path);
      ADA_ASSERT (sgnode->get_select_type() == SgAdaSelectStmt::e_asynchronous);

      auto alt = commonMakeBlock(path);

      sg::linkParentChild(*sgnode, *(alt.first), &SgAdaSelectStmt::set_abort_path);
      alt.second();
    }

    void operator()(Element_Struct& elem)
    {
      Path_Struct& path = elem.The_Union.Path;

      switch (path.Path_Kind)
        {
        case A_Select_Path:
          {
            selectAlternative(path);
            break;
          }

        case An_Or_Path:
          {
            orAlternative(path);
            break;
          }

        case An_Else_Path:
          {
            elseAlternative(path);
            break;
          }

        case A_Then_Abort_Path:
          {
            abortAlternative(path);
            break;
          }

        default:
          ROSE_ABORT();
        }
    }


#if 0
    SgAdaSelectAlternativeStmt*
    chainOr()
    {
      SgAdaSelectAlternativeStmt *cur = NULL;

      for (SgAdaSelectAlternativeStmt* s : or_paths) {
        if (cur == NULL)
          {
            cur = s;
          }
        else
          {
            cur->set_next(s);
            s->set_parent(cur);
            cur = s;
          }
      }

      return or_paths.front();
    }


    operator SgStatement&()
    {
      sgnode->set_select_type(ty);
      switch (ty)
        {
        case SgAdaSelectStmt::e_selective_accept:
          {
            sgnode->set_select_path(select_path);
            select_path->set_parent(sgnode);

            SgAdaSelectAlternativeStmt* orRoot = chainOr();
            sgnode->set_or_path(orRoot);
            orRoot->set_parent(sgnode);
            sgnode->set_else_path(else_path);
            if (else_path != nullptr) {
              else_path->set_parent(sgnode);
            }
            break;
          }

        case SgAdaSelectStmt::e_timed_entry:

          sgnode->set_select_path(select_path);
          select_path->set_parent(sgnode);
          // require only one or path
          ADA_ASSERT (or_paths.size() == 1);
          sgnode->set_or_path(or_paths.front());
          or_paths.front()->set_parent(sgnode);
          break;

        case SgAdaSelectStmt::e_conditional_entry:
          sgnode->set_select_path(select_path);
          select_path->set_parent(sgnode);
          sgnode->set_else_path(else_path);
          else_path->set_parent(sgnode);
          break;

        case SgAdaSelectStmt::e_asynchronous:
          sgnode->set_select_path(select_path);
          select_path->set_parent(sgnode);
          sgnode->set_abort_path(abort_path);
          abort_path->set_parent(sgnode);
          break;

        default:
          ROSE_ABORT();
        }
      return SG_DEREF( sgnode );
    }
#endif /* 0 */

  private:
    SgAdaSelectAlternativeStmt*       currOrPath    = nullptr;
    SgAdaSelectStmt*                  sgnode        = nullptr;
    AstContext                        ctx;
/*
    SgAdaSelectStmt::select_type_enum ty;
    alternative_container             or_paths;
    alternative                       select_path   = nullptr;
    SgBasicBlock*                     abort_path    = nullptr;
    SgBasicBlock*                     else_path     = nullptr;
*/

    SelectStmtCreator() = delete;
  };


  struct PragmaCreator
  {
      typedef std::vector<SgPragmaDeclaration*> result_container;

      explicit
      PragmaCreator(AstContext astctx)
      : ctx(astctx)
      {}

      void operator()(Element_Struct& el)
      {
        ADA_ASSERT (el.Element_Kind == A_Pragma);
        logKind("A_Pragma");

        Pragma_Struct&       pragma = el.The_Union.The_Pragma;
        std::string          name{pragma.Pragma_Name_Image};
        ElemIdRange          argRange = idRange(pragma.Pragma_Argument_Associations);
        SgExprListExp&       args = traverseIDs(argRange, elemMap(), ArgListCreator{ctx});
        SgPragmaDeclaration& sgnode = mkPragmaDeclaration(name, args);

        ADA_ASSERT (args.get_parent());
        attachSourceLocation(sgnode, el, ctx);
        attachSourceLocation(SG_DEREF(sgnode.get_pragma()), el, ctx);
        res.push_back(&sgnode);
      }

      operator result_container() && { return std::move(res); }

    private:
      result_container res;
      AstContext       ctx;
  };


  struct SourceLocationComparator
  {
    bool operator()(Sg_File_Info* lhs, Sg_File_Info* rhs) const
    {
      ADA_ASSERT (lhs && rhs);

      if (lhs->get_line() < rhs->get_line())
        return true;

      if (rhs->get_line() < lhs->get_line())
        return false;

      if (lhs->get_col() < rhs->get_col())
        return true;

      return false;
    }

    bool operator()(SgLocatedNode* n, Sg_File_Info* rhs) const
    {
      ADA_ASSERT (n);

      return (*this)(n->get_startOfConstruct(), rhs);
    }
  };


  struct PragmaPlacer
  {
      explicit
      PragmaPlacer(SgScopeStatement& one)
      : all(), last(one)
      {
        copyToAll(one);
      }

      PragmaPlacer(SgScopeStatement& one, SgScopeStatement& two)
      : all(), last(two)
      {
        copyToAll(one); copyToAll(two);
      }

      template <class Iterator>
      void copyToAll(Iterator begin, Iterator limit)
      {
        std::copy(begin, limit, std::back_inserter(all));
      }

      void copyToAll(SgScopeStatement& lst)
      {
        if (lst.containsOnlyDeclarations())
        {
          SgDeclarationStatementPtrList& stmts = lst.getDeclarationList();

          copyToAll(stmts.begin(), stmts.end());
        }
        else
        {
          SgStatementPtrList& stmts = lst.getStatementList();

          copyToAll(stmts.begin(), stmts.end());
        }
      }


      void operator()(SgPragmaDeclaration* pragma) const
      {
        typedef std::vector<SgStatement*>::const_iterator const_iterator;
        ADA_ASSERT (pragma);

        const_iterator pos = std::lower_bound( all.begin(), all.end(),
                                               pragma->get_startOfConstruct(),
                                               SourceLocationComparator{}
                                             );

        if (pos != all.end())
          SageInterface::insertStatementBefore(*pos, pragma);
        else
          SageInterface::appendStatement(pragma, &last);
      }

    private:
      std::vector<SgStatement*> all;
      SgScopeStatement&         last;
  };

  template <class... Scopes>
  void placePragmas(Pragma_Element_ID_List pragmalst, AstContext ctx, Scopes... scopes)
  {
    typedef PragmaCreator::result_container PragmaNodes;

    ElemIdRange pragmas = idRange(pragmalst);

    if (pragmas.empty()) return; // early exit to prevent scope flattening

    PragmaNodes pragmadcls = traverseIDs(pragmas, elemMap(), PragmaCreator{ctx});

    // retroactively place pragmas according to their source position information
    std::for_each(pragmadcls.begin(), pragmadcls.end(), PragmaPlacer{scopes...});
  }


  bool isForwardLoop(Element_Struct& forvar)
  {
    ADA_ASSERT (forvar.Element_Kind == A_Declaration);

    Declaration_Struct& decl = forvar.The_Union.Declaration;
    ADA_ASSERT (decl.Declaration_Kind == A_Loop_Parameter_Specification);

    return !decl.Has_Reverse;
  }

  //
  // combined block and declarative body handling

  //~ using TryBlockNodes = std::pair<SgTryStmt*, std::reference_wrapper<SgBasicBlock> >;
  using TryBlockNodes = std::pair<SgTryStmt*, std::reference_wrapper<SgScopeStatement> >;

  SgScopeStatement&
  createBlockIfNeeded(bool newStatementBlock, SgScopeStatement& outer)
  {
    if (!newStatementBlock) return outer;

    SgBasicBlock& newblk = mkBasicBlock();

    outer.append_statement(&newblk);
    ROSE_ASSERT(newblk.get_parent() == &outer);
    return newblk;
  }

  /// if handlers are present, create an inner try block
  /// \param hasHandlers true if a block has handlers
  /// \param requiresStmtBlock true if a new block is required
  ///        even when no exception handlers are present. (e.g., for package body code section).
  /// \param outer the current scope, in which the try or block will be created.
  /// \returns a pair where second indicates the scope/block in which new statements will reside;
  ///                       first is an optional try statement (can be nullptr if no handlers exist)
  TryBlockNodes
  createTryOrBlockIfNeeded(bool hasHandlers, bool requiresStmtBlock, SgScopeStatement& outer)
  {
    if (!hasHandlers) return TryBlockNodes{nullptr, createBlockIfNeeded(requiresStmtBlock, outer)};

    SgBasicBlock& tryBlock = mkBasicBlock();
    SgTryStmt&    tryStmt  = mkTryStmt(tryBlock);

    //~ link_parent_child(outer, as<SgStatement>(tryStmt), SgBasicBlock::append_statement);
    outer.append_statement(&tryStmt);
    ADA_ASSERT (tryStmt.get_parent() == &outer);

    return TryBlockNodes{&tryStmt, tryBlock};
  }


  // a simple block handler just traverses the statement list and adds them to the \ref blk.
  void simpleBlockHandler(Statement_List bodyStatements, SgScopeStatement& blk, AstContext ctx)
  {
    ElemIdRange         range = idRange(bodyStatements);

    traverseIDs(range, elemMap(), StmtCreator{ctx.scope(blk)});
  }

  // at some point loops, labels, gotos need to be patched up. In this case, we do that at the
  //   end of a routine through the use of the LoopAndLabelManager
  // \todo As I (PP) document this, I wonder whether this works for exception handlers..?
  void routineBlockHandler(Statement_List bodyStatements, SgScopeStatement& blk, AstContext ctx)
  {
    LabelAndLoopManager lblmgr;
    ElemIdRange         range = idRange(bodyStatements);

    traverseIDs(range, elemMap(), StmtCreator{ctx.scope(blk).labelsAndLoops(lblmgr)});
  }


  // completes any block with exception handlers and pragmas attached
  void completeHandledBlock( Statement_List bodyStatements,
                             Exception_Handler_List exceptionHandlers,
                             Pragma_Element_ID_List pragmas,
                             std::function<void(Statement_List bodyStatements, SgScopeStatement& blk, AstContext ctx)> blockHandler,
                             SgScopeStatement& dominantBlock,
                             bool requiresStatementBlock,
                             AstContext ctx
                           )
  {
    ElemIdRange       hndlrs  = idRange(exceptionHandlers);
    TryBlockNodes     trydata = createTryOrBlockIfNeeded(hndlrs.size() > 0, requiresStatementBlock, dominantBlock);
    SgTryStmt*        trystmt = trydata.first;
    SgScopeStatement& stmtblk = trydata.second;

    blockHandler(bodyStatements, stmtblk, ctx);

    if (trystmt)
    {
      traverseIDs(hndlrs, elemMap(), ExHandlerCreator{ctx.scope(dominantBlock), SG_DEREF(trystmt)});
      placePragmas(pragmas, ctx, std::ref(dominantBlock), std::ref(stmtblk));
    }
    else
    {
      placePragmas(pragmas, ctx, std::ref(dominantBlock));
    }
  }


  using BlockHandler = std::function<void(Statement_List bodyStatements, SgScopeStatement& blk, AstContext ctx)>;

  // completes any block with declarative items and exception handlers and pragmas attached
  void completeDeclarationsWithHandledBlock( Element_ID_List declarativeItems,
                                             Statement_List bodyStatements,
                                             Exception_Handler_List exceptionHandlers,
                                             Pragma_Element_ID_List pragmas,
                                             BlockHandler blockHandler,
                                             SgScopeStatement& dominantBlock,
                                             bool requiresStatementBlock,
                                             AstContext ctx
                                           )
  {
    ElemIdRange range = idRange(declarativeItems);

    traverseIDs(range, elemMap(), ElemCreator{ctx.scope(dominantBlock)});

    completeHandledBlock( bodyStatements,
                          exceptionHandlers,
                          pragmas,
                          blockHandler,
                          dominantBlock,
                          requiresStatementBlock,
                          ctx
                        );
  }

  void completeRoutineBody(Declaration_Struct& decl, SgBasicBlock& declblk, AstContext ctx)
  {
    completeDeclarationsWithHandledBlock( decl.Body_Declarative_Items,
                                          decl.Body_Statements,
                                          decl.Body_Exception_Handlers,
                                          decl.Pragmas,
                                          routineBlockHandler,
                                          declblk,
                                          false /* same block for declarations and statements */,
                                          ctx
                                        );
  }

  // handles elements that can appear in a statement context (from a ROSE point of view)
  //   besides statements this also includes declarations and clauses
  void handleStmt(Element_Struct& elem, AstContext ctx)
  {
    logTrace() << "a statement/decl/clause " << elem.Element_Kind << std::endl;

    if (elem.Element_Kind == A_Declaration)
    {
      handleDeclaration(elem, ctx);
      return;
    }

    if (elem.Element_Kind == A_Clause)
    {
      handleClause(elem, ctx);
      return;
    }

    ADA_ASSERT (elem.Element_Kind == A_Statement);

    Statement_Struct& stmt = elem.The_Union.Statement;

    switch (stmt.Statement_Kind)
    {
      case A_Null_Statement:                    // 5.1
        {
          logKind("A_Null_Statement");

          completeStmt(mkNullStatement(), elem, ctx);
          /* unused fields:
          */
          break;
        }

      case An_Assignment_Statement:             // 5.2
        {
          logKind("An_Assignment_Statement");

          SgExpression& lhs    = getExprID(stmt.Assignment_Variable_Name, ctx);
          SgExpression& rhs    = getExprID(stmt.Assignment_Expression, ctx);

          SgExpression& assign = SG_DEREF(sb::buildAssignOp(&lhs, &rhs));
          SgStatement&  sgnode = SG_DEREF(sb::buildExprStatement(&assign));

          attachSourceLocation(assign, elem, ctx);
          completeStmt(sgnode, elem, ctx);
          /* unused fields:
          */
          break;
        }

      case An_If_Statement:                     // 5.3
        {
          logKind("An_If_Statement");

          SgIfStmt&   sgnode = mkIfStmt();
          ElemIdRange range  = idRange(stmt.Statement_Paths);

          completeStmt(sgnode, elem, ctx);
          traverseIDs(range, elemMap(), IfStmtCreator{sgnode, ctx});

          /* unused fields:
          */
          break;
        }

      case A_Case_Statement:                    // 5.4
        {
          logKind("A_Case_Statement");

          SgExpression&      caseexpr = getExprID(stmt.Case_Expression, ctx);
          SgBasicBlock&      casebody = mkBasicBlock();
          SgSwitchStatement& sgnode   = mkAdaCaseStmt(caseexpr, casebody);

          completeStmt(sgnode, elem, ctx);

          ElemIdRange        range    = idRange(stmt.Statement_Paths);

          traverseIDs(range, elemMap(), CaseStmtCreator{ctx.scope(casebody), sgnode});
          /* unused fields:
          */
          break;
        }

      case A_While_Loop_Statement:              // 5.5
        {
          logKind("A_While_Loop_Statement");

          SgExpression& cond     = getExprID(stmt.While_Condition, ctx);
          SgBasicBlock& block    = mkBasicBlock();
          ElemIdRange   adaStmts = idRange(stmt.Loop_Statements);
          SgWhileStmt&  sgnode   = mkWhileStmt(cond, block);

          completeStmt(sgnode, elem, ctx, stmt.Statement_Identifier);

          recordNode(ctx.labelsAndLoops().asisLoops(), elem.ID, sgnode);
          traverseIDs(adaStmts, elemMap(), StmtCreator{ctx.scope(block)});

          placePragmas(stmt.Pragmas, ctx, std::ref(block));
          /* unused fields:
                Element_ID                Corresponding_End_Name;
          */
          break;
        }

        case A_Loop_Statement:                    // 5.5
        {
          logKind("A_Loop_Statement");

          SgBasicBlock&  block    = mkBasicBlock();
          ElemIdRange    adaStmts = idRange(stmt.Loop_Statements);
          SgAdaLoopStmt& sgnode   = mkLoopStmt(block);

          completeStmt(sgnode, elem, ctx, stmt.Statement_Identifier);

          recordNode(ctx.labelsAndLoops().asisLoops(), elem.ID, sgnode);
          traverseIDs(adaStmts, elemMap(), StmtCreator{ctx.scope(block)});

          placePragmas(stmt.Pragmas, ctx, std::ref(block));

          /* unused fields:
                Element_ID                Corresponding_End_Name;
                bool                      Is_Name_Repeated;
          */
          break;
        }

      case A_For_Loop_Statement:                // 5.5
        {
          logKind("A_For_Loop_Statement");

          SgBasicBlock&          block  = mkBasicBlock();
          SgForStatement&        sgnode = mkForStatement(block);
          Element_Struct&        forvar = retrieveAs(elemMap(), stmt.For_Loop_Parameter_Specification);
          SgForInitStatement&    forini = SG_DEREF( sb::buildForInitStatement(sgnode.getStatementList()) );

          attachSourceLocation(forini, forvar, ctx);
          sg::linkParentChild(sgnode, forini, &SgForStatement::set_for_init_stmt);
          completeStmt(sgnode, elem, ctx, stmt.Statement_Identifier);
          handleDeclaration(forvar, ctx.scope(sgnode));

          // this swap is needed, b/c SgForInitStatement is not a scope
          // and when the loop variable declaration is created, the declaration
          // is pushed to the wrong statement list.
          ADA_ASSERT (sgnode.getStatementList().size() == 1);
          std::swap(forini.get_init_stmt(), block.get_statements());

          SgVariableDeclaration* inductionVar = isSgVariableDeclaration(forini.get_init_stmt().front());
          SgExpression&          direction    = mkForLoopIncrement(isForwardLoop(forvar), SG_DEREF(inductionVar));

          sg::linkParentChild(sgnode, direction, &SgForStatement::set_increment);

          // loop body
          {
            ElemIdRange            loopStmts = idRange(stmt.Loop_Statements);

            recordNode(ctx.labelsAndLoops().asisLoops(), elem.ID, sgnode);
            traverseIDs(loopStmts, elemMap(), StmtCreator{ctx.scope(block)});
          }

          placePragmas(stmt.Pragmas, ctx, std::ref(block));

          /* unused fields:
               Pragma_Element_ID_List Pragmas;
               Element_ID             Corresponding_End_Name;
          */
          break;
        }

      case A_Block_Statement:                   // 5.6
        {
          logKind("A_Block_Statement");

          SgBasicBlock& sgnode   = mkBasicBlock();

          recordNode(asisBlocks(), elem.ID, sgnode);
          completeStmt(sgnode, elem, ctx, stmt.Statement_Identifier);

          completeDeclarationsWithHandledBlock( stmt.Block_Declarative_Items,
                                                stmt.Block_Statements,
                                                stmt.Block_Exception_Handlers,
                                                stmt.Pragmas,
                                                simpleBlockHandler,
                                                sgnode,
                                                false /* same block for declarations and statements */,
                                                ctx
                                              );

          /* unused fields:
                Element_ID                Corresponding_End_Name;
                bool                      Is_Name_Repeated;
                bool                      Is_Declare_Block;
          */
          break;
        }

      case An_Exit_Statement:                   // 5.7
        {
          logKind("An_Exit_Statement");

          SgStatement&  exitedLoop    = lookupNode(ctx.labelsAndLoops().asisLoops(), stmt.Corresponding_Loop_Exited);
          SgExpression& exitCondition = getExprID_opt(stmt.Exit_Condition, ctx);
          const bool    loopIsNamed   = stmt.Exit_Loop_Name > 0;
          SgStatement&  sgnode        = mkAdaExitStmt(exitedLoop, exitCondition, loopIsNamed);

          completeStmt(sgnode, elem, ctx);
          /* unused fields:
          */
          break;
        }

      case A_Goto_Statement:                    // 5.8
        {
          logKind("A_Goto_Statement");
          SgGotoStatement& sgnode = SG_DEREF( sb::buildGotoStatement() );

          ctx.labelsAndLoops().gotojmp(getLabelRef(stmt.Goto_Label, ctx), sgnode);

          completeStmt(sgnode, elem, ctx);
          /* unused fields:
                Statement_ID           Corresponding_Destination_Statement;
          */
          break;
        }


      case An_Entry_Call_Statement:             // 9.5.3
      case A_Procedure_Call_Statement:          // 6.4
        {
          logKind(stmt.Statement_Kind == An_Entry_Call_Statement ? "An_Entry_Call_Statement" : "A_Procedure_Call_Statement");

          SgExpression&    target = getExprID(stmt.Called_Name, ctx);
          ElemIdRange      args   = idRange(stmt.Call_Statement_Parameters);
          SgExpression&    call   = createCall(target, args, true /* call syntax */, ctx);
          SgExprStatement& sgnode = SG_DEREF(sb::buildExprStatement(&call));

          attachSourceLocation(call, elem, ctx);
          completeStmt(sgnode, elem, ctx);
          /* unused fields:
              + for A_Procedure_Call_Statement / An_Entry_Call_Statement
              Declaration Corresponding_Called_Entity_Unwound

              + for A_Procedure_Call_Statement
              bool        Is_Prefix_Notation
              bool        Is_Dispatching_Call
              bool        Is_Call_On_Dispatching_Operation
              break;
          */
          break;
        }

      case A_Return_Statement:                  // 6.5
        {
          logKind("A_Return_Statement");
          Element_Struct* exprel = retrieveAsOpt(elemMap(), stmt.Return_Expression);
          SgExpression*   retval = exprel ? &getExpr(*exprel, ctx) : NULL;
          SgStatement&    sgnode = SG_DEREF( sb::buildReturnStmt(retval) );

          completeStmt(sgnode, elem, ctx);
          /* unused fields:
          */
          break;
        }


      case An_Accept_Statement:                 // 9.5.2
        {
          logKind("An_Accept_Statement");
          SgExpression&           entryref = getExprID(stmt.Accept_Entry_Direct_Name, ctx);
          SgExpression&           idx      = getExprID_opt(stmt.Accept_Entry_Index, ctx);
          SgAdaAcceptStmt&        sgnode   = mkAdaAcceptStmt(entryref, idx);

          completeStmt(sgnode, elem, ctx);

          ElemIdRange             params   = idRange(stmt.Accept_Parameters);
          AstContext              parmctx  = ctx.scope(SG_DEREF(sgnode.get_parameterScope()));

          traverseIDs(params, elemMap(), ParmlistCreator{sgnode, parmctx});

          ElemIdRange             stmts   = idRange(stmt.Accept_Body_Statements);

          if (stmts.empty())
          {
            SgStatement&          noblock = mkNullStatement();

            sg::linkParentChild(sgnode, noblock, &SgAdaAcceptStmt::set_body);
          }
          else
          {
            SgBasicBlock&         block   = mkBasicBlock();

            sg::linkParentChild(sgnode, as<SgStatement>(block), &SgAdaAcceptStmt::set_body);

            completeHandledBlock( stmt.Accept_Body_Statements,
                                  stmt.Accept_Body_Exception_Handlers,
                                  stmt.Pragmas,
                                  simpleBlockHandler,
                                  block,
                                  false /* no separate block is needed */,
                                  ctx
                                );
          }

          /* unused fields:
              Element_ID                   Corresponding_End_Name;
              bool                         Is_Name_Repeated;
              Declaration_ID               Corresponding_Entry;
          */
          break;
        }

      case A_Requeue_Statement:                 // 9.5.4
      case A_Requeue_Statement_With_Abort:      // 9.5.4
        {
          const bool withAbort = stmt.Statement_Kind == A_Requeue_Statement_With_Abort;

          logKind(withAbort ? "A_Requeue_Statement_With_Abort" : "A_Requeue_Statement");

          SgExpression&              expr   = getExprID(stmt.Requeue_Entry_Name, ctx);
          SgProcessControlStatement& sgnode = mkRequeueStmt(expr, withAbort);

          completeStmt(sgnode, elem, ctx);

          /* unused fields:
              Pragma_Element_ID_List       Pragmas;
          */
          break;
        }

      case A_Delay_Until_Statement:             // 9.6
      case A_Delay_Relative_Statement:          // 9.6
        {
          logKind(stmt.Statement_Kind == A_Delay_Until_Statement ? "A_Delay_Until_Statement" : "A_Delay_Relative_Statement");

          SgExpression&   delayexpr = getExprID(stmt.Delay_Expression, ctx);
          SgAdaDelayStmt& sgnode    = mkAdaDelayStmt(delayexpr, stmt.Statement_Kind != A_Delay_Until_Statement);

          completeStmt(sgnode, elem, ctx);

          /* unused fields:
              Pragma_Element_ID_List       Pragmas;
          */
          break;
        }

      case A_Raise_Statement:                   // 11.3
        {
          logKind("A_Raise_Statement");
          SgExpression&   raised = getExprID_opt(stmt.Raised_Exception, ctx);
          SgStatement&    sgnode = mkRaiseStmt(raised);

          completeStmt(sgnode, elem, ctx);
          /* unused fields:
               Expression_ID          Associated_Message;
          */
          break;
        }
      case A_Selective_Accept_Statement:        // 9.7.1
        {
          ElemIdRange      range  = idRange(stmt.Statement_Paths);
          SgAdaSelectStmt& sgnode = mkAdaSelectStmt(SgAdaSelectStmt::e_selective_accept);

          completeStmt(sgnode, elem, ctx);
          traverseIDs(range, elemMap(), SelectStmtCreator{sgnode, ctx});
          break;
        }
      case A_Timed_Entry_Call_Statement:        // 9.7.2
        {
          ElemIdRange  range  = idRange(stmt.Statement_Paths);
          SgAdaSelectStmt& sgnode = mkAdaSelectStmt(SgAdaSelectStmt::e_timed_entry);

          completeStmt(sgnode, elem, ctx);
          traverseIDs(range, elemMap(), SelectStmtCreator{sgnode, ctx});
          break;
        }
      case A_Conditional_Entry_Call_Statement:  // 9.7.3
        {
          ElemIdRange  range  = idRange(stmt.Statement_Paths);
          SgAdaSelectStmt& sgnode = mkAdaSelectStmt(SgAdaSelectStmt::e_conditional_entry);

          completeStmt(sgnode, elem, ctx);
          traverseIDs(range, elemMap(), SelectStmtCreator{sgnode, ctx});
          break;
        }
      case An_Asynchronous_Select_Statement:    // 9.7.4
        {
          ElemIdRange  range  = idRange(stmt.Statement_Paths);
          SgAdaSelectStmt& sgnode = mkAdaSelectStmt(SgAdaSelectStmt::e_asynchronous);

          completeStmt(sgnode, elem, ctx);
          traverseIDs(range, elemMap(), SelectStmtCreator{sgnode, ctx});
          break;
        }
      case An_Abort_Statement:                  // 9.8
        {
          logKind("An_Abort_Statement");

          ElemIdRange                range     = idRange(stmt.Aborted_Tasks);
          std::vector<SgExpression*> aborted   = traverseIDs(range, elemMap(), ExprSeqCreator{ctx});
          SgExprListExp&             abortList = mkExprListExp(aborted);
          SgProcessControlStatement& sgnode    = mkAbortStmt(abortList);

          ADA_ASSERT (abortList.get_parent());
          completeStmt(sgnode, elem, ctx);

          /* unused fields:
          */
          break;
        }
      case A_Terminate_Alternative_Statement:   // 9.7.1
        {
          logKind("A_Terminate_Alternative_Statement");

          completeStmt(mkTerminateStmt(), elem, ctx);
          /* unused fields:
          */
          break;
        }

      case A_Code_Statement:                    // 13.8 assembly
        {
          logKind("A_Code_Statement");
          logError() << "A_Code_Statement (example sought!)" << std::endl;
          // LOOKING FOR an example of a code statement in Asis
          ADA_ASSERT(!FAIL_ON_ERROR(ctx));
          break;
        }


      case Not_A_Statement: /* break; */        // An unexpected element
      //|A2005 start
      case An_Extended_Return_Statement:        // 6.5
      //|A2005 end
      default:
        logWarn() << "Unhandled statement " << stmt.Statement_Kind << std::endl;
        ADA_ASSERT (!FAIL_ON_ERROR(ctx));
    }
  }


  /// returns NameData objects for all names associated with the declararion \ref id
  NameCreator::result_container
  queryDeclNames(Declaration_ID id, AstContext ctx)
  {
    if (id == 0) return NameCreator::result_container();

    Element_Struct& elem = retrieveAs(elemMap(), id);

    ADA_ASSERT (elem.Element_Kind == A_Declaration);
    Declaration_Struct&      asisDecl = elem.The_Union.Declaration;

    ADA_ASSERT (asisDecl.Declaration_Kind == A_Choice_Parameter_Specification);

    logKind("A_Choice_Parameter_Specification");
    // SgType&                   dcltype = tyModifier(getVarType(decl, ctx));
    ElemIdRange              range    = idRange(asisDecl.Names);

    return allNames(range, ctx);
  }

  void handleExceptionHandler(Element_Struct& elem, SgTryStmt& tryStmt, AstContext ctx)
  {
    typedef NameCreator::result_container name_container;

    ADA_ASSERT (elem.Element_Kind == An_Exception_Handler);

    logKind("An_Exception_Handler");
    Exception_Handler_Struct& ex      = elem.The_Union.Exception_Handler;
    name_container            names   = queryDeclNames(ex.Choice_Parameter_Specification, ctx);

    if (names.size() == 0)
    {
      // add an unnamed exception handler
      names.emplace_back(std::string{}, std::string{}, ctx.scope(), elem);
    }

    ADA_ASSERT (names.size() == 1);
    ElemIdRange              tyRange = idRange(ex.Exception_Choices);
    SgType&                  extypes = traverseIDs(tyRange, elemMap(), ExHandlerTypeCreator{ctx});
    SgInitializedNamePtrList lst     = constructInitializedNamePtrList(ctx, asisVars(), names, extypes, nullptr);
    SgBasicBlock&            body    = mkBasicBlock();

    ADA_ASSERT (lst.size() == 1);
    SgCatchOptionStmt&       sgnode  = mkExceptionHandler(SG_DEREF(lst[0]), body);
    ElemIdRange              range   = idRange(ex.Handler_Statements);

    sg::linkParentChild(tryStmt, as<SgStatement>(sgnode), &SgTryStmt::append_catch_statement);
    sgnode.set_trystmt(&tryStmt);
    sgnode.set_parent(tryStmt.get_catch_statement_seq_root());

    traverseIDs(range, elemMap(), StmtCreator{ctx.scope(body)});

    placePragmas(ex.Pragmas, ctx, std::ref(body));
    /* unused fields:
    */
  }

  Expression_Struct& asisExpression(Element_Struct& elem)
  {
    ADA_ASSERT (elem.Element_Kind == An_Expression);

    return elem.The_Union.Expression;
  }


  /// Functor to create an import statement (actually, a declaration)
  ///   for each element of a with clause; e.g., with Ada.Calendar, Ada.Clock;
  ///   yields two important statements, each of them is its own declaration
  ///   that can be referenced.
  struct WithClauseCreator
  {
      explicit
      WithClauseCreator(AstContext astctx)
      : ctx(astctx)
      {}

      void operator()(Element_Struct& el)
      {
        ADA_ASSERT (el.Element_Kind == An_Expression);

        NameData                   imported = getName(el, ctx);
        Element_Struct&            impEl    = imported.elem();
        Element_ID                 impID    = asisExpression(impEl).Corresponding_Name_Declaration;
        SgExpression*              res      = &getExpr(impEl, ctx);
        SgScopeStatement&          scope = ctx.scope();

        if (SgVarRefExp* varref = isSgVarRefExp(res))
        {
          // PP: we should not get here in the first place (after the frontend is complete).
          logError() << "Unresolved Unit Name: " << SG_DEREF(varref->get_symbol()).get_name()
                     << std::endl;

          // \todo band-aid until frontend is complete
          // HACK: the unit was not found, and its name is not properly scope qualified
          //       ==> create a properly qualified name
          // LEAK: symbol, and initialized name will leak
          //~ scope.remove_symbol(varref->get_symbol());
          delete res;

          res = &mkUnresolvedName(imported.fullName, scope);
        }

        std::vector<SgExpression*> elems{res};
        SgImportStatement&         sgnode = mkWithClause(std::move(elems));

        recordNode(asisDecls(), el.ID, sgnode);
        attachSourceLocation(sgnode, el, ctx);


        //~ sg::linkParentChild(scope, as<SgStatement>(sgnode), &SgScopeStatement::append_statement);
        scope.append_statement(&sgnode);

        // an imported element may not be in the AST
        //   -> add the mapping if the element has not been seen.

        recordNonUniqueNode(asisDecls(), impID, sgnode);
        ADA_ASSERT (sgnode.get_parent() == &scope);
      }

    private:
      AstContext ctx;

      WithClauseCreator() = delete;
  };


  /// Functor to create an import statement (actually, a declaration)
  ///   for each element of a with clause; e.g., with Ada.Calendar, Ada.Clock;
  ///   yields two important statements, each of them is its own declaration
  ///   that can be referenced.
  struct UseClauseCreator
  {
      UseClauseCreator(map_t<int, SgDeclarationStatement*>& mapping, AstContext astctx)
      : m(mapping), ctx(astctx)
      {}

      void operator()(Element_Struct& el)
      {
        ADA_ASSERT (el.Element_Kind == An_Expression);

        NameData                     usepkg = getName(el, ctx);
        SgScopeStatement&            scope  = ctx.scope();
        Expression_Struct&           expr   = asisExpression(usepkg.elem());
        SgDeclarationStatement*      used   = findFirst(m, expr.Corresponding_Name_Definition, expr.Corresponding_Name_Declaration);

        // fallback code for packages not extracted from Asis
        if (!used)
        {
          logWarn() << "using unknown package: " << usepkg.fullName
                    << std::endl;

          used = findFirst(adaPkgs(), AdaIdentifier{usepkg.fullName});
        }

        SgUsingDeclarationStatement& sgnode = mkUseClause(SG_DEREF(used));

        //~ std::cerr
        //~ logError() << "use decl: " << usepkg.fullName
                   //~ << " " << typeid(*used).name()
                   //~ << " (" << expr.Corresponding_Name_Definition
                   //~ << ", " << expr.Corresponding_Name_Declaration << ")"
                   //~ << std::endl;

        recordNode(asisDecls(), el.ID, sgnode);
        attachSourceLocation(sgnode, el, ctx);
        scope.append_statement(&sgnode);

        ADA_ASSERT (sgnode.get_parent() == &scope);
      }

    private:
      map_t<int, SgDeclarationStatement*>& m;
      AstContext ctx;

      UseClauseCreator() = delete;
  };


  struct ComponentClauseCreator
  {
      explicit
      ComponentClauseCreator(AstContext astctx)
      : ctx(astctx)
      {}

      void operator()(Element_Struct& el)
      {
        ADA_ASSERT (el.Element_Kind == A_Clause);

        Clause_Struct& clause = el.The_Union.Clause;
        ADA_ASSERT (clause.Clause_Kind == A_Component_Clause);

        // \todo use getQualName?
        SgExpression&         field  = getExprID(clause.Representation_Clause_Name, ctx);
        SgVarRefExp&          fldref = SG_DEREF(isSgVarRefExp(&field));
        SgExpression&         ofs    = getExprID(clause.Component_Clause_Position, ctx);
        SgExpression&         rngexp = getDiscreteRangeID(clause.Component_Clause_Range, ctx);
        SgRangeExp*           range  = isSgRangeExp(&rngexp);
        SgAdaComponentClause& sgnode = mkAdaComponentClause(fldref, ofs, SG_DEREF(range));

        //~ recordNode(asisDecls(), el.ID, sgnode);
        attachSourceLocation(sgnode, el, ctx);

        ctx.scope().append_statement(&sgnode);
      }

    private:
      AstContext                       ctx;
  };

  struct EnumValueCreator
  {
      explicit
      EnumValueCreator(AstContext astctx)
      : ctx(astctx)
      {}

      SgAssignOp& itemValuePair(Element_Struct& el, Element_ID item, SgExpression& enumval)
      {
        //~ SgExpression& enumitem = getExprID(item, ctx.enumBuilder(mkEnumeratorRef_repclause));
        SgExpression& enumitem = getExprID(item, ctx);

        return SG_DEREF(sb::buildAssignOp(&enumitem, &enumval));
      }

      void operator()(Element_Struct& el)
      {
        ADA_ASSERT (el.Element_Kind == An_Association);
        logKind("An_Association");

        Association_Struct&        assoc = el.The_Union.Association;
        ADA_ASSERT (assoc.Association_Kind == An_Array_Component_Association);
        logKind("An_Array_Component_Association");

        ElemIdRange                range  = idRange(assoc.Array_Component_Choices);
        SgExpression&              enumval = getExprID(assoc.Component_Expression, ctx);

        ADA_ASSERT (range.size() <= 1);
        SgExpression&              sgnode = range.size() == 0
                                                ? enumval
                                                : itemValuePair(el, *range.first, enumval);
                                                ;

        attachSourceLocation(sgnode, el, ctx);
        values.push_back(&sgnode);
      }

      operator SgExprListExp& ()
      {
        return mkExprListExp(values);
      }

    private:
      AstContext                 ctx;
      std::vector<SgExpression*> values;
  };


  struct DefinitionDetails : std::tuple<Element_ID, Declaration_Kinds, Type_Kinds>
  {
    using base = std::tuple<Element_ID, Declaration_Kinds, Type_Kinds>;
    using base::base;

    // returns Asis ID
    Element_ID id() const { return std::get<0>(*this); }

    // returns the declaration kind
    Element_ID declKind() const { return std::get<1>(*this); }

    // returns the type kind (only valid for certain declarations)
    Element_ID typeKind() const { return std::get<2>(*this); }
  };


  DefinitionDetails
  queryDefinitionDetails(Element_Struct& complElem, AstContext ctx);

  Type_Kinds
  queryBaseDefinitionData(Definition_Struct& typeDefn, Type_Kinds tyKind, AstContext ctx)
  {
    ADA_ASSERT (tyKind == A_Derived_Type_Definition);

    Element_Struct* baseElem = retrieveAsOpt(elemMap(), typeDefn.The_Union.The_Type_Definition.Corresponding_Type_Structure);

    // derived enumeration types are handled differently ...
    //   so, if the representation is a derived enum, we change the tyKind;
    //   otherwise we keep the A_Derived_Type_Definition.
    if (baseElem)
    {
      DefinitionDetails detail = queryDefinitionDetails(*baseElem, ctx);

      if (detail.typeKind() == An_Enumeration_Type_Definition)
      tyKind = An_Enumeration_Type_Definition;
    }

    return tyKind;
  }



  DefinitionDetails
  queryDefinitionDetails(Element_Struct& complElem, AstContext ctx)
  {
    ADA_ASSERT (complElem.Element_Kind == A_Declaration);

    Declaration_Struct& complDecl = complElem.The_Union.Declaration;
    NameData            declname  = singleName(complDecl, ctx);

    if (complDecl.Declaration_Kind == A_Task_Type_Declaration)
    {
      logKind("A_Task_Type_Declaration");

      return DefinitionDetails{declname.id(), A_Task_Type_Declaration, Not_A_Type_Definition};
    }


    if (complDecl.Declaration_Kind == An_Ordinary_Type_Declaration)
      logKind("An_Ordinary_Type_Declaration");
    else if (complDecl.Declaration_Kind == A_Formal_Type_Declaration)
      logKind("A_Formal_Type_Declaration");
    else
    {
      logError() << "unexpected decl kind [queryDefinitionData]: " << complDecl.Declaration_Kind
                    << std::endl;
    }

    Element_Struct&     typeElem = retrieveAs(elemMap(), complDecl.Type_Declaration_View);
    ADA_ASSERT (typeElem.Element_Kind == A_Definition);

    Definition_Struct&  typeDefn = typeElem.The_Union.Definition;

    // \todo
    // this is questionable, but how shall we deal with non-plain type definitions ..?
    Type_Kinds          resKind  = A_Derived_Type_Definition;

    switch (typeDefn.Definition_Kind)
    {
      case A_Type_Definition:
    {
      resKind = typeDefn.The_Union.The_Type_Definition.Type_Kind;

      // look at the base of a derived type
      if (resKind == A_Derived_Type_Definition)
        resKind = queryBaseDefinitionData(typeDefn, resKind, ctx);

      break;
    }

      case A_Formal_Type_Definition:
        break;

      default:
        logError() << "unexpected def kind [queryDefinitionData]: " << typeDefn.Definition_Kind
                      << std::endl;
        ROSE_ABORT();
    }

    return DefinitionDetails{declname.id(), complDecl.Declaration_Kind, resKind};
  }

  // find the element and type kind of the corresponding complete declaration
  DefinitionDetails
  queryDefinitionDetailsID(Element_ID completeElementId, AstContext ctx)
  {
    Element_Struct& complElem = retrieveAs(elemMap(), completeElementId);

    return queryDefinitionDetails(complElem, ctx);
  }


  // In Asis forward declarations are represented as anonymous types.
  // In ROSE forward declarations are represented with the same AST node type.
  //   (e.g., a class is forward declared as a class not as typedef).
  // In order to create the right ROSE AST node, the actual information of
  //   the type in Asis is needed.
  // This methods looks at the actual declared element and returns the
  //   corresponding details.
  // e.g.,
  //   type T1;           -- This should become a first nondefining declaration in ROSE.
  //                      -- To create the proper AST node, we need to know what
  //                      -- T1 will be (in this case a task type).
  //   task type T1 ...;
  DefinitionDetails
  queryDeclarationDetails(Declaration_Struct& decl, AstContext ctx)
  {
    //~ return queryDefinitionDetailsID(decl.Corresponding_Type_Completion, ctx);
    return queryDefinitionDetailsID(decl.Corresponding_Type_Declaration, ctx);
  }


  void
  setParentRecordConstraintIfAvail(SgClassDeclaration& sgnode, Definition_Struct& def, AstContext ctx)
  {
    if (def.Definition_Kind != A_Private_Extension_Definition)
      return;

    logKind("A_Private_Extension_Definition");

    Private_Extension_Definition_Struct& ext = def.The_Union.The_Private_Extension_Definition;
    SgBaseClass&                         pardcl = getParentTypeID(ext.Ancestor_Subtype_Indication, ctx);

    sg::linkParentChild(sgnode, pardcl, &SgClassDeclaration::set_adaParentType);
  }

  void
  setTypeModifiers(SgDeclarationStatement& dcl, Definition_Struct& def, AstContext ctx)
  {
    switch (def.Definition_Kind)
    {
      case A_Private_Type_Definition:
        {
          logKind("A_Private_Type_Definition");

          setModifiersUntagged(dcl, def.The_Union.The_Private_Type_Definition);
          break;
        }

      case A_Private_Extension_Definition:
        {
          logKind("A_Private_Extension_Definition");

          setModifiersUntagged(dcl, def.The_Union.The_Private_Extension_Definition);
          break;
        }

      case A_Tagged_Private_Type_Definition:
        {
          logKind("A_Tagged_Private_Type_Definition");

          setModifiers(dcl, def.The_Union.The_Tagged_Private_Type_Definition);
          break;
        }

      default:
        logWarn() << "Unknown type declaration view: " << def.Definition_Kind
                  << std::endl;
        ADA_ASSERT (!FAIL_ON_ERROR(ctx));
    }
  }

  struct InheritedSymbolCreator
  {
      InheritedSymbolCreator(SgTypedefType& declDervType, AstContext astctx)
      : declaredDervivedType(declDervType), ctx(astctx)
      {}

      void operator()(Element_Struct& elem)
      {
        SgDeclarationStatement*       fndcl  = findFirst(asisDecls(), elem.ID);
        SgFunctionDeclaration*        fn     = isSgFunctionDeclaration(fndcl);

        if (fn == nullptr)
        {
          logError() << "unable to find function with Asis element ID " << elem.ID << std::endl;
          return;
        }

        SgAdaInheritedFunctionSymbol& sgnode = mkAdaInheritedFunctionSymbol(*fn, declaredDervivedType, ctx.scope());
        const bool inserted = inheritedSymbols().insert(std::make_pair(std::make_pair(fn, &declaredDervivedType), &sgnode)).second;

        ROSE_ASSERT(inserted);
    }

    private:
      SgTypedefType& declaredDervivedType;
      AstContext     ctx;
  };

  struct InheritedEnumeratorCreator
  {
      InheritedEnumeratorCreator(SgEnumDeclaration& enumDcl, SgEnumDeclaration& orig, AstContext astctx)
      : derivedDcl(enumDcl), // origAA(orig.get_enumerators().begin()), origZZ(orig.get_enumerators().end()),
        ctx(astctx)
      {
        //~ logError() << "|| " << std::distance(origAA, origZZ) << std::endl;
      }

      // assuming that the inherited enumerators appear in the same order
      void operator()(Element_ID id)
      {
        //~ ROSE_ASSERT(origAA != origZZ);

#if OLD_CODE
        SgInitializedName& origEnum = SG_DEREF(*origAA);
        ADA_ASSERT (isSgAssignInitializer(origEnum.get_initializer()));

        SgVarRefExp&       enumInit = SG_DEREF(sb::buildVarRefExp(&origEnum, &ctx.scope()));

        enumInit.unsetTransformation();
        enumInit.setCompilerGenerated();

        SgType&            enumTy   = SG_DEREF(derivedDcl.get_type());
        SgInitializedName& sgnode   = mkInitializedName(origEnum.get_name(), enumTy, &enumInit);

        std::cerr << "derenum " << reinterpret_cast<uint64_t>(&sgnode) << std::endl;
#endif /* OLD _CODE */

        // PP (01/25/22) new code: the old code tried to link the enumerators to their original definition
        // by setting the initializer to the old value. However, derived enumerators can have
        // different representation values, and to support JACCEL-265 and the translation to C++
        // in general, the link to the original declaration is no longer maintained.
        // Instead, the initializer now links to the representation value. The relationshio to the
        // inherited values is no implied.
        // \todo The code is most similar to the normal EnumeratorCreator in AdaType.C and
        //       could be refactored to eliminate code duplication.
        SgType&             enumTy = SG_DEREF(derivedDcl.get_type());
        Element_Struct&     elem = retrieveAs(elemMap(), id);
        ADA_ASSERT (elem.Element_Kind == A_Declaration);
        logKind("A_Declaration");

        Declaration_Struct& decl = elem.The_Union.Declaration;
        ADA_ASSERT (decl.Declaration_Kind == An_Enumeration_Literal_Specification);
        logKind("An_Enumeration_Literal_Specification");

        NameData            name = singleName(decl, ctx);
        ADA_ASSERT (name.ident == name.fullName);

        // \todo name.ident could be a character literal, such as 'c'
        //       since SgEnumDeclaration only accepts SgInitializedName as enumerators
        //       SgInitializedName are created with the name 'c' instead of character constants.
        SgExpression&       repval = getEnumRepresentationValue(name.elem(), ctx);
        SgInitializedName&  sgnode = mkInitializedName(name.ident, enumTy, &repval);

        sgnode.set_scope(derivedDcl.get_scope());
        //~ sg::linkParentChild(enumdcl, sgnode, &SgEnumDeclaration::append_enumerator);
        derivedDcl.append_enumerator(&sgnode);
        ADA_ASSERT (sgnode.get_parent() == &derivedDcl);

        recordNode(asisVars(), name.id(), sgnode);

        //~ ++origAA;
      }

    private:
      SgEnumDeclaration&                             derivedDcl;
//      SgInitializedNamePtrList::const_iterator       origAA;
//      const SgInitializedNamePtrList::const_iterator origZZ;
      AstContext                                     ctx;
  };


  void
  processInheritedSubroutines( Type_Definition_Struct& tydef,
                               SgTypedefDeclaration& derivedTypeDcl,
                               AstContext ctx
                             )
  {
    {
      SgTypedefType& ty    = SG_DEREF(derivedTypeDcl.get_type());
      ElemIdRange    range = idRange(tydef.Implicit_Inherited_Subprograms);

      traverseIDs(range, elemMap(), InheritedSymbolCreator{ty, ctx});
    }

    {
      ElemIdRange range = idRange(tydef.Implicit_Inherited_Declarations);

      if (!range.empty())
        logError() << "A derived types implicit declaration is not empty: "
                   << derivedTypeDcl.get_name()
                   << std::endl;
    }
  }

  std::tuple<SgEnumDeclaration*, SgAdaRangeConstraint*>
  getBaseEnum(SgType* baseTy)
  {
    SgAdaRangeConstraint* constraint = nullptr;
    SgEnumDeclaration*    basedecl   = nullptr;

    if (SgAdaDerivedType* deriveTy = isSgAdaDerivedType(baseTy))
    {
      SgType* ty = deriveTy->get_base_type();

      if (SgAdaSubtype* subTy = isSgAdaSubtype(ty))
      {
        ty = subTy->get_base_type();
        constraint = isSgAdaRangeConstraint(subTy->get_constraint());
      }

      if (SgEnumType* enumTy = isSgEnumType(ty))
        basedecl = isSgEnumDeclaration(enumTy->get_declaration());
    }

    ROSE_ASSERT(basedecl);
    if (SgEnumDeclaration* realdecl = isSgEnumDeclaration(basedecl->get_definingDeclaration()))
      basedecl = realdecl;

    return std::make_tuple(basedecl, constraint);
  }

  void
  processInheritedEnumValues( Type_Definition_Struct& tydef,
                              SgEnumDeclaration& derivedTypeDcl,
                              AstContext ctx
                            )
  {
    {
      ElemIdRange range = idRange(tydef.Implicit_Inherited_Subprograms);

      //~ traverseIDs(range, elemMap(), InheritedSymbolCreator{declDerivedType, ctx});
      if (!range.empty())
        logError() << "A derived enum has implicitly inherited subprograms: "
                   << derivedTypeDcl.get_name()
                   << std::endl;
    }

    {
      using BaseTuple = std::tuple<SgEnumDeclaration*, SgAdaRangeConstraint*>;

      BaseTuple          baseInfo = getBaseEnum(derivedTypeDcl.get_adaParentType());
      SgEnumDeclaration& origDecl = SG_DEREF(std::get<0>(baseInfo));
      ElemIdRange        range    = idRange(tydef.Implicit_Inherited_Declarations);

      // just traverse the IDs, as the elements are not present
      std::for_each(range.first, range.second, InheritedEnumeratorCreator{derivedTypeDcl, origDecl, ctx});
    }
  }

  void
  processInheritedElementsOfDerivedTypes(TypeData& ty, SgDeclarationStatement& dcl, AstContext ctx)
  {
    Type_Definition_Struct& tydef = ty.definitionStruct();

    if (tydef.Type_Kind != A_Derived_Type_Definition)
      return;

    if (SgTypedefDeclaration* derivedTypeDcl = isSgTypedefDeclaration(&dcl))
      processInheritedSubroutines(tydef, *derivedTypeDcl, ctx);
    else if (SgEnumDeclaration* derivedEnumDcl = isSgEnumDeclaration(&dcl))
      processInheritedEnumValues(tydef, *derivedEnumDcl, ctx);
    else
      ROSE_ABORT();
  }



  SgDeclarationStatement&
  createOpaqueDecl(NameData adaname, Declaration_Struct& decl, const DefinitionDetails detail, AstContext ctx)
  {
    SgScopeStatement&       scope = ctx.scope();
    Element_Struct*         typeview = nullptr;
    SgDeclarationStatement* res = nullptr;

    if (!isInvalidId(decl.Type_Declaration_View))
    {
      typeview = &retrieveAs(elemMap(), decl.Type_Declaration_View);
      ADA_ASSERT (typeview->Element_Kind == A_Definition);
    }

    if (detail.declKind() == A_Task_Type_Declaration)
      res = &mkAdaTaskTypeDecl(adaname.ident, nullptr /* no spec */, scope);
    else
    {
      switch (detail.typeKind())
    {
      case A_Derived_Type_Definition:
      case A_Signed_Integer_Type_Definition:
      case A_Modular_Type_Definition:
      case A_Floating_Point_Definition:
      case A_Decimal_Fixed_Point_Definition:
      case An_Access_Type_Definition:
      case An_Ordinary_Fixed_Point_Definition:// \todo untested
      case An_Unconstrained_Array_Definition: // \todo untested
      case A_Constrained_Array_Definition:    // \todo untested
        {
          res = &mkTypeDecl(adaname.ident, mkOpaqueType(), scope);
          break;
        }

      case An_Enumeration_Type_Definition:
        {
          SgEnumDeclaration& sgnode = mkEnumDecl(adaname.ident, scope);

          res = &sgnode;
          break;
        }

      case A_Record_Type_Definition:
      case A_Derived_Record_Extension_Definition:
      case A_Tagged_Record_Type_Definition:
        {
          SgClassDeclaration& sgnode = mkRecordDecl(adaname.ident, scope);

          if (typeview)
            setParentRecordConstraintIfAvail(sgnode, typeview->The_Union.Definition, ctx);

          res = &sgnode;
          break;
        }

      default:
        logWarn() << "unhandled opaque type declaration: " << detail.typeKind()
                  << std::endl;
        ADA_ASSERT (!FAIL_ON_ERROR(ctx));
        res = &mkTypeDecl(adaname.ident, mkOpaqueType(), scope);
      }
    }

    // \todo put declaration tags on
    SgDeclarationStatement& resdcl = SG_DEREF(res);

    if (typeview)
      setTypeModifiers(resdcl, typeview->The_Union.Definition, ctx);
    else if (decl.Declaration_Kind == A_Tagged_Incomplete_Type_Declaration)
      setModifiers(resdcl, false /*abstract*/, false /*limited*/, true /*tagged*/);

    return resdcl;
  }

  /// chooses whether an additional non-defining declaration is required

  SgFunctionDeclaration&
  createFunDef( SgFunctionDeclaration* nondef,
                const std::string& name,
                SgScopeStatement& scope,
                SgType& rettype,
                std::function<void(SgFunctionParameterList&, SgScopeStatement&)> complete
              )
  {
    return nondef ? mkProcedureDefn(*nondef, SG_DEREF(nondef->get_scope()), rettype, std::move(complete))
                  : mkProcedureDefn(name,    scope, rettype, std::move(complete));
  }


  SgFunctionDeclaration&
  createFunDcl( SgFunctionDeclaration* nondef,
                const std::string& name,
                SgScopeStatement& scope,
                SgType& rettype,
                std::function<void(SgFunctionParameterList&, SgScopeStatement&)> complete
              )
  {
    return nondef ? mkProcedureDecl(*nondef, scope, rettype, std::move(complete))
                  : mkProcedureDecl(name,    scope, rettype, std::move(complete));
  }

  void completeDiscriminatedDecl( SgAdaDiscriminatedTypeDecl& sgnode,
                                  SgDeclarationStatement* nondef,
                                  Element_ID id,
                                  SgDeclarationStatement& child,
                                  Element_Struct& elem,
                                  bool isPrivate,
                                  AstContext ctx
                                )
  {
    privatize(sgnode, isPrivate);
    recordNode(asisTypes(), id, child, true /* replace */);
    attachSourceLocation(sgnode, elem, ctx);

    ADA_ASSERT (sgnode.get_parent() == &ctx.scope());
  }

  SgAdaDiscriminatedTypeDecl&
  createDiscriminatedDeclID(Element_Struct& elem, AstContext ctx)
  {
    ADA_ASSERT (elem.Element_Kind == A_Definition);
    logKind("A_Definition");

    // many definitions are handled else where
    // here we want to convert the rest that can appear in declarative context

    SgScopeStatement&           scope  = ctx.scope();
    SgAdaDiscriminatedTypeDecl& sgnode = mkAdaDiscriminatedTypeDecl(scope);

    scope.append_statement(&sgnode);
    ROSE_ASSERT(sgnode.get_parent());

    Definition_Struct&          def = elem.The_Union.Definition;

    if (def.Definition_Kind == A_Known_Discriminant_Part)
    {
      logKind("A_Known_Discriminant_Part");

      ElemIdRange       discriminants = idRange(def.The_Union.The_Known_Discriminant_Part.Discriminants);
      SgScopeStatement& scope         = SG_DEREF(sgnode.get_discriminantScope());

      traverseIDs(discriminants, elemMap(), DiscriminantCreator{sgnode, ctx.scope(scope)});
    }
    else
    {
      ADA_ASSERT (def.Definition_Kind == An_Unknown_Discriminant_Part);

      logKind("An_Unknown_Discriminant_Part");
    }

    return sgnode;
  }

  SgAdaDiscriminatedTypeDecl*
  createDiscriminatedDeclID_opt(Element_ID id, AstContext ctx)
  {
    if (id == 0) return nullptr;

    return &createDiscriminatedDeclID(retrieveAs(elemMap(), id), ctx);
  }


  // handles incomplete (but completed) and private types
  void handleOpaqueTypes( Element_Struct& elem,
                          Declaration_Struct& decl,
                          bool isPrivate,
                          AstContext ctx
                        )
  {
    logTrace() << "\n  abstract: " << decl.Has_Abstract
               << "\n  limited: " << decl.Has_Limited
               << "\n  private: " << decl.Has_Private
               << std::endl;

    DefinitionDetails       defdata = queryDeclarationDetails(decl, ctx);
    NameData                adaname = singleName(decl, ctx);
    ADA_ASSERT (adaname.fullName == adaname.ident);

    SgScopeStatement*       parentScope = &ctx.scope();
    SgAdaDiscriminatedTypeDecl* discr = createDiscriminatedDeclID_opt(decl.Discriminant_Part, ctx);

    if (discr)
    {
      parentScope = discr->get_discriminantScope();
    }

    Element_ID              id     = adaname.id();
    SgScopeStatement&       scope  = SG_DEREF(parentScope);
    SgDeclarationStatement& sgdecl = createOpaqueDecl(adaname, decl, defdata, ctx.scope(scope));

    attachSourceLocation(sgdecl, elem, ctx);
    privatize(sgdecl, isPrivate);
    recordNode(asisTypes(), id, sgdecl);
    recordNode(asisTypes(), defdata.id(), sgdecl); // rec @ def

    if (!discr)
    {
      scope.append_statement(&sgdecl);
      ADA_ASSERT (sgdecl.get_parent() == &scope);
    }
    else
    {
      sg::linkParentChild(*discr, sgdecl, &SgAdaDiscriminatedTypeDecl::set_discriminatedDecl);
      completeDiscriminatedDecl(*discr, nullptr /* no nondef dcl */, id, sgdecl, elem, isPrivate, ctx);
    }
  }

  SgExprListExp* createVariantChoice_opt(AstContext ctx)
  {
    const std::vector<Name>&            variantNames = ctx.variantNames();

    if (variantNames.size() == 0) return nullptr;

    const std::vector<Element_ID_List>& variantChoices = ctx.variantChoices();
    SgExpressionPtrList                 reslst;

    ADA_ASSERT (variantNames.size() == variantChoices.size());

    std::transform( variantNames.begin(), variantNames.end(),
                    variantChoices.begin(),
                    std::back_inserter(reslst),
                    [=](Name n, Element_ID_List els) -> SgExpression*
                    {
                      SgExpression&       nameexpr  = getExprID(n, ctx);
                      ElemIdRange         range     = idRange(els);
                      SgExpressionPtrList exprlst   = traverseIDs(range, elemMap(), ExprSeqCreator{ctx});
                      SgExprListExp&      choicelst = mkExprListExp(exprlst);

                      return sb::buildIsOp(&nameexpr, &choicelst);
                    }
                  );

    return &mkExprListExp(reslst);
  }

  // compare SgAdaType::getExceptionBase
  std::pair<SgInitializedName*, SgAdaRenamingDecl*>
  getObjectBase(Element_Struct& el, AstContext ctx)
  {
    ADA_ASSERT (el.Element_Kind == An_Expression);

    NameData        name = getQualName(el, ctx);
    Element_Struct& elem = name.elem();

    ADA_ASSERT (elem.Element_Kind == An_Expression);
    Expression_Struct& obj  = elem.The_Union.Expression;

    //~ use this if package standard is included
    //~ return lookupNode(asisExcps(), ex.Corresponding_Name_Definition);

    // first try: look up in user defined exceptions
    if (SgInitializedName* ini = findFirst(asisVars(), obj.Corresponding_Name_Definition, obj.Corresponding_Name_Declaration))
      return std::make_pair(ini, nullptr);

    // second try: look up in renamed declarations
    if (SgDeclarationStatement* dcl = findFirst(asisDecls(), obj.Corresponding_Name_Definition, obj.Corresponding_Name_Declaration))
    {
      SgAdaRenamingDecl& rendcl = SG_DEREF(isSgAdaRenamingDecl(dcl));

      return std::make_pair(nullptr, &rendcl);
    }

    // last resort: create a new initialized name representing the exception
    logError() << "Unknown object: " << obj.Name_Image << std::endl;
    ADA_ASSERT (!FAIL_ON_ERROR(ctx));

    // \todo what else can we do???
    SgInitializedName& init = mkInitializedName(obj.Name_Image, lookupNode(adaTypes(), AdaIdentifier{"Integer"}), nullptr);

    init.set_scope(&ctx.scope());
    return std::make_pair(&init, nullptr);
  }

  // returns a function declaration statement for a declaration statement
  //   checks if the function is an Ada generic function, where the declaration
  //   is hidden under an SgAdaGenericDecl.
  SgFunctionDeclaration* getFunctionDeclaration(SgDeclarationStatement* dcl)
  {
    // PP: 2/6/22 refactored function out of handleDeclaration
    if (SgFunctionDeclaration* fndcl  = isSgFunctionDeclaration(dcl))
      return fndcl;

    // MS: 7/26/2021 This point may be reached for both regular and
    // generic subprograms.  If it is a generic subprogram that was
    // declared, then we'll need to get the declaration out of the
    // SgAdaGenericDecl node.
    if (SgAdaGenericDecl* generic = isSgAdaGenericDecl(dcl))
      return isSgFunctionDeclaration(generic->get_declaration());

    return nullptr;
  }


  struct EntryIndexCompletion
  {
      EntryIndexCompletion(Declaration_ID indexid, AstContext astctx)
      : id(indexid), ctx(astctx)
      {}

      SgInitializedName& operator()(SgScopeStatement& scope)
      {
        std::string     name;
        SgType*         ty   = &mkTypeVoid();
        Element_Struct* elem = retrieveAsOpt(elemMap(), id);

        if (elem)
        {
          ADA_ASSERT (elem->Element_Kind == A_Declaration);
          logKind("A_Declaration");

          Declaration_Struct& decl = elem->The_Union.Declaration;

          ADA_ASSERT (decl.Declaration_Kind == An_Entry_Index_Specification);
          logKind("An_Entry_Index_Specification");

          NameData            adaname = singleName(decl, ctx);

          ADA_ASSERT (adaname.ident == adaname.fullName);

          name = adaname.ident;
          ty   = &getDefinitionTypeID(decl.Specification_Subtype_Definition, ctx);
        }

        SgInitializedName&     ini = mkInitializedName(name, SG_DEREF(ty));
        SgVariableDeclaration& var = mkVarDecl(ini, scope);

        if (elem)
        {
          attachSourceLocation(var, *elem, ctx);
        }

        /* unhandled field
              Declaration_ID                 Corresponding_Declaration
              Declaration_ID                 Corresponding_Body;
        */
        return ini;
      }

    private:
      Declaration_ID id;
      AstContext     ctx;
  };
} // anonymous


SgDeclarationStatement*
getDecl_opt(Expression_Struct& expr, AstContext ctx)
{
  return findFirst(asisDecls(), expr.Corresponding_Name_Definition, expr.Corresponding_Name_Declaration);
}


void handleRepresentationClause(Element_Struct& elem, AstContext ctx)
{
  Clause_Struct&                clause = elem.The_Union.Clause;

  ADA_ASSERT (clause.Clause_Kind == A_Representation_Clause);

  Representation_Clause_Struct& repclause = clause.Representation_Clause;

  switch (repclause.Representation_Clause_Kind)
  {
    case A_Record_Representation_Clause:           // 13.5.1
      {
        using SageRecordClause = SgAdaRepresentationClause;

        logKind("A_Record_Representation_Clause");

        SgType&           tyrec      = getDeclTypeID(repclause.Representation_Clause_Name, ctx);
        SgExpression&     modexp     = getExprID_opt(repclause.Mod_Clause_Expression, ctx);
        SageRecordClause& sgnode     = mkAdaRepresentationClause(tyrec, modexp);
        SgBasicBlock&     components = SG_DEREF(sgnode.get_components());
        ElemIdRange       range      = idRange(repclause.Component_Clauses);

        // sgnode is not a decl: recordNode(asisDecls(), el.ID, sgnode);
        attachSourceLocation(sgnode, elem, ctx);
        ctx.scope().append_statement(&sgnode);

        traverseIDs(range, elemMap(), ComponentClauseCreator{ctx.scope(components)});

        placePragmas(repclause.Pragmas, ctx, std::ref(components));
        /* unhandled fields:
         */
        break;
      }

    case An_At_Clause:                             // J.7
      {
        using SageRecordClause = SgAdaRepresentationClause;

        logKind("An_At_Clause");

        SgType&           ty     = getDeclTypeID(repclause.Representation_Clause_Name, ctx);
        SgExpression&     modexp = getExprID_opt(repclause.Mod_Clause_Expression, ctx);
        SageRecordClause& sgnode = mkAdaRepresentationClause(ty, modexp, true /* at-clause */);

        attachSourceLocation(sgnode, elem, ctx);
        ctx.scope().append_statement(&sgnode);

        /* unhandled fields:
         */
        break;
      }

    case An_Attribute_Definition_Clause:           // 13.3
      {
        SgAdaAttributeExp&    lenattr = getAttributeExprID(repclause.Representation_Clause_Name, ctx);
        SgExpression&         lenexpr = getExprID(repclause.Representation_Clause_Expression, ctx);
        SgAdaAttributeClause& sgnode  = mkAdaAttributeClause(lenattr, lenexpr);

        attachSourceLocation(sgnode, elem, ctx);
        ctx.scope().append_statement(&sgnode);
        /* unhandled fields:
         */
        break;
      }

    case An_Enumeration_Representation_Clause:     // 13.4
      {
        SgType&                 enumty   = getDeclTypeID(repclause.Representation_Clause_Name, ctx);
        Element_Struct&         inielem  = retrieveAs(elemMap(), repclause.Representation_Clause_Expression);
        ADA_ASSERT (inielem.Element_Kind == An_Expression);

        Expression_Struct&      inilist  = inielem.The_Union.Expression;

        ADA_ASSERT (  inilist.Expression_Kind == A_Named_Array_Aggregate
                  || inilist.Expression_Kind == A_Positional_Array_Aggregate
                  );

        ElemIdRange             range    = idRange(inilist.Array_Component_Associations);
        SgExprListExp&          enumvals = traverseIDs(range, elemMap(), EnumValueCreator{ctx});
        SgAdaEnumRepresentationClause& sgnode = mkAdaEnumRepresentationClause(enumty, enumvals);

        attachSourceLocation(sgnode, elem, ctx);
        ctx.scope().append_statement(&sgnode);
        /* unhandled fields:
         */
        break;
      }

    case Not_A_Representation_Clause:              // An unexpected element
    default:
      logWarn() << "unhandled representation clause kind: " << repclause.Representation_Clause_Kind << std::endl;
      ADA_ASSERT (!FAIL_ON_ERROR(ctx));
  }
}

void handleClause(Element_Struct& elem, AstContext ctx)
{
  ADA_ASSERT (elem.Element_Kind == A_Clause);
  logKind("A_Clause");

  Clause_Struct& clause = elem.The_Union.Clause;

  switch (clause.Clause_Kind)
  {
    case A_With_Clause:                // 10.1.2
      {
        logKind("A_Choice_Parameter_Specification");
        ElemIdRange        range  = idRange(clause.Clause_Names);

        traverseIDs(range, elemMap(), WithClauseCreator{ctx});
        /* unused fields:
            bool   Has_Limited
        */
        break;
      }

    case A_Use_Type_Clause:            // 8.4
    case A_Use_Package_Clause:         // 8.4
      {
        const bool typeClause = (clause.Clause_Kind == A_Use_Type_Clause);

        logKind(typeClause ? "A_Use_Type_Clause" : "A_Use_Package_Clause");

        ElemIdRange        range  = idRange(clause.Clause_Names);
        traverseIDs(range, elemMap(), UseClauseCreator{(typeClause ? asisTypes() : asisDecls()), ctx});
        /* unused fields:
            bool   Has_Limited
        */
        break;
      }

    case A_Representation_Clause:      // 13.1     -> Representation_Clause_Kinds
      {
        handleRepresentationClause(elem, ctx);
        break;
      }

    case A_Component_Clause:           // 13.5.1
      // handled in ComponentClauseCreator
      // break;

    case A_Use_All_Type_Clause:        // 8.4: Ada 2012
    case Not_A_Clause: /* break; */    // An unexpected element
    default:
      logWarn() << "unhandled clause kind: " << clause.Clause_Kind << std::endl;
      ADA_ASSERT (!FAIL_ON_ERROR(ctx));
  }
}

void handleVariant(Element_Struct& elem, Variant_Part_Struct& variant, AstContext ctx)
{
  ElemIdRange range = idRange(variant.Variants);

  traverseIDs(range, elemMap(), VariantCreator{ctx.variantName(variant.Discriminant_Direct_Name)});

  /* unused fields:
  */
}

void handleDefinition(Element_Struct& elem, AstContext ctx)
{
  ADA_ASSERT (elem.Element_Kind == A_Definition);
  logKind("A_Definition");

  // many definitions are handled else where
  // here we want to convert the rest that can appear in declarative context

  Definition_Struct& def = elem.The_Union.Definition;

  switch (def.Definition_Kind)
  {
    case A_Null_Component:                 // 3.8(4)
      {
        SgScopeStatement&       scope = ctx.scope();
        SgExprListExp*          variant_choice = createVariantChoice_opt(ctx);
        SgDeclarationStatement& sgnode = variant_choice
                                            ? mkAdaVariantFieldDecl(*variant_choice, scope)
                                            : static_cast<SgDeclarationStatement&>(mkNullDecl());

        attachSourceLocation(sgnode, elem, ctx);
        scope.append_statement(&sgnode);
        ADA_ASSERT (sgnode.get_parent() == &scope);
        break;
      }

    case A_Variant_Part:                   // 3.8.1(2)
      {
        handleVariant(elem, def.The_Union.The_Variant_Part, ctx);
        break;
      }

    case A_Type_Definition:                // 3.2.1(4)    -> Type_Kinds
      // handled in getTypeFoundation
    case A_Subtype_Indication:             // 3.2.2(3)
      // handled in getDefinitionType
    case A_Constraint:                     // 3.2.2(5)    -> Constraint_Kinds
      // handled in getRangeConstraint
    case A_Component_Definition:           // 3.6(7)      -> Trait_Kinds
      // handled in getDefinitionType
    case A_Discrete_Range:                 // 3.6.1(3)    -> Discrete_Range_Kinds
      // handled in getDefinitionExpr
    case A_Record_Definition:              // 3.8(3)
      // handled in getRecordBodyID
    case A_Null_Record_Definition:         // 3.8(3)
      // handled in getRecordBodyID
    case An_Others_Choice:                 // 3.8.1(5): 4.3.1(5): 4.3.3(5): 11.2(5)
      // handled in case creation (and getDefinitionExpr (obsolete?))
    case An_Access_Definition:             // 3.10(6/2)   -> Access_Definition_Kinds, A2005 start
      // handled in getAccessType
    case A_Task_Definition:                // 9.1(4)
      // handled in getTaskSpec
    case An_Unknown_Discriminant_Part:     // 3.7(3)
      //
    case A_Known_Discriminant_Part:        // 3.7(2)
      //
    case A_Variant:                        // 3.8.1(3)
      //

    case A_Discrete_Subtype_Definition:    // 3.6(6)      -> Discrete_Range_Kinds
      //
      ROSE_ABORT();

    case Not_A_Definition:                 // An unexpected element
    case A_Private_Type_Definition:        // 7.3(2)      -> Trait_Kinds
    case A_Tagged_Private_Type_Definition: // 7.3(2)      -> Trait_Kinds
    case A_Private_Extension_Definition:   // 7.3(3)      -> Trait_Kinds
    case A_Protected_Definition:           // 9.4(4)
    case A_Formal_Type_Definition:         // 12.5(3)     -> Formal_Type_Kinds
    case An_Aspect_Specification:          // 13.3.1, A2012
    default:
      logWarn() << "unhandled definition kind: " << def.Definition_Kind << std::endl;
      ADA_ASSERT (!FAIL_ON_ERROR(ctx));
  }
}

void handleDeclaration(Element_Struct& elem, AstContext ctx, bool isPrivate)
{
  ADA_ASSERT (elem.Element_Kind == A_Declaration);
  logKind("A_Declaration");

  Declaration_Struct& decl = elem.The_Union.Declaration;

  switch (decl.Declaration_Kind)
  {
    case A_Package_Declaration:                    // 7.1(2)
      {
        logKind("A_Package_Declaration");

        SgScopeStatement&     outer   = ctx.scope();
        NameData              adaname = singleName(decl, ctx);
        SgAdaPackageSpecDecl& sgnode  = mkAdaPackageSpecDecl(adaname.ident, adaname.parent_scope());
        SgAdaPackageSpec&     pkgspec = SG_DEREF(sgnode.get_definition());

        logTrace() << "package decl " << adaname.ident
                   << " (" <<  adaname.fullName << ")"
                   << std::endl;

        recordNode(asisDecls(), elem.ID, sgnode);
        recordNode(asisDecls(), adaname.id(), sgnode);

        privatize(sgnode, isPrivate);
        attachSourceLocation(pkgspec, elem, ctx);
        attachSourceLocation(sgnode, elem, ctx);
        outer.append_statement(&sgnode);

        ADA_ASSERT (sgnode.get_parent() == &outer);
        ADA_ASSERT (sgnode.search_for_symbol_from_symbol_table());

        // visible items
        {
          ElemIdRange range = idRange(decl.Visible_Part_Declarative_Items);

          traverseIDs(range, elemMap(), ElemCreator{ctx.scope(pkgspec)});
        }

        // private items
        {
          ElemIdRange range = idRange(decl.Private_Part_Declarative_Items);

          traverseIDs(range, elemMap(), ElemCreator{ctx.scope(pkgspec), true /* private items */});

          // a package may contain an empty private section
          pkgspec.set_hasPrivate(decl.Is_Private_Present);
        }

        placePragmas(decl.Pragmas, ctx, std::ref(pkgspec));

        /* unused nodes:
               Element_ID                     Corresponding_End_Name;
               bool                           Is_Name_Repeated;
               Declaration_ID                 Corresponding_Declaration;
         */
        break;
      }

    case A_Package_Body_Declaration:               // 7.2(2)
      {
        logKind("A_Package_Body_Declaration");

        SgScopeStatement&     outer    = ctx.scope();
        Element_ID            specID   = decl.Corresponding_Declaration;

        // we need to check if the SgAdaPackageSpecDecl is directly available
        // or if it is wrapped by an SgAdaGenericDecl node.
        SgNode*               declnode = &lookupNode(asisDecls(), specID);
        SgAdaPackageSpecDecl* specdcl  = isSgAdaPackageSpecDecl(declnode);

        if (!specdcl) {
          if (SgAdaGenericDecl* generic = isSgAdaGenericDecl(declnode)) {
            if (isSgAdaPackageSpecDecl(generic->get_declaration())) {
              specdcl = isSgAdaPackageSpecDecl(generic->get_declaration());
            } else {
              logWarn() << "generic package declaration contains incorrect declaration type." << std::endl;
              ADA_ASSERT (!FAIL_ON_ERROR(ctx));
            }
          } else {
            logWarn() << "package declaration resolves to neither SgAdaPackageSpecDecl or SgAdaGenericDecl." << std::endl;
            ADA_ASSERT (!FAIL_ON_ERROR(ctx));
          }
        }

        // unhandled package bodies
        if (specdcl == nullptr) break;

        SgAdaPackageBodyDecl& sgnode  = mkAdaPackageBodyDecl(SG_DEREF(specdcl));
        SgAdaPackageBody&     pkgbody = SG_DEREF(sgnode.get_definition());

        //~ recordNode(asisDecls(), elem.ID, sgnode);
        //~ recordNode(asisDecls(), adaname.id(), sgnode);

        sgnode.set_scope(specdcl->get_scope());
        attachSourceLocation(pkgbody, elem, ctx);
        attachSourceLocation(sgnode, elem, ctx);
        outer.append_statement(&sgnode);
        ADA_ASSERT (sgnode.get_parent() == &outer);

#if OLD_CODE_REPLACED_WITH_COMPLETE_CALL_UNDERNEATH
        // declarative items
        {
          ElemIdRange range = idRange(decl.Body_Declarative_Items);

          traverseIDs(range, elemMap(), ElemCreator{ctx.scope(pkgbody)});
        }

        // statements
        {
          ElemIdRange range = idRange(decl.Body_Statements);

          if (range.size())
          {
            LabelAndLoopManager lblmgr;
            SgBasicBlock&       pkgblock = mkBasicBlock();

            pkgbody.append_statement(&pkgblock);
            traverseIDs(range, elemMap(), StmtCreator{ctx.scope(pkgblock).labelsAndLoops(lblmgr)});
            placePragmas(decl.Pragmas, ctx, std::ref(pkgbody), std::ref(pkgblock));
          }
          else
          {
            placePragmas(decl.Pragmas, ctx, std::ref(pkgbody));
          }
        }
#endif

        const bool hasBodyStatements = idRange(decl.Body_Statements).size() > 0;

        completeDeclarationsWithHandledBlock( decl.Body_Declarative_Items,
                                              decl.Body_Statements,
                                              decl.Body_Exception_Handlers,
                                              decl.Pragmas,
                                              routineBlockHandler,
                                              pkgbody,
                                              hasBodyStatements /* create new block for statements if needed */,
                                              ctx
                                            );

        /*
         * unused nodes:
               Element_ID                     Corresponding_End_Name;
               Declaration_ID                 Body_Block_Statement;
               bool                           Is_Name_Repeated;
               bool                           Is_Subunit;
               Declaration_ID                 Corresponding_Body_Stub;
         */
        break;
      }

    case A_Generic_Package_Declaration:            // 12.1(2)
      {
        logKind("A_Generic_Package_Declaration");

        SgScopeStatement&       outer      = ctx.scope();
        NameData                adaname    = singleName(decl, ctx);
        // create generic declaration
        SgAdaGenericDecl&       sgnode     = mkAdaGenericDecl(outer);
        SgAdaGenericDefn&       gen_defn   = SG_DEREF(sgnode.get_definition());
        SgScopeStatement&       logicalScope = adaname.parent_scope();

        //////// PP (2/2/22): add to scope, similar to generic procedures
        logicalScope.insert_symbol(adaname.ident, &mkBareNode<SgAdaGenericSymbol>(&sgnode));

        // create package in the scope of the generic
        SgAdaPackageSpecDecl&   pkgnode    = mkAdaPackageSpecDecl(adaname.ident, logicalScope);
        SgAdaPackageSpec&       pkgspec    = SG_DEREF(pkgnode.get_definition());

        // set declaration component of generic decl to package decl
        sgnode.set_declaration(&pkgnode);
        pkgnode.set_parent(&gen_defn);

        // record ID to sgnode mapping
        recordNode(asisDecls(), elem.ID, sgnode);
        recordNode(asisDecls(), adaname.id(), sgnode);

        privatize(pkgnode, isPrivate);
        attachSourceLocation(pkgspec, elem, ctx);
        attachSourceLocation(pkgnode, elem, ctx);
        attachSourceLocation(gen_defn, elem, ctx);
        attachSourceLocation(sgnode, elem, ctx);

        outer.append_statement(&sgnode);
        ADA_ASSERT (sgnode.get_parent() == &outer);
        ADA_ASSERT (pkgnode.get_parent() == &gen_defn);

        ADA_ASSERT (pkgnode.search_for_symbol_from_symbol_table());

        // generic formal part: this must be done first so the types defined in
        // the generic formal part exist when the package definition is processed.
        {
          ElemIdRange range = idRange(decl.Generic_Formal_Part);

          traverseIDs(range, elemMap(), ElemCreator{ctx.scope(gen_defn)});
        }

        // visible items
        {
          ElemIdRange range = idRange(decl.Visible_Part_Declarative_Items);

          traverseIDs(range, elemMap(), ElemCreator{ctx.scope(pkgspec)});
        }

        // private items
        {
          ElemIdRange range = idRange(decl.Private_Part_Declarative_Items);
          //ADA_ASSERT ((!range.empty()) == decl.Is_Private_Present);

          traverseIDs(range, elemMap(), ElemCreator{ctx.scope(pkgspec), true});
        }


        placePragmas(decl.Pragmas, ctx, std::ref(pkgspec));

        /* unused nodes:
               Element_ID                     Corresponding_End_Name;
               bool                           Is_Name_Repeated;
               Declaration_ID                 Corresponding_Declaration;
         */
        break;
      }

    case A_Generic_Procedure_Declaration:          // 12.1(2)
    case A_Generic_Function_Declaration:           // 12.1(2)
      {
        logKind(decl.Declaration_Kind == A_Generic_Function_Declaration
                ? "A_Generic_Function_Declaration"
                : "A_Generic_Procedure_Declaration");

        const bool             isFunc  = decl.Declaration_Kind == A_Generic_Function_Declaration;
        NameData               adaname = singleName(decl, ctx);
        SgScopeStatement&      outer   = ctx.scope();

        // PP (20/10/21): the assertion does not hold for proc/func defined in their own unit
        //~ ADA_ASSERT (adaname.fullName == adaname.ident);
        SgAdaGenericDecl&      sgnode     = mkAdaGenericDecl(outer);
        SgAdaGenericDefn&      gen_defn   = SG_DEREF(sgnode.get_definition());
        SgScopeStatement&      logicalScope = adaname.parent_scope();

        // PP (20/10/21): use the logical scope
        //    the logical scope is the parent package in the package structure
        //    this could be different from the physical parent, for example when
        //    the generic proc/func forms its own subpackage.
        //~ outer.insert_symbol(adaname.ident, &mkBareNode<SgAdaGenericSymbol>(&sgnode));
        logicalScope.insert_symbol(adaname.ident, &mkBareNode<SgAdaGenericSymbol>(&sgnode));

        // generic formal part
        {
          ElemIdRange range = idRange(decl.Generic_Formal_Part);

          traverseIDs(range, elemMap(), ElemCreator{ctx.scope(gen_defn)});
        }

        ElemIdRange            params  = idRange(decl.Parameter_Profile);
        SgType&                rettype = isFunc ? getDeclTypeID(decl.Result_Profile, ctx)
                                                : mkTypeVoid();

        // PP (10/20/21): changed scoping for packages and procedures/functions
        //                the generic proc/func is declared in the logical parent scope
        // was: SgFunctionDeclaration&  fundec     = mkProcedureDecl(adaname.fullName, gen_defn, rettype, ParameterCompletion{params, ctx});
        SgFunctionDeclaration&  fundec     = mkProcedureDecl(adaname.ident, logicalScope, rettype, ParameterCompletion{params, ctx});

        sgnode.set_declaration(&fundec);
        fundec.set_parent(&gen_defn);

        setOverride(fundec.get_declarationModifier(), decl.Is_Overriding_Declaration);

        recordNode(asisDecls(), elem.ID, sgnode);
        recordNode(asisDecls(), adaname.id(), sgnode);

        privatize(fundec, isPrivate);
        attachSourceLocation(fundec, elem, ctx);
        attachSourceLocation(sgnode, elem, ctx);
        attachSourceLocation(gen_defn, elem, ctx);

        outer.append_statement(&sgnode);
        ADA_ASSERT (fundec.get_parent() == &gen_defn);
        ADA_ASSERT (sgnode.get_parent() == &outer);

        /* unhandled fields

           bool                          Has_Abstract
           bool                          Is_Not_Overriding_Declaration
           bool                          Is_Dispatching_Operation
           Declaration_ID                Corresponding_Declaration
           Declaration_ID                Corresponding_Body
           Declaration_ID                Corresponding_Subprogram_Derivation
           Type_Definition_ID            Corresponding_Type

         +func:
           bool                          Is_Not_Null_Return
           Declaration_ID                Corresponding_Equality_Operator

           break;
        */


        break;
      }

    case A_Function_Declaration:                   // 6.1(4)   -> Trait_Kinds
    case A_Procedure_Declaration:                  // 6.1(4)   -> Trait_Kinds
      {
        logKind(decl.Declaration_Kind == A_Function_Declaration ? "A_Function_Declaration" : "A_Procedure_Declaration");

        const bool             isFunc  = decl.Declaration_Kind == A_Function_Declaration;
        SgScopeStatement&      outer   = ctx.scope();
        NameData               adaname = singleName(decl, ctx);
        ElemIdRange            params  = idRange(decl.Parameter_Profile);
        SgType&                rettype = isFunc ? getDeclTypeID(decl.Result_Profile, ctx)
                                                : mkTypeVoid();

        SgScopeStatement&      logicalScope = adaname.parent_scope();
        SgFunctionDeclaration& sgnode  = mkProcedureDecl(adaname.ident, logicalScope, rettype, ParameterCompletion{params, ctx});

        setOverride(sgnode.get_declarationModifier(), decl.Is_Overriding_Declaration);
        recordNode(asisDecls(), elem.ID, sgnode);
        recordNode(asisDecls(), adaname.id(), sgnode);

        privatize(sgnode, isPrivate);
        attachSourceLocation(sgnode, elem, ctx);
        outer.append_statement(&sgnode);
        ADA_ASSERT (sgnode.get_parent() == &outer);

        /* unhandled fields

           bool                          Has_Abstract
           bool                          Is_Not_Overriding_Declaration
           bool                          Is_Dispatching_Operation
           Declaration_ID                Corresponding_Declaration
           Declaration_ID                Corresponding_Body
           Declaration_ID                Corresponding_Subprogram_Derivation
           Type_Definition_ID            Corresponding_Type

         +func:
           bool                          Is_Not_Null_Return
           Declaration_ID                Corresponding_Equality_Operator

           break;
        */

        break;
      }

    case A_Null_Procedure_Declaration:             // 6.7
    case A_Function_Body_Declaration:              // 6.3(2)
    case A_Procedure_Body_Declaration:             // 6.3(2)
      {
        if (decl.Declaration_Kind == A_Function_Declaration)
          logKind("A_Function_Body_Declaration");
        else if (decl.Declaration_Kind == A_Procedure_Body_Declaration)
          logKind("A_Procedure_Body_Declaration");
        else
          logKind("A_Null_Procedure_Declaration");

        const bool              isFunc  = decl.Declaration_Kind == A_Function_Body_Declaration;
        SgScopeStatement&       outer   = ctx.scope();
        NameData                adaname = singleName(decl, ctx);
        ElemIdRange             params  = idRange(decl.Parameter_Profile);
        SgType&                 rettype = isFunc ? getDeclTypeID(decl.Result_Profile, ctx)
                                                 : mkTypeVoid();

        ADA_ASSERT (adaname.fullName == adaname.ident);
        SgDeclarationStatement* ndef    = findFirst(asisDecls(), decl.Corresponding_Declaration, decl.Corresponding_Body_Stub);
        SgFunctionDeclaration*  nondef  = getFunctionDeclaration(ndef);

        ADA_ASSERT (nondef || !ndef);

        SgScopeStatement&       logicalScope = adaname.parent_scope();
        SgFunctionDeclaration&  sgnode  = createFunDef(nondef, adaname.ident, logicalScope, rettype, ParameterCompletion{params, ctx});
        SgBasicBlock&           declblk = getFunctionBody(sgnode);

        recordNode(asisDecls(), elem.ID, sgnode);
        recordNode(asisDecls(), adaname.id(), sgnode);
        privatize(sgnode, isPrivate);
        attachSourceLocation(sgnode, elem, ctx);
        outer.append_statement(&sgnode);
        ADA_ASSERT (sgnode.get_parent() == &outer);

        // PP 2/6/22: Since a null procedure does not have any body,
        //            there should be no pragmas to process.
        //            Pragmas on the declaration are processed by
        //            the parent scope.
        if (decl.Declaration_Kind != A_Null_Procedure_Declaration)
          completeRoutineBody(decl, declblk, ctx);

        /* unhandled field
           Declaration_ID                 Body_Block_Statement;
           bool                           Is_Overriding_Declaration;
           bool                           Is_Not_Overriding_Declaration;

         +func:
           bool                           Is_Not_Null_Return

           break;
        */
        break;
      }

    case A_Function_Body_Stub:                     // 10.1.3(3)
    case A_Procedure_Body_Stub:                    // 10.1.3(3)
      {
        const bool isFunc  = decl.Declaration_Kind == A_Function_Body_Stub;

        logKind(isFunc ? "A_Function_Body_Stub" : "A_Procedure_Body_Stub");

        SgScopeStatement&       outer   = ctx.scope();
        NameData                adaname = singleName(decl, ctx);
        ElemIdRange             params  = idRange(decl.Parameter_Profile);
        SgType&                 rettype = isFunc ? getDeclTypeID(decl.Result_Profile, ctx)
                                                 : mkTypeVoid();

        ADA_ASSERT (adaname.fullName == adaname.ident);
        SgDeclarationStatement* ndef    = findFirst(asisDecls(), decl.Corresponding_Declaration);
        SgFunctionDeclaration*  nondef  = getFunctionDeclaration(ndef);

        ADA_ASSERT (nondef || !ndef);

        SgScopeStatement&       logicalScope = adaname.parent_scope();
        SgFunctionDeclaration&  sgnode  = createFunDcl(nondef, adaname.ident, logicalScope, rettype, ParameterCompletion{params, ctx});

        sgnode.get_declarationModifier().setAdaSeparate();

        recordNode(asisDecls(), elem.ID, sgnode);
        recordNode(asisDecls(), adaname.id(), sgnode);
        privatize(sgnode, isPrivate);
        attachSourceLocation(sgnode, elem, ctx);
        outer.append_statement(&sgnode);
        ADA_ASSERT (sgnode.get_parent() == &outer);

        /* unhandled field
           bool                           Is_Overriding_Declaration;
           bool                           Is_Not_Overriding_Declaration;
           Declaration_ID                 Corresponding_Subunit
           bool                           Is_Dispatching_Operation

         +func:
           bool                           Is_Not_Null_Return

           break;
        */
        break;
      }


    case An_Incomplete_Type_Declaration:           // 3.2.1(2):3.10(2)
    case A_Tagged_Incomplete_Type_Declaration:     //  3.10.1(2)
      {
        logKind( decl.Declaration_Kind == An_Incomplete_Type_Declaration
                        ? "An_Incomplete_Type_Declaration"
                        : "A_Tagged_Incomplete_Type_Declaration"
               );

        if (decl.Corresponding_Type_Declaration)
        {
          handleOpaqueTypes(elem, decl, isPrivate, ctx);
        }
        else
        {
          // no definition is available, ... (e.g., in System)
          NameData                adaname = singleName(decl, ctx);
          ADA_ASSERT (adaname.fullName == adaname.ident);
          SgScopeStatement&       scope  = ctx.scope();
          SgType&                 opaque = mkOpaqueType();
          SgDeclarationStatement& sgnode = mkTypeDecl(adaname.ident, opaque, scope);

          attachSourceLocation(sgnode, elem, ctx);
          privatize(sgnode, isPrivate);
          scope.append_statement(&sgnode);
          recordNode(asisTypes(), adaname.id(), sgnode);
          ADA_ASSERT (sgnode.get_parent() == &scope);
        }

        /*
           unhandled fields:
             bool                           Has_Abstract;
             bool                           Has_Limited;
             bool                           Has_Private;
             Definition_ID                  Discriminant_Part
             Declaration_ID                 Corresponding_Type_Completion
             Declaration_ID                 Corresponding_Type_Partial_View
        */
        break;
      }

    case A_Private_Extension_Declaration:
    case A_Private_Type_Declaration:               // 3.2.1(2):7.3(2) -> Trait_Kinds
      {
        logKind( decl.Declaration_Kind == A_Private_Type_Declaration
                        ? "A_Private_Type_Declaration"
                        : "A_Private_Extension_Declaration"
               );

        handleOpaqueTypes(elem, decl, isPrivate, ctx);

        /* unused fields:
              bool                           Has_Abstract;
              bool                           Has_Limited;
              bool                           Has_Private;
              Definition_ID                  Discriminant_Part;
              Declaration_ID                 Corresponding_Type_Completion;
              Declaration_ID                 Corresponding_Type_Partial_View;
              Declaration_ID                 Corresponding_First_Subtype;
              Declaration_ID                 Corresponding_Last_Constraint;
              Declaration_ID                 Corresponding_Last_Subtype;
        */
        break;
      }

    case An_Ordinary_Type_Declaration:            // 3.2.1(3)
      {
        logKind("An_Ordinary_Type_Declaration");

        logTrace() << "Ordinary Type "
                   << "\n  abstract: " << decl.Has_Abstract
                   << "\n  limited: " << decl.Has_Limited
                   << std::endl;
        // \todo this may only declare one name (use singleName)
        NameData                adaname = singleName(decl, ctx);
        ADA_ASSERT (adaname.fullName == adaname.ident);

        SgScopeStatement*       parentScope = &ctx.scope();
        SgAdaDiscriminatedTypeDecl* discr = createDiscriminatedDeclID_opt(decl.Discriminant_Part, ctx);

        if (discr)
        {
          parentScope = discr->get_discriminantScope();
        }

        SgScopeStatement&       scope = SG_DEREF(parentScope);
        TypeData                ty   = getTypeFoundation(adaname.ident, decl, ctx.scope(scope));
        Element_ID              id   = adaname.id();
        SgDeclarationStatement* nondef = findFirst(asisTypes(), id);

        SgDeclarationStatement& sgdecl = sg::dispatch(TypeDeclMaker{adaname.ident, scope, ty, nondef}, &ty.sageNode());

        privatize(sgdecl, isPrivate);
        recordNode(asisTypes(), id, sgdecl, nondef != nullptr);
        attachSourceLocation(sgdecl, elem, ctx);

        if (!discr)
        {
          scope.append_statement(&sgdecl);
          ADA_ASSERT (sgdecl.get_parent() == &scope);

          processInheritedElementsOfDerivedTypes(ty, sgdecl, ctx.scope(scope));
        }
        else
        {
          sg::linkParentChild(*discr, sgdecl, &SgAdaDiscriminatedTypeDecl::set_discriminatedDecl);
          completeDiscriminatedDecl(*discr, nondef, id, sgdecl, elem, isPrivate, ctx);
        }


        /* unused fields
            bool                           Has_Abstract;
            bool                           Has_Limited;
            Declaration_ID                 Corresponding_Type_Declaration;
            Declaration_ID                 Corresponding_Type_Partial_View;
            Declaration_ID                 Corresponding_First_Subtype;
            Declaration_ID                 Corresponding_Last_Constraint;
            Declaration_ID                 Corresponding_Last_Subtype;
        */
        break;
      }

    case A_Formal_Object_Declaration:              // 12.4(2)  -> Mode_Kinds
      {
        SgVariableDeclaration& sgnode = getParm(elem, ctx);

        ctx.scope().append_statement(&sgnode);
        /* unused fields:
        */
        break;
      }

    case A_Formal_Type_Declaration:                // 12.5(2)
      {
        logKind("A_Formal_Type_Declaration");

        NameData              adaname = singleName(decl, ctx);
        ROSE_ASSERT(adaname.fullName == adaname.ident);
        TypeData              ty = getFormalTypeFoundation(adaname.ident, decl, ctx);
        SgScopeStatement&     scope = ctx.scope();
        ROSE_ASSERT(scope.get_parent());

        Element_ID              id     = adaname.id();
        SgDeclarationStatement* nondef = findFirst(asisTypes(), id);
        SgDeclarationStatement& sgnode = sg::dispatch(TypeDeclMaker{adaname.ident, scope, ty, nondef}, &ty.sageNode());

        setModifiers(sgnode, ty);

        privatize(sgnode, isPrivate);
        recordNode(asisTypes(), id, sgnode, nondef != nullptr);
        attachSourceLocation(sgnode, elem, ctx);
        scope.append_statement(&sgnode);
        ROSE_ASSERT(sgnode.get_parent() == &scope);

        break;
      }

    case A_Formal_Procedure_Declaration:           // 12.6(2)
    case A_Formal_Function_Declaration:            // 12.6(2)
      {
        logKind(decl.Declaration_Kind == A_Formal_Procedure_Declaration ?
                "A_Formal_Procedure_Declaration" : "A_Formal_Function_Declaration");

        const bool        isFormalFuncDecl = decl.Declaration_Kind == A_Formal_Function_Declaration;
        NameData          adaname          = singleName(decl, ctx);
        ElemIdRange       range            = idRange(decl.Parameter_Profile);
        SgType&           rettype          = isFormalFuncDecl
          ? getDeclTypeID(decl.Result_Profile, ctx)
          : mkTypeVoid();

        ADA_ASSERT (adaname.fullName == adaname.ident);

        SgScopeStatement&     logicalScope = adaname.parent_scope();

        // create a function declaration for formal function/procedure declaration.
        SgFunctionDeclaration& sgnode = mkProcedureDecl(adaname.ident, logicalScope, rettype, ParameterCompletion{range, ctx});
        sgnode.set_ada_formal_subprogram_decl(true);
        recordNode(asisDecls(), elem.ID, sgnode);
        recordNode(asisDecls(), adaname.id(), sgnode);

        attachSourceLocation(sgnode, elem, ctx);
        ctx.scope().append_statement(&sgnode);

        break;
      }


    case A_Subtype_Declaration:                    // 3.2.2(2)
      {
        logKind("A_Subtype_Declaration");

        NameData              adaname = singleName(decl, ctx);
        ADA_ASSERT (adaname.fullName == adaname.ident);

        SgType&               subtype = getDefinitionTypeID(decl.Type_Declaration_View, ctx);
        SgScopeStatement&     scope   = ctx.scope();
        SgTypedefDeclaration& sgnode  = mkTypeDecl(adaname.ident, subtype, scope);

        privatize(sgnode, isPrivate);
        attachSourceLocation(sgnode, elem, ctx);
        scope.append_statement(&sgnode);
        recordNode(asisTypes(), adaname.id(), sgnode);
        ADA_ASSERT (sgnode.get_parent() == &scope);

        /* unused fields:
              Declaration_ID                 Corresponding_First_Subtype;
              Declaration_ID                 Corresponding_Last_Constraint;
              Declaration_ID                 Corresponding_Last_Subtype;
        */
        break;
      }

    case A_Variable_Declaration:                   // 3.3.1(2) -> Trait_Kinds
      {
        logKind("A_Variable_Declaration");

        handleVarCstDecl(elem, decl, ctx, isPrivate, tyIdentity);
        /* unused fields:
        */
        break;
      }

    case An_Integer_Number_Declaration:            // 3.3.2(2)
      {
        logKind("An_Integer_Number_Declaration");

        handleNumberDecl(elem, decl, ctx, isPrivate, SG_DEREF(sb::buildAutoType()), mkIntegralType());

        /* unused fields:
        */
        break;
      }

    case A_Deferred_Constant_Declaration:          // 3.3.1(6):7.4(2) -> Trait_Kinds
    case A_Constant_Declaration:
      {
        logKind(decl.Declaration_Kind == A_Constant_Declaration ? "A_Constant_Declaration" : "A_Deferred_Constant_Declaration");

        handleVarCstDecl(elem, decl, ctx, isPrivate, mkConstType);
        /* unused fields:
        */
        break;
      }

    case A_Real_Number_Declaration:                // 3.5.6(2)
      {
        logKind("A_Real_Number_Declaration");

        handleNumberDecl(elem, decl, ctx, isPrivate, SG_DEREF(sb::buildAutoType()), mkRealType());

        /* unused fields:
         */
        break;
      }

    case A_Loop_Parameter_Specification:           // 5.5(4)   -> Trait_Kinds
      {
        logKind("A_Loop_Parameter_Specification");

        ADA_ASSERT (!isPrivate);

        NameData               adaname = singleName(decl, ctx);
        ADA_ASSERT (adaname.fullName == adaname.ident);

        SgType&                vartype = SG_DEREF( sb::buildIntType() ); // \todo
        SgExpression&          range   = getDefinitionExprID(decl.Specification_Subtype_Definition, ctx);
        SgInitializedName&     loopvar = mkInitializedName(adaname.fullName, vartype, &range);
        SgScopeStatement&      scope   = ctx.scope();

        recordNode(asisVars(), adaname.id(), loopvar);

        SgVariableDeclaration& sgnode  = mkVarDecl(loopvar, scope);

        attachSourceLocation(loopvar, elem, ctx);
        attachSourceLocation(sgnode, elem, ctx);
        scope.append_statement(&sgnode);
        ADA_ASSERT (sgnode.get_parent() == &scope);

        /* unused fields:
         */
        break;
      }

    case A_Parameter_Specification:                // 6.1(15)  -> Trait_Kinds
      {
        // handled in getParm
        ROSE_ABORT();
      }

    case A_Protected_Type_Declaration:             // 9.4(2)
      {
        logKind("A_Protected_Type_Declaration");

        auto               spec    = getProtectedSpecForProtectedType(decl, ctx);
        NameData           adaname = singleName(decl, ctx);
        ADA_ASSERT (adaname.fullName == adaname.ident);


        Element_ID              nameId  = adaname.id();
        SgDeclarationStatement* incomp  = findFirst(asisTypes(), nameId);
        SgAdaProtectedTypeDecl* nondef  = isSgAdaProtectedTypeDecl(incomp);
        ADA_ASSERT(!incomp || nondef);

        SgAdaProtectedTypeDecl& sgnode  = nondef ? mkAdaProtectedTypeDecl(*nondef, SG_DEREF(spec.first), ctx.scope())
                                                 : mkAdaProtectedTypeDecl(adaname.fullName, spec.first, ctx.scope());

        attachSourceLocation(sgnode, elem, ctx);
        privatize(sgnode, isPrivate);
        ctx.scope().append_statement(&sgnode);
        ADA_ASSERT (sgnode.get_parent() == &ctx.scope());
        recordNode(asisTypes(), adaname.id(), sgnode, nondef != nullptr);
        recordNode(asisDecls(), adaname.id(), sgnode);
        recordNode(asisDecls(), elem.ID, sgnode);
        spec.second(); // complete the body

        /* unused fields:
             bool                           Has_Protected;
             Element_ID                     Corresponding_End_Name;
             Definition_ID                  Discriminant_Part;
             Definition_ID                  Type_Declaration_View;
             Declaration_ID                 Corresponding_Type_Declaration;
             Declaration_ID                 Corresponding_Type_Partial_View
             Declaration_ID                 Corresponding_First_Subtype;
             Declaration_ID                 Corresponding_Last_Constraint;
             Declaration_ID                 Corresponding_Last_Subtype;
             bool                           Is_Name_Repeated;
             Declaration_ID                 Corresponding_Declaration;
             Declaration_ID                 Corresponding_Body
             Expression_List                Declaration_Interface_List;
         */
        break;
      }

    case A_Task_Type_Declaration:                  // 9.1(2)
      {
        logKind("A_Task_Type_Declaration");

        auto               spec    = getTaskSpecForTaskType(decl, ctx);
        NameData           adaname = singleName(decl, ctx);
        ADA_ASSERT (adaname.fullName == adaname.ident);

        Element_ID              nameId  = adaname.id();
        SgDeclarationStatement* incomp  = findFirst(asisTypes(), nameId);
        SgAdaTaskTypeDecl*      nondef  = isSgAdaTaskTypeDecl(incomp);
        ADA_ASSERT(!incomp || nondef);

        SgAdaTaskTypeDecl& sgnode  = nondef ? mkAdaTaskTypeDecl(*nondef, SG_DEREF(spec.first), ctx.scope())
                                            : mkAdaTaskTypeDecl(adaname.fullName, spec.first, ctx.scope());

        attachSourceLocation(sgnode, elem, ctx);
        privatize(sgnode, isPrivate);
        ctx.scope().append_statement(&sgnode);
        ADA_ASSERT (sgnode.get_parent() == &ctx.scope());

        recordNode(asisTypes(), nameId, sgnode, nondef != nullptr);
        recordNode(asisDecls(), nameId, sgnode);
        recordNode(asisDecls(), elem.ID, sgnode);
        spec.second(); // complete the body
        /* unused fields:
             bool                           Has_Task;
             Element_ID                     Corresponding_End_Name;
             Definition_ID                  Discriminant_Part;
             Definition_ID                  Type_Declaration_View;
             Declaration_ID                 Corresponding_Type_Declaration;
             Declaration_ID                 Corresponding_Type_Partial_View;
             Declaration_ID                 Corresponding_First_Subtype;
             Declaration_ID                 Corresponding_Last_Constraint;
             Declaration_ID                 Corresponding_Last_Subtype;
             bool                           Is_Name_Repeated;
             Declaration_ID                 Corresponding_Declaration;
             Declaration_ID                 Corresponding_Body;
             Expression_List                Declaration_Interface_List;
         */
         break;
      }

    case A_Single_Protected_Declaration:           // 3.3.1(2):9.4(2)
    {
        logKind("A_Single_Protected_Declaration");

        auto               spec    = getProtectedSpecForSingleProtected(decl, ctx);
        NameData           adaname = singleName(decl, ctx);
        ADA_ASSERT (adaname.fullName == adaname.ident);
        SgAdaProtectedSpecDecl& sgnode  = mkAdaProtectedSpecDecl(adaname.fullName, SG_DEREF(spec.first), ctx.scope());

        attachSourceLocation(sgnode, elem, ctx);
        privatize(sgnode, isPrivate);
        ctx.scope().append_statement(&sgnode);
        ADA_ASSERT (sgnode.get_parent() == &ctx.scope());
        //~ recordNode(asisTypes(), adaname.id(), sgnode);
        recordNode(asisDecls(), elem.ID, sgnode);
        recordNode(asisDecls(), adaname.id(), sgnode);
        spec.second();
        /* unused fields:
             bool                           Has_Protected;
             Element_ID                     Corresponding_End_Name;
             bool                           Is_Name_Repeated;
             Declaration_ID                 Corresponding_Declaration
             Declaration_ID                 Corresponding_Body
             Expression_List                Declaration_Interface_List
        */
        break;
      }

    case A_Single_Task_Declaration:                // 3.3.1(2):9.1(3)
      {
        logKind("A_Single_Task_Declaration");

        auto               spec    = getTaskSpecForSingleTask(decl, ctx);
        NameData           adaname = singleName(decl, ctx);
        ADA_ASSERT (adaname.fullName == adaname.ident);
        SgAdaTaskSpecDecl& sgnode  = mkAdaTaskSpecDecl(adaname.fullName, SG_DEREF(spec.first), ctx.scope());

        attachSourceLocation(sgnode, elem, ctx);
        privatize(sgnode, isPrivate);
        ctx.scope().append_statement(&sgnode);
        ADA_ASSERT (sgnode.get_parent() == &ctx.scope());
        //~ recordNode(asisTypes(), adaname.id(), sgnode);
        recordNode(asisDecls(), elem.ID, sgnode);
        recordNode(asisDecls(), adaname.id(), sgnode);
        spec.second();
        /* unused fields:
             bool                           Has_Task;
             Element_ID                     Corresponding_End_Name;
             bool                           Is_Name_Repeated;
             Declaration_ID                 Corresponding_Declaration;
             Declaration_ID                 Corresponding_Body;
             Expression_List                Declaration_Interface_List
        */
        break;
      }

    case A_Protected_Body_Declaration:             // 9.4(7)
      {
        logKind("A_Protected_Body_Declaration");

        SgAdaProtectedBody&     pobody = mkAdaProtectedBody();
        NameData                adaname = singleName(decl, ctx);
        Element_ID              declID  = decl.Corresponding_Declaration;
        SgDeclarationStatement& podecl = lookupNode(asisDecls(), declID);
        ADA_ASSERT (adaname.fullName == adaname.ident);

        SgAdaProtectedBodyDecl& sgnode  = mkAdaProtectedBodyDecl(podecl, pobody, ctx.scope());

        attachSourceLocation(sgnode, elem, ctx);
        privatize(sgnode, isPrivate);
        ctx.scope().append_statement(&sgnode);
        ADA_ASSERT (sgnode.get_parent() == &ctx.scope());
        //~ recordNode(asisDecls(), elem.ID, sgnode);
        recordNode(asisDecls(), adaname.id(), sgnode);

        {
          ElemIdRange decls = idRange(decl.Protected_Operation_Items);

          traverseIDs(decls, elemMap(), StmtCreator{ctx.scope(pobody)});
        }

        placePragmas(decl.Pragmas, ctx, std::ref(pobody));

        /* unused fields:
               bool                           Has_Protected
               Element_ID                     Corresponding_End_Name
               bool                           Is_Name_Repeated
               bool                           Is_Subunit
               Declaration_ID                 Corresponding_Body_Stub
        */
        break;
      }

    case A_Task_Body_Declaration:                  // 9.1(6)
      {
        logKind("A_Task_Body_Declaration");

        SgAdaTaskBody&          tskbody = mkAdaTaskBody();
        NameData                adaname = singleName(decl, ctx);
        SgDeclarationStatement& tskdecl = lookupNode(asisDecls(), decl.Corresponding_Declaration);
        ADA_ASSERT (adaname.fullName == adaname.ident);

        // \todo \review not sure why a task body could be independently created
        //~ SgDeclarationStatement* tskdecl = findNode(asisDecls(), decl.Corresponding_Declaration);
        //~ if (tskdecl == nullptr)
          //~ logError() << adaname.fullName << " task body w/o decl" << std::endl;

        //~ SgAdaTaskBodyDecl&      sgnode  = tskdecl ? mkAdaTaskBodyDecl(*tskdecl, tskbody, ctx.scope())
                                                  //~ : mkAdaTaskBodyDecl(adaname.fullName, tskbody, ctx.scope());

        SgAdaTaskBodyDecl&      sgnode  = mkAdaTaskBodyDecl(tskdecl, tskbody, ctx.scope());

        attachSourceLocation(sgnode, elem, ctx);
        privatize(sgnode, isPrivate);
        ctx.scope().append_statement(&sgnode);
        ADA_ASSERT (sgnode.get_parent() == &ctx.scope());
        //~ recordNode(asisDecls(), elem.ID, sgnode);
        recordNode(asisDecls(), adaname.id(), sgnode);

        placePragmas(decl.Pragmas, ctx, std::ref(tskbody));

        completeDeclarationsWithHandledBlock( decl.Body_Declarative_Items,
                                              decl.Body_Statements,
                                              decl.Body_Exception_Handlers,
                                              decl.Pragmas,
                                              routineBlockHandler,
                                              tskbody,
                                              false /* same block for declarations and statements */,
                                              ctx
                                            );

        /* unused fields:
             bool                           Has_Task;
             Element_ID                     Corresponding_End_Name;
             Declaration_ID                 Body_Block_Statement;
             bool                           Is_Name_Repeated;
             Declaration_ID                 Corresponding_Declaration;
             bool                           Is_Subunit;
             Declaration_ID                 Corresponding_Body_Stub;
        */
        break;
      }

    case An_Entry_Declaration:                     // 9.5.2(2)
      {
        logKind("An_Entry_Declaration");

        NameData        adaname = singleName(decl, ctx);
        ElemIdRange     range   = idRange(decl.Parameter_Profile);

        ADA_ASSERT (adaname.fullName == adaname.ident);
        SgType&         idxTy  = getDefinitionTypeID_opt(decl.Entry_Family_Definition, ctx);
        SgAdaEntryDecl& sgnode = mkAdaEntryDecl(adaname.fullName, ctx.scope(), ParameterCompletion{range, ctx}, idxTy);

        attachSourceLocation(sgnode, elem, ctx);
        privatize(sgnode, isPrivate);
        ctx.scope().append_statement(&sgnode);
        ADA_ASSERT (sgnode.get_parent() == &ctx.scope());

        // the entry call links back to the declaration ID
        recordNode(asisDecls(), elem.ID, sgnode);

        // for consistency, also map the name id to the node
        recordNode(asisDecls(), adaname.id(), sgnode);

        /* unused fields:
             bool                           Is_Overriding_Declaration;
             bool                           Is_Not_Overriding_Declaration;
             Declaration_ID                 Corresponding_Body;
        */
        break;
      }

    case An_Entry_Body_Declaration:                // 9.5.2(5)
      {
        NameData                adaname  = singleName(decl, ctx);
        SgDeclarationStatement* sagedcl  = findFirst(asisDecls(), decl.Corresponding_Declaration);
        SgAdaEntryDecl&         entrydcl = SG_DEREF(isSgAdaEntryDecl(sagedcl));
        ADA_ASSERT (adaname.fullName == adaname.ident);

        //~ SgScopeStatement&       logicalScope = adaname.parent_scope();
        //~ SgAdaEntryDecl&         sgnode  = mkAdaEntryDef(entrydcl, logicalScope, ParameterCompletion{range, ctx});
        ElemIdRange             range   = idRange(decl.Parameter_Profile);
        SgScopeStatement&       outer   = ctx.scope();
        SgAdaEntryDecl&         sgnode  = mkAdaEntryDefn( entrydcl,
                                                          outer,
                                                          ParameterCompletion{range, ctx},
                                                          EntryIndexCompletion{decl.Entry_Index_Specification, ctx}
                                                       );

        recordNode(asisDecls(), elem.ID, sgnode);
        recordNode(asisDecls(), adaname.id(), sgnode);
        privatize(sgnode, isPrivate);
        attachSourceLocation(sgnode, elem, ctx);

        outer.append_statement(&sgnode);
        ADA_ASSERT (sgnode.get_parent() == &outer);

        SgFunctionDefinition&   fndef   = SG_DEREF(sgnode.get_definition());
        SgBasicBlock&           declblk = SG_DEREF(fndef.get_body());

        if (isInvalidId(decl.Entry_Barrier))
          logError() << "Entry_Barrier-id " << decl.Entry_Barrier << std::endl;

        SgExpression&           barrier = getExprID_opt(decl.Entry_Barrier, ctx.scope(fndef));

        sgnode.set_entryBarrier(&barrier);
        completeRoutineBody(decl, declblk, ctx);

        /* unused fields:
             Element_ID                     Corresponding_End_Name
             Declaration_ID                 Body_Block_Statement
             bool                           Is_Name_Repeated
        */
        break;
      }

    case An_Exception_Declaration:                 // 11.1(2)
      {
        logKind("An_Exception_Declaration");

        typedef NameCreator::result_container name_container;

        ElemIdRange              range    = idRange(decl.Names);
        name_container           names    = allNames(range, ctx);
        SgScopeStatement&        scope    = ctx.scope();
        SgType&                  excty    = lookupNode(adaTypes(), AdaIdentifier{"Exception"});
        SgInitializedNamePtrList dclnames = constructInitializedNamePtrList(ctx, asisExcps(), names, excty, nullptr);
        SgVariableDeclaration&   sgnode   = mkExceptionDecl(dclnames, scope);

        attachSourceLocation(sgnode, elem, ctx);
        privatize(sgnode, isPrivate);
        scope.append_statement(&sgnode);
        ADA_ASSERT (sgnode.get_parent() == &scope);
        break;
      }

    case A_Component_Declaration:                  // 3.8(6)
      {
        logKind("A_Component_Declaration");

        ADA_ASSERT (ctx.variantChoices().size() == ctx.variantNames().size());

        SgExprListExp* variant_choice = createVariantChoice_opt(ctx);

        auto compMaker =
            [&variant_choice](const SgInitializedNamePtrList& names, SgScopeStatement& scope) -> SgVariableDeclaration&
            {
              if (variant_choice == nullptr)
                return mkVarDecl(names, scope);

              return mkAdaVariantFieldDecl(names, *variant_choice, scope);
            };

        handleVarCstDecl(elem, decl, ctx, isPrivate, tyIdentity, compMaker);
        /* unused clause:
              Pragma_Element_ID_List         Corresponding_Pragmas;
              Element_ID_List                Aspect_Specifications;
              Representation_Clause_List     Corresponding_Representation_Clauses;
        */
        break;
      }

/*
* only for Ada 2012
*
    case An_Expression_Function_Declaration:       // 6.8
      {
        / * unhandled fields - 2012 feature
        bool                           Is_Not_Null_Return;
        Parameter_Specification_List   Parameter_Profile;
        Element_ID                     Result_Profile;
        Expression_ID                  Result_Expression;
        bool                           Is_Overriding_Declaration;
        bool                           Is_Not_Overriding_Declaration;
        Declaration_ID                 Corresponding_Declaration;
        Type_Definition_ID             Corresponding_Type;
        bool                           Is_Dispatching_Operation;
        * /

        break ;
      }
*/

    case A_Generic_Package_Renaming_Declaration:   // 8.5.3(2)
    case A_Package_Renaming_Declaration:           // 8.5.3(2)
      {
        logKind(decl.Declaration_Kind == A_Package_Renaming_Declaration ?
                "A_Package_Renaming_Declaration" : "A_Generic_Package_Renaming_Declaration");

        NameData                adaname = singleName(decl, ctx);

        if (isInvalidId(decl.Renamed_Entity))
        {
          logWarn() << "skipping unknown package renaming: " << adaname.ident << "/" << adaname.fullName
                    << ": " << elem.ID << " / " << decl.Renamed_Entity
                    << std::endl;
          return;
        }

        SgDeclarationStatement* aliased = &getAliasedID(decl.Renamed_Entity, ctx);

        if (SgAdaGenericDecl* gendcl = isSgAdaGenericDecl(aliased))
          aliased = gendcl->get_declaration();

        SgScopeStatement&       scope   = ctx.scope();
        SgType*                 pkgtype = &mkTypeVoid(); // or nullptr?
        SgAdaRenamingDecl&      sgnode  = mkAdaRenamingDecl(adaname.ident, SG_DEREF(aliased), pkgtype, scope);

        recordNode(asisDecls(), elem.ID, sgnode);
        recordNode(asisDecls(), adaname.id(), sgnode);

        attachSourceLocation(sgnode, elem, ctx);
        privatize(sgnode, isPrivate);
        scope.append_statement(&sgnode);
        ADA_ASSERT (sgnode.get_parent() == &scope);
        /* unhandled field
             Declaration_ID                 Corresponding_Declaration;
             Expression_ID                  Corresponding_Base_Entity;
        */
        break;
      }

    case A_Procedure_Renaming_Declaration:         // 8.5.4(2)
    case A_Function_Renaming_Declaration:          // 8.5.4(2)
      {
        logKind(decl.Declaration_Kind == A_Function_Renaming_Declaration ?
                "A_Function_Renaming_Declaration" : "A_Procedure_Renaming_Declaration");

        const bool        isFuncRename = decl.Declaration_Kind == A_Function_Renaming_Declaration;
        SgScopeStatement& outer        = ctx.scope();
        NameData          adaname      = singleName(decl, ctx);
        ElemIdRange       range        = idRange(decl.Parameter_Profile);
        SgType&           rettype      = isFuncRename ? getDeclTypeID(decl.Result_Profile, ctx)
                                                      : mkTypeVoid();

        ADA_ASSERT (adaname.fullName == adaname.ident);
        SgAdaFunctionRenamingDecl& sgnode  = mkAdaFunctionRenamingDecl(adaname.fullName,
                                                                       outer,
                                                                       rettype,
                                                                       ParameterCompletion{range, ctx});
        setOverride(sgnode.get_declarationModifier(), decl.Is_Overriding_Declaration);
        recordNode(asisDecls(), elem.ID, sgnode);
        recordNode(asisDecls(), adaname.id(), sgnode);

        // find declaration for the thing being renamed
        Element_Struct&        renamedElemFull = retrieveAs(elemMap(), decl.Renamed_Entity);
        NameData               renamedName = getQualName(renamedElemFull, ctx);
        Element_Struct&        renamedElem = renamedName.elem();
        ROSE_ASSERT(renamedElem.Element_Kind == An_Expression);

        Expression_Struct&     renamedFuncRef = renamedElem.The_Union.Expression;
        SgFunctionDeclaration* renamedDecl = isSgFunctionDeclaration(getDecl_opt(renamedFuncRef, ctx));

        if (renamedDecl != nullptr)
        {
          sgnode.set_renamed_function(renamedDecl);
        }
        else
        {
          logError() << "cannot find renamed proc/func decl for:" << adaname.fullName << std::endl;
        }

        privatize(sgnode, isPrivate);
        attachSourceLocation(sgnode, elem, ctx);
        outer.append_statement(&sgnode);
        ADA_ASSERT (sgnode.get_parent() == &outer);
        break;
      }

    case An_Object_Renaming_Declaration:           // 8.5.1(2)
      {
        using ObjBasePair = std::pair<SgInitializedName*, SgAdaRenamingDecl*>;

        logKind("An_Object_Renaming_Declaration");

        NameData           adaname = singleName(decl, ctx);
        if (isInvalidId(decl.Renamed_Entity))
        {
          logWarn() << "skipping unknown renaming: " << adaname.ident << "/" << adaname.fullName
                    << ": " << elem.ID << " / " << decl.Renamed_Entity
                    << std::endl;
          return;
        }

        Declaration_Struct& asisDecl = elem.The_Union.Declaration;
        Element_Struct&     renamed_entity_elem = retrieveAs(elemMap(), decl.Renamed_Entity);
        ObjBasePair         objpair = getObjectBase(renamed_entity_elem, ctx);
        ADA_ASSERT (objpair.first || objpair.second);

        SgScopeStatement&   scope   = ctx.scope();
        SgType&             ty      = getDeclTypeID(asisDecl.Object_Declaration_View, ctx);
        SgAdaRenamingDecl&  sgnode  = objpair.first
                                         ? mkAdaRenamingDecl(adaname.ident, *objpair.first,  &ty, scope)
                                         : mkAdaRenamingDecl(adaname.ident, *objpair.second, &ty, scope);

        recordNode(asisDecls(), elem.ID, sgnode);
        recordNode(asisDecls(), adaname.id(), sgnode);

        attachSourceLocation(sgnode, elem, ctx);
        privatize(sgnode, isPrivate);
        scope.append_statement(&sgnode);
        ADA_ASSERT (sgnode.get_parent() == &scope);
        break;
      }

    case An_Exception_Renaming_Declaration:        // 8.5.2(2)
      {
        using ExBasePair = std::pair<SgInitializedName*, SgAdaRenamingDecl*>;

        logKind("An_Exception_Renaming_Declaration");

        NameData           adaname = singleName(decl, ctx);

        if (isInvalidId(decl.Renamed_Entity))
        {
          logWarn() << "skipping unknown exception renaming: " << adaname.ident << "/" << adaname.fullName
                    << ": " << elem.ID << " / " << decl.Renamed_Entity
                    << std::endl;
          return;
        }

        Element_Struct&    renamed_entity_elem = retrieveAs(elemMap(), decl.Renamed_Entity);
        ExBasePair         expair = getExceptionBase(renamed_entity_elem, ctx);
        ADA_ASSERT (expair.first || expair.second);

        SgScopeStatement&  scope   = ctx.scope();
        SgAdaRenamingDecl& sgnode
                   = expair.first
                       ? mkAdaRenamingDecl(adaname.ident, *expair.first,  expair.first->get_type(),  scope)
                       : mkAdaRenamingDecl(adaname.ident, *expair.second, expair.second->get_type(), scope);

        recordNode(asisDecls(), elem.ID, sgnode);
        recordNode(asisDecls(), adaname.id(), sgnode);

        attachSourceLocation(sgnode, elem, ctx);
        privatize(sgnode, isPrivate);
        scope.append_statement(&sgnode);
        ADA_ASSERT (sgnode.get_parent() == &scope);
        break;
      }

    case A_Generic_Procedure_Renaming_Declaration: // 8.5.5(2)
    case A_Generic_Function_Renaming_Declaration:  // 8.5.5(2)
      {
        // \todo consider folding the code into generic_package_renaming

        logKind(decl.Declaration_Kind == A_Generic_Function_Renaming_Declaration
                      ? "A_Generic_Function_Renaming_Declaration"
                      : "A_Generic_Procedure_Renaming_Declaration"
               );

        NameData                adaname = singleName(decl, ctx);

        if (isInvalidId(decl.Renamed_Entity))
        {
          logWarn() << "skipping unknown package renaming: " << adaname.ident << "/" << adaname.fullName
                    << ": " << elem.ID << " / " << decl.Renamed_Entity
                    << std::endl;
          return;
        }

        SgDeclarationStatement* aliased = &getAliasedID(decl.Renamed_Entity, ctx);
        SgFunctionDeclaration*  fundecl = getFunctionDeclaration(aliased);

        if (fundecl) aliased = fundecl;

        SgScopeStatement&       scope   = ctx.scope();
        SgType*                 pkgtype = &mkTypeVoid(); // or nullptr or function type?
        SgAdaRenamingDecl&      sgnode  = mkAdaRenamingDecl(adaname.ident, *aliased, pkgtype, scope);

        recordNode(asisDecls(), elem.ID, sgnode);
        recordNode(asisDecls(), adaname.id(), sgnode);

        attachSourceLocation(sgnode, elem, ctx);
        privatize(sgnode, isPrivate);
        scope.append_statement(&sgnode);
        ADA_ASSERT (sgnode.get_parent() == &scope);

        /* unhandled field
             Declaration_ID                 Corresponding_Declaration;
             Expression_ID                  Corresponding_Base_Entity;
        */
        break;
      }


    case A_Package_Instantiation:                  // 12.3(2)
    case A_Procedure_Instantiation:                // 12.3(2)
    case A_Function_Instantiation:                 // 12.3(2)
      {
        // generic instantiation
        if (decl.Declaration_Kind == A_Function_Instantiation)
          logKind("A_Function_Instantiation");
        else if (decl.Declaration_Kind == A_Procedure_Instantiation)
          logKind("A_Procedure_Instantiation");
        else
          logKind("A_Package_Instantiation");

        SgScopeStatement&         outer   = ctx.scope();
        NameData                  adaname = singleName(decl, ctx);

        // need to look up the Generic_Unit_Name to find the ID for the generic
        // declaration.  The Corresponding_Declaration ID doesn't seem to point at
        // an entity that was traversed during the AST creation.  Instead we need
        // to dig up the Corresponding_Name_Declaration from the Expression
        // associated with Generic_Unit_Name. (MS: 8/1/21)
        NameData                  basename = getNameID(decl.Generic_Unit_Name, ctx);
        Element_Struct&           baseelem = basename.elem();
        Expression_Struct&        baseexpr = baseelem.The_Union.Expression;
        SgDeclarationStatement*   basedecl = findFirst(asisDecls(), baseexpr.Corresponding_Name_Declaration, baseexpr.Corresponding_Name_Definition);

        // PP (2/2/22): the base decl can also be a renamed generic declaration
        SgAdaGenericInstanceDecl& sgnode   = mkAdaGenericInstanceDecl(adaname.ident, SG_DEREF(basedecl), outer);

        {
          // generic actual part
          ElemIdRange range = idRange(decl.Generic_Actual_Part);

          SgExprListExp& args = traverseIDs(range, elemMap(), ArgListCreator{ctx});
          sgnode.set_actual_parameters(&args);
        }

        recordNode(asisDecls(), elem.ID, sgnode);
        recordNode(asisDecls(), adaname.id(), sgnode);

        attachSourceLocation(sgnode, elem, ctx);
        privatize(sgnode, isPrivate);
        outer.append_statement(&sgnode);
        ADA_ASSERT (sgnode.get_parent() == &outer);

        break;
      }

    case An_Entry_Index_Specification:             // 9.5.2(2)
      {
        // handled by EntryIndexCompletion;
        ROSE_ABORT();
      }

    case A_Choice_Parameter_Specification:         // 11.2(4)
      {
        // handled in handleExceptionHandler
        ROSE_ABORT();
      }

    case An_Enumeration_Literal_Specification:     // 3.5.1(3)
      {
        // handled in EnumElementCreator
        ROSE_ABORT();
      }

    case A_Discriminant_Specification:             // 3.7(5)   -> Trait_Kinds
      {
        // handled in getDiscriminant
        ROSE_ABORT();
      }

    case Not_A_Declaration: /* break; */           // An unexpected element
    case A_Generalized_Iterator_Specification:     // 5.5.2    -> Trait_Kinds
    case An_Element_Iterator_Specification:        // 5.5.2    -> Trait_Kinds
    case A_Return_Variable_Specification:          // 6.5
    case A_Return_Constant_Specification:          // 6.5
    case A_Package_Body_Stub:                      // 10.1.3(4)
    case A_Task_Body_Stub:                         // 10.1.3(5)
    case A_Protected_Body_Stub:                    // 10.1.3(6)
    case A_Formal_Incomplete_Type_Declaration:
    case A_Formal_Package_Declaration:             // 12.7(2)
    case A_Formal_Package_Declaration_With_Box:    // 12.7(3)
    default:
      logWarn() << "unhandled declaration kind: " << decl.Declaration_Kind << std::endl;
      ADA_ASSERT (!FAIL_ON_ERROR(ctx));
  }
}

void ParameterCompletion::operator()(SgFunctionParameterList& lst, SgScopeStatement& parmscope)
{
  traverseIDs(range, elemMap(), ParmlistCreator{lst, ctx.scope_npc(parmscope)});
}

void StmtCreator::operator()(Element_Struct& elem)
{
  handleStmt(elem, ctx);
}

/// asserts that \ref decl has exactly one associated name
/// and returns the respective NameData data.
NameData
singleName(Declaration_Struct& decl, AstContext ctx)
{
  ElemIdRange     range = idRange(decl.Names);
  ADA_ASSERT (range.size() == 1);

  return getNameID(*range.first, ctx);
}

std::vector<NameData>
allNames(ElemIdRange range, AstContext ctx)
{
  return traverseIDs(range, elemMap(), NameCreator{ctx});
}


NameData
getName(Element_Struct& elem, AstContext ctx)
{
  // \todo can getName be distinguished from getQualName by the call context?
  if (elem.Element_Kind == An_Expression)
    return getQualName(elem, ctx);

  if (elem.Element_Kind != A_Defining_Name)
    logWarn() << "unexpected elem kind (getName)" << elem.Element_Kind << std::endl;

  ADA_ASSERT (elem.Element_Kind == A_Defining_Name);
  logKind("A_Defining_Name");

  Defining_Name_Struct& asisname = elem.The_Union.Defining_Name;
  SgScopeStatement*     parent = &ctx.scope();
  std::string           ident{asisname.Defining_Name_Image};
  std::string           name{ident};

  switch (asisname.Defining_Name_Kind)
  {
    case A_Defining_Expanded_Name:
      {
        logKind("A_Defining_Expanded_Name");

        NameData        defname    = getNameID(asisname.Defining_Selector, ctx);

        ident  = defname.ident;
        parent = &getScopeID(asisname.Defining_Prefix, ctx);
        break;
      }

    case A_Defining_Identifier:
      {
        logKind("A_Defining_Identifier");
        // nothing to do, the fields are already set
        break;
      }

    case A_Defining_Operator_Symbol:     // 6.1(9)
      {
        static const std::string cxxprefix{"operator"};

        logKind("A_Defining_Operator_Symbol");

        ADA_ASSERT (ident.size() > 2);
        name = ident = cxxprefix + ident.substr(1, ident.size() - 2);

        // nothing to do, the fields are already set

        /* unused field:
             enum Operator_Kinds       Operator_Kind
        */
        break;
      }

    case A_Defining_Enumeration_Literal: // 3.5.1(3)
      {
        logKind("A_Defining_Enumeration_Literal");

        /* unused fields:
             char* Position_Number_Image;      // \pp implied by name sequence
             char* Representation_Value_Image; // \pp not in code, but could be defined by representation clause
        */
        break;
      }

    case A_Defining_Character_Literal:   // 3.5.1(4)
      {
        logKind("A_Defining_Enumeration_Literal");

        logWarn() << "a A_Defining_Character_Literal (reconsider translation)"
                  << " (" << name << ")"
                  << std::endl;
        break;
      }

    case Not_A_Defining_Name:
      /* break; */

    default:
      logWarn() << "unknown name kind " << asisname.Defining_Name_Kind
                << " (" << name << ")"
                << std::endl;
      ADA_ASSERT (!FAIL_ON_ERROR(ctx));
  }

  return NameData{ ident, name, SG_DEREF(parent), elem };
}

NameData
getQualName(Element_Struct& elem, AstContext ctx)
{
  ADA_ASSERT (elem.Element_Kind == An_Expression);

  Expression_Struct& idex  = elem.The_Union.Expression;

  if (idex.Expression_Kind == An_Identifier)
  {
    logKind("An_Identifier");

    std::string ident{idex.Name_Image};

    return NameData{ ident, ident, ctx.scope(), elem };
  }

  if (idex.Expression_Kind == An_Operator_Symbol)
  {
    logKind("An_Operator_Symbol");

    std::string ident{idex.Name_Image};

    return NameData{ ident, ident, ctx.scope(), elem };
  }

  if (idex.Expression_Kind != A_Selected_Component)
    logError() << "Unexpected Expression_kind [getQualName]: " << idex.Expression_Kind
               << std::endl;

  ADA_ASSERT (idex.Expression_Kind == A_Selected_Component);
  logKind("A_Selected_Component");

  NameData compound = getNameID(idex.Prefix, ctx);
  NameData selected = getNameID(idex.Selector, ctx);

  return NameData{ selected.ident,
                   compound.fullName + "." + selected.fullName,
                   ctx.scope(),
                   selected.elem()
                 };
}


NameData
getNameID(Element_ID el, AstContext ctx)
{
  return getName(retrieveAs(elemMap(), el), ctx);
}



}
