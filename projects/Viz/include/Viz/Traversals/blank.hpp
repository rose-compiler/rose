
#ifndef __ROSE_VIZ_TRAVERSALS_BLANK_HPP__
#define __ROSE_VIZ_TRAVERSALS_BLANK_HPP__

#include "Viz/Traversals/traversal.hpp"

namespace Rose {

namespace Viz {

namespace Traversals {

template <class InheritedAttr_, class SynthesizedAttr_>
struct Blank : Traversal<InheritedAttr_, SynthesizedAttr_> {
  typedef InheritedAttr_ InheritedAttr;
  typedef SynthesizedAttr_ SynthesizedAttr;

  virtual InheritedAttr evaluateInheritedAttribute(SgNode * node, InheritedAttr attr);
  virtual SynthesizedAttr evaluateSynthesizedAttribute(SgNode * node, InheritedAttr attr, StackFrameVector<SynthesizedAttr> attrs);

  virtual bool stopAfter(SgNode *);
  virtual bool skip(SgNode *);

  virtual Objects::graph_t * startOn(SgNode *);
  virtual Objects::graph_t * getSubgraph(SgNode * node, Objects::graph_t * graph);

  virtual void edit(SgNode * node, Objects::node_desc_t & desc, const InheritedAttr & inhr_attr, const SynthesizedAttr & synth_attr) const;
  virtual void edit(SgNode * node, SgNode * parent, Objects::edge_desc_t & desc) const;
  virtual void edit(Objects::graph_t * graph, Objects::cluster_desc_t & desc) const;
  virtual void edit(Objects::graph_desc_t & desc) const;
};

}

}

}

#include "Viz/Traversals/blank.tpp"

#endif /* __ROSE_VIZ_TRAVERSALS_BLANK_HPP__ */

