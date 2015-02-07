
#ifndef __ROSE_VIZ_OBJECTS_HPP__
#define __ROSE_VIZ_OBJECTS_HPP__

#include <string>
#include <vector>
#include <map>

class SgNode;

namespace Rose {

namespace Viz {

namespace Objects {

struct desc_t {
  std::string label;
  std::string color;
  std::string style;
};

struct node_desc_t : public desc_t {
  std::string shape;
  std::string fillcolor;
  float penwidth;
  std::string url;
};

struct edge_desc_t : public desc_t {
  int minlen;
  float penwidth;
  bool constraint;
};

struct cluster_desc_t : public desc_t {
  float penwidth;
};

struct graph_desc_t : public desc_t {
  std::string rankdir;
  float ranksep;
};

struct graph_t {
  std::string tag;

  graph_t * parent;
  std::map<std::string, graph_t *> children;

  std::vector<SgNode *> ast_nodes;

  graph_t * subgraph(const std::string & tag);

  ~graph_t();
};

}

}

}

#endif /*  */
