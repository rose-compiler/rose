#include "sage3basic.h"

#include <limits>
#include <cmath>

#include <boost/algorithm/string.hpp>

#include "LibadalangType.h"

#include "sageGeneric.h"
#include "sageInterfaceAda.h"

#include "Libadalang_to_ROSE.h"
//#include "AdaExpression.h"
#include "LibadalangStatement.h"
#include "AdaMaker.h"


// turn on all GCC warnings after include files have been processed
#pragma GCC diagnostic warning "-Wall"
#pragma GCC diagnostic warning "-Wextra"


// workaround: set internal variable before the frontend starts
namespace SageInterface
{
namespace Ada
{
extern SgAdaPackageSpecDecl* stdpkg;
}
}

namespace sb = SageBuilder;
namespace si = SageInterface;



namespace Libadalang_ROSE_Translation
{

namespace
{
  SgTypedefDeclaration&
  declareIntSubtype(const std::string& name, int64_t lo, int64_t hi, SgAdaPackageSpec& scope)
  {
    SgType&               ty = mkIntegralType();
    SgLongLongIntVal&     lb = SG_DEREF(sb::buildLongLongIntVal(lo));
    //ADA_ASSERT(lb.get_value() == lo);

    SgLongLongIntVal&     ub = SG_DEREF(sb::buildLongLongIntVal(hi));
    //ADA_ASSERT(ub.get_value() == hi);

    SgRangeExp&           rng = mkRangeExp(lb, ub);
    SgAdaRangeConstraint& range = mkAdaRangeConstraint(rng);
    SgAdaSubtype&         subtype = mkAdaSubtype(ty, range, false /* from root*/ );
    SgTypedefDeclaration& sgnode = mkTypeDecl(name, subtype, scope);

    scope.append_statement(&sgnode);
    return sgnode;
  }

  template <class CxxType>
  SgTypedefDeclaration&
  declareIntSubtype(const std::string& name, SgAdaPackageSpec& scope)
  {
    using TypeLimits = std::numeric_limits<CxxType>;

    return declareIntSubtype(name, TypeLimits::min(), TypeLimits::max(), scope);
  }

  SgTypedefDeclaration&
  declareRealSubtype(const std::string& name, int ndigits, long double base, int exp, SgAdaPackageSpec& scope)
  {
    SgType&               ty = mkRealType();
    SgExpression&         lb = SG_DEREF(sb::buildFloat128Val(-std::pow(base, exp)));
    SgExpression&         ub = SG_DEREF(sb::buildFloat128Val(+std::pow(base, exp)));
    SgRangeExp&           rng = mkRangeExp(lb, ub);
    SgAdaRangeConstraint& range = mkAdaRangeConstraint(rng);
    SgExpression&         dig = SG_DEREF(sb::buildIntVal(ndigits));
    SgAdaTypeConstraint&  digits = mkAdaDigitsConstraint(dig, &range);
    SgAdaSubtype&         subtype = mkAdaSubtype(ty, digits);
    SgTypedefDeclaration& sgnode = mkTypeDecl(name, subtype, scope);

    scope.append_statement(&sgnode);
    return sgnode;
  }

  SgTypedefDeclaration&
  declareStringType(const std::string& name, SgType& positive, SgType& comp, SgAdaPackageSpec& scope)
  {
    SgExprListExp&        idx     = mkExprListExp({&mkTypeExpression(positive)});
    SgArrayType&          strtype = mkArrayType(comp, idx, true);
    SgTypedefDeclaration& sgnode  = mkTypeDecl(name, strtype, scope);

    scope.append_statement(&sgnode);
    return sgnode;
  }

  SgInitializedName&
  declareException(const std::string& name, SgType& base, SgAdaPackageSpec& scope)
  {
    SgInitializedName&              sgnode = mkInitializedName(name, base, nullptr);
    std::vector<SgInitializedName*> exdecl{ &sgnode };
    SgVariableDeclaration&          exvar = mkExceptionDecl(exdecl, scope);

    exvar.set_firstNondefiningDeclaration(&exvar);
    scope.append_statement(&exvar);
    return sgnode;
  }

  SgAdaPackageSpecDecl&
  declarePackage(const std::string& name, SgAdaPackageSpec& scope)
  {
    SgAdaPackageSpecDecl& sgnode = mkAdaPackageSpecDecl(name, scope);
    SgAdaPackageSpec&     pkgspec = SG_DEREF(sgnode.get_definition());

    scope.append_statement(&sgnode);
    sgnode.set_scope(&scope);

    markCompilerGenerated(pkgspec);
    markCompilerGenerated(sgnode);
    return sgnode;
  }

  // declares binary and unary operator declarations
  //   (all arguments are called left, right and right respectively)
  // see https://www.adaic.org/resources/add_content/standards/05rm/html/RM-A-1.html
  void declareOp(
                  //map_t<OperatorKey, std::vector<OperatorDesc> >& fns,
                  //~ map_t<AdaIdentifier, std::vector<SgFunctionDeclaration*> >& fns,
                  const std::string& name,
                  SgType& retty,
                  std::vector<SgType*> params,
                  SgScopeStatement& scope
                )
  {
    ASSERT_require(params.size() > 0);

    std::string            fnname = si::Ada::roseOperatorPrefix + name;
    auto                   complete =
       [&params](SgFunctionParameterList& fnParmList, SgScopeStatement& scope)->void
       {
         static constexpr int MAX_PARAMS = 2;
         static const std::string parmNames[MAX_PARAMS] = { "Left", "Right" };

         int            parmNameIdx = MAX_PARAMS - params.size() - 1;
         SgTypeModifier defaultInMode;

         defaultInMode.setDefault();

         //ADA_ASSERT(params.size() <= MAX_PARAMS);
         for (SgType* parmType : params)
         {
           const std::string&       parmName = parmNames[++parmNameIdx];
           SgInitializedName&       parmDecl = mkInitializedName(parmName, SG_DEREF(parmType), nullptr);
           SgInitializedNamePtrList parmList = {&parmDecl};
           /* SgVariableDeclaration&   pvDecl   =*/ mkParameter(parmList, defaultInMode, scope);

           parmDecl.set_parent(&fnParmList);
           fnParmList.get_args().push_back(&parmDecl);
         }
       };

    SgFunctionDeclaration& fndcl  = mkProcedureDecl_nondef(fnname, scope, retty, complete);

    //fns[OperatorKey{&scope, name}].emplace_back(&fndcl, OperatorDesc::DECLARED_IN_STANDARD);
    //~ fns[].emplace_back(&fndcl);
  }

  void declareCharConstants(const std::string& name, char ch, SgType& ty, SgScopeStatement& scope)
  {
    SgCharVal*             val = sb::buildCharVal(ch);
    markCompilerGenerated(SG_DEREF(val));

    SgInitializedName&     var = mkInitializedName(name, ty, val);
    SgVariableDeclaration& dcl = mkVarDecl(var, scope);

    scope.append_statement(&dcl);
  }

  void declareCharConstants(SgType& adaCharType, SgScopeStatement& asciispec)
  {
    char cval = 0;
    declareCharConstants("nul",   cval, adaCharType, asciispec);
    declareCharConstants("soh", ++cval, adaCharType, asciispec);
    declareCharConstants("stx", ++cval, adaCharType, asciispec);
    declareCharConstants("etx", ++cval, adaCharType, asciispec);
    declareCharConstants("eot", ++cval, adaCharType, asciispec);
    declareCharConstants("enq", ++cval, adaCharType, asciispec);
    declareCharConstants("ack", ++cval, adaCharType, asciispec);
    declareCharConstants("bel", ++cval, adaCharType, asciispec);
    declareCharConstants("bs",  ++cval, adaCharType, asciispec);
    declareCharConstants("ht",  ++cval, adaCharType, asciispec);
    declareCharConstants("lf",  ++cval, adaCharType, asciispec);
    declareCharConstants("vt",  ++cval, adaCharType, asciispec);
    declareCharConstants("ff",  ++cval, adaCharType, asciispec);
    declareCharConstants("cr",  ++cval, adaCharType, asciispec);
    declareCharConstants("so",  ++cval, adaCharType, asciispec);
    declareCharConstants("si",  ++cval, adaCharType, asciispec);

    declareCharConstants("dle", ++cval, adaCharType, asciispec);
    declareCharConstants("dc1", ++cval, adaCharType, asciispec);
    declareCharConstants("dc2", ++cval, adaCharType, asciispec);
    declareCharConstants("dc3", ++cval, adaCharType, asciispec);
    declareCharConstants("dc4", ++cval, adaCharType, asciispec);
    declareCharConstants("nak", ++cval, adaCharType, asciispec);
    declareCharConstants("syn", ++cval, adaCharType, asciispec);
    declareCharConstants("etb", ++cval, adaCharType, asciispec);
    declareCharConstants("can", ++cval, adaCharType, asciispec);
    declareCharConstants("em",  ++cval, adaCharType, asciispec);
    declareCharConstants("sub", ++cval, adaCharType, asciispec);
    declareCharConstants("esc", ++cval, adaCharType, asciispec);
    declareCharConstants("fs",  ++cval, adaCharType, asciispec);
    declareCharConstants("gs",  ++cval, adaCharType, asciispec);
    declareCharConstants("rs",  ++cval, adaCharType, asciispec);
    declareCharConstants("us",  ++cval, adaCharType, asciispec);
    //ADA_ASSERT(cval == 31);

    declareCharConstants("del",         127, adaCharType, asciispec);
    declareCharConstants("exclam",      '!', adaCharType, asciispec);
    declareCharConstants("quotation",   '"', adaCharType, asciispec);
    declareCharConstants("sharp",       '#', adaCharType, asciispec);
    declareCharConstants("dollar",      '$', adaCharType, asciispec);
    declareCharConstants("percent",     '%', adaCharType, asciispec);
    declareCharConstants("ampersand",   '&', adaCharType, asciispec);
    declareCharConstants("colon",       ',', adaCharType, asciispec);
    declareCharConstants("semicolon",   ';', adaCharType, asciispec);
    declareCharConstants("query",       '?', adaCharType, asciispec);
    declareCharConstants("at_sign",     '@', adaCharType, asciispec);
    declareCharConstants("l_bracket",   '[', adaCharType, asciispec);
    declareCharConstants("r_bracket",   ']', adaCharType, asciispec);
    declareCharConstants("back_slash", '\\', adaCharType, asciispec);
    declareCharConstants("circumflex",  '^', adaCharType, asciispec);
    declareCharConstants("underline",   '_', adaCharType, asciispec);
    declareCharConstants("grave",       '`', adaCharType, asciispec);
    declareCharConstants("l_brace",     '{', adaCharType, asciispec);
    declareCharConstants("r_brace",     '}', adaCharType, asciispec);
    declareCharConstants("bar",         '_', adaCharType, asciispec);
    declareCharConstants("tilde",       '~', adaCharType, asciispec);

    cval = 'a'-1;
    declareCharConstants("lc_a", ++cval, adaCharType, asciispec);
    declareCharConstants("lc_b", ++cval, adaCharType, asciispec);
    declareCharConstants("lc_c", ++cval, adaCharType, asciispec);
    declareCharConstants("lc_d", ++cval, adaCharType, asciispec);
    declareCharConstants("lc_e", ++cval, adaCharType, asciispec);
    declareCharConstants("lc_f", ++cval, adaCharType, asciispec);
    declareCharConstants("lc_g", ++cval, adaCharType, asciispec);
    declareCharConstants("lc_h", ++cval, adaCharType, asciispec);
    declareCharConstants("lc_i", ++cval, adaCharType, asciispec);
    declareCharConstants("lc_j", ++cval, adaCharType, asciispec);
    declareCharConstants("lc_k", ++cval, adaCharType, asciispec);
    declareCharConstants("lc_l", ++cval, adaCharType, asciispec);
    declareCharConstants("lc_m", ++cval, adaCharType, asciispec);
    declareCharConstants("lc_n", ++cval, adaCharType, asciispec);
    declareCharConstants("lc_o", ++cval, adaCharType, asciispec);
    declareCharConstants("lc_p", ++cval, adaCharType, asciispec);
    declareCharConstants("lc_q", ++cval, adaCharType, asciispec);
    declareCharConstants("lc_r", ++cval, adaCharType, asciispec);
    declareCharConstants("lc_s", ++cval, adaCharType, asciispec);
    declareCharConstants("lc_t", ++cval, adaCharType, asciispec);
    declareCharConstants("lc_u", ++cval, adaCharType, asciispec);
    declareCharConstants("lc_v", ++cval, adaCharType, asciispec);
    declareCharConstants("lc_w", ++cval, adaCharType, asciispec);
    declareCharConstants("lc_x", ++cval, adaCharType, asciispec);
    declareCharConstants("lc_y", ++cval, adaCharType, asciispec);
    declareCharConstants("lc_z", ++cval, adaCharType, asciispec);
    //ADA_ASSERT(cval == 'z');
  }

  void buildAsciiPkg(SgType& adaCharType, SgAdaPackageSpec& stdspec)
  {
    SgAdaPackageSpecDecl& asciipkg    = declarePackage("Ascii", stdspec);
    SgAdaPackageSpec&     asciispec   = SG_DEREF(asciipkg.get_definition());

    //adaPkgs()["STANDARD.ASCII"]       = &asciipkg;
    //adaPkgs()["ASCII"]                = &asciipkg;
    declareCharConstants(adaCharType, asciispec);
  }

} //end unnamed namespace

void declareEnumItem(SgEnumDeclaration& enumdcl, const std::string& name, int repval)
{
  SgEnumType&         enumty = SG_DEREF(enumdcl.get_type());
  SgExpression&       sginit = SG_DEREF(sb::buildIntVal(repval));
  markCompilerGenerated(sginit);

  SgInitializedName&  sgnode = mkInitializedName(name, enumty, &sginit);

  sgnode.set_scope(enumdcl.get_scope());
  enumdcl.append_enumerator(&sgnode);
  //ADA_ASSERT(sgnode.get_parent() == &enumdcl);
}

void initializePkgStandard(SgGlobal& global)
{
  // make available declarations from the package standard
  // https://www.adaic.org/resources/add_content/standards/05rm/html/RM-A-1.html

  constexpr auto ADAMAXINT = std::numeric_limits<std::int64_t>::max();

  SgAdaPackageSpecDecl& stdpkg  = mkAdaPackageSpecDecl(si::Ada::packageStandardName, global);
  SgAdaPackageSpec&     stdspec = SG_DEREF(stdpkg.get_definition());

  stdpkg.set_scope(&global);

  // \todo reconsider using a true Ada exception representation
  SgType&               exceptionType = SG_DEREF(sb::buildOpaqueType(si::Ada::exceptionName, &stdspec));

  //adaTypes()["EXCEPTION"]           = &exceptionType;

  // boolean enum type
  //AdaIdentifier         boolname{"BOOLEAN"};

  //~ SgType& adaBoolType               = SG_DEREF(sb::buildBoolType());
  SgEnumDeclaration&    boolDecl    = mkEnumDefn("BOOLEAN", stdspec);
  SgType&               adaBoolType = SG_DEREF(boolDecl.get_type());
  //adaTypes()[boolname]              = &adaBoolType;

  declareEnumItem(boolDecl, "False", 0);
  declareEnumItem(boolDecl, "True",  1);

  // \todo reconsider adding a true Ada Duration type
  AdaIdentifier         durationName{si::Ada::durationTypeName};
  SgType&               adaDuration = SG_DEREF(sb::buildOpaqueType(durationName, &stdspec));

  //adaTypes()[durationName]          = &adaDuration;

  // integral types
  SgType&               adaIntType  = mkIntegralType(); // the root integer type in ROSE

  declareIntSubtype<std::int8_t> ("Short_Short_Integer", stdspec);
  declareIntSubtype<std::int16_t>("Short_Integer",       stdspec);
  declareIntSubtype<std::int32_t>("Integer",             stdspec);
  declareIntSubtype<std::int64_t>("Long_Integer",        stdspec);
  declareIntSubtype<std::int64_t>("Long_Long_Integer",   stdspec);

  SgType& adaPositiveType           = SG_DEREF(declareIntSubtype("Positive", 1, ADAMAXINT, stdspec).get_type());
  SgType& adaNaturalType            = SG_DEREF(declareIntSubtype("Natural",  0, ADAMAXINT, stdspec).get_type());

  /*adaTypes()["POSITIVE"]            = &adaPositiveType;
  adaTypes()["NATURAL"]             = &adaNaturalType;*/

  // characters
  // \todo in Ada char, wide_char, and wide_wide_char are enums.
  //       Consider revising the ROSE representation.
  SgType& adaCharType               = SG_DEREF(sb::buildCharType());
  SgType& adaWideCharType           = SG_DEREF(sb::buildChar16Type());
  SgType& adaWideWideCharType       = SG_DEREF(sb::buildChar32Type());

  /*adaTypes()["CHARACTER"]           = &adaCharType;
  adaTypes()["WIDE_CHARACTER"]      = &adaWideCharType;
  adaTypes()["WIDE_WIDE_CHARACTER"] = &adaWideWideCharType;*/

  // from https://en.wikibooks.org/wiki/Ada_Programming/Libraries/Standard/GNAT
  // \todo consider using C++ min/max limits
  static constexpr long double VAL_S_FLOAT  = 3.40282;
  static constexpr int         EXP_S_FLOAT  = 38;
  static constexpr int         DIG_S_FLOAT  = 6;
  static constexpr long double VAL_FLOAT    = 3.40282;
  static constexpr int         EXP_FLOAT    = 38;
  static constexpr int         DIG_FLOAT    = 6;
  static constexpr long double VAL_L_FLOAT  = 1.79769313486232;
  static constexpr int         EXP_L_FLOAT  = 308;
  static constexpr int         DIG_L_FLOAT  = 15;
  static constexpr long double VAL_LL_FLOAT = 1.18973149535723177;
  static constexpr int         EXP_LL_FLOAT = 4932;
  static constexpr int         DIG_LL_FLOAT = 18;

  SgType& adaRealType               = mkRealType();
  declareRealSubtype("Short_Float",     DIG_S_FLOAT,  VAL_S_FLOAT,  EXP_S_FLOAT,  stdspec);
  declareRealSubtype("Float",           DIG_FLOAT,    VAL_FLOAT,    EXP_FLOAT,    stdspec);
  declareRealSubtype("Long_Float",      DIG_L_FLOAT,  VAL_L_FLOAT,  EXP_L_FLOAT,  stdspec);
  declareRealSubtype("Long_Long_Float", DIG_LL_FLOAT, VAL_LL_FLOAT, EXP_LL_FLOAT, stdspec);

  // String types
  SgType& adaStringType             = SG_DEREF(declareStringType("String",           adaPositiveType, adaCharType,         stdspec).get_type());
  SgType& adaWideStringType         = SG_DEREF(declareStringType("Wide_String",      adaPositiveType, adaWideCharType,     stdspec).get_type());
  SgType& adaWideWideStringType     = SG_DEREF(declareStringType("Wide_Wide_String", adaPositiveType, adaWideWideCharType, stdspec).get_type());

  /*adaTypes()["STRING"]              = &adaStringType;
  adaTypes()["WIDE_STRING"]         = &adaWideStringType;
  adaTypes()["WIDE_WIDE_STRING"]    = &adaWideWideStringType;*/

  // Ada standard exceptions
  SgInitializedName& adaConstraintError    = declareException("Constraint_Error", exceptionType, stdspec);
  SgInitializedName& adaProgramError       = declareException("Program_Error",    exceptionType, stdspec);
  SgInitializedName& adaStorageError       = declareException("Storage_Error",    exceptionType, stdspec);
  SgInitializedName& adaTaskingError       = declareException("Tasking_Error",    exceptionType, stdspec);

  // added packages
  //adaPkgs()["STANDARD"]             = &stdpkg;

  // build ASCII package
  buildAsciiPkg(adaCharType, stdspec);

  //
  // build standard functions
  //~ map_t<OperatorKey, std::vector<OperatorDesc> >& opsMap = operatorSupport();
  //auto& opsMap = operatorSupport();

  // bool
  declareOp("=",   adaBoolType, { &adaBoolType, &adaBoolType },    stdspec);
  declareOp("/=",  adaBoolType, { &adaBoolType, &adaBoolType },    stdspec);
  declareOp("<",   adaBoolType, { &adaBoolType, &adaBoolType },    stdspec);
  declareOp("<=",  adaBoolType, { &adaBoolType, &adaBoolType },    stdspec);
  declareOp(">",   adaBoolType, { &adaBoolType, &adaBoolType },    stdspec);
  declareOp(">=",  adaBoolType, { &adaBoolType, &adaBoolType },    stdspec);
  declareOp("and", adaBoolType, { &adaBoolType, &adaBoolType },    stdspec);
  declareOp("or",  adaBoolType, { &adaBoolType, &adaBoolType },    stdspec);
  declareOp("xor", adaBoolType, { &adaBoolType, &adaBoolType },    stdspec);
  declareOp("not", adaBoolType, { &adaBoolType }, /* unary */      stdspec);

  // integer
  declareOp("=",   adaBoolType, { &adaIntType,  &adaIntType },     stdspec);
  declareOp("/=",  adaBoolType, { &adaIntType,  &adaIntType },     stdspec);
  declareOp("<",   adaBoolType, { &adaIntType,  &adaIntType },     stdspec);
  declareOp("<=",  adaBoolType, { &adaIntType,  &adaIntType },     stdspec);
  declareOp(">",   adaBoolType, { &adaIntType,  &adaIntType },     stdspec);
  declareOp(">=",  adaBoolType, { &adaIntType,  &adaIntType },     stdspec);

  declareOp("+",   adaIntType,  { &adaIntType,  &adaIntType },     stdspec);
  declareOp("-",   adaIntType,  { &adaIntType,  &adaIntType },     stdspec);
  declareOp("*",   adaIntType,  { &adaIntType,  &adaIntType },     stdspec);
  declareOp("/",   adaIntType,  { &adaIntType,  &adaIntType },     stdspec);
  declareOp("rem", adaIntType,  { &adaIntType,  &adaIntType },     stdspec);
  declareOp("mod", adaIntType,  { &adaIntType,  &adaIntType },     stdspec);
  declareOp("**",  adaIntType,  { &adaIntType,  &adaNaturalType }, stdspec);

  declareOp("+",   adaIntType,  { &adaIntType }, /* unary */       stdspec);
  declareOp("-",   adaIntType,  { &adaIntType }, /* unary */       stdspec);
  declareOp("abs", adaIntType,  { &adaIntType }, /* unary */       stdspec);

  // float
  declareOp("=",   adaBoolType, { &adaRealType, &adaRealType },    stdspec);
  declareOp("/=",  adaBoolType, { &adaRealType, &adaRealType },    stdspec);
  declareOp("<",   adaBoolType, { &adaRealType, &adaRealType },    stdspec);
  declareOp("<=",  adaBoolType, { &adaRealType, &adaRealType },    stdspec);
  declareOp(">",   adaBoolType, { &adaRealType, &adaRealType },    stdspec);
  declareOp(">=",  adaBoolType, { &adaRealType, &adaRealType },    stdspec);

  declareOp("+",   adaRealType, { &adaRealType, &adaRealType },    stdspec);
  declareOp("-",   adaRealType, { &adaRealType, &adaRealType },    stdspec);
  declareOp("*",   adaRealType, { &adaRealType, &adaRealType },    stdspec);
  declareOp("/",   adaRealType, { &adaRealType, &adaRealType },    stdspec);
  declareOp("**",  adaRealType, { &adaRealType, &adaIntType },     stdspec);

  declareOp("+",   adaRealType, { &adaRealType }, /* unary */      stdspec);
  declareOp("-",   adaRealType, { &adaRealType }, /* unary */      stdspec);
  declareOp("abs", adaRealType, { &adaRealType }, /* unary */      stdspec);

  // mixed float and int
  declareOp("*",   adaRealType, { &adaIntType,  &adaRealType },    stdspec);
  declareOp("*",   adaRealType, { &adaRealType, &adaIntType },     stdspec);
  declareOp("/",   adaRealType, { &adaRealType, &adaIntType },     stdspec);

  // \todo what are built in fixed type operations??


  // operations on strings
  declareOp("=",   adaBoolType,           { &adaStringType,         &adaStringType },         stdspec);
  declareOp("/=",  adaBoolType,           { &adaStringType,         &adaStringType },         stdspec);
  declareOp("<",   adaBoolType,           { &adaStringType,         &adaStringType },         stdspec);
  declareOp("<=",  adaBoolType,           { &adaStringType,         &adaStringType },         stdspec);
  declareOp(">",   adaBoolType,           { &adaStringType,         &adaStringType },         stdspec);
  declareOp(">=",  adaBoolType,           { &adaStringType,         &adaStringType },         stdspec);
  declareOp("&",   adaStringType,         { &adaStringType,         &adaStringType },         stdspec);
  declareOp("&",   adaStringType,         { &adaCharType,           &adaStringType },         stdspec);
  declareOp("&",   adaStringType,         { &adaStringType,         &adaCharType   },         stdspec);
  declareOp("&",   adaStringType,         { &adaCharType,           &adaCharType   },         stdspec);

  declareOp("=",   adaBoolType,           { &adaWideStringType,     &adaWideStringType },     stdspec);
  declareOp("/=",  adaBoolType,           { &adaWideStringType,     &adaWideStringType },     stdspec);
  declareOp("<",   adaBoolType,           { &adaWideStringType,     &adaWideStringType },     stdspec);
  declareOp("<=",  adaBoolType,           { &adaWideStringType,     &adaWideStringType },     stdspec);
  declareOp(">",   adaBoolType,           { &adaWideStringType,     &adaWideStringType },     stdspec);
  declareOp(">=",  adaBoolType,           { &adaWideStringType,     &adaWideStringType },     stdspec);
  declareOp("&",   adaWideStringType,     { &adaWideStringType,     &adaWideStringType },     stdspec);
  declareOp("&",   adaWideStringType,     { &adaWideCharType,       &adaWideStringType },     stdspec);
  declareOp("&",   adaWideStringType,     { &adaWideStringType,     &adaWideCharType   },     stdspec);
  declareOp("&",   adaWideStringType,     { &adaWideCharType,       &adaWideCharType   },     stdspec);

  declareOp("=",   adaBoolType,           { &adaWideWideStringType, &adaWideWideStringType }, stdspec);
  declareOp("/=",  adaBoolType,           { &adaWideWideStringType, &adaWideWideStringType }, stdspec);
  declareOp("<",   adaBoolType,           { &adaWideWideStringType, &adaWideWideStringType }, stdspec);
  declareOp("<=",  adaBoolType,           { &adaWideWideStringType, &adaWideWideStringType }, stdspec);
  declareOp(">",   adaBoolType,           { &adaWideWideStringType, &adaWideWideStringType }, stdspec);
  declareOp(">=",  adaBoolType,           { &adaWideWideStringType, &adaWideWideStringType }, stdspec);
  declareOp("&",   adaWideWideStringType, { &adaWideWideStringType, &adaWideWideStringType }, stdspec);
  declareOp("&",   adaWideWideStringType, { &adaWideWideCharType,   &adaWideWideStringType }, stdspec);
  declareOp("&",   adaWideWideStringType, { &adaWideWideStringType, &adaWideWideCharType   }, stdspec);
  declareOp("&",   adaWideWideStringType, { &adaWideWideCharType,   &adaWideWideCharType   }, stdspec);

  // \todo operations on Duration

  // access types
  SgType& adaAccessType = SG_DEREF(sb::buildNullptrType());

  declareOp("=",   adaBoolType, { &adaAccessType, &adaAccessType   }, stdspec);
  declareOp("/=",  adaBoolType, { &adaAccessType, &adaAccessType   }, stdspec);

  // set the standard package in the SageInterface::ada namespace
  // \todo this should go away for a cleaner interface
  si::Ada::stdpkg = &stdpkg;
}

} //End Libadalang_ROSE_Translation namepsace
