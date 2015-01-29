/*
 * AffineConverter.hpp: This file is part of the PolyOpt project.
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
 * @file: AffineConverter.hpp
 * @author: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 */

#ifndef PLUTO_ROSE_AFFINECONVERTER_HPP
# define PLUTO_ROSE_AFFINECONVERTER_HPP

# include <polyopt/PolyRoseOptions.hpp>
# ifndef SCOPLIB_INT_T_IS_LONGLONG
#  define SCOPLIB_INT_T_IS_LONGLONG
# endif
# include <scoplib/scop.h>


class AffineConverter
{
public:

  AffineConverter(PolyRoseOptions& options);

  ~AffineConverter();


  /****************************************************************************/
  /********************** High level extraction routines **********************/
  /****************************************************************************/


  /**
   *
   * Convert a set of Scops identified by their root node into a set of
   * scoplib_scop_p. The Sage root node is stored in scoplib_scop_p->usr.
   *
   * controlMap maps SgExprStatement (ie, polyhedral statement) to the
   * set of control nodes (ie, SgIf and SgFor) surrounding them.
   *
   */
  std::vector<scoplib_scop_p>
  createScopLibFromScops(std::vector<SgNode*>& scopRoots,
			 std::map<SgNode*,
			 std::vector<SgNode*> >& controlMap);

  /**
   * Create the matrix representation of all control nodes (eg, SgFor,
   * SgIf).  Associate a matrix to each node.
   *
   */
  bool
  createControlMatrices(std::vector<SgNode*>& controlStack,
			std::map<SgVariableSymbol*, int>&
			iteratorsMap,
			std::map<SgVariableSymbol*, int>&
			parametersMap,
			std::map<SgNode*, scoplib_matrix_p>&
			matrixMap);

  /**
   * Create the iterator symbol array for the scoplib_scop_p.
   *
   */
  char**
  createIteratorsArray(std::map<SgVariableSymbol*, int>&
		       iteratorsMap);

  /**
   * Create the parameter symbol array for the scoplib_scop_p.
   *
   */
  char**
  createParametersArray(std::map<SgVariableSymbol*, int>&
			parametersMap);

  /**
   * Create the variable symbol array for the scoplib_scop_p.
   *
   */
  char**
  createVariablesArray(std::map<SgVariableSymbol*, int>&
		       variablesMap);

  /**
   * Create the context matrix for the scoplib_scop_p.
   *
   */
  scoplib_matrix_p
  createContext(std::map<SgVariableSymbol*, int>& parametersMap);

  /**
   * Create the iteration domain matrix for a scoplib_statement_p.
   *
   */
  scoplib_matrix_list_p
  createDomain(std::vector<SgNode*>& ctrl,
	       std::map<SgNode*, scoplib_matrix_p>& matrixMap,
	       int nbIters,
	       int nbParams);

  /**
   * Create the scheduling matrix for a scoplib_statement_p.
   *
   */
  scoplib_matrix_p
  createSchedule(SgNode* node,
		 std::vector<SgNode*>& control,
		 int nbIters,
		 int nbParams);

  /**
   * Create the access function matrix for a scoplib_statement_p (r or w).
   *
   * Return the access function for read or write references, given a
   * polyhedral statement. Automatically update the variables map.
   * Ugly but does the job.
   *
   *
   */
  scoplib_matrix_p
  createAccessFunction(SgStatement* node,
		       std::map<SgVariableSymbol*, int>&
		       iteratorsMap,
		       std::map<SgVariableSymbol*, int>&
		       parametersMap,
		       std::map<SgVariableSymbol*, int>&
		       variablesMap,
		       bool isReadRef);


  /****************************************************************************/
  /************************ (Pre-)Processing functions ************************/
  /****************************************************************************/


  /**
   * Annotate the AST to reflect the schedule. Simple prefix visit.
   *
   */
  void
  buildScopAstOrdering(SgNode* root);

  /**
   * Convenience for the handling of fake iterators (used for statements
   * not surrounded by any loop: they are represented as surrounded by a
   * 'fake' loop iterating once.
   *
   *
   */
  std::map<SgVariableSymbol*, int>&
  getFakeControlIteratorMap();

  /**
   * Retrieve the control node associated with the fake iterator.
   *
   *
   */
  SgNode*
  getFakeControlNode();

  /**
   * Create a new matrix from a given matrix, and possibly extend the
   * columns for iterators and parameters if needed, given nbIters and
   * nbParams.
   *
   */
  scoplib_matrix_p
  buildAdjustedMatrixIterators(scoplib_matrix_p mat,
			       int nbIters, int nbParams);

  /**
   *
   * Create the parameter list for a node. All variables that are used
   * in affine expressions (array subscript / control) and that are not
   * loop iterators are parameters.
   *
   */
  void
  updateParametersMap(SgNode* s,
		      std::vector<SgNode*>& control,
		      std::map<SgVariableSymbol*, int>&
		      parametersMap);

  /**
   * Internal helper.
   *
   */
  void
  updateParams(SgNode* node,
	       std::map<SgVariableSymbol*, int>& iteratorsMap,
	       std::map<SgVariableSymbol*, int>& parametersMap);

  /**
   * Get the list of loop iterators in a given control stack.
   *
   *
   */
  std::map<SgVariableSymbol*, int>
  getLoopIteratorsMap(std::vector<SgNode*>& controlStack);





  /****************************************************************************/
  /*********************** Affine expression processing ***********************/
  /****************************************************************************/

  /**
   * Create a matrix corresponding to an arbitrary expression.
   *
   */
  scoplib_matrix_p
  createMatrix(SgExpression* expr,
	       std::map<SgVariableSymbol*, int>&
	       iteratorsMap,
	       std::map<SgVariableSymbol*, int>&
	       parametersMap);

  /**
   * Create a matrix corresponding to a linearized array subscript
   * function as generated by F2C (note: ad-hoc parsing).
   *
   */
  scoplib_matrix_p
  createMatrixFromF2CExpression(SgExpression* expr,
				std::map<SgVariableSymbol*, int>&
				iteratorsMap,
				std::map<SgVariableSymbol*, int>&
				parametersMap);

  /**
   * Generic expression parser.
   *
   * Parse conditionals and access functions.
   *
   *
   */
  scoplib_matrix_p
  parseExpression(SgExpression* expr,
		  std::map<SgVariableSymbol*, int>&
		  iteratorsMap,
		  std::map<SgVariableSymbol*, int>&
		  parametersMap);

  /**
   * Parse a demangled binary expression, eg a '<' b. To conform scoplib
   * representation, a < b is represented as b - a - 1 >= 0. So, a is
   * given a mult of -1, b a mult of 1, and the offset is -1.
   *
   */
  scoplib_matrix_p
  parseBinExpr(SgExpression* lhs,
	       int lhsmult,
	       SgExpression* rhs,
	       int rhsmult,
	       int offset,
	       std::map<SgVariableSymbol*, int>&
	       iteratorsMap,
	       std::map<SgVariableSymbol*, int>&
	       parametersMap);

  /**
   * Check if the expression is a ceil/floor expression.
   *
   */
  bool
  parseCeilFloorExpression(SgExpression* e,
			   SgExpression** lhs,
			   SgExpression** rhs);

  /**
   * Check if the expression is a min/max expression.
   *
   */
  bool
  parseMinMaxExpression(SgExpression* e,
			SgExpression** lhs, SgExpression** rhs);

  /**
   * Parse a linear affine expression, made of a lhs and a rhs (possibly
   * NULL). return the scoplib_matrix representation (1 row always).
   *
   */
  scoplib_matrix_p
  parseLinearExpression(SgExpression* llhs,
			int lhsmult,
			SgExpression* lrhs,
			int rhsmult,
			int offset,
			std::map<SgVariableSymbol*, int>&
			iteratorsMap,
			std::map<SgVariableSymbol*, int>&
			parametersMap);

  /**
   * Internal Helper.
   *
   */
  bool
  convertLinearExpression(SgExpression* e,
			  int mult,
			  std::map<SgVariableSymbol*, int>&
			  iteratorsMap,
			  std::map<SgVariableSymbol*, int>&
			  parametersMap,
			  scoplib_matrix_p matrix);

  /**
   * Internal Helper.
   *
   */
  scoplib_int_t
  computeCoefficient(SgNode* e,
		     SgVariableSymbol* symb);

  // Attributes:
  std::map<SgVariableSymbol*, int> _fakeControlMap;

  static const bool READ_REFERENCES = true;
  static const bool WRITE_REFERENCES = false;

private:
  PolyRoseOptions _polyoptions;
  std::set<std::pair<SgVariableSymbol*, std::vector<bool> > > _arraysAsScalars;
  std::map<std::string, SgVariableSymbol*> _fakeSymbolMap;
};

#endif

