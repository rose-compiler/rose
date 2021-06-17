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

#include <boost/algorithm/string/case_conv.hpp>

#define ROSE_TRACK_PROGRESS_OF_ROSE_COMPILING_ROSE 0

#include "sage_support.h"
#include "sageGeneric.h"
#include "sageInterfaceAda.h"

namespace si = SageInterface;

Unparse_Ada::Unparse_Ada(Unparser* unp, std::string fname)
   : UnparseLanguageIndependentConstructs(unp,fname)
   {
  // Nothing to do here!
   }


void
Unparse_Ada::unparseAdaFile(SgSourceFile *sourcefile, SgUnparse_Info& info)
{
  SgGlobal* globalScope = sourcefile->get_globalScope();
  ASSERT_not_null(globalScope);

  unparseStatement(globalScope, info);
}


namespace
{
  struct ScopeUpdateGuard
  {
      ScopeUpdateGuard(SgUnparse_Info& info, SgScopeStatement& scope)
      : ui(info), oldScope(info.get_current_scope())
      {
        ui.set_current_scope(&scope);
      }

      ~ScopeUpdateGuard()
      {
        ui.set_current_scope(oldScope);
      }

    private:
      ScopeUpdateGuard(const ScopeUpdateGuard&) = delete;

      SgUnparse_Info&   ui;
      SgScopeStatement* oldScope;
  };

  SgVariableSymbol& symOf(const SgVarRefExp& n)
  {
    return SG_DEREF(n.get_symbol());
  }

  inline
  SgName nameOf(const SgSymbol& sy)
  {
    return sy.get_name();
  }

  inline
  SgName nameOf(const SgVarRefExp& n)
  {
    return nameOf(symOf(n));
  }

  inline
  SgName nameOf(const SgImportStatement& import)
  {
    const SgExpressionPtrList& lst = import.get_import_list();
    ROSE_ASSERT(lst.size() == 1);

    return nameOf(SG_DEREF(isSgVarRefExp(lst.back())));
  }

  const std::string NO_SEP = "";
  const std::string COMMA_SEP = ", ";
  const std::string STMT_SEP = ";\n";
  const std::string PARAM_SEP = "; ";

  bool isAdaFunction(SgFunctionType& ty)
  {
    return isSgTypeVoid(ty.get_return_type()) == nullptr;
  }

  SgExpression* underlyingExpr(SgStatement* s)
  {
    SgExprStatement* es = isSgExprStatement(s);

    return SG_DEREF(es).get_expression();
  }

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

      unparser.unparseType(primary.get_type(), info);

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

  template <class AstVisitor>
  void unparseElseBranch(AstVisitor vis, SgStatement* elseBranch)
  {
    if (!elseBranch) return;

    sg::dispatch(vis, elseBranch);
  }


  struct AdaElseUnparser : AdaDetailsUnparser
  {
    using AdaDetailsUnparser::AdaDetailsUnparser;

    void handle(SgNode& n) { SG_UNEXPECTED_NODE(n); }

    void handle(SgBasicBlock& n)
    {
      prn("else\n");
      unparser.unparseStatement(&n, info);
    }

    void handle(SgIfStmt& n)
    {
      prn("elsif ");
      unparser.unparseExpression(underlyingExpr(n.get_conditional()), info);
      prn(" then\n");
      unparser.unparseStatement(n.get_true_body(), info);

      unparseElseBranch(*this, n.get_false_body());
    }

    void handle(SgNullStatement&)
    {
      prn("null");
      prn(STMT_SEP);
    }
  };


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
    typedef sg::DispatchHandler<std::pair<std::string, std::string> > base;

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

  struct RenamingSyntax
  {
      RenamingSyntax()
      : prefixSyntax(), infixSyntax(), renamedName()
      {}

      RenamingSyntax(std::string prefix, std::string infix, std::string renamed)
      : prefixSyntax(prefix), infixSyntax(infix), renamedName(renamed)
      {}

      std::string prefixSyntax;
      std::string infixSyntax;
      std::string renamedName;
  };

  std::pair<SgDeclarationStatementPtrList::iterator, SgDeclarationStatementPtrList::iterator>
  declsInPackage(SgDeclarationStatementPtrList& lst, const std::string& mainFile)
  {
    auto declaredInMainFile = [&mainFile](const SgDeclarationStatement* dcl)
                              {
                                ROSE_ASSERT(dcl);

                                const Sg_File_Info& fileInfo = SG_DEREF(dcl->get_startOfConstruct());

                                return fileInfo.get_filenameString() == mainFile;
                              };

    SgDeclarationStatementPtrList::iterator zz    = lst.end();
    SgDeclarationStatementPtrList::iterator first = std::find_if(lst.begin(), zz, declaredInMainFile);

    return std::make_pair(first, zz);
  }


  struct AdaStatementUnparser : AdaDetailsUnparser
  {
    AdaStatementUnparser(Unparse_Ada& unp, SgUnparse_Info& inf, std::ostream& outp)
    : AdaDetailsUnparser(unp, inf, outp), publicMode(true)
    {}

    template <class SageStmtList>
    void list(SageStmtList& lst, bool hasPrivateSection = false);

    template <class ForwardIterator>
    void list(ForwardIterator aa, ForwardIterator zz, bool hasPrivateSection = false);

    void handleBasicBlock(SgBasicBlock& n, bool functionbody = false);

    void handleBasicBlock(SgBasicBlock* n, bool functionbody = false)
    {
      handleBasicBlock(SG_DEREF(n), functionbody);
    }

    void handleStringLabel(const std::string& s);

    void handleFunctionEntryDecl(SgFunctionDeclaration&, std::string keyword, bool hasReturn = false);

    void handleParameterList(SgInitializedNamePtrList& params);

    static
    std::pair<std::string, std::string>
    typedeclSyntax(SgType* n);

    static
    RenamingSyntax
    renamingDeclSyntax(SgSymbol* n);

    static
    std::pair<std::string, std::string>
    useClauseSyntax(SgDeclarationStatement*);

    void startPrivateIfNeeded(SgDeclarationStatement* n);

    void modifiers(SgDeclarationStatement& n);
    bool hasModifiers(SgDeclarationStatement& n);

    //
    // handlers

    void handle(SgNode& n)      { SG_UNEXPECTED_NODE(n); }

    void handle(SgStatement& n);

    void handle(SgGlobal& n)
    {
      typedef SgDeclarationStatementPtrList::iterator Iterator;

      ScopeUpdateGuard scopeGuard(info, n);

      std::pair<Iterator, Iterator> declRange = declsInPackage(n.get_declarations(), unparser.getFileName());

      list(declRange.first, declRange.second);
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
      if (isSgNullExpression(n.get_guard()) == NULL) {
        prn("when ");
        expr(n.get_guard());
        prn(" =>\n");
      }
      stmt(n.get_body());
      if (n.get_next() != NULL) {
        prn("or\n");
        stmt(n.get_next());
      }
    }

    void handle(SgAdaTaskTypeDecl& n)
    {
      prn("task type ");
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

    void handle(SgAdaTaskBodyDecl& n)
    {
      prn("task body ");
      prn(n.get_name());
      prn(" is\n");

      stmt(n.get_definition());

      prn(" ");
      prn(n.get_name());
      prn(STMT_SEP);
    }

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

    void handle(SgAdaTaskSpec& n)
    {
      ScopeUpdateGuard scopeGuard(info, n);

      list(n.get_declarations());
    }

    void handle(SgAdaTaskBody& n)
    {
      ScopeUpdateGuard scopeGuard(info, n);

      SgStatementPtrList&          stmts    = n.get_statements();
      SgStatementPtrList::iterator aa       = stmts.begin();
      SgStatementPtrList::iterator zz       = stmts.end();
      SgStatementPtrList::iterator dcllimit = si::ada::declarationLimit(stmts);

      list(aa, dcllimit);

      prn("begin\n");
      list(dcllimit, zz);
      prn("end"); // omit newline, which will be added by the parent
    }

    void handle(SgAdaPackageSpecDecl& n)
    {
      const std::string qual = scopeQual(n.get_scope());

      prn("package ");
      prn(qual);
      prn(n.get_name());
      prn(" is\n");

      stmt(n.get_definition());

      prn("end ");
      prn(qual);
      prn(n.get_name());
      prn(STMT_SEP);
    }

    void handle(SgAdaPackageBodyDecl& n)
    {
      prn("package body ");
      prn(n.get_name());
      prn(" is\n");

      stmt(n.get_definition());

      prn("end ");
      prn(n.get_name());
      prn(STMT_SEP);
    }

    void handle(SgAdaPackageSpec& n)
    {
      ScopeUpdateGuard scopeGuard(info, n);

      list(n.get_declarations(), n.get_hasPrivate());
    }

    void handle(SgAdaPackageBody& n)
    {
      typedef SgStatementPtrList::iterator Iterator;

      ScopeUpdateGuard    scopeGuard(info, n);
      SgStatementPtrList& stmts = n.get_statements();
      SgBasicBlock*       block = nullptr;
      Iterator            zz = stmts.end();

      if (stmts.size()) block = isSgBasicBlock(stmts.back());
      if (block) --zz;

      list(stmts.begin(), zz);

      if (block)
      {
        prn("begin\n");
        list(block->get_statements());
        // the block's end is printed in the parent
      }
    }

    void handle(SgAdaRenamingDecl& n)
    {
      SgSymbol*      orig    = n.get_renamed();
      RenamingSyntax renamed = renamingDeclSyntax(orig);

      prn(renamed.prefixSyntax);
      prn(n.get_name());
      prn(renamed.infixSyntax);
      prn(" renames ");
      prn(scopeQual(orig->get_scope()));
      prn(renamed.renamedName);
      prn(STMT_SEP);
    }

    void handle(SgUsingDeclarationStatement& n)
    {
      typedef std::pair<std::string, std::string> syntax_t;

      SgDeclarationStatement* orig      = n.get_declaration();
      syntax_t                usesyntax = useClauseSyntax(orig);

      // \todo record usedName for improved scope qualification

      prn("use ");
      prn(usesyntax.first);
      prn(scopeQual(orig->get_scope()));
      prn(usesyntax.second);
      prn(STMT_SEP);
    }

    void handle(SgTypedefDeclaration& n)
    {
      std::pair<std::string, std::string> declwords = typedeclSyntax(n.get_base_type());

      prn(declwords.first);
      prn(" ");
      prn(n.get_name());


      const bool isDefinition    = &n == n.get_definingDeclaration();
      const bool requiresPrivate = (!isDefinition) && si::ada::withPrivateDefinition(&n);
      const bool requiresIs      = (  requiresPrivate
                                   || hasModifiers(n)
                                   || declwords.second.size() != 0
                                   || !isSgTypeDefault(n.get_base_type())
                                   );

      if (requiresIs)
        prn(" is");

      modifiers(n);
      prn(declwords.second);
      prn(" ");
      type(n.get_base_type());

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

      type(primary.get_type());
      expr_opt(primary.get_initializer(), " := ");
      prn(STMT_SEP);
    }

    void handle(SgFunctionDefinition& n)
    {
      ScopeUpdateGuard scopeGuard(info, n);

      handleBasicBlock(n.get_body(), true /* function body */);
    }

    void handle(SgAdaGenericDefn& n)
    {
      SgDeclarationStatementPtrList& decls = n.get_declarations();
      list(decls);
    }

    void handle(SgAdaFormalTypeDecl& n)
    {
      prn("type ");
      prn(n.get_name());
      prn(" is ");
      SgAdaFormalType* ty = n.get_formal_type();
      if (ty->get_is_limited()) {
        prn("limited ");
      }
      if (ty->get_is_private()) {
        prn("private");
      }
      prn(";\n");
    }

    void handle(SgAdaGenericDecl& n)
    {
      prn("generic\n");
      stmt(n.get_definition());
      // check which kind of generic we have:
      if (isSgAdaPackageSpecDecl(n.get_declaration())) {
        SgAdaPackageSpecDecl* pkgspec = isSgAdaPackageSpecDecl((SgNode*)n.get_declaration());
        stmt(pkgspec);
        return;
      }
      if (isSgFunctionDeclaration(n.get_declaration())) {
        SgFunctionDeclaration* fundec = isSgFunctionDeclaration((SgNode*)n.get_declaration());
        stmt(fundec);
        return;
      }

      ROSE_ABORT();
    }

    void handle(SgIfStmt& n)
    {
      prn("if ");
      expr(underlyingExpr(n.get_conditional()));
      prn(" then\n");
      stmt(n.get_true_body());

      unparseElseBranch(AdaElseUnparser(unparser, info, os), n.get_false_body());

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
      expr(underlyingExpr(n.get_condition()));
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

      SgVariableDeclaration& dcl = SG_ASSERT_TYPE(SgVariableDeclaration, SG_DEREF(stmts[0]));
      ROSE_ASSERT(dcl.get_variables().size() == 1);

      SgInitializedName&     ini = SG_DEREF(dcl.get_variables()[0]);

      prn(ini.get_name());
      prn(" in ");
      if (reverse) prn("reverse ");
      expr(ini.get_initializer());
      prn(" ");
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

      if (isSgNullExpression(n.get_condition()) == nullptr)
      {
        prn(" when ");
        expr(n.get_condition());
      }

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
      prn("with ");

      SgExpressionPtrList& lst = n.get_import_list();
      //~ ROSE_ASSERT(lst.size() != 0);
      ROSE_ASSERT(lst.size() == 1);

      /*
      for (size_t i = 0; i < lst.size()-1; ++i)
      {
        expr(lst[i]);
        prn(".");
      }
      */

      expr(lst.back());
      prn(STMT_SEP);
    }

    void handle(SgProcessControlStatement& n)
    {
      ROSE_ASSERT(n.get_control_kind() == SgProcessControlStatement::e_abort);

      prn("abort ");
      expr(n.get_code());
      prn(STMT_SEP);
    }

    void handle(SgSwitchStatement& n)
    {
      prn("case ");
      expr(underlyingExpr(n.get_item_selector()));
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

    void handle(SgAdaRecordRepresentationClause& n)
    {
      SgBasicBlock& blk = SG_DEREF(n.get_components());

      prn("for ");
      type(n.get_recordType());
      prn(" use record\n");
      expr_opt(n.get_alignment(), "at mod ", STMT_SEP);

      // do not unparse the block like a normal block..
      // it just contains a sequence of clauses and declarations.
      list(blk.get_statements());
      prn("end record");
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
      type(n.get_enumType());
      prn(" use (");
      enuminiList(components.get_expressions());
      prn(")");
      prn(STMT_SEP);
    }


    void handle(SgAdaLengthClause& n)
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



    //~ ScopePath pathToGlobal(SgStatement& n);
    //~ std::string recoverScopeName(SgLocatedNode& n);

    std::string scopeQual(SgScopeStatement& remote);

    std::string scopeQual(SgScopeStatement* remote)
    {
      return scopeQual(SG_DEREF(remote));
    }

    void parentRecord(SgBaseClass& parentType)
    {
      SgClassDeclaration& decl   = SG_DEREF(parentType.get_base_class());

      prn(" new ");
      prn(scopeQual(decl.get_scope()));
      prn(decl.get_name());
      prn(" with");
    }

    void parentRecord_opt(SgBaseClass* baserec)
    {
      if (baserec) parentRecord(*baserec);
    }

    void parentRecord_opt(SgClassDefinition& def)
    {
      SgBaseClassPtrList& parents = def.get_inheritances();

      if (parents.size() == 1)
        parentRecord(SG_DEREF(parents.at(0)));
    }

    void handle(SgClassDeclaration& n)
    {
      prn("type ");
      prn(n.get_name());

      if (SgClassDefinition* def = n.get_definition())
      {
        const bool explicitNullrec = (  def->get_members().empty()
                                     && def->get_inheritances().empty()
                                     );

        prn(" is");
        if (!explicitNullrec) parentRecord_opt(*def);
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
        const bool requiresPrivate = si::ada::withPrivateDefinition(&n);
        const bool requiresIs = requiresPrivate || hasModifiers(n);

        //~ std::cerr << "private type: " << requiresPrivate << std::endl;

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
      SgInitializedNamePtrList& lst = n.get_enumerators();

      prn("type ");
      prn(n.get_name());
      prn(" is (");

      std::for_each(lst.begin(), lst.end(), AdaEnumeratorUnparser(unparser, info, os));

      prn(")");
      prn(STMT_SEP);
    }

    void handle(SgEmptyDeclaration&)
    {
      prn("null");
      prn(STMT_SEP);
    }

    void handle(SgClassDefinition& n)
    {
      ScopeUpdateGuard scopeGuard(info, n); // \todo required?

      list(n.get_members());
    }

    void handle(SgTryStmt& n)
    {
      // skip the block, just print the statements
      const bool    requiresBeginEnd = !si::ada::isFunctionTryBlock(n);
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
      prn("when ");

      SgVariableDeclaration&    dcl   = SG_DEREF(n.get_condition());
      SgInitializedNamePtrList& vars  = dcl.get_variables();
      ROSE_ASSERT(vars.size() == 1);
      SgInitializedName&        exvar = SG_DEREF(vars[0]);
      std::string               name  = exvar.get_name();

      if (name.size())
      {
        prn(name);
        prn(": ");
      }

      type(exvar.get_type());
      prn(" =>\n");

      stmt(n.get_body());
    }


    void handle(SgFunctionParameterList& n)
    {
      // handled by the SgFunctionDeclaration and friends
    }


    void handle(SgFunctionDeclaration& n)
    {
      SgFunctionType& ty      = SG_DEREF(n.get_type());
      const bool      isFunc  = isAdaFunction(ty);
      std::string     keyword = isFunc ? "function" : "procedure";

      if (n.get_declarationModifier().isOverride())
        prn("overriding ");

      handleFunctionEntryDecl(n, keyword, isFunc);
    }

    void handle(SgAdaEntryDecl& n)
    {
      handleFunctionEntryDecl(n, "entry");
    }

    void stmt(SgStatement* s);

    void expr(SgExpression* e)
    {
      unparser.unparseExpression(e, info);
    }

    void expr_opt(SgExpression* e, std::string prefix_opt = std::string(), std::string postfix_opt = std::string())
    {
      if (!e || isSgNullExpression(e)) return;

      prn(prefix_opt);
      expr(e);
      prn(postfix_opt);
    }

    void type(SgType* t)
    {
      unparser.unparseType(t, info);
    }

    void operator()(SgStatement* s)
    {
      stmt(s);
    }

    void operator()(SgAdaComponentClause* compclause)
    {
      handle(SG_DEREF(compclause));
    }

    bool            publicMode;
  };

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
      void handle(SgAdaRecordRepresentationClause&) { res = true; }
    };

    bool adaStmtSequence(SgBasicBlock& n)
    {
      return sg::dispatch(AdaStmtSequence{}, n.get_parent());
    }
  }


  void AdaStatementUnparser::handleBasicBlock(SgBasicBlock& n, bool functionbody)
  {
    SgStatementPtrList&          stmts    = n.get_statements();
    SgStatementPtrList::iterator aa       = stmts.begin();
    SgStatementPtrList::iterator zz       = stmts.end();
    SgStatementPtrList::iterator dcllimit = si::ada::declarationLimit(stmts);
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
  AdaStatementUnparser::handleParameterList(SgInitializedNamePtrList& params)
  {
    typedef std::vector<SgVariableDeclaration*> parameter_decl_t;

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
      std::for_each(aa, zz, AdaParamUnparser(unparser, info, os));
      prn(")");
    }
  }


  std::set<std::string> adaOperatorNames()
  {
    std::string elems[] = { "+",   "-",   "*",  "/",   "**", "REM", "MOD", "ABS"
                          , "=",   "/=",  "<",  ">",   "<=", ">="
                          , "NOT", "AND", "OR", "XOR", "&"
                          };

    return std::set<std::string>(elems, elems + sizeof(elems) / sizeof(elems[0]));
  }

  bool isOperatorName(const std::string& id)
  {
    static std::set<std::string> adaops = adaOperatorNames();

    const std::string canonicalname = boost::to_upper_copy(id);

    return adaops.find(canonicalname) != adaops.end();
  }

  std::string convertOperatorNames(const std::string& name)
  {
    static const std::string cxxprefix = "operator";
    static const std::string quotes    = "\"";

    if (name.rfind(cxxprefix, 0) != 0)
      return name;

    const std::string op = name.substr(cxxprefix.size());

    if (!isOperatorName(op))
      return name;

    return quotes + op + quotes;
  }

  void
  AdaStatementUnparser::handleFunctionEntryDecl(SgFunctionDeclaration& n, std::string keyword, bool hasReturn)
  {
    std::string name = convertOperatorNames(n.get_name());

    prn(keyword);
    prn(" ");
    prn(name);

    handleParameterList( SG_DEREF(n.get_parameterList()).get_args() );

    if (hasReturn)
    {
      prn(" return");
      type(n.get_orig_return_type());
    }

    // MS 12/22/20 : if this is actually a function renaming declaration,
    // print the renaming syntax after the function/procedure declaration
    // and immediately return.
    SgAdaFunctionRenamingDecl* renaming = isSgAdaFunctionRenamingDecl(&n);
    if (renaming != nullptr)
    {
      prn(" renames ");
      prn(convertOperatorNames(renaming->get_renamed_function()->get_name()));
      prn(STMT_SEP);
      return;
    }

    SgFunctionDefinition* def = n.get_definition();

    if (!def)
    {
      prn(STMT_SEP);
      return;
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
    const bool endedInPublicMode = std::for_each(aa, zz, *this).publicMode;

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

  struct TypedeclSyntax : sg::DispatchHandler<std::pair<std::string, std::string> >
  {
    void handle(SgNode& n)         { SG_UNEXPECTED_NODE(n); }

    void handle(SgType&)           { res = ReturnType{"subtype", ""    }; }
    void handle(SgAdaFormalType&)  { res = ReturnType{"type",    ""    }; }
    void handle(SgAdaAccessType&)  { res = ReturnType{"type",    ""    }; }
    void handle(SgAdaDerivedType&) { res = ReturnType{"type",    " new"}; }
    void handle(SgAdaModularType&) { res = ReturnType{"type",    ""    }; }
    void handle(SgTypeDefault&)    { res = ReturnType{"type",    ""    }; }
    void handle(SgArrayType&)      { res = ReturnType{"type",    ""    }; }
    void handle(SgAdaFloatType&)   { res = ReturnType{"type",    ""    }; }

    void handle(SgAdaSubtype& n)
    {
      res = n.get_fromRootType() ? ReturnType{"type",    ""}
                                 : ReturnType{"subtype", ""};
    }
  };

  std::pair<std::string, std::string>
  AdaStatementUnparser::typedeclSyntax(SgType* n)
  {
    return sg::dispatch(TypedeclSyntax(), n);
  }

  struct RenamingDeclSyntax : sg::DispatchHandler<RenamingSyntax>
  {
    void handle(SgNode& n)      { SG_UNEXPECTED_NODE(n); }
/*
    void handle(SgDeclarationStatement& n)
    {
      static const s  td::string unknown("-- unknown todo");

      res = RenamingSyntax(unknown, unknown, unknown);
    }

    // band-aid until generic packages are supported
    void handle(SgImportStatement& n)
    {
      ROSE_ASSERT(idx == 0);

      res = RenamingSyntax("package ", "", nameOf(n));
    }
*/
    void handle(SgAdaRenamingSymbol& n)
    {
      SgAdaRenamingDecl& dcl = SG_DEREF(n.get_declaration());

      res = AdaStatementUnparser::renamingDeclSyntax(dcl.get_renamed());
      res.renamedName = n.get_name();
    }

    void handle(SgAdaPackageSymbol& n)
    {
      res = RenamingSyntax{"package ", "", n.get_name()};
    }

    void handle(SgVariableSymbol& n)
    {
      SgInitializedName& el = SG_DEREF(n.get_declaration());

      ROSE_ASSERT(SG_DEREF(isSgTypedefType(el.get_type())).get_name() == std::string{"Exception"});
      res = RenamingSyntax{"", ": exception", el.get_name()};
    }
  };

  RenamingSyntax
  AdaStatementUnparser::renamingDeclSyntax(SgSymbol* n)
  {
    return sg::dispatch(RenamingDeclSyntax{}, n);
  }

  struct UseClauseSyntax : sg::DispatchHandler<std::pair<std::string, std::string> >
  {
    void usepkg(const std::string& s)     { res = ReturnType{"", s}; }
    void usetype(const std::string& s)    { res = ReturnType{"type ", s}; }

    void handle(SgNode& n)                { SG_UNEXPECTED_NODE(n); }
    void handle(SgAdaPackageSpecDecl& n)  { usepkg(n.get_name()); }
    void handle(SgAdaPackageBodyDecl& n)  { usepkg(n.get_name()); }

    // band-aid until generic packages are supported
    void handle(SgImportStatement& n)     { usepkg(nameOf(n)); }

    void handle(SgTypedefDeclaration& n)  { usetype(n.get_name()); }
    void handle(SgAdaTaskTypeDecl& n)     { usetype(n.get_name()); }
    void handle(SgClassDeclaration& n)    { usetype(n.get_name()); }
    void handle(SgEnumDeclaration& n)     { usetype(n.get_name()); }
    void handle(SgAdaRenamingDecl& n)     { usetype(n.get_name()); }
  };

  std::pair<std::string, std::string>
  AdaStatementUnparser::useClauseSyntax(SgDeclarationStatement* n)
  {
    return sg::dispatch(UseClauseSyntax{}, n);
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

  void AdaStatementUnparser::stmt(SgStatement* s)
  {
    startPrivateIfNeeded(isSgDeclarationStatement(s));

    unparser.unparseAttachedPreprocessingInfo(s, info, PreprocessingInfo::before);
    unparser.unparseAttachedPreprocessingInfo(s, info, PreprocessingInfo::inside);
    sg::dispatch(*this, s);
    unparser.unparseAttachedPreprocessingInfo(s, info, PreprocessingInfo::after);
  }

  /*
  struct RecoverScopeName : sg::DispatchHandler<std::string>
  {
    void handle(SgNode& n)           { SG_UNEXPECTED_NODE(n); }

    void handle(SgAdaPackageSpecDecl& n) { res = n.get_name(); }
    void handle(SgAdaPackageBodyDecl& n) { res = n.get_name(); }
  };

  std::string AdaStatementUnparser::recoverScopeName(SgLocatedNode& n)
  {
    return sg::dispatch(RecoverScopeName(), &n);
  }
  */

  struct IsNamedScope : sg::DispatchHandler<std::pair<std::string, bool> >
  {
    void withName(const std::string& name);
    void withoutName();
    void checkParent(SgScopeStatement& n);

    void handle(SgNode& n)               { SG_UNEXPECTED_NODE(n); }

    void handle(SgStatement& n)
    {
      withoutName();
    }

    // scopes that may have names
    void handle(SgAdaTaskSpec& n)        { checkParent(n); }
    void handle(SgAdaTaskBody& n)        { checkParent(n); }
    void handle(SgAdaPackageSpec& n)     { checkParent(n); }
    void handle(SgAdaPackageBody& n)     { checkParent(n); }

    // parent handlers
    void handle(SgAdaTaskSpecDecl& n)    { withName(n.get_name()); }
    void handle(SgAdaTaskBodyDecl& n)    { withName(n.get_name()); }
    void handle(SgAdaPackageSpecDecl& n) { withName(n.get_name()); }
    void handle(SgAdaPackageBodyDecl& n) { withName(n.get_name()); }
  };

  void IsNamedScope::withName(const std::string& s)
  {
    res = std::make_pair(s, true);
  }

  void IsNamedScope::withoutName()
  {
    res = std::make_pair(std::string(), false);
  }

  void IsNamedScope::checkParent(SgScopeStatement& n)
  {
    res = sg::dispatch(*this, n.get_parent());
  }

  namespace
  {
    struct RootScope : sg::DispatchHandler<bool>
    {
      bool isStandardPkg(const SgAdaPackageSpec& n)
      {
        const SgAdaPackageSpecDecl& dcl = SG_DEREF(isSgAdaPackageSpecDecl(n.get_parent()));

        return (dcl.get_name() == "Standard") && isSgGlobal(dcl.get_parent());
      }

      void handle(const SgNode& n)           { SG_UNEXPECTED_NODE(n); }
      void handle(const SgScopeStatement&)   { res = false; }
      void handle(const SgGlobal&)           { res = true; }
      void handle(const SgAdaPackageSpec& n) { res = isStandardPkg(n); }
    };

    bool rootScope(const SgScopeStatement* n)
    {
      return sg::dispatch(RootScope{}, n);
    }
  };

  typedef std::vector<std::string> ScopePath;

  ScopePath
  pathToGlobal(SgScopeStatement& n)
  {
    // \todo
    // some scopes (e.g., procedures) have no names and thus the search
    // to global should not include scopes past this point.

    ScopePath         res;
    SgScopeStatement* curr = &n;

    while (!rootScope(curr))
    {
      std::pair<std::string, bool> data = sg::dispatch(IsNamedScope(), curr);

      if (data.second)
        res.push_back(data.first);

      curr = curr->get_scope();
      ASSERT_not_null(curr);
    }

    return res;
  }

  std::string
  AdaStatementUnparser::scopeQual(SgScopeStatement& remote)
  {
    SgScopeStatement& current = SG_DEREF(info.get_current_scope());

    return unparser.computeScopeQual(current, remote);
  }
}

std::string
Unparse_Ada::computeScopeQual(SgScopeStatement& local, SgScopeStatement& remote)
{
  typedef ScopePath::reverse_iterator PathIterator;

  ScopePath         localPath  = pathToGlobal(local);
  ScopePath         remotePath = pathToGlobal(remote);

/*
  std::cerr << "localPath " << typeid(local).name() << ": ";
  for (std::string x : localPath)
    std::cerr << x << '.';

  std::cerr << "\nremotePath: ";
  for (std::string x : remotePath)
    std::cerr << x << '.';

  std::cerr << std::endl;
*/

  size_t            pathlen    = std::min(localPath.size(), remotePath.size());
  PathIterator      localstart = localPath.rbegin();
  PathIterator      pathit     = std::mismatch( localstart, localstart + pathlen,
                                                remotePath.rbegin()
                                              ).second;
  std::stringstream qual;

  for (; pathit != remotePath.rend(); ++pathit)
  {
    qual << *pathit << '.';
  }

  return qual.str();
}

void
Unparse_Ada::unparseStatement(SgStatement* stmt, SgUnparse_Info& info)
{
  AdaStatementUnparser adaUnparser{*this, info, std::cerr};

  adaUnparser.stmt(stmt);
}

void
Unparse_Ada::unparseLanguageSpecificStatement(SgStatement* stmt, SgUnparse_Info& info)
{
  ASSERT_not_null(stmt);

  SG_UNEXPECTED_NODE(*stmt);
}



/*
void
Unparse_Ada::unparseStatement(SgStatement* stmt, SgUnparse_Info& info)
{
  base::unparseStatement(stmt, info);
}
*/
