#ifndef CAST_GRAPH_VIS_H
#define CAST_GRAPH_VIS_H

#include "Sawyer/Graph.h"

class EdgeData {
};

class VertexData {
public:
  VertexData(SgNode* node):node(node) {
  }
  VertexData(SgNode* node, SgType* type):node(node),type(type) {
  }
private:
  SgNode* node=nullptr;
  SgType* type=nullptr;
};

typedef Sawyer::Container::Graph<VertexData, EdgeData> CastGraph;
typedef CastGraph::VertexIterator VertexIterType;
typedef std::map<SgNode*, VertexIterType> NodeVertexMapping;

bool generateTypeGraph(SgProject* root, std::string dotFileName);

#endif
