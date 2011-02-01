// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
#include "rose.h"

// DQ (2/9/2010): Testing use of ROSE to compile ROSE.
#ifndef USE_ROSE

#include <algorithm>
#include <functional>
#include <numeric>
#include <string>

#include "rosez.hpp"

#include "RtedSymbols.h"
#include "DataStructures.h"
#include "RtedTransformation.h"

#include "RtedVisit.h"

VariableTraversal::VariableTraversal(RtedTransformation* t)
: Base(), transf(t), binary_ops(), for_loops()
{
  ROSE_ASSERT(transf != NULL);
}

static
bool isInitializedNameInForStatement(SgForInitStatement* inits, const SgInitializedName& name)
{
   // Capture for( int i = 0;
   std::vector<SgNode*> initialized_names = NodeQuery::querySubTree(inits, V_SgInitializedName);
   // Capture int i; for( i = 0;
   std::vector<SgNode*> init_var_refs = NodeQuery::querySubTree(inits, V_SgVarRefExp);
   for (std::vector<SgNode*>::iterator i = init_var_refs.begin(); i != init_var_refs.end(); ++i)
      initialized_names.push_back(isSgVarRefExp(*i) -> get_symbol() -> get_declaration());

   return (find(initialized_names.begin(), initialized_names.end(), &name) != initialized_names.end());
}

/// \brief   tests whether astNode directly or indirectly is on the right hand
///          side of an expressions.
/// \details indirectly means that if astNode is a child of a non-binary
///          expression or a dot-expression, the isRightOfBinaryOp is invoked
///          'recursively' (with the parent node) as the new astNode.
static
bool isRightOfBinaryOp(const SgNode* astNode) {
   const SgNode* temp = astNode;

   // \pp \todo the while loop can probably stop when we reach a non expression
   while (!isSgProject(temp))
   {
     const SgNode*     parent = temp->get_parent();
     const SgBinaryOp* binop = isSgBinaryOp(parent);

     if (binop && !isSgDotExp(binop))
     {
       return (binop->get_rhs_operand() == temp);
     }

     temp = parent;
   }

   return false;
}

static
bool isUsableAsSgPointerType( SgType* type ) {
    return isSgPointerType( skip_ReferencesAndTypedefs( type ));
}


struct InheritedAttributeHandler
{
  VariableTraversal& vt;
  InheritedAttribute ia;

  InheritedAttributeHandler(VariableTraversal& trav, const InheritedAttribute& inh)
  : vt(trav), ia(inh)
  {}

  void handle(SgNode&) {}

  void handle(SgFunctionDefinition& n)
  {
      vt.transf->visit_checkIsMain(&n);
      vt.transf->function_definitions.push_back(&n);

      ia.function = true;
      // do not handle as SgScopeStatement
  }

  void handle(SgVariableDeclaration& n)
  {
    if (isSgClassDefinition(n.get_parent())) return;

    const SgInitializedNamePtrList& vars = n.get_variables();

    for (SgInitializedNamePtrList::const_iterator it = vars.begin();it!=vars.end();++it)
    {
         SgInitializedName* initName = *it;
         ROSE_ASSERT(initName);
         if( isSgReferenceType( initName -> get_type() ))
           continue;

         vt.transf->variable_declarations.push_back(initName);
    }
  }

  void handle(SgInitializedName& n)
  {
      SgArrayType*       array = isSgArrayType(n.get_typeptr());

      // something like: struct type { int before; char c[ 10 ]; int after; }
      // does not need a createarray call, as the type is registered and any array
      // information will be tracked when variables of that type are created.
      // ignore arrays in parameter lists as they're actually pointers, not stack arrays
      if ( !array || isStructMember(n) || isFunctionParameter(n) ) return;

      RtedArray* arrayRted = new RtedArray(&n, getSurroundingStatement(&n), akStack);

      vt.transf->populateDimensions( arrayRted, &n, array );
      vt.transf->create_array_define_varRef_multiArray_stack[&n] = arrayRted;
  }

  void handle(SgAssignInitializer& n)
  {
      std::cerr << n.unparseToString() << std::endl;

      vt.transf->visit_isAssignInitializer(&n);
      ia.isAssignInitializer = true;
   }

   void handle(SgReturnStmt& n)
   {
     if (!n.get_expression()) return;

     vt.transf->returnstmt.push_back(&n);
   }

   void handle(SgUpcBarrierStatement& n)
   {
     vt.transf->upcbarriers.push_back(&n);
   }

   void handle(SgScopeStatement& n)
   {
     vt.transf->visit_isSgScopeStatement(&n);
   }

   void handle(SgClassDefinition& n)
   {
     SgScopeStatement& scope = n;

     handle(scope); // first handle as scope
     vt.transf->visit_isClassDefinition(&n);
   }

   void handle_gfor(SgScopeStatement& sgfor)
   {
     handle(sgfor); // as ScopeStatement

     vt.for_loops.push_back(&sgfor);
     ia.isForStatement = true;
   }

   void handle(SgForStatement& n)
   {
     handle_gfor(n);
   }

   void handle(SgUpcForAllStatement& n)
   {
     handle_gfor(n);
   }

   //
   // Expressions
   //


   /// Visit pointer assignments whose lhs is computed from the original value of
   /// the pointer by virtue of the operator alone (e.g. ++, --)  As a heuristic,
   /// we say that such operations should not change the @e "Memory Chunk", i.e.
   /// the array the pointer refers to.
   void push_if_ptr_movement(SgExpression& astNode, SgExpression* operand)
   {
      if( isUsableAsSgPointerType( operand -> get_type() )) {
         // we don't care about int++, only pointers, or reference to pointers.
         vt.transf->pointer_movements.push_back( &astNode );
      }
   }

   // unary

   void handle(SgAddressOfOp& n)
   {
     ia.isAddressOfOp = true;
     /* returns immediately */
   }

   void handle(SgPointerDerefExp& n)
   {
      // if this is a varrefexp and it is not initialized, we flag it.
      // do only if it is by itself or on right hand side of assignment
      vt.transf->visit_isSgPointerDerefExp(&n);
   }


   // binary

   void handle(SgBinaryOp& n)
   {
      if (ia.isArrowExp || ia.isAddressOfOp) return;

      vt.binary_ops.push_back(&n);
      ia.isBinaryOp = true;
   }

   void handle_binary(SgBinaryOp& n) { handle(n); }  // implcitely casts to SgBinaryOp

   void handle(SgDotExp&) { /* skip binary handling */ }

   void handle(SgPntrArrRefExp& n)
   {
     vt.transf->visit_isArrayPntrArrRefExp(&n);
     handle_binary(n);
   }

   void handle(SgArrowExp& n)
   {
     vt.transf->visit_isSgArrowExp(&n);
     ia.isArrowExp = true;
     /* skip binary handling */
   }

   void handle(SgAssignOp& n)
   {
      // 1. look for MALLOC
      // 2. Look for assignments to variables - i.e. a variable is initialized
      // 3. Assign variables that come from assign initializers (not just assignments
      std::cerr << n.unparseToString() << std::endl;
      vt.transf->visit_isArraySgAssignOp(&n);
      handle_binary(n);
   }

   void handle(SgMinusAssignOp& n)
   {
     push_if_ptr_movement(n, n.get_lhs_operand());
     handle_binary(n);
   }

   void handle(SgPlusAssignOp& n)
   {
     push_if_ptr_movement(n, n.get_lhs_operand());
     handle_binary(n);
   }

   // n-ary

   void handle(SgFunctionCallExp& n)
   {
      vt.transf->visit_isFunctionCall(&n);
   }

   void handle(SgPlusPlusOp& n)
   {
     push_if_ptr_movement(n, n.get_operand());
   }

   void handle(SgMinusMinusOp& n)
   {
     push_if_ptr_movement(n, n.get_operand());
   }

   void handle(SgDeleteExp& del)
   {
      typedef RtedTransformation::Deallocations Deallocations;

      const AllocKind allocKind = (del.get_is_array() ? akCxxArrayNew : akCxxNew);

      vt.transf->frees.push_back( Deallocations::value_type(&del, allocKind) );
   }

   operator InheritedAttribute()
   {
     return ia;
   }
};

InheritedAttribute VariableTraversal::evaluateInheritedAttribute(SgNode* astNode, InheritedAttribute inheritedAttribute)
{
  return ez::visitSgNode(InheritedAttributeHandler(*this, inheritedAttribute), astNode);
}

/// wraps RtedTransformation::isUsableAsSgArrayType and makes sure that type is
/// not null.
static
bool isUsableAsSgArrayType(RtedTransformation* transf, SgType* type)
{
  return (type != NULL) && transf->isUsableAsSgArrayType(type);
}

/// wraps RtedTransformation::isUsableAsSgReferenceType and makes sure that
/// type is not null.
static
bool isUsableAsSgReferenceType(RtedTransformation* transf, SgType* type)
{
  return (type != NULL) && transf->isUsableAsSgReferenceType(type);
}

/// \brief tests if the first argument is an ancestor of the second argument
/// \note  see also SageInterface::isAncestor (which is defined over arbitrary
///        nodes).
static
bool isAncestorOf(const SgExpression& ancestor, const SgExpression& expr)
{
  const SgExpression* parent = isSgExpression(expr.get_parent());

  return (  (parent != NULL)
         && ( (&ancestor == parent) || isAncestorOf(ancestor, *parent) )
         );
}

/// tests if the second argument is (or is part of) the upc_forall loop's
/// affinity expression.
static
bool partOfUpcForallAffinityExpr(const SgUpcForAllStatement& upcForall, const SgExpression& expr)
{
  const SgExpression* affinity = upcForall.get_affinity();

  ROSE_ASSERT(affinity != NULL);
  return (affinity == &expr || isAncestorOf(*affinity, expr));
}

/// tests if the first argument is an upc_forall loop AND if the second
/// argument occurs in the context of the former's affinity expression.
static
bool partOfUpcForallAffinityExpr(const SgStatement& forLoop, const SgExpression& expr)
{
  const SgUpcForAllStatement* upcForall = isSgUpcForAllStatement(&forLoop);

  return (upcForall != NULL && partOfUpcForallAffinityExpr(*upcForall, expr));
}

/// \brief   defines patterns of binary operations that should not be tested
/// \return  true, if an access guard should be skipped
///          false, otherwise
static
bool test_binary_op(const VariableTraversal::BinaryOpStack& binary_ops, const SgVarRefExp& varref)
{
  if (binary_ops.empty()) return false;

  const SgBinaryOp*   binop = binary_ops.back();
  const SgExpression* rhs = binop->get_rhs_operand();

  // \note while astNode is on the right hand side of some expression
  //       this does not mean that astNode == rhs. astNode could also be
  //       an (indirect) child of rhs.
  return (  !isRightOfBinaryOp(&varref)
         || isSgArrayType(rhs->get_type())
         || isSgNewExp(rhs)
         || isSgReferenceType(binop->get_lhs_operand()->get_type())
         );
}

/// \brief   tests whether a varref is related to a for loop
/// \return  true, iff an access guard test should be skipped
/// \details returns true if the accessed variable is initialized in a for loop
///          or if the variable is accessed in the context of a upc_forall.
/// \note    currently we do not test whether the UPC affinity could be out of
///          bounds.
static
bool test_for_loops(const VariableTraversal::LoopStack& for_loops, const SgVarRefExp& varref, const SgInitializedName& varname)
{
  if (for_loops.empty()) return false;

  SgStatement* forloop = for_loops.back();

  return (  ::isInitializedNameInForStatement(GeneralizdFor::initializer(forloop), varname)
         || ::partOfUpcForallAffinityExpr(*forloop, varref)
         );
}

/// \brief   tests whether a varref is related to an assign initializer
static
bool test_assign_initializer(const InheritedAttribute& inh, const SgVarRefExp& varref)
{
   if (!inh.isAssignInitializer) return false;

   SgInitializedName* initName = isSgInitializedName(varref.get_parent() ->get_parent()-> get_parent());
   if (initName == NULL)
      initName = isSgInitializedName(varref.get_parent() ->get_parent());

   return (initName && isSgReferenceType(initName -> get_type()));
}


/// \brief   tests whether a varref is related to a function call
/// \note    do we assume that the function has a declaration?
/// \todo    Can't we take the type from the call expression
///          (in case that the callee is computed)? (PP)
static
bool test_call_argument(RtedTransformation* transf, const SgVarRefExp& varref)
{
  const SgExprListExp* exprl = isSgExprListExp(varref.get_parent());
  if (exprl == NULL) return false;

  const SgFunctionCallExp* callexp = isSgFunctionCallExp(exprl->get_parent());
  if (callexp == NULL) return false;

  // try to determine the parameter type
  SgType* param_type = NULL;
  const SgFunctionDeclaration* fndecl = callexp->getAssociatedFunctionDeclaration();

  if (fndecl)
  {
    size_t                     arg_pos = 0;
    const SgExpressionPtrList& args = exprl->get_expressions();

    ROSE_ASSERT(args.size() > 0); // at least varref is in args

    while (args[arg_pos] != &varref)
    {
      ++arg_pos;
      ROSE_ASSERT( arg_pos < args.size() );
    }

    SgInitializedNamePtrList& param_lst = fndecl->get_parameterList()->get_args();
    ROSE_ASSERT(arg_pos < param_lst.size());

    if (param_lst[arg_pos] != NULL)
       param_type = param_lst[arg_pos] -> get_type();
  }

  // \pp why do we test for SgArrayType here? (PP)
  return (  ::isUsableAsSgArrayType(transf, varref.get_type())
         || ::isUsableAsSgReferenceType(transf, param_type)
         );
}


/// wraps RtedTransformation::isInInstrumentedFile and makes sure that
/// the argument is not null.
static
bool in_instrumented_file(RtedTransformation* transf, SgInitializedName* name)
{
  return (name != NULL) && !transf->isInInstrumentedFile(name -> get_declaration());
}


void VariableTraversal::handleIfVarRefExp(SgVarRefExp* varref, const InheritedAttribute& inheritedAttribute)
{
  if (varref == NULL) return;

  SgInitializedName *name = varref -> get_symbol() -> get_declaration();

  const bool elide_access_guard = (  inheritedAttribute.isArrowExp
                                  || inheritedAttribute.isAddressOfOp
                                  || in_instrumented_file(transf, name)
                                  || test_binary_op(binary_ops, *varref)
                                  || test_for_loops(for_loops, *varref, *name)
                                  || test_assign_initializer(inheritedAttribute, *varref)
                                  || test_call_argument(transf, *varref)
                                  );

  if (elide_access_guard) return;

  // its a plain variable access
  transf->variable_access_varref.push_back(varref);

  //~ std::cerr << " @@@@@@@@@ ADDING Variable access : " << varref->unparseToString() << "  vec size: "
  //~           << varref->get_parent()->unparseToString() << std::endl;
}


SynthesizedAttribute VariableTraversal::evaluateSynthesizedAttribute(SgNode* astNode, InheritedAttribute inheritedAttribute,
      SynthesizedAttributesList childAttributes) {
   SynthesizedAttribute localResult = std::accumulate(childAttributes.begin(), childAttributes.end(), false,
         std::logical_or<bool>());

   if (!inheritedAttribute.function) return localResult;

   // take from stacks, if applicable
   if (GeneralizdFor::is(astNode))
   {
     ROSE_ASSERT(!for_loops.empty());
     ROSE_ASSERT(astNode == for_loops.back());
     for_loops.pop_back();
   }
   else if (isSgBinaryOp(astNode) && !inheritedAttribute.isArrowExp && !inheritedAttribute.isAddressOfOp && !isSgDotExp(astNode))
   {
     ROSE_ASSERT(!binary_ops.empty() && astNode == binary_ops.back());
     binary_ops.pop_back();
   }

   // ------------------------------ visit isSgVarRefExp ----------------------------------------------
   handleIfVarRefExp(isSgVarRefExp(astNode), inheritedAttribute);

   return localResult;
}

#endif
