/*
 * ClastToSage.cpp: This file is part of the PolyOpt project.
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
 * @file: ClastToSage.cpp
 * @author: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 */

#include <fstream>

#include <polyopt/ClastToSage.hpp>
#include <polyopt/SageNodeAnnotation.hpp>
#include <polyopt/SageTools.hpp>

#ifdef CLOOG_INT_GMP
# define CLOOG_GET_SI(x) mpz_get_si(x)
#else
# define CLOOG_GET_SI(x) x
#endif


// This is baaaaad but the Clast header file is to blame.
enum { expr_name, expr_term, expr_bin, expr_red };


static
void traverseClast (struct clast_stmt* s, std::vector<const char*>& its) {
  // Traverse the clast.
  for ( ; s; s = s->next)
    {
      if (CLAST_STMT_IS_A(s, stmt_for) || CLAST_STMT_IS_A(s, stmt_parfor) ||
	  CLAST_STMT_IS_A(s, stmt_vectorfor))
	{
	  struct clast_for* f = (struct clast_for*) s;
	  std::vector<const char*>::const_iterator i;
	  for (i = its.begin(); i != its.end(); ++i)
	    if (*i == f->iterator)
	      break;
	  if (i == its.end())
	    its.push_back(f->iterator);
	  traverseClast (((struct clast_for*)s)->body, its);
	}
      else if (CLAST_STMT_IS_A(s, stmt_guard))
	traverseClast (((struct clast_guard*)s)->then, its);
      else if (CLAST_STMT_IS_A(s, stmt_block))
	traverseClast (((struct clast_block*)s)->body, its);
    }
}


std::vector<const char*>
ClastToSage::collectAllIterators(clast_stmt* root) {
  std::vector<const char*> res;

  traverseClast(root, res);

  return res;
}


std::map<const char*, SgVariableDeclaration*>
ClastToSage::createNewIterators(std::vector<const char*> vars,
				SgScopeStatement* scope) {
  std::map<const char*, SgVariableDeclaration*> res;

  std::vector<const char*>::const_iterator i;
  for (i = vars.begin(); i != vars.end(); ++i)
    {
      /// FIXME: Ensure *i is not already in the symbol table.
      SgVariableDeclaration* decl =
	SageBuilder::buildVariableDeclaration(std::string(*i),
					      SageBuilder::buildIntType(),
					      NULL, scope);
      std::pair<const char*, SgVariableDeclaration*> newelt(*i, decl);
      res.insert(newelt);
    }

  return res;
}


bool
ClastToSage::isIterator(const char* ptr) {
  std::vector<const char*>::const_iterator i;
  for (i = _scoplibIterators.begin(); i != _scoplibIterators.end(); ++i)
    if (ptr == *i)
      return true;

  return false;
}



bool
ClastToSage::insertPragmas(SgNode* root)
{
  // 1- Collect the list of outer-parallel loop iterators specified
  // by pluto, as given by the .pragmas file.  Grammar is: 1 line per
  // iterator, "ITER \SPACE PRAGMA STRING \ENDLNE"
  std::ifstream plutofile;
  plutofile.open(".pragmas");
  std::vector<std::string> ompLoopIterators;
  if (plutofile)
    {
      std::string iter;
      char junk[256];
      while (plutofile >> iter)
	{
	  ompLoopIterators.push_back(iter);
	  plutofile.getline(junk, 256);
	}
      plutofile.close();
    }

  // 2- Collect the list of inner-parallel loop iterators specified
  // by pluto, as given by the .vectorize file.  Grammar is: 1 line per
  // iterator, "ITER\ENDLINE"
  plutofile.open(".vectorize");
  std::vector<std::string> simdLoopIterators;
  if (plutofile)
    {
      std::string iter;
      char junk[256];
      while (plutofile >> iter)
	{
	  simdLoopIterators.push_back(iter);
	  plutofile.getline(junk, 256);
	}
      plutofile.close();
    }


  // 3- Collect all for loops.
  std::vector<SgNode*> forLoops =
    NodeQuery::querySubTree(root, V_SgForStatement);

  std::set<std::string>::const_iterator i;
  std::set<std::string> allIterators;
  allIterators.insert(ompLoopIterators.begin(), ompLoopIterators.end());
  allIterators.insert(simdLoopIterators.begin(), simdLoopIterators.end());
  // 4- Iterate on all dimensions to parallelize.
  for (i = allIterators.begin(); i != allIterators.end(); ++i)
    {
      SgName iterName(*i);
      SgSymbol* iterSymb = isSgScopeStatement(root)->lookup_symbol(iterName);

      if (iterSymb == NULL)
	{
	  // The loop iterator symbol does not exist. Typical case
	  // where the tile size exceeds the iteration domain size:
	  // there is only one parallel iteration, thus no loop,
	  // thus no iterator. Safely proceed to the next loop iterator.
	  continue;
	}

      std::vector<SgNode*>::const_iterator j;
      for (j = forLoops.begin(); j != forLoops.end(); ++j)
	{
	  SgForStatement* fornode = isSgForStatement(*j);
	  ROSE_ASSERT(fornode);
	  // Get the loop iterator.
	  SgVariableSymbol* forSymb =
	    SageTools::getLoopIteratorSymbol(fornode);
	  ROSE_ASSERT(forSymb);

	  if (forSymb == iterSymb)
	    {
	      std::string pragmaClause;

	      if (std::find(simdLoopIterators.begin(),
			    simdLoopIterators.end(),*i)
		  != simdLoopIterators.end())
		{
		  // This is a SIMDizable loop.
		  // For a vectorizable and parallelizable dimension,
		  // vectorization has precedence.
		  pragmaClause = "#pragma ivdep\n#pragma vector always";
		}
	      else
		if (std::find(ompLoopIterators.begin(), ompLoopIterators.end(),
			      *i) != ompLoopIterators.end())
		  {
		    // This is an OpenMP parallelizable loop.

		    // Collect all loop iterator names in the enclosed loops
		    std::vector<SgNode*> innerLoops =
		      NodeQuery::querySubTree(fornode->get_loop_body(),
					      V_SgForStatement);

		    // Create the pragma clause.
		    pragmaClause = "#pragma omp parallel for";
		    bool first = true;
		    std::vector<SgNode*>::const_iterator k;
		    std::set<SgVariableSymbol*> loopSymbols;
		    for (k = innerLoops.begin(); k != innerLoops.end(); ++k)
		      loopSymbols.insert(SageTools::getLoopIteratorSymbol
					 (isSgForStatement(*k)));
		    if (loopSymbols.size() > 0)
		      pragmaClause = pragmaClause + " private(";
		    std::set<SgVariableSymbol*>::const_iterator l;
		    for (l = loopSymbols.begin(); l != loopSymbols.end(); ++l)
		      {
			std::string iterName = (*l)->get_name().getString();
			if (first)
			  {
			    pragmaClause = pragmaClause + iterName;
			    first = false;
			  }
			else
			  pragmaClause = pragmaClause + ", " + iterName;

		      }
		    if (loopSymbols.size() > 0)
		      pragmaClause = pragmaClause + ")";
		  }
		else
		  {
		    // Should never occur.
		    ROSE_ASSERT(0);
		  }
	      // Annotate the for node with the pragma clause.
	      SageInterface::attachArbitraryText(fornode, pragmaClause,
						 PreprocessingInfo::before);

	      // Put loop lower bound and upper bound outside the loop
	      // (OpenMp does not like complex loop bounds).
	      /// LNP: FIXME: do it if it becomes needed.
	    }


	}
    }

  return true;

}

ClastToSage::ClastToSage(SgScopeStatement* scopScope,
			 clast_stmt* root,
			 scoplib_scop_p scoplibScop,
			 PolyRoseOptions& options)
{
  //SgScopeStatement* scope = isSgScopeStatement((SgNode*) scoplibScop->usr);
  _polyoptions = options;
  SgScopeStatement* scope = scopScope;
  ROSE_ASSERT(scope);
  m_scopRoot = NULL;
  /// OLD:
  m_scope = scope;
  m_scoplib_scop = scoplibScop;
  m_verbose = _polyoptions.isVerbose();

  // 0- Retrive meta information stored as an annotation of the
  // SageAST root.
  SgStatement* scopRoot = isSgStatement((SgNode*)(scoplibScop->usr));
  ScopRootAnnotation* annot =
    (ScopRootAnnotation*)(scopRoot->getAttribute("ScopRoot"));
  ROSE_ASSERT(annot);
  _fakeSymbolMap = annot->fakeSymbolMap;

  // 1- Collect all iterators in the clast. They are of the from 'cXX'
  // where XX is an integer.
  _scoplibIterators = collectAllIterators(root);

  // 2- Map clast iterator to new variables that does not conflict
  // with existing names, and register the symbols in the symbol
  // table.
  _sageIterators = createNewIterators(_scoplibIterators, scope);

  // 3- Create the basic block containing the transformed scop.
  SgBasicBlock* bb = buildBasicBlock(root);
  // 4- Update the variable scope with the new bb, and insert the
  // declaration statement in the AST.
  std::map<const char*, SgVariableDeclaration*>::iterator iter;
  Rose_STL_Container<SgNode*> varRefs =
    NodeQuery::querySubTree(bb,V_SgVarRefExp);
  for(iter = _sageIterators.begin(); iter != _sageIterators.end(); ++iter)
    {
      // Deal with the symbol tables.
      SgInitializedNamePtrList& l = iter->second->get_variables();
      for (SgInitializedNamePtrList::iterator i = l.begin(); i != l.end(); i++)
	{
	  (*i)->set_scope(bb);
	  SgVariableSymbol* sym = new SgVariableSymbol(*i);
          bb->insert_symbol((*i)->get_name(), sym);
	  // Ugly hack: replace the old symbol with the new entry in
	  // the symbol table.
	  for (Rose_STL_Container<SgNode *>::iterator j = varRefs.begin();
	       j != varRefs.end(); j++)
	    {
	      SgVarRefExp *vRef = isSgVarRefExp((*j));
	      if (vRef->get_symbol()->get_name() == (*i)->get_name())
		vRef->set_symbol(sym);
	    }

	}
      // Insert the declaration statement in the BB.
      bb->prepend_statement(iter->second);
    }

  // Post-process for pragma insertion.
  if (options.getGeneratePragmas())
    insertPragmas(bb);

  m_scopRoot = bb;
}


SgBasicBlock* ClastToSage::getBasicBlock()
{
  return m_scopRoot;
}

SgBasicBlock* ClastToSage::buildBasicBlock(clast_stmt* root)
{
  IF_PLUTO_ROSE_VERBOSE(std::cout << "[ClastToSage] Starting basic block" << std::endl;)

  SgBasicBlock* retBlock = SageBuilder::buildBasicBlock();

  clast_stmt* curr;
  for(curr = root; curr != NULL; curr = curr->next)
    {
      if(CLAST_STMT_IS_A(curr, stmt_root))
	{
	  /* Ignore statement roots */
	  continue;
	}
      else if(CLAST_STMT_IS_A(curr, stmt_ass))
	{
	  /* Assignment statement */
	  SgStatement* assignStmt = buildAssignment((clast_assignment*)curr);
	  retBlock->append_statement(assignStmt);
	}
      else if(CLAST_STMT_IS_A(curr, stmt_user))
	{
	  /* User statement */
	  SgStatement* userStmt = buildUserStatement((clast_user_stmt*)curr);
	  retBlock->append_statement(userStmt);
	}
      else if(CLAST_STMT_IS_A(curr, stmt_for))
	{
	  /* For statement */
	  SgStatement* forStmt = buildFor((clast_for*)curr);
	  retBlock->append_statement(forStmt);
	  updateAnnotation(forStmt, curr);
	}
      else if(CLAST_STMT_IS_A(curr, stmt_parfor))
	{
	  /* ParFor statement. For the moment it is a standard for
	     node. */
	  SgStatement* forStmt = buildFor((clast_for*)curr);

	  SageInterface::attachArbitraryText(forStmt, "#PRAGMA PARALLEL",
					     PreprocessingInfo::before);

	  retBlock->append_statement(forStmt);
	  updateAnnotation(forStmt, curr);
	  // Mark the loop as parallel.
	  if (_polyoptions.getScVerboseLevel() == PolyRoseOptions::VERBOSE_FULL)
	    {
	      std::cout << "[PolyOpt] Parallel outer loop for: " << std::endl;
	      SageTools::debugPrintNode(forStmt);
	    }
	  SageForInfo* annot = new SageForInfo();
	  annot->isParallel = 1;
	  forStmt->setAttribute(SageForInfo::name(), annot);
	}
      else if(CLAST_STMT_IS_A(curr, stmt_vectorfor))
	{
	  /* VectorFor statement. For the moment it is a standard for
	     node. */
	  SgStatement* forStmt = buildFor((clast_for*)curr);
	  retBlock->append_statement(forStmt);
	  updateAnnotation(forStmt, curr);
	}
      else if(CLAST_STMT_IS_A(curr, stmt_guard))
	{
	  /* Guard statement */
	  SgStatement* guardStmt = buildGuard((clast_guard*)curr);
	  retBlock->append_statement(guardStmt);
	  updateAnnotation(guardStmt, curr);
	}
      else if(CLAST_STMT_IS_A(curr, stmt_block))
	{
	  /* Block statement */
	  SgBasicBlock* block = buildBasicBlock(((clast_block*)curr)->body);
	  retBlock->append_statement(block);
	  updateAnnotation(block, curr);
	}
      else if(CLAST_STMT_IS_A(curr, stmt_pragma_omp_for) ||
	      CLAST_STMT_IS_A(curr, stmt_pragma_vectorize))
	annotationNodes.push_back(curr);
    }

  IF_PLUTO_ROSE_VERBOSE(std::cout << "[ClastToSage] Finished basic block" << std::endl;)

  return retBlock;
}


SgStatement* ClastToSage::buildGuard(clast_guard* guard)
{
  IF_PLUTO_ROSE_VERBOSE(std::cout << "[ClastToSage] Starting guard"
			<< std::endl;)

  SgStatement* retStmt = NULL;
  SgExpression* guardTest = NULL;

  for(int i = 0; i < guard->n; ++i)
  {
    SgExpression* guardExpr = buildEquation(&guard->eq[i]);

    if(guardTest == NULL)
      guardTest = guardExpr;
    else
      guardTest =
	SageBuilder::buildBinaryExpression<SgAndOp>(guardTest, guardExpr);
  }

  SgBasicBlock* body = buildBasicBlock(guard->then);

  retStmt = SageBuilder::buildIfStmt(guardTest, body, NULL);

  IF_PLUTO_ROSE_VERBOSE(std::cout << "[ClastToSage] Finished guard"
			<< std::endl;)

  return retStmt;
}


SgStatement* ClastToSage::buildFor(clast_for* forStmt)
{
  IF_PLUTO_ROSE_VERBOSE(std::cout << "[ClastToSage] Starting for" << std::endl;)

  SgStatement* retStmt = NULL;
  SgVarRefExp* iteratorVar =
    SageBuilder::buildVarRefExp(_sageIterators.find(forStmt->iterator)->second);

  SgExprStatement* initAssignStmt = NULL;
  SgExprStatement* testStmt = NULL;
  SgExpression* incrExpr = NULL;

  if(forStmt->LB != NULL)
    {
      initAssignStmt =
	SageBuilder::buildAssignStatement(iteratorVar,
					  buildExpression(forStmt->LB));
      iteratorVar =
	SageBuilder::buildVarRefExp
	(_sageIterators.find(forStmt->iterator)->second);
    }

  if(forStmt->UB != NULL)
    {
      testStmt =
	SageBuilder::buildExprStatement
	(SageBuilder::buildBinaryExpression<SgLessOrEqualOp>
	 (iteratorVar, buildExpression(forStmt->UB)));
      iteratorVar =
	SageBuilder::buildVarRefExp
	(_sageIterators.find(forStmt->iterator)->second);
    }
  else
    {
      // ROSE does not support empty test part in a for loop. Fake
      // infinite loop with always-true conditional.
      testStmt = SageBuilder::buildExprStatement(SageBuilder::buildIntVal(1));
    }

  if(cloog_int_gt_si(forStmt->stride, 1))
    incrExpr = SageBuilder::buildBinaryExpression<SgPlusAssignOp>
      (iteratorVar, SageBuilder::buildIntVal(CLOOG_GET_SI(forStmt->stride)));
  else
    incrExpr = SageBuilder::buildUnaryExpression<SgPlusPlusOp>(iteratorVar);

  SgStatement* body = buildBasicBlock(forStmt->body);
  //  SageInterface::rebuildSymbolTable(isSgBasicBlock(body));

  retStmt =
    SageBuilder::buildForStatement(initAssignStmt, testStmt, incrExpr, body);


  IF_PLUTO_ROSE_VERBOSE(std::cout << "[ClastToSage] Finished for" << std::endl;)

  return retStmt;
}

SgStatement* ClastToSage::buildAssignment(clast_assignment* assignment)
{
  IF_PLUTO_ROSE_VERBOSE(std::cout << "[ClastToSage] Starting assignment" << std::endl;)

  SgStatement* retStmt = NULL;
  const char* LHS = assignment->LHS;

  if (LHS)
    {
      ROSE_ASSERT (isIterator(LHS));
      SgVarRefExp* var =
	SageBuilder::buildVarRefExp(_sageIterators.find(LHS)->second);

      retStmt = SageBuilder::buildAssignStatement
	(var, buildExpression(assignment->RHS));
    }
  else
    retStmt = SageBuilder::buildExprStatement(buildExpression(assignment->RHS));

  IF_PLUTO_ROSE_VERBOSE(std::cout << "[ClastToSage] Finished assignment" << std::endl;)

  return retStmt;
}


SgStatement* ClastToSage::buildUserStatement(clast_user_stmt* statement)
{
  IF_PLUTO_ROSE_VERBOSE(std::cout << "[ClastToSage] Starting user statement" << std::endl;)

  SgStatement* retStmt = NULL;
  // Get a pointer to the correct statement in the scoplib
  // representation.
  int statementNumber = statement->statement->number;
  scoplib_statement_p stmt = m_scoplib_scop->statement;
  while (stmt && stmt->next && --statementNumber)
    stmt = stmt->next;
  SgTreeCopy treeCopy;

  ROSE_ASSERT(stmt);
  SgNode* origStatement = (SgNode*)(stmt->body);
  ROSE_ASSERT(origStatement);
  retStmt = isSgStatement(origStatement->copy(treeCopy));
  ROSE_ASSERT(retStmt);

  AttachedPreprocessingInfoType* attached =
    retStmt->getAttachedPreprocessingInfo();
  if(attached != NULL)
    {
      std::cout << "[ClastToSage] Warning:  SCoP statement has attached"
	" preprocessing info!  It will be cleared." << std::endl;
      std::cout << "[ClastToSage]   Offending Statement: " <<
	origStatement->unparseToCompleteString() << std::endl;
      attached->clear();
    }

  // Perform variable substitution
  // 1- Iterate on all iterators.
  for(int i = 0; i < stmt->nb_iterators; ++i)
    {
      // 2- skip fake iterators inserted by extra domain dimensions
      // during tiling. As an output of PoCC, they are labeled
      // 'NULL' in the scoplib representation.
      if (stmt->iterators[i] == NULL)
	continue;

      // 3- Proceed with the substitution of the current iterator.
      SgVariableSymbol* originalIter =
	isSgVariableSymbol((SgNode*)(stmt->iterators[i]));

      clast_stmt* subAssign = statement->substitutions;
      for(int j = 0; j < i; ++j)
	subAssign = subAssign->next;


//       IF_PLUTO_ROSE_VERBOSE(std::cout << "[ClastToSage] Substituting `" <<
// 			    originalIter->unparseToCompleteString() <<
// 			    "' with `" << newExpr->unparseToCompleteString() <<
// 			    "'." << std::endl;)


      Rose_STL_Container<SgNode*> varNodes =
	NodeQuery::querySubTree(retStmt, V_SgVarRefExp);
      Rose_STL_Container<SgNode*>::iterator varIter;
      for(varIter = varNodes.begin(); varIter != varNodes.end(); ++varIter)
	{
	  SgVarRefExp* varRefExp = isSgVarRefExp(*varIter);
	  if(varRefExp->get_symbol() == originalIter)
	    {
	      SgExpression* newExpr =
		buildExpression(((clast_assignment*)subAssign)->RHS);
	      SageInterface::replaceExpression(varRefExp, newExpr, false);
	    }
	}
    }


  IF_PLUTO_ROSE_VERBOSE(std::cout << "[ClastToSage] Finished user statement"
			<< std::endl;)

    return retStmt;
}

SgExpression* ClastToSage::buildEquation(clast_equation* equation)
{
  IF_PLUTO_ROSE_VERBOSE(std::cout << "[ClastToSage] Starting equation"
			<< std::endl;)

  SgExpression* lhs = buildExpression(equation->LHS);
  SgExpression* rhs = buildExpression(equation->RHS);
  SgExpression* retExpr = NULL;

  if(equation->sign == 0)
    retExpr = SageBuilder::buildBinaryExpression<SgEqualityOp>(lhs, rhs);
  else if(equation->sign > 0)
    retExpr = SageBuilder::buildBinaryExpression<SgGreaterOrEqualOp>(lhs, rhs);
  else
    retExpr = SageBuilder::buildBinaryExpression<SgLessOrEqualOp>(lhs, rhs);

  IF_PLUTO_ROSE_VERBOSE(std::cout << "[ClastToSage] Finished equation ("
			<< retExpr->unparseToCompleteString() << ")"
			<< std::endl;)

  return retExpr;
}

SgExpression* ClastToSage::buildExpression(clast_expr* expr)
{
  IF_PLUTO_ROSE_VERBOSE(std::cout << "[ClastToSage] Starting expression"
			<< std::endl;)

  SgExpression* retExpr = NULL;

  switch(expr->type)
  {
    case expr_name:
    {
      retExpr = buildName((clast_name*)expr);
      break;
    }
    case expr_term:
    {
      retExpr = buildTerm((clast_term*)expr);
      break;
    }
    case expr_red:
    {
      retExpr = buildReduction((clast_reduction*)expr);
      break;
    }
    case expr_bin:
    {
      retExpr = buildBinaryOp((clast_binary*)expr);
      break;
    }
  }

  IF_PLUTO_ROSE_VERBOSE(std::cout << "[ClastToSage] Finished expression (" << retExpr->unparseToCompleteString() << ")" << std::endl;)

  return retExpr;
}

SgExpression* ClastToSage::buildName(clast_name* name)
{
  IF_PLUTO_ROSE_VERBOSE(std::cout << "[ClastToSage] Starting name" << std::endl;)

  ROSE_ASSERT(name);
  ROSE_ASSERT(name->name);

  SgExpression* var;

  // Check if it is a fake symbol (eg, a.b).
  std::map<std::string, SgVariableSymbol*>::const_iterator i;
  for (i = _fakeSymbolMap.begin(); i != _fakeSymbolMap.end(); ++i)
    if (i->second == (SgNode*)(name->name))
      break;
  if (i != _fakeSymbolMap.end())
    {
      std::vector<std::string> names;
      std::string str = i->first;
      int cutAt;
      while((cutAt = str.find_first_of(".")) != str.npos)
	{
	  if(cutAt > 0)
	    names.push_back(str.substr(0, cutAt));
	  str = str.substr(cutAt + 1);
	}
      if(str.length() > 0)
	names.push_back(str);
      ROSE_ASSERT(names.size() == 2);
      SgName n1(names[0]);
      SgName n2(names[1]);
      SgSymbol* s1 = m_scope->lookup_symbol(n1);
      SgSymbol* s2 = m_scope->lookup_symbol(n2);
      SgNode* parent = m_scope->get_parent();
      SgScopeStatement* scope;
      while (! (s1 && s2))
	{
	  while (parent && ! (scope = isSgScopeStatement(parent)))
	    parent = parent->get_parent();
	  if (parent)
	    {
	      s1 = scope->lookup_symbol(n1);
	      s2 = scope->lookup_symbol(n2);
	    }
	  else
	    ROSE_ASSERT(0);
	}
      //SageBuilder::buildBinaryExpression<SgDotExp>
      return SageBuilder::buildDotExp
	(SageBuilder::buildVarRefExp(isSgVariableSymbol(s1)),
	 SageBuilder::buildVarRefExp(isSgVariableSymbol(s2)));
    }
  // If the name is an iterator, simply create a reference to its declaration.
  else if (isIterator(name->name))
    {
      var =
	SageBuilder::buildVarRefExp(_sageIterators.find(name->name)->second);
    }
  else
    // Else if the name is a parameter, create a reference using its
    // existing symbol.
    {
      ROSE_ASSERT(isSgVariableSymbol((SgNode*)(name->name)) != NULL);
      var =
	SageBuilder::buildVarRefExp(isSgVariableSymbol((SgNode*)(name->name)));
    }

  IF_PLUTO_ROSE_VERBOSE(std::cout << "[ClastToSage] Finished name  (" <<
			var->unparseToCompleteString() << ")" << std::endl;)

  return var;
}

SgExpression* ClastToSage::buildTerm(clast_term* term)
{
  IF_PLUTO_ROSE_VERBOSE(std::cout << "[ClastToSage] Starting term"
			<< std::endl;)

  SgExpression* retTerm = NULL;

  if(term->var)
  {
    retTerm = buildExpression(term->var);
    // We have a variable term.
    if(cloog_int_is_neg_one(term->val))
      retTerm = SageBuilder::buildUnaryExpression<SgMinusOp>(retTerm);
    else if(!cloog_int_is_one(term->val))
    {
      SgExpression* valExpr = SageBuilder::buildIntVal(CLOOG_GET_SI(term->val));
      retTerm =
	SageBuilder::buildBinaryExpression<SgMultiplyOp>(valExpr, retTerm);
    }
  }
  else
    // We just have a constant.
    retTerm = SageBuilder::buildIntVal(CLOOG_GET_SI(term->val));

  IF_PLUTO_ROSE_VERBOSE(std::cout << "[ClastToSage] Finished term ("
			<< retTerm->unparseToCompleteString() << ")"
			<< std::endl;)

  return retTerm;
}

SgExpression* ClastToSage::buildSum(clast_reduction* reduction)
{
  IF_PLUTO_ROSE_VERBOSE(std::cout << "[ClastToSage] Starting sum" << std::endl;)

  SgExpression* retSum = NULL;

  for(int i = 0; i < reduction->n; ++i)
  {
    clast_term* term = (clast_term*)reduction->elts[i];

    if(retSum == NULL)
      retSum = buildTerm(term);
    else
      retSum = SageBuilder::buildBinaryExpression<SgAddOp>(retSum,
							   buildTerm(term));
  }

  IF_PLUTO_ROSE_VERBOSE(std::cout << "[ClastToSage] Finished sum ("
			<< retSum->unparseToCompleteString() << ")"
			<< std::endl;)

  return retSum;
}


SgExpression* ClastToSage::buildBinaryOp(clast_binary* binary)
{
  IF_PLUTO_ROSE_VERBOSE(std::cout << "[ClastToSage] Starting binary operation"
			<< std::endl;)

  SgExpression* lhs = buildExpression(binary->LHS);
  SgExpression* rhs = SageBuilder::buildIntVal(CLOOG_GET_SI(binary->RHS));
  SgExpression* retExpr = NULL;

  switch(binary->type)
  {
    case clast_bin_fdiv:
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

      break;
    }
    case clast_bin_cdiv:
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

      break;
    }
    case clast_bin_div:
    {
      retExpr = SageBuilder::buildBinaryExpression<SgDivideOp>(lhs, rhs);
      break;
    }
    case clast_bin_mod:
    {
      retExpr = SageBuilder::buildBinaryExpression<SgModOp>(lhs, rhs);
      break;
    }
  }

  IF_PLUTO_ROSE_VERBOSE(std::cout << "[ClastToSage] Finished binary operation (" << retExpr->unparseToCompleteString() << ")" << std::endl;)

  return retExpr;
}

SgExpression* ClastToSage::buildMinMax(clast_reduction* minmax)
{

  IF_PLUTO_ROSE_VERBOSE(std::cout <<
			"[ClastToSage] Starting minmax" << std::endl;)

  SgExpression*   retExpr   = NULL;
  SgExpression*   exprA     = buildExpression(minmax->elts[0]);
  SgExpression*   exprB     = buildExpression(minmax->elts[1]);
  SgExpression*   compExpr  = NULL;

  if(minmax->type == clast_red_max)
    compExpr =
      SageBuilder::buildBinaryExpression<SgGreaterThanOp>(exprA, exprB);
  else
    compExpr = SageBuilder::buildBinaryExpression<SgLessThanOp>(exprA, exprB);
  retExpr =
    SageBuilder::buildConditionalExp(compExpr,
				     SageInterface::copyExpression(exprA),
				     SageInterface::copyExpression(exprB));


  for(int i = 2; i < minmax->n; ++i)
  {
    exprB = buildExpression(minmax->elts[i]);

    if(minmax->type == clast_red_max)
      compExpr =
	SageBuilder::buildBinaryExpression<SgGreaterThanOp>(retExpr, exprB);
    else
      compExpr =
	SageBuilder::buildBinaryExpression<SgLessThanOp>(retExpr, exprB);
    retExpr =
      SageBuilder::buildConditionalExp(compExpr,
				       SageInterface::copyExpression(retExpr),
				       SageInterface::copyExpression(exprB));
  }

  IF_PLUTO_ROSE_VERBOSE(std::cout << "[ClastToSage] Finished minmax ("
			<< retExpr->unparseToCompleteString() << ")"
			<< std::endl;)

  return retExpr;
}

SgExpression* ClastToSage::buildReduction(clast_reduction* reduction)
{
  IF_PLUTO_ROSE_VERBOSE(std::cout << "[ClastToSage] Starting reduction"
			<< std::endl;)

  SgExpression* retExpr = NULL;

  switch(reduction->type)
  {
    case clast_red_sum:
    {
      retExpr = buildSum(reduction);
      break;
    }
    case clast_red_min:
    case clast_red_max:
    {
      if(reduction->n == 1)
      {
        retExpr = buildExpression(reduction->elts[0]);
        break;
      }
      else
      {
        retExpr = buildMinMax(reduction);
        break;
      }
    }
  }

  IF_PLUTO_ROSE_VERBOSE(std::cout << "[ClastToSage] Finished reduction ("
			<< retExpr->unparseToCompleteString() << ")"
			<< std::endl;)

  return retExpr;
}


void ClastToSage::updateAnnotation(SgStatement* stmt, clast_stmt* cnode)
{
  // No annotation to put.
  if (annotationNodes.empty ())
    return;
  std::vector<clast_stmt*>::iterator i;
  for (i = annotationNodes.begin(); i != annotationNodes.end(); ++i)
    if ((*i)->next == cnode)
      break;
  // No annotation corresponding to this clast node.
  if (i == annotationNodes.end())
    return;

  if (CLAST_STMT_IS_A(*i, stmt_pragma_omp_for))
    {
      clast_pragma_omp_for* annot = (clast_pragma_omp_for*) (*i);
      SageInterface::attachArbitraryText(stmt, annot->clause,
					 PreprocessingInfo::before);
    }
  else if (CLAST_STMT_IS_A(*i, stmt_pragma_vectorize))
    {
      clast_pragma_vectorize* annot = (clast_pragma_vectorize*) (*i);
      SageInterface::attachArbitraryText(stmt, annot->clause,
					 PreprocessingInfo::before);
    }

  annotationNodes.erase(i);
}
