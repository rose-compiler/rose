// tps (1/14/2010) : Switching from rose.h to sage3 changed size from 17,9 MB to 0,002MB
//#include "rose.h"
//#include "SB_Graph.h"

//using namespace std;

#if 0

string
SB_DirectedGraph::getProperty(SgGraph::GraphProperties property, SgGraphEdge* edge) {
  string value="";
  map < int , string> node_p = edge->get_properties();
  map < int , string>::iterator prop = node_p.find(property);
  if (prop!=node_p.end()) {
    value = prop->second;
  }
  return value;
}

string
SB_DirectedGraph::getProperty(SgGraph::GraphProperties property, SgGraphNode* node) {
  string value="";
  map < int , string> node_p = node->get_properties();
  map < int , string>::iterator prop = node_p.find(property);
  if (prop!=node_p.end()) {
    value = prop->second;
  }
  return value;
}

void
SB_DirectedGraph::setProperty(SgGraph::GraphProperties property, SgGraphNode* node,
                              std::string value) {
  node->append_properties(property, value);
}

void
SB_DirectedGraph::setProperty(SgGraph::GraphProperties property, SgGraphEdge* edge,
                              std::string value) {
  edge->append_properties(property, value);
}

bool
SB_DirectedGraph::checkIfGraphEdgeExists(SgGraphNode* src) {
  //  SgGraphEdgeList* gedges = get_edges();
  rose_graph_node_edge_hash_multimap edges = get_edges()->get_edges();
  bool exists = false;
  rose_graph_node_edge_hash_multimap::iterator it = edges.find(src);
  if (it!=edges.end()) {
    exists=true;
  }
  return exists;
}

SgGraphNode*
SB_DirectedGraph::checkIfGraphNodeExists(string& trg_mnemonic) {
  SgGraphNode* node = NULL;
  //SgGraphNodeList* gnodes = get_nodes();
  rose_graph_hash_multimap nodes = get_nodes()->get_nodes();
  rose_graph_hash_multimap::iterator it = nodes.find(trg_mnemonic);
  if (it!=nodes.end()) {
    node =  it->second;
    return node;
  }
  return NULL;
}


set<SgDirectedGraphEdge*>
SB_DirectedGraph::getDirectedEdge(SgGraphNode* src, SgGraphNode* trg) {
  //SgGraphEdgeList* gedges = get_edges();
  rose_graph_node_edge_hash_multimap edges = get_edges()->get_edges();
  set<SgDirectedGraphEdge*> medges;
  rose_graph_node_edge_hash_multimap::iterator it1, it2;
  pair <rose_graph_node_edge_hash_multimap::iterator, rose_graph_node_edge_hash_multimap::iterator> iter =
          edges.equal_range(src);
  it1 = iter.first;
  it2 = iter.second;
  for (;it1!=it2; ++it1) {
    //    SgGraphNode* source = it1->first;
    SgDirectedGraphEdge* edge = isSgDirectedGraphEdge(it1->second);
    SgGraphNode* target = isSgGraphNode(edge->get_to());
    if (target==trg)
      medges.insert(edge);
  }
  return medges;
}

bool
SB_DirectedGraph::checkIfDirectedGraphEdgeExists(SgGraphNode* src, SgGraphNode* trg) {
  //  SgGraphEdgeList* gedges = get_edges();
  rose_graph_node_edge_hash_multimap edges = get_edges()->get_edges();
  bool exists = false;
  rose_graph_node_edge_hash_multimap::iterator it1, it2;
  pair <rose_graph_node_edge_hash_multimap::iterator, rose_graph_node_edge_hash_multimap::iterator> iter = edges.equal_range(src);
  it1 = iter.first;
  it2 = iter.second;
  for (;it1!=it2; ++it1) {
    SgDirectedGraphEdge* edge = isSgDirectedGraphEdge(it1->second);
    SgGraphNode* target = isSgGraphNode(edge->get_to());
    if (target==trg)
      exists=true;
  }
  return exists;
}



#if 1
     SgGraphNode*
SB_DirectedGraph::addNode( const std::string & name, SgNode* sg_node )
{
  ROSE_ASSERT(sg_node);
  SgGraphNode* n_source =
    new SgGraphNode(name);
  ROSE_ASSERT(n_source);
  n_source->set_SgNode(sg_node);
  n_source->set_name(name);
  get_nodes()->get_nodes().insert(make_pair(name,n_source));
  //nodes->get_nodes()[name]=n_source;
  return n_source;
}
#endif


void
SB_DirectedGraph::getSuccessors(SgGraphNode* node, std::vector <SgGraphNode*>& vec ) {
  //  SgGraphEdgeList* gedges = get_edges();
  rose_graph_node_edge_hash_multimap edges = get_edges()->get_edges();
  rose_graph_node_edge_hash_multimap::iterator it1, it2;
  pair <rose_graph_node_edge_hash_multimap::iterator, rose_graph_node_edge_hash_multimap::iterator> iter =
          edges.equal_range(node);
  it1 = iter.first;
  it2 = iter.second;
  for (;it1!=it2; ++it1) {
    SgDirectedGraphEdge* edge = isSgDirectedGraphEdge(it1->second);
    if (edge) {
      SgGraphNode* target = isSgGraphNode(edge->get_to());
      if (target) {
        string type_n = getProperty(SgGraph::type, edge);
        if (type_n==RoseBin_support::ToString(SgGraph::cfg))
          vec.push_back(target);
      }
    }
  }
}

void
SB_DirectedGraph::getPredecessors(SgGraphNode* node, std::vector <SgGraphNode*>& vec ) {
  rose_graph_node_edge_hash_multimap::iterator it1, it2;
  pair <rose_graph_node_edge_hash_multimap::iterator, rose_graph_node_edge_hash_multimap::iterator> iter =
          get_reverse_edges()->get_edges().equal_range(node);
  it1 = iter.first;
  it2 = iter.second;
  for (;it1!=it2; ++it1) {
    SgDirectedGraphEdge* edge = isSgDirectedGraphEdge(it1->second);
    if (edge) {
      SgGraphNode* source = isSgGraphNode(edge->get_from());
      if (source) {
        string type_n = getProperty(SgGraph::type, edge);
        if (type_n==RoseBin_support::ToString(SgGraph::cfg))
          vec.push_back(source);
      }
    }
  }
}

#endif







#if 0
SgGraphNode*
SB_DirectedGraph::createNode(string& name, string& type, int address, int graph_id,
                             bool isFunction, SgNode* int_node) {
  ROSE_ASSERT(int_node);
#if 0
  ostringstream addrhex;
  addrhex << hex << setw(8) << address ;
  string addr_str = addrhex.str();
  SgGraphNode* n_source = NULL;
  if (isFunction) {
          addr_str+="_f";
      createSBNode(addr_str, type, graph_id, int_node);
  } else
      createSBNode(addr_str, type, graph_id, int_node);
#endif
  ROSE_ASSERT(n_source);
    nodes.insert(make_pair(addr_str,n_source));
  return n_source;
}

#endif





#if 0

SgDirectedGraphEdge*
SB_DirectedGraph::createSBEdge(string& type, int graph_id,
                                  SgGraphNode* from,
                                  SgGraphNode* to) {
  string key = "";//getEdgeKeyName(from, from_addr, to, to_addr);
  SgDirectedGraphEdge* edge =
    new SgDirectedGraphEdge(from, to,type);

  edges.insert(pair<SgGraphNode*,SgDirectedGraphEdge*>( from, edge)) ;
  redges.insert(pair<SgGraphNode*,SgDirectedGraphEdge*>( to, edge)) ;

  return edge;
}

SgDirectedGraphEdge*
SB_DirectedGraph::createEdge(string& type, int graph_id,
                                  SgGraphNode* from, int from_addr,
                                  SgGraphNode* to, int to_addr) {
          SgDirectedGraphEdge* edge =
        createSBEdge(type, graph_id, from, to);
  return edge;
}
#endif











#if 0



SgGraphNode*
SB_DirectedGraph::getDefinitionForUsage(SgGraphNode* node) {
  rose_graph_node_edge_hash_multimap::iterator it1, it2;
  pair <rose_graph_node_edge_hash_multimap::iterator, rose_graph_node_edge_hash_multimap::iterator> iter =
          get_reverse_edges()->get_edges().equal_range(node);
  it1 = iter.first;
  it2 = iter.second;
  for (;it1!=it2; ++it1) {
    SgDirectedGraphEdge* edge = isSgDirectedGraphEdge(it1->second);
    if (edge) {
      SgGraphNode* source = isSgGraphNode(edge->get_from());
      if (source) {
        string type_n = getProperty(SgGraph::type, edge);
        //if (RoseBin_support::DEBUG_MODE())
        //cout << " found a predecessor with type : " << type_n << endl;
        if (type_n==RoseBin_support::ToString(SgGraph::usage))
          return source;
      }
    }
  }
  return NULL;
}


void
SB_DirectedGraph::getDirectCFGSuccessors(SgGraphNode* node, std::vector <SgGraphNode*>& vec ) {
  //  SgGraphEdgeList* gedges = get_edges();
  rose_graph_node_edge_hash_multimap& edges = get_edges()->get_edges();
  rose_graph_node_edge_hash_multimap::iterator it1, it2;
  pair <rose_graph_node_edge_hash_multimap::iterator, rose_graph_node_edge_hash_multimap::iterator> iter = edges.equal_range(node);
  it1 = iter.first;
  it2 = iter.second;
  for (;it1!=it2; ++it1) {
    SgDirectedGraphEdge* edge = isSgDirectedGraphEdge(it1->second);
    if (edge) {
      SgGraphNode* target = isSgGraphNode(edge->get_to());
      if (target) {
        string type_n = getProperty(SgGraph::type, edge);
        if (type_n==RoseBin_support::ToString(SgGraph::cfg))
          if (isDirectCFGEdge(target, node))
            vec.push_back(target);
      }
    }
  }
}



void
SB_DirectedGraph::getDirectCFGPredecessors(SgGraphNode* node, std::vector <SgGraphNode*>& vec ) {
  rose_graph_node_edge_hash_multimap::iterator it1, it2;
  pair <rose_graph_node_edge_hash_multimap::iterator, rose_graph_node_edge_hash_multimap::iterator> iter =
          get_reverse_edges()->get_edges().equal_range(node);
  it1 = iter.first;
  it2 = iter.second;
  for (;it1!=it2; ++it1) {
    SgDirectedGraphEdge* edge = isSgDirectedGraphEdge(it1->second);
    if (edge) {
      SgGraphNode* source = isSgGraphNode(edge->get_from());
      if (source) {
        string type_n = getProperty(SgGraph::type, edge);
        if (type_n==RoseBin_support::ToString(SgGraph::cfg))
          if (isDirectCFGEdge(node, source))
            vec.push_back(source);
      }
    }
  }
}

bool
SB_DirectedGraph::isDirectCFGEdge(SgGraphNode* sgNode,
                                  SgGraphNode* sgNodeBefore) {
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
SB_DirectedGraph::isValidCFGEdge(SgGraphNode* sgNode,
                                 SgGraphNode* sgNodeBefore) {
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



#endif
