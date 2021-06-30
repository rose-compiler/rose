#ifndef UNPARSER_ADA
#define UNPARSER_ADA

#include "unparser.h"

#include <tuple>

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

struct AdaRenamedNames : std::tuple<std::string, std::string>
{
  using base = std::tuple<std::string, std::string>;
  using base::base;

  const std::string& newname()  const { return std::get<0>(*this); }
  const std::string& original() const { return std::get<1>(*this); }
};

struct Unparse_Ada : UnparseLanguageIndependentConstructs
   {
          using base                   = UnparseLanguageIndependentConstructs;
          using VisibleScopeContainer  = std::vector<std::string>;
          using UsePkgContainer        = std::vector<std::string>;
          using ScopeRenamingContainer = std::vector<AdaRenamedNames>;

          //
          // in unparseAda_statements.C

          Unparse_Ada(Unparser* unp, std::string fname);

          void unparseAdaFile(SgSourceFile *file, SgUnparse_Info &info);
          void unparseStatement(SgStatement* stmt, SgUnparse_Info& info) ROSE_OVERRIDE;
          void unparseLanguageSpecificStatement  (SgStatement* stmt,  SgUnparse_Info& info) ROSE_OVERRIDE;
          std::string computeScopeQual(SgScopeStatement& local, SgScopeStatement& remote);
          void setInitialScope(SgUnparse_Info& info, SgExpression*);
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
          void unparseType(SgType* type, SgUnparse_Info& info);

          // DQ (9/12/2017): Mark the derived class to support debugging.
          // virtual std::string languageName() const;
          std::string languageName() const ROSE_OVERRIDE { return "Ada Unparser"; }

                VisibleScopeContainer& visibleScopes()       { return visible_scopes; }
          const VisibleScopeContainer& visibleScopes() const { return visible_scopes; }

                UsePkgContainer& usedScopes()       { return use_scopes; }
          const UsePkgContainer& usedScopes() const { return use_scopes; }

                ScopeRenamingContainer& renamedScopes()       { return renamed_scopes; }
          const ScopeRenamingContainer& renamedScopes() const { return renamed_scopes; }

     private:
          /// fully-qualified names of visible scopes
          VisibleScopeContainer  visible_scopes;

          /// fully-qualified names of used scopes (i.e., use the.used.package;)
          UsePkgContainer        use_scopes;

          /// renamed scopes
          ScopeRenamingContainer renamed_scopes;
   };

#endif



