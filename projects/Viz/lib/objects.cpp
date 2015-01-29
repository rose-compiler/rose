
#include "Viz/objects.hpp"

#include "cassert"

namespace Rose {

namespace Viz {

namespace Objects {

graph_t * graph_t::subgraph(const std::string & tag) {
  graph_t * res = children[tag];
  if (res == NULL) {
    res = new graph_t();
      res->tag = tag;
      res->parent = this;
    children[tag] = res;
  }
  return res;
}

graph_t::~graph_t() {
  std::map<std::string, graph_t *>::iterator it_child;
  for (it_child = children.begin(); it_child != children.end(); it_child++) {
    assert(it_child->second != NULL);
    delete it_child->second;
  }
}

}

}

}

