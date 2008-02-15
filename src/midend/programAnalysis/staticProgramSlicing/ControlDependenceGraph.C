#ifdef HAVE_CONFIG_H
//#include <config.h>
#endif

#include "rose.h"

#include "DependenceGraph.h"
#include "DominatorTree.h"
#include <set>

using namespace DominatorTreesAndDominanceFrontiers;
using namespace std;

ControlDependenceGraph::ControlDependenceGraph (SgNode * head, InterproceduralInfo * ii) {
  _head = head;
  _interprocedural = ii;
  _dt = new DominatorTree(head, DominatorTree::POST);
  _cfg = _dt->getCFG();
  _df = new DominanceFrontier(_dt);
  _buildCDG();
}

DependenceNode * ControlDependenceGraph::createNodeC(ControlNode * cnode) {
  DependenceNode * newNode;
  if (_cnode_map.count(cnode) == 0) {
    if (cnode->getType() == ControlNode::EMPTY) {
      newNode = createNode(new DependenceNode(DependenceNode::CONTROL));
    } else {
      SgNode * tmp = cnode->getNode();
      while (!isSgStatement(tmp)) {
	tmp = tmp->get_parent();
      }
      newNode = createNode(tmp);
    }

    _cnode_map[cnode] = newNode;
    return newNode;
  } else {
    return _cnode_map[cnode];
  }
}

void ControlDependenceGraph::_buildCDG() {

  //Go through the dominance frontier. For every SgNode with a
  //non-empty frontier, add dependences from the nodes in the frontier
  //to that node.

  int size = _dt->getSize();
  for (int i = 0; i < size; i++) {
    set<int> frontier = _df->getFrontier(i);
    
    if (frontier.empty()) {//if there is no frontier, we want to just add the node as an empty node
      ControlNode * cfg_node = _cfg->getNode(i, ControlFlowGraph::BACKWARD);
      if (cfg_node->getType() == ControlNode::EMPTY) continue;
      createNodeC(cfg_node);
    }

    ControlNode * cfg_to = _cfg->getNode(i, ControlFlowGraph::BACKWARD);
    if (cfg_to->getType() == ControlNode::EMPTY) continue;
    DependenceNode * to = createNodeC(cfg_to);

    for (set<int>::iterator j = frontier.begin(); j != frontier.end(); j++) {
      int nodeIndex = *j;

      ControlNode * cfg_from = _cfg->getNode(nodeIndex, ControlFlowGraph::BACKWARD);

      if (cfg_from->getType() == ControlNode::EMPTY)
	continue;

      DependenceNode * from = createNodeC(cfg_from);

      //link them (we need to link the nodes from the new graph, not the originals)
      establishEdge(from, to);

      //If the "to" link is a break or a continue statement, we need
      //to link it in the opposite direction too, to ensure that they
      //are correctly accounted for when calculating
      //slices.
      /* If we have something like this:

      while (x < 5) {
         if (x == 3)
	    break;
	 x++;
      }

      We would determine that (x < 5) is control dependent on (x ==
      3). This is true, but is only the case because of the break
      statement. Thus, whenever (x == 3) is involved, we would also
      like the break statement to be involved.

      NOTE: I'm not convinced that making (x == 3) control dependent
      on the break statement is the best way to do this, but it
      suffices for now.
       */

      if ((to->getType() == DependenceNode::SGNODE) &&
	  ((isSgBreakStmt(to->getNode())) || (isSgContinueStmt(to->getNode()))))
	establishEdge(to, from);
    }
  }

  if (_interprocedural != NULL) {
    _buildInterprocedural();
  }

  printf("\n\n");

}

void ControlDependenceGraph::_buildInterprocedural() {


  //Go through the SGNODE dependence nodes and create the appropriate
  //call site nodes, entry nodes etc.

  SgFunctionDefinition * func = isSgFunctionDefinition(_head);
  ROSE_ASSERT(func != NULL);

  //First create the entry node for the procedure
  _interprocedural->procedureEntry.entry = new DependenceNode(DependenceNode::ENTRY, func->get_declaration());
  DependenceNode * entry = createNode(_interprocedural->procedureEntry.entry);

  //Link the entry node up with all the nodes in the CDG which do not have predecessors
  for (set<SimpleDirectedGraphNode *>::iterator i = _nodes.begin(); i != _nodes.end(); i++) {
    DependenceNode * node = dynamic_cast<DependenceNode *>(*i);
    if ((node->numPredecessors() == 0) && (node != entry)) {
      establishEdge(entry, node);
    }
  }

  //create a formal out return argument, control dependent on the entry node
  string return_name = func->get_declaration()->get_name().str();
  return_name = return_name + " return";
  _interprocedural->procedureEntry.formal_return = new DependenceNode(DependenceNode::FORMALOUT, return_name);
  DependenceNode * formal_return = createNode(_interprocedural->procedureEntry.formal_return);
  establishEdge(entry, formal_return);

  //for each of the arguments in the function parameter list, add a formal-in and formal-out node
  SgFunctionParameterList * paramlist = func->get_declaration()->get_parameterList();
  SgInitializedNamePtrList params = paramlist->get_args();
  for (SgInitializedNamePtrList::iterator i = params.begin(); i != params.end(); i++) {
    SgInitializedName * name = *i;
    DependenceNode * formal_in = new DependenceNode(DependenceNode::FORMALIN, name->get_name().str());
    DependenceNode * formal_out = new DependenceNode(DependenceNode::FORMALOUT, name->get_name().str());
    establishEdge(entry, createNode(formal_in));
    establishEdge(entry, createNode(formal_out));
    _interprocedural->procedureEntry.formal_in[name] = formal_in;
    _interprocedural->procedureEntry.formal_out[name] = formal_out;

    //To preserve the order of arguments, we insert them into arg_order
    _interprocedural->procedureEntry.arg_order.push_back(name);
  }

  //Now we go through each of the SgNodes in our CDG. If any of them
  //contain a function call, we want to build a call site node for them.
  map<SgNode *, DependenceNode *>::iterator sgnode_iterator;
  for (sgnode_iterator = _sgnode_map.begin(); sgnode_iterator != _sgnode_map.end(); sgnode_iterator++) {
    SgNode * currnode = sgnode_iterator->first;

    list<SgFunctionCallExp *> calls = InterproceduralInfo::extractFunctionCalls(currnode);
    if (calls.empty()) continue;

    for (list<SgFunctionCallExp *>::iterator i = calls.begin(); i != calls.end(); i++) {
      SgFunctionCallExp * call = *i;

      //This needs to be replaced with some call graph analysis
      SgFunctionRefExp * func = isSgFunctionRefExp(call->get_function());
      ROSE_ASSERT(func != NULL);
      SgName func_name = func->get_symbol()->get_name();

      InterproceduralInfo::CallSiteStructure callstructure;
      callstructure.callsite = new DependenceNode(DependenceNode::CALLSITE, call);
      //the call site is control dependent on the statement (i.e. for
      //the call site to happen, the statement must be executed)
      DependenceNode * callsite = createNode(callstructure.callsite);
      //addLink(callsite, getNode(currnode));
      establishEdge(getNode(currnode), callsite);

      //create an actual out node for the return value, control dependent on callsite
      string return_name = func_name.str();
      return_name = return_name + " return";
      callstructure.actual_return = new DependenceNode(DependenceNode::ACTUALOUT, return_name);
      DependenceNode * actual_return = createNode(callstructure.actual_return);
      establishEdge(callsite, actual_return);

      //For each argument in the function call, build an actual_in and actual_out, control dependent on callsite
      SgExpressionPtrList args = call->get_args()->get_expressions();
      for (SgExpressionPtrList::iterator j = args.begin(); j != args.end(); j++) {
	SgExpression * arg = *j;
	DependenceNode * actual_in = new DependenceNode(DependenceNode::ACTUALIN, arg);
	DependenceNode * actual_out = new DependenceNode(DependenceNode::ACTUALOUT, arg);
	establishEdge(callsite, createNode(actual_in));
	establishEdge(callsite, createNode(actual_out));
	callstructure.actual_in[arg] = actual_in;
	callstructure.actual_out[arg] = actual_out;

	//To preserve the order of expressions in the parameter list, we insert them into expr_order
	callstructure.expr_order.push_back(arg);
      }

      //add the callstructure to interprocedural info
      _interprocedural->callsite_map[call] = callstructure;
    }
  }
}
