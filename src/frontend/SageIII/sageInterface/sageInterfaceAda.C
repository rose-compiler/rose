
#include "sageInterfaceAda.h"
#include "sageInterface.h"
#include "sageGeneric.h"

#include <iostream>
#include <exception>

#include <boost/lexical_cast.hpp>

#include "Rose/Diagnostics.h"

namespace si = SageInterface;

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

    ReturnType recurse(SgNode* n);

    // invalid case
    void handle(SgNode& n)                { SG_UNEXPECTED_NODE(n); }

    // base cases for expressions
    //~ void handle(SgExpression&)            { res = nullptr; }
    void handle(SgRangeExp& n)            { res = &n; }
    void handle(SgAdaAttributeExp& n)     { res = &n; }

    // switch from expression to types
    void handle(SgTypeExpression& n)      { res = recurse(n.get_type()); }

    // base case for types
    //~ void handle(SgType& n)                { res = nullptr; }

    // type expressions
    void handle(SgTypedefType& n)         { res = recurse(n.get_base_type()); }

    void handle(SgAdaSubtype& n)
    {
      SgAdaRangeConstraint& range = SG_DEREF(isSgAdaRangeConstraint(n.get_constraint()));

      res = recurse(range.get_range());
    }
  };

  SgExpression&
  DimRange::find(SgNode* n)
  {
    SgExpression* res = sg::dispatch(DimRange(), n);

    return SG_DEREF(res);
  }

  DimRange::ReturnType
  DimRange::recurse(SgNode* n)
  {
    return &DimRange::find(n);
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
        // the first subtype must be an index constraint
        SgAdaIndexConstraint& idx = SG_DEREF(isSgAdaIndexConstraint(n.get_constraint()));
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
    void handle(SgNode& n)     { SG_UNEXPECTED_NODE(n); }

    void handle(SgExpression& n)
    {
      static const char* const msg = "sageInterface::ada: Expected constant integral value, got ";

      throw std::runtime_error(msg + n.unparseToString());
    }

    void handle(SgIntVal& n)                 { res = n.get_value(); }
    void handle(SgLongIntVal& n)             { res = n.get_value(); }
    void handle(SgLongLongIntVal& n)         { res = n.get_value(); }
    void handle(SgShortVal& n)               { res = n.get_value(); }
    void handle(SgUnsignedCharVal& n)        { res = n.get_value(); }
    void handle(SgUnsignedIntVal& n)         { res = n.get_value(); }
    void handle(SgUnsignedLongLongIntVal& n) { res = n.get_value(); }
    void handle(SgUnsignedLongVal& n)        { res = n.get_value(); }
    void handle(SgUnsignedShortVal& n)       { res = n.get_value(); }
  };

  size_t dimValue(SgExprListExp& args)
  {
    SgExpressionPtrList& exprlst = args.get_expressions();

    if (exprlst.size() == 0)
      return 1;

    ROSE_ASSERT(exprlst.size() == 1);
    return sg::dispatch(IntegralValue(), exprlst[0]);
  }

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
      void handle(SgAdaDerivedType& n)      { res = recurse(n.get_base_type()); }
      void handle(SgModifierType& n)        { res = recurse(n.get_base_type()); }
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
}

namespace SageInterface
{
namespace ada
{
  bool withPrivateDefinition(const SgDeclarationStatement& dcl)
  {
    // \todo check that dcl refers to a type
    const SgDeclarationStatement* def = dcl.get_definingDeclaration();

    return def && def->get_declarationModifier().get_accessModifier().isPrivate();
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

  SgRangeExp*
  range(const SgAdaAttributeExp& n)
  {
    if (boost::to_upper_copy(n.get_attribute().getString()) != "RANGE")
      return nullptr;

    const size_t dim = dimValue(SG_DEREF(n.get_args()));

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

    SgAdaPackageBody*     body = bodyDecl->get_definition();
    if (!body) return nullptr;

    SgAdaPackageSpec*     spec = body->get_spec();
    if (!spec) return nullptr;

    return isSgAdaPackageSpecDecl(spec->get_parent());
  }

  SgAdaPackageSpecDecl&
  getSpecificationDeclaration(const SgAdaPackageBodyDecl& bodyDecl)
  {
    SgAdaPackageBody&     body = SG_DEREF(bodyDecl.get_definition());
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
    return isFunctionTryBlock(SG_DEREF(n));
  }



  //
  // \todo move code below to Ada to C++ translator


  struct ConversionTraversal : AstSimpleProcessing
  {
      explicit
      ConversionTraversal(std::function<void(SgNode*)>&& conversionFn)
      : AstSimpleProcessing(), fn(std::move(conversionFn))
      {}

      void visit(SgNode*) ROSE_OVERRIDE;

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


  /// Traversal to change the comment style from Ada to C++
  struct CommentCxxifier
  {
      explicit
      CommentCxxifier(bool useLineComments)
      : prefix(useLineComments ? "//" : "/*"),
        suffix(useLineComments ? ""   : "*/"),
        commentKind(useLineComments ? PreprocessingInfo::CplusplusStyleComment : PreprocessingInfo:: C_StyleComment)
      {}

      CommentCxxifier& operator=(CommentCxxifier&&)      = default;
      CommentCxxifier(CommentCxxifier&&)                 = default;
      CommentCxxifier(const CommentCxxifier&)            = default;
      CommentCxxifier& operator=(const CommentCxxifier&) = default;

      void operator()(SgNode*) const;

    private:
      //~ bool lineComments;
      const std::string                      prefix;
      const std::string                      suffix;
      const PreprocessingInfo::DirectiveType commentKind;

      CommentCxxifier()                                  = delete;
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

  std::pair<size_t, bool>
  check(size_t s, size_t m)
  {
    return std::make_pair(s, s < m);
  }

  std::pair<size_t, bool>
  char2Val(char c, size_t max)
  {
    using ResultType = std::pair<size_t, bool>;

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
  parseDec(const char* buf, size_t base = 10)
  {
    ROSE_ASSERT((*buf != 0) && char2Val(*buf, base).second);

    T res = 0;

    while (*buf != 0)
    {
      const auto v = char2Val(*buf, base);

      if (!v.second)
        return std::make_pair(res, buf);

      res = res*base + v.first;

      ++buf;

      // skip underscores
      // \note (this is imprecise, since an underscore must be followed
      //       by an integer.
      while (*buf == '_') ++buf;
    }

    return std::make_pair(res, buf);
  }

  template <class T>
  std::pair<T, const char*>
  parseFrac(const char* buf, size_t base = 10)
  {
    ROSE_ASSERT((*buf != 0) && char2Val(*buf, base).second);

    T      res = 0;
    size_t divisor = 1*base;

    while ((*buf != 0) && (!isBasedDelimiter(*buf)))
    {
      const auto v = char2Val(*buf, base);

      ROSE_ASSERT(v.second);

      T val = v.first;

      res += val/divisor;
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
    return val * std::pow(base, exp);
  }


  long int
  basedLiteral(long int res, const char* cur, int base)
  {
    int exp = 0;

    ROSE_ASSERT(isBasedDelimiter(*cur));

    ++cur;
    base = res;

    std::tie(res, cur) = parseDec<long int>(cur, base);

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


int convertIntLiteral(const char* img)
{
  long int    res  = 0;
  int         base = 10;
  int         exp  = 0;
  const char* cur  = img;

  std::tie(res, cur) = parseDec<long int>(cur);

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

bool
explicitNullProcedure(const SgFunctionDefinition& fndef)
{
  const SgBasicBlock& body = SG_DEREF(fndef.get_body());

  return body.get_statements().empty();
}

bool
explicitNullRecord(const SgClassDefinition& recdef)
{
  return recdef.get_members().empty() && recdef.get_inheritances().empty();
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
