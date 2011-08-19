#ifndef UNPARSER_PHP
#define UNPARSER_PHP

#include "unparser.h"

class SgStatement;
class SgBasicBlock;
class SgClassDefinition;
class SgNamespaceDefinitionStatement;

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

       // DQ (9/6/2010): Mark the derived class to support debugging.
          virtual std::string languageName() const { return "PHP Unparser"; }
   };

#endif
 



