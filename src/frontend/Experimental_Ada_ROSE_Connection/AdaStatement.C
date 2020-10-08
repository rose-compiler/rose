
#include "sage3basic.h"
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
  /// auto deleter of an expression object
  typedef std::unique_ptr<SgExpression> GuardedExpression;

  struct NameData
  {
      NameData(std::string id, std::string full, SgScopeStatement* scope, Element_Struct* el)
      : ident(id), fullName(full), parent(scope), asisElem(el)
      {}

      Element_ID id() const { return SG_DEREF(asisElem).ID; }

      std::string       ident;
      std::string       fullName;
      SgScopeStatement* parent;
      Element_Struct*   asisElem;

    private:
      NameData() = delete;
  };

  /// extracts NameData from \ref elem
  NameData
  getName(Element_Struct& elem, AstContext ctx);

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
    // \todo add handlers as needed
  };

  /// returns the ROSE scope of an already converted Asis element \ref elem.
  SgScopeStatement&
  getScope(Element_Struct& elem, AstContext ctx)
  {
    ROSE_ASSERT(elem.Element_Kind == An_Expression);

    Expression_Struct& expr = elem.The_Union.Expression;
    ROSE_ASSERT (expr.Expression_Kind == An_Identifier);

    SgDeclarationStatement* dcl = findFirst( asisDecls(), expr.Corresponding_Name_Definition, expr.Corresponding_Name_Declaration);
    SgScopeStatement*       res = sg::dispatch(ScopeQuery(), dcl);

    return SG_DEREF(res);
  }


  /// returns a NameData object for the element \ref el
  NameData
  getNameID(Element_ID el, AstContext ctx);

  /// asserts that \ref decl has exactly one associated name
  /// and returns the respective NameData data.
  NameData
  singleName(Declaration_Struct& decl, AstContext ctx)
  {
    ElemIdRange     range = idRange(decl.Names);
    ROSE_ASSERT(range.size() == 1);

    return getNameID(*range.first, ctx);
  }

  /// returns the NameData object for a name that is represented
  /// as expression in Asis (e.g., identifier or selected)
  NameData
  getExprName(Element_Struct& elem, AstContext ctx)
  {
    ROSE_ASSERT(elem.Element_Kind == An_Expression);

    Expression_Struct& idex  = elem.The_Union.Expression;

    if (idex.Expression_Kind == An_Identifier)
    {
      std::string        ident{idex.Name_Image};

      return NameData{ ident, ident, &ctx.scope(), &elem };
    }

    ROSE_ASSERT(idex.Expression_Kind == A_Selected_Component);

    NameData compound = getNameID(idex.Prefix, ctx);
    NameData selected = getNameID(idex.Selector, ctx);

    return NameData{ selected.ident,
                     compound.fullName + "." + selected.fullName,
                     &ctx.scope(),
                     selected.asisElem
                   };
  }


  NameData
  getName(Element_Struct& elem, AstContext ctx)
  {
    if (elem.Element_Kind == An_Expression)
      return getExprName(elem, ctx);

    if (elem.Element_Kind != A_Defining_Name)
      logWarn() << "unexpected elem kind (getName)" << elem.Element_Kind << std::endl;

    ROSE_ASSERT(elem.Element_Kind == A_Defining_Name);

    Defining_Name_Struct& asisname = elem.The_Union.Defining_Name;
    SgScopeStatement*     parent = &ctx.scope();
    std::string           ident{asisname.Defining_Name_Image};
    std::string           name{asisname.Defining_Name_Image};

    switch (asisname.Defining_Name_Kind)
    {
      case A_Defining_Expanded_Name:
        {
          NameData        defname    = getNameID(asisname.Defining_Selector, ctx);
          Element_Struct& prefixelem = retrieveAs<Element_Struct>(elemMap(), asisname.Defining_Prefix);

          ident  = defname.ident;
          parent = &getScope(prefixelem, ctx);
          break;
        }

      case A_Defining_Identifier:
        // nothing to do, the fields are already set
        break;

      case Not_A_Defining_Name:
        /* break; */

      case A_Defining_Character_Literal:   // 3.5.1(4)
      case A_Defining_Enumeration_Literal: // 3.5.1(3)
      case A_Defining_Operator_Symbol:     // 6.1(9)
      default:
        logWarn() << "unknown name kind " << asisname.Defining_Name_Kind
                  << " (" << name << ")"
                  << std::endl;
        ROSE_ASSERT(!FAIL_ON_ERROR);
    }

    return NameData{ ident, name, parent, &elem };
  }


  NameData
  getNameID(Element_ID el, AstContext ctx)
  {
    return getName(retrieveAs<Element_Struct>(elemMap(), el), ctx);
  }


  /// if \ref isPrivate \ref dcl's accessibility is set to private;
  /// otherwise nothing.
  void
  privatize(SgDeclarationStatement& dcl, bool isPrivate)
  {
    if (!isPrivate) return;

    dcl.get_declarationModifier().get_accessModifier().setPrivate();
  }

  /// labels a statement with a block label or a sequence of labels (if needed)
  /// @{
  SgStatement&
  labelIfNeeded(SgStatement& stmt, std::string lblname, Defining_Name_ID lblid, AstContext ctx)
  {
    ROSE_ASSERT(lblid > 0);

    SgNode&           parent  = SG_DEREF(stmt.get_parent());
    SgLabelStatement& sgn     = mkLabelStmt(lblname, stmt, ctx.scope());
    Element_Struct&   lblelem = retrieveAs<Element_Struct>(elemMap(), lblid);

    //~ copyFileInfo(stmt, sgn);
    attachSourceLocation(sgn, lblelem);
    sgn.set_parent(&parent);
    ctx.labels().label(lblid, sgn);

    ROSE_ASSERT(stmt.get_parent() == &sgn);
    return sgn;
  }

  std::string
  getLabelName(Element_ID el, AstContext ctx)
  {
    return getNameID(el, ctx).fullName;
  }

  template <class SageAdaStmt>
  SgStatement&
  labelIfNeeded(SageAdaStmt& stmt, Defining_Name_ID lblid, AstContext ctx)
  {
    ROSE_ASSERT(lblid >= 0);

    if (!lblid) return stmt;

    std::string lblname = getLabelName(lblid, ctx);

    stmt.set_string_label(lblname);
    return labelIfNeeded(stmt, lblname, lblid, ctx);
  }

  SgStatement&
  labelIfNeeded(SgStatement& stmt, Statement_Struct& adastmt, AstContext ctx)
  {
    typedef NameCreator::result_container name_container;

    ElemIdRange    range  = idRange(adastmt.Label_Names);
    name_container names  = traverseIDs(range, elemMap(), NameCreator{ctx});
    SgStatement*   sgnode = std::accumulate( names.rbegin(), names.rend(),
                                             &stmt,
                                             [&](SgStatement* labeled, const NameData& el) -> SgStatement*
                                             {
                                               ROSE_ASSERT(el.fullName == el.ident);
                                               return &labelIfNeeded(SG_DEREF(labeled), el.fullName, el.id(), ctx);
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
    ROSE_ASSERT(elem.Element_Kind == A_Statement);

    attachSourceLocation(sgnode, elem);
    sgnode.set_parent(&ctx.scope());

    Statement_Struct& stmt = elem.The_Union.Statement;
    SgStatement&      sgn  = labelIfNeeded(sgnode, stmt, ctx);

    ctx.scope().append_statement(&sgn);
    ROSE_ASSERT(sgn.get_parent() == &ctx.scope());
  }

  template <class SageAdaStmt>
  void
  completeStmt(SageAdaStmt& sgnode, Element_Struct& elem, AstContext ctx, Defining_Name_ID lblid)
  {
    ROSE_ASSERT(elem.Element_Kind == A_Statement);

    attachSourceLocation(sgnode, elem);
    sgnode.set_parent(&ctx.scope());

    SgStatement&      sgn0 = labelIfNeeded(sgnode, lblid, ctx);
    Statement_Struct& stmt = elem.The_Union.Statement;
    SgStatement&      sgn  = labelIfNeeded(sgn0, stmt, ctx);

    ctx.scope().append_statement(&sgn);
    ROSE_ASSERT(sgn.get_parent() == &ctx.scope());
  }

  /// @}

  typedef std::pair<SgTryStmt*, std::reference_wrapper<SgBasicBlock> > TryBlockNodes;

  /// if handlers are present, create an inner try block
  /// \returns a pair where second indicates the current block
  ///          and first a try (can be nullptr if no handlers exist)
  TryBlockNodes
  createTryBlockIfNeeded(bool hasHandlers, SgBasicBlock& outer)
  {
    if (!hasHandlers) return TryBlockNodes{nullptr, outer};

    SgBasicBlock& tryBlock = mkBasicBlock();
    SgTryStmt&    tryStmt  = mkTryStmt(tryBlock);

    //~ link_parent_child(outer, as<SgStatement>(tryStmt), SgBasicBlock::append_statement);
    outer.append_statement(&tryStmt);
    ROSE_ASSERT(tryStmt.get_parent() == &outer);

    return TryBlockNodes{&tryStmt, tryBlock};
  }

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
  cloneNonNull(SgExpression* exp)
  {
    return exp ? si::deepCopy(exp) : exp;
  }


  /// creates a sequence of initialized names for all names \ref names.
  /// \param m        a map that maintains mappings between Asis IDs and variables/parameters
  /// \param names    the list of Asis names
  /// \param dcltype  the type of all initialized name
  /// \param initexpr the initializer (if it exists) that will be cloned for each
  ///                 of the initialized names.
  SgInitializedNamePtrList
  constructInitializedNamePtrList( std::map<int, SgInitializedName*>& m,
                                   const NameCreator::result_container& names,
                                   SgType& dcltype,
                                   GuardedExpression initexpr = nullptr
                                 )
  {
    SgInitializedNamePtrList lst;
    int                      num      = names.size();

    for (int i = 0; i < num; ++i)
    {
      ROSE_ASSERT(names.at(i).fullName == names.at(i).ident);

      const std::string& name = names.at(i).fullName;
      Element_ID         id   = names.at(i).id();
      SgInitializedName& dcl  = mkInitializedName(name, dcltype, cloneNonNull(initexpr.get()));

      attachSourceLocation(dcl, retrieveAs<Element_Struct>(elemMap(), id));

      lst.push_back(&dcl);
      recordNonUniqueNode(m, id, dcl);
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
        res.setDefault();
        break;

      case An_In_Mode:
        res.setIntent_in();
        break;

      case An_Out_Mode:
        res.setIntent_out();
        break;

      case An_In_Out_Mode:
        res.setIntent_inout();
        break;

      case Not_A_Mode: /* break; */
      default:
        ROSE_ASSERT(false);
        break;
    }

    return res;
  }



  /// creates a ROSE expression for an Asis declaration's initializer expression
  /// returns null, if no declaration exists.
  SgExpression*
  getVarInit(Declaration_Struct& decl, AstContext ctx)
  {
    if (decl.Declaration_Kind == A_Deferred_Constant_Declaration)
      return nullptr;

    ROSE_ASSERT(  decl.Declaration_Kind == A_Variable_Declaration
               || decl.Declaration_Kind == A_Constant_Declaration
               || decl.Declaration_Kind == A_Parameter_Specification
               || decl.Declaration_Kind == A_Real_Number_Declaration
               || decl.Declaration_Kind == An_Integer_Number_Declaration
               || decl.Declaration_Kind == A_Component_Declaration
               );

    //~ logWarn() << "decl.Initialization_Expression = " << decl.Initialization_Expression << std::endl;
    if (decl.Initialization_Expression == 0)
      return nullptr;

    return &getExprID_opt(decl.Initialization_Expression, ctx);
  }

  SgType&
  getVarType(Declaration_Struct& decl, AstContext ctx)
  {
    ROSE_ASSERT(  decl.Declaration_Kind == A_Variable_Declaration
               || decl.Declaration_Kind == A_Constant_Declaration
               || decl.Declaration_Kind == A_Component_Declaration
               || decl.Declaration_Kind == A_Deferred_Constant_Declaration
               );

    return getDefinitionTypeID(decl.Object_Declaration_View, ctx);
  }


  /// converts an Asis parameter declaration to a ROSE paramter (i.e., variable)
  ///   declaration.
  SgVariableDeclaration&
  getParm(Element_Struct& elem, AstContext ctx)
  {
    typedef NameCreator::result_container name_container;

    ROSE_ASSERT(elem.Element_Kind == A_Declaration);

    Declaration_Struct&      asisDecl = elem.The_Union.Declaration;
    ROSE_ASSERT(asisDecl.Declaration_Kind == A_Parameter_Specification);

    // SgType&                   dcltype = tyModifier(getVarType(decl, ctx));
    ElemIdRange              range    = idRange(asisDecl.Names);
    name_container           names    = traverseIDs(range, elemMap(), NameCreator{ctx});
    SgType&                  parmtype = getDeclTypeID(asisDecl.Object_Declaration_View, ctx);
    GuardedExpression        initexpr{getVarInit(asisDecl, ctx)};
    SgInitializedNamePtrList dclnames = constructInitializedNamePtrList(asisVars(), names, parmtype, std::move(initexpr));
    SgVariableDeclaration&   sgnode   = mkParameter(dclnames, getMode(asisDecl.Mode_Kind), ctx.scope());

    attachSourceLocation(sgnode, elem);
    /* unused fields:
         bool                           Has_Aliased;
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

  /// call-back to complete a function/procedure/entry declarations
  ///   by adding parameters to the scopes (after they have been created)
  struct ParameterCompletion
  {
      ParameterCompletion(ElemIdRange paramrange, AstContext astctx)
      : range(paramrange), ctx(astctx)
      {}

      void operator()(SgFunctionParameterList& lst, SgScopeStatement& parmscope)
      {
        traverseIDs(range, elemMap(), ParmlistCreator{lst, ctx.scope(parmscope)});
      }

    private:
      ElemIdRange range;
      AstContext  ctx;

      ParameterCompletion() = delete;
  };


  /// creates a sequence of type declarations in ROSE from a
  ///   a single Asis declarations with multiple names.
  struct DeclarePrivateType
  {
      DeclarePrivateType(SgType& tyrep, AstContext astctx, bool privateItems)
      : ty(tyrep), scope(astctx.scope()), privateElems(privateItems)
      {}

      void operator()(const NameData& nameelem)
      {
        ROSE_ASSERT(nameelem.fullName == nameelem.ident);

        const std::string&      name = nameelem.fullName;
        Element_ID              id   = nameelem.id();
        SgDeclarationStatement* dcl  = &mkTypeDecl(name, ty, scope);
        ROSE_ASSERT(dcl);

        privatize(*dcl, privateElems);
        scope.append_statement(dcl);
        recordNode(asisTypes(), id, *dcl);
        ROSE_ASSERT(dcl->get_parent() == &scope);
      }

    private:
      SgType&           ty;
      SgScopeStatement& scope;
      bool              privateElems;
  };


  /// creates a ROSE declaration depending on the provided type/definition
  struct MakeDeclaration : sg::DispatchHandler<SgDeclarationStatement*>
  {
      typedef sg::DispatchHandler<SgDeclarationStatement*> base;

      MakeDeclaration(const std::string& name, SgScopeStatement& scope, TypeData basis)
      : base(), dclname(name), dclscope(scope), foundation(basis)
      {}

      void handle(SgNode& n) { SG_UNEXPECTED_NODE(n); }
      void handle(SgType& n) { res = &mkTypeDecl(dclname, n, dclscope); }

      void handle(SgClassDefinition& n)
      {
        SgClassDeclaration&    rec = mkRecordDecl(dclname, n, dclscope);
        SgDeclarationModifier& mod = rec.get_declarationModifier();

        if (foundation.hasAbstract) mod.setAdaAbstract();
        if (foundation.hasLimited)  mod.setAdaLimited();
        if (foundation.hasTagged)   mod.setAdaTagged();

        res = &rec;
      }

    private:
      std::string       dclname;
      SgScopeStatement& dclscope;
      TypeData   foundation;
  };


  /// creates a sequence of type declarations in ROSE from a
  ///   a single Asis declarations with multiple names.
  /// \note the created type depends on data provided in the TypeData object.
  struct DeclareType
  {
      DeclareType(TypeData what, AstContext astctx, bool privateItems)
      : foundation(what), scope(astctx.scope()), privateElems(privateItems)
      {}

      void operator()(const NameData& nameelem)
      {
        ROSE_ASSERT(nameelem.fullName == nameelem.ident);

        const std::string&      name = nameelem.fullName;
        Element_ID              id   = nameelem.id();
        SgDeclarationStatement* dcl = sg::dispatch(MakeDeclaration(name, scope, foundation), foundation.n);
        ROSE_ASSERT(dcl);

        privatize(*dcl, privateElems);
        scope.append_statement(dcl);
        recordNode(asisTypes(), id, *dcl);
        ROSE_ASSERT(dcl->get_parent() == &scope);
      }

    private:
      TypeData   foundation;
      SgScopeStatement& scope;
      bool              privateElems;

      DeclareType() = delete;
  };


/*
  needed?

  struct DeclareTaskType
  {
    DeclareTaskType(SgAdaTaskSpec& taskspec, AstContext astctx, bool privateItems)
    : spec(taskspec), scope(astctx.scope()), privateElems(privateItems)
    {}

    void operator()(const NameData& nameelem)
    {
      const std::string& name = nameelem.first;
      Element_ID         id   = nameelem.second;
      SgAdaTaskTypeDecl& dcl  = mkAdaTaskTypeDecl(name, spec);

      privatize(dcl, privateElems);
      scope.append_statement(&dcl);
      recordNode(asisTypes(), id, dcl);
    }

    SgAdaTaskSpec&    spec;
    SgScopeStatement& scope;
    bool              privateElems;
  };
*/


  typedef SgType& (*TypeModiferFn) (SgType&);

  SgType& tyIdentity(SgType& ty) { return ty; }

  SgType& tyConstify(SgType& ty) { return SG_DEREF(sb::buildConstType(&ty)); }


  //
  // helper function for combined handling of variables and constant declarations

  void
  handleNumVarCstDecl(Declaration_Struct& decl, AstContext ctx, bool isPrivate, SgType& dcltype, Element_Struct& elem)
  {
    typedef NameCreator::result_container name_container;

    ElemIdRange              range    = idRange(decl.Names);
    name_container           names    = traverseIDs(range, elemMap(), NameCreator{ctx});
    SgScopeStatement&        scope    = ctx.scope();
    GuardedExpression        initexp{getVarInit(decl, ctx)};
    SgInitializedNamePtrList dclnames = constructInitializedNamePtrList(asisVars(), names, dcltype, std::move(initexp));
    SgVariableDeclaration&   vardcl   = mkVarDecl(dclnames, scope);

    attachSourceLocation(vardcl, elem);
    privatize(vardcl, isPrivate);
    scope.append_statement(&vardcl);

    ROSE_ASSERT(vardcl.get_parent() == &scope);
  }

  void
  handleNumberDecl(Declaration_Struct& decl, AstContext ctx, bool isPrivate, SgType& numty, Element_Struct& elem)
  {
    SgType& cstty = SG_DEREF( sb::buildConstType(&numty) );

    handleNumVarCstDecl(decl, ctx, isPrivate, cstty, elem);
  }

  void
  handleVarCstDecl(Declaration_Struct& dcl, AstContext ctx, bool isPrivate, TypeModiferFn tyModifier, Element_Struct& elem)
  {
    handleNumVarCstDecl(dcl, ctx, isPrivate, tyModifier(getVarType(dcl, ctx)), elem);
  }

  SgDeclarationStatement&
  getAliasedID(Element_ID declid, AstContext ctx);

  SgDeclarationStatement&
  getAliased(Expression_Struct& expr, AstContext ctx)
  {
    if (expr.Expression_Kind == An_Identifier)
      return SG_DEREF(getDecl_opt(expr, ctx));

    ROSE_ASSERT(expr.Expression_Kind == A_Selected_Component);
    return getAliasedID(expr.Selector, ctx);
  }

  SgDeclarationStatement&
  getAliasedID(Element_ID declid, AstContext ctx)
  {
    Element_Struct& elem = retrieveAs<Element_Struct>(elemMap(), declid);
    ROSE_ASSERT(elem.Element_Kind == An_Expression);

    return getAliased(elem.The_Union.Expression, ctx);
  }


  SgAdaTaskBody&
  getTaskBody(Declaration_Struct& decl, AstContext ctx)
  {
    ROSE_ASSERT(decl.Declaration_Kind == A_Task_Body_Declaration);

    SgAdaTaskBody& sgnode = mkAdaTaskBody();
    ElemIdRange    decls = idRange(decl.Body_Declarative_Items);
    ElemIdRange    stmts = idRange(decl.Body_Statements);

    traverseIDs(decls, elemMap(), StmtCreator{ctx.scope_npc(sgnode)});
    traverseIDs(stmts, elemMap(), StmtCreator{ctx.scope_npc(sgnode)});

    return sgnode;
  }

  SgAdaTaskSpec&
  getTaskSpec(Declaration_Struct& decl, AstContext ctx)
  {
    ROSE_ASSERT(decl.Declaration_Kind == A_Task_Type_Declaration);

    SgAdaTaskSpec&          sgnode = mkAdaTaskSpec();

    if (decl.Type_Declaration_View == 0)
      return sgnode;

    sgnode.set_hasMembers(true);

    // Definition_ID Discriminant_Part
    Element_Struct&         elem = retrieveAs<Element_Struct>(elemMap(), decl.Type_Declaration_View);
    ROSE_ASSERT(elem.Element_Kind == A_Definition);

    Definition_Struct&      def = elem.The_Union.Definition;
    ROSE_ASSERT(def.Definition_Kind == A_Task_Definition);

    Task_Definition_Struct& tasknode = def.The_Union.The_Task_Definition;

    // visible items
    {
      ElemIdRange range = idRange(tasknode.Visible_Part_Items);

      traverseIDs(range, elemMap(), ElemCreator{ctx.scope_npc(sgnode)});
    }

    // private items
    {
      ElemIdRange range = idRange(tasknode.Private_Part_Items);
      ROSE_ASSERT((!range.empty()) == tasknode.Is_Private_Present);

      traverseIDs(range, elemMap(), ElemCreator{ctx.scope_npc(sgnode), true /* private items */});
    }

    /* unused fields: (Task_Definition_Struct)
         bool                  Has_Task;
    */
    return sgnode;
  }


  struct CaseStmtCreator
  {
      typedef std::pair<SgExpression*, SgStatement*> branch_type;
      typedef std::vector<branch_type>               branch_container;

      CaseStmtCreator(AstContext astctx, SgSwitchStatement& caseStmt)
      : ctx(astctx), caseNode(caseStmt)
      {}

      void operator()(Element_Struct& elem)
      {
        Path_Struct& path = elem.The_Union.Path;

        switch (path.Path_Kind)
        {
          case A_Case_Path:
            {
              ElemIdRange                caseChoices = idRange(path.Case_Path_Alternative_Choices);
              std::vector<SgExpression*> choices     = traverseIDs(caseChoices, elemMap(), ExprSeqCreator{ctx});
              ElemIdRange                caseBlock   = idRange(path.Sequence_Of_Statements);

              // \todo reconsider the "reuse" of SgCommaOp
              //   SgCommaOp is only used to separate discrete choices in case-when
              ROSE_ASSERT(choices.size());
              SgExpression&              caseCond    = SG_DEREF( std::accumulate( choices.begin()+1, choices.end(),
                                                                                  choices.front(),
                                                                                  sb::buildCommaOpExp
                                                                                ));

              SgBasicBlock&              block       = mkBasicBlock();
              SgCaseOptionStmt*          sgnode      = sb::buildCaseOptionStmt(&caseCond, &block);

              attachSourceLocation(SG_DEREF(sgnode), elem);
              sgnode->set_has_fall_through(false);
              caseNode.append_case(sgnode);
              traverseIDs(caseBlock, elemMap(), StmtCreator{ctx.scope(block)});
              break;
            }

          default:
            ROSE_ASSERT(false);
        }
      }

    private:
      AstContext         ctx;
      SgSwitchStatement& caseNode; // \todo consider introducing SgAdaCaseStmt
  };

  struct IfStmtCreator
  {
      typedef std::pair<SgExpression*, SgStatement*> branch_type;
      typedef std::vector<branch_type>               branch_container;

      explicit
      IfStmtCreator(AstContext astctx)
      : ctx(astctx)
      {}

      void commonBranch(Path_Struct& path, SgExpression* cond = nullptr)
      {
        SgBasicBlock& block     = mkBasicBlock();
        ElemIdRange   thenStmts = idRange(path.Sequence_Of_Statements);

        traverseIDs(thenStmts, elemMap(), StmtCreator{ctx.scope_npc(block)});
        branches.emplace_back(cond, &block);
      }

      void conditionedBranch(Path_Struct& path)
      {
        commonBranch(path, &getExprID(path.Condition_Expression, ctx));
      }

      void operator()(Element_Struct& elem)
      {
        Path_Struct& path = elem.The_Union.Path;

        switch (path.Path_Kind)
        {
          case An_If_Path:
            {
              ROSE_ASSERT(branches.size() == 0);
              conditionedBranch(path);
              break;
            }

          case An_Elsif_Path:
            {
              ROSE_ASSERT(branches.size() != 0);
              conditionedBranch(path);
              break;
            }

          case An_Else_Path:
            {
              ROSE_ASSERT(branches.size() != 0);
              commonBranch(path);
              break;
            }

          default:
            ROSE_ASSERT(false);
        }
      }

      static
      SgStatement*
      createIfStmt(SgStatement* elsePath, branch_type thenPath)
      {
        ROSE_ASSERT(thenPath.first && thenPath.second);

        return sb::buildIfStmt(thenPath.first, thenPath.second, elsePath);
      }

      operator SgStatement&()
      {
        SgStatement* elseStmt = nullptr;

        if (!branches.back().first)
        {
          elseStmt = branches.back().second;
          branches.pop_back();
        }

        return SG_DEREF( std::accumulate(branches.rbegin(), branches.rend(), elseStmt, createIfStmt) );
      }

    private:
      AstContext       ctx;
      branch_container branches;

      IfStmtCreator() = delete;
  };



  void handleStmt(Element_Struct& elem, AstContext ctx)
  {
    logTrace() << "a statement (in progress) " << elem.Element_Kind << std::endl;

    // declarations are statements too
    if (elem.Element_Kind == A_Declaration)
    {
      handleDeclaration(elem, ctx);
      return;
    }

    ROSE_ASSERT(elem.Element_Kind == A_Statement);

    Statement_Struct& stmt = elem.The_Union.Statement;

    switch (stmt.Statement_Kind)
    {
      case A_Null_Statement:                    // 5.1
        {
          completeStmt(mkNullStmt(), elem, ctx);
          /* unused fields:
          */
          break;
        }

      case An_Assignment_Statement:             // 5.2
        {
          SgExpression& lhs    = getExprID(stmt.Assignment_Variable_Name, ctx);
          SgExpression& rhs    = getExprID(stmt.Assignment_Expression, ctx);
          SgExpression& assign = SG_DEREF(sb::buildAssignOp(&lhs, &rhs));
          SgStatement&  sgnode = SG_DEREF(sb::buildExprStatement(&assign));

          completeStmt(sgnode, elem, ctx);
          /* unused fields:
          */
          break;
        }

      case An_If_Statement:                     // 5.3
        {
          ElemIdRange  range  = idRange(stmt.Statement_Paths);
          SgStatement& sgnode = traverseIDs(range, elemMap(), IfStmtCreator{ctx});

          completeStmt(sgnode, elem, ctx);
          /* unused fields:
          */
          break;
        }

      case A_Case_Statement:                    // 5.4
        {
          SgExpression&      caseexpr = getExprID(stmt.Case_Expression, ctx);
          SgBasicBlock&      casebody = mkBasicBlock();
          SgSwitchStatement& sgnode   = mkAdaCaseStmt(caseexpr, casebody);
          ElemIdRange        range    = idRange(stmt.Statement_Paths);

          traverseIDs(range, elemMap(), CaseStmtCreator{ctx.scope(casebody), sgnode});
          completeStmt(sgnode, elem, ctx);
          /* unused fields:
          */
          break;
        }

      case A_While_Loop_Statement:              // 5.5
        {
          SgExpression& cond     = getExprID(stmt.While_Condition, ctx);
          SgBasicBlock& block    = mkBasicBlock();
          ElemIdRange   adaStmts = idRange(stmt.Loop_Statements);
          SgWhileStmt&  sgnode   = mkWhileStmt(cond, block);

          completeStmt(sgnode, elem, ctx, stmt.Statement_Identifier);

          recordNode(asisLoops(), elem.ID, sgnode);
          traverseIDs(adaStmts, elemMap(), StmtCreator{ctx.scope(block)});
          /* unused fields:
                Pragma_Element_ID_List    Pragmas;
                Element_ID                Corresponding_End_Name;
          */
          break;
        }

        case A_Loop_Statement:                    // 5.5
        {
          SgBasicBlock&  block    = mkBasicBlock();
          ElemIdRange    adaStmts = idRange(stmt.Loop_Statements);
          SgAdaLoopStmt& sgnode   = mkLoopStmt(block);

          completeStmt(sgnode, elem, ctx, stmt.Statement_Identifier);

          recordNode(asisLoops(), elem.ID, sgnode);
          traverseIDs(adaStmts, elemMap(), StmtCreator{ctx.scope(block)});

          /* unused fields:
                Pragma_Element_ID_List    Pragmas;
                Element_ID                Corresponding_End_Name;
                bool                      Is_Name_Repeated;
          */
          break;
        }

      case A_For_Loop_Statement:                // 5.5
        {
          SgBasicBlock&       block  = mkBasicBlock();
          SgForStatement&     sgnode = mkForStatement(block);
          Element_Struct&     forvar = retrieveAs<Element_Struct>(elemMap(), stmt.For_Loop_Parameter_Specification);
          SgForInitStatement& forini = SG_DEREF( sb::buildForInitStatement(sgnode.getStatementList()) );

          sg::linkParentChild(sgnode, forini, &SgForStatement::set_for_init_stmt);
          handleDeclaration(forvar, ctx.scope_npc(sgnode));
          completeStmt(sgnode, elem, ctx, stmt.Statement_Identifier);
          ROSE_ASSERT(sgnode.getStatementList().size() <= 1 /* \todo should be 0 */);

          // \todo this swap is needed, otherwise the variable declaration ends
          //       up in the body instead of the initializer.. ???
          std::swap(forini.get_init_stmt(), block.get_statements());

          ElemIdRange         loopStmts = idRange(stmt.Loop_Statements);

          recordNode(asisLoops(), elem.ID, sgnode);
          traverseIDs(loopStmts, elemMap(), StmtCreator{ctx.scope(block)});

          /* unused fields:
               Pragma_Element_ID_List Pragmas;
               Element_ID             Corresponding_End_Name;
          */
          break;
        }

      case A_Block_Statement:                   // 5.6
        {
          SgBasicBlock& sgnode   = mkBasicBlock();
          ElemIdRange   blkDecls = idRange(stmt.Block_Declarative_Items);
          ElemIdRange   blkStmts = idRange(stmt.Block_Statements);
          ElemIdRange   exHndlrs = idRange(stmt.Block_Exception_Handlers);
          logWarn() << "block ex handlers: " << exHndlrs.size() << std::endl;

          TryBlockNodes trydata  = createTryBlockIfNeeded(exHndlrs.size() > 0, sgnode);
          SgTryStmt*    tryblk   = trydata.first;
          SgBasicBlock& block    = trydata.second;

          completeStmt(block, elem, ctx, stmt.Statement_Identifier);
          traverseIDs(blkDecls, elemMap(), StmtCreator{ctx.scope(sgnode)});
          traverseIDs(blkStmts, elemMap(), StmtCreator{ctx.scope(block)});

          if (tryblk)
          {
            traverseIDs(exHndlrs, elemMap(), ExHandlerCreator{ctx.scope(sgnode), SG_DEREF(tryblk)});
          }

          /* unused fields:
                Pragma_Element_ID_List    Pragmas;
                Element_ID                Corresponding_End_Name;
                bool                      Is_Name_Repeated;
                bool                      Is_Declare_Block;
          */
          break;
        }

      case An_Exit_Statement:                   // 5.7
        {
          SgStatement&  exitedLoop    = lookupNode(asisLoops(), stmt.Corresponding_Loop_Exited);
          SgExpression& exitCondition = getExprID_opt(stmt.Exit_Condition, ctx);
          const bool    loopIsNamed   = stmt.Exit_Loop_Name > 0;
          SgStatement&  sgnode        = mkAdaExitStmt(exitedLoop, exitCondition, loopIsNamed);

          completeStmt(sgnode, elem, ctx);
          /* unused fields:
               Expression_ID          Exit_Loop_Name;
          */
          break;
        }

      case A_Goto_Statement:                    // 5.8
        {
          SgGotoStatement& sgnode = SG_DEREF( sb::buildGotoStatement() );

          ctx.labels().gotojmp(stmt.Goto_Label, sgnode);

          completeStmt(sgnode, elem, ctx);
          /* unused fields:
                Statement_ID           Corresponding_Destination_Statement;
          */
          break;
        }

      case A_Procedure_Call_Statement:          // 6.4
        {
          SgDeclarationStatement& tgt    = lookupNode(asisDecls(), stmt.Corresponding_Called_Entity);
          SgFunctionDeclaration*  fundcl = isSgFunctionDeclaration(&tgt);
          SgFunctionRefExp&       funref = SG_DEREF(sb::buildFunctionRefExp(fundcl));
          ElemIdRange             range  = idRange(stmt.Call_Statement_Parameters);
          SgExprListExp&          arglst = traverseIDs(range, elemMap(), ArgListCreator{ctx});
          SgStatement&            sgnode = SG_DEREF(sb::buildFunctionCallStmt(&funref, &arglst));

          completeStmt(sgnode, elem, ctx);
          /* unused fields:
              bool        Is_Prefix_Notation
              bool        Is_Dispatching_Call
              bool        Is_Call_On_Dispatching_Operation
              Element_ID  Called_Name
              Declaration Corresponding_Called_Entity_Unwound
          */
          break;
        }

      case A_Return_Statement:                  // 6.5
        {
          Element_Struct* exprel = retrieveAsOpt<Element_Struct>(elemMap(), stmt.Return_Expression);
          SgExpression*   retval = exprel ? &getExpr(*exprel, ctx) : NULL;
          SgStatement&    sgnode = SG_DEREF( sb::buildReturnStmt(retval) );

          completeStmt(sgnode, elem, ctx);
          /* unused fields:
          */
          break;
        }

      case An_Accept_Statement:                 // 9.5.2
        {
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
            SgStatement&          noblock = mkNullStmt();

            sg::linkParentChild(sgnode, noblock, &SgAdaAcceptStmt::set_body);
          }
          else
          {
            SgBasicBlock&         block   = mkBasicBlock();

            sg::linkParentChild(sgnode, as<SgStatement>(block), &SgAdaAcceptStmt::set_body);
            traverseIDs(stmts, elemMap(), StmtCreator{parmctx.scope(block)});
          }

          /* unused fields:
              Pragma_Element_ID_List       Pragmas;
              Element_ID                   Corresponding_End_Name;
              bool                         Is_Name_Repeated;
              Statement_List               Accept_Body_Exception_Handlers;
              Declaration_ID               Corresponding_Entry;
          */
          break;
        }

      case A_Raise_Statement:                   // 11.3
        {
          SgExpression&   raised = getExprID(stmt.Raised_Exception, ctx);
          SgStatement&    sgnode = mkRaiseStmt(raised);

          completeStmt(sgnode, elem, ctx);
          /* unused fields:
               Expression_ID          Associated_Message;
          */
          break;
        }


      //|A2005 start
      case An_Extended_Return_Statement:        // 6.5
      //|A2005 end

      case Not_A_Statement: /* break; */        // An unexpected element
      case An_Entry_Call_Statement:             // 9.5.3
      case A_Requeue_Statement:                 // 9.5.4
      case A_Requeue_Statement_With_Abort:      // 9.5.4
      case A_Delay_Until_Statement:             // 9.6
      case A_Delay_Relative_Statement:          // 9.6
      case A_Terminate_Alternative_Statement:   // 9.7.1
      case A_Selective_Accept_Statement:        // 9.7.1
      case A_Timed_Entry_Call_Statement:        // 9.7.2
      case A_Conditional_Entry_Call_Statement:  // 9.7.3
      case An_Asynchronous_Select_Statement:    // 9.7.4
      case An_Abort_Statement:                  // 9.8
      case A_Code_Statement:                    // 13.8 assembly
      default:
        logWarn() << "Unhandled statement " << stmt.Statement_Kind << std::endl;
        ROSE_ASSERT(!FAIL_ON_ERROR);
    }
  }


  /// returns NameData objects for all names associated with the declararion \ref id
  NameCreator::result_container
  queryDeclNames(Declaration_ID id, AstContext ctx)
  {
    if (id == 0) return NameCreator::result_container();

    Element_Struct& elem = retrieveAs<Element_Struct>(elemMap(), id);

    ROSE_ASSERT(elem.Element_Kind == A_Declaration);
    Declaration_Struct&      asisDecl = elem.The_Union.Declaration;

    ROSE_ASSERT((asisDecl.Declaration_Kind == A_Choice_Parameter_Specification));

    // SgType&                   dcltype = tyModifier(getVarType(decl, ctx));
    ElemIdRange              range    = idRange(asisDecl.Names);

    return traverseIDs(range, elemMap(), NameCreator{ctx});
  }

  void handleExceptionHandler(Element_Struct& elem, SgTryStmt& tryStmt, AstContext ctx)
  {
    typedef NameCreator::result_container name_container;

    ROSE_ASSERT(elem.Element_Kind == An_Exception_Handler);

    Exception_Handler_Struct& ex      = elem.The_Union.Exception_Handler;
    name_container            names   = queryDeclNames(ex.Choice_Parameter_Specification, ctx);

    if (names.size() == 0)
    {
      // add an unnamed exception handler
      names.emplace_back(std::string{}, std::string{}, &ctx.scope(), &elem);
    }

    ROSE_ASSERT (names.size() == 1);
    ElemIdRange              tyRange = idRange(ex.Exception_Choices);
    SgType&                  extypes = traverseIDs(tyRange, elemMap(), ExHandlerTypeCreator{ctx});
    SgInitializedNamePtrList lst     = constructInitializedNamePtrList(asisVars(), names, extypes);
    SgBasicBlock&            body    = mkBasicBlock();

    ROSE_ASSERT(lst.size() == 1);
    SgCatchOptionStmt&       sgnode  = mkExceptionHandler(SG_DEREF(lst[0]), body);
    ElemIdRange              range   = idRange(ex.Handler_Statements);

    logWarn() << "catch handler" << std::endl;
    sg::linkParentChild(tryStmt, as<SgStatement>(sgnode), &SgTryStmt::append_catch_statement);
    sgnode.set_trystmt(&tryStmt);

    traverseIDs(range, elemMap(), StmtCreator{ctx.scope(body)});

    /* unused fields:
         Pragma_Element_ID_List Pragmas;
    */
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
        ROSE_ASSERT (el.Element_Kind == An_Expression);
        //~ else if (el.Element_Kind == A_Definition)
          //~ res = &getDefinitionExpr(el.The_Union.Definition, ctx);

        NameData                   imported = getName(el, ctx);
        SgExpression&              res = mkUnresolvedName(imported.fullName, ctx.scope());
        std::vector<SgExpression*> elems{&res};
        SgImportStatement&         sgnode = mkWithClause(elems);

        recordNode(asisDecls(), el.ID, sgnode);
        attachSourceLocation(sgnode, el);
        ctx.scope().append_statement(&sgnode);

        // an imported element may not be in the AST
        //   -> add the mapping if the element has not been seen.
        Element_Struct&            asisElem = SG_DEREF(imported.asisElem);
        Expression_Struct&         asisExpr = asisElem.The_Union.Expression;
        Element_ID                 impID    = asisExpr.Corresponding_Name_Declaration;

        recordNonUniqueNode(asisDecls(), impID, sgnode);
        ROSE_ASSERT(sgnode.get_parent() == &ctx.scope());
      }

    private:
      AstContext ctx;

      WithClauseCreator() = delete;
  };

} // anonymous


SgDeclarationStatement*
getDecl_opt(Expression_Struct& expr, AstContext ctx)
{
  return findFirst(asisDecls(), expr.Corresponding_Name_Definition, expr.Corresponding_Name_Declaration);
}


void handleClause(Element_Struct& elem, AstContext ctx)
{
  Clause_Struct& clause = elem.The_Union.Clause;

  switch (clause.Clause_Kind)
  {
    case A_With_Clause:                // 10.1.2
      {
        ElemIdRange        range  = idRange(clause.Clause_Names);

        traverseIDs(range, elemMap(), WithClauseCreator{ctx});
        /* unused fields:
            bool   Has_Limited
        */
        break;
      }

    case Not_A_Clause: /* break; */    // An unexpected element
    case A_Use_Package_Clause:         // 8.4
    case A_Use_Type_Clause:            // 8.4
    case A_Use_All_Type_Clause:        // 8.4: Ada 2012
    case A_Representation_Clause:      // 13.1     -> Representation_Clause_Kinds
    case A_Component_Clause:           // 13.5.1
    default:
      logWarn() << "unhandled clause kind: " << clause.Clause_Kind << std::endl;
      ROSE_ASSERT(!FAIL_ON_ERROR);
  }
}

void handleDeclaration(Element_Struct& elem, AstContext ctx, bool isPrivate)
{
  ROSE_ASSERT(elem.Element_Kind == A_Declaration);

  Declaration_Struct& decl = elem.The_Union.Declaration;

  switch (decl.Declaration_Kind)
  {
    case A_Package_Declaration:                    // 7.1(2)
      {
        SgScopeStatement&     outer   = ctx.scope();
        NameData              adaname = singleName(decl, ctx);
        //~ SgAdaPackageSpecDecl& sgnode  = mkAdaPackageSpecDecl(adaname.ident, outer);
        SgAdaPackageSpecDecl& sgnode  = mkAdaPackageSpecDecl(adaname.ident, SG_DEREF(adaname.parent));
        SgAdaPackageSpec&     pkgspec = SG_DEREF(sgnode.get_definition());

        ROSE_ASSERT(adaname.parent);
        sgnode.set_scope(adaname.parent);
        //~ sgnode.set_scope(&outer);
        logTrace() << "package decl " << adaname.ident
                   << " (" <<  adaname.fullName << ")"
                   << std::endl;

        recordNode(asisDecls(), elem.ID, sgnode);
        //~ recordNode(asisDecls(), adaname.id(), sgnode);

        privatize(sgnode, isPrivate);
        attachSourceLocation(sgnode, elem);
        outer.append_statement(&sgnode);
        ROSE_ASSERT(sgnode.get_parent() == &outer);

        ROSE_ASSERT(sgnode.search_for_symbol_from_symbol_table());

        // visible items
        {
          ElemIdRange range = idRange(decl.Visible_Part_Declarative_Items);

          traverseIDs(range, elemMap(), ElemCreator{ctx.scope(pkgspec)});
        }

        // private items
        {
          ElemIdRange range = idRange(decl.Private_Part_Declarative_Items);
          ROSE_ASSERT((!range.empty()) == decl.Is_Private_Present);

          traverseIDs(range, elemMap(), ElemCreator{ctx.scope(pkgspec), true /* private items */});
        }

        /* unused nodes:
               Pragma_Element_ID_List         Pragmas;
               Element_ID                     Corresponding_End_Name;
               bool                           Is_Name_Repeated;
               Declaration_ID                 Corresponding_Declaration;
         */
        break;
      }

    case A_Package_Body_Declaration:               // 7.2(2)
      {
        logTrace() << "package body decl" << std::endl;

        SgScopeStatement&     outer   = ctx.scope();
        Element_ID            specID  = decl.Corresponding_Declaration;
        SgAdaPackageSpecDecl& specdcl = lookupNodeAs<SgAdaPackageSpecDecl>(asisDecls(), specID);
        SgAdaPackageBodyDecl& sgnode  = mkAdaPackageBodyDecl(specdcl, outer);
        SgAdaPackageBody&     pkgbody = SG_DEREF(sgnode.get_definition());

        sgnode.set_scope(specdcl.get_scope());
        outer.append_statement(&sgnode);
        ROSE_ASSERT(sgnode.get_parent() == &outer);

        // declarative items
        {
          ElemIdRange range = idRange(decl.Body_Declarative_Items);

          traverseIDs(range, elemMap(), ElemCreator{ctx.scope(pkgbody)});
        }

        // statements
        {
          ElemIdRange range = idRange(decl.Body_Statements);

          traverseIDs(range, elemMap(), ElemCreator{ctx.scope(pkgbody)});
        }

        /*
         * unused nodes:
               Pragma_Element_ID_List         Pragmas;
               Element_ID                     Corresponding_End_Name;
               Exception_Handler_List         Body_Exception_Handlers;
               Declaration_ID                 Body_Block_Statement;
               bool                           Is_Name_Repeated;
               bool                           Is_Subunit;
               Declaration_ID                 Corresponding_Body_Stub;
         */
        break;
      }

    case A_Function_Declaration:                   // 6.1(4)   -> Trait_Kinds
    case A_Procedure_Declaration:                  // 6.1(4)   -> Trait_Kinds
      {
        logTrace() << (decl.Declaration_Kind == A_Function_Declaration ? "function" : "procedure") << " decl"
                   << std::endl;

        const bool             isFunc  = decl.Declaration_Kind == A_Function_Declaration;
        SgScopeStatement&      outer   = ctx.scope();
        NameData               adaname = singleName(decl, ctx);
        ElemIdRange            range   = idRange(decl.Parameter_Profile);
        SgType&                rettype = isFunc ? getDeclTypeID(decl.Result_Profile, ctx)
                                                : SG_DEREF(sb::buildVoidType());

        ROSE_ASSERT(adaname.fullName == adaname.ident);
        SgFunctionDeclaration& sgnode  = mkProcedure(adaname.fullName, outer, rettype, ParameterCompletion{range, ctx});

        setOverride(sgnode.get_declarationModifier(), decl.Is_Overriding_Declaration);
        recordNode(asisDecls(), elem.ID, sgnode);
        //~ recordNode(asisDecls(), adaname.id(), sgnode);

        privatize(sgnode, isPrivate);
        attachSourceLocation(sgnode, elem);
        outer.append_statement(&sgnode);
        ROSE_ASSERT(sgnode.get_parent() == &outer);

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

    case A_Function_Body_Declaration:              // 6.3(2)
    case A_Procedure_Body_Declaration:             // 6.3(2)
      {
        logTrace() << (decl.Declaration_Kind == A_Function_Body_Declaration ? "function" : "procedure") << " body decl"
                   << std::endl;

        const bool             isFunc  = decl.Declaration_Kind == A_Function_Body_Declaration;
        SgScopeStatement&      outer   = ctx.scope();
        NameData               adaname = singleName(decl, ctx);
        ElemIdRange            params  = idRange(decl.Parameter_Profile);
        SgType&                rettype = isFunc ? getDeclTypeID(decl.Result_Profile, ctx)
                                                : SG_DEREF(sb::buildVoidType());

        ROSE_ASSERT(adaname.fullName == adaname.ident);
        SgFunctionDeclaration& sgnode  = mkProcedureDef(adaname.fullName, outer, rettype, ParameterCompletion{params, ctx});
        SgBasicBlock&          declblk = getFunctionBody(sgnode);

        recordNode(asisDecls(), elem.ID, sgnode);
        //~ recordNode(asisDecls(), adaname.id(), sgnode);
        privatize(sgnode, isPrivate);
        attachSourceLocation(sgnode, elem);
        outer.append_statement(&sgnode);
        ROSE_ASSERT(sgnode.get_parent() == &outer);

        ElemIdRange            hndlrs  = idRange(decl.Body_Exception_Handlers);
        logWarn() << "block ex handlers: " << hndlrs.size() << std::endl;

        TryBlockNodes          trydata = createTryBlockIfNeeded(hndlrs.size() > 0, declblk);
        SgTryStmt*             trystmt = trydata.first;
        SgBasicBlock&          stmtblk = trydata.second;

        {
          ElemIdRange range = idRange(decl.Body_Declarative_Items);

          traverseIDs(range, elemMap(), ElemCreator{ctx.scope(declblk)});
        }

        {
          LabelManager lblmgr;
          ElemIdRange  range = idRange(decl.Body_Statements);

          traverseIDs(range, elemMap(), StmtCreator{ctx.scope(stmtblk).labels(lblmgr)});
        }

        if (trystmt)
        {
          traverseIDs(hndlrs, elemMap(), ExHandlerCreator{ctx.scope(declblk), SG_DEREF(trystmt)});
        }

        /* unhandled field
           Declaration_ID                 Body_Block_Statement;

         +func:
           bool                           Is_Not_Null_Return

           break;
        */
        break;
      }

    case A_Private_Type_Declaration:               // 3.2.1(2):7.3(2) -> Trait_Kinds
      {
        typedef NameCreator::result_container name_container;

        logTrace() << "Private Type "
                   << "\n  abstract: " << decl.Has_Abstract
                   << "\n  limited: " << decl.Has_Limited
                   << "\n  private: " << decl.Has_Private
                   << std::endl;
        ElemIdRange     range  = idRange(decl.Names);
        name_container  names  = traverseIDs(range, elemMap(), NameCreator{ctx});
        SgType&         opaque = mkDefaultType();

        ROSE_ASSERT(ctx.scope().get_parent());
        std::for_each(names.begin(), names.end(), DeclarePrivateType{opaque, ctx, isPrivate});

        /*
          bool                           Has_Abstract;
          bool                           Has_Limited;
          bool                           Has_Private;
          Definition_ID                  Discriminant_Part;
          Definition_ID                  Type_Declaration_View;
          Declaration_ID                 Corresponding_Type_Declaration;
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
        typedef NameCreator::result_container name_container;

        logTrace() << "Ordinary Type "
                   << "\n  abstract: " << decl.Has_Abstract
                   << "\n  limited: " << decl.Has_Limited
                   << std::endl;
        ElemIdRange    range = idRange(decl.Names);
        name_container names = traverseIDs(range, elemMap(), NameCreator{ctx});
        TypeData       ty    = getTypeFoundation(decl, ctx);

        ROSE_ASSERT(ctx.scope().get_parent());
        std::for_each(names.begin(), names.end(), DeclareType{ty, ctx, isPrivate});

        /* unused fields
            bool                           Has_Abstract;
            bool                           Has_Limited;
            Definition_ID                  Discriminant_Part;
            Declaration_ID                 Corresponding_Type_Declaration;
            Declaration_ID                 Corresponding_Type_Partial_View;
            Declaration_ID                 Corresponding_First_Subtype;
            Declaration_ID                 Corresponding_Last_Constraint;
            Declaration_ID                 Corresponding_Last_Subtype;
        */
        break;
      }


    //~ case A_Subtype_Declaration:                    // 3.2.2(2)
      //~ {

        //~ /* unused fields:
              //~ Declaration_ID                 Corresponding_First_Subtype;
              //~ Declaration_ID                 Corresponding_Last_Constraint;
              //~ Declaration_ID                 Corresponding_Last_Subtype;
        //~ */
        //~ break ;
      //~ }

    case A_Variable_Declaration:                   // 3.3.1(2) -> Trait_Kinds
      {
        handleVarCstDecl(decl, ctx, isPrivate, tyIdentity, elem);
        /* unused fields:
             bool                           Has_Aliased;
        */
        break;
      }

    case An_Integer_Number_Declaration:            // 3.3.2(2)
      {
        handleNumberDecl(decl, ctx, isPrivate, SG_DEREF(sb::buildIntType()), elem);

        /* unused fields:
        */
        break;
      }

    case A_Deferred_Constant_Declaration:          // 3.3.1(6):7.4(2) -> Trait_Kinds
    case A_Constant_Declaration:
      {
        handleVarCstDecl(decl, ctx, isPrivate, tyConstify, elem);
        /* unused fields:
             bool                           Has_Aliased;
        */
        break;
      }

    case A_Real_Number_Declaration:                // 3.5.6(2)
      {
        handleNumberDecl(decl, ctx, isPrivate, SG_DEREF(sb::buildFloatType()), elem);

        /* unused fields:
         */
        break;
      }

    case A_Loop_Parameter_Specification:           // 5.5(4)   -> Trait_Kinds
      {
        ROSE_ASSERT(!isPrivate);

        NameData               adaname = singleName(decl, ctx);
        ROSE_ASSERT(adaname.fullName == adaname.ident);

        SgType&                vartype = SG_DEREF( sb::buildIntType() ); // \todo
        SgInitializedName&     loopvar = mkInitializedName(adaname.fullName, vartype, nullptr);
        SgScopeStatement&      scope   = ctx.scope();

        recordNode(asisVars(), adaname.id(), loopvar);

        SgVariableDeclaration& sgnode  = mkVarDecl(loopvar, scope);

        attachSourceLocation(sgnode, elem);
        scope.append_statement(&sgnode);
        ROSE_ASSERT(sgnode.get_parent() == &scope);

        /* unused fields:
             bool                           Has_Reverse;
             Discrete_Subtype_Definition_ID Specification_Subtype_Definition;
         */
        break;
      }

    case A_Parameter_Specification:                // 6.1(15)  -> Trait_Kinds
      {
        // handled in getParm
        // break;
        ROSE_ASSERT(false);
      }

    case A_Task_Type_Declaration:                  // 9.1(2)
      {
        SgAdaTaskSpec&     spec    = getTaskSpec(decl, ctx);
        NameData           adaname = singleName(decl, ctx);
        ROSE_ASSERT(adaname.fullName == adaname.ident);
        SgAdaTaskTypeDecl& sgnode  = mkAdaTaskTypeDecl(adaname.fullName, spec, ctx.scope());

        attachSourceLocation(sgnode, elem);
        privatize(sgnode, isPrivate);
        ctx.scope().append_statement(&sgnode);
        ROSE_ASSERT(sgnode.get_parent() == &ctx.scope());
        recordNode(asisTypes(), adaname.id(), sgnode);
        recordNode(asisDecls(), adaname.id(), sgnode);

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

    case A_Task_Body_Declaration:                  // 9.1(6)
      {
        SgAdaTaskBody&          tskbody = getTaskBody(decl, ctx);
        NameData                adaname = singleName(decl, ctx);
        Element_ID              declID  = decl.Corresponding_Declaration;
        SgDeclarationStatement* tskdecl = findNode(asisDecls(), declID);
        ROSE_ASSERT(adaname.fullName == adaname.ident);

        // \todo \review not sure why a task body could be independently created
        SgAdaTaskBodyDecl&      sgnode  = tskdecl ? mkAdaTaskBodyDecl(*tskdecl, tskbody, ctx.scope())
                                                  : mkAdaTaskBodyDecl(adaname.fullName, tskbody, ctx.scope());

        attachSourceLocation(sgnode, elem);
        privatize(sgnode, isPrivate);
        ctx.scope().append_statement(&sgnode);
        ROSE_ASSERT(sgnode.get_parent() == &ctx.scope());
        //~ recordNode(asisDecls(), elem.ID, sgnode);
        recordNode(asisDecls(), adaname.id(), sgnode);

        /* unused fields:
             bool                           Has_Task;
             Pragma_Element_ID_List         Pragmas;
             Element_ID                     Corresponding_End_Name;
             Exception_Handler_List         Body_Exception_Handlers;
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
        NameData        adaname = singleName(decl, ctx);
        ElemIdRange     range   = idRange(decl.Parameter_Profile);

        ROSE_ASSERT(adaname.fullName == adaname.ident);
        SgAdaEntryDecl& sgnode  = mkAdaEntryDecl(adaname.fullName, ctx.scope(), ParameterCompletion{range, ctx});

        attachSourceLocation(sgnode, elem);
        privatize(sgnode, isPrivate);
        ctx.scope().append_statement(&sgnode);
        ROSE_ASSERT(sgnode.get_parent() == &ctx.scope());
        //~ recordNode(asisDecls(), elem.ID, sgnode);
        recordNode(asisDecls(), adaname.id(), sgnode);

        /* unused fields:
             bool                           Is_Overriding_Declaration;
             bool                           Is_Not_Overriding_Declaration;
             Declaration_ID                 Corresponding_Body;
             Discrete_Subtype_Definition_ID Entry_Family_Definition;
        */
        break;
      }

    case An_Exception_Declaration:                 // 11.1(2)
      {
        typedef NameCreator::result_container name_container;

        ElemIdRange              range    = idRange(decl.Names);
        name_container           names    = traverseIDs(range, elemMap(), NameCreator{ctx});
        SgScopeStatement&        scope    = ctx.scope();
        SgType&                  excty    = lookupNode(adaTypes(), std::string{"Exception"});
        SgInitializedNamePtrList dclnames = constructInitializedNamePtrList(asisExcps(), names, excty);
        SgVariableDeclaration&   sgnode   = mkExceptionDecl(dclnames, scope);

        attachSourceLocation(sgnode, elem);
        privatize(sgnode, isPrivate);
        scope.append_statement(&sgnode);
        ROSE_ASSERT(sgnode.get_parent() == &scope);
        break;
      }

    case A_Component_Declaration:                  // 3.8(6)
      {
        handleVarCstDecl(decl, ctx, isPrivate, tyIdentity, elem);
        /* unused clause:
              Pragma_Element_ID_List         Corresponding_Pragmas;
              Element_ID_List                Aspect_Specifications;
              Representation_Clause_List     Corresponding_Representation_Clauses;
              bool                           Has_Aliased;
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

    case A_Package_Renaming_Declaration:           // 8.5.3(2)
      {
        NameData                adaname = singleName(decl, ctx);

        if (decl.Renamed_Entity < 0)
        {
          logWarn() << "skipping unknown renaming: " << adaname.ident << "/" << adaname.fullName
                    << ": " << elem.ID << " / " << decl.Renamed_Entity
                    << std::endl;
          return;
        }

        SgDeclarationStatement& aliased = getAliasedID(decl.Renamed_Entity, ctx);
        SgScopeStatement&       scope   = ctx.scope();
        SgAdaRenamingDecl&      sgnode  = mkAdaRenamingDecl(adaname.ident, aliased, scope);

        attachSourceLocation(sgnode, elem);
        privatize(sgnode, isPrivate);
        scope.append_statement(&sgnode);
        ROSE_ASSERT(sgnode.get_parent() == &scope);
        /* unhandled field
             Declaration_ID                 Corresponding_Declaration;
             Expression_ID                  Corresponding_Base_Entity;
        */
        break;
      }

    case A_Choice_Parameter_Specification:         // 11.2(4)
      {
        // handled in handleExceptionHandler
        ROSE_ASSERT(false);
        break;
      }

    case Not_A_Declaration: /* break; */           // An unexpected element
    case A_Protected_Type_Declaration:             // 9.4(2)
    case An_Incomplete_Type_Declaration:           // 3.2.1(2):3.10(2)
    case A_Tagged_Incomplete_Type_Declaration:     //  3.10.1(2)
    case A_Private_Extension_Declaration:          // 3.2.1(2):7.3(3) -> Trait_Kinds
    case A_Single_Task_Declaration:                // 3.3.1(2):9.1(3)
    case A_Single_Protected_Declaration:           // 3.3.1(2):9.4(2)
    case An_Enumeration_Literal_Specification:     // 3.5.1(3)
    case A_Discriminant_Specification:             // 3.7(5)   -> Trait_Kinds
    case A_Generalized_Iterator_Specification:     // 5.5.2    -> Trait_Kinds
    case An_Element_Iterator_Specification:        // 5.5.2    -> Trait_Kinds
    case A_Return_Variable_Specification:          // 6.5
    case A_Return_Constant_Specification:          // 6.5
    case A_Null_Procedure_Declaration:             // 6.7
    case An_Object_Renaming_Declaration:           // 8.5.1(2)
    case An_Exception_Renaming_Declaration:        // 8.5.2(2)
    case A_Procedure_Renaming_Declaration:         // 8.5.4(2)
    case A_Function_Renaming_Declaration:          // 8.5.4(2)
    case A_Generic_Package_Renaming_Declaration:   // 8.5.5(2)
    case A_Generic_Procedure_Renaming_Declaration: // 8.5.5(2)
    case A_Generic_Function_Renaming_Declaration:  // 8.5.5(2)
    case A_Protected_Body_Declaration:             // 9.4(7)
    case An_Entry_Body_Declaration:                // 9.5.2(5)
    case An_Entry_Index_Specification:             // 9.5.2(2)
    case A_Procedure_Body_Stub:                    // 10.1.3(3)
    case A_Function_Body_Stub:                     // 10.1.3(3)
    case A_Package_Body_Stub:                      // 10.1.3(4)
    case A_Task_Body_Stub:                         // 10.1.3(5)
    case A_Protected_Body_Stub:                    // 10.1.3(6)
    case A_Generic_Procedure_Declaration:          // 12.1(2)
    case A_Generic_Function_Declaration:           // 12.1(2)
    case A_Generic_Package_Declaration:            // 12.1(2)
    case A_Package_Instantiation:                  // 12.3(2)
    case A_Procedure_Instantiation:                // 12.3(2)
    case A_Function_Instantiation:                 // 12.3(2)
    case A_Formal_Object_Declaration:              // 12.4(2)  -> Mode_Kinds
    case A_Formal_Type_Declaration:                // 12.5(2)
    case A_Formal_Incomplete_Type_Declaration:
    case A_Formal_Procedure_Declaration:           // 12.6(2)
    case A_Formal_Function_Declaration:            // 12.6(2)
    case A_Formal_Package_Declaration:             // 12.7(2)
    case A_Formal_Package_Declaration_With_Box:    // 12.7(3)
    default:
      logWarn() << "unhandled declaration kind: " << decl.Declaration_Kind << std::endl;
      ROSE_ASSERT(!FAIL_ON_ERROR);
  }
}

void StmtCreator::operator()(Element_Struct& elem)
{
  handleStmt(elem, ctx);
}



}
