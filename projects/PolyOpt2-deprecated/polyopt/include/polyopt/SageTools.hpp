/*
 * SageTools.hpp: This file is part of the PolyOpt project.
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
 * @file: SageTools.hpp
 * @author: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 */

#ifndef POLYOPT_SAGE_TOOLS_HPP
# define POLYOPT_SAGE_TOOLS_HPP

# include <rose.h>
# include <vector>
# include <set>
# include <map>

/**
 * \brief Sage tools.
 *
 * This class provides static methods to process Sage nodes (eg,
 * collecting references or identifying particular types/structures).
 *
 */
class SageTools
{
private:
  ~SageTools();

public:

  /****************************************************************************/
  /*********************** Variable reference handling ************************/
  /****************************************************************************/

  /**
   * Collect read-only symbols.
   * Create a unique fake symbol for SgDotExp(variable, field).
   *
   */
  static
  bool
  collectReadOnlySymbols(SgStatement* node,
			 std::set<SgVariableSymbol*>& symbols,
			 std::map<std::string, SgVariableSymbol*>&
			 fakeSymbolMap,
			 bool allowMathFunc);

  /**
   * Collect all references in the subtree 'node'.
   * Ensure no pointer arithmetic for any of the symbols used.
   *
   */
  static
  bool
  collectReadWriteRefs(SgStatement* node,
		       std::vector<SgNode*>& readRefs,
		       std::vector<SgNode*>& writeRefs,
		       bool allowMathFunc);


  /**
   * Collect R/W references in the subtree 'node'.
   * Efficient re-implementation of SageInterface::collectReadWriteRefs
   *
   * FIXME: Does not deal properly with pointer arithmetics.
   *
   */
  static
  bool
  collectReadWriteRefsFastImpl(SgStatement* node,
			       std::vector<SgNode*>& readRefs,
			       std::vector<SgNode*>& writeRefs,
			       bool allowMathFuncs);

  /**
   * Helper. Convert std::vector<SgNode*> constaining references to
   * std::set<SgVariableSymbol*>
   *
   *
   */
  static
  std::set<SgVariableSymbol*>
  convertToSymbolSet(std::vector<SgNode*>& refs,
		     std::map<std::string, SgVariableSymbol*>& fakeSymbolMap);

  /**
   * Create a fake symbol for a SgDotExp(variable, field) node. Store
   * the symbol in the map if not already existing.
   *
   */
  static
  SgVariableSymbol*
  getFakeSymbol(SgDotExp* node, std::map<std::string, SgVariableSymbol*>& map);

  /**
   * Get a symbol from a reference. If the reference is part of a C
   * structure/field reference, return the associated fake symbol
   * (create it if needed).
   * Return NULL if the symbol cannot be extracted.
   *
   */
  static
  SgVariableSymbol*
  getSymbolFromReference(SgNode* node,
			 std::map<std::string, SgVariableSymbol*>& map);

  /**
   * Get the symbol associated with the loop iterator.
   *
   * Works only for loops with increment of the form
   *  'iterator++'
   *  '++iterator'
   *  'iterator += 1'
   *
   */
  static
  SgVariableSymbol*
  getLoopIteratorSymbol(SgForStatement* fornode);




  /****************************************************************************/
  /************************ Node and tree recognition *************************/
  /****************************************************************************/

  /**
   * Returns true if 'node' is a math function call.
   *
   *
   */
  static
  bool
  isMathFunc(SgFunctionSymbol* node);

  /**
   * Return true if the node is '>', '>=', '<', '<=' or '=='.
   *
   */
  static
  bool
  isCompareOp(SgNode* node);

  /**
   * Returns true if the value 'expr' can be promoted to an integer
   * type.
   *
   */
  static
  bool
  isIntegerTypeValue(SgNode* expr);

  /**
   * Returns an inter value of an integer type node into a long long int.
   * Careful of overflow with integer types larger than long int.
   *
   */
  static
  long int
  getIntegerTypeValue(SgNode* expr);

  /**
   * Check if the node is a min(x,y) expression using x < y ? x : y
   *
   */
  static
  bool
  isMinFuncCall(SgNode* node);

  /**
   * Check if the node is a max(x,y) expression using x > y ? x : y
   *
   */
  static
  bool
  isMaxFuncCall(SgNode* node);

  /**
   * Check if the node is floord(n, d), defined as:
   * (((n*d)<0) ? (((d)<0) ? -((-(n)+(d)+1)/(d)) : -((-(n)+(d)-1)/(d))) : (n)/(d))
   *
   */
  static
  bool
  isFloorFuncCall(SgNode* node);

  /**
   * Check if the node is ceil(n, d), defined as:
   * (((n*d)<0) ? -((-(n))/(d)) : (((d)<0) ? (((-n)+(-d)-1)/(-d)) :  ((n)+(d)-1)/(d)))
   *
   */
  static
  bool
  isCeilFuncCall(SgNode* node);

  /**
   * FIXME: implement this
   *
   */
  static
  bool
  checkTreeExpAreEquivalent(SgNode* e1, SgNode* e2);

  /**
   * FIXME: implement this
   *
   */
  static
  bool
  checkTreeAreEquivalent(SgNode* e1, SgNode* e2);

  /**
   * Check if a node is dominated by a specific node.
   *
   */
  static
  bool
  isDominatedBy(SgNode* child, SgNode* dominator);




  /****************************************************************************/
  /**************************** Output verbosifier ****************************/
  /****************************************************************************/

  /**
   * Verbose output. return false.
   *
   */
  static
  bool
  invalidNode(SgNode* node, const char* message);

  /**
   * Verbose output. return true.
   *
   */
  static
  bool
  validNode(SgNode* node, const char* message);

  /**
   * Prints a tree and the type of its root.
   *
   */
  static
  void
  debugPrintNode(SgNode* node);

};

#endif

