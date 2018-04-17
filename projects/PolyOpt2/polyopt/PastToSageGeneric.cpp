/*
 * PastToSageGeneric.cpp: This file is part of the PolyOpt project.
 *
 * PolyOpt: a Polyhedral Optimizer for the ROSE compiler
 *
 * Copyright (C) 2011 the Ohio State University
 *
 * This program can be redistributed and/or modified under the terms
 * of the license specified in the LICENSE.txt file at the root of the
 * project.
 *
 * Contact: P. Sadayappan <saday@cse.ohio-state.edu>
 *
 */
/**
 * @file: PastToSageGeneric.cpp
 * @author: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 */

#include <fstream>

#include <polyopt/SageToPast.hpp>

#include <AstTraversal.h>
#include <sageInterface.h>
#include <sageBuilder.h>

#include <polyopt/PastToSageGeneric.hpp>
#include <polyopt/SageNodeAnnotation.hpp>
#include <polyopt/SageTools.hpp>

#ifdef CLOOG_INT_GMP
# define CLOOG_GET_SI(x) mpz_get_si(x)
#else
# define CLOOG_GET_SI(x) x
#endif


#include <past/past_api.h>
#include <past/pprint.h>


SgType*
PastToSageGeneric::build_sage_type(s_past_node_t* type, s_past_node_t* arr)
{
  if (past_node_is_a (type, past_type))
    {
      PAST_DECLARE_TYPED(type, pt, type);
      SgType* t = NULL;
      if (past_node_is_a (pt->texpr, past_generic))
	{
	  PAST_DECLARE_TYPED(generic, pg, pt->texpr);
	  t = (SgType*) pg->ptr_data;
	}
      else if (past_node_is_a (pt->texpr, past_string))
	{
	  PAST_DECLARE_TYPED(string, ps, pt->texpr);
	  t = SageBuilder::buildOpaqueType(ps->data, _scope);
	}
      else if (past_node_is_a (pt->texpr, past_varref))
	{
	  PAST_DECLARE_TYPED(varref, pv, pt->texpr);
	  if (! strcmp (pv->symbol->name_str, "double"))
	    t = SageBuilder::buildDoubleType();
	  else if (! strcmp (pv->symbol->name_str, "float"))
	    t = SageBuilder::buildFloatType();
	  else if (! strcmp (pv->symbol->name_str, "int"))
	    t = SageBuilder::buildIntType();
	  else
	    t = SageBuilder::buildOpaqueType(pv->symbol->name_str, _scope);
	}
      if (t && arr)
	{
	  ROSE_ASSERT(past_node_is_a (arr, past_arrayref));
	  SgExpression* expr = NULL;
	  while (arr && past_node_is_a (arr, past_arrayref))
	    {
	      PAST_DECLARE_TYPED(binary, pb, arr);
	      SgNode* e = converter (pb->rhs);
	      if (expr == NULL)
		expr = isSgExpression (e);
	      else
		expr = SageBuilder::buildBinaryExpression<SgPntrArrRefExp>
		  (expr, isSgExpression(e));
	      arr = pb->lhs;
	    }
	  ROSE_ASSERT(expr);
	  return SageBuilder::buildArrayType (t, expr);
	}

      return t;
    }
  if (type == NULL)
    {
      // Default type for opaque/untyped variables: int.
      return SageBuilder::buildIntType();
    }
  ROSE_ASSERT(! "Incorrect type tree");
  return SageBuilder::buildIntType();
}

static
SgExpression* buildCeildFloord(s_past_node_t* node,
			       SgExpression* lhs,
			       SgExpression* rhs)
{
  SgExpression* retExpr = NULL;

  if (past_node_is_a(node, past_floord))
    {
      SgExpression*   numerator;
      SgExpression*   denominator;
      SgExpression*   condExpr;
      SgExpression*   subExpr1;
      SgExpression*   subExpr2;
      SgExpression*   subExpr3;
      SgExpression*   subExpr4;
      SgExpression*   trueExpr;
      SgExpression*   falseExpr;
      SgExpression*   innerExpr;

      numerator = lhs;
      denominator = rhs;

      // floord(n, d) is defined as: (((n*d)<0) ? (((d)<0) ? -((-(n)+(d)+1)/(d)) : -((-(n)+(d)-1)/(d))) : (n)/(d))

      // (1) Build (d < 0) conditional
      condExpr = SageBuilder::buildBinaryExpression<SgLessThanOp>(denominator, SageBuilder::buildIntVal(0));

      // (2) Build -((-(n)+(d)+1)/(d)) expression
      subExpr1 = SageBuilder::buildUnaryExpression<SgMinusOp>(numerator);
      subExpr2 = SageBuilder::buildBinaryExpression<SgAddOp>
	(subExpr1, SageInterface::copyExpression(denominator));
      subExpr3 = SageBuilder::buildBinaryExpression<SgAddOp>
	(subExpr2, SageBuilder::buildIntVal(1));
      subExpr4 = SageBuilder::buildBinaryExpression<SgDivideOp>
	(subExpr3, SageInterface::copyExpression(denominator));
      trueExpr = SageBuilder::buildUnaryExpression<SgMinusOp>(subExpr4);

      // (3) Build -((-(n)+(d)-1)/(d)) expression
      subExpr1 = SageBuilder::buildUnaryExpression<SgMinusOp>
	(SageInterface::copyExpression(numerator));
      subExpr2 = SageBuilder::buildBinaryExpression<SgAddOp>
	(subExpr1, SageInterface::copyExpression(denominator));
      subExpr3 = SageBuilder::buildBinaryExpression<SgSubtractOp>
	(subExpr2, SageBuilder::buildIntVal(1));
      subExpr4 = SageBuilder::buildBinaryExpression<SgDivideOp>
	(subExpr3, SageInterface::copyExpression(denominator));
      falseExpr = SageBuilder::buildUnaryExpression<SgMinusOp>(subExpr4);

      // (4) Build (cond) ? (true) : (false) expression
      innerExpr =
	SageBuilder::buildConditionalExp(condExpr, trueExpr, falseExpr);

      // (5) The generated conditional expression is our new "true" expression
      trueExpr = innerExpr;

      // (6) Build (n)/(d) expression
      falseExpr = SageBuilder::buildBinaryExpression<SgDivideOp>
	(SageInterface::copyExpression(numerator),
	 SageInterface::copyExpression(denominator));

      // (7) Build (n*d) < 0 conditional
      subExpr1 = SageBuilder::buildBinaryExpression<SgMultiplyOp>
	(SageInterface::copyExpression(numerator),
	 SageInterface::copyExpression(denominator));
      condExpr = SageBuilder::buildBinaryExpression<SgLessThanOp>
	(subExpr1, SageBuilder::buildIntVal(0));

      // (8) Build (cond) ? (true) : (false) expression
      retExpr = SageBuilder::buildConditionalExp(condExpr, trueExpr, falseExpr);
    }
  else if (past_node_is_a(node, past_ceild))
    {
      SgExpression*   numerator;
      SgExpression*   denominator;
      SgExpression*   condExpr;
      SgExpression*   subExpr1;
      SgExpression*   subExpr2;
      SgExpression*   subExpr3;
      SgExpression*   subExpr4;
      SgExpression*   subExpr5;
      SgExpression*   trueExpr;
      SgExpression*   falseExpr;
      SgExpression*   innerExpr;

      numerator = lhs;
      denominator = rhs;

      // ceil(n, d) is defined as: (((n*d)<0) ? -((-(n))/(d)) : (((d)<0) ? (((-n)+(-d)-1)/(-d)) :  ((n)+(d)-1)/(d)))

      // (1) Build (d < 0) conditional
      condExpr = SageBuilder::buildBinaryExpression<SgLessThanOp>
	(denominator, SageBuilder::buildIntVal(0));

      // (2) Build ((-(n)+(-d)-1)/(-d)) expression
      subExpr1 = SageBuilder::buildUnaryExpression<SgMinusOp>(numerator);
      subExpr2 = SageBuilder::buildUnaryExpression<SgMinusOp>
	(SageInterface::copyExpression(denominator));
      subExpr3 = SageBuilder::buildBinaryExpression<SgAddOp>
	(subExpr1, subExpr2);
      subExpr4 = SageBuilder::buildBinaryExpression<SgSubtractOp>
	(subExpr3, SageBuilder::buildIntVal(1));
      subExpr5 = SageBuilder::buildUnaryExpression<SgMinusOp>
	(SageInterface::copyExpression(denominator));
      trueExpr = SageBuilder::buildBinaryExpression<SgDivideOp>
	(subExpr4, subExpr5);

      // (3) Build -(((n)+(d)-1)/(d)) expression
      subExpr1 = SageBuilder::buildBinaryExpression<SgAddOp>
	(SageInterface::copyExpression(numerator),
	 SageInterface::copyExpression(denominator));
      subExpr2 = SageBuilder::buildBinaryExpression<SgSubtractOp>
	(subExpr1, SageBuilder::buildIntVal(1));
      falseExpr = SageBuilder::buildBinaryExpression<SgDivideOp>
	(subExpr2, SageInterface::copyExpression(denominator));

      // (4) Build (cond) ? (true) : (false) expression
      innerExpr = SageBuilder::buildConditionalExp
	(condExpr, trueExpr, falseExpr);

      // (5) The generated conditional expression is our new "false" expression
      falseExpr = innerExpr;

      // (6) Build -(-(n)/(d)) expression
      subExpr1 = SageBuilder::buildUnaryExpression<SgMinusOp>
	(SageInterface::copyExpression(numerator));
      subExpr2 = SageBuilder::buildBinaryExpression<SgDivideOp>
	(subExpr1, SageInterface::copyExpression(denominator));
      trueExpr = SageBuilder::buildUnaryExpression<SgMinusOp>(subExpr2);

      // (7) Build (n*d) < 0 conditional
      subExpr1 = SageBuilder::buildBinaryExpression<SgMultiplyOp>
	(SageInterface::copyExpression(numerator),
	 SageInterface::copyExpression(denominator));
      condExpr = SageBuilder::buildBinaryExpression<SgLessThanOp>
	(subExpr1, SageBuilder::buildIntVal(0));

      // (8) Build (cond) ? (true) : (false) expression
      retExpr = SageBuilder::buildConditionalExp(condExpr, trueExpr, falseExpr);

    }

  return retExpr;
}


SgNode*
PastToSageGeneric::past_ternary_to_funcall (s_past_node_t* binop,
					    SgExpression* arg1,
					    SgExpression* arg2,
					    SgExpression* arg3)
{
  int i;
  PAST_DECLARE_TYPED(ternary, pb, binop);
  for (i = 0; simd_function_table && simd_function_table[i].type; ++i)
    {
      if (simd_function_table[i].type == binop->type)
	{
	  const char* str = NULL;
	  switch (pb->op_type)
	    {
	    case e_past_sse_single_op_type:
	      str = simd_function_table[i].sse_op_single; break;
	    case e_past_sse_double_op_type:
	      str = simd_function_table[i].sse_op_double; break;
	    case e_past_avx_single_op_type:
	      str = simd_function_table[i].avx_op_single; break;
	    case e_past_avx_double_op_type:
	      str = simd_function_table[i].avx_op_double; break;
	    case e_past_avx2_single_op_type:
	      str =  simd_function_table[i].avx2_op_single; break;
	    case e_past_avx2_double_op_type:
	      str = simd_function_table[i].avx2_op_double; break;
	    default: assert(! "Unsupported operation type"); return NULL;
	    }

	  // Build function call. The function must be defined
	  // somewhere in the project.
	  SgFunctionDeclaration* d = SageInterface::findFunctionDeclaration
	    (SageInterface::getProject(), str, NULL, false);
	  if (! d)
	    std::cout << "[PolOpt][ERROR] Cannot locate declaration/definition for function \"" << str << "\"" << std::endl;
	  ROSE_ASSERT(d);
	  return SageBuilder::buildFunctionCallExp
	    (SageBuilder::buildFunctionRefExp(d),
	     SageBuilder::buildExprListExp (arg1, arg2, arg3));
	}
    }
  return NULL;
}


SgNode*
PastToSageGeneric::past_binary_to_funcall (s_past_node_t* binop,
					   SgExpression* lhs,
					   SgExpression* rhs)
{
  int i;
  PAST_DECLARE_TYPED(binary, pb, binop);
  for (i = 0; simd_function_table && simd_function_table[i].type; ++i)
    {
      if (simd_function_table[i].type == binop->type)
	{
	  const char* str = NULL;
	  switch (pb->op_type)
	    {
	    case e_past_sse_single_op_type:
	      str = simd_function_table[i].sse_op_single; break;
	    case e_past_sse_double_op_type:
	      str = simd_function_table[i].sse_op_double; break;
	    case e_past_avx_single_op_type:
	      str = simd_function_table[i].avx_op_single; break;
	    case e_past_avx_double_op_type:
	      str = simd_function_table[i].avx_op_double; break;
	    case e_past_avx2_single_op_type:
	      str =  simd_function_table[i].avx2_op_single; break;
	    case e_past_avx2_double_op_type:
	      str = simd_function_table[i].avx2_op_double; break;
	    default: assert(! "Unsupported operation type"); return NULL;
	    }

	  // Build function call. The function must be defined
	  // somewhere in the project.
	  SgFunctionDeclaration* d = SageInterface::findFunctionDeclaration
	    (SageInterface::getProject(), str, NULL, false);
	  if (! d)
	    std::cout << "[PolOpt][ERROR] Cannot locate declaration/definition for function \"" << str << "\"" << std::endl;
	  ROSE_ASSERT(d);
	  return SageBuilder::buildFunctionCallExp
	    (SageBuilder::buildFunctionRefExp(d),
	     SageBuilder::buildExprListExp (lhs, rhs));
	}
    }
  return NULL;
}



SgNode* PastToSageGeneric::convert(s_past_node_t* node)
{
  // Pretty-print table for SIMD operations.
  struct PastToSageGeneric::pprint_op pprint_table[] = {
    /* Ternary nodes.	*/
    {past_fma, "pocc_fma", NULL, NULL, NULL, NULL, "_mm256_fmadd_ps", "_mm256_fmadd_pd"},
    {past_fms, "pocc_fms", NULL, NULL, NULL, NULL, "_mm256_fmsub_ps", "_mm256_fmsub_pd"},
    /* Binary nodes.*/
    {past_add, "+", "_mm_add_ps", "_mm_add_pd", "_mm256_add_ps", "_mm256_add_pd","_mm256_add_ps", "_mm256_add_pd"},
    {past_sub, "-", "_mm_sub_ps", "_mm_sub_pd", "_mm256_sub_ps", "_mm256_sub_pd","_mm256_sub_ps", "_mm256_sub_pd"},
    {past_mul, "*", "_mm_mul_ps", "_mm_mul_pd", "_mm256_mul_ps", "_mm256_mul_pd","_mm256_mul_ps", "_mm256_mul_pd"},
    {past_div, "/", "_mm_div_ps", "_mm_div_pd", "_mm256_div_ps", "_mm256_div_pd","_mm256_div_ps", "_mm256_div_pd"},
    /* prefix unaries.  */
    {past_sqrt, "sqrt", "_mm_sqrt_ps", "_mm_sqrt_pd", "_mm256_sqrt_ps", "_mm256_sqrt_pd", "_mm256_sqrt_ps", "_mm256_sqrt_pd"},
    { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}
  };
  simd_function_table = pprint_table;


  // Be safe: make sure the AST is well formed.
  past_ast_fixup_structure (node);

  // Update symbol table as needed.
  past_update_symbols_with_declaration (node);
  past_rebuild_symbol_table (node);
  if (past_node_is_a (node, past_root))
    {
      PAST_DECLARE_TYPED(root, pr, node);
      s_symbol_table_t* symt = pr->symboltable;
      // Update all symbols generated by PoCC with identical names /
      // structure description to point to have the same 'data' field.
      s_symbol_t* s1;
      s_symbol_t* s2;
      for (s1 = symt->symbols; s1; s1 = s1->next)
	for (s2 = symt->symbols; s2; s2 = s2->next)
	  {
	    if (s1->generated || s2->generated)
	      continue;
	    if (s1->data && s2->data)
	      continue;
	    if (s1->declaration && s2->declaration &&
		s1->declaration != s2->declaration)
	      continue;
	    void* s1data = s1->data;
	    void* s2data = s2->data;
	    s1->data = NULL;
	    s2->data = NULL;
	    s_past_node_t* s1decl = s1->declaration;
	    s_past_node_t* s2decl = s2->declaration;
	    s1->declaration = NULL;
	    s2->declaration = NULL;
	    if (symbol_equal (s1, s2))
	      {
		s1->data = s2->data = s1data == NULL ? s2data : s1data;
		if (s1decl && s2decl)
		  {
		    s1->declaration = s1decl;
		    s2->declaration = s2decl;
		  }
		else
		  s1->declaration = s2->declaration =
		    s1decl == NULL ? s2decl : s1decl;
	      }
	    else
	      {
	    	s1->data = s1data;
	    	s2->data = s2data;
		s1->declaration = s1decl;
		s2->declaration = s2decl;
	      }
	  }
      // Update all symbols generated by PoCC w/o data field to point to the
      // same
      SgScopeStatement* topScope = SageInterface::getScope(_base);
      if (topScope)
	for (s1 = symt->symbols; s1; s1 = s1->next)
	  {
	    if (s1->name_str && ! s1->data)
	      {
		SgVariableSymbol* vs =
		  SageInterface::lookupVariableSymbolInParentScopes
		  (SgName(s1->name_str), topScope);
		if (vs)
		  {
		    ROSE_ASSERT (vs->get_declaration());
		    ROSE_ASSERT (vs->get_declaration()->get_parent ());
		    s1->data = vs->get_declaration();
		  }
	      }
	  }
    }
  else
    ROSE_ASSERT(! "WARNING: node is not a past_root\n");
  past_rebuild_symbol_table (node);

  // Output basic block.
  SgBasicBlock* bb = SageBuilder::buildBasicBlock();
  _scope = bb;

  // Convert.
  SgNode* ret = converter (node);

  // Insert variable declarations for opaque variables in PAST.
  for (std::vector<SgVariableDeclaration*>::iterator it = _declTemp.begin();
       it != _declTemp.end(); ++it)
    {
      // Hardcoding the generation of 'int __tilesizes = __OUTER_TILE;'.
      if (! strcmp ((*it)->get_definition ()->get_vardefn ()->
		    get_name ().getString ().c_str (), "__tilesize"))
	{
	  (*it)->reset_initializer
	    (SageBuilder::buildAssignInitializer
	     (SageBuilder::buildOpaqueVarRefExp("__OUTER_TILE", bb),
	      (*it)->get_type ()));

	}
      bb->append_statement (*it);
      SageInterface::fixVariableDeclaration(isSgVariableDeclaration(*it), bb);
    }
  bb->append_statement (isSgStatement (ret));

  return bb;
}

SgNode* PastToSageGeneric::converter(s_past_node_t* node)
{
  if (node == NULL)
    return SageBuilder::buildNullExpression();

  // past_root.
  if (past_node_is_a (node, past_root))
    {
      PAST_DECLARE_TYPED(root, pr, node);
      SgBasicBlock* bb = SageBuilder::buildBasicBlock();
      SgScopeStatement* old_scope = _scope;
      _scope = bb;
      s_past_node_t* body = pr->body;
      for (; body; body = body->next)
	{
	  SgNode* ret = converter (body);
	  if (! ret)
	    ret = SageBuilder::buildNullStatement();
	  SgStatement* stm = isSgStatement(ret);
	  ROSE_ASSERT(stm);
	  bb->append_statement (stm);
	}
      _scope = old_scope;
      return bb;
    }

  // past_block.
  else if (past_node_is_a (node, past_block))
    {
      PAST_DECLARE_TYPED(block, pr, node);
      SgBasicBlock* bb = SageBuilder::buildBasicBlock();
      SgScopeStatement* old_scope = _scope;
      _scope = bb;
      s_past_node_t* body = pr->body;
      for (; body; body = body->next)
	{
	  SgNode* ret = converter (body);
	  if (! ret)
	    ret = SageBuilder::buildNullStatement();
	  SgStatement* stm = isSgStatement(ret);
	  ROSE_ASSERT(stm);
	  if (past_node_is_a (body, past_block))
	    {
	      PAST_DECLARE_TYPED(block, pbt, body);
	      if (pbt->artificial_bb)
		SageInterface::appendStatementList
		  (isSgBasicBlock(stm)->get_statements(), bb);
	      else
		bb->append_statement (stm);
	    }
	  else
	    bb->append_statement (stm);
	}
      _scope = old_scope;
      return bb;
    }

  // past_statement. May be a variable declaration or a "regular" statement.
  else if (past_node_is_a (node, past_statement))
    {
      PAST_DECLARE_TYPED(statement, pr, node);
      s_past_node_t* body = pr->body;
      SgExpression* initializer = NULL;
      s_past_node_t* vardecl_ref = NULL;
      s_past_node_t* vardecl_type = NULL;
      s_past_node_t* array_desc = NULL;
      if (past_node_is_a (body, past_assign))
	{
	  PAST_DECLARE_TYPED(binary, pb, body);
	  if (past_node_is_a (pb->lhs, past_vardecl))
	    {
	      // Variable declaration w/ initializer.
	      PAST_DECLARE_TYPED(vardecl, pv, pb->lhs);
	      if (past_node_is_a (pv->name, past_varref))
		{
		  // Scalar type.
		  vardecl_ref = pv->name;
		  vardecl_type = pv->type;
		}
	      else
		{
		  // Array type.
		  ROSE_ASSERT(past_node_is_a (pv->name, past_arrayref));
		  vardecl_ref = past_get_array_name (pv->name);
		  vardecl_type = pv->type;
		  array_desc = pv->name;
		}
	      // Variable declaration with initializer.
	      initializer = isSgExpression(converter (pb->rhs));
	      ROSE_ASSERT(initializer);
	    }
	}
      if (vardecl_ref || past_node_is_a (body, past_vardecl))
	{
	  // Variable declaration w/o initializer.
	  if (! vardecl_ref)
	    {
	      PAST_DECLARE_TYPED(vardecl, pv, body);
	      if (past_node_is_a (pv->name, past_varref))
		{
		  // Scalar type.
		  vardecl_ref = pv->name;
		  vardecl_type = pv->type;
		}
	      else
		{
		  // Array type.
		  ROSE_ASSERT(past_node_is_a (pv->name, past_arrayref));
		  vardecl_ref = past_get_array_name (pv->name);
		  vardecl_type = pv->type;
		  array_desc = pv->name;
		}
	    }
	  ROSE_ASSERT(past_node_is_a (vardecl_ref, past_varref));
	  PAST_DECLARE_TYPED(varref, v, vardecl_ref);
	  SgVariableDeclaration* decl = NULL;
	  SgAssignInitializer* init = NULL;
	  if (v->symbol->data)
	    {
	      SgInitializedName* initname =
		isSgInitializedName((SgInitializedName*) v->symbol->data);
	      ROSE_ASSERT(initname);
	      if (initializer)
		{
		  ROSE_ASSERT(initname->get_type());
		  init = SageBuilder::buildAssignInitializer
		    (initializer, initname->get_type());
		}
	      decl = SageBuilder::buildVariableDeclaration
		(initname->get_name(), initname->get_type(), init, _scope);
	      _declarations[v->symbol->name_str] =
		decl->get_definition ()->get_vardefn ();
	    }
	  else
	    {
	      if (initializer)
		{
		  SgType* inittype = build_sage_type (vardecl_type, array_desc);
		  ROSE_ASSERT(inittype);
		  init = SageBuilder::buildAssignInitializer
		    (initializer, inittype);
		}
	      /// FIXME: find available name in symbols.
	      decl =
		SageBuilder::buildVariableDeclaration
		(std::string((char*)v->symbol->name_str),
		 build_sage_type (vardecl_type, array_desc),
		 init, _scope);
	      _declarations[v->symbol->name_str] =
		decl->get_definition ()->get_vardefn ();
	    }
	  ROSE_ASSERT(decl);
	  return decl;
	}

      SgNode* res = converter (body);
      if (! res)
      	res = SageBuilder::buildNullStatement();
      SgStatement* stm = NULL;
      if (isSgExpression(res))
	return SageBuilder::buildExprStatement(isSgExpression(res));
      else if (isSgStatement(res))
	{
	  if (isSgVariableDeclaration(res))
	    SageInterface::fixVariableDeclaration(isSgVariableDeclaration(res),
						  _scope);
	  return res;
	}
      // Should never happen.
      ROSE_ASSERT(0);
    }

  // past_for.
  else if (past_node_is_a (node, past_for))
    {

      PAST_DECLARE_TYPED(for, pf, node);
      std::string pragmaClause;
      if (past_node_is_a (node, past_parfor))
      	{
      	  PAST_DECLARE_TYPED(parfor, parf, node);
      	  /// Compute the openmp pragma as needed, only
      	  /// for the outer-most parallel for loop.
      	  if (! past_get_enclosing_node (node, past_parfor))
      	    {
      	      pragmaClause = "#pragma omp parallel for ";
      	      // Collect all for loop iterators in the sub-tree, and
      	      // make them private.
      	      s_past_node_t** forLoops =
      		past_collect_nodetype (pf->body, past_for);
      	      std::set<std::string> iternames;
      	      int i;
      	      for (i = 0; forLoops && forLoops[i]; ++i)
      		{
      		  PAST_DECLARE_TYPED(for, localf, forLoops[i]);
      		  iternames.insert(std::string(localf->iterator->name_str));
      		}
      	      // FIXME: perform liveness analysis to compute all
      	      // actually private variables beyond loop iterators.


      	      if (iternames.size())
      		{
      		  pragmaClause += "private(";
      		  std::set<std::string>::iterator it = iternames.begin();
      		  while (it != iternames.end())
      		    {
      		      pragmaClause += *it;
      		      ++it;
      		      if (it != iternames.end())
      			pragmaClause += ",";
      		    }
      		  pragmaClause += ")";
      		}
      	    }
      	  else if (past_is_inner_for_loop (node))
      	    {
      	      // Compute the simd pragma as needed. Prefer OMP
      	      // parallelization to SIMD parallelization.
      	      pragmaClause = "#pragma ivdep\n#pragma simd";
      	    }
      	}
      SgNode* init = converter (pf->init);
      SgNode* test = converter (pf->test);
      SgNode* inc = converter (pf->increment);
      SgNode* body = converter (pf->body);
      if (! body)
	body = SageBuilder::buildNullStatement();
      if (! init)
	init = SageBuilder::buildNullStatement();
      if (! test)
	init = SageBuilder::buildNullStatement();
      if (! inc)
	init = SageBuilder::buildNullExpression();
      if (isSgExpression (init))
	init = SageBuilder::buildExprStatement (isSgExpression (init));
      if (isSgExpression (test))
	test = SageBuilder::buildExprStatement (isSgExpression (test));
      SgForStatement* forStmt =
	SageBuilder::buildForStatement (isSgStatement(init),
					isSgStatement(test),
					isSgExpression(inc),
					isSgStatement(body));
      if (pragmaClause != "")
	SageInterface::attachArbitraryText(forStmt, pragmaClause,
					   PreprocessingInfo::before);
      return forStmt;
    }

  // past_varref.
  else if (past_node_is_a (node, past_varref))
    {
      PAST_DECLARE_TYPED(varref, v, node);
      if (v->symbol->data)
	{
	  SgNode* symb = (SgNode*) v->symbol->data;
	  if (isSgFunctionSymbol(symb))
	    {
	      SgFunctionSymbol* fsymb = (SgFunctionSymbol*)symb;
	      if (fsymb->get_declaration ())
		return SageBuilder::buildFunctionRefExp
		  (fsymb->get_declaration ());
	      else
		{
		  if (SageInterface::is_Cxx_language ())
		    return SageBuilder::buildFunctionRefExp
		      (fsymb->get_name(), fsymb->get_type(), _scope);
		  else
		    return SageBuilder::buildFunctionRefExp
		      (fsymb->get_name(), _scope);
		}
	    }
	  else if (isSgInitializedName(symb))
	    {
	      SgInitializedName* isymb = (SgInitializedName*)symb;
	      ROSE_ASSERT (isymb->get_parent ());
	      return SageBuilder::buildVarRefExp (isymb);
	    }
	}
      else
	{
	  PAST_DECLARE_TYPED(funcall, parfunc, node->parent);
	  // Reference for a function name in a function call, outside
	  // of the file scope.
	  if (past_node_is_a (node->parent, past_funcall) &&
	      parfunc->name == node)
	    {
	      // Build function call. The function must be defined
	      // somewhere in the project.
	      SgFunctionDeclaration* d = SageInterface::findFunctionDeclaration
		(SageInterface::getProject(), v->symbol->name_str, NULL,
		 false);
	      if (! d)
		std::cout << "[PolOpt][ERROR] Cannot locate declaration/definition for function \"" << v->symbol->name_str << "\"" << std::endl;
	      ROSE_ASSERT(d);
	      return SageBuilder::buildFunctionRefExp(d);
	    }
	  // Reference for a variable, opaque.
	  if (_declarations[v->symbol->name_str] == NULL)
	    {
	      std::map<char*,SgInitializedName*>::iterator it;
	      for (it = _declarations.begin(); it != _declarations.end(); ++it)
		if (! strcmp (it->first, v->symbol->name_str) && it->second)
		  break;
	      if (it == _declarations.end())
		{
		  SgVariableDeclaration* decl =
		    SageBuilder::buildVariableDeclaration
		    (std::string(v->symbol->name_str),
		     build_sage_type (NULL, NULL), NULL, _scope);
		  _declarations[v->symbol->name_str] =
		    decl->get_definition ()->get_vardefn ();
		  _declTemp.push_back (decl);
		}
	      else
		_declarations[v->symbol->name_str] = it->second;
	    }
	  ROSE_ASSERT(_declarations[v->symbol->name_str]);
	  ROSE_ASSERT(isSgInitializedName(_declarations[v->symbol->name_str]));
	  ROSE_ASSERT(isSgInitializedName(_declarations[v->symbol->name_str])->get_parent ());
	  return
	    SageBuilder::buildVarRefExp(_declarations[v->symbol->name_str]);
	}
    }

  // past_funcall.
  else if (past_node_is_a (node, past_funcall))
    {
      PAST_DECLARE_TYPED(funcall, v, node);
      s_past_node_t* args;
      std::vector<SgExpression*> args_vec;
      for (args = v->args_list; args; args = args->next)
	{
	  SgExpression* e = isSgExpression (converter (args));
	  args_vec.push_back (e);
	}
      SgNode* ret = converter (v->name);
      if (isSgExpression(ret))
	return SageBuilder::buildFunctionCallExp
	  (isSgExpression(ret), SageBuilder::buildExprListExp (args_vec));
      // Unsupported cases.
      ROSE_ASSERT(0);
    }

  // past_arrayref.
  else if (past_node_is_a (node, past_arrayref))
    {
      PAST_DECLARE_TYPED(binary, v, node);
      return SageBuilder::buildPntrArrRefExp
	(isSgExpression (converter (v->lhs)),
	 isSgExpression (converter (v->rhs)));
    }

  // past_cast.
  else if (past_node_is_a (node, past_cast))
    {
      PAST_DECLARE_TYPED(cast, v, node);
      ROSE_ASSERT(past_node_is_a (v->type, past_type));
      PAST_DECLARE_TYPED(type, pt, v->type);
      if (past_node_is_a (pt->texpr, past_arrayref))
	return SageBuilder::buildCastExp
	  (isSgExpression (converter (v->expr)),
	   build_sage_type (v->type, pt->texpr));
      else
	return SageBuilder::buildCastExp
	  (isSgExpression (converter (v->expr)),
	   build_sage_type (v->type, NULL));
    }

  // past_affineguard.
  else if (past_node_is_a (node, past_affineguard))
    {
      PAST_DECLARE_TYPED(affineguard, v, node);
      return SageBuilder::buildIfStmt
	(isSgExpression (converter (v->condition)),
	 isSgStatement (converter (v->then_clause)), NULL);
    }

  // past_if.
  else if (past_node_is_a (node, past_if))
    {
      PAST_DECLARE_TYPED(if, v, node);
      return SageBuilder::buildIfStmt
	(isSgExpression (converter (v->condition)),
	 isSgStatement (converter (v->then_clause)),
	 isSgStatement (converter (v->else_clause)));
    }

  // past_ternary_cond.
  else if (past_node_is_a (node, past_ternary_cond))
    {
      PAST_DECLARE_TYPED(ternary_cond, v, node);
      return SageBuilder::buildConditionalExp
	(isSgExpression (converter (v->cond)),
	 isSgExpression (converter (v->true_clause)),
	 isSgExpression (converter (v->false_clause)));
    }

  // past_value.
  else if (past_node_is_a (node, past_value))
    {
      PAST_DECLARE_TYPED(value, v, node);
      switch (v->type)
	{
	case e_past_value_unknown:
	  return SageBuilder::buildIntVal(v->value.intval);
	case e_past_value_bool:
	  return SageBuilder::buildBoolValExp(v->value.boolval);
	case e_past_value_char:
	  return SageBuilder::buildCharVal(v->value.charval);
	case e_past_value_uchar:
	  return SageBuilder::buildUnsignedCharVal(v->value.ucharval);
	case e_past_value_int:
	  return SageBuilder::buildIntVal(v->value.intval);
	case e_past_value_uint:
	  return SageBuilder::buildUnsignedIntVal(v->value.uintval);
	case e_past_value_longint:
	  return SageBuilder::buildLongIntVal(v->value.longintval);
	case e_past_value_ulongint:
	  return SageBuilder::buildUnsignedLongVal(v->value.ulongintval);
	case e_past_value_longlongint:
	  return SageBuilder::buildLongLongIntVal(v->value.longlongintval);
	case e_past_value_ulonglongint:
	  return SageBuilder::buildUnsignedLongLongIntVal(v->value.ulonglongintval);
	case e_past_value_float:
	  return SageBuilder::buildFloatVal(v->value.floatval);
	case e_past_value_double:
	  return SageBuilder::buildDoubleVal(v->value.doubleval);
	case e_past_value_longdouble:
	  return SageBuilder::buildLongDoubleVal(v->value.longdoubleval);
	case e_past_value_longlong:
	  return SageBuilder::buildLongLongIntVal(v->value.longlongval);
	case e_past_value_short:
	  return SageBuilder::buildShortVal(v->value.shortval);
	case e_past_value_ushort:
	  return SageBuilder::buildUnsignedShortVal(v->value.ushortval);
	case e_past_value_wchar:
	  return SageBuilder::buildWcharVal(v->value.wcharval);
	default:
	  {
	    ROSE_ASSERT(0);
	    return NULL;
	  }
	}
    }

  // past_string.
  else if (past_node_is_a (node, past_string))
    {
      PAST_DECLARE_TYPED(string, v, node);
      return SageBuilder::buildStringVal(std::string(v->data));
    }

  // past_while.
  else if (past_node_is_a (node, past_while))
    {
      PAST_DECLARE_TYPED(while, v, node);
      return SageBuilder::buildWhileStmt
	(isSgExpression (converter (v->condition)),
	 isSgStatement (converter (v->body)));
    }

  // past_do_while.
  else if (past_node_is_a (node, past_do_while))
    {
      PAST_DECLARE_TYPED(do_while, v, node);
      return SageBuilder::buildDoWhileStmt
	(isSgStatement (converter (v->body)),
	 isSgExpression (converter (v->condition)));
    }

  // past_ternary
  else if (past_node_is_a (node, past_ternary))
    {
      PAST_DECLARE_TYPED(ternary, v, node);
      SgExpression* arg1 = (SgExpression*) converter (v->arg1);
      SgExpression* arg2 = (SgExpression*) converter (v->arg2);
      SgExpression* arg3 = (SgExpression*) converter (v->arg3);
      arg1 = isSgExpression (arg1);
      arg2 = isSgExpression (arg2);
      arg3 = isSgExpression (arg3);
      ROSE_ASSERT(arg1);
      ROSE_ASSERT(arg2);
      ROSE_ASSERT(arg3);
      SgNode* ret = NULL;
      if (v->op_type != e_past_unknown_op_type &&
	  v->op_type != e_past_scalar_op_type)
	ret = past_ternary_to_funcall (node, arg1, arg2, arg3);
      if (ret)
	return ret;
      ROSE_ASSERT(! "Error. Ternary node must be represented in the pprint table");
    }

  // past_binary
  else if (past_node_is_a (node, past_binary))
    {
      PAST_DECLARE_TYPED(binary, v, node);

      SgExpression* lhs = (SgExpression*) converter (v->lhs);
      SgExpression* rhs = (SgExpression*) converter (v->rhs);
      lhs = isSgExpression (lhs);
      rhs = isSgExpression (rhs);
      ROSE_ASSERT(lhs);
      ROSE_ASSERT(rhs);
      SgNode* ret = NULL;
      if (v->op_type != e_past_unknown_op_type &&
	  v->op_type != e_past_scalar_op_type)
	ret = past_binary_to_funcall (node, lhs, rhs);
      if (ret)
	return ret;
      if (past_node_is_a (node, past_add))
	return SageBuilder::buildBinaryExpression<SgAddOp>(lhs, rhs);
      if (past_node_is_a (node, past_sub))
	return SageBuilder::buildBinaryExpression<SgSubtractOp>(lhs, rhs);
      if (past_node_is_a (node, past_mul))
	return SageBuilder::buildBinaryExpression<SgMultiplyOp>(lhs, rhs);
      if (past_node_is_a (node, past_div))
	return SageBuilder::buildBinaryExpression<SgDivideOp>(lhs, rhs);
      if (past_node_is_a (node, past_mod))
	return SageBuilder::buildBinaryExpression<SgModOp>(lhs, rhs);
      if (past_node_is_a (node, past_and))
	return SageBuilder::buildBinaryExpression<SgAndOp>(lhs, rhs);
      if (past_node_is_a (node, past_or))
	return SageBuilder::buildBinaryExpression<SgOrOp>(lhs, rhs);
      if (past_node_is_a (node, past_equal))
	return SageBuilder::buildBinaryExpression<SgEqualityOp>(lhs, rhs);
      if (past_node_is_a (node, past_notequal))
	return SageBuilder::buildBinaryExpression<SgNotEqualOp>(lhs, rhs);
      if (past_node_is_a (node, past_assign))
	return SageBuilder::buildBinaryExpression<SgAssignOp>(lhs, rhs);
      if (past_node_is_a (node, past_geq))
	return SageBuilder::buildBinaryExpression<SgGreaterOrEqualOp>(lhs, rhs);
      if (past_node_is_a (node, past_leq))
	return SageBuilder::buildBinaryExpression<SgLessOrEqualOp>(lhs, rhs);
      if (past_node_is_a (node, past_gt))
	return SageBuilder::buildBinaryExpression<SgGreaterThanOp>(lhs, rhs);
      if (past_node_is_a (node, past_lt))
	return SageBuilder::buildBinaryExpression<SgLessThanOp>(lhs, rhs);
      if (past_node_is_a (node, past_xor))
	return SageBuilder::buildBinaryExpression<SgBitXorOp>(lhs, rhs);
      if (past_node_is_a (node, past_band))
	return SageBuilder::buildBinaryExpression<SgBitAndOp>(lhs, rhs);
      if (past_node_is_a (node, past_bor))
	return SageBuilder::buildBinaryExpression<SgBitOrOp>(lhs, rhs);
      if (past_node_is_a (node, past_lshift))
	return SageBuilder::buildBinaryExpression<SgLshiftOp>(lhs, rhs);
      if (past_node_is_a (node, past_rshift))
	return SageBuilder::buildBinaryExpression<SgRshiftOp>(lhs, rhs);
      if (past_node_is_a (node, past_addassign))
	return SageBuilder::buildBinaryExpression<SgPlusAssignOp>(lhs, rhs);
      if (past_node_is_a (node, past_subassign))
	return SageBuilder::buildBinaryExpression<SgMinusAssignOp>(lhs, rhs);
      if (past_node_is_a (node, past_mulassign))
	return SageBuilder::buildBinaryExpression<SgMultAssignOp>(lhs, rhs);
      if (past_node_is_a (node, past_divassign))
	return SageBuilder::buildBinaryExpression<SgDivAssignOp>(lhs, rhs);
      if (past_node_is_a (node, past_modassign))
	return SageBuilder::buildBinaryExpression<SgModAssignOp>(lhs, rhs);
      if (past_node_is_a (node, past_andassign))
	return SageBuilder::buildBinaryExpression<SgAndAssignOp>(lhs, rhs);
      if (past_node_is_a (node, past_orassign))
	return SageBuilder::buildBinaryExpression<SgIorAssignOp>(lhs, rhs);
      if (past_node_is_a (node, past_xorassign))
	return SageBuilder::buildBinaryExpression<SgXorAssignOp>(lhs, rhs);
      if (past_node_is_a (node, past_lshiftassign))
	return SageBuilder::buildBinaryExpression<SgLshiftAssignOp>(lhs, rhs);
      if (past_node_is_a (node, past_rshiftassign))
	return SageBuilder::buildBinaryExpression<SgRshiftAssignOp>(lhs, rhs);
      if (past_node_is_a (node, past_min))
	return SageBuilder::buildConditionalExp
	  (SageBuilder::buildBinaryExpression<SgLessThanOp>(lhs, rhs),
	   SageInterface::copyExpression(lhs),
	   SageInterface::copyExpression(rhs));
      if (past_node_is_a (node, past_max))
	return SageBuilder::buildConditionalExp
	  (SageBuilder::buildBinaryExpression<SgGreaterThanOp>(lhs, rhs),
	   SageInterface::copyExpression(lhs),
	   SageInterface::copyExpression(rhs));
      if (past_node_is_a (node, past_ceild))
	return buildCeildFloord(node, lhs, rhs);
      if (past_node_is_a (node, past_floord))
	return buildCeildFloord(node, lhs, rhs);
      if (past_node_is_a (node, past_dot))
      	return SageBuilder::buildDotExp(lhs, rhs);
      if (past_node_is_a (node, past_arrow))
      	return SageBuilder::buildArrowExp(lhs, rhs);
      ROSE_ASSERT(! "Unsupported past_binary node type in PastToSageGeneric");
      return NULL;
    }

  // past_unary
  else if (past_node_is_a (node, past_unary))
    {
      PAST_DECLARE_TYPED(unary, v, node);
      SgExpression* expr = (SgExpression*) converter (v->expr);
      expr = isSgExpression (expr);
      ROSE_ASSERT(expr);
      if (past_node_is_a (node, past_inc_before))
	return SageBuilder::buildPlusPlusOp(expr, SgUnaryOp::prefix);
      if (past_node_is_a (node, past_inc_after))
	return SageBuilder::buildPlusPlusOp(expr, SgUnaryOp::postfix);
      if (past_node_is_a (node, past_dec_before))
	return SageBuilder::buildMinusMinusOp(expr, SgUnaryOp::prefix);
      if (past_node_is_a (node, past_dec_after))
	return SageBuilder::buildMinusMinusOp(expr, SgUnaryOp::postfix);
      if (past_node_is_a (node, past_unaminus))
	return SageBuilder::buildMinusOp(expr);
      if (past_node_is_a (node, past_opsizeof))
	return SageBuilder::buildSizeOfOp(expr);
      if (past_node_is_a (node, past_unaplus))
	return expr;
      if (past_node_is_a (node, past_addressof))
	return SageBuilder::buildAddressOfOp(expr);
      if (past_node_is_a (node, past_derefof))
	return SageBuilder::buildPointerDerefExp(expr);
      if (past_node_is_a (node, past_round))
	return SageBuilder::buildFunctionCallExp
	  (SgName("round"), new SgTypeFloat(),
	   SageBuilder::buildExprListExp(expr), _scope);
      if (past_node_is_a (node, past_floor))
	return SageBuilder::buildFunctionCallExp
	  (SgName("floor"), new SgTypeFloat(),
	   SageBuilder::buildExprListExp(expr), _scope);
      if (past_node_is_a (node, past_ceil))
	return SageBuilder::buildFunctionCallExp
	  (SgName("ceil"), new SgTypeFloat(),
	   SageBuilder::buildExprListExp(expr), _scope);
      if (past_node_is_a (node, past_sqrt))
	return SageBuilder::buildFunctionCallExp
	  (SgName("sqrt"), new SgTypeFloat(),
	   SageBuilder::buildExprListExp(expr), _scope);
      if (past_node_is_a (node, past_not))
	return SageBuilder::buildNotOp(expr);
      if (past_node_is_a (node, past_bcomp))
      	return SageBuilder::buildBitComplementOp(expr);

      // //  &
      // if (past_node_is_a (node, past_referencetype))
      // 	return SageBuilder::buildreferencetypeOp(expr);
      // // *
      // if (past_node_is_a (node, past_pointertype))
      // 	return SageBuilder::buildpointertypeOp(expr);

    }


  // past_generic.
  else if (past_node_is_a (node, past_generic))
    {
      /// Fingers crossed...
      PAST_DECLARE_TYPED(generic, v, node);
      ROSE_ASSERT(v->ptr_data);
      SgNode* ret =
	(SgNode*) SageInterface::deepCopyNode((SgNode*)v->ptr_data);
      return ret;
    }

  // Default.
  else
    {
      printf ("Faulty tree:\n");
      past_pprint (stdout, node);
      printf ("\n==>\n");
      ROSE_ASSERT(! "Current node type not supported in PastToSageGeneric");
      return NULL;
    }
}
