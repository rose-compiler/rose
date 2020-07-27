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
  const char* COMMA_SEP = ", ";
  const char* EOS_NL = ";\n";
  
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
  
  struct AdaParamUnparser
  {
    AdaParamUnparser(Unparse_Ada& unp, SgUnparse_Info& inf, std::ostream& outp, const char* separator)
    : unparser(unp), info(inf), os(outp), sep(separator)
    {}
    
    void prn(const std::string& s)
    {
      unparser.curprint(s);
      os << s;
    }
    
    void handle(SgVariableDeclaration& n)
    {
      SgInitializedNamePtrList& params = n.get_variables();
      SgInitializedName*        first  = NULL;
      
      //~ for (SgInitializedName* ini : params)
      for (size_t i = 0; i < params.size(); ++i)
      {
        SgInitializedName* ini = params[i];
        ROSE_ASSERT(ini);
        
        if (first == NULL) 
          first = ini; 
        else 
          prn(COMMA_SEP);
          
        prn(ini->get_name());
      }
      
      prn(": ");
      unparseModifiers(*this, n);
      
      ASSERT_not_null(first);
      unparser.unparseType(first->get_type(), info);
    }
    
    void operator()(SgVariableDeclaration* s) 
    {
      prn(sep);
      handle(SG_DEREF(s));
      sep = COMMA_SEP;
    }

    Unparse_Ada&    unparser;
    SgUnparse_Info& info;
    std::ostream&   os;
    const char*     sep;
  };

  template <class AstVisitor>  
  void unparseElseBranch(AstVisitor vis, SgStatement* elseBranch)
  {
    if (!elseBranch) return;
    
    sg::dispatch(vis, elseBranch);
  }

  
  struct AdaElseUnparser
  {
    AdaElseUnparser(Unparse_Ada& unp, SgUnparse_Info& inf, std::ostream& outp)
    : unparser(unp), info(inf), os(outp)
    {}
    
    void prn(const std::string& s)
    {
      unparser.curprint(s);
      os << s;
    }
    
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
      prn("null;\n");
    }
    
    Unparse_Ada&    unparser;
    SgUnparse_Info& info;
    std::ostream&   os;
  };
  
 
  SgVariableDeclaration* variableDeclaration(SgInitializedName* ini)
  {
    ROSE_ASSERT(ini);
    SgVariableDeclaration* var = isSgVariableDeclaration(ini->get_definition());

    ROSE_ASSERT(var);                        
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
  
  struct AdaStatementUnparser
  {
    typedef std::vector<std::string> ScopePath;
    
    AdaStatementUnparser(Unparse_Ada& unp, SgUnparse_Info& inf, std::ostream& outp)
    : unparser(unp), info(inf), os(outp), publicMode(true)
    {}
    
    template <class SageStmtList>
    void list(SageStmtList& lst);

    template <class ForwardIterator>
    void list(ForwardIterator aa, ForwardIterator zz);

    void prn(const std::string& s)
    {
      unparser.curprint(s);
      os << s;
    }
    
    void handleBasicBlock(SgBasicBlock& n, bool functionbody = false);
    
    void handleBasicBlock(SgBasicBlock* n, bool functionbody = false)
    {
      handleBasicBlock(SG_DEREF(n), functionbody);
    }
        
    void handleStringLabel(const std::string& s);
    
    void handleFunctionEntryDecl(SgFunctionDeclaration&, std::string keyword, bool hasReturn = false);
    
    void handleParameterList(SgInitializedNamePtrList& params);
    
    bool requiresNew(SgType* n);
    void startPrivateIfNeeded(SgDeclarationStatement* n);
    
    //
    // handlers 
    
    void handle(SgNode& n)      { SG_UNEXPECTED_NODE(n); }
    
    void handle(SgStatement& n) 
    {
      // if not handled here, have the language independent parser handle it..
      unparser.UnparseLanguageIndependentConstructs::unparseStatement(&n, info);  
    }
    
    void handle(SgNullStatement& n) 
    {
      prn("null;\n");
    }
    
    void handle(SgAdaTaskTypeDecl& n)
    {
      prn("task type ");
      prn(n.get_name());
      
      SgAdaTaskSpec& spec = SG_DEREF(n.get_definition());
      
      if (!spec.get_hasMembers())
      {
        prn(EOS_NL);
        return;
      }  
      
      prn(" is\n");
      stmt(&spec);
      
      prn("end ");
      prn(n.get_name());
      prn(EOS_NL);
    }
    
    void handle(SgAdaTaskBodyDecl& n)
    {
      prn("task body ");
      prn(n.get_name());
      prn(" is\n");
      
      stmt(n.get_definition());
      
      prn("end ");
      prn(n.get_name());
      prn(EOS_NL);
    }
    
    void handle(SgAdaTaskSpecDecl& n)
    {
      prn("task ");
      prn(n.get_name());
      
      SgAdaTaskSpec& spec = SG_DEREF(n.get_definition());
      
      if (!spec.get_hasMembers())
      {
        prn(EOS_NL);
        return;
      }  
      
      prn(" is\n");
      stmt(&spec);
      
      prn("end ");
      prn(n.get_name());
      prn(EOS_NL);
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
      prn("package ");
      prnPrefix(n, SG_DEREF(n.get_scope()));
      prn(n.get_name());      
      prn(" is\n");
      
      stmt(n.get_definition());
      
      prn("end ");
      prnPrefix(n, SG_DEREF(n.get_scope()));
      prn(n.get_name());
      prn(EOS_NL);
    }
    
    void handle(SgAdaPackageBodyDecl& n)
    {
      prn("package body ");
      prn(n.get_name());
      prn(" is\n");
      
      stmt(n.get_definition());
      
      prn("end ");
      prn(n.get_name());
      prn(EOS_NL);
    }
    
    void handle(SgAdaPackageSpec& n)
    {
      list(n.get_declarations());
    }
    
    void handle(SgAdaPackageBody& n)
    {
      list(n.get_statements());
    }
    
    void handle(SgTypedefDeclaration& n)
    {
      prn("type ");
      prn(n.get_name());
      prn(" is");
      
      if (requiresNew(n.get_base_type())) prn(" new");
      
      type(n.get_base_type());
      prn(EOS_NL);
    }
    
    void handle(SgVariableDeclaration& n)
    {
      SgInitializedNamePtrList& params = n.get_variables();
      SgInitializedName*        first  = NULL;
      
      //~ for (SgInitializedName* ini : params)
      for (size_t i = 0; i < params.size(); ++i)
      {
        SgInitializedName* ini = params[i];
        ASSERT_not_null(ini);
        
        if (first == NULL) 
          first = ini; 
        else 
          prn(COMMA_SEP);
          
        ASSERT_not_null(ini);
        prn(ini->get_name());
      }

      prn(": ");
      unparseModifiers(*this, n);
      
      type(first->get_type());
      
      if (SgExpression* exp = first->get_initializer())
      {
        prn(" := ");
        expr(exp);
      }
      
      prn(EOS_NL);
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
      
      prn("end if;\n");
    }
    
    void handle(SgWhileStmt& n)
    {
      prn("while ");
      expr(underlyingExpr(n.get_condition()));
      prn(" loop\n");
      stmt(n.get_body());
      prn("end loop");
      handleStringLabel(n.get_string_label());
      prn(EOS_NL);
    }
    
    void handle(SgForInitStatement& n)
    {
      SgStatementPtrList&    stmts = n.get_init_stmt();
      ROSE_ASSERT(stmts.size() == 1);
      
      SgVariableDeclaration& dcl = SG_ASSERT_TYPE(SgVariableDeclaration, SG_DEREF(stmts[0]));
      ROSE_ASSERT(dcl.get_variables().size() == 1);
      
      SgInitializedName&     ini = SG_DEREF(dcl.get_variables()[0]);
      
      prn(ini.get_name());
      prn(" in ");
      
      if (SgExpression* range = ini.get_initializer())
        expr(range);
      else
        prn("<missing>");
        
      prn(" ");
    }
    
    void handle(SgForStatement& n)
    {
      prn("for ");
      stmt(n.get_for_init_stmt());
      prn(" loop\n");
      stmt(n.get_loop_body());
      prn("end loop");
      handleStringLabel(n.get_string_label());
      prn(EOS_NL);
    }
    
    void handle(SgAdaLoopStmt& n) 
    { 
      prn("loop\n");
      stmt(n.get_body());
      prn("end loop");
      handleStringLabel(n.get_string_label());
      prn(EOS_NL);
    }
    
    void handle(SgAdaAcceptStmt& n)
    {
      prn("accept ");
      expr(n.get_entry());
      prn(" ");
      expr_opt(n.get_index());
      
      handleParameterList( SG_DEREF(n.get_parameterList()).get_args() );
      
      SgStatement* body = n.get_body();
      
      if (SgBasicBlock* block = isSgBasicBlock(body))
      {
        prn("do\n");
        handleBasicBlock(*block);
        prn("end ");
        expr(n.get_entry());
      }   
      
      prn(EOS_NL);
    }
    
    void handle(SgLabelStatement& n)
    {
      prn(n.get_label());
      prn(": ");
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
      
      prn(EOS_NL);
    }
    
    void handle(SgImportStatement& n)
    {
      prn("with ");
      
      SgExpressionPtrList& lst = n.get_import_list();
      ROSE_ASSERT(lst.size() != 0);
      
      for (size_t i = 0; i < lst.size()-1; ++i)
      {
        expr(lst[i]);
        prn(".");
      }
      
      expr(lst.back());
      prn(EOS_NL);
    }
    
    void handle(SgSwitchStatement& n) 
    { 
      prn("case ");
      expr(underlyingExpr(n.get_item_selector()));
      prn(" is\n");
      stmt(n.get_body());
      prn("end case;\n");
    }
    
    void handleCaseWhenExp(SgExpression* n)
    {
      if (isSgNullExpression(n))
      {
        prn("others");
        return;
      }
      
      expr(n);
    }
    
    void handle(SgCaseOptionStmt& n)
    {
      prn("when ");
      handleCaseWhenExp(n.get_key());
      prn(" => ");
      stmt(n.get_body());
      prn("\n");      
    }
    
    void handle(SgReturnStmt& n) 
    { 
      prn("return");
      
      if (SgExpression* exp = n.get_expression())
      {
        prn(" ");
        expr(exp);
      }
      
      prn(EOS_NL);
    }
    
    void handle(SgExprStatement& n) 
    {
      expr(n.get_expression());
      prn(EOS_NL);
    }
    
    void handle(SgBasicBlock& n)
    {
      handleBasicBlock(n);
    }
    
    ScopePath pathToGlobal(SgStatement& n);
    //~ std::string recoverScopeName(SgLocatedNode& n);
    
    void prnPrefix(SgStatement& local, SgStatement& remote);
    
    void parentRecord(SgClassDefinition& def)
    {
      SgBaseClassPtrList& parents = def.get_inheritances();
      
      if (parents.size() == 0) return;
      
      SgBaseClass&        parent = SG_DEREF(parents.at(0));
      SgClassDeclaration& decl   = SG_DEREF(parent.get_base_class());
      
      prn(" new ");
      prnPrefix(def, decl);
      prn(decl.get_name());
    }
    
    void handle(SgClassDeclaration& n)
    {
      prn("type ");
      prn(n.get_name());
      
      if (SgClassDefinition* def = n.get_definition()) 
      {
        SgDeclarationModifier& mod = n.get_declarationModifier();
          
        prn(" is");
        
        parentRecord(*def);
        
        if (mod.isAdaAbstract()) prn(" abstract");
        if (mod.isAdaLimited())  prn(" limited");
        if (mod.isAdaTagged())   prn(" tagged");
        
        prn(" record\n");
        stmt(def);
        prn("end record");
      }
      
      prn(EOS_NL);
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
      
      type(exvar.get_type());
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
    
    void expr_opt(SgExpression* e)
    {
      if (!e || isSgNullExpression(e)) return;
      
      expr(e);
    }
    
    void type(SgType* t)
    {
      unparser.unparseType(t, info);
    }
    
    void operator()(SgStatement* s) 
    {
      stmt(s);
    }
    
    Unparse_Ada&    unparser;
    SgUnparse_Info& info;
    std::ostream&   os;
    bool            publicMode;
  };
  
  bool isNormalStatement(const SgStatement* s)
  {
    return isSgDeclarationStatement(s) == NULL;
  }
  
  void AdaStatementUnparser::handleBasicBlock(SgBasicBlock& n, bool functionbody)
  {
    SgStatementPtrList&          stmts    = n.get_statements();
    SgStatementPtrList::iterator aa       = stmts.begin();
    SgStatementPtrList::iterator zz       = stmts.end();
    SgStatementPtrList::iterator dcllimit = std::find_if(aa, zz, isNormalStatement); 
                                                       
    if (!functionbody && (aa != dcllimit))
      prn("declare\n");               
    
    list(stmts.begin(), dcllimit);
    
    if (functionbody || (aa != dcllimit))
      prn("begin\n");
      
    list(dcllimit, stmts.end());
    
    if (functionbody || (aa != dcllimit))
    {
      prn("end");
      
      if (!functionbody)
        prn(EOS_NL);
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
      std::for_each(aa, zz, AdaParamUnparser(unparser, info, os, "" /* initial sep */));
      prn(")");
    }
  }

  
  void 
  AdaStatementUnparser::handleFunctionEntryDecl(SgFunctionDeclaration& n, std::string keyword, bool hasReturn)
  {
    prn(keyword);
    prn(" ");
    prn(n.get_name());

    handleParameterList( SG_DEREF(n.get_parameterList()).get_args() );
    
    if (hasReturn)
    {
      prn(" return");
      type(n.get_orig_return_type());
    }
    
    SgFunctionDefinition* def = n.get_definition();
           
    if (!def)
    {
      prn(EOS_NL);
      return;
    }
    
    prn(" is\n");
    stmt(def);
    
    prn(" ");
    prn(n.get_name());
    prn(EOS_NL);
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
  
  bool AdaStatementUnparser::requiresNew(SgType* n)
  {
    return isSgTypeDefault(n) == NULL;
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
    
    void handle(SgNode& n)               { SG_UNEXPECTED_NODE(n); }
    
    void handle(SgLocatedNode& n)        { withoutName(); }
    
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
  
  AdaStatementUnparser::ScopePath
  AdaStatementUnparser::pathToGlobal(SgStatement& n)
  {
    ScopePath res;
    
    if (isSgGlobal(&n)) return res;
    
    SgNode*   curr = n.get_parent();
    
    ROSE_ASSERT(curr);
    while (!isSgGlobal(curr)) 
    {
      std::pair<std::string, bool> data = sg::dispatch(IsNamedScope(), curr);
      
      if (data.second) 
        res.push_back(data.first);
        
      curr = curr->get_parent();
      ROSE_ASSERT(curr);
    }
    
    return res;
  }
  
  void 
  AdaStatementUnparser::prnPrefix(SgStatement& local, SgStatement& remote)
  {
    typedef ScopePath::reverse_iterator PathIterator;
    
    ScopePath    localPath  = pathToGlobal(local);
    ScopePath    remotePath = pathToGlobal(remote);
    size_t       pathlen = std::min(localPath.size(), remotePath.size());
    PathIterator pathit = std::mismatch( localPath.rbegin(), localPath.rbegin() + pathlen,
                                         remotePath.rbegin() 
                                       ).second;
                 
    for (; pathit != remotePath.rend(); ++pathit)
    {
      prn(*pathit);
      prn(".");
    }
  }
}

void
Unparse_Ada::unparseStatement(SgStatement* stmt, SgUnparse_Info& info)
{
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
