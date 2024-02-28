
#include "sageInterfaceAda.h"
#include "sageInterface.h"
#include "sageGeneric.h"
#include "sageBuilder.h"

#include <iostream>
#include <limits>
#include <cmath>
#include <exception>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include "Rose/Diagnostics.h"

namespace si = SageInterface;
namespace sb = SageBuilder;

namespace SageInterface
{
namespace Ada
{
  // workaround to get the scope of package standard more easily
  //   set in AdaType.C:initializePkgStandard
  SgAdaPackageSpecDecl* stdpkg          = nullptr;
}
}


namespace
{
  //
  // convenience functions to identify Ada attributes
  bool isClassAttribute(const SgAdaAttributeExp& attr)
  {
    const std::string attrname = "class";

    return ::si::Ada::isAttribute(attr, attrname);
  }

  bool isRangeAttribute(const SgAdaAttributeExp& attr)
  {
    const std::string attrname = "range";

    return ::si::Ada::isAttribute(attr, attrname);
  }


  /// the dominant declaration of a typedef type is the defining declaration,
  ///   if not present the declaration associated with the typedef type.
  const SgTypedefDeclaration&
  dominant_declaration(const SgTypedefType& n)
  {
    const SgTypedefDeclaration* dcl    = isSgTypedefDeclaration(n.get_declaration());
    ASSERT_not_null(dcl);
    const SgTypedefDeclaration* defdcl = isSgTypedefDeclaration(dcl->get_definingDeclaration());

    if (defdcl != nullptr) dcl = defdcl;
    return *dcl;
  }

  /// queries the base type from the dominant declaration
  /// \details in Ada a first typedef can be incomplete and return e.g., SgTypeDefault.
  ///          to get the most defined available type, it is queried from
  ///          the dominant declaration.
  SgType& base_type(const SgTypedefType& n)
  {
    return SG_DEREF(dominant_declaration(n).get_base_type());
  }


  struct ArrayType : sg::DispatchHandler<SgArrayType*>
  {
      static
      SgArrayType* find(SgType* n);
      //~ find(SgType* n, TypeSkip skipWhat = skipNone);

      ReturnType descend(SgType* n);

      // invalid case
      void handle(SgNode& n)                { SG_UNEXPECTED_NODE(n); }

      // base cases
      void handle(SgType&)                  { res = nullptr; }
      void handle(SgArrayType& n)           { res = &n; }

      // possibly skipped types
      void handle(SgAdaSubtype& n)          { res = descend(n.get_base_type()); }
      void handle(SgAdaDerivedType& n)      { res = descend(n.get_base_type()); }
      void handle(SgTypedefType& n)         { res = descend(&base_type(n)); }
      void handle(SgModifierType& n)        { res = descend(n.get_base_type()); }
      void handle(SgAdaFormalType& n)       { res = descend(n.get_formal_type()); }

      //~ void handle(SgPointerType& n)         { res = descend(n.get_base_type()); }
      //~ void handle(SgReferenceType& n)       { res = descend(n.get_base_type()); }
      //~ void handle(SgRvalueReferenceType& n) { res = descend(n.get_base_type()); }
      //~ void handle(SgAdaAccessType& n)       { res = descend(n.get_base_type()); }
  };

  SgArrayType*
  ArrayType::find(SgType* n)
  {
    SgArrayType* res = sg::dispatch(ArrayType{}, n);

    return res;
  }

  ArrayType::ReturnType
  ArrayType::descend(SgType* n)
  {
    return ArrayType::find(n);
  }

  struct DimRange : sg::DispatchHandler<SgExpression*>
  {
      static
      SgExpression& find(SgNode* n);

      static
      ReturnType descend(SgNode* n); // may return null

      // invalid case
      void handle(SgNode& n)                { SG_UNEXPECTED_NODE(n); }

      // allow all expressions, to test if an expr. denotes a range
      void handle(SgExpression&)            { /* do nothing -> returns nullptr */ }

      void handle(SgRangeExp& n)            { res = &n; }

      void handle(SgAdaAttributeExp& n)
      {
        if (isRangeAttribute(n))
          res = &n;
      }

      // switch from expression to types
      void handle(SgTypeExpression& n)
      {
        // try to extract the range from the type
        res = descend(n.get_type());

        // if that did not work, return the type expression
        if (res == nullptr)
          res = &n;
      }

      // base case for types
      void handle(SgType&)                  { /* do nothing to return the type-expression */ }

      // type expressions
      void handle(SgTypedefType& n)         { res = descend(&base_type(n)); }
      void handle(SgAdaFormalType& n)       { res = descend(n.get_formal_type()); }

      void handle(SgAdaSubtype& n)
      {
        SgAdaRangeConstraint& range = SG_DEREF(isSgAdaRangeConstraint(n.get_constraint()));

        res = descend(range.get_range());
      }
  };

  SgExpression&
  DimRange::find(SgNode* n)
  {
    return SG_DEREF(descend(n));
  }

  DimRange::ReturnType
  DimRange::descend(SgNode* n)
  {
    return sg::dispatch(DimRange{}, n);
  }

  struct ArrayBounds : sg::DispatchHandler<std::vector<SgExpression*> >
  {
      static
      ReturnType
      find(SgType* n, SgArrayType* baseType);

      ReturnType
      descend(SgType* n);

      // invalid case
      void handle(SgNode& n)                { SG_UNEXPECTED_NODE(n); }

      // base cases
      //~ void handle(SgType&)                  { /* do nothing */; }

      // skipped types
      void handle(SgAdaDerivedType& n)      { res = descend(n.get_base_type()); }
      void handle(SgTypedefType& n)         { res = descend(&base_type(n)); }
      void handle(SgModifierType& n)        { res = descend(n.get_base_type()); }
      void handle(SgAdaFormalType& n)       { res = descend(n.get_formal_type()); }

      // subtype -> get the dimension info for each
      void handle(SgAdaSubtype& n)
      {
        SgAdaTypeConstraint* constraint = n.get_constraint();

        // if the subtype is unconstrained, then the real array types
        //   must be located underneath.
        if (isSgAdaNullConstraint(constraint))
        {
          res = descend(n.get_base_type());
          return;
        }

        // the first subtype must be an index constraint
        SgAdaIndexConstraint& idx = SG_DEREF(isSgAdaIndexConstraint(constraint));
        SgExpressionPtrList&  idxlst = idx.get_indexRanges();

        for (size_t i = 0; i < idxlst.size(); ++i)
        {
          SgExpression* expr = idxlst[i];

          res.push_back(&DimRange::find(expr));
        }
      }

      void handle(SgArrayType& n)
      {
        SgExprListExp&        idx = SG_DEREF(n.get_dim_info());
        SgExpressionPtrList&  idxlst = idx.get_expressions();

        res.insert(res.end(), idxlst.begin(), idxlst.end());
      }
  };

  ArrayBounds::ReturnType
  ArrayBounds::find(SgType* n, SgArrayType* baseType)
  {
    if (!baseType)
      return ArrayBounds::ReturnType{};

    return sg::dispatch(ArrayBounds{}, n);
  }

  ArrayBounds::ReturnType
  ArrayBounds::descend(SgType* n)
  {
    return sg::dispatch(ArrayBounds{}, n);
  }

  struct IntegralValue : sg::DispatchHandler<long long int>
  {
    void handle(const SgNode& n)     { SG_UNEXPECTED_NODE(n); }

    void handle(const SgExpression& n)
    {
      static const char* const msg = "sageInterface::ada: Expected constant integral value, got ";

      throw std::runtime_error(msg + n.unparseToString());
    }

    void handle(const SgShortVal& n)               { res = n.get_value(); }
    void handle(const SgIntVal& n)                 { res = n.get_value(); }
    void handle(const SgLongIntVal& n)             { res = n.get_value(); }
    void handle(const SgLongLongIntVal& n)         { res = n.get_value(); }

    void handle(const SgUnsignedCharVal& n)        { res = n.get_value(); }
    void handle(const SgUnsignedIntVal& n)         { res = n.get_value(); }
    void handle(const SgUnsignedLongLongIntVal& n) { res = n.get_value(); }
    void handle(const SgUnsignedLongVal& n)        { res = n.get_value(); }
    void handle(const SgUnsignedShortVal& n)       { res = n.get_value(); }
  };

  struct RangeExp : sg::DispatchHandler<SgRangeExp*>
  {
      using base = sg::DispatchHandler<SgRangeExp*>;

      explicit
      RangeExp(size_t whichDimension)
      : base(), dim(whichDimension)
      {}

      static
      ReturnType
      find(SgNode* n, size_t dim);

      ReturnType
      descend(SgNode* n);

      void notFound() const { ROSE_ASSERT(!res); }

      size_t dimension() const
      {
        ROSE_ASSERT(dim > 0);

        return dim-1;
      }

      //
      // handlers

      void handle(SgNode& n)               { SG_UNEXPECTED_NODE(n); }

      // success
      void handle(SgRangeExp& n)           { res = &n; }

      // not found
      void handle(SgType&)                 { notFound(); }

      // For any expressions, try with the expression's type
      // \note the ROSE AST may not yet compute the correct type for all
      //       Ada expressions. Rather than putting on band aid here,
      //       this would need to be fixed in the AST (if this is an issue).
      void handle(SgExpression& n)         { res = descend(n.get_type()); }

      void handle(SgAdaAttributeExp& n)    { res = ::si::Ada::range(n); }

      void handle(SgAdaRangeConstraint& n) { res = descend(n.get_range()); }

      void handle(SgAdaIndexConstraint& n)
      {
        res = find(n.get_indexRanges().at(dimension()), 1);
      }

      void handle(SgAdaSubtype& n)
      {
        SgNode* constraint = n.get_constraint();

        res = descend(constraint ? constraint : n.get_base_type());
      }

      void handle(SgArrayType& n)
      {
        if (::si::Ada::unconstrained(n))
          return notFound();

        SgExprListExp& exprlst = SG_DEREF(n.get_dim_info());

        res = find(exprlst.get_expressions().at(dimension()), 1);
      }

      void handle(SgAdaDerivedType& n)     { res = descend(n.get_base_type()); }
      void handle(SgTypedefType& n)        { res = descend(&base_type(n)); }

    private:
      size_t dim;
  };

  RangeExp::ReturnType
  RangeExp::find(SgNode* n, size_t dim)
  {
    return sg::dispatch(RangeExp(dim), n);
  }

  RangeExp::ReturnType
  RangeExp::descend(SgNode* n)
  {
    return find(n, dim);
  }

  struct BaseTypeDecl : sg::DispatchHandler<SgDeclarationStatement*>
  {
      static
      ReturnType find(SgType* n);

      ReturnType
      descend(SgType* n);

      void handle(const SgNode& n) { SG_UNEXPECTED_NODE(n); }
      void handle(const SgType&)   { /* not found */ }

      void handle(const SgNamedType& n)           { res = n.get_declaration(); }

      void handle(const SgAdaSubtype& n)          { res = descend(n.get_base_type()); }
      void handle(const SgModifierType& n)        { res = descend(n.get_base_type()); }
      void handle(const SgAdaDerivedType& n)      { res = descend(n.get_base_type()); }
  };

  BaseTypeDecl::ReturnType
  BaseTypeDecl::find(SgType* n)
  {
    return sg::dispatch(BaseTypeDecl{}, n);
  }

  BaseTypeDecl::ReturnType
  BaseTypeDecl::descend(SgType* n)
  {
    return find(n);
  }

  void convertSymbolTablesToCaseSensitive_internal(SgNode* node)
  {
    using SymbolTableEntry    = std::pair<const SgName, SgSymbol*>;
    using TmpSymbolTableEntry = std::pair<SgName, SgSymbol*>;

    SgScopeStatement*   scopestmt = isSgScopeStatement(node);
    if (!scopestmt) return;

    SgSymbolTable&      sytable  = SG_DEREF(scopestmt->get_symbol_table());
    ROSE_ASSERT(sytable.isCaseInsensitive());

    rose_hash_multimap& symap    = SG_DEREF(sytable.get_table());
    const size_t        mapsize  = symap.size();

    // store all entries for later use
    std::vector<TmpSymbolTableEntry> tmp;

    // std::for_each(symap.begin(), symap.end(), std::back_inserter(tmp));
    for (SymbolTableEntry& el : symap)
      tmp.push_back(el);

    symap.clear();
    sytable.setCaseInsensitive(false);

    //~ std::for_each(tmp.begin(), tmp.end(), boost::inserter(symap, symap.end()));
    for (TmpSymbolTableEntry& elTmp : tmp)
      symap.insert(elTmp);

    ROSE_ASSERT(symap.size() == mapsize);
  }

  struct CanonicalScope : sg::DispatchHandler<const SgScopeStatement*>
  {
    static
    ReturnType find(const SgScopeStatement* n);

    void handle(const SgNode& n)             { SG_UNEXPECTED_NODE(n); }
    void handle(const SgScopeStatement& n)   { res = &n; }
    void handle(const SgAdaPackageBody& n)   { res = n.get_spec(); }
    void handle(const SgAdaProtectedBody& n) { res = n.get_spec(); }
    void handle(const SgAdaTaskBody& n)      { res = n.get_spec(); }
  };

  CanonicalScope::ReturnType
  CanonicalScope::find(const SgScopeStatement* n)
  {
    return sg::dispatch(CanonicalScope{}, n);
  }

  bool isPrivate(const SgDeclarationStatement& dcl)
  {
    return dcl.get_declarationModifier().get_accessModifier().isPrivate();
  }
}

namespace SageInterface
{
namespace Ada
{
  const std::string roseOperatorPrefix  = "operator";
  const std::string packageStandardName = "Standard";
  const std::string durationTypeName    = "Duration";
  const std::string exceptionName       = "Exception";

  const SgScopeStatement* canonicalScope(const SgScopeStatement* scope)
  {
    return CanonicalScope::find(scope);
  }

  const SgScopeStatement& canonicalScope(const SgScopeStatement& scope)
  {
    return SG_DEREF(canonicalScope(&scope));
  }

  bool sameCanonicalScope(const SgScopeStatement* lhs, const SgScopeStatement* rhs)
  {
    ROSE_ASSERT(lhs && rhs);

    return (  (lhs == rhs) // short-cut
           || (canonicalScope(lhs) == canonicalScope(rhs))
           );
  }

  long long int
  staticIntegralValue(SgExpression* n)
  {
    return sg::dispatch(IntegralValue{}, n);
  }

  int
  firstLastDimension(SgExprListExp& args)
  {
    SgExpressionPtrList& exprlst = args.get_expressions();

    if (exprlst.size() == 0)
      return 1;

    ROSE_ASSERT(exprlst.size() == 1);
    long long res = staticIntegralValue(exprlst[0]);

    ROSE_ASSERT(res <= std::numeric_limits<int>::max());
    return res;
  }

  int
  firstLastDimension(SgExprListExp* args)
  {
    ASSERT_not_null(args);
    return firstLastDimension(*args);
  }


  bool withPrivateDefinition(const SgDeclarationStatement& dcl)
  {
    // return false if dcl is already private
    if (isPrivate(dcl))
      return false;

    const SgDeclarationStatement* def = dcl.get_definingDeclaration();

    return def && isPrivate(*def);
  }

  bool withPrivateDefinition(const SgDeclarationStatement* n)
  {
    return withPrivateDefinition(SG_DEREF(n));
  }

  FlatArrayType
  getArrayTypeInfo(SgType* atype)
  {
    if (atype == nullptr)
      return { nullptr, {} };

    return getArrayTypeInfo(*atype);
  }

  FlatArrayType
  getArrayTypeInfo(SgType& atype)
  {
    SgArrayType* restype = ArrayType::find(&atype);

    return { restype, ArrayBounds::find(&atype, restype) };
  }

  namespace
  {
    SgSymbol& originalSymbol(const SgAliasSymbol& sym)
    {
      SgSymbol* orig = sym.get_alias();

      if (const SgAliasSymbol* alisy = isSgAliasSymbol(orig))
        return originalSymbol(*alisy);

      return SG_DEREF(orig);
    }

    const SgSymbol& originalSymbol(const SgSymbol& sym)
    {
      if (const SgAliasSymbol* alisy = isSgAliasSymbol(&sym))
        return originalSymbol(*alisy);

      return sym;
    }

    void appendAllVariableSymbols(const SgScopeStatement& scope, std::vector<RecordField>& res)
    {
      SgSymbolTable&            symtbl = SG_DEREF(scope.get_symbol_table());
      const rose_hash_multimap& allsyms = SG_DEREF(symtbl.get_table());

      for (const auto& entry : allsyms)
      {
        const SgSymbol& sym  = SG_DEREF(entry.second);
        const SgSymbol& orig = originalSymbol(sym);

        if (/*SgVariableSymbol* varsy =*/ isSgVariableSymbol(&orig))
          res.emplace_back(&sym);
      }
    }

    void appendAllDiscriminantsFromParents(const SgClassDefinition&, std::vector<RecordField>&)
    {
      using namespace Rose::Diagnostics;

      // FIXME \todo
      mlog[TRACE] << "collecting discriminants from parents not implemented.."
                  << std::endl;
    }
  }

  const SgClassDefinition&
  RecordField::record() const
  {
    const SgVariableSymbol&  sym = SG_DEREF( isSgVariableSymbol(&originalSymbol()) );
    const SgInitializedName& var = SG_DEREF( sym.get_declaration() );

    return SG_DEREF( isSgClassDefinition(var.get_scope()) );
  }

  const SgSymbol&
  RecordField::originalSymbol() const
  {
    return SageInterface::Ada::originalSymbol(symbol());
  }

  const SgSymbol&
  RecordField::symbol() const
  {
    // symbols are never null - checked where RecordField are created
    return *std::get<0>(*this);
  }

  bool
  RecordField::discriminant() const
  {
    // not yet implemented
    ROSE_ABORT();
  }

  std::vector<RecordField>
  getAllRecordFields(const SgClassDefinition& rec)
  {
    // look through the symbol table and collect all fields
    //   (variants or otherwise).

    std::vector<RecordField> res;

    appendAllVariableSymbols(rec, res);

    if (const SgAdaDiscriminatedTypeDecl* disc = sg::ancestor_path<SgClassDeclaration, SgAdaDiscriminatedTypeDecl>(rec))
    {
      appendAllVariableSymbols(SG_DEREF(disc->get_discriminantScope()), res);
      appendAllDiscriminantsFromParents(rec, res);
    }

    return res;
  }

  std::vector<RecordField>
  getAllRecordFields(const SgClassDefinition* rec)
  {
    if (rec == nullptr) return {};

    return getAllRecordFields(*rec);
  }



  std::vector<IfExpressionInfo>
  flattenIfExpressions(const SgConditionalExp& n)
  {
    std::vector<IfExpressionInfo> res;
    const SgConditionalExp*       next = &n;
    const SgConditionalExp*       cond = nullptr;
    bool                          last = false;

    // flatten a sequence of unparenthesized conditional expressions
    // c0 ? x :  c1 ? y : z => { <c0, x>, <c1, y>, <null, z> }
    do
    {
      cond = next;
      res.emplace_back(cond->get_conditional_exp(), cond->get_true_exp());

      next = isSgConditionalExp(cond->get_false_exp());
      last = !next || next->get_need_paren();
    } while (!last);

    res.emplace_back(nullptr, SG_DEREF(cond).get_false_exp());
    return res;
  }

  SgExpression* underlyingExpr(const SgStatement* s)
  {
    const SgExprStatement* es = isSgExprStatement(s);

    return SG_DEREF(es).get_expression();
  }

  std::vector<IfStatementInfo>
  flattenIfStatements(const SgIfStmt& n)
  {
    std::vector<IfStatementInfo> res;
    const SgIfStmt*              next = &n;
    const SgIfStmt*              cond = nullptr;

    // flatten a sequence of if a then A (else if b then B (else if c then C else D)) if statements
    // into <a, A>, <b, B>, <c, C>, <nullptr, D>
    do
    {
      cond = next;
      res.emplace_back(underlyingExpr(cond->get_conditional()), cond->get_true_body());

      next = isSgIfStmt(cond->get_false_body());
    } while (next);

    if (SgStatement* falseBranch = cond->get_false_body())
      res.emplace_back(nullptr, falseBranch);

    return res;
  }


  SgRangeExp*
  range(const SgAdaAttributeExp& n)
  {
    if (!isRangeAttribute(n))
      return nullptr;

    const int dim = si::Ada::firstLastDimension(SG_DEREF(n.get_args()));

    return RangeExp::find(n.get_object(), dim);
  }

  SgRangeExp*
  range(const SgAdaAttributeExp* n)
  {
    return range(SG_DEREF(n));
  }

  bool unconstrained(const SgArrayType& ty)
  {
    return ty.get_is_variable_length_array();
  }

  bool unconstrained(const SgArrayType* ty)
  {
    return ty && unconstrained(*ty);
  }

  bool anonymousAccess(const SgType* ty)
  {
    return ty && anonymousAccess(*ty);
  }

  bool anonymousAccess(const SgType& ty)
  {
    const SgAdaAccessType* accty = isSgAdaAccessType(ty.stripType(SgType::STRIP_MODIFIER_TYPE));

    if (accty && !accty->get_is_anonymous())
    {
      using namespace Rose::Diagnostics;

      mlog[TRACE] << "surprising non-anonymous access type." << std::endl;
    }

    return accty != nullptr;
  }


  SgAdaPackageSpecDecl*
  getSpecificationDeclaration(const SgAdaPackageBodyDecl* bodyDecl)
  {
    if (!bodyDecl) return nullptr;

    const SgAdaPackageBodyDecl* defDecl = isSgAdaPackageBodyDecl(bodyDecl->get_definingDeclaration());
    if (!defDecl) defDecl = bodyDecl;

    SgAdaPackageBody*     body = defDecl->get_definition();
    if (!body) return nullptr;

    SgAdaPackageSpec*     spec = body->get_spec();
    if (!spec) return nullptr;

    return isSgAdaPackageSpecDecl(spec->get_parent());
  }

  SgAdaPackageSpecDecl&
  getSpecificationDeclaration(const SgAdaPackageBodyDecl& bodyDecl)
  {
    const SgAdaPackageBodyDecl* defDecl = isSgAdaPackageBodyDecl(bodyDecl.get_definingDeclaration());
    if (!defDecl) defDecl = &bodyDecl;

    SgAdaPackageBody&     body = SG_DEREF(defDecl->get_definition());
    SgAdaPackageSpec&     spec = SG_DEREF(body.get_spec());
    SgAdaPackageSpecDecl* specDecl = isSgAdaPackageSpecDecl(spec.get_parent());

    return SG_DEREF(specDecl);
  }

  SgDeclarationStatement& getSpecificationDeclaration(const SgAdaTaskBodyDecl& bodyDecl)
  {
    return SG_DEREF(bodyDecl.get_specificationDeclaration());
  }

  SgDeclarationStatement* getSpecificationDeclaration(const SgAdaTaskBodyDecl* bodyDecl)
  {
    return bodyDecl ? &getSpecificationDeclaration(*bodyDecl) : nullptr;
  }

  SgDeclarationStatement& getSpecificationDeclaration(const SgAdaProtectedBodyDecl& bodyDecl)
  {
    return SG_DEREF(bodyDecl.get_specificationDeclaration());
  }

  SgDeclarationStatement* getSpecificationDeclaration(const SgAdaProtectedBodyDecl* bodyDecl)
  {
    return bodyDecl ? &getSpecificationDeclaration(*bodyDecl) : nullptr;
  }

  SgAdaPackageBodyDecl*
  getPackageBodyDeclaration(const SgAdaPackageSpecDecl* specDecl)
  {
    if (!specDecl) return nullptr;

    SgAdaPackageSpec*     spec = specDecl->get_definition();
    if (!spec) return nullptr;

    SgAdaPackageBody*     body = spec->get_body();
    if (!body) return nullptr;

    return isSgAdaPackageBodyDecl(body->get_parent());
  }

  SgAdaPackageBodyDecl&
  getPackageBodyDeclaration(const SgAdaPackageSpecDecl& specDecl)
  {
    SgAdaPackageSpec&     spec = SG_DEREF(specDecl.get_definition());
    SgAdaPackageBody&     body = SG_DEREF(spec.get_body());
    SgAdaPackageBodyDecl* bodyDecl = isSgAdaPackageBodyDecl(body.get_parent());

    return SG_DEREF(bodyDecl);
  }

  namespace
  {
    struct CorrespondingBodyFinder : sg::DispatchHandler<const SgScopeStatement*>
    {
      void handle(const SgNode&)               { /* nothing to be done */ }
      void handle(const SgAdaPackageSpec& n)   { res = n.get_body(); }
      void handle(const SgAdaPackageBody& n)   { res = &n; /* n is the body */ }

      void handle(const SgAdaTaskSpec& n)      { res = n.get_body(); }
      void handle(const SgAdaTaskBody& n)      { res = &n; }

      void handle(const SgAdaProtectedSpec& n) { res = n.get_body(); }
      void handle(const SgAdaProtectedBody& n) { res = &n; }
    };
  }

  const SgScopeStatement*
  correspondingBody(const SgScopeStatement* scope)
  {
    ASSERT_not_null(scope);

    return sg::dispatch(CorrespondingBodyFinder{}, scope);
  }


  namespace
  {
    bool isNormalStatement(const SgStatement* s)
    {
      return isSgDeclarationStatement(s) == nullptr;
    }

    bool isNondeclarativePragma(const SgStatement* s)
    {
      const SgPragmaDeclaration* pdcl = isSgPragmaDeclaration(s);
      if (pdcl == nullptr) return false;

      const SgPragma&            prgm = SG_DEREF(pdcl->get_pragma());
      std::string                prgName = boost::to_lower_copy(prgm.get_pragma());

      return (  (prgName != "import")
             && (prgName != "export")
             && (prgName != "priority")
             );
    }

    template <class SageStatementPtrList>
    auto findDeclarationLimit(SageStatementPtrList& list) -> decltype(list.begin())
    {
      using IteratorType = decltype(list.begin());

      IteratorType beg = list.begin();
      IteratorType lim = list.end();
      IteratorType pos = std::find_if(beg, lim, isNormalStatement);
      IteratorType prv = std::prev(pos);

      // read back over pragmas in order not to introduce spurious declare sections
      while ((pos != beg) && isNondeclarativePragma(*prv))
      {
        pos = prv;
        prv = std::prev(pos);
      }

      return pos;
    }
  }

  SgStatementPtrList::const_iterator
  declarationLimit(const SgStatementPtrList& list)
  {
    return findDeclarationLimit(list);
  }

  SgStatementPtrList::const_iterator
  declarationLimit(const SgBasicBlock& block)
  {
    return declarationLimit(block.get_statements());
  }

  SgStatementPtrList::const_iterator
  declarationLimit(const SgBasicBlock* block)
  {
    return declarationLimit(SG_DEREF(block));
  }

  SgStatementPtrList::iterator
  declarationLimit(SgStatementPtrList& list)
  {
    return findDeclarationLimit(list);
  }

  SgStatementPtrList::iterator
  declarationLimit(SgBasicBlock& block)
  {
    return declarationLimit(block.get_statements());
  }

  SgStatementPtrList::iterator
  declarationLimit(SgBasicBlock* block)
  {
    return declarationLimit(SG_DEREF(block));
  }

  bool tryFollowsDeclarativeBlock(const SgTryStmt& n)
  {
    SgBasicBlock*                blk = isSgBasicBlock(n.get_parent());
    if (blk == nullptr) return false;

    //~ SgFunctionDefinition*        def = isSgFunctionDefinition(blk->get_parent());
    //~ if (def == nullptr) return false;

    SgStatementPtrList&          stmts = blk->get_statements();
    SgStatementPtrList::iterator dcllimit = declarationLimit(stmts);

    // return true iff n is the only stmt within the block
    return std::distance(dcllimit, stmts.end()) == 1;
  }

  bool tryFollowsDeclarativeBlock(const SgTryStmt* n)
  {
    return n && tryFollowsDeclarativeBlock(*n);
  }

  bool isPackageTryBlock(const SgTryStmt& n)
  {
    return isSgAdaPackageBody(n.get_parent());
  }


  bool isPackageTryBlock(const SgTryStmt* n)
  {
    return n && isPackageTryBlock(*n);
  }

  namespace
  {
    SgAdaGenericDecl& getRenamedGenericDecl(SgDeclarationStatement* n)
    {
      if (SgAdaGenericDecl* gendcl = isSgAdaGenericDecl(n))
        return *gendcl;

      SgAdaRenamingDecl* rendcl = isSgAdaRenamingDecl(n);
      ASSERT_not_null(rendcl);

      SgExpression*      renexp = rendcl->get_renamed();

      if (SgFunctionRefExp* funref = isSgFunctionRefExp(renexp))
        return SG_DEREF(isGenericDecl(funref->getAssociatedFunctionDeclaration()));

      if (SgAdaUnitRefExp* untref = isSgAdaUnitRefExp(renexp))
      {
        SgDeclarationStatement* refdcl = untref->get_decl();

        if (SgAdaPackageSpecDecl* pkgdcl = isSgAdaPackageSpecDecl(refdcl))
          return SG_DEREF(isGenericDecl(*pkgdcl));

        if (SgAdaGenericDecl* gendcl = isSgAdaGenericDecl(refdcl))
          return *gendcl;

        ROSE_ABORT();
      }

      if (SgAdaRenamingRefExp* renref = isSgAdaRenamingRefExp(renexp))
        return getRenamedGenericDecl(renref->get_decl());

      ROSE_ABORT();
    }
  }


  SgAdaGenericDecl& getGenericDecl(const SgAdaGenericInstanceDecl& n)
  {
    return getRenamedGenericDecl(n.get_genericDeclaration());
  }

  SgAdaGenericDecl* getGenericDecl(const SgAdaGenericInstanceDecl* n)
  {
    ASSERT_not_null(n);

    return &getGenericDecl(*n);
  }

  SgAdaGenericDecl* isGenericDecl(const SgDeclarationStatement& n)
  {
    if (SgAdaGenericDefn* defn = isSgAdaGenericDefn(n.get_parent()))
      return isSgAdaGenericDecl(defn->get_parent());

    return nullptr;
  }

  SgAdaGenericDecl* isGenericDecl(const SgDeclarationStatement* n)
  {
    if (n == nullptr) return nullptr;

    return isGenericDecl(*n);
  }

  //~ bool isDispatching(const SgFunctionCallExpr& n)
  //~ {
    //~ // an argument is of type 'class
    //~ // and the argument is of tagged type
    //~ // and the target is a primitive operation of that type
    //~ return false;
  //~ }

  //~ bool isDispatching(const SgFunctionCallExpr* n)
  //~ {
    //~ return n && isDispatching(*n);
  //~ }


  namespace
  {
    bool isLogicalChildScopeOfDecl(const SgScopeStatement* scope, const SgDeclarationStatement* decl)
    {
      if (scope == nullptr) return false;
      if (scope->get_parent() == decl) return true;

      return isLogicalChildScopeOfDecl(logicalParentScope(*scope), decl);
    }

    bool isLogicalChildOfDecl(const SgNode* n, const SgDeclarationStatement* decl)
    {
      if (n == nullptr) return false;

      return isLogicalChildScopeOfDecl(sg::ancestor<SgScopeStatement>(n), decl);
    }
  }

  bool unitRefDenotesGenericInstance(const SgAdaUnitRefExp& n)
  {
    SgAdaGenericDecl* gendcl = isSgAdaGenericDecl(n.get_decl());

    return gendcl && isLogicalChildOfDecl(&n, gendcl);
  }

  bool unitRefDenotesGenericInstance(const SgAdaUnitRefExp* n)
  {
    return n && unitRefDenotesGenericInstance(*n);
  }

  bool hasUnknownDiscriminants(const SgAdaDiscriminatedTypeDecl& n)
  {
    return SG_DEREF(n.get_discriminants()).get_parameters().size() == 0;
  }

  bool hasUnknownDiscriminants(const SgAdaDiscriminatedTypeDecl* n)
  {
    return n && hasUnknownDiscriminants(*n);
  }

  SgAdaDiscriminatedTypeDecl*
  getAdaDiscriminatedTypeDecl(const SgDeclarationStatement& n)
  {
    return isSgAdaDiscriminatedTypeDecl(n.get_parent());
  }

  SgAdaDiscriminatedTypeDecl*
  getAdaDiscriminatedTypeDecl(const SgDeclarationStatement* n)
  {
    return n ? getAdaDiscriminatedTypeDecl(*n) : nullptr;
  }

  AggregateInfo splitAggregate(const SgExprListExp& lst)
  {
    using Iterator = SgExpressionPtrList::const_iterator;

    const SgExpressionPtrList& exprs = lst.get_expressions();
    Iterator                   aa = exprs.begin();
    Iterator                   zz = exprs.end();
    SgAdaAncestorInitializer*  ini = nullptr;

    if (aa != zz)
    {
      ini = isSgAdaAncestorInitializer(*aa);

      if (ini) ++aa;
    }

    return AggregateInfo{ ini, aa, zz };
  }

  AggregateInfo splitAggregate(const SgExprListExp* exp)
  {
    return splitAggregate(SG_DEREF(exp));
  }


  SgAdaPackageSpecDecl* renamedPackage(const SgAdaRenamingDecl& n)
  {
    SgExpression* ren = n.get_renamed();

    // multiple levels of renaming declarations
    if (SgAdaRenamingRefExp* renref = isSgAdaRenamingRefExp(ren))
      return renamedPackage(renref->get_decl());

    //~ if (SgAdaRenamingSymbol* rensym = isSgAdaRenamingSymbol(sym))
      //~ return renamedPackageSymbol(rensym->get_declaration());

    if (SgAdaUnitRefExp* unitref = isSgAdaUnitRefExp(ren))
      return isSgAdaPackageSpecDecl(unitref->get_decl());

    return nullptr;
  }

  SgAdaPackageSpecDecl* renamedPackage(const SgAdaRenamingDecl* n)
  {
    return n ? renamedPackage(*n) : nullptr;
  }

  bool isFunction(const SgFunctionType& ty)
  {
    return isSgTypeVoid(ty.get_return_type()) == nullptr;
  }

  bool isFunction(const SgFunctionType* ty)
  {
    return ty ? isFunction(*ty) : false;
  }

  bool isFunction(const SgAdaSubroutineType& ty)
  {
    return isSgTypeVoid(ty.get_return_type()) == nullptr;
  }

  bool isFunction(const SgAdaSubroutineType* ty)
  {
    return ty ? isFunction(*ty) : false;
  }

  const SgFunctionType* functionType(const SgFunctionSymbol* funsy)
  {
    return funsy ? &functionType(*funsy) : nullptr;
  }

  const SgFunctionType& functionType(const SgFunctionSymbol& funsy)
  {
    const SgAdaInheritedFunctionSymbol* inhsy = isSgAdaInheritedFunctionSymbol(&funsy);
    const SgFunctionType*               res   = inhsy ? inhsy->get_derivedFunctionType()
                                                      : isSgFunctionType(funsy.get_type());

    return SG_DEREF(res);
  }


  namespace
  {
    bool definedInStandard(const SgDeclarationStatement& n)
    {
      const SgAdaPackageSpec*      pkgspec = isSgAdaPackageSpec(n.get_scope());
      if (pkgspec == nullptr) return false;

      const SgAdaPackageSpecDecl*  pkgdecl = isSgAdaPackageSpecDecl(pkgspec->get_parent());
      // test for properties of package standard, which is a top-level package
      //   and has the name "Standard".
      // \note The comparison is case sensitive, but as long as the creation
      //       of the fictitious package uses the same constant, this is fine.
      return (  (pkgdecl != nullptr)
             && (pkgdecl->get_name() == packageStandardName)
             && (isSgGlobal(pkgdecl->get_scope()))
             );
    }

    bool isExceptionType(const SgType& n)
    {
      const SgTypedefType* ty = isSgTypedefType(&n);
      if (ty == nullptr || (ty->get_name() == exceptionName)) // \todo should be !=
        return false;

      SgTypedefDeclaration* dcl = isSgTypedefDeclaration(ty->get_declaration());

      return definedInStandard(SG_DEREF(dcl));
    }
  }

  bool isObjectRenaming(const SgAdaRenamingDecl& dcl)
  {
    const SgType* ty = dcl.get_type();

    return ty && (!isSgTypeVoid(ty)) && (!isExceptionType(*ty));
  }

  bool isObjectRenaming(const SgAdaRenamingDecl* dcl)
  {
    return dcl && isObjectRenaming(*dcl);
  }

  /// returns true iff \ref ty refers to an exception renaming
  /// @{
  bool isExceptionRenaming(const SgAdaRenamingDecl& dcl)
  {
    const SgType* ty = dcl.get_type();

    return ty && isExceptionType(*ty);
  }


  bool isExceptionRenaming(const SgAdaRenamingDecl* dcl)
  {
    return dcl && isExceptionRenaming(*dcl);
  }
  /// @}


  bool isModularType(const SgType& ty)
  {
    return isModularType(&ty);
  }

  bool isModularType(const SgType* ty)
  {
    return isSgAdaModularType(ty);
  }


  bool isIntegerType(const SgType& ty)
  {
    return isIntegerType(&ty);
  }

  bool isIntegerType(const SgType* ty)
  {
    return isSgTypeLongLong(ty);
  }

  bool isFloatingPointType(const SgType& ty)
  {
    return isFloatingPointType(&ty);
  }

  bool isFloatingPointType(const SgType* ty)
  {
    return isSgTypeLongDouble(ty);
  }

  bool isDiscreteType(const SgType& ty)
  {
    return isDiscreteType(&ty);
  }

  bool isDiscreteType(const SgType* ty)
  {
    return isSgAdaDiscreteType(ty);
  }

  bool isFixedType(const SgType& ty)
  {
    return isFixedType(&ty);
  }

  bool isFixedType(const SgType* ty)
  {
    return isSgTypeFixed(ty);
  }

  bool isBooleanType(const SgType& ty)
  {
    return isBooleanType(&ty);
  }

  bool isBooleanType(const SgType* ty)
  {
    const SgEnumType* boolTy = isSgEnumType(ty);
    if (!boolTy) return false;

    const SgEnumDeclaration* boolDcl = isSgEnumDeclaration(boolTy->get_declaration());

    return (  (boolDcl != nullptr)
           && (boolDcl->get_name() == "BOOLEAN")
           && definedInStandard(*boolDcl)
           );
  }

  bool isScalarType(const SgType* ty)
  {
    ty = si::Ada::typeRoot(const_cast<SgType*>(ty)).typerep();

    if (!ty) return false;

    if (const SgTypedefType* tydef = isSgTypedefType(ty))
      return isScalarType(&base_type(*tydef));

    return (  isModularType(*ty)
           || isIntegerType(*ty)
           || isFloatingPointType(*ty)
           || isDiscreteType(*ty)
           || isFixedType(*ty)
           || isDecimalFixedType(*ty)
           || isSgEnumType(ty)
           );
  }

  bool isScalarType(const SgType& ty)
  {
    return isScalarType(&ty);
  }

  bool isDiscreteArrayType(const SgType* ty)
  {
    si::Ada::FlatArrayType arrinfo  = si::Ada::getArrayTypeInfo(const_cast<SgType*>(ty));
    SgArrayType const*     arrty    = arrinfo.type();

    if ((arrty == nullptr) || (arrinfo.dims().size() > 1))
      return false;

    SgType const*          elmty = si::Ada::typeRoot(arrty->get_base_type()).typerep();

    if (elmty == nullptr) return false;

    return (  si::Ada::isModularType(*elmty)
           || si::Ada::isIntegerType(*elmty)
           //~ || si::Ada::isFloatingPointType(*elmty)
           || si::Ada::isDiscreteType(*elmty)
           //~ || si::Ada::isFixedType(*elmty)
           //~ || si::Ada::isDecimalFixedType(*elmty)
           || isSgEnumType(elmty)
           );
  }

  bool isDiscreteArrayType(const SgType& ty)
  {
    return isDiscreteArrayType(&ty);
  }


  namespace
  {
    struct TypeResolver : sg::DispatchHandler<bool>
    {
        using base = sg::DispatchHandler<bool>;

        explicit
        TypeResolver(std::function<bool (const SgType&)> typetest)
        : base(), checker(typetest)
        {}

        bool descend(const SgType*);

        void handle(SgNode& n)              { SG_UNEXPECTED_NODE(n); }

        //
        // expression based types
        // ...

        //
        // types

        void handle(SgType& n)              { res = checker(n); }
        void handle(SgModifierType& n)      { res = descend(n.get_base_type()); }
        void handle(SgAdaSubtype& n)        { res = descend(n.get_base_type()); }
        void handle(SgAdaDerivedType& n)    { res = descend(n.get_base_type()); }
        void handle(SgTypedefType& n)       { res = descend(&base_type(n)); }
        // void handle(SgAdaFormalType& n)     { checkChild(n.get_base_type()); } // \todo enable??

      private:
        std::function<bool (const SgType&)> checker;
    };

    bool TypeResolver::descend(const SgType* ty)
    {
      return ty && sg::dispatch(TypeResolver{std::move(checker)}, ty);
    }
  }

  bool resolvesToFixedType(const SgType& ty)
  {
    return sg::dispatch(TypeResolver{[](const SgType& t)->bool { return isFixedType(t); }}, &ty);
  }

  bool resolvesToFixedType(const SgType* ty)
  {
    return ty && resolvesToFixedType(*ty);
  }

  namespace
  {
    bool isDecimalConstraint(SgAdaTypeConstraint* constr)
    {
      SgAdaDeltaConstraint* delta = isSgAdaDeltaConstraint(constr);

      return delta && delta->get_isDecimal();
    }
  }

  bool isDecimalFixedType(const SgType* ty)
  {
    if (const SgAdaSubtype* sub = isSgAdaSubtype(ty))
      return isFixedType(sub->get_base_type()) && isDecimalConstraint(sub->get_constraint());

    return false;
  }

  bool isDecimalFixedType(const SgType& ty)
  {
    return isDecimalFixedType(&ty);
  }


  bool hasSeparatedBody(const SgDeclarationStatement& dcl)
  {
    return dcl.get_declarationModifier().isAdaSeparate();
  }

  bool hasSeparatedBody(const SgDeclarationStatement* dcl)
  {
    return dcl && hasSeparatedBody(*dcl);
  }

  bool isSeparatedBody(const SgDeclarationStatement& n)
  {
    return hasSeparatedBody(n.get_firstNondefiningDeclaration());
  }

  bool isSeparatedBody(const SgDeclarationStatement* n)
  {
    return n && isSeparatedBody(*n);
  }

  namespace
  {
    template <class SageNodeSequence>
    const SgFunctionDeclaration*
    findSecondaryFunctionDecl(const SageNodeSequence& seq, const SgFunctionDeclaration* key)
    {
      using iterator = typename SageNodeSequence::const_reverse_iterator;

      auto sameFirstNondefining = [key](typename SageNodeSequence::value_type ptr) -> bool
                                  {
                                    const SgFunctionDeclaration* fndcl = isSgFunctionDeclaration(ptr);

                                    return fndcl && (fndcl->get_firstNondefiningDeclaration() == key);
                                  };

      iterator lim = seq.rend();
      iterator pos = std::find_if(seq.rbegin(), lim, sameFirstNondefining);

      return pos != lim ? isSgFunctionDeclaration(*pos) : nullptr;
    }

    const SgFunctionDeclaration*
    findSecondaryFunctionDecl(const SgScopeStatement& scope, const SgFunctionDeclaration* key)
    {
      return scope.containsOnlyDeclarations()
                  ? findSecondaryFunctionDecl(scope.getDeclarationList(), key)
                  : findSecondaryFunctionDecl(scope.getStatementList(), key);
    }
  }

  bool hasSeparatedDefinition(const SgFunctionDeclaration* nondef)
  {
    ASSERT_not_null(nondef);

    const SgScopeStatement* nondefScope = nondef->get_scope();

    // \note should we look for the package spec or package body scope parent?
    if (const SgAdaGenericDefn* genScope = isSgAdaGenericDefn(nondefScope))
      nondefScope = logicalParentScope(*genScope);

    if (const SgScopeStatement* bodyScope = correspondingBody(nondefScope))
      if (const SgFunctionDeclaration* secondary = findSecondaryFunctionDecl(*bodyScope, nondef))
        nondef = secondary;

    return nondef->get_declarationModifier().isAdaSeparate();
  }

  bool isSeparatedDefinition(const SgFunctionDeclaration& n)
  {
    return (  (n.get_definition() != nullptr)
           && hasSeparatedDefinition(isSgFunctionDeclaration(n.get_firstNondefiningDeclaration()))
           );
  }

  bool isSeparatedDefinition(const SgFunctionDeclaration* n)
  {
    return n && isSeparatedDefinition(*n);
  }

  namespace
  {
    // root types as implemented by AdaMaker.C
    SgType& integralType() { return SG_DEREF(sb::buildLongLongType()); }
    SgType& realType()     { return SG_DEREF(sb::buildLongDoubleType()); }
    SgType& fixedType()    { return SG_DEREF(sb::buildFixedType(nullptr, nullptr)); }
    SgType& pointerType()  { return SG_DEREF(sb::buildNullptrType()); }

    SgType& arrayType(SgType* base)
    {
      // poor man's type unifier
      static std::map<SgType*, SgArrayType*> m;

      SgArrayType*& res = m[base];

      // \todo this is missing a dimension value
      //       check mkArrayType in AdaMaker.C for details.
      if (res == nullptr) res = sb::buildArrayType(base);

      return SG_DEREF(res);
    };

    SgType& standardType(std::string name)
    {
      ASSERT_not_null(pkgStandardScope());

      SgTypedefSymbol&      tysy  = SG_DEREF(pkgStandardScope()->lookup_typedef_symbol(name));
      SgTypedefDeclaration& tydcl = SG_DEREF(tysy.get_declaration());

      return SG_DEREF(tydcl.get_type());
    }

    bool denotesRange(const SgExpression* exp)
    {
      return sg::dispatch(DimRange{}, exp) != nullptr;
    }

    bool containsRange(const SgExpression& arglst)
    {
      const SgExprListExp* args = isSgExprListExp(&arglst);
      if (args == nullptr) return false;

      const SgExpressionPtrList& lst = args->get_expressions();
      const auto                 lim = lst.end();

      return lim != std::find_if(lst.begin(), lim, denotesRange);
    }

    template <class ResolverT>
    struct TypeDescResolver : sg::DispatchHandler<TypeDescription>
    {
      TypeDescription descend(SgType* ty)
      {
        return static_cast<ResolverT&>(*this).descend(ty);
      }

      void handle(SgNode& n)           { SG_UNEXPECTED_NODE(n); }

      // could be enabled as fallback, but generally indicates an error..
      //~ void handle(SgType& n)           { res = TypeDescription{n}; }

      // get the real typedefed type, which can be an access type
      void handle(SgTypedefType& n)    { res = descend(&base_type(n)); }

      // a modifier type can be an access type ??
      void handle(SgModifierType& n)   { res = descend(n.get_base_type()); }

      // a derived type can be an access type
      void handle(SgAdaDerivedType& n) { res = descend(n.get_base_type()); }

      // a subtype could be an access type with range constraints whose subtype is an array
      void handle(SgAdaSubtype& n)
      {
        TypeDescription                   dt = descend(n.get_base_type());
        std::vector<SgAdaTypeConstraint*> constr = std::move(dt).toplevelConstraints();

        constr.insert(constr.begin(), n.get_constraint());

        res = TypeDescription{dt.typerep_ref(), dt.polymorphic(), std::move(constr)};
      }

      // a formal type can be an access type
      void handle(SgAdaFormalType& n)  { res = descend(n.get_formal_type()); }
    };



    struct DerefedType : TypeDescResolver<DerefedType>
    {
      using base = TypeDescResolver<DerefedType>;

      TypeDescription descend(SgType* ty);

      using base::handle;

      // deref one level of access types
      void handle(SgAdaAccessType& n)  { res = TypeDescription{n.get_base_type()}; }

      // \todo should not be reached in Ada NO_POINTER_IN_ADA
      void handle(SgPointerType& n)    { res = TypeDescription{n.get_base_type()}; }
    };

    TypeDescription
    DerefedType::descend(SgType* ty)
    {
      return sg::dispatch(DerefedType{}, ty);
    }


    // \todo should this be moved into the Sage class hierarchy?
    struct ExprTypeFinder : sg::DispatchHandler<TypeDescription>
    {
      SgType* arrayBaseType(SgType* ty)
      {
        if (SgArrayType* arrty = getArrayTypeInfo(ty).type())
          return arrty->get_base_type();

        // should not happen
        return nullptr;
      }


      void handle(const SgNode& n)              { SG_UNEXPECTED_NODE(n); }

      // by default use the expression's type
      void handle(const SgExpression& n)        { res = TypeDescription{n.get_type()}; }

      // If a var is just constant, take the type from the initializer
      // NOTE: this is not correct, as the type should be determined by the context
      void handle(const SgVarRefExp& n)
      {
        res = TypeDescription{n.get_type()};

        if (isSgAutoType(res.typerep()))
        {
          // if this is an auto constant, check the initializer
          SgVariableSymbol&  sy  = SG_DEREF(n.get_symbol());
          SgInitializedName& var = SG_DEREF(sy.get_declaration());

          if (SgAssignInitializer* init = isSgAssignInitializer(var.get_initializer()))
            res = typeRoot(SG_DEREF(init->get_operand()).get_type());
        }
      }

      void handle(const SgPointerDerefExp& n)
      {
        const SgExpression& op = SG_DEREF(n.get_operand());

        res = sg::dispatch(DerefedType{}, typeOfExpr(op).typerep());
      }

      void handle(const SgPntrArrRefExp& n)
      {
        const SgExpression& arr   = SG_DEREF(n.get_lhs_operand());
        SgType*             resty = typeOfExpr(arr).typerep();
        const SgExpression& idx   = SG_DEREF(n.get_rhs_operand());
        const bool          slice = containsRange(idx);

        if (!slice) resty = arrayBaseType(resty); // resolve to element type

        res = TypeDescription{resty};
      }

      // SgRangeExp::get_type returns TypeDefault
      //   returning the type of the range elements seems more appropriate
      // \todo it seems appropriate to introduce a RangeType with underlying type?
      void handle(const SgRangeExp& n)
      {
        const SgExpression& lhs = SG_DEREF(n.get_start());

        res = TypeDescription{lhs.get_type()};
      }

      void handle(const SgTypeExpression& n)
      {
        res = TypeDescription{n.get_type()};
      }

      // SgTypeString does not preserve the 'wideness', so let's just get
      //   this info from the literal.
      void handle(const SgStringVal& n)
      {
        SgType* strty = nullptr;

        if (n.get_is16bitString())      strty = &standardType("Wide_String");
        else if (n.get_is32bitString()) strty = &standardType("Wide_Wide_String");
        else                            strty = &standardType("String");

        ASSERT_not_null(strty);
        //~ std::cerr << typeid(*strty).name() << std::endl;
        res = TypeDescription{strty};
      }

      // Currently, we have no concept of class-wide type,
      //   thus the information is returned as flag.
      // \todo it seems appropriate to introduce a AdaClassType with underlying type?
      void handle(const SgAdaAttributeExp& n)
      {
        const bool polymorph = isClassAttribute(n);

        res = TypeDescription{ SG_DEREF(n.get_type()), polymorph };
      }

      // an actual argument expression could have one of the special cases
      //   underneath. => use typeOfExpr..
      void handle(const SgActualArgumentExpression& n)
      {
        res = typeOfExpr(n.get_expression());
      }
    };

    bool fromRootType(SgAdaSubtype* ty)
    {
      return ty && ty->get_fromRootType();
    }

    struct RootTypeFinder : sg::DispatchHandler<TypeDescription>
    {
      void handle(SgNode& n)              { SG_UNEXPECTED_NODE(n); }

      //~ void handle(SgType& n) { res = &n; }

      // all root types (according to the three builder function in AdaMaker.C)
      void handle(SgTypeLongLong&)        { res = TypeDescription{integralType()}; }
      void handle(SgTypeLongDouble&)      { res = TypeDescription{realType()}; }
      void handle(SgTypeFixed&)           { res = TypeDescription{fixedType()}; }

      // plus discrete type indicator for Ada generics
      void handle(SgAdaDiscreteType& n)   { res = TypeDescription{n}; }

      // modular type: handle like int?
      void handle(SgAdaModularType&)      { res = TypeDescription{integralType()}; }

      // are subroutines their own root type?
      void handle(SgAdaSubroutineType& n) { res = TypeDescription{n}; }

      // plus types used by AdaMaker but that do not have a direct correspondence
      //   in the Ada Standard.
      void handle(SgTypeVoid& n)          { res = TypeDescription{n}; }
      void handle(SgTypeUnknown& n)       { res = TypeDescription{n}; }
      void handle(SgAutoType& n)          { res = TypeDescription{n}; }

      void handle(SgTypeDefault& n)
      {
        // this is a type used by a opaque declaration.
        // \todo maybe needs replacement with the actual type.
        res = TypeDescription{n};
      }

      // the package standard uses an enumeration to define boolean, so include the
      //   ROSE bool type also.
      // \todo remove BOOL_IS_ENUM_IN_ADA
      void handle(SgTypeBool& n)          { res = TypeDescription{n}; }

      // plus: map all other fundamental types introduced by initializeStandardPackage in AdaType.C
      //       onto the root types defined by AdaMaker.C
      // \todo eventually all types in initializeStandardPackage should be rooted in
      //       the root types as defined by AdaMaker.C.
      void handle(SgTypeInt&)             { res = TypeDescription{integralType()}; }
      void handle(SgTypeLong&)            { res = TypeDescription{integralType()}; }
      void handle(SgTypeShort&)           { res = TypeDescription{integralType()}; }

      void handle(SgTypeFloat&)           { res = TypeDescription{realType()}; }
      void handle(SgTypeDouble&)          { res = TypeDescription{realType()}; }
      void handle(SgTypeFloat128&)        { res = TypeDescription{realType()}; }

      void handle(SgTypeChar& n)          { res = TypeDescription{n}; }
      void handle(SgTypeChar16& n)        { res = TypeDescription{n}; }
      void handle(SgTypeChar32& n)        { res = TypeDescription{n}; }

      // true fundamental types
      void handle(SgClassType& n)         { res = TypeDescription{n}; /* \todo check if this is a derived class */ }
      void handle(SgEnumType& n)          { res = TypeDescription{n}; /* \todo check if this is a derived enum */ }
      void handle(SgAdaTaskType& n)       { res = TypeDescription{n}; }
      void handle(SgAdaProtectedType& n)  { res = TypeDescription{n}; }
      void handle(SgAdaFormalType& n)     { res = TypeDescription{n}; /* what else? */ }


      // an array is fundamental - its underlying type may not be, so it may can be discovered if needed
      void handle(SgArrayType& n)
      {
        res = TypeDescription{arrayType(find(n.get_base_type()).typerep())};
      }

      void handle(SgTypeString&)
      {
        // can be reached from any string literal, which stores the character type is attribute...
        // since we have no info about the character type, just return the standard string type
        res = TypeDescription{arrayType(sb::buildCharType())};
      }

      // pointer types
      void handle(SgTypeNullptr&)         { res = TypeDescription{pointerType()}; } // \todo correct?
      void handle(SgPointerType&)         { res = TypeDescription{pointerType()}; } // \todo NO_POINTER_IN_ADA
      void handle(SgAdaAccessType&)       { res = TypeDescription{pointerType()}; }


      // \todo add string types as introduced by initializeStandardPackage in AdaType.C
      // \todo add other fundamental types as introduced by initializeStandardPackage in AdaType.C

      // all type indirections that do not define fundamental types
      void handle(SgModifierType& n)      { res = find(n.get_base_type()); }
      void handle(SgAdaSubtype& n)        { res = find(n.get_base_type()); }
      void handle(SgAdaDerivedType& n)    { res = find(n.get_base_type()); }

      void handle(SgTypedefType& n)
      {
        SgType* basety = base_type(n).stripType(SgType::STRIP_MODIFIER_TYPE);

        const bool useThisDecl = (  isSgAdaDerivedType(basety)
                                 || isSgAdaAccessType(basety)
                                 || isSgAdaModularType(basety)
                                 || isSgArrayType(basety)
                                 || fromRootType(isSgAdaSubtype(basety))
                                 );

        // end copied code
        res = useThisDecl ? TypeDescription{n} : find(basety);
      }

      void handle(SgAdaDiscriminatedType& n)
      {
        // \todo not sure..
        res = TypeDescription{n};
      }

      void handle(SgDeclType& n)
      {
        // \todo not sure..
        //~ std::cerr << typeid(*n.get_base_expression()).name() << ' '
                  //~ << n.get_base_expression()->unparseToString()
                  //~ << std::endl;
        res = typeRoot(typeOfExpr(n.get_base_expression()).typerep());
      }

      static
      TypeDescription find(SgType* ty);
    };

    TypeDescription
    RootTypeFinder::find(SgType* ty)
    {
      return sg::dispatch(RootTypeFinder{}, ty);
    }

    struct DeclScopeFinder : sg::DispatchHandler<SgScopeStatement*>
    {
        using base = sg::DispatchHandler<SgScopeStatement*>;

        static
        SgScopeStatement* find(const SgNode*);

        void handle(const SgNode& n)              { SG_UNEXPECTED_NODE(n); }

        //
        // expression based types

        // base case, do nothing
        void handle(const SgExpression&)          {}

        void handle(const SgAdaAttributeExp& n)
        {
          res = pkgStandardScope();

          if (isClassAttribute(n))
            if (SgTypeExpression* tyex = isSgTypeExpression(n.get_object()))
              res = find(tyex->get_type());
        }

        void handle(const SgVarRefExp& n)         { res = declOf(n).get_scope(); }

        //
        // types

        void handle(const SgType&)                { /* \todo do nothing for now; should disappear and raise error */ }

        // all root types (according to the three builder function in AdaMaker.C)
        void handle(const SgTypeLongLong&)        { res = pkgStandardScope(); }
        void handle(const SgTypeLongDouble&)      { res = pkgStandardScope(); }
        void handle(const SgTypeFixed&)           { res = pkgStandardScope(); }

        // modular type: handle like int?
        void handle(const SgAdaModularType&)      { res = pkgStandardScope(); }

        // are subroutines their own root type?
        void handle(const SgAdaSubroutineType&)   { res = pkgStandardScope(); }

        // plus types used by AdaMaker but that do not have a direct correspondence
        //   in the Ada Standard.
        void handle(const SgTypeVoid&)            { res = pkgStandardScope(); }
        void handle(const SgTypeUnknown&)         { res = pkgStandardScope(); }
        void handle(const SgAutoType&)            { res = pkgStandardScope(); }
        void handle(const SgTypeDefault&)         { res = pkgStandardScope(); }

        // the package standard uses an enumeration to define boolean, so include the
        //   ROSE bool type also.
        // \todo remove BOOL_IS_ENUM_IN_ADA
        void handle(const SgTypeBool&)            { res = pkgStandardScope(); }

        // plus composite type of literals in the AST
        void handle(const SgTypeString&)          { res = pkgStandardScope(); }


        // \todo implement generics based on test cases
        // void handle(const SgAdaFormalType& n)     { res = &n; }
        // void handle(const SgAdaDiscreteType& n)   { res = pkgStandardScope(); } // \todo

        // plus: map all other fundamental types introduced by AdaType.C:initializePkgStandard
        //       onto the root types defined by AdaMaker.C
        // \todo eventually all types in initializePkgStandard should be rooted in
        //       the root types as defined by AdaMaker.C.
        void handle(const SgTypeInt&)             { res = pkgStandardScope(); }
        void handle(const SgTypeLong&)            { res = pkgStandardScope(); }
        void handle(const SgTypeShort&)           { res = pkgStandardScope(); }

        void handle(const SgTypeFloat&)           { res = pkgStandardScope(); }
        void handle(const SgTypeDouble&)          { res = pkgStandardScope(); }

        void handle(const SgTypeChar& n)          { res = pkgStandardScope(); }
        void handle(const SgTypeChar16&)          { res = pkgStandardScope(); }
        void handle(const SgTypeChar32&)          { res = pkgStandardScope(); }


        // Ada kind of fundamental types
        void handle(const SgArrayType&)           { res = pkgStandardScope(); }
        void handle(const SgTypeNullptr&)         { res = pkgStandardScope(); }

        void handle(const SgPointerType& n)       { res = find(n.get_base_type()); } // \todo NO_POINTER_IN_ADA
        void handle(const SgAdaAccessType& n)     { res = find(n.get_base_type()); } // \todo or scope of underlying type?

        // \todo add string types as introduced by AdaType.C:initializePkgStandard
        // \todo add other fundamental types as introduced by AdaType.C:initializePkgStandard

        // all type indirections that do not have a separate declaration associated
        // \todo may need to be reconsidered
        void handle(const SgModifierType& n)      { res = find(n.get_base_type()); }
        void handle(const SgAdaSubtype& n)        { res = find(n.get_base_type()); }
        void handle(const SgAdaDerivedType& n)    { res = find(n.get_base_type()); }
        // void handle(const SgDeclType& n)             { res = pkgStandardScope(); }

        // for records, enums, typedefs, discriminated types, and types with a real declarations
        //   => return the scope where they were defined.
        void handle(const SgNamedType& n)         { res = SG_DEREF(n.get_declaration()).get_scope(); }

        void handle(const SgTypedefType& n)
        {
          SgType* basety = base_type(n).stripType(SgType::STRIP_MODIFIER_TYPE);

          const bool useThisDecl = (  isSgAdaDerivedType(basety)
                                   || isSgAdaAccessType(basety)
                                   || isSgAdaModularType(basety)
                                   || isSgArrayType(basety)
                                   || fromRootType(isSgAdaSubtype(basety))
                                   );

          //~ std::cerr << "DeclScopeFinder: " << (basety ? typeid(*basety).name() : std::string{"0"})
                    //~ << ": " << n.get_name() << " " << useThisDecl
                    //~ << std::endl;

          if (useThisDecl)
            res = dominant_declaration(n).get_scope();
          else
            res = find(basety);
        }

        //
        void handle(const SgDeclType& n)          { res = find(n.get_base_expression()); }
    };

    SgScopeStatement* DeclScopeFinder::find(const SgNode* n)
    {
      return sg::dispatch(DeclScopeFinder{}, n);
    }

    /// \todo remove after adding Ada specific types to stripType
    struct DeclFinder : sg::DispatchHandler<SgDeclarationStatement*>
    {
      void handle(SgNode& n)              { SG_UNEXPECTED_NODE(n); }

      void handle(SgType&)                { /* \todo do nothing for now; should disappear and raise error */ }

      // \todo may need to be reconsidered
      void handle(SgModifierType& n)      { res = associatedDeclaration(n.get_base_type()); }
      void handle(SgAdaSubtype& n)        { res = associatedDeclaration(n.get_base_type()); }
      void handle(SgAdaDerivedType& n)    { res = associatedDeclaration(n.get_base_type()); }
      void handle(SgPointerType& n)       { res = associatedDeclaration(n.get_base_type()); } // \todo NO_POINTER_IN_ADA
      void handle(SgAdaAccessType& n)     { res = associatedDeclaration(n.get_base_type()); } // \todo or scope of underlying type?
      //~ void handle(SgArrayType& n)         { res = associatedDeclaration(n.get_base_type()); }
      // void handle(SgDeclType& n)             { res = pkgStandardScope(); }

      // for records, enums, typedefs, discriminated types, and types with a real declarations
      //   => return the associated declaration.
      void handle(SgNamedType& n)         { res = n.get_declaration(); }
    };

    struct ImportedUnit : sg::DispatchHandler<ImportedUnitResult>
    {
        using base = sg::DispatchHandler<ImportedUnitResult>;

#if OBSOLETE_CODE
        explicit
        ImportedUnit(const SgImportStatement& import)
        : base(), impdcl(import)
        {}
#endif /* OBSOLETE_CODE */

        void handle(const SgNode& n) { SG_UNEXPECTED_NODE(n); }

        void handle(const SgFunctionRefExp& n)
        {
          res = ReturnType{ nameOf(n), &declOf(n), nullptr, &n };
        }

        void handle(const SgAdaUnitRefExp& n)
        {
          const SgDeclarationStatement* dcl = &declOf(n);

          if (const SgAdaGenericDecl* gendcl = isSgAdaGenericDecl(dcl))
            dcl = gendcl->get_declaration();

          ASSERT_not_null(dcl);
          res = ReturnType{ nameOf(n), dcl, nullptr, &n };
        }

        void handle(const SgAdaRenamingRefExp& n)
        {
          res = ReturnType{ nameOf(n), n.get_decl(), n.get_decl(), &n};
        }
    };
  } // end anonymous namespace

  SgScopeStatement* pkgStandardScope()
  {
    return SG_DEREF(stdpkg).get_definition();
  }

  SgType& TypeDescription::typerep_ref() const
  {
    return SG_DEREF(typerep());
  }

  TypeDescription typeRoot(SgType& ty)
  {
    //~ std::cerr << typeid(ty).name() << std::endl;
    return RootTypeFinder::find(&ty);
  }

  TypeDescription typeRoot(SgType* ty)
  {
    return ty ? typeRoot(*ty) : TypeDescription{nullptr};
  }

  TypeDescription typeOfExpr(const SgExpression& exp)
  {
    return sg::dispatch(ExprTypeFinder{}, &exp);
  }

  TypeDescription typeOfExpr(const SgExpression* exp)
  {
    return exp ? typeOfExpr(*exp) : TypeDescription{nullptr};
  }

  SgType* baseOfAccessType(const SgType* ty)
  {
    if (const SgAdaAccessType* acc = isSgAdaAccessType(ty))
      return acc->get_base_type();

    if (const SgAdaDerivedType* drv = isSgAdaDerivedType(ty))
      return baseOfAccessType(drv->get_base_type());

    if (const SgAdaSubtype* sub = isSgAdaSubtype(ty))
      return baseOfAccessType(sub->get_base_type());

    if (const SgPointerType* ptr = isSgPointerType(ty))
      return ptr->get_base_type();

    if (const SgTypedefType* tydef = isSgTypedefType(ty))
      return baseOfAccessType(base_type(*tydef));

    if (const SgModifierType* modty = isSgModifierType(ty))
      return baseOfAccessType(modty->get_base_type());

    if (const SgAdaFormalType* frmty = isSgAdaFormalType(ty))
      return baseOfAccessType(frmty->get_formal_type());

    return nullptr;
  }

  SgType* baseOfAccessType(const SgType& ty)
  {
    return baseOfAccessType(&ty);
  }

  namespace
  {
    // function checks if opname requires special handling for fixed types
    // \note PP
    //     operator scope for fixed types is weirdly bizarre.
    //     behavior re-engineered from ACATS tests.
    //     see types_operators.adb for some test cases.
    bool isFixedSpecialOperator(const std::string& opname)
    {
      return opname == "*" || opname == "/";
    }

    // function checks if operator opname(argtypes.front(), argtypes.back())
    //   needs to be placed in package standard.
    bool isFixedSpecial(const std::string& opname, const SgTypePtrList& argtypes)
    {
      if (!isFixedSpecialOperator(opname))
        return false;

      ROSE_ASSERT(argtypes.size() >= 1);
      return resolvesToFixedType(argtypes.front()) && resolvesToFixedType(argtypes.back());
    }

    // function checks if operator opname(argtypes.front(), argtypes.back())
    //   needs to be placed in package standard.
    bool isFixedSpecial(const std::string& opname, const SgType& ty)
    {
      return isFixedSpecial(opname, { const_cast<SgType*>(&ty) });
    }
  }

  DominantArgInfo
  operatorArgumentWithNamedRootIfAvail(const SgTypePtrList& argtypes)
  {
    ROSE_ASSERT(argtypes.size() == 1 || argtypes.size() == 2);

    SgType* lhsty = argtypes.front();

    if (argtypes.size() == 1 || isSgNamedType(typeRoot(lhsty).typerep()))
      return { lhsty, 0 };

    SgType* rhsty = argtypes.back();

    if (isSgNamedType(typeRoot(rhsty).typerep()))
      return { rhsty, argtypes.size()-1 };

    return { lhsty, 0 };
  }


  OperatorScopeInfo
  operatorScope(const std::string& opname, const SgTypePtrList& argtypes)
  {
    ROSE_ASSERT(argtypes.size());

    if (isFixedSpecial(opname, argtypes))
    {
      // fixedSpecial requires both arguments to be of fixed type
      return { pkgStandardScope(), 0 };
    }

    DominantArgInfo dominantType = operatorArgumentWithNamedRootIfAvail(argtypes);

    return { declarationScope(dominantType.type()), dominantType.pos() };
  }

  SgScopeStatement&
  operatorScope(const std::string& opname, const SgType& ty)
  {
    if (isFixedSpecial(opname, ty))
      return SG_DEREF(pkgStandardScope());

    return SG_DEREF(declarationScope(ty));
  }

  SgScopeStatement*
  operatorScope(const std::string& opname, const SgType* ty)
  {
    if (ty == nullptr) return nullptr;

    return &operatorScope(opname, *ty);
  }


  SgScopeStatement* declarationScope(const SgType* ty)
  {
    return DeclScopeFinder::find(ty);
  }

  SgScopeStatement* declarationScope(const SgType& ty)
  {
    return declarationScope(&ty);
  }

  SgDeclarationStatement* associatedDeclaration(const SgType& ty)
  {
    return sg::dispatch(DeclFinder{}, &ty);
  }

  SgDeclarationStatement* associatedDeclaration(const SgType* ty)
  {
    return ty ? associatedDeclaration(*ty) : nullptr;
  }

#if OBSOLETE_CODE
  const SgExpression&
  importedElement(const SgImportStatement& n)
  {
    const SgExpressionPtrList& lst = n.get_import_list();
    ROSE_ASSERT(lst.size() == 1);

    return SG_DEREF(lst.back());
  }
#endif /* OBSOLETE_CODE */

  std::vector<ImportedUnitResult>
  importedUnits(const SgImportStatement& impdcl)
  {
    //~ return sg::dispatch(ImportedUnit{ impdcl }, &importedElement(impdcl));

    std::vector<ImportedUnitResult> res;
    const SgExpressionPtrList&      lst = impdcl.get_import_list();

    std::transform( lst.begin(), lst.end(),
                    std::back_inserter(res),
                    [](const SgExpression* exp) -> ImportedUnitResult
                    {
                      return sg::dispatch(ImportedUnit{}, exp);
                    }
                  );

    return res;
  }



/*
  std::set<std::string> adaOperatorNames()
  {
    std::string elems[] =

    return std::set<std::string>(elems, elems + sizeof(elems) / sizeof(elems[0]));
  }
(/
*/

  namespace
  {
    bool isOperatorName(const std::string& id)
    {
      static std::set<std::string> adaops =
                            { "+",   "-",   "*",  "/",   "**", "REM", "MOD", "ABS"
                            , "=",   "/=",  "<",  ">",   "<=", ">="
                            , "NOT", "AND", "OR", "XOR", "&"
                            };

      const std::string canonicalname = boost::to_upper_copy(id);

      return adaops.find(canonicalname) != adaops.end();
    }
  }

  std::string convertRoseOperatorNameToAdaOperator(const std::string& name)
  {
    if (name.rfind(si::Ada::roseOperatorPrefix, 0) != 0)
      return "";

    const std::string op = name.substr(si::Ada::roseOperatorPrefix.size());

    if (!isOperatorName(op))
      return "";

    return op;
  }

  std::string convertRoseOperatorNameToAdaName(const std::string& name)
  {
    static const std::string quotes    = "\"";

    if (name.rfind(si::Ada::roseOperatorPrefix, 0) != 0)
      return name;

    const std::string op = name.substr(si::Ada::roseOperatorPrefix.size());

    if (!isOperatorName(op))
      return name;

    return quotes + op + quotes;
  }




  struct ConversionTraversal : AstSimpleProcessing
  {
      explicit
      ConversionTraversal(std::function<void(SgNode*)>&& conversionFn)
      : AstSimpleProcessing(), fn(std::move(conversionFn))
      {}

      void visit(SgNode*) override;

    private:
      std::function<void(SgNode*)> fn;

      ConversionTraversal()                                      = delete;
      ConversionTraversal(const ConversionTraversal&)            = delete;
      ConversionTraversal(ConversionTraversal&&)                 = delete;
      ConversionTraversal& operator=(ConversionTraversal&&)      = delete;
      ConversionTraversal& operator=(const ConversionTraversal&) = delete;
  };

  void ConversionTraversal::visit(SgNode* n)
  {
    fn(n);
  }

  StatementRange
  declsInPackage(SgGlobal& globalScope, const std::string& mainFile)
  {
    auto declaredInMainFile = [&mainFile](const SgDeclarationStatement* dcl)->bool
                              {
                                ROSE_ASSERT(dcl);

                                const Sg_File_Info& fileInfo = SG_DEREF(dcl->get_startOfConstruct());

                                return fileInfo.get_filenameString() == mainFile;
                              };
    auto notDeclaredInMainFile = [&declaredInMainFile](const SgDeclarationStatement* dcl)->bool
                                 {
                                   return !declaredInMainFile(dcl);
                                 };

    SgDeclarationStatementPtrList&          lst   = globalScope.get_declarations();
    SgDeclarationStatementPtrList::iterator zz    = lst.end();
    SgDeclarationStatementPtrList::iterator first = std::find_if(lst.begin(), zz, declaredInMainFile);
    SgDeclarationStatementPtrList::iterator limit = std::find_if(first, zz, notDeclaredInMainFile);

    return std::make_pair(first, limit);
  }


  StatementRange
  declsInPackage(SgGlobal& globalScope, const SgSourceFile& mainFile)
  {
    return declsInPackage(globalScope, mainFile.getFileName());
  }



  /// Traversal to change the comment style from Ada to C++
  struct CommentCxxifier
  {
      explicit
      CommentCxxifier(bool useLineComments)
      : prefix(useLineComments ? "//" : "/*"),
        suffix(useLineComments ? ""   : "*/"),
        commentKind(useLineComments ? PreprocessingInfo::CplusplusStyleComment : PreprocessingInfo:: C_StyleComment)
      {}

      CommentCxxifier(CommentCxxifier&&)                 = default;
      CommentCxxifier(const CommentCxxifier&)            = default;

      void operator()(SgNode*) const;

    private:
      //~ bool lineComments;
      const std::string                      prefix;
      const std::string                      suffix;
      const PreprocessingInfo::DirectiveType commentKind;

      CommentCxxifier()                                  = delete;
      CommentCxxifier& operator=(const CommentCxxifier&) = delete;
      CommentCxxifier& operator=(CommentCxxifier&&)      = delete;
  };


  void CommentCxxifier::operator()(SgNode* n) const
  {
    SgLocatedNode* node = isSgLocatedNode(n);
    if (node == nullptr) return;

    AttachedPreprocessingInfoType* prepInfo = node->getAttachedPreprocessingInfo();
    if (prepInfo == nullptr) return;

    for (PreprocessingInfo* ppinfo : *prepInfo)
    {
      ROSE_ASSERT(ppinfo);

      if (ppinfo->getTypeOfDirective() != PreprocessingInfo::AdaStyleComment) continue;

      std::string comment = ppinfo->getString();

      ROSE_ASSERT(comment.rfind("--", 0) == 0);
      comment.replace(0, 2, prefix);
      comment.append(suffix);
      ppinfo->setString(comment);
      ppinfo->setTypeOfDirective(commentKind);
    }
  }

  struct FunctionCallToOperatorConverter
  {
      FunctionCallToOperatorConverter(bool convOperatorCalls, bool convNamedArgs)
      : convertOperatorCalls(convOperatorCalls), convertNamedArguments(convNamedArgs)
      {}

      ~FunctionCallToOperatorConverter() { executeTransformations(); }

      void executeTransformations() const;

      void operator()(SgNode*);

    private:
      using replacement_t = std::tuple<SgFunctionCallExp*, std::string>;

      std::vector<replacement_t> work;
      const bool                 convertOperatorCalls;
      const bool                 convertNamedArguments;

      FunctionCallToOperatorConverter() = delete;
      //~ FunctionCallToOperatorConverter& operator=(const FunctionCallToOperatorConverter&) = delete;
      //~ FunctionCallToOperatorConverter& operator=(FunctionCallToOperatorConverter&&)      = delete;
  };


  int arity(const SgFunctionCallExp& fncall)
  {
    SgExprListExp* args = fncall.get_args();

    return SG_DEREF(args).get_expressions().size();
  }

  inline
  bool hasNullArg(const SgFunctionCallExp& fncall)
  {
    SgExprListExp* args = fncall.get_args();

    return isSgNullExpression(SG_DEREF(args).get_expressions().at(0));
  }

  void FunctionCallToOperatorConverter::operator()(SgNode* n)
  {
    SgFunctionCallExp*     fncall = isSgFunctionCallExp(n);
    if (  (fncall == nullptr)
       || ((!convertOperatorCalls) && (!fncall->get_uses_operator_syntax()))
       || (arity(*fncall) > 2)
       //~ || (hasNullArg(*fncall))
       )
     return;

    SgFunctionDeclaration* fndecl = fncall->getAssociatedFunctionDeclaration();
    if (fndecl == nullptr || (!definedInStandard(*fndecl))) return;

    // only consider function names that map onto operators
    std::string op = convertRoseOperatorNameToAdaOperator(fndecl->get_name());
    if (op.empty()) return;

    // none of the functions in Standard should be defined.
    ROSE_ASSERT(fndecl->get_definingDeclaration() == nullptr);
    work.emplace_back(fncall, std::move(op));
  }

  using CallToOperatorTransformer = std::function<SgExpression&(SgExpressionPtrList)>;

  template <class BinaryBuilderFn>
  CallToOperatorTransformer tf2(BinaryBuilderFn fn)
  {
    return [fn](SgExpressionPtrList operands) -> SgExpression&
           {
             ROSE_ASSERT(operands.size() == 2);
             SgExpression*       lhs = operands[0];
             SgExpression*       rhs = operands[1];

             ROSE_ASSERT(lhs && rhs);
             SgExpression*       lhs_dummy = sb::buildNullExpression();
             SgExpression*       rhs_dummy = sb::buildNullExpression();

             si::replaceExpression(lhs, lhs_dummy, true /* keep */);
             si::replaceExpression(rhs, rhs_dummy, true /* keep */);

             return SG_DEREF(fn(lhs, rhs));
           };
  }

  template <class UnaryBuilderFn>
  CallToOperatorTransformer tf1(UnaryBuilderFn fn)
  {
    return [fn](SgExpressionPtrList operands) -> SgExpression&
           {
             ROSE_ASSERT(operands.size() == 1);
             SgExpression*       arg = operands[0];

             ROSE_ASSERT(arg);
             SgExpression*       arg_dummy = sb::buildNullExpression();

             si::replaceExpression(arg, arg_dummy, true /* keep */);
             return SG_DEREF(fn(arg));
           };
  }

  SgExpressionPtrList
  simpleArgumentExtractor(const SgFunctionCallExp& n)
  {
    return SG_DEREF(n.get_args()).get_expressions();
  }

  std::size_t
  namedArgumentPosition(const SgInitializedNamePtrList& paramList, const std::string& name)
  {
    SgInitializedNamePtrList::const_iterator aaa = paramList.begin();
    SgInitializedNamePtrList::const_iterator zzz = paramList.end();
    SgInitializedNamePtrList::const_iterator pos = std::find_if( aaa, zzz,
                                                                 [&name](const SgInitializedName* n) -> bool
                                                                 {
                                                                   ASSERT_not_null(n);
                                                                   return boost::iequals(name, n->get_name().getString());
                                                                 }
                                                               );

    if (pos == zzz)
    {
      // \todo this currently occurs for derived types, where the publicly
      //       declared ancestor type differs from the actual parent.
      //       see test case: ancestors.adb
      throw std::logic_error(std::string{"unable to find argument position for "} + name);
    }

    return std::distance(aaa, pos);
  }

  /// resize the container if \ref pos is outside the valid index range
  void extend(SgExpressionPtrList& container, std::size_t pos, SgExpression* val = nullptr)
  {
    const std::size_t sz = pos; // pos needs to be a valid index

    if (container.size() < sz) container.resize(sz, val);
  }

  void FunctionCallToOperatorConverter::executeTransformations() const
  {
    using OperandExtractor = decltype(&simpleArgumentExtractor);
    using BuilderMap = std::map<std::string, CallToOperatorTransformer>;

    static const BuilderMap tfFn2 = { { "=",   tf2(&sb::buildEqualityOp) }
                                    , { "/=",  tf2(&sb::buildNotEqualOp) }
                                    , { "<",   tf2(&sb::buildLessThanOp) }
                                    , { "<=",  tf2(&sb::buildLessOrEqualOp) }
                                    , { ">",   tf2(&sb::buildGreaterThanOp) }
                                    , { ">=",  tf2(&sb::buildGreaterOrEqualOp) }
                                    , { "and", tf2(&sb::buildBitAndOp) }
                                    , { "or",  tf2(&sb::buildBitOrOp) }
                                    , { "xor", tf2(&sb::buildBitXorOp) }
                                    , { "+",   tf2(&sb::buildAddOp) }
                                    , { "-",   tf2(&sb::buildSubtractOp) }
                                    , { "*",   tf2(&sb::buildMultiplyOp) }
                                    , { "/",   tf2(&sb::buildDivideOp) }
                                    , { "rem", tf2(&sb::buildRemOp) }
                                    , { "mod", tf2(&sb::buildModOp) }
                                    , { "**",  tf2(&sb::buildExponentiationOp) }
                                    , { "&",   tf2(&sb::buildConcatenationOp) }
                                    };

    static const BuilderMap tfFn1 = { { "not", tf1(&sb::buildNotOp) }
                                    , { "abs", tf1(&sb::buildAbsOp) }
                                    , { "+",   tf1(&sb::buildUnaryAddOp) }
                                    //~ , { "-",   tf1(&sb::buildMinusOp) }
                                    , { "-",   tf1(&sb::buildUnaryExpression<SgMinusOp>) }
                                    };

    OperandExtractor operandExtractor = convertNamedArguments ? normalizedCallArguments
                                                              : simpleArgumentExtractor;

    for (const replacement_t& r : work)
    {
      SgFunctionCallExp& orig  = SG_DEREF(std::get<0>(r));
      const int          numargs = arity(orig);
      ROSE_ASSERT(numargs == 1 || numargs == 2);
      std::string const  key   = boost::to_lower_copy(std::get<1>(r));
      SgExpression&      repl  = numargs == 1 ? tfFn1.at(key)(operandExtractor(orig))
                                              : tfFn2.at(key)(operandExtractor(orig));

      //~ if (orig.get_parent() == nullptr)
        //~ std::cerr << "parent is null: " << orig->unparseToString() << std::endl;

      repl.set_need_paren(orig.get_need_paren());
      si::replaceExpression(&orig, &repl, false /* delete orig sub-tree */ );
    }
  }

  namespace
  {
    bool declaresException(const SgVariableDeclaration& n)
    {
      const SgInitializedNamePtrList& varlst = n.get_variables();
      const SgInitializedName*        first = varlst.at(0);
      if (!first) return false;

      const SgTypedefType*            ty = isSgTypedefType(first->get_type());

      return ty && (ty->get_name() == si::Ada::exceptionName);
    }
  }


  const SgVariableDeclaration*
  exceptionTypeDecl(const SgVariableDeclaration& n)
  {
    return declaresException(n) ? &n : nullptr;
  }

  const SgVariableDeclaration*
  exceptionTypeDecl(const SgVariableDeclaration* n)
  {
    return (n && declaresException(*n)) ? n : nullptr;
  }

  SgVariableDeclaration*
  exceptionTypeDecl(SgVariableDeclaration& n)
  {
    return declaresException(n) ? &n : nullptr;
  }

  SgVariableDeclaration*
  exceptionTypeDecl(SgVariableDeclaration* n)
  {
    return (n && declaresException(*n)) ? n : nullptr;
  }

  SgFunctionParameterList*
  calleeParameterList(const SgFunctionCallExp& n)
  {
    if (SgFunctionDeclaration* assocfn = n.getAssociatedFunctionDeclaration())
      return assocfn->get_parameterList();

    const SgExpression& target = SG_DEREF(n.get_function());

    if (const SgAdaRenamingRefExp* renex = isSgAdaRenamingRefExp(&target))
    {
      const SgAdaRenamingDecl& rendcl = SG_DEREF(renex->get_decl());

      if (const SgAdaSubroutineType* routty = isSgAdaSubroutineType(rendcl.get_type()))
        return routty->get_parameterList();

      // return nullptr;
    }
    else if (const SgPointerDerefExp* drfexp = isSgPointerDerefExp(&target))
    {
      if (const SgAdaSubroutineType* routty = isSgAdaSubroutineType(drfexp->get_type()))
        return routty->get_parameterList();
    }

    return nullptr;
  }

  SgFunctionParameterList*
  calleeParameterList(const SgFunctionCallExp* n)
  {
    return n ? calleeParameterList(*n) : nullptr;
  }

  namespace
  {
    /// \param n a class type
    /// \param publicAncestorConstraint, if false gets the base
    ///
    SgType*
    baseTypeOfClass(const SgClassType& n, bool publicInfoPreferred = false)
    {
      // \todo this requires revision
      //       do we want to get the base type constraint from get_adaParentType
      //       or the precise base class from the definition?
      SgType*             res       = nullptr;
      SgClassDeclaration& cldcl     = SG_DEREF(isSgClassDeclaration(n.get_declaration()));
      SgClassDeclaration& nondefdcl = SG_DEREF(isSgClassDeclaration(cldcl.get_firstNondefiningDeclaration()));
      SgClassDeclaration& defdcl    = SG_DEREF(isSgClassDeclaration(nondefdcl.get_definingDeclaration()));
      SgBaseClass*        basecl    = nullptr;

      // the specific parent is known by the defining declaration
      const bool          privateDecl = isPrivate(nondefdcl);
      const bool          privateDefn = isPrivate(defdcl);

      // get the direct inheritance if both decls are public, both decls are private,
      //   or if publicInfoPreferred was not set.
      if ((privateDecl == privateDefn) || !publicInfoPreferred)
      {
        SgClassDefinition&  cldef = SG_DEREF(defdcl.get_definition());
        SgBaseClassPtrList& bases = cldef.get_inheritances();

        if (bases.size()) basecl = bases.front();
      }

      // when publicInfoPreferred was requested or when the previous method failed.
      // \todo maybe this should just be an else branch of the previous condition..
      if (basecl == nullptr)
      {
        basecl = nondefdcl.get_adaParentType();
      }

      if (const SgExpBaseClass* basexp = isSgExpBaseClass(basecl))
      {
        //~ std::cerr << "BaseTypeFinder: found expression base class" << std::endl;
        res = typeOfExpr(basexp->get_base_class_exp()).typerep();
      }
      else if (basecl)
      {
        SgClassDeclaration& basedcl = SG_DEREF(basecl->get_base_class());

        res = basedcl.get_type();
      }

      return res;
    }



    /// checks if the scope of \ref dcl is visible from \ref scope.
    bool
    isVisibleFrom(const SgDeclarationStatement& dcl, const SgScopeStatement& scope)
    {
      ScopePath path = pathToGlobal(scope);
      auto      lim  = path.end();

      return lim != std::find(path.begin(), lim, dcl.get_scope());
    }




#if 0
    SgNamedType& assertNamedType(SgType* ty)
    {
      SgNamedType* res = isSgNamedType(ty);
      ASSERT_not_null(res);

      return *res;
    }

    /// extracts the base type from a base and inherited function signature.
    /// \note
    ///   the implementation matches the type of each argument position
    ///   and return type and returns the first mismatched type in \ref orig.
    SgNamedType&
    extractBaseTypeFromSignature(const SgFunctionType& orig, const SgFunctionType& derv)
    {
      if (orig.get_return_type() != derv.get_return_type())
        return assertNamedType(orig.get_return_type());

      SgTypePtrList& origParams = SG_DEREF(orig.get_argument_list()).get_arguments();
      SgTypePtrList& dervParams = SG_DEREF(derv.get_argument_list()).get_arguments();
      ASSERT_require(origParams.size() == dervParams.size());

      auto iterpair = std::mismatch( origParams.begin(), origParams.end(), dervParams.begin() );
      ASSERT_require(iterpair.first != origParams.end());

      return assertNamedType(*iterpair.first);
    }

    /// produce a linearized list of ancestor classes (records) of cldcl
    std::vector<const SgType*>
    linearizePublicAncestors(const SgClassDeclaration& cldcl, const SgScopeStatement* /*scope_opt*/ = nullptr)
    {
      std::vector<const SgType*> res;
      SgClassType*               clsty = cldcl.get_type();

      while (clsty)
      {
        res.push_back(clsty);

        // \todo passing true (only public) is not entirely correct
        //       b/c whether private or public is needed depends on
        //       whether the FULL decl associated with clsty is
        //       visible from scope \ref scope_opt.
        clsty = isSgClassType(baseTypeOfClass(*clsty, true /* only public */));
      }

      return res;
    }

    /// tests if basety is a public ancestor of drvty.
    bool isPublicAncestorOf(const SgNamedType& basety, const SgNamedType& drvty)
    {
      // find the most derived public declaration of drvty
      SgDeclarationStatement& drvdeclany    = SG_DEREF(drvty.get_declaration());

      // if the def is public => immediate parent is public => basetype is a public ancestor
      SgDeclarationStatement* drvdeclDef    = drvdeclany.get_definingDeclaration();
      if (drvdeclDef && (!isPrivate(*drvdeclDef)))
        return true;

      SgDeclarationStatement& drvdeclNondef = SG_DEREF(drvdeclany.get_firstNondefiningDeclaration());
      SgClassDeclaration*     drvclssNondef = isSgClassDeclaration(&drvdeclNondef);

      // PP believes that only classes can have distinct public and private ancestor/parent
      //    specification.
      if (drvclssNondef == nullptr)
        return isPrivate(drvdeclNondef);

      // check if basety is an ancestor of the most derived public declaration inheritance spec
      std::vector<const SgType*> ancestors = linearizePublicAncestors(*drvclssNondef);
      auto                       lim = ancestors.end();

      return lim != std::find(ancestors.begin(), lim, &basety);
    }
#endif /* 0 */


    bool overridesBase(const SgFunctionType& dervFunTy, const SgType& dervTy, const SgFunctionType& baseFunTy, const SgType& baseTy)
    {
      auto validBaseOverriderType =
         [&dervTy, &baseTy](const SgType* drv, const SgType* bas) -> bool
         {
           return (  ((drv == &dervTy) && (bas == &baseTy))
                  || ((drv != &dervTy) && (bas == drv))
                  );
         };

      const SgTypePtrList& dervArgs = SG_DEREF(dervFunTy.get_argument_list()).get_arguments();
      const SgTypePtrList& baseArgs = SG_DEREF(baseFunTy.get_argument_list()).get_arguments();

      return (  validBaseOverriderType(dervFunTy.get_return_type(), baseFunTy.get_return_type())
             && std::equal( dervArgs.begin(), dervArgs.end(),
                            baseArgs.begin(), baseArgs.end(),
                            validBaseOverriderType
                          )
             );
    }

    SgFunctionSymbol*
    symbolForBaseFunction(SgSymbol& sym, const SgFunctionType& dervFunTy, const SgType& baseTy, const SgType& dervTy)
    {
      SgFunctionSymbol* res = nullptr;

      if (SgFunctionType* candty = isSgFunctionType(sym.get_type()))
      {
        if (overridesBase(dervFunTy, dervTy, *candty, baseTy))
        {
          res = isSgFunctionSymbol(&sym);
          ASSERT_not_null(res);
        }
      }

      return res;
    }


    /// finds a function symbol (or inherited function symbol) with the same name
    ///   as fndcl, such as the symbol has a similar type-signature with a derived class replaced
    ///   by the class \ref cls.
    SgFunctionSymbol*
    findFunctionSymbolForType( const SgClassType& basety,
                               const SgType& dervty,
                               std::string fnname,
                               const SgFunctionType& drvFunType
                             )
    {
      const SgClassDeclaration& clsdcl = SG_DEREF(isSgClassDeclaration(basety.get_declaration()));
      const SgScopeStatement&   scope  = SG_DEREF(clsdcl.get_scope());
      SgSymbol*                 sym    = scope.lookup_symbol(fnname, nullptr, nullptr);
      SgFunctionSymbol*         res    = nullptr;

      while (sym && !res)
      {
        res = symbolForBaseFunction(*sym, drvFunType, basety, dervty);
        sym = scope.next_any_symbol();
      }

      return res;
    }

    SgDeclarationStatement&
    instantiatedDeclaration(const SgAdaGenericInstanceDecl& n)
    {
      SgScopeStatement&       scope = SG_DEREF(n.get_instantiatedScope());
      SgDeclarationStatement* res   = isSgDeclarationStatement(scope.firstStatement());

      ASSERT_not_null(res);
      ASSERT_require(res == scope.lastStatement()); // any instantiation contains one decl
      return *res;
    }

    struct AccessibleArgumentListFinder : sg::DispatchHandler<SgFunctionParameterList*>
    {
        using base = sg::DispatchHandler<SgFunctionParameterList*>;

        explicit
        AccessibleArgumentListFinder(const SgFunctionCallExp& callexp)
        : base(), call(&callexp)
        {}

        void handle(const SgNode& n)                   { SG_UNEXPECTED_NODE(n); }

        //
        // types

        void handle(const SgAdaSubroutineType& n)      { res = n.get_parameterList(); }

        //
        // declarations
        void handle(const SgFunctionDeclaration& n)    { res = n.get_parameterList(); }
        void handle(const SgAdaGenericInstanceDecl& n) { res = find(&instantiatedDeclaration(n)); }

        //
        // expressions

        void handle(const SgPointerDerefExp& n)        { res = find(n.get_type()); }
        void handle(const SgFunctionRefExp& n)         { res = find(n.get_symbol()); }
        void handle(const SgAdaUnitRefExp& n)          { res = find(n.get_decl()); }

        void handle(const SgAdaRenamingRefExp& n)
        {
          // \todo give AdaRenamingRefExp a proper type
          // res = find(n.get_type());

          res = find( SG_DEREF(n.get_decl()).get_type() );
        }

        //
        // symbols

        // trust Asis for directly linked functions -> no visibility check
        void handle(const SgFunctionSymbol& n)
        {
          res = find(n.get_declaration());
        }

        void handle(const SgAdaInheritedFunctionSymbol& n)
        {
          SgFunctionDeclaration& fndcl    = SG_DEREF(n.get_declaration());

          if (isVisibleFrom(fndcl, sg::ancestor<const SgScopeStatement>(*call)))
          {
            res = find(&fndcl);
            return;
          }

          res = find(n.get_publiclyVisibleFunctionSymbol());
        }

        //
        // helpers

        SgFunctionParameterList* find(SgNode* n)
        {
          return find(n, *call);
        }

        static
        SgFunctionParameterList* find(SgNode* n, const SgFunctionCallExp& call);

      private:
        const SgFunctionCallExp* call;
    };

    SgFunctionParameterList*
    AccessibleArgumentListFinder::find(SgNode* n, const SgFunctionCallExp& call)
    {
      return sg::dispatch(AccessibleArgumentListFinder{call}, n);
    }
  }

  SgFunctionSymbol*
  findPubliclyVisibleFunction( SgFunctionSymbol& fnsym,
                               const SgFunctionType& drvFunTy,
                               const SgNamedType& dervTy
                             )
  {
    // note: the function returns fnsym in many cases for which it was
    //       not written.

    //~ std::cerr << dervTy.get_name() << " " << &dervTy << std::endl;

    const SgDeclarationStatement& dervDcl      = SG_DEREF(dervTy.get_declaration());
    const SgDeclarationStatement& firstDervDcl = SG_DEREF(dervDcl.get_firstNondefiningDeclaration());
    if (isPrivate(firstDervDcl))
      return &fnsym; // nullptr

    const SgDeclarationStatement* defnDervDcl  = firstDervDcl.get_definingDeclaration();

    if (defnDervDcl == nullptr || (!isPrivate(*defnDervDcl)))
      return &fnsym;

    const SgClassDeclaration*     firstClsDcl  = isSgClassDeclaration(&firstDervDcl);
    if (firstClsDcl == nullptr)
      return &fnsym;

    SgFunctionSymbol*  res   = &fnsym; // nullptr
    const SgClassType& clsty = SG_DEREF(firstClsDcl->get_type());

    if (const SgClassType* basety = isSgClassType(baseTypeOfClass(clsty, true /* public ancestor preferred*/)))
    {
      res = findFunctionSymbolForType(*basety, dervTy, fnsym.get_name(), drvFunTy);
    }

    return res ? res : &fnsym;
  }

  SgExpressionPtrList
  normalizedCallArguments2(const SgFunctionCallExp& n, const SgFunctionParameterList& arglist)
  {
    SgExpressionPtrList           res;
    SgExpressionPtrList&          orig = SG_DEREF(n.get_args()).get_expressions();
    const size_t                  posArgLimit = positionalArgumentLimit(orig);
    SgExpressionPtrList::iterator aaa = orig.begin();
    SgExpressionPtrList::iterator pos = aaa + posArgLimit;
    SgExpressionPtrList::iterator zzz = orig.end();

    res.reserve(orig.size());
    std::copy(aaa, pos, std::back_inserter(res));

    const SgInitializedNamePtrList& parmList = arglist.get_args();

    ROSE_ASSERT(res.size() <= parmList.size());
    extend(res, parmList.size()); // make arglist as long as function parameter list

    std::for_each( pos, zzz,
                   [&parmList, &res](SgExpression* e) -> void
                   {
                     SgActualArgumentExpression& arg = SG_DEREF(isSgActualArgumentExpression(e));
                     const std::size_t           pos = namedArgumentPosition(parmList, arg.get_argument_name());

                     ASSERT_require(res[pos] == nullptr); // do not overwrite a valid arg
                     res[pos] = arg.get_expression();
                   }
                 );

    return res;
  }

  SgExpressionPtrList
  normalizedCallArguments(const SgFunctionCallExp& n)
  {
    SgFunctionParameterList*      fnparms  = AccessibleArgumentListFinder::find(n.get_function(), n);
    if (fnparms == nullptr)
      throw std::logic_error("unable to retrieve associated function parameter list");

    return normalizedCallArguments2(n, *fnparms);
  }

  std::size_t
  normalizedArgumentPosition(const SgFunctionCallExp& call, const SgExpression& arg)
  {
    ROSE_ASSERT(isSgActualArgumentExpression(&arg) == nullptr);

    SgExpressionPtrList                 normargs = si::Ada::normalizedCallArguments(call);
    SgExpressionPtrList::iterator const beg = normargs.begin();
    SgExpressionPtrList::iterator const lim = normargs.end();
    SgExpressionPtrList::iterator const pos = std::find(beg, lim, &arg);

    if (pos == lim) throw std::logic_error{"si::Ada::normalizedArgumentPosition: unable to find argument position"};

    return std::distance(beg, pos);
  }

  namespace
  {
    bool
    nodeProperty(const Sg_File_Info* n, bool (Sg_File_Info::*property)() const)
    {
      return n && (n->*property)();
    }

    bool
    nodeProperty(const SgLocatedNode& n, bool (Sg_File_Info::*property)() const)
    {
      return (  nodeProperty(n.get_file_info(),        property)
             || nodeProperty(n.get_startOfConstruct(), property)
             || nodeProperty(n.get_endOfConstruct(),   property)
             );
    }
  }


  bool blockExistsInSource(const SgBasicBlock& n)
  {
    if (nodeProperty(n, &Sg_File_Info::isCompilerGenerated))
      return false;

    const SgNode* par = n.get_parent();

    bool blockIsFrontendGenerated = (  isSgFunctionDefinition(par)
                                    || isSgTryStmt(par)
                                    || isSgIfStmt(par)
                                    || isSgSwitchStatement(par)
                                    || isSgForStatement(par)
                                    || isSgWhileStmt(par)
                                    || isSgAdaLoopStmt(par)
                                    || isSgAdaAcceptStmt(par)
                                    || isSgCatchOptionStmt(par)
                                    // \todo AdaSelectStmt, AdaGenericInstance, ..
                                    );

    return !blockIsFrontendGenerated;
  }

  bool blockExistsInSource(const SgBasicBlock* n)
  {
    return n && blockExistsInSource(*n);
  }

  ScopePath pathToGlobal(const SgScopeStatement& n)
  {
    ScopePath               res;
    const SgScopeStatement* curr = &n;

    /// add all scopes on the path to the global scope
    while (!isSgGlobal(curr))
    {
      // check for circular scopes
      // ASSERT_require(std::find(res.rbegin(), res.rend(), curr) == res.rend());
      res.push_back(curr);
      curr = logicalParentScope(*curr);
    }

    return res;
  }

  void conversionTraversal(std::function<void(SgNode*)>&& fn, SgNode* root)
  {
    ASSERT_not_null(root);

    ConversionTraversal converter(std::move(fn));

    converter.traverse(root, preorder);
  }

  void convertAdaToCxxComments(SgNode* root, bool cxxLineComments)
  {
    conversionTraversal(CommentCxxifier{cxxLineComments}, root);
  }

  void convertToCaseSensitiveSymbolTables(SgNode* root)
  {
    conversionTraversal(convertSymbolTablesToCaseSensitive_internal, root);
  }

  void convertToOperatorRepresentation(SgNode* root, bool convertCallSyntax, bool convertNamedArguments)
  {
    conversionTraversal(FunctionCallToOperatorConverter{convertCallSyntax, convertNamedArguments}, root);
  }


  /*
  template<class T>
  T powInt(T num, size_t exp, size_t res = 1)
  {
    if (exp == 0)
      return res;

    if ((exp % 2) == 0)
      return powInt(num*num, exp/2, res);

    return powInt(num, exp-1, num*res);
  }
*/

namespace
{
  bool
  isBasedDelimiter(char ch)
  {
    return ch == '#' || ch == ':';
  }

  bool
  isExponentChar(char ch)
  {
    return ch == 'E' || ch == 'e';
  }

  std::pair<int, bool>
  check(int s, int m)
  {
    return std::make_pair(s, s < m);
  }

  std::pair<int, bool>
  char2Val(char c, int max)
  {
    using ResultType = std::pair<int, bool>;

    if ((c >= '0') && (c <= '9'))
      return check(c - '0', max);

    if ((c >= 'A') && (c <= 'F'))
      return check(c - 'A' + 10, max);

    if ((c >= 'a') && (c <= 'f'))
      return check(c - 'a' + 10, max);

    return ResultType{0, false};
  }

  template <class T>
  std::pair<T, const char*>
  parseDec(const char* buf, int base = 10)
  {
    ROSE_ASSERT((*buf != '\0') && (base > 0));

    // In constants folded by ASIS there can be a leading '-'
    //   otherwise a '-' is represented as unary operator.
    const int negmul = (*buf == '-') ? -1 : 1;

    if (negmul < 0) ++buf;

    ROSE_ASSERT((*buf != '\0') && char2Val(*buf, base).second);
    T res = 0;

    while (*buf != '\0')
    {
      const auto v = char2Val(*buf, base);

      // \todo why is this exit needed?
      if (!v.second)
        return std::make_pair(res, buf);

      // The digits cannot be summed all positive and negmul only applied once,
      // because this leads to an integer underflow for System.Min_Int.
      // While the underflow is likely benign (System.Min_Int == -System.Min_Int)
      // for a two's complement representation, it seems more prudent to avoid it
      // altogether.
      ROSE_ASSERT(  (std::numeric_limits<T>::lowest() / base <= res)
                 && (std::numeric_limits<T>::max() / base >= res)
                 && ("arithmethic over-/underflow during literal parsing (mul)")
                 );
      res = res*base;

      ROSE_ASSERT(  ((negmul < 0) && (std::numeric_limits<T>::lowest() + v.first <= res))
                 || ((negmul > 0) && (std::numeric_limits<T>::max() - v.first >= res))
                 || (!"arithmethic over-/underflow during literal parsing (add)")
                 );
      res += (v.first * negmul);

      ++buf;

      // skip underscores
      // \note this is imprecise, since an underscore must be followed
      //       by an integer.
      while (*buf == '_') ++buf;
    }

    return std::make_pair(res, buf);
  }

  template <class T>
  std::pair<T, const char*>
  parseFrac(const char* buf, size_t base = 10)
  {
    ROSE_ASSERT((*buf != '\0') && char2Val(*buf, base).second);

    T      res = 0;
    size_t divisor = 1*base;

    while ((*buf != '\0') && (!isBasedDelimiter(*buf)))
    {
      const auto v = char2Val(*buf, base);
      ROSE_ASSERT(v.second);

      T val = v.first;

      if (val)
      {
        ROSE_ASSERT(!std::isnan(divisor));

        T frac = val/divisor;
        ROSE_ASSERT(!std::isnan(frac));

        res += frac;
      }

      divisor = divisor*base;

      ++buf;

      // skip underscores
      // \note (this is imprecise, since an underscore must be followed
      //       by an integer.
      while (*buf == '_') ++buf;
    }

    return std::make_pair(res, buf);
  }


  std::pair<int, const char*>
  parseExp(const char* buf)
  {
    long int exp = 0;

    if (isExponentChar(*buf))
    {
      ++buf;
      const bool positiveE = (*buf != '-');

      // skip sign
      if (!positiveE || (*buf == '+')) ++buf;

      std::tie(exp, buf) = parseDec<long int>(buf, 10);

      if (!positiveE) exp = -exp;
    }

    return std::make_pair(exp, buf);
  }

  template <class T>
  T computeLiteral(T val, int base, int exp)
  {
    T res = val * std::pow(base, exp);

    // std::cerr << "complit: " << res << std::endl;
    return res;
  }


  long long int
  basedLiteral(long long int res, const char* cur, int base)
  {
    int exp = 0;

    ROSE_ASSERT(isBasedDelimiter(*cur));

    ++cur;
    ROSE_ASSERT(  (res >= std::numeric_limits<decltype(base)>::min())
               && (res <= std::numeric_limits<decltype(base)>::max())
               );
    base = res;

    std::tie(res, cur) = parseDec<long long int>(cur, base);

    if (isBasedDelimiter(*cur))
    {
      ++cur;

      std::tie(exp, cur) = parseExp(cur);
    }

    return computeLiteral(res, base, exp);
  }

  bool isNamedArgument(const SgExpression* expr)
  {
    return isSgActualArgumentExpression(expr);
  }
} // anonymous


long long int convertIntegerLiteral(const char* img)
{
  long long int res  = 0;
  int           base = 10;
  int           exp  = 0;
  const char*   cur  = img;

  std::tie(res, cur) = parseDec<long long int>(cur);

  if (isBasedDelimiter(*cur))
  {
    return basedLiteral(res, cur, base);
  }

  if (*cur == '.')
  {
    throw std::logic_error("SageInterfaceAda.C: Ada decimal literals not yet handled");

    //~ long int decimal = 0;

    //~ ++cur;
    //~ std::tie(decimal, cur) = parseDec<long int>(cur);
  }

  std::tie(exp, cur) = parseExp(cur);

  //~ logWarn() << "r: "
            //~ << res << ' ' << base << '^' << exp << '\n'
            //~ << std::endl;

  return computeLiteral(res, base, exp);
}

std::string convertStringLiteral(const char* textrep)
{
  ROSE_ASSERT(textrep);

  std::stringstream buf;
  const char        delimiter = *textrep;
  ROSE_ASSERT(delimiter == '"' || delimiter == '%');

  ++textrep;
  while (*(textrep+1))
  {
    // a delimiter within a text requires special handling
    //   -> skip the first occurrence if the delimiter is doubled
    if (*textrep == delimiter)
    {
      ++textrep;
      ROSE_ASSERT(*textrep == delimiter);
    }

    buf << *textrep;
    ++textrep;
  }

  return std::move(buf).str();
}


long double convertRealLiteral(const char* img)
{
  std::string litText{img};

  boost::replace_all(litText, "_", "");

  // handle 'normal' real literals
  if (litText.find_first_of("#:") == std::string::npos)
  {
    // logWarn() << "R: " << conv<long double>(litText) << std::endl;
    return boost::lexical_cast<long double>(litText);
  }

  // handle based real literals
  long double dec  = 0;
  long double frac = 0;
  int         base = 10;
  int         exp  = 0;
  const char* cur  = img;

  std::tie(base, cur) = parseDec<long int>(cur);
  ROSE_ASSERT(isBasedDelimiter(*cur));

  ++cur;
  std::tie(dec, cur) = parseDec<long double>(cur, base);

  if (*cur == '.')
  {
    ++cur;
    std::tie(frac, cur) = parseFrac<long double>(cur, base);
  }

  const long double res = dec + frac;

  ROSE_ASSERT(isBasedDelimiter(*cur));
  ++cur;

  std::tie(exp, cur) = parseExp(cur);

/*
    logWarn() << "r: "
            << res << ' ' << dec << '+' << frac << ' ' << base << ' ' << exp << '\n'
            << res * base
            << std::endl;
*/
  return computeLiteral(res, base, exp);
}

char convertCharLiteral(const char* img)
{
  // recognized form: 'l' -> l
  ASSERT_not_null(img);

  const char delimiter = *img;
  ROSE_ASSERT(delimiter == '\'');

  const char res = img[1];
  // \todo could we have a null character in quotes?
  ROSE_ASSERT(res && img[2] == '\'');
  return res;
}


std::vector<PrimitiveParameterDesc>
primitiveParameterPositions(const SgFunctionDeclaration& dcl)
{
  std::vector<PrimitiveParameterDesc> res;
  size_t                              parmpos = 0;
  const SgScopeStatement*             scope = dcl.get_scope();

  for (const SgInitializedName* parm : SG_DEREF(dcl.get_parameterList()).get_args())
  {
    ASSERT_not_null(parm);
    // PP: note to self: BaseTypeDecl::find does NOT skip the initial typedef decl
    const SgDeclarationStatement* tydcl = associatedDeclaration(parm->get_type());

    if (tydcl && sameCanonicalScope(tydcl->get_scope(), scope))
      res.emplace_back(parmpos, parm);

    ++parmpos;
  }

  return res;
}

std::vector<PrimitiveParameterDesc>
primitiveParameterPositions(const SgFunctionDeclaration* dcl)
{
  ASSERT_not_null(dcl);

  return primitiveParameterPositions(*dcl);
}

size_t
positionalArgumentLimit(const SgExpressionPtrList& arglst)
{
  SgExpressionPtrList::const_iterator aaa = arglst.begin();
  SgExpressionPtrList::const_iterator pos = std::find_if(aaa, arglst.end(), isNamedArgument);

  return std::distance(aaa, pos);
}

size_t
positionalArgumentLimit(const SgExprListExp& args)
{
  return positionalArgumentLimit(args.get_expressions());
}

size_t
positionalArgumentLimit(const SgExprListExp* args)
{
  ROSE_ASSERT(args);

  return positionalArgumentLimit(*args);
}


SgScopeStatement*
overridingScope(const SgExprListExp& args, const std::vector<PrimitiveParameterDesc>& primitiveArgs)
{
  using PrimitiveParmIterator = std::vector<PrimitiveParameterDesc>::const_iterator;
  using ArgumentIterator      = SgExpressionPtrList::const_iterator;

  if (primitiveArgs.size() == 0)
    return nullptr;

  const SgExpressionPtrList& arglst      = args.get_expressions();
  const size_t               posArgLimit = positionalArgumentLimit(args);
  PrimitiveParmIterator      aa          = primitiveArgs.begin();
  PrimitiveParmIterator      zz          = primitiveArgs.end();

  // check all positional arguments
  while ((aa != zz) && (aa->pos() < posArgLimit))
  {
    const SgExpression* arg = arglst.at(aa->pos());

    if (const SgDeclarationStatement* tydcl = associatedDeclaration(arg->get_type()))
      return tydcl->get_scope();

    ++aa;
  }

  ROSE_ASSERT(posArgLimit <= arglst.size());
  ArgumentIterator firstNamed = arglst.begin() + posArgLimit;
  ArgumentIterator argLimit   = arglst.end();

  // check all named arguments
  while (aa != zz)
  {
    const std::string parmName = SG_DEREF(aa->name()).get_name();
    auto              sameNamePred = [&parmName](const SgExpression* arg) -> bool
                                     {
                                       const SgActualArgumentExpression* actarg = isSgActualArgumentExpression(arg);

                                       ROSE_ASSERT(actarg);
                                       return parmName == std::string{actarg->get_argument_name()};
                                     };
    ArgumentIterator argpos   = std::find_if(firstNamed, argLimit, sameNamePred);

    ++aa;

    if (argpos == argLimit)
      continue;

    if (const SgDeclarationStatement* tydcl = associatedDeclaration((*argpos)->get_type()))
      return tydcl->get_scope();
  }

  // not found
  return nullptr;
}


SgScopeStatement*
overridingScope(const SgExprListExp* args, const std::vector<PrimitiveParameterDesc>& primitiveArgs)
{
  ROSE_ASSERT(args);

  return overridingScope(*args, primitiveArgs);
}

namespace
{
  struct BaseTypeFinder : sg::DispatchHandler<SgType*>
  {
    void handle(const SgNode& n)            { SG_UNEXPECTED_NODE(n); }

    // \todo not sure if this should  be reachable..
    // void handle(const SgType& n)            { }

    // base cases
/*
    void handle(const SgAdaDerivedType& n)  { res = { &n }; }
    void handle(const SgLongLongType& n)    { res = { &n }; }
    void handle(const SgLongDoubleType& n)  { res = { &n }; }
    void handle(const SgFixedType& n)       { res = { &n }; }
    void handle(const SgAdaAccessType& n)   { res = { &n }; }
    void handle(const SgArrayType& n)       { res = { &n }; }

    // reachable?
    void handle(const SgPointerType& n)     { res = { &n }; }
*/

    // non-base cases
    void handle(const SgTypedefType& n)
    {
      res = &base_type(n);
    }

    void handle(const SgAdaFormalType& n)
    {
      res = n.get_formal_type();
    }

    void handle(const SgAdaDiscriminatedType& n)
    {
      const SgAdaDiscriminatedTypeDecl& dcl = SG_DEREF(isSgAdaDiscriminatedTypeDecl(n.get_declaration()));
      const SgDeclarationStatement&     subdcl = SG_DEREF(dcl.get_discriminatedDecl());

      res = baseType(si::getDeclaredType(&subdcl));
    }

    void handle(const SgClassType& n)
    {
      res = baseTypeOfClass(n);
    }

    void handle(const SgEnumType& n)
    {
      // \todo baseEnumDeclaration may skips some intermediate types...
      if (SgEnumDeclaration* enmdcl = baseEnumDeclaration(const_cast<SgEnumType&>(n)))
      {
        SgEnumDeclaration* defdecl = isSgEnumDeclaration(enmdcl->get_definingDeclaration());

        if (defdecl == nullptr) defdecl = enmdcl;

        res = defdecl->get_adaParentType();
      }
    }

    // \todo should this handle SgAdaSubtype also? void handle(const SgAdaSubtype& n) ..
  };
}

SgType*
baseType(const SgType& ty)
{
  return baseType(&ty);
}

SgType*
baseType(const SgType* ty)
{
  //~ std::cerr << (ty ? typeid(*ty).name() : std::string{"<0>"}) << std::endl;
  return sg::dispatch(BaseTypeFinder{}, ty);
}

SgEnumDeclaration*
baseEnumDeclaration(SgType& ty)
{
  return baseEnumDeclaration(&ty);
}

SgEnumDeclaration*
baseEnumDeclaration(SgType* ty)
{
  SgDeclarationStatement* basedcl = associatedDeclaration(ty);

  if (SgTypedefDeclaration* tydcl = isSgTypedefDeclaration(basedcl))
    return baseEnumDeclaration(tydcl->get_base_type());

  return isSgEnumDeclaration(basedcl);
}


bool
explicitNullProcedure(const SgFunctionDefinition& fndef)
{
  const SgBasicBlock& body = SG_DEREF(fndef.get_body());

  return body.get_statements().empty();
}

bool
explicitNullRecord(const SgClassDefinition& recdef)
{
  return recdef.get_members().empty();
}

bool
isReverseForLoop(const SgForStatement& n)
{
  return isSgMinusMinusOp(n.get_increment());
}

bool
isReverseForLoop(const SgForStatement* n)
{
  return n && isReverseForLoop(*n);
}

bool
isReverseForLoop(const SgForInitStatement& n)
{
  return isReverseForLoop(isSgForStatement(n.get_parent()));
}

bool
isReverseForLoop(const SgForInitStatement* n)
{
  return n && isReverseForLoop(*n);
}



boost::optional<bool>
booleanConstant(const SgExpression* e)
{
  using ResultType = boost::optional<bool>;

  if (const SgEnumVal* enumval = isSgEnumVal(e))
    if (isBooleanType(enumval->get_type()))
      return ResultType{enumval->get_name() == "True"}; // spelling in AdaType.C

  return ResultType{};
}

boost::optional<bool>
booleanConstant(const SgExpression& e)
{
  return booleanConstant(&e);
}


namespace
{
  // In contrast to si::getEnclosingScope, which seems to return the actual parent scope
  //   in the AST, this returns the logical parent.
  // e.g., a separate function has the package as logical parent, but
  //       the global scope as the actual parent scope.
  struct LogicalParent : sg::DispatchHandler<SgScopeStatement*>
  {
    void handle(const SgNode& n)                 { SG_UNEXPECTED_NODE(n); }

    void handle(const SgAdaPackageSpecDecl& n)   { res = n.get_scope(); }
    void handle(const SgAdaTaskSpecDecl& n)      { res = n.get_scope(); }
    void handle(const SgAdaTaskTypeDecl& n)      { res = n.get_scope(); }
    void handle(const SgAdaProtectedSpecDecl& n) { res = n.get_scope(); }
    void handle(const SgAdaProtectedTypeDecl& n) { res = n.get_scope(); }
    void handle(const SgFunctionDeclaration& n)  { res = n.get_scope(); }
    void handle(const SgAdaGenericDecl& n)       { res = n.get_scope(); }
    void handle(const SgClassDeclaration& n)     { res = n.get_scope(); }

    // do not look beyond global
    // (during AST construction the parents of global may not yet be properly linked).
    void handle(const SgGlobal&)                 { res = nullptr; }

    // For Ada features that have a spec/body combination
    //   we define the logical parent to be the spec.
    // This way, both scopes are on the path for identifying overloaded symbols.
    void handle(const SgAdaPackageBody& n)       { res = n.get_spec(); }
    void handle(const SgAdaTaskBody& n)          { res = n.get_spec(); }
    void handle(const SgAdaProtectedBody& n)     { res = n.get_spec(); }

    void handle(const SgAdaPackageSpec& n)       { res = fromParent(n); }
    void handle(const SgAdaTaskSpec& n)          { res = fromParent(n); }
    void handle(const SgAdaProtectedSpec& n)     { res = fromParent(n); }
    void handle(const SgFunctionDefinition& n)   { res = fromParent(n); }
    void handle(const SgAdaGenericDefn& n)       { res = fromParent(n); }
    void handle(const SgClassDefinition& n)      { res = fromParent(n); }

    void handle(const SgScopeStatement& n)
    {
      const SgNode* node = &n;

      res = si::getEnclosingScope(const_cast<SgNode*>(node));
    }

    static
    SgScopeStatement*
    fromParent(const SgNode& n);
  };

  SgScopeStatement*
  LogicalParent::fromParent(const SgNode& n)
  {
    return sg::dispatch(LogicalParent{}, n.get_parent());
  }


  struct AssociatedDecl : sg::DispatchHandler<SgDeclarationStatement*>
  {
    // for any valid return
    ReturnType filterReturnType(ReturnType v)   { return v; }

    // for non-declaration statements (e.g., InitializedName) and other unhandled symbols
    ReturnType filterReturnType(const SgNode*)  { return nullptr; }

    // all non-symbols
    auto handlesyms(const SgNode& n, const SgNode&) -> const SgNode* { SG_UNEXPECTED_NODE(n); }

    // calls get_declaration on the most derived SgSymbol type
    // \note the second parameter is a tag to guarantee the type requirement.
    template <class SageSymbol>
    auto handlesyms(const SageSymbol& n, const SgSymbol&) -> decltype(n.get_declaration())
    {
      return n.get_declaration();
    }

    //
    // special cases
    auto handlesyms(const SgSymbol& n, const SgSymbol&) -> const SgNode*
    {
      std::stringstream out;

      out << "unhandled symbol kind: " << typeid(n).name() << std::endl;
      throw std::runtime_error{out.str()};

      // return will be filtered out
      return &n;
    }

    template <class SageNode>
    void handle(const SageNode& n)
    {
      res = filterReturnType(handlesyms(n, n));
    }
  };
}

SgScopeStatement*
logicalParentScope(const SgScopeStatement& curr)
{
  return sg::dispatch(LogicalParent{}, &curr);
}

SgScopeStatement*
logicalParentScope(const SgScopeStatement* curr)
{
  return curr ? logicalParentScope(*curr) : nullptr;
}

std::tuple<const SgScopeStatement*, const SgSymbol*>
findSymbolInContext(std::string id, const SgScopeStatement& scope, const SgScopeStatement* limit)
{
  constexpr SgTemplateParameterPtrList* templParams = nullptr;
  constexpr SgTemplateArgumentPtrList*  templArgs   = nullptr;

  const SgScopeStatement* curr = &scope;
  const SgSymbol*         sym  = nullptr;

  while ((curr != nullptr) && (curr != limit))
  {
    sym = curr->lookup_symbol(id, templParams, templArgs);
    if (sym) break;

    curr = logicalParentScope(*curr);
  }

  return {curr, sym};
}

SgDeclarationStatement* associatedDeclaration(const SgSymbol& n)
{
  return sg::dispatch(AssociatedDecl{}, &n);
}

bool isAttribute(const SgAdaAttributeExp& attr, const std::string& attrname)
{
  return boost::iequals(attr.get_attribute().getString(), attrname);
}



} // ada
} // SageInterface

namespace sg
{
  [[noreturn]]
  void report_error(std::string desc, const char* file, size_t ln)
  {
    static const char* AT = " at ";
    static const char* SEP = " : ";

    if (file)
    {
      const std::string filename(file);
      const std::string num(conv<std::string>(ln));

      desc.reserve(desc.size() + num.size() + filename.size() + std::strlen(AT) + std::strlen(SEP)+1);

      desc.append(AT);
      desc.append(filename);
      desc.append(SEP);
      desc.append(num);
    }

    {
      using namespace Rose::Diagnostics;

      //~ mlog[FATAL] << "[abort] " << desc << std::endl;
      //~ ROSE_ABORT();

      mlog[FATAL] << "[throw] " << desc << std::endl;
      throw std::runtime_error(desc);

    //~ std::cerr << "[exit] [FATAL] " << desc << std::endl;
    //~ std::exit(1);
    }
  }

  [[noreturn]]
  void unexpected_node(const SgNode& n, const char* file, size_t ln)
  {
    static const std::string msg = "assertion failed: unexpected node-type: ";

    report_error(msg + typeid(n).name(), file, ln);
  }
}
