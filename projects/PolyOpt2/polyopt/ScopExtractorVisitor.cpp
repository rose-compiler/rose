/*
 * ScopExtractorVisitor.cpp: This file is part of the PolyOpt project.
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
 * @file: ScopExtractorVisitor.cpp
 * @author: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 */


// LNP: To avoid redefined macro errors in pocc.
/// LNP: FIXME: Find a better solution!
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


#include <AstTraversal.h>
#include <polyopt/ScopExtractorVisitor.hpp>
#include <polyopt/SageNodeAnnotation.hpp>
#include <polyopt/SageTools.hpp>


using namespace std;
using namespace SageInterface;


/**
 * Special constructor to store the root node of the sub-tree.
 *
 *
 */
ScopExtractorVisitor::ScopExtractorVisitor(SgNode* root,
					   PolyRoseOptions& options) :
  AstPrePostOrderTraversal()
{
  _rootNode = root;
  _scopParser.setVerboseLevel(options.getScVerboseLevel());
  _scopParser.setNonaffineArrays(options.getScApproximateExtractor());
  _scopParser.setAllowMathFunc(options.getAllowMathFunc());
  _polyoptions = options;
}


/**
 * Postfix algorithm for SCoP detection.
 *
 * Algorithm (roughly):
 *
 *   postfix:
 *   if (! current.all_children_are_scops)
 *      current.isscop = false
 *      set = collect_all_children_in_worker_list
 *      consecutive_siblings_in_set become a scop
 *      non_consecutive_siblings_in_set are distinct scops
 *      remove set from worker_list
 *      mark_all_parents_of_current as non scop
 *   else
 *      if (check_if_is_scop current)
 *         current.isscop = true
 *         add_to_worker_list current
 *
 *
 *
 */
void
ScopExtractorVisitor::postOrderVisit(SgNode* node)
{
  // 0- Skip the node if it has already been marked as non-scop and
  // no current scop node detected.
  bool is_non_scop = _nonScopNodes.find(node) != _nonScopNodes.end();
  if (_scopNodes.size() == 0 && is_non_scop)
    return;

  // 1- Retrieve children nodes, for later use.
  std::vector<SgNode*> children = node->get_traversalSuccessorContainer();
  std::vector<SgNode*>::const_iterator i;

  if (! is_non_scop)
    {
      // 2- Skip inner nodes of SgExprStatement nodes.
      SgExprStatement* encloseExpr = getEnclosingNode<SgExprStatement>(node);
      if (encloseExpr != NULL && encloseExpr != node)
	return;

      // 3- Skip inner nodes of SgVariableDeclaration nodes.
      SgVariableDeclaration* encloseDecl =
	getEnclosingNode<SgVariableDeclaration>(node);
      if (encloseDecl != NULL && encloseDecl != node)
	return;

      // 4- Skip inner nodes of SgStatement.
      SgStatement* encloseStmt = getEnclosingStatement(node);
      if (encloseStmt != NULL && encloseStmt != isSgStatement(node))
	return;

      // 5- Skip SgFor member related (init,test,increment) nodes.
      SgForStatement* encloseFor = getEnclosingNode<SgForStatement>(node);
      if (encloseFor != NULL)
	{
	  SgForInitStatement* encloseForInit =
	    getEnclosingNode<SgForInitStatement>(node);
	  if ((node->get_parent() == isSgNode(encloseFor) ||
	       node->get_parent() == isSgNode(encloseForInit)) &&
	      encloseFor->get_loop_body() != node)
	    return;
	}

      // 6- Skip conditional from a SgIfStmt.
      SgIfStmt* ifStmt = getEnclosingNode<SgIfStmt>(node);
      if (ifStmt != NULL && ifStmt->get_conditional() == node)
	return;

      // 7- Skip conditional from a SgWhileStmt.
      SgWhileStmt* whileStmt = getEnclosingNode<SgWhileStmt>(node);
      if (whileStmt != NULL && whileStmt->get_condition() == node)
	return;

      // 8- Skip conditional from a SgSwitchStmt.
      SgSwitchStatement* switchStmt =
	getEnclosingNode<SgSwitchStatement>(node);
      if (switchStmt != NULL && switchStmt->get_item_selector() == node)
	return;

      // 8- Skip case label from a SgCaseOptionStmt.
      SgCaseOptionStmt* caseStmt =
	getEnclosingNode<SgCaseOptionStmt>(node);
      if (caseStmt != NULL && caseStmt->get_key() == node)
	return;

      // 8- Skip nodes that contain pointers.
      std::vector<SgNode*> pointers =
	NodeQuery::querySubTree(node, V_SgPointerDerefExp);
      if (pointers.size() == 0)
	{
	  // 9- Check if the current node is scop-compatible. At this stage,
	  // node can be:
	  //    - SgForStatement (is scop if affine bound and iv not written
	  //      in loop)
	  //    - SgExprStatement (is scop if affine array access)
	  //    - SgVariableDeclaration (is scop if not an alias)
	  //    - SgBasicBlock (is scop if all children are scop)
	  //    - SgIfStatement (is scop if condition is affine)
	  //	  - or an unmatched node (type, asm, class declaration, while,
	  //	    etc.) for which we always return false.
	  if (_scopParser.isScopCompatible(node, _scopNodes, _nonScopNodes))
	    {
	      _scopNodes.insert(node);
	      // If the current node is the root node, then the whole
	      // sub-tree is a Scop.
	      if (node == _rootNode)
		{
		  std::vector<SgNode*> nodes;
		  nodes.push_back(node);
		  ROSE_ASSERT(node->get_parent());
		  createScopFromNodes(node->get_parent(), nodes);
		}
	      return;
	    }
	}
    }


  // 10- The node is not a scop. Mark it and its ancestors as
  // non-scop, and create scop with its scop-compatible children.
  // Mark self and all ancestors as not scop-compatible.
  _nonScopNodes.insert(node);
  SgNode* tmp = node->get_parent();
  while (tmp)
    {
      _nonScopNodes.insert(tmp);
      tmp = tmp->get_parent();
    }
  // Build scops in the current worker list.
  std::vector<SgNode*> siblings;
  bool split_scop = false;
  SgNode* ifStmtElseClause = isSgIfStmt(node);
  if (ifStmtElseClause)
    ifStmtElseClause = isSgIfStmt(ifStmtElseClause)->get_false_body() ?
      isSgIfStmt(ifStmtElseClause)->get_false_body() : NULL;
  for (i = children.begin(); i != children.end(); ++i)
    {
      if (*i == NULL)
	continue;
      // Child is a scop. Store consecutive scop-compatible
      // siblings.
      if (_scopNodes.find(*i) != _scopNodes.end())
	{
	  // If the child is the first node of a 'then' clause, then break
	  // the scop into 2 scops: one for the 'then' and one for the
	  // 'else' parts.
	  if (! split_scop && ifStmtElseClause &&
	      SageTools::isDominatedBy(*i, ifStmtElseClause) &&
	      siblings.size() > 0)
	    {
	      split_scop = true;
	      // Create a Scop from the current sibling list.
	      // 1- Create a SgBasicBlock around the nodes.
	      // 2- Set the successors as the sibling list.
	      // 3- Insert the SgBasicBlock in the AST in place
	      // of the sibligs
	      // 4- Store the SgBasicBlock as the root of a Scop.
	      createScopFromNodes(node, siblings);

	      // 5- Empty the sibling list.
	      siblings.clear();
	    }

	  // 0- Ensure a loop iterator defined in '*i' is not used
	  // in 'siblings' (live-in iterator). If so, break the scop.
	  if (siblings.size() &&
	      ! _scopParser.checkIteratorUsedBeforeLoop(siblings, *i))
	    {
	      createScopFromNodes(node, siblings);
	      siblings.clear();
	      siblings.push_back(*i);
	    }
	  else
	    {
	      // 1- Ensure a loop iterator defined in 'siblings' is not used
	      // outside the loop which defines it (live-out iterator).
	      if (_scopParser.checkIteratorUsedOutsideLoop(siblings, *i))
		{
		  // 2- Ensure no symbol produced in 'siblings U *i' is used in
		  // any affine expression in siblings U *i.
		  if (_scopParser.checkAffineExpressionsReadOnlySymbols
		      (siblings, *i))
		    siblings.push_back(*i);
		  else
		    {
		      // A variable produced by a node in 'sibling' is used in
		      // an affine expression of *i. Split the scop.
		      createScopFromNodes(node, siblings);
		      siblings.clear();
		      siblings.push_back(*i);
		    }
		}
	      else
		{
		  // A loop iterator is used outside its defining
		  // loop. For the moment, be lazy, discard the nodes.
		  /// FIXME: Implement a clever scop-partitioning algorithm.
		  siblings.clear();
		}
	    }
	}
      else
	{
	  if (siblings.size() > 0)
	    {
	      // Child is not a scop. Create a Scop from the sibling list.
	      // 1- Create a SgBasicBlock around the nodes.
	      // 2- Set the successors as the sibling list.
	      // 3- Insert the SgBasicBlock in the AST in place
	      // of the sibligs
	      // 4- Store the SgBasicBlock as the root of a Scop.
	      createScopFromNodes(node, siblings);

	      // 5- Empty the sibling list.
	      siblings.clear();
	    }
	}
    }
  // Create a scop with the remaining scop-compatible siblings.
  if (siblings.size() > 0)
    {
      // 1- Create a SgBasicBlock around the nodes.
      // 2- Set the successors as the sibling list.
      // 3- Insert the SgBasicBlock in the AST in place of the sibligs
      // 4- Store the SgBasicBlock as the root of a Scop.
      createScopFromNodes(node, siblings);
    }

  // Empty the control stack and the set of polyhedral statements.
  _scopParser.emptyParser();
}


/**
 * Create a SCoP in the AST
 * - create a BB to contain the consecutive scop nodes
 * - register the BB in the list of SCoP roots
 *
 */
void
ScopExtractorVisitor::createScopFromNodes(SgNode* node,
					  std::vector<SgNode*>& siblings)
{
  SgNode* root;

  // Nothing to do.
  if (siblings.size() == 0)
    return;

  // Sanity check.
  if (*(siblings.begin()) == NULL) {
    cout << "[ScopExtraction] Fatal error" << endl;
    exit (1);
  }

  // The SCoP is useful only if it contains at least 1 loop.
  if (querySubTree<SgForStatement>(node, V_SgForStatement).size() == 0)
    return;

  // Always encapsulate a scop into a basic block.

  // If there's a single root and it is a basic block already, do nothing.
  if (siblings.size() == 1 && isSgBasicBlock(*(siblings.begin())))
    root = *(siblings.begin());
  else
    {
      // Else, create a basic block.
      SgBasicBlock* bb = SageBuilder::buildBasicBlock();

      // Plug it.
      SageInterface::replaceStatement(isSgStatement(*(siblings.begin())),
				      isSgStatement(bb));

      // Move the siblings into the bb.
      SgStatement* parent = isSgStatement((*(siblings.begin()))->get_parent());
      ROSE_ASSERT(parent);
      std::vector<SgNode*>::iterator i;
      bool first = true;
      for (i = siblings.begin(); i != siblings.end(); ++i)
	{
	  if (first)
	    first = false;
	  else
	    parent->remove_statement(isSgStatement(*i));
	  bb->append_statement(isSgStatement(*i));
	}

      root = bb;
    }

  // Final check: ensure any arrays interpreted as scalars are not in
  // fact linearized array access. If so, promote them to
  // linearizedArrayAsArrays container.
  if (_polyoptions.getScApproximateExtractor())
    _scopParser.checkLinearizedArrayAccesses(root);

  // Set annotation for the scop.
  ScopRootAnnotation* annot = new ScopRootAnnotation();
  annot->arraysAsScalars = _scopParser.getArraysAsScalars();
  annot->linearizedArraysAsArrays = _scopParser.getLinearizedArraysAsArrays();
  annot->fakeSymbolMap = _scopParser.getFakeSymbolMap();
  root->setAttribute("ScopRoot", annot);

  // Store the root of the newly found scop.
  _scopRoots.push_back(root);

  // Clear the worker list from the current scop nodes.
  eraseTreeFromScopNodes(node);
}


/**
 * Erase the sub-tree with 'node' as its root from the worker list
 * '_scopNodes'.
 *
 *
 */
void
ScopExtractorVisitor::eraseTreeFromScopNodes(SgNode* node)
{
  // Internal visitor to delete sub-tree from worker list.
  class EraseVisitor : public AstPrePostOrderTraversal {
  public:
    EraseVisitor(std::set<SgNode*> nodelist) : AstPrePostOrderTraversal() {
      _nodelist = nodelist;
    }
    virtual void preOrderVisit(SgNode* node) {};
    virtual void postOrderVisit(SgNode* node) {
      _nodelist.erase(node);
    }
  private:
    std::set<SgNode*> _nodelist;
  };

  EraseVisitor ev(_scopNodes);
  ev.traverse(node);
}


/**
 * Remove all scops which does not contain at least one for loop.
 *
 *
 */
void
ScopExtractorVisitor::pruneScopWithoutLoop()
{
  std::vector<SgNode*>::iterator i;

  std::vector<SgNode*> newScopRoots;
  for (i = _scopRoots.begin(); i != _scopRoots.end(); ++i)
    if (querySubTree<SgForStatement>(*i, V_SgForStatement).size() != 0)
      newScopRoots.push_back(*i);
  _scopRoots = newScopRoots;
}

/**
 *
 *
 */
void
ScopExtractorVisitor::buildControlInformation()
{
  // 1- Finalize control map, by creating map entries for polyhedral
  // statements without any control.
  _scopParser.finalizeControlMap();
}


/**
 * Accessor.
 *
 */
std::vector<SgNode*>
ScopExtractorVisitor::getScopRoots()
{
  return _scopRoots;
}


/**
 * Accessor.
 *
 */
std::map<SgNode*, std::vector<SgNode*> >
ScopExtractorVisitor::getControlMap()
{
  return _scopParser.getControlMap();
}


/**
 * Initializer.
 *
 */
void
ScopExtractorVisitor::reinitializeAttributes()
{
  _scopRoots.clear();
  _scopNodes.clear();
  _nonScopNodes.clear();
}
