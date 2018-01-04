/*
 * SageToPast.cpp: This file is part of the PolyOpt project.
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
 * @file: SageToPast.cpp
 * @author: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 */
#undef PACKAGE_BUGREPORT
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_NAME
#undef PACKAGE_VERSION
#include <rose.h>
#ifndef SCOPLIB_INT_T_IS_LONGLONG
# define SCOPLIB_INT_T_IS_LONGLONG
#endif
#include <scoplib/scop.h>
#include <past/past.h>

#include <polyopt/SageToPast.hpp>

#include <AstTraversal.h>
#include <sageInterface.h>

#include <polyopt/ScopExtractorVisitor.hpp>
#include <polyopt/SageNodeAnnotation.hpp>
#include <polyopt/SageTools.hpp>
#include <past/past_api.h>
#include <past/pprint.h>


static
void PolyOptWarning (std::string str)
{
  std::cout << "[PolyOpt][Warning] " << str << std::endl;
}

s_past_node_t* SageToPast::toPast (SgNode* root)
{
  _past_symt = symbol_table_malloc ();

  traverse (root);

  s_past_node_t* n = NULL;
  if (_past_stack.size () != 1)
    PolyOptWarning ("Problem: only one PAST tree should be found...");
  while (_past_stack.size ())
    {
      n = (s_past_node_t*) _past_stack.top ();
      _past_stack.pop ();
    }
  s_past_node_t* ret = past_node_root_create (_past_symt, n);
  past_update_symbols_with_declaration (ret);
  ret->usr = root;

  // std::cout << "[DEBUG: PAST TREE CREATED]" << std::endl;
  // past_pprint (stdout, ret);
  // std::cout << "[DEBUG: SYMBOL TABLE CREATED]" << std::endl;
  // symbol_table_print (_past_symt);


  return ret;
}

static
void build_past_binary_node (cs_past_node_type_t* type,
			     std::stack<void*>& st,
			     SgNode* base)
{
  s_past_node_t* rhs = (s_past_node_t*)st.top ();
  st.pop ();
  s_past_node_t* lhs = (s_past_node_t*)st.top ();
  st.pop ();
  s_past_node_t* ret = past_node_binary_create (type, lhs, rhs);
  ret->usr = base;
  st.push (ret);
}


static
void build_past_unary_node (cs_past_node_type_t* type,
			    std::stack<void*>& st,
			    SgNode* base)
{
  s_past_node_t* n = (s_past_node_t*)st.top ();
  st.pop ();
  s_past_node_t* ret = past_node_unary_create (type, n);
  ret->usr = base;
  st.push (ret);
}


static
void get_structbinop (SgNode* node, SgNode** lhs, SgNode** rhs)
{
  // Recover the structure depth of the node
  // (e.g., 2 for a . b[c . d] . node); 0 for node . a).
  if (! node)
    return;
  SgNode* base = node;
  SgNode* last = node;
  SgNode* top = NULL;
  top = SageInterface::getEnclosingNode<SgPntrArrRefExp> (node);
  if (! top)
    top = SageInterface::getEnclosingNode<SgExprListExp> (node);
  node = node->get_parent ();
  int struct_depth = 0;
  while (node && node != top)
    {
      SgBinaryOp* bin = NULL;
      if (isSgDotExp (node) || isSgArrowExp (node) ||
	  isSgArrowStarOp(node) || isSgDotStarOp (node))
	bin = isSgBinaryOp (node);
      if (bin)
	{
	  if (bin->get_rhs_operand () == last)
	    *rhs = node;
	  else
	    // Top lhs entry.
	    *lhs = node;
	  break;
	}
      last = node;
      node = node->get_parent ();
    }
}


/**
 * Implement conversion from Sage to PAST.
 *
 */
void
SageToPast::postOrderVisit(SgNode* node)
{
  // std::cout << node->class_name() << std::endl;
  // std::cout << "DEBUG print start" << std::endl;
  // std::cout << node->unparseToCompleteString() << std::endl;
  // std::cout << "============ !DEBUG" << std::endl;

  switch (node->variantT())
    {
    case V_SgInitializedName:
      {
	SgInitializedName* val = isSgInitializedName(node);
	s_symbol_t* s = symbol_get_or_insert (_past_symt, val->unparseToCompleteString().c_str(), val);
	_past_stack.push (s);
	break;
      }

    case V_SgAssignInitializer: {
      SgAssignInitializer* val = isSgAssignInitializer(node);
      s_past_node_t* n = (s_past_node_t*)_past_stack.top ();
      _past_stack.pop ();
      n = past_node_binary_create (past_assign, NULL, n);
      n->usr = val;
      _past_stack.push (n);
      break;
    }

    case V_SgCastExp: {
      SgCastExp* val = isSgCastExp(node);
      std::string type = val->get_type ()->unparseToCompleteString ();
      s_past_node_t* n = (s_past_node_t*)_past_stack.top ();
      _past_stack.pop ();
      s_past_node_t* tp = past_node_type_create
	(past_node_generic_create (strdup (type.c_str ()), val->get_type()));
      n = past_node_cast_create (tp, n);
      n->usr = val;
      _past_stack.push (n);
      break;
    }

    case V_SgVariableDeclaration: {
      SgVariableDeclaration* val = isSgVariableDeclaration(node);
      // top must be a sginitializedname.
      s_symbol_t* s = (s_symbol_t*)_past_stack.top ();
      _past_stack.pop ();
      SgInitializedName* nm = (SgInitializedName*)(s->data);
      std::string st = nm->get_type ()->unparseToCompleteString ();
      s_past_node_t* tp = past_node_type_create
    	(past_node_generic_create (strdup (st.c_str ()), nm->get_type ()));
      s_past_node_t* vref = past_node_varref_create (s);
      s_past_node_t* vardecl = past_node_vardecl_create (tp, vref);
      if (_past_stack.size ())
    	{
    	  s_past_node_t* n2 = (s_past_node_t*)_past_stack.top ();
    	  if (past_node_is_a (n2, past_assign))
    	    {
    	      PAST_DECLARE_TYPED(binary, pb, n2);
    	      if (pb->lhs == NULL)
    		{
    		  // This is a pending assignment in a vardecl.
    		  pb->lhs = vardecl;
    		  vardecl = n2;
    		  _past_stack.pop ();
    		}
    	    }
    	  else if (past_node_is_a (n2, past_generic))
    	    {
    	      PAST_DECLARE_TYPED(generic, g, n2);
    	      SgNode* gn = isSgInitializer((SgNode*) g->ptr_data);
    	      if (gn)
    		{
    		  // Some unsupported variable declaration. Return a
    		  // generic for the whole declaration.
    		  _past_stack.pop();
    		  vardecl =
    		    past_node_generic_create
    		    (strdup (node->unparseToCompleteString().c_str()), node);
    		}
    	    }
    	}
      vardecl = past_node_statement_create (vardecl);
      vardecl->usr = val;
      _past_stack.push (vardecl);
      break;
    }

    case V_SgVarRefExp: {
      SgVarRefExp* val = isSgVarRefExp(node);
      SgNode* rhs = NULL;
      SgNode* lhs = NULL;
      s_symbol_t* s = symbol_create
	(past_unknown, val->unparseToCompleteString().c_str(),
	 val->get_symbol ()->get_declaration ());
      get_structbinop (node, &lhs, &rhs);
      if (lhs)
	_structMap[lhs] = s;
      else if (rhs)
	{
	  ROSE_ASSERT(_structMap[rhs]);
	  s->parent_symb = _structMap[rhs];
	  // Get the enclosing dot/arrow, if any.
	  get_structbinop (rhs, &lhs, &rhs);
	  if (lhs)
	    _structMap[lhs] = s;
	  else if (rhs)
	    _structMap[rhs] = s;
	}
      s_symbol_t* news = symbol_add (_past_symt, s);
      if (news != s)
	{
	  symbol_free (s);
	  s = news;
	}
      s_past_node_t* n = past_node_varref_create (s);
      n->usr = node;
      _past_stack.push (n);
      break;
    }
    case V_SgFunctionRefExp: {
      SgFunctionRefExp* val = isSgFunctionRefExp(node);
      SgNode* rhs = NULL;
      SgNode* lhs = NULL;
      int struct_depth = 0;//get_struct_depth (node, &lhs, &rhs);
      s_symbol_t* s =
	symbol_find_from_data (_past_symt, val->get_symbol ());
      if (! s)
	s = symbol_get_or_insert (_past_symt,
				  val->unparseToCompleteString().c_str(),
				  val->get_symbol ());
      else if (s->struct_depth != struct_depth)
	{
	  s = symbol_create
	    (past_unknown, val->unparseToCompleteString().c_str(),
	     val->get_symbol ());
	  s = symbol_add (_past_symt, s);
	}
      s_past_node_t* n = past_node_varref_create (s);
      n->usr = node;
      _past_stack.push (n);
      break;
    }


      /**
       *  Value conversion rules.
       *
       */
    case V_SgIntVal: {
      SgIntVal* val = isSgIntVal(node);
      u_past_value_data_t v;
      v.intval = val->get_value ();
      s_past_node_t* n = past_node_value_create (e_past_value_int, v);
      n->usr = val;
      _past_stack.push (n);
      break;
    }
    case V_SgStringVal: {
      SgStringVal* val = isSgStringVal(node);
      s_past_node_t* n =
	past_node_string_create (strdup (val->get_value ().c_str ()));
      n->usr = val;
      _past_stack.push (n);
      break;
    }
    case V_SgShortVal: {
      SgShortVal* val = isSgShortVal(node);
      u_past_value_data_t v;
      v.shortval = val->get_value ();
      s_past_node_t* n = past_node_value_create (e_past_value_short, v);
      n->usr = val;
      _past_stack.push (n);
      break;
    }
    case V_SgCharVal: {
      SgCharVal* val = isSgCharVal(node);
      u_past_value_data_t v;
      v.charval = val->get_value ();
      s_past_node_t* n = past_node_value_create (e_past_value_char, v);
      n->usr = val;
      _past_stack.push (n);
      break;
    }
    case V_SgUnsignedCharVal: {
      SgUnsignedCharVal* val = isSgUnsignedCharVal(node);
      u_past_value_data_t v;
      v.ucharval = val->get_value ();
      s_past_node_t* n = past_node_value_create (e_past_value_uchar, v);
      n->usr = val;
      _past_stack.push (n);
      break;
    }
    case V_SgWcharVal: {
      SgWcharVal* val = isSgWcharVal(node);
      u_past_value_data_t v;
      PolyOptWarning ("wchar not fully supported, casted as char");
      v.wcharval = (char) val->get_value ();
      s_past_node_t* n = past_node_value_create (e_past_value_wchar, v);
      n->usr = val;
      _past_stack.push (n);
      break;
    }
    case V_SgUnsignedShortVal: {
      SgUnsignedShortVal* val = isSgUnsignedShortVal(node);
      u_past_value_data_t v;
      v.ushortval = val->get_value ();
      s_past_node_t* n = past_node_value_create (e_past_value_ushort, v);
      n->usr = val;
      _past_stack.push (n);
      break;
    }
    case V_SgEnumVal: {
      SgEnumVal* val = isSgEnumVal(node);
      u_past_value_data_t v;
      v.enumval = (int) val->get_value ();
      s_past_node_t* n = past_node_value_create (e_past_value_enum, v);
      n->usr = val;
      _past_stack.push (n);
      break;
    }
    case V_SgUnsignedIntVal: {
      SgUnsignedIntVal* val = isSgUnsignedIntVal(node);
      u_past_value_data_t v;
      v.uintval = val->get_value ();
      s_past_node_t* n = past_node_value_create (e_past_value_uint, v);
      n->usr = val;
      _past_stack.push (n);
      break;
    }
    case V_SgLongIntVal: {
      SgLongIntVal* val = isSgLongIntVal(node);
      u_past_value_data_t v;
      v.longintval = val->get_value ();
      s_past_node_t* n = past_node_value_create (e_past_value_longint, v);
      n->usr = val;
      _past_stack.push (n);
      break;
    }
    case V_SgLongLongIntVal: {
      SgLongLongIntVal* val = isSgLongLongIntVal(node);
      u_past_value_data_t v;
      v.longlongintval = val->get_value ();
      s_past_node_t* n = past_node_value_create (e_past_value_longlongint, v);
      n->usr = val;
      _past_stack.push (n);
      break;
    }
    case V_SgUnsignedLongLongIntVal: {
      SgUnsignedLongLongIntVal* val = isSgUnsignedLongLongIntVal(node);
      u_past_value_data_t v;
      v.ulonglongintval = val->get_value ();
      s_past_node_t* n = past_node_value_create (e_past_value_ulonglongint, v);
      n->usr = val;
      _past_stack.push (n);
      break;
    }
    case V_SgUnsignedLongVal: {
      SgUnsignedLongVal* val = isSgUnsignedLongVal(node);
      u_past_value_data_t v;
      v.ulongintval = val->get_value ();
      s_past_node_t* n = past_node_value_create (e_past_value_ulongint, v);
      n->usr = val;
      _past_stack.push (n);
      break;
    }
    case V_SgFloatVal: {
      SgFloatVal* val = isSgFloatVal(node);
      u_past_value_data_t v;
      v.floatval = val->get_value ();
      s_past_node_t* n = past_node_value_create (e_past_value_float, v);
      n->usr = val;
      _past_stack.push (n);
      break;
    }
    case V_SgDoubleVal: {
      SgDoubleVal* val = isSgDoubleVal(node);
      u_past_value_data_t v;
      v.doubleval = val->get_value ();
      s_past_node_t* n = past_node_value_create (e_past_value_double, v);
      n->usr = val;
      _past_stack.push (n);
      break;
    }
    case V_SgLongDoubleVal: {
      SgLongDoubleVal* val = isSgLongDoubleVal(node);
      u_past_value_data_t v;
      v.longdoubleval = val->get_value ();
      s_past_node_t* n = past_node_value_create (e_past_value_longdouble, v);
      n->usr = val;
      _past_stack.push (n);
      break;
    }
    case V_SgBoolValExp: {
      SgBoolValExp* val = isSgBoolValExp(node);
      u_past_value_data_t v;
      v.boolval = val->get_value ();
      s_past_node_t* n = past_node_value_create (e_past_value_bool, v);
      n->usr = val;
      _past_stack.push (n);
      break;
    }


      /**
       * Unary operators.
       *
       */
    case V_SgMinusOp:
      build_past_unary_node (past_unaminus, _past_stack, node); break;
    case V_SgUnaryAddOp:
      build_past_unary_node (past_unaplus, _past_stack, node); break;
    case V_SgNotOp:
      build_past_unary_node (past_not, _past_stack, node); break;
    case V_SgPointerDerefExp:
      build_past_unary_node (past_derefof, _past_stack, node); break;
    case V_SgAddressOfOp:
      build_past_unary_node (past_addressof, _past_stack, node); break;
    case V_SgMinusMinusOp:
      build_past_unary_node (past_dec_after, _past_stack, node); break;
    case V_SgPlusPlusOp:
      build_past_unary_node (past_inc_after, _past_stack, node); break;
    case V_SgBitComplementOp:
      build_past_unary_node (past_bcomp, _past_stack, node); break;

      /**
       * Binary operators.
       *
       *
       *
       */
    case V_SgPntrArrRefExp:
      build_past_binary_node (past_arrayref, _past_stack, node); break;
    case V_SgAddOp:
      build_past_binary_node (past_add, _past_stack, node); break;
    case V_SgSubtractOp:
      build_past_binary_node (past_sub, _past_stack, node); break;
    case V_SgMultiplyOp:
      build_past_binary_node (past_mul, _past_stack, node); break;
    case V_SgDivideOp:
      build_past_binary_node (past_div, _past_stack, node); break;
    case V_SgIntegerDivideOp:
      build_past_binary_node (past_div, _past_stack, node); break;
    case V_SgModOp:
      build_past_binary_node (past_mod, _past_stack, node); break;
    case V_SgAndOp:
      build_past_binary_node (past_and, _past_stack, node); break;
    case V_SgOrOp:
      build_past_binary_node (past_or, _past_stack, node); break;
    case V_SgEqualityOp:
      build_past_binary_node (past_equal, _past_stack, node); break;
    case V_SgLessThanOp:
      build_past_binary_node (past_lt, _past_stack, node); break;
    case V_SgGreaterThanOp:
      build_past_binary_node (past_gt, _past_stack, node); break;
    case V_SgNotEqualOp:
      build_past_binary_node (past_notequal, _past_stack, node); break;
    case V_SgLessOrEqualOp:
      build_past_binary_node (past_leq, _past_stack, node); break;
    case V_SgGreaterOrEqualOp:
      build_past_binary_node (past_geq, _past_stack, node); break;
    case V_SgAssignOp:
      build_past_binary_node (past_assign, _past_stack, node); break;
    case V_SgPlusAssignOp:
      build_past_binary_node (past_addassign, _past_stack, node); break;
    case V_SgMinusAssignOp:
      build_past_binary_node (past_subassign, _past_stack, node); break;
    case V_SgAndAssignOp:
      build_past_binary_node (past_andassign, _past_stack, node); break;
    case V_SgIorAssignOp:
      build_past_binary_node (past_orassign, _past_stack, node); break;
    case V_SgMultAssignOp:
      build_past_binary_node (past_mulassign, _past_stack, node); break;
    case V_SgDivAssignOp:
      build_past_binary_node (past_divassign, _past_stack, node); break;
    case V_SgModAssignOp:
      build_past_binary_node (past_modassign, _past_stack, node); break;
    case V_SgXorAssignOp:
      build_past_binary_node (past_xorassign, _past_stack, node); break;
    case V_SgLshiftAssignOp:
      build_past_binary_node (past_lshiftassign, _past_stack, node); break;
    case V_SgRshiftAssignOp:
      build_past_binary_node (past_rshiftassign, _past_stack, node); break;
    case V_SgBitXorOp:
      build_past_binary_node (past_xor, _past_stack, node); break;
    case V_SgBitAndOp:
      build_past_binary_node (past_band, _past_stack, node); break;
    case V_SgBitOrOp:
      build_past_binary_node (past_bor, _past_stack, node); break;
    case V_SgLshiftOp:
      build_past_binary_node (past_lshift, _past_stack, node); break;
    case V_SgRshiftOp:
      build_past_binary_node (past_rshift, _past_stack, node); break;
    case V_SgArrowExp:
      build_past_binary_node (past_arrow, _past_stack, node); break;
    case V_SgDotExp:
      build_past_binary_node (past_dot, _past_stack, node); break;

      // case V_SgDotStarOp: {
      //   SgDotStarOp* val = isSgDotStarOp(node);
      //   break;
      // }
      // case V_SgArrowStarOp: {
      //   SgArrowStarOp* val = isSgArrowStarOp(node);
      //   break;
      // }



      /**
       * Elaborate operators.
       *
       *
       */
    case V_SgExprStatement: {
      s_past_node_t* n = (s_past_node_t*)_past_stack.top ();
      _past_stack.pop ();
      n = past_node_statement_create (n);
      n->usr = node;
      _past_stack.push (n);
      break;
    }

    case V_SgForStatement: {
      // Body.
      s_past_node_t* body = (s_past_node_t*)_past_stack.top ();
      _past_stack.pop ();
      // Inc.
      s_past_node_t* inc = (s_past_node_t*)_past_stack.top ();
      _past_stack.pop ();
      // Test.
      s_past_node_t* test = (s_past_node_t*)_past_stack.top ();
      _past_stack.pop ();
      // Init.
      s_past_node_t* init = (s_past_node_t*)_past_stack.top ();
      _past_stack.pop ();

      // Inc and test are encapsulated in a statement. Demangle.
      PAST_DECLARE_TYPED(statement, sinit, init);
      PAST_DECLARE_TYPED(statement, stest, test);
      s_past_node_t* finit = sinit->body; XFREE(sinit);
      s_past_node_t* ftest = stest->body; XFREE(stest);

      // Find the for loop iterator. Assume it is the lhs of the init.
      s_symbol_t* iterator = NULL;
      if (past_node_is_a (finit, past_assign))
	{
	  PAST_DECLARE_TYPED(binary, pb, finit);
	  s_past_node_t* lhs = pb->lhs;
	  if (past_node_is_a (lhs, past_vardecl))
	    {
	      PAST_DECLARE_TYPED(vardecl, vd, lhs);
	      lhs = vd->name;
	    }
	  if (past_node_is_a (lhs, past_varref))
	    {
	      PAST_DECLARE_TYPED(varref, pv, lhs);
	      iterator = pv->symbol;
	    }
	}
      s_past_node_t* fnode =
	past_node_for_create (finit, ftest, iterator, inc, body);
      fnode->usr = node;
      _past_stack.push (fnode);
      break;
    }

    case V_SgExprListExp: {
      s_past_node_t* args = NULL;
      s_past_node_t* args_lst = NULL;
      SgExprListExp* val = isSgExprListExp (node);
      for (int i = 0; i < val->get_expressions ().size (); ++i)
	{
	  s_past_node_t* n = (s_past_node_t*) _past_stack.top ();
	  _past_stack.pop ();
	  ROSE_ASSERT(n);
	  if (args == NULL)
	    args = n;
	  else
	    {
	      n->next = args;
	      args = n;
	    }
	}
      if (args)
	args->usr = node;
      _past_stack.push (args);
      break;
    }

    case V_SgFunctionCallExp: {
      s_past_node_t* args = (s_past_node_t*) _past_stack.top ();
      _past_stack.pop ();
      s_past_node_t* fname = (s_past_node_t*) _past_stack.top ();
      _past_stack.pop ();
      ROSE_ASSERT(fname);
      s_past_node_t* funcall = NULL;
      // Check if it is one of the special functions:
      // min, max, ceild, floord, round, floor, ceil, sqrt.
      if (past_node_is_a (fname, past_varref))
	{
	  PAST_DECLARE_TYPED(varref, pv, fname);
	  SgVariableSymbol* sym = (SgVariableSymbol*) pv->symbol->data;
	  const char* func = sym->get_name ().getString().c_str ();
	  cs_past_node_type_t* func_type = NULL;
	  if (! strcmp (func, "min"))
	    func_type = past_min;
	  else if (! strcmp (func, "max"))
	    func_type = past_max;
	  else if (! strcmp (func, "ceild"))
	    func_type = past_ceild;
	  else if (! strcmp (func, "floord"))
	    func_type = past_floord;
	  else
	    {
	      if (! strcmp (func, "round"))
		func_type = past_round;
	      else if (! strcmp (func, "floor"))
		func_type = past_floor;
	      else if (! strcmp (func, "ceil"))
		func_type = past_ceil;
	      else if (! strcmp (func, "sqrt"))
		func_type = past_sqrt;
	      if (func_type != NULL && args && args->next == NULL)
		{
		  funcall = past_node_unary_create (func_type, args);
		  funcall->usr = node;
		  _past_stack.push (funcall);
		  break;
		}
	    }
	  if (func_type != NULL && args && args->next
	      && args->next->next == NULL)
	    {
	      s_past_node_t* arg1 = args;
	      s_past_node_t* arg2 = args->next;
	      arg1->next = NULL;
	      funcall = past_node_binary_create (func_type, arg1, arg2);
	      funcall->usr = node;
	      _past_stack.push (funcall);
	      break;
	    }
	}

      // Generic function call support.
      funcall = past_node_funcall_create (fname, args);
      funcall->usr = node;
      _past_stack.push (funcall);
      break;
    }

    case V_SgBasicBlock: {
      SgBasicBlock* val = isSgBasicBlock(node);

      s_past_block_t* bb = past_block_create (NULL);
      s_past_node_t* body = NULL;
      while (_past_stack.size ())
	{
	  s_past_node_t* n = (s_past_node_t*) _past_stack.top ();
	  if (! n->usr && body == NULL)
	    {
	      PolyOptWarning ("Problem: the stack should contain annotated nodes");
	      ROSE_ASSERT(0);
	    }
	  SgNode* base = (SgNode*) n->usr;
	  SgBasicBlock* encloseBB =
	    SageInterface::getEnclosingNode<SgBasicBlock>(base);
	  if (encloseBB != node)
	    break;
	  _past_stack.pop ();
	  if (body == NULL)
	    body = n;
	  else
	    {
	      n->next = body;
	      body = n;
	    }
	}
      bb->body = body;
      bb->node.usr = node;
      _past_stack.push (bb);
      break;
    }

    case V_SgIfStmt: {
      SgIfStmt* val = isSgIfStmt(node);
      s_past_node_t* elseclause = NULL;
      // Last clause.
      s_past_node_t* thenclause = (s_past_node_t*)_past_stack.top ();
      _past_stack.pop ();
      if (val->get_false_body() != NULL)
	{
	  elseclause = thenclause;
	  thenclause = (s_past_node_t*)_past_stack.top ();
	  _past_stack.pop ();
	}
      // Conditional.
      s_past_node_t* test = (s_past_node_t*)_past_stack.top ();
      _past_stack.pop ();
      // Test is encapsulated in a statement. Demangle.
      PAST_DECLARE_TYPED(statement, stest, test);
      s_past_node_t* ftest = stest->body; XFREE(stest);

      s_past_node_t* ifstmt =
	past_node_if_create (ftest, thenclause, elseclause);
      ifstmt->usr = node;
      _past_stack.push (ifstmt);
      break;
    }

    case V_SgWhileStmt: {
      s_past_node_t* body = (s_past_node_t*)_past_stack.top ();
      _past_stack.pop ();
      s_past_node_t* test = (s_past_node_t*)_past_stack.top ();
      _past_stack.pop ();
      // Test is encapsulated in a statement. Demangle.
      PAST_DECLARE_TYPED(statement, stest, test);
      s_past_node_t* ftest = stest->body; XFREE(stest);

      s_past_node_t* whilestmt =
	past_node_while_create (ftest, body);
      whilestmt->usr = node;
      _past_stack.push (whilestmt);
      break;
    }
    case V_SgDoWhileStmt: {
      s_past_node_t* test = (s_past_node_t*)_past_stack.top ();
      _past_stack.pop ();
      s_past_node_t* body = (s_past_node_t*)_past_stack.top ();
      _past_stack.pop ();
      // Test is encapsulated in a statement. Demangle.
      PAST_DECLARE_TYPED(statement, stest, test);
      s_past_node_t* ftest = stest->body; XFREE(stest);

      s_past_node_t* whilestmt =
	past_node_do_while_create (ftest, body);
      whilestmt->usr = node;
      _past_stack.push (whilestmt);
      break;
    }

    case V_SgNullExpression: {
      _past_stack.push (NULL);
      break;
    }

      //   /**
      //    * Scop-breaking nodes, seen as past_generic.
      //    *
      //    *
      //    */
      // case V_SgLabelStatement:
      // case V_SgBreakStmt:
      // case V_SgContinueStmt: {
      //   s_past_node_t* ret =
      // 	past_node_generic_create
      // 	(strdup (node->unparseToCompleteString().c_str()), node);
      //   ret->usr = node;
      //   _past_stack.push (ret);
      //   break;
      // }
      // case V_SgReturnStmt: {
      //   SgReturnStmt* val = isSgReturnStmt(node);
      //   if (val->get_expression () != NULL)
      // 	{
      // 	  s_past_node_t* t = (s_past_node_t*)_past_stack.top ();
      // 	  past_deep_free (t);
      // 	  _past_stack.pop ();
      // 	}
      //   s_past_node_t* ret =
      // 	past_node_generic_create
      // 	(strdup (node->unparseToCompleteString().c_str()), node);
      //   ret->usr = node;
      //   _past_stack.push (ret);
      //   break;
      // }
      // case V_SgGotoStatement: {
      //   s_past_node_t* t = (s_past_node_t*)_past_stack.top ();
      //   past_deep_free (t);
      //   _past_stack.pop ();
      //   s_past_node_t* ret =
      // 	past_node_generic_create
      // 	(strdup (node->unparseToCompleteString().c_str()), node);
      //   ret->usr = node;
      //   _past_stack.push (ret);
      //   break;
      // }
      // case V_SgConstructorInitializer: {
      //   SgConstructorInitializer* val = isSgConstructorInitializer(node);
      //   s_past_node_t* t = (s_past_node_t*)_past_stack.top ();
      //   past_deep_free (t);
      //   _past_stack.pop ();
      //   s_past_node_t* ret =
      // 	past_node_generic_create
      // 	(strdup (node->unparseToCompleteString().c_str()), node);
      //   ret->usr = node;
      //   _past_stack.push (ret);
      //   break;
      // }


      /**
       * Do-nothing nodes.
       *
       *
       */
    case V_SgForInitStatement: break;


      /**
       * Currently unsupported/unimplemented nodes.
       *
       *
       */
    default:
      {
	// PolyOptWarning ("Unsupported node type:");
	// std::cout << node->class_name() << std::endl;

	std::vector<SgNode*> succ = node->get_traversalSuccessorContainer();
	for (std::vector<SgNode*>::iterator it = succ.begin ();
	     it != succ.end(); ++it)
	  {
	    // What the hell... SizeOfOp can have a null successor...
	    if (*it == NULL)
	      continue;
	    // Skip type trees (should never happen)
	    if (isSgType(*it))
		continue;
	    s_past_node_t* t = (s_past_node_t*)_past_stack.top ();
	    past_deep_free (t);
	    _past_stack.pop ();
	  }
	s_past_node_t* ret =
	  past_node_generic_create
	  (strdup (node->unparseToCompleteString().c_str()), node);
	ret->usr = node;
	_past_stack.push (ret);
	break;
      }
      ////

      // case V_SgTemplateVariableDeclaration: {
      //   SgTemplateVariableDeclaration* val = isSgTemplateVariableDeclaration(node);

      //   break;
      // }
      // case V_SgName: {
      //   SgName* val = isSgName(node);
      //   break;
      // }
      // case V_SgSymbolTable: {
      //   SgSymbolTable* val = isSgSymbolTable(node);
      //   break;
      // }
      // case V_SgPragma: {
      //   SgPragma* val = isSgPragma(node);
      //   break;
      // }
      // case V_SgModifierNodes: {
      //   SgModifierNodes* val = isSgModifierNodes(node);
      //   break;
      // }
      // case V_SgConstVolatileModifier: {
      //   SgConstVolatileModifier* val = isSgConstVolatileModifier(node);
      //   break;
      // }
      // case V_SgStorageModifier: {
      //   SgStorageModifier* val = isSgStorageModifier(node);
      //   break;
      // }
      // case V_SgAccessModifier: {
      //   SgAccessModifier* val = isSgAccessModifier(node);
      //   break;
      // }
      // case V_SgFunctionModifier: {
      //   SgFunctionModifier* val = isSgFunctionModifier(node);
      //   break;
      // }
      // case V_SgUPC_AccessModifier: {
      //   SgUPC_AccessModifier* val = isSgUPC_AccessModifier(node);
      //   break;
      // }
      // case V_SgLinkageModifier: {
      //   SgLinkageModifier* val = isSgLinkageModifier(node);
      //   break;
      // }
      // case V_SgSpecialFunctionModifier: {
      //   SgSpecialFunctionModifier* val = isSgSpecialFunctionModifier(node);
      //   break;
      // }
      // case V_SgTypeModifier: {
      //   SgTypeModifier* val = isSgTypeModifier(node);
      //   break;
      // }
      // case V_SgElaboratedTypeModifier: {
      //   SgElaboratedTypeModifier* val = isSgElaboratedTypeModifier(node);
      //   break;
      // }
      // case V_SgBaseClassModifier: {
      //   SgBaseClassModifier* val = isSgBaseClassModifier(node);
      //   break;
      // }
      // case V_SgDeclarationModifier: {
      //   SgDeclarationModifier* val = isSgDeclarationModifier(node);
      //   break;
      // }
      // case V_SgOpenclAccessModeModifier: {
      //   SgOpenclAccessModeModifier* val = isSgOpenclAccessModeModifier(node);
      //   break;
      // }
      // case V_SgModifier: {
      //   SgModifier* val = isSgModifier(node);
      //   break;
      // }
      // case V_Sg_File_Info: {
      //   Sg_File_Info* val = isSg_File_Info(node);
      //   break;
      // }
      // case V_SgSourceFile: {
      //   SgSourceFile* val = isSgSourceFile(node);
      //   break;
      // }
      // case V_SgBinaryComposite: {
      //   SgBinaryComposite* val = isSgBinaryComposite(node);
      //   break;
      // }
      // case V_SgUnknownFile: {
      //   SgUnknownFile* val = isSgUnknownFile(node);
      //   break;
      // }
      // case V_SgFile: {
      //   SgFile* val = isSgFile(node);
      //   break;
      // }
      // case V_SgFileList: {
      //   SgFileList* val = isSgFileList(node);
      //   break;
      // }
      // case V_SgDirectory: {
      //   SgDirectory* val = isSgDirectory(node);
      //   break;
      // }
      // case V_SgDirectoryList: {
      //   SgDirectoryList* val = isSgDirectoryList(node);
      //   break;
      // }
      // case V_SgProject: {
      //   SgProject* val = isSgProject(node);
      //   break;
      // }
      // case V_SgOptions: {
      //   SgOptions* val = isSgOptions(node);
      //   break;
      // }
      // case V_SgUnparse_Info: {
      //   SgUnparse_Info* val = isSgUnparse_Info(node);
      //   break;
      // }
      // case V_SgFuncDecl_attr: {
      //   SgFuncDecl_attr* val = isSgFuncDecl_attr(node);
      //   break;
      // }
      // case V_SgClassDecl_attr: {
      //   SgClassDecl_attr* val = isSgClassDecl_attr(node);
      //   break;
      // }
      // case V_SgTypedefSeq: {
      //   SgTypedefSeq* val = isSgTypedefSeq(node);
      //   break;
      // }
      // case V_SgFunctionParameterTypeList: {
      //   SgFunctionParameterTypeList* val = isSgFunctionParameterTypeList(node);
      //   break;
      // }
      // case V_SgTemplateParameter: {
      //   SgTemplateParameter* val = isSgTemplateParameter(node);
      //   break;
      // }
      // case V_SgTemplateArgument: {
      //   SgTemplateArgument* val = isSgTemplateArgument(node);
      //   break;
      // }
      // case V_SgTemplateParameterList: {
      //   SgTemplateParameterList* val = isSgTemplateParameterList(node);
      //   break;
      // }
      // case V_SgTemplateArgumentList: {
      //   SgTemplateArgumentList* val = isSgTemplateArgumentList(node);
      //   break;
      // }
      // case V_SgBitAttribute: {
      //   SgBitAttribute* val = isSgBitAttribute(node);
      //   break;
      // }
      // case V_SgAttribute: {
      //   SgAttribute* val = isSgAttribute(node);
      //   break;
      // }
      // case V_SgExpBaseClass: {
      //   SgExpBaseClass* val = isSgExpBaseClass(node);
      //   break;
      // }
      // case V_SgBaseClass: {
      //   SgBaseClass* val = isSgBaseClass(node);
      //   break;
      // }
      // case V_SgUndirectedGraphEdge: {
      //   SgUndirectedGraphEdge* val = isSgUndirectedGraphEdge(node);
      //   break;
      // }
      // case V_SgDirectedGraphEdge: {
      //   SgDirectedGraphEdge* val = isSgDirectedGraphEdge(node);
      //   break;
      // }
      // case V_SgGraphNode: {
      //   SgGraphNode* val = isSgGraphNode(node);
      //   break;
      // }
      // case V_SgGraphEdge: {
      //   SgGraphEdge* val = isSgGraphEdge(node);
      //   break;
      // }
      // case V_SgStringKeyedBidirectionalGraph: {
      //   SgStringKeyedBidirectionalGraph* val = isSgStringKeyedBidirectionalGraph(node);
      //   break;
      // }
      // case V_SgIntKeyedBidirectionalGraph: {
      //   SgIntKeyedBidirectionalGraph* val = isSgIntKeyedBidirectionalGraph(node);
      //   break;
      // }
      // case V_SgBidirectionalGraph: {
      //   SgBidirectionalGraph* val = isSgBidirectionalGraph(node);
      //   break;
      // }
      // case V_SgIncidenceDirectedGraph: {
      //   SgIncidenceDirectedGraph* val = isSgIncidenceDirectedGraph(node);
      //   break;
      // }
      // case V_SgIncidenceUndirectedGraph: {
      //   SgIncidenceUndirectedGraph* val = isSgIncidenceUndirectedGraph(node);
      //   break;
      // }
      // case V_SgGraph: {
      //   SgGraph* val = isSgGraph(node);
      //   break;
      // }
      // case V_SgGraphNodeList: {
      //   SgGraphNodeList* val = isSgGraphNodeList(node);
      //   break;
      // }
      // case V_SgGraphEdgeList: {
      //   SgGraphEdgeList* val = isSgGraphEdgeList(node);
      //   break;
      // }
      // case V_SgQualifiedName: {
      //   SgQualifiedName* val = isSgQualifiedName(node);
      //   break;
      // }
      // case V_SgNameGroup: {
      //   SgNameGroup* val = isSgNameGroup(node);
      //   break;
      // }
      // case V_SgDimensionObject: {
      //   SgDimensionObject* val = isSgDimensionObject(node);
      //   break;
      // }
      // case V_SgDataStatementGroup: {
      //   SgDataStatementGroup* val = isSgDataStatementGroup(node);
      //   break;
      // }
      // case V_SgDataStatementObject: {
      //   SgDataStatementObject* val = isSgDataStatementObject(node);
      //   break;
      // }
      // case V_SgDataStatementValue: {
      //   SgDataStatementValue* val = isSgDataStatementValue(node);
      //   break;
      // }
      // case V_SgFormatItem: {
      //   SgFormatItem* val = isSgFormatItem(node);
      //   break;
      // }
      // case V_SgFormatItemList: {
      //   SgFormatItemList* val = isSgFormatItemList(node);
      //   break;
      // }
      // case V_SgTypeTable: {
      //   SgTypeTable* val = isSgTypeTable(node);
      //   break;
      // }
      // case V_SgJavaImportStatementList: {
      //   SgJavaImportStatementList* val = isSgJavaImportStatementList(node);
      //   break;
      // }
      // case V_SgJavaClassDeclarationList: {
      //   SgJavaClassDeclarationList* val = isSgJavaClassDeclarationList(node);
      //   break;
      // }
      // case V_SgSupport: {
      //   SgSupport* val = isSgSupport(node);
      //   break;
      // }
      // case V_SgTypeUnknown: {
      //   SgTypeUnknown* val = isSgTypeUnknown(node);
      //   break;
      // }
      // case V_SgTypeChar: {
      //   SgTypeChar* val = isSgTypeChar(node);
      //   break;
      // }
      // case V_SgTypeSignedChar: {
      //   SgTypeSignedChar* val = isSgTypeSignedChar(node);
      //   break;
      // }
      // case V_SgTypeUnsignedChar: {
      //   SgTypeUnsignedChar* val = isSgTypeUnsignedChar(node);
      //   break;
      // }
      // case V_SgTypeShort: {
      //   SgTypeShort* val = isSgTypeShort(node);
      //   break;
      // }
      // case V_SgTypeSignedShort: {
      //   SgTypeSignedShort* val = isSgTypeSignedShort(node);
      //   break;
      // }
      // case V_SgTypeUnsignedShort: {
      //   SgTypeUnsignedShort* val = isSgTypeUnsignedShort(node);
      //   break;
      // }
      // case V_SgTypeInt: {
      //   SgTypeInt* val = isSgTypeInt(node);
      //   break;
      // }
      // case V_SgTypeSignedInt: {
      //   SgTypeSignedInt* val = isSgTypeSignedInt(node);
      //   break;
      // }
      // case V_SgTypeUnsignedInt: {
      //   SgTypeUnsignedInt* val = isSgTypeUnsignedInt(node);
      //   break;
      // }
      // case V_SgTypeLong: {
      //   SgTypeLong* val = isSgTypeLong(node);
      //   break;
      // }
      // case V_SgTypeSignedLong: {
      //   SgTypeSignedLong* val = isSgTypeSignedLong(node);
      //   break;
      // }
      // case V_SgTypeUnsignedLong: {
      //   SgTypeUnsignedLong* val = isSgTypeUnsignedLong(node);
      //   break;
      // }
      // case V_SgTypeVoid: {
      //   SgTypeVoid* val = isSgTypeVoid(node);
      //   break;
      // }
      // case V_SgTypeGlobalVoid: {
      //   SgTypeGlobalVoid* val = isSgTypeGlobalVoid(node);
      //   break;
      // }
      // case V_SgTypeWchar: {
      //   SgTypeWchar* val = isSgTypeWchar(node);
      //   break;
      // }
      // case V_SgTypeFloat: {
      //   SgTypeFloat* val = isSgTypeFloat(node);
      //   break;
      // }
      // case V_SgTypeDouble: {
      //   SgTypeDouble* val = isSgTypeDouble(node);
      //   break;
      // }
      // case V_SgTypeLongLong: {
      //   SgTypeLongLong* val = isSgTypeLongLong(node);
      //   break;
      // }
      // case V_SgTypeSignedLongLong: {
      //   SgTypeSignedLongLong* val = isSgTypeSignedLongLong(node);
      //   break;
      // }
      // case V_SgTypeUnsignedLongLong: {
      //   SgTypeUnsignedLongLong* val = isSgTypeUnsignedLongLong(node);
      //   break;
      // }
      // case V_SgTypeLongDouble: {
      //   SgTypeLongDouble* val = isSgTypeLongDouble(node);
      //   break;
      // }
      // case V_SgTypeString: {
      //   SgTypeString* val = isSgTypeString(node);
      //   break;
      // }
      // case V_SgTypeBool: {
      //   SgTypeBool* val = isSgTypeBool(node);
      //   break;
      // }
      // case V_SgTypeNullptr: {
      //   SgTypeNullptr* val = isSgTypeNullptr(node);
      //   break;
      // }
      // case V_SgTypeComplex: {
      //   SgTypeComplex* val = isSgTypeComplex(node);
      //   break;
      // }
      // case V_SgTypeImaginary: {
      //   SgTypeImaginary* val = isSgTypeImaginary(node);
      //   break;
      // }
      // case V_SgTypeDefault: {
      //   SgTypeDefault* val = isSgTypeDefault(node);
      //   break;
      // }
      // case V_SgPointerMemberType: {
      //   SgPointerMemberType* val = isSgPointerMemberType(node);
      //   break;
      // }
      // case V_SgReferenceType: {
      //   SgReferenceType* val = isSgReferenceType(node);
      //   break;
      // }
      // case V_SgRvalueReferenceType: {
      //   SgRvalueReferenceType* val = isSgRvalueReferenceType(node);
      //   break;
      // }
      // case V_SgDeclType: {
      //   SgDeclType* val = isSgDeclType(node);
      //   break;
      // }
      // case V_SgTypeCAFTeam: {
      //   SgTypeCAFTeam* val = isSgTypeCAFTeam(node);
      //   break;
      // }
      // case V_SgTypeUnsigned128bitInteger: {
      //   SgTypeUnsigned128bitInteger* val = isSgTypeUnsigned128bitInteger(node);
      //   break;
      // }
      // case V_SgTypeSigned128bitInteger: {
      //   SgTypeSigned128bitInteger* val = isSgTypeSigned128bitInteger(node);
      //   break;
      // }
      // case V_SgTypeLabel: {
      //   SgTypeLabel* val = isSgTypeLabel(node);
      //   break;
      // }
      // case V_SgJavaParameterizedType: {
      //   SgJavaParameterizedType* val = isSgJavaParameterizedType(node);
      //   break;
      // }
      // case V_SgJavaQualifiedType: {
      //   SgJavaQualifiedType* val = isSgJavaQualifiedType(node);
      //   break;
      // }
      // case V_SgJavaWildcardType: {
      //   SgJavaWildcardType* val = isSgJavaWildcardType(node);
      //   break;
      // }
      // case V_SgJavaUnionType: {
      //   SgJavaUnionType* val = isSgJavaUnionType(node);
      //   break;
      // }
      // case V_SgJavaParameterType: {
      //   SgJavaParameterType* val = isSgJavaParameterType(node);
      //   break;
      // }
      // case V_SgTemplateType: {
      //   SgTemplateType* val = isSgTemplateType(node);
      //   break;
      // }
      // case V_SgEnumType: {
      //   SgEnumType* val = isSgEnumType(node);
      //   break;
      // }
      // case V_SgTypedefType: {
      //   SgTypedefType* val = isSgTypedefType(node);
      //   break;
      // }
      // case V_SgModifierType: {
      //   SgModifierType* val = isSgModifierType(node);
      //   break;
      // }
      // case V_SgPartialFunctionModifierType: {
      //   SgPartialFunctionModifierType* val = isSgPartialFunctionModifierType(node);
      //   break;
      // }
      // case V_SgArrayType: {
      //   SgArrayType* val = isSgArrayType(node);
      //   break;
      // }
      // case V_SgTypeEllipse: {
      //   SgTypeEllipse* val = isSgTypeEllipse(node);
      //   break;
      // }
      // case V_SgTypeCrayPointer: {
      //   SgTypeCrayPointer* val = isSgTypeCrayPointer(node);
      //   break;
      // }
      // case V_SgPartialFunctionType: {
      //   SgPartialFunctionType* val = isSgPartialFunctionType(node);
      //   break;
      // }
      // case V_SgMemberFunctionType: {
      //   SgMemberFunctionType* val = isSgMemberFunctionType(node);
      //   break;
      // }
      // case V_SgFunctionType: {
      //   SgFunctionType* val = isSgFunctionType(node);
      //   break;
      // }
      // case V_SgPointerType: {
      //   SgPointerType* val = isSgPointerType(node);
      //   break;
      // }
      // case V_SgClassType: {
      //   SgClassType* val = isSgClassType(node);
      //   break;
      // }
      // case V_SgNamedType: {
      //   SgNamedType* val = isSgNamedType(node);
      //   break;
      // }
      // case V_SgQualifiedNameType: {
      //   SgQualifiedNameType* val = isSgQualifiedNameType(node);
      //   break;
      // }
      // case V_SgType: {
      //   SgType* val = isSgType(node);
      //   break;
      // }
      // case V_SgCatchStatementSeq: {
      //   SgCatchStatementSeq* val = isSgCatchStatementSeq(node);
      //   break;
      // }
      // case V_SgFunctionParameterList: {
      //   SgFunctionParameterList* val = isSgFunctionParameterList(node);
      //   break;
      // }
      // case V_SgCtorInitializerList: {
      //   SgCtorInitializerList* val = isSgCtorInitializerList(node);
      //   break;
      // }
      // case V_SgGlobal: {
      //   SgGlobal* val = isSgGlobal(node);
      //   break;
      // }
      // case V_SgSwitchStatement: {
      //   SgSwitchStatement* val = isSgSwitchStatement(node);
      //   break;
      // }
      // case V_SgCatchOptionStmt: {
      //   SgCatchOptionStmt* val = isSgCatchOptionStmt(node);
      //   break;
      // }
      // case V_SgVariableDefinition: {
      //   SgVariableDefinition* val = isSgVariableDefinition(node);
      //   break;
      // }
      // case V_SgStmtDeclarationStatement: {
      //   SgStmtDeclarationStatement* val = isSgStmtDeclarationStatement(node);
      //   break;
      // }
      // case V_SgEnumDeclaration: {
      //   SgEnumDeclaration* val = isSgEnumDeclaration(node);
      //   break;
      // }
      // case V_SgAsmStmt: {
      //   SgAsmStmt* val = isSgAsmStmt(node);
      //   break;
      // }
      // case V_SgFunctionTypeTable: {
      //   SgFunctionTypeTable* val = isSgFunctionTypeTable(node);
      //   break;
      // }
      // case V_SgCaseOptionStmt: {
      //   SgCaseOptionStmt* val = isSgCaseOptionStmt(node);
      //   break;
      // }
      // case V_SgTryStmt: {
      //   SgTryStmt* val = isSgTryStmt(node);
      //   break;
      // }
      // case V_SgDefaultOptionStmt: {
      //   SgDefaultOptionStmt* val = isSgDefaultOptionStmt(node);
      //   break;
      // }
      // case V_SgSpawnStmt: {
      //   SgSpawnStmt* val = isSgSpawnStmt(node);
      //   break;
      // }
      // case V_SgTemplateTypedefDeclaration: {
      //   SgTemplateTypedefDeclaration* val = isSgTemplateTypedefDeclaration(node);
      //   break;
      // }
      // // case V_SgTemplateInstantiationTypedefDeclaration: {
      // //   SgTemplateInstantiationTypedefDeclaration* val = isSgTemplateInstantiationTypedefDeclaration(node);
      // //   break;
      // // }
      // case V_SgTypedefDeclaration: {
      //   SgTypedefDeclaration* val = isSgTypedefDeclaration(node);
      //   break;
      // }
      // case V_SgNullStatement: {
      //   SgNullStatement* val = isSgNullStatement(node);
      //   break;
      // }
      // case V_SgVariantStatement: {
      //   SgVariantStatement* val = isSgVariantStatement(node);
      //   break;
      // }
      // case V_SgPragmaDeclaration: {
      //   SgPragmaDeclaration* val = isSgPragmaDeclaration(node);
      //   break;
      // }
      // case V_SgTemplateClassDeclaration: {
      //   SgTemplateClassDeclaration* val = isSgTemplateClassDeclaration(node);
      //   break;
      // }
      // case V_SgTemplateMemberFunctionDeclaration: {
      //   SgTemplateMemberFunctionDeclaration* val = isSgTemplateMemberFunctionDeclaration(node);
      //   break;
      // }
      // case V_SgTemplateFunctionDeclaration: {
      //   SgTemplateFunctionDeclaration* val = isSgTemplateFunctionDeclaration(node);
      //   break;
      // }
      // case V_SgTemplateDeclaration: {
      //   SgTemplateDeclaration* val = isSgTemplateDeclaration(node);
      //   break;
      // }
      // case V_SgTemplateInstantiationDecl: {
      //   SgTemplateInstantiationDecl* val = isSgTemplateInstantiationDecl(node);
      //   break;
      // }
      // case V_SgTemplateInstantiationDefn: {
      //   SgTemplateInstantiationDefn* val = isSgTemplateInstantiationDefn(node);
      //   break;
      // }
      // case V_SgTemplateInstantiationFunctionDecl: {
      //   SgTemplateInstantiationFunctionDecl* val = isSgTemplateInstantiationFunctionDecl(node);
      //   break;
      // }
      // case V_SgTemplateInstantiationMemberFunctionDecl: {
      //   SgTemplateInstantiationMemberFunctionDecl* val = isSgTemplateInstantiationMemberFunctionDecl(node);
      //   break;
      // }
      // case V_SgWithStatement: {
      //   SgWithStatement* val = isSgWithStatement(node);
      //   break;
      // }
      // case V_SgPythonGlobalStmt: {
      //   SgPythonGlobalStmt* val = isSgPythonGlobalStmt(node);
      //   break;
      // }
      // case V_SgPythonPrintStmt: {
      //   SgPythonPrintStmt* val = isSgPythonPrintStmt(node);
      //   break;
      // }
      // case V_SgPassStatement: {
      //   SgPassStatement* val = isSgPassStatement(node);
      //   break;
      // }
      // case V_SgAssertStmt: {
      //   SgAssertStmt* val = isSgAssertStmt(node);
      //   break;
      // }
      // case V_SgExecStatement: {
      //   SgExecStatement* val = isSgExecStatement(node);
      //   break;
      // }
      // case V_SgProgramHeaderStatement: {
      //   SgProgramHeaderStatement* val = isSgProgramHeaderStatement(node);
      //   break;
      // }
      // case V_SgProcedureHeaderStatement: {
      //   SgProcedureHeaderStatement* val = isSgProcedureHeaderStatement(node);
      //   break;
      // }
      // case V_SgEntryStatement: {
      //   SgEntryStatement* val = isSgEntryStatement(node);
      //   break;
      // }
      // case V_SgFortranNonblockedDo: {
      //   SgFortranNonblockedDo* val = isSgFortranNonblockedDo(node);
      //   break;
      // }
      // case V_SgInterfaceStatement: {
      //   SgInterfaceStatement* val = isSgInterfaceStatement(node);
      //   break;
      // }
      // case V_SgParameterStatement: {
      //   SgParameterStatement* val = isSgParameterStatement(node);
      //   break;
      // }
      // case V_SgCommonBlock: {
      //   SgCommonBlock* val = isSgCommonBlock(node);
      //   break;
      // }
      // case V_SgModuleStatement: {
      //   SgModuleStatement* val = isSgModuleStatement(node);
      //   break;
      // }
      // case V_SgUseStatement: {
      //   SgUseStatement* val = isSgUseStatement(node);
      //   break;
      // }
      // case V_SgStopOrPauseStatement: {
      //   SgStopOrPauseStatement* val = isSgStopOrPauseStatement(node);
      //   break;
      // }
      // case V_SgPrintStatement: {
      //   SgPrintStatement* val = isSgPrintStatement(node);
      //   break;
      // }
      // case V_SgReadStatement: {
      //   SgReadStatement* val = isSgReadStatement(node);
      //   break;
      // }
      // case V_SgWriteStatement: {
      //   SgWriteStatement* val = isSgWriteStatement(node);
      //   break;
      // }
      // case V_SgOpenStatement: {
      //   SgOpenStatement* val = isSgOpenStatement(node);
      //   break;
      // }
      // case V_SgCloseStatement: {
      //   SgCloseStatement* val = isSgCloseStatement(node);
      //   break;
      // }
      // case V_SgInquireStatement: {
      //   SgInquireStatement* val = isSgInquireStatement(node);
      //   break;
      // }
      // case V_SgFlushStatement: {
      //   SgFlushStatement* val = isSgFlushStatement(node);
      //   break;
      // }
      // case V_SgBackspaceStatement: {
      //   SgBackspaceStatement* val = isSgBackspaceStatement(node);
      //   break;
      // }
      // case V_SgRewindStatement: {
      //   SgRewindStatement* val = isSgRewindStatement(node);
      //   break;
      // }
      // case V_SgEndfileStatement: {
      //   SgEndfileStatement* val = isSgEndfileStatement(node);
      //   break;
      // }
      // case V_SgWaitStatement: {
      //   SgWaitStatement* val = isSgWaitStatement(node);
      //   break;
      // }
      // case V_SgCAFWithTeamStatement: {
      //   SgCAFWithTeamStatement* val = isSgCAFWithTeamStatement(node);
      //   break;
      // }
      // case V_SgFormatStatement: {
      //   SgFormatStatement* val = isSgFormatStatement(node);
      //   break;
      // }
      // case V_SgFortranDo: {
      //   SgFortranDo* val = isSgFortranDo(node);
      //   break;
      // }
      // case V_SgForAllStatement: {
      //   SgForAllStatement* val = isSgForAllStatement(node);
      //   break;
      // }
      // case V_SgIOStatement: {
      //   SgIOStatement* val = isSgIOStatement(node);
      //   break;
      // }
      // case V_SgUpcNotifyStatement: {
      //   SgUpcNotifyStatement* val = isSgUpcNotifyStatement(node);
      //   break;
      // }
      // case V_SgUpcWaitStatement: {
      //   SgUpcWaitStatement* val = isSgUpcWaitStatement(node);
      //   break;
      // }
      // case V_SgUpcBarrierStatement: {
      //   SgUpcBarrierStatement* val = isSgUpcBarrierStatement(node);
      //   break;
      // }
      // case V_SgUpcFenceStatement: {
      //   SgUpcFenceStatement* val = isSgUpcFenceStatement(node);
      //   break;
      // }
      // case V_SgUpcForAllStatement: {
      //   SgUpcForAllStatement* val = isSgUpcForAllStatement(node);
      //   break;
      // }
      // case V_SgOmpParallelStatement: {
      //   SgOmpParallelStatement* val = isSgOmpParallelStatement(node);
      //   break;
      // }
      // case V_SgOmpSingleStatement: {
      //   SgOmpSingleStatement* val = isSgOmpSingleStatement(node);
      //   break;
      // }
      // case V_SgOmpTaskStatement: {
      //   SgOmpTaskStatement* val = isSgOmpTaskStatement(node);
      //   break;
      // }
      // case V_SgOmpForStatement: {
      //   SgOmpForStatement* val = isSgOmpForStatement(node);
      //   break;
      // }
      // case V_SgOmpDoStatement: {
      //   SgOmpDoStatement* val = isSgOmpDoStatement(node);
      //   break;
      // }
      // case V_SgOmpSectionsStatement: {
      //   SgOmpSectionsStatement* val = isSgOmpSectionsStatement(node);
      //   break;
      // }
      // case V_SgOmpTargetStatement: {
      //   SgOmpTargetStatement* val = isSgOmpTargetStatement(node);
      //   break;
      // }
      // case V_SgOmpTargetDataStatement: {
      //   SgOmpTargetDataStatement* val = isSgOmpTargetDataStatement(node);
      //   break;
      // }
      // case V_SgOmpSimdStatement: {
      //   SgOmpSimdStatement* val = isSgOmpSimdStatement(node);
      //   break;
      // }
      // case V_SgOmpClauseBodyStatement: {
      //   SgOmpClauseBodyStatement* val = isSgOmpClauseBodyStatement(node);
      //   break;
      // }
      // case V_SgOmpAtomicStatement: {
      //   SgOmpAtomicStatement* val = isSgOmpAtomicStatement(node);
      //   break;
      // }
      // case V_SgOmpMasterStatement: {
      //   SgOmpMasterStatement* val = isSgOmpMasterStatement(node);
      //   break;
      // }
      // case V_SgOmpSectionStatement: {
      //   SgOmpSectionStatement* val = isSgOmpSectionStatement(node);
      //   break;
      // }
      // case V_SgOmpOrderedStatement: {
      //   SgOmpOrderedStatement* val = isSgOmpOrderedStatement(node);
      //   break;
      // }
      // case V_SgOmpWorkshareStatement: {
      //   SgOmpWorkshareStatement* val = isSgOmpWorkshareStatement(node);
      //   break;
      // }
      // case V_SgOmpCriticalStatement: {
      //   SgOmpCriticalStatement* val = isSgOmpCriticalStatement(node);
      //   break;
      // }
      // case V_SgOmpBodyStatement: {
      //   SgOmpBodyStatement* val = isSgOmpBodyStatement(node);
      //   break;
      // }
      // case V_SgJavaThrowStatement: {
      //   SgJavaThrowStatement* val = isSgJavaThrowStatement(node);
      //   break;
      // }
      // case V_SgJavaForEachStatement: {
      //   SgJavaForEachStatement* val = isSgJavaForEachStatement(node);
      //   break;
      // }
      // case V_SgJavaSynchronizedStatement: {
      //   SgJavaSynchronizedStatement* val = isSgJavaSynchronizedStatement(node);
      //   break;
      // }
      // case V_SgJavaLabelStatement: {
      //   SgJavaLabelStatement* val = isSgJavaLabelStatement(node);
      //   break;
      // }
      // case V_SgJavaImportStatement: {
      //   SgJavaImportStatement* val = isSgJavaImportStatement(node);
      //   break;
      // }
      // case V_SgJavaPackageDeclaration: {
      //   SgJavaPackageDeclaration* val = isSgJavaPackageDeclaration(node);
      //   break;
      // }
      // case V_SgJavaPackageStatement: {
      //   SgJavaPackageStatement* val = isSgJavaPackageStatement(node);
      //   break;
      // }
      // // case V_SgAsyncStmt: {
      // //   SgAsyncStmt* val = isSgAsyncStmt(node);
      // //   break;
      // // }
      // // case V_SgFinishStmt: {
      // //   SgFinishStmt* val = isSgFinishStmt(node);
      // //   break;
      // // }
      // // case V_SgAtStmt: {
      // //   SgAtStmt* val = isSgAtStmt(node);
      // //   break;
      // // }
      // case V_SgBlockDataStatement: {
      //   SgBlockDataStatement* val = isSgBlockDataStatement(node);
      //   break;
      // }
      // case V_SgImplicitStatement: {
      //   SgImplicitStatement* val = isSgImplicitStatement(node);
      //   break;
      // }
      // case V_SgStatementFunctionStatement: {
      //   SgStatementFunctionStatement* val = isSgStatementFunctionStatement(node);
      //   break;
      // }
      // case V_SgWhereStatement: {
      //   SgWhereStatement* val = isSgWhereStatement(node);
      //   break;
      // }
      // case V_SgNullifyStatement: {
      //   SgNullifyStatement* val = isSgNullifyStatement(node);
      //   break;
      // }
      // case V_SgEquivalenceStatement: {
      //   SgEquivalenceStatement* val = isSgEquivalenceStatement(node);
      //   break;
      // }
      // case V_SgDerivedTypeStatement: {
      //   SgDerivedTypeStatement* val = isSgDerivedTypeStatement(node);
      //   break;
      // }
      // case V_SgAttributeSpecificationStatement: {
      //   SgAttributeSpecificationStatement* val = isSgAttributeSpecificationStatement(node);
      //   break;
      // }
      // case V_SgAllocateStatement: {
      //   SgAllocateStatement* val = isSgAllocateStatement(node);
      //   break;
      // }
      // case V_SgDeallocateStatement: {
      //   SgDeallocateStatement* val = isSgDeallocateStatement(node);
      //   break;
      // }
      // case V_SgContainsStatement: {
      //   SgContainsStatement* val = isSgContainsStatement(node);
      //   break;
      // }
      // case V_SgSequenceStatement: {
      //   SgSequenceStatement* val = isSgSequenceStatement(node);
      //   break;
      // }
      // case V_SgElseWhereStatement: {
      //   SgElseWhereStatement* val = isSgElseWhereStatement(node);
      //   break;
      // }
      // case V_SgArithmeticIfStatement: {
      //   SgArithmeticIfStatement* val = isSgArithmeticIfStatement(node);
      //   break;
      // }
      // case V_SgAssignStatement: {
      //   SgAssignStatement* val = isSgAssignStatement(node);
      //   break;
      // }
      // case V_SgComputedGotoStatement: {
      //   SgComputedGotoStatement* val = isSgComputedGotoStatement(node);
      //   break;
      // }
      // case V_SgAssignedGotoStatement: {
      //   SgAssignedGotoStatement* val = isSgAssignedGotoStatement(node);
      //   break;
      // }
      // case V_SgNamelistStatement: {
      //   SgNamelistStatement* val = isSgNamelistStatement(node);
      //   break;
      // }
      // case V_SgImportStatement: {
      //   SgImportStatement* val = isSgImportStatement(node);
      //   break;
      // }
      // case V_SgAssociateStatement: {
      //   SgAssociateStatement* val = isSgAssociateStatement(node);
      //   break;
      // }
      // case V_SgFortranIncludeLine: {
      //   SgFortranIncludeLine* val = isSgFortranIncludeLine(node);
      //   break;
      // }
      // case V_SgNamespaceDeclarationStatement: {
      //   SgNamespaceDeclarationStatement* val = isSgNamespaceDeclarationStatement(node);
      //   break;
      // }
      // case V_SgNamespaceAliasDeclarationStatement: {
      //   SgNamespaceAliasDeclarationStatement* val = isSgNamespaceAliasDeclarationStatement(node);
      //   break;
      // }
      // case V_SgNamespaceDefinitionStatement: {
      //   SgNamespaceDefinitionStatement* val = isSgNamespaceDefinitionStatement(node);
      //   break;
      // }
      // case V_SgUsingDeclarationStatement: {
      //   SgUsingDeclarationStatement* val = isSgUsingDeclarationStatement(node);
      //   break;
      // }
      // case V_SgUsingDirectiveStatement: {
      //   SgUsingDirectiveStatement* val = isSgUsingDirectiveStatement(node);
      //   break;
      // }
      // case V_SgTemplateInstantiationDirectiveStatement: {
      //   SgTemplateInstantiationDirectiveStatement* val = isSgTemplateInstantiationDirectiveStatement(node);
      //   break;
      // }
      // case V_SgStaticAssertionDeclaration: {
      //   SgStaticAssertionDeclaration* val = isSgStaticAssertionDeclaration(node);
      //   break;
      // }
      // case V_SgTemplateClassDefinition: {
      //   SgTemplateClassDefinition* val = isSgTemplateClassDefinition(node);
      //   break;
      // }
      // case V_SgTemplateFunctionDefinition: {
      //   SgTemplateFunctionDefinition* val = isSgTemplateFunctionDefinition(node);
      //   break;
      // }
      // case V_SgClassDeclaration: {
      //   SgClassDeclaration* val = isSgClassDeclaration(node);
      //   break;
      // }
      // case V_SgClassDefinition: {
      //   SgClassDefinition* val = isSgClassDefinition(node);
      //   break;
      // }
      // case V_SgFunctionDefinition: {
      //   SgFunctionDefinition* val = isSgFunctionDefinition(node);
      //   break;
      // }
      // case V_SgScopeStatement: {
      //   SgScopeStatement* val = isSgScopeStatement(node);
      //   break;
      // }
      // case V_SgMemberFunctionDeclaration: {
      //   SgMemberFunctionDeclaration* val = isSgMemberFunctionDeclaration(node);
      //   break;
      // }
      // case V_SgFunctionDeclaration: {
      //   SgFunctionDeclaration* val = isSgFunctionDeclaration(node);
      //   break;
      // }
      // case V_SgIncludeDirectiveStatement: {
      //   SgIncludeDirectiveStatement* val = isSgIncludeDirectiveStatement(node);
      //   break;
      // }
      // case V_SgDefineDirectiveStatement: {
      //   SgDefineDirectiveStatement* val = isSgDefineDirectiveStatement(node);
      //   break;
      // }
      // case V_SgUndefDirectiveStatement: {
      //   SgUndefDirectiveStatement* val = isSgUndefDirectiveStatement(node);
      //   break;
      // }
      // case V_SgIfdefDirectiveStatement: {
      //   SgIfdefDirectiveStatement* val = isSgIfdefDirectiveStatement(node);
      //   break;
      // }
      // case V_SgIfndefDirectiveStatement: {
      //   SgIfndefDirectiveStatement* val = isSgIfndefDirectiveStatement(node);
      //   break;
      // }
      // case V_SgIfDirectiveStatement: {
      //   SgIfDirectiveStatement* val = isSgIfDirectiveStatement(node);
      //   break;
      // }
      // case V_SgDeadIfDirectiveStatement: {
      //   SgDeadIfDirectiveStatement* val = isSgDeadIfDirectiveStatement(node);
      //   break;
      // }
      // case V_SgElseDirectiveStatement: {
      //   SgElseDirectiveStatement* val = isSgElseDirectiveStatement(node);
      //   break;
      // }
      // case V_SgElseifDirectiveStatement: {
      //   SgElseifDirectiveStatement* val = isSgElseifDirectiveStatement(node);
      //   break;
      // }
      // case V_SgEndifDirectiveStatement: {
      //   SgEndifDirectiveStatement* val = isSgEndifDirectiveStatement(node);
      //   break;
      // }
      // case V_SgLineDirectiveStatement: {
      //   SgLineDirectiveStatement* val = isSgLineDirectiveStatement(node);
      //   break;
      // }
      // case V_SgWarningDirectiveStatement: {
      //   SgWarningDirectiveStatement* val = isSgWarningDirectiveStatement(node);
      //   break;
      // }
      // case V_SgErrorDirectiveStatement: {
      //   SgErrorDirectiveStatement* val = isSgErrorDirectiveStatement(node);
      //   break;
      // }
      // case V_SgEmptyDirectiveStatement: {
      //   SgEmptyDirectiveStatement* val = isSgEmptyDirectiveStatement(node);
      //   break;
      // }
      // case V_SgIncludeNextDirectiveStatement: {
      //   SgIncludeNextDirectiveStatement* val = isSgIncludeNextDirectiveStatement(node);
      //   break;
      // }
      // case V_SgIdentDirectiveStatement: {
      //   SgIdentDirectiveStatement* val = isSgIdentDirectiveStatement(node);
      //   break;
      // }
      // case V_SgLinemarkerDirectiveStatement: {
      //   SgLinemarkerDirectiveStatement* val = isSgLinemarkerDirectiveStatement(node);
      //   break;
      // }
      // case V_SgC_PreprocessorDirectiveStatement: {
      //   SgC_PreprocessorDirectiveStatement* val = isSgC_PreprocessorDirectiveStatement(node);
      //   break;
      // }
      // case V_SgClinkageStartStatement: {
      //   SgClinkageStartStatement* val = isSgClinkageStartStatement(node);
      //   break;
      // }
      // case V_SgClinkageEndStatement: {
      //   SgClinkageEndStatement* val = isSgClinkageEndStatement(node);
      //   break;
      // }
      // case V_SgClinkageDeclarationStatement: {
      //   SgClinkageDeclarationStatement* val = isSgClinkageDeclarationStatement(node);
      //   break;
      // }
      // case V_SgOmpFlushStatement: {
      //   SgOmpFlushStatement* val = isSgOmpFlushStatement(node);
      //   break;
      // }
      // case V_SgOmpBarrierStatement: {
      //   SgOmpBarrierStatement* val = isSgOmpBarrierStatement(node);
      //   break;
      // }
      // case V_SgOmpTaskwaitStatement: {
      //   SgOmpTaskwaitStatement* val = isSgOmpTaskwaitStatement(node);
      //   break;
      // }
      // case V_SgOmpThreadprivateStatement: {
      //   SgOmpThreadprivateStatement* val = isSgOmpThreadprivateStatement(node);
      //   break;
      // }
      // case V_SgMicrosoftAttributeDeclaration: {
      //   SgMicrosoftAttributeDeclaration* val = isSgMicrosoftAttributeDeclaration(node);
      //   break;
      // }
      // case V_SgDeclarationStatement: {
      //   SgDeclarationStatement* val = isSgDeclarationStatement(node);
      //   break;
      // }
      // case V_SgStatement: {
      //   SgStatement* val = isSgStatement(node);
      //   break;
      // }
      // case V_SgCompoundLiteralExp: {
      //   SgCompoundLiteralExp* val = isSgCompoundLiteralExp(node);
      //   break;
      // }
      // case V_SgLabelRefExp: {
      //   SgLabelRefExp* val = isSgLabelRefExp(node);
      //   break;
      // }
      // case V_SgClassNameRefExp: {
      //   SgClassNameRefExp* val = isSgClassNameRefExp(node);
      //   break;
      // }
      // case V_SgMemberFunctionRefExp: {
      //   SgMemberFunctionRefExp* val = isSgMemberFunctionRefExp(node);
      //   break;
      // }
      // case V_SgTemplateFunctionRefExp: {
      //   SgTemplateFunctionRefExp* val = isSgTemplateFunctionRefExp(node);
      //   break;
      // }
      // case V_SgTemplateMemberFunctionRefExp: {
      //   SgTemplateMemberFunctionRefExp* val = isSgTemplateMemberFunctionRefExp(node);
      //   break;
      // }
      // case V_SgSizeOfOp: {
      //   SgSizeOfOp* val = isSgSizeOfOp(node);
      //   break;
      // }
      // case V_SgAlignOfOp: {
      //   SgAlignOfOp* val = isSgAlignOfOp(node);
      //   break;
      // }
      // case V_SgJavaInstanceOfOp: {
      //   SgJavaInstanceOfOp* val = isSgJavaInstanceOfOp(node);
      //   break;
      // }
      // case V_SgJavaMarkerAnnotation: {
      //   SgJavaMarkerAnnotation* val = isSgJavaMarkerAnnotation(node);
      //   break;
      // }
      // case V_SgJavaSingleMemberAnnotation: {
      //   SgJavaSingleMemberAnnotation* val = isSgJavaSingleMemberAnnotation(node);
      //   break;
      // }
      // case V_SgJavaNormalAnnotation: {
      //   SgJavaNormalAnnotation* val = isSgJavaNormalAnnotation(node);
      //   break;
      // }
      // case V_SgJavaAnnotation: {
      //   SgJavaAnnotation* val = isSgJavaAnnotation(node);
      //   break;
      // }
      // case V_SgJavaTypeExpression: {
      //   SgJavaTypeExpression* val = isSgJavaTypeExpression(node);
      //   break;
      // }
      // case V_SgTypeExpression: {
      //   SgTypeExpression* val = isSgTypeExpression(node);
      //   break;
      // }
      // case V_SgLambdaExp: {
      //   SgLambdaExp* val = isSgLambdaExp(node);
      //   break;
      // }
      // case V_SgUpcLocalsizeofExpression: {
      //   SgUpcLocalsizeofExpression* val = isSgUpcLocalsizeofExpression(node);
      //   break;
      // }
      // case V_SgUpcBlocksizeofExpression: {
      //   SgUpcBlocksizeofExpression* val = isSgUpcBlocksizeofExpression(node);
      //   break;
      // }
      // case V_SgUpcElemsizeofExpression: {
      //   SgUpcElemsizeofExpression* val = isSgUpcElemsizeofExpression(node);
      //   break;
      // }
      // case V_SgVarArgStartOp: {
      //   SgVarArgStartOp* val = isSgVarArgStartOp(node);
      //   break;
      // }
      // case V_SgVarArgStartOneOperandOp: {
      //   SgVarArgStartOneOperandOp* val = isSgVarArgStartOneOperandOp(node);
      //   break;
      // }
      // case V_SgVarArgOp: {
      //   SgVarArgOp* val = isSgVarArgOp(node);
      //   break;
      // }
      // case V_SgVarArgEndOp: {
      //   SgVarArgEndOp* val = isSgVarArgEndOp(node);
      //   break;
      // }
      // case V_SgVarArgCopyOp: {
      //   SgVarArgCopyOp* val = isSgVarArgCopyOp(node);
      //   break;
      // }
      // case V_SgTypeIdOp: {
      //   SgTypeIdOp* val = isSgTypeIdOp(node);
      //   break;
      // }
      // case V_SgConditionalExp: {
      //   SgConditionalExp* val = isSgConditionalExp(node);
      //   break;
      // }
      // case V_SgNewExp: {
      //   SgNewExp* val = isSgNewExp(node);
      //   break;
      // }
      // case V_SgDeleteExp: {
      //   SgDeleteExp* val = isSgDeleteExp(node);
      //   break;
      // }
      // case V_SgThisExp: {
      //   SgThisExp* val = isSgThisExp(node);
      //   break;
      // }
      // case V_SgSuperExp: {
      //   SgSuperExp* val = isSgSuperExp(node);
      //   break;
      // }
      // case V_SgClassExp: {
      //   SgClassExp* val = isSgClassExp(node);
      //   break;
      // }
      // case V_SgRefExp: {
      //   SgRefExp* val = isSgRefExp(node);
      //   break;
      // }
      // case V_SgAggregateInitializer: {
      //   SgAggregateInitializer* val = isSgAggregateInitializer(node);
      //   break;
      // }
      // case V_SgCompoundInitializer: {
      //   SgCompoundInitializer* val = isSgCompoundInitializer(node);
      //   break;
      // }
      // case V_SgExpressionRoot: {
      //   SgExpressionRoot* val = isSgExpressionRoot(node);
      //   break;
      // }
      // case V_SgRealPartOp: {
      //   SgRealPartOp* val = isSgRealPartOp(node);
      //   break;
      // }
      // case V_SgImagPartOp: {
      //   SgImagPartOp* val = isSgImagPartOp(node);
      //   break;
      // }
      // case V_SgConjugateOp: {
      //   SgConjugateOp* val = isSgConjugateOp(node);
      //   break;
      // }
      // case V_SgThrowOp: {
      //   SgThrowOp* val = isSgThrowOp(node);
      //   break;
      // }
      // case V_SgCommaOpExp: {
      //   SgCommaOpExp* val = isSgCommaOpExp(node);
      //   break;
      // }
      // case V_SgJavaUnsignedRshiftOp: {
      //   SgJavaUnsignedRshiftOp* val = isSgJavaUnsignedRshiftOp(node);
      //   break;
      // }
      // case V_SgScopeOp: {
      //   SgScopeOp* val = isSgScopeOp(node);
      //   break;
      // }
      // case V_SgJavaUnsignedRshiftAssignOp: {
      //   SgJavaUnsignedRshiftAssignOp* val = isSgJavaUnsignedRshiftAssignOp(node);
      //   break;
      // }
      // case V_SgIntegerDivideAssignOp: {
      //   SgIntegerDivideAssignOp* val = isSgIntegerDivideAssignOp(node);
      //   break;
      // }
      // case V_SgExponentiationAssignOp: {
      //   SgExponentiationAssignOp* val = isSgExponentiationAssignOp(node);
      //   break;
      // }
      // case V_SgConcatenationOp: {
      //   SgConcatenationOp* val = isSgConcatenationOp(node);
      //   break;
      // }
      // case V_SgNaryComparisonOp: {
      //   SgNaryComparisonOp* val = isSgNaryComparisonOp(node);
      //   break;
      // }
      // case V_SgNaryBooleanOp: {
      //   SgNaryBooleanOp* val = isSgNaryBooleanOp(node);
      //   break;
      // }
      // case V_SgBoolValExp: {
      //   SgBoolValExp* val = isSgBoolValExp(node);
      //   break;
      // }
      // case V_SgNullptrValExp: {
      //   SgNullptrValExp* val = isSgNullptrValExp(node);
      //   break;
      // }
      // case V_SgFunctionParameterRefExp: {
      //   SgFunctionParameterRefExp* val = isSgFunctionParameterRefExp(node);
      //   break;
      // }
      // case V_SgTemplateParameterVal: {
      //   SgTemplateParameterVal* val = isSgTemplateParameterVal(node);
      //   break;
      // }
      // case V_SgUpcThreads: {
      //   SgUpcThreads* val = isSgUpcThreads(node);
      //   break;
      // }
      // case V_SgUpcMythread: {
      //   SgUpcMythread* val = isSgUpcMythread(node);
      //   break;
      // }
      // case V_SgComplexVal: {
      //   SgComplexVal* val = isSgComplexVal(node);
      //   break;
      // }
      // case V_SgVariantExpression: {
      //   SgVariantExpression* val = isSgVariantExpression(node);
      //   break;
      // }
      // case V_SgStatementExpression: {
      //   SgStatementExpression* val = isSgStatementExpression(node);
      //   break;
      // }
      // case V_SgAsmOp: {
      //   SgAsmOp* val = isSgAsmOp(node);
      //   break;
      // }
      // case V_SgCudaKernelExecConfig: {
      //   SgCudaKernelExecConfig* val = isSgCudaKernelExecConfig(node);
      //   break;
      // }
      // case V_SgCudaKernelCallExp: {
      //   SgCudaKernelCallExp* val = isSgCudaKernelCallExp(node);
      //   break;
      // }
      // case V_SgLambdaRefExp: {
      //   SgLambdaRefExp* val = isSgLambdaRefExp(node);
      //   break;
      // }
      // case V_SgTupleExp: {
      //   SgTupleExp* val = isSgTupleExp(node);
      //   break;
      // }
      // case V_SgListExp: {
      //   SgListExp* val = isSgListExp(node);
      //   break;
      // }
      // case V_SgDictionaryExp: {
      //   SgDictionaryExp* val = isSgDictionaryExp(node);
      //   break;
      // }
      // case V_SgKeyDatumPair: {
      //   SgKeyDatumPair* val = isSgKeyDatumPair(node);
      //   break;
      // }
      // case V_SgComprehension: {
      //   SgComprehension* val = isSgComprehension(node);
      //   break;
      // }
      // case V_SgListComprehension: {
      //   SgListComprehension* val = isSgListComprehension(node);
      //   break;
      // }
      // case V_SgSetComprehension: {
      //   SgSetComprehension* val = isSgSetComprehension(node);
      //   break;
      // }
      // case V_SgDictionaryComprehension: {
      //   SgDictionaryComprehension* val = isSgDictionaryComprehension(node);
      //   break;
      // }
      // case V_SgMembershipOp: {
      //   SgMembershipOp* val = isSgMembershipOp(node);
      //   break;
      // }
      // case V_SgNonMembershipOp: {
      //   SgNonMembershipOp* val = isSgNonMembershipOp(node);
      //   break;
      // }
      // case V_SgIsOp: {
      //   SgIsOp* val = isSgIsOp(node);
      //   break;
      // }
      // case V_SgIsNotOp: {
      //   SgIsNotOp* val = isSgIsNotOp(node);
      //   break;
      // }
      // case V_SgStringConversion: {
      //   SgStringConversion* val = isSgStringConversion(node);
      //   break;
      // }
      // case V_SgYieldExpression: {
      //   SgYieldExpression* val = isSgYieldExpression(node);
      //   break;
      // }
      // // case V_SgHereExp: {
      // //   SgHereExp* val = isSgHereExp(node);
      // //   break;
      // // }
      // case V_SgSubscriptExpression: {
      //   SgSubscriptExpression* val = isSgSubscriptExpression(node);
      //   break;
      // }
      // case V_SgColonShapeExp: {
      //   SgColonShapeExp* val = isSgColonShapeExp(node);
      //   break;
      // }
      // case V_SgAsteriskShapeExp: {
      //   SgAsteriskShapeExp* val = isSgAsteriskShapeExp(node);
      //   break;
      // }
      // case V_SgIOItemExpression: {
      //   SgIOItemExpression* val = isSgIOItemExpression(node);
      //   break;
      // }
      // case V_SgImpliedDo: {
      //   SgImpliedDo* val = isSgImpliedDo(node);
      //   break;
      // }
      // case V_SgExponentiationOp: {
      //   SgExponentiationOp* val = isSgExponentiationOp(node);
      //   break;
      // }
      // case V_SgUnknownArrayOrFunctionReference: {
      //   SgUnknownArrayOrFunctionReference* val = isSgUnknownArrayOrFunctionReference(node);
      //   break;
      // }
      // case V_SgActualArgumentExpression: {
      //   SgActualArgumentExpression* val = isSgActualArgumentExpression(node);
      //   break;
      // }
      // case V_SgUserDefinedBinaryOp: {
      //   SgUserDefinedBinaryOp* val = isSgUserDefinedBinaryOp(node);
      //   break;
      // }
      // case V_SgPointerAssignOp: {
      //   SgPointerAssignOp* val = isSgPointerAssignOp(node);
      //   break;
      // }
      // case V_SgCAFCoExpression: {
      //   SgCAFCoExpression* val = isSgCAFCoExpression(node);
      //   break;
      // }
      // case V_SgDesignatedInitializer: {
      //   SgDesignatedInitializer* val = isSgDesignatedInitializer(node);
      //   break;
      // }
      // case V_SgInitializer: {
      //   SgInitializer* val = isSgInitializer(node);
      //   break;
      // }
      // case V_SgUserDefinedUnaryOp: {
      //   SgUserDefinedUnaryOp* val = isSgUserDefinedUnaryOp(node);
      //   break;
      // }
      // case V_SgPseudoDestructorRefExp: {
      //   SgPseudoDestructorRefExp* val = isSgPseudoDestructorRefExp(node);
      //   break;
      // }
      // case V_SgUnaryOp: {
      //   SgUnaryOp* val = isSgUnaryOp(node);
      //   break;
      // }
      // case V_SgCompoundAssignOp: {
      //   SgCompoundAssignOp* val = isSgCompoundAssignOp(node);
      //   break;
      // }
      // case V_SgBinaryOp: {
      //   SgBinaryOp* val = isSgBinaryOp(node);
      //   break;
      // }
      // case V_SgNaryOp: {
      //   SgNaryOp* val = isSgNaryOp(node);
      //   break;
      // }
      // case V_SgValueExp: {
      //   SgValueExp* val = isSgValueExp(node);
      //   break;
      // }
      // case V_SgCallExpression: {
      //   SgCallExpression* val = isSgCallExpression(node);
      //   break;
      // }
      // case V_SgTypeTraitBuiltinOperator: {
      //   SgTypeTraitBuiltinOperator* val = isSgTypeTraitBuiltinOperator(node);
      //   break;
      // }
      // case V_SgExpression: {
      //   SgExpression* val = isSgExpression(node);
      //   break;
      // }
      // case V_SgTemplateVariableSymbol: {
      //   SgTemplateVariableSymbol* val = isSgTemplateVariableSymbol(node);
      //   break;
      // }
      // case V_SgVariableSymbol: {
      //   SgVariableSymbol* val = isSgVariableSymbol(node);
      //   break;
      // }
      // case V_SgFunctionTypeSymbol: {
      //   SgFunctionTypeSymbol* val = isSgFunctionTypeSymbol(node);
      //   break;
      // }
      // case V_SgTemplateClassSymbol: {
      //   SgTemplateClassSymbol* val = isSgTemplateClassSymbol(node);
      //   break;
      // }
      // case V_SgClassSymbol: {
      //   SgClassSymbol* val = isSgClassSymbol(node);
      //   break;
      // }
      // case V_SgTemplateSymbol: {
      //   SgTemplateSymbol* val = isSgTemplateSymbol(node);
      //   break;
      // }
      // case V_SgEnumSymbol: {
      //   SgEnumSymbol* val = isSgEnumSymbol(node);
      //   break;
      // }
      // case V_SgEnumFieldSymbol: {
      //   SgEnumFieldSymbol* val = isSgEnumFieldSymbol(node);
      //   break;
      // }
      // // case V_SgTemplateTypedefSymbol: {
      // //   SgTemplateTypedefSymbol* val = isSgTemplateTypedefSymbol(node);
      // //   break;
      // // }
      // case V_SgTypedefSymbol: {
      //   SgTypedefSymbol* val = isSgTypedefSymbol(node);
      //   break;
      // }
      // case V_SgTemplateFunctionSymbol: {
      //   SgTemplateFunctionSymbol* val = isSgTemplateFunctionSymbol(node);
      //   break;
      // }
      // case V_SgTemplateMemberFunctionSymbol: {
      //   SgTemplateMemberFunctionSymbol* val = isSgTemplateMemberFunctionSymbol(node);
      //   break;
      // }
      // case V_SgLabelSymbol: {
      //   SgLabelSymbol* val = isSgLabelSymbol(node);
      //   break;
      // }
      // case V_SgJavaLabelSymbol: {
      //   SgJavaLabelSymbol* val = isSgJavaLabelSymbol(node);
      //   break;
      // }
      // case V_SgDefaultSymbol: {
      //   SgDefaultSymbol* val = isSgDefaultSymbol(node);
      //   break;
      // }
      // case V_SgNamespaceSymbol: {
      //   SgNamespaceSymbol* val = isSgNamespaceSymbol(node);
      //   break;
      // }
      // case V_SgIntrinsicSymbol: {
      //   SgIntrinsicSymbol* val = isSgIntrinsicSymbol(node);
      //   break;
      // }
      // case V_SgModuleSymbol: {
      //   SgModuleSymbol* val = isSgModuleSymbol(node);
      //   break;
      // }
      // case V_SgInterfaceSymbol: {
      //   SgInterfaceSymbol* val = isSgInterfaceSymbol(node);
      //   break;
      // }
      // case V_SgCommonSymbol: {
      //   SgCommonSymbol* val = isSgCommonSymbol(node);
      //   break;
      // }
      // case V_SgRenameSymbol: {
      //   SgRenameSymbol* val = isSgRenameSymbol(node);
      //   break;
      // }
      // case V_SgMemberFunctionSymbol: {
      //   SgMemberFunctionSymbol* val = isSgMemberFunctionSymbol(node);
      //   break;
      // }
      // case V_SgFunctionSymbol: {
      //   SgFunctionSymbol* val = isSgFunctionSymbol(node);
      //   break;
      // }
      // case V_SgAsmBinaryAddressSymbol: {
      //   SgAsmBinaryAddressSymbol* val = isSgAsmBinaryAddressSymbol(node);
      //   break;
      // }
      // case V_SgAsmBinaryDataSymbol: {
      //   SgAsmBinaryDataSymbol* val = isSgAsmBinaryDataSymbol(node);
      //   break;
      // }
      // case V_SgAliasSymbol: {
      //   SgAliasSymbol* val = isSgAliasSymbol(node);
      //   break;
      // }
      // case V_SgSymbol: {
      //   SgSymbol* val = isSgSymbol(node);
      //   break;
      // }
      // case V_SgAsmArmInstruction: {
      //   SgAsmArmInstruction* val = isSgAsmArmInstruction(node);
      //   break;
      // }
      // case V_SgAsmX86Instruction: {
      //   SgAsmX86Instruction* val = isSgAsmX86Instruction(node);
      //   break;
      // }
      // case V_SgAsmPowerpcInstruction: {
      //   SgAsmPowerpcInstruction* val = isSgAsmPowerpcInstruction(node);
      //   break;
      // }
      // case V_SgAsmMipsInstruction: {
      //   SgAsmMipsInstruction* val = isSgAsmMipsInstruction(node);
      //   break;
      // }
      // case V_SgAsmM68kInstruction: {
      //   SgAsmM68kInstruction* val = isSgAsmM68kInstruction(node);
      //   break;
      // }
      // case V_SgAsmInstruction: {
      //   SgAsmInstruction* val = isSgAsmInstruction(node);
      //   break;
      // }
      // case V_SgAsmOperandList: {
      //   SgAsmOperandList* val = isSgAsmOperandList(node);
      //   break;
      // }
      // case V_SgAsmBinaryAdd: {
      //   SgAsmBinaryAdd* val = isSgAsmBinaryAdd(node);
      //   break;
      // }
      // case V_SgAsmBinarySubtract: {
      //   SgAsmBinarySubtract* val = isSgAsmBinarySubtract(node);
      //   break;
      // }
      // case V_SgAsmBinaryMultiply: {
      //   SgAsmBinaryMultiply* val = isSgAsmBinaryMultiply(node);
      //   break;
      // }
      // case V_SgAsmBinaryDivide: {
      //   SgAsmBinaryDivide* val = isSgAsmBinaryDivide(node);
      //   break;
      // }
      // case V_SgAsmBinaryMod: {
      //   SgAsmBinaryMod* val = isSgAsmBinaryMod(node);
      //   break;
      // }
      // case V_SgAsmBinaryAddPreupdate: {
      //   SgAsmBinaryAddPreupdate* val = isSgAsmBinaryAddPreupdate(node);
      //   break;
      // }
      // case V_SgAsmBinarySubtractPreupdate: {
      //   SgAsmBinarySubtractPreupdate* val = isSgAsmBinarySubtractPreupdate(node);
      //   break;
      // }
      // case V_SgAsmBinaryAddPostupdate: {
      //   SgAsmBinaryAddPostupdate* val = isSgAsmBinaryAddPostupdate(node);
      //   break;
      // }
      // case V_SgAsmBinarySubtractPostupdate: {
      //   SgAsmBinarySubtractPostupdate* val = isSgAsmBinarySubtractPostupdate(node);
      //   break;
      // }
      // case V_SgAsmBinaryLsl: {
      //   SgAsmBinaryLsl* val = isSgAsmBinaryLsl(node);
      //   break;
      // }
      // case V_SgAsmBinaryLsr: {
      //   SgAsmBinaryLsr* val = isSgAsmBinaryLsr(node);
      //   break;
      // }
      // case V_SgAsmBinaryAsr: {
      //   SgAsmBinaryAsr* val = isSgAsmBinaryAsr(node);
      //   break;
      // }
      // case V_SgAsmBinaryRor: {
      //   SgAsmBinaryRor* val = isSgAsmBinaryRor(node);
      //   break;
      // }
      // case V_SgAsmBinaryExpression: {
      //   SgAsmBinaryExpression* val = isSgAsmBinaryExpression(node);
      //   break;
      // }
      // case V_SgAsmUnaryPlus: {
      //   SgAsmUnaryPlus* val = isSgAsmUnaryPlus(node);
      //   break;
      // }
      // case V_SgAsmUnaryMinus: {
      //   SgAsmUnaryMinus* val = isSgAsmUnaryMinus(node);
      //   break;
      // }
      // case V_SgAsmUnaryRrx: {
      //   SgAsmUnaryRrx* val = isSgAsmUnaryRrx(node);
      //   break;
      // }
      // case V_SgAsmUnaryArmSpecialRegisterList: {
      //   SgAsmUnaryArmSpecialRegisterList* val = isSgAsmUnaryArmSpecialRegisterList(node);
      //   break;
      // }
      // case V_SgAsmUnaryExpression: {
      //   SgAsmUnaryExpression* val = isSgAsmUnaryExpression(node);
      //   break;
      // }
      // case V_SgAsmDirectRegisterExpression: {
      //   SgAsmDirectRegisterExpression* val = isSgAsmDirectRegisterExpression(node);
      //   break;
      // }
      // case V_SgAsmIndirectRegisterExpression: {
      //   SgAsmIndirectRegisterExpression* val = isSgAsmIndirectRegisterExpression(node);
      //   break;
      // }
      // case V_SgAsmRegisterReferenceExpression: {
      //   SgAsmRegisterReferenceExpression* val = isSgAsmRegisterReferenceExpression(node);
      //   break;
      // }
      // case V_SgAsmRegisterNames: {
      //   SgAsmRegisterNames* val = isSgAsmRegisterNames(node);
      //   break;
      // }
      // case V_SgAsmIntegerValueExpression: {
      //   SgAsmIntegerValueExpression* val = isSgAsmIntegerValueExpression(node);
      //   break;
      // }
      // case V_SgAsmFloatValueExpression: {
      //   SgAsmFloatValueExpression* val = isSgAsmFloatValueExpression(node);
      //   break;
      // }
      // case V_SgAsmConstantExpression: {
      //   SgAsmConstantExpression* val = isSgAsmConstantExpression(node);
      //   break;
      // }
      // case V_SgAsmValueExpression: {
      //   SgAsmValueExpression* val = isSgAsmValueExpression(node);
      //   break;
      // }
      // case V_SgAsmMemoryReferenceExpression: {
      //   SgAsmMemoryReferenceExpression* val = isSgAsmMemoryReferenceExpression(node);
      //   break;
      // }
      // case V_SgAsmControlFlagsExpression: {
      //   SgAsmControlFlagsExpression* val = isSgAsmControlFlagsExpression(node);
      //   break;
      // }
      // case V_SgAsmCommonSubExpression: {
      //   SgAsmCommonSubExpression* val = isSgAsmCommonSubExpression(node);
      //   break;
      // }
      // case V_SgAsmExprListExp: {
      //   SgAsmExprListExp* val = isSgAsmExprListExp(node);
      //   break;
      // }
      // case V_SgAsmExpression: {
      //   SgAsmExpression* val = isSgAsmExpression(node);
      //   break;
      // }
      // case V_SgAsmIntegerType: {
      //   SgAsmIntegerType* val = isSgAsmIntegerType(node);
      //   break;
      // }
      // case V_SgAsmFloatType: {
      //   SgAsmFloatType* val = isSgAsmFloatType(node);
      //   break;
      // }
      // case V_SgAsmScalarType: {
      //   SgAsmScalarType* val = isSgAsmScalarType(node);
      //   break;
      // }
      // case V_SgAsmVectorType: {
      //   SgAsmVectorType* val = isSgAsmVectorType(node);
      //   break;
      // }
      // case V_SgAsmType: {
      //   SgAsmType* val = isSgAsmType(node);
      //   break;
      // }
      // case V_SgAsmFunction: {
      //   SgAsmFunction* val = isSgAsmFunction(node);
      //   break;
      // }
      // case V_SgAsmBlock: {
      //   SgAsmBlock* val = isSgAsmBlock(node);
      //   break;
      // }
      // case V_SgAsmStaticData: {
      //   SgAsmStaticData* val = isSgAsmStaticData(node);
      //   break;
      // }
      // case V_SgAsmSynthesizedDataStructureDeclaration: {
      //   SgAsmSynthesizedDataStructureDeclaration* val = isSgAsmSynthesizedDataStructureDeclaration(node);
      //   break;
      // }
      // case V_SgAsmSynthesizedFieldDeclaration: {
      //   SgAsmSynthesizedFieldDeclaration* val = isSgAsmSynthesizedFieldDeclaration(node);
      //   break;
      // }
      // case V_SgAsmSynthesizedDeclaration: {
      //   SgAsmSynthesizedDeclaration* val = isSgAsmSynthesizedDeclaration(node);
      //   break;
      // }
      // case V_SgAsmStatement: {
      //   SgAsmStatement* val = isSgAsmStatement(node);
      //   break;
      // }
      // case V_SgAsmInterpretationList: {
      //   SgAsmInterpretationList* val = isSgAsmInterpretationList(node);
      //   break;
      // }
      // case V_SgAsmInterpretation: {
      //   SgAsmInterpretation* val = isSgAsmInterpretation(node);
      //   break;
      // }
      // case V_SgAsmElfFileHeader: {
      //   SgAsmElfFileHeader* val = isSgAsmElfFileHeader(node);
      //   break;
      // }
      // case V_SgAsmElfSectionTable: {
      //   SgAsmElfSectionTable* val = isSgAsmElfSectionTable(node);
      //   break;
      // }
      // case V_SgAsmElfSectionTableEntry: {
      //   SgAsmElfSectionTableEntry* val = isSgAsmElfSectionTableEntry(node);
      //   break;
      // }
      // case V_SgAsmElfSegmentTable: {
      //   SgAsmElfSegmentTable* val = isSgAsmElfSegmentTable(node);
      //   break;
      // }
      // case V_SgAsmElfSegmentTableEntryList: {
      //   SgAsmElfSegmentTableEntryList* val = isSgAsmElfSegmentTableEntryList(node);
      //   break;
      // }
      // case V_SgAsmElfSegmentTableEntry: {
      //   SgAsmElfSegmentTableEntry* val = isSgAsmElfSegmentTableEntry(node);
      //   break;
      // }
      // case V_SgAsmElfSymbolSection: {
      //   SgAsmElfSymbolSection* val = isSgAsmElfSymbolSection(node);
      //   break;
      // }
      // case V_SgAsmElfSymbolList: {
      //   SgAsmElfSymbolList* val = isSgAsmElfSymbolList(node);
      //   break;
      // }
      // case V_SgAsmElfSymbol: {
      //   SgAsmElfSymbol* val = isSgAsmElfSymbol(node);
      //   break;
      // }
      // case V_SgAsmElfSymverSection: {
      //   SgAsmElfSymverSection* val = isSgAsmElfSymverSection(node);
      //   break;
      // }
      // case V_SgAsmElfSymverEntry: {
      //   SgAsmElfSymverEntry* val = isSgAsmElfSymverEntry(node);
      //   break;
      // }
      // case V_SgAsmElfSymverEntryList: {
      //   SgAsmElfSymverEntryList* val = isSgAsmElfSymverEntryList(node);
      //   break;
      // }
      // case V_SgAsmElfSymverDefinedSection: {
      //   SgAsmElfSymverDefinedSection* val = isSgAsmElfSymverDefinedSection(node);
      //   break;
      // }
      // case V_SgAsmElfSymverDefinedEntryList: {
      //   SgAsmElfSymverDefinedEntryList* val = isSgAsmElfSymverDefinedEntryList(node);
      //   break;
      // }
      // case V_SgAsmElfSymverDefinedEntry: {
      //   SgAsmElfSymverDefinedEntry* val = isSgAsmElfSymverDefinedEntry(node);
      //   break;
      // }
      // case V_SgAsmElfSymverDefinedAuxList: {
      //   SgAsmElfSymverDefinedAuxList* val = isSgAsmElfSymverDefinedAuxList(node);
      //   break;
      // }
      // case V_SgAsmElfSymverDefinedAux: {
      //   SgAsmElfSymverDefinedAux* val = isSgAsmElfSymverDefinedAux(node);
      //   break;
      // }
      // case V_SgAsmElfSymverNeededSection: {
      //   SgAsmElfSymverNeededSection* val = isSgAsmElfSymverNeededSection(node);
      //   break;
      // }
      // case V_SgAsmElfSymverNeededEntryList: {
      //   SgAsmElfSymverNeededEntryList* val = isSgAsmElfSymverNeededEntryList(node);
      //   break;
      // }
      // case V_SgAsmElfSymverNeededEntry: {
      //   SgAsmElfSymverNeededEntry* val = isSgAsmElfSymverNeededEntry(node);
      //   break;
      // }
      // case V_SgAsmElfSymverNeededAuxList: {
      //   SgAsmElfSymverNeededAuxList* val = isSgAsmElfSymverNeededAuxList(node);
      //   break;
      // }
      // case V_SgAsmElfSymverNeededAux: {
      //   SgAsmElfSymverNeededAux* val = isSgAsmElfSymverNeededAux(node);
      //   break;
      // }
      // case V_SgAsmElfRelocSection: {
      //   SgAsmElfRelocSection* val = isSgAsmElfRelocSection(node);
      //   break;
      // }
      // case V_SgAsmElfRelocEntryList: {
      //   SgAsmElfRelocEntryList* val = isSgAsmElfRelocEntryList(node);
      //   break;
      // }
      // case V_SgAsmElfRelocEntry: {
      //   SgAsmElfRelocEntry* val = isSgAsmElfRelocEntry(node);
      //   break;
      // }
      // case V_SgAsmElfDynamicSection: {
      //   SgAsmElfDynamicSection* val = isSgAsmElfDynamicSection(node);
      //   break;
      // }
      // case V_SgAsmElfDynamicEntryList: {
      //   SgAsmElfDynamicEntryList* val = isSgAsmElfDynamicEntryList(node);
      //   break;
      // }
      // case V_SgAsmElfDynamicEntry: {
      //   SgAsmElfDynamicEntry* val = isSgAsmElfDynamicEntry(node);
      //   break;
      // }
      // case V_SgAsmElfStringSection: {
      //   SgAsmElfStringSection* val = isSgAsmElfStringSection(node);
      //   break;
      // }
      // case V_SgAsmElfStrtab: {
      //   SgAsmElfStrtab* val = isSgAsmElfStrtab(node);
      //   break;
      // }
      // case V_SgAsmElfNoteSection: {
      //   SgAsmElfNoteSection* val = isSgAsmElfNoteSection(node);
      //   break;
      // }
      // case V_SgAsmElfNoteEntryList: {
      //   SgAsmElfNoteEntryList* val = isSgAsmElfNoteEntryList(node);
      //   break;
      // }
      // case V_SgAsmElfNoteEntry: {
      //   SgAsmElfNoteEntry* val = isSgAsmElfNoteEntry(node);
      //   break;
      // }
      // case V_SgAsmElfEHFrameSection: {
      //   SgAsmElfEHFrameSection* val = isSgAsmElfEHFrameSection(node);
      //   break;
      // }
      // case V_SgAsmElfEHFrameEntryCIList: {
      //   SgAsmElfEHFrameEntryCIList* val = isSgAsmElfEHFrameEntryCIList(node);
      //   break;
      // }
      // case V_SgAsmElfEHFrameEntryCI: {
      //   SgAsmElfEHFrameEntryCI* val = isSgAsmElfEHFrameEntryCI(node);
      //   break;
      // }
      // case V_SgAsmElfEHFrameEntryFDList: {
      //   SgAsmElfEHFrameEntryFDList* val = isSgAsmElfEHFrameEntryFDList(node);
      //   break;
      // }
      // case V_SgAsmElfEHFrameEntryFD: {
      //   SgAsmElfEHFrameEntryFD* val = isSgAsmElfEHFrameEntryFD(node);
      //   break;
      // }
      // case V_SgAsmElfSection: {
      //   SgAsmElfSection* val = isSgAsmElfSection(node);
      //   break;
      // }
      // case V_SgAsmDOSFileHeader: {
      //   SgAsmDOSFileHeader* val = isSgAsmDOSFileHeader(node);
      //   break;
      // }
      // case V_SgAsmDOSExtendedHeader: {
      //   SgAsmDOSExtendedHeader* val = isSgAsmDOSExtendedHeader(node);
      //   break;
      // }
      // case V_SgAsmPEFileHeader: {
      //   SgAsmPEFileHeader* val = isSgAsmPEFileHeader(node);
      //   break;
      // }
      // case V_SgAsmPERVASizePairList: {
      //   SgAsmPERVASizePairList* val = isSgAsmPERVASizePairList(node);
      //   break;
      // }
      // case V_SgAsmPERVASizePair: {
      //   SgAsmPERVASizePair* val = isSgAsmPERVASizePair(node);
      //   break;
      // }
      // case V_SgAsmPEImportItem: {
      //   SgAsmPEImportItem* val = isSgAsmPEImportItem(node);
      //   break;
      // }
      // case V_SgAsmPEImportItemList: {
      //   SgAsmPEImportItemList* val = isSgAsmPEImportItemList(node);
      //   break;
      // }
      // case V_SgAsmPEImportDirectory: {
      //   SgAsmPEImportDirectory* val = isSgAsmPEImportDirectory(node);
      //   break;
      // }
      // case V_SgAsmPEImportDirectoryList: {
      //   SgAsmPEImportDirectoryList* val = isSgAsmPEImportDirectoryList(node);
      //   break;
      // }
      // case V_SgAsmPEImportSection: {
      //   SgAsmPEImportSection* val = isSgAsmPEImportSection(node);
      //   break;
      // }
      // case V_SgAsmPEExportSection: {
      //   SgAsmPEExportSection* val = isSgAsmPEExportSection(node);
      //   break;
      // }
      // case V_SgAsmPEExportDirectory: {
      //   SgAsmPEExportDirectory* val = isSgAsmPEExportDirectory(node);
      //   break;
      // }
      // case V_SgAsmPEExportEntryList: {
      //   SgAsmPEExportEntryList* val = isSgAsmPEExportEntryList(node);
      //   break;
      // }
      // case V_SgAsmPEExportEntry: {
      //   SgAsmPEExportEntry* val = isSgAsmPEExportEntry(node);
      //   break;
      // }
      // case V_SgAsmPEStringSection: {
      //   SgAsmPEStringSection* val = isSgAsmPEStringSection(node);
      //   break;
      // }
      // case V_SgAsmPESectionTable: {
      //   SgAsmPESectionTable* val = isSgAsmPESectionTable(node);
      //   break;
      // }
      // case V_SgAsmPESectionTableEntry: {
      //   SgAsmPESectionTableEntry* val = isSgAsmPESectionTableEntry(node);
      //   break;
      // }
      // case V_SgAsmPESection: {
      //   SgAsmPESection* val = isSgAsmPESection(node);
      //   break;
      // }
      // case V_SgAsmCoffSymbolTable: {
      //   SgAsmCoffSymbolTable* val = isSgAsmCoffSymbolTable(node);
      //   break;
      // }
      // case V_SgAsmCoffSymbolList: {
      //   SgAsmCoffSymbolList* val = isSgAsmCoffSymbolList(node);
      //   break;
      // }
      // case V_SgAsmCoffSymbol: {
      //   SgAsmCoffSymbol* val = isSgAsmCoffSymbol(node);
      //   break;
      // }
      // case V_SgAsmCoffStrtab: {
      //   SgAsmCoffStrtab* val = isSgAsmCoffStrtab(node);
      //   break;
      // }
      // case V_SgAsmNEFileHeader: {
      //   SgAsmNEFileHeader* val = isSgAsmNEFileHeader(node);
      //   break;
      // }
      // case V_SgAsmNEEntryTable: {
      //   SgAsmNEEntryTable* val = isSgAsmNEEntryTable(node);
      //   break;
      // }
      // case V_SgAsmNEEntryPoint: {
      //   SgAsmNEEntryPoint* val = isSgAsmNEEntryPoint(node);
      //   break;
      // }
      // case V_SgAsmNESectionTable: {
      //   SgAsmNESectionTable* val = isSgAsmNESectionTable(node);
      //   break;
      // }
      // case V_SgAsmNESectionTableEntry: {
      //   SgAsmNESectionTableEntry* val = isSgAsmNESectionTableEntry(node);
      //   break;
      // }
      // case V_SgAsmNERelocTable: {
      //   SgAsmNERelocTable* val = isSgAsmNERelocTable(node);
      //   break;
      // }
      // case V_SgAsmNERelocEntry: {
      //   SgAsmNERelocEntry* val = isSgAsmNERelocEntry(node);
      //   break;
      // }
      // case V_SgAsmNENameTable: {
      //   SgAsmNENameTable* val = isSgAsmNENameTable(node);
      //   break;
      // }
      // case V_SgAsmNEModuleTable: {
      //   SgAsmNEModuleTable* val = isSgAsmNEModuleTable(node);
      //   break;
      // }
      // case V_SgAsmNEStringTable: {
      //   SgAsmNEStringTable* val = isSgAsmNEStringTable(node);
      //   break;
      // }
      // case V_SgAsmNESection: {
      //   SgAsmNESection* val = isSgAsmNESection(node);
      //   break;
      // }
      // case V_SgAsmLEFileHeader: {
      //   SgAsmLEFileHeader* val = isSgAsmLEFileHeader(node);
      //   break;
      // }
      // case V_SgAsmLEEntryTable: {
      //   SgAsmLEEntryTable* val = isSgAsmLEEntryTable(node);
      //   break;
      // }
      // case V_SgAsmLEEntryPoint: {
      //   SgAsmLEEntryPoint* val = isSgAsmLEEntryPoint(node);
      //   break;
      // }
      // case V_SgAsmLESectionTable: {
      //   SgAsmLESectionTable* val = isSgAsmLESectionTable(node);
      //   break;
      // }
      // case V_SgAsmLESectionTableEntry: {
      //   SgAsmLESectionTableEntry* val = isSgAsmLESectionTableEntry(node);
      //   break;
      // }
      // case V_SgAsmLERelocTable: {
      //   SgAsmLERelocTable* val = isSgAsmLERelocTable(node);
      //   break;
      // }
      // case V_SgAsmLENameTable: {
      //   SgAsmLENameTable* val = isSgAsmLENameTable(node);
      //   break;
      // }
      // case V_SgAsmLEPageTable: {
      //   SgAsmLEPageTable* val = isSgAsmLEPageTable(node);
      //   break;
      // }
      // case V_SgAsmLEPageTableEntry: {
      //   SgAsmLEPageTableEntry* val = isSgAsmLEPageTableEntry(node);
      //   break;
      // }
      // case V_SgAsmLESection: {
      //   SgAsmLESection* val = isSgAsmLESection(node);
      //   break;
      // }
      // case V_SgAsmDwarfArrayType: {
      //   SgAsmDwarfArrayType* val = isSgAsmDwarfArrayType(node);
      //   break;
      // }
      // case V_SgAsmDwarfClassType: {
      //   SgAsmDwarfClassType* val = isSgAsmDwarfClassType(node);
      //   break;
      // }
      // case V_SgAsmDwarfEntryPoint: {
      //   SgAsmDwarfEntryPoint* val = isSgAsmDwarfEntryPoint(node);
      //   break;
      // }
      // case V_SgAsmDwarfEnumerationType: {
      //   SgAsmDwarfEnumerationType* val = isSgAsmDwarfEnumerationType(node);
      //   break;
      // }
      // case V_SgAsmDwarfFormalParameter: {
      //   SgAsmDwarfFormalParameter* val = isSgAsmDwarfFormalParameter(node);
      //   break;
      // }
      // case V_SgAsmDwarfImportedDeclaration: {
      //   SgAsmDwarfImportedDeclaration* val = isSgAsmDwarfImportedDeclaration(node);
      //   break;
      // }
      // case V_SgAsmDwarfLabel: {
      //   SgAsmDwarfLabel* val = isSgAsmDwarfLabel(node);
      //   break;
      // }
      // case V_SgAsmDwarfLexicalBlock: {
      //   SgAsmDwarfLexicalBlock* val = isSgAsmDwarfLexicalBlock(node);
      //   break;
      // }
      // case V_SgAsmDwarfMember: {
      //   SgAsmDwarfMember* val = isSgAsmDwarfMember(node);
      //   break;
      // }
      // case V_SgAsmDwarfPointerType: {
      //   SgAsmDwarfPointerType* val = isSgAsmDwarfPointerType(node);
      //   break;
      // }
      // case V_SgAsmDwarfReferenceType: {
      //   SgAsmDwarfReferenceType* val = isSgAsmDwarfReferenceType(node);
      //   break;
      // }
      // case V_SgAsmDwarfCompilationUnit: {
      //   SgAsmDwarfCompilationUnit* val = isSgAsmDwarfCompilationUnit(node);
      //   break;
      // }
      // case V_SgAsmDwarfStringType: {
      //   SgAsmDwarfStringType* val = isSgAsmDwarfStringType(node);
      //   break;
      // }
      // case V_SgAsmDwarfStructureType: {
      //   SgAsmDwarfStructureType* val = isSgAsmDwarfStructureType(node);
      //   break;
      // }
      // case V_SgAsmDwarfSubroutineType: {
      //   SgAsmDwarfSubroutineType* val = isSgAsmDwarfSubroutineType(node);
      //   break;
      // }
      // case V_SgAsmDwarfTypedef: {
      //   SgAsmDwarfTypedef* val = isSgAsmDwarfTypedef(node);
      //   break;
      // }
      // case V_SgAsmDwarfUnionType: {
      //   SgAsmDwarfUnionType* val = isSgAsmDwarfUnionType(node);
      //   break;
      // }
      // case V_SgAsmDwarfUnspecifiedParameters: {
      //   SgAsmDwarfUnspecifiedParameters* val = isSgAsmDwarfUnspecifiedParameters(node);
      //   break;
      // }
      // case V_SgAsmDwarfVariant: {
      //   SgAsmDwarfVariant* val = isSgAsmDwarfVariant(node);
      //   break;
      // }
      // case V_SgAsmDwarfCommonBlock: {
      //   SgAsmDwarfCommonBlock* val = isSgAsmDwarfCommonBlock(node);
      //   break;
      // }
      // case V_SgAsmDwarfCommonInclusion: {
      //   SgAsmDwarfCommonInclusion* val = isSgAsmDwarfCommonInclusion(node);
      //   break;
      // }
      // case V_SgAsmDwarfInheritance: {
      //   SgAsmDwarfInheritance* val = isSgAsmDwarfInheritance(node);
      //   break;
      // }
      // case V_SgAsmDwarfInlinedSubroutine: {
      //   SgAsmDwarfInlinedSubroutine* val = isSgAsmDwarfInlinedSubroutine(node);
      //   break;
      // }
      // case V_SgAsmDwarfModule: {
      //   SgAsmDwarfModule* val = isSgAsmDwarfModule(node);
      //   break;
      // }
      // case V_SgAsmDwarfPtrToMemberType: {
      //   SgAsmDwarfPtrToMemberType* val = isSgAsmDwarfPtrToMemberType(node);
      //   break;
      // }
      // case V_SgAsmDwarfSetType: {
      //   SgAsmDwarfSetType* val = isSgAsmDwarfSetType(node);
      //   break;
      // }
      // case V_SgAsmDwarfSubrangeType: {
      //   SgAsmDwarfSubrangeType* val = isSgAsmDwarfSubrangeType(node);
      //   break;
      // }
      // case V_SgAsmDwarfWithStmt: {
      //   SgAsmDwarfWithStmt* val = isSgAsmDwarfWithStmt(node);
      //   break;
      // }
      // case V_SgAsmDwarfAccessDeclaration: {
      //   SgAsmDwarfAccessDeclaration* val = isSgAsmDwarfAccessDeclaration(node);
      //   break;
      // }
      // case V_SgAsmDwarfBaseType: {
      //   SgAsmDwarfBaseType* val = isSgAsmDwarfBaseType(node);
      //   break;
      // }
      // case V_SgAsmDwarfCatchBlock: {
      //   SgAsmDwarfCatchBlock* val = isSgAsmDwarfCatchBlock(node);
      //   break;
      // }
      // case V_SgAsmDwarfConstType: {
      //   SgAsmDwarfConstType* val = isSgAsmDwarfConstType(node);
      //   break;
      // }
      // case V_SgAsmDwarfConstant: {
      //   SgAsmDwarfConstant* val = isSgAsmDwarfConstant(node);
      //   break;
      // }
      // case V_SgAsmDwarfEnumerator: {
      //   SgAsmDwarfEnumerator* val = isSgAsmDwarfEnumerator(node);
      //   break;
      // }
      // case V_SgAsmDwarfFileType: {
      //   SgAsmDwarfFileType* val = isSgAsmDwarfFileType(node);
      //   break;
      // }
      // case V_SgAsmDwarfFriend: {
      //   SgAsmDwarfFriend* val = isSgAsmDwarfFriend(node);
      //   break;
      // }
      // case V_SgAsmDwarfNamelist: {
      //   SgAsmDwarfNamelist* val = isSgAsmDwarfNamelist(node);
      //   break;
      // }
      // case V_SgAsmDwarfNamelistItem: {
      //   SgAsmDwarfNamelistItem* val = isSgAsmDwarfNamelistItem(node);
      //   break;
      // }
      // case V_SgAsmDwarfPackedType: {
      //   SgAsmDwarfPackedType* val = isSgAsmDwarfPackedType(node);
      //   break;
      // }
      // case V_SgAsmDwarfSubprogram: {
      //   SgAsmDwarfSubprogram* val = isSgAsmDwarfSubprogram(node);
      //   break;
      // }
      // case V_SgAsmDwarfTemplateTypeParameter: {
      //   SgAsmDwarfTemplateTypeParameter* val = isSgAsmDwarfTemplateTypeParameter(node);
      //   break;
      // }
      // case V_SgAsmDwarfTemplateValueParameter: {
      //   SgAsmDwarfTemplateValueParameter* val = isSgAsmDwarfTemplateValueParameter(node);
      //   break;
      // }
      // case V_SgAsmDwarfThrownType: {
      //   SgAsmDwarfThrownType* val = isSgAsmDwarfThrownType(node);
      //   break;
      // }
      // case V_SgAsmDwarfTryBlock: {
      //   SgAsmDwarfTryBlock* val = isSgAsmDwarfTryBlock(node);
      //   break;
      // }
      // case V_SgAsmDwarfVariantPart: {
      //   SgAsmDwarfVariantPart* val = isSgAsmDwarfVariantPart(node);
      //   break;
      // }
      // case V_SgAsmDwarfVariable: {
      //   SgAsmDwarfVariable* val = isSgAsmDwarfVariable(node);
      //   break;
      // }
      // case V_SgAsmDwarfVolatileType: {
      //   SgAsmDwarfVolatileType* val = isSgAsmDwarfVolatileType(node);
      //   break;
      // }
      // case V_SgAsmDwarfDwarfProcedure: {
      //   SgAsmDwarfDwarfProcedure* val = isSgAsmDwarfDwarfProcedure(node);
      //   break;
      // }
      // case V_SgAsmDwarfRestrictType: {
      //   SgAsmDwarfRestrictType* val = isSgAsmDwarfRestrictType(node);
      //   break;
      // }
      // case V_SgAsmDwarfInterfaceType: {
      //   SgAsmDwarfInterfaceType* val = isSgAsmDwarfInterfaceType(node);
      //   break;
      // }
      // case V_SgAsmDwarfNamespace: {
      //   SgAsmDwarfNamespace* val = isSgAsmDwarfNamespace(node);
      //   break;
      // }
      // case V_SgAsmDwarfImportedModule: {
      //   SgAsmDwarfImportedModule* val = isSgAsmDwarfImportedModule(node);
      //   break;
      // }
      // case V_SgAsmDwarfUnspecifiedType: {
      //   SgAsmDwarfUnspecifiedType* val = isSgAsmDwarfUnspecifiedType(node);
      //   break;
      // }
      // case V_SgAsmDwarfPartialUnit: {
      //   SgAsmDwarfPartialUnit* val = isSgAsmDwarfPartialUnit(node);
      //   break;
      // }
      // case V_SgAsmDwarfImportedUnit: {
      //   SgAsmDwarfImportedUnit* val = isSgAsmDwarfImportedUnit(node);
      //   break;
      // }
      // case V_SgAsmDwarfMutableType: {
      //   SgAsmDwarfMutableType* val = isSgAsmDwarfMutableType(node);
      //   break;
      // }
      // case V_SgAsmDwarfCondition: {
      //   SgAsmDwarfCondition* val = isSgAsmDwarfCondition(node);
      //   break;
      // }
      // case V_SgAsmDwarfSharedType: {
      //   SgAsmDwarfSharedType* val = isSgAsmDwarfSharedType(node);
      //   break;
      // }
      // case V_SgAsmDwarfFormatLabel: {
      //   SgAsmDwarfFormatLabel* val = isSgAsmDwarfFormatLabel(node);
      //   break;
      // }
      // case V_SgAsmDwarfFunctionTemplate: {
      //   SgAsmDwarfFunctionTemplate* val = isSgAsmDwarfFunctionTemplate(node);
      //   break;
      // }
      // case V_SgAsmDwarfClassTemplate: {
      //   SgAsmDwarfClassTemplate* val = isSgAsmDwarfClassTemplate(node);
      //   break;
      // }
      // case V_SgAsmDwarfUpcSharedType: {
      //   SgAsmDwarfUpcSharedType* val = isSgAsmDwarfUpcSharedType(node);
      //   break;
      // }
      // case V_SgAsmDwarfUpcStrictType: {
      //   SgAsmDwarfUpcStrictType* val = isSgAsmDwarfUpcStrictType(node);
      //   break;
      // }
      // case V_SgAsmDwarfUpcRelaxedType: {
      //   SgAsmDwarfUpcRelaxedType* val = isSgAsmDwarfUpcRelaxedType(node);
      //   break;
      // }
      // case V_SgAsmDwarfUnknownConstruct: {
      //   SgAsmDwarfUnknownConstruct* val = isSgAsmDwarfUnknownConstruct(node);
      //   break;
      // }
      // case V_SgAsmDwarfConstruct: {
      //   SgAsmDwarfConstruct* val = isSgAsmDwarfConstruct(node);
      //   break;
      // }
      // case V_SgAsmDwarfMacroList: {
      //   SgAsmDwarfMacroList* val = isSgAsmDwarfMacroList(node);
      //   break;
      // }
      // case V_SgAsmDwarfMacro: {
      //   SgAsmDwarfMacro* val = isSgAsmDwarfMacro(node);
      //   break;
      // }
      // case V_SgAsmDwarfLine: {
      //   SgAsmDwarfLine* val = isSgAsmDwarfLine(node);
      //   break;
      // }
      // case V_SgAsmDwarfLineList: {
      //   SgAsmDwarfLineList* val = isSgAsmDwarfLineList(node);
      //   break;
      // }
      // case V_SgAsmDwarfConstructList: {
      //   SgAsmDwarfConstructList* val = isSgAsmDwarfConstructList(node);
      //   break;
      // }
      // case V_SgAsmDwarfCompilationUnitList: {
      //   SgAsmDwarfCompilationUnitList* val = isSgAsmDwarfCompilationUnitList(node);
      //   break;
      // }
      // case V_SgAsmDwarfInformation: {
      //   SgAsmDwarfInformation* val = isSgAsmDwarfInformation(node);
      //   break;
      // }
      // case V_SgAsmStringStorage: {
      //   SgAsmStringStorage* val = isSgAsmStringStorage(node);
      //   break;
      // }
      // case V_SgAsmBasicString: {
      //   SgAsmBasicString* val = isSgAsmBasicString(node);
      //   break;
      // }
      // case V_SgAsmStoredString: {
      //   SgAsmStoredString* val = isSgAsmStoredString(node);
      //   break;
      // }
      // case V_SgAsmGenericString: {
      //   SgAsmGenericString* val = isSgAsmGenericString(node);
      //   break;
      // }
      // case V_SgAsmGenericHeaderList: {
      //   SgAsmGenericHeaderList* val = isSgAsmGenericHeaderList(node);
      //   break;
      // }
      // case V_SgAsmGenericHeader: {
      //   SgAsmGenericHeader* val = isSgAsmGenericHeader(node);
      //   break;
      // }
      // case V_SgAsmGenericSymbolList: {
      //   SgAsmGenericSymbolList* val = isSgAsmGenericSymbolList(node);
      //   break;
      // }
      // case V_SgAsmGenericSymbol: {
      //   SgAsmGenericSymbol* val = isSgAsmGenericSymbol(node);
      //   break;
      // }
      // case V_SgAsmGenericStrtab: {
      //   SgAsmGenericStrtab* val = isSgAsmGenericStrtab(node);
      //   break;
      // }
      // case V_SgAsmGenericSectionList: {
      //   SgAsmGenericSectionList* val = isSgAsmGenericSectionList(node);
      //   break;
      // }
      // case V_SgAsmGenericSection: {
      //   SgAsmGenericSection* val = isSgAsmGenericSection(node);
      //   break;
      // }
      // case V_SgAsmGenericDLLList: {
      //   SgAsmGenericDLLList* val = isSgAsmGenericDLLList(node);
      //   break;
      // }
      // case V_SgAsmGenericDLL: {
      //   SgAsmGenericDLL* val = isSgAsmGenericDLL(node);
      //   break;
      // }
      // case V_SgAsmGenericFormat: {
      //   SgAsmGenericFormat* val = isSgAsmGenericFormat(node);
      //   break;
      // }
      // case V_SgAsmGenericFileList: {
      //   SgAsmGenericFileList* val = isSgAsmGenericFileList(node);
      //   break;
      // }
      // case V_SgAsmGenericFile: {
      //   SgAsmGenericFile* val = isSgAsmGenericFile(node);
      //   break;
      // }
      // case V_SgAsmExecutableFileFormat: {
      //   SgAsmExecutableFileFormat* val = isSgAsmExecutableFileFormat(node);
      //   break;
      // }
      // case V_SgAsmNode: {
      //   SgAsmNode* val = isSgAsmNode(node);
      //   break;
      // }
      // case V_SgCommonBlockObject: {
      //   SgCommonBlockObject* val = isSgCommonBlockObject(node);
      //   break;
      // }
      // case V_SgLambdaCapture: {
      //   SgLambdaCapture* val = isSgLambdaCapture(node);
      //   break;
      // }
      // case V_SgLambdaCaptureList: {
      //   SgLambdaCaptureList* val = isSgLambdaCaptureList(node);
      //   break;
      // }
      // case V_SgJavaMemberValuePair: {
      //   SgJavaMemberValuePair* val = isSgJavaMemberValuePair(node);
      //   break;
      // }
      // case V_SgOmpOrderedClause: {
      //   SgOmpOrderedClause* val = isSgOmpOrderedClause(node);
      //   break;
      // }
      // case V_SgOmpNowaitClause: {
      //   SgOmpNowaitClause* val = isSgOmpNowaitClause(node);
      //   break;
      // }
      // case V_SgOmpUntiedClause: {
      //   SgOmpUntiedClause* val = isSgOmpUntiedClause(node);
      //   break;
      // }
      // case V_SgOmpDefaultClause: {
      //   SgOmpDefaultClause* val = isSgOmpDefaultClause(node);
      //   break;
      // }
      // case V_SgOmpCollapseClause: {
      //   SgOmpCollapseClause* val = isSgOmpCollapseClause(node);
      //   break;
      // }
      // case V_SgOmpIfClause: {
      //   SgOmpIfClause* val = isSgOmpIfClause(node);
      //   break;
      // }
      // case V_SgOmpNumThreadsClause: {
      //   SgOmpNumThreadsClause* val = isSgOmpNumThreadsClause(node);
      //   break;
      // }
      // case V_SgOmpDeviceClause: {
      //   SgOmpDeviceClause* val = isSgOmpDeviceClause(node);
      //   break;
      // }
      // case V_SgOmpSafelenClause: {
      //   SgOmpSafelenClause* val = isSgOmpSafelenClause(node);
      //   break;
      // }
      // case V_SgOmpExpressionClause: {
      //   SgOmpExpressionClause* val = isSgOmpExpressionClause(node);
      //   break;
      // }
      // case V_SgOmpCopyprivateClause: {
      //   SgOmpCopyprivateClause* val = isSgOmpCopyprivateClause(node);
      //   break;
      // }
      // case V_SgOmpPrivateClause: {
      //   SgOmpPrivateClause* val = isSgOmpPrivateClause(node);
      //   break;
      // }
      // case V_SgOmpFirstprivateClause: {
      //   SgOmpFirstprivateClause* val = isSgOmpFirstprivateClause(node);
      //   break;
      // }
      // case V_SgOmpSharedClause: {
      //   SgOmpSharedClause* val = isSgOmpSharedClause(node);
      //   break;
      // }
      // case V_SgOmpCopyinClause: {
      //   SgOmpCopyinClause* val = isSgOmpCopyinClause(node);
      //   break;
      // }
      // case V_SgOmpLastprivateClause: {
      //   SgOmpLastprivateClause* val = isSgOmpLastprivateClause(node);
      //   break;
      // }
      // case V_SgOmpReductionClause: {
      //   SgOmpReductionClause* val = isSgOmpReductionClause(node);
      //   break;
      // }
      // case V_SgOmpMapClause: {
      //   SgOmpMapClause* val = isSgOmpMapClause(node);
      //   break;
      // }
      // case V_SgOmpLinearClause: {
      //   SgOmpLinearClause* val = isSgOmpLinearClause(node);
      //   break;
      // }
      // case V_SgOmpUniformClause: {
      //   SgOmpUniformClause* val = isSgOmpUniformClause(node);
      //   break;
      // }
      // case V_SgOmpAlignedClause: {
      //   SgOmpAlignedClause* val = isSgOmpAlignedClause(node);
      //   break;
      // }
      // case V_SgOmpVariablesClause: {
      //   SgOmpVariablesClause* val = isSgOmpVariablesClause(node);
      //   break;
      // }
      // case V_SgOmpScheduleClause: {
      //   SgOmpScheduleClause* val = isSgOmpScheduleClause(node);
      //   break;
      // }
      // case V_SgOmpClause: {
      //   SgOmpClause* val = isSgOmpClause(node);
      //   break;
      // }
      // case V_SgRenamePair: {
      //   SgRenamePair* val = isSgRenamePair(node);
      //   break;
      // }
      // case V_SgInterfaceBody: {
      //   SgInterfaceBody* val = isSgInterfaceBody(node);
      //   break;
      // }
      // case V_SgHeaderFileBody: {
      //   SgHeaderFileBody* val = isSgHeaderFileBody(node);
      //   break;
      // }
      // case V_SgUntypedUnaryOperator: {
      //   SgUntypedUnaryOperator* val = isSgUntypedUnaryOperator(node);
      //   break;
      // }
      // case V_SgUntypedBinaryOperator: {
      //   SgUntypedBinaryOperator* val = isSgUntypedBinaryOperator(node);
      //   break;
      // }
      // case V_SgUntypedValueExpression: {
      //   SgUntypedValueExpression* val = isSgUntypedValueExpression(node);
      //   break;
      // }
      // case V_SgUntypedArrayReferenceExpression: {
      //   SgUntypedArrayReferenceExpression* val = isSgUntypedArrayReferenceExpression(node);
      //   break;
      // }
      // case V_SgUntypedOtherExpression: {
      //   SgUntypedOtherExpression* val = isSgUntypedOtherExpression(node);
      //   break;
      // }
      // case V_SgUntypedFunctionCallOrArrayReferenceExpression: {
      //   SgUntypedFunctionCallOrArrayReferenceExpression* val = isSgUntypedFunctionCallOrArrayReferenceExpression(node);
      //   break;
      // }
      // case V_SgUntypedReferenceExpression: {
      //   SgUntypedReferenceExpression* val = isSgUntypedReferenceExpression(node);
      //   break;
      // }
      // case V_SgUntypedExpression: {
      //   SgUntypedExpression* val = isSgUntypedExpression(node);
      //   break;
      // }
      // case V_SgUntypedImplicitDeclaration: {
      //   SgUntypedImplicitDeclaration* val = isSgUntypedImplicitDeclaration(node);
      //   break;
      // }
      // case V_SgUntypedVariableDeclaration: {
      //   SgUntypedVariableDeclaration* val = isSgUntypedVariableDeclaration(node);
      //   break;
      // }
      // case V_SgUntypedProgramHeaderDeclaration: {
      //   SgUntypedProgramHeaderDeclaration* val = isSgUntypedProgramHeaderDeclaration(node);
      //   break;
      // }
      // case V_SgUntypedSubroutineDeclaration: {
      //   SgUntypedSubroutineDeclaration* val = isSgUntypedSubroutineDeclaration(node);
      //   break;
      // }
      // case V_SgUntypedFunctionDeclaration: {
      //   SgUntypedFunctionDeclaration* val = isSgUntypedFunctionDeclaration(node);
      //   break;
      // }
      // case V_SgUntypedModuleDeclaration: {
      //   SgUntypedModuleDeclaration* val = isSgUntypedModuleDeclaration(node);
      //   break;
      // }
      // case V_SgUntypedDeclarationStatement: {
      //   SgUntypedDeclarationStatement* val = isSgUntypedDeclarationStatement(node);
      //   break;
      // }
      // case V_SgUntypedAssignmentStatement: {
      //   SgUntypedAssignmentStatement* val = isSgUntypedAssignmentStatement(node);
      //   break;
      // }
      // case V_SgUntypedFunctionCallStatement: {
      //   SgUntypedFunctionCallStatement* val = isSgUntypedFunctionCallStatement(node);
      //   break;
      // }
      // case V_SgUntypedBlockStatement: {
      //   SgUntypedBlockStatement* val = isSgUntypedBlockStatement(node);
      //   break;
      // }
      // case V_SgUntypedNamedStatement: {
      //   SgUntypedNamedStatement* val = isSgUntypedNamedStatement(node);
      //   break;
      // }
      // case V_SgUntypedOtherStatement: {
      //   SgUntypedOtherStatement* val = isSgUntypedOtherStatement(node);
      //   break;
      // }
      // case V_SgUntypedFunctionScope: {
      //   SgUntypedFunctionScope* val = isSgUntypedFunctionScope(node);
      //   break;
      // }
      // case V_SgUntypedModuleScope: {
      //   SgUntypedModuleScope* val = isSgUntypedModuleScope(node);
      //   break;
      // }
      // case V_SgUntypedGlobalScope: {
      //   SgUntypedGlobalScope* val = isSgUntypedGlobalScope(node);
      //   break;
      // }
      // case V_SgUntypedScope: {
      //   SgUntypedScope* val = isSgUntypedScope(node);
      //   break;
      // }
      // case V_SgUntypedStatement: {
      //   SgUntypedStatement* val = isSgUntypedStatement(node);
      //   break;
      // }
      // case V_SgUntypedArrayType: {
      //   SgUntypedArrayType* val = isSgUntypedArrayType(node);
      //   break;
      // }
      // case V_SgUntypedType: {
      //   SgUntypedType* val = isSgUntypedType(node);
      //   break;
      // }
      // case V_SgUntypedAttribute: {
      //   SgUntypedAttribute* val = isSgUntypedAttribute(node);
      //   break;
      // }
      // case V_SgUntypedInitializedName: {
      //   SgUntypedInitializedName* val = isSgUntypedInitializedName(node);
      //   break;
      // }
      // case V_SgUntypedFile: {
      //   SgUntypedFile* val = isSgUntypedFile(node);
      //   break;
      // }
      // case V_SgUntypedStatementList: {
      //   SgUntypedStatementList* val = isSgUntypedStatementList(node);
      //   break;
      // }
      // case V_SgUntypedDeclarationList: {
      //   SgUntypedDeclarationList* val = isSgUntypedDeclarationList(node);
      //   break;
      // }
      // case V_SgUntypedFunctionDeclarationList: {
      //   SgUntypedFunctionDeclarationList* val = isSgUntypedFunctionDeclarationList(node);
      //   break;
      // }
      // case V_SgUntypedInitializedNameList: {
      //   SgUntypedInitializedNameList* val = isSgUntypedInitializedNameList(node);
      //   break;
      // }
      // case V_SgUntypedNode: {
      //   SgUntypedNode* val = isSgUntypedNode(node);
      //   break;
      // }
      // case V_SgLocatedNodeSupport: {
      //   SgLocatedNodeSupport* val = isSgLocatedNodeSupport(node);
      //   break;
      // }
      // case V_SgToken: {
      //   SgToken* val = isSgToken(node);
      //   break;
      // }
      // case V_SgLocatedNode: {
      //   SgLocatedNode* val = isSgLocatedNode(node);
      //   break;
      // }
      // case V_SgAterm: {
      //   SgAterm* val = isSgAterm(node);
      //   break;
      // }
      // case V_SgNode: {
      //   SgNode* val = isSgNode(node);
      //   break;
      // }
      // case V_SgNumVariants: {
      // //   SgNumVariants* val = isSgNumVariants(node);
      // //   break;
      // }
    }




}
