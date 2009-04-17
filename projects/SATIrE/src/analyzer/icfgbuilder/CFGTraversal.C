// Copyright 2005,2006,2007,2008 Markus Schordan, Gergo Barany
// $Id: CFGTraversal.C,v 1.56 2009-02-11 10:03:44 gergo Exp $

#include <iostream>
#include <string.h>

#include <satire_rose.h>
#include "patternRewrite.h"

#include "CFGTraversal.h"
#include "ExprLabeler.h"
#include "ExprTransformer.h"
#include "analysis_info.h"
#include "IrCreation.h"
#include "EqualityTraversal.h"

#define REPLACE_FOR_BY_WHILE

// CFGTraversal::CFGTraversal(std::deque<Procedure *> *procs)
CFGTraversal::CFGTraversal(ProcTraversal &p, AnalyzerOptions *options)
    : node_id(0), procnum(0), cfg(new CFG()), real_cfg(NULL), proc(NULL),
      call_num(0), lognum(0), expnum(0), current_statement(NULL),
      traversalTimer(NULL), flag_numberExpressions(true)
{
  TimingPerformance timer("Initial setup of ICFG (entry, exit, argument nodes):");
  cfg->procedures = p.get_procedures();
  cfg->proc_map = p.proc_map;
  cfg->mangled_proc_map = p.mangled_proc_map;
  cfg->analyzerOptions = options;

  cfg->global_return_variable_symbol = p.global_return_variable_symbol;
  cfg->global_argument_variable_symbols = p.global_argument_variable_symbols;
  cfg->global_this_variable_symbol = p.global_this_variable_symbol;
  cfg->global_unknown_type = p.global_unknown_type;

// GB (2008-05-05): Refactored.
  setProcedureEndNodes();
  processProcedureArgBlocks();
}

// This method creates the ICFG nodes for assigning global parameter
// variables to each function's parameters. The necessary information about
// function parameters is stored in each procedure's arg_block which was set
// by the ProcTraversal.
void
CFGTraversal::processProcedureArgBlocks()
{
  std::deque<Procedure *> *procs = cfg->procedures;
  std::deque<Procedure *>::const_iterator i;
  for (i = procs->begin(); i != procs->end(); ++i) {
    if ((*i)->arg_block != NULL) {
      std::deque<SgStatement *>::const_iterator j;
      BasicBlock *b, *prev = NULL, *first = NULL;
   // GB (2008-10-21): The call_index variable is used to number each
   // function's param assignments from 0 to n.
      int call_index = 0;
      for (j = (*i)->arg_block->statements.begin();
           j != (*i)->arg_block->statements.end(); ++j) {
        /* deal with constructor initializers */
        ArgumentAssignment* aa
          = dynamic_cast<ArgumentAssignment *>(*j);
        if (aa && isSgConstructorInitializer(aa->get_rhs())) {
          SgExpression* new_expr= isSgExpression(Ir::deepCopy(aa->get_rhs()));
#if 0
          ExprLabeler el(expnum);
          el.traverse(new_expr, preorder);
          expnum = el.get_expnum();
          ExprTransformer et(node_id, (*i)->procnum, expnum, cfg, NULL);
          et.traverse(new_expr, preorder);
          for (int z = node_id; z < et.get_node_id(); ++z) {
            cfg->registerStatementLabel(z, current_statement);
          }
#else
          ExprTransformer et(node_id, (*i)->procnum, expnum, cfg, NULL,
                  current_statement);
          et.labelAndTransformExpression(new_expr, aa->get_rhs());
#endif
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
                      ->get_declaration()->get_type());
           // GB (2008-05-26): To ensure that symbol pointers are unique,
           // and to avoid creating the same expression over and over again,
           // the this pointer symbol and expression are now stored in the
           // procedure.
           // SgVarRefExp* thisVarRefExp =Ir::createVarRefExp("this",ptrType);
              SgVarRefExp* thisVarRefExp = p->this_exp;
              f->statements[0] = Ir::createArgumentAssignment(a->get_lhs(),
                                                              thisVarRefExp);
           // GB (2008-04-02): Now that we have created a new argument
           // assignment, the old ones are no longer needed.
              delete aa;
              delete a;
            }
          }
        } else {
          /* empty block for param assignments */
          b = new BasicBlock(node_id++, INNER, (*i)->procnum);
          cfg->nodes.push_back(b);
          b->statements.push_back(*j);
          b->call_target = (*i)->arg_block->call_target;
          b->call_index = call_index++;
          if (first == NULL)
            first = b;
          if (prev != NULL)
            add_link(prev, b, NORMAL_EDGE);
          prev = b;
        }
      }
   // GB (2008-04-02): We can empty the arg_block now; it was only temporary
   // storage for some statements. Those statements have now either been
   // freed, or copied somewhere else where another component will be
   // responsible for deleting them at the appropriate time.
      (*i)->arg_block->statements.clear();

      (*i)->first_arg_block = first;
      (*i)->last_arg_block = b;
    }
  }
}

// This methods inserts the entry and exit nodes for each procedure into the
// ICFG. These nodes were computed by the ProcTraversal and are accessible
// via cfg->procedures.
void
CFGTraversal::setProcedureEndNodes()
{
  std::deque<Procedure *> *procs = cfg->procedures;
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
}

extern CFG *global_cfg;

void
CFGTraversal::atTraversalStart() {
    traversalTimer = new TimingPerformance("Traversal to construct ICFG:");
}

#include "pointsto.h"
          
void
CFGTraversal::atTraversalEnd() {
    if (traversalTimer != NULL)
        delete traversalTimer;
    traversalTimer = NULL;

    global_cfg = cfg;

 // GB (2008-03-27): Moved this section of code from getCFG(). Logically,
 // this is where it belongs. (The comment is preserved for historical
 // reasons.)
 // GB (2008-03-05): Execute these functions exactly once, when getCFG is
 // first called. This is the case when real_cfg == NULL.
    if (real_cfg == NULL)
    {
        TimingPerformance timer("Final touches on the ICFG:");
        perform_goto_backpatching();
     // GB (2008-03-10): This new function removes unreachable nodes from the
     // CFG. It does not remove unreachable cycles because it is meant to be
     // somewhat efficient, and because we don't seem to have a problem with
     // unreachable cycles.
     // GB (2008-04-23): After lots of trouble, removed the removal of
     // unreachable nodes.
     // kill_unreachable_nodes();

     // GB (2008-05-30): NULL-terminate the CFG's node lists.
        cfg->nodes.push_back(NULL);
        cfg->entries.push_back(NULL);
        cfg->exits.push_back(NULL);
        cfg->calls.push_back(NULL);
        cfg->returns.push_back(NULL);

     // If the user asked for resolution of function pointers, we must force
     // points-to analysis.
        if (cfg->analyzerOptions->resolveFuncPtrCalls())
            cfg->analyzerOptions->runPointsToAnalysisOn();

     // GB (2008-11-11): Added points-to analysis to the ICFG.
        if (cfg->analyzerOptions->runPointsToAnalysis())
        {
            cfg->pointsToAnalysis = new SATIrE::Analyses::PointsToAnalysis();
            cfg->pointsToAnalysis->run(cfg);
         // GB (2009-03-13): Added context-sensitive points-to analysis. The
         // analyzer object is initialized here, but it is not run until
         // later (because PAG must have computed its mappings before).
            cfg->contextSensitivePointsToAnalysis
                = new SATIrE::Analyses::PointsToAnalysis(/* ctxsens = */ true);
        }
        else
            cfg->pointsToAnalysis = NULL;

     // GB (2008-11-14): Added the option of resolving external calls due to
     // function pointers in the ICFG.
        if (cfg->analyzerOptions->resolveFuncPtrCalls())
        {
            IcfgExternalCallResolver iecr;
            iecr.run(cfg);
        }

     // GB (2008-04-08): Made numbering of expressions optional, but
     // default.
        if (flag_numberExpressions)
            number_exprs();
    }
}

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
  TimingPerformance timer("goto backpatching:");
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
        std::cerr << "warning: goto statement with unknown label!"
                  << std::endl;
      }
      ++gotos;
    }
  }
}

#if 0
void
CFGTraversal::kill_unreachable_nodes() {
    TimingPerformance timer("Elimination of unreachable nodes:");
 // GB (2008-03-10): Eliminate unreachable nodes from the CFG. We do not do
 // a complete reachability test, but rather check for nodes without
 // predecessors and remove those and possibly their successors. This means
 // that unreachable cycles are not found, but these don't appear to be a
 // pressing problem at the moment.
    BlockList worklist;
    BlockList::iterator n;
 // Copy old node list to a temporary.
    BlockList old_nodes = cfg->nodes;
 // Add all inner nodes (i.e. not function entry, exit, call, return)
 // without predecessors to the worklist. These are definitely unreachable.
    for (n = old_nodes.begin(); n != old_nodes.end(); ++n) {
#if 0
        std::cout << "examining node " << (*n)->id << std::endl;
        std::cout << "node has " << (*n)->predecessors.size() << " preds"
            << std::endl;
        std::cout << "inner node? "
            << ((*n)->node_type == INNER ? "yes" : "no")
            << std::endl;
#endif
        if ((*n)->predecessors.empty() && (*n)->node_type == INNER) {
            worklist.push_back(*n);
#if 0
            std::cout << "added node " << (*n)->id << " to initial worklist"
                << std::endl;
#endif
        }
    }
 // Process the worklist, adding unreachable successors of unreachable
 // nodes.
    while (!worklist.empty()) {
        BasicBlock *b = worklist.front();
        worklist.pop_front();
        b->reachable = false;
        std::vector<Edge>::iterator e;
        for (e = b->successors.begin(); e != b->successors.end(); ++e) {
         // If the successor has only one predecessor, that predecessor must
         // be the unreachable block b. Thus, the successor is itself
         // unreachable.
            BasicBlock *succ = e->first;
            KFG_EDGE_TYPE etype = e->second;
            if (succ->predecessors.size() == 1 && succ->node_type == INNER)
                worklist.push_back(succ);
         // In any case, remove the reverse edge from the successor to b.
         // The edge from b to the successor will be removed when b is
         // destroyed. We remove by finding the edge to b (of the
         // appropriate type) and calling the erase method.
            std::vector<Edge>::iterator pos;
            pos = std::find(succ->predecessors.begin(),
                            succ->predecessors.end(),
                            std::make_pair(b, etype));
            if (pos != succ->predecessors.end())
                succ->predecessors.erase(pos);
        }
    }
 // Clear original node list; we will refill it right away, but only with
 // reachable nodes.
    cfg->nodes.clear();
    KFG_NODE_ID id = 0;
 // Iterate over copied node list and copy back those that are not marked
 // unreachable. Renumber nodes using id.
    for (n = old_nodes.begin(); n != old_nodes.end(); ++n) {
        if ((*n)->reachable) {
#if 0
            std::cout << "renumbering old node "
                << (void *) *n << " " << (*n)->id
                << " " << Ir::fragmentToString((*n)->statements.front())
                << "   to: " << id << std::endl;
#endif
            (*n)->id = id++;
            cfg->nodes.push_back(*n);
        }
        else
        {
#if 0
            std::cout << "*** collecting garbage node " << (*n)->id
                << std::endl;
            if ((*n)->statements.size() != 1)
            {
                std::cout << "weird! block contains "
                    << (*n)->statements.size() << " statements"
                    << std::endl;
                delete *n;
                continue;
            }
            else
            {
                SgStatement *stmt = (*n)->statements.front();
                std::cout << "*** stmt: "
                    << (void *) stmt << " "
                    << Ir::fragmentToString(stmt)
                    << std::endl;
            }
#endif
         // GB (2008-04-09): Remove attributes from unreachable statements.
         // SgStatement *stmt = (*n)->statements.front();
            SgStatement *stmt;
         // The statement in question might be transformed, so see if we can
         // get the original statement through the block_stmt_map.
            if (block_stmt_map!(*n)->id! != NULL)
                stmt = block_stmt_map!(*n)->id!;
            else
                stmt = (*n)->statements.front();

         // In each of these cases, we should delete a, but it might be
         // shared, so let's not do it.
            if (stmt->attributeExists("PAG statement start"))
            {
                AstAttribute *a = stmt->getAttribute("PAG statement start");
                stmt->removeAttribute("PAG statement start");
            }
            if (stmt->attributeExists("PAG statement end"))
            {
                AstAttribute *a = stmt->getAttribute("PAG statement end");
                stmt->removeAttribute("PAG statement end");
            }
            if (stmt->attributeExists("PAG statement head"))
            {
                AstAttribute *a = stmt->getAttribute("PAG statement head");
                stmt->removeAttribute("PAG statement head");
            }

         // Negate node id; this might make it easier to recognize in debug
         // output.
            (*n)->id = -(*n)->id;

            delete *n;
        }
    }
 // Note that the block_stmt_map is broken after renumbering! Sorry about
 // that.

#if 0
 // dump new ICFG
    for (n = cfg->nodes.begin(); n != cfg->nodes.end(); ++n)
    {
        std::cout << "id: " << (*n)->id
            << " " << (void *) *n
            << " " << Ir::fragmentToString((*n)->statements.front())
            << " (" << (*n)->statements.front()->class_name()
            << ")" << std::endl;
        std::vector<Edge>::iterator edge;
        for (edge = (*n)->successors.begin(); edge != (*n)->successors.end(); ++edge)
        {
            std::cout << "    "
                << "-> " << edge->first->id << " (type " << edge->second << ")"
                << std::endl;
        }
    }
#endif
}
#endif

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
  TimingPerformance timer("Making expression and type containers nonredundant:");
  expression_set parens, *no_parens = new expression_set();
  expression_set::const_iterator e;
  for (e = exprs.begin(); e != exprs.end(); ++e) {
#if 0
    const char *str = expr_to_string(*e);
    if (strlen(str) >= 2 && str[0] == '(' && str[strlen(str)-1] == ')')
#else
    const std::string &str = Ir::fragmentToString(*e);
    std::string::size_type len = str.length();
    if (len >= 2 && str[0] == '(' && str[len-1] == ')')
#endif
      parens.insert(*e);
    else
      no_parens->insert(*e);
  }
  expression_set::iterator p, np;
  for (p = parens.begin(); p != parens.end(); ++p) {
    bool duplicate = false;
 // const char *pstr = expr_to_string(*p);
    const std::string &pstr = Ir::fragmentToString(*p);
    std::string::size_type plen = pstr.length();
    for (np = no_parens->begin(); np != no_parens->end(); ++np) {
#if 0
      const char *npstr = expr_to_string(*np);
      if (strlen(pstr) == strlen(npstr) + 2
          && strncmp(pstr+1, npstr, strlen(npstr)) == 0) {
#else
      const std::string &npstr = Ir::fragmentToString(*np);
      std::string::size_type nplen = npstr.length();
      if (plen == nplen + 2 && pstr.find(npstr) == 1) {
#endif
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

#if 0
// GB (2008-04-08): This is the old version that implements a manual
// traversal of the ICFG. The new version below is much neater.
void 
CFGTraversal::number_exprs() {
  TimingPerformance timer("Numbering of expressions and types:");
  std::deque<BasicBlock *>::const_iterator block;
  std::deque<SgStatement *>::const_iterator stmt;
  std::set<SgExpression *, ExprPtrComparator> expr_set;
  std::set<SgType *, TypePtrComparator> type_set;

// add all global initializer expressions and the types of all global
// variables
  TimingPerformance *nestedTimer
      = new TimingPerformance("Traversing ICFG to number expressions and types:");
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
  ExprSetTraversal est(&expr_set, &type_set);
  for (block = cfg->nodes.begin(); block != cfg->nodes.end(); ++block) {
    for (stmt = (*block)->statements.begin();
         stmt != (*block)->statements.end(); ++stmt) {
      if (isSgCaseOptionStmt(*stmt) 
          || isSgExprStatement(*stmt)
          || isSgScopeStatement(*stmt)) {
        est.traverse(*stmt, preorder);
      } else if (ArgumentAssignment *aa =
              dynamic_cast<ArgumentAssignment *>(*stmt)) {
        est.traverse(aa->get_lhs(), preorder);
        est.traverse(aa->get_rhs(), preorder);
#if 0
      } else if (ReturnAssignment *ra =
              dynamic_cast<ReturnAssignment *>(*stmt)) {
        est.traverse(Ir::createVarRefExp(ra->get_lhs()), preorder);
        est.traverse(Ir::createVarRefExp(ra->get_rhs()), preorder);
      } else if (ParamAssignment *pa =
              dynamic_cast<ParamAssignment *>(*stmt)) {
        est.traverse(Ir::createVarRefExp(pa->get_lhs()), preorder);
        est.traverse(Ir::createVarRefExp(pa->get_rhs()), preorder);
#else
      } else if (MyAssignment *ma = dynamic_cast<MyAssignment *>(*stmt)) {
        est.traverse(ma->get_lhsVarRefExp(), preorder);
        est.traverse(ma->get_rhsVarRefExp(), preorder);
#endif
      } else if (LogicalIf *li = dynamic_cast<LogicalIf *>(*stmt)) {
        est.traverse(li->get_condition(), preorder);
      } else if (DeclareStmt *ds = dynamic_cast<DeclareStmt *>(*stmt)) {
        type_set.insert(ds->get_type());
      }
    }
  }
  delete nestedTimer;
  unsigned int i = 0;
  std::set<SgExpression *, ExprPtrComparator> *exprs_nonred;
  exprs_nonred = make_nonredundant(expr_set);
  nestedTimer = new TimingPerformance("Filling containers:");
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
  delete exprs_nonred;
  delete nestedTimer;
}
#endif

class IcfgExprSetTraversal: public IcfgTraversal
{
public:
    IcfgExprSetTraversal(std::set<SgExpression *, ExprPtrComparator> *expr_set,
                         std::set<SgType *, TypePtrComparator> *type_set)
      : expr_set(expr_set), type_set(type_set), est(expr_set, type_set)
    {
    }

protected:
    void visit(SgNode *node)
    {
     // No more special cases, just invoke the AST traversal on this node.
        est.traverse(node, preorder);

     // OK, so there is still this one special case.
        if (DeclareStmt *decl = isDeclareStmt(node))
            type_set->insert(decl->get_type());
    }

private:
    std::set<SgExpression *, ExprPtrComparator> *expr_set;
    std::set<SgType *, TypePtrComparator> *type_set;
    ExprSetTraversal est;
};

#if 0
void
CFGTraversal::number_exprs()
{
    TimingPerformance timer("Numbering of expressions and types:");
    std::set<SgExpression *, ExprPtrComparator> expr_set;
    std::set<SgType *, TypePtrComparator> type_set;

    TimingPerformance *nestedTimer
        = new TimingPerformance("Traversing ICFG to number expressions and types:");
 // add the types of global variables
    std::vector<SgVariableSymbol *>::iterator g_itr;
    for (g_itr = cfg->globals.begin(); g_itr != cfg->globals.end(); ++g_itr)
        type_set.insert((*g_itr)->get_type());
 // traverse everything else
    IcfgExprSetTraversal iest(&expr_set, &type_set);
    iest.traverse(cfg);
    delete nestedTimer;

    std::set<SgExpression *, ExprPtrComparator> *exprs_nonred;
    exprs_nonred = make_nonredundant(expr_set);

    nestedTimer = new TimingPerformance("Filling containers:");
    unsigned int i = 0;
    std::set<SgExpression *, ExprPtrComparator>::const_iterator expr;
    for (expr = exprs_nonred->begin(); expr != exprs_nonred->end(); ++expr)
    {
     // cfg->numbers_exprs[i] = *expr;
        cfg->numbers_exprs.push_back(*expr);
        cfg->exprs_numbers[*expr] = i;
        i++;
    }
    delete exprs_nonred;

    unsigned int j = 0;
    std::set<SgType *, TypePtrComparator>::const_iterator type;
    for (type = type_set.begin(); type != type_set.end(); ++type)
    {
        cfg->numbers_types[j] = *type;
        cfg->types_numbers[*type] = j;
        j++;
    }
    delete nestedTimer;
}
#else

class TypeSetTraversal: public AstSimpleProcessing
{
public:
    TypeSetTraversal(std::set<SgType *, TypePtrComparator> &type_set)
      : type_set(type_set)
    {
    }

protected:
    void visit(SgNode *node)
    {
        if (SgExpression *expr = isSgExpression(node))
            type_set.insert(expr->get_type());
    }
 
private:
    std::set<SgType *, TypePtrComparator> &type_set;
};

class IcfgHashingTraversal: public IcfgTraversal
{
public:
    IcfgHashingTraversal(EqualityTraversal &eTraversal,
            std::set<SgType *, TypePtrComparator> &type_set)
      : eTraversal(eTraversal), type_set(type_set), tTraversal(type_set)
    {
    }

protected:
    void icfgVisit(SgNode *node)
    {
        if (isIcfgStmt(node))
        {
            handleIcfgStmt(node);
            return;
        }

     // No more special cases, just invoke the AST traversal on this node.
        eTraversal.traverse(node);

#if 0
     // Types need special handling at the moment.
        tTraversal.traverse(node, preorder);
     // OK, so there is still this one special case.
        if (DeclareStmt *decl = isDeclareStmt(node))
            type_set.insert(decl->get_type());
#else
     // GB (2008-05-20): The equality traversal now also collects the types
     // of expressions automagically. We only need to call it for the types
     // in declarations.
#if 0
     // Moved to handleIcfgStmt below.
        if (DeclareStmt *decl = isDeclareStmt(node))
            eTraversal.evaluateSynthesizedAttribute(decl->get_type(),
                                                    emptySynAttributes);
#endif
#endif
    }

private:
    void handleIcfgStmt(SgNode *node)
    {
     // GB (2008-05-21): Our own IcfgStmts cannot be handled by the
     // EqualityTraversal since it's only written for ROSE. However, we
     // don't really need it to hash our statements themselves (yet?), only
     // the expressions and types contained therein.
     // Two possible solutions:
     //  - manually traverse the few special cases (implemented here)
     //  - add  virtual bool isNodeToTraverse(SgNode *)  method to
     //    EqualityTraversal and make it ignore nodes where this is false;
     //    then implement a derived class and use that
        if (ExternalCall *ec = isExternalCall(node))
        {
            eTraversal.traverse(ec->get_function());
        }
        else if (ExternalReturn *er = isExternalReturn(node))
        {
            eTraversal.traverse(er->get_function());
        }
        else if (ArgumentAssignment *aa = isArgumentAssignment(node))
        {
            eTraversal.traverse(aa->get_lhs());
            eTraversal.traverse(aa->get_rhs());
        }
        else if (LogicalIf *li = isLogicalIf(node))
        {
            eTraversal.traverse(li->get_condition());
        }
        else if (DeclareStmt *decl = isDeclareStmt(node))
        {
            eTraversal.evaluateSynthesizedAttribute(decl->get_type(),
                                                    emptySynAttributes);
        }
        else if (ConstructorCall *cc = isConstructorCall(node))
        {
            eTraversal.evaluateSynthesizedAttribute(cc->get_type(),
                                                    emptySynAttributes);
        }
     // Don't need to handle the MyAssignment statements that only contain
     // SgVariableSymbols as SgVariableSymbols are not traversed anyway.
    }

    EqualityTraversal &eTraversal;
    std::set<SgType *, TypePtrComparator> &type_set;
    TypeSetTraversal tTraversal;
    EqualityTraversal::SynthesizedAttributesList emptySynAttributes;
};

void
CFGTraversal::number_exprs()
{
    TimingPerformance timer("Numbering of expressions and types:");
 // std::set<SgExpression *, ExprPtrComparator> expr_set;
    std::set<SgType *, TypePtrComparator> type_set;

    TimingPerformance *nestedTimer
        = new TimingPerformance("Traversing ICFG to number expressions and types:");
 // add the types of global variables
    std::vector<SgVariableSymbol *>::iterator g_itr;
#if 0
    for (g_itr = cfg->globals.begin(); g_itr != cfg->globals.end(); ++g_itr)
        type_set.insert((*g_itr)->get_type());
#else
 // GB (2008-05-20): Types of globals are now collected by calling the
 // equality traversal.
    EqualityTraversal::SynthesizedAttributesList emptySynAttributes;
    for (g_itr = cfg->globals.begin(); g_itr != cfg->globals.end(); ++g_itr)
        cfg->equalityTraversal.evaluateSynthesizedAttribute((*g_itr)->get_type(),
                                                            emptySynAttributes);
#endif
 // traverse everything else
#if 0
    IcfgExprSetTraversal iest(&expr_set, &type_set);
    iest.traverse(cfg);
#else
    IcfgHashingTraversal iht(cfg->equalityTraversal, type_set);
    iht.traverse(cfg);
#endif
    delete nestedTimer;

 // std::set<SgExpression *, ExprPtrComparator> *exprs_nonred;
 // exprs_nonred = make_nonredundant(expr_set);

    nestedTimer = new TimingPerformance("Filling containers:");
#if 0
    unsigned int i = 0;
    std::set<SgExpression *, ExprPtrComparator>::const_iterator expr;
    for (expr = exprs_nonred->begin(); expr != exprs_nonred->end(); ++expr)
    {
        cfg->numbers_exprs[i] = *expr;
        cfg->exprs_numbers[*expr] = i;
        i++;
    }
    delete exprs_nonred;
#else
 // Now get all expressions from the traversal and put them into our maps.
    std::vector<EqualityId> ids;
    cfg->equalityTraversal.get_all_exprs(ids);
 // Start numbering expressions with the current size of numbers_exprs. We
 // do this because numbers_exprs might already contain some expressions
 // that are put there by the points-to analysis (var refs for fake
 // allocation site variables).
    unsigned long i = cfg->numbers_exprs.size();
    std::vector<SgNode *>::const_iterator expr;
    std::vector<EqualityId>::const_iterator id;
    cfg->numbers_exprs.reserve(ids.size());
    for (id = ids.begin(); id != ids.end(); ++id)
    {
        const std::vector<SgNode *> &exprs
            = cfg->equalityTraversal.get_nodes_for_id(*id);
        for (expr = exprs.begin(); expr != exprs.end(); ++expr)
        {
         // The reference returned by get_nodes_for_id is const so that we
         // don't muck around with the contents of the hash bucket. We won't
         // muck around, we promise! But const is just too awkward for some
         // of this stuff, so we cast it away.
            SgExpression *e = const_cast<SgExpression *>(isSgExpression(*expr));
            if (e != NULL)
                cfg->exprs_numbers[e] = i;
#if 0
            std::cout
                << "setting " << (void *) e << " "
                << e->class_name() << " '"
                << Ir::fragmentToString(e)
                << "' -> " << i << std::endl;
#endif
         // For variable references, we need to fill the map connecting
         // VarRefs with their symbols. Symbols are used for
         // declaration-related stuff; filling this map allows analyzers to
         // use the same ID for a variable's uses and declarations. Here we
         // fill the map for all variables that actually have VarRefs
         // referring to them.
            if (SgVarRefExp *varRef = isSgVarRefExp(e))
            {
                SgVariableSymbol *sym = varRef->get_symbol();
                cfg->varsyms_ids[sym] = i;
                cfg->ids_varsyms[i] = sym;
#if 0
                std::cout
                    << "visiting varref "
                    << (void *) varRef << " "
                    << sym->get_name().str() << "/"
                    << sym->get_declaration()->get_mangled_name().str()
                    << "; added it with ID " << i
                    << std::endl;
#endif
            }
        }
        if (exprs.empty())
        {
            std::cerr << __FILE__ << ":" << __LINE__
                << ":number_exprs: empty list of expressions for id "
                << *id << std::endl;
            abort();
        }
        const SgExpression *e = isSgExpression(exprs.front());
     // cfg->numbers_exprs[i] = const_cast<SgExpression *>(e);
     // Use the first element of the expression list as the representative
     // of this equivalence class; that is, this is the pointer that will be
     // returned for this expression ID.
        cfg->numbers_exprs.push_back(const_cast<SgExpression *>(e));

#if 0
        std::cout << "setting " << i << " -> " << (void *) e
            << " " << e->class_name();
        if (const SgIntVal *i = isSgIntVal(e))
            std::cout << " value: " << i->get_value();
        std::cout << std::endl;
#endif
        i++;
    }
#endif
 // Now there still might be some variable symbols not associated with a
 // variable ID because they have no uses (as SgVarRefExp) in the ICFG. This
 // might be because they are declared but never used, or because they are
 // special function parameters that we only ever pass around via
 // VariableSymbol. For numbering, we simply use i from above.
 // We try to get the variable symbols from the memory pool.
    std::vector<SgVariableSymbol *> varsyms;
    class VarSymCollector: public ROSE_VisitTraversal
    {
    public:
        VarSymCollector(std::vector<SgVariableSymbol *> &varsyms)
          : varsyms(varsyms)
        {
        }

        void visit(SgNode *node)
        {
            if (SgVariableSymbol *sym = isSgVariableSymbol(node))
                varsyms.push_back(sym);
        }

    private:
        std::vector<SgVariableSymbol *> &varsyms;
    };
    VarSymCollector varSymCollector(varsyms);
    SgVariableSymbol::traverseMemoryPoolNodes(varSymCollector);
 // std::cout << "got " << varsyms.size() << " varsyms total" << std::endl;
    std::vector<SgVariableSymbol *>::iterator varsym;
    std::map<SgVariableSymbol *, unsigned long>::iterator pos;
    for (varsym = varsyms.begin(); varsym != varsyms.end(); ++varsym)
    {
        SgVariableSymbol *sym = *varsym;
#if 0
        std::cout << "visiting varsym "
            << (void *) sym << " "
            << sym->get_name().str()
            << "/" << sym->get_declaration()->get_mangled_name().str();
#endif
        pos = cfg->varsyms_ids.find(sym);
        if (pos == cfg->varsyms_ids.end())
        {
         // Add new ID.
            cfg->varsyms_ids[sym] = i;
            cfg->ids_varsyms[i] = sym;
         // GB (2008-05-20): Markus and I decided to generate VarRefExps
         // even for those variables that don't have any. This makes the set
         // of VariableIds a subset of the ExpressionIds, which might be
         // neat for some applications.
            SgExpression *varref = Ir::createVarRefExp(sym);
            cfg->exprs_numbers[varref] = i;
            cfg->numbers_exprs.push_back(varref);
            i++;
         // std::cout << "; added it with ID " << (i-1) << std::endl;
        }
        else
        {
         // std::cout << "; present with ID " << pos->second << std::endl;
        }
    }

#if 0
    std::set<SgType *, TypePtrComparator>::const_iterator type;
    for (type = type_set.begin(); type != type_set.end(); ++type)
    {
        cfg->numbers_types[j] = *type;
        cfg->types_numbers[*type] = j;
        j++;
    }
#else
 // GB (2008-05-20): Finally switched to using the AST hashing mechanism for
 // types as well.
    ids.clear();
    cfg->equalityTraversal.get_all_types(ids);
    std::vector<SgNode *>::const_iterator type;
    cfg->numbers_types.reserve(ids.size());
    unsigned long j = 0;
    for (id = ids.begin(); id != ids.end(); ++id)
    {
        const std::vector<SgNode *> &types
            = cfg->equalityTraversal.get_nodes_for_id(*id);
        for (type = types.begin(); type != types.end(); ++type)
        {
            SgType *t = const_cast<SgType *>(isSgType(*type));
         // GB (2008-06-05): The [] operator is wrong here.
         // cfg->numbers_types[j] = t;
            cfg->numbers_types.push_back(t);
            cfg->types_numbers[t] = j;
#if 0
            std::cout << "id " << j << ": added type "
                << (void *) t << "/" << Ir::fragmentToString(t)
                << std::endl;
#endif
        }
        j++;
    }
#endif
    delete nestedTimer;
}
#endif

void
CFGTraversal::processGlobalVariableDeclarations(SgGlobal *global)
{
  std::map<std::string, SgVariableSymbol *>::iterator
      cfg_names_globals_end = cfg->names_globals.end();
  std::map<std::string, SgExpression *>::iterator
      cfg_names_initializers_end = cfg->names_initializers.end();
  SgDeclarationStatementPtrList::const_iterator itr;
  SgDeclarationStatementPtrList::const_iterator decls_end = global->getDeclarationList().end();
  for (itr = global->getDeclarationList().begin(); itr != decls_end; ++itr)
  {
      if (const SgVariableDeclaration *vardecl = isSgVariableDeclaration(*itr))
      {
          SgInitializedName *initname = vardecl->get_variables().front();
          SgVariableSymbol *varsym
              = isSgVariableSymbol(initname->get_symbol_from_symbol_table());
          std::string name;
       // GB (2008-08-04): Global variable names are not unique. Some global
       // variable names in different files refer to the same variable,
       // while others don't. Assume that variables with the same name are
       // meant to refer to the same variable, except ones declared static;
       // we encode the symbol address in the names of static variables,
       // thus enabling us to keep them apart.
          if (varsym != NULL)
          {
              std::stringstream symname;
              symname << varsym->get_name().str();
           // static
              const SgStorageModifier &sm
                  = vardecl->get_declarationModifier().get_storageModifier();
              if (sm.isStatic())
              {
                  symname << "@" << (void *) varsym;
              }
              name = symname.str();
           // std::cout << "global symbol: " << name << std::endl;
          }
          else
          {
           // redeclarations of variables have NULL symbols, for some weird
           // reason; getting the first nondefining declaration doesn't
           // really work either
              continue;
          }

          if (cfg->names_globals.find(name) == cfg_names_globals_end)
          {
              cfg->names_globals[name] = varsym;
              cfg->globals.push_back(varsym);
              cfg_names_globals_end = cfg->names_globals.end();
          }
          if (cfg->names_initializers.find(name) == cfg_names_initializers_end)
          {
           // GB (2008-02-14): Added support for aggregate initializers.
              if (isSgAssignInitializer(initname->get_initializer())
                      || isSgAggregateInitializer(initname->get_initializer()))
              {
                  cfg->names_initializers[name] = initname->get_initializer();
                  cfg->globals_initializers[varsym] = cfg->names_initializers[name];
                  cfg_names_initializers_end = cfg->names_initializers.end();
              }
          }
      }
  }
}

void
CFGTraversal::processFunctionDeclarations(SgFunctionDeclaration *decl)
{
  if (decl->get_definition() != NULL) {
    proc = (*cfg->procedures)[procnum++];

    BasicBlock *last_node = (proc->this_assignment 
                             ? proc->this_assignment 
                             : proc->exit);

    if (is_destructor_decl(decl))
      last_node = call_base_destructors(proc, last_node);

    BasicBlock *begin
      = transform_block(decl->get_definition()->get_body(),
                        last_node, NULL, NULL, NULL);
    if (proc->arg_block != NULL) {
      add_link(proc->entry, proc->first_arg_block, NORMAL_EDGE);
      add_link(proc->last_arg_block, begin, NORMAL_EDGE);
    } else {
      add_link(proc->entry, begin, NORMAL_EDGE);
    }
  }
}

void 
CFGTraversal::visit(SgNode *node) {
  // collect all global variables into a list
//if (SgGlobal *global = isSgGlobal(node->get_parent()))
// GB (2008-04-30): Why in the world did this use node->get_parent() instead
// of node?
// GB (2008-05-05): Refactored.
  if (SgGlobal *global = isSgGlobal(node))
      processGlobalVariableDeclarations(global);

  // visit all function definitions
  // GB (2008-05-05): Refactored.
  if (SgFunctionDeclaration *decl = isSgFunctionDeclaration(node))
      processFunctionDeclarations(decl);
}

void 
add_link(BasicBlock *from, BasicBlock *to, KFG_EDGE_TYPE type) {
  if (from != NULL && to != NULL) {
    from->successors.push_back(Edge(to, type));
    to->predecessors.push_back(Edge(from, type));
  }
}

std::pair<BasicBlock *, BasicBlock *>
CFGTraversal::introduceUndeclareStatements(SgBasicBlock *block,
        BasicBlock *after)
{
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
              // SgVariableSymbol *varsym = Ir::createVariableSymbol(*n);
              // local_var_decls->push_back(varsym);
              else
              {
               // GB (2008-05-09): Why was the code above commented out? We
               // never created undeclare statements, as far as I can see!
               // Well, this version is better (hopefully).
                  SgVariableSymbol *sym = Ir::createVariableSymbol(*n);
                  if (sym != NULL)
                      local_var_decls->push_back(sym);
                  else
                  {
                      std::cerr
                          << "ICFG builder error: "
                          << "no symbol for var "
                          << (*n)->get_name().str() << std::endl;
                      std::abort();
                  }
              }
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
    else
        delete local_var_decls;
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
    return std::make_pair(after, last_block_of_this_block);
}

BasicBlock*
CFGTraversal::transform_block(SgStatement *ast_statement, BasicBlock *after,
                              BasicBlock *break_target,
                              BasicBlock *continue_target,
                              BasicBlock *enclosing_switch)
{
    /*
     * The basic block is split into several new ones: One block per
     * statement. Consecutive blocks are connected via normal edges,
     * loops and ifs by true and false edges to their successors.
     * The first block is linked from before, *all* new blocks
     * without successors, and the last block, are linked to after.
     */
    BasicBlock *new_block = NULL, *last_block_of_this_block = NULL;
 // GB (2008-08-21): Starting with ROSE 0.9.3a, many things that used to be
 // SgBasicBlocks are now SgStatements. This is closer to the C++ grammar,
 // where the bodies of if statements and loops can be any statement, not
 // only compound statements. This change made it necessary to change this
 // method's first parameter from SgBasicBlock to SgStatement; now we cannot
 // simply get the list of statements in the block as before:
 // SgStatementPtrList stmts = block->get_statements();
 // In the new version, if the statement is not a block, we simply create a
 // list of statements of one element, and iterate over it as before.
    SgStatementPtrList stmts;
    if (SgBasicBlock *block = isSgBasicBlock(ast_statement))
    {
        stmts = block->get_statements();
        std::pair<BasicBlock *, BasicBlock* > after_lastblock
            = introduceUndeclareStatements(block, after);
        after = after_lastblock.first;
        last_block_of_this_block = after_lastblock.second;
    }
    else
    {
        stmts.push_back(ast_statement);
     // after unchanged, I think that's fine
     // not sure what to do about last_block_of_this_block
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
        cfg->registerStatementLabel(node_id, current_statement);
        BasicBlock *join_block
          = new BasicBlock(node_id++, INNER, proc->procnum);
        cfg->nodes.push_back(join_block);
        join_block->statements.push_back(Ir::createIfJoin());
        add_link(join_block, after, TRUE_EDGE);
        after = join_block;

        SgIfStmt *ifs = isSgIfStmt(*i);
        cfg->registerStatementLabel(node_id, current_statement);
        BasicBlock *if_block
          = new BasicBlock(node_id++, INNER, proc->procnum);
        cfg->nodes.push_back(if_block);
 
        SgExprStatement* cond
            = isSgExprStatement(ifs->get_conditional());
        SgExpression* new_expr
            = isSgExpression(Ir::deepCopy(cond->get_expression()));
#if 0
        ExprLabeler el(expnum);
        el.traverse(new_expr, preorder);
        expnum = el.get_expnum();
        ExprTransformer et(node_id, proc->procnum, expnum, cfg, if_block);
        et.traverse(new_expr, preorder);
        for (int z = node_id; z < et.get_node_id(); ++z)
            cfg->registerStatementLabel(z, current_statement);
#else
        ExprTransformer et(node_id, proc->procnum, expnum, cfg, if_block,
                cond);
        new_expr = et.labelAndTransformExpression(new_expr,
                                                  cond->get_expression());
#endif
        node_id = et.get_node_id();
        expnum = et.get_expnum();

#if 0
        if (et.get_root_var() != NULL) {
            new_expr = Ir::createVarRefExp(et.get_root_var());
        }
#endif

        if_block->statements.push_front(
                Ir::createIfStmt(Ir::createExprStatement(new_expr)));

	    BasicBlock *t = transform_block(ifs->get_true_body(),
                after, break_target, continue_target, enclosing_switch);
        add_link(if_block, t, TRUE_EDGE);
        SgStatement *false_body = ifs->get_false_body();
        if (false_body != NULL) {
            BasicBlock *f = transform_block(false_body,
                    after, break_target, continue_target, enclosing_switch);
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
     // GB (2008-10-20): Removed a big commented-out piece of code. It used
     // to try (and fail) to remove unreachable nodes from the ICFG. We
     // decided some time ago that unreachable nodes should be accepted,
     // because they do no harm, but not having them causes lots of problems
     // (most of which I would have to implement around).
        stmt_end = new StatementAttribute(after, POS_PRE);

        new_block = NULL;
        after = et.get_after();

        (*i)->addNewAttribute("PAG statement head",
                new StatementAttribute(if_block, POS_POST));
      }
	break;

      case V_SgForStatement: {
        SgForStatement *fors = isSgForStatement(*i);
        /* create a block for the "real" head of the for
         * statement (where the condition is tested) */
        cfg->registerStatementLabel(node_id, current_statement);
        BasicBlock *for_block
          = new BasicBlock(node_id++, INNER, proc->procnum);
        cfg->nodes.push_back(for_block);
        BasicBlock *init_block_after = for_block;
        /* create a block containing the initialization
         * statement */
        BasicBlock *init_block = NULL;

        SgStatementPtrList init_list
          = fors->get_for_init_stmt()->get_init_stmt();
        SgStatementPtrList::reverse_iterator it;
        for (it = init_list.rbegin(); it != init_list.rend(); ++it) {
       // GB (2008-04-09): Moved allocation of the init_block(s) here.
          init_block = allocate_new_block(init_block, init_block_after);

          SgExprStatement *init_expr = isSgExprStatement(*it);
          SgVariableDeclaration *init_decl = isSgVariableDeclaration(*it);
          if (init_expr) {
         // init_block = allocate_new_block(init_block, init_block_after);
            SgExpression* new_expr
              = isSgExpression(Ir::deepCopy(init_expr->get_expression()));
#if 0
            ExprLabeler el(expnum);
            el.traverse(new_expr, preorder);
            expnum = el.get_expnum();
            ExprTransformer et(node_id, proc->procnum, expnum, cfg, init_block);
            et.traverse(new_expr, preorder);
            for (int z = node_id; z < et.get_node_id(); ++z)
              cfg->registerStatementLabel(z, current_statement);
#else
            ExprTransformer et(node_id, proc->procnum, expnum, cfg, init_block,
                    current_statement);
            et.labelAndTransformExpression(new_expr,
                                           init_expr->get_expression());
#endif
            node_id = et.get_node_id();
            expnum = et.get_expnum();

            init_block->statements.push_front(Ir::createExprStatement(new_expr));
            init_block_after = et.get_after();
            init_block = NULL;
          } else if (init_decl) {
         // GB (2008-03-26): The block we generate here cannot be linked to
         // a successor yet; we do not know the successor block until after
         // the condition is evaluated below! In fact, I don't see why we
         // would need to allocate a new init_block here at all.
         // init_block = allocate_new_block(init_block, init_block_after);
            SgInitializedNamePtrList vars = init_decl->get_variables();
            SgInitializedNamePtrList::reverse_iterator it;
            for (it = vars.rbegin(); it != vars.rend(); ++it) {
           // init_block = allocate_new_block(init_block, init_block_after);
              init_block = allocate_block_without_successor(init_block);
              SgAssignInitializer *init
                  = isSgAssignInitializer((*it)->get_initializer());
              if (!init) {
                  std::cerr << "unsupported initializer "
                      << Ir::fragmentToString((*it)->get_initializer())
                      << " (" << (*it)->get_initializer()->class_name()
                      << ") in for loop" << std::endl;
                  break;
              }
              SgExpression* new_expr = isSgExpression(Ir::deepCopy(init));
#if 0
              ExprLabeler el(expnum);
              el.traverse(new_expr, preorder);
              expnum = el.get_expnum();
              ExprTransformer et(node_id, proc->procnum, expnum, cfg,
                      init_block);
                   // GB (2008-03-26): Not sure what is going on here, but
                   // links were wrong. Intuitively, init_block_after should
                   // never be NULL; but also, init_block should always be
                   // the node to jump to from the transformed expression.
                   // (init_block_after != NULL ? init_block_after : init_block));
              et.traverse(new_expr, preorder);
              for (int z = node_id; z < et.get_node_id(); ++z) {
                  cfg->registerStatementLabel(z, current_statement);
              }
#else
              ExprTransformer et(node_id, proc->procnum, expnum, cfg,
                      init_block, current_statement);
              new_expr = et.labelAndTransformExpression(new_expr, init);
#endif
              node_id = et.get_node_id();
              expnum = et.get_expnum();

#if 0
              if (et.get_root_var() != NULL) {
                  new_expr = Ir::createVarRefExp(et.get_root_var());
              }
#endif

              SgVarRefExp *var = Ir::createVarRefExp(*it);
              SgExprStatement* exprStatement
                  = Ir::createExprStatement(Ir::createAssignOp(var, new_expr));
              init_block->statements.push_front(exprStatement);
              init_block = NULL;

              /* Don't try to understand this, because
               * It Works By Magic(tm). But feel free
               * to rewrite it cleanly. */
           // GB (2008-03-26): All the Magic(tm) was used up, so I fixed
           // this code. It is now simpler and maybe even correct.
              SgVariableSymbol *varsym = Ir::createVariableSymbol(*it);
              init_block_after = allocate_new_block(NULL, et.get_after());
              init_block_after->statements.push_front(
                      Ir::createDeclareStmt(varsym, (*it)->get_type()));
            }
          } else {
            std::cerr << "TODO: for init stmt with "
                << (*it)->sage_class_name()
                << std::endl;
            init_block->statements.push_front(*it);
          }
        }

        // MS: 2007: in ROSE 0.8.10 the for-test is a statement (not an expression anymore)
        SgExprStatement* cond
          = isSgExprStatement(fors->get_test());
        SgExpression* new_expr
          = isSgExpression(Ir::deepCopy(cond->get_expression()));

        assert(new_expr);
#if 0
        ExprLabeler el(expnum);
        el.traverse(new_expr, preorder);
        expnum = el.get_expnum();
        ExprTransformer et(node_id, proc->procnum, expnum, cfg, for_block);
        et.traverse(new_expr, preorder);
        for (int z = node_id; z < et.get_node_id(); ++z) {
          cfg->registerStatementLabel(z, current_statement);
        }
#else
        ExprTransformer et(node_id, proc->procnum, expnum, cfg, for_block,
                cond);
        new_expr = et.labelAndTransformExpression(new_expr,
                                                  cond->get_expression());
#endif
        node_id = et.get_node_id();
        expnum = et.get_expnum();

#if 0
        if (et.get_root_var() != NULL) {
          new_expr = Ir::createVarRefExp(et.get_root_var());
        }
#endif

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
        cfg->registerStatementLabel(node_id, current_statement);
        BasicBlock *incr_block
          = new BasicBlock(node_id++, INNER, proc->procnum);
        cfg->nodes.push_back(incr_block);

        SgExpression *new_expr_inc
          = isSgExpression(Ir::deepCopy(fors->get_increment()));
#if 0
        ExprLabeler el_inc(expnum);
        el_inc.traverse(new_expr_inc, preorder);
        expnum = el_inc.get_expnum();
        ExprTransformer et_inc(node_id, proc->procnum, expnum, cfg, incr_block);
        et_inc.traverse(new_expr_inc, preorder);
        for (int z = node_id; z < et_inc.get_node_id(); ++z) {
          cfg->registerStatementLabel(z, current_statement);
        }
#else
        ExprTransformer et_inc(node_id, proc->procnum, expnum, cfg, incr_block,
                current_statement);
        new_expr_inc = et_inc.labelAndTransformExpression(new_expr_inc,
                                                      fors->get_increment());
#endif
        node_id = et_inc.get_node_id();
        expnum = et_inc.get_expnum();

#if 0
        if (et_inc.get_root_var() != NULL) {
          new_expr_inc = Ir::createVarRefExp(et_inc.get_root_var());
        }
#endif
        incr_block->statements.push_front(Ir::createExprStatement(new_expr_inc));
        BasicBlock *incr_block_after = et_inc.get_after();
        /* link everything together */
     // GB (2008-04-02): Set these links BEFORE traversing the body. This
     // aids in identifying unreachable code generated by statements within
     // the body; return statements within ifs within the loop, to be exact.
        add_link(init_block, for_block_after, NORMAL_EDGE);
        add_link(for_block, after, FALSE_EDGE);
        add_link(incr_block, for_block_after, NORMAL_EDGE);
        /* unfold the body */
        BasicBlock *body = transform_block(fors->get_loop_body(),
                                           incr_block_after,
                                           after, incr_block_after,
                                           enclosing_switch);
     // Now add the final link to the body.
        add_link(for_block, body, TRUE_EDGE);

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
	cfg->registerStatementLabel(node_id, current_statement);
	BasicBlock *join_block
	  = new BasicBlock(node_id++, INNER, proc->procnum);
	cfg->nodes.push_back(join_block);
	join_block->statements.push_back(Ir::createWhileJoin());
	add_link(join_block, after, TRUE_EDGE);
	after = join_block;

	SgWhileStmt* whiles = isSgWhileStmt(*i);
	cfg->registerStatementLabel(node_id, current_statement);
	BasicBlock* while_block
	  = new BasicBlock(node_id++, INNER, proc->procnum);
	cfg->nodes.push_back(while_block);

	SgExprStatement* cond
	  = isSgExprStatement(whiles->get_condition());
	SgExpression* new_expr
		  = isSgExpression(Ir::deepCopy(cond->get_expression()));
#if 0
	ExprLabeler el(expnum);
	el.traverse(new_expr, preorder);
	expnum = el.get_expnum();
	ExprTransformer et(node_id, proc->procnum, expnum,
			   cfg, while_block);
	et.traverse(new_expr, preorder);
	for (int z = node_id; z < et.get_node_id(); ++z) {
	  cfg->registerStatementLabel(z, current_statement);
	}
#else
    ExprTransformer et(node_id, proc->procnum, expnum, cfg, while_block,
            cond);
    new_expr = et.labelAndTransformExpression(new_expr,
                                              cond->get_expression());
#endif
	node_id = et.get_node_id();
	expnum = et.get_expnum();

#if 0
	if (et.get_root_var() != NULL)
	  new_expr = Ir::createVarRefExp(et.get_root_var());
#endif
	
	SgWhileStmt* whileStatement=Ir::createWhileStmt(Ir::createExprStatement(new_expr));
	while_block->statements.push_front(whileStatement);

	add_link(while_block, after, FALSE_EDGE);
	BasicBlock *body = transform_block(whiles->get_body(), et.get_after(),
					   after, et.get_after(), enclosing_switch);
	add_link(while_block, body, TRUE_EDGE);

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
	cfg->registerStatementLabel(node_id, current_statement);
	BasicBlock *dowhile_block
	  = new BasicBlock(node_id++, INNER, proc->procnum);
	cfg->nodes.push_back(dowhile_block);
	
	SgExprStatement *cond
	  = isSgExprStatement(dowhiles->get_condition());
	SgExpression *new_expr
	  = isSgExpression(Ir::deepCopy(cond->get_expression()));
#if 0
	ExprLabeler el(expnum);
	el.traverse(new_expr, preorder);
	expnum = el.get_expnum();
	ExprTransformer et(node_id, proc->procnum, expnum,
			   cfg, dowhile_block);
	et.traverse(new_expr, preorder);
	for (int z = node_id; z < et.get_node_id(); ++z) {
                    cfg->registerStatementLabel(z, current_statement);
	}
#else
    ExprTransformer et(node_id, proc->procnum, expnum, cfg, dowhile_block,
            cond);
    new_expr = et.labelAndTransformExpression(new_expr,
                                              cond->get_expression());
#endif
	node_id = et.get_node_id();
	expnum = et.get_expnum();

#if 0
	if (et.get_root_var() != NULL) {
	  new_expr = Ir::createVarRefExp(et.get_root_var());
	}
#endif
                
	SgDoWhileStmt* doWhileStmt
	  = Ir::createDoWhileStmt(Ir::createExprStatement(new_expr));
	dowhile_block->statements.push_front(doWhileStmt);
	
	add_link(dowhile_block, after, FALSE_EDGE);
	BasicBlock* body
	  = transform_block(dowhiles->get_body(), et.get_after(),
                            after, et.get_after(), enclosing_switch);
	add_link(dowhile_block, body, TRUE_EDGE);
	
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
	  cfg->registerStatementLabel(node_id, current_statement);
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
	  cfg->registerStatementLabel(node_id, current_statement);
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
	cfg->registerStatementLabel(node_id, current_statement);
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
	  std::cerr << "TODO: unsupported statement type in switch"
          << " (" << item_sel->class_name() << ")"
		    << std::endl;
	  new_expr = NULL;
	}

#if 0
	ExprLabeler el(expnum);
	el.traverse(new_expr, preorder);
	expnum = el.get_expnum();
	ExprTransformer et(node_id, proc->procnum, expnum,
			   cfg, switch_block);
	et.traverse(new_expr, preorder);
	for (int z = node_id; z < et.get_node_id(); ++z) {
	  cfg->registerStatementLabel(z, current_statement);
	}
#else
    ExprTransformer et(node_id, proc->procnum, expnum, cfg, switch_block,
            item_sel);
    new_expr = et.labelAndTransformExpression(new_expr,
                    isSgExprStatement(item_sel)->get_expression());
#endif
	node_id = et.get_node_id();
	expnum = et.get_expnum();

#if 0
	if (et.get_root_var() != NULL) {
	  new_expr = Ir::createVarRefExp(et.get_root_var());
	}
#endif
	
	SgSwitchStatement* switchStatement
	  = Ir::createSwitchStatement(Ir::createExprStatement(new_expr));
	switch_block->statements.push_front(switchStatement);

 // GB (2008-08-21): Since ROSE 0.9.3a fixed handling of switch statements
 // and their cases, we had to change our handling as well. We do not use
 // do_switch_body anymore; instead, we pass the switch block into the
 // recursive call of transform_block, and every case statement links itself
 // with the block. This is also why we now have the enclosing_switch
 // parameter to transform_block.
 // This is not the nicest design, but we detect presence of a default case
 // as follows: Wherever do_switch_body finds a default case, it gets an
 // attribute saying so. If that attribute is missing, we know that a
 // default edge from switch_block to the after block must be added to
 // bypass all the switch cases.
    BasicBlock *after_whole_switch = after;
    after = transform_block(switchs->get_body(),
                            after,
                            /* break target = */ after,
                            continue_target,
                            switch_block);
    if (!switchStatement->attributeExists("SATIrE: switch has default case"))
        add_link(switch_block, after_whole_switch, NORMAL_EDGE);

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

 // GB (2008-08-22): Added this case here, adapted from old do_switch_body
 // function.
    case V_SgDefaultOptionStmt: {
      SgDefaultOptionStmt *defaults = isSgDefaultOptionStmt(*i);
   // The default case might have a body, although it's usually empty,
   // apparently. Transform it anyway.
      after = transform_block(defaults->get_body(),
                              after, break_target,
                              continue_target, enclosing_switch);
   // Build the new block for the default case.
      BasicBlock *case_block = allocate_new_block(NULL, after);
      case_block->statements.push_front(defaults);
   // Add a link from the enclosing switch block to this block.
      add_link(enclosing_switch, case_block, NORMAL_EDGE);
   // Mark the enclosing switch block with an attribute saying that it has a
   // default case.
      enclosing_switch->statements[0]
          ->addNewAttribute("SATIrE: switch has default case", NULL);
   // I think the pre and post attributes are simply the pre and post
   // positions of this case block. Previously, we struggled to put the post
   // info at the end of the case's "body". But in C and C++, the concept of
   // a case body is quite fuzzy, so we don't even try to mess with it.
      stmt_start = new StatementAttribute(case_block, POS_PRE);
      stmt_end = new StatementAttribute(case_block, POS_POST);
   // And make sure the following links are set correctly.
      after = case_block;
    }
      break;

 // GB (2008-08-22): Adapted from the default case above. It's so similar
 // that it hurts.
    case V_SgCaseOptionStmt: {
      SgCaseOptionStmt *cases = isSgCaseOptionStmt(*i);
   // The case might have a body, although it's usually empty, apparently.
   // Transform it anyway.
      after = transform_block(cases->get_body(),
                              after, break_target, 
                              continue_target, enclosing_switch);
   // Build the new block for the default case.
      BasicBlock *case_block = allocate_new_block(NULL, after);
      case_block->statements.push_front(cases);
   // Add a link from the enclosing switch block to this block.
      add_link(enclosing_switch, case_block, NORMAL_EDGE);
   // I think the pre and post attributes are simply the pre and post
   // positions of this case block. Previously, we struggled to put the post
   // info at the end of the case's "body". But in C and C++, the concept of
   // a case body is quite fuzzy, so we don't even try to mess with it.
      stmt_start = new StatementAttribute(case_block, POS_PRE);
      stmt_end = new StatementAttribute(case_block, POS_POST);
   // And make sure the following links are set correctly.
      after = case_block;
    }
      break;

      case V_SgBasicBlock: {
	SgBasicBlock *block = isSgBasicBlock(*i);
	BasicBlock *body = transform_block(block, 
					   after,
					   break_target, 
					   continue_target,
                       enclosing_switch);

	/* incoming information is at the body's incoming
	 * edge */
	stmt_start = new StatementAttribute(body, POS_PRE);
	/* outgoing information is usually theoretically at
	 * the outgoing edge of the last statement of the
	 * block. if that statement is unreachable... hmm
	 * also, if the block is empty, it is a no-op, so we
	 * use the pre-info as post-info */
	if (!block->get_statements().empty()) {
      SgStatement *stmt = block->get_statements().back();
      if (stmt->attributeExists("PAG statement end"))
      {
          stmt_end = dynamic_cast<StatementAttribute *>(
                  stmt->getAttribute("PAG statement end"));
      }
      else
          stmt_end = stmt_start;
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
    new_block->call_target = Ir::createFunctionRefExp(proc->funcsym);
	SgExpression *new_expr 
	  = isSgExpression(Ir::deepCopy(returns->get_expression()));
#if 0
	ExprLabeler el(expnum);
	el.traverse(new_expr, preorder);
	expnum = el.get_expnum();
	ExprTransformer et(node_id, proc->procnum, expnum,
			   cfg, new_block);
	et.traverse(new_expr, preorder);
	for (int z = node_id; z < et.get_node_id(); ++z) {
	  cfg->registerStatementLabel(z, current_statement);
	}
#else
    ExprTransformer et(node_id, proc->procnum, expnum, cfg, new_block,
            current_statement);
    new_expr = et.labelAndTransformExpression(new_expr,
                                              returns->get_expression());
#endif
	node_id = et.get_node_id();
	expnum = et.get_expnum();

#if 0
	if (et.get_root_var() != NULL) {
	  new_expr = Ir::createVarRefExp(et.get_root_var());
	}
#endif

	SgVarRefExp* varref1
   // GB (2008-06-23): Trying unique global return variable instead of the
   // procedure-specific ones.
   // = Ir::createVarRefExp(proc->returnvar);
      = Ir::createVarRefExp(cfg->global_return_variable_symbol);
 // GB (2008-11-12): In addition to adding a call target to the block
 // containing this return variable expression, we also annotate it directly
 // so that we can arrive at the corresponding location directly from the
 // var ref exp without having to go through the enclosing block.
    varref1->addNewAttribute("SATIrE: call target",
                             new CallAttribute(new_block->call_target));

	SgExprStatement* exprstat
	  = Ir::createExprStatement(Ir::createAssignOp(varref1, new_expr));
	new_block->statements.push_front(exprstat);
    new_block->isReturnStmt = true;
	
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
          = Ir::createVariableSymbol(*j);
#if 0
	    = Ir::createVariableSymbol((*j)->get_name(),
				       (*j)->get_type());
#endif
	  proc->exit->get_params()->push_back(declared_var);
      proc->exit->stmt->update_infolabel();
	  SgAssignInitializer *initializer
	    = isSgAssignInitializer((*j)->get_initptr());
	  SgAggregateInitializer *agg_init
	    = isSgAggregateInitializer((*j)->get_initptr());
	  SgConstructorInitializer *constr_init
	    = isSgConstructorInitializer((*j)->get_initptr());

   // GB (2008-02-14): Unified the "normal" and "aggregate" initializer
   // cases. Even "normal" initializers are now wrapped in a
   // SgAssignInitializer node! Analysis specifications must be aware.
      SgExpression *new_expr = NULL;
      SgExpression *orig_expr = NULL;
      if (agg_init)
      {
          orig_expr = agg_init;
          new_expr = isSgExpression(Ir::deepCopy(orig_expr));
      }
      else if (initializer)
      {
          orig_expr = initializer;
          new_expr = isSgExpression(Ir::deepCopy(orig_expr));
      }

	  if (new_expr) {

          new_block = allocate_new_block(new_block, after);
#if 0
          ExprLabeler el(expnum);
          el.traverse(new_expr, preorder);
          expnum = el.get_expnum();
          ExprTransformer et(node_id, proc->procnum, expnum, cfg, new_block);
          et.traverse(new_expr, preorder);
          for (int z = node_id; z < et.get_node_id(); ++z)
              cfg->registerStatementLabel(z, current_statement);
#else
          ExprTransformer et(node_id, proc->procnum, expnum, cfg, new_block,
                  current_statement);
          new_expr = et.labelAndTransformExpression(new_expr, orig_expr);
#endif
          node_id = et.get_node_id();
          expnum = et.get_expnum();
          after = et.get_after();
          stmt_start = new StatementAttribute(after, POS_PRE);

#if 0
          if (et.get_root_var() != NULL)
              new_expr = Ir::createVarRefExp(et.get_root_var());
#endif
          SgExprStatement *expstmt
              = Ir::createExprStatement(Ir::createAssignOp(
                          Ir::createVarRefExp(declared_var), new_expr));
          new_block->statements.push_front(expstmt);
          new_block = NULL;
	  } else if (constr_init) {
     // GB (2008-02-14): It's not clear to me why this case is almost, but
     // not entirely, identical to the other cases. Could we merge these?
     // TODO: investigate!
	    new_expr = isSgExpression(Ir::deepCopy(constr_init));
     // GB (2008-03-12): Parent pointer must be set manually.
     // GB (2008-03-13): Parent pointer is now set by Ir::deepCopy.
     // new_expr->set_parent(constr_init->get_parent());

#if 0
	    ExprLabeler el(expnum);
	    el.traverse(new_expr, preorder);
	    expnum = el.get_expnum();
	    //new_block = allocate_new_block(new_block, after);
	    ExprTransformer et(node_id, proc->procnum, expnum,
			       cfg, after);
	    et.traverse(new_expr, preorder);
	    for (int z = node_id; z < et.get_node_id(); ++z) {
	      cfg->registerStatementLabel(z, current_statement);
	    }
#else
        ExprTransformer et(node_id, proc->procnum, expnum, cfg, after,
                current_statement);
        new_expr = et.labelAndTransformExpression(new_expr, constr_init);
#endif
	    node_id = et.get_node_id();
	    expnum = et.get_expnum();
	    /* incoming information at the incoming edge
	     * of et.after, outgoing on the incoming
	     * edge of after */
     // GB (2008-07-14): This made the declaration's post info identical
     // with the pre info. Which is not a surprise, since the declaration
     // statement ICFG node hat not even been created yet.
	 // stmt_start
	 //   = new StatementAttribute(et.get_after(), POS_PRE);
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
     // GB (2008-07-14): See above and below. The pre info should be the
     // DeclareStmt's pre info, of course.
	 // stmt_start = stmt_end;
	  }
	  /* declare the variable */
	  new_block = allocate_new_block(new_block, after);
	  new_block->statements.push_front(Ir::createDeclareStmt(declared_var,
							   declared_var->get_type()));
   // GB (2008-07-14): *Now* we can set the pre info attribute.
      stmt_start = new StatementAttribute(new_block, POS_PRE);
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
    // GB (2009-02-25): Right now, I think that the post information of the
    // goto should in fact be on its outgoing edge. In fact, I cannot reason
    // of any reason *not* to put it here. So here it is.
	stmt_end = new StatementAttribute(new_block, POS_POST);

	after = new_block;
	new_block = NULL;
      }
	break;

      case V_SgPragmaDeclaration: /* Adrian: Ignore Pragma Decls */
   // GB (2008-03-26): Ignore function declarations and typedefs as well.
      case V_SgFunctionDeclaration:
      case V_SgTypedefDeclaration:
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
	std::cerr << "ICFG builder: warning: "
        << "unsupported statement type within a function: "
        << (*i)->sage_class_name() << std::endl;
    std::cerr << "at: "
        << (*i)->get_startOfConstruct()->get_filenameString()
        << ":" << (*i)->get_startOfConstruct()->get_line()
        << ":" << (*i)->get_startOfConstruct()->get_col()
        << ": " << Ir::fragmentToString(*i)
        << std::endl;
	/* fall through */

      case V_SgExprStatement:
	SgExprStatement *exprs = isSgExprStatement(*i);
	if (exprs != NULL) {
	  SgExpression *new_expr
	    = isSgExpression(Ir::deepCopy(exprs->get_expression()));

   // Make sure a block exists with a normal edge to after. This new_block
   // will be used to hold the result of expression transformation; i.e.,
   // the transformer might add some code *before* this block, and the final
   // use of temporary variables or whatever (or the original expression)
   // will live in the new_block.
	  new_block = allocate_new_block(new_block, after);
#if 0
	  ExprLabeler el(expnum);
	  el.traverse(new_expr, preorder);
	  expnum = el.get_expnum();
	  ExprTransformer et(node_id, proc->procnum, expnum,
			     cfg, new_block);
	  et.traverse(new_expr, preorder);
	  for (int z = node_id; z < et.get_node_id(); ++z) {
                    cfg->registerStatementLabel(z, current_statement);
	  }
#else
      ExprTransformer et(node_id, proc->procnum, expnum, cfg, new_block,
              current_statement);
      new_expr = et.labelAndTransformExpression(new_expr,
                                                exprs->get_expression());
#endif
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
	  
#if 0
	  if (et.get_root_var() != NULL)
	    new_expr = Ir::createVarRefExp(et.get_root_var());
#endif
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
      // now also checking whether attributes exist already; for expression
      // statements, they might be set by the expression transformer
      if (stmt_start == NULL)
          stmt_start = new StatementAttribute(NULL, POS_PRE);
      if (!(*i)->attributeExists("PAG statement start"))
          (*i)->addNewAttribute("PAG statement start", stmt_start);
      if (stmt_end == NULL)
          stmt_end = new StatementAttribute(NULL, POS_PRE);
      if (!(*i)->attributeExists("PAG statement end"))
          (*i)->addNewAttribute("PAG statement end", stmt_end);
    }
    
    return after;
}

// GB (2008-08-22): This function is not needed anymore with ROSE 0.9.3,
// which fixed switch handling. Everything is now much simpler, and handled
// directly using transform_block.
#if 0
BlockList* 
CFGTraversal::do_switch_body(SgBasicBlock *block,
                          // BasicBlock *switch_block,
                             BasicBlock *after, 
                             BasicBlock *continue_target) {
  std::cerr
      << "*** ICFG builder warning: internal inconsistency! "
      << "deprecated function do_switch_body was called"
      << std::endl;

  BlockList *blocks = new BlockList();
#if 0
  SgStatementPtrList stmts = block->get_statements();
  SgStatement *spare_stmt = NULL;
  
  BasicBlock *previous = after;
  SgStatementPtrList::reverse_iterator i;
  for (i = stmts.rbegin(); i != stmts.rend(); ++i) {
    switch ((*i)->variantT()) {
    case V_SgCaseOptionStmt: {
      if (spare_stmt != NULL) {
        previous = transform_block(spare_stmt, previous,
                                   after, continue_target,
                                   switch_block);
        spare_stmt = NULL;
      }
      SgCaseOptionStmt *cases = isSgCaseOptionStmt(*i);
      /* transform this block */
      previous = transform_block(cases->get_body(),
                                 previous, after, continue_target,
                                 switch_block);
      /*
        previous->statements.push_front(cases);
        blocks->push_front(previous);
      */
      BasicBlock *case_block = allocate_new_block(NULL, previous);
      case_block->statements.push_front(cases);
      blocks->push_back(case_block);
      
      /* pre info is the pre info of the first block in
       * the case body */
      StatementAttribute *stmt_start =
        new StatementAttribute(previous, POS_PRE);
      cases->addNewAttribute("PAG statement start", stmt_start);
      /* post info is the post info of the last statement
       * in the case body; if the body is empty, it is the
       * pre info */
   // GB (2008-04-09): Additional complication: The post info is the post
   // info of the last statement in the case body THAT HAS A POST INFO. The
   // final part of the body may be unreachable as in test5.C.
      AstAttribute *stmt_end = NULL;
#if 0
      if (!cases->get_body()->get_statements().empty()) {
     // AstAttribute* stmt_end =
     //   cases->get_body()->get_statements().back()->getAttribute("PAG statement end");
        std::vector<SgStatement *> &stmts
            = cases->get_body()->get_statements();
        std::vector<SgStatement *>::reverse_iterator stmt;
        stmt = stmts.rbegin();
        while (stmt != stmts.rend()
                && !(*stmt)->attributeExists("PAG statement end"))
        {
            ++stmt;
        }
        if (stmt != stmts.rend()
                && (*stmt)->attributeExists("PAG statement end"))
        {
            stmt_end = (*stmt)->getAttribute("PAG statement end");
        }
      }
#endif
   // GB (2008-08-21): With ROSE 0.9.3a, cases->get_body() returns a
   // SgStatement *, not a SgBasicBlock * anymore. We need to differentiate
   // what we do.
      SgStatement *body = cases->get_body();
      if (body->attributeExists("PAG statement end"))
          stmt_end = body->getAttribute("PAG statement end");
      else if (SgBasicBlock *block = isSgBasicBlock(body))
      {
       // Code adapted from the commented-out section above.
          SgStatementPtrList &stmts = block->get_statements();
          SgStatementPtrList::reverse_iterator stmt;
          stmt = stmts.rbegin();
          while (stmt != stmts.rend()
                  && !(*stmt)->attributeExists("PAG statement end"))
          {
              ++stmt;
          }
          if (stmt != stmts.rend()
                  && (*stmt)->attributeExists("PAG statement end"))
          {
              stmt_end = (*stmt)->getAttribute("PAG statement end");
          }
      }
      if (stmt_end != NULL)
        cases->addNewAttribute("PAG statement end", stmt_end);
      else
        cases->addNewAttribute("PAG statement end", stmt_start);
    }
      break;

    case V_SgDefaultOptionStmt: {
      if (spare_stmt != NULL) {
        previous = transform_block(spare_stmt, previous,
                                   after, continue_target, switch_block);
        spare_stmt = NULL;
      }
                
      SgDefaultOptionStmt *defaults = isSgDefaultOptionStmt(*i);
      /* post info is before the following block */
      StatementAttribute *stmt_end =
        new StatementAttribute(previous, POS_PRE);
      defaults->addNewAttribute("PAG statement end", stmt_end);
      defaults->get_body()->addNewAttribute("PAG statement end", stmt_end);
      /* transform this block */
      previous = transform_block(defaults->get_body(),
                                 previous, after, continue_target,
                                 switch_block);
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
      blocks->push_back(case_block);
    }
      break;
    default:
      if (spare_stmt == NULL) {
     // spare_stmt = new SgBasicBlock(NULL, *i);
        spare_stmt = *i;
      } else {
     // spare_stmt->prepend_statement(*i);
     // GB (2008-08-22): It would have been nice if I had documented this
     // function when I wrote it. I think the right thing to do when we
     // encounter two "spare statements" next to each other is to process
     // the old one using transform_block, and to save the new one as a
     // spare.
        previous = transform_block(spare_stmt, previous,
                                   after, continue_target, switch_block);
        spare_stmt = *i;
      }
      break;
    }
  }
#endif
  return blocks;
}
#endif

int 
CFGTraversal::find_procnum(std::string name) const
{
  int num = 0;
  std::deque<Procedure *>::const_iterator i;
  
  for (i = cfg->procedures->begin(); i != cfg->procedures->end(); ++i) {
    if (name == (*i)->name) {
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
      cfg->registerStatementLabel(node_id, current_statement);
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
 // std::string class_name((*cfg->procedures)[(*bi)->procnum]->name + 1);
    std::string procname = (*cfg->procedures)[(*bi)->procnum]->name;
    std::string class_name = procname.substr(1);
    std::string destructor_name = class_name + "::~" + class_name;
 // std::string this_var_name = std::string() + "$~" + class_name + "$this";

    SgPointerType* ptrType
      = Ir::createPointerType(p->class_type->get_declaration()->get_type());
 // SgVariableSymbol* this_var_sym
 //   = Ir::createVariableSymbol(this_var_name,ptrType);
    SgVariableSymbol* this_var_sym = cfg->global_this_variable_symbol;
 // GB (2008-05-26): The this symbol is now stored with the procedure.
 // SgVariableSymbol* this_sym 
 //   = Ir::createVariableSymbol("this", ptrType);
    SgVariableSymbol* this_sym = p->this_sym;

    CallBlock *call_block = new CallBlock(node_id++, CALL, p->procnum,
            new std::vector<SgVariableSymbol *>(), destructor_name);
    CallBlock *return_block = new CallBlock(node_id++, RETURN, p->procnum,
            new std::vector<SgVariableSymbol *>(), destructor_name);
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
CFGTraversal::numberExpressions(bool flag)
{
    flag_numberExpressions = flag;
}

void
IcfgTraversal::traverse(CFG *cfg)
{
    icfg = cfg;
 // We are not traversing statements yet.
    icfg_statement = false;
 // Call start hook.
    atIcfgTraversalStart();

 // First, we traverse expressions, not statements.
    icfg_statement = false;
    std::map<SgVariableSymbol *, SgExpression *>::iterator itr;
    for (itr = cfg->globals_initializers.begin();
         itr != cfg->globals_initializers.end(); ++itr)
    {
        icfgVisit(itr->second);
    }

 // From now on, we traverse statements.
    icfg_statement = true;
    BlockList::const_iterator block;
    std::deque<SgStatement *>::const_iterator stmt;
    for (block = cfg->nodes.begin(); block != cfg->nodes.end(); ++block)
    {
        if (*block == NULL)
        {
         // GB (2008-10-17): The ICFG's node lists are NULL-terminated.
         // Thus, it is normal and expected to find a NULL node in the list.
            continue;
        }

        node_id = (*block)->id;
        node_type = (*block)->node_type;
        node_procnum = (*block)->procnum;
        statement_index = 0;
        for (stmt = (*block)->statements.begin();
             stmt != (*block)->statements.end(); ++stmt)
        {
            icfgVisit(*stmt);
            statement_index++;
        }
    }
 // We are not traversing statements anymore.
    icfg_statement = false;
 // Call end hook.
    atIcfgTraversalEnd();
}

void
IcfgTraversal::atIcfgTraversalStart()
{
}

void
IcfgTraversal::atIcfgTraversalEnd()
{
}

CFG *
IcfgTraversal::get_icfg() const
{
    return icfg;
}

bool
IcfgTraversal::is_icfg_statement() const
{
    return icfg_statement;
}

int
IcfgTraversal::get_node_id() const
{
    return node_id;
}

KFG_NODE_TYPE
IcfgTraversal::get_node_type() const
{
    return node_type;
}

int
IcfgTraversal::get_node_procnum() const
{
    return node_procnum;
}

int
IcfgTraversal::get_statement_index() const
{
    return statement_index;
}

IcfgTraversal::~IcfgTraversal()
{
}


void
IcfgExternalCallResolver::run(CFG *icfg)
{
    traverse(icfg);
}

void
IcfgExternalCallResolver::atTraversalStart()
{
    timer = new TimingPerformance("Resolve function pointer calls in ICFG:");
}

void
IcfgExternalCallResolver::icfgVisit(SgNode *node)
{
    using namespace SATIrE::Analyses;
    PointsToAnalysis::PointsToAnalysis *pointsToAnalysis
        = get_icfg()->pointsToAnalysis;
    const PointsToAnalysis::CallGraph &callGraph
        = pointsToAnalysis->getCallGraph();

    if (ExternalCall *ec = isExternalCall(node))
    {
        CallBlock *icfgNode
            = dynamic_cast<CallBlock *>(get_icfg()->nodes[get_node_id()]);
        SgExpression *callTarget = icfgNode->call_target;

        PointsToAnalysis::CallGraph::CallCandidateMap::const_iterator ccmi;
        ccmi = callGraph.callCandidateMap.find(callTarget);
        if (ccmi != callGraph.callCandidateMap.end())
        {
            const std::set<SgFunctionSymbol *> &cands = ccmi->second;
            std::set<SgFunctionSymbol *>::const_iterator cand;
            for (cand = cands.begin(); cand != cands.end(); ++cand)
            {
                std::string candname = (*cand)->get_name().str();
                std::multimap<std::string, Procedure *>::iterator mmi, mmi_e;
                mmi = get_icfg()->proc_map.lower_bound(candname);
                mmi_e = get_icfg()->proc_map.upper_bound(candname);
                while (mmi != mmi_e)
                {
                 // set links from call to entry, and from exit to return
                 // nodes
                    Procedure *target = mmi->second;
                    add_link(icfgNode, target->entry, CALL_EDGE);
                    CallBlock *returnNode = icfgNode->partner;
                    add_link(target->exit, returnNode, RETURN_EDGE);
                    ++mmi;
                }
            }
        }
        else
        {
            std::cerr
                << "*** warning: tried to resolve function expression "
                << Ir::fragmentToString(callTarget)
                << ", found no candidates!"
                << std::endl;
        }
    }
}

void
IcfgExternalCallResolver::atTraversalEnd()
{
    delete timer;
}
