#include "sage3basic.h"

#include <limits>
#include <cmath>

#include <boost/algorithm/string.hpp>

#include "LibadalangType.h"

#include "sageGeneric.h"
#include "sageInterfaceAda.h"

#include "Libadalang_to_ROSE.h"
#include "LibadalangExpression.h"
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
  struct MakeTyperef : sg::DispatchHandler<SgType*>
  {
      using base = sg::DispatchHandler<SgType*>;

      MakeTyperef(ada_base_entity* lal_elem, AstContext astctx)
      : base(), el(lal_elem), ctx(astctx)
      {}

      // checks whether this is a discriminated declaration and sets the type accordingly
      void handleDiscrDecl(SgDeclarationStatement& n, SgType* declaredType)
      {
        if (SgAdaDiscriminatedTypeDecl* discrDcl = si::Ada::getAdaDiscriminatedTypeDecl(n))
          declaredType = discrDcl->get_type();

        set(declaredType);
      }

      void set(SgType* ty)                       { res = ty; }

      // error handler
      void handle(SgNode& n)                     { SG_UNEXPECTED_NODE(n); }

      // just use the type
      void handle(SgType& n)                     { set(&n); }
      void handle(SgAdaDiscriminatedTypeDecl& n) { set(n.get_type()); }

      // undecorated declarations

      // possibly decorated with an SgAdaDiscriminatedTypeDecl
      // \{
      void handle(SgAdaFormalTypeDecl& n)        { handleDiscrDecl(n, n.get_type()); }
      void handle(SgClassDeclaration& n)         { handleDiscrDecl(n, n.get_type()); }
      void handle(SgAdaTaskTypeDecl& n)          { handleDiscrDecl(n, n.get_type()); }
      void handle(SgAdaProtectedTypeDecl& n)     { handleDiscrDecl(n, n.get_type()); }
      void handle(SgEnumDeclaration& n)          { handleDiscrDecl(n, n.get_type()); }
      void handle(SgTypedefDeclaration& n)       { handleDiscrDecl(n, n.get_type()); }
      // \}

      // others
      void handle(SgInitializedName& n)          { set(&mkExprAsType(SG_DEREF(sb::buildVarRefExp(&n)))); }

      void handle(SgAdaAttributeExp& n)
      {
        attachSourceLocation(n, el, ctx); // \todo why is this not set where the node is made?
        set(&mkExprAsType(n));
      }


    private:
      ada_base_entity* el;
      AstContext      ctx;
  };

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
  declareRealSubtype(const std::string& name, int ndigits, int int_base, int int_exp, SgAdaPackageSpec& scope)
  {
    long double base = 0;
    int exp = 0;

    //TODO Is there a fast way to convert?
    // from https://en.wikibooks.org/wiki/Ada_Programming/Libraries/Standard/GNAT
    if(int_base == 16 && int_exp == 32){
      base = 3.40282;
      exp = 38;
    } else if(int_base == 16 && int_exp == 256){
      base = 1.79769313486232;
      exp = 308;
    } else if(int_base == 16 && int_exp == 4096){
      base = 1.18973149535723177;
      exp = 4932;
    } else {
      logError() << "Unknown base and exp in declareRealSubtype: " << base << ", " << exp << ".\n";
    }

    return declareRealSubtype(name, ndigits, base, exp, scope);
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
                  map_t<OperatorKey, std::vector<OperatorDesc> >& fns,
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

    fns[OperatorKey{&scope, name}].emplace_back(&fndcl, OperatorDesc::DECLARED_IN_STANDARD);
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

  SgType&
  excludeNullIf(SgType& ty, bool exclNull, AstContext)
  {
    return exclNull ? mkNotNullType(ty) : ty;
  }

  SgType&
  getDefinitionType(ada_base_entity* lal_def, AstContext ctx, bool forceSubtype)
  {
    //ADA_ASSERT(elem.Element_Kind == A_Definition);

    //Get the kind of this node
    ada_node_kind_enum kind;
    kind = ada_node_kind(lal_def);

    SgType*            res = nullptr;

    switch(kind)
    {
      case ada_subtype_indication:
        {
          logKind("ada_subtype_indication", kind);

          res = &getDeclType(lal_def, ctx);

          // \todo if there is no subtype constraint, shall we produce
          //       a subtype w/ NoConstraint, or leave the original type?
          ada_base_entity subtype_constraint;
          ada_subtype_indication_f_constraint(lal_def, &subtype_constraint);
          if(forceSubtype || !ada_node_is_null(&subtype_constraint))
          {
            //~ SgAdaTypeConstraint& range = getConstraintID_opt(subtype.Subtype_Constraint, ctx);
            SgAdaTypeConstraint& range = getConstraint(&subtype_constraint, ctx);

            res = &mkAdaSubtype(SG_DEREF(res), range);
          } //TODO How do we get the constraint status here? 

          ada_base_entity has_not_null;
          ada_subtype_indication_f_has_not_null(lal_def, &has_not_null);

          ada_node_kind_enum null_kind = ada_node_kind(&has_not_null);
          bool not_null_present = (null_kind == ada_not_null_present);

          res = &excludeNullIf(SG_DEREF(res), not_null_present, ctx);
          break;
        }
      default:
        logWarn() << "Unhandled type definition: " << kind << std::endl;
        res = &mkTypeUnknown();
        //ADA_ASSERT(!FAIL_ON_ERROR(ctx));
    }

    return SG_DEREF(res);
  }

//Function to hash a unique int from a node using the node's kind and location.
//The kind and location can be provided, but if not they will be determined in the function
int hash_node(ada_base_entity *node, int kind, std::string full_sloc){
    //Get the kind/sloc if they weren't provided
    if(kind == -1){
        kind = ada_node_kind(node);
    }
    if(full_sloc == ""){
        full_sloc = dot_ada_full_sloc(node);
    }

    std::string word_to_hash = full_sloc + std::to_string(kind);

    //Generate the hash
    int seed = 131; 
    unsigned int hash = 0;
    for(int i = 0; i < word_to_hash.length(); i++){
        hash = (hash * seed) + word_to_hash[i];
    }
    return hash;
}

  SgNode&
  getExprType(ada_base_entity* lal_expr, AstContext ctx)
  {
    //Get the kind of this node
    ada_node_kind_enum kind;
    kind = ada_node_kind(lal_expr);

    static constexpr bool findFirstOf = false;

    SgNode* res = nullptr;

    switch(kind)
    {
      case ada_type_decl:
        {
          //logKind("An_Identifier");

          int hash = hash_node(lal_expr);
          logInfo() << "Searching for hash: " << hash;

          // is it a type?
          findFirstOf
          || (res = findFirst(adaTypes(),         hash))
          || (res = findFirst(libadalangTypes(),  hash))
          ;

          if(res != nullptr){
              logInfo() << ", found.\n";
          } else {
              logInfo() << ", couldn't find.\n";
          }

          break;
        }
      default:
        logWarn() << "Unknown type expression: " << kind << std::endl;
        //ADA_ASSERT(!FAIL_ON_ERROR(ctx));
        res = &mkTypeUnknown();
    }

    return SG_DEREF(res);
  }

  SgType&
  getDeclType(ada_base_entity* lal_id, AstContext ctx)
  {
    //Get the kind of this node
    ada_node_kind_enum kind;
    kind = ada_node_kind(lal_id);

    if(kind == ada_subtype_indication){
        //Get the type this references
        ada_base_entity lal_declaration;
        ada_type_expr_p_designated_type_decl(lal_id, &lal_declaration);
        SgNode& basenode = getExprType(&lal_declaration, ctx);
        SgType* res      = sg::dispatch(MakeTyperef(lal_id, ctx), &basenode);

        return SG_DEREF(res);
    }

    /*ADA_ASSERT(elem.Element_Kind == A_Definition);
    Definition_Struct& def = elem.The_Union.Definition;

    if (def.Definition_Kind == An_Access_Definition)
      return getAnonymousAccessType(def, ctx);*/

    logError() << "getDeclType: unhandled definition kind: " << kind
               << std::endl;
    return mkTypeUnknown();
  }

  TypeData
  getTypeFoundation(const std::string& name, ada_base_entity* lal_def, AstContext ctx)
  {
    //Get the kind of this node
    ada_node_kind_enum kind = ada_node_kind(lal_def);

    logKind("A_Type_Definition", kind);

    TypeData                res{lal_def, nullptr, false, false, false};

    /* unused fields:
       Definition_Struct
         bool                           Has_Null_Exclusion;
    */

    switch(kind)
    {
      case ada_derived_type_def:              // 3.4(2)     -> Trait_Kinds
        {
          logKind("ada_derived_type_def", kind);
          /*
             unused fields: (derivedTypeDef)
                Declaration_List     Implicit_Inherited_Declarations;
          */
          ada_base_entity subtype_indication;
          ada_derived_type_def_f_subtype_indication(lal_def, &subtype_indication);

          SgType& basetype = getDefinitionType(&subtype_indication, ctx);

          //~ if (isSgEnumType(si::Ada::base)
          res.sageNode(mkAdaDerivedType(basetype));
          break;
        }

      default:
        {
          logWarn() << "unhandled type kind " << kind << std::endl;
          //ADA_ASSERT(!FAIL_ON_ERROR(ctx));
          res.sageNode(mkTypeUnknown());
        }
    }

    //ADA_ASSERT(&res.sageNode());
    return res;
  }

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

//Struct that holds some hash keys for a few types that we will need to access multiple times
struct ada_type_hashes{
  int bool_hash;
  int char_hash;
  int wide_char_hash;
  int wide_wide_char_hash;
  int positive_hash;
  int natural_hash;
  int string_hash;
  int wide_string_hash;
  int wide_wide_string_hash;
};

template<class MapT>
void handleStdDecl(MapT& m, ada_base_entity* lal_decl, SgAdaPackageSpec& stdspec, ada_type_hashes& type_hashes)
{
  //Get the name of the type for logging purposes
  ada_text_type fully_qualified_name;
  ada_basic_decl_p_canonical_fully_qualified_name(lal_decl, &fully_qualified_name);
  AdaIdentifier canonical_fully_qualified_name (dot_ada_text_type_to_string(fully_qualified_name));
  //Get the name of this type without the "STANDARD."
  std::string type_name = canonical_fully_qualified_name.substr(9, canonical_fully_qualified_name.length()-1);

  //Get the hash of this decl
  int hash = hash_node(lal_decl);

  //logInfo() << "handleStdDecl called for " << canonical_fully_qualified_name << std::endl;

  if(canonical_fully_qualified_name.find("BOOLEAN") != std::string::npos){
    // boolean enum type
    SgEnumDeclaration&    boolDecl    = mkEnumDefn("BOOLEAN", stdspec);
    SgType&               adaBoolType = SG_DEREF(boolDecl.get_type());
    m[hash]               = &adaBoolType;

    declareEnumItem(boolDecl, "False", 0);
    declareEnumItem(boolDecl, "True",  1);
    //Add this hash to the type_hashes struct
    type_hashes.bool_hash = hash;
  } else if(canonical_fully_qualified_name.find("INTEGER") != std::string::npos){
    //All Ada int types have a range of -(2**<exp>)..(2**<exp>)-1, so find the exp
    ada_base_entity lal_exponent;
    ada_type_decl_f_type_def(lal_decl, &lal_exponent);
    ada_signed_int_type_def_f_range(&lal_exponent, &lal_exponent);
    ada_range_spec_f_range(&lal_exponent, &lal_exponent);
    ada_bin_op_f_left(&lal_exponent, &lal_exponent);
    ada_un_op_f_expr(&lal_exponent, &lal_exponent);
    ada_paren_expr_f_expr(&lal_exponent, &lal_exponent);
    ada_bin_op_f_right(&lal_exponent, &lal_exponent);

    ada_symbol_type    p_canonical_text;
    ada_text           ada_canonical_text;
    ada_single_tok_node_p_canonical_text(&lal_exponent, &p_canonical_text);
    ada_symbol_text(&p_canonical_text, &ada_canonical_text);
    const std::string  name = ada_text_to_locale_string(&ada_canonical_text);
    int exponent = std::stoi(name);
    int lower_bound = -std::pow(2, exponent);
    int upper_bound = std::pow(2, exponent) - 1;
    //Call declareIntSubtype
    SgType& adaIntType = SG_DEREF(declareIntSubtype(type_name, lower_bound, upper_bound, stdspec).get_type());
    m[hash] = &adaIntType;
  } else if(canonical_fully_qualified_name.find("FLOAT") != std::string::npos){
    //For floats, we need to get the number of digits, and then the value and exponent for the range
    ada_base_entity float_def;
    ada_type_decl_f_type_def(lal_decl, &float_def);
    ada_base_entity lal_num_digits, lal_range;
    ada_floating_point_def_f_num_digits(&float_def, &lal_num_digits);
    ada_symbol_type    p_canonical_text;
    ada_text           ada_canonical_text;
    ada_single_tok_node_p_canonical_text(&lal_num_digits, &p_canonical_text);
    ada_symbol_text(&p_canonical_text, &ada_canonical_text);
    std::string name = ada_text_to_locale_string(&ada_canonical_text);
    int num_digits = std::stoi(name);
    
    ada_floating_point_def_f_range(&float_def, &lal_range);
    ada_range_spec_f_range(&lal_range, &lal_range);
    ada_bin_op_f_right(&lal_range, &lal_range);
    ada_single_tok_node_p_canonical_text(&lal_range, &p_canonical_text);
    ada_symbol_text(&p_canonical_text, &ada_canonical_text);
    name = ada_text_to_locale_string(&ada_canonical_text);
    int base = stoi(name.substr(0, name.find("#")));
    int exponent = stoi(name.substr(name.find("#e+")+3, name.length()-1));
    //Call declareRealSubtype
    m[hash] = declareRealSubtype(type_name, num_digits, base, exponent, stdspec).get_type();
  } else if(canonical_fully_qualified_name.find("CHARACTER") != std::string::npos){
    //   in Ada character is of enum type.
    //   Currently, the enum is not filled with members, but they are rather logically
    //   injected via the adaParent_Type set to char char16 or char32.
    //TODO Is there a way to make this more general?
    SgEnumDeclaration& adaCharDecl = mkEnumDecl(type_name, stdspec);
    if(type_name == "CHARACTER"){
      adaCharDecl.set_adaParentType(sb::buildCharType());
      type_hashes.char_hash = hash;
    } else if(type_name == "WIDE_CHARACTER"){
      adaCharDecl.set_adaParentType(sb::buildChar16Type());
      type_hashes.wide_char_hash = hash;
    } else if(type_name == "WIDE_WIDE_CHARACTER"){
      adaCharDecl.set_adaParentType(sb::buildChar32Type());
      type_hashes.wide_wide_char_hash = hash;
    } else {
      logError() << "No plan for char type " << type_name << std::endl;
      return;
    }
    SgType& adaCharType = SG_DEREF(adaCharDecl.get_type());
    m[hash] = &adaCharType;
  } else if(canonical_fully_qualified_name.find("STRING") != std::string::npos){
    SgType* adaCharTypePtr;
    SgType& adaPositiveType = SG_DEREF(m.at(type_hashes.positive_hash));
    if(type_name == "STRING"){
      adaCharTypePtr = m.at(type_hashes.char_hash);
      type_hashes.string_hash = hash;
    } else if(type_name == "WIDE_STRING"){
      adaCharTypePtr = m.at(type_hashes.wide_char_hash);
      type_hashes.wide_string_hash = hash;
    } else if(type_name == "WIDE_WIDE_STRING"){
      adaCharTypePtr = m.at(type_hashes.wide_wide_char_hash);
      type_hashes.wide_wide_string_hash = hash;
    } else {
      logError() << "No plan for string type " << type_name << std::endl;
      return;
    }
    SgType& adaCharType = SG_DEREF(adaCharTypePtr);
    SgType& adaStringType = SG_DEREF(declareStringType(type_name, adaPositiveType, adaCharType, stdspec).get_type());
    m[hash] = &adaStringType;
  } else if(canonical_fully_qualified_name.find("DURATION") != std::string::npos){
    // \todo reconsider adding a true Ada Duration type
    SgType& adaDuration = SG_DEREF(sb::buildOpaqueType(type_name, &stdspec));
    m[hash]             = &adaDuration;
  } else {
    //TODO Universal int/real?
  }
}

template<class MapT>
void handleStdSubType(MapT& m, ada_base_entity* lal_decl, SgAdaPackageSpec& stdspec, ada_type_hashes& type_hashes)
{
  //Get the name of the type for logging purposes
  ada_text_type fully_qualified_name;
  ada_basic_decl_p_canonical_fully_qualified_name(lal_decl, &fully_qualified_name);
  AdaIdentifier canonical_fully_qualified_name (dot_ada_text_type_to_string(fully_qualified_name));
  //Get the name of this type without the "STANDARD."
  std::string type_name = canonical_fully_qualified_name.substr(9, canonical_fully_qualified_name.length()-1);

  //Get the hash of this decl
  int hash = hash_node(lal_decl);

  //logInfo() << "handleStdSubType called for " << canonical_fully_qualified_name << std::endl;

  //Get the Standard type this is a subtype of
  ada_base_entity subtype_indication, subtype_identifier;
  ada_subtype_decl_f_subtype(lal_decl, &subtype_indication);
  ada_subtype_indication_f_name(&subtype_indication, &subtype_identifier);
  ada_symbol_type    p_canonical_text;
  ada_text           ada_canonical_text;
  ada_single_tok_node_p_canonical_text(&subtype_identifier, &p_canonical_text);
  ada_symbol_text(&p_canonical_text, &ada_canonical_text);
  AdaIdentifier supertype_name(ada_text_to_locale_string(&ada_canonical_text));

  if(supertype_name.find("INTEGER") != std::string::npos){
    //Determine the range of the subtype
    //For now, we only have to worry about positive & natural
    //We can assume that they will always start from 1 & 0, respectively, so we just need to find the upper bound
    int lower_bound = -1;
    if(type_name == "NATURAL"){
      lower_bound = 0;
      type_hashes.natural_hash = hash;
    } else if(type_name == "POSITIVE"){
      lower_bound = 1;
      type_hashes.positive_hash = hash;
    }
    //The upper bound will be in the format (2**<exp>)-1, so find exp
    ada_base_entity lal_exponent;
    ada_subtype_indication_f_constraint(&subtype_indication, &lal_exponent);
    ada_range_constraint_f_range(&lal_exponent, &lal_exponent);
    ada_range_spec_f_range(&lal_exponent, &lal_exponent);
    ada_bin_op_f_right(&lal_exponent, &lal_exponent);
    ada_un_op_f_expr(&lal_exponent, &lal_exponent);
    ada_paren_expr_f_expr(&lal_exponent, &lal_exponent);
    ada_bin_op_f_left(&lal_exponent, &lal_exponent);
    ada_bin_op_f_right(&lal_exponent, &lal_exponent);

    ada_symbol_type    p_canonical_text;
    ada_text           ada_canonical_text;
    ada_single_tok_node_p_canonical_text(&lal_exponent, &p_canonical_text);
    ada_symbol_text(&p_canonical_text, &ada_canonical_text);
    const std::string  name = ada_text_to_locale_string(&ada_canonical_text);
    int exponent = std::stoi(name);
    int upper_bound = std::pow(2, exponent)-1;
    //Now make the type
    SgType& adaIntegerSubType         = SG_DEREF(declareIntSubtype(type_name, lower_bound, upper_bound, stdspec).get_type());
    m[hash]                           = &adaIntegerSubType;
  } else {
    logError() << "The Standard Package has a non-integer subtype.\n";
  }

}

void initializePkgStandard(SgGlobal& global, ada_base_entity* lal_root)
{
  logInfo() << "In initializePkgStandard.\n";
  // make available declarations from the package standard
  // https://www.adaic.org/resources/add_content/standards/05rm/html/RM-A-1.html

  constexpr auto ADAMAXINT = std::numeric_limits<std::int64_t>::max();

  SgAdaPackageSpecDecl& stdpkg  = mkAdaPackageSpecDecl(si::Ada::packageStandardName, global);
  SgAdaPackageSpec&     stdspec = SG_DEREF(stdpkg.get_definition());

  stdpkg.set_scope(&global);

  //Get the lal representation of the standard package
  ada_analysis_unit std_unit;
  ada_ada_node_p_standard_unit(lal_root, &std_unit);

  ada_base_entity std_root;
  ada_unit_root(std_unit, &std_root);

  //Go to the list of declarations
  ada_base_entity decl_list;
  ada_compilation_unit_f_body(&std_root, &decl_list);
  ada_library_item_f_item(&decl_list, &decl_list);
  ada_base_package_decl_f_public_part(&decl_list, &decl_list);
  ada_declarative_part_f_decls(&decl_list, &decl_list);

  //This struct will hold a few useful keys for the adaTypes map
  ada_type_hashes type_hashes;

  //For each decl, call a function to add it to std based on its type
  //Get the children
  int count = ada_node_children_count(&decl_list);
  for (int i = 0; i < count; ++i)
  {
    ada_base_entity lal_decl;

    if (ada_node_child(&decl_list, i, &lal_decl) == 0){
      logError() << "Error while getting a decl in initializePkgStandard.\n";
    }

    if(!ada_node_is_null(&lal_decl)){
      ada_node_kind_enum decl_kind = ada_node_kind(&lal_decl);
      switch(decl_kind)
      {
        case ada_type_decl:
        {
          handleStdDecl(adaTypes(), &lal_decl, stdspec, type_hashes);
          break;
        }
        case ada_subtype_decl:
        {
          handleStdSubType(adaTypes(), &lal_decl, stdspec, type_hashes);
          break;
        }
        case ada_exception_decl: //TODO
        case ada_attribute_def_clause: //TODO This is about duration?
        case ada_package_decl: //TODO ascii
        case ada_pragma_node:
          break;
        default:
          logInfo() << "Unhandled decl kind " << decl_kind << " in initializePkgStandard.\n";
      }
    }
  }

  // \todo reconsider using a true Ada exception representation
  SgType&               exceptionType = SG_DEREF(sb::buildOpaqueType(si::Ada::exceptionName, &stdspec));

  //adaTypes()["EXCEPTION"]           = &exceptionType;

  SgType&               adaBoolType = SG_DEREF(adaTypes().at(type_hashes.bool_hash));

  // integral types
  SgType&               adaIntType  = mkIntegralType(); // the root integer type in ROSE

  SgType& adaPositiveType           = SG_DEREF(adaTypes().at(type_hashes.natural_hash));
  adaTypes()[-5]                    = &adaPositiveType; //TODO This is here so other functions can get positive. Find a better way?
  SgType& adaNaturalType            = SG_DEREF(adaTypes().at(type_hashes.natural_hash));

  SgType& adaRealType               = mkRealType();
  
  //characters
  SgType& adaCharType               = SG_DEREF(adaTypes().at(type_hashes.char_hash));
  SgType& adaWideCharType           = SG_DEREF(adaTypes().at(type_hashes.wide_char_hash));
  SgType& adaWideWideCharType       = SG_DEREF(adaTypes().at(type_hashes.wide_wide_char_hash));

  // String types
  SgType& adaStringType             = SG_DEREF(adaTypes().at(type_hashes.string_hash));
  SgType& adaWideStringType         = SG_DEREF(adaTypes().at(type_hashes.wide_string_hash));
  SgType& adaWideWideStringType     = SG_DEREF(adaTypes().at(type_hashes.wide_wide_string_hash));

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
  auto& opsMap = operatorSupport();

  // bool
  declareOp(opsMap, "=",   adaBoolType, { &adaBoolType, &adaBoolType },    stdspec);
  declareOp(opsMap, "/=",  adaBoolType, { &adaBoolType, &adaBoolType },    stdspec);
  declareOp(opsMap, "<",   adaBoolType, { &adaBoolType, &adaBoolType },    stdspec);
  declareOp(opsMap, "<=",  adaBoolType, { &adaBoolType, &adaBoolType },    stdspec);
  declareOp(opsMap, ">",   adaBoolType, { &adaBoolType, &adaBoolType },    stdspec);
  declareOp(opsMap, ">=",  adaBoolType, { &adaBoolType, &adaBoolType },    stdspec);
  declareOp(opsMap, "and", adaBoolType, { &adaBoolType, &adaBoolType },    stdspec);
  declareOp(opsMap, "or",  adaBoolType, { &adaBoolType, &adaBoolType },    stdspec);
  declareOp(opsMap, "xor", adaBoolType, { &adaBoolType, &adaBoolType },    stdspec);
  declareOp(opsMap, "not", adaBoolType, { &adaBoolType }, /* unary */      stdspec);

  // integer
  declareOp(opsMap, "=",   adaBoolType, { &adaIntType,  &adaIntType },     stdspec);
  declareOp(opsMap, "/=",  adaBoolType, { &adaIntType,  &adaIntType },     stdspec);
  declareOp(opsMap, "<",   adaBoolType, { &adaIntType,  &adaIntType },     stdspec);
  declareOp(opsMap, "<=",  adaBoolType, { &adaIntType,  &adaIntType },     stdspec);
  declareOp(opsMap, ">",   adaBoolType, { &adaIntType,  &adaIntType },     stdspec);
  declareOp(opsMap, ">=",  adaBoolType, { &adaIntType,  &adaIntType },     stdspec);

  declareOp(opsMap, "+",   adaIntType,  { &adaIntType,  &adaIntType },     stdspec);
  declareOp(opsMap, "-",   adaIntType,  { &adaIntType,  &adaIntType },     stdspec);
  declareOp(opsMap, "*",   adaIntType,  { &adaIntType,  &adaIntType },     stdspec);
  declareOp(opsMap, "/",   adaIntType,  { &adaIntType,  &adaIntType },     stdspec);
  declareOp(opsMap, "rem", adaIntType,  { &adaIntType,  &adaIntType },     stdspec);
  declareOp(opsMap, "mod", adaIntType,  { &adaIntType,  &adaIntType },     stdspec);
  declareOp(opsMap, "**",  adaIntType,  { &adaIntType,  &adaNaturalType }, stdspec);

  declareOp(opsMap, "+",   adaIntType,  { &adaIntType }, /* unary */       stdspec);
  declareOp(opsMap, "-",   adaIntType,  { &adaIntType }, /* unary */       stdspec);
  declareOp(opsMap, "abs", adaIntType,  { &adaIntType }, /* unary */       stdspec);

  // float
  declareOp(opsMap, "=",   adaBoolType, { &adaRealType, &adaRealType },    stdspec);
  declareOp(opsMap, "/=",  adaBoolType, { &adaRealType, &adaRealType },    stdspec);
  declareOp(opsMap, "<",   adaBoolType, { &adaRealType, &adaRealType },    stdspec);
  declareOp(opsMap, "<=",  adaBoolType, { &adaRealType, &adaRealType },    stdspec);
  declareOp(opsMap, ">",   adaBoolType, { &adaRealType, &adaRealType },    stdspec);
  declareOp(opsMap, ">=",  adaBoolType, { &adaRealType, &adaRealType },    stdspec);

  declareOp(opsMap, "+",   adaRealType, { &adaRealType, &adaRealType },    stdspec);
  declareOp(opsMap, "-",   adaRealType, { &adaRealType, &adaRealType },    stdspec);
  declareOp(opsMap, "*",   adaRealType, { &adaRealType, &adaRealType },    stdspec);
  declareOp(opsMap, "/",   adaRealType, { &adaRealType, &adaRealType },    stdspec);
  declareOp(opsMap, "**",  adaRealType, { &adaRealType, &adaIntType },     stdspec);

  declareOp(opsMap, "+",   adaRealType, { &adaRealType }, /* unary */      stdspec);
  declareOp(opsMap, "-",   adaRealType, { &adaRealType }, /* unary */      stdspec);
  declareOp(opsMap, "abs", adaRealType, { &adaRealType }, /* unary */      stdspec);

  // mixed float and int
  declareOp(opsMap, "*",   adaRealType, { &adaIntType,  &adaRealType },    stdspec);
  declareOp(opsMap, "*",   adaRealType, { &adaRealType, &adaIntType },     stdspec);
  declareOp(opsMap, "/",   adaRealType, { &adaRealType, &adaIntType },     stdspec);

  // \todo what are built in fixed type operations??


  // operations on strings
  declareOp(opsMap, "=",   adaBoolType,           { &adaStringType,         &adaStringType },         stdspec);
  declareOp(opsMap, "/=",  adaBoolType,           { &adaStringType,         &adaStringType },         stdspec);
  declareOp(opsMap, "<",   adaBoolType,           { &adaStringType,         &adaStringType },         stdspec);
  declareOp(opsMap, "<=",  adaBoolType,           { &adaStringType,         &adaStringType },         stdspec);
  declareOp(opsMap, ">",   adaBoolType,           { &adaStringType,         &adaStringType },         stdspec);
  declareOp(opsMap, ">=",  adaBoolType,           { &adaStringType,         &adaStringType },         stdspec);
  declareOp(opsMap, "&",   adaStringType,         { &adaStringType,         &adaStringType },         stdspec);
  declareOp(opsMap, "&",   adaStringType,         { &adaCharType,           &adaStringType },         stdspec);
  declareOp(opsMap, "&",   adaStringType,         { &adaStringType,         &adaCharType   },         stdspec);
  declareOp(opsMap, "&",   adaStringType,         { &adaCharType,           &adaCharType   },         stdspec);

  declareOp(opsMap, "=",   adaBoolType,           { &adaWideStringType,     &adaWideStringType },     stdspec);
  declareOp(opsMap, "/=",  adaBoolType,           { &adaWideStringType,     &adaWideStringType },     stdspec);
  declareOp(opsMap, "<",   adaBoolType,           { &adaWideStringType,     &adaWideStringType },     stdspec);
  declareOp(opsMap, "<=",  adaBoolType,           { &adaWideStringType,     &adaWideStringType },     stdspec);
  declareOp(opsMap, ">",   adaBoolType,           { &adaWideStringType,     &adaWideStringType },     stdspec);
  declareOp(opsMap, ">=",  adaBoolType,           { &adaWideStringType,     &adaWideStringType },     stdspec);
  declareOp(opsMap, "&",   adaWideStringType,     { &adaWideStringType,     &adaWideStringType },     stdspec);
  declareOp(opsMap, "&",   adaWideStringType,     { &adaWideCharType,       &adaWideStringType },     stdspec);
  declareOp(opsMap, "&",   adaWideStringType,     { &adaWideStringType,     &adaWideCharType   },     stdspec);
  declareOp(opsMap, "&",   adaWideStringType,     { &adaWideCharType,       &adaWideCharType   },     stdspec);

  declareOp(opsMap, "=",   adaBoolType,           { &adaWideWideStringType, &adaWideWideStringType }, stdspec);
  declareOp(opsMap, "/=",  adaBoolType,           { &adaWideWideStringType, &adaWideWideStringType }, stdspec);
  declareOp(opsMap, "<",   adaBoolType,           { &adaWideWideStringType, &adaWideWideStringType }, stdspec);
  declareOp(opsMap, "<=",  adaBoolType,           { &adaWideWideStringType, &adaWideWideStringType }, stdspec);
  declareOp(opsMap, ">",   adaBoolType,           { &adaWideWideStringType, &adaWideWideStringType }, stdspec);
  declareOp(opsMap, ">=",  adaBoolType,           { &adaWideWideStringType, &adaWideWideStringType }, stdspec);
  declareOp(opsMap, "&",   adaWideWideStringType, { &adaWideWideStringType, &adaWideWideStringType }, stdspec);
  declareOp(opsMap, "&",   adaWideWideStringType, { &adaWideWideCharType,   &adaWideWideStringType }, stdspec);
  declareOp(opsMap, "&",   adaWideWideStringType, { &adaWideWideStringType, &adaWideWideCharType   }, stdspec);
  declareOp(opsMap, "&",   adaWideWideStringType, { &adaWideWideCharType,   &adaWideWideCharType   }, stdspec);

  // \todo operations on Duration

  // access types
  SgType& adaAccessType = SG_DEREF(sb::buildNullptrType());

  declareOp(opsMap, "=",   adaBoolType, { &adaAccessType, &adaAccessType   }, stdspec);
  declareOp(opsMap, "/=",  adaBoolType, { &adaAccessType, &adaAccessType   }, stdspec);

  // set the standard package in the SageInterface::ada namespace
  // \todo this should go away for a cleaner interface
  si::Ada::stdpkg = &stdpkg;
}

SgAdaTypeConstraint&
getConstraint(ada_base_entity* lal_constraint, AstContext ctx)
{
  if(lal_constraint == nullptr || ada_node_is_null(lal_constraint)){
    return mkAdaNullConstraint();
  }

  SgAdaTypeConstraint*  res = nullptr;

  ada_node_kind_enum kind = ada_node_kind(lal_constraint);
  logKind("A_Constraint", kind);

  switch(kind)
  {
    case ada_range_constraint:             // 3.2.2: 3.5(3)
      {
        logKind("ada_range_constraint", kind);

        //Get the lower/upper bounds of the range
        ada_base_entity lal_range_op, lal_lower, lal_upper;
        ada_range_constraint_f_range(lal_constraint, &lal_range_op);
        ada_range_spec_f_range(&lal_range_op, &lal_range_op);
        ada_bin_op_f_left(&lal_range_op, &lal_lower);
        ada_bin_op_f_right(&lal_range_op, &lal_upper);
        SgExpression& lb       = getExpr(&lal_lower, ctx);
        SgExpression& ub       = getExpr(&lal_upper, ctx);
        SgRangeExp&   rangeExp = mkRangeExp(lb, ub);

        res = &mkAdaRangeConstraint(rangeExp);
        break;
      }

    default:
      logError() << "Unhandled constraint: " << kind << std::endl;
      ROSE_ABORT();
  }

  attachSourceLocation(SG_DEREF(res), lal_constraint, ctx);
  return *res;
}

} //End Libadalang_ROSE_Translation namepsace
