// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
#include "rose.h"

// DQ (2/9/2010): Testing use of ROSE to compile ROSE.
#ifndef USE_ROSE

#include <algorithm>
#include <functional>
#include <numeric>

#include <string>
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

InheritedAttribute VariableTraversal::evaluateInheritedAttribute(SgNode* astNode, InheritedAttribute inheritedAttribute) {

   if (isSgFunctionDefinition(astNode)) {
      // ------------------------------ visit isSgFunctionDefinition ----------------------------------------------
      transf->visit_checkIsMain(astNode);
      transf->function_definitions.push_back(isSgFunctionDefinition(astNode));
      return InheritedAttribute(true, inheritedAttribute.isAssignInitializer, inheritedAttribute.isArrowExp,
            inheritedAttribute.isAddressOfOp, inheritedAttribute.isForStatement, inheritedAttribute.isBinaryOp);
   }

   if (isSgVariableDeclaration(astNode) && !isSgClassDefinition(isSgVariableDeclaration(astNode) -> get_parent())) {
      // ------------------------------ visit Variable Declarations ----------------------------------------------
      Rose_STL_Container<SgInitializedName*> vars = isSgVariableDeclaration(astNode)->get_variables();
      for (Rose_STL_Container<SgInitializedName*>::const_iterator it = vars.begin();it!=vars.end();++it) {
         SgInitializedName* initName = *it;
         ROSE_ASSERT(initName);
         if( isSgReferenceType( initName -> get_type() ))
         continue;
         transf->variable_declarations.push_back(initName);
      }
   }

   if (isSgInitializedName(astNode)) {
      // ------------------------------ visit isSgInitializedName ----------------------------------------------
      ROSE_ASSERT(isSgInitializedName(astNode)->get_typeptr());
      SgArrayType* array = isSgArrayType(isSgInitializedName(astNode)->get_typeptr());
      SgNode* gp = astNode -> get_parent() -> get_parent();
      // something like: struct type { int before; char c[ 10 ]; int after; }
      // does not need a createarray call, as the type is registered and any array
      // information will be tracked when variables of that type are created.
      // ignore arrays in parameter lists as they're actually pointers, not stack arrays
      if ( array && !( isSgClassDefinition( gp )) && !( isSgFunctionDeclaration( gp ) )) {
         RTedArray* arrayRted = new RTedArray(isSgInitializedName(astNode), NULL, false);
         transf->populateDimensions( arrayRted, isSgInitializedName(astNode), array );
         transf->create_array_define_varRef_multiArray_stack[isSgInitializedName(astNode)] = arrayRted;
      }
   }

   if (isSgAssignOp(astNode)) {
      // 1. look for MALLOC
      // 2. Look for assignments to variables - i.e. a variable is initialized
      // 3. Assign variables that come from assign initializers (not just assignments
      std::cerr << astNode->unparseToString() << std::endl;
      transf->visit_isArraySgAssignOp(astNode);
   }


   if (isSgAssignInitializer(astNode)) {
      // ------------------------------  DETECT ALL array creations  ----------------------------------------------
      std::cerr << astNode->unparseToString() << std::endl;
      transf->visit_isAssignInitializer(astNode);
      return InheritedAttribute(inheritedAttribute.function, true, inheritedAttribute.isArrowExp,
            inheritedAttribute.isAddressOfOp, inheritedAttribute.isForStatement, inheritedAttribute.isBinaryOp);
   }


   if (isSgPntrArrRefExp(astNode)) {
      // ------------------------------ checks for array access  ----------------------------------------------
      transf->visit_isArrayPntrArrRefExp(astNode);
   } // pntrarrrefexp

   if (isSgPointerDerefExp(astNode)) {
      // if this is a varrefexp and it is not initialized, we flag it.
      // do only if it is by itself or on right hand side of assignment
      transf->visit_isSgPointerDerefExp(isSgPointerDerefExp(astNode));
   }



   if (isSgArrowExp(astNode)) {
      // if this is a varrefexp and it is not initialized, we flag it.
      // do only if it is by itself or on right hand side of assignment
      transf->visit_isSgArrowExp(isSgArrowExp(astNode));
   return InheritedAttribute(inheritedAttribute.function, inheritedAttribute.isAssignInitializer, true,
         inheritedAttribute.isAddressOfOp, inheritedAttribute.isForStatement, inheritedAttribute.isBinaryOp);
   }

   transf->visit(astNode);
#if 0
   if (isSgScopeStatement(astNode)) {
      // if, while, do, etc., where we need to check for locals going out of scope
      transf->visit_isSgScopeStatement(astNode);
      // *********************** DETECT structs and class definitions ***************
      if (isSgClassDefinition(astNode)) {
         // call to a specific function that needs to be checked
         transf->visit_isClassDefinition(isSgClassDefinition(astNode));
      }
   }
#endif


   if (isSgFunctionCallExp(astNode)) {
      // call to a specific function that needs to be checked
      transf->visit_isFunctionCall(astNode);
   }

   if( isSgPlusPlusOp( astNode ) || isSgMinusMinusOp( astNode )
         || isSgMinusAssignOp( astNode ) || isSgPlusAssignOp( astNode )) {
      // ------------------------------  Detect pointer movements, e.g ++, --  ----------------------------------------------
      ROSE_ASSERT( isSgUnaryOp( astNode ) || isSgBinaryOp( astNode ) );
      SgExpression* operand = NULL;
      if( isSgUnaryOp( astNode ) )
      operand = isSgUnaryOp( astNode ) -> get_operand();
      else if( isSgBinaryOp( astNode ) )
      operand = isSgBinaryOp( astNode ) -> get_lhs_operand();
      if( transf->isUsableAsSgPointerType( operand -> get_type() )) {
         // we don't care about int++, only pointers, or reference to pointers.
         transf->pointer_movements.push_back( isSgExpression( astNode ));
      }
   }

   if( isSgDeleteExp( astNode )) {
      // ------------------------------ Detect delete (c++ free) ----------------------------------------------
      transf->frees.push_back( isSgDeleteExp( astNode ) );
   }

   if (isSgReturnStmt(astNode)) {
      // ------------------------------ visit isSgReturnStmt ----------------------------------------------
      if (isSgReturnStmt(astNode)->get_expression())
      transf->returnstmt.push_back(isSgReturnStmt(astNode));
   }


   if (isSgAddressOfOp(astNode))
   return InheritedAttribute(inheritedAttribute.function, inheritedAttribute.isAssignInitializer,
         inheritedAttribute.isArrowExp, true, inheritedAttribute.isForStatement, inheritedAttribute.isBinaryOp);

   if (SgStatement* forLoop = GeneralizdFor::is(astNode))
   {
      for_loops.push_back(forLoop);
      return InheritedAttribute(inheritedAttribute.function, inheritedAttribute.isAssignInitializer,
            inheritedAttribute.isArrowExp, inheritedAttribute.isAddressOfOp, true, inheritedAttribute.isBinaryOp);
   }

   if (isSgBinaryOp(astNode) && !inheritedAttribute.isArrowExp && !inheritedAttribute.isAddressOfOp && !isSgDotExp(astNode)) {
      binary_ops.push_back(isSgBinaryOp(astNode));
      return InheritedAttribute(inheritedAttribute.function, inheritedAttribute.isAssignInitializer,
            inheritedAttribute.isArrowExp, inheritedAttribute.isAddressOfOp, inheritedAttribute.isForStatement, true);
   }

   return inheritedAttribute;
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
     ROSE_ASSERT(!for_loops.empty() && astNode == for_loops.back());
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
