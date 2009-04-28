#include "rose.h"
#include "SB_Graph.h"

using namespace std;


string
SB_DirectedGraph::getProperty(SB_Graph_Def::GraphProperties property, SgDirectedGraphEdge* edge) {
  string value="";
  map < int , string> node_p = edge->get_properties();
  map < int , string>::iterator prop = node_p.find(property);
  if (prop!=node_p.end()) {
    value = prop->second;
  }
  return value;
}

string
SB_DirectedGraph::getProperty(SB_Graph_Def::GraphProperties property, SgDirectedGraphNode* node) {
  string value="";
  map < int , string> node_p = node->get_properties();
  map < int , string>::iterator prop = node_p.find(property);
  if (prop!=node_p.end()) {
    value = prop->second;
  }
  return value;
}

void
SB_DirectedGraph::setProperty(SB_Graph_Def::GraphProperties property, SgDirectedGraphNode* node,
			   std::string value) {
  node->append_properties(property, value);
}

void
SB_DirectedGraph::setProperty(SB_Graph_Def::GraphProperties property, SgDirectedGraphEdge* edge,
			   std::string value) {
  edge->append_properties(property, value);
}


void
SB_DirectedGraph::getSuccessors(SgDirectedGraphNode* node, std::vector <SgDirectedGraphNode*>& vec ) {
  edgeType::iterator it1, it2;
  pair <edgeType::iterator, edgeType::iterator> iter = edges.equal_range(node);
  it1 = iter.first;
  it2 = iter.second;
  for (;it1!=it2; ++it1) {
    SgDirectedGraphEdge* edge = it1->second;
    if (edge) {
      SgDirectedGraphNode* target = isSgDirectedGraphNode(edge->get_to());
      if (target) {
	string type_n = getProperty(SB_Graph_Def::type, edge);
	if (type_n==RoseBin_support::ToString(SB_Edgetype::cfg))
	  vec.push_back(target);
      }
    }
  }
}

void
SB_DirectedGraph::getPredecessors(SgDirectedGraphNode* node, std::vector <SgDirectedGraphNode*>& vec ) {
  edgeType::iterator it1, it2;
  pair <edgeType::iterator, edgeType::iterator> iter = edgesR.equal_range(node);
  it1 = iter.first;
  it2 = iter.second;
  for (;it1!=it2; ++it1) {
    SgDirectedGraphEdge* edge = it1->second;
    if (edge) {
      SgDirectedGraphNode* source = isSgDirectedGraphNode(edge->get_from());
      if (source) {
	string type_n = getProperty(SB_Graph_Def::type, edge);
	if (type_n==RoseBin_support::ToString(SB_Edgetype::cfg))
	  vec.push_back(source);
      }
    }
  }
}

bool
SB_DirectedGraph::checkIfGraphEdgeExists(SgDirectedGraphNode* src) {
  bool exists = false;
  edgeType::iterator it = edges.find(src);
  if (it!=edges.end()) {
    exists=true;
  }
  return exists;
}

bool
SB_DirectedGraph::checkIfGraphEdgeExists(SgDirectedGraphNode* src, SgDirectedGraphNode* trg) {
  bool exists = false;
  edgeType::iterator it1, it2;
  pair <edgeType::iterator, edgeType::iterator> iter = edges.equal_range(src);
  it1 = iter.first;
  it2 = iter.second;
  for (;it1!=it2; ++it1) {
    SgDirectedGraphEdge* edge = it1->second;
    SgDirectedGraphNode* target = isSgDirectedGraphNode(edge->get_to());
    if (target==trg)
      exists=true;
  }
  return exists;
}


set <SgDirectedGraphEdge*>
SB_DirectedGraph::getEdge(SgDirectedGraphNode* src) {
  set<SgDirectedGraphEdge*> medges;
  edgeType::iterator it1, it2;
  pair <edgeType::iterator, edgeType::iterator> iter = edges.equal_range(src);
  it1 = iter.first;
  it2 = iter.second;
  for (;it1!=it2; ++it1) {
    //    SgDirectedGraphNode* source = it1->first;
    SgDirectedGraphEdge* edge = it1->second;
    medges.insert(edge);
  }
  return medges;
}

set<SgDirectedGraphEdge*>
SB_DirectedGraph::getEdge(SgDirectedGraphNode* src, SgDirectedGraphNode* trg) {
  set<SgDirectedGraphEdge*> medges;
  edgeType::iterator it1, it2;
  pair <edgeType::iterator, edgeType::iterator> iter = edges.equal_range(src);
  it1 = iter.first;
  it2 = iter.second;
  for (;it1!=it2; ++it1) {
    //    SgDirectedGraphNode* source = it1->first;
    SgDirectedGraphEdge* edge = it1->second;
    SgDirectedGraphNode* target = isSgDirectedGraphNode(edge->get_to());
    if (target==trg)
      medges.insert(edge);
  }
  return medges;
}


SgDirectedGraphEdge*
SB_DirectedGraph::createSBEdge(string& type, int graph_id,
				  SgDirectedGraphNode* from,
				  SgDirectedGraphNode* to) {
  string key = "";//getEdgeKeyName(from, from_addr, to, to_addr);
  SgDirectedGraphEdge* edge =
    new SgDirectedGraphEdge(key,type,graph_id, from, to);

  edges.insert(pair<SgDirectedGraphNode*,SgDirectedGraphEdge*>( from, edge)) ;
  edgesR.insert(pair<SgDirectedGraphNode*,SgDirectedGraphEdge*>( to, edge)) ;

  return edge;
}

SgDirectedGraphNode*
SB_DirectedGraph::createSBNode(string& name, string& type, int graph_id,
					 SgNode* int_node) {
  ROSE_ASSERT(int_node);

  SgDirectedGraphNode* n_source =
    new SgDirectedGraphNode(name,
			    type,
			    graph_id);
  ROSE_ASSERT(n_source);
  //  n_source->append_properties(address, name);
  n_source->append_properties(SB_Graph_Def::name, name);
  n_source->set_SgNode(int_node);

  return n_source;
}

SgDirectedGraphNode*
SB_DirectedGraph::getDefinitionForUsage(SgDirectedGraphNode* node) {
  edgeType::iterator it1, it2;
  pair <edgeType::iterator, edgeType::iterator> iter = edgesR.equal_range(node);
  it1 = iter.first;
  it2 = iter.second;
  for (;it1!=it2; ++it1) {
    SgDirectedGraphEdge* edge = it1->second;
    if (edge) {
      SgDirectedGraphNode* source = isSgDirectedGraphNode(edge->get_from());
      if (source) {
	string type_n = getProperty(SB_Graph_Def::type, edge);
	//if (RoseBin_support::DEBUG_MODE())
	//cout << " found a predecessor with type : " << type_n << endl;
	if (type_n==RoseBin_support::ToString(SB_Edgetype::usage))
	  return source;
      }
    }
  }
  return NULL;
}


void
SB_DirectedGraph::createUniqueEdges() {
  if (unique_edges.size()==0) {
    cerr << " Creating unique edge map. Edges : " << RoseBin_support::ToString(edges.size()) << endl;
    edgeType::const_iterator it2 = edges.begin();
    for (;it2!=edges.end();it2++) {
      SgDirectedGraphEdge* edgeIt = it2->second;
      SgDirectedGraphNode* from = isSgDirectedGraphNode(edgeIt->get_from());
      SgDirectedGraphNode* to = isSgDirectedGraphNode(edgeIt->get_to());

      edgeTypeUnique::iterator it = unique_edges.find(from);
      if (it==unique_edges.end())
	unique_edges.insert(pair<SgDirectedGraphNode*,SgDirectedGraphEdge*>( from, edgeIt)) ;
      else {
	bool found=false;
	while (it!=unique_edges.end()) {
	  SgDirectedGraphEdge* edge = it->second;
	  SgDirectedGraphNode* source = isSgDirectedGraphNode(edge->get_from());
	  SgDirectedGraphNode* target = isSgDirectedGraphNode(edge->get_to());
	  if (target==to && source==from)
	    found=true;
	  it++;
	  if (source!=from)
	    break;
	}
	if (!found)
	  unique_edges.insert(pair<SgDirectedGraphNode*,SgDirectedGraphEdge*>( from, edgeIt)) ;
      } // else
    } // for
  } // if
}


SgDirectedGraphNode*
SB_DirectedGraph::checkIfGraphNodeExists(string& trg_mnemonic) {
  SgDirectedGraphNode* node = NULL;
  nodeType::iterator it = nodes.find(trg_mnemonic);
  if (it!=nodes.end()) {
    node = it->second;
    return node;
  }
  return NULL;
}



void
SB_DirectedGraph::getDirectCFGSuccessors(SgDirectedGraphNode* node, std::vector <SgDirectedGraphNode*>& vec ) {
  edgeType::iterator it1, it2;
  pair <edgeType::iterator, edgeType::iterator> iter = edges.equal_range(node);
  it1 = iter.first;
  it2 = iter.second;
  for (;it1!=it2; ++it1) {
    SgDirectedGraphEdge* edge = it1->second;
    if (edge) {
      SgDirectedGraphNode* target = isSgDirectedGraphNode(edge->get_to());
      if (target) {
	string type_n = getProperty(SB_Graph_Def::type, edge);
	if (type_n==RoseBin_support::ToString(SB_Edgetype::cfg))
	  if (isDirectCFGEdge(target, node))
	    vec.push_back(target);
      }
    }
  }
}



void
SB_DirectedGraph::getDirectCFGPredecessors(SgDirectedGraphNode* node, std::vector <SgDirectedGraphNode*>& vec ) {
  edgeType::iterator it1, it2;
  pair <edgeType::iterator, edgeType::iterator> iter = edgesR.equal_range(node);
  it1 = iter.first;
  it2 = iter.second;
  for (;it1!=it2; ++it1) {
    SgDirectedGraphEdge* edge = it1->second;
    if (edge) {
      SgDirectedGraphNode* source = isSgDirectedGraphNode(edge->get_from());
      if (source) {
	string type_n = getProperty(SB_Graph_Def::type, edge);
	if (type_n==RoseBin_support::ToString(SB_Edgetype::cfg))
	  if (isDirectCFGEdge(node, source))
	    vec.push_back(source);
      }
    }
  }
}

bool
SB_DirectedGraph::isDirectCFGEdge(SgDirectedGraphNode* sgNode,
				       SgDirectedGraphNode* sgNodeBefore) {
  bool isDirectedControlFlowEdge = false;
  SgAsmInstruction* instSgNode = isSgAsmInstruction(sgNode->get_SgNode());
  SgAsmInstruction* instSgNodeBefore = isSgAsmInstruction(sgNodeBefore->get_SgNode());
  if (instSgNode && instSgNodeBefore) {
    SgAsmFunctionDeclaration* f1 = isSgAsmFunctionDeclaration(instSgNode->get_parent());
    SgAsmFunctionDeclaration* f2 = isSgAsmFunctionDeclaration(instSgNodeBefore->get_parent());
    if (f1==NULL)
      f1 = isSgAsmFunctionDeclaration(instSgNode->get_parent()->get_parent());
    if (f2==NULL)
      f2 = isSgAsmFunctionDeclaration(instSgNodeBefore->get_parent()->get_parent());
    if (f1 && f2 && f1==f2) {
      vector<VirtualBinCFG::CFGEdge> outEdges = instSgNodeBefore->cfgBinOutEdges(info);
      for (size_t i = 0; i < outEdges.size(); ++i) {
        if (outEdges[i].target().getNode() == instSgNode) {
          isDirectedControlFlowEdge = true;
          break;
        }
      }
    }
  }
  return isDirectedControlFlowEdge;
}

bool
SB_DirectedGraph::isValidCFGEdge(SgDirectedGraphNode* sgNode,
				       SgDirectedGraphNode* sgNodeBefore) {
  if (!sgNode || !sgNodeBefore)
    return false;
  //  bool isAUnconditionalControlTransfer = false;
  bool valid = true;
  bool isDirectedControlFlowEdge = false;
  SgAsmx86Instruction* inst = isSgAsmx86Instruction(sgNodeBefore->get_SgNode());

  SgAsmInstruction* instSgNode = isSgAsmInstruction(sgNode->get_SgNode());
  SgAsmInstruction* instSgNodeBefore = isSgAsmInstruction(sgNodeBefore->get_SgNode());
  if (instSgNode && instSgNodeBefore) {
  if (RoseBin_support::DEBUG_MODE())
    cout << " *** instSgNode && instSgNodeBefore " << endl;
    SgAsmFunctionDeclaration* f1 = isSgAsmFunctionDeclaration(instSgNode->get_parent());
    SgAsmFunctionDeclaration* f2 = isSgAsmFunctionDeclaration(instSgNodeBefore->get_parent());
    if (f1==NULL)
      f1 = isSgAsmFunctionDeclaration(instSgNode->get_parent()->get_parent());
    if (f2==NULL)
      f2 = isSgAsmFunctionDeclaration(instSgNodeBefore->get_parent()->get_parent());
    if (f1 && f2) {
      // (tps - 05/23/08) : the semantics of the previous implementation is:
      // check the node before in the instruction set and check if it is the same as the previous node
      // todo: the following line must be changed... the size of the current node does not give you the last node!
      if (RoseBin_support::DEBUG_MODE())
      cout << " *** f1 && f2 " << endl;
      SgAsmInstruction* nodeBeforeInSet = NULL;
      int byte = 1;
      while (nodeBeforeInSet==NULL && byte<8) {
	nodeBeforeInSet = info->getInstructionAtAddress(instSgNode->get_address() - byte);
	byte++;
      }
      if (RoseBin_support::DEBUG_MODE())
      cout << " *** nodeBeforeInSet = " << nodeBeforeInSet << "  instSgNodeBefore : " << instSgNodeBefore << "   byte : " << byte << endl;
      if (nodeBeforeInSet == instSgNodeBefore) {
	//if (!isAsmUnconditionalBranch(nodeBeforeInSet))
	if (RoseBin_support::DEBUG_MODE())
	cout << " isDirectedControlFlowEdge = true  --  isAsmUnconditionalBranch(nodeBeforeInSet) : " << isAsmUnconditionalBranch(nodeBeforeInSet) << endl;
	 isDirectedControlFlowEdge = true;
      }
      if (RoseBin_support::DEBUG_MODE()) {
      cout << " *** f1 && f2 -- isDirectionalControlFlowEdge: " << isDirectedControlFlowEdge << endl;
      cout << " inst->get_kind() == x86_call : " << (inst->get_kind() == x86_call) << "     inst->get_kind() == x86_ret : " << (inst->get_kind() == x86_ret) << endl;
      }
      if ((inst->get_kind() == x86_call || inst->get_kind() == x86_ret) && isDirectedControlFlowEdge)
	valid=false;
    }
  }
  /*
  if (RoseBin_support::DEBUG_MODE()) {
    cout << " ValidCFGEdge::: sgNode " << sgNode->get_name() <<
      "   sgNodeBefore " << sgNodeBefore->get_name() <<
      "   instSgNode << " << instSgNode <<
      "   instSgNodeBefore << " << instSgNodeBefore <<
      "   is Valid node ? " << RoseBin_support::resBool(valid) <<
      "   isControlFlowEdge " << RoseBin_support::resBool(isDirectedControlFlowEdge) << endl;
  }
  */

  return valid;
}



SgDirectedGraphNode*
SB_DirectedGraph::createNode(string& name, string& type, int address, int graph_id,
			     bool isFunction, SgNode* int_node) {
  ROSE_ASSERT(int_node);

  ostringstream addrhex;
  addrhex << hex << setw(8) << address ;
  string addr_str = addrhex.str();

  SgDirectedGraphNode* n_source =
    createSBNode(addr_str, type, graph_id, int_node);
  ROSE_ASSERT(n_source);

  if (isFunction)
    nodes[addr_str+"_f"]=n_source;
  else
    nodes[addr_str]=n_source;
  return n_source;
}

SgDirectedGraphEdge*
SB_DirectedGraph::createEdge(string& type, int graph_id,
				  SgDirectedGraphNode* from, int from_addr,
				  SgDirectedGraphNode* to, int to_addr) {
	  SgDirectedGraphEdge* edge =
	createSBEdge(type, graph_id, from, to);
  return edge;
}

