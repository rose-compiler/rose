/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 27 Sep07
 * Decription : DotGraph Visualization
 ****************************************************/

#include "rose.h"
#include "RoseBin_Graph.h"

using namespace std;

string 
RoseBin_Graph::getProperty(RoseBin_Def::GraphProperties property, SgDirectedGraphEdge* edge) {
  string value="";
  map < int , string> node_p = edge->get_properties();
  map < int , string>::iterator prop = node_p.find(property);
  if (prop!=node_p.end()) {
    value = prop->second;
  }
  return value;
}

string 
RoseBin_Graph::getProperty(RoseBin_Def::GraphProperties property, SgDirectedGraphNode* node) {
  string value="";
  map < int , string> node_p = node->get_properties();
  map < int , string>::iterator prop = node_p.find(property);
  if (prop!=node_p.end()) {
    value = prop->second;
  }
  return value;
}

void 
RoseBin_Graph::setProperty(RoseBin_Def::GraphProperties property, SgDirectedGraphNode* node,
			   std::string value) {
  node->append_properties(property, value);
}

void 
RoseBin_Graph::setProperty(RoseBin_Def::GraphProperties property, SgDirectedGraphEdge* edge,
			   std::string value) {
  edge->append_properties(property, value);
}


void 
RoseBin_Graph::getSuccessors(SgDirectedGraphNode* node, std::vector <SgDirectedGraphNode*>& vec ) {
  edgeType::iterator it1, it2;
  pair <edgeType::iterator, edgeType::iterator> iter = edges.equal_range(node);
  it1 = iter.first;
  it2 = iter.second;
  for (;it1!=it2; ++it1) {
    SgDirectedGraphEdge* edge = it1->second;
    if (edge) {
      SgDirectedGraphNode* target = isSgDirectedGraphNode(edge->get_to());
      if (target) {
	string type_n = getProperty(RoseBin_Def::type, edge);
	if (type_n==RoseBin_support::ToString(RoseBin_Edgetype::cfg))
	  vec.push_back(target);
      }
    }
  }
}

void 
RoseBin_Graph::getDirectCFGSuccessors(SgDirectedGraphNode* node, std::vector <SgDirectedGraphNode*>& vec ) {
  edgeType::iterator it1, it2;
  pair <edgeType::iterator, edgeType::iterator> iter = edges.equal_range(node);
  it1 = iter.first;
  it2 = iter.second;
  for (;it1!=it2; ++it1) {
    SgDirectedGraphEdge* edge = it1->second;
    if (edge) {
      SgDirectedGraphNode* target = isSgDirectedGraphNode(edge->get_to());
      if (target) {
	string type_n = getProperty(RoseBin_Def::type, edge);
	if (type_n==RoseBin_support::ToString(RoseBin_Edgetype::cfg))
	  if (isDirectCFGEdge(target, node))
	    vec.push_back(target);
      }
    }
  }
}

void 
RoseBin_Graph::getPredecessors(SgDirectedGraphNode* node, std::vector <SgDirectedGraphNode*>& vec ) {
  edgeType::iterator it1, it2;
  pair <edgeType::iterator, edgeType::iterator> iter = edgesR.equal_range(node);
  it1 = iter.first;
  it2 = iter.second;
  for (;it1!=it2; ++it1) {
    SgDirectedGraphEdge* edge = it1->second;
    if (edge) {
      SgDirectedGraphNode* source = isSgDirectedGraphNode(edge->get_from());
      if (source) {
	string type_n = getProperty(RoseBin_Def::type, edge);
	if (type_n==RoseBin_support::ToString(RoseBin_Edgetype::cfg))
	  vec.push_back(source);
      }
    }
  }
}

void 
RoseBin_Graph::getDirectCFGPredecessors(SgDirectedGraphNode* node, std::vector <SgDirectedGraphNode*>& vec ) {
  edgeType::iterator it1, it2;
  pair <edgeType::iterator, edgeType::iterator> iter = edgesR.equal_range(node);
  it1 = iter.first;
  it2 = iter.second;
  for (;it1!=it2; ++it1) {
    SgDirectedGraphEdge* edge = it1->second;
    if (edge) {
      SgDirectedGraphNode* source = isSgDirectedGraphNode(edge->get_from());
      if (source) {
	string type_n = getProperty(RoseBin_Def::type, edge);
	if (type_n==RoseBin_support::ToString(RoseBin_Edgetype::cfg))
	  if (isDirectCFGEdge(node, source))
	    vec.push_back(source);
      }
    }
  }
}

bool 
RoseBin_Graph::isDirectCFGEdge(SgDirectedGraphNode* sgNode,
				       SgDirectedGraphNode* sgNodeBefore) {
  bool isDirectedControlFlowEdge = false;
  SgAsmInstruction* instSgNode = isSgAsmInstruction(sgNode->get_SgNode());
  SgAsmInstruction* instSgNodeBefore = isSgAsmInstruction(sgNodeBefore->get_SgNode());
  if (instSgNode && instSgNodeBefore) {
    SgAsmFunctionDeclaration* f1 = isSgAsmFunctionDeclaration(instSgNode->get_parent());
    SgAsmFunctionDeclaration* f2 = isSgAsmFunctionDeclaration(instSgNodeBefore->get_parent());
    if (f1 && f2) {
      isDirectedControlFlowEdge = instSgNode->isSuccessorControlFlowEdge(instSgNodeBefore);
    }
  }
  return isDirectedControlFlowEdge;
}

bool 
RoseBin_Graph::isValidCFGEdge(SgDirectedGraphNode* sgNode,
				       SgDirectedGraphNode* sgNodeBefore) {
  if (!sgNode || !sgNodeBefore)
    return false;
  //  bool isAUnconditionalControlTransfer = false;
  bool valid = true;
  bool isDirectedControlFlowEdge = false;

  //  SgAsmx86UnConditionalControlTransferInstruction* instCall_Before_uncond = 
  //isSgAsmx86UnConditionalControlTransferInstruction(sgNodeBefore->get_SgNode());  

  SgAsmx86Call* instCall_Before_uncond = isSgAsmx86Call(sgNodeBefore->get_SgNode());
  SgAsmx86Ret* instRet_Before_uncond = isSgAsmx86Ret(sgNodeBefore->get_SgNode());

  SgAsmInstruction* instSgNode = isSgAsmInstruction(sgNode->get_SgNode());
  SgAsmInstruction* instSgNodeBefore = isSgAsmInstruction(sgNodeBefore->get_SgNode());
  if (instSgNode && instSgNodeBefore) {
    SgAsmFunctionDeclaration* f1 = isSgAsmFunctionDeclaration(instSgNode->get_parent());
    SgAsmFunctionDeclaration* f2 = isSgAsmFunctionDeclaration(instSgNodeBefore->get_parent());
    if (f1 && f2) {
      isDirectedControlFlowEdge = instSgNode->isSuccessorControlFlowEdge(instSgNodeBefore);
      if ((instCall_Before_uncond || instRet_Before_uncond) && isDirectedControlFlowEdge)
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
RoseBin_Graph::getDefinitionForUsage(SgDirectedGraphNode* node) {
  edgeType::iterator it1, it2;
  pair <edgeType::iterator, edgeType::iterator> iter = edgesR.equal_range(node);
  it1 = iter.first;
  it2 = iter.second;
  for (;it1!=it2; ++it1) {
    SgDirectedGraphEdge* edge = it1->second;
    if (edge) {
      SgDirectedGraphNode* source = isSgDirectedGraphNode(edge->get_from());
      if (source) {
	string type_n = getProperty(RoseBin_Def::type, edge);
	cout << " found a predecessor with type : " << type_n << endl;
	if (type_n==RoseBin_support::ToString(RoseBin_Edgetype::usage)) 
	  return source;
      }
    }
  }
  return NULL;
}

SgDirectedGraphNode*
RoseBin_Graph::createNode(string& name, string& type, int address, int graph_id, 
			     bool isFunction, SgNode* int_node) {
  ROSE_ASSERT(int_node);
  
  ostringstream addrhex;
  addrhex << hex << setw(8) << address ;
  string addr_str = addrhex.str();

  SgDirectedGraphNode* n_source = 
    new SgDirectedGraphNode(addr_str,
			    type,
			    graph_id);
  ROSE_ASSERT(n_source);
  //  n_source->append_properties(address, name);
  n_source->append_properties(RoseBin_Def::name, name);
  n_source->set_SgNode(int_node);

  //nodes[addr_str]=name;
  if (isFunction)
    nodes[addr_str+"_f"]=n_source;
  else
    nodes[addr_str]=n_source;
  return n_source;
}

void
RoseBin_Graph::createUniqueEdges() {
  if (unique_edges.size()==0) {
    cerr << " Creating unique edge map. " << endl;
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
	}
	if (!found) 
	  unique_edges.insert(pair<SgDirectedGraphNode*,SgDirectedGraphEdge*>( from, edgeIt)) ;
      } // else
    } // for
  } // if
}

SgDirectedGraphEdge*
 RoseBin_Graph::createEdge(string& type, int graph_id,
				  SgDirectedGraphNode* from, int from_addr,
				  SgDirectedGraphNode* to, int to_addr) {
  string key = "";//getEdgeKeyName(from, from_addr, to, to_addr);
  SgDirectedGraphEdge* edge =
    new SgDirectedGraphEdge(key,type,graph_id, from, to);

  edges.insert(pair<SgDirectedGraphNode*,SgDirectedGraphEdge*>( from, edge)) ;
  edgesR.insert(pair<SgDirectedGraphNode*,SgDirectedGraphEdge*>( to, edge)) ;

  return edge;
}


SgDirectedGraphNode* 
RoseBin_Graph::checkIfGraphNodeExists(string& trg_mnemonic) {
  SgDirectedGraphNode* node = NULL;
  nodeType::iterator it = nodes.find(trg_mnemonic);
  if (it!=nodes.end()) {
    node = it->second;
    return node;
  }
  return NULL;
}

bool 
RoseBin_Graph::checkIfGraphEdgeExists(SgDirectedGraphNode* src) {
  bool exists = false;
  edgeType::iterator it = edges.find(src);
  if (it!=edges.end()) {
    exists=true;
  }
  return exists;
}

bool
RoseBin_Graph::checkIfGraphEdgeExists(SgDirectedGraphNode* src, SgDirectedGraphNode* trg) {
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
RoseBin_Graph::getEdge(SgDirectedGraphNode* src) {
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
RoseBin_Graph::getEdge(SgDirectedGraphNode* src, SgDirectedGraphNode* trg) {
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

