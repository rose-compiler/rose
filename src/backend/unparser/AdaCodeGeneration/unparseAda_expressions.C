/* unparseAda_expressions.C
 * 
 *
 */
#include "sage3basic.h"
#include "unparser.h"
//~ #include "Utf8.h"
#include "sageGeneric.h"

using namespace std;

#define OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES 0
#define OUTPUT_HIDDEN_LIST_DATA 0
#define OUTPUT_DEBUGGING_INFORMATION 0

#ifdef _MSC_VER
#include "Cxx_Grammar.h"
#endif

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"

namespace
{
  SgName nameOf(const SgSymbol& sy)
  {
    return sy.get_name();
  }

  SgName nameOf(const SgVarRefExp& var_ref)
  {
    return nameOf(SG_DEREF(var_ref.get_symbol()));
  }

  SgName nameOf(const SgFunctionRefExp& fun_ref)
  {
    return nameOf(SG_DEREF(fun_ref.get_symbol()));
  }
  
  std::string 
  operator_sym(SgNode& n)
  {
    typedef std::map<int, std::string> operator_symbols_map;

    static const operator_symbols_map operator_symbols 
                = { 
                    { V_SgAssignOp,         ":=" },
                    { V_SgOrOp,             "or else" },          
                    { V_SgAndOp,            "and then" },          
                    { V_SgBitAndOp,         "and" },
                    { V_SgBitOrOp,          "or" },          
                    { V_SgBitXorOp,         "xor" },         
                    { V_SgEqualityOp,       "=" },       
                    { V_SgNotEqualOp,       "!=" },       
                    { V_SgLessThanOp,       "<" },       
                    { V_SgLessOrEqualOp,    "<=" },    
                    { V_SgGreaterThanOp,    ">" },    
                    { V_SgGreaterOrEqualOp, ">=" }, 
                    { V_SgAddOp,            "+" },            
                    { V_SgSubtractOp,       "-" },       
                    { V_SgConcatenationOp,  "&" },  
                    { V_SgUnaryAddOp,       "+" },       
                    { V_SgMinusOp,          "-" },          
                    { V_SgMultiplyOp,       "*" },       
                    { V_SgDivideOp,         "/" },         
                    { V_SgModOp,            "mod" },            
                    //~ { V_SgRemOp,            "rem" },        
                    { V_SgPowerOp,          "**" },          
                    //~ { V_SgAbsOp,            "abs" },       
                    { V_SgNotOp,            "not" },
                    // SgCommaOpExp is not really in Ada, but separates discrete choices in case-when.
                    { V_SgCommaOpExp,       "|" }
                  };

    operator_symbols_map::const_iterator pos = operator_symbols.find(n.variantT());
    
    if (pos == operator_symbols.end())
    {
      std::cerr << "unknown operator: " << typeid(n).name() << std::endl;
      
      return "<OP>";
    }
    
    return pos->second;
  }
  
  struct AdaExprUnparser
  {
    void prn(const std::string& s)
    {
      unparser.curprint(s);
      os << s;
    }
    
    void handle(SgNode& n)      { SG_UNEXPECTED_NODE(n); }
    
    void handle(SgBinaryOp& n);  
    void handle(SgUnaryOp& n);  
    
    /*
    void handle(SgIntVal& n)
    {
      prn(n.get_valueString());
    }
    */
    
    void handle(SgRangeExp& n)
    {
      expr(n.get_start());
      prn("..");
      expr(n.get_end());
    }
    
    void handle(SgCallExpression& n)
    {
      expr(n.get_function());
      prn("(");
      expr(n.get_args());
      prn(")");
    }
    
    void handle(SgVarRefExp& n)
    {
      prn(nameOf(n));
    }
    
    void handle(SgNullExpression& n)
    {
      prn("<null>");
    }
    
    void handle(SgFunctionRefExp& n)
    {
      prn(nameOf(n));
    }
    
    void expr(SgExpression* exp)
    {
      // let the generic unparser handle its things.. 
      unparser.unparseExpression(exp, info); 
      
      // or just handle everything
      //~ sg::dispatch(*this, exp);
    }
    
    void expr(SgExprListExp* exp)
    {
      SgExpressionPtrList& lst = exp->get_expressions();
      bool                 first = true;
      
      for (SgExpression* exp : lst)
      {
        if (!first)
        { 
          prn(", ");
        }
        else 
          first = false;
        
        expr(exp);
      }
    }
    
    void operator()(SgExpression* exp)
    {
      expr(exp);
    }
    
    Unparse_Ada&    unparser;
    SgUnparse_Info& info;
    std::ostream&   os;
  };
  
  void AdaExprUnparser::handle(SgBinaryOp& n)
  {
    expr(n.get_lhs_operand());
    prn(" ");
    prn(operator_sym(n));  
    prn(" ");
    expr(n.get_rhs_operand());  
  }
  
  void AdaExprUnparser::handle(SgUnaryOp& n)
  {
    const bool isprefix = true; // \todo
    
    if (isprefix) { prn(operator_sym(n)); prn(" "); }  
    expr(n.get_operand());  
    if (!isprefix) prn(operator_sym(n));  
  }
}

bool Unparse_Ada::requiresParentheses(SgExpression* expr, SgUnparse_Info& info)
{
  ASSERT_not_null(expr);
  
  if (!isTransformed(sg::ancestor<SgStatement>(expr)))
    return expr->get_need_paren();
  
  return base::requiresParentheses(expr, info);
}


void Unparse_Ada::unparseLanguageSpecificExpression(SgExpression* expr, SgUnparse_Info& info) 
{
  ASSERT_not_null(expr);
  
  sg::dispatch(AdaExprUnparser{*this, info, std::cerr}, expr);
}


void Unparse_Ada::unparseStringVal(SgExpression* expr, SgUnparse_Info& info)
{
  printf ("Unparse_Ada::unparseStringVal(): not implemented! \n");
  ROSE_ASSERT(false);
}
