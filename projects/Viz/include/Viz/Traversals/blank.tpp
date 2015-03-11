
namespace Rose {

namespace Viz {

namespace Traversals {

template <class InheritedAttr_, class SynthesizedAttr_>
InheritedAttr_ Blank<InheritedAttr_, SynthesizedAttr_>::evaluateInheritedAttribute(SgNode * node, InheritedAttr_ attr) {
  InheritedAttr res;
  return res;
}

template <class InheritedAttr_, class SynthesizedAttr_>
SynthesizedAttr_ Blank<InheritedAttr_, SynthesizedAttr_>::evaluateSynthesizedAttribute(SgNode * node, InheritedAttr_ attr, StackFrameVector<SynthesizedAttr_> attrs) {
  SynthesizedAttr res;
  return res;
}

template <class InheritedAttr_, class SynthesizedAttr_>
bool Blank<InheritedAttr_, SynthesizedAttr_>::stopAfter(SgNode *) {
  return false;
}

template <class InheritedAttr_, class SynthesizedAttr_>
bool Blank<InheritedAttr_, SynthesizedAttr_>::skip(SgNode *) {
  return false;
}

template <class InheritedAttr_, class SynthesizedAttr_>
Objects::graph_t * Blank<InheritedAttr_, SynthesizedAttr_>::startOn(SgNode *) {
  return NULL;
}

template <class InheritedAttr_, class SynthesizedAttr_>
Objects::graph_t * Blank<InheritedAttr_, SynthesizedAttr_>::getSubgraph(SgNode * node, Objects::graph_t * graph) {
  return graph;
}

template <class InheritedAttr_, class SynthesizedAttr_>
void Blank<InheritedAttr_, SynthesizedAttr_>::edit(SgNode * node, Objects::node_desc_t & desc, const InheritedAttr & inhr_attr, const SynthesizedAttr & synth_attr) const {
  desc.label="";
  desc.color="black";
  desc.style="";
  desc.shape="box";
  desc.fillcolor="";
  desc.penwidth=1;
}

template <class InheritedAttr_, class SynthesizedAttr_>
void Blank<InheritedAttr_, SynthesizedAttr_>::edit(SgNode * node, SgNode * parent, Objects::edge_desc_t & desc) const {
  desc.label="";
  desc.color="";
  desc.style="";
  desc.minlen=1;
  desc.penwidth=1;
  desc.constraint=true;
}

template <class InheritedAttr_, class SynthesizedAttr_>
void Blank<InheritedAttr_, SynthesizedAttr_>::edit(Objects::graph_t * graph, Objects::cluster_desc_t & desc) const {
  desc.label="";
  desc.color="black";
  desc.style="";
  desc.penwidth=1;
}

template <class InheritedAttr_, class SynthesizedAttr_>
void Blank<InheritedAttr_, SynthesizedAttr_>::edit(Objects::graph_desc_t & desc) const {
  desc.label="";
  desc.color="black";
  desc.style="";
  desc.rankdir="TB";
  desc.ranksep=1;
}

}

}

}

