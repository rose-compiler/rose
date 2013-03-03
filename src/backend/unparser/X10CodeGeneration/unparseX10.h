/** unparseX10.h
 * This header file contains the class declaration for the X10 unparser.
 */

#ifndef UNPARSE_X10
#define UNPARSE_X10

#include "unparser.h"

class SgExpression;
class SgStatement;
class SgTemplateParameter;
class SgTemplateArgument;
class SgFunctionDeclaration;
class SgBasicBlock;
class SgClassDefinition;
class SgTemplateInstantiationDecl;
class SgNamespaceDefinitionStatement;
class SgAsmOp;

#include "Cxx_Grammar.h"
#include "unparseLanguageIndependentConstructs.h"

#include "unparseJava.h"

class Unparser;

//class Unparse_X10 : public UnparseLanguageIndependentConstructs
class Unparse_X10 : public Unparse_Java
{
  public:
    Unparse_X10(Unparser* unparser, std::string fname);

    virtual ~Unparse_X10();

    // DQ (9/6/2010): Mark the derived class to support debugging.
    virtual std::string languageName() const { return "X10 Unparser"; }
};
#endif // UNPARSE_X10

