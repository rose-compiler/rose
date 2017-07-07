/*
 * PastToSageGeneric.hpp: This file is part of the PolyOpt project.
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
 * @file: PastToSageGeneric.hpp
 * @author: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 */

#ifndef PLUTO_ROSE_PAST_TO_SAGE_GENERIC_HPP
# define PLUTO_ROSE_PAST_TO_SAGE_GENERIC_HPP


// LNP: To avoid redefined macro errors in pocc.
/// LNP: FIXME: Find a better solution!
# undef PACKAGE_BUGREPORT
# undef PACKAGE_STRING
# undef PACKAGE_TARNAME
# undef PACKAGE_NAME
# undef PACKAGE_VERSION
# include <rose.h>
# ifndef SCOPLIB_INT_T_IS_LONGLONG
#  define SCOPLIB_INT_T_IS_LONGLONG
# endif
# include <scoplib/scop.h>
# ifndef CLOOG_INT_GMP
#  define CLOOG_INT_GMP
# endif
# include <cloog/cloog.h>
# include <pragmatize/pragmatize.h>
# include <past/past.h>

# include <polyopt/PolyRoseOptions.hpp>



/**
 * \brief Converter from Past to Sage.
 *
 * Converts a Past representation of generated code to an equivalent Sage
 * representation.
 */
class PastToSageGeneric
{
public:
  PastToSageGeneric() { }
  PastToSageGeneric(SgNode* base) : _base(base) { _scope = isSgScopeStatement(_base->get_parent());}
  SgNode* convert(s_past_node_t* node);
  SgNode* converter(s_past_node_t* node);

private:
  SgType* build_sage_type(s_past_node_t* type, s_past_node_t* arr_desc);
  SgNode* past_binary_to_funcall (s_past_node_t* binop,
				  SgExpression* lhs,
				  SgExpression* rhs);
  SgNode* past_ternary_to_funcall (s_past_node_t* binop,
				   SgExpression* arg1,
				   SgExpression* arg2,
				   SgExpression* arg3);

  /**
   * Pretty-print table for unary and binary operators.
   *
   */
  struct pprint_op {
    cs_past_node_type_t* type;
    const char* scalar_op;
    const char* sse_op_single;
    const char* sse_op_double;
    const char* avx_op_single;
    const char* avx_op_double;
    const char* avx2_op_single;
    const char* avx2_op_double;
  };
  // Use a global variable for the table. It is declared in past_pprint().
  struct pprint_op* simd_function_table;



  PolyRoseOptions _polyoptions;
  SgNode* _base;
  SgScopeStatement* _scope;
  SgScopeStatement* _top_scope;
  std::map<char*,SgInitializedName*> _declarations;
  std::vector<SgVariableDeclaration*> _declTemp;
};

#endif

