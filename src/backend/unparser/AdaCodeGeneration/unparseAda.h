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

struct ScopeStackEntry
{
  explicit
  ScopeStackEntry(SgUnparse_Info* info = nullptr, SgScopeStatement* scope = nullptr)
  : unparseInfo(info), parentScope(scope), addedRenamings(), addedUsedScopes()
  {}

  SgUnparse_Info*                      unparseInfo = nullptr;
  SgScopeStatement*                    parentScope = nullptr;
  std::vector<const SgScopeStatement*> addedRenamings;
  std::vector<const SgScopeStatement*> addedUsedScopes;
};

struct Unparse_Ada : UnparseLanguageIndependentConstructs
   {
          using base                   = UnparseLanguageIndependentConstructs;
          using VisibleScopeContainer  = std::set<const SgScopeStatement*>;
          using UsePkgContainer        = std::set<const SgScopeStatement*>;
          using ScopeRenamingContainer = std::map<const SgScopeStatement*, const SgDeclarationStatement*>;
          using NameQualMap            = std::map<SgNode*, std::string>;

          //
          // in unparseAda_statements.C

          Unparse_Ada(Unparser* unp, std::string fname);

          void unparseAdaFile(SgSourceFile *file, SgUnparse_Info &info);
          void unparseStatement(SgStatement* stmt, SgUnparse_Info& info) override;
          void unparseLanguageSpecificStatement  (SgStatement* stmt,  SgUnparse_Info& info) override;
          std::string computeScopeQual(const SgScopeStatement& local, const SgScopeStatement& remote);
          void setInitialScope(SgUnparse_Info& info, SgExpression*);

          //
          // in unparseAda_expressions.C

          void unparseExpression(SgExpression* expr, SgUnparse_Info& info) override;
          void unparseLanguageSpecificExpression (SgExpression* expr, SgUnparse_Info& info) override;
          void unparseStringVal(SgExpression* expr, SgUnparse_Info& info) override;

          void unparseExprListExp(SgExprListExp* n, SgUnparse_Info& info, std::string sep = ", ");

          /// uses the flag saved by the Asis to ROSE converter
          bool requiresParentheses(SgExpression* expr, SgUnparse_Info& info) override;

          /// unparses \ref type and uses \ref ref as the anchor node for scope qualification
          /// \note implemented in unparseAda_types.C
          void unparseType(const SgLocatedNode& ref, SgType* type, SgUnparse_Info& info);

          // DQ (9/12/2017): Mark the derived class to support debugging.
          // virtual std::string languageName() const;
          std::string languageName() const override { return "Ada Unparser"; }


          //
          // API to keep track of visible scopes
          // impl. in unparseAda_statements.C
          void addVisibleScope(const SgScopeStatement*);
          bool isVisibleScope(const SgScopeStatement*) const;

          void addUsedScope(const SgScopeStatement*);
          bool isUsedScope(const SgScopeStatement*) const;

          void addRenamedScope(ScopeRenamingContainer::key_type, ScopeRenamingContainer::mapped_type);
          ScopeRenamingContainer::mapped_type renamedScope(ScopeRenamingContainer::key_type) const;

          void openScope(SgUnparse_Info& info, SgScopeStatement& scope);
          void closeScope();

          const NameQualMap& nameQualificationMap() const;
          void withNameQualificationMap(const NameQualMap&);

     private:
          /// fully-qualified names of visible scopes
          VisibleScopeContainer  visible_scopes;

          /// fully-qualified names of used scopes (i.e., use the.used.package;)
          UsePkgContainer        use_scopes;

          /// renamed scopes
          ScopeRenamingContainer renamed_scopes;

          /// stores info about scope state
          std::vector<ScopeStackEntry> scope_state;

          ///
          const NameQualMap* currentNameQualificationMap;
   };

#endif



