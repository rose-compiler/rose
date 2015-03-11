/*
 * PastToSage.cpp: This file is part of the PolyOpt project.
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
 * @file: PastToSage.cpp
 * @author: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 */

#include <fstream>

#include <polyopt/PastToSage.hpp>
#include <polyopt/SageNodeAnnotation.hpp>
#include <polyopt/SageTools.hpp>

#ifdef CLOOG_INT_GMP
# define CLOOG_GET_SI(x) mpz_get_si(x)
#else
# define CLOOG_GET_SI(x) x
#endif


#include <past/past_api.h>

static
void traversePast (s_past_node_t* s, void* args)
{
  if (past_node_is_a (s, past_for))
    {
      std::vector<void*>* its = (std::vector<void*>*)args;
      PAST_DECLARE_TYPED(for, f, s);
      std::vector<void*>::const_iterator i;
      for (i = its->begin(); i != its->end(); ++i)
	if (f->iterator &&
	    f->iterator->symbol->is_char_data &&
	    ! strcmp ((char*)*i, (char*) f->iterator->symbol->data))
	  break;
      if (i == its->end())
	its->push_back(f->iterator->symbol->data);
    }
  else if (past_node_is_a (s, past_statement))
    {
      // Special case. OTL are written as iterator = expression.
      PAST_DECLARE_TYPED(statement, ps, s);
      if (past_node_is_a (ps->body, past_assign))
	{
	  PAST_DECLARE_TYPED(binary, pb, ps->body);
	  if (past_node_is_a (pb->lhs, past_variable))
	    {
	      PAST_DECLARE_TYPED(variable, pv, pb->lhs);
	      if (pv->symbol->is_char_data &&
		  pv->symbol->data && ((char*)(pv->symbol->data))[0] == 'c')
		{
		  std::vector<void*>* iters = (std::vector<void*>*)args;
		  std::vector<void*>::const_iterator i;
		  for (i = iters->begin(); i != iters->end(); ++i)
		    if (! strcmp ((char*)*i, (char*)pv->symbol->data))
		      break;
		  if (i == iters->end())
		    iters->push_back((char*)pv->symbol->data);
		}
	    }
	}
    }
}


std::vector<void*>
PastToSage::collectAllIterators(s_past_node_t* root) {
  std::vector<void*> res;

  past_visitor (root, traversePast, (void*)&res, NULL, NULL);

  return res;
}


std::map<void*, SgVariableDeclaration*>
PastToSage::createNewIterators(std::vector<void*> vars,
				SgScopeStatement* scope) {
  std::map<void*, SgVariableDeclaration*> res;

  std::vector<void*>::const_iterator i;
  for (i = vars.begin(); i != vars.end(); ++i)
    {
      std::string availablename = getAvailableName(std::string((char*)(*i)));
      SgVariableDeclaration* decl =
	SageBuilder::buildVariableDeclaration(availablename,
					      SageBuilder::buildIntType(),
					      NULL, scope);
      std::pair<void*, SgVariableDeclaration*> newelt(*i, decl);
      res.insert(newelt);
    }

  return res;
}


bool
PastToSage::isIterator(void* ptr) {
  std::vector<void*>::const_iterator i;
  for (i = _scoplibIterators.begin(); i != _scoplibIterators.end(); ++i)
    if (ptr == (void*) *i)
      return true;

  return false;
}



bool
PastToSage::isTileSizeParameter(void* ptr) {
  std::map<void*, SgVariableDeclaration*>::const_iterator i;
  for (i = _sageTileSizeParameters.begin(); i != _sageTileSizeParameters.end(); ++i)
    if (ptr == (void*) i->first)
      return true;

  return false;
}



bool
PastToSage::insertPragmas(SgNode* root)
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
		  pragmaClause = "#pragma ivdep\n#pragma vector always\n#pragma simd";
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


		    ScopParentAnnotation* annot = (ScopParentAnnotation*)
		      (root->get_parent()->getAttribute("PrivateVars"));
		    if (annot)
		      {
			std::vector<SgNode*> pv = annot->privateVars;
			for (k = pv.begin(); k != pv.end(); ++k)
			  {
			    SgVariableSymbol* symb = isSgVariableSymbol(*k);
			    ROSE_ASSERT(symb);
			    loopSymbols.insert(symb);
			  }
			root->get_parent()->removeAttribute("PrivateVars");
		      }

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


std::map<void*, SgVariableDeclaration*>
PastToSage::createTileSizeParameters(s_symbol_table_t* symtable,
				     SgScopeStatement* scope,
				     scoplib_scop_p scop,
				     PolyRoseOptions& options)
{
  // 1- Collect all tile size parameters in the PAST.
  /// FIXME: This should be given by PTile directly.
  s_symbol_t* tmp;
  int i;
  for (i = 0, tmp = symtable->symbols; tmp; tmp = tmp->next, ++i)
    ;
  s_symbol_t** allsyms = (s_symbol_t**)malloc(sizeof(s_symbol_t*) * (i + 1));
  ROSE_ASSERT(allsyms);
  for (i = 0, tmp = symtable->symbols; tmp; tmp = tmp->next)
    // Only symbol with char data type that are not iterators are tile size parameters.
    if (tmp->is_char_data && ! isIterator(tmp->data) && tmp->data
	&& ((char*)tmp->data)[0] == 'T')
      allsyms[i++] = tmp;
  allsyms[i] = NULL;

  // 2- Ensure the symbols are unique.
  int idx = 0;
  char* tile_size_array = NULL;
  for (i = 0; allsyms[i]; ++i)
    {
      std::string availablename = getAvailableName(std::string((char*)allsyms[i]->data));
      if (strcmp(availablename.c_str(), (char*)allsyms[i]->data))
	{
	  // Change the symbol name in the symbol table.
	  free (allsyms[i]->data);
	  allsyms[i]->data = strdup (availablename.c_str());
	}
    }

  // Create declarations for the new symbols.
  std::map<void*, SgVariableDeclaration*> res;
  for (i = 0; allsyms[i]; ++i)
    {
      SgAssignInitializer* init;
      SgExpression* initExpr;
      if (options.getInsertPtileAPI())
	initExpr = SageBuilder::buildPntrArrRefExp
	  (SageBuilder::buildOpaqueVarRefExp("___pace_tile_sizes", scope),
	   SageBuilder::buildIntVal(i));
      else
	initExpr = SageBuilder::buildIntVal(32);

      init = SageBuilder::buildAssignInitializer
	(SageBuilder::buildCastExp(initExpr, SageBuilder::buildFloatType()),
	 SageBuilder::buildFloatType());

      SgVariableDeclaration* decl =
	SageBuilder::buildVariableDeclaration(std::string((char*)allsyms[i]->data),
					      SageBuilder::buildFloatType(),
					      init, scope);
      std::pair<void*, SgVariableDeclaration*> newelt(allsyms[i]->data, decl);
      res.insert(newelt);
    }

  // Be clean.
  free (allsyms);

  return res;
}


/**
 *
 *
 *
 *
 */
PastToSage::PastToSage(SgScopeStatement* scopScope,
		       s_past_node_t* root,
		       scoplib_scop_p scoplibScop,
		       PolyRoseOptions& options,
		       int scopId)
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

  // 0.1- Rebuild symbol table.
  past_rebuild_symbol_table (root);

  // 1- Collect all iterators in the clast. They are of the from 'cXX'
  // where XX is an integer.
  _scoplibIterators = collectAllIterators(root);

  
  // 2- Create the basic block containing the transformed scop.
  SgBasicBlock* bb = SageBuilder::buildBasicBlock();
  bb->set_parent(scopRoot->get_parent());
  m_scope = bb->get_scope();
  m_var_counter_id = 0;

  // 3- Map PAST names to new variables that does not conflict
  // with existing names (ensured in PoCCOptimize), and register the
  // symbols in the symbol table.
  _sageIterators = createNewIterators(_scoplibIterators, m_scope);
  if (options.getCodegenUsePtile())
    // If parametric tiling is activated, deal with the newly
    // created tile size parameters.
    // 1- Find unique names, update the symbol table if required.
    // 2- Insert FP declaration for them
    // 3- Store those declarations.
    {
      if (past_node_is_a(root, past_root))
	{
	  PAST_DECLARE_TYPED(root, pr, root);
	  _sageTileSizeParameters = createTileSizeParameters(pr->symboltable,
							     m_scope, scoplibScop,
							     options);
	}
      else
	ROSE_ASSERT(0);
    }

  // 4- Translate PAST tree into Sage tree.
  buildBasicBlock(root, bb);

  // 5- Update the variable scope with the new bb, (FIXME: this may
  // not be needed anymore) and insert the declaration statement in
  // the AST.
  std::map<void*, SgVariableDeclaration*>::iterator vardecl;
  Rose_STL_Container<SgNode*> varRefs =
    NodeQuery::querySubTree(bb, V_SgVarRefExp);

  std::map<void*, SgVariableDeclaration*> unionDecls = _sageIterators;
  std::map<void*, SgVariableDeclaration*>::iterator it;
  for (it = _sageTileSizeParameters.begin(); it != _sageTileSizeParameters.end(); ++it)
    unionDecls.insert(*it);
  for (vardecl = unionDecls.begin(); vardecl != unionDecls.end(); ++vardecl)
    {
      // Deal with the symbol tables.
      SgInitializedNamePtrList& l = vardecl->second->get_variables();
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
      bb->prepend_statement(vardecl->second);
    }

  // Post-process for pragma insertion.
  if (options.getGeneratePragmas())
    insertPragmas(bb);

  // 6- Insert the parametric tiling API, if required.
  int nestSize = _sageTileSizeParameters.size();
  if (options.getCodegenUsePtile() && options.getInsertPtileAPI())
    {
      // Invoke the PTile API.
      SgExprStatement* tileSizeParameterArrayInit =
	SageBuilder::buildExprStatement
	(SageBuilder::buildFunctionCallExp
	 (SgName(std::string("PACETileSizeVectorInit")),
	  SageBuilder::buildVoidType(),
	  SageBuilder::buildExprListExp
	  (SageBuilder::buildOpaqueVarRefExp("___pace_tile_sizes", scope),
	   SageBuilder::buildIntVal(nestSize),
	   SageBuilder::buildIntVal(scopId)),
	   scope));
      bb->prepend_statement(tileSizeParameterArrayInit);
      // Declare the extern variable.
      SgVariableDeclaration* vardecl =
	SageBuilder::buildVariableDeclaration
	("___pace_tile_sizes",
	 SageBuilder::buildArrayType(SageBuilder::buildIntType(),
				     SageBuilder::buildIntVal(nestSize)),
	 NULL, scope);
      bb->prepend_statement(vardecl);
    }
  m_scopRoot = bb;
}


SgBasicBlock* PastToSage::getBasicBlock()
{
  return m_scopRoot;
}


std::string PastToSage::getAvailableName(std::string templatestr)
{
  // 1- Find the list of symbols used in the SCoP.
  if (_usedSymbols.size() == 0)
    {
      SgStatement* stmt = (SgStatement*)m_scoplib_scop->usr;
      if (stmt)
	{
	  std::set<SgVariableSymbol*> usedSymbols;
	  ScopRootAnnotation* annot =
	    (ScopRootAnnotation*)(stmt->getAttribute("ScopRoot"));
	  ROSE_ASSERT(annot);
	  std::map<std::string, SgVariableSymbol*> fakeSymbolMap =
	    annot->fakeSymbolMap;
	  std::vector<SgVarRefExp*> vrefs = 
	    SageInterface::querySubTree<SgVarRefExp>(stmt, V_SgVarRefExp);
	  std::vector<SgNode*> varrefs;
	  for (std::vector<SgVarRefExp*>::iterator i = vrefs.begin();
	       i != vrefs.end(); ++i)
	    varrefs.push_back(*i);
	  usedSymbols =
	    SageTools::convertToSymbolSet(varrefs, fakeSymbolMap);
	  std::set<SgVariableSymbol*>::const_iterator i;
	  for (i = usedSymbols.begin(); i != usedSymbols.end(); ++i)
	    _usedSymbols.push_back(std::string((*i)->get_name().str()));
	}
    }

  char buffer[templatestr.size() + 16];
  strcpy(buffer, templatestr.c_str());
  bool isUsedSymbol;
  bool needInsert = false;
  int counter = 0;
  do
    {
      // Ensure we will declare an unused symbol for the new loop
      // iterators.
      isUsedSymbol = false;
      std::vector<std::string>::iterator j;
      for (j = _usedSymbols.begin(); j != _usedSymbols.end(); ++j)
	if (! strcmp(j->c_str(), buffer))
	  {
	    isUsedSymbol = true;
	    break;
	  }
      if (isUsedSymbol)
	{
	  sprintf (buffer, "%s_%d", templatestr.c_str(), counter++);
	  needInsert = true;
	}
    }
  while (isUsedSymbol);
  if (needInsert)
    _usedSymbols.push_back(std::string(buffer));
  return std::string(buffer);
}

void
PastToSage::buildBasicBlock(s_past_node_t* root, SgBasicBlock* retBlock)
{
  IF_PLUTO_ROSE_VERBOSE(std::cout << "[PastToSage] Starting basic block" << std::endl;)

  // Go to the root body, if necessary.
  if (past_node_is_a(root, past_root))
    {
      PAST_DECLARE_TYPED(root, pr, root);
      root = pr->body;
    }

  s_past_node_t* curr;
  for (curr = root; curr != NULL; curr = curr->next)
    {
      if (past_node_is_a(curr, past_root))
	{
	  /* Misplaced statement root */
	  ROSE_ASSERT(0);
	}
      else if (past_node_is_a(curr, past_cloogstmt))
	{
	  /* User statement */
	  SgStatement* userStmt = buildUserStatement((s_past_cloogstmt_t*)curr);
	  retBlock->append_statement(userStmt);
	}
      // for <- parfor
      else if (past_node_is_a(curr, past_parfor))
	{
	  /* ParFor statement. For the moment it is a standard for
	     node. */
	  SgStatement* forStmt = buildFor((s_past_for_t*)curr);
	  // Ensure it's the outer-most parallel for in the tree.
	  s_past_node_t* parent;
	  for (parent = curr->parent; parent &&
		 !past_node_is_a (parent, past_parfor); parent = parent->parent)
	    ;
	  if (! parent)
	    {
	      SgForStatement* forStmtT = isSgForStatement(forStmt);
	      std::string pragmaOmpFor = buildPragmaOmpForC(curr);
	      SageInterface::attachArbitraryText(forStmt, pragmaOmpFor,
						 PreprocessingInfo::before);
	      SgBasicBlock* newbb = SageBuilder::buildBasicBlock();
	      // Declare 2 new variables, for lb and ub.
	      std::string nameLb = getAvailableName(std::string("tmpLb"));
	      SgVariableDeclaration* declLb =
		SageBuilder::buildVariableDeclaration
		(nameLb, SageBuilder::buildIntType(),
		 NULL, newbb);
	      newbb->append_statement(declLb);
	      std::string nameUb = getAvailableName(std::string("tmpUb"));
	      SgVariableDeclaration* declUb =
		SageBuilder::buildVariableDeclaration
		(nameUb, SageBuilder::buildIntType(),
		 NULL, newbb);
	      newbb->append_statement(declUb);
	      // Lower bound management.
	      SgVariableSymbol* symLb =
		new SgVariableSymbol(declLb->get_decl_item(SgName(nameLb)));
	      //newbb->insert_symbol(nameLb, symLb);
	      SgVarRefExp* vLb = SageBuilder::buildVarRefExp(symLb);
	      SgStatementPtrList finit = forStmtT->get_init_stmt();
	      SgExprStatement* firstinit = isSgExprStatement(*(finit.begin()));
	      ROSE_ASSERT(firstinit);
	      SgAssignOp* op = isSgAssignOp(firstinit->get_expression());
	      SgExpression* einit = op->get_rhs_operand();
	      SgStatement* initLb = SageBuilder::buildExprStatement(
		SageBuilder::buildBinaryExpression<SgAssignOp>
		(vLb, einit));
	      newbb->append_statement(initLb);
	      SgVarRefExp* vLb2 = SageBuilder::buildVarRefExp(symLb);
	      op->set_rhs_operand(vLb2);

	      // Upper bound management.
	      SgVariableSymbol* symUb =
		new SgVariableSymbol(declUb->get_decl_item(SgName(nameUb)));
	      newbb->insert_symbol(SgName(nameUb), symUb);
	      SgVarRefExp* vUb = SageBuilder::buildVarRefExp(symUb);
	      SgExprStatement* sexprub =
		isSgExprStatement(forStmtT->get_test());
	      ROSE_ASSERT(sexprub);
	      SgExpression* test = sexprub->get_expression();
	      SgLessOrEqualOp* lop = isSgLessOrEqualOp(test);
	      if (lop)
		{
		  SgExpression* testrhs = NULL;
		  SgStatement* initUb = SageBuilder::buildExprStatement(
		    SageBuilder::buildBinaryExpression<SgAssignOp>
		    (vUb, lop->get_rhs_operand()));
		  newbb->append_statement(initUb);
		  SgVarRefExp* vUb = SageBuilder::buildVarRefExp(symUb);
		  lop->set_rhs_operand(vUb);
		}
	      newbb->append_statement(forStmt);
	      forStmt = newbb;

	    }
	  else
	    {
	      s_past_node_t** inner = past_inner_loops(curr);
	      // This is an inner-most parallel for, insert #pragma ivdep.
	      if (inner && ! inner[0])
		{
		  std::string pragmaIvdep =
		    "#pragma ivdep\n#pragma vector always";
		  SageInterface::attachArbitraryText(forStmt, pragmaIvdep,
						     PreprocessingInfo::before);
		}
	      free(inner);
	    }

	  retBlock->append_statement(forStmt);
	  updateAnnotation(forStmt, curr);

	  // Mark the loop as parallel.
	  if (_polyoptions.getScVerboseLevel() == PolyRoseOptions::VERBOSE_FULL)
	    {
	      if (! _polyoptions.getQuiet())
		std::cout << "[PolyOpt] Parallel outer loop for: " << std::endl;
	      SageTools::debugPrintNode(forStmt);
	    }
	  SageForInfo* annot = new SageForInfo();
	  annot->isParallel = 1;
	  forStmt->setAttribute(SageForInfo::name(), annot);
	}
      else if (past_node_is_a(curr, past_for))
	{
	  /* For statement */
	  SgStatement* forStmt = buildFor((s_past_for_t*)curr);
	  retBlock->append_statement(forStmt);
	  updateAnnotation(forStmt, curr);
	}
//       else if(past_node_is_a(curr, past_vectorfor))
// 	{
// 	  /* VectorFor statement. For the moment it is a standard for
// 	     node. */
// 	  SgStatement* forStmt = buildFor((s_past_for_t*)curr);
// 	  retBlock->append_statement(forStmt);
// 	  updateAnnotation(forStmt, curr);
// 	}
      else if (past_node_is_a(curr, past_if))
	{
	  /* Generic if statement */
	  SgStatement* guardStmt = buildGenericIf(curr);
	  retBlock->append_statement(guardStmt);
	}
      else if (past_node_is_a(curr, past_affineguard))
	{
	  /* Affine guard statement */
	  SgStatement* guardStmt = buildGenericIf(curr);
	  retBlock->append_statement(guardStmt);
	  updateAnnotation(guardStmt, curr);
	}
      else if (past_node_is_a(curr, past_block))
	{
	  /* Block statement */
	  SgBasicBlock* block = SageBuilder::buildBasicBlock();
	  buildBasicBlock(((s_past_block_t*)curr)->body, block);
	  retBlock->append_statement(block);
	  updateAnnotation(block, curr);
	}
      else if (past_node_is_a (curr, past_assign))
	{
	  /* Cloog-generated assignemt of iterators, see reg_detect w/
	     tile+parallel */
	  SgStatement* stmt =
	    SageBuilder::buildExprStatement(buildExpressionTree(curr));
	  retBlock->append_statement(stmt);
	}
//       else if(past_node_is_a(curr, past_pragma_omp_for) ||
// 	      past_node_is_a(curr, past_pragma_vectorize))
// 	annotationNodes.push_back(curr);
      else if (past_node_is_a (curr, past_statement))
	{
	  // Current special case: otl being removed by cloog are
	  // reduced to a statement with the loop iterator value.
	  PAST_DECLARE_TYPED(statement, ps, curr);
	  SgStatement* stmt =
	    SageBuilder::buildExprStatement(buildExpressionTree(ps->body));
	  retBlock->append_statement(stmt);
	}
      else
	{
	  std::cerr << "[PastToSage] Unsupported/unimplemented node type"
		    << std::endl;
	  assert (0);
	}
    }

  IF_PLUTO_ROSE_VERBOSE(std::cout << "[PastToSage] Finished basic block" << std::endl;)
}


/**
 * Create a Sage if statement from a PAST guard.
 *
 *
 */
SgStatement* PastToSage::buildGenericIf(s_past_node_t* guard)
{
  IF_PLUTO_ROSE_VERBOSE(std::cout << "[PastToSage] Starting guard"
			<< std::endl;)

  SgStatement* retStmt = NULL;
  SgExpression* guardTest = NULL;
  SgBasicBlock* thenClause = NULL;
  SgBasicBlock* elseClause = NULL;

  if (past_node_is_a(guard, past_if))
    {
      PAST_DECLARE_TYPED(if, pif, guard);
      guardTest = buildExpressionTree(pif->condition);
      thenClause = SageBuilder::buildBasicBlock();
      buildBasicBlock(pif->then_clause, thenClause);
      elseClause = SageBuilder::buildBasicBlock();
      buildBasicBlock(pif->else_clause, elseClause);
    }
  else
    {
      assert(past_node_is_a(guard, past_affineguard));
      PAST_DECLARE_TYPED(affineguard, pif, guard);
      guardTest = buildExpressionTree(pif->condition);
      thenClause = SageBuilder::buildBasicBlock();
      buildBasicBlock(pif->then_clause, thenClause);
    }

  retStmt = SageBuilder::buildIfStmt(guardTest, thenClause, elseClause);

  IF_PLUTO_ROSE_VERBOSE(std::cout << "[PastToSage] Finished guard"
			<< std::endl;)

  return retStmt;
}


/**
 *
 * Create a Sage for statement.
 *
 */
SgStatement* PastToSage::buildFor(s_past_for_t* forStmt)
{
  IF_PLUTO_ROSE_VERBOSE(std::cout << "[PastToSage] Starting for" << std::endl;)

  SgStatement* retStmt = NULL;

  SgVarRefExp* iteratorVar =
    SageBuilder::buildVarRefExp
    (_sageIterators.find(forStmt->iterator->symbol->data)->second);
  SgExprStatement* initAssignStmt = NULL;
  SgExprStatement* testStmt = NULL;
  SgExpression* incrExpr = NULL;
  if (forStmt->init)
    initAssignStmt =
      SageBuilder::buildExprStatement(buildExpressionTree(forStmt->init));
  SgExpression* testExpr = buildExpressionTree(forStmt->test);
  if (! testExpr)
    {
      // ROSE does not support empty test part in a for loop. Fake
      // infinite loop with always-true conditional.
      testExpr = SageBuilder::buildIntVal(1);
    }
  testStmt = SageBuilder::buildExprStatement(testExpr);
  incrExpr = buildExpressionTree(forStmt->increment);

  SgBasicBlock* body = SageBuilder::buildBasicBlock();
  buildBasicBlock(forStmt->body, body);
  //  SageInterface::rebuildSymbolTable(isSgBasicBlock(body));

  retStmt =
    SageBuilder::buildForStatement(initAssignStmt, testStmt, incrExpr, body);


  IF_PLUTO_ROSE_VERBOSE(std::cout << "[PastToSage] Finished for" << std::endl;)

  return retStmt;
}


/**
 * Create a Sage statement with updated access functions.
 *
 *
 */
SgStatement* PastToSage::buildUserStatement(s_past_cloogstmt_t* statement)
{
  IF_PLUTO_ROSE_VERBOSE(std::cout << "[PastToSage] Starting user statement" << std::endl;)

  SgStatement* retStmt = NULL;
  // Get a pointer to the correct statement in the scoplib
  // representation.
  int statementNumber = statement->stmt_number;
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
      if (! _polyoptions.getQuiet())
	{
	  std::cout << "[PastToSage] Warning:  SCoP statement has attached"
	    " preprocessing info!  It will be cleared." << std::endl;
	  std::cout << "[PastToSage]   Offending Statement: " <<
	    origStatement->unparseToCompleteString() << std::endl;
	}
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

      s_past_node_t* subAssign = statement->substitutions;
      for(int j = 0; j < i; ++j)
	subAssign = subAssign->next;

      Rose_STL_Container<SgNode*> varNodes =
	NodeQuery::querySubTree(retStmt, V_SgVarRefExp);
      Rose_STL_Container<SgNode*>::iterator varIter;
      for(varIter = varNodes.begin(); varIter != varNodes.end(); ++varIter)
	{
	  SgVarRefExp* varRefExp = isSgVarRefExp(*varIter);
	  if(varRefExp->get_symbol() == originalIter)
	    {
	      SgExpression* newExpr =
		buildExpressionTree(subAssign);
	      SageInterface::replaceExpression(varRefExp, newExpr, false);
	    }
	}
    }


  IF_PLUTO_ROSE_VERBOSE(std::cout << "[PastToSage] Finished user statement"
			<< std::endl;)

  return retStmt;
}


/**
 * Create a SgVarRefExp from a PAST variable reference.
 *
 *
 */
SgExpression* PastToSage::buildVariableReference(s_past_variable_t* varref)
{
  IF_PLUTO_ROSE_VERBOSE(std::cout << "[PastToSage] Starting name" << std::endl;)

  ROSE_ASSERT(varref);
  ROSE_ASSERT(varref->symbol);

  SgExpression* var;
  // Check if it is a fake symbol (eg, a.b).
  if (! varref->symbol->is_char_data)
    {
      std::map<std::string, SgVariableSymbol*>::const_iterator i;
      for (i = _fakeSymbolMap.begin(); i != _fakeSymbolMap.end(); ++i)
	if (i->second == varref->symbol->data)
	  break;
      if (i != _fakeSymbolMap.end())
	{
	  /// FIXME: deal with struct of struct.
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
	  SgClassType* clsType = isSgClassType(s1->get_type());
	  ROSE_ASSERT(clsType);
	  SgClassDeclaration* decl =
	    isSgClassDeclaration(clsType->get_declaration());
	  decl = isSgClassDeclaration(decl->get_definingDeclaration());
	  ROSE_ASSERT(decl);
	  s2 = SageInterface::lookupSymbolInParentScopes
	    (n2, decl->get_definition());
	  return SageBuilder::buildDotExp
	    (SageBuilder::buildVarRefExp(isSgVariableSymbol(s1)),
	     SageBuilder::buildVarRefExp(isSgVariableSymbol(s2)));
	}
    }
  // If the name is an iterator, simply create a reference to its declaration.
  if (varref->symbol->is_char_data && isIterator(varref->symbol->data))
      var = SageBuilder::buildVarRefExp
	(_sageIterators.find(varref->symbol->data)->second);
  // Else if the name is a parameter, create a reference using its
  // existing symbol.
  else if (! varref->symbol->is_char_data &&
	   isSgVariableSymbol((SgNode*)(varref->symbol->data)) != NULL)
    var = SageBuilder::buildVarRefExp
      (isSgVariableSymbol((SgNode*)(varref->symbol->data)));
  // Else this is a tile parameter created by PTile, create an opaque
  // reference.
  else if (isTileSizeParameter(varref->symbol->data))
    var = SageBuilder::buildVarRefExp
      (_sageTileSizeParameters.find(varref->symbol->data)->second);
  else
    {
      std::cerr << "[PolyOpt][WARNING] Unattached symbol" << std::endl;
      ROSE_ASSERT(0);
      var =
	SageBuilder::buildOpaqueVarRefExp((const char*)varref->symbol->data,
					  m_scope);
    }
  IF_PLUTO_ROSE_VERBOSE(std::cout << "[PastToSage] Finished name  (" <<
			var->unparseToCompleteString() << ")" << std::endl;)

  return var;
}



/**
 * Generic PAST-to-Sage expression builder.
 *
 *
 */
SgExpression* PastToSage::buildExpressionTree(s_past_node_t* node)
{
  if (! node)
    return NULL;

  if (past_node_is_a(node, past_binary))
    {
      PAST_DECLARE_TYPED(binary, binnode, node);
#define pasttosage_traverse_binop(type, sageop)			\
      if (past_node_is_a(node, past_##type))			\
	{							\
	  return SageBuilder::buildBinaryExpression<sageop>	\
	    (buildExpressionTree(binnode->lhs),			\
	     buildExpressionTree(binnode->rhs));		\
	}
      pasttosage_traverse_binop(add, SgAddOp);
      pasttosage_traverse_binop(sub, SgSubtractOp);
      pasttosage_traverse_binop(mul, SgMultiplyOp);
      pasttosage_traverse_binop(div, SgDivideOp);
      pasttosage_traverse_binop(mod, SgModOp);
      pasttosage_traverse_binop(and, SgAndOp);
      pasttosage_traverse_binop(or, SgOrOp);
      pasttosage_traverse_binop(equal, SgEqualityOp);
      pasttosage_traverse_binop(assign, SgAssignOp);
      pasttosage_traverse_binop(geq, SgGreaterOrEqualOp);
      pasttosage_traverse_binop(leq, SgLessOrEqualOp);
      pasttosage_traverse_binop(gt, SgGreaterThanOp);
      pasttosage_traverse_binop(lt, SgLessThanOp);

      // Special cases: min, max, ceild, floord are hard-wired.
      if (past_node_is_a(node, past_min) || past_node_is_a(node, past_max))
	return buildMinMax(binnode);
      if (past_node_is_a(node, past_ceild) || past_node_is_a(node, past_floord))
	return buildCeildFloord(binnode);
    }
  else if (past_node_is_a(node, past_unary))
    {
      PAST_DECLARE_TYPED(unary, unnode, node);

      if (past_node_is_a(node, past_dec_before))
	{
	  return SageBuilder::buildMinusMinusOp
	    (buildExpressionTree(unnode->expr), SgUnaryOp::prefix);
	}
      if (past_node_is_a(node, past_dec_after))
	{
	  return SageBuilder::buildMinusMinusOp
	    (buildExpressionTree(unnode->expr), SgUnaryOp::postfix);
	}
      if (past_node_is_a(node, past_inc_before))
	{
	  return SageBuilder::buildPlusPlusOp
	    (buildExpressionTree(unnode->expr), SgUnaryOp::prefix);
	}
      if (past_node_is_a(node, past_inc_after))
	{
	  return SageBuilder::buildPlusPlusOp
	    (buildExpressionTree(unnode->expr), SgUnaryOp::postfix);
	}

#define pasttosage_traverse_funcunop(type)			\
      if (past_node_is_a(node, past_##type))			\
	{							\
	  return SageBuilder::buildFunctionCallExp		\
	    (SgName(#type), new SgTypeFloat(), SageBuilder::buildExprListExp(buildExpressionTree(unnode->expr)), m_scope); \
	}
      pasttosage_traverse_funcunop(ceil);
      pasttosage_traverse_funcunop(floor);
      pasttosage_traverse_funcunop(round);
      pasttosage_traverse_funcunop(sqrt);
    }
  else if (past_node_is_a(node, past_variable))
    {
      return buildVariableReference((s_past_variable_t*) node);
    }
  else if (past_node_is_a(node, past_value))
    {
      PAST_DECLARE_TYPED(value, num, node);
      switch (num->type)
	{
	case e_past_value_int:
	  return SageBuilder::buildIntVal(num->value.intval);
	case e_past_value_float:
	  return SageBuilder::buildFloatVal(num->value.floatval);
	case e_past_value_double:
	  return SageBuilder::buildDoubleVal(num->value.doubleval);
	default:
	  assert(0);
	}
    }
  else
    {
      std::cerr << "[PastToSage] Unsupported/unimplemented node type"
		<< std::endl;
      assert(0);
    }
}




SgExpression* PastToSage::buildCeildFloord(s_past_binary_t* binary)
{
  IF_PLUTO_ROSE_VERBOSE(std::cout << "[PastToSage] Starting binary operation"
			<< std::endl;)

  SgExpression* lhs = buildExpressionTree(binary->lhs);
  SgExpression* rhs = buildExpressionTree(binary->rhs);
  SgExpression* retExpr = NULL;

  if (past_node_is_a((s_past_node_t*)binary, past_floord))
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
    }
  else if (past_node_is_a((s_past_node_t*)binary, past_ceild))
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

    }

  IF_PLUTO_ROSE_VERBOSE(std::cout << "[PastToSage] Finished binary operation (" << retExpr->unparseToCompleteString() << ")" << std::endl;)

    return retExpr;
}


SgExpression* PastToSage::buildMinMax(s_past_binary_t* minmax)
{

  IF_PLUTO_ROSE_VERBOSE(std::cout <<
			"[PastToSage] Starting minmax" << std::endl;)

  SgExpression*   retExpr   = NULL;
  SgExpression*   exprA     = buildExpressionTree(minmax->lhs);
  SgExpression*   exprB     = buildExpressionTree(minmax->rhs);
  SgExpression*   compExpr  = NULL;

  if (past_node_is_a((s_past_node_t*)minmax, past_max))
    compExpr =
      SageBuilder::buildBinaryExpression<SgGreaterThanOp>(exprA, exprB);
  else
    compExpr = SageBuilder::buildBinaryExpression<SgLessThanOp>(exprA, exprB);
  retExpr =
    SageBuilder::buildConditionalExp(compExpr,
				     SageInterface::copyExpression(exprA),
				     SageInterface::copyExpression(exprB));


  IF_PLUTO_ROSE_VERBOSE(std::cout << "[PastToSage] Finished minmax ("
			<< retExpr->unparseToCompleteString() << ")"
			<< std::endl;)

  return retExpr;
}


void PastToSage::updateAnnotation(SgStatement* stmt, s_past_node_t* cnode)
{
  // No annotation to put.
  if (annotationNodes.empty ())
    return;
  std::vector<s_past_node_t*>::iterator i;
  for (i = annotationNodes.begin(); i != annotationNodes.end(); ++i)
    if ((*i)->next == cnode)
      break;
  // No annotation corresponding to this clast node.
  if (i == annotationNodes.end())
    return;

//   if (past_node_is_a(*i, stmt_pragma_omp_for))
//     {
//       clast_pragma_omp_for* annot = (clast_pragma_omp_for*) (*i);
//       SageInterface::attachArbitraryText(stmt, annot->clause,
// 					 PreprocessingInfo::before);
//     }
//   else if (past_node_is_a(*i, stmt_pragma_vectorize))
//     {
//       clast_pragma_vectorize* annot = (clast_pragma_vectorize*) (*i);
//       SageInterface::attachArbitraryText(stmt, annot->clause,
// 					 PreprocessingInfo::before);
//     }

  annotationNodes.erase(i);
}


static
void traverse_iters(s_past_node_t* node, void* args)
{
  if (past_node_is_a (node, past_for))
    {
      PAST_DECLARE_TYPED(for, pf, node);
      if (pf->iterator->symbol->is_char_data)
	{
	  std::vector<char*>* iters = (std::vector<char*>*)args;
	  std::vector<char*>::const_iterator i;
	  for (i = iters->begin(); i != iters->end(); ++i)
	    if (! strcmp (*i, (char*)pf->iterator->symbol->data))
	      break;
	  if (i == iters->end())
	    iters->push_back((char*)pf->iterator->symbol->data);
	}
    }
  else if (past_node_is_a (node, past_statement))
    {
      // Special case. OTL are written as iterator = expression.
      PAST_DECLARE_TYPED(statement, ps, node);
      if (past_node_is_a (ps->body, past_assign))
	{
	  PAST_DECLARE_TYPED(binary, pb, ps->body);
	  if (past_node_is_a (pb->lhs, past_variable))
	    {
	      PAST_DECLARE_TYPED(variable, pv, pb->lhs);
	      if (pv->symbol->is_char_data &&
		  pv->symbol->data && ((char*)(pv->symbol->data))[0] == 'c')
		{
		  std::vector<char*>* iters = (std::vector<char*>*)args;
		  std::vector<char*>::const_iterator i;
		  for (i = iters->begin(); i != iters->end(); ++i)
		    if (! strcmp (*i, (char*)pv->symbol->data))
		      break;
		  if (i == iters->end())
		    iters->push_back((char*)pv->symbol->data);
		}
	    }
	}
    }
}

/**
 * Build a string corresponding to the #pragma omp parallel for, by
 * collecting private loop iterators.
 *
 */
std::string
PastToSage::buildPragmaOmpForC(s_past_node_t* forTree)
{
  std::string ret;
  ret = "#pragma omp parallel for";
  // Collect private variables.
  std::vector<char*>* iters = new std::vector<char*>();
  PAST_DECLARE_TYPED(for, pf, forTree);
  past_visitor (pf->body, traverse_iters, (void*)iters, NULL, NULL);
  if (pf->usr)
    {
      std::vector<SgNode*>* pv = (std::vector<SgNode*>*)pf->usr;
      for (int i = 0; i < pv->size(); ++i)
	iters->push_back((char*)(isSgVariableSymbol((*pv)[i])->get_name().getString().c_str()));
    }

  // Build the clause.
  bool is_first = true;
  for (std::vector<char*>::const_iterator i = iters->begin();
       i != iters->end(); ++i)
    {
      if (is_first)
	{
	  ret = ret + " private(";
	  is_first = false;
	}
      else
	ret = ret + ", ";
      ret = ret + *i;
    }
  if (! is_first)
    ret = ret + ")";
  free(iters);

  return ret;
}
