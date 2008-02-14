// Copyright 2005,2006,2007,2008 Markus Schordan, Gergo Barany
// $Id: CFGTraversal.C,v 1.12 2008-02-14 11:22:09 gergo Exp $

#include <iostream>
#include <string.h>

#include "patternRewrite.h"

#include "CFGTraversal.h"
#include "ExprLabeler.h"
#include "ExprTransformer.h"
#include "analysis_info.h"
#include "IrCreation.h"

#define REPLACE_FOR_BY_WHILE

CFGTraversal::CFGTraversal(std::deque<Procedure *> *procs)
    : node_id(0), procnum(0), cfg(new CFG()), real_cfg(NULL), proc(NULL),
      call_num(0), lognum(0), expnum(0) 
{
  cfg->procedures = procs;
  std::deque<Procedure *>::const_iterator i;
  for (i = procs->begin(); i != procs->end(); ++i) {
    cfg->nodes.push_back((*i)->entry);
    node_id++;
    cfg->entries.push_back((*i)->entry);
    cfg->nodes.push_back((*i)->exit);
    node_id++;
    cfg->exits.push_back((*i)->exit);
    if ((*i)->this_assignment != NULL) {
      cfg->nodes.push_back((*i)->this_assignment);
      node_id++;
    }
  }
  for (i = procs->begin(); i != procs->end(); ++i) {
    if ((*i)->arg_block != NULL) {
      std::deque<SgStatement *>::const_iterator j;
      BasicBlock *b, *prev = NULL, *first = NULL;
      for (j = (*i)->arg_block->statements.begin();
	   j != (*i)->arg_block->statements.end(); ++j) {
	/* deal with constructor initializers */
	ArgumentAssignment* aa
	  = dynamic_cast<ArgumentAssignment *>(*j);
	if (aa && isSgConstructorInitializer(aa->get_rhs())) {
	  SgTreeCopy treecopy;
	  SgExpression* new_expr= isSgExpression(aa->get_rhs()->copy(treecopy));
	  if (isSgVarRefExp(aa->get_lhs())) {
	    new_expr->set_parent(isSgVarRefExp(aa->get_lhs())->get_symbol()->get_declaration());
	  } else {
	    new_expr->set_parent(NULL); 
	    //std::cout << "WARNING: CFGTraversal: setting parent = NULL!" << std::endl;
	  }
	  ExprLabeler el(expnum);
	  el.traverse(new_expr, preorder);
	  expnum = el.get_expnum();
	  ExprTransformer et(node_id, (*i)->procnum, expnum, cfg, NULL);
	  et.traverse(new_expr, preorder);
	  for (int z = node_id; z < et.get_node_id(); ++z) {
	    block_stmt_map[z] = current_statement;
	  }
	  node_id = et.get_node_id();
	  expnum = et.get_expnum();
	  if (first == NULL)
	    first = et.get_after();
	  if (prev != NULL)
	    add_link(prev, et.get_after(), NORMAL_EDGE);
	  b = prev = et.get_last();
	  /* first is a block containing an
	   * ArgumentAssignment($A$this, &$anonymous_var)
	   * if this is an initialization of a superclass,
	   * change into ArgumentAssignment($A$this, this)
	   */
	  if (isSgClassType(aa->get_lhs()->get_type())) {
	    Procedure* p = (*cfg->procedures)[(*i)->procnum];
	    SgClassDefinition* ca
	      = isSgClassDeclaration(isSgClassType(aa->get_lhs()->get_type())
				     ->get_declaration())->get_definition();
	    SgClassDefinition *cc = p->class_type;
	    if (subtype_of(cc, ca)) {
	      BasicBlock *f = first->successors[0].first;
	      ArgumentAssignment *a
		= dynamic_cast<ArgumentAssignment *>(f->statements[0]);
	      if (a == NULL)
		break;
	      SgPointerType* ptrType= Ir::createPointerType(p->class_type
							    ->get_declaration()
							    ->get_type());
	      SgVarRefExp* thisVarRefExp =Ir::createVarRefExp("this",ptrType);
	      f->statements[0] = Ir::createArgumentAssignment(a->get_lhs(),thisVarRefExp);
	    }
	  }
	} else {
	  /* empty block for argument assignments */
	  b = new BasicBlock(node_id++, INNER, (*i)->procnum);
	  cfg->nodes.push_back(b);
	  b->statements.push_back(*j);
	  if (first == NULL)
	    first = b;
	  if (prev != NULL)
	    add_link(prev, b, NORMAL_EDGE);
	  prev = b;
	}
      }
      (*i)->first_arg_block = first;
      (*i)->last_arg_block = b;
    }
  }
}

extern CFG *global_cfg;

CFG*
CFGTraversal::getCFG() {
#if 0
  CFGCheck cfgcheck;
  if (real_cfg != NULL) {
    cfgcheck.checkExpressions(real_cfg);
    return real_cfg;
  }
  cfgcheck.checkExpressions(cfg);
#endif

  perform_goto_backpatching();
  number_exprs();
  
  /*
    std::cout << cfg->numbers_types.size() << " types" << std::endl;
    std::map<int, SgType *>::const_iterator ne;
    for (ne = cfg->numbers_types.begin(); ne != cfg->numbers_types.end(); ++ne)
    std::cout << (*ne).first << "\t"
            << (*ne).second->unparseToString()
            << std::endl;
    std::cout << std::endl;
    */

    // check expressions
    /*
    std::cout << cfg->exprs_numbers.size() << " expressions" << std::endl;
    std::map<SgExpression *, int, ExprPtrComparator>::const_iterator en;
    for (en = cfg->exprs_numbers.begin(); en != cfg->exprs_numbers.end(); ++en)
      std::cout << "'" << expr_to_string((*en).first) << "'\t"
		<< (*en).second << std::endl;
    */

  return real_cfg = global_cfg = cfg;
}

void 
CFGTraversal::perform_goto_backpatching() {
  std::deque<Procedure *>::iterator pi;
  for (pi = cfg->procedures->begin(); pi != cfg->procedures->end(); ++pi) {
    typedef std::multimap<std::string, BasicBlock *> maptype;
    maptype::iterator gotos;
    gotos = (*pi)->goto_blocks.begin();
    while (gotos != (*pi)->goto_blocks.end()) {
      std::string label = (*gotos).first;
      maptype::iterator target = (*pi)->goto_targets.find(label);
      if (target != (*pi)->goto_targets.end()) {
	add_link((*gotos).second, (*target).second, NORMAL_EDGE);
      } else {
	std::cout << "warning: goto statement with unknown label!"
		  << std::endl;
      }
      ++gotos;
    }
  }
}

typedef std::set<SgExpression*, ExprPtrComparator> expression_set;

class ExprSetTraversal : public AstSimpleProcessing {
public:
  ExprSetTraversal(std::set<SgExpression *, ExprPtrComparator> *expr_set_,
		   std::set<SgType *, TypePtrComparator> *type_set_)
    : expr_set(expr_set_), type_set(type_set_) {}
  
protected:
  void visit(SgNode *node) {
    if (isSgExpression(node) && !isSgExpressionRoot(node)
	&& (!isSgConstructorInitializer(node)
	    || !isSgConstructorInitializer(node)->get_args()->empty())
	&& (!isSgExprListExp(node)
	    || !isSgExprListExp(node)->empty())
	) {
      expr_set->insert(isSgExpression(node));
      type_set->insert(isSgExpression(node)->get_type());
    }
  }
  
private:
  std::set<SgExpression *, ExprPtrComparator> *expr_set;
  std::set<SgType *, TypePtrComparator> *type_set;
};

expression_set*
make_nonredundant (const expression_set &exprs) {
  expression_set parens, *no_parens = new expression_set();
  expression_set::const_iterator e;
  for (e = exprs.begin(); e != exprs.end(); ++e) {
    const char *str = expr_to_string(*e);
    if (strlen(str) >= 2 && str[0] == '(' && str[strlen(str)-1] == ')')
      parens.insert(*e);
    else
      no_parens->insert(*e);
  }
  expression_set::iterator p, np;
  for (p = parens.begin(); p != parens.end(); ++p) {
    bool duplicate = false;
    const char *pstr = expr_to_string(*p);
    for (np = no_parens->begin(); np != no_parens->end(); ++np) {
      const char *npstr = expr_to_string(*np);
      if (strlen(pstr) == strlen(npstr) + 2
	  && strncmp(pstr+1, npstr, strlen(npstr)) == 0) {
	duplicate = true;
	break;
      }
    }
    if (!duplicate) {
      no_parens->insert(*p);
    }
  }
  return no_parens;
}

void 
CFGTraversal::number_exprs() {
  std::deque<BasicBlock *>::const_iterator block;
  std::deque<SgStatement *>::const_iterator stmt;
  std::set<SgExpression *, ExprPtrComparator> expr_set;
  std::set<SgType *, TypePtrComparator> type_set;

// add all global initializer expressions and the types of all global
// variables
  ExprSetTraversal global_est(&expr_set, &type_set);
  std::map<SgVariableSymbol *, SgExpression *>::iterator itr;
  for (itr = cfg->globals_initializers.begin();
       itr != cfg->globals_initializers.end(); ++itr)
  {
      global_est.traverse(itr->second, preorder);
  }
  std::vector<SgVariableSymbol *>::iterator g_itr;
  for (g_itr = cfg->globals.begin(); g_itr != cfg->globals.end(); ++g_itr)
      type_set.insert((*g_itr)->get_type());
  
// collect all expressions from the program blocks
  for (block = cfg->nodes.begin(); block != cfg->nodes.end(); ++block) {
    for (stmt = (*block)->statements.begin();
	 stmt != (*block)->statements.end(); ++stmt) {
      ExprSetTraversal est(&expr_set, &type_set);
      if (isSgCaseOptionStmt(*stmt) 
	  || isSgExprStatement(*stmt)
	  || isSgScopeStatement(*stmt)) {
	est.traverse(*stmt, preorder);
      } else if (dynamic_cast<ArgumentAssignment *>(*stmt)) {
	est.traverse(dynamic_cast<ArgumentAssignment *>(*stmt)->get_lhs(),
		     preorder);
	  est.traverse(dynamic_cast<ArgumentAssignment *>(*stmt)->get_rhs(),
		       preorder);
      } else if (dynamic_cast<ReturnAssignment *>(*stmt)) {
	est.traverse(Ir::createVarRefExp(dynamic_cast<ReturnAssignment *>(*stmt)->get_lhs()),
		     preorder);
	est.traverse(Ir::createVarRefExp(dynamic_cast<ReturnAssignment *>(*stmt)->get_lhs()),
		     preorder);
      } else if (dynamic_cast<ParamAssignment *>(*stmt)) {
	est.traverse(Ir::createVarRefExp(dynamic_cast<ParamAssignment *>(*stmt)->get_lhs()),
		     preorder);
	est.traverse(Ir::createVarRefExp(dynamic_cast<ParamAssignment *>(*stmt)->get_lhs()),
		     preorder);
      } else if (dynamic_cast<LogicalIf *>(*stmt)) {
	est.traverse(dynamic_cast<LogicalIf *>(*stmt)->get_condition(),
		     preorder);
      } else if (dynamic_cast<DeclareStmt *>(*stmt)) {
          type_set.insert(dynamic_cast<DeclareStmt *>(*stmt)->get_type());
      }
    }
  }
  unsigned int i = 0;
  std::set<SgExpression *, ExprPtrComparator> *exprs_nonred;
  exprs_nonred = make_nonredundant(expr_set);
  std::set<SgExpression *, ExprPtrComparator>::const_iterator expr;
  for (expr = exprs_nonred->begin(); expr != exprs_nonred->end(); ++expr) {
    cfg->numbers_exprs[i] = *expr;
    cfg->exprs_numbers[*expr] = i;
    i++;
  }
  unsigned int j = 0;
  std::set<SgType *, TypePtrComparator>::const_iterator type;
  for (type = type_set.begin(); type != type_set.end(); ++type) {
    cfg->numbers_types[j] = *type;
    cfg->types_numbers[*type] = j;
    j++;
  }
}

void 
CFGTraversal::visit(SgNode *node) {
  // collect all global variables into a list
  if (SgGlobal *global = isSgGlobal(node->get_parent()))
  {
      SgDeclarationStatementPtrList::iterator itr;
      for (itr = global->getDeclarationList().begin();
           itr != global->getDeclarationList().end();
           ++itr)
      {
          if (SgVariableDeclaration *vardecl = isSgVariableDeclaration(*itr))
          {
              SgInitializedName *initname = vardecl->get_variables().front();
              SgVariableSymbol *varsym
                  = global->lookup_var_symbol(initname->get_name());
              std::string name = initname->get_name().str();
              if (cfg->names_globals.find(name) == cfg->names_globals.end())
              {
                  cfg->names_globals[name] = varsym;
                  cfg->globals.push_back(varsym);
              }
              if (cfg->names_initializers.find(name) == cfg->names_initializers.end())
              {
               // GB (2008-02-14): Added support for aggregate initializers.
                  if (isSgAssignInitializer(initname->get_initializer())
                          || isSgAggregateInitializer(initname->get_initializer()))
                  {
                      cfg->names_initializers[name] = initname->get_initializer();
                      cfg->globals_initializers[varsym] = cfg->names_initializers[name];
                  }
              }
          }
      }
  }
  // visit all function definitions
  if (isSgFunctionDeclaration(node)) {
    SgFunctionDeclaration *decl = isSgFunctionDeclaration(node);
    if (decl->get_definition() != NULL) {
      proc = (*cfg->procedures)[procnum++];

      BasicBlock *last_node = (proc->this_assignment 
			       ? proc->this_assignment 
			       : proc->exit);

      if (is_destructor_decl(decl))
	last_node = call_base_destructors(proc, last_node);

      BasicBlock *begin
	= transform_block(decl->get_definition()->get_body(),
			  last_node, NULL, NULL);
      if (proc->arg_block != NULL) {
	add_link(proc->entry, proc->first_arg_block, NORMAL_EDGE);
	add_link(proc->last_arg_block, begin, NORMAL_EDGE);
      } else {
	add_link(proc->entry, begin, NORMAL_EDGE);
      }
    }
  }
}

void 
add_link(BasicBlock *from, BasicBlock *to, KFG_EDGE_TYPE type) {
  if (from != NULL && to != NULL) {
    from->successors.push_back(Edge(to, type));
    to->predecessors.push_back(Edge(from, type));
  }
}

BasicBlock*
CFGTraversal::transform_block(SgBasicBlock *block,
					  BasicBlock *after, BasicBlock *break_target,
					  BasicBlock *continue_target)
{
  /*
   * The basic block is split into several new ones: One block per
   * statement. Consecutive blocks are connected via normal edges,
   * loops and ifs by true and false edges to their successors.
   * The first block is linked from before, *all* new blocks
   * without successors, and the last block, are linked to after.
   */
  SgTreeCopy treecopy;
  BasicBlock *new_block = NULL, *last_block_of_this_block = NULL;
  SgStatementPtrList stmts = block->get_statements();

    /* Find all declarations of all variables in this block (but not
     * in blocks contained in this one). We need to undeclare them
     * and, in case of class objects, call their destructors at the
     * end of the block. Class objects are also tracked in a
     * seperate list used for calling their destructors. */
    SgStatementPtrList::iterator s;
    SgInitializedNamePtrList local_classvar_decls;
    std::vector<SgVariableSymbol *> *local_var_decls
        = new std::vector<SgVariableSymbol *>();
    for (s = stmts.begin(); s != stmts.end(); ++s) {
      // MS: transformation of variable declarations
      if (isSgVariableDeclaration(*s)) {
	SgVariableDeclaration *decl = isSgVariableDeclaration(*s);
	SgInitializedNamePtrList::iterator n;
	for (n = decl->get_variables().begin();
	     n != decl->get_variables().end(); ++n) {
	  /* TODO: do something about storage specifiers? */
	  if (isSgClassType((*n)->get_type()))
	    local_classvar_decls.push_back(*n);
	  // n is a InitializedName, but those names are of form '::name'
	  SgVariableSymbol *varsym = Ir::createVariableSymbol(*n);
	  local_var_decls->push_back(varsym);
	}
      }
    }
    /* Create an undeclare statement for these variables. */
    if (!local_var_decls->empty()) {
      /* If the last statement in the block is a return statement,
       * we must jump to the procedure's exit block after this
       * undeclare. (Additionally, we must collect the variables
       * of all enclosing blocks for undeclaration. This is a TODO.) */
      if (isSgReturnStmt(block->get_statements().back())) {
	current_statement = NULL;
	new_block = allocate_new_block(new_block, proc->exit);
	new_block->statements.push_back(Ir::createUndeclareStmt(local_var_decls));
	last_block_of_this_block = new_block;
      } else {
	/* No return: Just produce a normal block. */
	current_statement = NULL;
	new_block = allocate_new_block(new_block, after);
	new_block->statements.push_back(Ir::createUndeclareStmt(local_var_decls));
      }
      after = new_block;
      new_block = NULL;
    }
    /* Now call the destructors. These calls are non-virtual. */
    SgInitializedNamePtrList::iterator n;
    for (n = local_classvar_decls.begin(); 
	 n != local_classvar_decls.end();
	 ++n) {
      after = call_destructor(*n, cfg, proc->procnum, after, &node_id);
      // as an aside, why doesn't this print variable names?
      // std::cout << "destroyed " << (*n)->unparseToString() << std::endl;
      last_block_of_this_block = after;
    }
    /* Iterate over statements backwards: We need to know the blocks
     * that come later on to be able to set links. This is similar
     * to a continuation-style semantics definition. */
    SgStatementPtrList::reverse_iterator i;
    for (i = stmts.rbegin(); i != stmts.rend(); ++i) {
      StatementAttribute *stmt_start = NULL, *stmt_end = NULL;
      
      current_statement = *i;
      
      switch ((*i)->variantT()) {
      case V_SgIfStmt: {
	block_stmt_map[node_id] = current_statement;
	BasicBlock *join_block
	  = new BasicBlock(node_id++, INNER, proc->procnum);
	cfg->nodes.push_back(join_block);
	join_block->statements.push_back(Ir::createIfJoin());
	add_link(join_block, after, TRUE_EDGE);
	after = join_block;

	SgIfStmt *ifs = isSgIfStmt(*i);
	block_stmt_map[node_id] = current_statement;
	BasicBlock *if_block
	  = new BasicBlock(node_id++, INNER, proc->procnum);
	cfg->nodes.push_back(if_block);
                
	SgExprStatement* cond
	  = isSgExprStatement(ifs->get_conditional());
	SgExpression* new_expr
	  = isSgExpression(cond->get_expression()->copy(treecopy));
	ExprLabeler el(expnum);
	el.traverse(new_expr, preorder);
	expnum = el.get_expnum();
	ExprTransformer et(node_id, proc->procnum, expnum,
			   cfg, if_block);
	et.traverse(new_expr, preorder);
	for (int z = node_id; z < et.get_node_id(); ++z)
	  block_stmt_map[z] = current_statement;
	node_id = et.get_node_id();
	expnum = et.get_expnum();
	
	if (et.get_root_var() != NULL) {
	  new_expr = Ir::createVarRefExp(et.get_root_var());
	}
                
	if_block->statements.push_front(Ir::createIfStmt(Ir::createExprStatement(new_expr)));
                
	BasicBlock *t = transform_block(ifs->get_true_body(),
					after, 
					break_target, 
					continue_target);
	add_link(if_block, t, TRUE_EDGE);
	SgBasicBlock *false_body = ifs->get_false_body();
	if (false_body != NULL
	    && false_body->get_statements().size() > 0) {
	  BasicBlock *f
	    = transform_block(ifs->get_false_body(), after,
			      break_target, continue_target);
	  add_link(if_block, f, FALSE_EDGE);
	} else {
	  add_link(if_block, after, FALSE_EDGE);
	}
                
	/* incoming analysis information is at the incoming
	 * edge of the code produced by et */
	stmt_start = new StatementAttribute(et.get_after(), POS_PRE);
	/* outgoing analysis information is at the incoming
	 * edge of the after block, where true and false
	 * paths run together again */
	stmt_end = new StatementAttribute(after, POS_PRE);
                
	new_block = NULL;
	after = et.get_after();
	
	(*i)->addNewAttribute("PAG statement head",
			      new StatementAttribute(if_block, POS_POST));
      }
	break;

      case V_SgForStatement: {
	SgForStatement *fors = isSgForStatement(*i);
	/* create a block containing the initialization
	 * statement */
	block_stmt_map[node_id] = current_statement;
	BasicBlock *init_block
	  = new BasicBlock(node_id++, INNER, proc->procnum);
	cfg->nodes.push_back(init_block);
	/* create a block for the "real" head of the for
	 * statement (where the condition is tested) */
	block_stmt_map[node_id] = current_statement;
	BasicBlock *for_block
	  = new BasicBlock(node_id++, INNER, proc->procnum);
	cfg->nodes.push_back(for_block);
	BasicBlock *init_block_after = for_block;
                
	SgStatementPtrList init_list
	  = fors->get_for_init_stmt()->get_init_stmt();
	SgStatementPtrList::reverse_iterator it;
	for (it = init_list.rbegin(); it != init_list.rend(); ++it) {
	  SgExprStatement *init_expr
	    = isSgExprStatement(*it);
	  SgVariableDeclaration *init_decl
	    = isSgVariableDeclaration(*it);
	  if (init_expr) {
	    init_block = allocate_new_block(init_block,
					    init_block_after);
	    SgExpression* new_expr
	      = isSgExpression(init_expr->get_expression()->copy(treecopy));
	    new_expr->set_parent(NULL);
	    ExprLabeler el(expnum);
	    el.traverse(new_expr, preorder);
	    expnum = el.get_expnum();
	    ExprTransformer et(node_id, proc->procnum, expnum,
			       cfg, init_block);
	    et.traverse(new_expr, preorder);
	    for (int z = node_id; z < et.get_node_id(); ++z)
	      block_stmt_map[z] = current_statement;
	    node_id = et.get_node_id();
	    expnum = et.get_expnum();
	    
	    init_block->statements.push_front(Ir::createExprStatement(new_expr));
	    init_block_after = et.get_after();
	    init_block = NULL;
	  } else if (init_decl) {
	    init_block = allocate_new_block(init_block,
					    init_block_after);
	    SgInitializedNamePtrList vars
	      = init_decl->get_variables();
	    SgInitializedNamePtrList::reverse_iterator it;
	    for (it = vars.rbegin(); it != vars.rend(); ++it) {
	      SgAssignInitializer *init
		= isSgAssignInitializer((*it)->get_initializer());
	      if (!init) {
		std::cout << "unsupported initializer "
			  << "in for loop" << std::endl;
		break;
	      }
	      SgExpression* new_expr
		= isSgExpression(init->copy(treecopy));
	      ExprLabeler el(expnum);
	      el.traverse(new_expr, preorder);
	      expnum = el.get_expnum();
	      ExprTransformer et(node_id, 
				 proc->procnum, 
				 expnum,
				 cfg, (init_block_after != NULL
				       ? init_block_after 
				       : init_block));
	      et.traverse(new_expr, preorder);
	      for (int z = node_id; z < et.get_node_id(); ++z) {
		block_stmt_map[z] = current_statement;
	      }
	      node_id = et.get_node_id();
	      expnum = et.get_expnum();

	      if (et.get_root_var() != NULL) {
		new_expr = Ir::createVarRefExp(et.get_root_var());
	      }

	      SgVarRefExp *var = Ir::createVarRefExp(*it);
	      SgExprStatement* exprStatement
		= Ir::createExprStatement(Ir::createAssignOp(var, new_expr));
	      init_block->statements.push_front(exprStatement);

	      init_block_after = et.get_after();
	      /* Don't try to understand this, because
	       * It Works By Magic(tm). But feel free
	       * to rewrite it cleanly. */
	      init_block = allocate_new_block(NULL, init_block);
	      init_block->statements.push_front(Ir::createDeclareStmt(Ir::createVariableSymbol(*it),
								(*it)->get_type()));
	      init_block_after = init_block;
	      init_block = NULL;
	    }
	  } else {
	    std::cout << "TODO: for init stmt with "
		      << (*it)->sage_class_name()
		      << std::endl;
	    init_block->statements.push_front(*it);
	  }
	}
	
	// MS: 2007: in ROSE 0.8.10 the for-test is a statement (not an expression anymore)
	SgExprStatement* cond
	  = isSgExprStatement(fors->get_test());
	SgExpression* new_expr
	  = isSgExpression(cond->get_expression()->copy(treecopy));

	assert(new_expr);
	ExprLabeler el(expnum);
	el.traverse(new_expr, preorder);
	expnum = el.get_expnum();
	ExprTransformer et(node_id, 
			   proc->procnum, 
			   expnum,
			   cfg, 
			   for_block);
	et.traverse(new_expr, preorder);
	for (int z = node_id; z < et.get_node_id(); ++z) {
	  block_stmt_map[z] = current_statement;
	}
	node_id = et.get_node_id();
	expnum = et.get_expnum();

	if (et.get_root_var() != NULL) {
	  new_expr = Ir::createVarRefExp(et.get_root_var());
	}
                    
#ifdef REPLACE_FOR_BY_WHILE
	SgWhileStmt* whileStmt
	  = Ir::createWhileStmt(Ir::createExprStatement(new_expr));
	for_block->statements.push_front(whileStmt);

#else
	for_block->statements.push_front(
					 new SgForStatement(info,
							    new SgExprStatement(info, new_expr),
							    (SgExpression *) NULL, NULL));
#endif
	BasicBlock *for_block_after = et.get_after();
	/* create a block for the increment statement */
	block_stmt_map[node_id] = current_statement;
	BasicBlock *incr_block
	  = new BasicBlock(node_id++, INNER, proc->procnum);
	cfg->nodes.push_back(incr_block);

	SgExpression *new_expr_inc
	  = isSgExpression(fors->get_increment()->copy(treecopy));
	ExprLabeler el_inc(expnum);
	el_inc.traverse(new_expr_inc, preorder);
	expnum = el_inc.get_expnum();
	ExprTransformer et_inc(node_id, proc->procnum, expnum, cfg, incr_block);
	et_inc.traverse(new_expr_inc, preorder);
	for (int z = node_id; z < et_inc.get_node_id(); ++z) {
	  block_stmt_map[z] = current_statement;
	}
	node_id = et_inc.get_node_id();
	expnum = et_inc.get_expnum();

	if (et_inc.get_root_var() != NULL) {
	  new_expr_inc = Ir::createVarRefExp(et_inc.get_root_var());
	}
	incr_block->statements.push_front(Ir::createExprStatement(new_expr_inc));
	BasicBlock *incr_block_after = et_inc.get_after();
	/* unfold the body */
	BasicBlock *body = transform_block(fors->get_loop_body(), 
					   incr_block_after,
					   after, 
					   incr_block_after);
	/* link everything together */
	add_link(init_block, for_block_after, NORMAL_EDGE);
	add_link(for_block, body, TRUE_EDGE);
	add_link(for_block, after, FALSE_EDGE);
	add_link(incr_block, for_block_after, NORMAL_EDGE);
                
	/* incoming information is at the incoming edge of
	 * the init block */
	stmt_start = new StatementAttribute(init_block_after, POS_PRE);
	/* Outgoing information is at the incoming edge of
	 * the after block, *not* at the false edge of the
	 * condition block. This is because a break inside
	 * the body might jump directly to after. If there
	 * is no break (or goto or some other jump), the two
	 * edges are the same anyway. */
	stmt_end = new StatementAttribute(after, POS_PRE);
	
	new_block = NULL;
	after = init_block_after;
	
	(*i)->addNewAttribute("PAG statement head",
			      new StatementAttribute(for_block, POS_POST));
      }
	break;

      case V_SgWhileStmt: {
	block_stmt_map[node_id] = current_statement;
	BasicBlock *join_block
	  = new BasicBlock(node_id++, INNER, proc->procnum);
	cfg->nodes.push_back(join_block);
	join_block->statements.push_back(Ir::createWhileJoin());
	add_link(join_block, after, TRUE_EDGE);
	after = join_block;

	SgWhileStmt* whiles = isSgWhileStmt(*i);
	block_stmt_map[node_id] = current_statement;
	BasicBlock* while_block
	  = new BasicBlock(node_id++, INNER, proc->procnum);
	cfg->nodes.push_back(while_block);
                
	SgExprStatement* cond
	  = isSgExprStatement(whiles->get_condition());
	SgExpression* new_expr
		  = isSgExpression(cond->get_expression()->copy(treecopy));
	new_expr->set_parent(NULL);
	ExprLabeler el(expnum);
	el.traverse(new_expr, preorder);
	expnum = el.get_expnum();
	ExprTransformer et(node_id, proc->procnum, expnum,
			   cfg, while_block);
	et.traverse(new_expr, preorder);
	for (int z = node_id; z < et.get_node_id(); ++z) {
	  block_stmt_map[z] = current_statement;
	}
	node_id = et.get_node_id();
	expnum = et.get_expnum();

	if (et.get_root_var() != NULL)
	  new_expr = Ir::createVarRefExp(et.get_root_var());
                
	
	SgWhileStmt* whileStatement=Ir::createWhileStmt(Ir::createExprStatement(new_expr));
	while_block->statements.push_front(whileStatement);
                
	BasicBlock *body = transform_block(whiles->get_body(), et.get_after(),
					   after, et.get_after());

	add_link(while_block, body, TRUE_EDGE);
	add_link(while_block, after, FALSE_EDGE);

	/* incoming information is at the incoming edge of
	 * the code produced by et */
	stmt_start = new StatementAttribute(et.get_after(), POS_PRE);
	/* outgoing information is at the pre edge of the
	 * after block; like with the for statement, this is
	 * because a break might jump directly to this block */
	stmt_end = new StatementAttribute(after, POS_PRE);

	new_block = NULL;
	after = et.get_after();

	(*i)->addNewAttribute("PAG statement head",
			      new StatementAttribute(while_block, POS_POST));
      }
	break;

      case V_SgDoWhileStmt: {
	SgDoWhileStmt *dowhiles = isSgDoWhileStmt(*i);
	block_stmt_map[node_id] = current_statement;
	BasicBlock *dowhile_block
	  = new BasicBlock(node_id++, INNER, proc->procnum);
	cfg->nodes.push_back(dowhile_block);
	
	SgExprStatement *cond
	  = isSgExprStatement(dowhiles->get_condition());
	SgExpression *new_expr
	  = isSgExpression(cond->get_expression()->copy(treecopy));
	new_expr->set_parent(NULL);
	ExprLabeler el(expnum);
	el.traverse(new_expr, preorder);
	expnum = el.get_expnum();
	ExprTransformer et(node_id, proc->procnum, expnum,
			   cfg, dowhile_block);
	et.traverse(new_expr, preorder);
	for (int z = node_id; z < et.get_node_id(); ++z) {
                    block_stmt_map[z] = current_statement;
	}
	node_id = et.get_node_id();
	expnum = et.get_expnum();

	if (et.get_root_var() != NULL) {
	  new_expr = Ir::createVarRefExp(et.get_root_var());
	}
                
	SgDoWhileStmt* doWhileStmt
	  = Ir::createDoWhileStmt(Ir::createExprStatement(new_expr));
	dowhile_block->statements.push_front(doWhileStmt);
	
	BasicBlock* body
	  = transform_block(dowhiles->get_body(), et.get_after(),
                            after, et.get_after());
	add_link(dowhile_block, body, TRUE_EDGE);
	add_link(dowhile_block, after, FALSE_EDGE);
	
	/* incoming analysis information is at the beginning
	 * of the body */
	stmt_start = new StatementAttribute(body, POS_PRE);
	/* outgoing information is at the incoming edge of
	 * the after block; as with the other loops, this is
	 * because of possible break statements */
	stmt_end = new StatementAttribute(after, POS_PRE);
	
	new_block = NULL;
	after = body;
	
	(*i)->addNewAttribute("PAG statement head",
			      new StatementAttribute(dowhile_block, POS_POST));
      }
	break;
	
      case V_SgBreakStmt: {
	SgBreakStmt *breaks = isSgBreakStmt(*i);
	if (new_block == NULL) {
	  block_stmt_map[node_id] = current_statement;
	  new_block
	    = new BasicBlock(node_id++, INNER, proc->procnum);
	  cfg->nodes.push_back(new_block);
	}
	BasicBlock *break_block = new_block;
	break_block->statements.push_front(breaks);
	add_link(break_block, break_target, NORMAL_EDGE);

	/* incoming information is at the incoming edge of
	 * the break block */
	stmt_start = new StatementAttribute(break_block, POS_PRE);
	/* outgoing information is at the outgoing edge of
	 * the break block */
	stmt_end = new StatementAttribute(break_block, POS_POST);

	new_block = NULL;
	after = break_block;
      }
	break;

      case V_SgContinueStmt: {
	SgContinueStmt *continues = isSgContinueStmt(*i);
	if (new_block == NULL) {
	  block_stmt_map[node_id] = current_statement;
	  new_block
	    = new BasicBlock(node_id++, INNER, proc->procnum);
	  cfg->nodes.push_back(new_block);
	}
	BasicBlock *continue_block = new_block;
	continue_block->statements.push_front(continues);
	add_link(continue_block, continue_target, NORMAL_EDGE);

	/* incoming information is at the incoming edge of
	 * the continue block */
	stmt_start = new StatementAttribute(continue_block, POS_PRE);
	/* outgoing information is at the outgoing edge of
	 * the continue block */
	stmt_end = new StatementAttribute(continue_block, POS_POST);
	
	after = continue_block;
      }
            break;
      case V_SgSwitchStatement: {
	SgSwitchStatement *switchs = isSgSwitchStatement(*i);
	block_stmt_map[node_id] = current_statement;
	BasicBlock *switch_block
	  = new BasicBlock(node_id++, INNER, proc->procnum);
	cfg->nodes.push_back(switch_block);
	
	// SgExpression *new_expr
	//     = isSgExpression(switchs->get_item_selector_root()
	//             ->get_operand_i()->copy(treecopy));
	SgStatement *item_sel = switchs->get_item_selector();
	SgExpression *new_expr;
	if (isSgExprStatement(item_sel)) {
	  new_expr = isSgExprStatement(item_sel)->get_expression();
	} else {
	  std::cout << "TODO: unsupported statement type in switch"
		    << std::endl;
	  new_expr = NULL;
	}
	new_expr->set_parent(NULL);

	ExprLabeler el(expnum);
	el.traverse(new_expr, preorder);
	expnum = el.get_expnum();
	ExprTransformer et(node_id, proc->procnum, expnum,
			   cfg, switch_block);
	et.traverse(new_expr, preorder);
	for (int z = node_id; z < et.get_node_id(); ++z) {
	  block_stmt_map[z] = current_statement;
	}
	node_id = et.get_node_id();
	expnum = et.get_expnum();

	if (et.get_root_var() != NULL) {
	  new_expr = Ir::createVarRefExp(et.get_root_var());
	}
	
	SgSwitchStatement* switchStatement
	  = Ir::createSwitchStatement(Ir::createExprStatement(new_expr));
	switch_block->statements.push_front(switchStatement);
                
	BlockList* body
	  = do_switch_body(switchs->get_body(), after,
			   continue_target);
	BlockList::const_iterator bli;
	bool default_case = false;
	for (bli = body->begin(); bli != body->end(); ++bli) {
	  if (isSgDefaultOptionStmt((*bli)->statements[0])
	      || isSgCaseOptionStmt((*bli)->statements[0])) {
	    add_link(switch_block, *bli, NORMAL_EDGE);
	  }
	  if (isSgDefaultOptionStmt((*bli)->statements[0])) {
	    default_case = true;
	  }
	}
	if (!default_case) {
	  add_link(switch_block, after, NORMAL_EDGE);
	}

	/* incoming information is at the incoming edge of
	 * the code produced by et */
	stmt_start = new StatementAttribute(et.get_after(), POS_PRE);
	/* outgoing information is at the incoming edge of
	 * the after block, where all paths run together */
	stmt_end = new StatementAttribute(after, POS_PRE);

	new_block = NULL; //switch_block;
	after = et.get_after(); //switch_block;
	
	(*i)->addNewAttribute("PAG statement head",
			      new StatementAttribute(switch_block, POS_POST));
      }
	break;

      case V_SgBasicBlock: {
	SgBasicBlock *block = isSgBasicBlock(*i);
	BasicBlock *body = transform_block(block, 
					   after,
					   break_target, 
					   continue_target);

	/* incoming information is at the body's incoming
	 * edge */
	stmt_start = new StatementAttribute(body, POS_PRE);
	/* outgoing information is usually theoretically at
	 * the outgoing edge of the last statement of the
	 * block. if that statement is unreachable... hmm
	 * also, if the block is empty, it is a no-op, so we
	 * use the pre-info as post-info */
	if (!block->get_statements().empty()) {
	  stmt_end = dynamic_cast<StatementAttribute *>(block->get_statements().back()
							->getAttribute("PAG statement end"));
	} else {
	  stmt_end = stmt_start;
	}
	
	new_block = NULL;
	after = body;
      }
	break;

      case V_SgReturnStmt: {
	SgReturnStmt *returns = isSgReturnStmt(*i);
                
	/* If the following statement is an UndeclareStmt or
	 * an ArgumentAssignment for the end of this block
	 * (his is determined by last_block_of_this_block),
	 * go there; otherwise jump to the exit node of the
	 * procedure. */
	if (last_block_of_this_block != NULL) {
	  new_block = allocate_new_block(new_block,
					 last_block_of_this_block);
	} else {
	  new_block = allocate_new_block(new_block, proc->exit);
	}

	if (returns->get_expression() == NULL) {
	  new_block->statements.push_front(returns);
	  after = new_block;
	  new_block = NULL;
	  break;
	}
	SgExpression *new_expr 
	  = isSgExpression(returns->get_expression()->copy(treecopy));
	new_expr->set_parent(NULL);
	ExprLabeler el(expnum);
	el.traverse(new_expr, preorder);
	expnum = el.get_expnum();
	ExprTransformer et(node_id, proc->procnum, expnum,
			   cfg, new_block);
	et.traverse(new_expr, preorder);
	for (int z = node_id; z < et.get_node_id(); ++z) {
	  block_stmt_map[z] = current_statement;
	}
	node_id = et.get_node_id();
	expnum = et.get_expnum();

	if (et.get_root_var() != NULL) {
	  new_expr = Ir::createVarRefExp(et.get_root_var());
	}

	SgVarRefExp* varref1
	  = Ir::createVarRefExp(proc->returnvar);
	SgExprStatement* exprstat
	  = Ir::createExprStatement(Ir::createAssignOp(varref1, new_expr));
	new_block->statements.push_front(exprstat);
	
	/* incoming information is at the incoming edge of
	 * the code produced by et */
	stmt_start = new StatementAttribute(et.get_after(), POS_PRE);
	/* outgoing information is at the outgoing edge of
	 * the return block, i.e. new_block */
	stmt_end = new StatementAttribute(new_block, POS_POST);
	
	if (after != et.get_after()) {
	  after = et.get_after();
	  new_block = NULL;
	}
      }
	break;

      case V_SgVariableDeclaration: {
	stmt_end = new StatementAttribute(after, POS_PRE);
	SgVariableDeclaration *vardecl
	  = isSgVariableDeclaration(*i);
	SgInitializedNamePtrList list = vardecl->get_variables();
	SgInitializedNamePtrList::const_iterator j;
	for (j = list.begin(); j != list.end(); ++j) {
	  SgVariableSymbol* declared_var 
	    = Ir::createVariableSymbol((*j)->get_name(),
				       (*j)->get_type());
	  proc->exit->paramlist->push_back(declared_var);
	  SgAssignInitializer *initializer
	    = isSgAssignInitializer((*j)->get_initptr());
	  SgAggregateInitializer *agg_init
	    = isSgAggregateInitializer((*j)->get_initptr());
	  SgConstructorInitializer *constr_init
	    = isSgConstructorInitializer((*j)->get_initptr());

   // GB (2008-02-14): Unified the "normal" and "aggregate" initializer
   // cases. Even "normal" initializers are now wrapped in a
   // SgAssignInitializer node! Analysis specification must be aware.
      SgTreeCopy treecopy;
      SgExpression *new_expr = NULL;
      if (agg_init)
          new_expr = isSgExpression(agg_init->copy(treecopy));
      else if (initializer)
          new_expr = isSgExpression(initializer->copy(treecopy));

	  if (new_expr) {
          new_expr->set_parent(NULL);

          ExprLabeler el(expnum);
          el.traverse(new_expr, preorder);
          expnum = el.get_expnum();
          new_block = allocate_new_block(new_block, after);
          ExprTransformer et(node_id, proc->procnum, expnum, cfg, new_block);
          et.traverse(new_expr, preorder);
          for (int z = node_id; z < et.get_node_id(); ++z)
              block_stmt_map[z] = current_statement;
          node_id = et.get_node_id();
          expnum = et.get_expnum();
          after = et.get_after();
          stmt_start = new StatementAttribute(after, POS_PRE);

          if (et.get_root_var() != NULL)
              new_expr = Ir::createVarRefExp(et.get_root_var());
          SgExprStatement *expstmt
              = Ir::createExprStatement(Ir::createAssignOp(
                          Ir::createVarRefExp(declared_var), new_expr));
          new_block->statements.push_front(expstmt);
          new_block = NULL;
	  } else if (constr_init) {
     // GB (2008-02-14): It's not clear to me why this case is almost, but
     // not entirely, identical to the other cases. Could we merge these?
     // TODO: investigate!
	    new_expr = isSgExpression(constr_init->copy(treecopy));
	    new_expr->set_parent(*j);

	    ExprLabeler el(expnum);
	    el.traverse(new_expr, preorder);
	    expnum = el.get_expnum();
	    //new_block = allocate_new_block(new_block, after);
	    ExprTransformer et(node_id, proc->procnum, expnum,
			       cfg, after);
	    et.traverse(new_expr, preorder);
	    for (int z = node_id; z < et.get_node_id(); ++z) {
	      block_stmt_map[z] = current_statement;
	    }
	    node_id = et.get_node_id();
	    expnum = et.get_expnum();
	    /* incoming information at the incoming edge
	     * of et.after, outgoing on the incoming
	     * edge of after */
	    stmt_start
	      = new StatementAttribute(et.get_after(), POS_PRE);
	    after = et.get_after();
                
	    /*
                        if (et.get_root_var() != NULL)
                            new_expr = new SgVarRefExp(info,
                                et.get_root_var());
                        new_block->statements.push_front(
                            new SgExprStatement(info, new SgAssignOp(info,
                                new SgVarRefExp(info, declared_var),
                                new_expr)));
                        new_block = NULL;
	    */
	  } else {
	    /* if we do not produce a block, the
	     * declaration is a no-op, so we can just
	     * use the post info as pre info */
	    stmt_start = stmt_end;
	  }
	  /* declare the variable */
	  new_block = allocate_new_block(new_block, after);
	  new_block->statements.push_front(Ir::createDeclareStmt(declared_var,
							   declared_var->get_type()));
	  after = new_block;
	  new_block = NULL;
	}
      }
	break;

      case V_SgLabelStatement: {
	SgLabelStatement *ls = isSgLabelStatement(*i);
	std::string label = std::string(ls->get_label().str());
	new_block = allocate_new_block(new_block, after);
	new_block->statements.push_front(ls);

	proc->goto_targets[label] = new_block;
                
	/* incoming information is at the incoming edge of
	 * new_block */
	stmt_start = new StatementAttribute(new_block, POS_PRE);
	/* outgoing information is at the outgoing edge of
	 * new_block */
	stmt_end = new StatementAttribute(new_block, POS_POST);

	after = new_block;
	new_block = NULL;
      }
	break;

      case V_SgGotoStatement: {
	SgGotoStatement *gs = isSgGotoStatement(*i);
	std::string label
	  = std::string(gs->get_label()->get_label().str());
	new_block = allocate_block_without_successor(new_block);
	new_block->statements.push_front(gs);

	proc->goto_blocks.insert(std::pair<std::string, BasicBlock *>(label, new_block));
                
	/* incoming information is at the incoming edge of
	 * new_block */
	stmt_start = new StatementAttribute(new_block, POS_PRE);
	/* outgoing: TODO, insert it in
	 * perform_goto_backpatching */
	// stmt_end = new StatementAttribute();

	after = new_block;
	new_block = NULL;
      }
	break;

      case V_SgPragmaDeclaration: /* Adrian: Ignore Pragma Decls */
      case V_SgNullStatement: {
	/*
	  new_block = allocate_new_block(new_block, after);
	  after = new_block;
	  new_block->statements.push_front(*i);
	  new_block = NULL;
	*/
                
	/* incoming and outgoing information are identical,
	 * as this statement does nothing and is not even
	 * modeled in the CFG. Thus we use the pre
	 * information of the after block. */
	stmt_start = stmt_end = new StatementAttribute(after, POS_PRE);
      }
	break;

      default:
	/* A statement which is not yet handled in a special
	 * way. Print its class name and then fall through to
	 * treat it as a "normal" (non-branching, non-special)
	 * statement. */
	std::cout << "TODO: not supported -> " <<
	  (*i)->sage_class_name() << std::endl;
	/* fall through */

      case V_SgExprStatement:
	SgExprStatement *exprs = isSgExprStatement(*i);
	if (exprs != NULL) {
	  SgTreeCopy treecopy;
	  SgExpression *new_expr
	    = isSgExpression(exprs->get_expression()->copy(treecopy));
	  new_expr->set_parent(NULL);
	  
	  ExprLabeler el(expnum);
	  el.traverse(new_expr, preorder);
	  expnum = el.get_expnum();
	  new_block = allocate_new_block(new_block, after);
	  ExprTransformer et(node_id, proc->procnum, expnum,
			     cfg, new_block);
	  et.traverse(new_expr, preorder);
	  for (int z = node_id; z < et.get_node_id(); ++z) {
                    block_stmt_map[z] = current_statement;
	  }
	  node_id = et.get_node_id();
	  expnum = et.get_expnum();
                
	  /* incoming information is at the incoming edge of
	   * the code produced by et */
	  stmt_start = new StatementAttribute(et.get_after(), POS_PRE);
	  /* outgoing information is at the outgoing edge of
	   * new_block, the end of the evaluation of the
	   * expression */
	  stmt_end = new StatementAttribute(new_block, POS_POST);
	  
	  after = et.get_after();
	  
	  if (et.get_root_var() != NULL)
	    new_expr = Ir::createVarRefExp(et.get_root_var());
	  //if (!isSgTypeVoid(new_expr->get_type()))
	  {
	    new_block->statements.push_front(Ir::createExprStatement(new_expr));
	    new_block = NULL;
	  }
	} else {
	  new_block = allocate_new_block(new_block, after);
	  after = new_block;
	  new_block->statements.push_front(*i);
	  new_block = NULL;
                
	  /* incoming information is at the incoming edge of
	   * new_block */
	  stmt_start = new StatementAttribute(new_block, POS_PRE);
	  /* outgoing information is at the outgoing edge of
	   * new_block */
	  stmt_end = new StatementAttribute(new_block, POS_POST);
	}
	break;
      }

      /* if for some reason the attributes were not computed
       * above, insert dummies (because attributes may not be
       * null) */
      if (stmt_start == NULL)
	stmt_start = new StatementAttribute(NULL, POS_PRE);
      (*i)->addNewAttribute("PAG statement start", stmt_start);
      if (stmt_end == NULL)
	stmt_end = new StatementAttribute(NULL, POS_PRE);
      (*i)->addNewAttribute("PAG statement end", stmt_end);
    }
    
    return after;
}

BlockList* 
CFGTraversal::do_switch_body(SgBasicBlock *block,
			     BasicBlock *after, 
			     BasicBlock *continue_target) {
  BlockList *blocks = new BlockList();
  SgStatementPtrList stmts = block->get_statements();
  SgBasicBlock *spare_block = NULL;
  
  BasicBlock *previous = after;
  SgStatementPtrList::reverse_iterator i;
  for (i = stmts.rbegin(); i != stmts.rend(); ++i) {
    switch ((*i)->variantT()) {
    case V_SgCaseOptionStmt: {
      if (spare_block != NULL) {
	previous = transform_block(spare_block, previous,
				   after, continue_target);
	delete spare_block;
	spare_block = NULL;
      }
      SgCaseOptionStmt *cases = isSgCaseOptionStmt(*i);
      /* transform this block */
      previous = transform_block(cases->get_body(),
				 previous, after, continue_target);
      /*
	previous->statements.push_front(cases);
	blocks->push_front(previous);
      */
      BasicBlock *case_block = allocate_new_block(NULL, previous);
      case_block->statements.push_front(cases);
      blocks->push_front(case_block);
      
      /* pre info is the pre info of the first block in
       * the case body */
      StatementAttribute *stmt_start =
	new StatementAttribute(previous, POS_PRE);
      cases->addNewAttribute("PAG statement start", stmt_start);
      /* post info is the post info of the last statement
       * in the case body; if the body is empty, it is the
       * pre info */
      if (!cases->get_body()->get_statements().empty()) {
	AstAttribute* stmt_end =
	  cases->get_body()->get_statements().back()->getAttribute("PAG statement end");
	cases->addNewAttribute("PAG statement end", stmt_end);
      } else {
	cases->addNewAttribute("PAG statement end", stmt_start);
      }
    }
      break;

    case V_SgDefaultOptionStmt: {
      if (spare_block != NULL) {
	previous = transform_block(spare_block, previous,
				   after, continue_target);
	delete spare_block;
	spare_block = NULL;
      }
                
      SgDefaultOptionStmt *defaults = isSgDefaultOptionStmt(*i);
      /* post info is before the following block */
      StatementAttribute *stmt_end =
	new StatementAttribute(previous, POS_PRE);
      defaults->addNewAttribute("PAG statement end", stmt_end);
      defaults->get_body()->addNewAttribute("PAG statement end", stmt_end);
      /* transform this block */
      previous = transform_block(defaults->get_body(),
				 previous, after, continue_target);
      /* pre info is the pre info of the first block in
       * the case body */
      StatementAttribute *stmt_start =
	new StatementAttribute(previous, POS_PRE);
      defaults->addNewAttribute("PAG statement start", stmt_start);
      defaults->get_body()
	->addNewAttribute("PAG statement start", stmt_start);
      /*
	previous->statements.push_front(defaults);
	blocks->push_front(previous);
      */
      BasicBlock *case_block = allocate_new_block(NULL, previous);
      case_block->statements.push_front(defaults);
      blocks->push_front(case_block);
    }
      break;
    default:
      if (spare_block == NULL) {
	spare_block = new SgBasicBlock(NULL, *i);
      } else {
	spare_block->prepend_statement(*i);
      }
      break;
    }
  }
  return blocks;
}

int 
CFGTraversal::find_procnum(const char *name) const
{
  int num = 0;
  std::deque<Procedure *>::const_iterator i;
  
  for (i = cfg->procedures->begin(); i != cfg->procedures->end(); ++i) {
    if (strcmp(name, (*i)->name) == 0) {
      return num;
    }
    num++;
  }

  return -1;
}

BasicBlock*
CFGTraversal::allocate_new_block(BasicBlock *new_block,
				 BasicBlock *after)
{
    /* if (new_block == NULL)
    {
        new_block = new BasicBlock(node_id++, INNER, proc->procnum);
        cfg->nodes.push_back(new_block);
    } */
  new_block = allocate_block_without_successor(new_block);
  if (new_block->statements.size() == 0) {
    add_link(new_block, after, NORMAL_EDGE);
  }
  
  return new_block;
}

BasicBlock*
CFGTraversal::allocate_block_without_successor(BasicBlock *nb) {
  if (nb == NULL) {
    if (current_statement != NULL)
      block_stmt_map[node_id] = current_statement;
    nb = new BasicBlock(node_id++, INNER, proc->procnum);
    cfg->nodes.push_back(nb);
  }

  return nb;
}

bool 
CFGTraversal::is_destructor_decl(SgFunctionDeclaration *fd) const {
  SgMemberFunctionDeclaration *mfd = isSgMemberFunctionDeclaration(fd);
  if (mfd && mfd->get_name().str()[0] == '~')
    return true;
  else
    return false;
}

BasicBlock* 
CFGTraversal::call_base_destructors(Procedure *p, BasicBlock *after) {
  std::vector<CallBlock *> blocks;
  std::deque<Procedure *>::const_iterator pi;
  std::vector<SgBaseClass *>::iterator base;
  for (base = p->class_type->get_inheritances().begin();
       base != p->class_type->get_inheritances().end();
       ++base) {
    /* find implementation of destructor among our procedures
     * (i.e. functions and methods we have implementations for);
     * collect their entry points in list blocks */
    for (pi = cfg->procedures->begin(); pi != cfg->procedures->end(); ++pi) {
      if ((*pi)->name[0] == '~'
	  && (*pi)->class_type
	  == (*base)->get_base_class()->get_definition()) {
	blocks.push_back((*pi)->entry);
	break;
      }
    }
  }
  std::vector<CallBlock *>::iterator bi;
  for (bi = blocks.begin(); bi != blocks.end(); ++bi) {
    std::string class_name((*cfg->procedures)[(*bi)->procnum]->name + 1);
    std::string destructor_name = class_name + "::~" + class_name;
    std::string this_var_name = std::string() + "$~" + class_name + "$this";

    SgPointerType* ptrType
      = Ir::createPointerType(p->class_type->get_declaration()->get_type());
    SgVariableSymbol* this_var_sym
      = Ir::createVariableSymbol(this_var_name,ptrType);
    SgVariableSymbol* this_sym 
      = Ir::createVariableSymbol("this", ptrType);

    CallBlock *call_block = new CallBlock(node_id++, CALL,
					  p->procnum, NULL, strdup(destructor_name.c_str()));
    CallBlock *return_block = new CallBlock(node_id++, RETURN,
					    p->procnum, NULL, strdup(destructor_name.c_str()));
    cfg->nodes.push_back(call_block);
    cfg->calls.push_back(call_block);
    cfg->nodes.push_back(return_block);
    cfg->returns.push_back(return_block);
    call_block->partner = return_block;
    return_block->partner = call_block;
    BasicBlock *this_block
      = new BasicBlock(node_id++, INNER, p->procnum);
    cfg->nodes.push_back(this_block);
    this_block->statements.push_back(new ArgumentAssignment(Ir::createVarRefExp(this_var_sym),
							    Ir::createVarRefExp(this_sym)));
    /* set links */
    add_link(this_block, call_block, NORMAL_EDGE);
    add_link(call_block, *bi, CALL_EDGE);
    add_link((*bi)->partner, return_block, RETURN_EDGE);
    add_link(call_block, return_block, LOCAL);
    add_link(return_block, after, NORMAL_EDGE);
    after = this_block;
    //last = return_block;
  }
  if (blocks.empty() && !p->class_type->get_inheritances().empty()) {
    /* no destructor implementation found; generate
     * DestructorCall node */
    BasicBlock *b = new BasicBlock(node_id++, INNER, p->procnum);
    cfg->nodes.push_back(b);
    b->statements.push_back(new DestructorCall(p->class_type->get_declaration()->get_name().str(),
					       p->class_type->get_declaration()->get_type()));
    add_link(b, after, NORMAL_EDGE);
    after = b;
  }

  return after;
}

void 
CFGTraversal::print_map() const {
  std::map<int, SgStatement *>::const_iterator i;
  for (i = block_stmt_map.begin(); i != block_stmt_map.end(); ++i) {
    std::cout << "block " << std::setw(4) << i->first
	      << " stmt " << i->second << ": "
	      << Ir::fragmentToString((i->second)) << std::endl;
  }
}
