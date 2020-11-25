#ifndef UNPARSER_ADA
#define UNPARSER_ADA

#include "unparser.h"

#if 1
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
#endif

#include "Cxx_Grammar.h"
#include "unparseLanguageIndependentConstructs.h"

class Unparser;

struct Unparse_Ada : UnparseLanguageIndependentConstructs
   {
          typedef UnparseLanguageIndependentConstructs base;

          //
          // in unparseAda_statements.C

          Unparse_Ada(Unparser* unp, std::string fname);

          void unparseAdaFile(SgSourceFile *file, SgUnparse_Info &info);
          void unparseStatement(SgStatement* stmt, SgUnparse_Info& info) ROSE_OVERRIDE;
          void unparseLanguageSpecificStatement  (SgStatement* stmt,  SgUnparse_Info& info) ROSE_OVERRIDE;
          std::string computeScopeQual(SgScopeStatement& local, SgScopeStatement& remote);
          // std::string computeScopeQualStmts(SgStatement& local, SgStatement& remote);

          //
          // in unparseAda_expressions.C

          void unparseExpression(SgExpression* expr, SgUnparse_Info& info) ROSE_OVERRIDE;
          void unparseLanguageSpecificExpression (SgExpression* expr, SgUnparse_Info& info) ROSE_OVERRIDE;
          void unparseStringVal(SgExpression* expr, SgUnparse_Info& info) ROSE_OVERRIDE;

          /// uses the flag saved by the Asis to ROSE converter
          bool requiresParentheses(SgExpression* expr, SgUnparse_Info& info) ROSE_OVERRIDE;

          //
          // in unparseAda_types.C
          void unparseType(SgType* type, SgScopeStatement* where, SgUnparse_Info& info);

          // DQ (9/12/2017): Mark the derived class to support debugging.
          // virtual std::string languageName() const;
          std::string languageName() const ROSE_OVERRIDE { return "Ada Unparser"; }



   };

#endif



