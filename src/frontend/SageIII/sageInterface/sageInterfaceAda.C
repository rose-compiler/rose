

#ifndef _SAGEINTERFACE_ADA_H
#define _SAGEINTERFACE_ADA_H 1


#include "sageInterfaceAda.h"
#include "sageInterface.h"
#include "sageGeneric.h"

#include <iostream>

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
      //~ typedef sg::DispatchHandler<SgArrayType*> base;

      //~ explicit
      //~ ArrayType(TypeSkip skipped)
      //~ : base(), skipWhat(skipped)
      //~ {}

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
  //~ ArrayType::find(SgType* n, TypeSkip skipWhat)
  {
    SgArrayType* res = sg::dispatch(ArrayType(), n);

    return res;
  }

  ArrayType::ReturnType
  ArrayType::recurse(SgType* n)
  {
    return ArrayType::find(n);
    //~ return ArrayType::find(n, skipWhat);
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

        for (SgExpression* expr : idxlst)
        {
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


#if 0
       * get_typeptr(): SgAdaSubtype
       ** get_typeptr(): SgTypedefType // type
       *** get_declaration() : SgTypedefDeclaration
       **** get_base_type(): SgArrayType  // array type for Vector only
       ** get_constraint() SgAdaIndexConstraint // constraints on type
       *** get_indexRanges(): SgTypeExpression
       **** get_type() SgTypedefType
       ***** get_declaration() SgTypedefDeclaration // Index_Range_1
       ****** get_base_type() : SgAdaSubtype
       ******* get_constraint():  SgAdaRangeConstraint  // constraint on
       ******** get_range(): SgRangeExp
#endif /* USE_CASE */

/*
  struct ArrayBase : sg::DispatchHandler<SgType*>
  {
    static
    SgType& find(const SgArrayType&);

    // invalid case
    void handle(SgNode& n)      { SG_UNEXPECTED_NODE(n); }

    // base case
    void handle(SgType& n)      { res = &n; }

    // recurse on array types
    void handle(SgArrayType& n) { res = &find(n); }
  };
*/

  SgArrayType*
  findArrayType(SgType* ty)
  {
    //~ TypeSkip defaultSkips = TypeSkip(skipTypedefType | skipAdaDerivedType | skipAdaSubtype);

    return ArrayType::find(ty); // , defaultSkips);
  }

  std::vector<SgExpression*>
  findArrayBounds(SgType* ty, SgArrayType* arrty)
  {
    return ArrayBounds::find(ty, arrty);
  }
}

namespace SageInterface
{
namespace ada
{
  bool withPrivateDefinition(const SgDeclarationStatement* n)
  {
    // \todo check that dcl refers to a type

    const SgDeclarationStatement& dcl = SG_DEREF(n);
    const SgDeclarationStatement* def = dcl.get_definingDeclaration();

    return def && def->get_declarationModifier().get_accessModifier().isPrivate();
  }

  std::pair<SgArrayType*, std::vector<SgExpression*> >
  flattenArrayType(SgType* atype)
  {
    SgArrayType* restype = findArrayType(atype);

    return std::make_pair(restype, findArrayBounds(atype, restype));
  }
} // Ada
} // SageInterface

#endif /* _SAGEINTERFACE_ADA_H */
