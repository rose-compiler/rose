/* Unparse_Ada.C
 * This C file contains the general function to unparse types as well as
 * functions to unparse every kind of type.
 */

#include "sage3basic.h"
#include "unparser.h" //charles4:  I replaced this include:   #include "unparseX10.h"

#include "sageGeneric.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
//~ #include "rose_config.h"


//~ void replaceString (std::string& str, const std::string& from, const std::string& to);

namespace
{
  struct AdaTypeUnparser
  {
    AdaTypeUnparser(Unparse_Ada& unp, SgUnparse_Info& inf, std::ostream& outp)
    : unparser(unp), info(inf), os(outp)
    {}
    
    void prn(const std::string& s)
    {
      unparser.curprint(s);
      os << s;
    }
    
    void handle(SgNode& n)    { SG_UNEXPECTED_NODE(n); }
    
    //
    // Ada constraints

    void handle(SgAdaRangeConstraint& n)
    {
      prn(" range ");
      
      SgRangeExp& range = SG_DEREF(n.get_range());
      
      expr(range.get_start());
      prn(" .. ");
      expr(range.get_end());
    }
    
    //
    // Fundamental types
    
    void handle(SgTypeBool&)  { prn(" Boolean"); }
    void handle(SgTypeInt&)   { prn(" Integer"); }
    void handle(SgTypeChar&)  { prn(" Character"); }
    void handle(SgTypeFloat&) { prn(" Float"); }
    void handle(SgTypeVoid&)  { prn(" -- void\n"); }
    
    //
    // Ada types

    void handle(SgAdaSubtype& n) 
    { 
      type(n.get_base_type());
      support(n.get_constraint());
    }
    
    void handle(SgModifierType& n) 
    {
      if (n.get_typeModifier().get_constVolatileModifier().isConst())
        prn("constant ");
      
      type(n.get_base_type());
    }
    
    void handle(SgTypeDefault& n) 
    {
      prn(" private");
    }
    
    void handle(SgNamedType& n) 
    { 
      prn(" ");
      prn(n.get_name()); 
    }
    
    void handle(SgDeclType& n) 
    { 
      prn(" ");
      expr(n.get_base_expression()); 
    }
    
    void handle(SgTypeTuple& n) 
    { 
      SgTypePtrList& lst = n.get_types();
      
      for (size_t i = 0; i < lst.size()-1; ++i)
      {
        type(lst[i]);
        prn(" |");
      }
      
      type(lst.back());
    }
    
    void handle(SgArrayType& n)
    {
      prn(" array");
      // \todo add range
      prn(" of");
      type(n.get_base_type());
    }
    
    void handle(SgAdaTaskType& n) 
    { 
      // \todo fix in AST and override get_name and get_declaration in AdaTaskType
      prn(" ");
      prn(SG_DEREF(n.get_decl()).get_name()); 
    }
    
    void type(SgType* ty)
    {
      sg::dispatch(*this, ty);
    }
    
    void support(SgNode* n)
    {
      sg::dispatch(*this, n);
    }
    
    void expr(SgExpression* e)
    {
      unparser.unparseExpression(e, info);
    }
    
    
    Unparse_Ada&    unparser;
    SgUnparse_Info& info;
    std::ostream&   os;
  };
}


//-----------------------------------------------------------------------------------
//  void Unparse_Ada::unparseType
//
//  General function that gets called when unparsing an Ada type. Then it routes
//  to the appropriate function to unparse each Ada type.
//-----------------------------------------------------------------------------------
void
Unparse_Ada::unparseType(SgType* type, SgUnparse_Info& info)
{
  ASSERT_not_null(type);
  
  sg::dispatch(AdaTypeUnparser(*this, info, std::cerr), type);
}

