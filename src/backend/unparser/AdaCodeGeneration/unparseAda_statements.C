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
    AdaStatementUnparser(Unparse_Ada& unp, SgUnparse_Info& inf, std::ostream& outp)
    : unparser(unp), info(inf), os(outp)
    {}

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
    
    void handle(SgAdaPackageSpecDecl& n)
    {
      prn("package ");
      prn(n.get_name());
      prn(" is\n");
      
      stmt(n.get_definition());
      
      prn("end ");
      prn(n.get_name());
      prn(";\n");
    }
    
    void handle(SgAdaPackageSpec& n)
    {
      SgDeclarationStatementPtrList& lst = n.get_declarations();
      
      std::for_each(lst.begin(), lst.end(), *this);
    }
    
    void handle(SgTypedefDeclaration& n)
    {
      prn("type\n");
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
      
      type(first->get_type());
      
      if (SgExpression* exp = first->get_initializer())
      {
        prn(" := ");
        expr(exp);
      }
      
      prn(";\n");
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
      prn(";\n");
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
      prn(";\n");
    }
    
    void handle(SgAdaLoopStmt& n) 
    { 
      prn("loop\n");
      stmt(n.get_body());
      prn("end loop");
      handleStringLabel(n.get_string_label());
      prn(";\n");
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
      
      prn(";\n");
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
      
      prn(";\n");
    }
    
    void handle(SgExprStatement& n) 
    {
      expr(n.get_expression());
      prn(";\n");
    }
    
    void handle(SgBasicBlock& n)
    {
      handleBasicBlock(n);
    }
    
    void handle(SgFunctionDeclaration& n) 
    {
      typedef std::vector<SgVariableDeclaration*> parameter_decl_t;
      
      SgFunctionType&            ty     = SG_DEREF(n.get_type());
      const bool                 isFunc = isAdaFunction(ty);
      parameter_decl_t           paramdecls;
      SgInitializedNamePtrList&  params = n.get_parameterList()->get_args();
      
      // Since SgFunctionParameterScope (and SgFunctionDefinition) do not allow
      //   traversing the function parameter declarations, they are collected
      //   from initialized names.
      
      std::transform( params.begin(), params.end(), 
                      std::back_inserter(paramdecls),
                      variableDeclaration
                    );
      
      parameter_decl_t::iterator aa = paramdecls.begin();             
      parameter_decl_t::iterator zz = std::unique(aa, paramdecls.end());
      
      prn(isFunc ? "function" : "procedure");
      prn(" ");
      prn(n.get_name());
      
      // print parenthesis only if parameters were present
      if (aa != zz)
      {
        prn("(");      
        std::for_each(aa, zz, AdaParamUnparser(unparser, info, os, "" /* initial sep */));
        prn(")");
      }
      
      if (isFunc)
      {
        prn(" return");
        type(n.get_orig_return_type());
      }
      
      SgFunctionDefinition* def = n.get_definition();
             
      if (!def)
      {
        prn(";\n");
        return;
      }
      
      prn(" is\n");
      stmt(def);
      
      prn(" ");
      prn(n.get_name());
      prn(";\n");
    }
    
    void stmt(SgStatement* s)
    {
      sg::dispatch(*this, s);
    }
    
    void expr(SgExpression* e)
    {
      unparser.unparseExpression(e, info);
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
    
    std::for_each(stmts.begin(), dcllimit, *this);
    
    if (functionbody || (aa != dcllimit))
      prn("begin\n");
      
    std::for_each(dcllimit, stmts.end(), *this);
    
    if (functionbody || (aa != dcllimit))
    {
      prn("end");
      
      if (!functionbody)
        prn(";\n");
    }
  }  
  
  
  void AdaStatementUnparser::handleStringLabel(const std::string& s)
  {
    if (s.size() == 0) return;
    
    prn(" ");
    prn(s);
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
