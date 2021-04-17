
#include "sageInterfaceAda.h"
#include "sageInterface.h"
#include "sageGeneric.h"

#include <iostream>
#include <exception>

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
      void handle(SgType&)                  { res = NULL; }
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
    SgArrayType* res = sg::dispatch(ArrayType(), n);

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
    //~ void handle(SgExpression&)            { res = NULL; }
    void handle(SgRangeExp& n)            { res = &n; }
    void handle(SgAdaAttributeExp& n)     { res = &n; }

    // switch from expression to types
    void handle(SgTypeExpression& n)      { res = recurse(n.get_type()); }

    // base case for types
    //~ void handle(SgType& n)                { res = NULL; }

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
      return ArrayBounds::ReturnType();

    return sg::dispatch(ArrayBounds(), n);
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
  flattenArrayType(SgType* atype)
  {
    SgArrayType* restype = ArrayType::find(atype);

    return std::make_pair(restype, ArrayBounds::find(atype, restype));
  }

  SgRangeExp*
  range(const SgAdaAttributeExp& n)
  {
    if (boost::to_upper_copy(n.get_attribute().getString()) != "RANGE")
      return NULL;

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

  /// Traversal to change the comment style from Ada to C++
  struct CommentCxxifier : AstSimpleProcessing
  {
    explicit
    CommentCxxifier(bool useLineComments)
    : AstSimpleProcessing(),
      prefix(useLineComments ? "//" : "/*"),
      suffix(useLineComments ? ""   : "*/"),
      commentKind(useLineComments ? PreprocessingInfo::CplusplusStyleComment : PreprocessingInfo:: C_StyleComment)
    {}

    void visit(SgNode*) ROSE_OVERRIDE;

    private:
      //~ bool lineComments;
      const std::string                      prefix;
      const std::string                      suffix;
      const PreprocessingInfo::DirectiveType commentKind;

      CommentCxxifier()                                  = delete;
      CommentCxxifier(const CommentCxxifier&)            = delete;
      CommentCxxifier(CommentCxxifier&&)                 = delete;
      CommentCxxifier& operator=(CommentCxxifier&&)      = delete;
      CommentCxxifier& operator=(const CommentCxxifier&) = delete;
  };

  void CommentCxxifier::visit(SgNode* n)
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
      // \todo PreprocessingInfo does not allow to update the directive type
      //       possible solutions: either add new function or gen new object
      // ppinfo->setTypeOfDirective(commentKind);
    }
  }

  void convertAdaToCxxComments(SgNode* root, bool cxxLineComments)
  {
    ROSE_ASSERT(root);

    CommentCxxifier adaToCxxCommentConverter{cxxLineComments};

    adaToCxxCommentConverter.traverse(root, preorder);
  }

} // Ada
} // SageInterface

