#include "rose.h"

#include "DependenceGraph.h"

char* DependenceNode::typeNames[DependenceNode::NUM_NODE_TYPES] = {
  "CONTROL",
  "SGNODE",
  "CALLSITE",
  "ACTUALIN",
  "ACTUALOUT",
  "FORMALIN",
  "FORMALOUT",
  "ENTRY"
};

char* DependenceGraph::edgeNames[DependenceGraph::NUM_EDGE_TYPES] = {
  "", "CONTROL", "DATA", "", "SUMMARY", "", "", "", "CALL", "", "", "", "", "", "", "", "RETURN"
};

DependenceNode * DependenceGraph::createNode(DependenceNode * node) {
  DependenceNode * newNode = getNode(node);

  //If the node isn't in the map, create it.
  if (newNode == NULL) {
    newNode = new DependenceNode(node);
    _depnode_map[node] = newNode;
    addNode(newNode);
  }

  return newNode;
}

DependenceNode * DependenceGraph::createNode(SgNode * node) {
  DependenceNode * newNode = getNode(node);

  if (newNode == NULL) {
    newNode = createNode(new DependenceNode(DependenceNode::SGNODE, node));
    _sgnode_map[node] = newNode;
  }

  return newNode;
}

DependenceNode * DependenceGraph::getNode(DependenceNode * node) {
  if (_depnode_map.count(node)) {
    return _depnode_map[node];
  } else {
    return NULL;
  }
}

DependenceNode * DependenceGraph::getNode(SgNode * node) {
  if (_sgnode_map.count(node)) {
    return _sgnode_map[node];
  } else {
    return NULL;
  }
}

void DependenceGraph::establishEdge(DependenceNode * from, DependenceNode * to, EdgeType e) {
  _edgetype_map[e].insert(Edge(from, to));
  _edge_map[Edge(from, to)].insert(e);
  addLink(from, to);
}

bool DependenceGraph::edgeExists(DependenceNode * from, DependenceNode * to, EdgeType e) {
  return _edgetype_map[e].count(Edge(from, to));
}

std::set<DependenceGraph::EdgeType> DependenceGraph::edgeType(DependenceNode * from, DependenceNode * to) {
  return _edge_map[Edge(from, to)];
}

void DependenceGraph::writeDot(char * filename) {
  
  std::ofstream f(filename);
  
  f << "digraph \"G" << filename << "\" {" << std::endl;
  //output all of the nodes
  std::set<SimpleDirectedGraphNode *>::iterator i;
  for (i = _nodes.begin(); i != _nodes.end(); i++) {
    SimpleDirectedGraphNode * d = *i;
    char buf[sizeof(SimpleDirectedGraphNode *)*2 + 3];
    sprintf(buf, "%p", d);
    f << "\"" << buf << "\" [label = \"";
    _displayData(d, f);
    f << "\"];" << std::endl;
  }
  
  //output all of the edges (we'll just use successor edges
  for (i = _nodes.begin(); i != _nodes.end(); i++) {
    DependenceNode * d1 = dynamic_cast<DependenceNode *>(*i);
    std::set<SimpleDirectedGraphNode *> succs = d1->getSuccessors();
    std::set<SimpleDirectedGraphNode *>::iterator j;
    for (j = succs.begin(); j != succs.end(); j++) {
      DependenceNode * d2 = dynamic_cast<DependenceNode *>(*j);
      
      char buf1[sizeof(DependenceNode *)*2 + 3];
      char buf2[sizeof(DependenceNode *)*2 + 3];
      
      sprintf(buf1, "%p", d1);
      sprintf(buf2, "%p", d2);
      
      f << "\"" << buf1 << "\" -> \"" << buf2 << "\"";
      std::set<EdgeType> ets = edgeType(d1, d2);
      EdgeType et = *(ets.begin());
      f << "[label=\"" << edgeNames[et] << "\"];" << std::endl;
    }
  }
  
  f << "}" << std::endl;
}
