
#include "sageInterfaceAda.h"
#include "sageInterface.h"
#include "sageGeneric.h"
#include "sageBuilder.h"

#include <iostream>
#include <limits>
#include <cmath>
#include <exception>

#include <boost/lexical_cast.hpp>

#include "Rose/Diagnostics.h"

namespace si = SageInterface;
namespace sb = SageBuilder;

namespace
{
  ///
  enum TypeSkip
  {
    skipNone                 = 0,
    skipAdaSubtype           = (1 << 0),
    skipAdaDerivedType       = (1 << 1),
    skipTypedefType          = (1 << 2),
    skipModifierType         = (1 << 3),
    skipPointerType          = (1 << 4),
    skipReferenceType        = (1 << 5),  /* C++ */
    skipRvalueReferenceType  = (1 << 6),  /* C++ */
    skipAllReferenceTypes    = (skipReferenceType | skipRvalueReferenceType),  /* C++ */
    skipAdaAccessType        = (1 << 7),
    //~ skipUsingDecls           = (1 << 8),  /* C++ */
    //~ skipAdaUseTypes      = (1 << 9),  /* Ada */
    skipLast                 = (1 << 30)
  };


  struct ArrayType : sg::DispatchHandler<SgArrayType*>
  {
      static
      SgArrayType* find(SgType* n);
      //~ find(SgType* n, TypeSkip skipWhat = skipNone);

      ReturnType recurse(SgType* n);

      // invalid case
      void handle(SgNode& n)                { SG_UNEXPECTED_NODE(n); }

      // base cases
      void handle(SgType&)                  { res = nullptr; }
      void handle(SgArrayType& n)           { res = &n; }

      // possibly skipped types
      void handle(SgAdaSubtype& n)          { res = recurse(n.get_base_type()); }
      void handle(SgAdaDerivedType& n)      { res = recurse(n.get_base_type()); }
      void handle(SgTypedefType& n)         { res = recurse(n.get_base_type()); }
      void handle(SgModifierType& n)        { res = recurse(n.get_base_type()); }
      //~ void handle(SgPointerType& n)         { res = recurse(n.get_base_type()); }
      //~ void handle(SgReferenceType& n)       { res = recurse(n.get_base_type()); }
      //~ void handle(SgRvalueReferenceType& n) { res = recurse(n.get_base_type()); }
      //~ void handle(SgAdaAccessType& n)       { res = recurse(n.get_base_type()); }
  };

  SgArrayType*
  ArrayType::find(SgType* n)
  {
    SgArrayType* res = sg::dispatch(ArrayType{}, n);

    return res;
  }

  ArrayType::ReturnType
  ArrayType::recurse(SgType* n)
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

      //
      void handle(SgType&)                  { /* do nothing to return the type-expression */ }

      // base cases for expressions
      //~ void handle(SgExpression&)            { res = nullptr; }
      void handle(SgRangeExp& n)            { res = &n; }
      void handle(SgAdaAttributeExp& n)     { res = &n; }

      // switch from expression to types
      void handle(SgTypeExpression& n)
      {
        // try to extract the range form the type
        DimRange::ReturnType sub = descend(n.get_type());

        // if that did not work, return the type expression
        res = sub ? sub : &n;
      }

      // base case for types
      //~ void handle(SgType& n)                { res = nullptr; }

      // type expressions
      void handle(SgTypedefType& n)         { res = descend(n.get_base_type()); }

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
      recurse(SgType* n);

      // invalid case
      void handle(SgNode& n)                { SG_UNEXPECTED_NODE(n); }

      // base cases
      //~ void handle(SgType&)                  { /* do nothing */; }

      // skipped types
      void handle(SgAdaDerivedType& n)      { res = recurse(n.get_base_type()); }
      void handle(SgTypedefType& n)         { res = recurse(n.get_base_type()); }
      void handle(SgModifierType& n)        { res = recurse(n.get_base_type()); }

      // subtype -> get the dimension info for each
      void handle(SgAdaSubtype& n)
      {
        SgAdaTypeConstraint* constraint = n.get_constraint();

        // if the subtype has a null constraint, then the real array types
        //   must be located underneath.
        if (isSgAdaNullConstraint(constraint))
        {
          res = recurse(n.get_base_type());
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
  ArrayBounds::recurse(SgType* n)
  {
    return sg::dispatch(ArrayBounds(), n);
  }

  struct IntegralValue : sg::DispatchHandler<long long int>
  {
    void handle(const SgNode& n)     { SG_UNEXPECTED_NODE(n); }

    void handle(SgExpression& n)
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
      typedef sg::DispatchHandler<SgRangeExp*> base;

      explicit
      RangeExp(size_t whichDimension)
      : base(), dim(whichDimension)
      {}

      static
      ReturnType
      find(SgNode* n, size_t dim);

      ReturnType
      recurse(SgNode* n);

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
      void handle(SgExpression& n)         { res = recurse(n.get_type()); }

      void handle(SgAdaAttributeExp& n)    { res = ::si::ada::range(n); }

      void handle(SgAdaRangeConstraint& n) { res = recurse(n.get_range()); }

      void handle(SgAdaIndexConstraint& n)
      {
        res = find(n.get_indexRanges().at(dimension()), 1);
      }

      void handle(SgAdaSubtype& n)
      {
        SgNode* constraint = n.get_constraint();

        res = recurse(constraint ? constraint : n.get_base_type());
      }

      void handle(SgArrayType& n)
      {
        if (::si::ada::unconstrained(n))
          return notFound();

        SgExprListExp& exprlst = SG_DEREF(n.get_dim_info());

        res = find(exprlst.get_expressions().at(dimension()), 1);
      }

      void handle(SgAdaDerivedType& n)     { res = recurse(n.get_base_type()); }
      void handle(SgTypedefType& n)        { res = recurse(n.get_base_type()); }

    private:
      size_t dim;
  };

  RangeExp::ReturnType
  RangeExp::find(SgNode* n, size_t dim)
  {
    return sg::dispatch(RangeExp(dim), n);
  }

  RangeExp::ReturnType
  RangeExp::recurse(SgNode* n)
  {
    return find(n, dim);
  }

  struct BaseTypeDecl : sg::DispatchHandler<SgDeclarationStatement*>
  {
      static
      ReturnType find(SgType* n);

      ReturnType
      recurse(SgType* n);

      void handle(SgNode& n) { SG_UNEXPECTED_NODE(n); }
      void handle(SgType& n) { /* not found */ }

      void handle(SgNamedType& n)           { res = n.get_declaration(); }

      void handle(SgAdaSubtype& n)          { res = recurse(n.get_base_type()); }
      void handle(SgModifierType& n)        { res = recurse(n.get_base_type()); }
      void handle(SgAdaDerivedType& n)      { res = recurse(n.get_base_type()); }
  };


  BaseTypeDecl::ReturnType
  BaseTypeDecl::find(SgType* n)
  {
    return sg::dispatch(BaseTypeDecl{}, n);
  }

  BaseTypeDecl::ReturnType
  BaseTypeDecl::recurse(SgType* n)
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

  struct BaseScope : sg::DispatchHandler<const SgScopeStatement*>
  {
    static
    ReturnType find(const SgScopeStatement* n);

    void handle(const SgNode& n)           { SG_UNEXPECTED_NODE(n); }
    void handle(const SgScopeStatement& n) { res = &n; }
    void handle(const SgAdaPackageBody& n) { res = n.get_spec(); }
  };

  BaseScope::ReturnType
  BaseScope::find(const SgScopeStatement* n)
  {
    return sg::dispatch(BaseScope{}, n);
  }

  bool isSameScope(const SgScopeStatement* lhs, const SgScopeStatement* rhs)
  {
    ROSE_ASSERT(lhs && rhs);

    return (lhs == rhs) || (BaseScope::find(lhs) == BaseScope::find(rhs));
  }

  bool isPrivate(const SgDeclarationStatement& dcl)
  {
    return dcl.get_declarationModifier().get_accessModifier().isPrivate();
  }
}

namespace SageInterface
{
namespace ada
{
  const std::string roseOperatorPrefix  = "operator";
  const std::string packageStandardName = "Standard";
  const std::string durationTypeName    = "Duration";
  const std::string exceptionName       = "Exception";

  long long int
  staticIntegralValue(SgExpression* n)
  {
    return sg::dispatch(IntegralValue(), n);
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

  std::pair<SgArrayType*, std::vector<SgExpression*> >
  getArrayTypeInfo(SgType* atype)
  {
    SgArrayType* restype = ArrayType::find(atype);

    return std::make_pair(restype, ArrayBounds::find(atype, restype));
  }

  std::vector<IfExpressionInfo>
  flattenIfExpressions(SgConditionalExp& n)
  {
    std::vector<IfExpressionInfo> res;
    SgConditionalExp*             next = &n;
    SgConditionalExp*             cond = nullptr;
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
  flattenIfStatements(SgIfStmt& n)
  {
    std::vector<IfStatementInfo> res;
    SgIfStmt*                    next = &n;
    SgIfStmt*                    cond = nullptr;

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
    if (boost::to_upper_copy(n.get_attribute().getString()) != "RANGE")
      return nullptr;

    const int dim = si::ada::firstLastDimension(SG_DEREF(n.get_args()));

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
    return unconstrained(SG_DEREF(ty));
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

  SgAdaPackageBodyDecl*
  getBodyDeclaration(const SgAdaPackageSpecDecl* specDecl)
  {
    if (!specDecl) return nullptr;

    SgAdaPackageSpec*     spec = specDecl->get_definition();
    if (!spec) return nullptr;

    SgAdaPackageBody*     body = spec->get_body();
    if (!body) return nullptr;

    return isSgAdaPackageBodyDecl(body->get_parent());
  }

  SgAdaPackageBodyDecl&
  getBodyDeclaration(const SgAdaPackageSpecDecl& specDecl)
  {
    SgAdaPackageSpec&     spec = SG_DEREF(specDecl.get_definition());
    SgAdaPackageBody&     body = SG_DEREF(spec.get_body());
    SgAdaPackageBodyDecl* bodyDecl = isSgAdaPackageBodyDecl(body.get_parent());

    return SG_DEREF(bodyDecl);
  }


  namespace
  {
    bool isNormalStatement(const SgStatement* s)
    {
      return isSgDeclarationStatement(s) == nullptr;
    }
  }

  SgStatementPtrList::const_iterator
  declarationLimit(const SgStatementPtrList& list)
  {
    return std::find_if(list.begin(), list.end(), isNormalStatement);
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
    return std::find_if(list.begin(), list.end(), isNormalStatement);
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

  bool isFunctionTryBlock(const SgTryStmt& n)
  {
    SgBasicBlock*                blk = isSgBasicBlock(n.get_parent());
    if (blk == nullptr) return false;

    SgFunctionDefinition*        def = isSgFunctionDefinition(blk->get_parent());
    if (def == nullptr) return false;

    SgStatementPtrList&          stmts = blk->get_statements();
    SgStatementPtrList::iterator dcllimit = declarationLimit(stmts);

    // return true iff n is the only stmt within the block
    return std::distance(dcllimit, stmts.end()) == 1;
  }

  bool isFunctionTryBlock(const SgTryStmt* n)
  {
    return n && isFunctionTryBlock(*n);
  }

  bool isPackageTryBlock(const SgTryStmt& n)
  {
    return isSgAdaPackageBody(n.get_parent());
  }


  bool isPackageTryBlock(const SgTryStmt* n)
  {
    return n && isFunctionTryBlock(*n);
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
    return getRenamedGenericDecl(n.get_declaration());
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

  bool hasUnknownDiscriminants(const SgAdaDiscriminatedTypeDecl& n)
  {
    return n.get_discriminants().size() == 0;
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

  namespace
  {
    bool definedInStandard(const SgDeclarationStatement& n)
    {
      SgAdaPackageSpec*      pkgspec = isSgAdaPackageSpec(n.get_scope());
      if (pkgspec == nullptr) return false;

      SgAdaPackageSpecDecl*  pkgdecl = isSgAdaPackageSpecDecl(pkgspec->get_parent());
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
      if (ty == nullptr || (ty->get_name() == exceptionName))
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



  //
  // for variants

  VariantInfo
  variantInfo(const SgAdaVariantFieldDecl* n)
  {
    if (!n) return std::make_tuple(nullptr, 0);

    SgExprListExp* cond = n->get_variantConditions();

    return std::make_tuple(cond, cond->get_expressions().size());
  }

  namespace
  {
    VariantEntry getVariant_internal(const SgExpression& n)
    {
      //~ std::cerr << typeid(n).name() << std::endl;

      const SgIsOp& isop = SG_DEREF(isSgIsOp(&n));

      return VariantEntry{ isSgVarRefExp(isop.get_lhs_operand()),
                           isSgExprListExp(isop.get_rhs_operand())
                         };
    }

    const SgExprListExp& getCondition(const SgExpression& n)
    {
      const SgIsOp& isop = SG_DEREF(isSgIsOp(&n));

      return SG_DEREF(isSgExprListExp(isop.get_rhs_operand()));
    }

    const SgVarRefExp& getControl(const SgExpression& n)
    {
      const SgIsOp& isop = SG_DEREF(isSgIsOp(&n));

      return SG_DEREF(isSgVarRefExp(isop.get_lhs_operand()));
    }

    bool equalVariantExpr(const SgVarRefExp& lhs, const SgVarRefExp& rhs)
    {
      return lhs.get_symbol() == rhs.get_symbol();
    }

    bool equalVariantExpr(const SgVarRefExp* lhs, const SgVarRefExp* rhs)
    {
      return equalVariantExpr(SG_DEREF(lhs), SG_DEREF(rhs));
    }

    bool equalVariantExpr(const SgFunctionRefExp& lhs, const SgFunctionRefExp& rhs)
    {
      return lhs.get_symbol() == rhs.get_symbol();
    }
/*
    bool equalVariantExpr(const SgFunctionRefExp* lhs, const SgFunctionRefExp* rhs)
    {
      return equalVariantExpr(SG_DEREF(lhs), SG_DEREF(rhs));
    }
*/


    bool haveSameControl(const SgExpression* lhs, const SgExpression* rhs)
    {
      return equalVariantExpr(getControl(SG_DEREF(lhs)), getControl(SG_DEREF(rhs)));
    }
  };

  VariantEntry getVariant(const VariantInfo& info, int i)
  {
    const SgExpressionPtrList& allvariants = SG_DEREF(info.variants()).get_expressions();

    return getVariant_internal(SG_DEREF(allvariants.at(i)));
  }

  int getSharedControlDepth(const VariantInfo& prev, const VariantInfo& next)
  {
    using Iterator = SgExpressionPtrList::const_iterator;

    if (prev.depth() == 0) return 0;
    if (next.depth() == 0) return 0;

    const SgExpressionPtrList& pe = SG_DEREF(prev.variants()).get_expressions();
    const SgExpressionPtrList& ne = SG_DEREF(next.variants()).get_expressions();
    const size_t               minlen = std::min(pe.size(), ne.size());
    Iterator                   peaa = pe.begin();
    Iterator                   pezz = peaa + minlen;
    Iterator                   pepos = std::mismatch(peaa, pezz, ne.begin(), haveSameControl).first;

    return std::distance(peaa, pepos);
  }

  namespace
  {
    bool equalVariantElement(const SgExpression* lhs, const SgExpression* rhs);

    struct VariantConditionAreEqual : sg::DispatchHandler<bool>
    {
        using base = sg::DispatchHandler<bool>;

        explicit
        VariantConditionAreEqual(const SgExpression& expr)
        : n2(expr)
        {}

        template <class SageExpression>
        bool equalRef(const SageExpression& lhs, const SgExpression& rhs)
        {
          ROSE_ASSERT(lhs.variantT() == rhs.variantT());

          return equalVariantExpr(lhs, static_cast<const SageExpression&>(rhs));
        }

        template <class SageExpression>
        bool equalVal(const SageExpression& lhs, const SgExpression& rhs)
        {
          ROSE_ASSERT(lhs.variantT() == rhs.variantT());

          return lhs.get_value() == static_cast<const SageExpression&>(rhs).get_value();
        }

        template <class SageExpression>
        bool equalName(const SageExpression& lhs, const SageExpression& rhs)
        {
          return lhs.get_name() == rhs.get_name();
        }

        template <class SageExpression, class FnResult_SageExpression>
        bool equalChild( const SageExpression& lhs,
                         const SgExpression& rhs,
                         FnResult_SageExpression* (SageExpression::*getter)() const
                       )
        {
          ROSE_ASSERT(lhs.variantT() == rhs.variantT());

          const SgExpression* lhs_child = (lhs.*getter)();
          const SgExpression* rhs_child = (static_cast<const SageExpression&>(rhs).*getter)();

          return equalVariantElement(lhs_child, rhs_child);
        }

        [[noreturn]]
        void err(const SgNode& n) { SG_UNEXPECTED_NODE(n); }

        [[noreturn]]
        bool eval(const SgNode& n, const SgNode&, const SgNode&)
        {
          err(n);
          //~ return true;
        }

        bool eval(const SgAdaOthersExp&, const SgAdaOthersExp&, const SgAdaOthersExp&)
        {
          return true;
        }

        //
        // SgValueExp

        bool eval(const SgEnumVal& l, const SgEnumVal& r, const SgEnumVal&)
        {
          return equalName(l, r);
        }

        [[noreturn]]
        bool eval(const SgValueExp& l, const SgValueExp&, const SgValueExp&) { err(l); }

        [[noreturn]]
        bool eval(const SgJovialBitVal& l, const SgJovialBitVal&, const SgJovialBitVal&) { err(l); }

        [[noreturn]]
        bool eval(const SgTemplateParameterVal& l, const SgTemplateParameterVal&, const SgTemplateParameterVal&) { err(l); }

        bool eval(const SgVoidVal&, const SgVoidVal&, const SgVoidVal&)                   { return true; }
        bool eval(const SgNullptrValExp&, const SgNullptrValExp&, const SgNullptrValExp&) { return true; }

        bool eval(const SgComplexVal& l, const SgComplexVal& r, const SgComplexVal&)
        {
          return (  equalChild(l, r, &SgComplexVal::get_real_value)
                 && equalChild(l, r, &SgComplexVal::get_imaginary_value)
                 );
        }

        bool eval(const SgAdaFloatVal& l, const SgAdaFloatVal& r, const SgAdaFloatVal&)
        {
          return l.get_valueString() == r.get_valueString();
        }

        bool eval(const SgAdaAttributeExp& l, const SgAdaAttributeExp& r, const SgAdaAttributeExp&)
        {
          return (  (l.get_attribute() == r.get_attribute())
                 && equalChild(l, r, &SgAdaAttributeExp::get_object)
                 && equalChild(l, r, &SgAdaAttributeExp::get_args)
                 );
        }


        template <class SageValueExp>
        bool eval(const SageValueExp& l, const SageValueExp& r, const SgValueExp&)
        {
          return l.get_value() == r.get_value();
        }

        //
        // unary

        bool eval(const SgUnaryOp& l, const SgUnaryOp& r, const SgUnaryOp&)
        {
          return equalChild(l, r, &SgUnaryOp::get_operand);
        }

        //
        // binary

        bool eval(const SgBinaryOp& l, const SgBinaryOp& r, const SgBinaryOp&)
        {
          return (  equalChild(l, r, &SgBinaryOp::get_lhs_operand)
                 && equalChild(l, r, &SgBinaryOp::get_rhs_operand)
                 );
        }

        bool eval(const SgBinaryOp& l, const SgBinaryOp& r, const SgDotExp&)
        {
          return equalChild(l, r, &SgBinaryOp::get_rhs_operand);
        }

        //
        // calls
        bool eval(const SgCallExpression& l, const SgCallExpression& r, const SgCallExpression&)
        {
          return (  equalChild(l, r, &SgCallExpression::get_function)
                 && equalChild(l, r, &SgCallExpression::get_args)
                 );
        }


        //
        // special

        bool eval(const SgVarRefExp& l, const SgVarRefExp& r, const SgVarRefExp&)
        {
          return equalRef(l, r);
        }

        bool eval(const SgFunctionRefExp& l, const SgFunctionRefExp& r, const SgFunctionRefExp&)
        {
          return equalRef(l, r);
        }

        bool eval(const SgRangeExp& l, const SgRangeExp& r, const SgRangeExp&)
        {
          return (  equalChild(l, r, &SgRangeExp::get_start)
                 && equalChild(l, r, &SgRangeExp::get_end)
                 && equalChild(l, r, &SgRangeExp::get_stride) // not used in Ada
                 );
        }

        bool eval(const SgTypeExpression& l, const SgTypeExpression& r, const SgTypeExpression&)
        {
          // \todo this is true if the nodes are unified, not sure if this works pervasively
          return l.get_type() == r.get_type();
        }

        bool eval(const SgExprListExp& l, const SgExprListExp& r, const SgExprListExp&)
        {
          const SgExpressionPtrList& llst = l.get_expressions();
          const SgExpressionPtrList& rlst = r.get_expressions();

          if (llst.size() != rlst.size())
            return false;

          SgExpressionPtrList::const_iterator eol = llst.end();

          return eol == std::mismatch(llst.begin(), eol, rlst.begin(), equalVariantElement).first;
        }

        void handle(const SgNode& n, const SgNode&) {}

        template <class SageExpression>
        void handle(const SageExpression& n, const SgExpression&)
        {
          ROSE_ASSERT(n.variantT() == n2.variantT());

          res = eval(n, static_cast<const SageExpression&>(n2), n);
        }

        template <class SageNode>
        void handle(const SageNode& n)
        {
          handle(n, n);
        }

      private:
        const SgExpression& n2;
    };

    bool equalVariantElement(const SgExpression* lhs, const SgExpression* rhs)
    {
      if (lhs == nullptr)
        return rhs == nullptr;

      if (rhs == nullptr)
        return false;

      if (lhs->variantT() != rhs->variantT())
        return false;

      return sg::dispatch(VariantConditionAreEqual{SG_DEREF(lhs)}, rhs);
    }

    bool equalVariantExpr(const SgExprListExp& lhs, const SgExprListExp& rhs)
    {
      const SgExpressionPtrList& lhslst = lhs.get_expressions();
      const SgExpressionPtrList& rhslst = rhs.get_expressions();

      return (  ( lhslst.size() == rhslst.size() )
             && ( lhslst.end() == std::mismatch( lhslst.begin(), lhslst.end(),
                                                 rhslst.begin(),
                                                 equalVariantElement
                                               ).first
                )
             );
    }

    bool equalVariantExpr(const SgExprListExp* lhs, const SgExprListExp* rhs)
    {
      return equalVariantExpr(SG_DEREF(lhs), SG_DEREF(rhs));
    }

    bool variantsHaveSameCondition(const SgExpression& lhs, const SgExpression& rhs)
    {
      return equalVariantExpr(getCondition(lhs), getCondition(rhs));
    }
  }

  /// test if \ref prev and \rev next have the same variant condition at position \ref i
  bool haveSameConditionAt(const VariantInfo& prev, const VariantInfo& next, int i)
  {
    ROSE_ASSERT((i < prev.depth()) && (i < next.depth()));

    const SgExpressionPtrList& pe = SG_DEREF(prev.variants()).get_expressions();
    const SgExpressionPtrList& ne = SG_DEREF(next.variants()).get_expressions();

    return variantsHaveSameCondition(SG_DEREF(pe.at(i)), SG_DEREF(ne.at(i)));
  }

  namespace
  {
    bool haveSameVariants(const SgExpression* lhs, const SgExpression* rhs)
    {
      VariantEntry lhsVariant = getVariant_internal(SG_DEREF(lhs));
      VariantEntry rhsVariant = getVariant_internal(SG_DEREF(rhs));

      return (  equalVariantExpr(lhsVariant.control(), rhsVariant.control())
             && equalVariantExpr(lhsVariant.conditions(), rhsVariant.conditions())
             );
    }


    struct ConditionChange
    {
      bool operator()(SgStatement* dcl) const
      {
        using Iterator = SgExpressionPtrList::const_iterator;

        const SgAdaVariantFieldDecl* nextVariant = isSgAdaVariantFieldDecl(dcl);
        VariantInfo                  next = variantInfo(nextVariant);

        if (next.depth() != prev.depth())
          return true;

        if (next.depth() == 0)
          return false; // neither statement has a variant condition

        const SgExpressionPtrList& pe = SG_DEREF(prev.variants()).get_expressions();
        const SgExpressionPtrList& ne = SG_DEREF(next.variants()).get_expressions();
        const Iterator             peaa = pe.begin();

        ROSE_ASSERT(pe.size() == ne.size());

        return peaa == std::mismatch(peaa, pe.end(), ne.begin(), haveSameVariants).first;
      }

      VariantInfo prev;
    };

  }

  SgDeclarationStatementPtrList::const_iterator
  findVariantConditionChange( SgDeclarationStatementPtrList::const_iterator begin,
                              SgDeclarationStatementPtrList::const_iterator end,
                              const SgAdaVariantFieldDecl* prevVariant
                            )
  {
    return std::find_if(begin, end, ConditionChange{variantInfo(prevVariant)});
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
    // root types as implemented by AdaMaker.C
    SgType* integralType() { return sb::buildLongLongType(); }
    SgType* realType()     { return sb::buildLongDoubleType(); }
    SgType* fixedType()    { return sb::buildFixedType(nullptr, nullptr); }
    SgType* pointerType()  { return sb::buildNullptrType(); }

    SgType* arrayType(SgType* base)
    {
      // poor man's type unifier
      static std::map<SgType*, SgArrayType*> m;

      SgArrayType*& res = m[base];

      if (res == nullptr) res = sb::buildArrayType(base);

      return res;
    };

    struct RootTypeFinder : sg::DispatchHandler<SgType*>
    {
      void handle(SgNode& n)              { SG_UNEXPECTED_NODE(n); }

      //~ void handle(SgType& n) { res = &n; }

      // all root types (according to the three builder function in AdaMaker.C)
      void handle(SgTypeLongLong& n)      { res = integralType(); }
      void handle(SgTypeLongDouble& n)    { res = realType(); }
      void handle(SgTypeFixed& n)         { res = fixedType(); }

      // plus discrete type indicator for Ada generics
      void handle(SgAdaDiscreteType& n)   { res = &n; }

      // modular type: handle like int?
      void handle(SgAdaModularType& n)    { res = integralType(); }

      // are subroutines their own root type?
      void handle(SgAdaSubroutineType& n) { res = &n; }

      // plus types used by AdaMaker but that do not have a direct correspondence
      //   in the Ada Standard.
      void handle(SgTypeVoid& n)          { res = &n; }
      void handle(SgTypeUnknown& n)       { res = &n; }
      void handle(SgAutoType& n)          { res = &n; }

      void handle(SgAdaFormalType& n)
      {
        // \todo what else?
        res = &n;
      }

      void handle(SgTypeDefault& n)
      {
        // this is a type used by a opaque declaration.
        // \todo maybe needs replacement with the actual type.
        res = &n;
      }

      // the package standard uses an enumeration to define boolean, so include the
      //   ROSE bool type also.
      // \todo reconsider
      void handle(SgTypeBool& n)          { res = &n; }

      // plus: map all other fundamental types introduced by initializeStandardPackage in AdaType.C
      //       onto the root types defined by AdaMaker.C
      // \todo eventually all types in initializeStandardPackage should be rooted in
      //       the root types as defined by AdaMaker.C.
      void handle(SgTypeInt&)             { res = integralType(); }
      void handle(SgTypeLong&)            { res = integralType(); }
      void handle(SgTypeShort&)           { res = integralType(); }

      void handle(SgTypeFloat&)           { res = realType(); }
      void handle(SgTypeDouble&)          { res = realType(); }

      void handle(SgTypeChar& n)          { res = &n; }
      void handle(SgTypeChar16& n)        { res = &n; }
      void handle(SgTypeChar32& n)        { res = &n; }

      // true fundamental types
      void handle(SgClassType& n)         { res = &n; }
      void handle(SgEnumType& n)          { res = &n; /* \todo check if this is a derived enum */ }

      // an array is fundamental - its underlying type may not be, so it may can be discovered if needed
      void handle(SgArrayType& n)
      {
        res = arrayType(&find(n.get_base_type()));
      }

      void handle(SgTypeString& n)
      {
        // not sure why this can be reached ..
        // since we have no info about the character type, just return the standard string type
        res = arrayType(sb::buildCharType());
      }


      // pointer types
      void handle(SgPointerType& n)       { res = pointerType(); } // \todo should not be in Ada
      void handle(SgAdaAccessType& n)     { res = pointerType(); }
      void handle(SgTypeNullptr& n)       { res = pointerType(); }

      // \todo add string types as introduced by initializeStandardPackage in AdaType.C
      // \todo add other fundamental types as introduced by initializeStandardPackage in AdaType.C

      // all type indirections that do not define fundamental types
      void handle(SgTypedefType& n)       { res = &find(n.get_base_type()); }
      void handle(SgModifierType& n)      { res = &find(n.get_base_type()); }
      void handle(SgAdaSubtype& n)        { res = &find(n.get_base_type()); }
      void handle(SgAdaDerivedType& n)    { res = &find(n.get_base_type()); }

      void handle(SgAdaDiscriminatedType& n)
      {
        // \todo not sure..
        res = &n;
      }

      void handle(SgDeclType& n)
      {
        // \todo not sure..
        res = typeRoot(n.get_base_expression());
      }


      // expressions

      // by default use the expression's type
      void handle(SgExpression& n)        { res = &find(n.get_type()); }


      // by default use the expression's type
      void handle(SgVarRefExp& n)
      {
        res = &find(n.get_type());

        if (isSgAutoType(res))
        {
          // if this is an auto constant, check the initializer
          SgVariableSymbol&  sy  = SG_DEREF(n.get_symbol());
          SgInitializedName& var = SG_DEREF(sy.get_declaration());

          if (SgAssignInitializer* init = isSgAssignInitializer(var.get_initializer()))
            res = &find(SG_DEREF(init->get_operand()).get_type());
        }
      }


      // SgTypeString does not preserve the 'wideness', so let's just get
      //   this info from the literal.
      void handle(SgStringVal& n)
      {
        SgType* charType = nullptr;

        if (n.get_is16bitString())      charType = sb::buildChar16Type();
        else if (n.get_is32bitString()) charType = sb::buildChar32Type();
        else                            charType = sb::buildCharType();
        ASSERT_not_null(charType);

        res = arrayType(charType);
      }


      static
      SgType& find(SgType* ty);
    };

    SgType&
    RootTypeFinder::find(SgType* ty)
    {
      SgType* res = sg::dispatch(RootTypeFinder{}, ty);

      return SG_DEREF(res);
    }
  };

  SgType* typeRoot(SgType& ty)
  {
    return &RootTypeFinder::find(&ty);
  }

  SgType* typeRoot(SgType* ty)
  {
    return ty ? typeRoot(*ty) : nullptr;
  }

  SgType* typeRoot(SgExpression& exp)
  {
    return sg::dispatch(RootTypeFinder{}, &exp);
  }

  SgType* typeRoot(SgExpression* exp)
  {
    return exp ? typeRoot(*exp) : nullptr;
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
    if (name.rfind(si::ada::roseOperatorPrefix, 0) != 0)
      return "";

    const std::string op = name.substr(si::ada::roseOperatorPrefix.size());

    if (!isOperatorName(op))
      return "";

    return op;
  }

  std::string convertRoseOperatorNameToAdaName(const std::string& name)
  {
    static const std::string quotes    = "\"";

    if (name.rfind(si::ada::roseOperatorPrefix, 0) != 0)
      return name;

    const std::string op = name.substr(si::ada::roseOperatorPrefix.size());

    if (!isOperatorName(op))
      return name;

    return quotes + op + quotes;
  }



  // ******
  // \todo move code below to Ada to C++ translator
  // ******

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
    auto declaredInMainFile = [&mainFile](const SgDeclarationStatement* dcl)
                              {
                                ROSE_ASSERT(dcl);

                                const Sg_File_Info& fileInfo = SG_DEREF(dcl->get_startOfConstruct());

                                return fileInfo.get_filenameString() == mainFile;
                              };
    auto notDeclaredInMainFile = [&declaredInMainFile](const SgDeclarationStatement* dcl)
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
      FunctionCallToOperatorConverter(bool convPrefixCalls, bool convNamedArgs)
      : withPrefixCalls(convPrefixCalls), resolveNamedArguments(convNamedArgs)
      {}

      ~FunctionCallToOperatorConverter() { executeTransformations(); }

      void executeTransformations() const;

      void operator()(SgNode*);

    private:
      using replacement_t = std::tuple<SgFunctionCallExp*, std::string>;

      std::vector<replacement_t> work;
      bool                       withPrefixCalls;
      bool                       resolveNamedArguments;

      FunctionCallToOperatorConverter() = delete;
      //~ FunctionCallToOperatorConverter& operator=(const FunctionCallToOperatorConverter&) = delete;
      //~ FunctionCallToOperatorConverter& operator=(FunctionCallToOperatorConverter&&)      = delete;
  };

  int arity(const SgFunctionCallExp& fncall)
  {
    SgExprListExp* args = fncall.get_args();

    return SG_DEREF(args).get_expressions().size();
  }

  bool hasNullArg(const SgFunctionCallExp& fncall)
  {
    SgExprListExp* args = fncall.get_args();

    return isSgNullExpression(SG_DEREF(args).get_expressions().at(0));
  }

  void FunctionCallToOperatorConverter::operator()(SgNode* n)
  {
    SgFunctionCallExp*     fncall = isSgFunctionCallExp(n);
    if (  (fncall == nullptr)
       || (!fncall->get_uses_operator_syntax())
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

  using CallToOperatorTransformer = std::function<SgExpression*(SgFunctionCallExp*)>;

  template <class BinaryBuilderFn>
  CallToOperatorTransformer tf2(BinaryBuilderFn fn)
  {
    // in c++14: return [fn = std::move(fn)](SgFunctionCallExp* n) -> SgExpression*
    return [fn](SgFunctionCallExp* n) -> SgExpression*
           {
             SgExprListExp*       args = SG_DEREF(n).get_args();
             SgExpressionPtrList& list = SG_DEREF(args).get_expressions();

             ROSE_ASSERT(list.size() == 2);
             SgExpression*        lhs = list[0];
             SgExpression*        rhs = list[1];
             SgExpression*        lhs_dummy = sb::buildNullExpression();
             SgExpression*        rhs_dummy = sb::buildNullExpression();

             si::replaceExpression(lhs, lhs_dummy, true /* keep */);
             si::replaceExpression(rhs, rhs_dummy, true /* keep */);

             return fn(lhs, rhs);
           };
  }

  template <class UnaryBuilderFn>
  CallToOperatorTransformer tf1(UnaryBuilderFn fn)
  {
    // in c++14: return [fn = std::move(fn)](SgFunctionCallExp* n) -> SgExpression*
    return [fn](SgFunctionCallExp* n) -> SgExpression*
           {
             SgExprListExp*       args = SG_DEREF(n).get_args();
             SgExpressionPtrList& list = SG_DEREF(args).get_expressions();

             ROSE_ASSERT(list.size() == 1);
             SgExpression*        arg = list[0];
             SgExpression*        arg_dummy = sb::buildNullExpression();

             si::replaceExpression(arg, arg_dummy, true /* keep */);

             return fn(arg);
           };
  }


  void FunctionCallToOperatorConverter::executeTransformations() const
  {
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


    for (const replacement_t& r : work)
    {
      SgFunctionCallExp* orig  = std::get<0>(r);
      const int          numargs = arity(*orig);
      ROSE_ASSERT(numargs == 1 || numargs == 2);
      SgExpression*      repl  = numargs == 1 ? tfFn1.at(std::get<1>(r))(orig)
                                              : tfFn2.at(std::get<1>(r))(orig);

      ROSE_ASSERT(repl);
      repl->set_need_paren(orig->get_need_paren());

      if (orig->get_parent() == nullptr)
        std::cerr << "parent is null: " << orig->unparseToString() << std::endl;

      si::replaceExpression(orig, repl, false /* delete orig sub-tree */ );
    }
  }


  void conversionTraversal(std::function<void(SgNode*)>&& fn, SgNode* root)
  {
    ROSE_ASSERT(root);

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

  void convertToOperatorRepresentation(SgNode* root, bool withPrefixCalls, bool resolveNamedArguments)
  {
    conversionTraversal(FunctionCallToOperatorConverter{withPrefixCalls, resolveNamedArguments}, root);
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
    ROSE_ASSERT(parm);
    const SgDeclarationStatement* tydcl = BaseTypeDecl::find(parm->get_type());

    if (tydcl && isSameScope(tydcl->get_scope(), scope))
      res.emplace_back(parmpos, parm);

    ++parmpos;
  }

  return res;
}

std::vector<PrimitiveParameterDesc>
primitiveParameterPositions(const SgFunctionDeclaration* dcl)
{
  ROSE_ASSERT(dcl);

  return primitiveParameterPositions(*dcl);
}

size_t
positionalArgumentLimit(const SgExprListExp& args)
{
  using ConstIterator = SgExpressionPtrList::const_iterator;

  const SgExpressionPtrList& arglst = args.get_expressions();
  ConstIterator              aa     = arglst.begin();
  ConstIterator              pos    = std::find_if(aa, arglst.end(), isNamedArgument);

  return std::distance(aa, pos);
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

    if (const SgDeclarationStatement* tydcl = BaseTypeDecl::find(arg->get_type()))
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

    if (const SgDeclarationStatement* tydcl = BaseTypeDecl::find((*argpos)->get_type()))
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

SgDeclarationStatement*
baseDeclaration(SgType& ty)
{
  return baseDeclaration(&ty);
}

SgDeclarationStatement*
baseDeclaration(SgType* ty)
{
  return BaseTypeDecl::find(ty);
}

SgEnumDeclaration*
baseEnumDeclaration(SgType& ty)
{
  return baseEnumDeclaration(&ty);
}

SgEnumDeclaration*
baseEnumDeclaration(SgType* ty)
{
  SgDeclarationStatement* basedcl = baseDeclaration(ty);

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

      mlog[FATAL] << "[abort] " << desc << std::endl;
      ROSE_ABORT();

      //~ mlog[FATAL] << "[throw] " << desc << std::endl;
      //~ throw std::runtime_error(desc);

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
