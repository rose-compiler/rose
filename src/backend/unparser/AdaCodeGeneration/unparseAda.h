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

#if EXPLORING_BETTER_UNPARSER

struct AdaPrinter
{
  using StringView = boost::string_view;
  // using StringView = std::string_view;

  AdaPrinter& prn(StringView vw)
  {
    col += vw.size();
    curprint(vw);

    return *this;
  }

  AdaPrinter& newline()
  {
    col = 0;
    ++line;

    curprint("\n");
    return *this;
  }

  private:
    std::function<void(StringView)> curprint;
    std::size_t line = 0;
    std::size_t col  = 0;
};

struct AdaPrinterNewLine {};

template <char D>
struct AdaPrinterOutput
{


  std::string_view txt;
};

using Keyword     = AdaPrinterOutput<0>;
using WhiteSpace  = AdaPrinterOutput<1>;
using OtherToken  = AdaPrinterOutput<2>;
using LineComment = AdaPrinterOutput<3>;

template <char D>
AdaPrinter& operator<<(AdaPrinter& ap, AdaPrinterOutput<D> out)
{
  return ap.prn(out.txt);
}

template <char D>
AdaPrinter& operator<<(AdaPrinter& ap, AdaPrinterNewLine)
{
  return ap.newline();
}

#endif

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
          ~Unparse_Ada();

          void unparseAdaFile(SgSourceFile *file, SgUnparse_Info &info);
          void unparseStatement(SgStatement* stmt, SgUnparse_Info& info) override;
          void unparseLanguageSpecificStatement  (SgStatement* stmt,  SgUnparse_Info& info) override;
          void setInitialScope(SgUnparse_Info& info, SgExpression*);
          //~ std::string computeScopeQual(const SgScopeStatement& local, const SgScopeStatement& remote);

          Unparser* unparser() { return base::unp; }

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

          /// unparses \ref type without anchor node for scope qualification
          /// \note implemented in unparseAda_types.C
          void unparseType(SgType* ty, SgUnparse_Info& info);

          // DQ (9/12/2017): Mark the derived class to support debugging.
          // virtual std::string languageName() const;
          std::string languageName() const override { return "Ada Unparser"; }


          //
          // API to keep track of visible scopes
          // impl. in unparseAda_statements.C
          void unparseParameterList(const SgInitializedNamePtrList& params, SgUnparse_Info& info);

          const NameQualMap& nameQualificationMap() const;
          void withNameQualificationMap(const NameQualMap&);

     private:
          /// fully-qualified names of visible scopes
          //~ VisibleScopeContainer  visible_scopes;

          /// fully-qualified names of used scopes (i.e., use the.used.package;)
          //~ UsePkgContainer        use_scopes;

          /// renamed scopes
          //~ ScopeRenamingContainer renamed_scopes;

          /// stores info about scope state
          //~ std::vector<ScopeStackEntry> scope_state;

          ///
          const NameQualMap* currentNameQualificationMap;

          /// line wrapping setting of the parser; will be restored by unparser
          int oldLineWrap;
   };

#endif



