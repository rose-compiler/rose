/*
 * Contains functions that unparse statements
 *
 * FORMATTING WILL BE DONE IN TWO WAYS:
 * 1. using the file_info object to get information from line and column number
 *    (for original source code)
 * 2. following a specified format that I have specified with indentations of
 *    length TABINDENT (for transformations)
 *
 * REMEMBER: For types and symbols, we still call the original unparse function
 * defined in sage since they dont have file_info.
 */
#include "sage3basic.h"
#include "unparser.h"
#include "Rose/Diagnostics.h"

#include <boost/algorithm/string/case_conv.hpp>

#define ROSE_TRACK_PROGRESS_OF_ROSE_COMPILING_ROSE 0

#include "sage_support.h"
#include "sageGeneric.h"
#include "sageInterfaceAda.h"

namespace si = SageInterface;

Unparse_Ada::Unparse_Ada(Unparser* baseUnparser, std::string fname)
: UnparseLanguageIndependentConstructs(baseUnparser, fname),
  //~ visible_scopes(),
  //~ use_scopes(),
  //~ renamed_scopes(),
  //~ scope_state(1, ScopeStackEntry{}),
  currentNameQualificationMap(&SgNode::get_globalQualifiedNameMapForNames()),
  oldLineWrap(baseUnparser->cur.get_linewrap())
{
  static constexpr int MAX_GNAT_LINE_LENGTH    = (1<<15)-1;
  static constexpr int DEFAULT_MAX_LINE_LENGTH = 132;

  // if unparsing generates a file, set max line wrapping if not set otherwise
  if (fname.size() && ((oldLineWrap <= 0) || (oldLineWrap > MAX_GNAT_LINE_LENGTH)))
    unp->cur.set_linewrap(DEFAULT_MAX_LINE_LENGTH);
}

Unparse_Ada::~Unparse_Ada()
{
  unp->cur.set_linewrap(oldLineWrap);
}

void
Unparse_Ada::unparseAdaFile(SgSourceFile* sourcefile, SgUnparse_Info& info)
{
  SgGlobal* globalScope = sourcefile->get_globalScope();
  ASSERT_not_null(globalScope);

  unparseStatement(globalScope, info);
}


namespace
{
  inline
  auto logTrace() -> decltype(Rose::Diagnostics::mlog[Sawyer::Message::TRACE])
  {
    return Rose::Diagnostics::mlog[Sawyer::Message::TRACE];
  }

  inline
  auto logInfo() -> decltype(Rose::Diagnostics::mlog[Sawyer::Message::INFO])
  {
    return Rose::Diagnostics::mlog[Sawyer::Message::INFO];
  }

  inline
  auto logWarn() -> decltype(Rose::Diagnostics::mlog[Sawyer::Message::WARN])
  {
    return Rose::Diagnostics::mlog[Sawyer::Message::WARN];
  }

  inline
  auto logError() -> decltype(Rose::Diagnostics::mlog[Sawyer::Message::ERROR])
  {
    return Rose::Diagnostics::mlog[Sawyer::Message::ERROR];
  }

  inline
  auto logFatal() -> decltype(Rose::Diagnostics::mlog[Sawyer::Message::FATAL])
  {
    return Rose::Diagnostics::mlog[Sawyer::Message::FATAL];
  }

  /// returns m[key] iff \ref key is in \ref m
  ///                otherwise \ref defaultVal
  template <class Map>
  const typename Map::mapped_type&
  getQualMapping( const Map& m,
                  const typename Map::key_type& key,
                  const typename Map::mapped_type& defaultVal
                )
  {
    auto pos = m.find(key);

    return (pos == m.end()) ? defaultVal : pos->second;
  }

  SgInitializedName& onlyVariable(SgVariableDeclaration& n)
  {
    SgInitializedNamePtrList& lst = n.get_variables();

    ROSE_ASSERT(lst.size() == 1);
    return SG_DEREF(lst[0]);
  }

  const std::string NO_SEP = "";
  const std::string COMMA_SEP = ", ";
  const std::string STMT_SEP = ";\n";
  const std::string PARAM_SEP = "; ";

  template <class UnparserT>
  void unparseTypeModifiers(UnparserT& unparser, SgTypeModifier tymod)
  {
    if (tymod.isIntent_out())   unparser.prn(" out");
    if (tymod.isIntent_in())    unparser.prn(" in");
    if (tymod.isIntent_inout()) unparser.prn(" in out");
  }

  template <class UnparserT>
  void unparseModifiers(UnparserT& unparser, SgDeclarationStatement& n)
  {
    unparseTypeModifiers(unparser, n.get_declarationModifier().get_typeModifier());
  }

  struct AdaDetailsUnparser
  {
    AdaDetailsUnparser(Unparse_Ada& unp, SgUnparse_Info& inf, std::ostream& outp)
    : unparser(unp), info(inf), os(outp), sep(&NO_SEP)
    {}

    void prn(const std::string& s)
    {
      unparser.curprint(s);
      //~ os << s;
    }

    Unparse_Ada&       unparser;
    SgUnparse_Info&    info;
    std::ostream&      os;
    const std::string* sep;
  };

  struct AdaEnumeratorUnparser : AdaDetailsUnparser
  {
    using AdaDetailsUnparser::AdaDetailsUnparser;

    void operator()(SgInitializedName* enumerator)
    {
      ROSE_ASSERT(enumerator);

      prn(*sep);
      prn(enumerator->get_name());

      sep = &COMMA_SEP;
    }
  };

  struct AdaParamUnparser : AdaDetailsUnparser
  {
    using AdaDetailsUnparser::AdaDetailsUnparser;

    void handle(SgVariableDeclaration& n)
    {
      SgInitializedNamePtrList& params = n.get_variables();
      ROSE_ASSERT(params.size());

      SgInitializedName&        primary  = SG_DEREF(params[0]);

      prn(primary.get_name());

      for (size_t i = 1; i < params.size(); ++i)
      {
        SgInitializedName& secondary = SG_DEREF(params[i]);

        prn(COMMA_SEP);
        prn(secondary.get_name());
      }

      prn(": ");
      unparseModifiers(*this, n);

      unparser.unparseType(primary, primary.get_type(), info);

      if (SgInitializer* init = primary.get_initializer())
      {
        prn(" := ");
        unparser.unparseExpression(init, info);
      }
    }

    void operator()(SgVariableDeclaration* s)
    {
      prn(*sep);
      handle(SG_DEREF(s));
      sep = &PARAM_SEP;
    }
  };


  SgVariableDeclaration* asVariableDeclaration(SgDeclarationStatement* dcl)
  {
    ASSERT_not_null(dcl);
    SgVariableDeclaration* var = isSgVariableDeclaration(dcl);

    ASSERT_not_null(var);
    return var;
  }


  SgVariableDeclaration* variableDeclaration(SgInitializedName* ini)
  {
    ASSERT_not_null(ini);
    SgVariableDeclaration* var = isSgVariableDeclaration(ini->get_declptr());

    ASSERT_not_null(var);
    return var;
  }

  struct LoopLabelQuery : sg::DispatchHandler<std::string>
  {
    void handle(SgNode& n)         { SG_UNEXPECTED_NODE(n);      }

    void handle(SgForStatement& n) { res = n.get_string_label(); }
    void handle(SgAdaLoopStmt& n)  { res = n.get_string_label(); }
    void handle(SgWhileStmt& n)    { res = n.get_string_label(); }
    //~ void handle(SgBasicBlock& n)   { res = n.get_string_label(); }
  };

  struct LabelSyntax : sg::DispatchHandler<std::pair<std::string, std::string> >
  {
    using base = sg::DispatchHandler<std::pair<std::string, std::string> >;

    explicit
    LabelSyntax(const std::string& labelname)
    : base(), lbl(labelname)
    {}

    void handle(SgNode& n)           { SG_UNEXPECTED_NODE(n); }

    ReturnType stmtSyntax() const;
    ReturnType blockSyntax(const std::string& blklbl) const;

    void handle(SgStatement& n)      { res = stmtSyntax(); }
    void handle(SgForStatement& n)   { res = blockSyntax(n.get_string_label());  }
    void handle(SgAdaLoopStmt& n)    { res = blockSyntax(n.get_string_label());  }
    void handle(SgWhileStmt& n)      { res = blockSyntax(n.get_string_label());  }
    void handle(SgBasicBlock& n)     { res = blockSyntax(n.get_string_label());  }

    const std::string& lbl;
  };

  LabelSyntax::ReturnType
  LabelSyntax::blockSyntax(const std::string& blklbl) const
  {
    if (lbl != blklbl)
      return stmtSyntax();

    return std::make_pair(std::string(""), std::string(": "));
  }

  LabelSyntax::ReturnType
  LabelSyntax::stmtSyntax() const
  {
    return std::make_pair(std::string("<<"), std::string(">> "));
  }

  struct RenamingSyntaxResult : std::tuple<std::string, bool, const SgScopeStatement*>
  {
      using base = std::tuple<std::string, bool, const SgScopeStatement*>;
      using base::base;

      const std::string&      prefixSyntax() const { return std::get<0>(*this); }
      bool                    withType()     const { return std::get<1>(*this); }
      const SgScopeStatement* body()         const { return std::get<2>(*this); }
      //~ const std::string&      renamedName()  const { return std::get<3>(*this); }

      // mutators
      //~ std::string& renamedName() { return std::get<3>(*this); }
  };

  const SgScopeStatement*
  unitDefinition(const SgDeclarationStatement& n);

  struct UnitDefinition : sg::DispatchHandler<const SgScopeStatement*>
  {
      void handle(const SgNode& n) { SG_UNEXPECTED_NODE(n); }

      void handle(const SgFunctionDeclaration& n)
      {
        if (const SgFunctionDeclaration* def = isSgFunctionDeclaration(n.get_definingDeclaration()))
        {
          res = def->get_definition();
        }
      }

      void handle(const SgAdaPackageSpecDecl& n)
      {
        res = n.get_definition();
      }

      void handle(const SgAdaGenericDecl& n)
      {
        res = unitDefinition(SG_DEREF(n.get_declaration()));
      }

      void handle(const SgAdaGenericInstanceDecl&)
      {
        // should return the instantiated scope
        //   while the Asis part is incomplete,
        //   => return nothing for now
      }

      void handle(const SgImportStatement&)
      {
        // should not happen with a full Ada implemention
        //   SgImportStatement is used when the package is not available
        //   => do nothing for now
      }
  };

  const SgScopeStatement*
  unitDefinition(const SgDeclarationStatement& n)
  {
    return sg::dispatch(UnitDefinition{}, &n);
  }

  struct UseClauseSyntaxResult : std::tuple<std::string, std::string, const SgDeclarationStatement*>
  {
    using base = std::tuple<std::string, std::string, const SgDeclarationStatement*>;
    using base::base;

    const std::string& keyword()         const { return std::get<0>(*this); }
    const std::string& name()            const { return std::get<1>(*this); }
    const SgDeclarationStatement& decl() const { return SG_DEREF(std::get<2>(*this)); }
  };

  struct UseClauseSyntax : sg::DispatchHandler<UseClauseSyntaxResult>
  {
    void usepkg(const std::string& s, const SgDeclarationStatement& dcl)
    {
      res = ReturnType{"", s, &dcl};
    }

    void usetype(const std::string& s, const SgDeclarationStatement& dcl)
    {
      res = ReturnType{"type ", s, &dcl};
    }

    void handle(const SgNode& n)                   { SG_UNEXPECTED_NODE(n); }
    void handle(const SgAdaPackageSpecDecl& n)     { usepkg(n.get_name(), n); }
    void handle(const SgAdaPackageBodyDecl& n)     { usepkg(n.get_name(), n); }
    void handle(const SgAdaRenamingDecl& n)        { usepkg(n.get_name(), n); }
    void handle(const SgAdaGenericInstanceDecl& n) { usepkg(n.get_name(), n); }
    void handle(const SgAdaFormalPackageDecl& n)   { usepkg(n.get_name(), n); }
    void handle(const SgTypedefDeclaration& n)     { usetype(n.get_name(), n); }
    void handle(const SgAdaTaskTypeDecl& n)        { usetype(n.get_name(), n); }
    void handle(const SgAdaProtectedTypeDecl& n)   { usetype(n.get_name(), n); }
    void handle(const SgClassDeclaration& n)       { usetype(n.get_name(), n); }
    void handle(const SgEnumDeclaration& n)        { usetype(n.get_name(), n); }

    void handle(const SgImportStatement& n)
    {
      si::Ada::ImportedUnitResult imported = si::Ada::importedUnit(n);

      usepkg(imported.name(), imported.decl());
    }
  };

  UseClauseSyntaxResult
  useClauseSyntax(SgDeclarationStatement* n)
  {
    return sg::dispatch(UseClauseSyntax{}, n);
  }

  struct AdaStatementUnparser : AdaDetailsUnparser
  {
    AdaStatementUnparser(Unparse_Ada& unp, SgUnparse_Info& inf, std::ostream& outp)
    : AdaDetailsUnparser(unp, inf, outp), publicMode(true), pendingDiscriminants(nullptr)
    {}

    //
    // name qualification

    /// returns the name qualification recorded for scope \ref n in map \ref qualMap
    /// \todo use const std::string& as return type as soon as the old name qualification
    ///       has been phased out.
    std::string
    getQualification(const std::map<SgNode*, std::string>& qualMap, const SgNode& n, SgScopeStatement* scope);

    std::string
    getQualification(const SgNode& n, SgScopeStatement* scope);

    /// prints name qualification
    void prnNameQual(const SgNode& n, SgScopeStatement* scope);

    /// prints name qualification with an anchor point \ref ref
    void prnNameQual(const SgLocatedNode& ref, const SgNode& n, SgScopeStatement* scope);

    /// prints name qualification using separate syntax (i.e., separate(name.qualification)).
    /// \{
    void prnSeparateQual(const std::string& qual);
    void prnSeparateQual(const SgDeclarationStatement& n, SgScopeStatement* scope);
    /// \}

    //
    // sequences

    template <class SageStmtList>
    void list(SageStmtList& lst, bool hasPrivateSection = false);

    template <class ForwardIterator>
    void list(ForwardIterator aa, ForwardIterator zz, bool hasPrivateSection = false);

    //
    // SageNode handling

    void handleBasicBlock(SgBasicBlock& n, bool functionbody = false);

    void handleBasicBlock(SgBasicBlock* n, bool functionbody = false)
    {
      handleBasicBlock(SG_DEREF(n), functionbody);
    }

    void handleStringLabel(const std::string& s);

    void handleFunctionEntryDecl(SgFunctionDeclaration&, std::string keyword, bool hasReturn = false);

    void handleParameterList(const SgInitializedNamePtrList& params);
    void handleParameterList(const SgDeclarationStatementPtrList& params);

    static
    std::string
    typedeclSyntax(SgType* n);

    static
    RenamingSyntaxResult
    renamingSyntax(SgExpression* n);

    void startPrivateIfNeeded(SgDeclarationStatement* n);

    void modifiers(SgDeclarationStatement& n);
    bool hasModifiers(SgDeclarationStatement& n);

    // clones the unparser and sets the discriminant link
    AdaStatementUnparser
    unparserWith(SgAdaDiscriminatedTypeDecl* n);

    //
    // handlers

    void handle(SgNode& n)      { SG_UNEXPECTED_NODE(n); }

    void handle(SgStatement& n);

    void handle(SgGlobal& n)
    {
      //~ ScopeUpdateGuard        scopeGuard{unparser, info, n};
      si::Ada::StatementRange pkgRange = si::Ada::declsInPackage(n, unparser.getFileName());

      list(pkgRange.first, pkgRange.second);
    }

    void handle(SgNullStatement& n)
    {
      prn("null");
      prn(STMT_SEP);
    }

    void handle(SgAdaSelectStmt& n)
    {
      switch (n.get_select_type()) {
      case SgAdaSelectStmt::e_selective_accept:
        // 9.7.1
        prn("select\n");
        stmt(n.get_select_path());
        if (n.get_or_path() != NULL) {
          prn("or\n");
          stmt(n.get_or_path());
        }
        if (n.get_else_path() != NULL) {
          prn("else\n");
          stmt(n.get_else_path());
        }
        prn("end select");
        prn(STMT_SEP);
        break;
      case SgAdaSelectStmt::e_asynchronous:
        // 9.7.4
        prn("select\n");
        stmt(n.get_select_path());
        prn("then abort\n");
        stmt(n.get_abort_path());
        prn("end select");
        prn(STMT_SEP);
        break;
      case SgAdaSelectStmt::e_conditional_entry:
        // 9.7.3
        prn("select\n");
        stmt(n.get_select_path());
        prn("else\n");
        stmt(n.get_else_path());
        prn("end select");
        prn(STMT_SEP);
        break;
      case SgAdaSelectStmt::e_timed_entry:
        // 9.7.2
        prn("select\n");
        stmt(n.get_select_path());
        prn("or\n");
        stmt(n.get_or_path());
        prn("end select");
        prn(STMT_SEP);
        break;
      default:
        ROSE_ABORT();
      }
    }

    void handle(SgAdaSelectAlternativeStmt& n)
    {
      expr_opt(n.get_guard(), "when ", " =>\n");
      stmt(n.get_body());
      if (n.get_next() != NULL) {
        prn("or\n");
        stmt(n.get_next());
      }
    }

    void handle(SgAdaTaskTypeDecl& n)
    {
      if (SgAdaTaskSpec* spec = n.get_definition())
      {
      prn("task type ");
      prn(n.get_name());
        if (!spec->get_hasMembers())
      {
        prn(STMT_SEP);
        return;
      }

      prn(" is\n");
        stmt(spec);

      prn("end ");
      prn(n.get_name());
      }
      else
      {
        // forward declaration
        // \todo refactor code into a single location to handle these
        prn("type ");
        prn(n.get_name());

        const bool requiresPrivate = si::Ada::withPrivateDefinition(&n);
        const bool requiresIs      = requiresPrivate || hasModifiers(n);

        if (requiresIs)
          prn(" is");

        modifiers(n);

        if (requiresPrivate)
          prn(" private");
      }

      prn(STMT_SEP);
    }

    void handle(SgAdaProtectedTypeDecl& n)
    {

      if (SgAdaProtectedSpec* spec = n.get_definition())
      {
      prn("protected type ");
      prn(n.get_name());
      prn(" is\n");
        stmt(spec);

      prn("end ");
      prn(n.get_name());
      }
      else
      {
        // forward declaration
        // \todo refactor code into a single location to handle these
        prn("type ");
        prn(n.get_name());

        const bool requiresPrivate = si::Ada::withPrivateDefinition(&n);
        const bool requiresIs      = requiresPrivate || hasModifiers(n);

        if (requiresIs)
          prn(" is");

        modifiers(n);

        if (requiresPrivate)
          prn(" private");
      }

      prn(STMT_SEP);
    }

    template <class SageAdaConcurrentBodyDecl>
    void handleConcurrentBodyDecl(SageAdaConcurrentBodyDecl& n, const std::string& prefix)
    {
      prnSeparateQual(n, n.get_scope());

      prn(prefix);
      prn(n.get_name());

      if (si::Ada::hasSeparatedBody(n))
      {
        // separate declarations are nondefining
        prn(" is separate");
        prn(STMT_SEP);
        return;
      }

      prn(" is\n");
      stmt(n.get_definition());

      prn(" ");
      prn(n.get_name());
      prn(STMT_SEP);
    }

    void handle(SgAdaTaskBodyDecl& n) { handleConcurrentBodyDecl(n, "task body "); }
    void handle(SgAdaProtectedBodyDecl& n) { handleConcurrentBodyDecl(n, "protected body "); }

    void handle(SgAdaTaskSpecDecl& n)
    {
      prn("task ");
      prn(n.get_name());

      SgAdaTaskSpec& spec = SG_DEREF(n.get_definition());

      if (!spec.get_hasMembers())
      {
        prn(STMT_SEP);
        return;
      }

      prn(" is\n");
      stmt(&spec);

      prn("end ");
      prn(n.get_name());
      prn(STMT_SEP);
    }

    void handle(SgAdaProtectedSpecDecl& n)
    {
      prn("protected ");
      prn(n.get_name());

      SgAdaProtectedSpec& spec = SG_DEREF(n.get_definition());

      prn(" is\n");
      stmt(&spec);

      prn("end ");
      prn(n.get_name());
      prn(STMT_SEP);
    }

    void handle(SgAdaDiscriminatedTypeDecl& n)
    {
      // unparses the discriminated declaration, which is responsible for
      //   printing the discriminants
      stmt(n.get_discriminatedDecl(), &n);
    }

    void handle(SgAdaTaskSpec& n)
    {
      //~ ScopeUpdateGuard scopeGuard{unparser, info, n};

      list(n.get_declarations(), n.get_hasPrivate());
    }

    void handle(SgAdaProtectedSpec& n)
    {
      //~ ScopeUpdateGuard scopeGuard{unparser, info, n};

      list(n.get_declarations(), n.get_hasPrivate());
    }

    void handle(SgAdaTaskBody& n)
    {
      //~ ScopeUpdateGuard scopeGuard{unparser, info, n};

      SgStatementPtrList&          stmts    = n.get_statements();
      SgStatementPtrList::iterator aa       = stmts.begin();
      SgStatementPtrList::iterator zz       = stmts.end();
      SgStatementPtrList::iterator dcllimit = si::Ada::declarationLimit(stmts);

      list(aa, dcllimit);

      prn("begin\n");
      list(dcllimit, zz);
      prn("end"); // omit newline, which will be added by the parent
    }

    void handle(SgAdaProtectedBody& n)
    {
      //~ ScopeUpdateGuard scopeGuard{unparser, info, n};

      list(n.get_statements());
      prn("end"); // omit newline, which will be added by the parent
    }

    void handle(SgAdaPackageSpecDecl& n)
    {
      const std::string& pkgqual = getQualification(n, n.get_scope());

      prn("package ");
      prn(pkgqual);
      prn(n.get_name());
      prn(" is\n");

      stmt(n.get_definition());

      prn("end ");
      prn(pkgqual);
      prn(n.get_name());
      prn(STMT_SEP);
    }

    void handle(SgAdaPackageBodyDecl& n)
    {
      const bool         separated = si::Ada::isSeparatedBody(n);
      const std::string& pkgqual   = getQualification(n, n.get_scope());

      if (separated) prnSeparateQual(pkgqual);
      prn("package body ");
      if (!separated) prn(pkgqual);
      prn(n.get_name());

      if (si::Ada::hasSeparatedBody(n))
      {
        // separate declarations are nondefining
        prn(" is separate");
        prn(STMT_SEP);
        return;
      }

      prn(" is\n");
      stmt(n.get_definition());

      prn("end ");
      if (!separated) prn(pkgqual);
      prn(n.get_name());
      prn(STMT_SEP);
    }

    void handle(SgAdaPackageSpec& n)
    {
      //~ ScopeUpdateGuard scopeGuard{unparser, info, n};

      list(n.get_declarations(), n.get_hasPrivate());
    }

    void handle(SgAdaPackageBody& n)
    {
      using Iterator = SgStatementPtrList::iterator;

      //~ ScopeUpdateGuard    scopeGuard(unparser, info, n);
      SgStatementPtrList& stmts = n.get_statements();
      SgBasicBlock*       block = nullptr;
      SgTryStmt*          trystmt = nullptr;
      Iterator            zz = stmts.end();

      if (stmts.size())
      {
        block = isSgBasicBlock(stmts.back());
        trystmt = isSgTryStmt(stmts.back());
      }

      if (block || trystmt) --zz;

      list(stmts.begin(), zz);

      if (block)
      {
        prn("begin\n");
        list(block->get_statements());
        // the block's end is printed in the parent
      }
      else if (trystmt)
      {
        prn("begin\n");
        stmt(trystmt);
      }
    }

    void handle(SgAdaGenericInstanceDecl& n)
    {
      SgName                  name    = n.get_name();
      SgExprListExp*          args    = n.get_actual_parameters();
      SgDeclarationStatement* basedcl = n.get_declaration();
      const std::string&      pkgqual = getQualification(n, n.get_scope());

      if (SgAdaGenericDecl* gendcl = isSgAdaGenericDecl(basedcl))
        basedcl = gendcl->get_declaration();

      // determine which kind of generic instance this is
      if (SgAdaPackageSpecDecl* pkg = isSgAdaPackageSpecDecl(basedcl)) {
        // package
        prn("package ");
        prn(pkgqual);
        prn(name.getString());
        prn(" is new ");
        prnNameQual(n, *pkg, pkg->get_scope());
        prn(pkg->get_name().getString());
      } else if (SgAdaRenamingDecl* ren = isSgAdaRenamingDecl(basedcl)) {
        // renamed package
        prn("package ");
        prn(pkgqual);
        prn(name.getString());
        prn(" is new ");
        prnNameQual(n, *ren, ren->get_scope());
        prn(ren->get_name().getString());
      } else if (SgFunctionDeclaration* fn = isSgFunctionDeclaration(basedcl)) {
        // function/procedure
        prn(si::Ada::isFunction(fn->get_type()) ? "function " : "procedure ");
        prn(pkgqual);
        prn(si::Ada::convertRoseOperatorNameToAdaName(name));
        prn(" is new ");
        prnNameQual(n, *fn, fn->get_scope());
        prn(fn->get_name().getString());
      }
      else
      {
        ROSE_ABORT();
        // renamed generic function?
      }

      associationList(SG_DEREF(args));
      prn(STMT_SEP);
    }

    void handle(SgAdaRenamingDecl& n)
    {
      SgExpression*        orig     = n.get_renamed();
      RenamingSyntaxResult renamed  = renamingSyntax(orig);
      std::string          newName  = n.get_name();

      prn(renamed.prefixSyntax());
      prn(newName);

      if (renamed.withType())
      {
        prn(": ");
        type(n, n.get_type());
      }

      prn(" renames ");
      //~ prnNameQual(n, orig->get_scope());
      expr(orig);
      prn(STMT_SEP);

      //~ if (renamed.body())
        //~ unparser.addRenamedScope(renamed.body(), &n);
    }

    void handle(SgUsingDeclarationStatement& n)
    {
      UseClauseSyntaxResult useSyntax = useClauseSyntax(n.get_declaration());
      SgScopeStatement*     origScope = useSyntax.decl().get_scope();

      prn("use ");
      prn(useSyntax.keyword());
      prnNameQual(n, origScope);
      prn(useSyntax.name());
      prn(STMT_SEP);
    }

    void handle(SgTypedefDeclaration& n)
    {
      SgType* basety = n.get_base_type();

      prn(typedeclSyntax(basety));
      prn(" ");
      prn(n.get_name());

      printPendingDiscriminants();

      const bool isDefinition    = &n == n.get_definingDeclaration();
      const bool requiresPrivate = (!isDefinition) && si::Ada::withPrivateDefinition(&n);
      const bool requiresIs      = (  requiresPrivate
                                   || hasModifiers(n)
                                   //~ || declwords.second.size() != 0
                                   || !isSgTypeDefault(basety)
                                   );

      if (requiresIs)
        prn(" is");

      modifiers(n);
      prn(" ");
      type(n, basety);

      if (requiresPrivate)
        prn(" private");

      prn(STMT_SEP);
    }

    void handle(SgVariableDeclaration& n)
    {
      SgInitializedNamePtrList& vars = n.get_variables();
      ROSE_ASSERT(vars.size());

      SgInitializedName&        primary  = SG_DEREF(vars[0]);

      prn(primary.get_name());

      for (size_t i = 1; i < vars.size(); ++i)
      {
        SgInitializedName& secondary = SG_DEREF(vars[i]);

        prn(COMMA_SEP);
        prn(secondary.get_name());
      }

      prn(": ");
      unparseModifiers(*this, n);

      type(primary, primary.get_type());

      expr_opt(primary.get_initializer(), " := ");
      prn(STMT_SEP);
    }

    void handle(SgAdaVariantDecl& n)
    {
      prn("case ");
      expr(n.get_discriminant());
      prn(" is\n");

      list(SG_DEREF(n.get_variants()).get_statements());

      prn("end case");
      prn(STMT_SEP);
    }

    void handle(SgAdaVariantWhenStmt& n)
    {
      prn("when ");
      choicelst(n.get_choices());
      prn(" =>\n");
      list(SG_DEREF(n.get_components()).get_statements());
    }

    void handle(SgFunctionDefinition& n)
    {
      //~ ScopeUpdateGuard scopeGuard{unparser, info, n};

      handleBasicBlock(n.get_body(), true /* function body */);
    }

    void handle(SgAdaGenericDefn& n)
    {
      SgDeclarationStatementPtrList& decls = n.get_declarations();
      list(decls);
    }

    SgType&
    processUnknownDiscriminantPart(SgType* ty)
    {
      if (SgAdaSubtype* sub = isSgAdaSubtype(ty))
      {
        if (SgAdaDiscriminantConstraint* constr = isSgAdaDiscriminantConstraint(sub->get_constraint()))
        {
          ROSE_ASSERT(constr->get_discriminants().size() == 0);
          prn("(<>)");

          ty = sub->get_base_type();
        }
      }

      return SG_DEREF(ty);
    }

    void handle(SgAdaFormalTypeDecl& n)
    {
      prn("type ");
      prn(n.get_name());

      SgAdaFormalType* ty = n.get_type();
      ASSERT_not_null(ty);

      SgType* formalBase = ty->get_formal_type();
      ASSERT_not_null(formalBase);

      formalBase = &processUnknownDiscriminantPart(formalBase);

      prn(" is ");
      modifiers(n);

      // \todo what types need to be printed?
      //       print all non-null ty->get_formal_type() ?
      if (si::Ada::isModularType(formalBase))
        prn("mod <>");
      else if (si::Ada::isIntegerType(formalBase))
        prn("range <>");
      else if (si::Ada::isFloatingPointType(formalBase))
        prn("digits <>");
      else if (si::Ada::isDiscreteType(formalBase))
        prn("(<>)");
      else if (si::Ada::isDecimalFixedType(formalBase))
        prn(" delta<> digits<> ");
      else if (si::Ada::isFixedType(formalBase))
        prn(" delta<> ");
      else if (!isSgTypeDefault(formalBase))
      {
        //~ const bool withRequired = !isSgAdaAccessType(formalBase) && ty->get_is_private();
        const bool withRequired = ty->get_is_private();

        type(n, formalBase);

        if (withRequired) prn(" with");
      }

      if (ty->get_is_private()) {
        prn(" private");
      }
      prn(";\n");
    }

    void handle(SgAdaFormalPackageDecl& n)
    {
      SgDeclarationStatement* basedcl = n.get_declaration();

      prn("with package ");
      prn(n.get_name());
      prn(" is new ");

      if (SgAdaGenericDecl* gendcl = isSgAdaGenericDecl(basedcl))
        basedcl = gendcl->get_declaration();

      // determine which kind of generic instance this is
      if (SgAdaPackageSpecDecl* pkg = isSgAdaPackageSpecDecl(basedcl)) {
        // package
        prnNameQual(n, *pkg, pkg->get_scope());
        prn(pkg->get_name().getString());
      } else if (SgAdaRenamingDecl* ren = isSgAdaRenamingDecl(basedcl)) {
        // renamed package
        prnNameQual(n, *ren, ren->get_scope());
        prn(ren->get_name().getString());
      }

      prn("(");
      expr(n.get_actual_parameters());
      prn(")");
      prn(STMT_SEP);
    }


    void handle(SgAdaGenericDecl& n)
    {
      prn("generic\n");
      stmt(n.get_definition());

      SgDeclarationStatement* dcl = n.get_declaration();

      // check which kind of generic we have:
      ROSE_ASSERT(isSgAdaPackageSpecDecl(dcl) || isSgFunctionDeclaration(dcl));
      stmt(dcl);
    }

    void prnIfBranch(const si::Ada::IfStatementInfo& branch, const std::string& cond)
    {
      prn(cond);
      expr(branch.condition());
      prn(" then ");
      stmt(branch.trueBranch());
    }


    void handle(SgIfStmt& n)
    {
      using Iterator = std::vector<si::Ada::IfStatementInfo>::iterator;

      std::vector<si::Ada::IfStatementInfo> seq = si::Ada::flattenIfStatements(n);
      Iterator                              aa = seq.begin();
      const Iterator                        zz = seq.end();

      ROSE_ASSERT(aa != zz);
      prnIfBranch(*aa, "if ");

      ++aa;
      while ((aa != zz) && (!aa->isElse()))
      {
        prnIfBranch(*aa, "elsif ");
        ++aa;
      }

      if (aa != zz)
      {
        prn("else\n");
        stmt(aa->trueBranch());
        ++aa;
      }

      ROSE_ASSERT(aa == zz);
      prn("end if");
      prn(STMT_SEP);
    }

    void handle(SgGotoStatement& n)
    {
      prn("goto ");
      prn(SG_DEREF(n.get_label()).get_name());
      prn(STMT_SEP);
    }

    void handle(SgWhileStmt& n)
    {
      prn("while ");
      expr(si::Ada::underlyingExpr(n.get_condition()));
      prn(" loop\n");
      stmt(n.get_body());
      prn("end loop");
      handleStringLabel(n.get_string_label());
      prn(STMT_SEP);
    }

    void forInitStmt(SgForInitStatement& n, bool reverse)
    {
      SgStatementPtrList&    stmts = n.get_init_stmt();
      ROSE_ASSERT(stmts.size() == 1);

      SgVariableDeclaration& dcl = SG_DEREF(isSgVariableDeclaration(stmts[0]));
      SgInitializedName&     ini = onlyVariable(dcl);

      prn(ini.get_name());
      prn(" in ");
      if (reverse) prn("reverse ");
      expr(ini.get_initializer());
      prn(" ");
    }

    // added to support independent unparsing of SgForInitStatement
    void handle(SgForInitStatement& n)
    {
      bool isReverse = false;

      if (SgForStatement* forStmt = isSgForStatement(n.get_parent()))
        isReverse = isSgMinusMinusOp(forStmt->get_increment());

      forInitStmt(n, isReverse);
    }

    void handle(SgForStatement& n)
    {
      ROSE_ASSERT(n.get_increment());

      const bool isReverse = isSgMinusMinusOp(n.get_increment());

      prn("for ");
      forInitStmt(SG_DEREF(n.get_for_init_stmt()), isReverse);
      prn(" loop\n");
      stmt(n.get_loop_body());
      prn("end loop");
      handleStringLabel(n.get_string_label());
      prn(STMT_SEP);
    }

    void handle(SgAdaLoopStmt& n)
    {
      prn("loop\n");
      stmt(n.get_body());
      prn("end loop");
      handleStringLabel(n.get_string_label());
      prn(STMT_SEP);
    }

    void handle(SgAdaAcceptStmt& n)
    {
      prn("accept ");
      expr(n.get_entry());
      prn(" ");
      expr_opt(n.get_index(), "(", ")");

      handleParameterList( SG_DEREF(n.get_parameterList()).get_args() );

      SgStatement* body = n.get_body();

      if (SgBasicBlock* block = isSgBasicBlock(body))
      {
        prn("do\n");
        handleBasicBlock(*block);
        prn("end ");
        expr(n.get_entry());
      }

      prn(STMT_SEP);
    }

    void handle(SgLabelStatement& n)
    {
      typedef std::pair<std::string, std::string> syntax_t;

      syntax_t syntax = sg::dispatch(LabelSyntax(n.get_label()), n.get_statement());

      prn(syntax.first);
      prn(n.get_label());
      prn(syntax.second);
      stmt(n.get_statement());
    }

    void handle(SgAdaExitStmt& n)
    {
      prn("exit");

      if (n.get_explicitLoopName())
      {
        std::string loopName = sg::dispatch(LoopLabelQuery(), n.get_loop());

        prn(" ");
        prn(loopName);
      }

      expr_opt(n.get_condition(), " when ");
      prn(STMT_SEP);
    }

    void handle(SgAdaDelayStmt& n)
    {
      prn("delay ");
      if (!n.get_isRelative()) prn("until ");
      expr(n.get_time());
      prn(STMT_SEP);
    }


    void handle(SgImportStatement& n)
    {
      const SgExpression&         elem     = si::Ada::importedElement(n);
      si::Ada::ImportedUnitResult imported = si::Ada::importedUnit(n);
      SgScopeStatement*           scope    = imported.decl().get_scope();

      prn("with ");
      prnNameQual(elem, scope);
      prn(imported.name());
      prn(STMT_SEP);

#if OBSOLETE_CODE
      if (const SgAdaRenamingDecl* rendcl = imported.renamingDecl())
      {
        SgExpression*        orig     = rendcl->get_renamed();
        RenamingSyntaxResult renamed  = renamingSyntax(orig);

        //~ if (const SgScopeStatement* renscope = renamed.body())
          //~ unparser.addRenamedScope(renscope, rendcl);
      }
      else if (/*const SgScopeStatement* unitDef =*/ unitDefinition(imported.decl()))
      {
        // renamed units are not visible (unless also imported).
        //   therefore their scope is not added as visible scopes.
        //~ unparser.addVisibleScope(unitDef);
      }
#endif /* OBSOLETE_CODE */
    }

    void handle(SgProcessControlStatement& n)
    {
      std::string stmt;
      std::string postfix;

      switch (n.get_control_kind())
      {
        case SgProcessControlStatement::e_abort:
          stmt = "abort ";
          break;

        case SgProcessControlStatement::e_requeue_with_abort:
          stmt = " with abort";
          /* fall-through */

        case SgProcessControlStatement::e_requeue:
          stmt = "requeue ";
          break;

        default:
          ROSE_ASSERT(!"invalid SgProcessControlStatement::p_control_kind");
      };

      prn(stmt);
      expr(n.get_code());
      prn(postfix);
      prn(STMT_SEP);
    }

    void handle(SgSwitchStatement& n)
    {
      prn("case ");
      expr(si::Ada::underlyingExpr(n.get_item_selector()));
      prn(" is\n");
      stmt(n.get_body());
      prn("end case;\n");
    }


    void handle(SgCaseOptionStmt& n)
    {
      prn("when ");
      expr(n.get_key());
      prn(" => ");
      stmt(n.get_body());
      prn("\n");
    }

    void handle(SgDefaultOptionStmt& n)
    {
      prn("when others => ");
      stmt(n.get_body());
      prn("\n");
    }

    void handle(SgReturnStmt& n)
    {
      prn("return");
      expr_opt(n.get_expression(), " ");
      prn(STMT_SEP);
    }

    void handle(SgAdaTerminateStmt& n)
    {
      prn("terminate");
      prn(STMT_SEP);
    }

    void handle(SgExprStatement& n)
    {
      expr(n.get_expression());
      prn(STMT_SEP);
    }

    void handle(SgBasicBlock& n)
    {
      handleBasicBlock(n);
    }

    void handle(SgAdaComponentClause& n)
    {
      expr(n.get_component());
      prn(" at ");
      expr(n.get_offset());
      prn(" range ");
      expr(n.get_range());
      prn(STMT_SEP);
    }

    void handle(SgAdaRepresentationClause& n)
    {
      prn("for ");
      type(n, n.get_recordType());

      if (SgBasicBlock* blk = n.get_components())
      {
        // unparse as record representation clause
        prn(" use record\n");
        expr_opt(n.get_alignment(), "at mod ", STMT_SEP);

        // do not unparse the block like a normal block..
        // it just contains a sequence of clauses and declarations.
        list(blk->get_statements());
        prn("end record");

      }
      else
      {
        // unparse as at clause
        prn(" use at ");
        expr(n.get_alignment());
      }

      prn(STMT_SEP);
    }

    void enumInit(SgExpression* n)
    {
      if (SgAssignOp* ini = isSgAssignOp(n))
      {
        expr(ini->get_lhs_operand());
        prn("=>");
        expr(ini->get_rhs_operand());
      }
      else
      {
        expr(n);
      }
    }

    void enuminiList(SgExpressionPtrList& lst)
    {
      if (lst.empty()) return;

      enumInit(lst[0]);

      for (size_t i = 1; i < lst.size(); ++i)
      {
        prn(", ");
        enumInit(lst[i]);
      }
    }

    void handle(SgAdaEnumRepresentationClause& n)
    {
      SgExprListExp& components = SG_DEREF(n.get_components());

      prn("for ");
      type(n, n.get_enumType());
      prn(" use (");
      enuminiList(components.get_expressions());
      prn(")");
      prn(STMT_SEP);
    }


    void handle(SgAdaAttributeClause& n)
    {
      prn("for ");
      expr(n.get_attribute());
      prn(" use ");
      expr(n.get_size());
      prn(STMT_SEP);
    }

    void handle(SgPragmaDeclaration& n)
    {
      SgPragma&      pragma = SG_DEREF(n.get_pragma());
      SgExprListExp& args = SG_DEREF(pragma.get_args());

      prn("pragma ");
      prn(pragma.get_name());

      if (!args.get_expressions().empty())
      {
        prn("(");
        expr(&args);
        prn(")");
      }

      prn(STMT_SEP);
    }

    void handle(SgBaseClass& n)
    {
      SgClassDeclaration& decl = SG_DEREF(n.get_base_class());

      prn(" new ");
      prnNameQual(n, decl.get_scope());
      prn(decl.get_name());
      prn(" with");
    }

    void handle(SgExpBaseClass& n)
    {
      prn(" new ");
      // prnNameQual(parentType, decl.get_scope());
      expr(n.get_base_class_exp());
      prn(" with");
    }

    void parentRecord_opt(SgBaseClass* bc)
    {
      if (bc) support(bc);
    }

    void parentRecord_opt(SgClassDefinition& def)
    {
      SgBaseClassPtrList& parents = def.get_inheritances();

      ROSE_ASSERT (parents.size() <= 1);

      if (parents.size() == 1)
        support(parents.at(0));
    }

    void printPendingDiscriminants()
    {
      if (!pendingDiscriminants) return;

      const SgAdaDiscriminatedTypeDecl& dcl = *pendingDiscriminants;

      // consume discriminants (actually, not necessary)
      pendingDiscriminants = nullptr;

      if (si::Ada::hasUnknownDiscriminants(dcl))
      {
        prn("(<>)");
        return;
      }

      handleParameterList(SG_DEREF(dcl.get_discriminants()).get_parameters());
    }

    void handle(SgClassDeclaration& n)
    {
      prn("type ");
      prn(n.get_name());

      printPendingDiscriminants();

      if (SgClassDefinition* def = n.get_definition())
      {
        const bool explicitNullrec = si::Ada::explicitNullRecord(*def);

        prn(" is");
        parentRecord_opt(*def);
        modifiers(n);
        if (explicitNullrec) prn(" null");
        prn(" record");

        if (!explicitNullrec)
        {
          prn("\n");
          stmt(def);
          prn("end record");
        }
      }
      else
      {
        const bool requiresPrivate = si::Ada::withPrivateDefinition(&n);
        const bool requiresIs = requiresPrivate || hasModifiers(n);

        if (requiresIs)
        {
          prn(" is");

          modifiers(n);
          parentRecord_opt(n.get_adaParentType());

          if (requiresPrivate)
            prn(" private");
        }
      }

      prn(STMT_SEP);
    }

    void handle(SgEnumDeclaration& n)
    {
      const bool                isDefinition = &n == n.get_definingDeclaration();
      SgInitializedNamePtrList& lst          = n.get_enumerators();

      prn("type ");
      prn(n.get_name());

      if (!isDefinition)
      {
        // unparse as forward declaration
        const bool requiresPrivate = si::Ada::withPrivateDefinition(&n);
        const bool requiresIs      = requiresPrivate || hasModifiers(n);

        if (requiresIs)
          prn(" is");

        modifiers(n);

        if (requiresPrivate)
          prn(" private");
      }
      else if (SgType* parentType = n.get_adaParentType())
      {
        // unparse as derived type
        prn(" is ");
        type(n, parentType);
      }
      else
      {
        // unparse as normal enum
        prn(" is");
        prn(" (");
        std::for_each(lst.begin(), lst.end(), AdaEnumeratorUnparser{unparser, info, os});
        prn(")");
      }

      prn(STMT_SEP);
    }

    void handle(SgEmptyDeclaration&)
    {
      prn("null");
      prn(STMT_SEP);
    }

    // void printVariantConditionChange(SgAdaVariantFieldDecl* prev, SgAdaVariantFieldDecl* next);

    void handle(SgClassDefinition& n)
    {
      //~ ScopeUpdateGuard scopeGuard{unparser, info, n}; // \todo not required any more?

      list(n.get_members());
    }

    void handle(SgTryStmt& n)
    {
      // skip the block, just print the statements
      const bool    requiresBeginEnd = !(si::Ada::isFunctionTryBlock(n) || si::Ada::isPackageTryBlock(n));
      SgBasicBlock& blk = SG_DEREF(isSgBasicBlock(n.get_body()));

      if (requiresBeginEnd) prn("begin\n");
      list(blk.get_statements());
      prn("exception\n");
      stmt(n.get_catch_statement_seq_root());
      if (requiresBeginEnd) { prn("end"); prn(STMT_SEP); }
    }

    void handle(SgCatchStatementSeq& n)
    {
      list(n.get_catch_statement_seq());
    }

    void handle(SgCatchOptionStmt& n)
    {
      SgVariableDeclaration&    dcl   = SG_DEREF(n.get_condition());
      SgInitializedName&        exvar = onlyVariable(dcl);
      std::string               name  = exvar.get_name();

      prn("when ");

      if (name.size())
      {
        prn(name);
        prn(": ");
      }

      type(exvar, exvar.get_type());
      prn(" =>\n");

      stmt(n.get_body());
    }


    void handle(SgFunctionParameterList&)
    {
      // handled by the SgFunctionDeclaration and friends
    }

    void handle(SgDeclarationScope&)
    {
      // handled by the discriminant unparser
    }

    void handle(SgFunctionDeclaration& n)
    {
      prnSeparateQual(n, n.get_scope());

      const bool      isFunc  = si::Ada::isFunction(n.get_type());
      std::string     keyword = isFunc ? "function" : "procedure";

      if (n.get_ada_formal_subprogram_decl())
        prn("with ");
      if (n.get_declarationModifier().isOverride())
        prn("overriding ");

      handleFunctionEntryDecl(n, keyword, isFunc);
    }

    void handle(SgAdaEntryDecl& n)
    {
      handleFunctionEntryDecl(n, "entry");
    }

    void stmt(SgStatement* s, SgAdaDiscriminatedTypeDecl* d = nullptr);
    void support(SgSupport* s);

    void expr(SgExpression* e)
    {
      unparser.unparseExpression(e, info);
    }

    void exprlst(SgExprListExp* e, std::string sep = ", ")
    {
      unparser.unparseExprListExp(e, info, std::move(sep));
    }

    void choicelst(SgExprListExp* e)
    {
      exprlst(e, "| ");
    }

    void associationList(SgExprListExp& e)
    {
      if (e.get_expressions().empty())
        return;

      prn("(");
      exprlst(&e);
      prn(")");
    }


    void expr_opt(SgExpression* e, std::string prefix_opt = std::string(), std::string postfix_opt = std::string())
    {
      if (!e || isSgNullExpression(e)) return;

      prn(prefix_opt);
      expr(e);
      prn(postfix_opt);
    }

    void type(const SgLocatedNode& ref, SgType* t)
    {
      //~ std::cerr << "*** ref* " << typeid(ref).name() << std::endl;
      unparser.unparseType(ref, t, info);
    }

    void operator()(SgStatement* s)
    {
      stmt(s);
    }

    void operator()(SgAdaComponentClause* compclause)
    {
      handle(SG_DEREF(compclause));
    }

    bool                              publicMode;
    const SgAdaDiscriminatedTypeDecl* pendingDiscriminants;
  };

  std::string
  AdaStatementUnparser::getQualification(const std::map<SgNode*, std::string>& qualMap, const SgNode& n, SgScopeStatement* scope)
  {
    using Iterator = std::map<SgNode*, std::string>::const_iterator;

    static const std::string NOQUAL;

    const Iterator pos = qualMap.find(const_cast<SgNode*>(&n));

    return (pos != qualMap.end()) ? pos->second : NOQUAL;
  }

  std::string
  AdaStatementUnparser::getQualification(const SgNode& n, SgScopeStatement* scope)
  {
    return getQualification(unparser.nameQualificationMap(), n, scope);
  }


  void AdaStatementUnparser::prnNameQual(const SgNode& n, SgScopeStatement* scope)
  {
    prn(getQualification(n, scope));
  }

  void AdaStatementUnparser::prnSeparateQual(const std::string& qual)
  {
    if (qual.size() == 0)
      return;

    prn("separate(");
    prn(qual.substr(0, qual.size()-1)); // remove the trailing '.'
    prn(")\n");
  }

  void AdaStatementUnparser::prnSeparateQual(const SgDeclarationStatement& n, SgScopeStatement* scope)
  {
    prnSeparateQual(getQualification(n, n.get_scope()));
  }

  void AdaStatementUnparser::prnNameQual(const SgLocatedNode& ref, const SgNode& n, SgScopeStatement* scope)
  {
    using NameQualMap = Unparse_Ada::NameQualMap;
    using MapOfNameQualMap = std::map<SgNode*, NameQualMap>;

    SgLocatedNode*          key = const_cast<SgLocatedNode*>(&ref);
    const MapOfNameQualMap& typeQualMap = SgNode::get_globalQualifiedNameMapForMapsOfTypes();
    const NameQualMap&      nameQualMapForShared = getQualMapping(typeQualMap, key, SgNode::get_globalQualifiedNameMapForTypes());

    prn(getQualification(nameQualMapForShared, n, scope));
  }


  void AdaStatementUnparser::handle(SgStatement& n)
  {
    // if not handled here, have the language independent parser handle it..
    unparser.UnparseLanguageIndependentConstructs::unparseStatement(&n, info);
  }

  namespace
  {
    // returns true for basic blocks that have been introduced to store
    //   statement sequences in Ada, but are not true Ada scopes.
    struct AdaStmtSequence : sg::DispatchHandler<bool>
    {
      void handle(SgNode&)                          { /* default: false */ }
      void handle(SgTryStmt&)                       { res = true; }
      void handle(SgIfStmt&)                        { res = true; }
      void handle(SgWhileStmt&)                     { res = true; }
      void handle(SgForStatement&)                  { res = true; }
      void handle(SgAdaLoopStmt&)                   { res = true; }
      void handle(SgSwitchStatement&)               { res = true; }
      void handle(SgCaseOptionStmt&)                { res = true; }
      void handle(SgCatchOptionStmt&)               { res = true; }
      void handle(SgDefaultOptionStmt&)             { res = true; }
      void handle(SgAdaSelectStmt&)                 { res = true; }
      void handle(SgAdaSelectAlternativeStmt&)      { res = true; }
      void handle(SgAdaAcceptStmt&)                 { res = true; }
      void handle(SgAdaRepresentationClause&)       { res = true; }
    };

    bool adaStmtSequence(SgBasicBlock& n)
    {
      return sg::dispatch(AdaStmtSequence{}, n.get_parent());
    }
  }


  void AdaStatementUnparser::handleBasicBlock(SgBasicBlock& n, bool functionbody)
  {
    //~ ScopeUpdateGuard             scopeGuard{unparser, info, n};

    SgStatementPtrList&          stmts    = n.get_statements();
    SgStatementPtrList::iterator aa       = stmts.begin();
    SgStatementPtrList::iterator zz       = stmts.end();
    SgStatementPtrList::iterator dcllimit = si::Ada::declarationLimit(stmts);
    const std::string            label    = n.get_string_label();
    const bool                   requiresBeginEnd = !adaStmtSequence(n);
    //~ ROSE_ASSERT(aa == dcllimit || requiresBeginEnd);

    // was: ( functionbody || (aa != dcllimit) || label.size() );

    if (!functionbody && (aa != dcllimit))
      prn("declare\n");

    list(aa, dcllimit);

    if (requiresBeginEnd)
      prn("begin\n");

    list(dcllimit, zz);

    if (requiresBeginEnd)
    {
      prn("end");

      if (label.size())
      {
        prn(" ");
        prn(label);
      }

      if (!functionbody)
        prn(STMT_SEP);
    }
  }


  void AdaStatementUnparser::handleStringLabel(const std::string& s)
  {
    if (s.size() == 0) return;

    prn(" ");
    prn(s);
  }

  void
  AdaStatementUnparser::handleParameterList(const SgInitializedNamePtrList& params)
  {
    using parameter_decl_t = std::vector<SgVariableDeclaration*>;

    parameter_decl_t           paramdecls;

    // Since SgFunctionParameterScope (and SgFunctionDefinition) do not allow
    //   traversing the function parameter declarations, they are collected
    //   from initialized names.

    std::transform( params.begin(), params.end(),
                    std::back_inserter(paramdecls),
                    variableDeclaration
                  );

    parameter_decl_t::iterator aa = paramdecls.begin();
    parameter_decl_t::iterator zz = std::unique(aa, paramdecls.end());

    // print parenthesis only if parameters were present
    if (aa != zz)
    {
      prn("(");
      std::for_each(aa, zz, AdaParamUnparser{unparser, info, os});
      prn(")");
    }
  }

  void
  AdaStatementUnparser::handleParameterList(const SgDeclarationStatementPtrList& params)
  {
    using parameter_decl_t = std::vector<SgVariableDeclaration*>;

    parameter_decl_t           paramdecls;

    // Since SgFunctionParameterScope (and SgFunctionDefinition) do not allow
    //   traversing the function parameter declarations, they are collected
    //   from initialized names.

    std::transform( params.begin(), params.end(),
                    std::back_inserter(paramdecls),
                    asVariableDeclaration
                  );

    parameter_decl_t::iterator aa = paramdecls.begin();
    parameter_decl_t::iterator zz = paramdecls.end();

    // print parenthesis only if parameters were present
    if (aa != zz)
    {
      prn("(");
      std::for_each(aa, zz, AdaParamUnparser{unparser, info, os});
      prn(")");
    }
  }


  void
  AdaStatementUnparser::handleFunctionEntryDecl(SgFunctionDeclaration& n, std::string keyword, bool hasReturn)
  {
    std::string name = si::Ada::convertRoseOperatorNameToAdaName(n.get_name());

    prn(keyword);
    prn(" ");
    // \todo do we need to qualify the name?
    //       note, separated bodies have qualified names, but they are printed before
    //       in handle (SgFunctionDeclaration&).
    prn(name);

    SgAdaEntryDecl* adaEntry = isSgAdaEntryDecl(&n);

    if (adaEntry)
    {
      // print entry index if present, otherwise nothing
      //   for entry declarations: (type)
      //   for entry definitions:  (for varname in type)
      SgInitializedName& idxvar = SG_DEREF(adaEntry->get_entryIndex());
      SgType*            idxty = idxvar.get_type();

      if (isSgTypeVoid(idxty) == nullptr)
      {
        ROSE_ASSERT(idxty);

        std::string idxname = idxvar.get_name();

        prn("(");

        if (idxname.size())
        {
          prn("for ");
          prn(idxname);
          prn(" in ");
        }

        type(n, idxty);
        prn(")");
      }
    }

    handleParameterList( SG_DEREF(n.get_parameterList()).get_args() );

    if (hasReturn)
    {
      prn(" return");
      type(n, n.get_orig_return_type());
    }

    // MS 12/22/20 : if this is actually a function renaming declaration,
    // print the renaming syntax after the function/procedure declaration
    // and immediately return.
    if (SgAdaFunctionRenamingDecl* renaming = isSgAdaFunctionRenamingDecl(&n))
    {
      // PP 9/2/22 : updated renamed_function from SgFunctionDeclaration to SgExpression
      if (SgExpression* renamed = renaming->get_renamed_function())
      {
        // a true renaming
        prn(" renames ");
        expr(renamed);

        //~ prnNameQual(n, *renamed, renamed->get_scope());
        //~ prn(si::Ada::convertRoseOperatorNameToAdaName(renamed->get_name()));
      }
      // else this is a procedure declaration defined using renaming-as-body

      prn(STMT_SEP);
      return;
    }

    SgFunctionDefinition* def = n.get_definition();

    if (!def)
    {
      if (n.get_declarationModifier().isAdaSeparate())
        prn(" is separate");

      prn(STMT_SEP);
      return;
    }

    if (si::Ada::explicitNullProcedure(*def))
    {
      prn(" is null");
      prn(STMT_SEP);
      return;
    }

    if (adaEntry)
    {
      SgExpression* barrier = adaEntry->get_entryBarrier();

      expr_opt(barrier, " when ");
    }

    prn(" is\n");
    stmt(def);

    prn(" ");
    prn(name);
    prn(STMT_SEP);
  }


  template <class ForwardIterator>
  void AdaStatementUnparser::list(ForwardIterator aa, ForwardIterator zz, bool hasPrivateSection)
  {
    const bool endedInPublicMode = std::for_each( aa, zz,
                                                  AdaStatementUnparser{unparser, info, os}
                                                ).publicMode;

    // add private keyword for empty private sections
    if (hasPrivateSection && endedInPublicMode)
      prn("private\n");
  }

  template <class SageNodeList>
  void AdaStatementUnparser::list(SageNodeList& lst, bool hasPrivateSection)
  {
    list(lst.begin(), lst.end(), hasPrivateSection);
  }

  void AdaStatementUnparser::modifiers(SgDeclarationStatement& n)
  {
    SgDeclarationModifier& mod = n.get_declarationModifier();

    if (mod.isAdaAbstract()) prn(" abstract");
    if (mod.isAdaTagged())   prn(" tagged");
    if (mod.isAdaLimited())  prn(" limited");
  }

  bool AdaStatementUnparser::hasModifiers(SgDeclarationStatement& n)
  {
    SgDeclarationModifier& mod = n.get_declarationModifier();

    return mod.isAdaAbstract() || mod.isAdaTagged() || mod.isAdaLimited();
  }

  struct TypedeclSyntax : sg::DispatchHandler<std::string>
  {
    void handle(SgNode& n)         { SG_UNEXPECTED_NODE(n); }

    void handle(SgType&)           { res = "subtype"; }
    void handle(SgAdaFormalType&)  { res = "type"; }
    void handle(SgAdaAccessType&)  { res = "type"; }
    void handle(SgAdaDerivedType&) { res = "type"; }
    void handle(SgAdaModularType&) { res = "type"; }
    void handle(SgTypeDefault&)    { res = "type"; }
    void handle(SgArrayType&)      { res = "type"; }
    //~ void handle(SgAdaFloatType&)   { res = "type"; }

    void handle(SgAdaSubtype& n)
    {
      res = n.get_fromRootType() ? "type" : "subtype";
    }
  };

  std::string
  AdaStatementUnparser::typedeclSyntax(SgType* n)
  {
    return sg::dispatch(TypedeclSyntax{}, n);
  }

  struct RenamingSyntax : sg::DispatchHandler<RenamingSyntaxResult>
  {
      using base = sg::DispatchHandler<RenamingSyntaxResult>;

      explicit
      RenamingSyntax(bool isNonGeneric)
      : base(), forceNonGeneric(isNonGeneric)
      {}

      void handle(const SgNode& n)      { SG_UNEXPECTED_NODE(n); }

      void handle(const SgAdaRenamingSymbol& n)
      {
        SgAdaRenamingDecl& dcl = SG_DEREF(n.get_declaration());

        res = AdaStatementUnparser::renamingSyntax(dcl.get_renamed());
        //~ res.renamedName() = n.get_name();
      }

      void handle(const SgAdaPackageSpecDecl& n)
      {
        std::string prefix = genericPrefix(n) + "package ";

        res = ReturnType{prefix, false /* does not require type */, n.get_definition()};
        //~ res = ReturnType{prefix, false /* does not require type */, n.get_name(), n.get_definition()};
      }

      void handle(const SgAdaPackageBodyDecl& n)
      {
        res = ReturnType{"package ", false /* does not require type */, SG_DEREF(n.get_definition()).get_spec()};
        //~ res = ReturnType{"package ", false /* does not require type */, n.get_name(), SG_DEREF(n.get_definition()).get_spec()};
      }

      void handle(const SgAdaPackageSymbol& n)
      {
        res = compute(n.get_declaration(), forceNonGeneric);
      }

      void handle(const SgFunctionDeclaration& n)
      {
        std::string prefix = genericPrefix(n);

        prefix += si::Ada::isFunction(n.get_type()) ? "function " : "procedure ";

        res = ReturnType{prefix, false, nullptr};
        //~ res = ReturnType{prefix, false, n.get_name(), nullptr};
      }

      void handle(const SgAdaGenericDecl& n)
      {
        res = compute(n.get_declaration(), forceNonGeneric);
      }

      void handle(const SgAdaGenericInstanceDecl& n)
      {
        res = compute(n.get_declaration(), true /* force non generic */);
      }

      //
      // expression refs

      void handle(const SgAdaUnitRefExp& n)
      {
        // get the prefix from the declaration, then set the proper name.
        res = compute(n.get_decl(), forceNonGeneric);
        //~ res.renamedName() = n.get_name();
      }

      void handle(const SgVarRefExp&)
      {
        //~ res = ReturnType{"", true /* requires type */, n.get_name(), nullptr};
        res = ReturnType{"", true /* requires type */, nullptr};
      }

      void handle(const SgFunctionRefExp& n)
      {
        res = compute(n.getAssociatedFunctionDeclaration(), forceNonGeneric);
        // res.renamedName() = n.get_name();
      }

      void handle(const SgExpression&)
      {
        // object renaming
        res = ReturnType{"", true /* requires type */, nullptr};
      }

      static
      RenamingSyntaxResult
      compute(const SgNode* n, bool forceNonGeneric = false);

    private:
      bool forceNonGeneric;

      std::string
      genericPrefix(const SgDeclarationStatement& n);

      RenamingSyntax() = delete;
  };

  std::string
  RenamingSyntax::genericPrefix(const SgDeclarationStatement& n)
  {
    if (forceNonGeneric || !si::Ada::isGenericDecl(n))
      return "";

    return "generic ";
  }

  RenamingSyntaxResult
  RenamingSyntax::compute(const SgNode* n, bool forceNonGeneric)
  {
    return sg::dispatch(RenamingSyntax{forceNonGeneric}, n);
  }

  RenamingSyntaxResult
  AdaStatementUnparser::renamingSyntax(SgExpression* n)
  {
    return RenamingSyntax::compute(n);
  }

  bool isPrivate(SgDeclarationStatement& dcl)
  {
    return dcl.get_declarationModifier().get_accessModifier().isPrivate();
  }

  void AdaStatementUnparser::startPrivateIfNeeded(SgDeclarationStatement* n)
  {
    if (!publicMode || !n || !isPrivate(*n)) return;

    prn("private\n");
    publicMode = false;
  }

  AdaStatementUnparser
  AdaStatementUnparser::unparserWith(SgAdaDiscriminatedTypeDecl* n)
  {
    AdaStatementUnparser tmp{*this};

    tmp.pendingDiscriminants = n;
    return tmp;
  }


  void AdaStatementUnparser::stmt(SgStatement* s, SgAdaDiscriminatedTypeDecl* n)
  {
    startPrivateIfNeeded(isSgDeclarationStatement(s));

    unparser.unparseAttachedPreprocessingInfo(s, info, PreprocessingInfo::before);
    unparser.unparseAttachedPreprocessingInfo(s, info, PreprocessingInfo::inside);
    sg::dispatch(unparserWith(n), s);
    unparser.unparseAttachedPreprocessingInfo(s, info, PreprocessingInfo::after);
  }

  void AdaStatementUnparser::support(SgSupport* s)
  {
    sg::dispatch(*this, s);
  }

  struct ScopeName : sg::DispatchHandler<std::string>
  {
      void withName(const std::string& name);
      void withoutName() {}

      void checkParent(const SgScopeStatement& n);

      void handle(const SgNode& n)                 { SG_UNEXPECTED_NODE(n); }

      // default for all scopes and declarations
      void handle(const SgStatement& n)            { withoutName(); }

      // scopes that may have names
      // \todo do we also need named loops?
      void handle(const SgBasicBlock& n)           { withName(n.get_string_label()); }
      void handle(const SgAdaTaskSpec& n)          { checkParent(n); }
      void handle(const SgAdaTaskBody& n)          { checkParent(n); }
      void handle(const SgAdaProtectedSpec& n)     { checkParent(n); }
      void handle(const SgAdaProtectedBody& n)     { checkParent(n); }
      void handle(const SgAdaPackageBody& n)       { checkParent(n); }
      void handle(const SgAdaPackageSpec& n)       { checkParent(n); }
      // FunctionDefinition, ..


      // parent handlers
      void handle(const SgDeclarationStatement& n) { withoutName(); }
      void handle(const SgAdaTaskSpecDecl& n)      { withName(n.get_name()); }
      void handle(const SgAdaTaskBodyDecl& n)      { withName(n.get_name()); }
      void handle(const SgAdaProtectedSpecDecl& n) { withName(n.get_name()); }
      void handle(const SgAdaProtectedBodyDecl& n) { withName(n.get_name()); }
      void handle(const SgAdaPackageSpecDecl& n)   { withName(n.get_name()); }
      void handle(const SgAdaPackageBodyDecl& n)   { withName(n.get_name()); }
      void handle(const SgAdaRenamingDecl& n)      { withName(n.get_name()); }
      // FunctionDeclaration, ..
  };

  void ScopeName::withName(const std::string& s)
  {
    res = s;
  }

  void ScopeName::checkParent(const SgScopeStatement& n)
  {
    // get the name
    res = sg::dispatch(ScopeName{}, n.get_parent());
  }

  std::string
  scopeName(const SgStatement* n)
  {
    return sg::dispatch(ScopeName{}, n);
  }

  struct RequiresScopeQual : sg::DispatchHandler<bool>
  {
      bool isNormalPkg(const SgAdaPackageSpec& n)
      {
        const SgAdaPackageSpecDecl& dcl = SG_DEREF(isSgAdaPackageSpecDecl(n.get_parent()));

        return (dcl.get_name() != "Standard") || !isSgGlobal(dcl.get_parent());
      }

      void handle(const SgNode& n)           { SG_UNEXPECTED_NODE(n); }
      void handle(const SgScopeStatement&)   { res = true; }
      void handle(const SgGlobal&)           { res = false; }
      void handle(const SgAdaPackageSpec& n) { res = isNormalPkg(n); }
  };

#if OBSOLETE_CODE
  /// returns true iff \ref n requires scope qualification
  bool requiresScopeQual(const SgScopeStatement* n)
  {
    return sg::dispatch(RequiresScopeQual{}, n);
  }
#endif /* OBSOLETE_CODE */

  /// \brief stores a path from an innermost scope to the global scope (not part of the path)
  ///        in form of a sequence of Sage nodes that represent scopes
  ///        (SgScopeStatements or SgDeclarationStatements).
  /// \details
  ///    - The path is traversed using the range [rbegin(), rend()) to get the scopes
  ///      in order from outermost scope to innermost scope.
  ///    - The path may contain scopes without names. Those will be skipped
  ///      when the qualified name is stringified.
  struct ScopePath : private std::vector<const SgStatement*>
  {
    using base = std::vector<const SgStatement*>;
    using base::base;

    using base::const_reverse_iterator;
    using base::reverse_iterator;
    using base::rend;
    using base::rbegin;
    using base::value_type;
    using base::reference;
    using base::size;

    /// returns a string version of the scopes in range [rbegin(), rend())
    std::string path() const
    {
      return path(rbegin());
    }

    /// returns a string version of the scopes in range [\ref pos, rend())
    std::string path(const_reverse_iterator pos) const;

    /// overload vector's push_back to check element validity
    void push_back(base::value_type ptr)
    {
      ROSE_ASSERT(isSgScopeStatement(ptr) || isSgDeclarationStatement(ptr));
      ROSE_ASSERT(!isSgGlobal(ptr));

      base::push_back(ptr);
    }
  };


  std::string
  ScopePath::path(const_reverse_iterator pos) const
  {
    std::stringstream qual;

    for ( ; pos != rend(); ++pos)
    {
      std::string name = scopeName(*pos);

      if (name.size())
        qual << name << '.';
    }

    return qual.str();
    //~ return std::move(qual).str(); // C++-20
  }
}


void
Unparse_Ada::unparseStatement(SgStatement* stmt, SgUnparse_Info& info)
{
  SgNode* const currentReferenceNode = info.get_reference_node_for_qualification();

  // set the reference node, unless the unparser is already in type mode
  if (&nameQualificationMap() == &SgNode::get_globalQualifiedNameMapForNames())
    info.set_reference_node_for_qualification(stmt);

  AdaStatementUnparser adaUnparser{*this, info, std::cerr};

  adaUnparser.stmt(stmt);

  // restore reference node
  info.set_reference_node_for_qualification(currentReferenceNode);
}

void
Unparse_Ada::unparseParameterList(const SgInitializedNamePtrList& params, SgUnparse_Info& info)
{
  AdaStatementUnparser adaUnparser{*this, info, std::cerr};

  adaUnparser.handleParameterList(params);
}

const Unparse_Ada::NameQualMap&
Unparse_Ada::nameQualificationMap() const
{
  return SG_DEREF(currentNameQualificationMap);
}

void Unparse_Ada::withNameQualificationMap(const NameQualMap& m)
{
  currentNameQualificationMap = &m;
}


void
Unparse_Ada::unparseLanguageSpecificStatement(SgStatement* stmt, SgUnparse_Info& info)
{
  ASSERT_not_null(stmt);

  SG_UNEXPECTED_NODE(*stmt);
}
