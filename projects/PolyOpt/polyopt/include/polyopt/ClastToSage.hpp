/*
 * ClastToSage.hpp: This file is part of the PolyOpt project.
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
 * @file: ClastToSage.hpp
 * @author: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 */

#if !defined(PLUTO_ROSE_CLAST_TO_SAGE_INC)
# define PLUTO_ROSE_CLAST_TO_SAGE_INC 1


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
# include <clasttools/clastext.h>

# include <polyopt/PolyRoseOptions.hpp>


// Execute 'x' only if the containing class' verbose flag is set.
// The containing class must contain an m_verbose boolean flag.
#define IF_PLUTO_ROSE_VERBOSE(x) if(m_verbose){x;std::cout.flush();std::cerr.flush();}


/**
 * \brief Converter from Clast to Sage.
 *
 * Converts a Clast representation of generated code to an equivalent Sage
 * representation.
 */
class ClastToSage
{
public:

  /**
   * \brief Default constructor.
   *
   * Constructs a new %ClastToSage instance.
   *
   */
  /// LNP: Experimental constructor.
  ClastToSage(SgScopeStatement* scopScope,
	      clast_stmt* root,
	      scoplib_scop_p scoplibScop,
	      PolyRoseOptions& options);


  /**
   * \brief Returns the generated Sage basic block.
   *
   * Returns the Sage basic block that was generated from the Clast root node.
   *
   * \return              The generated Sage basic block.
   */
  SgBasicBlock* getBasicBlock();



private:

  /**
   * \brief Builds a Sage basic block from a list of Clast statements.
   *
   * Builds a Sage basic block froma  list of Clast statements, recursively
   * building each statement as it appears.
   *
   * \param root          The first Clast statement.
   *
   * \return              The generated Sage basic block.
   */
  SgBasicBlock* buildBasicBlock(clast_stmt* root);

  /**
   * \brief Builds a Sage statement from a Clast guard.
   *
   * Builds a Sage statement from a Clast guard, recursively building each
   * Clast sub-node.
   *
   * \param guard         The Clast guard.
   *
   * \return              The generated Sage statement.
   */
  SgStatement* buildGuard(clast_guard* guard);

  /**
   * \brief Builds a Sage statement from a Clast for statement.
   *
   * Builds a Sage statement from a Clast for statement, recursively building
   * each Clast sub-node.
   *
   * \param forStmt       The Clast for statement.
   *
   * \return              The generated Sage statement.
   */
  SgStatement* buildFor(clast_for* forStmt);

  /**
   * \brief Builds a Sage statement from a Clast assignment.
   *
   * Builds a Sage statement from a Clast assignment, recursively building
   * each Clast sub-node.
   *
   * \param assignment    The Clast assignment.
   *
   * \return              The generated Sage statement.
   */
  SgStatement* buildAssignment(clast_assignment* assignment);

  /**
   * \brief Builds a Sage statement from a Clast user statement.
   *
   * Builds a Sage statement from a Clast user statement, recursively building
   * each Clast sub-node.
   *
   * \param statement     The Clast user statement.
   *
   * \return              The generated Sage statement.
   */
  SgStatement* buildUserStatement(clast_user_stmt* statement);

  /**
   * \brief Builds a Sage expression from a Clast equation.
   *
   * Builds a Sage expression from a Clast equation, recursively building each
   * Clast sub-node.
   *
   * \param equation      The Clast equation.
   *
   * \return              The generated Sage expression.
   */
  SgExpression* buildEquation(clast_equation* equation);

  /**
   * \brief Builds a Sage expression from a Clast expression.
   *
   * Builds a Sage expression from a Clast expression, recursively building
   * each Clast sub-node.
   *
   * \param expr          The Clast expression.
   *
   * \return              The generated Sage expression.
   */
  SgExpression* buildExpression(clast_expr* expr);

  /**
   * \brief Builds a Sage expression from a Clast name.
   *
   * Builds a Sage expression from a Clast name, recursively building each
   * Clast sub-node.
   *
   * \param name          The Clast name.
   *
   * \return              The generated Sage expression.
   */
  SgExpression* buildName(clast_name* name);

  /**
   * \brief Builds a Sage expression from a Clast term.
   *
   * Builds a Sage expression from a Clast term, recursively building each
   * Clast sub-node.
   *
   * \param term          The Clast term.
   *
   * \return              The generated Sage expression.
   */
  SgExpression* buildTerm(clast_term* term);

  /**
   * \brief Builds a Sage expression from a Clast sum.
   *
   * Builds a Sage expression from a Clast sum, recursively building each
   * Clast sub-node.
   *
   * \param sum           The Clast sum.
   *
   * \return              The generated Sage expression.
   */
  SgExpression* buildSum(clast_reduction* reduction);

  /**
   * \brief Builds a Sage expression from a Clast binary operation.
   *
   * Builds a Sage expression from a Clast binary operation, recursively
   * building each Clast sub-node.
   *
   * \param binary        The Clast binary operation.
   *
   * \return              The generated Sage expression.
   */
  SgExpression* buildBinaryOp(clast_binary* binary);

  /**
   * \brief Builds a Sage expression from a Clast minmax.
   *
   * Builds a Sage expression from a Clast minmax, recursively building each
   * Clast sub-node.
   *
   * \param minmax        The Clast minmax.
   *
   * \return              The generated Sage expression.
   */
  SgExpression* buildMinMax(clast_reduction* minmax);

  /**
   * \brief Builds a Sage expression from a Clast reduction.
   *
   * Builds a Sage expression from a Clast reduction, recursively building each
   * Clast sub-node.
   *
   * \param reduction     The Clast reduction.
   *
   * \return              The generated Sage expression.
   */
  SgExpression* buildReduction(clast_reduction* reduction);


  /**
   * \brief Annotates the node with the currently stored annotation (#pragma)
   *
   * Adds an annotation to the current node, contained in the annotationNode
   *
   * \param stmt	  The Sage node to annotate.
   * \param cnode	  The Clast node to annotate.
   *
   */
  void updateAnnotation(SgStatement* stmt, clast_stmt* cnode);



  std::vector<const char*>
  collectAllIterators(clast_stmt* root);

  std::map<const char*, SgVariableDeclaration*>
  createNewIterators(std::vector<const char*> vars, SgScopeStatement* scope);

  bool isIterator(const char* ptr);

  bool insertPragmas(SgNode* root);


  std::vector<const char*> _scoplibIterators;
  std::map<const char*, SgVariableDeclaration*> _sageIterators;


  bool                          m_verbose;
  SgScopeStatement*             m_scope;
  SgBasicBlock*                 m_scopRoot;
  scoplib_scop_p                m_scoplib_scop;
  std::map<int, SgStatement*>   m_stmtMapping;
  std::set<std::string>         m_iterators;

  std::vector<clast_stmt*>     	annotationNodes;

private:
  std::map<std::string, SgVariableSymbol*> _fakeSymbolMap;
  PolyRoseOptions _polyoptions;
};

#endif

