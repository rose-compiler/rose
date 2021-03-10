

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
    skipArrayType            = (1 << 4),
    skipPointerType          = (1 << 5),
    skipReferenceType        = (1 << 6),  /* C++ */
    skipRvalueReferenceType  = (1 << 7),  /* C++ */
    skipAllReferenceTypes    = (skipReferenceType | skipRvalueReferenceType),  /* C++ */
    skipAdaAccessType        = (1 << 8),
    //~ skipUsingDecls           = (1 << 8),  /* C++ */
    //~ skipAdaUseTypes      = (1 << 9),  /* Ada */
    skipLast                 = (1 << 30)
  };


  struct BaseType : sg::DispatchHandler<SgType*>
  {
      typedef sg::DispatchHandler<SgType*> base;

      explicit
      BaseType(TypeSkip skipped)
      : base(NULL), skipWhat(skipped)
      {}

      static
      SgType& find(SgType* n, TypeSkip skipWhat = skipNone);

      SgType* recurse(SgType* n);

      template <class SageTypeNode>
      SgType*
      skipIf(SageTypeNode& n, TypeSkip nodeKind)
      {
        const bool skipNode = (skipWhat & nodeKind) == nodeKind;

        return skipNode ? recurse(n.get_base_type()) : &n;
      }

      // invalid case
      void handle(SgNode& n)                { SG_UNEXPECTED_NODE(n); }

      // base case
      void handle(SgType& n)                { res = &n; }

      // possibly skipped types
      void handle(SgAdaSubtype& n)          { res = skipIf(n, skipAdaSubtype); }
      void handle(SgAdaDerivedType& n)      { res = skipIf(n, skipAdaDerivedType); }
      void handle(SgTypedefType& n)         { res = skipIf(n, skipTypedefType); }
      void handle(SgModifierType& n)        { res = skipIf(n, skipModifierType); }
      void handle(SgArrayType& n)           { res = skipIf(n, skipArrayType); }
      void handle(SgPointerType& n)         { res = skipIf(n, skipPointerType); }
      void handle(SgReferenceType& n)       { res = skipIf(n, skipReferenceType); }
      void handle(SgRvalueReferenceType& n) { res = skipIf(n, skipRvalueReferenceType); }
      void handle(SgAdaAccessType& n)       { res = skipIf(n, skipAdaAccessType); }

    private:
      TypeSkip skipWhat;
  };

  SgType&
  BaseType::find(SgType* n, TypeSkip skipWhat)
  {
    SgType* res = sg::dispatch(BaseType(skipWhat), n);

    return SG_DEREF(res);
  }

  SgType*
  BaseType::recurse(SgType* n)
  {
    return &BaseType::find(n, skipWhat);
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

  SgType&
  findArrayBase(const SgArrayType& arrayType)
  {
    SgType& ty = BaseType::find( arrayType.get_base_type(),
                                 TypeSkip(skipTypedefType | skipAdaDerivedType | skipAdaSubtype)
                               );

    if (SgArrayType* arr = isSgArrayType(&ty))
      return findArrayBase(*arr);

    return ty;
  }

  std::vector<SgExpression*>
  findBounds(const SgArrayType& arrayType)
  {
    std::vector<SgExpression*> res;

    return res;
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

  std::pair<SgType*, std::vector<SgExpression*> >
  flattenArrayType(const SgArrayType* arrayType)
  {
    ROSE_ASSERT(arrayType);

    return std::make_pair( &findArrayBase(*arrayType),
                           findBounds(*arrayType)
                         );
  }
} // Ada
} // SageInterface

#endif /* _SAGEINTERFACE_ADA_H */
