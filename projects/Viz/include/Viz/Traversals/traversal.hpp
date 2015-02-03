
#ifndef __ROSE_VIZ_TRAVERSALS_TRAVERSAL_HPP__
#define __ROSE_VIZ_TRAVERSALS_TRAVERSAL_HPP__

#include "Viz/objects.hpp"

#include "sage3basic.h"

class SgNode;

namespace Rose {

namespace Viz {

namespace Traversals {

template <class InheritedAttr_, class SynthesizedAttr_>
struct Traversal {
  typedef InheritedAttr_ InheritedAttr;
  typedef SynthesizedAttr_ SynthesizedAttr;

  // For user defined traversal
  virtual InheritedAttr evaluateInheritedAttribute(SgNode * node, InheritedAttr attr) = 0;
  virtual SynthesizedAttr evaluateSynthesizedAttribute(SgNode * node, InheritedAttr attr, StackFrameVector<SynthesizedAttr> attrs) = 0;

  // Control the construction of the visualization graph
  virtual bool stopAfter(SgNode *) = 0;
  virtual bool skip(SgNode *) = 0;
  virtual Objects::graph_t * startOn(SgNode *) = 0;
  virtual Objects::graph_t * getSubgraph(SgNode * node, Objects::graph_t * graph) = 0;

  // Edit descriptors for the visualization graph elements
  virtual void edit(SgNode * node, Objects::node_desc_t & desc, const InheritedAttr & inhr_attr, const SynthesizedAttr & synth_attr) const = 0;
  virtual void edit(SgNode * node, SgNode * parent, Objects::edge_desc_t & desc) const = 0;
  virtual void edit(Objects::graph_t * graph, Objects::cluster_desc_t & desc) const = 0;
  virtual void edit(Objects::graph_desc_t & desc) const = 0;
};

}

}

}

#endif /*  __ROSE_VIZ_TRAVERSALS_TRAVERSAL_HPP__ */

