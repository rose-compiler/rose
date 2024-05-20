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
  getAnonymousAccessType(ada_base_entity* lal_element, AstContext ctx)
  {
    SgType*                   underty = nullptr;
    bool has_null_exclusion = false;

    ada_base_entity lal_type_access;
    ada_anonymous_type_f_type_decl(lal_element, &lal_type_access);
    ada_type_decl_f_type_def(&lal_type_access, &lal_type_access);

    ada_node_kind_enum lal_type_access_kind = ada_node_kind(&lal_type_access);

    switch(lal_type_access_kind)
    {
      //case An_Anonymous_Access_To_Constant:            // [...] access constant subtype_mark
      case ada_type_access_def:            // [...] access subtype_mark
      {
        //Get the constant state
        ada_base_entity lal_has_constant;
        ada_type_access_def_f_has_constant(&lal_type_access, &lal_has_constant);
        ada_node_kind_enum lal_has_constant_kind = ada_node_kind(&lal_has_constant);
        const bool isConstant = (lal_has_constant_kind == ada_constant_present);

        logKind(isConstant ? "An_Anonymous_Access_To_Constant" : "An_Anonymous_Access_To_Variable", lal_type_access_kind);

        //Get the not null state
        ada_base_entity lal_has_not_null;
        ada_access_def_f_has_not_null(&lal_type_access, &lal_has_not_null);
        ada_node_kind_enum lal_has_not_null_kind = ada_node_kind(&lal_has_not_null);
        has_null_exclusion = (lal_has_not_null_kind == ada_not_null_present);

        //Get the subtype indication
        ada_base_entity lal_subtype_indication;
        ada_type_access_def_f_subtype_indication(&lal_type_access, &lal_subtype_indication);
        underty = &getDeclType(&lal_subtype_indication, ctx);

        if(isConstant){
          underty = &mkConstType(*underty);
        }
        break;
      }

      /*case An_Anonymous_Access_To_Procedure:           // access procedure
      case An_Anonymous_Access_To_Protected_Procedure: // access protected procedure
      case An_Anonymous_Access_To_Function:            // access function
      case An_Anonymous_Access_To_Protected_Function:  // access protected function
      {
        if (access_type_kind == An_Anonymous_Access_To_Procedure)
          logKind("An_Anonymous_Access_To_Procedure");
        else if (access_type_kind == An_Anonymous_Access_To_Protected_Procedure)
          logKind("An_Anonymous_Access_To_Protected_Procedure");
        else if (access_type_kind == An_Anonymous_Access_To_Function)
          logKind("An_Anonymous_Access_To_Function");
        else
          logKind("An_Anonymous_Access_To_Protected_Function");

        // these are functions, so we need to worry about return types
        const bool  isFuncAccess = (  (access_type_kind == An_Anonymous_Access_To_Function)
                                   || (access_type_kind == An_Anonymous_Access_To_Protected_Function)
                                   );
        const bool  isProtected  = (  (access_type_kind == An_Anonymous_Access_To_Protected_Function)
                                   || (access_type_kind == An_Anonymous_Access_To_Protected_Procedure)
                                   );

        ElemIdRange params  = idRange(access.Access_To_Subprogram_Parameter_Profile);
        SgType&     rettype = isFuncAccess ? getDeclTypeID(access.Access_To_Function_Result_Profile, ctx)
                                           : mkTypeVoid();
        underty = &mkAdaSubroutineType(rettype, ParameterCompletion{params, ctx}, ctx.scope(), isProtected);
        break;
      }*/

      // An unexpected element
      default:
        logError() << "Unhandled anonymous access type kind: " << lal_type_access_kind << std::endl;
        underty = &mkTypeUnknown();
    }

    SgType& res = mkAdaAccessType(SG_DEREF(underty), false /* general access */, true /* anonymous */);

    return excludeNullIf(res, has_null_exclusion, ctx);
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
      case ada_anonymous_type: //TODO Does this node match multiple asis nodes?
        {
          logKind("ada_anonymous_type", kind);
          res = &getAnonymousAccessType(lal_def, ctx);

          break;
        }
      default:
        logWarn() << "Unhandled type definition: " << kind << std::endl;
        res = &mkTypeUnknown();
        //ADA_ASSERT(!FAIL_ON_ERROR(ctx));
    }

    return SG_DEREF(res);
  }

/// returns the ROSE type for an Asis definition \ref lal_element
/// iff lal_element is NULL, an SgTypeVoid is returned.
SgType&
getDefinitionType_opt(ada_base_entity* lal_element, AstContext ctx)
{
  // defid is null for example for an entry that does not specify a family type
  if(ada_node_is_null(lal_element)){
    return mkTypeVoid();
  }

  if(lal_element == nullptr)
  {
    logError() << "undefined type id: " << lal_element << std::endl;
    return mkTypeUnknown();
  }

  return getDefinitionType(lal_element, ctx);
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
  getAccessType(ada_base_entity* lal_element, AstContext ctx)
  {
    ada_node_kind_enum kind = ada_node_kind(lal_element);
    SgAdaAccessType* access_t = nullptr;

    ada_base_entity lal_not_null;
    ada_access_def_f_has_not_null(lal_element, &lal_not_null);
    ada_node_kind_enum lal_not_null_kind = ada_node_kind(&lal_not_null);
    bool has_not_null = (lal_not_null_kind == ada_not_null_present);

    switch(kind) {
      // variable access kinds
    case ada_type_access_def:
      {
        //Get the constant status
        ada_base_entity lal_has_constant;
        ada_type_access_def_f_has_constant(lal_element, &lal_has_constant);
        ada_node_kind_enum lal_has_constant_kind = ada_node_kind(&lal_has_constant);
        const bool isConstant = (lal_has_constant_kind == ada_constant_present);

        //Get the variable status
        ada_base_entity lal_has_all;
        ada_type_access_def_f_has_all(lal_element, &lal_has_all);
        ada_node_kind_enum lal_has_all_kind = ada_node_kind(&lal_has_all);
        const bool isVariable = (lal_has_all_kind == ada_all_present);

        //Get the subtype indication
        ada_base_entity lal_subtype_indication;
        ada_type_access_def_f_subtype_indication(lal_element, &lal_subtype_indication);
        SgType* ato = &getDefinitionType(&lal_subtype_indication, ctx);

        if (isConstant) ato = &mkConstType(*ato);
        access_t = &mkAdaAccessType(SG_DEREF(ato), isVariable /* general access */);

        break;
      }

      // subprogram access kinds
    case ada_access_to_subp_def:
      {
        //Get the subp kind
        ada_base_entity subp_spec;
        ada_access_to_subp_def_f_subp_spec(lal_element, &subp_spec);
        ada_base_entity subp_kind;
        ada_subp_spec_f_subp_kind(&subp_spec, &subp_kind);
        ada_node_kind_enum subp_kind_kind = ada_node_kind(&subp_kind);

        const bool isFuncAccess = ( subp_kind_kind == ada_subp_kind_function);

        //Get has_protected
        ada_base_entity has_protected;
        ada_access_to_subp_def_f_has_protected(lal_element, &has_protected);
        ada_node_kind_enum has_protected_kind = ada_node_kind(&has_protected);

        const bool isProtected  = (has_protected_kind == ada_protected_present);

        //Get the params
        ada_base_entity subp_params;
        ada_subp_spec_f_subp_params(&subp_spec, &subp_params);

        //Get the return for if this is a func
        ada_base_entity subp_returns;
        ada_subp_spec_f_subp_returns(&subp_spec, &subp_returns);

        SgType&              rettype = isFuncAccess ? getDeclType(&subp_returns, ctx)
                                                    : mkTypeVoid();
        SgAdaSubroutineType& funty   = mkAdaSubroutineType(rettype, ParameterCompletion{&subp_params, ctx}, ctx.scope(), isProtected);

        access_t = &mkAdaAccessType(funty);

        break;
      }

    default:
      logError() << "Unhandled access type kind: " << kind << std::endl;
      access_t = &mkAdaAccessType(mkTypeUnknown());
    }

    return excludeNullIf(SG_DEREF(access_t), has_not_null, ctx);
  }

  SgType&
  getDeclType(ada_base_entity* lal_id, AstContext ctx)
  {
    //Get the kind of this node
    ada_node_kind_enum kind;
    kind = ada_node_kind(lal_id);

    ada_base_entity lal_declaration;

    if(kind == ada_subtype_indication){
      //Get the type this references
      ada_type_expr_p_designated_type_decl(lal_id, &lal_declaration);
    } else if(kind == ada_identifier){
      //Get the type this references
      ada_expr_p_first_corresponding_decl(lal_id, &lal_declaration);
    } else {
      logError() << "getDeclType: unhandled definition kind: " << kind
                 << std::endl;
      return mkTypeUnknown();
    }

    if(ada_node_is_null(&lal_declaration)){
      logError() << "getDeclType cannot find definition.\n";
    }

    SgNode& basenode = getExprType(&lal_declaration, ctx);
    SgType* res      = sg::dispatch(MakeTyperef(lal_id, ctx), &basenode);

    return SG_DEREF(res);

    /*ADA_ASSERT(elem.Element_Kind == A_Definition);
    Definition_Struct& def = elem.The_Union.Definition;

    if (def.Definition_Kind == An_Access_Definition)
      return getAnonymousAccessType(def, ctx);*/
  }

  SgClassDefinition&
  getRecordBody(ada_base_entity* lal_record, AstContext ctx)
  {
    SgClassDefinition&        sgnode     = mkRecordBody();

    //Get the kind
    ada_node_kind_enum kind = ada_node_kind(lal_record);

    if(kind == ada_null_record_def)
    {
      logKind("ada_null_record_def", kind);
      SgClassDefinition&      sgdef = mkRecordBody();

      attachSourceLocation(sgdef, lal_record, ctx);
      return sgdef;
    }

    sgnode.set_parent(&ctx.scope());

    /*ElemIdRange               components = idRange(rec.Record_Components);
    //~ ElemIdRange               implicits  = idRange(rec.Implicit_Components);

    // NOTE: the true parent is when the record is created; this is set to enable
    //       traversal from the record body to global scope.
    traverseIDs(components, elemMap(), ElemCreator{ctx.scope(sgnode)});
    //~ was: traverseIDs(components, elemMap(), ElemCreator{ctx.scope_npc(sgnode)});

    // how to represent implicit components
    //~ traverseIDs(implicits, elemMap(), ElemCreator{ctx.scope_npc(sgnode)});

    //~ markCompilerGenerated(sgnode);*/ //TODO
    logWarn() << "Component traversal unimplemented!\n";
    return sgnode;
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

          ada_base_entity subtype_indication;
          ada_derived_type_def_f_subtype_indication(lal_def, &subtype_indication);

          SgType& basetype = getDefinitionType(&subtype_indication, ctx);

          //~ if (isSgEnumType(si::Ada::base)
          res.sageNode(mkAdaDerivedType(basetype));
          break;
        }

      case ada_signed_int_type_def:           // 3.5.4(3)
        {
          logKind("ada_signed_int_type_def", kind);

          //Get the constraint
          ada_base_entity int_constraint;
          ada_signed_int_type_def_f_range(lal_def, &int_constraint);

          SgAdaTypeConstraint& constraint = getConstraint(&int_constraint, ctx);
          SgType&              superty    = mkIntegralType();

          res.sageNode(mkAdaSubtype(superty, constraint, true /* from root */));

          break;
        }
      case ada_record_type_def:               // 3.8(2)     -> Trait_Kinds
        {
          logKind("ada_record_type_def", kind);

          //Get the definition
          ada_base_entity record_def;
          ada_record_type_def_f_record_def(lal_def, &record_def);

          SgClassDefinition& def = getRecordBody(&record_def, ctx);

          //Get the abstract, limited, & tagged status
          ada_base_entity has_abstract, has_limited, has_tagged;
          ada_record_type_def_f_has_abstract(lal_def, &has_abstract);
          ada_record_type_def_f_has_limited(lal_def, &has_limited);
          ada_record_type_def_f_has_tagged(lal_def, &has_tagged);
          ada_node_kind_enum abstract_kind = ada_node_kind(&has_abstract);
          ada_node_kind_enum limited_kind = ada_node_kind(&has_limited);
          ada_node_kind_enum tagged_kind = ada_node_kind(&has_tagged);
          bool abstract = (abstract_kind == ada_abstract_present);
          bool limited = (limited_kind == ada_limited_present);
          bool tagged = (tagged_kind == ada_tagged_present);

          res = TypeData{lal_def, &def, abstract, limited, tagged};

          break;
        }
      case ada_access_to_subp_def:              // 3.10(2)    -> Access_Type_Kinds
        {
          logKind("ada_access_to_subp_def", kind);
          SgType& access_t = getAccessType(lal_def, ctx);
          res.sageNode(access_t);
          break;
        }
      case ada_type_access_def:              // 3.10(2)    -> Access_Type_Kinds
        {
          logKind("ada_type_access_def", kind);
          SgType& access_t = getAccessType(lal_def, ctx);
          res.sageNode(access_t);
          break;
        }

      default:
        {
          logWarn() << "unhandled type kind " << kind << " in getTypeFoundation" << std::endl;
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

template<class MapT, class StringMap>
void handleStdDecl(MapT& map1, StringMap& map2, ada_base_entity* lal_decl, SgAdaPackageSpec& stdspec)
{
  //Get the name of the type for logging purposes
  ada_text_type fully_qualified_name;
  ada_basic_decl_p_canonical_fully_qualified_name(lal_decl, &fully_qualified_name);
  AdaIdentifier canonical_fully_qualified_name (dot_ada_text_type_to_string(fully_qualified_name));
  //Get the name of this type without the "STANDARD."
  std::string type_name = canonical_fully_qualified_name.substr(9, canonical_fully_qualified_name.length()-1);

  //Get the hash of this decl
  int hash = hash_node(lal_decl);
  SgType* generatedType = nullptr;

  //logInfo() << "handleStdDecl called for " << canonical_fully_qualified_name << std::endl;

  if(canonical_fully_qualified_name.find("BOOLEAN") != std::string::npos){
    // boolean enum type
    SgEnumDeclaration&    boolDecl    = mkEnumDefn("BOOLEAN", stdspec);
    SgType&               adaBoolType = SG_DEREF(boolDecl.get_type());
    generatedType                     = boolDecl.get_type();

    declareEnumItem(boolDecl, "False", 0);
    declareEnumItem(boolDecl, "True",  1);
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
    generatedType = declareIntSubtype(type_name, lower_bound, upper_bound, stdspec).get_type();
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
    generatedType = declareRealSubtype(type_name, num_digits, base, exponent, stdspec).get_type();
  } else if(canonical_fully_qualified_name.find("CHARACTER") != std::string::npos){
    //   in Ada character is of enum type.
    //   Currently, the enum is not filled with members, but they are rather logically
    //   injected via the adaParent_Type set to char char16 or char32.
    //TODO Is there a way to make this more general?
    SgEnumDeclaration& adaCharDecl = mkEnumDecl(type_name, stdspec);
    if(type_name == "CHARACTER"){
      adaCharDecl.set_adaParentType(sb::buildCharType());
    } else if(type_name == "WIDE_CHARACTER"){
      adaCharDecl.set_adaParentType(sb::buildChar16Type());
    } else if(type_name == "WIDE_WIDE_CHARACTER"){
      adaCharDecl.set_adaParentType(sb::buildChar32Type());
    } else {
      logError() << "No plan for char type " << type_name << std::endl;
      return;
    }
    generatedType = adaCharDecl.get_type();
  } else if(canonical_fully_qualified_name.find("STRING") != std::string::npos){
    SgType* adaCharTypePtr;
    SgType& adaPositiveType = SG_DEREF(map2.at(AdaIdentifier{"POSITIVE"}));
    if(type_name == "STRING"){
      adaCharTypePtr = map2.at(AdaIdentifier{"CHARACTER"});
    } else if(type_name == "WIDE_STRING"){
      adaCharTypePtr = map2.at(AdaIdentifier{"WIDE_CHARACTER"});
    } else if(type_name == "WIDE_WIDE_STRING"){
      adaCharTypePtr = map2.at(AdaIdentifier{"WIDE_WIDE_CHARACTER"});
    } else {
      logError() << "No plan for string type " << type_name << std::endl;
      return;
    }
    SgType& adaCharType = SG_DEREF(adaCharTypePtr);
    generatedType = declareStringType(type_name, adaPositiveType, adaCharType, stdspec).get_type();
  } else if(canonical_fully_qualified_name.find("DURATION") != std::string::npos){
    // \todo reconsider adding a true Ada Duration type
    generatedType = sb::buildOpaqueType(type_name, &stdspec);
  } else {
    //TODO Universal int/real?
  }
  map1[hash] = generatedType;
  map2[AdaIdentifier{type_name}] = generatedType;
}

template<class MapT, class StringMap>
void handleStdSubType(MapT& map1, StringMap& map2, ada_base_entity* lal_decl, SgAdaPackageSpec& stdspec)
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

  SgType* adaIntegerSubType = nullptr;

  if(supertype_name.find("INTEGER") != std::string::npos){
    //Determine the range of the subtype
    //For now, we only have to worry about positive & natural
    //We can assume that they will always start from 1 & 0, respectively, so we just need to find the upper bound
    int lower_bound = -1;
    if(type_name == "NATURAL"){
      lower_bound = 0;
    } else if(type_name == "POSITIVE"){
      lower_bound = 1;
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
    adaIntegerSubType            = declareIntSubtype(type_name, lower_bound, upper_bound, stdspec).get_type();

  } else {
    logError() << "The Standard Package has a non-integer subtype.\n";
  }

  map1[hash]                     = adaIntegerSubType;
  map2[AdaIdentifier{type_name}] = adaIntegerSubType;

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
          handleStdDecl(adaTypes(), adaTypesByName(), &lal_decl, stdspec);
          break;
        }
        case ada_subtype_decl:
        {
          handleStdSubType(adaTypes(), adaTypesByName(), &lal_decl, stdspec);
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

  for (auto const& element : adaTypesByName()) {
    logInfo() << element.first << std::endl;
  }

  SgType&               adaBoolType = SG_DEREF(adaTypesByName().at(AdaIdentifier{"BOOLEAN"}));
  // integral types
  SgType&               adaIntType  = mkIntegralType(); // the root integer type in ROSE

  SgType& adaPositiveType           = SG_DEREF(adaTypesByName().at(AdaIdentifier{"POSITIVE"}));
  adaTypes()[-5]                    = &adaPositiveType; //TODO This is here so other functions can get positive. Find a better way?
  SgType& adaNaturalType            = SG_DEREF(adaTypesByName().at(AdaIdentifier{"NATURAL"}));

  SgType& adaRealType               = mkRealType();
  
  //characters
  SgType& adaCharType               = SG_DEREF(adaTypesByName().at(AdaIdentifier{"CHARACTER"}));
  SgType& adaWideCharType           = SG_DEREF(adaTypesByName().at(AdaIdentifier{"WIDE_CHARACTER"}));
  SgType& adaWideWideCharType       = SG_DEREF(adaTypesByName().at(AdaIdentifier{"WIDE_WIDE_CHARACTER"}));

  // String types
  SgType& adaStringType             = SG_DEREF(adaTypesByName().at(AdaIdentifier{"STRING"}));
  SgType& adaWideStringType         = SG_DEREF(adaTypesByName().at(AdaIdentifier{"WIDE_STRING"}));
  SgType& adaWideWideStringType     = SG_DEREF(adaTypesByName().at(AdaIdentifier{"WIDE_WIDE_STRING"}));

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
    case ada_range_spec:
      {
        logKind("ada_range_constraint", kind);

        //Get the lower/upper bounds of the range
        ada_base_entity lal_range_op, lal_lower, lal_upper;
        if(kind == ada_range_constraint){
          ada_range_constraint_f_range(lal_constraint, &lal_range_op);
          ada_range_spec_f_range(&lal_range_op, &lal_range_op);
        } else {
          ada_range_spec_f_range(lal_constraint, &lal_range_op);
        }
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
