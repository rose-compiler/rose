
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

    logKind("An_Identifier");

    SgDeclarationStatement* dcl = getDecl_opt(expr, ctx);

    if (dcl == nullptr)
    {
      logError() << "Unable to find scope/declaration for " << expr.Name_Image
                 << std::endl;
      ROSE_ASSERT(false);
    }

    SgScopeStatement*       res = sg::dispatch(ScopeQuery(), dcl);

    return SG_DEREF(res);
  }

  /// returns the NameData object for a name that is represented
  /// as expression in Asis (e.g., identifier or selected)
  NameData
  getQualName(Element_Struct& elem, AstContext ctx)
  {
    ROSE_ASSERT(elem.Element_Kind == An_Expression);

    Expression_Struct& idex  = elem.The_Union.Expression;

    if (idex.Expression_Kind == An_Identifier)
    {
      logKind("An_Identifier");

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
    // \todo can getName be distinguished from getQualName by the call context?
    if (elem.Element_Kind == An_Expression)
      return getQualName(elem, ctx);

    if (elem.Element_Kind != A_Defining_Name)
      logWarn() << "unexpected elem kind (getName)" << elem.Element_Kind << std::endl;

    ROSE_ASSERT(elem.Element_Kind == A_Defining_Name);
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
          Element_Struct& prefixelem = retrieveAs<Element_Struct>(elemMap(), asisname.Defining_Prefix);

          ident  = defname.ident;
          parent = &getScope(prefixelem, ctx);
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

          ROSE_ASSERT(ident.size() > 2);
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

      case Not_A_Defining_Name:
        /* break; */

      case A_Defining_Character_Literal:   // 3.5.1(4)
      default:
        logWarn() << "unknown name kind " << asisname.Defining_Name_Kind
                  << " (" << name << ")"
                  << std::endl;
        ROSE_ASSERT(!FAIL_ON_ERROR);
    }

    return NameData{ ident, name, parent, &elem };
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
    ROSE_ASSERT(lblid > 0);

    SgNode&           parent  = SG_DEREF(stmt.get_parent());
    SgLabelStatement& sgn     = mkLabelStmt(lblname, stmt, ctx.scope());
    Element_Struct&   lblelem = retrieveAs<Element_Struct>(elemMap(), lblid);

    //~ copyFileInfo(stmt, sgn);
    attachSourceLocation(sgn, lblelem, ctx);
    sgn.set_parent(&parent);
    ctx.labelsAndLoops().label(lblid, sgn);

    ROSE_ASSERT(stmt.get_parent() == &sgn);
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
    ROSE_ASSERT(lblid >= 0);

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
    name_container names  = traverseIDs(range, elemMap(), NameCreator{ctx});
    SgStatement*   sgnode = std::accumulate( names.rbegin(), names.rend(),
                                             &stmt,
                                             [&](SgStatement* labeled, const NameData& el) -> SgStatement*
                                             {
                                               ROSE_ASSERT(el.fullName == el.ident);
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
    ROSE_ASSERT(elem.Element_Kind == A_Statement);

    attachSourceLocation(sgnode, elem, ctx);
    sgnode.set_parent(&ctx.scope());

    Statement_Struct& stmt = elem.The_Union.Statement;
    SgStatement&      sgn  = labelIfNeeded(sgnode, stmt, ctx);

    ctx.scope().append_statement(&sgn);
    ROSE_ASSERT(sgn.get_parent() == &ctx.scope());
  }

  template <class SageScopeStmt>
  void
  completeStmt(SageScopeStmt& sgnode, Element_Struct& elem, AstContext ctx, Defining_Name_ID lblid)
  {
    ROSE_ASSERT(elem.Element_Kind == A_Statement);

    attachSourceLocation(sgnode, elem, ctx);
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
  constructInitializedNamePtrList( AstContext ctx,
                                   std::map<int, SgInitializedName*>& m,
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

      attachSourceLocation(dcl, retrieveAs<Element_Struct>(elemMap(), id), ctx);

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
        ROSE_ASSERT(false);
        break;
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

    SgExpression& res = getExprID_opt(decl.Initialization_Expression, ctx);

    //  \todo this fails ...
    //        discuss with DQ expression types
    //~ if (expectedType) res.set_explicitly_stored_type(expectedType);
    return &res;
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

  Element_ID getLabelRef(Element_ID id, AstContext ctx)
  {
    Element_Struct&    labelref = retrieveAs<Element_Struct>(elemMap(), id);
    ROSE_ASSERT(labelref.Element_Kind == An_Expression);

    Expression_Struct& labelexp = labelref.The_Union.Expression;
    ROSE_ASSERT(labelexp.Expression_Kind == An_Identifier);

    logKind("An_Identifier");
    return labelexp.Corresponding_Name_Definition;
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
    GuardedExpression        initexpr{getVarInit(asisDecl, &parmtype, ctx)};
    SgInitializedNamePtrList dclnames = constructInitializedNamePtrList( ctx, asisVars(), names,
                                                                         parmtype, std::move(initexpr)
                                                                       );
    SgVariableDeclaration&   sgnode   = mkParameter(dclnames, getMode(asisDecl.Mode_Kind), ctx.scope());

    attachSourceLocation(sgnode, elem, ctx);
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

        markCompilerGenerated(*dcl);
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

      void handle(SgEnumDeclaration& n)
      {
        ROSE_ASSERT(n.get_scope() == &dclscope);

        n.set_name(dclname);
        res = &n;
      }

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
      TypeData          foundation;
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
        SgDeclarationStatement* dcl  = sg::dispatch(MakeDeclaration(name, scope, foundation), foundation.n);
        ROSE_ASSERT(dcl);

        markCompilerGenerated(*dcl);
        privatize(*dcl, privateElems);
        scope.append_statement(dcl);

        // \todo double check that recorded types are consistent with ROSE representation
        recordNode(asisTypes(), id, *dcl);
        ROSE_ASSERT(dcl->get_parent() == &scope);
      }

    private:
      TypeData          foundation;
      SgScopeStatement& scope;
      bool              privateElems;
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
  handleNumVarCstDecl(Declaration_Struct& decl, AstContext ctx, bool isPrivate, SgType& dclType, SgType* expectedType, Element_Struct& elem)
  {
    typedef NameCreator::result_container name_container;

    ElemIdRange              range    = idRange(decl.Names);
    name_container           names    = traverseIDs(range, elemMap(), NameCreator{ctx});
    SgScopeStatement&        scope    = ctx.scope();
    GuardedExpression        initexp{getVarInit(decl, expectedType, ctx)};
    SgInitializedNamePtrList dclnames = constructInitializedNamePtrList( ctx, asisVars(), names,
                                                                         dclType, std::move(initexp)
                                                                       );
    SgVariableDeclaration&   vardcl   = mkVarDecl(dclnames, scope);

    attachSourceLocation(vardcl, elem, ctx);
    privatize(vardcl, isPrivate);
    scope.append_statement(&vardcl);

    ROSE_ASSERT(vardcl.get_parent() == &scope);
  }

  void
  handleNumberDecl(Declaration_Struct& decl, AstContext ctx, bool isPrivate, SgType& numty, SgType& expctty, Element_Struct& elem)
  {
    SgType& cstty = SG_DEREF( sb::buildConstType(&numty) );

    handleNumVarCstDecl(decl, ctx, isPrivate, cstty, &expctty, elem);
  }

  void
  handleVarCstDecl(Declaration_Struct& dcl, AstContext ctx, bool isPrivate, TypeModiferFn tyModifier, Element_Struct& elem)
  {
    handleNumVarCstDecl(dcl, ctx, isPrivate, tyModifier(getVarType(dcl, ctx)), nullptr /* no expected type */, elem);
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

    ROSE_ASSERT(expr.Expression_Kind == A_Selected_Component);
    logKind("A_Selected_Component");
    return getAliasedID(expr.Selector, ctx);
  }

  SgDeclarationStatement&
  getAliasedID(Element_ID declid, AstContext ctx)
  {
    Element_Struct& elem = retrieveAs<Element_Struct>(elemMap(), declid);
    ROSE_ASSERT(elem.Element_Kind == An_Expression);

    return getAliased(elem.The_Union.Expression, ctx);
  }

  SgInitializedName&
  getAliasedExcnDecl(Element_ID declid, AstContext ctx)
  {
    Element_Struct& elem = retrieveAs<Element_Struct>(elemMap(), declid);
    ROSE_ASSERT(elem.Element_Kind == A_Defining_Name);

    SgInitializedName* the_name = findFirst(asisExcps(), elem.ID, elem.ID);
    return SG_DEREF(the_name);
  }

  SgAdaTaskBody&
  getTaskBody(Declaration_Struct& decl, AstContext ctx)
  {
    ROSE_ASSERT(decl.Declaration_Kind == A_Task_Body_Declaration);

    SgAdaTaskBody& sgnode = mkAdaTaskBody();

    {
      ElemIdRange    decls = idRange(decl.Body_Declarative_Items);

      traverseIDs(decls, elemMap(), StmtCreator{ctx.scope_npc(sgnode)});
    }

    {
      ElemIdRange         stmts = idRange(decl.Body_Statements);
      LabelAndLoopManager lblmgr;

      traverseIDs(stmts, elemMap(), StmtCreator{ctx.scope_npc(sgnode).labelsAndLoops(lblmgr)});
    }

    return sgnode;
  }

  SgAdaTaskSpec&
  getTaskSpec(Element_Struct& elem, AstContext ctx)
  {
    ROSE_ASSERT(elem.Element_Kind == A_Definition);

    SgAdaTaskSpec&          sgnode = mkAdaTaskSpec();

    sgnode.set_hasMembers(true);

    Definition_Struct&      def = elem.The_Union.Definition;
    ROSE_ASSERT(def.Definition_Kind == A_Task_Definition);

    logKind("A_Task_Definition");

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

  SgAdaTaskSpec&
  getTaskSpecID(Element_ID id, AstContext ctx)
  {
    return getTaskSpec(retrieveAs<Element_Struct>(elemMap(), id), ctx);
  }

  SgAdaTaskSpec&
  getTaskSpecForTaskType(Declaration_Struct& decl, AstContext ctx)
  {
    ROSE_ASSERT(decl.Declaration_Kind == A_Task_Type_Declaration);

    if (decl.Type_Declaration_View == 0)
      return mkAdaTaskSpec();

    return getTaskSpecID(decl.Type_Declaration_View, ctx);
  }

  SgAdaTaskSpec&
  getTaskSpecForSingleTask(Declaration_Struct& decl, AstContext ctx)
  {
    ROSE_ASSERT(decl.Declaration_Kind == A_Single_Task_Declaration);

    return getTaskSpecID(decl.Object_Declaration_View, ctx);
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
        ROSE_ASSERT(!choices.empty());

        if (choices.size() > 1) return false;

        Element_Struct& el = retrieveAs<Element_Struct>(elemMap(), *choices.first);

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
                ROSE_ASSERT(choices.size());
                SgExpression&              caseCond  = mkChoiceExpIfNeeded(std::move(choices));
                SgCaseOptionStmt*          whenNode  = &mkWhenPath(caseCond, block);

                caseNode.append_case(whenNode);
                sgnode = whenNode;
              }

              ROSE_ASSERT(sgnode);
              ElemIdRange caseBlock = idRange(path.Sequence_Of_Statements);

              traverseIDs(caseBlock, elemMap(), StmtCreator{ctx.scope(block)});
              attachSourceLocation(*sgnode, elem, ctx);
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
              logKind("An_If_Path");
              ROSE_ASSERT(branches.size() == 0);
              conditionedBranch(path);
              break;
            }

          case An_Elsif_Path:
            {
              logKind("An_Elsif_Path");
              ROSE_ASSERT(branches.size() != 0);
              conditionedBranch(path);
              break;
            }

          case An_Else_Path:
            {
              logKind("An_Else_Path");
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

  bool isForwardLoop(Element_Struct& forvar)
  {
    ROSE_ASSERT (forvar.Element_Kind == A_Declaration);

    Declaration_Struct& decl = forvar.The_Union.Declaration;
    ROSE_ASSERT(decl.Declaration_Kind == A_Loop_Parameter_Specification);

    return !decl.Has_Reverse;
  }


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
          logKind("A_Null_Statement");

          completeStmt(mkNullStmt(), elem, ctx);
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

          completeStmt(sgnode, elem, ctx);
          /* unused fields:
          */
          break;
        }

      case An_If_Statement:                     // 5.3
        {
          logKind("An_If_Statement");

          ElemIdRange  range  = idRange(stmt.Statement_Paths);
          SgStatement& sgnode = traverseIDs(range, elemMap(), IfStmtCreator{ctx});

          completeStmt(sgnode, elem, ctx);
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
          ElemIdRange        range    = idRange(stmt.Statement_Paths);

          traverseIDs(range, elemMap(), CaseStmtCreator{ctx.scope(casebody), sgnode});
          completeStmt(sgnode, elem, ctx);
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
          /* unused fields:
                Pragma_Element_ID_List    Pragmas;
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

          /* unused fields:
                Pragma_Element_ID_List    Pragmas;
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
          Element_Struct&        forvar = retrieveAs<Element_Struct>(elemMap(), stmt.For_Loop_Parameter_Specification);
          SgForInitStatement&    forini = SG_DEREF( sb::buildForInitStatement(sgnode.getStatementList()) );

          sg::linkParentChild(sgnode, forini, &SgForStatement::set_for_init_stmt);
          handleDeclaration(forvar, ctx.scope_npc(sgnode));
          completeStmt(sgnode, elem, ctx, stmt.Statement_Identifier);

          // this swap is needed, b/c SgForInitStatement is not a scope
          // and when the loop variable declaration is created, the declaration
          // is pushed to the wrong statement list.
          ROSE_ASSERT(sgnode.getStatementList().size() == 1);
          std::swap(forini.get_init_stmt(), block.get_statements());

          SgVariableDeclaration* inductionVar = isSgVariableDeclaration(forini.get_init_stmt().front());
          SgExpression&          direction    = mkForLoopIncrement(isForwardLoop(forvar), SG_DEREF(inductionVar));

          sgnode.set_increment(&direction);

          // loop body
          {
            ElemIdRange            loopStmts = idRange(stmt.Loop_Statements);

            recordNode(ctx.labelsAndLoops().asisLoops(), elem.ID, sgnode);
            traverseIDs(loopStmts, elemMap(), StmtCreator{ctx.scope(block)});
          }

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
          ElemIdRange   blkDecls = idRange(stmt.Block_Declarative_Items);
          ElemIdRange   blkStmts = idRange(stmt.Block_Statements);
          ElemIdRange   exHndlrs = idRange(stmt.Block_Exception_Handlers);
          //~ logInfo() << "block ex handlers: " << exHndlrs.size() << std::endl;

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
          logKind("An_Exit_Statement");

          SgStatement&  exitedLoop    = lookupNode(ctx.labelsAndLoops().asisLoops(), stmt.Corresponding_Loop_Exited);
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

          SgExpression*  funrefexp = nullptr;

          //~ SgDeclarationStatement& tgt    = lookupNode(asisDecls(), stmt.Corresponding_Called_Entity);
/*
          if (SgDeclarationStatement* tgt = findNode(asisDecls(), stmt.Corresponding_Called_Entity))
          {
            funrefexp = sb::buildFunctionRefExp(isSgFunctionDeclaration(tgt));
          }
          else
          {
            logWarn() << "unable to find declaration for "
                      << (stmt.Statement_Kind == An_Entry_Call_Statement ? "entry" : "procedure")
                      << " call"
                      << std::endl;

          }
*/
          funrefexp = &getExprID(stmt.Called_Name, ctx);

          ROSE_ASSERT(funrefexp);
          ElemIdRange    range  = idRange(stmt.Call_Statement_Parameters);
          SgExprListExp& arglst = traverseIDs(range, elemMap(), ArgListCreator{ctx});
          SgStatement&   sgnode = SG_DEREF(sb::buildFunctionCallStmt(funrefexp, &arglst));

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

      case A_Delay_Until_Statement:             // 9.6
      case A_Delay_Relative_Statement:          // 9.6
        {
          logKind(stmt.Statement_Kind == A_Delay_Until_Statement ? "A_Delay_Until_Statement" : "A_Delay_Relative_Statement");

          SgExpression&   delayexpr = getExprID(stmt.Delay_Expression, ctx);
          SgAdaDelayStmt& sgnode    = mkAdaDelayStmt(delayexpr, stmt.Statement_Kind != A_Delay_Until_Statement);

          completeStmt(sgnode, elem, ctx);

          /* unused fields:
              Pragma_Element_ID_List       Pragmas;

             break;
          */
          break;
        }

      case A_Raise_Statement:                   // 11.3
        {
          logKind("A_Raise_Statement");
          SgExpression&   raised = getExprID(stmt.Raised_Exception, ctx);
          SgStatement&    sgnode = mkRaiseStmt(raised);

          completeStmt(sgnode, elem, ctx);
          /* unused fields:
               Expression_ID          Associated_Message;
          */
          break;
        }

      case An_Abort_Statement:                  // 9.8
        {
          logKind("An_Abort_Statement");

          ElemIdRange                range     = idRange(stmt.Aborted_Tasks);
          std::vector<SgExpression*> aborted   = traverseIDs(range, elemMap(), ExprSeqCreator{ctx});
          SgExprListExp&             abortList = SG_DEREF(sb::buildExprListExp(aborted));
          SgProcessControlStatement& sgnode    = mkAbortStmt(abortList);

          completeStmt(sgnode, elem, ctx);

          /* unused fields:
          */
          break;
        }


      case Not_A_Statement: /* break; */        // An unexpected element
      //|A2005 start
      case An_Extended_Return_Statement:        // 6.5
      //|A2005 end
      case A_Requeue_Statement:                 // 9.5.4
      case A_Requeue_Statement_With_Abort:      // 9.5.4
      case A_Terminate_Alternative_Statement:   // 9.7.1
      case A_Selective_Accept_Statement:        // 9.7.1
      case A_Timed_Entry_Call_Statement:        // 9.7.2
      case A_Conditional_Entry_Call_Statement:  // 9.7.3
      case An_Asynchronous_Select_Statement:    // 9.7.4
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

    logKind("A_Choice_Parameter_Specification");
    // SgType&                   dcltype = tyModifier(getVarType(decl, ctx));
    ElemIdRange              range    = idRange(asisDecl.Names);

    return traverseIDs(range, elemMap(), NameCreator{ctx});
  }

  void handleExceptionHandler(Element_Struct& elem, SgTryStmt& tryStmt, AstContext ctx)
  {
    typedef NameCreator::result_container name_container;

    ROSE_ASSERT(elem.Element_Kind == An_Exception_Handler);

    logKind("An_Exception_Handler");
    Exception_Handler_Struct& ex      = elem.The_Union.Exception_Handler;
    name_container            names   = queryDeclNames(ex.Choice_Parameter_Specification, ctx);

    if (names.size() == 0)
    {
      // add an unnamed exception handler
      names.emplace_back(std::string{}, std::string{}, &ctx.scope(), &elem);
    }

    ROSE_ASSERT(names.size() == 1);
    ElemIdRange              tyRange = idRange(ex.Exception_Choices);
    SgType&                  extypes = traverseIDs(tyRange, elemMap(), ExHandlerTypeCreator{ctx});
    SgInitializedNamePtrList lst     = constructInitializedNamePtrList(ctx, asisVars(), names, extypes);
    SgBasicBlock&            body    = mkBasicBlock();

    ROSE_ASSERT(lst.size() == 1);
    SgCatchOptionStmt&       sgnode  = mkExceptionHandler(SG_DEREF(lst[0]), body);
    ElemIdRange              range   = idRange(ex.Handler_Statements);

    sg::linkParentChild(tryStmt, as<SgStatement>(sgnode), &SgTryStmt::append_catch_statement);
    sgnode.set_trystmt(&tryStmt);

    traverseIDs(range, elemMap(), StmtCreator{ctx.scope(body)});

    /* unused fields:
         Pragma_Element_ID_List Pragmas;
    */
  }

  Expression_Struct& asisExpression(Element_Struct& elem)
  {
    ROSE_ASSERT (elem.Element_Kind == An_Expression);

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
        ROSE_ASSERT (el.Element_Kind == An_Expression);

        NameData                   imported = getName(el, ctx);
        SgScopeStatement&          scope = ctx.scope();
        SgExpression&              res = mkUnresolvedName(imported.fullName, scope);
        std::vector<SgExpression*> elems{&res};
        SgImportStatement&         sgnode = mkWithClause(elems);

        recordNode(asisDecls(), el.ID, sgnode);
        attachSourceLocation(sgnode, el, ctx);

        //~ sg::linkParentChild(scope, as<SgStatement>(sgnode), &SgScopeStatement::append_statement);
        scope.append_statement(&sgnode);

        // an imported element may not be in the AST
        //   -> add the mapping if the element has not been seen.
        Element_ID                 impID    = asisExpression(imported.elem()).Corresponding_Name_Declaration;

        recordNonUniqueNode(asisDecls(), impID, sgnode);
        ROSE_ASSERT(sgnode.get_parent() == &scope);
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
        ROSE_ASSERT (el.Element_Kind == An_Expression);

        NameData                     usepkg = getName(el, ctx);
        SgScopeStatement&            scope  = ctx.scope();
        Expression_Struct&           expr   = asisExpression(usepkg.elem());
        SgDeclarationStatement*      used   = findFirst(m, expr.Corresponding_Name_Definition, expr.Corresponding_Name_Declaration);
        SgUsingDeclarationStatement& sgnode = mkUseClause(SG_DEREF(used));

        recordNode(asisDecls(), el.ID, sgnode);
        attachSourceLocation(sgnode, el, ctx);
        scope.append_statement(&sgnode);

        ROSE_ASSERT(sgnode.get_parent() == &scope);
      }

    private:
      map_t<int, SgDeclarationStatement*>& m;
      AstContext ctx;

      UseClauseCreator() = delete;
  };




  /// Functor to create an import statement (actually, a declaration)
  ///   for each element of a with clause; e.g., with Ada.Calendar, Ada.Clock;
  ///   yields two important statements, each of them is its own declaration
  ///   that can be referenced.
  struct ComponentClauseCreator
  {
      ComponentClauseCreator(SgAdaRecordRepresentationClause& recclause, AstContext astctx)
      : recordclause(recclause), ctx(astctx)
      {}

      void operator()(Element_Struct& el)
      {
        ROSE_ASSERT(el.Element_Kind == A_Clause);

        Clause_Struct& clause = el.The_Union.Clause;
        ROSE_ASSERT(clause.Clause_Kind == A_Component_Clause);

        // \todo use getQualName?
        SgExpression&         field  = getExprID(clause.Representation_Clause_Name, ctx);
        SgVarRefExp&          fldref = SG_DEREF(isSgVarRefExp(&field));
        SgExpression&         ofs    = getExprID(clause.Component_Clause_Position, ctx);
        SgExpression&         rngexp = getDiscreteRangeID(clause.Component_Clause_Range, ctx);
        SgRangeExp*           range  = isSgRangeExp(&rngexp);
        SgAdaComponentClause& sgnode = mkAdaComponentClause(fldref, ofs, SG_DEREF(range));

        //~ recordNode(asisDecls(), el.ID, sgnode);
        attachSourceLocation(sgnode, el, ctx);

        sg::linkParentChild(recordclause, sgnode, &SgAdaRecordRepresentationClause::append_component);
        //~ recordclause.append_component(&sgnode);
        //~ ROSE_ASSERT(sgnode.get_parent() == &recordclause);

        /* unused fields:
         */
      }

    private:
      SgAdaRecordRepresentationClause& recordclause;
      AstContext                       ctx;
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

  ROSE_ASSERT(clause.Clause_Kind == A_Representation_Clause);

  Representation_Clause_Struct& repclause = clause.Representation_Clause;
  SgType&                       tyrep     = getDeclTypeID(repclause.Representation_Clause_Name, ctx);

  switch (repclause.Representation_Clause_Kind)
  {
    case A_Record_Representation_Clause:           // 13.5.1
      {
        SgClassType&            rec = SG_DEREF(isSgClassType(&tyrep));
        SgExpression&           modclause = getExprID(repclause.Mod_Clause_Expression, ctx);
        SgAdaRecordRepresentationClause& sgnode = mkAdaRecordRepresentationClause(rec, modclause);
        ElemIdRange             range  = idRange(repclause.Component_Clauses);

        // sgnode is not a decl: recordNode(asisDecls(), el.ID, sgnode);
        attachSourceLocation(sgnode, elem, ctx);
        ctx.scope().append_statement(&sgnode);

        traverseIDs(range, elemMap(), ComponentClauseCreator{sgnode, ctx});
        /* unhandled fields:
             Pragma_Element_ID_List      Pragmas
         */
        break;
      }

    case An_Attribute_Definition_Clause:           // 13.3
    case An_Enumeration_Representation_Clause:     // 13.4
    case An_At_Clause:                             // J.7
    case Not_A_Representation_Clause:              // An unexpected element
    default:
      logWarn() << "unhandled representation clause kind: " << repclause.Representation_Clause_Kind << std::endl;
      ROSE_ASSERT(!FAIL_ON_ERROR);
  }
}

void handleClause(Element_Struct& elem, AstContext ctx)
{
  ROSE_ASSERT(elem.Element_Kind == A_Clause);

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
      ROSE_ASSERT(!FAIL_ON_ERROR);
  }
}


void handleDefinition(Element_Struct& elem, AstContext ctx)
{
  ROSE_ASSERT(elem.Element_Kind == A_Definition);

  // many definitions are handled else where
  // here we want to convert the rest that can appear in declarative context

  Definition_Struct& def = elem.The_Union.Definition;

  switch (def.Definition_Kind)
  {
    case A_Null_Component:                 // 3.8(4)
      {
        SgScopeStatement&   scope = ctx.scope();
        SgEmptyDeclaration& sgnode = mkNullDecl(scope);

        attachSourceLocation(sgnode, elem, ctx);
        ROSE_ASSERT(sgnode.get_parent() == &scope);
        break;
      }

    case A_Type_Definition:                // 3.2.1(4)    -> Type_Kinds
      // handled in getTypeFoundation
      ROSE_ASSERT(false);
      break;

    case A_Subtype_Indication:             // 3.2.2(3)
      // handled in getDefinitionType
      ROSE_ASSERT(false);
      break;

    case A_Constraint:                     // 3.2.2(5)    -> Constraint_Kinds
      // handled in getRangeConstraint
      ROSE_ASSERT(false);
      break;

    case A_Component_Definition:           // 3.6(7)      -> Trait_Kinds
      // handled in getDefinitionType
      ROSE_ASSERT(false);
      break;

    case A_Discrete_Range:                 // 3.6.1(3)    -> Discrete_Range_Kinds
      // handled in getDefinitionExpr
      ROSE_ASSERT(false);
      break;

    case A_Record_Definition:              // 3.8(3)
      // handled in getRecordBodyID
      ROSE_ASSERT(false);
      break;

    case A_Null_Record_Definition:         // 3.8(3)
      // handled in getRecordBodyID
      ROSE_ASSERT(false);
      break;

    case An_Others_Choice:                 // 3.8.1(5): 4.3.1(5): 4.3.3(5): 11.2(5)
      // handled in case creation (and getDefinitionExpr (obsolete?))
      ROSE_ASSERT(false);
      break;

    case An_Access_Definition:             // 3.10(6/2)   -> Access_Definition_Kinds, A2005 start
      // handled in getAccessType
      ROSE_ASSERT(false);
      break;

    case A_Task_Definition:                // 9.1(4)
      // handled in getTaskSpec
      ROSE_ASSERT(false);
      break;

    case A_Discrete_Subtype_Definition:    // 3.6(6)      -> Discrete_Range_Kinds
    case An_Unknown_Discriminant_Part:     // 3.7(3)
    case A_Known_Discriminant_Part:        // 3.7(2)
    case Not_A_Definition:                 // An unexpected element
    case A_Variant_Part:                   // 3.8.1(2)
    case A_Variant:                        // 3.8.1(3)
    case A_Private_Type_Definition:        // 7.3(2)      -> Trait_Kinds
    case A_Tagged_Private_Type_Definition: // 7.3(2)      -> Trait_Kinds
    case A_Private_Extension_Definition:   // 7.3(3)      -> Trait_Kinds
    case A_Protected_Definition:           // 9.4(4)
    case A_Formal_Type_Definition:         // 12.5(3)     -> Formal_Type_Kinds
    case An_Aspect_Specification:          // 13.3.1, A2012
    default:
      logWarn() << "unhandled definition kind: " << def.Definition_Kind << std::endl;
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
        logKind("A_Package_Declaration");

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
        recordNode(asisDecls(), adaname.id(), sgnode);

        privatize(sgnode, isPrivate);
        attachSourceLocation(pkgspec, elem, ctx);
        attachSourceLocation(sgnode, elem, ctx);
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
        logKind("A_Package_Body_Declaration");

        SgScopeStatement&     outer   = ctx.scope();
        Element_ID            specID  = decl.Corresponding_Declaration;
        SgAdaPackageSpecDecl& specdcl = lookupNodeAs<SgAdaPackageSpecDecl>(asisDecls(), specID);
        SgAdaPackageBodyDecl& sgnode  = mkAdaPackageBodyDecl(specdcl, outer);
        SgAdaPackageBody&     pkgbody = SG_DEREF(sgnode.get_definition());

        //~ recordNode(asisDecls(), elem.ID, sgnode);
        //~ recordNode(asisDecls(), adaname.id(), sgnode);

        sgnode.set_scope(specdcl.get_scope());
        attachSourceLocation(pkgbody, elem, ctx);
        attachSourceLocation(sgnode, elem, ctx);
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
        logKind(decl.Declaration_Kind == A_Function_Declaration ? "A_Function_Declaration" : "A_Procedure_Declaration");

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
        recordNode(asisDecls(), adaname.id(), sgnode);

        privatize(sgnode, isPrivate);
        attachSourceLocation(sgnode, elem, ctx);
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
        logKind(decl.Declaration_Kind == A_Function_Declaration ? "A_Function_Body_Declaration" : "A_Procedure_Body_Declaration");

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
        recordNode(asisDecls(), adaname.id(), sgnode);
        privatize(sgnode, isPrivate);
        attachSourceLocation(sgnode, elem, ctx);
        outer.append_statement(&sgnode);
        ROSE_ASSERT(sgnode.get_parent() == &outer);

        ElemIdRange            hndlrs  = idRange(decl.Body_Exception_Handlers);
        //~ logInfo() << "block ex handlers: " << hndlrs.size() << std::endl;

        TryBlockNodes          trydata = createTryBlockIfNeeded(hndlrs.size() > 0, declblk);
        SgTryStmt*             trystmt = trydata.first;
        SgBasicBlock&          stmtblk = trydata.second;

        {
          ElemIdRange range = idRange(decl.Body_Declarative_Items);

          traverseIDs(range, elemMap(), ElemCreator{ctx.scope(declblk)});
        }

        {
          LabelAndLoopManager lblmgr;
          ElemIdRange         range = idRange(decl.Body_Statements);

          traverseIDs(range, elemMap(), StmtCreator{ctx.scope(stmtblk).labelsAndLoops(lblmgr)});
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

    case An_Incomplete_Type_Declaration:           // 3.2.1(2):3.10(2)
      {
        logKind("An_Incomplete_Type_Declaration");

        NameData                adaname = singleName(decl, ctx);
        ROSE_ASSERT(adaname.fullName == adaname.ident);

        logTrace() << "Incomplete Type: " << adaname.fullName
                   << "\n  private: " << decl.Has_Private
                   << "\n  limited: " << decl.Has_Limited
                   << "\n  private: " << decl.Has_Private
                   << std::endl;

        SgScopeStatement&       scope  = ctx.scope();
        SgType&                 opaque = mkOpaqueType();
        SgDeclarationStatement& sgnode = mkTypeDecl(adaname.ident, opaque, scope);

        attachSourceLocation(sgnode, elem, ctx);
        privatize(sgnode, isPrivate);
        scope.append_statement(&sgnode);
        recordNode(asisTypes(), adaname.id(), sgnode);
        ROSE_ASSERT(sgnode.get_parent() == &scope);

        /*
           unhandled fields:
             Definition_ID                  Discriminant_Part
             Definition_ID                  Type_Declaration_View
             Declaration_ID                 Corresponding_Type_Declaration
             Declaration_ID                 Corresponding_Type_Completion
             Declaration_ID                 Corresponding_Type_Partial_View (* notes)
        */
        break;
      }

    case A_Private_Type_Declaration:               // 3.2.1(2):7.3(2) -> Trait_Kinds
      {
        logKind("A_Private_Type_Declaration");

        typedef NameCreator::result_container name_container;

        logTrace() << "Private Type "
                   << "\n  abstract: " << decl.Has_Abstract
                   << "\n  limited: " << decl.Has_Limited
                   << "\n  private: " << decl.Has_Private
                   << std::endl;

        // \todo this may only declare one name
        //       (use singleName -- e.g., An_Incomplete_Type_Declaration)
        ElemIdRange     range  = idRange(decl.Names);
        ROSE_ASSERT(range.size() == 1);
        name_container  names  = traverseIDs(range, elemMap(), NameCreator{ctx});
        SgType&         opaque = mkOpaqueType();

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
        logKind("An_Ordinary_Type_Declaration");

        typedef NameCreator::result_container name_container;

        logTrace() << "Ordinary Type "
                   << "\n  abstract: " << decl.Has_Abstract
                   << "\n  limited: " << decl.Has_Limited
                   << std::endl;
        // \todo this may only declare one name (use singleName)
        ElemIdRange    range  = idRange(decl.Names);
        ROSE_ASSERT(range.size() == 1);
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


    case A_Subtype_Declaration:                    // 3.2.2(2)
      {
        logKind("A_Subtype_Declaration");

        NameData              adaname = singleName(decl, ctx);
        ROSE_ASSERT(adaname.fullName == adaname.ident);

        SgType&               subtype = getDefinitionTypeID(decl.Type_Declaration_View, ctx);
        SgScopeStatement&     scope   = ctx.scope();
        SgTypedefDeclaration& sgnode  = mkTypeDecl(adaname.ident, subtype, scope);

        privatize(sgnode, isPrivate);
        attachSourceLocation(sgnode, elem, ctx);
        scope.append_statement(&sgnode);
        recordNode(asisTypes(), adaname.id(), sgnode);
        ROSE_ASSERT(sgnode.get_parent() == &scope);

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

        handleVarCstDecl(decl, ctx, isPrivate, tyIdentity, elem);
        /* unused fields:
             bool                           Has_Aliased;
        */
        break;
      }

    case An_Integer_Number_Declaration:            // 3.3.2(2)
      {
        logKind("An_Integer_Number_Declaration");

        handleNumberDecl(decl, ctx, isPrivate, SG_DEREF(sb::buildAutoType()), mkIntegralType(), elem);

        /* unused fields:
        */
        break;
      }

    case A_Deferred_Constant_Declaration:          // 3.3.1(6):7.4(2) -> Trait_Kinds
    case A_Constant_Declaration:
      {
        logKind(decl.Declaration_Kind == A_Constant_Declaration ? "A_Constant_Declaration" : "A_Deferred_Constant_Declaration");

        handleVarCstDecl(decl, ctx, isPrivate, tyConstify, elem);
        /* unused fields:
             bool                           Has_Aliased;

           break;
        */
        break;
      }

    case A_Real_Number_Declaration:                // 3.5.6(2)
      {
        logKind("A_Real_Number_Declaration");

        handleNumberDecl(decl, ctx, isPrivate, SG_DEREF(sb::buildAutoType()), mkRealType(), elem);

        /* unused fields:
         */
        break;
      }

    case A_Loop_Parameter_Specification:           // 5.5(4)   -> Trait_Kinds
      {
        logKind("A_Loop_Parameter_Specification");

        ROSE_ASSERT(!isPrivate);

        NameData               adaname = singleName(decl, ctx);
        ROSE_ASSERT(adaname.fullName == adaname.ident);

        SgType&                vartype = SG_DEREF( sb::buildIntType() ); // \todo
        SgExpression&          range   = getDefinitionExprID(decl.Specification_Subtype_Definition, ctx);
        SgInitializedName&     loopvar = mkInitializedName(adaname.fullName, vartype, &range);
        SgScopeStatement&      scope   = ctx.scope();

        recordNode(asisVars(), adaname.id(), loopvar);

        SgVariableDeclaration& sgnode  = mkVarDecl(loopvar, scope);

        attachSourceLocation(loopvar, elem, ctx);
        attachSourceLocation(sgnode, elem, ctx);
        scope.append_statement(&sgnode);
        ROSE_ASSERT(sgnode.get_parent() == &scope);

        /* unused fields:
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
        logKind("A_Task_Type_Declaration");

        SgAdaTaskSpec&     spec    = getTaskSpecForTaskType(decl, ctx);
        NameData           adaname = singleName(decl, ctx);
        ROSE_ASSERT(adaname.fullName == adaname.ident);
        SgAdaTaskTypeDecl& sgnode  = mkAdaTaskTypeDecl(adaname.fullName, spec, ctx.scope());

        attachSourceLocation(sgnode, elem, ctx);
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

    case A_Single_Task_Declaration:                // 3.3.1(2):9.1(3)
      {
        logKind("A_Single_Task_Declaration");

        SgAdaTaskSpec&     spec    = getTaskSpecForSingleTask(decl, ctx);
        NameData           adaname = singleName(decl, ctx);
        ROSE_ASSERT(adaname.fullName == adaname.ident);
        SgAdaTaskSpecDecl& sgnode  = mkAdaTaskSpecDecl(adaname.fullName, spec, ctx.scope());

        attachSourceLocation(sgnode, elem, ctx);
        privatize(sgnode, isPrivate);
        ctx.scope().append_statement(&sgnode);
        ROSE_ASSERT(sgnode.get_parent() == &ctx.scope());
        //~ recordNode(asisTypes(), adaname.id(), sgnode);
        recordNode(asisDecls(), adaname.id(), sgnode);

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

    case A_Task_Body_Declaration:                  // 9.1(6)
      {
        logKind("A_Task_Body_Declaration");

        SgAdaTaskBody&          tskbody = getTaskBody(decl, ctx);
        NameData                adaname = singleName(decl, ctx);
        Element_ID              declID  = decl.Corresponding_Declaration;
        SgDeclarationStatement* tskdecl = findNode(asisDecls(), declID);
        ROSE_ASSERT(adaname.fullName == adaname.ident);

        // \todo \review not sure why a task body could be independently created
        SgAdaTaskBodyDecl&      sgnode  = tskdecl ? mkAdaTaskBodyDecl(*tskdecl, tskbody, ctx.scope())
                                                  : mkAdaTaskBodyDecl(adaname.fullName, tskbody, ctx.scope());

        attachSourceLocation(sgnode, elem, ctx);
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
        logKind("An_Entry_Declaration");

        NameData        adaname = singleName(decl, ctx);
        ElemIdRange     range   = idRange(decl.Parameter_Profile);

        ROSE_ASSERT(adaname.fullName == adaname.ident);
        SgAdaEntryDecl& sgnode  = mkAdaEntryDecl(adaname.fullName, ctx.scope(), ParameterCompletion{range, ctx});

        attachSourceLocation(sgnode, elem, ctx);
        privatize(sgnode, isPrivate);
        ctx.scope().append_statement(&sgnode);
        ROSE_ASSERT(sgnode.get_parent() == &ctx.scope());

        // the entry call links back to the declaration ID
        recordNode(asisDecls(), elem.ID, sgnode);

        // for consistency, also map the name id to the node
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
        logKind("An_Exception_Declaration");

        typedef NameCreator::result_container name_container;

        ElemIdRange              range    = idRange(decl.Names);
        name_container           names    = traverseIDs(range, elemMap(), NameCreator{ctx});
        SgScopeStatement&        scope    = ctx.scope();
        SgType&                  excty    = lookupNode(adaTypes(), AdaIdentifier{"Exception"});
        SgInitializedNamePtrList dclnames = constructInitializedNamePtrList(ctx, asisExcps(), names, excty);
        SgVariableDeclaration&   sgnode   = mkExceptionDecl(dclnames, scope);

        attachSourceLocation(sgnode, elem, ctx);
        privatize(sgnode, isPrivate);
        scope.append_statement(&sgnode);
        ROSE_ASSERT(sgnode.get_parent() == &scope);
        break;
      }

    case A_Component_Declaration:                  // 3.8(6)
      {
        logKind("A_Component_Declaration");

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
        logKind("A_Package_Renaming_Declaration");

        NameData                adaname = singleName(decl, ctx);

        if (isInvalidId(decl.Renamed_Entity))
        {
          logWarn() << "skipping unknown package renaming: " << adaname.ident << "/" << adaname.fullName
                    << ": " << elem.ID << " / " << decl.Renamed_Entity
                    << std::endl;
          return;
        }

        SgDeclarationStatement& aliased = getAliasedID(decl.Renamed_Entity, ctx);
        SgScopeStatement&       scope   = ctx.scope();
        SgAdaRenamingDecl&      sgnode  = mkAdaRenamingDecl(adaname.ident, aliased, scope);

        attachSourceLocation(sgnode, elem, ctx);
        privatize(sgnode, isPrivate);
        scope.append_statement(&sgnode);
        ROSE_ASSERT(sgnode.get_parent() == &scope);
        /* unhandled field
             Declaration_ID                 Corresponding_Declaration;
             Expression_ID                  Corresponding_Base_Entity;
        */
        break;
      }

    case An_Exception_Renaming_Declaration:        // 8.5.2(2)
      {
        logKind("An_Exception_Renaming_Declaration");

        NameData                adaname = singleName(decl, ctx);

        if (isInvalidId(decl.Renamed_Entity))
        {
          logWarn() << "skipping unknown renaming: " << adaname.ident << "/" << adaname.fullName
                    << ": " << elem.ID << " / " << decl.Renamed_Entity
                    << std::endl;
          return;
        }

        Element_Struct& renamed_entity_elem = retrieveAs<Element_Struct>(elemMap(), decl.Renamed_Entity);
        ROSE_ASSERT(renamed_entity_elem.Element_Kind == An_Expression);

        Expression_Struct& renamed_entity_expr = renamed_entity_elem.The_Union.Expression;

        // PP (11/18/20) Test whether Corresponding_Name_Definition is a valid ID
        if (isInvalidId(renamed_entity_expr.Corresponding_Name_Definition))
        {
          logError() << "unavailable name definition" << std::endl;
        }
        else
        {
        SgInitializedName& aliased = getAliasedExcnDecl(renamed_entity_expr.Corresponding_Name_Definition, ctx);
        SgScopeStatement&       scope   = ctx.scope();
        SgAdaRenamingDecl&      sgnode  = mkAdaRenamingDecl(adaname.ident, aliased, scope);

        attachSourceLocation(sgnode, elem, ctx);
        privatize(sgnode, isPrivate);
        scope.append_statement(&sgnode);
        ROSE_ASSERT(sgnode.get_parent() == &scope);
        }
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
    case A_Tagged_Incomplete_Type_Declaration:     //  3.10.1(2)
    case A_Private_Extension_Declaration:          // 3.2.1(2):7.3(3) -> Trait_Kinds
    case A_Single_Protected_Declaration:           // 3.3.1(2):9.4(2)
    case A_Discriminant_Specification:             // 3.7(5)   -> Trait_Kinds
    case An_Enumeration_Literal_Specification:     // 3.5.1(3)
    case A_Generalized_Iterator_Specification:     // 5.5.2    -> Trait_Kinds
    case An_Element_Iterator_Specification:        // 5.5.2    -> Trait_Kinds
    case A_Return_Variable_Specification:          // 6.5
    case A_Return_Constant_Specification:          // 6.5
    case A_Null_Procedure_Declaration:             // 6.7
    case An_Object_Renaming_Declaration:           // 8.5.1(2)
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

/// asserts that \ref decl has exactly one associated name
/// and returns the respective NameData data.
NameData
singleName(Declaration_Struct& decl, AstContext ctx)
{
  ElemIdRange     range = idRange(decl.Names);
  ROSE_ASSERT(range.size() == 1);

  return getNameID(*range.first, ctx);
}

NameData
getNameID(Element_ID el, AstContext ctx)
{
  return getName(retrieveAs<Element_Struct>(elemMap(), el), ctx);
}


}
