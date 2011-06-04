#ifndef UNPARSER_PYTHON_H_
#define UNPARSER_PYTHON_H_

#include "unparser.h"

class SgStatement;
class SgBasicBlock;
class SgClassDefinition;
class SgNamespaceDefinitionStatement;

#include "unparseLanguageIndependentConstructs.h"

class Unparse_Python : public UnparseLanguageIndependentConstructs
   {
     public:
          Unparse_Python(Unparser* unp, std::string fname);
          virtual ~Unparse_Python();
          virtual void unparseStatement(SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseLanguageSpecificStatement(SgStatement*, SgUnparse_Info&);
          virtual void unparseLanguageSpecificExpression(SgExpression*, SgUnparse_Info&);
          virtual void unparseStringVal(SgExpression*, SgUnparse_Info&);

       // DQ (9/6/2010): Mark the derived class to support debugging.
          virtual std::string languageName() const { return "Python Unparser"; }
   };

#endif /* UNPARSER_PYTHON_H_ */
 



