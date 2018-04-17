/*
 * AffineConverter.cpp: This file is part of the PolyOpt project.
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
 * @file: AffineConverter.cpp
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
#include <polyopt/AffineConverter.hpp>
#include <polyopt/SageNodeAnnotation.hpp>
#include <polyopt/SageTools.hpp>


using namespace std;
using namespace SageInterface;

AffineConverter::AffineConverter(PolyRoseOptions& options)
{
  _polyoptions = options;
}

AffineConverter::~AffineConverter()
{ }




/******************************************************************************/
/********************** High level extraction routines ************************/
/******************************************************************************/

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
AffineConverter::createScopLibFromScops(std::vector<SgNode*>& scopRoots,
					std::map<SgNode*,
					std::vector<SgNode*> >& controlMap)
{
  std::vector<scoplib_scop_p> scops;

  std::vector<SgNode*>::const_iterator i;
  for (i = scopRoots.begin(); i != scopRoots.end(); ++i)
    {
      // Global references for the SCoP.
      scoplib_scop_p scop = scoplib_scop_malloc();
      scoplib_statement_p curr = NULL;
      std::map<SgVariableSymbol*, int> parametersMap;
      std::map<SgVariableSymbol*, int> variablesMap;
      std::map<SgNode*, scoplib_matrix_p> matrixMap;
      std::map<SgNode*, std::vector<SgNode*> >::iterator ctrl;
      // Get the scop annotations.
      ScopRootAnnotation* annot =
	(ScopRootAnnotation*)((*i)->getAttribute("ScopRoot"));
      ROSE_ASSERT(annot);
      _arraysAsScalars = annot->arraysAsScalars;
      _fakeSymbolMap = annot->fakeSymbolMap;

      // 0- Number the scop nodes in prefix visit order. Used to later
      // build the statements schedule.
      buildScopAstOrdering(*i);

      // 1- Store the Sage root node in the scop.
      scop->usr = *i;

      // 1- Get all polyhedral statements.
      std::vector<SgNode*> candidatestmts =
	NodeQuery::querySubTree(*i, V_SgExprStatement);
      std::vector<SgNode*>::const_iterator s;
      std::vector<SgNode*> polystmts;
      for (s = candidatestmts.begin(); s != candidatestmts.end(); ++s)
	if (controlMap.find(*s) != controlMap.end())
	  polystmts.push_back(*s);

      // 2- Create the parameters map. Collect all references in
      // the control and array accesses which are not loop
      // iterators, and add them into the parameters map.
      for (s = polystmts.begin(); s != polystmts.end(); ++s)
	{
	  ctrl = controlMap.find(*s);
	  updateParametersMap(*s, ctrl->second, parametersMap);
	}

      // 3- Create the context matrix for the SCoP.
      scop->context = createContext(parametersMap);
      scop->nb_parameters = scop->context->NbColumns - 2;
      scop->parameters = createParametersArray(parametersMap);

      // 3- For all nodes which have a control map entry (ie,
      // polyhedral statements), build the statement polyhedral IR.
      for (s = polystmts.begin(); s != polystmts.end(); ++s)
	{
	  std::map<SgVariableSymbol*, int> iteratorsMap;
	  ctrl = controlMap.find(*s);
	  ROSE_ASSERT(ctrl != controlMap.end());
	  SgStatement* stmt = isSgStatement(*s);
	  ROSE_ASSERT(stmt);

	  // a- get the loop iterators. We must introduce a
	  // one-time-loop for statements not surrounded by any loop.
	  iteratorsMap = getLoopIteratorsMap(ctrl->second);
	  if (iteratorsMap.size() == 0)
	    {
	      iteratorsMap = getFakeControlIteratorMap();
	      ctrl->second.push_back(getFakeControlNode());
	    }

	  // b- translate control into affine representation. This
	  // step is decoupled to allow caching computations, as a
	  // single control may be shared between several polyhedral
	  // statements.
	  createControlMatrices(ctrl->second, iteratorsMap, parametersMap,
				matrixMap);

	  // c- Create the scop entry.
	  scoplib_statement_p statement = scoplib_statement_malloc();
	  statement->domain = createDomain(ctrl->second, matrixMap,
					   iteratorsMap.size(),
					   parametersMap.size());
	  statement->schedule = createSchedule(*s, ctrl->second,
					       iteratorsMap.size(),
					       parametersMap.size());
	  statement->read = createAccessFunction(stmt, iteratorsMap,
						 parametersMap, variablesMap,
						 READ_REFERENCES);
	  statement->write = createAccessFunction(stmt, iteratorsMap,
						  parametersMap, variablesMap,
						  WRITE_REFERENCES);
	  statement->nb_iterators = iteratorsMap.size();
	  statement->iterators = createIteratorsArray(iteratorsMap);
	  statement->body      = (char*) (*s);
	  if (curr == NULL)
	    scop->statement = statement;
	  else
	    curr->next = statement;
	  curr = statement;

	}

      scop->nb_arrays = variablesMap.size();
      scop->arrays = createVariablesArray(variablesMap);

      // Normalize the schedule.
      scoplib_scop_normalize_schedule (scop);

      // Insert the newly created scop.
      scops.push_back(scop);
    }

  return scops;
}



/**
 * Create the matrix representation of all control nodes (eg, SgFor,
 * SgIf).  Associate a matrix to each node.
 *
 */
bool
AffineConverter::createControlMatrices(std::vector<SgNode*>& controlStack,
				       std::map<SgVariableSymbol*, int>&
				       iteratorsMap,
				       std::map<SgVariableSymbol*, int>&
				       parametersMap,
				       std::map<SgNode*, scoplib_matrix_p>&
				       matrixMap)
{
  std::vector<SgNode*>::const_iterator i;
  for (i = controlStack.begin(); i != controlStack.end(); ++i)
    {
      // Discard a control that has already been analyzed.
      if (matrixMap.find(*i) != matrixMap.end())
	continue;

      // Special case of fake controls.
      if (*i == getFakeControlNode())
	{
	  scoplib_matrix_p matrix =
	    scoplib_matrix_malloc(2, 3 + parametersMap.size());
	  SCOPVAL_set_si(matrix->p[0][0], 1);
	  SCOPVAL_set_si(matrix->p[1][0], 1);
	  SCOPVAL_set_si(matrix->p[0][1], 1);
	  SCOPVAL_set_si(matrix->p[1][1], -1);
	  std::pair<SgNode*, scoplib_matrix_p> newelt(*i, matrix);
	  matrixMap.insert(newelt);
	  continue;
	}

      // For statement.
      SgForStatement* fornode = isSgForStatement(*i);
      if (fornode)
	{
	  ScopForAnnotation* annot =
	    (ScopForAnnotation*)(fornode->getAttribute("ScopFor"));
	  scoplib_matrix_p mat1 = NULL;
	  // Special case for the lb: we do want to retrieve the full
	  // assignment: 'i = 0', and not only '0'.
	  if (isSgAssignInitializer(annot->lb->get_parent()))
	    {
	      // Special special case: variable declaration in For
	      // init statement. C99-specific.
	      // Temporarily create an expression.
	      SgVarRefExp* iterRef =
		SageBuilder::buildVarRefExp(annot->iterator);
	      SgAssignOp* asop = SageBuilder::buildBinaryExpression<SgAssignOp>
		(iterRef, annot->lb);
	      mat1 = createMatrix(isSgExpression(asop),
	      			  iteratorsMap, parametersMap);
	      delete iterRef;
	      delete asop;
	    }
	  else
	    mat1 = createMatrix(isSgExpression(annot->lb->get_parent()),
				iteratorsMap, parametersMap);
	  // For the ub, the full expression 'i < N' is already
	  // stored.
	  scoplib_matrix_p mat2 =
	    createMatrix(annot->ub, iteratorsMap, parametersMap);
	  scoplib_matrix_p matrix = scoplib_matrix_concat(mat1, mat2);
	  scoplib_matrix_free(mat1);
	  scoplib_matrix_free(mat2);
	  std::pair<SgNode*, scoplib_matrix_p> newelt(fornode, matrix);
	  matrixMap.insert(newelt);
	}
      // If statement.
      else
	{
	  SgIfStmt* ifnode = isSgIfStmt(*i);
	  if (ifnode)
	    {
	      ScopIfAnnotation* annot =
		(ScopIfAnnotation*)(ifnode->getAttribute("ScopIf"));
	      // Special case: if (cond) statement, must be translated
	      // to cond >= 1 and not cond >= 0.
	      // SgTreeCopy treeCopy;
	      // SgNode* origCond = annot->conditional;
	      // ROSE_ASSERT(origStatement);
	      // SgNode* tmp = origCond->copy(treeCopy);
	      // extendBooleanCond (tmp);
	      scoplib_matrix_p matrix = createMatrix(annot->conditional,
						     iteratorsMap,
						     parametersMap);

	      std::pair<SgNode*, scoplib_matrix_p> newelt(ifnode, matrix);
	      matrixMap.insert(newelt);
	    }
	}
    }


  return true;
}


/**
 * Create the iterator symbol array for the scoplib_scop_p.
 *
 */
char**
AffineConverter::createIteratorsArray(std::map<SgVariableSymbol*, int>&
				      iteratorsMap)
{
  char** res = new char*[iteratorsMap.size()];
  std::map<SgVariableSymbol*, int>::const_iterator i;
  for (i = iteratorsMap.begin(); i != iteratorsMap.end(); ++i)
    res[i->second] = (char*)(i->first);

  return res;
}


/**
 * Create the parameter symbol array for the scoplib_scop_p.
 *
 */
char**
AffineConverter::createParametersArray(std::map<SgVariableSymbol*, int>&
				       parametersMap)
{
  char** res = new char*[parametersMap.size()];
  std::map<SgVariableSymbol*, int>::const_iterator i;
  for (i = parametersMap.begin(); i != parametersMap.end(); ++i)
    res[i->second] = (char*)(i->first);

  return res;
}


/**
 * Create the variable symbol array for the scoplib_scop_p.
 *
 */
char**
AffineConverter::createVariablesArray(std::map<SgVariableSymbol*, int>&
				      variablesMap)
{
  char** res = new char*[variablesMap.size()];
  std::map<SgVariableSymbol*, int>::const_iterator i;
  for (i = variablesMap.begin(); i != variablesMap.end(); ++i)
    res[i->second] = (char*)(i->first);

  return res;
}


/**
 * Create the context matrix for the scoplib_scop_p.
 *
 */
scoplib_matrix_p
AffineConverter::createContext(std::map<SgVariableSymbol*, int>& parametersMap)
{
  return scoplib_matrix_malloc(0, parametersMap.size() + 2);;
}


/**
 * Create the iteration domain matrix for a scoplib_statement_p.
 *
 */
scoplib_matrix_list_p
AffineConverter::createDomain(std::vector<SgNode*>& ctrl,
			      std::map<SgNode*, scoplib_matrix_p>& matrixMap,
			      int nbIters,
			      int nbParams)
{
  scoplib_matrix_p res;
  int i;
  int last = ctrl.size() - 1;
  res = buildAdjustedMatrixIterators(matrixMap.find(ctrl[last])->second,
				     nbIters, nbParams);
  for (i = last - 1; i >= 0; --i)
    {
      scoplib_matrix_p ctmat =
	buildAdjustedMatrixIterators(matrixMap.find(ctrl[i])->second,
				     nbIters, nbParams);
      scoplib_matrix_p tmp = scoplib_matrix_concat(res, ctmat);
      scoplib_matrix_free(res);
      scoplib_matrix_free(ctmat);
      res = tmp;
    }
  scoplib_matrix_list_p list = scoplib_matrix_list_malloc();
  list->elt = res;

  return list;
}


/**
 * Create the scheduling matrix for a scoplib_statement_p.
 *
 */
scoplib_matrix_p
AffineConverter::createSchedule(SgNode* node,
				std::vector<SgNode*>& control,
				int nbIters,
				int nbParams)
{
  int stmtAstId;
  SgExprStatement* stmt = isSgExprStatement(node);
  if (stmt)
    {
      ScopStatementAnnotation* annot =
	(ScopStatementAnnotation*)(stmt->getAttribute("ScopStmt"));
      stmtAstId = annot->astId;
    }

  int i;
  int sz = control.size();
  std::vector<int> sched;
  for (i = sz - 1; i >= 0; --i)
    {
      SgForStatement* fornode = isSgForStatement(control[i]);
      if (fornode)
	{
	  ScopForAnnotation* annot =
	    (ScopForAnnotation*)(fornode->getAttribute("ScopFor"));
	  ROSE_ASSERT(annot);
	  sched.push_back(annot->astId);
	}
      else if (control[i] == getFakeControlNode())
	sched.push_back(0);
    }

  if (stmt)
    sched.push_back(stmtAstId);

  sz = sched.size();

  int nbScatt = 2 * sz - 1;
  scoplib_matrix_p res =
    scoplib_matrix_malloc(nbScatt, nbIters + nbParams + 2);

  for (i = 0; i < nbScatt; ++i)
    {
      /// DEBUG:
      ROSE_ASSERT(1 + i/2 < nbIters + nbParams + 2);
      if (i % 2 == 1)
	SCOPVAL_set_si(res->p[i][1 + i/2], 1);
      else
	SCOPVAL_set_si(res->p[i][res->NbColumns - 1], sched[i/2]);
    }

  return res;
}


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
AffineConverter::createAccessFunction(SgStatement* node,
				      std::map<SgVariableSymbol*, int>&
				      iteratorsMap,
				      std::map<SgVariableSymbol*, int>&
				      parametersMap,
				      std::map<SgVariableSymbol*, int>&
				      variablesMap,
				      bool isReadRef)
{
  // Collect all references in the statement.
  ScopStatementAnnotation* annotStmt =
    (ScopStatementAnnotation*)node->getAttribute("ScopStmt");
  ROSE_ASSERT(annotStmt);
  std::vector<SgNode*> readRefs = annotStmt->readRefs;
  std::vector<SgNode*> writeRefs = annotStmt->writeRefs;

  // Select the appropriate collection (read or write refs).
  std::vector<SgNode*>* refs;
  if (isReadRef == READ_REFERENCES)
    refs = &readRefs;
  else
    refs = &writeRefs;

  int nbCols = iteratorsMap.size() + parametersMap.size() + 2;
  scoplib_matrix_p res = scoplib_matrix_malloc(0, nbCols);

  // Iterate on all references, and:
  // 1- if the reference is not an iterator nor a parameter, update
  // the variable map if needed
  // 2- create a row in the access function for the variable.
  std::vector<SgNode*>::const_iterator i;
  for (i = refs->begin(); i != refs->end(); ++i)
    {
      SgVariableSymbol* symb;
      SgVarRefExp* ref = isSgVarRefExp(*i);
      SgDotExp* dexp = isSgDotExp((*i)->get_parent());
      SgPntrArrRefExp* aref = isSgPntrArrRefExp(*i);
      if (! (ref || dexp || aref))
	// Something weird happened...
	/// FIXME: probably safer to ROSE_ASSERT(0) here?
	continue;
      // Case 1: access to a scalar.
      if (ref || dexp)
	{
	  // Skip the RHS reference of a dexp, we process only the LHS.
	  if (dexp && dexp->get_rhs_operand() == ref)
	    continue;
	  // Get the symbol associated to the reference.
	  symb = dexp ? SageTools::getFakeSymbol(dexp, _fakeSymbolMap) :
	    ref->get_symbol();
	  // Ensure it is a variable, no read reference to
	  // iterators/parameters need to be stored (useless).
	  if (iteratorsMap.find(symb) == iteratorsMap.end() &&
	      parametersMap.find(symb) == parametersMap.end())
	    {
	      std::map<SgVariableSymbol*, int>::iterator elt =
		variablesMap.find(symb);
	      if (elt == variablesMap.end())
		{
		  std::pair<SgVariableSymbol*, int>
		    newelt(symb, variablesMap.size());
		  variablesMap.insert(newelt);
		  elt = variablesMap.find(symb);
		}
	      scoplib_matrix_p tmp = scoplib_matrix_malloc(1, nbCols);
	      /// DEBUG:
	      ROSE_ASSERT(nbCols > 0);
	      // Variables are 1-indexed.
	      SCOPVAL_set_si(tmp->p[0][0], elt->second + 1);
	      scoplib_matrix_p c = scoplib_matrix_concat(res, tmp);
	      scoplib_matrix_free(res);
	      scoplib_matrix_free(tmp);
	      res = c;
	      // Store the original reference pointer.
	      if (isReadRef == READ_REFERENCES)
		annotStmt->readAffineRefs.push_back(*i);
	      else
		annotStmt->writeAffineRefs.push_back(*i);
	    }
	}
      // Case 2: access to an array.
      else
	{
	  // Find the symbol associated with the array reference.
	  SgPntrArrRefExp* last;
	  while ((last = isSgPntrArrRefExp(aref->get_lhs_operand())))
	    aref = last;
	  ref = isSgVarRefExp(aref->get_lhs_operand());
	  SgDotExp* dexp = NULL;
	  if (! ref)
	    dexp = isSgDotExp(aref->get_lhs_operand());
	  ROSE_ASSERT(ref || dexp);
	  symb = ref ? ref->get_symbol() :
	    SageTools::getFakeSymbol(dexp, _fakeSymbolMap);
	  // Ensure the symbol is a variable. This is overkill for
	  // arrays.
	  if (iteratorsMap.find(symb) == iteratorsMap.end() &&
	      parametersMap.find(symb) == parametersMap.end())
	    {
	      std::map<SgVariableSymbol*, int>::iterator elt =
		variablesMap.find(symb);
	      if (elt == variablesMap.end())
		{
		  std::pair<SgVariableSymbol*, int>
		    newelt(symb, variablesMap.size());
		  variablesMap.insert(newelt);
		  elt = variablesMap.find(symb);
		}

	      // Get the varref/symbol, and count the dimensionality of the
	      // array.
	      aref = isSgPntrArrRefExp(*i);
	      ROSE_ASSERT(aref);
	      int pos = 0;
	      while (aref->get_lhs_operand() &&
		     ! (isSgVarRefExp(aref->get_lhs_operand()) ||
			isSgDotExp(aref->get_lhs_operand())))
		{
		  aref = isSgPntrArrRefExp(aref->get_lhs_operand());
		  pos++;
		  ROSE_ASSERT(aref);
		}

	      bool first = true;
	      while (aref)
		{
		  SgNode* exp = aref->get_rhs_operand();
		  scoplib_matrix_p tmp;
		  int is_affine_dim = true;
		  // Retrieve if the array dimension is affine for the scop.
		  if (_polyoptions.getScApproximateExtractor())
		    {
		      std::set<std::pair<SgVariableSymbol*,
					 std::vector<bool> > >::iterator it;

		      for (it = _arraysAsScalars.begin();
			   it != _arraysAsScalars.end(); ++it)
			if (it->first == symb)
			  {
			    is_affine_dim = it->second[pos];
			    break;
			  }
		    }
		  if (_polyoptions.getScF2CExtractor())
		    tmp = createMatrixFromF2CExpression
		      (isSgExpression(exp), iteratorsMap, parametersMap);
		  else
		    {
		      if (is_affine_dim)
			tmp = createMatrix(isSgExpression(exp),
					   iteratorsMap, parametersMap);
		      else
			// The dimension is not affine, approximate to '0'
			// the index function for all references to this
			// array dim in the scop.
			tmp = scoplib_matrix_malloc
			  (1, iteratorsMap.size() + parametersMap.size()+2);
		    }
		  ROSE_ASSERT(tmp->NbColumns > 0 && tmp->NbRows > 0);
		  if (first)
		    {
		      // Variables are 1-indexed.
		      /// DEBUG:
		      SCOPVAL_set_si(tmp->p[0][0], elt->second + 1);
		      first = false;
		    }
		  else
		    SCOPVAL_set_si(tmp->p[0][0], 0);
		  scoplib_matrix_p c = scoplib_matrix_concat(res, tmp);
		  scoplib_matrix_free(res);
		  scoplib_matrix_free(tmp);
		  res = c;
		  aref = isSgPntrArrRefExp(aref->get_parent());
		  --pos;
		}
	      // Store the original reference pointer.
	      if (isReadRef == READ_REFERENCES)
		annotStmt->readAffineRefs.push_back(*i);
	      else
		annotStmt->writeAffineRefs.push_back(*i);
	    }
	}
    }

  return res;
}




/******************************************************************************/
/************************* (Pre-)Processing functions *************************/
/******************************************************************************/


/**
 * Annotate the AST to reflect the schedule. Simple DFS visit, as
 * scoplib_scop_normalize_schedule will be invoked later.
 *
 */
void
AffineConverter::buildScopAstOrdering(SgNode* root)
{
  class ASTIdVisitor : public AstPrePostOrderTraversal {
  public:
    virtual void preOrderVisit(SgNode* node) {};
    virtual void postOrderVisit(SgNode* node) {
      if (isSgForStatement(node))
	{
	  ScopForAnnotation* annot =
	    (ScopForAnnotation*)((node)->getAttribute("ScopFor"));
	  if (annot)
	    annot->astId = idx++;
	}
      else if (isSgStatement(node))
	{
	  ScopStatementAnnotation* annot =
	    (ScopStatementAnnotation*)((node)->getAttribute("ScopStmt"));
	  if (annot)
	    annot->astId = idx++;
	}

    }
    int idx;
  };

  ASTIdVisitor visitor;
  visitor.idx = 0;
  visitor.traverse(root);
}


/**
 * Convenience for the handling of fake iterators (used for statements
 * not surrounded by any loop: they are represented as surrounded by a
 * 'fake' loop iterating once.
 *
 *
 */
std::map<SgVariableSymbol*, int>&
AffineConverter::getFakeControlIteratorMap()
{
  if (_fakeControlMap.size() == 0)
    {
      SgVariableSymbol* symb = new SgVariableSymbol();
      std::pair<SgVariableSymbol*, int> newelt(symb, 0);
      _fakeControlMap.insert(newelt);
    }

  return _fakeControlMap;
}


/**
 * Retrieve the control node associated with the fake iterator.
 *
 *
 */
SgNode*
AffineConverter::getFakeControlNode()
{
  return NULL;
}


/**
 * Create a new matrix from a given matrix, and possibly extend the
 * columns for iterators and parameters if needed, given nbIters and
 * nbParams.
 *
 */
scoplib_matrix_p
AffineConverter::buildAdjustedMatrixIterators(scoplib_matrix_p mat,
					      int nbIters, int nbParams)
{
  if (mat->NbColumns == nbIters + nbParams + 2)
    return scoplib_matrix_copy(mat);
  scoplib_matrix_p res =
    scoplib_matrix_malloc(mat->NbRows, nbIters + nbParams + 2);
  int i, j, k;
  for (i = 0; i < mat->NbRows; ++i)
    {
      for (j = 0; j <= mat->NbColumns - nbParams - 2; ++j)
	{
	  /// DEBUG:
	  ROSE_ASSERT(i < res->NbRows && i < mat->NbRows);
	  ROSE_ASSERT(j < res->NbColumns && j < mat->NbColumns);

	  SCOPVAL_assign(res->p[i][j], mat->p[i][j]);
	}
      for (k = 0; k < nbParams + 1; ++k)
	{
	  /// DEBUG:
	  ROSE_ASSERT(i < res->NbRows && i < mat->NbRows);
	  ROSE_ASSERT(k + nbIters + 1 < res->NbColumns && k + j < mat->NbColumns);

	  SCOPVAL_assign(res->p[i][k + nbIters + 1], mat->p[i][k + j]);
	}
    }

  return res;
}



/**
 *
 * Create the parameter list for a node. All variables that are used
 * in affine expressions (array subscript / control) and that are not
 * loop iterators are parameters.
 *
 */
void
AffineConverter::updateParametersMap(SgNode* s,
				     std::vector<SgNode*>& control,
				     std::map<SgVariableSymbol*, int>&
				     parametersMap)
{
  // 1- Get the loop iterators for the statement.
  std::map<SgVariableSymbol*, int> iteratorsMap =
    getLoopIteratorsMap(control);

  // 2- Any variable in the control stack that is not a loop iterator
  // is a parameter.
  std::vector<SgNode*> readRefs;
  std::vector<SgNode*> writeRefs;
  std::vector<SgNode*>::const_iterator i;
  for (i = control.begin(); i != control.end(); ++i)
    {
      SgForStatement* fornode = isSgForStatement(*i);
      SgIfStmt* ifnode = isSgIfStmt(*i);
      if (fornode)
	{
	  SgStatementPtrList& stmts = fornode->get_init_stmt();
	  SgStatementPtrList::const_iterator j;
	  for (j = stmts.begin(); j != stmts.end(); ++j)
	    updateParams(*j, iteratorsMap, parametersMap);
	  updateParams(fornode->get_test(), iteratorsMap, parametersMap);
	}
      else if (ifnode)
	updateParams(ifnode->get_conditional(), iteratorsMap, parametersMap);
    }

  // 3- Any variable in the subscript functions that is not a loop
  // iterator is a parameter.
  ScopStatementAnnotation* annotStmt =
    (ScopStatementAnnotation*)s->getAttribute("ScopStmt");
  ROSE_ASSERT(annotStmt);
  readRefs = annotStmt->readRefs;
  writeRefs = annotStmt->writeRefs;

  std::vector<SgNode*>& refs = readRefs;
  int cpt;
  for (cpt = 0; cpt < 2; refs = writeRefs, ++cpt)
    for (i = refs.begin(); i != refs.end(); ++i)
      {
	// Find the array references.
	SgPntrArrRefExp* aref = isSgPntrArrRefExp(*i);
	while (aref)
	  {
	    // Get all symbols read in subscript expressions.
	    SgNode* n = aref->get_rhs_operand();
	    updateParams(aref->get_rhs_operand(), iteratorsMap, parametersMap);

	    aref = isSgPntrArrRefExp(aref->get_lhs_operand());
	  }
      }
}


/**
 * Internal helper.
 *
 */
void
AffineConverter::updateParams(SgNode* node,
			      std::map<SgVariableSymbol*, int>& iteratorsMap,
			      std::map<SgVariableSymbol*, int>& parametersMap)
{

  // Internal helper:
  // Get all symbols referenced in a given expression
  struct GetVarSymbsUsedInExpressionVisitor : public AstSimpleProcessing {
    virtual void visit(SgNode* n) {
      if (isSgVarRefExp(n) && ! isSgDotExp(n->get_parent()))
	refs.insert(isSgVarRefExp(n)->get_symbol());
      else if (isSgDotExp(n))
	{
	  SgVariableSymbol* symb =
	    SageTools::getFakeSymbol(isSgDotExp(n), ac->_fakeSymbolMap);
	  refs.insert(symb);
	}
    }
    std::set<SgVariableSymbol*> refs;
    AffineConverter* ac;
  };

  GetVarSymbsUsedInExpressionVisitor v;
  v.ac = this;
  v.traverse(node, preorder);

  std::set<SgVariableSymbol*>::const_iterator i;
  for (i = v.refs.begin(); i != v.refs.end(); ++i)
    {
      if (iteratorsMap.find(*i) == iteratorsMap.end() &&
	  parametersMap.find(*i) == parametersMap.end())
	{
	  std::pair<SgVariableSymbol*, int> newelt(*i, parametersMap.size());
	  parametersMap.insert(newelt);
	}
    }
}


/**
 * Get the list of loop iterators in a given control stack.
 *
 *
 */
std::map<SgVariableSymbol*, int>
AffineConverter::getLoopIteratorsMap(std::vector<SgNode*>& controlStack)
{
  std::map<SgVariableSymbol*, int> loopIterators;

  std::vector<SgNode*>::const_iterator i;
  int cnt = 0;

  // Count the number of for loops in the control stack.
  int depth = 0;
  for (i = controlStack.begin(); i != controlStack.end(); ++i)
    if (isSgForStatement(*i))
      ++depth;
  // Traverse the control stack, and associate loop iterators to their
  // depth (0-indexed).
  for (i = controlStack.begin(); i != controlStack.end(); ++i)
    {
      SgForStatement* fornode = isSgForStatement(*i);
      if (fornode)
	{
	    SgExpression* increment = fornode->get_increment();
	    std::vector<SgNode*> symb =
	      NodeQuery::querySubTree(increment, V_SgVarRefExp);
	    ROSE_ASSERT(symb.size() == 1);
	    SgVarRefExp* ref = isSgVarRefExp(*(symb.begin()));
	    SgVariableSymbol* symbol;
	    SgDotExp* dexp = isSgDotExp(ref->get_parent());
	    if (dexp)
	      symbol = SageTools::getFakeSymbol(dexp, _fakeSymbolMap);
	    else
	      symbol = ref->get_symbol();
	    std::pair<SgVariableSymbol*, int > newelt(symbol, depth - ++cnt);
	    loopIterators.insert(newelt);
	}
    }

  return loopIterators;
}





/******************************************************************************/
/************************ Affine expression processing ************************/
/******************************************************************************/

/**
 * Create a matrix corresponding to an arbitrary expression.
 *
 */
scoplib_matrix_p
AffineConverter::createMatrix(SgExpression* expr,
			      std::map<SgVariableSymbol*, int>&
			      iteratorsMap,
			      std::map<SgVariableSymbol*, int>&
			      parametersMap)
{
  return parseExpression(expr, iteratorsMap, parametersMap);
}


/**
 * Create a matrix corresponding to a linearized array subscript
 * function as generated by F2C (note: ad-hoc parsing).
 *
 */
scoplib_matrix_p
AffineConverter::createMatrixFromF2CExpression(SgExpression* expr,
					       std::map<SgVariableSymbol*, int>&
					       iteratorsMap,
					       std::map<SgVariableSymbol*, int>&
					       parametersMap)
{
  return parseExpression(expr, iteratorsMap, parametersMap);
}




/**
 * Generic expression parser.
 *
 * Parse conditionals and access functions.
 *
 *
 */
scoplib_matrix_p
AffineConverter::parseExpression(SgExpression* expr,
				 std::map<SgVariableSymbol*, int>&
				 iteratorsMap,
				 std::map<SgVariableSymbol*, int>&
				 parametersMap)
{
  // Cases:

  // conjunction: a && b
  SgAndOp* andop = isSgAndOp(expr);
  if (andop)
    {
      scoplib_matrix_p mat1 =
      parseExpression(andop->get_lhs_operand(),
		      iteratorsMap, parametersMap);
      scoplib_matrix_p mat2 =
      parseExpression(andop->get_rhs_operand(),
		      iteratorsMap, parametersMap);
      scoplib_matrix_p mat = scoplib_matrix_concat(mat1, mat2);
      scoplib_matrix_free(mat1);
      scoplib_matrix_free(mat2);

      return mat;
    }
  else
    {
      SgBinaryOp* binop = isSgBinaryOp(expr);
      scoplib_matrix_p mat = NULL;
      if (binop && (SageTools::isCompareOp(expr) || isSgAssignOp(expr)))
	{
	  SgExpression* lhs = binop->get_lhs_operand();
	  SgExpression* rhs = binop->get_rhs_operand();;
	  if (isSgGreaterThanOp(expr))
	    mat =
	      parseBinExpr(lhs, 1, rhs, -1, -1, iteratorsMap, parametersMap);
	  else if (isSgLessThanOp(expr))
	    mat =
	      parseBinExpr(lhs, -1, rhs, 1, -1, iteratorsMap, parametersMap);
	  else if (isSgGreaterOrEqualOp(expr))
	    mat =
	      parseBinExpr(lhs, 1, rhs, -1, 0, iteratorsMap, parametersMap);
	  else if (isSgLessOrEqualOp(expr))
	    mat =
	      parseBinExpr(lhs, -1, rhs, 1, 0, iteratorsMap, parametersMap);
	  else if (isSgEqualityOp(expr))
	    {
	      scoplib_matrix_p mat1 =
		parseBinExpr(lhs, -1, rhs, 1, 0, iteratorsMap, parametersMap);
	      scoplib_matrix_p mat2 =
		parseBinExpr(lhs, 1, rhs, -1, 0, iteratorsMap, parametersMap);
	      mat = scoplib_matrix_concat(mat1, mat2);
	      scoplib_matrix_free(mat1);
	      scoplib_matrix_free(mat2);
	    }
	  else if (isSgAssignOp(expr))
	    mat = parseBinExpr(lhs, 1, rhs, -1, 0, iteratorsMap, parametersMap);
	  else
	    {
	      std::cerr << "ERROR: Unkown binop" << std::endl;
	      exit(1);
	    }
	}
      else
	{
	  mat =
	    parseBinExpr(expr, 1, NULL, 0, 0, iteratorsMap, parametersMap);
	}
      return mat;
    }

  // Never reached.
  return NULL;
}


/**
 * Parse a demangled binary expression, eg a '<' b. To conform scoplib
 * representation, a < b is represented as b - a - 1 >= 0. So, a is
 * given a mult of -1, b a mult of 1, and the offset is -1.
 *
 */
scoplib_matrix_p
AffineConverter::parseBinExpr(SgExpression* lhs,
			      int lhsmult,
			      SgExpression* rhs,
			      int rhsmult,
			      int offset,
			      std::map<SgVariableSymbol*, int>&
			      iteratorsMap,
			      std::map<SgVariableSymbol*, int>&
			      parametersMap)
{
  SgExpression* mlhs = NULL;
  SgExpression* mrhs = NULL;
  // Most restrictive pattern: ceil/floor
  if (parseCeilFloorExpression(lhs, &mlhs, &mrhs))
    {
      SgIntVal* val = isSgIntVal(mrhs);
      ROSE_ASSERT(val);
      rhsmult *= val->get_value();
      scoplib_matrix_p res =
	parseLinearExpression(mlhs, lhsmult, rhs, rhsmult, offset,
			      iteratorsMap, parametersMap);
      return res;
    }
  else if (parseCeilFloorExpression(rhs, &mlhs, &mrhs))
    {
      SgIntVal* val = isSgIntVal(mrhs);
      ROSE_ASSERT(val);
      lhsmult *= val->get_value();
      scoplib_matrix_p res =
	parseBinExpr(lhs, lhsmult, mlhs, rhsmult, offset,
		     iteratorsMap, parametersMap);
      return res;
    }
  // Less restrictive pattern: min/max
  else if (parseMinMaxExpression(lhs, &mlhs, &mrhs))
    {
      scoplib_matrix_p mat1 = parseBinExpr(mlhs, lhsmult,
					   rhs, rhsmult,
					   offset,
					   iteratorsMap, parametersMap);
      scoplib_matrix_p mat2 = parseBinExpr(mrhs, lhsmult,
					   rhs, rhsmult,
					   offset,
					   iteratorsMap, parametersMap);
      scoplib_matrix_p res = scoplib_matrix_concat(mat1, mat2);
      scoplib_matrix_free(mat1);
      scoplib_matrix_free(mat2);
      return res;
    }
  else if (parseMinMaxExpression(rhs, &mlhs, &mrhs))
    {
      scoplib_matrix_p mat1 = parseBinExpr(lhs, lhsmult,
					   mlhs, rhsmult,
					   offset,
					   iteratorsMap, parametersMap);
      scoplib_matrix_p mat2 = parseBinExpr(lhs, lhsmult,
					   mrhs, rhsmult,
					   offset,
					   iteratorsMap, parametersMap);
      scoplib_matrix_p res = scoplib_matrix_concat(mat1, mat2);
      scoplib_matrix_free(mat1);
      scoplib_matrix_free(mat2);
      return res;
    }
  // No pattern, simple affine expression.
  else
    {
      scoplib_matrix_p res =
	parseLinearExpression(lhs, lhsmult, rhs, rhsmult, offset,
			      iteratorsMap, parametersMap);
      return res;
    }

  return NULL;
}


/**
 * Check if the expression is a ceil/floor expression.
 *
 */
bool
AffineConverter::parseCeilFloorExpression(SgExpression* e,
					  SgExpression** lhs,
					  SgExpression** rhs)
{
  SgConditionalExp* cexp = isSgConditionalExp(e);
  if (cexp)
    {
      /// FIXME: UGLY: redo the parsing, must find a better solution...
      if (SageTools::isFloorFuncCall(e) || SageTools::isCeilFuncCall(e))
	{
	  SgLessThanOp* lop = isSgLessThanOp(cexp->get_conditional_exp());
	  // Parse ceil/floor.
	  if (lop)
	    {
	      SgMultiplyOp* mop = isSgMultiplyOp(lop->get_lhs_operand());
	      if (mop)
		{
		  *lhs = mop->get_lhs_operand();
		  *rhs = mop->get_rhs_operand();
		  return true;
		}
	    }
	}
    }

  return false;
}


/**
 * Check if the expression is a min/max expression.
 *
 */
bool
AffineConverter::parseMinMaxExpression(SgExpression* e,
				       SgExpression** lhs, SgExpression** rhs)
{
  SgConditionalExp* cexp = isSgConditionalExp(e);
  if (cexp)
    {
      *lhs = cexp->get_true_exp();
      *rhs = cexp->get_false_exp();
      return true;
    }

  SgFunctionCallExp* fe = isSgFunctionCallExp(e);
  if (fe)
    {
      SgExprListExp* args = fe->get_args();
      SgExpressionPtrList l = args->get_expressions();
      ROSE_ASSERT (l.size() == 2);
      *lhs = l[0];
      *rhs = l[1];
      return true;
    }

  return false;
}



/**
 * Parse a linear affine expression, made of a lhs and a rhs (possibly
 * NULL). return the scoplib_matrix representation (1 row always).
 *
 */
scoplib_matrix_p
AffineConverter::parseLinearExpression(SgExpression* llhs,
				       int lhsmult,
				       SgExpression* lrhs,
				       int rhsmult,
				       int offset,
				       std::map<SgVariableSymbol*, int>&
				       iteratorsMap,
				       std::map<SgVariableSymbol*, int>&
				       parametersMap)
{
  int nbIter = iteratorsMap.size();
  int nbPar = parametersMap.size();

  scoplib_matrix_p res = scoplib_matrix_malloc(1, nbIter + nbPar + 2);
  SCOPVAL_set_si(res->p[0][0], 1);

  convertLinearExpression(llhs, lhsmult, iteratorsMap, parametersMap, res);
  convertLinearExpression(lrhs, rhsmult, iteratorsMap, parametersMap, res);
  if (offset != 0)
    {
      scoplib_int_t val; SCOPVAL_init(val); SCOPVAL_set_si(val, offset);
      /// DEBUG:
      ROSE_ASSERT(nbIter + nbPar + 1 < res->NbColumns);
      SCOPVAL_addto(res->p[0][nbIter + nbPar + 1],
		    res->p[0][nbIter + nbPar + 1], val);
      SCOPVAL_clear(val);
    }

  return res;
}


/**
 * Internal Helper. Compute the coefficient associated to a symbol in
 * an affine expression.
 *
 */
scoplib_int_t
AffineConverter::computeCoefficient(SgNode* e,
				    SgVariableSymbol* symb)
{
  scoplib_int_t res;
  SCOPVAL_init(res);

  SgBinaryOp* bop = isSgBinaryOp(e);
  SgUnaryAddOp* aop = isSgUnaryAddOp(e);
  SgMinusOp* mop = isSgMinusOp(e);

  if (SageTools::isIntegerTypeValue(e))
    {
      long int value = SageTools::getIntegerTypeValue(e);
      SCOPVAL_set_si(res, value);
    }
  else if (isSgVarRefExp(e) || isSgDotExp(e))
    {
      SgVariableSymbol* symbol =
	SageTools::getSymbolFromReference(e, _fakeSymbolMap);
      if (symbol && symbol == symb)
	SCOPVAL_set_si(res, 1);
      else
	SCOPVAL_set_si(res, 0);
    }
  else if (bop)
    {
      scoplib_int_t lhs = computeCoefficient(bop->get_lhs_operand(), symb);
      scoplib_int_t rhs = computeCoefficient(bop->get_rhs_operand(), symb);
      if (isSgAddOp(e))
	SCOPVAL_addto(res, lhs, rhs);
      else if (isSgSubtractOp(e))
	SCOPVAL_subtract(res, lhs, rhs);
      else if (isSgMultiplyOp(e))
	SCOPVAL_multo(res, lhs, rhs);
      else
	ROSE_ASSERT(0);
      SCOPVAL_clear(lhs);
      SCOPVAL_clear(rhs);
    }
  else if (aop)
    {
      scoplib_int_t val = computeCoefficient(aop->get_operand_i(), symb);
      SCOPVAL_assign(res, val);
    }
  else if (mop)
    {
      scoplib_int_t val = computeCoefficient(mop->get_operand_i(), symb);
      scoplib_int_t mone; SCOPVAL_init(mone); SCOPVAL_set_si(mone, -1);
      SCOPVAL_assign(res, val);
      SCOPVAL_multo(res, res, mone);
      SCOPVAL_clear(mone);
    }

  return res;
}


/**
 * Helper.
 *
 */
bool
AffineConverter::convertLinearExpression(SgExpression* e,
					 int mult,
					 std::map<SgVariableSymbol*, int>&
					 iteratorsMap,
					 std::map<SgVariableSymbol*, int>&
					 parametersMap,
					 scoplib_matrix_p matrix)
{
  if (e == NULL)
    return true;
  scoplib_int_t zmult;
  SCOPVAL_init(zmult);
  SCOPVAL_set_si(zmult, mult);
  scoplib_int_t coeff;
  SCOPVAL_init(coeff);

  // 1- Compute the coefficient of the scalar constant.
  scoplib_int_t scalar;
  SCOPVAL_init(scalar);
  scalar = computeCoefficient(e, NULL);
  SCOPVAL_multo(coeff, scalar, mult);
  /// DEBUG:
  ROSE_ASSERT(matrix->NbRows > 0);
  SCOPVAL_addto(matrix->p[0][matrix->NbColumns - 1],
		matrix->p[0][matrix->NbColumns - 1], coeff);

  // 2- Iterate on all iterator symbols.
  std::map<SgVariableSymbol*, int>::const_iterator i;
  for (i = iteratorsMap.begin(); i != iteratorsMap.end(); ++i)
    {
      /// DEBUG:
      ROSE_ASSERT(1 + i->second < matrix->NbColumns);
      coeff = computeCoefficient(e, i->first);
      SCOPVAL_subtract(coeff, coeff, scalar);
      SCOPVAL_multo(coeff, coeff, zmult);
      SCOPVAL_addto(matrix->p[0][1 + i->second],
		    matrix->p[0][1 + i->second], coeff);
    }

  // 3- Iterate on all parameter symbols.
  int nbIters = iteratorsMap.size();
  for (i = parametersMap.begin(); i != parametersMap.end(); ++i)
    {
      /// DEBUG:
      ROSE_ASSERT(1 + nbIters + i->second < matrix->NbColumns);
      coeff = computeCoefficient(e, i->first);
      SCOPVAL_subtract(coeff, coeff, scalar);
      SCOPVAL_multo(coeff, coeff, zmult);
      SCOPVAL_addto(matrix->p[0][1 + nbIters + i->second],
		    matrix->p[0][1 + nbIters + i->second], coeff);
    }

  SCOPVAL_clear(coeff);
  SCOPVAL_clear(zmult);
  SCOPVAL_clear(scalar);

  return true;
}

