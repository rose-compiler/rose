// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "GraphAlgorithms.h"

using namespace std;

SgGraphNode*
GraphAlgorithms::getDefinitionForUsage(RoseBin_Graph* vizzGraph,
                                       SgGraphNode* node) {
  rose_graph_integer_edge_hash_multimap::iterator it1, it2;
  pair <rose_graph_integer_edge_hash_multimap::iterator, rose_graph_integer_edge_hash_multimap::iterator> iter =
    vizzGraph->get_node_index_to_edge_multimap_edgesIn().equal_range(node->get_index());
  it1 = iter.first;
  it2 = iter.second;
  for (;it1!=it2; ++it1) {
    SgDirectedGraphEdge* edge = isSgDirectedGraphEdge(it1->second);
    if (edge) {
      SgGraphNode* source = isSgGraphNode(edge->get_from());
      if (source) {
        string type_n = vizzGraph->getProperty(SgGraph::type, edge);
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
GraphAlgorithms::getDirectCFGSuccessors(RoseBin_Graph* vizzGraph,
                                        SgGraphNode* node, std::vector <SgGraphNode*>& vec ) {
  //SgGraphEdgeList* gedges = vizzGraph->get_edges();
  rose_graph_integer_edge_hash_multimap edges = vizzGraph->get_node_index_to_edge_multimap_edgesOut();
  rose_graph_integer_edge_hash_multimap::iterator it1, it2;
  pair <rose_graph_integer_edge_hash_multimap::iterator, rose_graph_integer_edge_hash_multimap::iterator> iter =
    edges.equal_range(node->get_index());
  it1 = iter.first;
  it2 = iter.second;
  //cerr << "    DirectCFGSucc >>>>>>>>>>>>>>> getDirect - Outedges for node " << endl; 
  for (;it1!=it2; ++it1) {
    SgDirectedGraphEdge* edge = isSgDirectedGraphEdge(it1->second);
    ROSE_ASSERT(edge);
    //cerr << "  found edge = " << edge ;
    if (edge) {
      SgGraphNode* target = isSgGraphNode(edge->get_to());
      //cerr << "  with target node = " << target->get_name() << endl;
      ROSE_ASSERT(target);
      if (target) {
        string type_n = vizzGraph->getProperty(SgGraph::type, edge);
        if (type_n==RoseBin_support::ToString(SgGraph::cfg))
          if (isDirectCFGEdge(target, node))
            vec.push_back(target);
      }
    }
  }
}



void
GraphAlgorithms::getDirectCFGPredecessors(RoseBin_Graph* vizzGraph,
                                          SgGraphNode* node, std::vector <SgGraphNode*>& vec ) {
  rose_graph_integer_edge_hash_multimap::iterator it1, it2;
  pair <rose_graph_integer_edge_hash_multimap::iterator, rose_graph_integer_edge_hash_multimap::iterator> iter =
          vizzGraph->get_node_index_to_edge_multimap_edgesIn().equal_range(node->get_index());
  it1 = iter.first;
  it2 = iter.second;
  for (;it1!=it2; ++it1) {
    SgDirectedGraphEdge* edge = isSgDirectedGraphEdge(it1->second);
    if (edge) {
      SgGraphNode* source = isSgGraphNode(edge->get_from());
      if (source) {
        string type_n = vizzGraph->getProperty(SgGraph::type, edge);
        if (type_n==RoseBin_support::ToString(SgGraph::cfg))
          if (isDirectCFGEdge(node, source))
            vec.push_back(source);
      }
    }
  }
}

bool
GraphAlgorithms::isDirectCFGEdge(SgGraphNode* sgNode,
                                 SgGraphNode* sgNodeBefore) {
  bool isDirectedControlFlowEdge = false;
  SgAsmInstruction* instSgNode = isSgAsmInstruction(sgNode->get_SgNode());
  SgAsmInstruction* instSgNodeBefore = isSgAsmInstruction(sgNodeBefore->get_SgNode());
  ROSE_ASSERT(instSgNode);
  ROSE_ASSERT(instSgNodeBefore);
  if (instSgNode && instSgNodeBefore) {
    SgAsmFunctionDeclaration* f1 = isSgAsmFunctionDeclaration(instSgNode->get_parent());
    SgAsmFunctionDeclaration* f2 = isSgAsmFunctionDeclaration(instSgNodeBefore->get_parent());
    if (f1==NULL)
      f1 = isSgAsmFunctionDeclaration(instSgNode->get_parent()->get_parent());
    if (f2==NULL)
      f2 = isSgAsmFunctionDeclaration(instSgNodeBefore->get_parent()->get_parent());
    //cerr << "           -- Checking DirectCFG : f1 == f2?  : " << f1 << " " << f2 << 
    //  "  node: " << sgNode->get_name() << "  and before: " << sgNodeBefore->get_name() << endl;
    if (f1 && f2 && f1==f2) {
      ROSE_ASSERT(info);
      vector<VirtualBinCFG::CFGEdge> outEdges = instSgNodeBefore->cfgBinOutEdges(info);
      //cerr << "           -- Checking DirectCFG between: " << sgNode->get_name() << "  and before: " << 
      //        sgNodeBefore->get_name() << "    nr outedges: " << outEdges.size() << endl;
      for (size_t i = 0; i < outEdges.size(); ++i) {
        if (outEdges[i].target().getNode() == instSgNode) {
          isDirectedControlFlowEdge = true;
          break;
        }
      }
    }
  }
  //  cerr << "     ... checking if isDirectedCFGEdge " << isDirectedControlFlowEdge << endl;
  return isDirectedControlFlowEdge;
}

bool
GraphAlgorithms::isValidCFGEdge(SgGraphNode* sgNode,
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
      ROSE_ASSERT(info);
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








