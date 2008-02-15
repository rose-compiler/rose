#include "rose.h"

#include "DependenceGraph.h"
#include <map>
#include <list>
using namespace std;

void SystemDependenceGraph::addFunction(ProgramDependenceGraph * pdg) {

  SgFunctionDeclaration * func = pdg->getInterprocedural()->getFunctionDeclaration();
  ROSE_ASSERT(func != NULL);
  _funcs_map[func] = pdg;

  _mergeGraph(pdg);
}

void SystemDependenceGraph::process() {
  map<SgFunctionDeclaration *, ProgramDependenceGraph *>::iterator func_iterator;
  for (func_iterator = _funcs_map.begin(); func_iterator != _funcs_map.end(); func_iterator++) {
    ProgramDependenceGraph * pdg = func_iterator->second;
    _processFunction(pdg);
  }
}

void SystemDependenceGraph::_processFunction(ProgramDependenceGraph * pdg) {

  //go through call sites, link up actual_in to formal_in and actual_out to formal_out nodes
  //and link call sites to call entry nodes
  InterproceduralInfo * info = pdg->getInterprocedural();

  map<SgFunctionCallExp *, InterproceduralInfo::CallSiteStructure>::iterator callsite_iter;
  for (callsite_iter = info->callsite_map.begin(); callsite_iter != info->callsite_map.end(); callsite_iter++) {
    InterproceduralInfo::CallSiteStructure callsite = callsite_iter->second;
    SgFunctionCallExp * funcCall = callsite_iter->first;

    //This only works with easily resolvable function calls
    list<SgFunctionDeclaration *> possibleFuncs = _getPossibleFuncs(funcCall);

    for (list<SgFunctionDeclaration *>::iterator i = possibleFuncs.begin(); i != possibleFuncs.end(); i++) {

      SgFunctionDeclaration * fD = *i;

      //Now get the interproc info for the function we are calling
      InterproceduralInfo::ProcedureEntryStructure procEntry = _funcs_map[fD]->getInterprocedural()->procedureEntry;
      
      //Link up the call site with the procedure entry
      establishEdge(createNode(callsite.callsite), createNode(procEntry.entry), CALL);
      
      //Link up the formal_out return and the actual_out return
      establishEdge(createNode(procEntry.formal_return), createNode(callsite.actual_return), RETURN);
      
      //Go through the actual_in nodes from callsite and link them to the formal_in nodes in procEntry
      //and link the formal_out nodes from procEntry to the actual_out nodes from callsite
      list<SgExpression *>::iterator actual_iterator = callsite.expr_order.begin();
      list<SgInitializedName *>::iterator formal_iterator = procEntry.arg_order.begin();
      while (actual_iterator != callsite.expr_order.end()) {
	ROSE_ASSERT(formal_iterator != procEntry.arg_order.end());
	
	DependenceNode * actual_in = createNode(callsite.actual_in[*actual_iterator]);
	DependenceNode * formal_in = createNode(procEntry.formal_in[*formal_iterator]);
	establishEdge(actual_in, formal_in, CALL);
	
	DependenceNode * actual_out = createNode(callsite.actual_out[*actual_iterator]);
	DependenceNode * formal_out = createNode(procEntry.formal_out[*formal_iterator]);
	establishEdge(formal_out, actual_out, RETURN);
	
	actual_iterator++;
	formal_iterator++;
      }
    }
  }
}

list<SgFunctionDeclaration*> SystemDependenceGraph::_getPossibleFuncs(SgFunctionCallExp * funcCall) {

  //This function currently assumes that the function call is resolved
  //statically, and returns a single function declaration (which we
  //can obtain directly from the AST). When a better callgraph
  //analysis is used, we can instead return all possible function
  //declarations, and the SDG will still be produced correctly: Each
  //call site will link up to multiple potential functions.

  list<SgFunctionDeclaration *> retval;

  SgFunctionRefExp * fref = isSgFunctionRefExp(funcCall->get_function());
  SgFunctionSymbol * fsym = fref->get_symbol();
  SgFunctionDeclaration * fD = fsym->get_declaration();
  
  retval.push_back(fD);
  return retval;
}

set<DependenceNode *> SystemDependenceGraph::getSlice(DependenceNode * node) {

  int edgeTypes1 =
    CONTROL |
    DATA |
    SUMMARY |
    CALL;

  int edgeTypes2 = 
    CONTROL |
    DATA |
    SUMMARY |
    RETURN;

  set<DependenceNode *> start;
  start.insert(node);

  set<DependenceNode *> phase1nodes = _getReachable(start, edgeTypes1);

  set<DependenceNode *> phase2nodes = _getReachable(phase1nodes, edgeTypes2);

  return phase2nodes;

}

set<ProgramDependenceGraph *> SystemDependenceGraph::getPDGs() {
  set<ProgramDependenceGraph *> retval;
  map<SgFunctionDeclaration *, ProgramDependenceGraph *>::iterator i;
  for (i = _funcs_map.begin(); i != _funcs_map.end(); i++) {
    retval.insert(i->second);
  }
  return retval;
}
