/* unparseX10_statements.C
 * Contains functions that unparse statements
 *
 * FORMATTING WILL BE DONE IN TWO WAYS:
 * 1. using the file_info object to get information from line and column number 
 *    (for original source code)
 * 2. following a specified format that I have specified with indentations of
 *    length TABINDENT (for transformations)
 * 
 * REMEMBER: For types and symbols, we still call the original unparse function 
 * defined in sage since they dont have file_info. For expressions, 
 * Unparse_X10::unparse is called, and for statements, 
 * Unparse_X10::unparseStatement is called.
 *
 */
#include "sage3basic.h"
#include "unparser.h"


#define ROSE_TRACK_PROGRESS_OF_ROSE_COMPILING_ROSE 0

#include "sage_support.h"
#include "sageGeneric.h"

// #ifdef ROSE_BUILD_X10_LANGUAGE_SUPPORT
// using namespace Rose::Frontend::X10::X10c;
// #endif

#define OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES 0
#define OUTPUT_DEBUGGING_FUNCTION_INTERNALS  0
#define OUTPUT_DEBUGGING_UNPARSE_INFO        0

// Output the class name and function names as we unparse (for debugging)
#define OUTPUT_DEBUGGING_CLASS_NAME    0
#define OUTPUT_DEBUGGING_FUNCTION_NAME 0
#define OUTPUT_HIDDEN_LIST_DATA 0


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
  
  struct AdaParamUnparser
  {
    void prn(const std::string& s)
    {
      unparser.curprint(s);
      os << s;
    }
    
    void handle(SgVariableDeclaration& n)
    {
      SgInitializedNamePtrList& params = n.get_variables();
      SgInitializedName*        first  = nullptr;
      
      for (SgInitializedName* ini : params)
      {
        ROSE_ASSERT(ini);
        
        if (first == nullptr) 
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
  
  
  struct AdaStatementUnparser
  {
    void prn(const std::string& s)
    {
      unparser.curprint(s);
      os << s;
    }
    
    void handle(SgNode& n) { SG_UNEXPECTED_NODE(n); }
    
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
    
    void handleBasicBlock(SgBasicBlock& n, bool functionbody = false);
    
    void handleBasicBlock(SgBasicBlock* n, bool functionbody = false)
    {
      handleBasicBlock(SG_DEREF(n), functionbody);
    }
    
    void handle(SgVariableDeclaration& n)
    {
      SgInitializedNamePtrList& params = n.get_variables();
      SgInitializedName*        first  = nullptr;
      
      for (SgInitializedName* ini : params)
      {
        ROSE_ASSERT(ini);
        
        if (first == nullptr) 
          first = ini; 
        else 
          prn(COMMA_SEP);
          
        prn(ini->get_name());
      }
      
      prn(": ");
      unparseModifiers(*this, n);
      
      unparser.unparseType(first->get_type(), info);
      prn(";\n");
    }
    
    void handle(SgFunctionDefinition& n)
    {
      handleBasicBlock(n.get_body(), true /* function body */);
      //~ unparser.unparseStatement(n.get_body(), info);
    }
    
    void handle(SgIfStmt& n) 
    { 
      prn("if ");
      unparser.unparseExpression(underlyingExpr(n.get_conditional()), info);
      prn(" then\n");
      stmt(n.get_true_body());
      
      unparseElseBranch(AdaElseUnparser{unparser, info, os}, n.get_false_body());
      
      prn("end if;\n");
    }
    
    void handle(SgSwitchStatement& n) 
    { 
      prn("case ");
      unparser.unparseExpression(underlyingExpr(n.get_item_selector()), info);
      prn(" is\n");
      stmt(n.get_body());
      prn("end case;\n");
    }
    
    void handleWhenExp(SgExpression* n)
    {
      if (isSgNullExpression(n))
      {
        prn("others");
        return;
      }
      
      unparser.unparseExpression(n, info);
    }
    
    void handle(SgCaseOptionStmt& n)
    {
      prn("when ");
      handleWhenExp(n.get_key());
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
        unparser.unparseExpression(exp, info);
      }
      
      prn(";\n");
    }
    
    void handle(SgExprStatement& n) 
    {
      unparser.unparseExpression(n.get_expression(), info);
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
                      [](SgInitializedName* ini) -> SgVariableDeclaration*
                      {
                        ROSE_ASSERT(ini);
                        SgVariableDeclaration* var = isSgVariableDeclaration(ini->get_definition());

                        ROSE_ASSERT(var);                        
                        return var;
                      } 
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
        std::for_each(aa, zz, AdaParamUnparser{unparser, info, os, "" /* initial sep */});
        prn(")");
      }
      
      if (isFunc)
      {
        prn(" return");
        unparser.unparseType(n.get_orig_return_type(), info);
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
      unparser.unparseStatement(s, info);
    }
    
    void operator()(SgStatement* s) 
    {
      stmt(s);
    }
    
    Unparse_Ada&    unparser;
    SgUnparse_Info& info;
    std::ostream&   os;
  };
  
  void AdaStatementUnparser::handleBasicBlock(SgBasicBlock& n, bool functionbody)
  {
    SgStatementPtrList&          stmts    = n.get_statements();
    SgStatementPtrList::iterator aa       = stmts.begin();
    SgStatementPtrList::iterator zz       = stmts.end();
    SgStatementPtrList::iterator dcllimit = std::find_if( aa, zz, 
                                                          [](SgStatement* s) -> bool
                                                          {
                                                            return isSgDeclarationStatement(s) == nullptr;
                                                          }
                                                        );
                                                       
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
}

void
Unparse_Ada::unparseLanguageSpecificStatement(SgStatement* stmt, SgUnparse_Info& info)
{
  ASSERT_not_null(stmt);
  
  sg::dispatch(AdaStatementUnparser{*this, info, std::cerr}, stmt);
}


/*
void 
Unparse_Ada::unparseStatement(SgStatement* stmt, SgUnparse_Info& info)
{
  base::unparseStatement(stmt, info);
}
*/
