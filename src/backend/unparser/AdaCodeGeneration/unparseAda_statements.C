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


#define ROSE_TRACK_PROGRESS_OF_ROSE_COMPILING_ROSE 0

#include "sage_support.h"
#include "sageGeneric.h"

#include <boost/algorithm/string.hpp>

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
  inline
  SgName nameOf(const SgSymbol& sy)
  {
    return sy.get_name();
  }

  inline
  SgName nameOf(const SgVarRefExp& var_ref)
  {
    return nameOf(SG_DEREF(var_ref.get_symbol()));
  }

  const std::string NO_SEP = "";
  const std::string COMMA_SEP = ", ";
  const std::string STMT_SEP = ";\n";
  const std::string PARAM_SEP = "; ";

  bool isAdaFunction(SgFunctionType& ty)
  {
    return isSgTypeVoid(ty.get_return_type()) == NULL;
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
    AdaEnumeratorUnparser(Unparse_Ada& unp, SgUnparse_Info& inf, std::ostream& outp)
    : AdaDetailsUnparser(unp, inf, outp)
    {}

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
    AdaParamUnparser(Unparse_Ada& unp, SgUnparse_Info& inf, std::ostream& outp)
    : AdaDetailsUnparser(unp, inf, outp)
    {}

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

      unparser.unparseType(primary.get_type(), &sg::ancestor<SgScopeStatement>(n), info);
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
    AdaElseUnparser(Unparse_Ada& unp, SgUnparse_Info& inf, std::ostream& outp)
    : AdaDetailsUnparser(unp, inf, outp)
    {}

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
    SgVariableDeclaration* var = isSgVariableDeclaration(ini->get_definition());

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

  bool declaredInMainFile(SgDeclarationStatement& dcl, const std::string& mainFile)
  {
    Sg_File_Info& fileInfo = SG_DEREF(dcl.get_startOfConstruct());

    return fileInfo.get_filenameString() == mainFile;
/*

    std::string mainFileUp = boost::to_upper_copy(mainFile);
    std::string fileNameUp = boost::to_upper_copy(fileInfo.get_filenameString());
    size_t      pos = mainFileUp.rfind(fileNameUp);

    std::cerr << (pos + fileNameUp.size()) << " =?= " << mainFileUp.size()
              << std::endl;

    return (  (pos != std::string::npos)
           && (pos + fileNameUp.size() == mainFileUp.size())
           );
*/
  }

  std::pair<SgDeclarationStatementPtrList::iterator, SgDeclarationStatementPtrList::iterator>
  declsInPackage(SgDeclarationStatementPtrList& lst, const std::string& mainFile)
  {
    SgDeclarationStatementPtrList::iterator aa = lst.begin();
    SgDeclarationStatementPtrList::iterator zz = lst.end();

    // skip over imported packages
    while (aa != zz && !declaredInMainFile(SG_DEREF(*aa), mainFile))
      ++aa;

    return std::make_pair(aa, zz);
  }


  struct AdaStatementUnparser : AdaDetailsUnparser
  {
    AdaStatementUnparser(Unparse_Ada& unp, SgUnparse_Info& inf, std::ostream& outp)
    : AdaDetailsUnparser(unp, inf, outp), publicMode(true)
    {}

    template <class SageStmtList>
    void list(SageStmtList& lst);

    template <class ForwardIterator>
    void list(ForwardIterator aa, ForwardIterator zz);

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
    renamingDeclSyntax(SgDeclarationStatement* n, size_t idx);

    static
    std::pair<std::string, std::string>
    useClauseSyntax(SgDeclarationStatement*);

    void startPrivateIfNeeded(SgDeclarationStatement* n);

    //
    // handlers

    void handle(SgNode& n)      { SG_UNEXPECTED_NODE(n); }

    void handle(SgStatement& n);

    void handle(SgGlobal& n)
    {
      typedef SgDeclarationStatementPtrList::iterator Iterator;

      std::pair<Iterator, Iterator> declRange = declsInPackage(n.get_declarations(), unparser.getFileName());

      list(declRange.first, declRange.second);
    }

    void handle(SgNullStatement& n)
    {
      prn("null");
      prn(STMT_SEP);
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
      prn("begin\n");

      stmt(n.get_definition());

      prn("end ");
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
      ROSE_ASSERT(n.get_hasMembers());

      list(n.get_declarations());
    }

    void handle(SgAdaTaskBody& n)
    {
      list(n.get_statements());
    }

    void handle(SgAdaPackageSpecDecl& n)
    {
      const std::string qual = scopeQual(n, n.get_scope());

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
      list(n.get_declarations());
    }

    void handle(SgAdaPackageBody& n)
    {
      list(n.get_statements());
    }

    void handle(SgAdaRenamingDecl& n)
    {
      SgDeclarationStatement* orig    = n.get_renamedDecl();
      RenamingSyntax          renamed = renamingDeclSyntax(orig, n.get_renamedIndex());

      prn(renamed.prefixSyntax);
      prn(n.get_name());
      prn(renamed.infixSyntax);
      prn(" renames ");
      prn(scopeQual(n, orig->get_scope()));
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
      prn(scopeQual(n, orig->get_scope()));
      prn(usesyntax.second);
      prn(STMT_SEP);
    }

    void handle(SgTypedefDeclaration& n)
    {
      std::pair<std::string, std::string> declwords = typedeclSyntax(n.get_base_type());

      prn(declwords.first);
      prn(" ");
      prn(n.get_name());
      prn(" is");
      prn(declwords.second);
      prn(" ");
      type(n.get_base_type(), n);
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

      type(primary.get_type(), n);
      expr_opt(primary.get_initializer(), " := ");
      prn(STMT_SEP);
    }

    void handle(SgFunctionDefinition& n)
    {
      handleBasicBlock(n.get_body(), true /* function body */);
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

      const bool isReverse = isSgMinusOp(n.get_increment());

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

      if (isSgNullExpression(n.get_condition()) == NULL)
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
      SgClassType& rec = SG_DEREF(n.get_recordType());

      prn("for ");
      prn(rec.get_name());
      prn(" use record\n");
      expr_opt(n.get_alignment(), "at mod ", STMT_SEP);
      list(n.get_components());
      prn("end record");
      prn(STMT_SEP);
    }


    //~ ScopePath pathToGlobal(SgStatement& n);
    //~ std::string recoverScopeName(SgLocatedNode& n);

    std::string scopeQual(SgStatement& local, SgScopeStatement& remote);

    std::string scopeQual(SgStatement& local, SgScopeStatement* remote)
    {
      return scopeQual(local, SG_DEREF(remote));
    }

    void parentRecord(SgClassDefinition& def)
    {
      SgBaseClassPtrList& parents = def.get_inheritances();

      if (parents.size() == 0) return;

      SgBaseClass&        parent = SG_DEREF(parents.at(0));
      SgClassDeclaration& decl   = SG_DEREF(parent.get_base_class());

      prn(" new ");
      prn(scopeQual(def, decl.get_scope()));
      prn(decl.get_name());
      prn(" with");
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
        SgDeclarationModifier& mod = n.get_declarationModifier();

        prn(" is");

        if (!explicitNullrec) parentRecord(*def);

        if (mod.isAdaAbstract()) prn(" abstract");
        if (mod.isAdaLimited())  prn(" limited");
        if (mod.isAdaTagged())   prn(" tagged");

        if (explicitNullrec) prn(" null");
        prn(" record");

        if (!explicitNullrec)
        {
          prn("\n");
          stmt(def);
          prn("end record");
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
      list(n.get_members());
    }

    void handle(SgTryStmt& n)
    {
      // skip the block, just print the statements
      SgBasicBlock& blk = SG_DEREF(isSgBasicBlock(n.get_body()));

      list(blk.get_statements());
      prn("exception\n");
      stmt(n.get_catch_statement_seq_root());
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

      type(exvar.get_type(), n);
      prn(" =>\n");

      stmt(n.get_body());
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

    void type(SgType* t, SgStatement& ctx)
    {
      unparser.unparseType(t, &sg::ancestor<SgScopeStatement>(ctx), info);
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

  bool isNormalStatement(const SgStatement* s)
  {
    return isSgDeclarationStatement(s) == NULL;
  }

  void AdaStatementUnparser::handle(SgStatement& n)
  {
    // if not handled here, have the language independent parser handle it..
    unparser.UnparseLanguageIndependentConstructs::unparseStatement(&n, info);
  }

  void AdaStatementUnparser::handleBasicBlock(SgBasicBlock& n, bool functionbody)
  {
    SgStatementPtrList&          stmts    = n.get_statements();
    SgStatementPtrList::iterator aa       = stmts.begin();
    SgStatementPtrList::iterator zz       = stmts.end();
    SgStatementPtrList::iterator dcllimit = std::find_if(aa, zz, isNormalStatement);

    if (!functionbody && (aa != dcllimit))
      prn("declare\n");

    const std::string label = n.get_string_label();
    const bool        requiresBeginEnd = (functionbody || (aa != dcllimit) || label.size());

    list(stmts.begin(), dcllimit);

    if (requiresBeginEnd)
      prn("begin\n");

    list(dcllimit, stmts.end());

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
    std::string elems[] = { "+",   "-",   "*",  "/",   "**", "rem", "mod", "abs"
                          , "=",   "/=",  "<",  ">",   "<=", ">="
                          , "not", "and", "or", "xor", "&"
                          };

    return std::set<std::string>(elems, elems + sizeof(elems) / sizeof(elems[0]));
  }

  bool isOperatorName(const std::string& id)
  {
    static std::set<std::string> adaops = adaOperatorNames();

    return adaops.find(id) != adaops.end();
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
      type(n.get_orig_return_type(), n);
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
    prn(n.get_name());
    prn(STMT_SEP);
  }

  template <class ForwardIterator>
  void AdaStatementUnparser::list(ForwardIterator aa, ForwardIterator zz)
  {
    std::for_each(aa, zz, *this);
  }

  template <class SageStmtList>
  void AdaStatementUnparser::list(SageStmtList& lst)
  {
    list(lst.begin(), lst.end());
  }

  struct TypedeclSyntax : sg::DispatchHandler<std::pair<std::string, std::string> >
  {
    void handle(SgNode& n)         { SG_UNEXPECTED_NODE(n); }

    void handle(SgType&)           { res = ReturnType("type",    " new"); }
    void handle(SgAdaSubtype&)     { res = ReturnType("subtype", ""); }
    void handle(SgAdaModularType&) { res = ReturnType("type",    ""); }
    void handle(SgTypeDefault&)    { res = ReturnType("type",    ""); }
    void handle(SgArrayType&)      { res = ReturnType("type",    ""); }
  };

  std::pair<std::string, std::string>
  AdaStatementUnparser::typedeclSyntax(SgType* n)
  {
    return sg::dispatch(TypedeclSyntax(), n);
  }

  struct RenamingDeclSyntax : sg::DispatchHandler<RenamingSyntax>
  {
    typedef sg::DispatchHandler<RenamingSyntax> base;

    explicit
    RenamingDeclSyntax(size_t i)
    : base(), idx(i)
    {}

    void handle(SgNode& n)      { SG_UNEXPECTED_NODE(n); }
/*
    void handle(SgDeclarationStatement& n)
    {
      static const s  td::string unknown("-- unknown todo");

      res = RenamingSyntax(unknown, unknown, unknown);
    }
*/
    void handle(SgImportStatement& n)
    {
      ROSE_ASSERT(idx == 0);

      SgExpressionPtrList& lst = n.get_import_list();
      //~ ROSE_ASSERT(lst.size() != 0);
      ROSE_ASSERT(lst.size() == 1);

      std::string renamed = nameOf(SG_DEREF(isSgVarRefExp(lst.back())));

      res = RenamingSyntax("package ", "", renamed);
    }

    void handle(SgAdaPackageSpecDecl& n)
    {
      ROSE_ASSERT(idx == 0);

      res = RenamingSyntax("package ", "", n.get_name());
    }

    void handle(SgAdaPackageBodyDecl& n)
    {
      ROSE_ASSERT(idx == 0);

      res = RenamingSyntax("package ", "", n.get_name());
    }

    void handle(SgVariableDeclaration& n)
    {
      SgInitializedName& el = SG_DEREF(n.get_variables().at(idx));

      ROSE_ASSERT(SG_DEREF(isSgTypedefType(el.get_type())).get_name() == std::string("Exception"));
      res = RenamingSyntax("", ": exception", el.get_name());
    }

    const size_t idx;
  };

  RenamingSyntax
  AdaStatementUnparser::renamingDeclSyntax(SgDeclarationStatement* n, size_t idx)
  {
    return sg::dispatch(RenamingDeclSyntax(idx), n);
  }

  struct UseClauseSyntax : sg::DispatchHandler<std::pair<std::string, std::string> >
  {
    void usepkg(const std::string& s)     { res = ReturnType("", s); }
    void usetype(const std::string& s)    { res = ReturnType("type ", s); }

    void handle(SgNode& n)                { SG_UNEXPECTED_NODE(n); }
    void handle(SgAdaPackageSpecDecl& n)  { usepkg(n.get_name()); }
    void handle(SgAdaPackageBodyDecl& n)  { usepkg(n.get_name()); }
    void handle(SgTypedefDeclaration& n)  { usetype(n.get_name()); }
    void handle(SgAdaTaskTypeDecl& n)     { usetype(n.get_name()); }
    void handle(SgClassDeclaration& n)    { usetype(n.get_name()); }
  };

  std::pair<std::string, std::string>
  AdaStatementUnparser::useClauseSyntax(SgDeclarationStatement* n)
  {
    return sg::dispatch(UseClauseSyntax(), n);
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

    sg::dispatch(*this, s);
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

  typedef std::vector<std::string> ScopePath;

  ScopePath
  pathToGlobal(SgScopeStatement& n)
  {
    // \todo
    // some scopes (e.g., procedures) have no names and thus the search
    // to global should not include scopes past this point.

    ScopePath         res;
    SgScopeStatement* curr = &n;

    while (!isSgGlobal(curr))
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
  AdaStatementUnparser::scopeQual(SgStatement& local, SgScopeStatement& remote)
  {
    return unparser.computeScopeQual(sg::ancestor<SgScopeStatement>(local), remote);
  }
}

std::string
Unparse_Ada::computeScopeQual(SgScopeStatement& local, SgScopeStatement& remote)
{
  typedef ScopePath::reverse_iterator PathIterator;

  ScopePath         localPath  = pathToGlobal(local);
  ScopePath         remotePath = pathToGlobal(remote);
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
  //~ std::cerr << typeid(*stmt).name() << std::endl;
  sg::dispatch(AdaStatementUnparser(*this, info, std::cerr), stmt);
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
