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
  /// a type encapsulating a lambda function that completes a body
  ///   after it has been structurally connected to the AST.
  using DeferredSimpleBodyCompletion = std::function<void()>;

  // may need special processing for pragmas in body
  using DeferredPragmaBodyCompletion = std::function<void(AstContext::PragmaContainer)>;

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

  //
  // pragma handling

  SgPragmaDeclaration&
  createPragma_common(Element_Struct& el, SgStatement* stmtOpt, AstContext ctx)
  {
    ADA_ASSERT (el.Element_Kind == A_Pragma);
    logKind("A_Pragma", el.ID);

    Pragma_Struct&             pragma   = el.The_Union.The_Pragma;
    std::string                name{pragma.Pragma_Name_Image};
    ElemIdRange                argRange = idRange(pragma.Pragma_Argument_Associations);
    SgExprListExp&             arglist  = mkExprListExp();
    SgPragmaDeclaration&       sgnode   = mkPragmaDeclaration(name, arglist, stmtOpt);
    ADA_ASSERT (arglist.get_parent());

    sgnode.set_parent(&ctx.scope()); // set fictitious parent (will be overwritten when pragma is actually placed)

    std::vector<SgExpression*> args     = traverseIDs(argRange, elemMap(), ArgListCreator{ctx.pragmaAspectAnchor(sgnode)});

    arglist.get_expressions().reserve(args.size());
    for (SgExpression* arg : args) arglist.append_expression(arg);

    // \todo do we need to privatize pragmas in the private section?
    attachSourceLocation(sgnode, el, ctx);
    attachSourceLocation(SG_DEREF(sgnode.get_pragma()), el, ctx);
    recordNode(asisDecls(), el.ID, sgnode);

    return sgnode;
  }

  struct PragmaCreator
  {
      using result_container = std::vector<SgPragmaDeclaration*>;

      PragmaCreator(const ASIS_element_id_to_ASIS_MapType& m, AstContext astctx)
      : res(), elementMap(m), ctx(astctx)
      {}

      void createPragma(Element_Struct& el, SgStatement* stmt)
      {
        // if the pragma was already processed, then skip processing
        // \todo currently secondary pragma-stmt associations are not stored
        //       in the pragma. This could be achieved by using a vector
        //       of statement pointers instead of a single statement.
        if (findFirst(asisDecls(), el.ID))
          return;

        //~ logWarn() << "fallback to legacy pragma procesing.."
                  //~ << std::endl;

        res.push_back(&createPragma_common(el, stmt, ctx));
      }

      void createPragma(Element_Struct* el, const ExtendedPragmaID& pid)
      {
        if (el == nullptr)
        {
          logFlaw() << "unable to retrieve pragma " << pid.id() << std::endl;
          return;
        }

        createPragma(*el, pid.stmt());
      }

      void operator()(const ExtendedPragmaID& pid)
      {
        createPragma(retrieveElemOpt(elementMap, pid.id()), pid);
      }

      operator result_container() && { return std::move(res); }

    private:
      result_container                       res;
      const ASIS_element_id_to_ASIS_MapType& elementMap;
      AstContext                             ctx;
  };


  struct SourceLocationLessThan
  {
    static constexpr bool DEBUG_LOCATION = false;

    bool operator()(const Sg_File_Info* lhs, const Sg_File_Info* rhs) const
    {
      ADA_ASSERT (lhs && rhs);

      if (lhs->get_line() < rhs->get_line())
      {
        if (DEBUG_LOCATION) logTrace() << "ll< " << lhs->get_line() << "/" << rhs->get_line() << std::endl;
        return true;
      }

      if (rhs->get_line() < lhs->get_line())
      {
        if (DEBUG_LOCATION) logTrace() << "rl< " << lhs->get_line() << "/" << rhs->get_line() << std::endl;
        return false;
      }

      if (lhs->get_col() < rhs->get_col())
      {
        if (DEBUG_LOCATION) logTrace() << "lc< " << lhs->get_col() << "/" << rhs->get_col() << std::endl;
        return true;
      }

      if (DEBUG_LOCATION) logTrace() << "ff< " << lhs->get_col() << "/" << rhs->get_col() << std::endl;
      return false;
    }

    bool operator()(const SgLocatedNode* n, const Sg_File_Info* rhs) const
    {
      ADA_ASSERT (n);

      return (*this)(n->get_startOfConstruct(), rhs);
    }

    bool operator()(const SgLocatedNode* lhs, const SgLocatedNode* rhs) const
    {
      ADA_ASSERT ((lhs != nullptr) && (rhs != nullptr));

      //~ logError() << typeid(*lhs).name() << " " << typeid(*rhs).name() << std::endl;
      return (*this)(lhs->get_startOfConstruct(), rhs->get_startOfConstruct());
    }
  };


  struct PragmaPlacer
  {
      // \pre scopes.size() > 0
      PragmaPlacer(std::vector<SgScopeStatement*> scopes)
      : all(), last(SG_DEREF(scopes.back()))
      {
        for (SgScopeStatement* pscope : scopes)
          copyToAll(SG_DEREF(pscope));
      }

      template <class Iterator>
      void copyToAll(Iterator begin, Iterator limit)
      {
        using SageStmtPtr = decltype(*begin);

        auto  hasSourceLocation = [](SageStmtPtr p) -> bool { return !p->isCompilerGenerated(); };

        std::copy_if( begin, limit,
                      std::back_inserter(all),
                      hasSourceLocation
                    );
      }

      template <class Sequence>
      void copyToAll(Sequence& seq)
      {
        copyToAll(seq.begin(), seq.end());
      }

      void copyToAll(SgScopeStatement& lst)
      {
        if (lst.containsOnlyDeclarations())
          copyToAll(lst.getDeclarationList());
        else
          copyToAll(lst.getStatementList());
      }

      void operator()(SgPragmaDeclaration* pragma)
      {
        using iterator = std::vector<SgStatement*>::iterator;

        ADA_ASSERT (pragma);

        // \todo to resolve the confusion where to insert the pragma
        //       (in the first or second scope)
        //       we could use the scope of SgPragma::get_associatedStmt
        // SgPragma* details = pragma->get_pragma();

        iterator       beg = all.begin();
        iterator       lim = all.end();
        iterator const pos = std::lower_bound( beg, lim,
                                               pragma->get_startOfConstruct(),
                                               SourceLocationLessThan{}
                                             );

        // The following if-else structure gives preference
        //   to inserting the pragma into the first scope.
        //   \todo see pragmaplacenment.adb for examples where this method fails.
        if (pos == lim)
          SageInterface::appendStatement(pragma, &last);
        else if (pos == beg)
          SageInterface::insertStatementBefore(*pos, pragma);
        else
          SageInterface::insertStatementAfter(*std::prev(pos), pragma); // gives preference to end of first scope

        // insert into combined list
        all.insert(pos, pragma);

        if (false) // debug all.insert \todo remove
        {
          bool sorted = all.end() == std::adjacent_find( all.begin(), all.end(),
                                                         [](const SgLocatedNode* lhs, const SgLocatedNode* rhs)
                                                         {
                                                           return SourceLocationLessThan{}(rhs, lhs);
                                                         }
                                                       );

          if (!sorted)
          {
            if (pos == lim)      logTrace() << "lim" << std::endl;
            else if (pos == beg) logTrace() << "beg" << std::endl;
            else                 logTrace() << "mid" << std::endl;
          }

          ADA_ASSERT(sorted);
        }
      }

    private:
      std::vector<SgStatement*> all;  ///< combined list of pragmas
      SgScopeStatement&         last;
  };


  // records all pragmas in the range \ref pragmas so that they can processed at the end of the block
  // \note this is necessary because it seems that Asis does not store pragmas on declare blocks
  //       (field Statement::Pragmas).
  void recordPragmasID(std::vector<Element_ID> pragmas, SgStatement* stmt, AstContext ctx)
  {
    if (pragmas.empty()) return;

    if (!ctx.collectsPragmas())
    {
      logFlaw() << "no place to collect available pragmas.." << std::endl;
      return;
    }

    AstContext::PragmaContainer& cont = ctx.pragmas();

    std::transform( pragmas.begin(), pragmas.end(),
                    std::back_inserter(cont),
                    [stmt](Element_ID id) -> ExtendedPragmaID { return { id, stmt }; }
                  );
  }

  void placePragmas(std::vector<SgScopeStatement*> scopes, AstContext ctx)
  {
    ADA_ASSERT(scopes.size() > 0);

    using PragmaNodes     = PragmaCreator::result_container;
    using PragmaContainer = AstContext::PragmaContainer;

    PragmaContainer& allpragmas = ctx.pragmas();

    if (allpragmas.empty()) return; // early exit to prevent scope flattening

    // sort pragmas by id, stmt
    auto byIdStmt = [](const ExtendedPragmaID& lhs, const ExtendedPragmaID& rhs) -> bool
                    {
                      if (lhs.id() < rhs.id()) return true;
                      if (lhs.id() > rhs.id()) return false;

                      // if ids equal, sort entries with statements before nullptr statements
                      return lhs.stmt() > rhs.stmt();
                    };

    std::sort(allpragmas.begin(), allpragmas.end(), byIdStmt);

    // remove duplicate pragma IDs, since any entry with an associated stmt is sorted before
    //   entries without associated stmt (nullptr), the entry with associated stmt survives.
    auto        idEquality = [](const ExtendedPragmaID& lhs, const ExtendedPragmaID& rhs) -> bool
                             {
                               return lhs.id() == rhs.id();
                             };
    auto const  lim        = std::unique(allpragmas.begin(), allpragmas.end(), idEquality);

    // generate declarations for pragmas and place them in file-lexicographical order
    PragmaNodes pragmadcls = std::for_each(allpragmas.begin(), lim, PragmaCreator{elemMap(), ctx});

    std::sort(pragmadcls.begin(), pragmadcls.end(), SourceLocationLessThan{});

    // retroactively place pragmas according to their source position information
    std::for_each(pragmadcls.begin(), pragmadcls.end(), PragmaPlacer{std::move(scopes)});
  }


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

    template <class SageDecl>
    void def(SageDecl* n) { if (n) def(*n); }

    void handle(SgNode& n)                   { SG_UNEXPECTED_NODE(n); }

    // declarations
    void handle(SgAdaPackageSpecDecl& n)     { def(n); }
    void handle(SgAdaPackageBodyDecl& n)     { def(n); }
    void handle(SgFunctionDeclaration& n)    { def(isSgFunctionDeclaration(n.get_definingDeclaration())); }

    void handle(SgAdaGenericDecl& n)
    {
      //~ SgDeclarationStatement* dcl = n.get_declaration();

      //~ if (isSgFunctionDeclaration(dcl)) // \todo correct?
        //~ return def(n);

      //~ res = find(dcl);
      res = find(n.get_declaration());
    }

    void handle(SgAdaGenericInstanceDecl& n)
    {
      SgBasicBlock&       scope   = SG_DEREF(isSgBasicBlock(n.get_instantiatedScope()));
      SgStatementPtrList& stmts   = scope.get_statements();
      SgStatement*        dclstmt = stmts.at(0);

      res = find(dclstmt);
    }

    // renamings
    void handle(SgAdaRenamingDecl& n) { res = find(n.get_renamed()); }
    void handle(SgAdaUnitRefExp& n)   { res = find(n.get_decl()); }

    // others
    void handle(SgBasicBlock& n)      { res = &n; }

    // \todo add handlers as needed
    // ...

    static
    ReturnType find(SgNode*);
  };

  ScopeQuery::ReturnType
  ScopeQuery::find(SgNode* n)
  {
    return sg::dispatch(ScopeQuery{}, n);
  }


  /// returns the ROSE scope of an already converted Asis element \ref elem.
  SgScopeStatement&
  queryScopeOf(Element_Struct& elem, AstContext ctx)
  {
    ADA_ASSERT (elem.Element_Kind == An_Expression);

    Expression_Struct& expr = elem.The_Union.Expression;

    if (expr.Expression_Kind == A_Selected_Component)
      return queryScopeOfID(expr.Selector, ctx);

    if (expr.Expression_Kind != An_Identifier)
      logError() << "unexpected identifier: " << expr.Expression_Kind;

    ADA_ASSERT (expr.Expression_Kind == An_Identifier);
    logKind("An_Identifier", elem.ID);

    SgNode* dcl = queryCorrespondingAstNode(expr, ctx);

    if (dcl == nullptr)
    {
      logFatal() << "Unable to find scope/declaration for " << expr.Name_Image
                 << std::endl;
      ADA_ASSERT(false);
    }

    return SG_DEREF(ScopeQuery::find(dcl));
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
    Element_Struct&   lblelem = retrieveElem(elemMap(), lblid);

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
    using name_container = NameCreator::result_container;

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

    ctx.appendStatement(sgn);
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

    ctx.appendStatement(sgn);
  }

  /// @}

  /// gets the body of a function declaration \ref defdcl
  /// \pre defdcl is the defining declaration
  SgBasicBlock& functionBody(SgFunctionDeclaration& defdcl)
  {
    SgFunctionDefinition* def = isSgFunctionDefinition(defdcl.get_definition());

    return SG_DEREF(SG_DEREF(def).get_body());
  }

  /// sets the override flag in \ref dcl
  void setOverride(SgDeclarationStatement& dcl, bool isOverride)
  {
    SgDeclarationModifier& sgmod = dcl.get_declarationModifier();

    if (isOverride) sgmod.setOverride(); else sgmod.unsetOverride();
  }

  /// sets the override flag in \ref dcl
  void setAdaSeparate(SgDeclarationStatement& dcl, bool isSeparate)
  {
    SgDeclarationModifier& sgmod = dcl.get_declarationModifier();

    if (isSeparate) sgmod.setAdaSeparate(); else sgmod.unsetAdaSeparate();
  }

  /// creates an initializer for a variable/parameter declaration if needed
  /// \param lst the subset of completed variable declarations
  /// \param exp the original initializing expression
  /// \param ctx the context
  /// \details
  ///    consider a variable or parameter declaration of the form.
  ///      a,b : Integer := InitExpr
  ///    The ROSE AST looks like:
  ///      int a = InitExpr, int b = InitExpr
  SgExpression*
  createInit(SgInitializedNamePtrList& lst, SgExpression* exp, AstContext ctx)
  {
    // the first variable declarations gets the original initializer
    if ((exp == nullptr) || lst.empty()) return exp;

    // \todo consider rebuilding from the ASIS expression
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
                                   map_t<int, SgInitializedName*>& m,
                                   const NameCreator::result_container& names,
                                   SgType& dcltype,
                                   SgExpression* initexpr,
                                   std::vector<Element_ID>& secondaries
                                 )
  {
    SgInitializedNamePtrList lst;

    for (const NameData& obj : names)
    {
      ADA_ASSERT (obj.fullName == obj.ident);

      const std::string& name = obj.fullName;
      Element_ID         id   = obj.id();
      SgExpression*      init = createInit(lst, initexpr, ctx);
      SgInitializedName& dcl  = mkInitializedName(name, dcltype, init);

      attachSourceLocation(dcl, retrieveElem(elemMap(), id), ctx);

      lst.push_back(&dcl);
      recordNonUniqueNode(m, id, dcl, true /* overwrite existing entries if needed */);

      //~ logError() << name << " = " << id << std::endl;

      if (!secondaries.empty())
      {
        //~ logError() << name << "' = " << secondaries.back() << std::endl;
        recordNonUniqueNode(m, secondaries.back(), dcl, true /* overwrite existing entries if needed */);
        secondaries.pop_back();
      }
    }

    return lst;
  }

  SgInitializedNamePtrList
  constructInitializedNamePtrList( AstContext ctx,
                                   map_t<int, SgInitializedName*>& m,
                                   const NameCreator::result_container& names,
                                   SgType& dcltype,
                                   SgExpression* initexpr
                                 )
  {
    std::vector<Element_ID> dummy;

    return constructInitializedNamePtrList(ctx, m, names, dcltype, initexpr, dummy);
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
        ADA_ASSERT(false);
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
               || decl.Declaration_Kind == A_Formal_Object_Declaration
               );

    //~ logWarn() << "decl.Initialization_Expression = " << decl.Initialization_Expression << std::endl;
    if (decl.Initialization_Expression == 0)
      return nullptr;

    return &getExprID(decl.Initialization_Expression, ctx);
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
    Element_Struct&    labelref = retrieveElem(elemMap(), id);
    ADA_ASSERT (labelref.Element_Kind == An_Expression);

    Expression_Struct& labelexp = labelref.The_Union.Expression;

    if (labelexp.Expression_Kind == A_Selected_Component)
    {
      logKind("A_Selected_Component", id);
      return getLabelRef(labelexp.Selector, ctx);
    }

    ADA_ASSERT (labelexp.Expression_Kind == An_Identifier);
    logKind("An_Identifier", labelref.ID);
    return labelexp.Corresponding_Name_Definition;
  }


  /// converts an Asis parameter declaration to a ROSE paramter (i.e., variable)
  ///   declaration.
  SgVariableDeclaration&
  getParm(Element_Struct& elem, AstContext ctx)
  {
    using name_container = NameCreator::result_container;

    ADA_ASSERT (elem.Element_Kind == A_Declaration);

    Declaration_Struct&      asisDecl = elem.The_Union.Declaration;
    ADA_ASSERT (  asisDecl.Declaration_Kind == A_Parameter_Specification
               || asisDecl.Declaration_Kind == A_Formal_Object_Declaration
               );

    ElemIdRange              range    = idRange(asisDecl.Names);
    name_container           names    = allNames(range, ctx);
    const bool               aliased  = (asisDecl.Declaration_Kind == A_Parameter_Specification) && asisDecl.Has_Aliased;
    SgType&                  basety   = getDeclTypeID(asisDecl.Object_Declaration_View, ctx);
    SgType&                  parmtype = aliased ? mkAliasedType(basety) : basety;
    SgInitializedNamePtrList dclnames = constructInitializedNamePtrList( ctx,
                                                                         asisVars(),
                                                                         names,
                                                                         parmtype,
                                                                         getVarInit(asisDecl, &parmtype, ctx)
                                                                       );
    SgVariableDeclaration&   sgnode   = mkParameter(dclnames, getMode(asisDecl.Mode_Kind), ctx.scope());

    attachSourceLocation(sgnode, elem, ctx);
    /* unused fields:
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
        SgVariableDeclaration& decl = getParm(elem, ctx);

        // in Ada multiple parameters can be declared
        //   within a single declaration.
        for (SgInitializedName* parm : decl.get_variables())
          parmlist.append_arg(parm);
      }

    private:
      SgFunctionParameterList& parmlist;
      AstContext               ctx;
  };


  /// converts an Asis parameter declaration to a ROSE parameter (i.e., variable)
  ///   declaration.
  SgVariableDeclaration&
  getDiscriminant(Element_Struct& elem, std::vector<Element_ID>& secondaries, AstContext ctx)
  {
    using name_container = NameCreator::result_container;

    ADA_ASSERT (elem.Element_Kind == A_Declaration);

    Declaration_Struct&      asisDecl = elem.The_Union.Declaration;
    ADA_ASSERT (asisDecl.Declaration_Kind == A_Discriminant_Specification);

    // SgType&                   dcltype = tyModifier(getVarType(decl, ctx));
    ElemIdRange              range    = idRange(asisDecl.Names);
    name_container           names    = allNames(range, ctx);
    SgType&                  basety   = getDeclTypeID(asisDecl.Object_Declaration_View, ctx);
    SgInitializedNamePtrList dclnames = constructInitializedNamePtrList( ctx,
                                                                         asisVars(),
                                                                         names,
                                                                         basety,
                                                                         getVarInit(asisDecl, &basety, ctx),
                                                                         secondaries
                                                                       );
    SgVariableDeclaration&   sgnode   = mkVarDecl(dclnames, ctx.scope());

    attachSourceLocation(sgnode, elem, ctx);
    /* unused fields:
    */
    return sgnode;
  }


  struct DiscriminantCreator
  {
      DiscriminantCreator(SgAdaDiscriminatedTypeDecl& discrNode, std::vector<Element_ID> sec, AstContext astctx)
      : params(SG_DEREF(discrNode.get_discriminants())), secondaries(std::move(sec)), ctx(astctx)
      {}

/*
      DiscriminantCreator(SgAdaFormalTypeDecl& discrNode, AstContext astctx)
      : params(SG_DEREF(discrNode.get_discriminants())), secondaries(), ctx(astctx)
      {}
*/
      void operator()(Element_Struct& elem)
      {
        SgVariableDeclaration& decl = getDiscriminant(elem, secondaries, ctx);

        params.append_parameter(&decl);
      }

    private:
      SgAdaParameterList&     params;
      std::vector<Element_ID> secondaries;
      AstContext              ctx;

      DiscriminantCreator() = delete;
  };

  struct VariantCreator
  {
      explicit
      VariantCreator(AstContext astctx)
      : ctx(astctx)
      {}

      void operator()(Element_Struct& elem)
      {
        ADA_ASSERT (elem.Element_Kind == A_Definition);

        Definition_Struct&  def = elem.The_Union.Definition;
        ADA_ASSERT (def.Definition_Kind == A_Variant);

        Variant_Struct&     variant = def.The_Union.The_Variant;
        ElemIdRange         choiceRange = idRange(variant.Variant_Choices);
        SgExpressionPtrList exprlst   = traverseIDs(choiceRange, elemMap(), ExprSeqCreator{ctx});
        SgExprListExp&      choicelst = mkExprListExp(exprlst);

        SgAdaVariantWhenStmt& sgnode  = mkAdaVariantWhenStmt(choicelst);
        ElemIdRange         compRange = idRange(variant.Record_Components);
        SgAdaUnscopedBlock& blk = SG_DEREF(sgnode.get_components());

        ctx.appendStatement(sgnode);
        traverseIDs(compRange, elemMap(), ElemCreator{ ctx.unscopedBlock(blk) });

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
      logFlaw() << name << " " << typeid(*nondefdcl).name() << " not a class declaration!"
                << std::endl;

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


  template <class AsisStruct>
  void
  setAbstractModifier(SgDeclarationStatement& dcl, const AsisStruct& info)
  {
    setModifiers(dcl, info.Has_Abstract, false, false);
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

      void handle(SgAdaDiscriminatedTypeDecl& n)
      {
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
        SgEnumDeclaration* enmdcl = si::Ada::baseEnumDeclaration(n);

        if (enmdcl == nullptr)
          return handle(sg::asBaseType(n));

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


  SgDeclarationStatement&
  handleNumVarCstDecl( Element_Struct& elem,
                       Declaration_Struct& decl,
                       AstContext ctx,
                       bool isPrivate,
                       SgType& dclType,
                       SgType* expectedType = nullptr
                     )
  {
    typedef NameCreator::result_container name_container;

    ElemIdRange              range    = idRange(decl.Names);
    name_container           names    = allNames(range, ctx);
    SgScopeStatement&        scope    = ctx.scope();
    //
    // https://www.adaic.com/resources/add_content/standards/05rm/html/RM-3-3-1.html#S0032
    // $7: Any declaration that includes a defining_identifier_list with more than one defining_identifier
    //     is equivalent to a series of declarations each containing one defining_identifier from the list,
    //     with the rest of the text of the declaration copied for each declaration in the series, in the
    //     same order as the list. The remainder of this International Standard relies on this equivalence;
    //     explanations are given for declarations with a single defining_identifier.
    // => clone the initializing expression instead of creating references to the first one..
    SgInitializedNamePtrList dclnames = constructInitializedNamePtrList( ctx,
                                                                         asisVars(),
                                                                         names,
                                                                         dclType,
                                                                         getVarInit(decl, expectedType, ctx)
                                                                       );
    SgVariableDeclaration&   sgnode   = mkVarDecl(dclnames, scope);

    attachSourceLocation(sgnode, elem, ctx);
    privatize(sgnode, isPrivate);
    ctx.appendStatement(sgnode);
    return sgnode;
  }

  SgDeclarationStatement&
  handleNumberDecl(Element_Struct& elem, Declaration_Struct& decl, AstContext ctx, bool isPrivate, SgType& numty, SgType& expctty)
  {
    SgType& cstty = SG_DEREF( sb::buildConstType(&numty) );

    return handleNumVarCstDecl(elem, decl, ctx, isPrivate, cstty, &expctty);
  }

  SgDeclarationStatement&
  handleVarCstDecl( Element_Struct& elem,
                    Declaration_Struct& dcl,
                    AstContext ctx,
                    bool isPrivate,
                    TypeModifierFn constMaker
                  )
  {
    SgType& basety = constMaker(getVarType(dcl, ctx));
    SgType& varty  = dcl.Has_Aliased ? mkAliasedType(basety) : basety;

    return handleNumVarCstDecl(elem, dcl, ctx, isPrivate, varty);
  }


  std::pair<SgAdaTaskSpec*, DeferredPragmaBodyCompletion>
  getTaskSpec(Element_Struct& elem, AstContext ctx)
  {
    ADA_ASSERT (elem.Element_Kind == A_Definition);
    logKind("A_Definition", elem.ID);

    Definition_Struct&      def = elem.The_Union.Definition;
    ADA_ASSERT (def.Definition_Kind == A_Task_Definition);

    logKind("A_Task_Definition", elem.ID);

    Task_Definition_Struct* tasknode = &def.The_Union.The_Task_Definition;
    SgAdaTaskSpec&          sgnode   = mkAdaTaskSpec();
    SgAdaTaskSpec*          nodePtr  = &sgnode;

    // sgnode.set_hasMembers(true);
    sgnode.set_hasPrivate(tasknode->Is_Private_Present);

    auto deferred = [ctx,nodePtr,tasknode](AstContext::PragmaContainer pragmas) -> void
                    {
                      AstContext pragmaCtx = ctx.pragmas(pragmas);

                      // visible items
                      {
                        ElemIdRange range = idRange(tasknode->Visible_Part_Items);

                        traverseIDs(range, elemMap(), ElemCreator{pragmaCtx.scope(*nodePtr)});
                      }

                      // private items
                      {
                        ElemIdRange range = idRange(tasknode->Private_Part_Items);
                        ADA_ASSERT ((!range.empty()) == tasknode->Is_Private_Present);

                        traverseIDs(range, elemMap(), ElemCreator{pragmaCtx.scope(*nodePtr), true /* private items */});
                      }

                      placePragmas({ nodePtr }, pragmaCtx.scope(*nodePtr));
                    };

    /* unused fields: (Task_Definition_Struct)
         bool                  Has_Task;
         Declaration_List      Corresponding_Type_Operators;
    */
    return std::make_pair(&sgnode, deferred);
  }

  void nothingToComplete(AstContext::PragmaContainer pragmas)
  {
     ADA_ASSERT(pragmas.empty());
  }

  std::pair<SgAdaTaskSpec*, DeferredPragmaBodyCompletion>
  getTaskSpecID_opt(Element_ID id, AstContext ctx)
  {
    if (id == 0)
      return std::make_pair(&mkAdaTaskSpec(), nothingToComplete);

    return getTaskSpec(retrieveElem(elemMap(), id), ctx);
  }

  std::pair<SgAdaTaskSpec*, DeferredPragmaBodyCompletion>
  getTaskSpecForTaskType(Declaration_Struct& decl, AstContext ctx)
  {
    ADA_ASSERT (decl.Declaration_Kind == A_Task_Type_Declaration);

    return getTaskSpecID_opt(decl.Type_Declaration_View, ctx);
  }

  std::pair<SgAdaTaskSpec*, DeferredPragmaBodyCompletion>
  getTaskSpecForSingleTask(Declaration_Struct& decl, AstContext ctx)
  {
    ADA_ASSERT (decl.Declaration_Kind == A_Single_Task_Declaration);

    return getTaskSpecID_opt(decl.Object_Declaration_View, ctx);
  }

  //
  // protected objects
  //
  // protected objects are represented as ClassDeclaration and ClassDefinition

  std::pair<SgAdaProtectedSpec*, DeferredPragmaBodyCompletion>
  getProtectedSpec(Element_Struct& elem, AstContext ctx)
  {
    ADA_ASSERT (elem.Element_Kind == A_Definition);
    logKind("A_Definition", elem.ID);

    Definition_Struct&      def = elem.The_Union.Definition;
    ADA_ASSERT (def.Definition_Kind == A_Protected_Definition);

    logKind("A_Protected_Definition", elem.ID);

    Protected_Definition_Struct* protectedNode = &def.The_Union.The_Protected_Definition;
    SgAdaProtectedSpec&     sgnode = mkAdaProtectedSpec();
    SgAdaProtectedSpec*     nodePtr  = &sgnode;

    sgnode.set_hasPrivate(protectedNode->Is_Private_Present);

    auto deferred = [ctx,nodePtr,protectedNode](AstContext::PragmaContainer pragmas) -> void
                    {
                      AstContext pragmaCtx = ctx.pragmas(pragmas);

                      // visible items
                      {
                        ElemIdRange range = idRange(protectedNode->Visible_Part_Items);

                        traverseIDs(range, elemMap(), ElemCreator{pragmaCtx.scope(*nodePtr)});
                      }

                      // private items
                      {
                        ElemIdRange range = idRange(protectedNode->Private_Part_Items);
                        ADA_ASSERT ((!range.empty()) == protectedNode->Is_Private_Present);

                        traverseIDs(range, elemMap(), ElemCreator{pragmaCtx.scope(*nodePtr), true /* private items */});
                      }

                      placePragmas({ nodePtr }, pragmaCtx.scope(*nodePtr));
                    };

    /* unused fields: (Protected_Definition_Struct)
         bool                  Has_Protected;
         Declaration_List      Corresponding_Type_Operators;
    */
    return std::make_pair(&sgnode, deferred);
  }

  std::pair<SgAdaProtectedSpec*, DeferredPragmaBodyCompletion>
  getProtectedSpecID(Element_ID id, AstContext ctx)
  {
    return getProtectedSpec(retrieveElem(elemMap(), id), ctx);
  }

  std::pair<SgAdaProtectedSpec*, DeferredPragmaBodyCompletion>
  getProtectedSpecForProtectedType(Declaration_Struct& decl, AstContext ctx)
  {
    ADA_ASSERT (decl.Declaration_Kind == A_Protected_Type_Declaration);

    if (decl.Type_Declaration_View == 0)
      return std::make_pair(&mkAdaProtectedSpec(), nothingToComplete);

    return getProtectedSpecID(decl.Type_Declaration_View, ctx);
  }


  std::pair<SgAdaProtectedSpec*, DeferredPragmaBodyCompletion>
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

        Element_Struct& el = retrieveElem(elemMap(), *choices.first);

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
              logKind("A_Case_Path", elem.ID);

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
            ADA_ASSERT(false);
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
              logKind("An_If_Path", elem.ID);
              ADA_ASSERT (ifStmt);
              conditionedBranch(path);
              break;
            }

          case An_Elsif_Path:
            {
              logKind("An_Elsif_Path", elem.ID);
              ADA_ASSERT (ifStmt);

              SgIfStmt& cascadingIf = mkIfStmt(true /* elsif */);

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
              logKind("An_Else_Path", elem.ID);
              ADA_ASSERT (ifStmt);
              commonBranch(path, &SgIfStmt::set_false_body);
              break;
            }

          default:
            ADA_ASSERT(false);
        }
      }

    private:
      SgIfStmt*        ifStmt;
      AstContext       ctx;

      IfStmtCreator() = delete;
  };

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

    std::pair<SgAdaSelectAlternativeStmt*, DeferredSimpleBodyCompletion>
    commonAltStmt(Path_Struct& path) {
      // create body of alternative
      SgBasicBlock* block = &mkBasicBlock();

      // create guard
      SgExpression& guard = getExprID_opt(path.Guard, ctx);

      // instantiate SgAdaSelectAlternativeStmt node and return it
      SgAdaSelectAlternativeStmt& stmt = mkAdaSelectAlternativeStmt(guard, *block);
      Path_Struct* pathptr = &path;
      AstContext   astctx{ctx};

      auto completion = [pathptr, block, astctx]() -> void
                        {
                          ElemIdRange altStmts = idRange(pathptr->Sequence_Of_Statements);

                          traverseIDs(altStmts, elemMap(), StmtCreator{astctx.scope(*block)});
                        };

      return std::make_pair(&stmt, completion);
    }

    std::pair<SgBasicBlock*, DeferredSimpleBodyCompletion>
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
          ADA_ASSERT(false);
        }
    }

  private:
    SgAdaSelectAlternativeStmt*       currOrPath    = nullptr;
    SgAdaSelectStmt*                  sgnode        = nullptr;
    AstContext                        ctx;

    SelectStmtCreator() = delete;
  };




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
  createBlockIfNeeded(bool newStatementBlock, AstContext ctx)
  {
    if (!newStatementBlock) return ctx.scope();

    SgBasicBlock& newblk = mkBasicBlock();

    ctx.appendStatement(newblk);
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
  createTryOrBlockIfNeeded(bool hasHandlers, bool requiresStmtBlock, AstContext ctx)
  {
    if (!hasHandlers) return TryBlockNodes{nullptr, createBlockIfNeeded(requiresStmtBlock, ctx)};

    SgBasicBlock& tryBlock = mkBasicBlock();
    SgTryStmt&    tryStmt  = mkTryStmt(tryBlock);

    ctx.appendStatement(tryStmt);

    return TryBlockNodes{&tryStmt, tryBlock};
  }


  using BlockHandler = std::function<void(Statement_List, SgScopeStatement&, AstContext ctx)>;
  using ExceptionHandlerHandler = std::function<void(ElemIdRange, SgScopeStatement&, SgTryStmt&, AstContext ctx)>;

  // a simple block handler just traverses the statement list and adds them to the \ref blk.
  void simpleBlockHandler(Statement_List bodyStatements, SgScopeStatement& blk, AstContext ctx)
  {
    ElemIdRange         range = idRange(bodyStatements);

    traverseIDs(range, elemMap(), StmtCreator{ctx.scope(blk)});
  }

  // at some point loops, labels, gotos need to be patched up. In this case, we do that at the
  //   end of a routine through the use of the LoopAndLabelManager.
  void routineBlockHandler(Statement_List bodyStatements, SgScopeStatement& blk, AstContext ctx)
  {
    LabelAndLoopManager lblmgr;

    simpleBlockHandler(bodyStatements, blk, ctx.labelsAndLoops(lblmgr));
  }

  void simpleExceptionBlockHandler(ElemIdRange handlers, SgScopeStatement& blk, SgTryStmt& trystmt, AstContext ctx)
  {
    traverseIDs(handlers, elemMap(), ExHandlerCreator{ctx.scope(blk), trystmt});
  }

  // at some point loops, labels, gotos need to be patched up. In this case, we do that at the
  //   end of a routine through the use of the LoopAndLabelManager
  void routineExceptionBlockHandler(ElemIdRange handlers, SgScopeStatement& blk, SgTryStmt& trystmt, AstContext ctx)
  {
    LabelAndLoopManager lblmgr;

    simpleExceptionBlockHandler(handlers, blk, trystmt, ctx.labelsAndLoops(lblmgr));
  }

  // completes any block with exception handlers and pragmas attached
  void completeHandledBlock( Statement_List bodyStatements,
                             Exception_Handler_List exceptionHandlers,
                             Pragma_Element_ID_List pragmas,
                             BlockHandler blockHandler,
                             ExceptionHandlerHandler exhandlerHandler,
                             SgScopeStatement& dominantBlock,
                             bool requiresStatementBlock,
                             AstContext ctx,
                             AstContext::PragmaContainer pendingPragmas = {} // moved in to capture pragmas seen earlier
                           )
  {
    using ScopeSequence = std::vector<SgScopeStatement*>;

    ElemIdRange       hndlrs    = idRange(exceptionHandlers);
    TryBlockNodes     trydata   = createTryOrBlockIfNeeded( hndlrs.size() > 0, requiresStatementBlock, ctx.scope(dominantBlock));
    SgTryStmt*        trystmt   = trydata.first;
    SgScopeStatement& stmtblk   = trydata.second;
    AstContext        pragmaCtx = ctx.pragmas(pendingPragmas);
    ScopeSequence     activeScopes = { &dominantBlock };

    if (&dominantBlock != &stmtblk)
      activeScopes.push_back(&stmtblk);

    blockHandler(bodyStatements, stmtblk, pragmaCtx);

    if (trystmt)
    {
      exhandlerHandler(hndlrs, dominantBlock, *trystmt, pragmaCtx);

      computeSourceRangeFromChildren(SG_DEREF(trystmt->get_body()));
      computeSourceRangeFromChildren(SG_DEREF(trystmt->get_catch_statement_seq_root()));
      computeSourceRangeFromChildren(*trystmt);
    }

    processAndPlacePragmas(pragmas, std::move(activeScopes), pragmaCtx.scope(dominantBlock));
  }



  // completes any block with declarative items and exception handlers and pragmas attached
  void completeDeclarationsWithHandledBlock( Element_ID_List declarativeItems,
                                             Statement_List bodyStatements,
                                             Exception_Handler_List exceptionHandlers,
                                             Pragma_Element_ID_List pragmas,
                                             BlockHandler blockHandler,
                                             ExceptionHandlerHandler exhandlerHandler,
                                             SgScopeStatement& dominantBlock,
                                             bool requiresStatementBlock,
                                             AstContext ctx
                                           )
  {
    using PragmaContainer = AstContext::PragmaContainer;

    ElemIdRange     range = idRange(declarativeItems);
    PragmaContainer pendingPragmas;
    AstContext      pragmaCtx = ctx.pragmas(pendingPragmas);

    traverseIDs(range, elemMap(), ElemCreator{pragmaCtx.scope(dominantBlock)});

    completeHandledBlock( bodyStatements,
                          exceptionHandlers,
                          pragmas,
                          blockHandler,
                          exhandlerHandler,
                          dominantBlock,
                          requiresStatementBlock,
                          pragmaCtx,
                          std::move(pendingPragmas)
                        );
  }

  void completeRoutineBody(Declaration_Struct& decl, SgBasicBlock& declblk, AstContext ctx)
  {
    completeDeclarationsWithHandledBlock( decl.Body_Declarative_Items,
                                          decl.Body_Statements,
                                          decl.Body_Exception_Handlers,
                                          decl.Pragmas,
                                          routineBlockHandler,
                                          routineExceptionBlockHandler,
                                          declblk,
                                          false /* same block for declarations and statements */,
                                          ctx
                                        );
  }

  // handles elements that can appear in a statement context (from a ROSE point of view)
  //   besides statements this also includes declarations and clauses
  void handleStmt(Element_Struct& elem, AstContext ctx)
  {
    using PragmaContainer = AstContext::PragmaContainer;

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

    if (elem.Element_Kind == A_Pragma)
    {
      handlePragma(elem, nullptr, ctx);
      return;
    }

    //~ logError() << ":: " << elem.Element_Kind << std::endl;
    ADA_ASSERT (elem.Element_Kind == A_Statement);

    SgStatement*            assocstmt = nullptr;
    Statement_Struct&       stmt = elem.The_Union.Statement;
    ElemIdRange             pragmaRange  = idRange(stmt.Corresponding_Pragmas);
    std::vector<Element_ID> pragmaVector;

    std::copy(pragmaRange.first, pragmaRange.second, std::back_inserter(pragmaVector));

    switch (stmt.Statement_Kind)
    {
      case A_Null_Statement:                    // 5.1
        {
          logKind("A_Null_Statement", elem.ID);

          SgNullStatement& sgnode = mkNullStatement();

          completeStmt(sgnode, elem, ctx);
          /* unused fields:
          */
          assocstmt = &sgnode;
          break;
        }

      case An_Assignment_Statement:             // 5.2
        {
          logKind("An_Assignment_Statement", elem.ID);

          SgExpression&    lhs    = getExprID(stmt.Assignment_Variable_Name, ctx);
          SgExpression&    rhs    = getExprID(stmt.Assignment_Expression, ctx);
          SgExprStatement& sgnode = mkAssignStmt(lhs, rhs);
          SgExpression&    assign = SG_DEREF(sgnode.get_expression());

          attachSourceLocation(assign, elem, ctx);
          completeStmt(sgnode, elem, ctx);
          /* unused fields:
          */
          assocstmt = &sgnode;
          break;
        }

      case An_If_Statement:                     // 5.3
        {
          logKind("An_If_Statement", elem.ID);

          SgIfStmt&   sgnode = mkIfStmt();
          ElemIdRange range  = idRange(stmt.Statement_Paths);

          completeStmt(sgnode, elem, ctx);
          traverseIDs(range, elemMap(), IfStmtCreator{sgnode, ctx});

          /* unused fields:
          */
          assocstmt = &sgnode;
          break;
        }

      case A_Case_Statement:                    // 5.4
        {
          logKind("A_Case_Statement", elem.ID);

          SgExpression&      caseexpr = getExprID(stmt.Case_Expression, ctx);
          SgBasicBlock&      casebody = mkBasicBlock();
          SgSwitchStatement& sgnode   = mkAdaCaseStmt(caseexpr, casebody);

          completeStmt(sgnode, elem, ctx);

          ElemIdRange        range    = idRange(stmt.Statement_Paths);

          traverseIDs(range, elemMap(), CaseStmtCreator{ctx.scope(casebody), sgnode});
          /* unused fields:
          */
          assocstmt = &sgnode;
          break;
        }

      case A_While_Loop_Statement:              // 5.5
        {
          logKind("A_While_Loop_Statement", elem.ID);

          SgExpression&   cond     = getExprID(stmt.While_Condition, ctx);
          SgBasicBlock&   block    = mkBasicBlock();
          ElemIdRange     adaStmts = idRange(stmt.Loop_Statements);
          SgWhileStmt&    sgnode   = mkWhileStmt(cond, block);

          completeStmt(sgnode, elem, ctx, stmt.Statement_Identifier);
          recordNode(ctx.labelsAndLoops().asisLoops(), elem.ID, sgnode);

          PragmaContainer pendingPragmas;
          AstContext      pragmaCtx  = ctx.pragmas(pendingPragmas);

          traverseIDs(adaStmts, elemMap(), StmtCreator{pragmaCtx.scope(block)});
          processAndPlacePragmas(stmt.Pragmas, { &block }, pragmaCtx); // pragmaCtx.scope(block) ?
          /* unused fields:
                Element_ID                Corresponding_End_Name;
          */
          assocstmt = &sgnode;
          break;
        }

        case A_Loop_Statement:                    // 5.5
        {
          logKind("A_Loop_Statement", elem.ID);

          SgBasicBlock&   block    = mkBasicBlock();
          ElemIdRange     adaStmts = idRange(stmt.Loop_Statements);
          SgAdaLoopStmt&  sgnode   = mkAdaLoopStmt(block);

          completeStmt(sgnode, elem, ctx, stmt.Statement_Identifier);

          recordNode(ctx.labelsAndLoops().asisLoops(), elem.ID, sgnode);

          PragmaContainer pendingPragmas;
          AstContext      pragmaCtx  = ctx.pragmas(pendingPragmas);

          traverseIDs(adaStmts, elemMap(), StmtCreator{pragmaCtx.scope(block)});
          processAndPlacePragmas(stmt.Pragmas, { &block }, pragmaCtx); // pragmaCtx.scope(block) ?

          /* unused fields:
                Element_ID                Corresponding_End_Name;
                bool                      Is_Name_Repeated;
          */
          assocstmt = &sgnode;
          break;
        }

      case A_For_Loop_Statement:                // 5.5
        {
          logKind("A_For_Loop_Statement", elem.ID);

          SgBasicBlock&          block  = mkBasicBlock();
          SgForStatement&        sgnode = mkForStatement(block);
          Element_Struct&        forvar = retrieveElem(elemMap(), stmt.For_Loop_Parameter_Specification);
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

          // test is not strictly necessary; added for convenience
          SgStatement&           test    = mkForLoopTest(isForwardLoop(forvar), SG_DEREF(inductionVar));
          sg::linkParentChild(sgnode, test, &SgForStatement::set_test);

          PragmaContainer pendingPragmas;
          AstContext      pragmaCtx = ctx.pragmas(pendingPragmas);

          // loop body
          {
            ElemIdRange            loopStmts = idRange(stmt.Loop_Statements);

            recordNode(pragmaCtx.labelsAndLoops().asisLoops(), elem.ID, sgnode);
            traverseIDs(loopStmts, elemMap(), StmtCreator{pragmaCtx.scope(block)});
          }

          processAndPlacePragmas(stmt.Pragmas, { &block }, pragmaCtx); // pragmaCtx.scope(block) ?

          /* unused fields:
               Element_ID             Corresponding_End_Name;
          */
          assocstmt = &sgnode;
          break;
        }

      case A_Block_Statement:                   // 5.6
        {
          logKind("A_Block_Statement", elem.ID);

          SgBasicBlock& sgnode   = mkBasicBlock();

          recordNode(asisBlocks(), elem.ID, sgnode);
          completeStmt(sgnode, elem, ctx, stmt.Statement_Identifier);

          completeDeclarationsWithHandledBlock( stmt.Block_Declarative_Items,
                                                stmt.Block_Statements,
                                                stmt.Block_Exception_Handlers,
                                                stmt.Pragmas,
                                                simpleBlockHandler,
                                                simpleExceptionBlockHandler,
                                                sgnode,
                                                false /* same block for declarations and statements */,
                                                ctx
                                              );

          /* unused fields:
                Element_ID                Corresponding_End_Name;
                bool                      Is_Name_Repeated;
                bool                      Is_Declare_Block;
          */
          assocstmt = &sgnode;
          break;
        }

      case An_Exit_Statement:                   // 5.7
        {
          logKind("An_Exit_Statement", elem.ID);

          SgStatement&  exitedLoop    = lookupNode(ctx.labelsAndLoops().asisLoops(), stmt.Corresponding_Loop_Exited);
          SgExpression& exitCondition = getExprID_opt(stmt.Exit_Condition, ctx);
          const bool    loopIsNamed   = stmt.Exit_Loop_Name > 0;
          SgStatement&  sgnode        = mkAdaExitStmt(exitedLoop, exitCondition, loopIsNamed);

          completeStmt(sgnode, elem, ctx);
          /* unused fields:
          */
          assocstmt = &sgnode;
          break;
        }

      case A_Goto_Statement:                    // 5.8
        {
          logKind("A_Goto_Statement", elem.ID);
          SgGotoStatement& sgnode = SG_DEREF( sb::buildGotoStatement() );

          ctx.labelsAndLoops().gotojmp(getLabelRef(stmt.Goto_Label, ctx), sgnode);

          completeStmt(sgnode, elem, ctx);
          /* unused fields:
                Statement_ID           Corresponding_Destination_Statement;
          */
          assocstmt = &sgnode;
          break;
        }


      case An_Entry_Call_Statement:             // 9.5.3
      case A_Procedure_Call_Statement:          // 6.4
        {
          logKind(stmt.Statement_Kind == An_Entry_Call_Statement ? "An_Entry_Call_Statement" : "A_Procedure_Call_Statement", elem.ID);

          //~ if (stmt.Statement_Kind == A_Procedure_Call_Statement)
            //~ logTrace() << "A_Procedure_Call_Statement: "
                       //~ << stmt.Is_Prefix_Notation << " (Is_Prefix_Notation)"
                       //~ << std::endl;

          ElemIdRange      args   = idRange(stmt.Call_Statement_Parameters);
          // oocall indicates if code uses object-oriented syntax: x.init instead of init(x)
          const bool       oocall = (stmt.Statement_Kind == A_Procedure_Call_Statement) && stmt.Is_Prefix_Notation;
          SgExpression&    call   = createCall(stmt.Called_Name, args, true /* prefix call */, oocall, ctx);
          SgExprStatement& sgnode = SG_DEREF(sb::buildExprStatement(&call));

          attachSourceLocation(call, elem, ctx);
          completeStmt(sgnode, elem, ctx);
          /* unused fields:
              + for A_Procedure_Call_Statement / An_Entry_Call_Statement
              Declaration Corresponding_Called_Entity_Unwound

              + for A_Procedure_Call_Statement
              bool        Is_Dispatching_Call
              bool        Is_Call_On_Dispatching_Operation
              break;
          */
          assocstmt = &sgnode;
          break;
        }

      case A_Return_Statement:                  // 6.5
        {
          logKind("A_Return_Statement", elem.ID);
          Element_Struct* exprel = retrieveElemOpt(elemMap(), stmt.Return_Expression);
          SgExpression*   retval = exprel ? &getExpr(*exprel, ctx) : nullptr;
          SgReturnStmt&   sgnode = mkReturnStmt(retval);

          completeStmt(sgnode, elem, ctx);
          /* unused fields:
          */
          assocstmt = &sgnode;
          break;
        }


      case An_Accept_Statement:                 // 9.5.2
        {
          logKind("An_Accept_Statement", elem.ID);
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
                                  simpleExceptionBlockHandler,
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
          assocstmt = &sgnode;
          break;
        }

      case A_Requeue_Statement:                 // 9.5.4
      case A_Requeue_Statement_With_Abort:      // 9.5.4
        {
          const bool withAbort = stmt.Statement_Kind == A_Requeue_Statement_With_Abort;

          logKind(withAbort ? "A_Requeue_Statement_With_Abort" : "A_Requeue_Statement", elem.ID);

          SgExpression&              expr   = getExprID(stmt.Requeue_Entry_Name, ctx);
          SgProcessControlStatement& sgnode = mkRequeueStmt(expr, withAbort);

          completeStmt(sgnode, elem, ctx);

          /* unused fields:
              Pragma_Element_ID_List       Pragmas;
          */
          assocstmt = &sgnode;
          break;
        }

      case A_Delay_Until_Statement:             // 9.6
      case A_Delay_Relative_Statement:          // 9.6
        {
          logKind(stmt.Statement_Kind == A_Delay_Until_Statement ? "A_Delay_Until_Statement" : "A_Delay_Relative_Statement", elem.ID);

          SgExpression&   delayexpr = getExprID(stmt.Delay_Expression, ctx);
          SgAdaDelayStmt& sgnode    = mkAdaDelayStmt(delayexpr, stmt.Statement_Kind != A_Delay_Until_Statement);

          completeStmt(sgnode, elem, ctx);

          /* unused fields:
              Pragma_Element_ID_List       Pragmas;
          */
          assocstmt = &sgnode;
          break;
        }

      case A_Raise_Statement:                   // 11.3
        {
          logKind("A_Raise_Statement", elem.ID);
          SgExpression&   raised = getExprID_opt(stmt.Raised_Exception, ctx);
          SgExpression*   msg    = stmt.Associated_Message ? &getExprID(stmt.Associated_Message, ctx)
                                                           : nullptr;
          SgStatement&    sgnode = mkRaiseStmt(raised, msg);

          completeStmt(sgnode, elem, ctx);
          /* unused fields:
               Expression_ID          Associated_Message;
          */
          assocstmt = &sgnode;
          break;
        }
      case A_Selective_Accept_Statement:        // 9.7.1
        {
          logKind("A_Selective_Accept_Statement", elem.ID);
          ElemIdRange      range  = idRange(stmt.Statement_Paths);
          SgAdaSelectStmt& sgnode = mkAdaSelectStmt(SgAdaSelectStmt::e_selective_accept);

          completeStmt(sgnode, elem, ctx);
          traverseIDs(range, elemMap(), SelectStmtCreator{sgnode, ctx});
          assocstmt = &sgnode;
          break;
        }
      case A_Timed_Entry_Call_Statement:        // 9.7.2
        {
          logKind("A_Timed_Entry_Call_Statement", elem.ID);
          ElemIdRange  range  = idRange(stmt.Statement_Paths);
          SgAdaSelectStmt& sgnode = mkAdaSelectStmt(SgAdaSelectStmt::e_timed_entry);

          completeStmt(sgnode, elem, ctx);
          traverseIDs(range, elemMap(), SelectStmtCreator{sgnode, ctx});
          assocstmt = &sgnode;
          break;
        }
      case A_Conditional_Entry_Call_Statement:  // 9.7.3
        {
          logKind("A_Conditional_Entry_Call_Statement", elem.ID);
          ElemIdRange  range  = idRange(stmt.Statement_Paths);
          SgAdaSelectStmt& sgnode = mkAdaSelectStmt(SgAdaSelectStmt::e_conditional_entry);

          completeStmt(sgnode, elem, ctx);
          traverseIDs(range, elemMap(), SelectStmtCreator{sgnode, ctx});
          assocstmt = &sgnode;
          break;
        }
      case An_Asynchronous_Select_Statement:    // 9.7.4
        {
          logKind("An_Asynchronous_Select_Statement", elem.ID);
          ElemIdRange  range  = idRange(stmt.Statement_Paths);
          SgAdaSelectStmt& sgnode = mkAdaSelectStmt(SgAdaSelectStmt::e_asynchronous);

          completeStmt(sgnode, elem, ctx);
          traverseIDs(range, elemMap(), SelectStmtCreator{sgnode, ctx});
          assocstmt = &sgnode;
          break;
        }
      case An_Abort_Statement:                  // 9.8
        {
          logKind("An_Abort_Statement", elem.ID);

          ElemIdRange                range     = idRange(stmt.Aborted_Tasks);
          std::vector<SgExpression*> aborted   = traverseIDs(range, elemMap(), ExprSeqCreator{ctx});
          SgExprListExp&             abortList = mkExprListExp(aborted);
          SgProcessControlStatement& sgnode    = mkAbortStmt(abortList);

          ADA_ASSERT (abortList.get_parent());
          completeStmt(sgnode, elem, ctx);

          /* unused fields:
          */
          assocstmt = &sgnode;
          break;
        }
      case A_Terminate_Alternative_Statement:   // 9.7.1
        {
          logKind("A_Terminate_Alternative_Statement", elem.ID);

          SgAdaTerminateStmt& sgnode = mkTerminateStmt();

          completeStmt(sgnode, elem, ctx);
          /* unused fields:
          */
          assocstmt = &sgnode;
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

    recordPragmasID(std::move(pragmaVector), assocstmt, ctx);
  }


  /// returns NameData objects for all names associated with the declararion \ref id
  NameCreator::result_container
  queryDeclNames(Declaration_ID id, AstContext ctx)
  {
    if (id == 0) return NameCreator::result_container();

    Element_Struct& elem = retrieveElem(elemMap(), id);

    ADA_ASSERT (elem.Element_Kind == A_Declaration);
    Declaration_Struct&      asisDecl = elem.The_Union.Declaration;

    ADA_ASSERT (asisDecl.Declaration_Kind == A_Choice_Parameter_Specification);

    logKind("A_Choice_Parameter_Specification", elem.ID);
    // SgType&                   dcltype = tyModifier(getVarType(decl, ctx));
    ElemIdRange              range    = idRange(asisDecl.Names);

    return allNames(range, ctx);
  }

  void handleExceptionHandler(Element_Struct& elem, SgTryStmt& tryStmt, AstContext ctx)
  {
    using PragmaContainer = AstContext::PragmaContainer;
    using name_container  = NameCreator::result_container ;

    ADA_ASSERT (elem.Element_Kind == An_Exception_Handler);

    logKind("An_Exception_Handler", elem.ID);
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
    SgCatchOptionStmt&       sgnode  = mkExceptionHandler(SG_DEREF(lst[0]), body, tryStmt);
    ElemIdRange              range   = idRange(ex.Handler_Statements);

    sg::linkParentChild(tryStmt, as<SgStatement>(sgnode), &SgTryStmt::append_catch_statement);
    sgnode.set_trystmt(&tryStmt);
    sgnode.set_parent(tryStmt.get_catch_statement_seq_root());

    PragmaContainer pendingPragmas;
    AstContext      pragmaCtx  = ctx.pragmas(pendingPragmas);

    traverseIDs(range, elemMap(), StmtCreator{pragmaCtx.scope(body)});

    computeSourceRangeFromChildren(body);
    attachSourceLocation(sgnode, elem, pragmaCtx);
    processAndPlacePragmas(ex.Pragmas, { &body }, pragmaCtx); // pragmaCtx.scope(body) ?
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
  struct WithListCreator
  {
      explicit
      WithListCreator(AstContext astctx)
      : ctx(astctx)
      {}

      ~WithListCreator() = default;

      void operator()(Element_Struct& el)
      {
        ADA_ASSERT (el.Element_Kind == An_Expression);

        NameData                   imported = getName(el, ctx);
        Element_Struct&            impEl    = imported.elem();
        SgExpression&              sgnode   = getExpr(impEl, ctx);

        // make sure an early workaround is not used any longer
        ADA_ASSERT(isSgVarRefExp(&sgnode) == nullptr);

        // store source location of the fully qualified name
        attachSourceLocation(sgnode, el, ctx);
        importedUnits.push_back(&sgnode);
      }

      operator SgExpressionPtrList () &&
      {
        return std::move(importedUnits);
      }


    private:
      SgExpressionPtrList importedUnits;
      AstContext          ctx;

      WithListCreator() = delete;
  };


  /// Functor to create a use clause.
  struct UseClauseCreator
  {
      UseClauseCreator(map_t<int, SgDeclarationStatement*>& mapping, AstContext astctx)
      : m(mapping), ctx(astctx)
      {}

      void operator()(Element_Struct& el)
      {
        ADA_ASSERT (el.Element_Kind == An_Expression);

        NameData                usedEl = getName(el, ctx); // either unit or type
        AdaIdentifier           fullName{usedEl.fullName};
        Expression_Struct&      expr   = asisExpression(usedEl.elem());
        SgDeclarationStatement* used   = findFirst(m, expr.Corresponding_Name_Definition, expr.Corresponding_Name_Declaration);

        // fallback code for packages that are not properly linked in Asis
        //   i.e., generic packages withed in a formal part
        if (!used)
        {
          // try to look up the symbol from the context by name
          const SgSymbol*       sym = nullptr;

          std::tie(std::ignore, sym) = si::Ada::findSymbolInContext(fullName, ctx.scope());

          // do we need to check the symbol type?
          if (sym) used = si::Ada::associatedDeclaration(*sym);
        }

        // if a package is not available otherwise, maybe it is part of the Ada standard?
        if (!used)
        {
          logInfo() << "using unknown package/type: "
                    << usedEl.fullName << " : "
                    << expr.Corresponding_Name_Definition << " / " << expr.Corresponding_Name_Declaration
                    << "\n   in scope type: " << typeid(ctx.scope()).name()
                    << std::endl;

          used = findFirst(adaPkgs(), fullName);
          if (!used)
          {
            if (SgNamedType* ty = isSgNamedType(findFirst(adaTypes(), fullName)))
              used = ty->get_declaration();
          }
        }

        SgUsingDeclarationStatement& sgnode  = mkUseClause(SG_DEREF(used));
        std::size_t                  attrPos = fullName.find("'");

        if (attrPos != std::string::npos)
        {
          ADA_ASSERT(fullName.size() > attrPos);
          // \todo introduce proper flag
          sgnode.set_adaTypeAttribute(fullName.substr(attrPos+1));
        }


        //~ std::cerr
        //~ logError() << "use decl: " << usedEl.fullName
                   //~ << " " << typeid(*used).name()
                   //~ << " (" << expr.Corresponding_Name_Definition
                   //~ << ", " << expr.Corresponding_Name_Declaration << ")"
                   //~ << std::endl;

        recordNode(asisDecls(), el.ID, sgnode);
        attachSourceLocation(sgnode, el, ctx);
        ctx.appendStatement(sgnode);
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

        logKind("A_Component_Clause", el.ID);

        // \todo use getQualName?
        SgExpression&         field  = getExprID(clause.Representation_Clause_Name, ctx);
        SgVarRefExp&          fldref = SG_DEREF(isSgVarRefExp(&field));
        SgExpression&         ofs    = getExprID(clause.Component_Clause_Position, ctx);
        SgExpression&         rngexp = getDiscreteRangeID(clause.Component_Clause_Range, ctx);
        SgRangeExp*           range  = isSgRangeExp(&rngexp);
        SgAdaComponentClause& sgnode = mkAdaComponentClause(fldref, ofs, SG_DEREF(range));

        //~ recordNode(asisDecls(), el.ID, sgnode);
        attachSourceLocation(sgnode, el, ctx);

        ctx.appendStatement(sgnode);
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

      SgAssignOp& itemValuePair(Element_Struct& /*el*/, Element_ID item, SgExpression& enumval)
      {
        SgExpression& enumitem = getExprID(item, ctx);

        return SG_DEREF(sb::buildAssignOp(&enumitem, &enumval));
      }

      void operator()(Element_Struct& el)
      {
        ADA_ASSERT (el.Element_Kind == An_Association);
        logKind("An_Association", el.ID);

        Association_Struct&        assoc = el.The_Union.Association;
        ADA_ASSERT (assoc.Association_Kind == An_Array_Component_Association);
        logKind("An_Array_Component_Association", el.ID);

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

  struct DbgRecursionError : std::logic_error
  {
    using base = std::logic_error;
    using base::base;
  };

  bool alwaysFalse(AstContext) { return false; } // \todo consider making this assertFalse

  bool queryIfDerivedFromEnumID(Element_ID id, AstContext ctx, std::function<bool(AstContext)> fallback = alwaysFalse);

  bool queryIfDerivedFromEnum(Element_Struct& elem, AstContext ctx)
  {
    if (elem.Element_Kind == A_Definition)
    {
      logKind("qid: A_Definition", elem.ID);

      Definition_Struct&         def             = elem.The_Union.Definition;

      if (def.Definition_Kind == A_Subtype_Indication)
      {
        Subtype_Indication_Struct& subtype = def.The_Union.The_Subtype_Indication;

        return queryIfDerivedFromEnumID(subtype.Subtype_Mark, ctx);
      }

      if (def.Definition_Kind == A_Formal_Type_Definition)
      {
        // \todo could this require enum handling?
        logInfo() << "qid: definition-kind: A_Formal_Type_Definition"
                  << std::endl;
        return false;
      }

      if (def.Definition_Kind != A_Type_Definition)
      {
        logWarn() << "qid: unexpected definition-kind: " << def.Definition_Kind
                  << std::endl;
        return false;
      }

      Type_Definition_Struct& tydef = def.The_Union.The_Type_Definition;

      return (  (tydef.Type_Kind == An_Enumeration_Type_Definition)
             || (  (tydef.Type_Kind == A_Derived_Type_Definition)
                && queryIfDerivedFromEnumID(tydef.Parent_Subtype_Indication, ctx)
                )
             );
    }

    if (elem.Element_Kind == An_Expression)
    {
      logKind("qid: An_Expression", elem.ID);
      Expression_Struct&         expr = elem.The_Union.Expression;

      if (expr.Expression_Kind == A_Selected_Component)
      {
        // \todo consider checking for Standard prefix..
        return queryIfDerivedFromEnumID(expr.Selector, ctx);
      }

      return (  (expr.Expression_Kind == An_Identifier)
             && queryIfDerivedFromEnumID( expr.Corresponding_Name_Declaration,
                                          ctx,
                                          [&expr](AstContext) -> bool
                                          {
                                            // check for all enums in Standard
                                            return AdaIdentifier{expr.Name_Image} == "BOOLEAN";
                                          }
                                        )
             );
    }

    if (elem.Element_Kind == A_Declaration)
    {
      logKind("qid: A_Declaration", elem.ID);
      Declaration_Struct&         decl = elem.The_Union.Declaration;

      if (  (decl.Declaration_Kind == A_Task_Type_Declaration)
         || (decl.Declaration_Kind == A_Protected_Type_Declaration)
         || (decl.Declaration_Kind == A_Private_Extension_Declaration)
         )
        return false;

      if (  (decl.Declaration_Kind == A_Private_Type_Declaration)
         || (decl.Declaration_Kind == An_Incomplete_Type_Declaration)
         )
        return queryIfDerivedFromEnumID(decl.Corresponding_Type_Completion, ctx);

      ADA_ASSERT(  (decl.Declaration_Kind == A_Subtype_Declaration)
                || (decl.Declaration_Kind == An_Ordinary_Type_Declaration)
                || (decl.Declaration_Kind == A_Formal_Type_Declaration)
                );

      return queryIfDerivedFromEnumID(decl.Type_Declaration_View, ctx);
    }

    logError() << "unhandled element kind [queryIfDerivedFromEnum]: " << elem.Element_Kind
               << std::endl;
    return false;
  }

  bool queryIfDerivedFromEnumID(Element_ID id, AstContext ctx, std::function<bool(AstContext)> fallback)
  {
    Element_Struct* el = retrieveElemOpt(elemMap(), id);

    return el ? queryIfDerivedFromEnum(*el, ctx) : fallback(ctx);
  }

  DefinitionDetails
  queryDefinitionDetails(Element_Struct& complElem, AstContext ctx);

  Type_Kinds
  queryBaseDefinitionData(Definition_Struct& typeDefn, Type_Kinds tyKind, Element_ID /*currID*/, AstContext ctx)
  {
    ADA_ASSERT (tyKind == A_Derived_Type_Definition);

    if (queryIfDerivedFromEnumID(typeDefn.The_Union.The_Type_Definition.Parent_Subtype_Indication, ctx))
    {
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
      logKind("A_Task_Type_Declaration", complElem.ID);

      return DefinitionDetails{declname.id(), A_Task_Type_Declaration, Not_A_Type_Definition};
    }

    if (complDecl.Declaration_Kind == A_Protected_Type_Declaration)
    {
      logKind("A_Protected_Type_Declaration", complElem.ID);

      return DefinitionDetails{declname.id(), A_Protected_Type_Declaration, Not_A_Type_Definition};
    }

    if (complDecl.Declaration_Kind == An_Ordinary_Type_Declaration)
      logKind("An_Ordinary_Type_Declaration", complElem.ID);
    else if (complDecl.Declaration_Kind == A_Formal_Type_Declaration)
      logKind("A_Formal_Type_Declaration", complElem.ID);
    else if (complDecl.Declaration_Kind == A_Subtype_Declaration)
      logKind("A_Subtype_Declaration", complElem.ID);
    else
    {
      logFlaw() << "unexpected decl kind [queryDefinitionData]: " << complDecl.Declaration_Kind
                << std::endl;
    }

    Element_Struct&     typeElem = retrieveElem(elemMap(), complDecl.Type_Declaration_View);
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
          resKind  = queryBaseDefinitionData(typeDefn, resKind, complElem.ID, ctx);

        break;
      }

      case A_Formal_Type_Definition:
        break;

      default:
        logFatal() << "unexpected def kind [queryDefinitionData]: " << typeDefn.Definition_Kind
                   << std::endl;
        ADA_ASSERT(false);
    }

    return DefinitionDetails{declname.id(), complDecl.Declaration_Kind, resKind};
  }

  // find the element and type kind of the corresponding complete declaration
  DefinitionDetails
  queryDefinitionDetailsID(Element_ID completeElementId, AstContext ctx)
  {
    Element_Struct& complElem = retrieveElem(elemMap(), completeElementId);

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
      InheritedSymbolCreator(SgNamedType& sourceRootType, SgNamedType& dervType, AstContext astctx)
      : baseType(sourceRootType), dervivedType(dervType), ctx(astctx)
      {}

      void operator()(Element_Struct& elem)
      {
        SgDeclarationStatement*       fndcl  = findFirst(asisDecls(), elem.ID);
        SgFunctionDeclaration*        fn     = isSgFunctionDeclaration(fndcl);

        if (fn == nullptr)
        {
          logFlaw() << "unable to find function with Asis element ID " << elem.ID << std::endl;
          return;
        }

        SgFunctionSymbol*             fnsym  = findFirst(inheritedSymbols(), std::make_pair(fn, &baseType));
        if (fnsym == nullptr) fnsym = isSgFunctionSymbol(fn->search_for_symbol_from_symbol_table());

        if (fnsym == nullptr)
        {
          logFlaw() << "unable to find (derived) function symbol for " << fn->get_name() << std::endl;
          return;
        }

        SgDeclarationStatement const* symdcl = fnsym->get_declaration();
        if (symdcl && (symdcl->get_firstNondefiningDeclaration() != fn->get_firstNondefiningDeclaration()))
        {
          logFlaw() << fn->get_name() << " sym/dcl mismatch"
                    << std::endl;
        }


        SgAdaInheritedFunctionSymbol& sgnode = mkAdaInheritedFunctionSymbol(*fnsym, dervivedType, ctx.scope());
        const auto inserted = inheritedSymbols().insert(std::make_pair(InheritedSymbolKey{fn, &dervivedType}, &sgnode));

        ROSE_ASSERT(inserted.second);
    }

    private:
      SgNamedType& baseType;
      SgNamedType& dervivedType;
      AstContext   ctx;
  };

  struct InheritedEnumeratorCreator
  {
      InheritedEnumeratorCreator(SgEnumDeclaration& enumDcl, SgEnumDeclaration& orig, AstContext astctx)
      : derivedDcl(enumDcl), ctx(astctx)
      {}

      // assuming that the inherited enumerators appear in the same order
      void operator()(Element_ID id)
      {
        // PP (01/25/22) new code: the old code tried to link the enumerators to their original definition
        // by setting the initializer to the old value. However, derived enumerators can have
        // different representation values, and to support JACCEL-265 and the translation to C++
        // in general, the link to the original declaration is no longer maintained.
        // Instead, the initializer now links to the representation value. The relationship to the
        // inherited values is now implied.
        // \todo The code is most similar to the normal EnumeratorCreator in AdaType.C and
        //       could be refactored to eliminate code duplication.
        SgType&             enumTy = SG_DEREF(derivedDcl.get_type());
        Element_Struct&     elem = retrieveElem(elemMap(), id);
        ADA_ASSERT (elem.Element_Kind == A_Declaration);
        logKind("A_Declaration", id);

        Declaration_Struct& decl = elem.The_Union.Declaration;
        ADA_ASSERT (decl.Declaration_Kind == An_Enumeration_Literal_Specification);
        logKind("An_Enumeration_Literal_Specification (inherited)", id);

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
      }

    private:
      SgEnumDeclaration&                             derivedDcl;
      AstContext                                     ctx;
  };

  // following proc is public and located at the bottom of the file
  //   void
  //   processInheritedSubroutines( SgNamedType& derivedType,
  //                                ElemIdRange subprograms,
  //                                ElemIdRange declarations,
  //                                AstContext ctx
  //                              )

  template <class AsisTypeDefinitionStruct>
  void
  processInheritedSubroutines( SgNamedType* derivedTy,
                               AsisTypeDefinitionStruct& tydef,
                               AstContext ctx
                             )
  {
    processInheritedSubroutines( SG_DEREF(derivedTy),
                                 idRange(tydef.Implicit_Inherited_Subprograms),
                                 idRange(tydef.Implicit_Inherited_Declarations),
                                 ctx
                               );
  }

  template <class SageTypeDeclStmt>
  void
  processInheritedSubroutines( Type_Definition_Struct& tydef,
                               SageTypeDeclStmt& tyDecl,
                               AstContext ctx
                             )
  {
    processInheritedSubroutines(tyDecl.get_type(), tydef, ctx);
  }


  std::tuple<SgEnumDeclaration*, SgAdaRangeConstraint*>
  getBaseEnum(SgType* baseTy)
  {
    ADA_ASSERT(baseTy);

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

      basedecl = si::Ada::baseEnumDeclaration(ty);
    }

    if (basedecl == nullptr)
    {
      logFlaw() << "basedecl == nullptr: " << typeid(*baseTy).name() << std::endl;
      ASSERT_not_null(basedecl);
    }

    if (SgEnumDeclaration* realdecl = isSgEnumDeclaration(basedecl->get_definingDeclaration()))
      basedecl = realdecl;

    return { basedecl, constraint };
  }

  void
  processInheritedEnumValues( Type_Definition_Struct& tydef,
                              SgEnumDeclaration& derivedTypeDcl,
                              AstContext ctx
                            )
  {
    {
      ElemIdRange  range        = idRange(tydef.Implicit_Inherited_Subprograms);
      SgEnumType&  derivedType  = SG_DEREF(derivedTypeDcl.get_type());
      SgType*      baseType     = si::Ada::baseType(derivedType);
      SgNamedType* baseRootType = isSgNamedType(si::Ada::typeRoot(baseType).typerep());

      if (baseRootType == nullptr)
      {
        logFlaw() << "unable to find base-root for enum " << derivedType.get_name()
                  << " / base = " << baseType
                  << std::endl;
        return;
      }

      traverseIDs(range, elemMap(), InheritedSymbolCreator{*baseRootType, derivedType, ctx});
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

    if (  (tydef.Type_Kind != A_Derived_Type_Definition)
       && (tydef.Type_Kind != A_Derived_Record_Extension_Definition)
       )
    {
      return;
    }

    if (SgTypedefDeclaration* derivedTypeDcl = isSgTypedefDeclaration(&dcl))
      processInheritedSubroutines(tydef, *derivedTypeDcl, ctx);
    else if (SgClassDeclaration* classTypeDcl = isSgClassDeclaration(&dcl))
      processInheritedSubroutines(tydef, *classTypeDcl, ctx);
    else if (SgEnumDeclaration* derivedEnumDcl = isSgEnumDeclaration(&dcl))
      processInheritedEnumValues(tydef, *derivedEnumDcl, ctx);
    else if (SgAdaDiscriminatedTypeDecl* discrTypeDcl = isSgAdaDiscriminatedTypeDecl(&dcl))
      processInheritedSubroutines(tydef, *discrTypeDcl, ctx);
    else
      ADA_ASSERT(false);
  }


  SgDeclarationStatement&
  createOpaqueDecl(NameData adaname, Declaration_Struct& decl, const DefinitionDetails detail, AstContext ctx)
  {
    SgScopeStatement&       scope = ctx.scope();
    Element_Struct*         typeview = nullptr;
    SgDeclarationStatement* res = nullptr;

    if (!isInvalidId(decl.Type_Declaration_View))
    {
      typeview = &retrieveElem(elemMap(), decl.Type_Declaration_View);
      ADA_ASSERT (typeview->Element_Kind == A_Definition);
    }

    if (detail.declKind() == A_Task_Type_Declaration)
      res = &mkAdaTaskTypeDecl(adaname.ident, nullptr /* no spec */, scope);
    else if (detail.declKind() == A_Protected_Type_Declaration)
      res = &mkAdaProtectedTypeDecl(adaname.ident, nullptr /* no spec */, scope);
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
            res = &mkEnumDecl(adaname.ident, scope);
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
    // PP (20/1/23): why do we use the nondefining function's scope?
    //               for one, ROSE scope fixup currently unifies the scopes.
    //               see also SCOPE_COMMENT_1 .
    return nondef ? mkProcedureDecl(*nondef, SG_DEREF(nondef->get_scope()), rettype, std::move(complete))
                  : mkProcedureDecl(name,    scope, rettype, std::move(complete));
  }


  SgFunctionDeclaration&
  createFunDcl( SgFunctionDeclaration* nondef,
                const std::string& name,
                SgScopeStatement& scope,
                SgType& rettype,
                std::function<void(SgFunctionParameterList&, SgScopeStatement&)> complete
              )
  {
    // we use the nondefining function's scope if available
    // see SCOPE_COMMENT_1
    return nondef ? mkProcedureDecl_nondef(*nondef, SG_DEREF(nondef->get_scope()), rettype, std::move(complete))
                  : mkProcedureDecl_nondef(name,    scope, rettype, std::move(complete));
  }

  void completeDiscriminatedDecl( Element_ID id,
                                  Element_Struct& elem,
                                  SgAdaDiscriminatedTypeDecl& sgnode,
                                  SgDeclarationStatement& sgdecl,
                                  bool isPrivate,
                                  AstContext ctx
                                )
  {
    sg::linkParentChild(sgnode, sgdecl, &SgAdaDiscriminatedTypeDecl::set_discriminatedDecl);
    // sgdecl.set_scope(sgnode.get_discriminantScope());

    privatize(sgnode, isPrivate);
    recordNode(asisTypes(), id, sgdecl, true /* replace */);
    attachSourceLocation(sgnode, elem, ctx);

    // \todo link nondef and def SgAdaDiscriminatedTypeDecl

    ADA_ASSERT (sgnode.get_parent() == &ctx.scope());
  }

  ElemIdRange
  secondaryKnownDiscrimnants(Element_Struct* elem, AstContext)
  {
    if (elem == nullptr) return { nullptr, nullptr };

    ADA_ASSERT (elem->Element_Kind == A_Definition);
    Definition_Struct& def = elem->The_Union.Definition;

    if (def.Definition_Kind != A_Known_Discriminant_Part) return { nullptr, nullptr };

    return idRange(def.The_Union.The_Known_Discriminant_Part.Discriminants);
  }

#if 0
  std::vector<SgInitializedName*>
  flattenDiscriminants(SgAdaDiscriminatedTypeDecl& sgnode, AstContext)
  {
    std::vector<SgInitializedName*>      res;
    const SgAdaParameterList&            lst = SG_DEREF(sgnode.get_discriminants());
    const SgDeclarationStatementPtrList& parms = lst.get_parameters();

    std::for_each( parms.begin(), parms.end(),
                   [&res](SgDeclarationStatement* dcl)->void
                   {
                     if (SgVariableDeclaration* var = isSgVariableDeclaration(dcl))
                     {
                       res.insert(res.end(), var->get_variables().begin(), var->get_variables().end());
                     }
                   }
                 );

    return res;
  }
#endif /* 0 */

  std::vector<Element_ID>
  flattenNameLists(ElemIdRange idrange, AstContext ctx)
  {
    std::vector<Element_ID> res;

    traverseIDs( idrange, elemMap(),
                 [&res, ctx](Element_Struct& elem)->void
                 {
                   ADA_ASSERT (elem.Element_Kind == A_Declaration);

                   Declaration_Struct& asisDecl = elem.The_Union.Declaration;
                   ADA_ASSERT (asisDecl.Declaration_Kind == A_Discriminant_Specification);

                   ElemIdRange         range    = idRange(asisDecl.Names);

                   for (const NameData& obj : allNames(range, ctx))
                   {
                     res.push_back(obj.id());
                   }
                 }
               );

    return res;
  }

  template <class Container>
  Container reverseElems(Container cont)
  {
    std::reverse(cont.begin(), cont.end());
    return cont;
  }


  SgAdaDiscriminatedTypeDecl&
  createDiscriminatedDeclID(Element_Struct& elem, Element_Struct* secondary, AstContext ctx)
  {
    ADA_ASSERT (elem.Element_Kind == A_Definition);
    logKind("A_Definition", elem.ID);

    // many definitions are handled else where
    // here we want to convert the rest that can appear in declarative context

    SgScopeStatement&           scope  = ctx.scope();
    SgAdaDiscriminatedTypeDecl& sgnode = mkAdaDiscriminatedTypeDecl(scope);
    Definition_Struct&          def    = elem.The_Union.Definition;

    ctx.appendStatement(sgnode);

    if (def.Definition_Kind == A_Known_Discriminant_Part)
    {
      logKind("A_Known_Discriminant_Part", elem.ID);

      SgScopeStatement&       scope         = SG_DEREF(sgnode.get_discriminantScope());
      ElemIdRange             discriminants = idRange(def.The_Union.The_Known_Discriminant_Part.Discriminants);
      ElemIdRange             secondaryIDs  = secondaryKnownDiscrimnants(secondary, ctx);
      std::vector<Element_ID> secondaries   = reverseElems(flattenNameLists(secondaryIDs, ctx));

      traverseIDs(discriminants, elemMap(), DiscriminantCreator{sgnode, std::move(secondaries), ctx.scope(scope)});
    }
    else
    {
      ADA_ASSERT (def.Definition_Kind == An_Unknown_Discriminant_Part);

      logKind("An_Unknown_Discriminant_Part", elem.ID);
    }

    return sgnode;
  }


  // handles incomplete (but completed) and private types
  SgDeclarationStatement&
  handleOpaqueTypes( Element_Struct& elem,
                     Declaration_Struct& decl,
                     bool isPrivate,
                     AstContext ctx
                   )
  {
    logTrace() << "\n  abstract: " << decl.Has_Abstract
               << "\n  limited: " << decl.Has_Limited
               << "\n  private: " << decl.Has_Private
               << std::endl;

    NameData                adaname = singleName(decl, ctx);
    ADA_ASSERT (adaname.fullName == adaname.ident);

#if DEBUG_RECURSION
    try // for debug purposes
    {
#endif /* DEBUG_RECURSION */
      DefinitionDetails       defdata = queryDeclarationDetails(decl, ctx);
      SgScopeStatement*       parentScope = &ctx.scope();
      SgAdaDiscriminatedTypeDecl* discr = createDiscriminatedDeclID_opt(decl.Discriminant_Part, 0, ctx);

      if (discr)
      {
        parentScope = discr->get_discriminantScope();
      }

      SgScopeStatement&       scope  = SG_DEREF(parentScope);
      Element_ID              id     = adaname.id();
      SgDeclarationStatement& sgdecl = createOpaqueDecl(adaname, decl, defdata, ctx.scope(scope));
      SgDeclarationStatement* assocdecl = nullptr;

      attachSourceLocation(sgdecl, elem, ctx);
      privatize(sgdecl, isPrivate);
      recordNode(asisTypes(), id, sgdecl);
      recordNode(asisTypes(), defdata.id(), sgdecl); // rec @ def

      if (!discr)
      {
        ADA_ASSERT(&ctx.scope() == parentScope);
        ctx.appendStatement(sgdecl);
        assocdecl = &sgdecl;
      }
      else
      {
        completeDiscriminatedDecl(id, elem, *discr, sgdecl, isPrivate, ctx);
        assocdecl = discr;
      }

      if (decl.Declaration_Kind == A_Private_Extension_Declaration)
      {
        if (Element_Struct* tyview = retrieveElemOpt(elemMap(), decl.Type_Declaration_View))
        {
          ADA_ASSERT(tyview->Element_Kind == A_Definition);
          Definition_Struct& tydef = tyview->The_Union.Definition;

          if (tydef.Definition_Kind == A_Private_Extension_Definition)
          {
            SgDeclarationStatement* tydcl = discr ? discr : &sgdecl;

            processInheritedSubroutines( SG_DEREF(si::getDeclaredType(tydcl)),
                                         idRange(tydef.The_Union.The_Private_Extension_Definition.Implicit_Inherited_Subprograms),
                                         idRange(tydef.The_Union.The_Private_Extension_Definition.Implicit_Inherited_Declarations),
                                         ctx
                                       );
          }
        }
      }
#if DEBUG_RECURSION
    }
    catch (const DbgRecursionError& n)
    {
      logFlaw() << "root of recursion error: " << adaname.fullName
                << "\n   declared at: " << elem.Source_Location.Unit_Name
                << ":" << elem.Source_Location.First_Line
                << ":" << elem.Source_Location.First_Column
                << std::endl;
      throw;
    }
#endif /* DEBUG_RECURSION */

    return SG_DEREF(assocdecl);
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

  // retrieves package spec declarations, similar to getFunctionDeclaration
  SgAdaPackageSpecDecl* getAdaPackageSpecDecl(SgDeclarationStatement* dcl)
  {
    if (SgAdaPackageSpecDecl* spcdcl  = isSgAdaPackageSpecDecl(dcl))
      return spcdcl;

    if (SgAdaGenericDecl* generic = isSgAdaGenericDecl(dcl))
      return isSgAdaPackageSpecDecl(generic->get_declaration());

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
        Element_Struct* elem = retrieveElemOpt(elemMap(), id);

        if (elem)
        {
          ADA_ASSERT (elem->Element_Kind == A_Declaration);
          logKind("A_Declaration", elem->ID);

          Declaration_Struct& decl = elem->The_Union.Declaration;

          ADA_ASSERT (decl.Declaration_Kind == An_Entry_Index_Specification);
          logKind("An_Entry_Index_Specification", elem->ID);

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

  bool
  definedByRenamingID(Element_ID id, AstContext /* unused */)
  {
    if (id == 0)
      return false;

    Element_Struct&     elem = retrieveElem(elemMap(), id);

    if (elem.Element_Kind != A_Declaration)
      return false;

    Declaration_Struct& decl = elem.The_Union.Declaration;

    return (  decl.Declaration_Kind == A_Procedure_Renaming_Declaration
           || decl.Declaration_Kind == A_Function_Renaming_Declaration
           );
  }
} // anonymous

SgAdaDiscriminatedTypeDecl*
createDiscriminatedDeclID_opt(Element_ID primary, Element_ID secondary, AstContext ctx)
{
  if (primary == 0)
  {
    if (secondary != 0) logFlaw() << "Unexpected secondary discriminants" << std::endl;

    return nullptr;
  }

  return &createDiscriminatedDeclID(retrieveElem(elemMap(), primary), retrieveElemOpt(elemMap(), secondary), ctx);
}

SgScopeStatement&
queryScopeOfID(Element_ID el, AstContext ctx)
{
  return queryScopeOf(retrieveElem(elemMap(), el), ctx);
}

SgDeclarationStatement*
queryDecl(Expression_Struct& expr, AstContext /*ctx*/)
{
  SgDeclarationStatement* res = findFirst(asisDecls(), expr.Corresponding_Name_Definition, expr.Corresponding_Name_Declaration);

  if ((res == nullptr) && (expr.Expression_Kind == An_Identifier))
  {
    res = findFirst(adaPkgs(), AdaIdentifier{expr.Name_Image});
  }

  return res;
}

void handleRepresentationClause(Element_Struct& elem, AstContext ctx)
{
  using PragmaContainer = AstContext::PragmaContainer;

  Clause_Struct&                clause = elem.The_Union.Clause;
  ADA_ASSERT (clause.Clause_Kind == A_Representation_Clause);

  Representation_Clause_Struct& repclause = clause.Representation_Clause;

  switch (repclause.Representation_Clause_Kind)
  {
    case A_Record_Representation_Clause:           // 13.5.1
      {
        using SageRecordClause = SgAdaRepresentationClause;

        logKind("A_Record_Representation_Clause", elem.ID);

        SgType&                 tyrec      = getDeclTypeID(repclause.Representation_Clause_Name, ctx);
        SgExpression&           modexp     = getExprID_opt(repclause.Mod_Clause_Expression, ctx);
        SageRecordClause&       sgnode     = mkAdaRepresentationClause(tyrec, modexp);
        SgBasicBlock&           components = SG_DEREF(sgnode.get_components());
        ElemIdRange             range      = idRange(repclause.Component_Clauses);


        // sgnode is not a decl: recordNode(asisDecls(), el.ID, sgnode);
        attachSourceLocation(sgnode, elem, ctx);
        ctx.appendStatement(sgnode);

        PragmaContainer pendingPragmas;
        AstContext      pragmaCtx  = ctx.pragmas(pendingPragmas);

        traverseIDs(range, elemMap(), ComponentClauseCreator{pragmaCtx.scope(components)});

        processAndPlacePragmas(repclause.Pragmas, { &components }, pragmaCtx.scope(components));
        /* unhandled fields:
         */
        break;
      }

    case An_At_Clause:                             // J.7
      {
        using SageRecordClause = SgAdaRepresentationClause;

        logKind("An_At_Clause", elem.ID);

        // \todo Representation_Clause_Name may not refer to a type but to a variable
        //       (e.g.,rep_sys_address.adb)
        //       consider using expressions as base for AdaRepresentationClause...
        SgType&           ty     = getDeclTypeID(repclause.Representation_Clause_Name, ctx);
        SgExpression&     modexp = getExprID_opt(repclause.Representation_Clause_Expression, ctx);
        SageRecordClause& sgnode = mkAdaRepresentationClause(ty, modexp, true /* at-clause */);

        attachSourceLocation(sgnode, elem, ctx);
        ctx.appendStatement(sgnode);
        /* unhandled fields:
         */
        break;
      }

    case An_Attribute_Definition_Clause:           // 13.3
      {
        logKind("An_Attribute_Definition_Clause", elem.ID);
        SgAdaAttributeExp&    lenattr = getAttributeExprID(repclause.Representation_Clause_Name, ctx);
        SgExpression&         lenexpr = getExprID(repclause.Representation_Clause_Expression, ctx);
        SgAdaAttributeClause& sgnode  = mkAdaAttributeClause(lenattr, lenexpr);

        attachSourceLocation(sgnode, elem, ctx);
        ctx.appendStatement(sgnode);
        /* unhandled fields:
         */
        break;
      }

    case An_Enumeration_Representation_Clause:     // 13.4
      {
        logKind("An_Enumeration_Representation_Clause", elem.ID);
        SgType&                 enumty   = getDeclTypeID(repclause.Representation_Clause_Name, ctx);
        Element_Struct&         inielem  = retrieveElem(elemMap(), repclause.Representation_Clause_Expression);
        ADA_ASSERT (inielem.Element_Kind == An_Expression);

        Expression_Struct&      inilist  = inielem.The_Union.Expression;

        ADA_ASSERT(  inilist.Expression_Kind == A_Named_Array_Aggregate
                  || inilist.Expression_Kind == A_Positional_Array_Aggregate
                  );

        ElemIdRange             range    = idRange(inilist.Array_Component_Associations);
        SgExprListExp&          enumvals = traverseIDs(range, elemMap(), EnumValueCreator{ctx});
        SgAdaEnumRepresentationClause& sgnode = mkAdaEnumRepresentationClause(enumty, enumvals);

        attachSourceLocation(sgnode, elem, ctx);
        ctx.appendStatement(sgnode);
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
        logKind("A_With_Clause", elem.ID);
        ElemIdRange         range  = idRange(clause.Clause_Names);
        SgExpressionPtrList implst = traverseIDs(range, elemMap(), WithListCreator{ctx});
        SgImportStatement&  sgnode = mkWithClause(std::move(implst));
        SgScopeStatement&   scope  = ctx.scope();

        attachSourceLocation(sgnode, elem, ctx);
        recordNode(asisDecls(), elem.ID, sgnode);
        ctx.appendStatement(sgnode);
        ADA_ASSERT (sgnode.get_parent() == &scope);

        // recordNonUniqueNode(asisDecls(), impID, sgnode);

        /* unused fields:
            bool   Has_Limited
        */
        break;
      }

    case A_Use_Type_Clause:            // 8.4
    case A_Use_Package_Clause:         // 8.4
      {
        const bool typeClause = (clause.Clause_Kind == A_Use_Type_Clause);

        logKind(typeClause ? "A_Use_Type_Clause" : "A_Use_Package_Clause", elem.ID);

        ElemIdRange        range  = idRange(clause.Clause_Names);
        traverseIDs(range, elemMap(), UseClauseCreator{(typeClause ? asisTypes() : asisDecls()), ctx});
        /* unused fields:
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
      ADA_ASSERT(false);
      break;

    case A_Use_All_Type_Clause:        // 8.4: Ada 2012
    case Not_A_Clause: /* break; */    // An unexpected element
    default:
      logWarn() << "unhandled clause kind: " << clause.Clause_Kind << std::endl;
      ADA_ASSERT (!FAIL_ON_ERROR(ctx));
  }
}

void handleVariant(Element_Struct& /*elem*/, Variant_Part_Struct& variant, AstContext ctx)
{
  ElemIdRange range = idRange(variant.Variants);

  SgExpression&       discrExpr = getExprID(variant.Discriminant_Direct_Name, ctx);
  SgAdaVariantDecl&   sgnode    = mkAdaVariantDecl(discrExpr);
  SgAdaUnscopedBlock& blk       = SG_DEREF(sgnode.get_variants());

  ctx.appendStatement(sgnode);
  traverseIDs(range, elemMap(), VariantCreator{ctx.unscopedBlock(blk)});

  /* unused fields:
  */
}

void handleDefinition(Element_Struct& elem, AstContext ctx)
{
  ADA_ASSERT (elem.Element_Kind == A_Definition);
  logKind("A_Definition", elem.ID);

  // many definitions are handled else where
  // here we want to convert the rest that can appear in declarative context

  Definition_Struct& def = elem.The_Union.Definition;

  switch (def.Definition_Kind)
  {
    case A_Null_Component:                 // 3.8(4)
      {
        SgDeclarationStatement& sgnode = mkNullDecl();

        attachSourceLocation(sgnode, elem, ctx);
        ctx.appendStatement(sgnode);
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
      ADA_ASSERT(false);
      break;

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


namespace
{
  Declaration_Struct& firstDeclaration(Declaration_Struct& dcl)
  {
    // PP (11/14/22): RC-1418 (Asis only?)
    // Since Corresponding_Declaration is not set on routines with body stubs,
    // the converter reads through Corresponding_Body_Stub to find the
    // relevant declaration.

    if (dcl.Corresponding_Declaration > 0)
    {
      if (Element_Struct* res = retrieveElemOpt(elemMap(), dcl.Corresponding_Declaration))
      {
        ADA_ASSERT (res->Element_Kind == A_Declaration);
        return res->The_Union.Declaration;
      }
    }

    if (dcl.Corresponding_Body_Stub > 0)
    {
      if (Element_Struct* stub = retrieveElemOpt(elemMap(), dcl.Corresponding_Body_Stub))
      {
        ADA_ASSERT(stub && (stub->Element_Kind == A_Declaration));
        return firstDeclaration(stub->The_Union.Declaration);
      }
    }

    return dcl;
  }


  Parameter_Specification_List
  usableParameterProfile(Declaration_Struct& decl, AstContext)
  {
    // PP (7/29/22): RC-1372 (Asis only?)
    // In the Asis rep. parameter references inside of routine bodies
    //   refer back to the parameter list of the first declaration.
    //   Not sure if this is by design or a bug (\todo ask CR or PL).
    //   As work around, use the first decl's parameter list to
    //   generate the body's parameter list. Unlike C++, Ada requires
    //   parameter names AND default initializers of the declaration and
    //   definition to match.
    //   One caveat is that multiple initialized names could have a combined
    //   VariableDeclaration in the declaration, while having separate
    //   VariableDeclarations in the definition (and vice versa).

    // if the @decl is not a body, return the original parameter list
    if (  (decl.Declaration_Kind != A_Procedure_Body_Declaration)
       && (decl.Declaration_Kind != A_Function_Body_Declaration)
       && (decl.Declaration_Kind != An_Entry_Body_Declaration)
       )
      return decl.Parameter_Profile;

    return firstDeclaration(decl).Parameter_Profile;
  }

  Element_ID
  secondaryDiscriminants(Element_ID id, Declaration_Struct& decl, AstContext)
  {
    const bool useThisDecl = (  (decl.Corresponding_Type_Declaration == id)
                             || (decl.Corresponding_Type_Declaration == 0)
                             || (decl.Discriminant_Part == 0)
                             );

    if (useThisDecl) return 0;

    Element_Struct& firstDecl = retrieveElem(elemMap(), decl.Corresponding_Type_Declaration);
    ADA_ASSERT(firstDecl.Element_Kind == A_Declaration);

    return firstDecl.The_Union.Declaration.Discriminant_Part;
  }

  SgExpression&
  getDefaultFunctionExpr(Declaration_Struct& decl, SgAdaSubroutineType& ty, AstContext ctx)
  {
    SgExpression* res = nullptr;

    switch (decl.Default_Kind)
    {
      case A_Box_Default:
        res = &mkAdaBoxExp();
        break;

      case A_Name_Default:
        {
          SgFunctionParameterList&  fnparams = SG_DEREF(ty.get_parameterList());
          SgInitializedNamePtrList& parmlst  = fnparams.get_args();
          OperatorCallSupplement::ArgDescList arglist;
          auto argDescExtractor = [](SgInitializedName* ini) -> ArgDesc
                                  {
                                    return { "", ini->get_type() };
                                  };

          arglist.reserve(parmlst.size());
          std::transform(parmlst.begin(), parmlst.end(), std::back_inserter(arglist), argDescExtractor);

          res = &getExprID(decl.Formal_Subprogram_Default, ctx, OperatorCallSupplement(std::move(arglist), ty.get_return_type()));
          break;
        }

      case A_Null_Default:
        res = sb::buildNullptrValExp();
        break;

      case A_Nil_Default:
        res = &mkNullExpression();
        break;

      default: ;
    }

    return SG_DEREF(res);
  }

  AstContext::PragmaContainer
  splitOfPragmas(std::vector<Element_ID>& pragmaVector, const std::vector<Pragma_Kinds>& splitKinds, const AstContext&)
  {
    AstContext::PragmaContainer res;

    auto splitPragma = [aaa = splitKinds.begin(), zzz = splitKinds.end()]
                       (Element_ID pragmaId) -> bool
                       {
                         Element_Struct* elem = retrieveElemOpt(elemMap(), pragmaId);

                         return (  (elem != nullptr)
                                && (elem->Element_Kind == A_Pragma)
                                && (std::find(aaa, zzz, elem->The_Union.The_Pragma.Pragma_Kind) != zzz)
                                );
                       };

    auto const beg = pragmaVector.begin();
    auto const lim = pragmaVector.end();

    std::copy_if(beg, lim, std::back_inserter(res), splitPragma);
    auto const pos = std::remove_if(beg, lim, splitPragma);

    pragmaVector.erase(pos, lim);
    return res;
  }

  std::vector<Pragma_Kinds> const taskDeclPragmas{ A_Storage_Size_Pragma,
                                                   A_Priority_Pragma,
                                                   An_Interrupt_Priority_Pragma
                                                 };

  std::vector<Pragma_Kinds> const protectedDeclPragmas{ A_Priority_Pragma,
                                                        An_Interrupt_Priority_Pragma,
                                                        An_Attach_Handler_Pragma,
                                                        An_Interrupt_Handler_Pragma
                                                      };

  OperatorCallSupplement::ArgDescList
  toArgDescList(const SgTypePtrList& typlist)
  {
    OperatorCallSupplement::ArgDescList res;
    auto toArgDesc = [](SgType* ty) -> ArgDesc { return {"", ty}; };

    std::transform(typlist.begin(), typlist.end(), std::back_inserter(res), toArgDesc);
    return res;
  }

  Element_ID queryAsisIDOfDeclaration(Declaration_Struct& decl, Declaration_Kinds expected_stub, const AstContext& /* not used */)
  {
    if (decl.Corresponding_Declaration)
      return decl.Corresponding_Declaration;

    ADA_ASSERT(decl.Corresponding_Body_Stub);
    Element_Struct&     stubelem = retrieveElem(elemMap(), decl.Corresponding_Body_Stub);
    ADA_ASSERT(stubelem.Element_Kind == A_Declaration);

    Declaration_Struct& stubdecl = stubelem.The_Union.Declaration;
    ADA_ASSERT(stubdecl.Declaration_Kind == expected_stub);

    return stubdecl.Corresponding_Declaration;
  }


  struct AspectCreator
  {
      explicit
      AspectCreator(AstContext astctx)
      : ctx(astctx), args()
      {}

      AspectCreator(AspectCreator&&)                 = default;
      AspectCreator& operator=(AspectCreator&&)      = default;

      // \todo the following copying functions should be removed post C++17
      // @{
      AspectCreator(const AspectCreator&)            = default;
      AspectCreator& operator=(const AspectCreator&) = default;
      // @}

      const char* aspectName(Expression_Struct& mark)
      {
        if (mark.Expression_Kind != An_Identifier) return nullptr;

        return mark.Name_Image;
      }

      void operator()(Element_Struct& elem)
      {
        ADA_ASSERT(elem.Element_Kind == A_Definition);

        Definition_Struct&           def = elem.The_Union.Definition;
        ADA_ASSERT (def.Definition_Kind == An_Aspect_Specification);

        Aspect_Specification_Struct& asp = def.The_Union.The_Aspect_Specification;

        Element_Struct&              aspmark = retrieveElem(elemMap(), asp.Aspect_Mark);
        Element_Struct*              aspdefn = retrieveElemOpt(elemMap(), asp.Aspect_Definition);

        if (  (aspmark.Element_Kind != An_Expression)
           || (aspdefn && (aspdefn->Element_Kind != An_Expression))
           )
        {
          logError() << "Skipping unexpected Aspect representation: "
                     << aspmark.Element_Kind << "/" << (aspdefn ? aspdefn->Element_Kind : 0)
                     << std::endl;
          return;
        }

        Expression_Struct&           markex   = aspmark.The_Union.Expression;
        const char*                  markname = aspectName(markex);

        if (markname == nullptr)
        {
          logError() << "Unexpected Aspect mark representation: "
                     << aspmark.Element_Kind << "/" << (aspdefn ? aspdefn->Element_Kind : 0)
                     << std::endl;
          return;
        }

        SgExpression&                sgdefn = getExprID_opt(asp.Aspect_Definition, ctx);
        SgExpression&                sgnode = SG_DEREF(sb::buildActualArgumentExpression(markname, &sgdefn));

        attachSourceLocation(sgnode, elem, ctx);
        args.push_back(&sgnode);
      }

      /// result read-out
      operator SgExprListExp& ()
      {
        return mkExprListExp(args);
      }

    private:
      AstContext                 ctx;
      std::vector<SgExpression*> args;

      AspectCreator() = delete;
  };


  void processAspects(Element_Struct& /*elem*/, Declaration_Struct& decl, SgDeclarationStatement* sgnode, AstContext ctx)
  {
    ElemIdRange aspectRange = idRange(decl.Aspect_Specifications);

    if (aspectRange.empty()) return;

    if (sgnode == nullptr)
    {
      logError() << "found aspects w/o corresponding Sage declaration."
                 << std::endl;
      return;
    }

    // aspects use deferred elaboration, so that they can use
    //   type-bound operations defined later in scope.

    auto deferredAspectCompletion =
        [sgnode, aspectRange, ctx]()->void
        {
          SgExprListExp& asplist = traverseIDs(aspectRange, elemMap(), AspectCreator{ctx.pragmaAspectAnchor(*sgnode)});

          sg::linkParentChild(*sgnode, asplist, &SgDeclarationStatement::set_adaAspects);
        };

    ctx.storeDeferredUnitCompletion( std::move(deferredAspectCompletion) );
  }
}

void handlePragma(Element_Struct& el, SgStatement* stmtOpt, AstContext ctx)
{
  ctx.appendStatement(createPragma_common(el, stmtOpt, ctx));
}

void handleDeclaration(Element_Struct& elem, AstContext ctx, bool isPrivate)
{
  using PragmaContainer = AstContext::PragmaContainer;

  ADA_ASSERT (elem.Element_Kind == A_Declaration);
  logKind("A_Declaration", elem.ID);

  SgDeclarationStatement* assocdecl = nullptr;
  Declaration_Struct&     decl = elem.The_Union.Declaration;
  ElemIdRange             pragmaRange  = idRange(decl.Corresponding_Pragmas);
  std::vector<Element_ID> pragmaVector;

  std::copy(pragmaRange.first, pragmaRange.second, std::back_inserter(pragmaVector));

  switch (decl.Declaration_Kind)
  {
    case A_Package_Declaration:                    // 7.1(2)
      {
        logKind("A_Package_Declaration", elem.ID);

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
        ctx.appendStatement(sgnode);

        ADA_ASSERT (sgnode.search_for_symbol_from_symbol_table());

        PragmaContainer pendingPragmas;
        AstContext      pragmaCtx  = ctx.pragmas(pendingPragmas);

        // visible items
        {
          ElemIdRange range = idRange(decl.Visible_Part_Declarative_Items);

          traverseIDs(range, elemMap(), ElemCreator{pragmaCtx.scope(pkgspec)});
        }

        // private items
        {
          ElemIdRange range = idRange(decl.Private_Part_Declarative_Items);

          traverseIDs(range, elemMap(), ElemCreator{pragmaCtx.scope(pkgspec), true /* private items */});

          // a package may contain an empty private section
          pkgspec.set_hasPrivate(decl.Is_Private_Present);
        }

        // \todo consider:
        // currently, missing built-in function declarations are generated when needed
        //   a more principled implementation could generate them here, but they still would
        //   need to be identified when needed.
        // generateBuiltinFunctionsOnTypes(ctx.scope(pkgspec));

        processAndPlacePragmas(decl.Pragmas, { &pkgspec }, pragmaCtx.scope(pkgspec));

        /* unused nodes:
               Element_ID                     Corresponding_End_Name;
               bool                           Is_Name_Repeated;
               Declaration_ID                 Corresponding_Declaration;
         */

        assocdecl = &sgnode;
        break;
      }

    case A_Package_Body_Declaration:               // 7.2(2)
      {
        logKind("A_Package_Body_Declaration", elem.ID);

        Element_ID              specID   = queryAsisIDOfDeclaration(decl, A_Package_Body_Stub, ctx);
        // we need to check if the SgAdaPackageSpecDecl is directly available
        // or if it is wrapped by an SgAdaGenericDecl node.
        SgDeclarationStatement& declnode = lookupNode(asisDecls(), specID);
        SgAdaPackageSpecDecl*   specdcl  = getAdaPackageSpecDecl(&declnode);

        // unhandled package bodies
        ADA_ASSERT(specdcl != nullptr);

        SgDeclarationStatement* ndef    = findFirst(asisDecls(), decl.Corresponding_Body_Stub);
        SgAdaPackageBodyDecl*   nondef  = isSgAdaPackageBodyDecl(ndef);
        ADA_ASSERT(!ndef || nondef); // ndef => nondef

        // when this is an implementation of a stub, use the scope of the stub, instead of the global scope
        SgScopeStatement&       logicalScope = nondef ? SG_DEREF(nondef->get_scope())
                                                      : singleName(decl, ctx).parent_scope();

        SgAdaPackageBodyDecl&   sgnode  = mkAdaPackageBodyDecl(SG_DEREF(specdcl), nondef, logicalScope);
        SgAdaPackageBody&       pkgbody = SG_DEREF(sgnode.get_definition());

        recordNode(asisDecls(), elem.ID, sgnode);
        //~ recordNode(asisDecls(), adaname.id(), sgnode);

        sgnode.set_scope(specdcl->get_scope());
        attachSourceLocation(pkgbody, elem, ctx);
        attachSourceLocation(sgnode, elem, ctx);
        ctx.appendStatement(sgnode);

        const bool hasBodyStatements = idRange(decl.Body_Statements).size() > 0;

        completeDeclarationsWithHandledBlock( decl.Body_Declarative_Items,
                                              decl.Body_Statements,
                                              decl.Body_Exception_Handlers,
                                              decl.Pragmas,
                                              routineBlockHandler,
                                              routineExceptionBlockHandler,
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
         */
        assocdecl = &sgnode;
        break;
      }

    case A_Generic_Package_Declaration:            // 12.1(2)
      {
        logKind("A_Generic_Package_Declaration", elem.ID);

        NameData                adaname    = singleName(decl, ctx);
        // create generic declaration
        SgScopeStatement&       logicalScope = adaname.parent_scope();
        SgAdaGenericDecl&       sgnode     = mkAdaGenericDecl(adaname.ident, logicalScope);
        SgAdaGenericDefn&       gen_defn   = SG_DEREF(sgnode.get_definition());

        // create package in the scope of the generic
        //~ SgAdaPackageSpecDecl&   pkgnode    = mkAdaPackageSpecDecl(adaname.ident, logicalScope);
        SgAdaPackageSpecDecl&   pkgnode    = mkAdaPackageSpecDecl(adaname.ident, gen_defn);
        SgAdaPackageSpec&       pkgspec    = SG_DEREF(pkgnode.get_definition());

        // set declaration component of generic decl to package decl
        sgnode.set_declaration(&pkgnode);
        pkgnode.set_parent(&gen_defn);

        // record ID to sgnode mapping
        recordNode(asisDecls(), elem.ID, sgnode);
        recordNode(asisDecls(), adaname.id(), sgnode);

        // should private be set on the generic or on the package?
        //~ privatize(pkgnode, isPrivate);
        privatize(sgnode, isPrivate);
        attachSourceLocation(pkgspec, elem, ctx);
        attachSourceLocation(pkgnode, elem, ctx);
        attachSourceLocation(gen_defn, elem, ctx);
        attachSourceLocation(sgnode, elem, ctx);

        ctx.appendStatement(sgnode);
        ADA_ASSERT (pkgnode.get_parent() == &gen_defn);

        ADA_ASSERT (pkgnode.search_for_symbol_from_symbol_table());

        // generic formal part: this must be done first so the types defined in
        // the generic formal part exist when the package definition is processed.
        {
          ElemIdRange range = idRange(decl.Generic_Formal_Part);

          traverseIDs(range, elemMap(), ElemCreator{ctx.scope(gen_defn)});
        }

        // can the formal part also have pragmas?
        PragmaContainer pendingPragmas;
        AstContext      pragmaCtx  = ctx.pragmas(pendingPragmas);

        // visible items
        {
          ElemIdRange range = idRange(decl.Visible_Part_Declarative_Items);

          traverseIDs(range, elemMap(), ElemCreator{pragmaCtx.scope(pkgspec)});
        }

        // private items
        {
          ElemIdRange range = idRange(decl.Private_Part_Declarative_Items);

          traverseIDs(range, elemMap(), ElemCreator{pragmaCtx.scope(pkgspec), true});
        }

        processAndPlacePragmas(decl.Pragmas, { &pkgspec }, pragmaCtx.scope(pkgspec));

        /* unused nodes:
               Element_ID                     Corresponding_End_Name;
               bool                           Is_Name_Repeated;
               Declaration_ID                 Corresponding_Declaration;
         */
        assocdecl = &sgnode;
        break;
      }

    case A_Generic_Procedure_Declaration:          // 12.1(2)
    case A_Generic_Function_Declaration:           // 12.1(2)
      {
        logKind( decl.Declaration_Kind == A_Generic_Function_Declaration
                    ? "A_Generic_Function_Declaration"
                    : "A_Generic_Procedure_Declaration"
               , elem.ID
               );

        const bool             isFunc  = decl.Declaration_Kind == A_Generic_Function_Declaration;
        NameData               adaname = singleName(decl, ctx);
        SgScopeStatement&      logicalScope = adaname.parent_scope();

        // PP (20/10/21): the assertion does not hold for proc/func defined in their own unit
        //~ ADA_ASSERT (adaname.fullName == adaname.ident);
        SgAdaGenericDecl&      sgnode     = mkAdaGenericDecl(adaname.ident, logicalScope);
        SgAdaGenericDefn&      gen_defn   = SG_DEREF(sgnode.get_definition());

        // PP (20/10/21): use the logical scope
        //    the logical scope is the parent package in the package structure
        //    this could be different from the physical parent, for example when
        //    the generic proc/func forms its own subpackage.
        //~ outer.insert_symbol(adaname.ident, &mkBareNode<SgAdaGenericSymbol>(&sgnode));
        ctx.appendStatement(sgnode);

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
        // was: SgFunctionDeclaration&  fundec     = mkProcedureDecl_nondef(adaname.fullName, gen_defn, rettype, ParameterCompletion{params, ctx});
        SgFunctionDeclaration&  fundec     = mkProcedureDecl_nondef( adaname.ident,
                                                                     gen_defn, //~ logicalScope,
                                                                     rettype,
                                                                     ParameterCompletion{params, ctx}
                                                                   );

        sgnode.set_declaration(&fundec);
        fundec.set_parent(&gen_defn);

        setOverride(fundec, decl.Is_Overriding_Declaration);

        recordNode(asisDecls(), elem.ID, sgnode);
        recordNode(asisDecls(), adaname.id(), sgnode);

        // should private be set on the generic or on the proc?
        //~ privatize(fundec, isPrivate);
        privatize(sgnode, isPrivate);

        attachSourceLocation(fundec, elem, ctx);
        attachSourceLocation(sgnode, elem, ctx);
        attachSourceLocation(gen_defn, elem, ctx);

        ADA_ASSERT (fundec.get_parent() == &gen_defn);

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
        */
        assocdecl = &sgnode;
        break;
      }

    case A_Function_Declaration:                   // 6.1(4)   -> Trait_Kinds
    case A_Procedure_Declaration:                  // 6.1(4)   -> Trait_Kinds
      {
        logKind(decl.Declaration_Kind == A_Function_Declaration ? "A_Function_Declaration" : "A_Procedure_Declaration", elem.ID);

        const bool             isFunc  = decl.Declaration_Kind == A_Function_Declaration;
        NameData               adaname = singleName(decl, ctx);
        ElemIdRange            params  = idRange(decl.Parameter_Profile);
        SgType&                rettype = isFunc ? getDeclTypeID(decl.Result_Profile, ctx)
                                                : mkTypeVoid();

        SgScopeStatement&      logicalScope = adaname.parent_scope();
        const bool             renamingAsBody = definedByRenamingID(decl.Corresponding_Body, ctx);
        ParameterCompletion    complete{params, ctx};
        const std::string&     ident = adaname.ident;
        SgFunctionDeclaration& sgnode  = renamingAsBody
                                            ? mkAdaFunctionRenamingDecl(ident, logicalScope, rettype, std::move(complete))
                                            : mkProcedureDecl_nondef(ident, logicalScope, rettype, std::move(complete))
                                            ;

        setAbstractModifier(sgnode, decl);
        setOverride(sgnode, decl.Is_Overriding_Declaration);
        recordNode(asisDecls(), elem.ID, sgnode);
        recordNode(asisDecls(), adaname.id(), sgnode);

        privatize(sgnode, isPrivate);
        attachSourceLocation(sgnode, elem, ctx);
        ctx.appendStatement(sgnode);

        /* unhandled fields
             bool                          Is_Not_Overriding_Declaration
             bool                          Is_Dispatching_Operation
             Declaration_ID                Corresponding_Declaration
             Declaration_ID                Corresponding_Subprogram_Derivation
             Type_Definition_ID            Corresponding_Type
           +func:
             bool                          Is_Not_Null_Return
             Declaration_ID                Corresponding_Equality_Operator
        */
        assocdecl = &sgnode;
        break;
      }

    case A_Null_Procedure_Declaration:             // 6.7
    case A_Function_Body_Declaration:              // 6.3(2)
    case A_Procedure_Body_Declaration:             // 6.3(2)
      {
        if (decl.Declaration_Kind == A_Function_Declaration)
          logKind("A_Function_Body_Declaration", elem.ID);
        else if (decl.Declaration_Kind == A_Procedure_Body_Declaration)
          logKind("A_Procedure_Body_Declaration", elem.ID);
        else
          logKind("A_Null_Procedure_Declaration", elem.ID);

        const bool              isFunc  = decl.Declaration_Kind == A_Function_Body_Declaration;
        NameData                adaname = singleName(decl, ctx);
        ElemIdRange             params  = idRange(usableParameterProfile(decl, ctx));
        SgType&                 rettype = isFunc ? getDeclTypeID(decl.Result_Profile, ctx)
                                                 : mkTypeVoid();

        SgDeclarationStatement* ndef    = findFirst(asisDecls(), decl.Corresponding_Declaration, decl.Corresponding_Body_Stub);
        SgFunctionDeclaration*  nondef  = getFunctionDeclaration(ndef ? ndef->get_firstNondefiningDeclaration() : nullptr);
        ADA_ASSERT(!ndef || nondef); // ndef => nondef

        //~ logError() << "proc body: " << nondef << std::endl;

        // SCOPE_COMMENT_1: the logical scope is only used, if nondef is nullptr
        //   createFunDef chooses the scope as needed.
        SgScopeStatement&       logicalScope = adaname.parent_scope();
        SgFunctionDeclaration&  sgnode  = createFunDef(nondef, adaname.ident, logicalScope, rettype, ParameterCompletion{params, ctx});
        SgBasicBlock&           declblk = functionBody(sgnode);

        recordNode(asisDecls(), elem.ID, sgnode);
        recordNode(asisDecls(), adaname.id(), sgnode);
        privatize(sgnode, isPrivate);
        attachSourceLocation(sgnode, elem, ctx);
        attachSourceLocation(declblk, elem, ctx); // experimental
        ctx.appendStatement(sgnode);

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
        assocdecl = &sgnode;
        break;
      }



    case An_Incomplete_Type_Declaration:           // 3.2.1(2):3.10(2)
    case A_Tagged_Incomplete_Type_Declaration:     //  3.10.1(2)
      {
        logKind( decl.Declaration_Kind == An_Incomplete_Type_Declaration
                        ? "An_Incomplete_Type_Declaration"
                        : "A_Tagged_Incomplete_Type_Declaration"
               , elem.ID
               );

        if (decl.Corresponding_Type_Declaration)
        {
          // \todo handle pragmas in opaque types
          assocdecl = &handleOpaqueTypes(elem, decl, isPrivate, ctx);
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
          ctx.appendStatement(sgnode);
          recordNode(asisTypes(), adaname.id(), sgnode);
          assocdecl = &sgnode;
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
        const bool recordExtension = decl.Declaration_Kind != A_Private_Type_Declaration;

        logKind( recordExtension ? "A_Private_Extension_Declaration": "A_Private_Type_Declaration"
               , elem.ID
               );

        assocdecl = &handleOpaqueTypes(elem, decl, isPrivate, ctx);

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
        logKind("An_Ordinary_Type_Declaration", elem.ID);

        logTrace() << "Ordinary Type "
                   << "\n  abstract: " << decl.Has_Abstract
                   << "\n  limited: " << decl.Has_Limited
                   << std::endl;
        // \todo this may only declare one name (use singleName)
        NameData                    adaname = singleName(decl, ctx);
        ADA_ASSERT (adaname.fullName == adaname.ident);

        SgScopeStatement*           parentScope = &ctx.scope();
        Element_ID                  declDisrElemID = decl.Discriminant_Part;
        Element_ID                  scndDisrElemID = secondaryDiscriminants(elem.ID, decl, ctx);
        SgAdaDiscriminatedTypeDecl* discr = createDiscriminatedDeclID_opt(declDisrElemID, scndDisrElemID, ctx);

        if (discr)
        {
          parentScope = discr->get_discriminantScope();
        }

        SgScopeStatement&       scope  = SG_DEREF(parentScope);
        TypeData                ty     = getTypeFoundation(adaname.ident, decl, ctx.scope(scope));
        Element_ID              id     = adaname.id();
        SgDeclarationStatement* nondef = findFirst(asisTypes(), id);
        SgDeclarationStatement& sgdecl = sg::dispatch(TypeDeclMaker{adaname.ident, scope, ty, nondef}, &ty.sageNode());

        privatize(sgdecl, isPrivate);
        recordNode(asisTypes(), id, sgdecl, nondef != nullptr);
        attachSourceLocation(sgdecl, elem, ctx);

        if (!discr)
        {
          ADA_ASSERT(&ctx.scope() == parentScope);
          ctx.appendStatement(sgdecl);

          assocdecl = &sgdecl;
        }
        else
        {
          completeDiscriminatedDecl(id, elem, *discr, sgdecl, isPrivate, ctx);

          assocdecl = discr;
        }

        processInheritedElementsOfDerivedTypes(ty, discr ? *discr : sgdecl, ctx);

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

        ctx.appendStatement(sgnode);
        /* unused fields:
        */
        assocdecl = &sgnode;
        break;
      }

    case A_Formal_Type_Declaration:                // 12.5(2)
      {
        logKind("A_Formal_Type_Declaration", elem.ID);

        NameData                adaname = singleName(decl, ctx);
        ADA_ASSERT(adaname.fullName == adaname.ident);
        FormalTypeData          ty = getFormalTypeFoundation(adaname.ident, decl, ctx);
        SgScopeStatement&       scope = ctx.scope();
        ADA_ASSERT(scope.get_parent());

        Element_ID              id     = adaname.id();
        SgDeclarationStatement* nondef = findFirst(asisTypes(), id);
        SgDeclarationStatement& sgnode = ty.sageNode();

        setModifiers(sgnode, ty.isAbstract(), ty.isLimited(), ty.isTagged());

        privatize(sgnode, isPrivate);
        attachSourceLocation(sgnode, elem, ctx);

        if (/*SgAdaDiscriminatedTypeDecl* discr =*/ isSgAdaDiscriminatedTypeDecl(&sgnode))
          /* do nothing */;
        else
          ctx.appendStatement(sgnode);

        recordNode(asisTypes(), id, sgnode, nondef != nullptr);

        if (ty.inheritsRoutines())
        {
          processInheritedSubroutines(si::getDeclaredType(&sgnode), ty.definitionStruct(), ctx);
        }

        assocdecl = &sgnode;
        break;
      }

    case A_Formal_Procedure_Declaration:           // 12.6(2)
    case A_Formal_Function_Declaration:            // 12.6(2)
      {
        // \todo instead of modeling function parameters as functions,
        //       consider modeling them as variables of functions..
        const bool        isFormalFuncDecl = decl.Declaration_Kind == A_Formal_Function_Declaration;

        logKind( isFormalFuncDecl ? "A_Formal_Function_Declaration" : "A_Formal_Procedure_Declaration"
               , elem.ID
               );

        NameData               adaname = singleName(decl, ctx);
        ElemIdRange            params  = idRange(decl.Parameter_Profile);
        SgType&                rettype = isFormalFuncDecl ? getDeclTypeID(decl.Result_Profile, ctx)
                                                          : mkTypeVoid();

        ADA_ASSERT (adaname.fullName == adaname.ident);
        SgScopeStatement&      logicalScope = adaname.parent_scope();
        SgAdaSubroutineType&   funty   = mkAdaSubroutineType(rettype, ParameterCompletion{params, ctx}, ctx.scope(), false  /*isProtected*/ );
        SgExpression&          defaultInit = getDefaultFunctionExpr(decl, funty, ctx);
        SgAdaRenamingDecl&     sgnode  = mkAdaRenamingDecl(adaname.ident, defaultInit, funty, logicalScope);

        recordNode(asisDecls(), elem.ID, sgnode);
        recordNode(asisDecls(), adaname.id(), sgnode);

        attachSourceLocation(sgnode, elem, ctx);
        ctx.appendStatement(sgnode);

        /* unused fields:
              bool                           Has_Abstract;
              bool                           Is_Not_Null_Return;
              enum Subprogram_Default_Kinds  Default_Kind;
              bool                           Is_Not_Overriding_Declaration;
              bool                           Is_Overriding_Declaration;
              Expression_ID                  Formal_Subprogram_Default;
        */
        assocdecl = &sgnode;
        break;
      }


    case A_Subtype_Declaration:                    // 3.2.2(2)
      {
        logKind("A_Subtype_Declaration", elem.ID);

        NameData              adaname = singleName(decl, ctx);
        ADA_ASSERT (adaname.fullName == adaname.ident);

        const bool            forceSubtype = true;
        SgType&               subtype = getDefinitionTypeID(decl.Type_Declaration_View, ctx, forceSubtype);
        SgScopeStatement&     scope   = ctx.scope();
        SgTypedefDeclaration& sgnode  = mkTypeDecl(adaname.ident, subtype, scope);

        privatize(sgnode, isPrivate);
        attachSourceLocation(sgnode, elem, ctx);
        ctx.appendStatement(sgnode);
        recordNode(asisTypes(), adaname.id(), sgnode);

        /* unused fields:
              Declaration_ID                 Corresponding_First_Subtype;
              Declaration_ID                 Corresponding_Last_Constraint;
              Declaration_ID                 Corresponding_Last_Subtype;
        */
        assocdecl = &sgnode;
        break;
      }

    case A_Variable_Declaration:                   // 3.3.1(2) -> Trait_Kinds
      {
        logKind("A_Variable_Declaration", elem.ID);

        assocdecl = &handleVarCstDecl(elem, decl, ctx, isPrivate, tyIdentity);
        /* unused fields:
        */
        break;
      }

    case An_Integer_Number_Declaration:            // 3.3.2(2)
      {
        logKind("An_Integer_Number_Declaration", elem.ID);

        assocdecl = &handleNumberDecl(elem, decl, ctx, isPrivate, SG_DEREF(sb::buildAutoType()), mkIntegralType());
        /* unused fields:
        */
        break;
      }

    case A_Deferred_Constant_Declaration:          // 3.3.1(6):7.4(2) -> Trait_Kinds
    case A_Constant_Declaration:
      {
        logKind(decl.Declaration_Kind == A_Constant_Declaration ? "A_Constant_Declaration" : "A_Deferred_Constant_Declaration", elem.ID);

        assocdecl = &handleVarCstDecl(elem, decl, ctx, isPrivate, mkConstType);
        /* unused fields:
        */
        break;
      }

    case A_Real_Number_Declaration:                // 3.5.6(2)
      {
        logKind("A_Real_Number_Declaration", elem.ID);

        assocdecl = &handleNumberDecl(elem, decl, ctx, isPrivate, SG_DEREF(sb::buildAutoType()), mkRealType());
        /* unused fields:
         */
        break;
      }

    case A_Loop_Parameter_Specification:           // 5.5(4)   -> Trait_Kinds
      {
        logKind("A_Loop_Parameter_Specification", elem.ID);

        ADA_ASSERT (!isPrivate);

        NameData               adaname = singleName(decl, ctx);
        ADA_ASSERT (adaname.fullName == adaname.ident);

        SgExpression&          range   = getDefinitionExprID(decl.Specification_Subtype_Definition, ctx);
        SgType&                vartype = si::Ada::typeOfExpr(range).typerep_ref();
        SgInitializedName&     loopvar = mkInitializedName(adaname.fullName, vartype, &range);
        SgScopeStatement&      scope   = ctx.scope();

        recordNode(asisVars(), adaname.id(), loopvar);

        SgVariableDeclaration& sgnode  = mkVarDecl(loopvar, scope);

        attachSourceLocation(loopvar, elem, ctx);
        attachSourceLocation(sgnode, elem, ctx);
        ctx.appendStatement(sgnode);

        /* unused fields:
         */
        assocdecl = &sgnode;
        break;
      }

    case A_Parameter_Specification:                // 6.1(15)  -> Trait_Kinds
        // handled in getParm
        ADA_ASSERT(false);
        // \todo add pragma handling
        break;

    case A_Protected_Type_Declaration:             // 9.4(2)
      {
        logKind("A_Protected_Type_Declaration", elem.ID);

        auto               spec    = getProtectedSpecForProtectedType(decl, ctx);
        NameData           adaname = singleName(decl, ctx);
        ADA_ASSERT (adaname.fullName == adaname.ident);


        Element_ID                  id      = adaname.id();
        SgDeclarationStatement*     incomp  = findFirst(asisTypes(), id);
        SgAdaProtectedTypeDecl*     nondef  = isSgAdaProtectedTypeDecl(incomp);
        ADA_ASSERT(!incomp || nondef);

        SgScopeStatement*           parentScope = &ctx.scope();
        SgAdaDiscriminatedTypeDecl* discr = createDiscriminatedDeclID_opt(decl.Discriminant_Part, 0, ctx);

        if (discr)
        {
          parentScope = discr->get_discriminantScope();
          ADA_ASSERT(parentScope != nullptr);
        }

        SgAdaProtectedTypeDecl& sgdecl  = nondef ? mkAdaProtectedTypeDecl(*nondef, SG_DEREF(spec.first), *parentScope)
                                                 : mkAdaProtectedTypeDecl(adaname.fullName, spec.first,  *parentScope);

        attachSourceLocation(sgdecl, elem, ctx);
        privatize(sgdecl, isPrivate);

        recordNode(asisTypes(), id, sgdecl, nondef != nullptr);
        recordNode(asisDecls(), id, sgdecl);
        recordNode(asisDecls(), elem.ID, sgdecl);

        if (!discr)
        {
          ADA_ASSERT(&ctx.scope() == parentScope);
          ctx.appendStatement(sgdecl);
          assocdecl = &sgdecl;
        }
        else
        {
          completeDiscriminatedDecl(id, elem, *discr, sgdecl, isPrivate, ctx);
          assocdecl = discr;
        }

        AstContext::PragmaContainer protectedPragmas = splitOfPragmas(pragmaVector, protectedDeclPragmas, ctx);

        spec.second(std::move(protectedPragmas)); // complete the body

        /* unused fields:
             bool                           Has_Protected;
             Element_ID                     Corresponding_End_Name;
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
        logKind("A_Task_Type_Declaration", elem.ID);

        auto                        spec    = getTaskSpecForTaskType(decl, ctx);
        NameData                    adaname = singleName(decl, ctx);
        ADA_ASSERT (adaname.fullName == adaname.ident);

        Element_ID                  id     = adaname.id();
        SgDeclarationStatement*     ndef   = findFirst(asisTypes(), id);
        SgAdaTaskTypeDecl*          nondef = isSgAdaTaskTypeDecl(ndef);
        ADA_ASSERT(!ndef || nondef); // ndef => nondef

        SgScopeStatement*           parentScope = &ctx.scope();
        SgAdaDiscriminatedTypeDecl* discr = createDiscriminatedDeclID_opt(decl.Discriminant_Part, 0, ctx);

        if (discr)
        {
          parentScope = discr->get_discriminantScope();
          ADA_ASSERT(parentScope != nullptr);
        }

        SgAdaTaskTypeDecl& sgdecl  = nondef ? mkAdaTaskTypeDecl(*nondef, SG_DEREF(spec.first), *parentScope)
                                            : mkAdaTaskTypeDecl(adaname.fullName, spec.first,  *parentScope);

        attachSourceLocation(sgdecl, elem, ctx);
        privatize(sgdecl, isPrivate);

        recordNode(asisTypes(), id, sgdecl, nondef != nullptr);
        recordNode(asisDecls(), id, sgdecl);
        recordNode(asisDecls(), elem.ID, sgdecl);

        if (!discr)
        {
          ADA_ASSERT(&ctx.scope() == parentScope);
          ctx.appendStatement(sgdecl);
          assocdecl = &sgdecl;
        }
        else
        {
          completeDiscriminatedDecl(id, elem, *discr, sgdecl, isPrivate, ctx);
          assocdecl = discr;
        }

        AstContext::PragmaContainer taskPragmas = splitOfPragmas(pragmaVector, taskDeclPragmas, ctx);

        spec.second(std::move(taskPragmas)); // complete the body

        /* unused fields:
             bool                           Has_Task;
             Element_ID                     Corresponding_End_Name;
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
        logKind("A_Single_Protected_Declaration", elem.ID);

        auto               spec    = getProtectedSpecForSingleProtected(decl, ctx);
        NameData           adaname = singleName(decl, ctx);
        ADA_ASSERT (adaname.fullName == adaname.ident);
        SgAdaProtectedSpecDecl& sgnode  = mkAdaProtectedSpecDecl(adaname.fullName, SG_DEREF(spec.first), ctx.scope());

        attachSourceLocation(sgnode, elem, ctx);
        privatize(sgnode, isPrivate);
        ctx.appendStatement(sgnode);
        //~ recordNode(asisTypes(), adaname.id(), sgnode);
        recordNode(asisDecls(), elem.ID, sgnode);
        recordNode(asisDecls(), adaname.id(), sgnode);

        AstContext::PragmaContainer protectedPragmas = splitOfPragmas(pragmaVector, protectedDeclPragmas, ctx);

        spec.second(std::move(protectedPragmas)); // complete the body
        /* unused fields:
             bool                           Has_Protected;
             Element_ID                     Corresponding_End_Name;
             bool                           Is_Name_Repeated;
             Declaration_ID                 Corresponding_Declaration
             Declaration_ID                 Corresponding_Body
             Expression_List                Declaration_Interface_List
        */
        assocdecl = &sgnode;
        break;
      }

    case A_Single_Task_Declaration:                // 3.3.1(2):9.1(3)
      {
        logKind("A_Single_Task_Declaration", elem.ID);

        auto               spec    = getTaskSpecForSingleTask(decl, ctx);
        NameData           adaname = singleName(decl, ctx);
        ADA_ASSERT (adaname.fullName == adaname.ident);
        SgAdaTaskSpecDecl& sgnode  = mkAdaTaskSpecDecl(adaname.fullName, SG_DEREF(spec.first), ctx.scope());

        attachSourceLocation(sgnode, elem, ctx);
        privatize(sgnode, isPrivate);
        ctx.appendStatement(sgnode);
        //~ recordNode(asisTypes(), adaname.id(), sgnode);
        recordNode(asisDecls(), elem.ID, sgnode);
        recordNode(asisDecls(), adaname.id(), sgnode);

        AstContext::PragmaContainer taskPragmas = splitOfPragmas(pragmaVector, taskDeclPragmas, ctx);

        spec.second(std::move(taskPragmas)); // complete the body
        /* unused fields:
             bool                           Has_Task;
             Element_ID                     Corresponding_End_Name;
             bool                           Is_Name_Repeated;
             Declaration_ID                 Corresponding_Declaration;
             Declaration_ID                 Corresponding_Body;
             Expression_List                Declaration_Interface_List
        */
        assocdecl = &sgnode;
        break;
      }

    case A_Protected_Body_Declaration:             // 9.4(7)
      {
        logKind("A_Protected_Body_Declaration", elem.ID);

        SgAdaProtectedBody&     pobody  = mkAdaProtectedBody();
        NameData                adaname = singleName(decl, ctx);
        SgDeclarationStatement* ndef    = findFirst(asisDecls(), decl.Corresponding_Body_Stub);
        SgAdaProtectedBodyDecl* nondef  = isSgAdaProtectedBodyDecl(ndef);
        ADA_ASSERT(!ndef || nondef); // ndef => nondef

        Element_ID              specID  = queryAsisIDOfDeclaration(decl, A_Protected_Body_Stub, ctx);
        SgDeclarationStatement& podecl  = lookupNode(asisDecls(), specID);
        SgScopeStatement&       logicalScope = adaname.parent_scope();
        SgAdaProtectedBodyDecl& sgnode  = mkAdaProtectedBodyDecl(podecl, nondef, pobody, logicalScope);

        attachSourceLocation(sgnode, elem, ctx);
        privatize(sgnode, isPrivate);
        ctx.appendStatement(sgnode);
        //~ recordNode(asisDecls(), elem.ID, sgnode);
        recordNode(asisDecls(), adaname.id(), sgnode);

        PragmaContainer pendingPragmas;
        AstContext      pragmaCtx  = ctx.pragmas(pendingPragmas);

        {
          ElemIdRange decls = idRange(decl.Protected_Operation_Items);

          traverseIDs(decls, elemMap(), StmtCreator{pragmaCtx.scope(pobody)});
        }

        processAndPlacePragmas(decl.Pragmas, { &pobody }, pragmaCtx.scope(pobody));

        /* unused fields:
               bool                           Has_Protected
               Element_ID                     Corresponding_End_Name
               bool                           Is_Name_Repeated
               bool                           Is_Subunit
        */
        assocdecl = &sgnode;
        break;
      }

    case A_Task_Body_Declaration:                  // 9.1(6)
      {
        logKind("A_Task_Body_Declaration", elem.ID);

        SgAdaTaskBody&          tskbody = mkAdaTaskBody();
        NameData                adaname = singleName(decl, ctx);
        SgDeclarationStatement* ndef    = findFirst(asisDecls(), decl.Corresponding_Body_Stub);
        SgAdaTaskBodyDecl*      nondef  = isSgAdaTaskBodyDecl(ndef);
        ADA_ASSERT(!ndef || nondef); // ndef => nondef

        Element_ID              specID  = queryAsisIDOfDeclaration(decl, A_Task_Body_Stub, ctx);
        SgDeclarationStatement& tskdecl = lookupNode(asisDecls(), specID);

        // ADA_ASSERT (adaname.fullName == adaname.ident);
        SgScopeStatement&       logicalScope = adaname.parent_scope();
        SgAdaTaskBodyDecl&      sgnode  = mkAdaTaskBodyDecl(tskdecl, nondef, tskbody, logicalScope);

        attachSourceLocation(sgnode, elem, ctx);
        privatize(sgnode, isPrivate);
        ctx.appendStatement(sgnode);

        //~ recordNode(asisDecls(), elem.ID, sgnode);
        recordNode(asisDecls(), adaname.id(), sgnode);

        completeDeclarationsWithHandledBlock( decl.Body_Declarative_Items,
                                              decl.Body_Statements,
                                              decl.Body_Exception_Handlers,
                                              decl.Pragmas,
                                              routineBlockHandler,
                                              routineExceptionBlockHandler,
                                              tskbody,
                                              false /* same block for declarations and statements */,
                                              ctx
                                            );

        /* unused fields:
             bool                           Has_Task;
             Element_ID                     Corresponding_End_Name;
             Declaration_ID                 Body_Block_Statement;
             bool                           Is_Name_Repeated;
             bool                           Is_Subunit;
        */
        assocdecl = &sgnode;
        break;
      }

    case An_Entry_Declaration:                     // 9.5.2(2)
      {
        logKind("An_Entry_Declaration", elem.ID);

        NameData        adaname = singleName(decl, ctx);
        ElemIdRange     range   = idRange(decl.Parameter_Profile);

        ADA_ASSERT (adaname.fullName == adaname.ident);
        SgType&         idxTy  = getDefinitionTypeID_opt(decl.Entry_Family_Definition, ctx);
        SgAdaEntryDecl& sgnode = mkAdaEntryDecl(adaname.fullName, ctx.scope(), ParameterCompletion{range, ctx}, idxTy);

        attachSourceLocation(sgnode, elem, ctx);
        privatize(sgnode, isPrivate);
        ctx.appendStatement(sgnode);

        // the entry call links back to the declaration ID
        recordNode(asisDecls(), elem.ID, sgnode);

        // for consistency, also map the name id to the node
        recordNode(asisDecls(), adaname.id(), sgnode);

        /* unused fields:
             bool                           Is_Overriding_Declaration;
             bool                           Is_Not_Overriding_Declaration;
             Declaration_ID                 Corresponding_Body;
        */
        assocdecl = &sgnode;
        break;
      }

    case An_Entry_Body_Declaration:                // 9.5.2(5)
      {
        logKind("An_Entry_Body_Declaration", elem.ID);
        NameData                adaname  = singleName(decl, ctx);
        SgDeclarationStatement* sagedcl  = findFirst(asisDecls(), decl.Corresponding_Declaration);
        SgAdaEntryDecl&         entrydcl = SG_DEREF(isSgAdaEntryDecl(sagedcl));
        ADA_ASSERT (adaname.fullName == adaname.ident);

        //~ SgScopeStatement&       logicalScope = adaname.parent_scope();
        //~ SgAdaEntryDecl&         sgnode  = mkAdaEntryDef(entrydcl, logicalScope, ParameterCompletion{range, ctx});
        ElemIdRange             params  = idRange(usableParameterProfile(decl, ctx));

        // PP (1/20/23): *SCOPE_COMMENT_1
        //               replace outer with entrydcl.get_scope()
        //               note, this will have entries use the same scope handling as
        //                     functions and procedures.
        //                     Not sure if this is correct, because by giving a definition
        //                     in the body the scope of a spec, will make symbols
        //                     in the body invisible, unless the physical scope is used
        //                     for lookup.
        SgAdaEntryDecl&         sgnode  = mkAdaEntryDefn( entrydcl,
                                                          SG_DEREF(entrydcl.get_scope()), // was: ctx.scope(),
                                                          ParameterCompletion{params, ctx},
                                                          EntryIndexCompletion{decl.Entry_Index_Specification, ctx}
                                                        );

        recordNode(asisDecls(), elem.ID, sgnode);
        recordNode(asisDecls(), adaname.id(), sgnode);
        privatize(sgnode, isPrivate);
        attachSourceLocation(sgnode, elem, ctx);

        ctx.appendStatement(sgnode);

        SgFunctionDefinition&   fndef   = SG_DEREF(sgnode.get_definition());
        SgBasicBlock&           declblk = SG_DEREF(fndef.get_body());

        //~ if (isInvalidId(decl.Entry_Barrier))
        //~ logError() << "Entry_Barrier-id " << decl.Entry_Barrier << std::endl;

        SgExpression&           barrier = getExprID_opt(decl.Entry_Barrier, ctx.scope(fndef));

        sg::linkParentChild(sgnode, barrier, &SgAdaEntryDecl::set_entryBarrier);
        completeRoutineBody(decl, declblk, ctx);

        /* unused fields:
             Element_ID                     Corresponding_End_Name
             Declaration_ID                 Body_Block_Statement
             bool                           Is_Name_Repeated
        */
        assocdecl = &sgnode;
        break;
      }

    case A_Function_Body_Stub:                     // 10.1.3(3)
    case A_Procedure_Body_Stub:                    // 10.1.3(3)
      {
        const bool isFunc  = decl.Declaration_Kind == A_Function_Body_Stub;

        logKind(isFunc ? "A_Function_Body_Stub" : "A_Procedure_Body_Stub", elem.ID);

        NameData                adaname = singleName(decl, ctx);
        ElemIdRange             params  = idRange(decl.Parameter_Profile);
        SgType&                 rettype = isFunc ? getDeclTypeID(decl.Result_Profile, ctx)
                                                 : mkTypeVoid();

        ADA_ASSERT (adaname.fullName == adaname.ident);
        SgDeclarationStatement* ndef    = findFirst(asisDecls(), decl.Corresponding_Declaration);
        SgFunctionDeclaration*  nondef  = getFunctionDeclaration(ndef);
        ADA_ASSERT(!ndef || nondef); // ndef => nondef

        SgScopeStatement&       logicalScope = adaname.parent_scope();
        SgFunctionDeclaration&  sgnode  = createFunDcl(nondef, adaname.ident, logicalScope, rettype, ParameterCompletion{params, ctx});

        setAdaSeparate(sgnode, true /* separate */);

        recordNode(asisDecls(), elem.ID, sgnode);
        recordNode(asisDecls(), adaname.id(), sgnode);
        privatize(sgnode, isPrivate);
        attachSourceLocation(sgnode, elem, ctx);
        ctx.appendStatement(sgnode);

        /* unhandled field
           bool                           Is_Overriding_Declaration;
           bool                           Is_Not_Overriding_Declaration;
           Declaration_ID                 Corresponding_Subunit
           bool                           Is_Dispatching_Operation

         +func:
           bool                           Is_Not_Null_Return

           break;
        */
        assocdecl = &sgnode;
        break;
      }

    case A_Task_Body_Stub:                         // 10.1.3(5)
      {
        logKind("A_Task_Body_Stub", elem.ID);

        NameData                adaname = singleName(decl, ctx);
        SgDeclarationStatement& tskdecl = lookupNode(asisDecls(), decl.Corresponding_Declaration);
        SgScopeStatement&       logicalScope = adaname.parent_scope();
        SgAdaTaskBodyDecl&      sgnode  = mkAdaTaskBodyDecl_nondef(tskdecl, logicalScope);

        setAdaSeparate(sgnode, true /* separate */);

        recordNode(asisDecls(), elem.ID, sgnode);
        recordNode(asisDecls(), adaname.id(), sgnode);
        privatize(sgnode, isPrivate);
        attachSourceLocation(sgnode, elem, ctx);
        ctx.appendStatement(sgnode);

        /* unhandled field
            bool                           Has_Task;
            Declaration_ID                 Corresponding_Subunit;
        */
        assocdecl = &sgnode;
        break;
      }

    case A_Protected_Body_Stub:                    // 10.1.3(6)
      {
        logKind("A_Protected_Body_Stub", elem.ID);

        // \todo combine code with A_Task_Body_Stub
        NameData                adaname = singleName(decl, ctx);
        SgDeclarationStatement& podecl  = lookupNode(asisDecls(), decl.Corresponding_Declaration);
        SgScopeStatement&       logicalScope = adaname.parent_scope();
        SgAdaProtectedBodyDecl& sgnode  = mkAdaProtectedBodyDecl_nondef(podecl, logicalScope);

        setAdaSeparate(sgnode, true /* separate */);

        recordNode(asisDecls(), elem.ID, sgnode);
        recordNode(asisDecls(), adaname.id(), sgnode);
        privatize(sgnode, isPrivate);
        attachSourceLocation(sgnode, elem, ctx);
        ctx.appendStatement(sgnode);

        /* unhandled field
            bool                           Has_Protected;
            Declaration_ID                 Corresponding_Subunit;
        */
        assocdecl = &sgnode;
        break;
      }

    case A_Package_Body_Stub:                      // 10.1.3(4)
      {
        logKind("A_Package_Body_Stub", elem.ID);

        NameData                adaname  = singleName(decl, ctx);
        SgDeclarationStatement& declnode = lookupNode(asisDecls(), decl.Corresponding_Declaration);
        SgAdaPackageSpecDecl*   specdcl  = getAdaPackageSpecDecl(&declnode);
        ADA_ASSERT(specdcl != nullptr);

        SgScopeStatement&       logicalScope = adaname.parent_scope();
        SgAdaPackageBodyDecl&   sgnode   = mkAdaPackageBodyDecl_nondef(*specdcl, logicalScope);

        setAdaSeparate(sgnode, true /* separate */);

        recordNode(asisDecls(), elem.ID, sgnode);
        recordNode(asisDecls(), adaname.id(), sgnode);
        privatize(sgnode, isPrivate);
        attachSourceLocation(sgnode, elem, ctx);
        ctx.appendStatement(sgnode);
        /* unhandled field
            Declaration_ID                 Corresponding_Subunit;
        */
        assocdecl = &sgnode;
        break;
      }

    case An_Exception_Declaration:                 // 11.1(2)
      {
        logKind("An_Exception_Declaration", elem.ID);

        using name_container = NameCreator::result_container;

        ElemIdRange              range    = idRange(decl.Names);
        name_container           names    = allNames(range, ctx);
        SgScopeStatement&        scope    = ctx.scope();
        SgType&                  excty    = lookupNode(adaTypes(), AdaIdentifier{"Exception"});
        SgInitializedNamePtrList dclnames = constructInitializedNamePtrList(ctx, asisExcps(), names, excty, nullptr);
        SgVariableDeclaration&   sgnode   = mkExceptionDecl(dclnames, scope);

        attachSourceLocation(sgnode, elem, ctx);
        privatize(sgnode, isPrivate);
        ctx.appendStatement(sgnode);
        assocdecl = &sgnode;
        break;
      }

    case A_Component_Declaration:                  // 3.8(6)
      {
        logKind("A_Component_Declaration", elem.ID);

        handleVarCstDecl(elem, decl, ctx, isPrivate, tyIdentity);
        /* unused clause:
              Pragma_Element_ID_List         Corresponding_Pragmas;
              Element_ID_List                Aspect_Specifications;
              Representation_Clause_List     Corresponding_Representation_Clauses;
        */
        // assocdecl = &sgnode;
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
        logKind( decl.Declaration_Kind == A_Package_Renaming_Declaration
                   ? "A_Package_Renaming_Declaration" : "A_Generic_Package_Renaming_Declaration"
               , elem.ID
               );

        NameData                adaname = singleName(decl, ctx);

        if (isInvalidId(decl.Renamed_Entity))
        {
          logWarn() << "skipping unknown package renaming: " << adaname.ident << "/" << adaname.fullName
                    << ": " << elem.ID << " / " << decl.Renamed_Entity
                    << std::endl;
          return;
        }

/*
        SgDeclarationStatement* aliased = &getAliasedID(decl.Renamed_Entity, ctx);

        if (SgAdaGenericDecl* gendcl = isSgAdaGenericDecl(aliased))
          aliased = gendcl->get_declaration();
*/
        SgExpression&           renamed = getExprID(decl.Renamed_Entity, ctx);
        SgScopeStatement&       scope   = ctx.scope();
        SgType&                 pkgtype = mkTypeVoid();
        SgAdaRenamingDecl&      sgnode  = mkAdaRenamingDecl(adaname.ident, renamed, pkgtype, scope);

        recordNode(asisDecls(), elem.ID, sgnode);
        recordNode(asisDecls(), adaname.id(), sgnode);

        attachSourceLocation(sgnode, elem, ctx);
        privatize(sgnode, isPrivate);
        ctx.appendStatement(sgnode);
        /* unhandled field
             Declaration_ID                 Corresponding_Declaration;
             Expression_ID                  Corresponding_Base_Entity;
        */
        assocdecl = &sgnode;
        break;
      }

    case A_Procedure_Renaming_Declaration:         // 8.5.4(2)
    case A_Function_Renaming_Declaration:          // 8.5.4(2)
      {
        const bool                 isFuncRename = decl.Declaration_Kind == A_Function_Renaming_Declaration;

        logKind(isFuncRename ? "A_Function_Renaming_Declaration" : "A_Procedure_Renaming_Declaration", elem.ID);

        SgScopeStatement&          outer     = ctx.scope();
        NameData                   adaname   = singleName(decl, ctx);
        ElemIdRange                range     = idRange(decl.Parameter_Profile);
        SgType&                    rettype   = isFuncRename ? getDeclTypeID(decl.Result_Profile, ctx)
                                                            : mkTypeVoid();
        SgDeclarationStatement*    nondefDcl = findFirst(asisDecls(), decl.Corresponding_Declaration);
        SgAdaFunctionRenamingDecl* nondefFun = isSgAdaFunctionRenamingDecl(nondefDcl);
        ADA_ASSERT(!nondefDcl || nondefFun); // nondefDcl => nondefFun

        ADA_ASSERT (adaname.fullName == adaname.ident);
        SgAdaFunctionRenamingDecl& sgnode    = mkAdaFunctionRenamingDecl( adaname.fullName,
                                                                          outer,
                                                                          rettype,
                                                                          ParameterCompletion{range, ctx},
                                                                          nondefFun );
        setOverride(sgnode, decl.Is_Overriding_Declaration);
        recordNode(asisDecls(), elem.ID, sgnode);
        recordNode(asisDecls(), adaname.id(), sgnode);

        // find declaration for the thing being renamed
        SgFunctionType&            fntype = SG_DEREF(isSgFunctionType(sgnode.get_type()));
        const SgTypePtrList&       typlist = SG_DEREF(fntype.get_argument_list()).get_arguments();
        OperatorCallSupplement     suppl(toArgDescList(typlist), &rettype);
        SgExpression&              renamedFun = getExprID(decl.Renamed_Entity, ctx, std::move(suppl));

        sgnode.set_renamed_function(&renamedFun);
        privatize(sgnode, isPrivate);
        attachSourceLocation(sgnode, elem, ctx);
        ctx.appendStatement(sgnode);

        /* unhandled field
           bool                           Is_Not_Null_Return;
           bool                           Is_Not_Overriding_Declaration;
           Expression_ID                  Corresponding_Base_Entity;
           bool                           Is_Dispatching_Operation;
         */
        assocdecl = &sgnode;
        break;
      }

    case An_Object_Renaming_Declaration:           // 8.5.1(2)
      {
        logKind("An_Object_Renaming_Declaration", elem.ID);

        NameData            adaname = singleName(decl, ctx);
        SgScopeStatement&   scope   = ctx.scope();
        Declaration_Struct& asisDcl = elem.The_Union.Declaration;
        SgType&             ty      = getDeclTypeID(asisDcl.Object_Declaration_View, ctx);
        SgExpression&       renamed = getExprID(decl.Renamed_Entity, ctx);
        SgAdaRenamingDecl&  sgnode  = mkAdaRenamingDecl(adaname.ident, renamed, ty, scope);

        recordNode(asisDecls(), elem.ID, sgnode);
        recordNode(asisDecls(), adaname.id(), sgnode);

        attachSourceLocation(sgnode, elem, ctx);
        privatize(sgnode, isPrivate);
        ctx.appendStatement(sgnode);
        assocdecl = &sgnode;
        break;
      }

    case An_Exception_Renaming_Declaration:        // 8.5.2(2)
      {
        //~ using ExBasePair = std::pair<SgInitializedName*, SgAdaRenamingDecl*>;

        logKind("An_Exception_Renaming_Declaration", elem.ID);

        NameData           adaname = singleName(decl, ctx);
        SgScopeStatement&  scope   = ctx.scope();
        SgExpression&      renamed = getExprID(decl.Renamed_Entity, ctx);
        SgType&            excty   = lookupNode(adaTypes(), AdaIdentifier{"Exception"});

        // ADA_ASSERT(renamed.get_type() == &excty); \todo not sure why this does not hold...
        SgAdaRenamingDecl& sgnode  = mkAdaRenamingDecl(adaname.ident, renamed, excty, scope);

        recordNode(asisDecls(), elem.ID, sgnode);
        recordNode(asisDecls(), adaname.id(), sgnode);

        attachSourceLocation(sgnode, elem, ctx);
        privatize(sgnode, isPrivate);
        ctx.appendStatement(sgnode);
        assocdecl = &sgnode;
        break;
      }

    case A_Generic_Procedure_Renaming_Declaration: // 8.5.5(2)
    case A_Generic_Function_Renaming_Declaration:  // 8.5.5(2)
      {
        // \todo consider folding the code into generic_package_renaming

        logKind( decl.Declaration_Kind == A_Generic_Function_Renaming_Declaration
                      ? "A_Generic_Function_Renaming_Declaration"
                      : "A_Generic_Procedure_Renaming_Declaration"
               , elem.ID
               );

        NameData                adaname = singleName(decl, ctx);
        SgExpression&           renamed = getExprID(decl.Renamed_Entity, ctx);
        SgScopeStatement&       scope   = ctx.scope();
        SgType&                 pkgtype = mkTypeVoid();
        SgAdaRenamingDecl&      sgnode  = mkAdaRenamingDecl(adaname.ident, renamed, pkgtype, scope);

        recordNode(asisDecls(), elem.ID, sgnode);
        recordNode(asisDecls(), adaname.id(), sgnode);

        attachSourceLocation(sgnode, elem, ctx);
        privatize(sgnode, isPrivate);
        ctx.appendStatement(sgnode);

        /* unused field
             Declaration_ID                 Corresponding_Declaration;
             Expression_ID                  Corresponding_Base_Entity;
        */
        assocdecl = &sgnode;
        break;
      }

    case A_Package_Instantiation:                  // 12.3(2)
    case A_Procedure_Instantiation:                // 12.3(2)
    case A_Function_Instantiation:                 // 12.3(2)
      {
        // generic instantiation
        if (decl.Declaration_Kind == A_Function_Instantiation)
          logKind("A_Function_Instantiation", elem.ID);
        else if (decl.Declaration_Kind == A_Procedure_Instantiation)
          logKind("A_Procedure_Instantiation", elem.ID);
        else
          logKind("A_Package_Instantiation", elem.ID);

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

        if (false && basedecl == nullptr)
        {
          // Integer_IO: 24138136 and 24551081 not found
          logFlaw() << basename.ident << ": "
                    << baseexpr.Corresponding_Name_Declaration << " and "
                    << baseexpr.Corresponding_Name_Definition << " not found"
                    << std::endl;

          logFlaw() << elemMap().at(baseexpr.Corresponding_Name_Declaration)->Element_Kind
                    << " / " << elemMap().at(baseexpr.Corresponding_Name_Definition)->Element_Kind
                    << std::endl;
        }

        // PP (2/2/22): the base decl can also be a renamed generic declaration
        SgScopeStatement&         logicalScope = adaname.parent_scope();
        SgAdaGenericInstanceDecl& sgnode   = mkAdaGenericInstanceDecl(adaname.ident, SG_DEREF(basedecl), logicalScope);

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
        ctx.appendStatement(sgnode);
        ADA_ASSERT (sgnode.get_instantiatedScope()->get_parent() == &sgnode);

        // PP (4/1/22): fill in the declaration
        ADA_ASSERT(decl.Corresponding_Declaration);
        handleDeclaration( retrieveElem(elemMap(), decl.Corresponding_Declaration),
                           ctx.instantiation(sgnode).scope(SG_DEREF(sgnode.get_instantiatedScope()))
                         );
        // \todo mark whole subtree under sgnode.get_instantiatedScope() as instantiated

        /* unused field
             Declaration_ID                 Corresponding_Body;
        */
        assocdecl = &sgnode;
        break;
      }

    case A_Formal_Package_Declaration:             // 12.7(2)
    case A_Formal_Package_Declaration_With_Box:    // 12.7(3)
      {
        if (decl.Declaration_Kind == A_Formal_Package_Declaration)
          logKind("A_Formal_Package_Declaration", elem.ID);
        else
          logKind("A_Formal_Package_Declaration_With_Box", elem.ID);

        // \todo consider merging the code with A_Package_Instantiation above
        NameData                  adaname  = singleName(decl, ctx);

        // re basedecl see MS comment on A_Package_Instantiation above
        NameData                  basename = getNameID(decl.Generic_Unit_Name, ctx);
        Element_Struct&           baseelem = basename.elem();
        Expression_Struct&        baseexpr = baseelem.The_Union.Expression;
        SgDeclarationStatement*   basedecl = findFirst(asisDecls(), baseexpr.Corresponding_Name_Declaration, baseexpr.Corresponding_Name_Definition);

        // generic actual part
        SgScopeStatement&         scope    = ctx.scope();
        ElemIdRange               range    = idRange(decl.Generic_Actual_Part);
        SgExprListExp&            args     = traverseIDs(range, elemMap(), ArgListCreator{ctx});

        if (decl.Declaration_Kind == A_Formal_Package_Declaration_With_Box)
          args.append_expression(&mkAdaBoxExp());

        // PP (3/31/22): In contrast to the instantiations above (check correctness),
        //               the scope should be the current (generic parameter) scope.
        SgAdaFormalPackageDecl&   sgnode   = mkAdaFormalPackageDecl(adaname.ident, SG_DEREF(basedecl), args, scope);

        recordNode(asisDecls(), elem.ID, sgnode);
        recordNode(asisDecls(), adaname.id(), sgnode);

        attachSourceLocation(sgnode, elem, ctx);
        //~ privatize(sgnode, isPrivate);
        ctx.appendStatement(sgnode);

        if (Element_Struct* dclElem = retrieveElemOpt(elemMap(), decl.Corresponding_Declaration))
        {
          handleDeclaration(*dclElem, ctx.scope(SG_DEREF(sgnode.get_prototypeScope())));

          // look up declarations
          SgDeclarationStatement& protoDecl = lookupNode(asisDecls(), decl.Corresponding_Declaration);

          sgnode.set_prototype(&protoDecl);
        }


        /* unused field
          +  A_Formal_Package_Declaration
             Declaration_ID                 Corresponding_Body;
        */

        assocdecl = &sgnode;
        break;
      }

    // \todo handle pragmas and aspects on these elements
    case An_Entry_Index_Specification:             // 9.5.2(2)
        // handled by EntryIndexCompletion;
    case A_Choice_Parameter_Specification:         // 11.2(4)
        // handled in handleExceptionHandler
    case An_Enumeration_Literal_Specification:     // 3.5.1(3)
        // handled in EnumElementCreator
    case A_Discriminant_Specification:             // 3.7(5)   -> Trait_Kinds
        // handled in getDiscriminant
        ADA_ASSERT(false);
        break;

    case Not_A_Declaration:                        // An unexpected element
    case A_Generalized_Iterator_Specification:     // 5.5.2    -> Trait_Kinds (Ada 2012)
    case An_Element_Iterator_Specification:        // 5.5.2    -> Trait_Kinds (Ada 2012)
    case A_Return_Variable_Specification:          // 6.5 (Ada 2005)
    case A_Return_Constant_Specification:          // 6.5 (Ada 2005)
    case A_Formal_Incomplete_Type_Declaration:     // (Ada 2012)
    default:
      logWarn() << "unhandled declaration kind: " << decl.Declaration_Kind << std::endl;
      ADA_ASSERT (!FAIL_ON_ERROR(ctx));
  }

  processAspects(elem, decl, assocdecl, ctx);
  recordPragmasID(std::move(pragmaVector), assocdecl, ctx);
}

void ParameterCompletion::operator()(SgFunctionParameterList& lst, SgScopeStatement& parmscope)
{
  traverseIDs(range, elemMap(), ParmlistCreator{lst, ctx.scope(parmscope)});
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
  logKind("A_Defining_Name", elem.ID);

  Defining_Name_Struct& asisname = elem.The_Union.Defining_Name;
  SgScopeStatement*     parent = &ctx.scope();
  std::string           ident{asisname.Defining_Name_Image};
  std::string           name{ident};

  switch (asisname.Defining_Name_Kind)
  {
    case A_Defining_Expanded_Name:
      {
        logKind("A_Defining_Expanded_Name", elem.ID);

        NameData        defname    = getNameID(asisname.Defining_Selector, ctx);

        ident  = defname.ident;
        parent = &queryScopeOfID(asisname.Defining_Prefix, ctx);
        break;
      }

    case A_Defining_Identifier:
      {
        logKind("A_Defining_Identifier", elem.ID);

        //~ logTrace() << "* " << ident << std::endl;
        // nothing to do, the fields are already set
        break;
      }

    case A_Defining_Operator_Symbol:     // 6.1(9)
      {
        logKind("A_Defining_Operator_Symbol", elem.ID);

        ADA_ASSERT (ident.size() > 2);
        name = ident = si::Ada::roseOperatorPrefix + ident.substr(1, ident.size() - 2);

        // nothing to do, the fields are already set

        /* unused field:
             enum Operator_Kinds       Operator_Kind
        */
        break;
      }

    case A_Defining_Enumeration_Literal: // 3.5.1(3)
      {
        logKind("A_Defining_Enumeration_Literal", elem.ID);

        /* unused fields:
             char* Position_Number_Image;      // \pp implied by name sequence
             char* Representation_Value_Image; // \pp not in code, but could be defined by representation clause
        */
        break;
      }

    case A_Defining_Character_Literal:   // 3.5.1(4)
      {
        logKind("A_Defining_Enumeration_Literal", elem.ID);

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
    logKind("An_Identifier", elem.ID);

    std::string ident{idex.Name_Image};

    return NameData{ ident, ident, ctx.scope(), elem };
  }

  if (idex.Expression_Kind == An_Operator_Symbol)
  {
    logKind("An_Operator_Symbol", elem.ID);

    std::string ident{idex.Name_Image};

    return NameData{ ident, ident, ctx.scope(), elem };
  }

  if (idex.Expression_Kind == An_Attribute_Reference)
  {
    NameData prefix = getNameID(idex.Prefix, ctx);
    NameData attr   = getNameID(idex.Attribute_Designator_Identifier, ctx);

    return NameData{ prefix.ident, prefix.fullName + "'" + attr.fullName, ctx.scope(), prefix.elem() };
  }

  if (idex.Expression_Kind != A_Selected_Component)
    logError() << "Unexpected Expression_kind [getQualName]: " << idex.Expression_Kind
               << ( elem.Source_Location.Unit_Name ? elem.Source_Location.Unit_Name : "" )
               << " @" << elem.Source_Location.First_Line << ":" << elem.Source_Location.First_Column
               << std::endl;

  ADA_ASSERT (idex.Expression_Kind == A_Selected_Component);
  logKind("A_Selected_Component", elem.ID);

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
  return getName(retrieveElem(elemMap(), el), ctx);
}

void
processInheritedSubroutines( SgNamedType& derivedType,
                             ElemIdRange subprograms,
                             ElemIdRange declarations,
                             AstContext ctx
                           )
{
  // use a deferred unit completion, so that all information of records
  //   (i.e., detailed inheritance relationship) have been seen when
  //   the inherited subroutines are processed.
  auto deferredSubRoutineProcessing =
       [&derivedType, subprograms, declarations, ctx]() -> void
       {
         SgType*      baseType     = si::Ada::baseType(derivedType);
         SgType*      baseRootRaw  = si::Ada::typeRoot(baseType).typerep();
         SgNamedType* baseRootType = isSgNamedType(baseRootRaw);

         if (baseRootType == nullptr)
         {
           // if baseRootRaw != nullptr
           //   it will correspond to a universal type in package standard.
           //   -> not an error
           if (baseRootRaw == nullptr)
             logFlaw() << "unable to find any base-root for " << derivedType.get_name()
                       << " / base = " << baseType
                       << std::endl;
           return;
         }

         //~ logWarn() << "drv: " << derivedType.get_name() << " / " << baseRootType->get_name()
                   //~ << std::endl;
         traverseIDs(subprograms, elemMap(), InheritedSymbolCreator{*baseRootType, derivedType, ctx});

         if (!declarations.empty())
         {
           // fields will be added later during the fixup pass: FixupAstSymbolTablesToSupportAliasedSymbols
           // \todo also implement fixup for discriminants and enumerations..
           logInfo() << "A derived/extension record type's implicit declaration is not empty: "
                     << derivedType.get_name()
                     << std::endl;
         }
       };

  ctx.storeDeferredUnitCompletion(std::move(deferredSubRoutineProcessing));
}

void processAndPlacePragmas(Pragma_Element_ID_List pragmalst, std::vector<SgScopeStatement*> scopes, AstContext ctx)
{
  // append all pragmas in pragmalst to the unprocessed pragma list
  ElemIdRange             pragmaRange  = idRange(pragmalst);
  std::vector<Element_ID> pragmaVector;

  std::copy(pragmaRange.first, pragmaRange.second, std::back_inserter(pragmaVector));
  recordPragmasID(std::move(pragmaVector), nullptr /* no available statement */, ctx);

  placePragmas(std::move(scopes), ctx);
}


}
