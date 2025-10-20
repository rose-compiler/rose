#include <sage3basic.h>

#include "ExternalMangling.h"

#include <unordered_map>
#include <set>
#include <sstream>
#include <iostream>
#include <cctype>
#include <algorithm>

#include <sageGeneric.h>
#include <sageInterface.h>
#include <Combinatorics.h>

#include <boost/range/adaptors.hpp>
#include <boost/range/numeric.hpp>
#include <boost/algorithm/string/replace.hpp>

#include "CodeThornLib.h"

// #include "stringcompress.hpp"

//~ #include <boost/range/algorithm_ext/for_each.hpp>
// #include <boost/range/join.hpp>

namespace si = SageInterface;
namespace adapt = boost::adaptors;



namespace
{
  std::set<std::string> unhandledSageNodeTypes;

  // ROSE string name mangling literals

  constexpr const char* const mglVoid                     = "v";
  constexpr const char* const mglGlobalVoid               = "gv";

  constexpr const char* const mglDefaultType              = "__DefTpb_";
  constexpr const char* const mglDefaultTypeDelim         = "__DefTpe_";

  constexpr const char* const mglBool                     = "b";

  constexpr const char* const mglChar                     = "c";
  constexpr const char* const mglSignedChar               = "si";
  constexpr const char* const mglUnsignedChar             = "Uc";
  constexpr const char* const mglWChar                    = "wc";
  constexpr const char* const mglChar16                   = "c16";
  constexpr const char* const mglChar32                   = "c32";

  constexpr const char* const mglSignedShort              = "s";
  constexpr const char* const mglUnsignedShort            = "Us";
  constexpr const char* const mglSignedInt                = "i";
  constexpr const char* const mglUnsignedInt              = "Ui";
  constexpr const char* const mglSignedLong               = "l";
  constexpr const char* const mglUnsignedLong             = "Ul";

  constexpr const char* const mglSignedLongLong           = "SL";
  constexpr const char* const mglUnsignedLongLong         = "UL";
  constexpr const char* const mglSignedInt128             = "SL128";
  constexpr const char* const mglUnsignedInt128           = "UL128";

  constexpr const char* const mglFloat                    = "f";
  constexpr const char* const mglDouble                   = "d";
  constexpr const char* const mglLongDouble               = "L";
  constexpr const char* const mglFloat16                  = "x";
  constexpr const char* const mglFloat32                  = "y";
  constexpr const char* const mglFloat80                  = "w";
  constexpr const char* const mglFloat128                 = "q";

  constexpr const char* const mglComplex                  = "Complex";

  constexpr const char* const mglEllipse                  = "e";
  constexpr const char* const mglNullptr                  = "nullptr_t";

  constexpr const char* const mglFunctionType             = "_Fb_";
  constexpr const char* const mglFunctionTypeDelim        = "_Fe_";
  constexpr const char* const mglMemberFunctionType       = "__MFb_";

  constexpr const char* const mglPointerType              = "_Pb_";
  constexpr const char* const mglPointerTypeDelim         = "_Pe_";
  constexpr const char* const mglPointerMemberType        = "_PMb_";

  constexpr const char* const mglReferenceType            = "_Rb_";
  constexpr const char* const mglReferenceTypeDelim       = "_Re_";

  constexpr const char* const mglRvalueReferenceType      = "_RValueb_";
  constexpr const char* const mglRvalueReferenceTypeDelim = "_RValuee_";

  constexpr const char* const mglArrayType                = "_Ab_";
  constexpr const char* const mglArrayTypeDelim           = "_Ae_";
  constexpr const char* const mglArrayIndex               = "_index_";
  constexpr const char* const mglArrayVL                  = "vla_dimension";

  constexpr const char* const mglAnonymousTypeName        = "_anonymousType_";

  constexpr const char* const mglModifierAliased          = "A";
  constexpr const char* const mglModifierNotNull          = "N";
  constexpr const char* const mglModifierConst            = "C";
  constexpr const char* const mglModifierVolatile         = "V";
  constexpr const char* const mglModifierRestrict         = "RST";
  constexpr const char* const mglModifierUpcShared        = "SHD";
  constexpr const char* const mglModifierUpcStrict        = "STR";
  constexpr const char* const mglModifierUpcRelaxed       = "RLX";

  constexpr const char* const mglMemFuncConst             = "_cf";
  constexpr const char* const mglMemFuncVolatile          = "_vf";
  constexpr const char* const mglMemFuncRestrict          = "_rf";
  constexpr const char* const mglMemFuncLvalueRef         = "_lvf";
  constexpr const char* const mglMemFuncRvalueRef         = "_rvf";

  constexpr const char* const mglMemFuncDtor              = "__dt";
  constexpr const char* const mglFuncOperatorNew          = "__nw";
  constexpr const char* const mglFuncOperatorNewArray     = "__na";
  constexpr const char* const mglFuncOperatorDel          = "__dl";
  constexpr const char* const mglFuncOperatorDelArray     = "__da";
  constexpr const char* const mglFuncOperatorConv         = "__opB__";
  constexpr const char* const mglFuncOperatorConvDelim    = "__opE__";

  constexpr const char* const mglFuncOperatorArrow        = "__rf";
  constexpr const char* const mglFuncOperatorArrowStar    = "__rm";
  constexpr const char* const mglFuncOperatorEquals       = "__eq";
  constexpr const char* const mglFuncOperatorNotEquals    = "__ne";
  constexpr const char* const mglFuncOperatorLogicalAnd   = "__aa";
  constexpr const char* const mglFuncOperatorLogicalOr    = "__oo";
  constexpr const char* const mglFuncOperatorNot          = "__nt";
  constexpr const char* const mglFuncOperatorXor          = "__er";
  constexpr const char* const mglFuncOperatorOr           = "__or";
  constexpr const char* const mglFuncOperatorAnd          = "__ad";
  constexpr const char* const mglFuncOperatorLShift       = "__ls";
  constexpr const char* const mglFuncOperatorRShift       = "__rs";
  constexpr const char* const mglFuncOperatorDec          = "__mm";
  constexpr const char* const mglFuncOperatorInc          = "__pp";
  constexpr const char* const mglFuncOperatorComplement   = "__cm";
  constexpr const char* const mglFuncOperatorComma        = "__cm";
  constexpr const char* const mglFuncOperatorApply        = "__cl";
  constexpr const char* const mglFuncOperatorSubscript    = "__xi";
  constexpr const char* const mglFuncOperatorAssign       = "__as";
  constexpr const char* const mglFuncOperatorPlus         = "__pl";
  constexpr const char* const mglFuncOperatorMinus        = "__mi";
  constexpr const char* const mglFuncOperatorMul          = "__ml";
  constexpr const char* const mglFuncOperatorDiv          = "__dv";
  constexpr const char* const mglFuncOperatorMod          = "__md";
  constexpr const char* const mglFuncOperatorAndAssign    = "__aad";
  constexpr const char* const mglFuncOperatorOrAssign     = "__aor";
  constexpr const char* const mglFuncOperatorXorAssign    = "__aer";
  constexpr const char* const mglFuncOperatorLShiftAssign = "__adv";
  constexpr const char* const mglFuncOperatorRShiftAssign = "__amd";
  constexpr const char* const mglFuncOperatorPlusAssign   = "__apl";
  constexpr const char* const mglFuncOperatorMinusAssign  = "__ami";
  constexpr const char* const mglFuncOperatorMulAssign    = "__amu";
  constexpr const char* const mglFuncOperatorDivAssign    = "__adv";
  constexpr const char* const mglFuncOperatorModAssign    = "__amd";
  constexpr const char* const mglFuncOperatorLess         = "__lt";
  constexpr const char* const mglFuncOperatorLessThan     = "__le";
  constexpr const char* const mglFuncOperatorGreater      = "__gt";
  constexpr const char* const mglFuncOperatorGreaterThan  = "__gq";

  constexpr const char* const mglTypedefDeclaration       = "typedef_";
  constexpr const char* const mglTypedefDeclarationDelim  = "__typedef_declaration";
  constexpr const char* const mglEnum                     = "_enum_";
  constexpr const char* const mglClass                    = "_class_";
  constexpr const char* const mglTemplateArgs             = "_tas_";
  constexpr const char* const mglTemplateArgsDelim        = "_tab_";
  constexpr const char* const mglTemplateArgumentPack     = "start_of_pack_expansion_argument";

  constexpr const char* const mglElideFunctionScope       = "_function_parameter_scope_";

  constexpr const char* const mglSimpleSep                = "_";
  constexpr const char* const mglExtendedSep              = "_sep_";

  constexpr const char* const mglItaniumCxx               = "_Z";
  constexpr const char* const mglItaniumEmptyPrefix       = "";
  constexpr const char* const mglItaniumStd               = "St";
  constexpr const char* const mglItaniumNested            = "N";
  constexpr const char* const mglItaniumNestedDelim       = "E";
  constexpr const char* const mglItaniumRoseExt           = "_R";

  constexpr bool              ITANIUM_ABI                 = false;


  void join_(std::string&) {}

  template<typename First, typename... Rest>
  void join_(std::string& os, First&& first, Rest&&... rest)
  {
    os.append(first);

    join_(os, rest...);
  }

  template <class... Args>
  std::string join(std::string s, Args&&... args)
  {
    join_(s, std::forward<Args>(args)...);
    return s;
  }

  void joinNames_(std::string&) {}

  template<typename First, typename... Rest>
  void joinNames_(std::string& os, First&& first, Rest&&... rest)
  {
    if (ITANIUM_ABI)
      os.append(std::to_string(first.size()));

    os.append(first);

    joinNames_(os, rest...);
  }

  template <class... Args>
  std::string joinNames(std::string s, Args&&... args)
  {
    joinNames_(s, std::forward<Args>(args)...);
    return s;
  }

  const SgScopeStatement*
  enclosingScope(const SgNode& n)
  {
    return si::getEnclosingScope(const_cast<SgNode*>(&n));
  }

  std::string replaceFirst(std::string s, const std::string& pat, const std::string& rep)
  {
    boost::algorithm::replace_first(s, pat, rep);
    return s;
  }

  template <class... Args>
  std::string externalName(const SgNode& n, Args&&... args);

  template <class... Args>
  std::string externalName(sg::NotNull<const SgNode> n, Args&&... args)
  {
    return externalName(*n, std::forward<Args>(args)...);
  }

  std::string todo(const char* knd, const SgNode& n)
  {
    std::string res = knd + n.class_name();

    unhandledSageNodeTypes.insert(res);
    return res;
  }

  template <class SageDeclarationPtr>
  bool anonymousTypedef_(SageDeclarationPtr dcl)
  {
    return (  dcl
           //~ && (!dcl->get_isAutonomousDeclaration())
           && dcl->get_isUnNamed()
           );
  }

  bool anonymousTypedef(const SgClassDeclaration* dcl)
  {
    return anonymousTypedef_(dcl);
  }

  bool anonymousTypedef(const SgEnumDeclaration* dcl)
  {
    return anonymousTypedef_(dcl);
  }

  using SubstitutionMapBase = std::unordered_map<const SgNode*, std::size_t>;

  struct SubstitutionMap : private SubstitutionMapBase
  {
    using base = SubstitutionMapBase;
    using base::base;

    using base::insert;
  };



  struct ExternalMangler : sg::DispatchHandler<std::string>
  {
      enum Arguments : bool
      {
        WITH_UNIQUE_MAIN    = true,
        NO_UNQIUE_MAIN      = !WITH_UNIQUE_MAIN,

        WITH_RETURN_TYPE    = true,
        NO_RETURN_TYPE      = !WITH_RETURN_TYPE,

        ARRAY_TO_POINTER    = true,
        NO_ARRAY_TO_POINTER = !WITH_RETURN_TYPE
      };

      explicit
      ExternalMangler( bool uniqueMain = NO_UNQIUE_MAIN,
                       bool useReturnType = WITH_RETURN_TYPE,
                       bool arrayToPointer = NO_ARRAY_TO_POINTER
                     )
      : Base(),
        makeMainUnique(uniqueMain),
        withReturnType(useReturnType),
        arrayAsPointerType(arrayToPointer),
        internalLinkageID()
      {}

      //
      // helpers
      //

      /// mangles the base type of \p n
      template <class SageNode>
      std::string mangleBase(const SageNode& n)
      {
        std::string mangled;

        std::swap(res, mangled);
        handle(sg::asBaseType(n));
        std::swap(res, mangled);

        return mangled;
      }

      std::string mangleEnclosingScope(const SgScopeStatement& n);
      std::string mangleEnclosingScope(sg::NotNull<const SgScopeStatement> n) { return mangleEnclosingScope(*n); }

      std::string mangleParentNode(const SgNode& n);

      std::string mangleScope(const SgScopeStatement& n);
      std::string mangleScope(sg::NotNull<const SgScopeStatement> n)
      {
        return mangleScope(*n);
      }

      //~ std::string mangleModifier(const SgFunctionModifier&);
      //~ std::string mangleModifier(const SgDeclarationModifier&);
      //~ std::string mangleModifier(const SgTypeModifier&);
      //~ std::string mangleModifier(const SgConstVolatileModifier&);

      std::string mangleType(const SgType&, bool withReturn = WITH_RETURN_TYPE, bool pointerToArray = NO_ARRAY_TO_POINTER);
      std::string mangleType(sg::NotNull<const SgType> n, bool withReturn = WITH_RETURN_TYPE, bool pointerToArray = NO_ARRAY_TO_POINTER)
      {
        return mangleType(*n, withReturn, pointerToArray);
      }

      std::string mangleReturnType(const SgType&);
      std::string mangleReturnType(sg::NotNull<const SgType> n)
      {
        return mangleReturnType(*n);
      }

      template <class ForwardIterator>
      std::string mangleTypeRange(ForwardIterator beg, ForwardIterator lim);

      template <class Sequence>
      std::string mangleNodeSequence(Sequence&& rng, const char* listsep = mglSimpleSep, bool arrayToPointer = NO_ARRAY_TO_POINTER);

      std::string mangleTypeRange(const std::vector<SgType*>& vec);

      std::string mangleDeclaration(const SgDeclarationStatement&);
      std::string mangleDeclaration(sg::NotNull<const SgDeclarationStatement> n)
      {
        return mangleDeclaration(*n);
      }

      std::string mangleInternalLinkage(const SgLocatedNode&);

      std::string mangleInternalLinkageIfStatic(const SgDeclarationStatement&);
      std::string mangleInternalLinkageIfStatic(sg::NotNull<const SgDeclarationStatement> n)
      {
        return mangleInternalLinkageIfStatic(*n);
      }

      std::string mangleInternalLinkageIfAnonymous(const SgNamespaceDeclarationStatement&);

      std::string mangleInitializedName(const SgInitializedName&);
      std::string mangleInitializedName(sg::NotNull<const SgInitializedName> n)
      {
        return mangleInitializedName(*n);
      }

  /*
      std::string mangleParameterlist(const SgFunctionParameterList&);
      std::string mangleParameterlist(sg::NotNull<const SgFunctionParameterList> n)
      {
        return mangleParameterlist(*n);
      }
  */
      std::string mangleArrayIndexExpressionOpt(bool vla, const SgExpression* idx);
      std::string mangleMemberFunctionQualifiers(const SgMemberFunctionType& n);
      std::string mangleIndirection(const char* prefix, const char* suffix, const SgType*);


      //
      //

      std::string coreName(const SgNamespaceDeclarationStatement& n);
      std::string coreName(const SgFunctionDeclaration& n, std::string linkage = {});
      std::string coreName(const SgInitializedName& n);


      std::string functionName(const SgFunctionDeclaration&);
      std::string simpleName(std::string n, const char* caller = "");
      std::string simpleName(const char* prefix, std::string n, const char* caller = "");


      //
      // handlers
      //

      void handle(const SgNode& n) { SG_UNEXPECTED_NODE(n); }

      //
      // types

      //~ void handle(const SgType& n) { res = todo("typ:", n); }

      void handle(const SgTypeVoid&)                  { res = mglVoid; }
      void handle(const SgTypeGlobalVoid&)            { res = mglGlobalVoid; }

      void handle(const SgTypeBool&)                  { res = mglBool; }

      void handle(const SgTypeChar&)                  { res = mglChar; }
      void handle(const SgTypeSignedChar&)            { res = mglSignedChar; }
      void handle(const SgTypeUnsignedChar&)          { res = mglUnsignedChar; }
      void handle(const SgTypeWchar&)                 { res = mglWChar; }
      void handle(const SgTypeChar16&)                { res = mglChar16; }
      void handle(const SgTypeChar32&)                { res = mglChar32; }

      void handle(const SgTypeShort&)                 { res = mglSignedShort; }
      void handle(const SgTypeSignedShort&)           { res = mglSignedShort; }
      void handle(const SgTypeUnsignedShort&)         { res = mglUnsignedShort; }

      void handle(const SgTypeInt&)                   { res = mglSignedInt; }
      void handle(const SgTypeSignedInt&)             { res = mglSignedInt; }
      void handle(const SgTypeUnsignedInt&)           { res = mglUnsignedInt; }

      void handle(const SgTypeLong&)                  { res = mglSignedLong; }
      void handle(const SgTypeSignedLong&)            { res = mglSignedLong; }
      void handle(const SgTypeUnsignedLong&)          { res = mglUnsignedLong; }

      void handle(const SgTypeLongLong&)              { res = mglSignedLongLong; }
      void handle(const SgTypeSignedLongLong&)        { res = mglSignedLongLong; }
      void handle(const SgTypeUnsignedLongLong&)      { res = mglUnsignedLongLong; }
      void handle(const SgTypeSigned128bitInteger&)   { res = mglSignedInt128; }
      void handle(const SgTypeUnsigned128bitInteger&) { res = mglUnsignedInt128; }

      void handle(const SgTypeFloat&)                 { res = mglFloat; }
      void handle(const SgTypeDouble&)                { res = mglDouble; }
      void handle(const SgTypeLongDouble&)            { res = mglLongDouble; }
      void handle(const SgTypeFloat16&)               { res = mglFloat16; }
      void handle(const SgTypeFloat32&)               { res = mglFloat32; }
      void handle(const SgTypeFloat80&)               { res = mglFloat80; }
      void handle(const SgTypeFloat128&)              { res = mglFloat128; }

      void handle(const SgTypeComplex& n)
      {
        ASSERT_require(n.get_type_kind() == nullptr);
        res = join( mglComplex
                  , mangleType(n.get_base_type())
                  );
      }

      void handle(const SgTypeEllipse&)           { res = mglEllipse; }
      void handle(const SgTypeNullptr&)           { res = mglNullptr; }

      void handle(const SgDeclType& n)            { res = mangleType(n.get_base_type()); }

      void handle(const SgTypedefType& n)
      {
        sg::NotNull<const SgType> baseTy = n.get_base_type();
        bool                      typedefsAnonymousDecl = false;

        // for a anonymous class/union/enum declared in a typedef
        //   use the typedef name.
        if (const SgClassType* baseClassTy = isSgClassType(baseTy))
          typedefsAnonymousDecl = anonymousTypedef(isSgClassDeclaration(baseClassTy->get_declaration()));
        else if (const SgEnumType* baseEnumTy = isSgEnumType(baseTy))
          typedefsAnonymousDecl = anonymousTypedef(isSgEnumDeclaration(baseEnumTy->get_declaration()));

        res = typedefsAnonymousDecl ? mangleDeclaration(n.get_declaration())
                                    : mangleType(n.get_base_type(), WITH_RETURN_TYPE, arrayAsPointerType);

      }

      void handle(const SgTypeDefault& n)
      {
        res = join( mglDefaultType
                  , n.get_name()
                  , mglDefaultTypeDelim
                  );
      }

      void handle(const SgPointerType& n)         { res = mangleIndirection(mglPointerType, mglPointerTypeDelim, n.get_base_type()); }
      void handle(const SgReferenceType& n)       { res = mangleIndirection(mglReferenceType, mglReferenceTypeDelim, n.get_base_type()); }
      void handle(const SgRvalueReferenceType& n) { res = mangleIndirection(mglRvalueReferenceType, mglRvalueReferenceTypeDelim, n.get_base_type()); }

      void handle(const SgPointerMemberType& n)
      {
        res = replaceFirst( mangleBase(n), mglPointerType,
                            join(mglPointerMemberType, mangleType(n.get_class_type()))
                          );
      }

      void handle(const SgArrayType& n);

      void handle(const SgFunctionType& n)
      {
        sg::NotNull<const SgFunctionParameterTypeList> funParameterTypeList = n.get_argument_list();
        const SgTypePtrList&                           parmTypeList = funParameterTypeList->get_arguments();

        res = join( mglFunctionType
                  , mangleReturnType(n.get_return_type()) // return types are not part of the C mangling
                  , mangleNodeSequence(parmTypeList, mglSimpleSep, ARRAY_TO_POINTER)
                  , mglFunctionTypeDelim
                  );
      }

      void handle(const SgMemberFunctionType& n)
      {
        std::string functionTypeName = mangleBase(n);

        res = join( mglMemberFunctionType
                  , mangleType(n.get_class_type())
                  , mangleMemberFunctionQualifiers(n)
                  , functionTypeName
                  );
      }

      void handle(const SgModifierType& n)
      {
        const SgTypeModifier& type_mod = n.get_typeModifier();

        // Ada?
        if (type_mod.isAliased()) res += mglModifierAliased;
        if (type_mod.isNotNull()) res += mglModifierNotNull;

        {
          const SgConstVolatileModifier& cv_mod = type_mod.get_constVolatileModifier();

          if (cv_mod.isConst())      res += mglModifierConst;
          if (cv_mod.isVolatile())   res += mglModifierVolatile;
          if (type_mod.isRestrict()) res += mglModifierRestrict;
        }

        {
          const SgUPC_AccessModifier& upc_mod = type_mod.get_upcModifier();

          if (upc_mod.get_isShared())  res += (mglModifierUpcShared + std::to_string(upc_mod.get_layout()));
          if (upc_mod.isUPC_Strict())  res += mglModifierUpcStrict;
          if (upc_mod.isUPC_Relaxed()) res += mglModifierUpcRelaxed;
        }

        res += mangleType(n.get_base_type(), WITH_RETURN_TYPE, arrayAsPointerType);
      }

      void handle(const SgNamedType& n) { res = mangleDeclaration(n.get_declaration()); }

      //
      // others

      void handle(const SgTemplateArgument& n)
      {
        switch (n.get_argumentType())
        {
          case SgTemplateArgument::type_argument:
            res = mangleType(n.get_type());
            break;

          case SgTemplateArgument::nontype_argument:
            if (const SgExpression* expr = n.get_expression())
              res = mangleExpression(expr);
            else
              res = SG_DEREF(n.get_initializedName()).get_name();
            break;

          case SgTemplateArgument::template_template_argument:
            res = mangleDeclaration(n.get_templateDeclaration());
            break;

         // DQ (5/19/2014): Added support for varadic template argument.
          case SgTemplateArgument::start_of_pack_expansion_argument:
            res = mglTemplateArgumentPack;
            break;

          default:
            ROSE_ABORT();
        }
      }


      //

      void handle(const SgInitializedName& n)
      {
        res = join( coreName(n)
                  , mangleInternalLinkageIfStatic(isSgVariableDeclaration(n.get_parent()))
                  );
      }

      //
      // scopes

      void handle(const SgScopeStatement& n)
      {
        res = todo("scp:", n);
      }

      void handle(const SgGlobal&)
      {
        if (ITANIUM_ABI)
          res = mglItaniumCxx;
      }

      void handle(const SgBasicBlock& n)                { res = mangleEnclosingScope(n); }

      // scopes with declarations
      void handle(const SgClassDefinition& n)           { res = mangleParentNode(n); }
      void handle(const SgFunctionDefinition& n)        { res = mangleParentNode(n); }
      void handle(const SgNamespaceDefinitionStatement& n) { res = mangleParentNode(n); }


      //
      // declarations

      void handle(const SgDeclarationStatement& n)
      {
        res = todo("dcl:", n);
      }

      void handle(const SgEnumDeclaration& n)
      {
        sg::NotNull<const SgScopeStatement> scope = n.get_scope();
        std::string                         qualifiers = isSgFunctionDefinition(scope)
                                                             ?  std::string{mglElideFunctionScope}
                                                             :  mangleScope(scope);

        res = join( qualifiers
                  , mglEnum
                  , simpleName(n.get_name(), "enm")
                  );
      }

      void handle(const SgClassDeclaration& n)
      {
        // \todo how about class/unions in parameter lists? Handle like enums?

        res = join( mangleScope(n.get_scope())
                  , mglClass
                  , simpleName(n.get_name(), "cls")
                  );
      }

      void handle(const SgTemplateInstantiationDecl& n)
      {
        res = join( mangleScope(n.get_scope())
                  , simpleName(n.get_templateName(), "tcl")
                  , mglTemplateArgs
                  , mangleNodeSequence(n.get_templateArguments(), mglExtendedSep)
                  , mglTemplateArgsDelim
                  );
      }


      void handle(const SgNamespaceDeclarationStatement& n)
      {
        res = join( coreName(n)
                  , mangleInternalLinkageIfAnonymous(n)
                  );
      }

      void handle(const SgFunctionDeclaration& n)
      {
        res = coreName(n, mangleInternalLinkageIfStatic(n));
      }

      void handle(const SgMemberFunctionDeclaration& n)
      {
        res = coreName(n);
      }

      void handle(const SgTemplateInstantiationFunctionDecl& n)
      {
        res = join( mangleScope(n.get_scope())
                  , simpleName(n.get_templateName(), "tfn")
                  , mglTemplateArgs
                  , mangleNodeSequence(n.get_templateArguments(), mglExtendedSep)
                  , mglTemplateArgsDelim
                  , mangleType(n.get_type(), NO_RETURN_TYPE)
                  , mangleInternalLinkageIfStatic(n)
                  );
      }

      void handle(const SgTemplateInstantiationMemberFunctionDecl& n)
      {
        res = join( mangleScope(n.get_scope())
                  , simpleName(n.get_templateName(), "tmf")
                  , mglTemplateArgs
                  , mangleNodeSequence(n.get_templateArguments(), mglExtendedSep)
                  , mglTemplateArgsDelim
                  , mangleType(n.get_type(), NO_RETURN_TYPE)
                  );
      }

      void handle(const SgTypedefDeclaration& n)
      {
        res = join( mangleScope(n.get_scope())
                  , mglTypedefDeclaration
                  , simpleName(n.get_name(), "tdf")
                  , mglTypedefDeclarationDelim
                  );
      }

      void handle(const SgTemplateInstantiationTypedefDeclaration& n)
      {
        res = join( mangleScope(n.get_scope())
                  , mglTypedefDeclaration
                  , simpleName(n.get_templateName(), "tid")
                  , mglTemplateArgs
                  , mangleNodeSequence(n.get_templateArguments(), mglExtendedSep)
                  , mglTemplateArgsDelim
                  , mglTypedefDeclarationDelim
                  );
      }

    private:
      bool        makeMainUnique;
      bool        withReturnType;
      bool        arrayAsPointerType;
      std::string internalLinkageID;
  };

  std::string ExternalMangler::simpleName(std::string name, const char*)
  {
    return name;
  }

  std::string ExternalMangler::simpleName(const char* prefix, std::string name, const char* dbg)
  {
    if (!ITANIUM_ABI)
      return simpleName(name, dbg);

    std::string mangled;

    mangled.append(prefix);
    mangled.append(std::to_string(name.size()));
    mangled.append(name);

    return mangled;
  }

  bool isMainFunction(const SgFunctionDeclaration& n)
  {
    return (n.get_name() == "main") && isSgGlobal(n.get_scope());
  }

  std::string ExternalMangler::functionName(const SgFunctionDeclaration& n)
  {
    const SgSpecialFunctionModifier& special = n.get_specialFunctionModifier();

    // Special case: destructor names
    if (special.isDestructor())
      return mglMemFuncDtor;

    std::string name = n.get_name();
    const bool  isOperator = special.isOperator();

    if (isOperator) // begins with "operator"
    {
      std::string mangled;

      // note, testing the ending of a token
      //   requires that longer operators with the same ending characters
      //   need to be tested before operators with a subset of ending tokens.
      //   e.g., test new[] before [], ++ before +, etc.
      if (boost::ends_with(name, "="))
      {
        if (boost::ends_with(name, "<<="))
          mangled = mglFuncOperatorLShiftAssign;
        else if (boost::ends_with(name, ">>="))
          mangled = mglFuncOperatorRShiftAssign;
        else if (boost::ends_with(name, "=="))
          mangled = mglFuncOperatorEquals;
        else if (boost::ends_with(name, "!="))
          mangled = mglFuncOperatorNotEquals;
        else if (boost::ends_with(name, "+="))
          mangled = mglFuncOperatorPlusAssign;
        else if (boost::ends_with(name, "-="))
          mangled = mglFuncOperatorMinusAssign;
        else if (boost::ends_with(name, "*="))
          mangled = mglFuncOperatorMulAssign;
        else if (boost::ends_with(name, "/="))
          mangled = mglFuncOperatorDivAssign;
        else if (boost::ends_with(name, "%="))
          mangled = mglFuncOperatorModAssign;
        else if (boost::ends_with(name, "|="))
          mangled = mglFuncOperatorOrAssign;
        else if (boost::ends_with(name, "&="))
          mangled = mglFuncOperatorAndAssign;
        else if (boost::ends_with(name, "^="))
          mangled = mglFuncOperatorXorAssign;
        else if (boost::ends_with(name, "<="))
          mangled = mglFuncOperatorLessThan;
        else if (boost::ends_with(name, ">="))
          mangled = mglFuncOperatorGreaterThan;
        else // if (boost::ends_with(name, "="))
          mangled = mglFuncOperatorAssign;
      }
      else if (boost::ends_with(name, "++"))
        mangled = mglFuncOperatorInc;
      else if (boost::ends_with(name, "+"))
        mangled = mglFuncOperatorPlus;
      else if (boost::ends_with(name, "--"))
        mangled = mglFuncOperatorDec;
      else if (boost::ends_with(name, "-"))
        mangled = mglFuncOperatorMinus;
      else if (boost::ends_with(name, "->*"))
        mangled = mglFuncOperatorArrowStar;
      else if (boost::ends_with(name, "*"))
        mangled = mglFuncOperatorMul;
      else if (boost::ends_with(name, "/"))
        mangled = mglFuncOperatorDiv;
      else if (boost::ends_with(name, "()"))
        mangled = mglFuncOperatorApply;
      else if (boost::ends_with(name, "<<"))
        mangled = mglFuncOperatorLShift;
      else if (boost::ends_with(name, "<"))
        mangled = mglFuncOperatorLess;
      else if (boost::ends_with(name, "->"))
        mangled = mglFuncOperatorArrow;
      else if (boost::ends_with(name, ">>"))
        mangled = mglFuncOperatorRShift;
      else if (boost::ends_with(name, ">"))
        mangled = mglFuncOperatorGreater;
      else if (boost::ends_with(name, "%"))
        mangled = mglFuncOperatorMod;
      else if (boost::ends_with(name, "!"))
        mangled = mglFuncOperatorNot;
      else if (boost::ends_with(name, "^"))
        mangled = mglFuncOperatorXor;
      else if (boost::ends_with(name, "~"))
        mangled = mglFuncOperatorComplement;
      else if (boost::ends_with(name, "new"))
        mangled = mglFuncOperatorNew;
      else if (boost::ends_with(name, "new[]"))
        mangled = mglFuncOperatorNewArray;
      else if (boost::ends_with(name, "delete"))
        mangled = mglFuncOperatorDel;
      else if (boost::ends_with(name, "delete[]"))
        mangled = mglFuncOperatorDelArray;
      else if (boost::ends_with(name, "[]"))
        mangled = mglFuncOperatorSubscript;
      else if (boost::ends_with(name, "&&"))
        mangled = mglFuncOperatorLogicalAnd;
      else if (boost::ends_with(name, "&"))
        mangled = mglFuncOperatorAnd;
      else if (boost::ends_with(name, "||"))
        mangled = mglFuncOperatorLogicalOr;
      else if (boost::ends_with(name, "|"))
        mangled = mglFuncOperatorOr;
      else if (boost::ends_with(name, ","))
        mangled = mglFuncOperatorComma;

      ASSERT_require(mangled.size());
      return mangled;
    }

    if (special.isConversion())
    {
      return join( mglFuncOperatorConv
                 , mangleType(SG_DEREF(n.get_type()).get_return_type())
                 , mglFuncOperatorConvDelim
                 );
    }

    if (makeMainUnique && isMainFunction(n))
    {
      return join( name
                 , mglSimpleSep
                 , mangleInternalLinkage(n)
                 );
    }

    return name;
  }


  struct GetType
  {
    template <class SageNode>
    const SgType* operator()(sg::NotNull<const SageNode> n)
    {
      return n->get_type();
    }
  };

  struct MangleNode
  {
    std::string operator()(const SgNode* n) const
    {
      return externalName(n, uniqueName, ExternalMangler::WITH_RETURN_TYPE, arrayToPointer);
    }

    bool uniqueName;
    bool arrayToPointer;
  };

  struct ConcatStrings
  {
    std::string operator()(std::string lhs, const std::string& rhs)
    {
      lhs.reserve(lhs.size() + rhs.size() + 1);
      lhs += sep;
      lhs += rhs;

      sep = listsep;
      return lhs;
    }

    const char* listsep;
    const char* sep = "";
  };

  template <class Sequence>
  std::string ExternalMangler::mangleNodeSequence(Sequence&& seq, const char* sep, bool arrayToPointer)
  {
    return boost::accumulate( seq | adapt::transformed(MangleNode{makeMainUnique, arrayToPointer}),
                              std::string{},
                              ConcatStrings{sep}
                            );
  }

  template <class ForwardIterator>
  std::string ExternalMangler::mangleTypeRange(ForwardIterator beg, ForwardIterator lim)
  {
    return mangleNodeSequence(boost::make_iterator_range(beg, lim));
  }

/*
  std::string ExternalMangler::mangleParameterlist(const SgFunctionParameterList& n)
  {
    // auto typeList = n.get_args() | adapt::transformed(GetType{});

    return ""; // boost::accumulate(typeList, std::string{}, ConcatStrings{});
  }
*/

  std::string ExternalMangler::mangleType(const SgType& n, bool withReturnType, bool pointerToArray)
  {
    return externalName(n, makeMainUnique, withReturnType, pointerToArray);
  }

  std::string ExternalMangler::mangleReturnType(const SgType& n)
  {
    if (!withReturnType)
      return {};

    return mangleType(n);
  }

  std::string ExternalMangler::mangleDeclaration(const SgDeclarationStatement& n)
  {
    return externalName(n, makeMainUnique);
  }

  std::string mangleFilename(std::string filename)
  {
    auto replCh = [](char c) -> char
                  {
                    if ((c == '/') | (c == '\\') | (c == '.'))
                      c = '_';

                    return c;
                  };

    std::transform(filename.begin(), filename.end(), filename.begin(), replCh);
    return filename;
  }

  std::string ExternalMangler::mangleInternalLinkage(const SgLocatedNode& n)
  {
    if (internalLinkageID.size() == 0)
    {
      sg::NotNull<const Sg_File_Info> fi = n.get_file_info();
      std::string                     linkageCandidate = fi->get_filenameString();

      if (linkageCandidate == "NULL_FILE")
      {
        if (const SgDeclarationStatement* dcl = isSgDeclarationStatement(&n))
        {
          if (const SgDeclarationStatement* defdcl = dcl->get_definingDeclaration())
          {
            return mangleInternalLinkage(*defdcl);
          }
        }
      }

      if (linkageCandidate.size() == 0)
        return mangleInternalLinkage(SG_DEREF(isSgLocatedNode(n.get_parent())));

      if (linkageCandidate != "NULL_FILE") // \pp todo revise
        internalLinkageID = mangleFilename(linkageCandidate);
    }

    if (ITANIUM_ABI)
      return join( mglItaniumRoseExt
                 , internalLinkageID
                 );

    return internalLinkageID;
  }

  std::string ExternalMangler::mangleInternalLinkageIfStatic(const SgDeclarationStatement& n)
  {
    if (si::isStatic(const_cast<SgDeclarationStatement*>(&n)))
      return mangleInternalLinkage(n);

    return {};
  }

  std::string ExternalMangler::mangleInternalLinkageIfAnonymous(const SgNamespaceDeclarationStatement& n)
  {
    if (n.get_name().get_length() == 0)
      return mangleInternalLinkage(n);

    return {};
  }

  std::string ExternalMangler::mangleInitializedName(const SgInitializedName& n)
  {
    return externalName(n, makeMainUnique);
  }

  std::string ExternalMangler::mangleScope(const SgScopeStatement& n)
  {
    return externalName(n, makeMainUnique);
  }

  std::string ExternalMangler::mangleEnclosingScope(const SgScopeStatement& n)
  {
    return externalName(enclosingScope(n), makeMainUnique);
  }

  std::string ExternalMangler::mangleParentNode(const SgNode& n)
  {
    return externalName(n.get_parent(), makeMainUnique);
  }


  std::string ExternalMangler::mangleArrayIndexExpressionOpt(bool vla, const SgExpression* idx)
  {
    if (idx == nullptr)
      return {};

    if (/*const SgNullExpression* valExp =*/ isSgNullExpression(idx))
      return {};

    if (/*const SgValueExp* valExp =*/ isSgValueExp(idx))
      return idx->unparseToString();

    if (const SgCastExp* castExp = isSgCastExp(idx))
      return mangleArrayIndexExpressionOpt(vla, castExp->get_operand());

    CodeThorn::AbstractValue aVal = CodeThorn::evaluateExpressionWithEmptyState(const_cast<SgExpression*>(idx));

    if (aVal.isConstInt())
      return std::to_string(aVal.getIntValue());

    if (vla)
      return mglArrayVL;

    SG_UNEXPECTED_NODE(*idx);
/*
    if (const SgVarRefExp* varRefExp = isSgVarRefExp(idx))
    {
      std::string                    mangled;
      sg::NotNull<SgVariableSymbol>  variableSymbol   = isSgVariableSymbol(varRefExp->get_symbol());
      sg::NotNull<SgInitializedName> indexDeclaration = variableSymbol->get_declaration();

      if (vla)
      {
        mangled = mglArrayVL;
      }
      else
      {
        mangled = mangleInitializedName(indexDeclaration); // find constant value
      }

      return mangled;
    }
*/
  }

  std::string ExternalMangler::mangleMemberFunctionQualifiers(const SgMemberFunctionType& n)
  {
    std::stringstream mangled;

    if (n.isConstFunc())           mangled << mglMemFuncConst;
    if (n.isVolatileFunc())        mangled << mglMemFuncVolatile;
    if (n.isRestrictFunc())        mangled << mglMemFuncRestrict;
    if (n.isLvalueReferenceFunc()) mangled << mglMemFuncLvalueRef;
    if (n.isRvalueReferenceFunc()) mangled << mglMemFuncRvalueRef;

    return mangled.str();
  }


  std::string ExternalMangler::mangleIndirection(const char* prefix, const char* suffix, const SgType* baseTy)
  {
    std::string tyName = mangleType(baseTy);

    if (tyName.empty()) tyName = mglAnonymousTypeName;

    return join(prefix, tyName, suffix);
  }

  void ExternalMangler::handle(const SgArrayType& n)
  {
    if (arrayAsPointerType)
    {
      res = mangleIndirection(mglPointerType, mglPointerTypeDelim, n.get_base_type());
      return;
    }

    res = join( mglArrayType
              , mangleType(n.get_base_type())
              , mglArrayIndex
              , mangleArrayIndexExpressionOpt(n.get_is_variable_length_array(), n.get_index())
              , mglArrayTypeDelim
              );
  }

  std::string ExternalMangler::coreName(const SgNamespaceDeclarationStatement& n)
  {
    std::string             name  = n.get_name();
    const SgScopeStatement* scope = n.get_scope();
    const bool              isCxxStd = (name == "std") && isSgGlobal(scope);

    if (ITANIUM_ABI & isCxxStd)
      return mglItaniumStd;

    return join( mangleScope(scope)
               , simpleName(mglItaniumEmptyPrefix, name, "nsp")
               );
  }


  const SgScopeStatement*
  scopeForFunctionMangling(const SgFunctionDeclaration& n)
  {
    if (si::is_C_language() || n.get_linkage() == "C" || isMainFunction(n))
      return nullptr;

    return n.get_scope();
  }

  std::string ExternalMangler::coreName(const SgFunctionDeclaration& n, std::string linkage)
  {
    const SgScopeStatement* scope = scopeForFunctionMangling(n);

    if (!scope)
      return simpleName(functionName(n), "fn2");

    return join( joinNames(mangleScope(n.get_scope()), linkage, simpleName(functionName(n), "fun"))
               , mangleType(n.get_type(), NO_RETURN_TYPE)
               );
  }

  std::string ExternalMangler::coreName(const SgInitializedName& n)
  {
    const SgScopeStatement* scope     = enclosingScope(n);
    const bool              skipScope = isSgGlobal(scope);

    if (skipScope)
      return simpleName(n.get_name(), "in2");

    return join( mangleScope(scope)
               , simpleName(n.get_name(), "ini")
               );
  }


  template <class... Args>
  std::string externalName(const SgNode& n, Args&&... args)
  {
    static std::unordered_map<const SgNode*, std::string> m;

    std::string& mangled = m[&n];

    if (mangled.empty())
      mangled = sg::dispatch(ExternalMangler{std::forward<Args>(args)...}, &n);

    return mangled;
  }


  std::unordered_map<std::string, uint64_t>      shortMangledNameCache;
  std::unordered_map<uint64_t, std::string>      mangledNameHashCollisionCheckMap;
  std::unordered_map<const SgNode*, std::string> alreadyMangled;
}




namespace CodeThorn
{

NameShortener longNames()
{
  return [](std::string s) -> std::string { return s; };
}

NameShortener hashNames()
{
  constexpr bool withCollisionDetection = true;

  return [](std::string oldMangledName) -> std::string
         {
           if (oldMangledName.size() <= 40)
             return oldMangledName;

           std::uint64_t idNumber = 0;
           auto          shortMNIter = shortMangledNameCache.find(oldMangledName);

           if (shortMNIter != shortMangledNameCache.end())
           {
             idNumber = shortMNIter->second;
           }
           else
           {
             Rose::Combinatorics::HasherSha256Builtin hasher;

             hasher.insert(oldMangledName);
             hasher.digest();
             idNumber = hasher.toU64();

             if (withCollisionDetection)
             {
               auto res = mangledNameHashCollisionCheckMap.insert({idNumber, oldMangledName});

               if ((!res.second) && (res.first->second != oldMangledName))
               {
                 std::cerr << "hash collision." << std::endl;
                 ROSE_ABORT();
               }
             }

             shortMangledNameCache.insert({oldMangledName, idNumber});
           }

           std::ostringstream mn;
           mn << 'L' << Rose::Combinatorics::toBase62String(idNumber) << 'R';
           return mn.str();
         };
}


//~ NameShortener compressNames(int windowsize)
//~ {
  //~ return [w=windowsize](std::string s) -> std::string { return compress(std::move(s), w); };
//~ }

const std::string& _mangle( const SgNode& n,
                            NameShortener shorten,
                            bool withUniqueMain = false,
                            bool withReturnType = ExternalMangler::WITH_RETURN_TYPE,
                            bool withPointerToArrayDecay = ExternalMangler::NO_ARRAY_TO_POINTER
                          )
{
  std::string& cached = alreadyMangled[&n];

  if (cached.size() == 0)
  {
    cached = shorten(externalName(n, withUniqueMain, withReturnType, withPointerToArrayDecay));
    ASSERT_require(cached.size() != 0);
  }

  return cached;
}

const std::string& mangle(const SgDeclarationStatement& n, NameShortener shorten, bool withUniqueMain)
{
  return _mangle(n, std::move(shorten), withUniqueMain);
}

const std::string& mangle(const SgDeclarationStatement* n, NameShortener shorten, bool withUniqueMain)
{
  ASSERT_not_null(n);

  return mangle(*n, std::move(shorten), withUniqueMain);
}

const std::string& mangle(const SgType& n, NameShortener shorten, bool withReturnType, bool withArrayToPointerDecay)
{
  return _mangle(n, std::move(shorten), false/*no unique main*/, withReturnType, withArrayToPointerDecay);
}

const std::string& mangle(const SgType* n, NameShortener shorten, bool withReturnType, bool withArrayToPointerDecay)
{
  ASSERT_not_null(n);

  return mangle(*n, std::move(shorten), withReturnType, withArrayToPointerDecay);
}

const std::string& mangle(const SgInitializedName& n, NameShortener shorten)
{
  return _mangle(n, std::move(shorten));
}

const std::string& mangle(const SgInitializedName* n, NameShortener shorten)
{
  ASSERT_not_null(n);

  return mangle(*n, std::move(shorten));
}


void printUnhandledNodes(std::ostream& os)
{
  if (unhandledSageNodeTypes.size() == 0)
    return;

  os << "* Unhandled sage-nodes:\n";
  for (const std::string& s : unhandledSageNodeTypes)
    os << "    " << s << std::endl;
}

void clearManglingCache()
{
  shortMangledNameCache.clear();
  mangledNameHashCollisionCheckMap.clear();
  alreadyMangled.clear();
}

}
