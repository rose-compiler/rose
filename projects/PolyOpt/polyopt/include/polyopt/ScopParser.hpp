/*
 * ScopParser.hpp: This file is part of the PolyOpt project.
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
 * @file: ScopParser.hpp
 * @author: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 */

#ifndef PLUTO_ROSE_SCOP_EXTRACT_UTILS_INC
# define PLUTO_ROSE_SCOP_EXTRACT_UTILS_INC

# include <rose.h>
# include <vector>
# include <set>
# include <map>

/**
 * \brief Sage AST analyzer for extracting SCoPs.
 *
 * This class implements a Sage AST analyzer that identifies AST sub-trees that
 * constitute a SCoP.
 */
class ScopParser
{
public:

  ScopParser();

  ~ScopParser();


  /****************************************************************************/
  /************************* High-level parser routines ***********************/
  /****************************************************************************/

  /**
   * Check if a node respects the SCoP assumption.
   *
   *
   */
  bool isScopCompatible(SgNode* node,
			std::set<SgNode*>& scopNodes,
			std::set<SgNode*>& nonScopNodes);

  /**
   * Check if a basic block is a scop.
   * - all statements in the BB are scops
   * - all affine expressions in the scop use read-only variables
   *
   */
  bool isBasicBlockNodeScopCompatible(SgBasicBlock* node,
				      std::set<SgNode*>& scopNodes);

  /**
   * Ensure a symbol written in the scop-to-be is not used in any affine
   * expression of the node to test.
   *
   *
   */
  bool checkAffineExpressionsReadOnlySymbols(std::vector<SgNode*>& siblings,
					     SgNode* node);

  /**
   * Ensure a loop iterator value is not used outside the loop declaring
   * it.
   *
   *
   */
  bool checkIteratorUsedOutsideLoop(std::vector<SgNode*>& siblings,
				    SgNode* node);

  /**
   * Check if a loop iterator value is used before the current scop.
   *
   */
  bool checkIteratorUsedBeforeLoop(std::vector<SgNode*>& siblings,
				   SgNode* node);


  /**
   * Check if an 'if' node is a scop.
   * - conditional is an affine expression
   * - no 'else' clause
   * - 'then' clause is a scop
   *
   */
  bool isIfNodeScopCompatible(SgIfStmt* ifnode,
			      std::set<SgNode*>& scopNodes);

  /**
   * Check if a SgForNode respects the SCoP definition.
   * - loop has stride one (for the moment)
   * - loop body  is a scop
   * - loop iterator is not written in the body
   * - lb and ub of loop are affine expressions
   *
   */
  bool isForNodeScopCompatible(SgForStatement* fornode,
			       std::set<SgNode*>& scopNodes);



  /****************************************************************************/
  /************************** Parsing functions *******************************/
  /****************************************************************************/


  /**
   * Top-down parsing of an expression, according to the following
   * grammar (rule is activated if <arg> is true):
   *
   *            expr -> affineExpr
   * <min_ok>   expr -> min(expr, expr)
   * <max_ok>   expr -> max(expr, expr)
   * <conj_ok>  expr -> expr && expr
   * <cmp_ok>   expr -> expr CompareOp expr
   * <fc_ok>    expr -> {ceil,floor}{d,i}(expr,Integer)
   *
   */
  bool parseBoundExpression(SgExpression* expr,
			    std::set<SgVariableSymbol*>& symbols,
			    bool min_ok,
			    bool max_ok,
			    bool conj_ok,
			    bool cmp_ok,
			    bool fc_ok);

  /**
   * Return true if the expression is an affine form of the symbols
   * referenced in the 'symbols' set.
   *
   */
  bool isAffineExpr(SgExpression* expr,
		    std::set<SgVariableSymbol*>& symbols);

  /**
   * Top-down parsing of an affine expression, returns true if it is
   * affine.
   * - var_ok = true -> expression can contain variable references.
   *
   */
  bool parseAffineExpression(SgExpression* expr, bool var_ok);

  /**
   * returns true if expr -> lhs '&&' rhs
   *
   *
   */
  bool parseConjunction(SgExpression* expr,
			SgExpression** lhs,
			SgExpression** rhs);

  /**
   * returns true if stmt -> var '=' expr
   *
   *
   */
  bool parseAssignment(SgStatement* stmt, SgVariableSymbol** var,
		       SgExpression** expr);

  /**
   * returns true if:
   *       expr -> var++
   *       expr -> var += 1
   *
   *
   */
  bool parseSimpleIncrement(SgVariableSymbol** var,
			    SgExpression* expr);

  /**
   * Generic expression parser. Returns true if the array access
   * functions are affine.
   *
   *
   */
  bool parseExpression(SgExpression* node);

  /**
   * Collect all SgVarRefExp nodes in the subtree, such that a reference
   * is being used in an affine expression (array access, loop bound or
   * conditional).
   *
   *
   */
  bool collectAffineRefs(SgNode* node, std::vector<SgNode*>& refs);



  /****************************************************************************/
  /************************* Interface to SageTools ***************************/
  /****************************************************************************/


  /**
   * Verbose output. return false.
   *
   */
  bool invalidNode(SgNode* node, const char* message);

  /**
   * Verbose output. return true.
   *
   */
  bool validNode(SgNode* node, const char* message);

  /**
   * Helper. Convert std::vector<SgNode*> constaining references to
   * std::set<SgVariableSymbol*>
   *
   *
   */
  std::set<SgVariableSymbol*> convertToSymbolSet(std::vector<SgNode*>& refs);

  /**
   * Interface to SageTools::collectReadOnlySymbols.
   *
   * Perform additional checks to ensure the absence of pointer
   * arithmetic in the references.
   *
   * DOES NOT perform additional check on the existence of the symbol
   * in the symbol table.
   *
   */
  bool
  collectReadOnlySymbols(SgStatement* node,
			 std::set<SgVariableSymbol*>& symbols);

  /**
   * Interface to SageTools::collectReadWriteRefs, using
   * SageTools::_fakeSymbolMap.
   *
   */
  bool
  collectReadWriteRefs(SgStatement* node,
		       std::vector<SgNode*>& readRefs,
		       std::vector<SgNode*>& writeRefs);

  /**
   * Compute a fake symbol for a field access (eg, for a.b compute a
   * fake a_b symbol).
   *
   */
  SgVariableSymbol*
  getFakeSymbol(SgDotExp* node);

  /**
   * Retrieve the symbol associated to the node, if any. Return a fake
   * symbol for the case of C structure/field references.
   *
   */
  SgVariableSymbol*
  getSymbolFromReference(SgNode* node);



  /****************************************************************************/
  /************************** Array delinearization ***************************/
  /****************************************************************************/

  /**
   * Inspect all references to an array that has been marked as having a
   * non-affine access function. Check if the references represent a
   * linearization of a multidimensional access function.
   *
   */
  void
  checkLinearizedArrayAccesses(SgNode* root);



  /****************************************************************************/
  /************************** Control stack handling **************************/
  /****************************************************************************/

  /**
   * Empty the parser state.
   *
   */
  void emptyParser();

  /**
   * Update the control stack of each polyhedral statements with the
   * new control.
   *
   */
  bool updateControlStack(SgNode* control);

  /**
   * Finalize control map, by creating map entries for polyhedral
   * statements without any control.
   *
   */
  void finalizeControlMap();



  /****************************************************************************/
  /***************************** getter / setter ******************************/
  /****************************************************************************/

  /**
   * Accessor.
   *
   */
  std::map<SgNode*, std::vector<SgNode*> > getControlMap();

  /**
   * Accessor.
   *
   */
  std::set<std::pair<SgVariableSymbol*,std::vector<bool> > > 
  getArraysAsScalars();

  /**
   * Accessor.
   *
   */
  std::set<SgVariableSymbol*> getLinearizedArraysAsArrays();

  /**
   * Accessor.
   *
   */
  std::map<std::string, SgVariableSymbol*> getFakeSymbolMap();

  /**
   * Setter.
   *
   */
  void setVerboseLevel(int level);

  /**
   * Setter.
   *
   */
  void setNonaffineArrays(bool val);

  /**
   * Setter.
   *
   */
  void setAllowMathFunc(bool val);


private:
  std::set<SgNode*> _polyStatements;
  std::vector<SgNode*> _controlStack;
  std::map<SgNode*, std::vector<SgNode*> > _controlMap;

  int _verboseLevel;
  bool _nonaffineArrays;
  bool _allowMathFunc;

  std::set<std::pair<SgVariableSymbol*, std::vector<bool> > > _arraysAsScalars;
  std::set<SgVariableSymbol*> _linearizedArraysAsArrays;
  std::map<std::string, SgVariableSymbol*> _fakeSymbolMap;

};

#endif

