#ifndef UNPARSER_PHP
#define UNPARSER_PHP


#include "unparser.h"
#include "unparseLanguageIndependentConstructs.h"

class Unparse_PHP : public UnparseLanguageIndependentConstructs
{
 public:
  Unparse_PHP(Unparser* unp, std::string fname);
  virtual ~Unparse_PHP();
  virtual void unparseStatement(SgStatement* stmt, SgUnparse_Info& info);
  virtual void unparseLanguageSpecificStatement(SgStatement*, SgUnparse_Info&);
  virtual void unparseLanguageSpecificExpression(SgExpression*, SgUnparse_Info&);
  virtual void unparseStringVal(SgExpression*, SgUnparse_Info&);
};
   
#endif
 



