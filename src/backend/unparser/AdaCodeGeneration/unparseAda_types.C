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
    
    void handle(SgTypeBool&)  { prn(" Boolean"); }
    void handle(SgTypeInt&)   { prn(" Integer"); }
    void handle(SgTypeChar&)  { prn(" Character"); }
    void handle(SgTypeFloat&) { prn(" Float"); }
    
    void handle(SgTypedefType& n) 
    { 
      prn(" ");
      prn(n.get_name()); 
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

