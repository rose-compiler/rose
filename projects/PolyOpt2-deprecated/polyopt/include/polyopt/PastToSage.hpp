/*
 * PastToSage.hpp: This file is part of the PolyOpt project.
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
 * @file: PastToSage.hpp
 * @author: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 */

#if !defined(PLUTO_ROSE_PAST_TO_SAGE_INC)
# define PLUTO_ROSE_PAST_TO_SAGE_INC 1


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


// Execute 'x' only if the containing class' verbose flag is set.
// The containing class must contain an m_verbose boolean flag.
#define IF_PLUTO_ROSE_VERBOSE(x) if(m_verbose){x;std::cout.flush();std::cerr.flush();}


/**
 * \brief Converter from Past to Sage.
 *
 * Converts a Past representation of generated code to an equivalent Sage
 * representation.
 */
class PastToSage
{
public:

  /**
   * \brief Default constructor.
   *
   * Constructs a new %PastToSage instance.
   *
   */
  /// LNP: Experimental constructor.
  PastToSage(SgScopeStatement* scopScope,
	     s_past_node_t* root,
	     scoplib_scop_p scoplibScop,
	     PolyRoseOptions& options,
	     int scopId);


  /**
   * \brief Returns the generated Sage basic block.
   *
   * Returns the Sage basic block that was generated from the Past root node.
   *
   * \return              The generated Sage basic block.
   */
  SgBasicBlock* getBasicBlock();



private:

  void buildBasicBlock(s_past_node_t* root, SgBasicBlock* bb);


  /**
   * Create a Sage if statement from a PAST guard.
   *
   *
   */
  SgStatement* buildGenericIf(s_past_node_t* guard);


  /**
   *
   * Create a Sage for statement.
   *
   */
  SgStatement* buildFor(s_past_for_t* forStmt);


  /**
   * Create a Sage statement with updated access functions.
   *
   *
   */
  SgStatement* buildUserStatement(s_past_cloogstmt_t* statement);

  /**
   * Create a SgVarRefExp from a PAST variable reference.
   *
   *
   */
  SgExpression* buildVariableReference(s_past_variable_t* varref);



  /**
   * Generic PAST-to-Sage expression builder.
   *
   *
   */
  SgExpression* buildExpressionTree(s_past_node_t* node);

  SgExpression* buildCeildFloord(s_past_binary_t* binary);

  SgExpression* buildMinMax(s_past_binary_t* minmax);


  /**
   * \brief Annotates the node with the currently stored annotation (#pragma)
   *
   * Adds an annotation to the current node, contained in the annotationNode
   *
   * \param stmt	  The Sage node to annotate.
   * \param cnode	  The Past node to annotate.
   *
   */
  void updateAnnotation(SgStatement* stmt, s_past_node_t* cnode);

  std::string buildPragmaOmpForC(s_past_node_t* forTree);


  std::vector<void*>
  collectAllIterators(s_past_node_t* root);

  std::map<void*, SgVariableDeclaration*>
  createNewIterators(std::vector<void*> vars, SgScopeStatement* scope);
  std::map<void*, SgVariableDeclaration*>
  createTileSizeParameters(s_symbol_table_t* symtable,
			   SgScopeStatement* scope,
			   scoplib_scop_p scop,
			   PolyRoseOptions& options);

  bool isIterator(void* ptr);
  bool isTileSizeParameter(void* ptr);
  std::string getAvailableName(std::string templatestr);

  bool insertPragmas(SgNode* root);


  std::vector<void*> _scoplibIterators;
  std::map<void*, SgVariableDeclaration*> _sageIterators;
  std::map<void*, SgVariableDeclaration*> _sageTileSizeParameters;
  std::vector<std::string> _usedSymbols;

  bool                          m_verbose;
  SgScopeStatement*             m_scope;
  SgBasicBlock*                 m_scopRoot;
  scoplib_scop_p                m_scoplib_scop;
  std::map<int, SgStatement*>   m_stmtMapping;
  std::set<std::string>         m_iterators;
  int                           m_var_counter_id;
  std::vector<s_past_node_t*>   annotationNodes;

private:
  std::map<std::string, SgVariableSymbol*> _fakeSymbolMap;
  PolyRoseOptions _polyoptions;
};

#endif

