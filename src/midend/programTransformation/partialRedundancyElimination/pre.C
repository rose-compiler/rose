// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "pre.h"

#include <stdint.h>
#include <queue>
#include <fstream>
#include "replaceExpressionWithStatement.h"
#include "inlinerSupport.h"
#include "expressionTreeEqual.h"
#include "preControlFlowGraph.h"

#include "constantFolding.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

// DQ (8/1/2005): test use of new static function to create 
// Sg_File_Info object that are marked as transformations
#undef SgNULL_FILE
#define SgNULL_FILE Sg_File_Info::generateDefaultFileInfoForTransformationNode()

// Get the function definition containing a given node
SgFunctionDefinition* PRE::getFunctionDefinition(SgNode* n) {
  while (true) {
    if (isSgFunctionDefinition(n))
      return isSgFunctionDefinition(n);
    n = n->get_parent();
    assert (n);
  }
  return 0; // Should not happen
}

// Are any variables in syms modified anywhere within n, or is n a declaration
// of one of them?
// FIXME: move to inliner
bool anyOfListPotentiallyModifiedIn(const vector<SgVariableSymbol*>& syms,
                                    SgNode* n) {
  bool modified = false;
  for (vector<SgVariableSymbol*>::const_iterator j = syms.begin();
      j != syms.end(); ++j) {
    SgVarRefExp* vr = new SgVarRefExp(SgNULL_FILE, *j);
    vr->set_endOfConstruct(SgNULL_FILE);
    if (isPotentiallyModified(vr, n)) {
      modified = true;
    }
    delete vr;
    if (modified) break;
    if (isSgVariableDeclaration(n)) {
      SgVariableDeclaration* decl = isSgVariableDeclaration(n);
      for (SgInitializedNamePtrList::const_iterator i = 
             decl->get_variables().begin();
           i != decl->get_variables().end(); ++i) {
        if (*i == (*j)->get_declaration()) {
          modified = true;
          break;
        }
      }
    }
    if (modified) break;
  }
  return modified;
}

class ExpressionComputedInVisitor: public AstSimpleProcessing {
  int& count;
  SgExpression* expr;

  public:
  ExpressionComputedInVisitor(int& count, SgExpression* expr): 
    count(count), expr(expr) {}

  virtual void visit(SgNode* n) {
    if (isSgExpression(n) && expressionTreeEqual(isSgExpression(n), expr))
      ++count;
  }
};

// Count how many computations of expr are within root.  Same caveats as
// expressionComputedIn.
int countComputationsOfExpressionIn(SgExpression* expr, SgNode* root) {
  int count = 0;
  ExpressionComputedInVisitor(count, expr).traverse(root, postorder);
  return count;
}

// Is the expression expr computed within root?  This test is simple, and does
// not account for changes to the variables in expr.
bool expressionComputedIn(SgExpression* expr, SgNode* root) {
  return countComputationsOfExpressionIn(expr, root) != 0;
}

class ExpressionTreeEqualTo {
  SgExpression* target;

  public:
  ExpressionTreeEqualTo(SgExpression* target): target(target) {}

  bool operator()(SgExpression* x) const {
    return expressionTreeEqual(x, target);
  }
};

#define FIXPOINT_BEGIN(cfg) \
{ \
  VertexIter i = cfg.graph.vertices().begin(), \
             end = cfg.graph.vertices().end(); \
  queue<Vertex> Q; \
  for (; i != end; ++i) { \
    Q.push(*i); \
  } \
  while (!Q.empty()) { \
    Vertex v = Q.front(); \
    Q.pop(); \
    bool changed = false;

#define FIXPOINT_END(cfg, get_outflow, outflow_iter) \
    if (changed) { \
      outflow_iter j = cfg.graph.get_outflow(v).begin(),\
                   jend = cfg.graph.get_outflow(v).end(); \
      for (; j != jend; ++j) { \
        if (cfg.graph.source(*j) != v) \
          Q.push(cfg.graph.source(*j)); \
        else \
          Q.push(cfg.graph.target(*j));  \
          /* Includes case when j->source = j->target = v */ \
      } \
    } \
  } \
}

#define FIXPOINT_OUTPUT_BEGIN(pmap, cfg) \
  bool old_##pmap = pmap[v]

#define FIXPOINT_OUTPUT_END(pmap, leq, cfg) \
  assert (pmap[v] leq old_##pmap); \
  changed |= (pmap[v] != old_##pmap)

// Accumulate a certain property of all neighbors of a graph node.  Used
// internally by PRE.
template <class PropMap, class BinOp>
typename BinOp::result_type
accumulate_neighbors(
    const PRE::myControlFlowGraph& cfg,
    int v,
    const PropMap& prop,
    const vector<int>& edges,
    int (PRE::simpleGraph::*edge_to_neighbor)(int) const,
    const BinOp& combine,
    typename BinOp::result_type init,
    typename BinOp::result_type ifnone) {
  std::vector<int>::const_iterator i = edges.begin(), end = edges.end();
  if (i == end)
    return ifnone;
  else {
    for (; i != end; ++i)
      init = combine(init, prop[(cfg.graph.*edge_to_neighbor)(*i)]);
    return init;
  }
}

class ReplaceExpressionWithVarrefVisitor: public AstSimpleProcessing {
  SgExpression* expr;
  SgVarRefExp* vr;

  public:
  ReplaceExpressionWithVarrefVisitor(SgExpression* expr, 
                                     SgVarRefExp* vr):
    expr(expr), vr(vr) {}

  virtual void visit(SgNode* n) {
    SgExpression* n2 = isSgExpression(n);
    if (n2 && expressionTreeEqual(n2, expr) && isSgExpression(n->get_parent())) {
      isSgExpression(n->get_parent())->replace_expression(n2, vr);
    }
  }
};

// Do partial redundancy elimination, looking for copies of one expression expr
// within the basic block root.  A control flow graph for root must be provided
// in cfg, with a map from nodes to their statements in node_statements, a map
// from edges to their CFG edge types in edge_type, and a map from edges to
// their insertion points in edge_insertion_point.  The algorithm used is that
// of Paleri, Srikant, and Shankar ("Partial redundancy elimination: a simple,
// pragmatic, and provably correct algorithm", Science of Computer Programming
// 48 (2003) 1--20).
void PRE::partialRedundancyEliminationOne( SgExpression* expr, SgBasicBlock* root, const myControlFlowGraph& cfg)
   {
  // SgBasicBlock* myFunctionBody = getFunctionDefinition(expr)->get_body();

  // DQ (3/16/2006): Added assertions
     ROSE_ASSERT(expr != NULL);
     ROSE_ASSERT(root != NULL);

     vector<SgVariableSymbol*> symbols_in_expression = SageInterface::getSymbolsUsedInExpression(expr);

     if (anyOfListPotentiallyModifiedIn(symbols_in_expression, expr))
        {
       // This expression updates its own arguments, and so is not idempotent
       // Return immediately
          return;
        }

  // Simple or not user-definable expressions
     if (isSgVarRefExp(expr)) return;
     if (isSgValueExp(expr)) return;
     if (isSgFunctionRefExp(expr)) return;
     if (isSgExprListExp(expr)) return;
     if (isSgInitializer(expr)) return;
#if 0
     if ( (isSgAddOp(expr) || isSgSubtractOp(expr)) && 
          (isSgVarRefExp(isSgBinaryOp(expr)->get_lhs_operand()) || 
           isSgValueExp(isSgBinaryOp(expr)->get_lhs_operand())) &&
          (isSgVarRefExp(isSgBinaryOp(expr)->get_rhs_operand()) || 
           isSgValueExp(isSgBinaryOp(expr)->get_rhs_operand())))
          return;
#endif

  // Expressions which do not keep a consistent value each time they are used
     if (!expressionTreeEqual(expr, expr))
          return;

  // cerr << "Trying to do PRE using expression " << expr->unparseToString() << " whose type is " << expr->sage_class_name() << endl;

     VertexIter i   = cfg.graph.vertices().begin(), 
                     end = cfg.graph.vertices().end();

  // cerr << "CFG has " << distance(i, end) << " nodes" << endl;

     bool needToMakeCachevar = false;
     set<SgNode*> replacements;
     vector<pair<SgNode*, bool /* before */> > insertions;

     vector<bool> transp(cfg.graph.vertices().size()),
                  comp(cfg.graph.vertices().size()),
                  antloc(cfg.graph.vertices().size());

     vector<SgNode*> first_computation(cfg.graph.vertices().size()),
                     last_computation(cfg.graph.vertices().size());

  // Set values of local node properties
     for (i = cfg.graph.vertices().begin(); i != end; ++i)
        {
          const vector<SgNode*>& stmts = cfg.node_statements[*i];

       // Precompute test values for each statement
          vector<bool> argumentsModifiedInStatement(stmts.size());
          vector<int> expressionComputedInStatement(stmts.size());
          for (unsigned int j = 0; j < stmts.size(); ++j)
             {
               if (anyOfListPotentiallyModifiedIn(symbols_in_expression, stmts[j]))
                         argumentsModifiedInStatement[j] = true;
               expressionComputedInStatement[j] = countComputationsOfExpressionIn(expr, stmts[j]);
             }

       // Compute transp
          transp[*i] = true;
          for (unsigned int j = 0; j < stmts.size(); ++j)
               if (argumentsModifiedInStatement[j])
                    transp[*i] = false;

       // Compute comp and do local redundancy elimination
          comp[*i] = false;
          SgNode* firstComputationInChain = 0;
          bool needToInsertComputation = false;
          bool computationInsertedOrUsed = false;
       // cout << "In node " << *i << endl;
          for (unsigned int j = 0; j < stmts.size(); ++j)
             {
            // cout << "In stmt " << j << ", expressionComputedInStatement = " << expressionComputedInStatement[j]
            //      << ", argumentsModifiedInStatement = " << argumentsModifiedInStatement[j] << endl;
               if (expressionComputedInStatement[j] && !argumentsModifiedInStatement[j] && comp[*i] /* from last iter */)
                  {
                 // Do local redundancy elimination
                    if (firstComputationInChain && needToInsertComputation)
                       {
                         insertions.push_back(make_pair(firstComputationInChain, true));
                         replacements.insert(firstComputationInChain);
                         needToInsertComputation = false;
                       }
                    replacements.insert(stmts[j]);
                    computationInsertedOrUsed = true;
                    needToMakeCachevar = true;
                  }
               if (expressionComputedInStatement[j])
                  {
                    comp[*i] = true;
                    if (!firstComputationInChain)
                       {
                         firstComputationInChain = stmts[j];
                         needToInsertComputation = true;
                         if (expressionComputedInStatement[j] >= 2)
                            {
                              insertions.push_back(make_pair(stmts[j], true));
                              needToMakeCachevar = true;
                              needToInsertComputation = false;
                              computationInsertedOrUsed = true;
                              replacements.insert(stmts[j]);
                            }
                       }
                    last_computation[*i] = stmts[j];
                  }
               if (argumentsModifiedInStatement[j])
                  {
                    comp[*i] = false; // Must come after expressionComputedInStatement check
                    firstComputationInChain = 0;
                    needToInsertComputation = false;
                  }
             }

          ASSERT_always_require (!computationInsertedOrUsed || needToMakeCachevar);

       // Compute antloc
          antloc[*i] = false;
          for (unsigned int j = 0; j < stmts.size(); ++j)
             {
               if (expressionComputedInStatement[j] && !argumentsModifiedInStatement[j])
                  {
                    antloc[*i] = true;
                    first_computation[*i] = stmts[j];
                    break;
                  }
               if (argumentsModifiedInStatement[j])
                  {
                    antloc[*i] = false;
                    break;
                  }
             }
        }

// #define PRINT_PROPERTY(p) // for (i = cfg.graph.vertices().begin(); i != end; ++i) if (p[*i]) cerr << #p ": " << *i << endl;
#define PRINT_PROPERTY(p) // for (i = cfg.graph.vertices().begin(); i != end; ++i) if (p[*i]) cerr << #p ": " << *i << endl;
     PRINT_PROPERTY(transp);
     PRINT_PROPERTY(comp);
     PRINT_PROPERTY(antloc);

     int (simpleGraph::*source_ptr)(int) const = &simpleGraph::source;
     int (simpleGraph::*target_ptr)(int) const = &simpleGraph::target;

     vector<bool> avin(cfg.graph.vertices().size(), true);
     vector<bool> avout(cfg.graph.vertices().size(), true);
     FIXPOINT_BEGIN(cfg)
     FIXPOINT_OUTPUT_BEGIN(avin, cfg);
     FIXPOINT_OUTPUT_BEGIN(avout, cfg);
     avin[v] = accumulate_neighbors(cfg, v, avout,cfg.graph.in_edges(v),&simpleGraph::source, logical_and<bool>(), true, false);
     avout[v] = comp[v] || (avin[v] && transp[v]);
     FIXPOINT_OUTPUT_END(avout, <=, cfg);
     FIXPOINT_OUTPUT_END(avin, <=, cfg);
     FIXPOINT_END(cfg, out_edges, OutEdgeIter)

     PRINT_PROPERTY(avin);
     PRINT_PROPERTY(avout);

     vector<bool> antin(cfg.graph.vertices().size(), true);
     vector<bool> antout(cfg.graph.vertices().size(), true);
     FIXPOINT_BEGIN(cfg)
     FIXPOINT_OUTPUT_BEGIN(antin, cfg);
     FIXPOINT_OUTPUT_BEGIN(antout, cfg);
     antout[v] = accumulate_neighbors(cfg, v, antin, cfg.graph.out_edges(v), target_ptr, logical_and<bool>(), true, false);
     antin[v] = antloc[v] || (antout[v] && transp[v]);
     FIXPOINT_OUTPUT_END(antout, <=, cfg);
     FIXPOINT_OUTPUT_END(antin, <=, cfg);
     FIXPOINT_END(cfg, in_edges, InEdgeIter)

     PRINT_PROPERTY(antin);
     PRINT_PROPERTY(antout);

     vector<bool> safein(cfg.graph.vertices().size()), safeout(cfg.graph.vertices().size());

     for (i = cfg.graph.vertices().begin(); i != end; ++i)
        {
          safein[*i] = avin[*i] || antin[*i];
          safeout[*i] = avout[*i] || antout[*i];
        }

     PRINT_PROPERTY(safein);
     PRINT_PROPERTY(safeout);

     vector<bool> spavin(cfg.graph.vertices().size(), false);
     vector<bool> spavout(cfg.graph.vertices().size(), false);
     FIXPOINT_BEGIN(cfg)
     FIXPOINT_OUTPUT_BEGIN(spavin, cfg);
     FIXPOINT_OUTPUT_BEGIN(spavout, cfg);
     spavin[v] = safein[v] && accumulate_neighbors(cfg, v, spavout, cfg.graph.in_edges(v), source_ptr, logical_or<bool>(), false, false);
     spavout[v] = safeout[v] && (comp[v] || (spavin[v] && transp[v]));
     FIXPOINT_OUTPUT_END(spavout, >=, cfg);
     FIXPOINT_OUTPUT_END(spavin, >=, cfg);
     FIXPOINT_END(cfg, out_edges, OutEdgeIter)

     PRINT_PROPERTY(spavin);
     PRINT_PROPERTY(spavout);

     vector<bool> spantin(cfg.graph.vertices().size(), false);
     vector<bool> spantout(cfg.graph.vertices().size(), false);
     FIXPOINT_BEGIN(cfg)
     FIXPOINT_OUTPUT_BEGIN(spantin, cfg);
     FIXPOINT_OUTPUT_BEGIN(spantout, cfg);
     spantout[v] = safeout[v] && accumulate_neighbors(cfg, v, spantin, cfg.graph.out_edges(v), target_ptr, logical_or<bool>(), false, false);
     spantin[v] = safein[v] && (antloc[v] || (spantout[v] && transp[v]));
     FIXPOINT_OUTPUT_END(spantout, >=, cfg);
     FIXPOINT_OUTPUT_END(spantin, >=, cfg);
     FIXPOINT_END(cfg, in_edges, InEdgeIter)

     PRINT_PROPERTY(spantin);
     PRINT_PROPERTY(spantout);
#undef PRINT_PROPERTY

     vector<bool> node_insert(cfg.graph.vertices().size());
     vector<bool> replacef(cfg.graph.vertices().size());
     vector<bool> replacel(cfg.graph.vertices().size());

  // printf ("Intermediate test 1: needToMakeCachevar = %s \n",needToMakeCachevar ? "true" : "false");

     for (i = cfg.graph.vertices().begin(); i != end; ++i)
        {
          node_insert[*i] = comp[*i] && spantout[*i] && (!transp[*i] || !spavin[*i]);
          replacef[*i] = antloc[*i] && (spavin[*i] || (transp[*i] && spantout[*i]));
          replacel[*i] = comp[*i] && (spantout[*i] || (transp[*i] && spavin[*i]));

          if (node_insert[*i])
             {
               needToMakeCachevar = true;
               insertions.push_back(make_pair(last_computation[*i], true));
            // cerr << "Insert computation of " << expr->unparseToString() << " just before last computation in " << *i << endl;
             }

          if (replacef[*i])
             {
               needToMakeCachevar = true;
               replacements.insert(first_computation[*i]);
            // cerr << "Replace first computation of " << *i << endl;
             }

          if (replacel[*i])
             {
               needToMakeCachevar = true;
               replacements.insert(last_computation[*i]);
            // cerr << "Replace last computation of " << *i << endl;
             }
        }

     vector<bool> edge_insert(cfg.graph.edges().size());

  // printf ("Intermediate test 2: needToMakeCachevar = %s \n",needToMakeCachevar ? "true" : "false");

     EdgeIter j = cfg.graph.edges().begin(), jend = cfg.graph.edges().end();
     for (; j != jend; ++j)
        {
          edge_insert[*j] = !spavout[cfg.graph.source(*j)] && spavin[cfg.graph.target(*j)] && spantin[cfg.graph.target(*j)];

       // printf ("edge_insert[*j] = %s \n",edge_insert[*j] ? "true" : "false");
          if (edge_insert[*j])
             {
               needToMakeCachevar = true;
            // cerr << "Insert computation of " << expr->unparseToString() << " on edge from " 
            //      << cfg.graph.source(*j) << " to " << cfg.graph.target(*j) << endl;
             }
        }

  // printf ("Before final test: needToMakeCachevar = %s \n",needToMakeCachevar ? "true" : "false");

  // Add cache variable if necessary
     SgVarRefExp* cachevar = 0;
     if (needToMakeCachevar)
        {
          SgName cachevarname = "cachevar__";
          cachevarname << ++SageInterface::gensym_counter;

       // printf ("Building variable name = %s \n",cachevarname.str());

          SgType* type = expr->get_type();
          if (isSgArrayType(type))
               type = new SgPointerType(isSgArrayType(type)->get_base_type());
          assert (SageInterface::isDefaultConstructible(type));
       // FIXME: assert (isAssignable(type));
          SgVariableDeclaration* decl = new SgVariableDeclaration(SgNULL_FILE, cachevarname, type, NULL);
          decl->set_definingDeclaration(decl);
          SgInitializedName* initname = decl->get_variables().back();

       // DQ (10/5/2007): Added an assertion.
          ROSE_ASSERT(initname != NULL);

          decl->addToAttachedPreprocessingInfo( 
               new PreprocessingInfo(PreprocessingInfo::CplusplusStyleComment, 
               (string("// Partial redundancy elimination: ") + cachevarname.str() + 
               " is a cache of " + expr->unparseToString()).c_str(),
               "Compiler-Generated in PRE", 0, 0, 0, PreprocessingInfo::before));
          SgVariableSymbol* cachevarsym = new SgVariableSymbol(initname);
          decl->set_parent(root);

       // DQ (10/5/2007): Added scope (suggested by Jeremiah).
          initname->set_scope(root);

          root->get_statements().insert(root->get_statements().begin(),decl);

          root->insert_symbol(cachevarname, cachevarsym);
          cachevar = new SgVarRefExp(SgNULL_FILE, cachevarsym);
          cachevar->set_endOfConstruct(SgNULL_FILE);
        }

  // Do expression computation replacements
     for (set<SgNode*>::iterator i = replacements.begin(); i != replacements.end(); ++i)
        {
          ReplaceExpressionWithVarrefVisitor(expr, cachevar).traverse(*i, postorder);
        }

  // Do edge insertions
  // int count = 0;
     bool failAtEndOfFunction = false;
     for (j = cfg.graph.edges().begin(); j != jend; ++j)
        {
       // printf ("Build the insertion list! count = %d \n",count++);
          if (edge_insert[*j])
             {
#if 0
            // DQ (3/13/2006): Compiler warns that "src" is unused, so I have commented it out!
               Vertex src = cfg.graph.source(*j), tgt = cfg.graph.target(*j);
               cerr << "Doing insertion between " << src << " and " << tgt << endl;
#endif
               pair<SgNode*, bool> insert_point = cfg.edge_insertion_point[*j];
               if (insert_point.first)
                  {
                    insertions.push_back(insert_point);
                  }
                 else
                  {
                 // DQ (3/16/2006): This is a visited when we fixup the NULL pointer to the initializer in a SgForStatment.
                    cerr << "Warning: no insertion point found" << endl; //FIXME was assert
                    printf ("Need to figure out what to do here! cfg.edge_insertion_point[*j] = %p \n",&(cfg.edge_insertion_point[*j]));

                    failAtEndOfFunction = true;
                    ROSE_ASSERT(false);
                  }
             }
        }

  // Do within-node insertions
  // printf ("At start of loop: insertions.size() = %" PRIuPTR " \n",insertions.size());
     for (vector<pair<SgNode*, bool> >::iterator i = insertions.begin(); i != insertions.end(); ++i)
        {
          SgTreeCopy tc1, tc2;
          SgVarRefExp* cachevarCopy = isSgVarRefExp(cachevar->copy(tc1));
          ROSE_ASSERT (cachevarCopy);
          cachevarCopy->set_lvalue(true);
          SgExpression* operation = new SgAssignOp(SgNULL_FILE, cachevarCopy, isSgExpression(expr->copy(tc2)));
#if 0
          printf ("Inside of loop: insertions.size() = %" PRIuPTR " \n",insertions.size());
          printf ("\n\ni->first = %p = %s = %s \n",i->first,i->first->class_name().c_str(),i->first->unparseToString().c_str());
          printf ("operation = %p = %s = %s \n",operation,operation->class_name().c_str(),operation->unparseToString().c_str());
#endif
          if (isSgExpression(i->first) && !i->second)
             {
               SgNode* ifp = i->first->get_parent();
               SgCommaOpExp* comma = new SgCommaOpExp(SgNULL_FILE, isSgExpression(i->first), operation);
               operation->set_parent(comma);
               comma->set_parent(ifp);
               i->first->set_parent(comma);
               if (isSgForStatement(ifp))
                  {
                    isSgForStatement(ifp)->set_increment(comma);
                  }
                 else
                    if (isSgBinaryOp(ifp) && isSgBinaryOp(ifp)->get_lhs_operand() == i->first)
                       {
                         isSgBinaryOp(ifp)->set_lhs_operand(comma);
                       }
                      else
                         if (isSgBinaryOp(ifp) && isSgBinaryOp(ifp)->get_rhs_operand() == i->first)
                            {
                              isSgBinaryOp(ifp)->set_rhs_operand(comma);
                            }
                           else
                              if (isSgUnaryOp(ifp) && isSgUnaryOp(ifp)->get_operand() == i->first)
                                 {
                                   isSgUnaryOp(ifp)->set_operand(comma);
                                 }
                                else
                                 {
                                   cerr << ifp->sage_class_name() << endl;
                                   assert (!"Bad parent type for inserting comma expression");
                                 }
             }
            else
             {
               SgStatement* the_computation = new SgExprStatement(SgNULL_FILE, operation);
               operation->set_parent(the_computation);
            // printf ("In pre.C: the_computation = %p = %s \n",the_computation,the_computation->class_name().c_str());

               if (isSgBasicBlock(i->first) && i->second)
                  {
                    isSgBasicBlock(i->first)->get_statements().insert(isSgBasicBlock(i->first)->get_statements().begin(),the_computation);
                    the_computation->set_parent(i->first);
                  }
                 else
                  {
#if 0
                 // DQ (3/14/2006): Bug here when SgExprStatement from SgForStatement test is used here!
                    printf ("Bug here when SgExprStatement from SgForStatement test is used: i->first = %s \n",i->first->class_name().c_str());
                    i->first->get_file_info()->display("Location i->first");
                    printf ("Bug here when SgExprStatement from SgForStatement test is used: TransformationSupport::getStatement(i->first) = %s \n",
                         TransformationSupport::getStatement(i->first)->class_name().c_str());
                    printf ("Bug here when SgExprStatement from SgForStatement test is used: the_computation = %s \n",the_computation->class_name().c_str());
                    the_computation->get_file_info()->display("Location the_computation: debug");
                    ROSE_ASSERT(i->first != NULL);
                    ROSE_ASSERT(i->first->get_parent() != NULL);
                    printf ("i->first->get_parent() = %s \n",i->first->get_parent()->class_name().c_str());
                    i->first->get_file_info()->display("Location i->first: debug");
#endif

                    SgForStatement*  forStatement = isSgForStatement(i->first->get_parent());
                    if (forStatement != NULL)
                       {
                      // Make sure that both pointers are not equal because they are both NULL!
                         ROSE_ASSERT(forStatement->get_test() != NULL);
                         SgExprStatement* possibleTest = isSgExprStatement(i->first);
                         if ( forStatement->get_test() == possibleTest )
                            {
                           // This is a special case of a SgExpressionStatement as a target in a SgForStatement
                           // printf ("Found special case of target being the test of a SgForStatement \n");
                              forStatement->set_test(the_computation);
                              the_computation->set_parent(forStatement);
                            }
                       }
                      else
                       {
                         SgForInitStatement*  forInitStatement = isSgForInitStatement(i->first->get_parent());
                         if (forInitStatement != NULL)
                            {
                           // printf ("Found the SgForInitStatement \n");
                              SgVariableDeclaration* possibleVariable = isSgVariableDeclaration(i->first);
                              SgExprStatement* possibleExpression = isSgExprStatement(i->first);
                              SgStatementPtrList & statementList = forInitStatement->get_init_stmt();
                              SgStatementPtrList::iterator i = statementList.begin();
                              bool addToForInitList = false;
                              while ( (addToForInitList == false) && (i != statementList.end()) )
                                 {
                                // if ( *i == possibleVariable )
                                   if ( *i == possibleVariable || *i == possibleExpression )
                                      {
                                     // This is a special case of a SgExpressionStatement as a target in a SgForStatement
                                     // printf ("Found special case of SgForInitStatement transformation \n");
                                        addToForInitList = true;
                                      }
                                   i++;
                                 }

                           // Only modify the STL list outside of the loop over the list to avoid interator invalidation
                              if (addToForInitList == true)
                                 {
                                // Add the the_computation statment to the list in the SgForInitStatement. 
                                // Later if we abandon the SgForInitStatement then we would have to add it to 
                                // the list of SgInitializedName objects in the SgVariableDeclaration OR modify 
                                // the expression list to handle the extra expression (but I think this case in 
                                // handled above).
                                // printf ("Adding the_computation to the list in the SgForInitStatement \n");
                                   statementList.push_back(the_computation);
                                   the_computation->set_parent(forInitStatement);
                                 }
                            }
                           else
                            {
                              myStatementInsert(TransformationSupport::getStatement(i->first),the_computation,i->second,true);
                              the_computation->set_parent(TransformationSupport::getStatement(i->first));
                            }
                       }
                  }
             }
        }

  // DQ (3/16/2006): debugging code to force failure at inspection point
     if (failAtEndOfFunction == true)
        {
          printf ("Error: internal error detected \n");
          ROSE_ASSERT(false);
        }
   }

#if 1
class FindExpressionsVisitor: public AstSimpleProcessing {
  public:
  vector<SgExpression*> exprs;

  virtual void visit(SgNode* n) {
    if (isSgExpression(n))
      exprs.push_back(isSgExpression(n));
  }
};
#else
// DQ (4/8/2006): This has been reimplemented to support apparent problems in PRE
// when expressions are used that are a part of the constant folded expression trees
// now available (and traversed) in the AST.  Because we traverse the expression trees
// of constant folded values we were collecting them into the list of expressions
// upon which to apply PRE.  This is likely nore a great idea, just because we 
// make such things available in the AST and in the traversal does not imply that
// they need to be optimized.  In general the rule is that the AST represents
// the structure of the original source code, and the traversals in ROSE traverse
// that structure.  What we want to apply optimization to is in this case less
// than the full AST, and instead the semantically equivalent AST (after constant folding).
// We will soon implement constant folding which can eliminate (prune) the AST of
// all extra such information.

// Here we collect the expression that are in the AST (as before, in the previous version),
// but we exclude those in the constant folded expression trees that have been recently added
// to the AST.  We use inherited attributes to do this.

// Build an inherited attribute for the tree traversal to skip constant folded expressions
class FindExpressionsVisitorInheritedAttribute
   {
     public:
          bool isConstantFoldedValue;
          bool isPartOfFoldedExpression;

      //! Specific constructors are required
          FindExpressionsVisitorInheritedAttribute()
             : isConstantFoldedValue(false), isPartOfFoldedExpression(false) {};

       // Need to implement the copy constructor
          FindExpressionsVisitorInheritedAttribute ( const FindExpressionsVisitorInheritedAttribute & X )
             : isConstantFoldedValue(X.isConstantFoldedValue), isPartOfFoldedExpression(X.isPartOfFoldedExpression) {};
   };

// Tree traversal to find expressions in the AST
class FindExpressionsVisitor
   : public SgTopDownProcessing<FindExpressionsVisitorInheritedAttribute>
   {
     public:
       // Storage for expressions that are found (excluding those in constant folded expression trees which cause errors in PRE)
          vector<SgExpression*> exprs;

       // Functions required by the rewrite mechanism
          FindExpressionsVisitorInheritedAttribute evaluateInheritedAttribute ( SgNode* n, FindExpressionsVisitorInheritedAttribute inheritedAttribute )
             {
               if (inheritedAttribute.isConstantFoldedValue == true)
                  {
                    inheritedAttribute.isConstantFoldedValue = false;
                    inheritedAttribute.isPartOfFoldedExpression = true;
                  }
                 else
                  {
                    SgValueExp* valueExpression = isSgValueExp(n);
                 // Check for constant folded value
                    if (valueExpression != NULL && valueExpression->get_valueExpressionTree() != NULL)
                       {
                         inheritedAttribute.isConstantFoldedValue = true;
                       }
                  }

               if (inheritedAttribute.isPartOfFoldedExpression == false)
                  {
                    SgExpression* expr = isSgExpression(n);
                    if ( expr != NULL )
                         exprs.push_back(expr);
                  }

               return inheritedAttribute;
             }
   };
#endif

// Do partial redundancy for all expressions within a given function, whose
// body is given in n.
void
PRE::partialRedundancyEliminationFunction(SgFunctionDefinition* n)
{
    // FIXME: do expressions in batches using bit vectors
    typedef PRE::myControlFlowGraph ControlFlowGraph;
    typedef int Vertex;
    typedef int Edge;
    typedef vector<int>::const_iterator VertexIter;
    typedef vector<int>::const_iterator OutEdgeIter;
    typedef vector<int>::const_iterator InEdgeIter;
    typedef vector<int>::const_iterator EdgeIter;

    // DQ (4/8/2006): Call the new constant folding (not fully implemented except that it works well to eliminate the stored
    // constant expression trees in the AST which are redundant with the values on SgValueExp IR nodes.  The storage of the
    // constant expression trees from which constant folded values are generated is a new development within ROSE and required
    // to represent the complete structure of the source code (and handle details like marcos, floating literals as strings,
    // etc.).  The storage, and traversal of these redundant constant expressions is a problem for PRE, and since I could not
    // fix it easily, I implemented the constant folding in ROSE (not finished) so that we could remove the constant expression
    // trees which interfere with PRE and which are philosophically a problem for other forms of program analysis as well. So
    // calling this here is as if we have constant folding as a phase before PRE.
    ConstantFolding::constantFoldingOptimization(n);

    FindExpressionsVisitor vis;
    vis.traverse(n, postorder);
    vector<SgExpression*> exprs_done;
    for (vector<SgExpression*>::iterator i = vis.exprs.begin(); i != vis.exprs.end(); ++i) {
        if (std::find_if(exprs_done.begin(), exprs_done.end(), ExpressionTreeEqualTo(*i)) != exprs_done.end())
            continue;
        exprs_done.push_back(*i);

        myControlFlowGraph controlflow;
        makeCfg(n, controlflow);
#if 0
        { ofstream dotfile("cfgnew_preip.dot"); printCfgAsDot(dotfile, controlflow); }
#endif

        // Add insertion points to each edge
        addEdgeInsertionPoints(controlflow);
#if 0
        { ofstream dotfile("cfgnew.dot"); printCfgAsDot(dotfile, controlflow); }
#endif

        partialRedundancyEliminationOne(*i, n->get_body(), controlflow);
    }
}

class DoPreOnEachFunctionVisitor: public AstSimpleProcessing
   {
     public:
          virtual void visit(SgNode* n)
             {
               if (isSgFunctionDefinition(n))
                  {
                 // printf ("Calling partialRedundancyEliminationFunction on node = %s \n",n->class_name().c_str());
                 // n->get_file_info()->display("location of SgFunctionDefinition");

                    PRE::partialRedundancyEliminationFunction(isSgFunctionDefinition(n));
                  }
             }
   };

// Do partial redundancy elimination on all functions within n.
void
PRE::partialRedundancyElimination(SgNode* n)
   {
     DoPreOnEachFunctionVisitor().traverse(n, preorder);
   }

