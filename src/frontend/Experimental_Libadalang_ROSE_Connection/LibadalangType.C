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

  /// Handles type creation for constrained/unconstrained array declarations
  SgArrayType&
  createArrayType(ada_base_entity* lal_element, AstContext ctx)
  {
    //Get the indices
    ada_base_entity lal_indices;
    ada_array_type_def_f_indices(lal_element, &lal_indices);
    ada_node_kind_enum lal_indices_kind = ada_node_kind(&lal_indices);
    bool unconstrained = true;

    if(lal_indices_kind == ada_unconstrained_array_indices){
      ada_unconstrained_array_indices_f_types(&lal_indices, &lal_indices);
    } else {
      unconstrained = false;
      ada_constrained_array_indices_f_list(&lal_indices, &lal_indices);
    }

    //Get the component type
    ada_base_entity lal_component_type;
    ada_array_type_def_f_component_type(lal_element, &lal_component_type);

    std::vector<SgExpression*> indicesSeq;

    int count = ada_node_children_count(&lal_indices);
    for(int i = 0; i < count; ++i){
      ada_base_entity lal_array_index;
      if(ada_node_child(&lal_indices, i, &lal_array_index) != 0){
        //Call getExpr on the name
        ada_base_entity lal_identifier;
        if(unconstrained){
          ada_unconstrained_array_index_f_subtype_indication(&lal_array_index, &lal_identifier);
          ada_subtype_indication_f_name(&lal_identifier, &lal_identifier);
          SgExpression& index = getExpr(&lal_identifier, ctx);
          indicesSeq.push_back(&index);
        } else {
          //Constrained arrays have a different format, so they need getDefinitionExpr 
          SgExpression& index = getDefinitionExpr(&lal_array_index, ctx);
          indicesSeq.push_back(&index);
        }
      }
    }

    SgExprListExp&             indicesAst  = mkExprListExp(indicesSeq);
    SgType&                    compType    = getDefinitionType(&lal_component_type, ctx);

    return mkArrayType(compType, indicesAst, unconstrained);
  }

  /// Handles a discriminant association, placing each name into \ref elems
  void createDiscriminantAssoc(ada_base_entity* lal_element, SgExpressionPtrList& elems, AstContext ctx)
  {
    ada_node_kind_enum kind = ada_node_kind(lal_element);
    logKind("ada_discriminant_assoc", kind);

    //Get the list of names
    ada_base_entity lal_name_list;
    ada_discriminant_assoc_f_ids(lal_element, &lal_name_list);

    //Get the discr expr
    ada_base_entity lal_discr_expr;
    ada_discriminant_assoc_f_discr_expr(lal_element, &lal_discr_expr);

    SgExpression& sgnode = getExpr(&lal_discr_expr, ctx);
    int num_names = ada_node_children_count(&lal_name_list);

    attachSourceLocation(sgnode, lal_element, ctx);

    if(num_names <= 0)
    {
      elems.push_back(&sgnode);
    }
    else
    {
      //Get the first name from the list
      ada_base_entity lal_identifier; //TODO Can this node be a non-ada_identifier?
      if(ada_node_child(&lal_name_list, 0, &lal_identifier) != 0){
        std::string ident = canonical_text_as_string(&lal_identifier);
        elems.push_back(sb::buildActualArgumentExpression_nfi(ident, &sgnode));
      }

      //Handle the remaining names, if they exist
      for(int i = 1; i < num_names; ++i){
        // \todo SgActualArgumentExpression only supports 1:1 mapping from name to an expression
        //       but not n:1.
        //       => create an entry for each name beyond the first, and duplicate the expression
        if(ada_node_child(&lal_name_list, i, &lal_identifier) != 0){
          std::string ident = canonical_text_as_string(&lal_identifier);
          elems.push_back(sb::buildActualArgumentExpression_nfi(ident, si::deepCopy(&sgnode)));
        }
      }
    }
  }

  /// Creates an integer subtype from the standard package, with a name and range
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

  /// Creates an integer subtype with a \ref name, and a range based on the size of a Cxx type
  template <class CxxType>
  SgTypedefDeclaration&
  declareIntSubtype(const std::string& name, SgAdaPackageSpec& scope)
  {
    using TypeLimits = std::numeric_limits<CxxType>;

    return declareIntSubtype(name, TypeLimits::min(), TypeLimits::max(), scope);
  }

  /// Creates a real subtype from the standard package with a name, max size, and digit constraint
  /// @{
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
  /// @}

  /// Declares a string as an array of \ref comp type chars
  SgTypedefDeclaration&
  declareStringType(const std::string& name, SgType& positive, SgType& comp, SgAdaPackageSpec& scope)
  {
    SgExprListExp&        idx     = mkExprListExp({&mkTypeExpression(positive)});
    SgArrayType&          strtype = mkArrayType(comp, idx, true);
    SgTypedefDeclaration& sgnode  = mkTypeDecl(name, strtype, scope);

    scope.append_statement(&sgnode);
    return sgnode;
  }

  /// Creates a standard exception and adds it to the provided map
  template<class MapT>
  void declareException(MapT& m, ada_base_entity* lal_element, SgType& base, SgAdaPackageSpec& scope)
  {
    //Get the name of this excp
    ada_text_type fully_qualified_name;
    ada_basic_decl_p_canonical_fully_qualified_name(lal_element, &fully_qualified_name);
    std::string canonical_fully_qualified_name = dot_ada_text_type_to_string(fully_qualified_name);
    //Get the name of this pkg without the "STANDARD."
    std::string excp_name = canonical_fully_qualified_name.substr(9, canonical_fully_qualified_name.length()-1);

    SgInitializedName&              sgnode = mkInitializedName(excp_name, base, nullptr);
    std::vector<SgInitializedName*> exdecl{ &sgnode };
    SgVariableDeclaration&          exvar = mkExceptionDecl(exdecl, scope);

    exvar.set_firstNondefiningDeclaration(&exvar);
    scope.append_statement(&exvar);

    ada_base_entity lal_defining_name;
    ada_exception_decl_f_ids(lal_element, &lal_defining_name);
    ada_node_child(&lal_defining_name, 0, &lal_defining_name);
    int hash = hash_node(&lal_defining_name);
    m[hash] = &sgnode;
  }

  /// Create a new package that is defined in standard. Currently used only for the ascii package
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

} //end unnamed namespace

  SgType&
  excludeNullIf(SgType& ty, bool exclNull, AstContext)
  {
    return exclNull ? mkNotNullType(ty) : ty;
  }

  /// Handles the type def of an ada_anonymous_type
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
      case ada_array_type_def: //TODO This is a hack b/c array type defs look the same to accesses in lal
      {
        return createArrayType(&lal_type_access, ctx);
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

  /// Get the type associated with a definition
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
      case ada_component_def:
        {
          logKind("ada_component_def", kind);

          //Get the type expr
          ada_base_entity lal_type_expr;
          ada_component_def_f_type_expr(lal_def, &lal_type_expr);

          res = &getDefinitionType(&lal_type_expr, ctx);

          //Determine has_aliased
          ada_base_entity lal_has_aliased;
          ada_component_def_f_has_aliased(lal_def, &lal_has_aliased);
          ada_node_kind_enum lal_has_aliased_kind = ada_node_kind(&lal_has_aliased);

          if(lal_has_aliased_kind == ada_aliased_present){
            res = &mkAliasedType(*res);
          }

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
    for(long unsigned int i = 0; i < word_to_hash.length(); ++i){
      hash = (hash * seed) + word_to_hash[i];
    }
    return hash;
  }

  /// returns the ROSE type a Libadalang type decl corresponds to
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
      case ada_subtype_decl:
        {
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
      case ada_attribute_ref:
        {
          res = &getAttributeExpr(lal_expr, ctx);
          break;
        }
      default:
        logFlaw() << "Unknown type expression: " << kind << std::endl;
        //ADA_ASSERT(!FAIL_ON_ERROR(ctx));
        res = &mkTypeUnknown();
    }

    return SG_DEREF(res);
  }

  /// returns a ROSE SgType for what \ref lal_element is accessing
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

  /// Returns a reference to the type the decl \ref lal_element is indicating
  SgType&
  getDeclType(ada_base_entity* lal_id, AstContext ctx)
  {
    //Get the kind of this node
    ada_node_kind_enum kind;
    kind = ada_node_kind(lal_id);

    ada_base_entity lal_declaration;

    if(kind == ada_subtype_indication){
      //Call getDeclType on the f_name property
      ada_subtype_indication_f_name(lal_id, &lal_declaration);
      return getDeclType(&lal_declaration, ctx);
    } else if(kind == ada_identifier || kind == ada_dotted_name){
      //Get the type this references
      ada_expr_p_first_corresponding_decl(lal_id, &lal_declaration);
    } else if(kind == ada_anonymous_type){
      //Can ada_anonymous_type ever not be an access?
      return getAnonymousAccessType(lal_id, ctx);
    } else if(kind == ada_attribute_ref){
      //Pass this node on to getExprType
      lal_declaration = *lal_id;
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
  }

  SgAdaTypeConstraint*
  getConstraint_opt(ada_base_entity* lal_element, AstContext ctx)
  {
    if(lal_element && ada_node_is_null(lal_element)){
      return nullptr;
    }
    return lal_element ? &getConstraint(lal_element, ctx) : nullptr;
  }

  /// Returns a record class with associated components as represetned in \ref lal_element
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

    if(kind == ada_record_def){
      logKind("ada_record_def", kind);

      //~ ElemIdRange               implicits  = idRange(rec.Implicit_Components);

      // NOTE: the true parent is when the record is created; this is set to enable
      //       traversal from the record body to global scope.
      //Get the list of components, and the variant part
      ada_base_entity lal_component_list, lal_variant_part;
      ada_base_record_def_f_components(lal_record, &lal_component_list);
      ada_component_list_f_variant_part(&lal_component_list, &lal_variant_part);
      ada_component_list_f_components(&lal_component_list, &lal_component_list);

      //Call handleElement on each component
      int component_count = ada_node_children_count(&lal_component_list);
      for(int i = 0; i < component_count; ++i){
        ada_base_entity lal_component;
        if(ada_node_child(&lal_component_list, i, &lal_component) != 0){
          handleElement(&lal_component, ctx.scope(sgnode));
        }
      }

      //lal has a separate section for variants, so call handleElement on it if it exists
      if(!ada_node_is_null(&lal_variant_part)){
        handleElement(&lal_variant_part, ctx.scope(sgnode));
      }

      // how to represent implicit components
      //~ traverseIDs(implicits, elemMap(), ElemCreator{ctx.scope_npc(sgnode)});

      //~ markCompilerGenerated(sgnode);
    } else {
      logWarn() << "Component traversal unimplemented for kind " << kind << "!\n";
    }
    return sgnode;
  }

  /// Creates an enum value and attaches it to the \ref enumdef
  void addEnumLiteral(ada_base_entity* lal_element, SgEnumDeclaration& enumdef, int enum_position, AstContext ctx)
  {
    SgType& enumty = SG_DEREF(enumdef.get_type());
    ada_node_kind_enum kind = ada_node_kind(lal_element);
    logKind("ada_enum_literal_decl", kind);

    //Get the name
    ada_base_entity lal_defining_name, lal_identifier;
    ada_enum_literal_decl_f_name(lal_element, &lal_defining_name);
    ada_defining_name_f_name(&lal_defining_name, &lal_identifier);
    std::string ident = canonical_text_as_string(&lal_identifier);

    // \todo name.ident could be a character literal, such as 'c'
    //       since SgEnumDeclaration only accepts SgInitializedName as enumerators
    //       SgInitializedName are created with the name 'c' instead of character constants.
    SgExpression&       repval = getEnumRepresentationValue(lal_element, enum_position, ctx);
    SgInitializedName&  sgnode = mkEnumeratorDecl(enumdef, ident, enumty, repval);

    attachSourceLocation(sgnode, lal_element, ctx);
    //~ sg::linkParentChild(enumdcl, sgnode, &SgEnumDeclaration::append_enumerator);
    enumdef.append_enumerator(&sgnode);

    int hash = hash_node(lal_element);
    recordNode(libadalangVars(), hash, sgnode);
  }

  SgBaseClass&
  getParentType(ada_base_entity* lal_element, AstContext ctx)
  {
    SgType& basety = getDefinitionType(lal_element, ctx);

    return mkRecordParent(basety);
  }

  /// Create a ROSE representation of the def, and record whether it is abstract, limited, tagged, or inherits any routines
  TypeData
  getTypeFoundation(const std::string& name, ada_base_entity* lal_def, AstContext ctx)
  {
    //Get the kind of this node
    ada_node_kind_enum kind = ada_node_kind(lal_def);

    logKind("A_Type_Definition", kind);

    TypeData                res{lal_def, nullptr, false, false, false};

    switch(kind){
      case ada_derived_type_def:              // 3.4(2)     -> Trait_Kinds
        {
          logKind("ada_derived_type_def", kind);

          //Get the subtype indication
          ada_base_entity subtype_indication;
          ada_derived_type_def_f_subtype_indication(lal_def, &subtype_indication);

          //Check if this is a record extension
          ada_base_entity lal_record_ext;
          ada_derived_type_def_f_record_extension(lal_def, &lal_record_ext);

          if(ada_node_is_null(&lal_record_ext)){
            SgType& basetype = getDefinitionType(&subtype_indication, ctx);

            //~ if (isSgEnumType(si::Ada::base)
            res.sageNode(mkAdaDerivedType(basetype));
            break;
          } else {
            SgClassDefinition&  def    = getRecordBody(&lal_record_ext, ctx);
            SgBaseClass&        parent = getParentType(&subtype_indication, ctx);

            sg::linkParentChild(def, parent, &SgClassDefinition::append_inheritance);

            res.sageNode(def);
            break;
          }
        }
      case ada_enum_type_def:         // 3.5.1(2)
        {
          logKind("ada_enum_type_def", kind);

          SgEnumDeclaration& sgnode = mkEnumDefn(name, ctx.scope());

          //Get the enum values
          ada_base_entity lal_enum_literals;
          ada_enum_type_def_f_enum_literals(lal_def, &lal_enum_literals);

          int count = ada_node_children_count(&lal_enum_literals);
          for(int i = 0; i < count; ++i){
            ada_base_entity lal_enum_literal;
            if(ada_node_child(&lal_enum_literals, i, &lal_enum_literal) != 0){
              addEnumLiteral(&lal_enum_literal, sgnode, i, ctx);
            }
          }

          res.sageNode(sgnode);
          break ;
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
      case ada_array_type_def:      // 3.6(2)
        {
          logKind("ada_array_type_def", kind);

          res.sageNode(createArrayType(lal_def, ctx));
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

/// Creates a ROSE representation for a type defined in the Ada standard package
/// All types created are stored in 2 maps: 1 by hash (\ref map2), and 1 by name (\ref map2)
/// Types handled are: boolean, int, float, character, string, duration
template<class MapT, class StringMap>
void handleStdDecl(MapT& map1, StringMap& map2, ada_base_entity* lal_decl, SgAdaPackageSpec& stdspec, SgGlobal& global)
{
  //Get the name of the type for logging purposes
  ada_text_type fully_qualified_name;
  ada_basic_decl_p_canonical_fully_qualified_name(lal_decl, &fully_qualified_name);
  AdaIdentifier canonical_fully_qualified_name (dot_ada_text_type_to_string(fully_qualified_name));
  //Get the name of this type without the "STANDARD."
  std::string type_name = canonical_fully_qualified_name.substr(9, canonical_fully_qualified_name.length()-1);

  //Get the defining name
  ada_base_entity lal_defining_name;
  ada_base_type_decl_f_name(lal_decl, &lal_defining_name);

  //Get the hash of this decl
  int hash = hash_node(lal_decl);
  int defining_name_hash = hash_node(&lal_defining_name);
  SgType* generatedType = nullptr;

  //logTrace() << "handleStdDecl called for " << canonical_fully_qualified_name << std::endl;

  if(canonical_fully_qualified_name.find("BOOLEAN") != std::string::npos){
    // boolean enum type
    SgEnumDeclaration&    boolDecl    = mkEnumDefn("BOOLEAN", stdspec);
    SgType&               adaBoolType = SG_DEREF(boolDecl.get_type());
    generatedType                     = boolDecl.get_type();

    //Add the enum values to the libadalangVars() map
    //They aren't technically, but this is the only standard enum
    ada_base_entity lal_enum_decl_list;
    ada_type_decl_f_type_def(lal_decl, &lal_enum_decl_list);
    ada_enum_type_def_f_enum_literals(&lal_enum_decl_list, &lal_enum_decl_list);
    int count = ada_node_children_count(&lal_enum_decl_list);
    for(int i = 0; i < count; ++i){
      ada_base_entity lal_enum_decl;
      if(ada_node_child(&lal_enum_decl_list, i, &lal_enum_decl) != 0){
        //Get the name
        ada_base_entity lal_defining_name, lal_identifier;
        ada_enum_literal_decl_f_name(&lal_enum_decl, &lal_defining_name);
        ada_defining_name_f_name(&lal_defining_name, &lal_identifier);
        std::string ident = canonical_text_as_string(&lal_identifier);

        std::string         std_name = "__standard";
        SgExpression&       repval   = getEnumRepresentationValue(&lal_enum_decl, i, AstContext{}.scope(global).sourceFileName(std_name));
        SgInitializedName&  sgnode   = mkEnumeratorDecl(boolDecl, ident, adaBoolType, repval);
        sgnode.set_scope(boolDecl.get_scope());
        boolDecl.append_enumerator(&sgnode);
        int hash = hash_node(&lal_enum_decl);
        recordNode(libadalangVars(), hash, sgnode);
      }
    }
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

    const std::string name = canonical_text_as_string(&lal_exponent);
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
    std::string name = canonical_text_as_string(&lal_num_digits);
    int num_digits = std::stoi(name);
    
    ada_floating_point_def_f_range(&float_def, &lal_range);
    ada_range_spec_f_range(&lal_range, &lal_range);
    ada_bin_op_f_right(&lal_range, &lal_range);
    name = canonical_text_as_string(&lal_range);
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
  map1[defining_name_hash] = generatedType;
  map2[AdaIdentifier{type_name}] = generatedType;
}

/// Creates a ROSE representation for a subtype defined in the Ada standard package
/// All types created are stored in 2 maps: 1 by hash (\ref map2), and 1 by name (\ref map2)
/// Types handled are: positive, natural
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
  AdaIdentifier supertype_name(canonical_text_as_string(&subtype_identifier));

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

    const std::string name = canonical_text_as_string(&lal_exponent);
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

/// Creates a ROSE represntation of the ascii package as defined in the Ada standard package
/// Handles all of the possible values of the ascii enum
/// Values are registered to the map \ref m
template<class MapT>
void handleAsciiPkg(MapT& m, ada_base_entity* lal_decl, SgAdaPackageSpec& stdspec)
{
  //Get the name of this pkg
  ada_text_type fully_qualified_name;
  ada_basic_decl_p_canonical_fully_qualified_name(lal_decl, &fully_qualified_name);
  std::string canonical_fully_qualified_name = dot_ada_text_type_to_string(fully_qualified_name);
  //Get the name of this pkg without the "STANDARD."
  std::string pkg_name = canonical_fully_qualified_name.substr(9, canonical_fully_qualified_name.length()-1);

  //Make a new SgAdaPackageSpec using the name
  SgAdaPackageSpecDecl& asciipkg    = declarePackage(pkg_name, stdspec);
  SgAdaPackageSpec&     asciispec   = SG_DEREF(asciipkg.get_definition());

  //Get the decls for this pkg
  ada_base_entity lal_ascii_decls;
  ada_base_package_decl_f_public_part(lal_decl, &lal_ascii_decls);
  ada_declarative_part_f_decls(&lal_ascii_decls, &lal_ascii_decls);

  SgType& adaCharType = SG_DEREF(adaTypesByName().at(AdaIdentifier{"CHARACTER"}));

  int count = ada_node_children_count(&lal_ascii_decls);
  for(int i = 0; i < count; ++i){
    ada_base_entity lal_ascii_decl;
    if(ada_node_child(&lal_ascii_decls, i, &lal_ascii_decl) != 0){
      //Get the name of this decl
      ada_text_type decl_full_name;
      ada_basic_decl_p_canonical_fully_qualified_name(&lal_ascii_decl, &decl_full_name);
      std::string decl_full_name_string = dot_ada_text_type_to_string(decl_full_name);
      //Get the name without the "standard.ascii."
      std::string decl_name = decl_full_name_string.substr(15, canonical_fully_qualified_name.length()-1);

      //Get the char value for this decl
      char cval = 0;
      ada_base_entity lal_default_expr;
      ada_object_decl_f_default_expr(&lal_ascii_decl, &lal_default_expr);
      ada_node_kind_enum lal_default_expr_kind = ada_node_kind(&lal_default_expr);
      if(lal_default_expr_kind == ada_char_literal){
        uint32_t lal_denoted_value;
        ada_char_literal_p_denoted_value(&lal_default_expr, &lal_denoted_value);
        cval = char(lal_denoted_value);
      } else if(lal_default_expr_kind == ada_attribute_ref){
        ada_base_entity lal_int_literal;
        ada_attribute_ref_f_args(&lal_default_expr, &lal_int_literal);
        ada_node_child(&lal_int_literal, 0, &lal_int_literal);
        ada_param_assoc_f_r_expr(&lal_int_literal, &lal_int_literal);
        //Get the value of the int literal
        ada_big_integer denoted_value;
        ada_int_literal_p_denoted_value(&lal_int_literal, &denoted_value);
        LibadalangText value_text(denoted_value);
        std::string denoted_text = value_text.string_value();
        cval = char(stoi(denoted_text));
      } else {
        logFlaw() << "Unhandled default expr kind in handleAsciiPkg: " << lal_default_expr_kind << std::endl;
      }

      ada_base_entity lal_defining_name;
      ada_object_decl_f_ids(&lal_ascii_decl, &lal_defining_name);
      ada_node_child(&lal_defining_name, 0, &lal_defining_name);

      SgCharVal*             val  = sb::buildCharVal(cval);
      markCompilerGenerated(SG_DEREF(val));

      SgInitializedName&     var  = mkInitializedName(decl_name, adaCharType, val);
      SgVariableDeclaration& dcl  = mkVarDecl(var, asciispec);
      int                    hash = hash_node(&lal_defining_name);

      asciispec.append_statement(&dcl);
      m[hash] = &var;
    }
  }

  int pkg_hash = hash_node(lal_decl);
  adaPkgs()[pkg_hash] = &asciipkg;
}

void initializePkgStandard(SgGlobal& global, ada_base_entity* lal_root)
{
  logTrace() << "In initializePkgStandard.\n";
  // make available declarations from the package standard
  // https://www.adaic.org/resources/add_content/standards/05rm/html/RM-A-1.html

  SgAdaPackageSpecDecl& stdpkg  = mkAdaPackageSpecDecl(si::Ada::packageStandardName, global);
  SgAdaPackageSpec&     stdspec = SG_DEREF(stdpkg.get_definition());

  stdpkg.set_scope(&global);

  // \todo reconsider using a true Ada exception representation
  SgType&               exceptionType = SG_DEREF(sb::buildOpaqueType(si::Ada::exceptionName, &stdspec));

  //Get the lal representation of the standard package
  ada_analysis_unit std_unit;
  ada_ada_node_p_standard_unit(lal_root, &std_unit);

  ada_base_entity std_root;
  ada_unit_root(std_unit, &std_root);

  //Go to the list of declarations
  ada_base_entity decl_list;
  ada_compilation_unit_f_body(&std_root, &decl_list);
  ada_library_item_f_item(&decl_list, &decl_list);
  int pkg_hash = hash_node(&decl_list);
  ada_base_package_decl_f_public_part(&decl_list, &decl_list);
  ada_declarative_part_f_decls(&decl_list, &decl_list);

  //For each decl, call a function to add it to std based on its type
  //Get the children
  int count = ada_node_children_count(&decl_list);
  for(int i = 0; i < count; ++i)
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
          handleStdDecl(adaTypes(), adaTypesByName(), &lal_decl, stdspec, global);
          break;
        }
        case ada_subtype_decl:
        {
          handleStdSubType(adaTypes(), adaTypesByName(), &lal_decl, stdspec);
          break;
        }
        case ada_package_decl:
        {
          //This should only be the ascii pkg
          handleAsciiPkg(adaVars(), &lal_decl, stdspec);
          break;
        }
        case ada_exception_decl:
        {
          declareException(adaExcps(), &lal_decl, exceptionType, stdspec);
          break;
        }
        case ada_attribute_def_clause: //TODO This is about duration?
        case ada_pragma_node:
          break;
        default:
          logInfo() << "Unhandled decl kind " << decl_kind << " in initializePkgStandard.\n";
      }
    }
  }

  adaTypesByName()["EXCEPTION"]           = &exceptionType;

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
  //SgInitializedName& adaConstraintError    = declareException("Constraint_Error", exceptionType, stdspec);
  //SgInitializedName& adaProgramError       = declareException("Program_Error",    exceptionType, stdspec);
  //SgInitializedName& adaStorageError       = declareException("Storage_Error",    exceptionType, stdspec);
  //SgInitializedName& adaTaskingError       = declareException("Tasking_Error",    exceptionType, stdspec);

  // added packages
  adaPkgs()[pkg_hash]             = &stdpkg;

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

/// Creates a reference to a previously declared exception that \ref lal_element refers to
std::pair<SgInitializedName*, SgAdaRenamingDecl*>
getExceptionBase(ada_base_entity* lal_element, AstContext ctx)
{
  std::string name;
  //Get the corresponding definition
  ada_base_entity lal_corresponding_decl;
  ada_node_kind_enum kind = ada_node_kind(lal_element);

  if(kind == ada_identifier){
    ada_expr_p_first_corresponding_decl(lal_element, &lal_corresponding_decl);
    name = canonical_text_as_string(lal_element);
  } else if(kind == ada_dotted_name) {
    ada_dotted_name_f_suffix(lal_element, &lal_corresponding_decl);
    kind = ada_node_kind(&lal_corresponding_decl);
    while(kind == ada_dotted_name){
      ada_dotted_name_f_suffix(&lal_corresponding_decl, &lal_corresponding_decl);
      kind = ada_node_kind(&lal_corresponding_decl);
    }
    name = canonical_text_as_string(&lal_corresponding_decl);
    ada_expr_p_first_corresponding_decl(&lal_corresponding_decl, &lal_corresponding_decl);
  } else {
    logError() << "Unhandled kind " << kind << " in getExceptionBase!\n"; 
  }

  ada_ada_node_array defining_name_list;
  ada_basic_decl_p_defining_names(&lal_corresponding_decl, &defining_name_list);
  int decl_hash;
          
  //Find the correct decl in the defining name list
  for(int i = 0; i < defining_name_list->n; ++i){
    ada_base_entity defining_name = defining_name_list->items[i];
    ada_base_entity name_identifier;
    ada_defining_name_f_name(&defining_name, &name_identifier);
    const std::string test_name = canonical_text_as_string(&name_identifier);
    if(name == test_name){
      decl_hash = hash_node(&defining_name);
      ada_node_kind_enum def_kind = ada_node_kind(&defining_name);
      logInfo() << "Found definition for exception " << name << ", kind = " << def_kind << std::endl;
      break;
    }
  }

  // first try: look up in user defined exceptions
  if (SgInitializedName* ini = findFirst(libadalangExcps(), decl_hash))
    return std::make_pair(ini, nullptr);

  // second try: look up in renamed declarations
  if (SgDeclarationStatement* dcl = findFirst(libadalangDecls(), decl_hash))
  {
    SgAdaRenamingDecl& rendcl = SG_DEREF(isSgAdaRenamingDecl(dcl));

    return std::make_pair(nullptr, &rendcl);
  }

  // third try: look up in exceptions defined in standard
  if (SgInitializedName* ini = findFirst(adaExcps(), decl_hash))
    return std::make_pair(ini, nullptr);

  // last resort: create a new initialized name representing the exception
  logError() << "Unknown exception: " << name << std::endl;

  // \todo create an SgInitializedName if the exception was not found
  SgInitializedName& init = mkInitializedName(name, lookupNode(adaTypesByName(), AdaIdentifier{"EXCEPTION"}), nullptr);

  init.set_scope(&ctx.scope());
  return std::make_pair(&init, nullptr);
}

SgType&
getDiscreteSubtype(ada_base_entity* lal_type, ada_base_entity* lal_constraint, AstContext ctx)
{
  SgType*              res = &getDeclType(lal_type, ctx);
  SgAdaTypeConstraint* constraint = getConstraint_opt(lal_constraint, ctx);

  return constraint ? mkAdaSubtype(*res, *constraint) : *res;
}

/// Creates a constraint for a type based on \ref lal_constraint
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

    case ada_index_constraint:                   // 3.2.2: 3.6.1
      {
        logKind("ada_index_constraint", kind);

        //Get the constraint list 
        ada_base_entity lal_constraint_list;
        ada_index_constraint_f_constraints(lal_constraint, &lal_constraint_list);
        int count = ada_node_children_count(&lal_constraint_list);

        SgExpressionPtrList ranges;
        for(int i = 0; i < count; ++i){
          ada_base_entity lal_range_constraint;
          if(ada_node_child(&lal_constraint_list, i, &lal_range_constraint) != 0){
            SgExpression& range_constraint = getDiscreteRange(&lal_range_constraint, ctx);
            ranges.push_back(&range_constraint);
          }
        }

        res = &mkAdaIndexConstraint(std::move(ranges));
        break;
      }

    case ada_discriminant_constraint:             // 3.2.2
      {
        logKind("ada_discriminant_constraint", kind);

        //Get the list of associations
        ada_base_entity lal_assoc_list;
        ada_discriminant_constraint_f_constraints(lal_constraint, &lal_assoc_list);

        SgExpressionPtrList constraints;

        int count = ada_node_children_count(&lal_assoc_list);
        for(int i = 0; i < count; ++i){
          ada_base_entity lal_discr_assoc;
          if(ada_node_child(&lal_assoc_list, i, &lal_discr_assoc) != 0){
            createDiscriminantAssoc(&lal_discr_assoc, constraints, ctx);
          }
        }

        res = &mkAdaDiscriminantConstraint(std::move(constraints));
        break;
      }

    default:
      logError() << "Unhandled constraint: " << kind << std::endl;
      ROSE_ABORT();
  }

  attachSourceLocation(SG_DEREF(res), lal_constraint, ctx);
  return *res;
}

/// Creates a combined type that represents the type of all choices for an exception
SgType& createExHandlerType(ada_base_entity* lal_exception_choices, AstContext ctx){
  std::vector<SgType*> lst;
  int count = ada_node_children_count(lal_exception_choices);
  //Get the type for each choice
  for(int i = 0; i < count; ++i){
    ada_base_entity lal_exception_choice;
    if(ada_node_child(lal_exception_choices, i, &lal_exception_choice) != 0){
      SgExpression* exceptExpr = nullptr;
       ada_node_kind_enum kind = ada_node_kind(&lal_exception_choice);

      if(kind == ada_identifier || kind == ada_dotted_name)
      {
        auto              expair = getExceptionBase(&lal_exception_choice, ctx);
        SgScopeStatement& scope = ctx.scope();

        exceptExpr = expair.first ? &mkExceptionRef(*expair.first, scope)
                                  : &mkAdaRenamingRefExp(SG_DEREF(expair.second))
                                  ;

        attachSourceLocation(SG_DEREF(exceptExpr), &lal_exception_choice, ctx);
      }
      else if(kind == ada_others_designator) {
        exceptExpr = &getDefinitionExpr(&lal_exception_choice, ctx);
      } else {
        logError() << "Unrecognized kind " << kind << " in createExHandlerType!\n";
      }

      lst.push_back(&mkExceptionType(SG_DEREF(exceptExpr)));
    }
  }
  //Now, combine the types if necessary and return
  if(lst.size() == 1){
    return SG_DEREF(lst[0]);
  } else {
    return mkTypeUnion(std::move(lst));
  }
}

/// Get the type for a constant declaration
/// This function can call itself recursively
SgType* getNumberDeclType(ada_base_entity* lal_element){
  ada_base_entity lal_expr = *lal_element;
  ada_node_kind_enum lal_expr_kind = ada_node_kind(&lal_expr);
  switch(lal_expr_kind){
    case ada_int_literal:
      return &mkIntegralType();
      break;
    case ada_real_literal:
      return &mkRealType();
      break;
    case ada_bin_op:
      {
        //Take f_left until we get something that isn't a bin op
        while(lal_expr_kind == ada_bin_op){
          ada_bin_op_f_left(&lal_expr, &lal_expr);
          lal_expr_kind = ada_node_kind(&lal_expr);
        }
        return getNumberDeclType(&lal_expr);
        break;
      }
    case ada_identifier:
      {
        ada_base_entity lal_first_decl;
        ada_expr_p_first_corresponding_decl(&lal_expr, &lal_first_decl);
        lal_expr_kind = ada_node_kind(&lal_first_decl);
        if(lal_expr_kind == ada_number_decl){
          ada_number_decl_f_expr(&lal_first_decl, &lal_first_decl);
          return getNumberDeclType(&lal_first_decl);
        } else {
          logWarn() << "Constant references unhandled kind " << lal_expr_kind << std::endl;
          return &mkTypeUnknown();
        }
        break;
      }
    default:
      logError() << "Unexpected expr kind " << lal_expr_kind << " for ada_number_decl!\n";
      return &mkTypeUnknown();
      break;
  }
}

} //End Libadalang_ROSE_Translation namepsace
