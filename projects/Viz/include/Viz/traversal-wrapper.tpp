
namespace Rose {

namespace Viz {

template <class InheritedAttr_>
VizAttrWrapper<InheritedAttr_>::VizAttrWrapper() :
  viz_attr(),
  user_attr()
{}

template <class InheritedAttr_>
VizAttrWrapper<InheritedAttr_>::VizAttrWrapper(const InheritedAttr_ & user_attr_) :
  viz_attr(),
  user_attr(user_attr_)
{}

template <class InheritedAttr_, class SynthesizedAttr_>
TraversalWrapper<InheritedAttr_, SynthesizedAttr_>::TraversalWrapper(Traversals::Traversal<InheritedAttr_, SynthesizedAttr_> & traversal_) :
  AstTopDownBottomUpProcessing<InheritedAttr, SynthesizedAttr>(),
  traversal(traversal_),
  inherited_attr_map(),
  synthesized_attr_map(),
  universe_graph(new Objects::graph_t())
{
  universe_graph->tag = "universe";
}

template <class InheritedAttr_, class SynthesizedAttr_>
TraversalWrapper<InheritedAttr_, SynthesizedAttr_>::~TraversalWrapper() {
  assert(universe_graph != NULL);
  delete universe_graph;
}

template <class InheritedAttr_, class SynthesizedAttr_>
typename Objects::graph_t * TraversalWrapper<InheritedAttr_, SynthesizedAttr_>::getGraph(const std::string & tag, typename Objects::graph_t * parent) {
  if (parent != NULL)
    return parent->subgraph(tag);
  else if (universe_graph != NULL)
    return universe_graph->subgraph(tag);
  else assert(false);
}

template <class InheritedAttr_, class SynthesizedAttr_>
typename Objects::graph_t * TraversalWrapper<InheritedAttr_, SynthesizedAttr_>::mergeGraph(typename Objects::graph_t * g1, typename Objects::graph_t * g2, bool fuse) {
  typename Objects::graph_t * res = new Objects::graph_t();
  if (fuse && g1->tag == g2->tag) {
    res->tag = g1->tag;

    res->ast_nodes.insert(res->ast_nodes.begin(), g1->ast_nodes.begin(), g1->ast_nodes.end());
    res->ast_nodes.insert(res->ast_nodes.begin(), g2->ast_nodes.begin(), g2->ast_nodes.end());

    typename std::map<std::string, typename Objects::graph_t *>::iterator it_1 = g1->children.begin();
    typename std::map<std::string, typename Objects::graph_t *>::iterator it_2 = g2->children.begin();
    while (it_1 != g1->children.end() && it_2 != g2->children.end()) {
      if (it_1->second->tag == it_2->second->tag) {
        Objects::graph_t * merged = mergeGraph(it_1->second, it_2->second, true);
        res->children[merged->tag] = merged;
        it_1++; it_2++;
      }
      else if (it_1->second->tag < it_2->second->tag) {
        res->children[it_1->second->tag] = it_1->second;
        it_1++;
      }
      else if (it_1->second->tag > it_2->second->tag) {
        res->children[it_2->second->tag] = it_2->second;
        it_2++;
      }
      else assert(false);
    }
    while (it_1 != g1->children.end()) {
      res->children[it_1->second->tag] = it_1->second;
      it_1++;
    }
    while (it_2 != g2->children.end()) {
      res->children[it_2->second->tag] = it_2->second;
      it_2++;
    }
  }
  else {
    res->tag = g1->tag + "_" + g2->tag;

    res->children[g1->tag] = g1;
    g1->parent->children.erase(g1->tag);
    g1->parent = res;

    res->children[g2->tag] = g2;
    g2->parent->children.erase(g2->tag);
    g2->parent = res;
  }
  return res;
}

template <class InheritedAttr_, class SynthesizedAttr_>
typename Objects::graph_t * TraversalWrapper<InheritedAttr_, SynthesizedAttr_>::ancestorGraph(Objects::graph_t * g1, Objects::graph_t * g2) {
  assert(g1 != NULL);
  assert(g2 != NULL);

  std::vector<typename Objects::graph_t *> l1;
  while (g1 != NULL) {
    l1.push_back(g1);
    g1 = g1->parent;
  }

  std::vector<typename Objects::graph_t *> l2;
  while (g2 != NULL) {
    l2.push_back(g2);
    g2 = g2->parent;
  }

  assert(l1.size() > 0 && l2.size() > 0);

  if (l1[0] != l2[0]) return NULL;

  typename std::vector<typename Objects::graph_t *>::iterator it1 = l1.begin();
  typename std::vector<typename Objects::graph_t *>::iterator it2 = l2.begin();
  while (it1 != l1.end() && it2 != l2.end() && *it1 == *it2) { it1++; it2++; };
  assert(it1 != l1.end() && it2 != l2.end());
  it1--;
  it2--;
  assert(*it1 == *it2);
  return *it1;
}

template <class InheritedAttr_, class SynthesizedAttr_>
typename TraversalWrapper<InheritedAttr_, SynthesizedAttr_>::InheritedAttr TraversalWrapper<InheritedAttr_, SynthesizedAttr_>::makeAttribute(const std::string & tag) {
  typename TraversalWrapper<InheritedAttr_, SynthesizedAttr_>::InheritedAttr res;

  res.viz_attr.node   = NULL;
  res.viz_attr.parent = NULL;
  res.viz_attr.graph  = getGraph(tag, NULL);
  res.viz_attr.stopped_by    = NULL;
  res.viz_attr.skipped_since = NULL;

  return res;
}

template <class InheritedAttr_, class SynthesizedAttr_>
typename TraversalWrapper<InheritedAttr_, SynthesizedAttr_>::InheritedAttr TraversalWrapper<InheritedAttr_, SynthesizedAttr_>::makeAttribute(const std::string & tag, const InheritedAttr_ & user_attr) {
  typename TraversalWrapper<InheritedAttr_, SynthesizedAttr_>::InheritedAttr res;

  res.viz_attr.node   = NULL;
  res.viz_attr.parent = NULL;
  res.viz_attr.graph  = getGraph(tag, NULL);
  res.viz_attr.stopped_by    = NULL;
  res.viz_attr.skipped_since = NULL;

  res.user_attr = user_attr;

  return res;
}

template <class InheritedAttr_, class SynthesizedAttr_>
void TraversalWrapper<InheritedAttr_, SynthesizedAttr_>::collectGraphElements(
  Objects::graph_t * graph,
  std::vector<Objects::graph_t *> & clusters,
  std::map<Objects::graph_t *, std::set<std::pair<SgNode *, SgNode *> > > & edge_map,
  std::map<SgNode *, Objects::graph_t *> node_graph_map
) {
  edge_map[graph];
  std::vector<SgNode *>::iterator it_node;
  for (it_node = graph->ast_nodes.begin(); it_node != graph->ast_nodes.end(); it_node++) {
    node_graph_map[*it_node] = graph;
    SgNode * parent = getVizAttribute(*it_node).parent;
    Objects::graph_t * parent_graph = node_graph_map[parent];
    edge_map[parent_graph].insert(std::pair<SgNode *, SgNode *>(parent, *it_node));
  }

  typename std::map<std::string, Objects::graph_t *>::iterator it_cluster;
  for (it_cluster = graph->children.begin(); it_cluster != graph->children.end(); it_cluster++) {
    clusters.push_back(it_cluster->second);
    collectGraphElements(it_cluster->second, clusters, edge_map, node_graph_map);
  }
}

template <class InheritedAttr_, class SynthesizedAttr_>
void TraversalWrapper<InheritedAttr_, SynthesizedAttr_>::editGraphElement(
  Objects::graph_t * graph,
  const std::vector<Objects::graph_t *> & clusters,
  const std::map<Objects::graph_t *, std::set<std::pair<SgNode *, SgNode *> > > & edge_map,
  std::map<Objects::graph_t *, Objects::cluster_desc_t> & cluster_desc,
  std::map<SgNode *, Objects::node_desc_t> & node_desc,
  std::map<std::pair<SgNode *, SgNode *>, Objects::edge_desc_t> & edge_desc
) {
  std::vector<SgNode *>::iterator it_node;
  for (it_node = graph->ast_nodes.begin(); it_node != graph->ast_nodes.end(); it_node++) {
    traversal.edit(*it_node, node_desc[*it_node], getInheritedAttribute(*it_node).user_attr, getSynthesizedAttribute(*it_node));
    SgNode * parent = getVizAttribute(*it_node).parent;
    traversal.edit(parent, *it_node, edge_desc[std::pair<SgNode *, SgNode *>(parent, *it_node)]);
  }

  typename std::map<std::string, Objects::graph_t *>::iterator it_cluster;
  for (it_cluster = graph->children.begin(); it_cluster != graph->children.end(); it_cluster++) {
    traversal.edit(it_cluster->second, cluster_desc[it_cluster->second]);
    editGraphElement(it_cluster->second, clusters, edge_map, cluster_desc, node_desc, edge_desc);
  }
}

template <class InheritedAttr_, class SynthesizedAttr_>
std::string TraversalWrapper<InheritedAttr_, SynthesizedAttr_>::getNodeName(SgNode * node) {
  std::ostringstream oss;
  oss << "node_" << node->class_name() << "_" << std::hex << node;
  return oss.str();
}

template <class InheritedAttr_, class SynthesizedAttr_>
void TraversalWrapper<InheritedAttr_, SynthesizedAttr_>::toGraphViz(const Objects::graph_desc_t & desc, std::ostream & out, std::string indent) {
  out << indent << "label=\"" << desc.label << "\";" << std::endl;
  out << indent << "color=\"" << desc.color << "\";" << std::endl;
  out << indent << "style=\"" << desc.style << "\";" << std::endl;
  out << indent << "rankdir=\"" << desc.rankdir << "\";" << std::endl;
  out << indent << "ranksep=" << desc.ranksep << ";" << std::endl;
}

template <class InheritedAttr_, class SynthesizedAttr_>
void TraversalWrapper<InheritedAttr_, SynthesizedAttr_>::toGraphViz(const Objects::cluster_desc_t & desc, std::ostream & out, std::string indent) {
  out << indent << "label=\"" << desc.label << "\";" << std::endl;
  out << indent << "color=\"" << desc.color << "\";" << std::endl;
  out << indent << "style=\"" << desc.style << "\";" << std::endl;
  out << indent << "penwidth=" << desc.penwidth << ";" << std::endl;
}

template <class InheritedAttr_, class SynthesizedAttr_>
void TraversalWrapper<InheritedAttr_, SynthesizedAttr_>::toGraphViz(SgNode * node, const Objects::node_desc_t & desc, std::ostream & out, std::string indent) {

  out << getNodeName(node);

  out << " [label=\"" << desc.label << "\",";
  out << "  color=\"" << desc.color << "\",";
  out << "  style=\"" << desc.style << "\",";
  out << "  shape=\"" << desc.shape << "\",";
  out << "  fillcolor=\"" << desc.fillcolor << "\",";
  out << "  penwidth=" << desc.penwidth << "," << std::endl;
  out << "  URL=\"" << desc.url << "\"];" << std::endl;
}

template <class InheritedAttr_, class SynthesizedAttr_>
void TraversalWrapper<InheritedAttr_, SynthesizedAttr_>::toGraphViz(std::pair<SgNode *, SgNode *> edge, const Objects::edge_desc_t & desc, std::ostream & out, std::string indent) {

  out << getNodeName(edge.first) << " -> " << getNodeName(edge.second);

  out << " [label=\"" << desc.label << "\",";
  out << "  color=\"" << desc.color << "\",";
  out << "  style=\"" << desc.style << "\",";
  out << "  minlen=" << desc.minlen << ",";
  out << "  penwidth=" << desc.penwidth << ",";
  out << "  constraint=" << desc.constraint << "];" << std::endl;
}

template <class InheritedAttr_, class SynthesizedAttr_>
void TraversalWrapper<InheritedAttr_, SynthesizedAttr_>::toGraphViz(
  Objects::graph_t * graph,
  const std::vector<Objects::graph_t *> & clusters,
  const std::map<Objects::graph_t *, std::set<std::pair<SgNode *, SgNode *> > > & edge_map,
  const std::map<Objects::graph_t *, Objects::cluster_desc_t> & cluster_desc,
  const std::map<SgNode *, Objects::node_desc_t> & node_desc,
  const std::map<std::pair<SgNode *, SgNode *>, Objects::edge_desc_t> & edge_desc,
  std::ostream & out, std::string indent
) {
  out << indent << "subgraph cluster_" << graph->tag << " {" << std::endl;

  toGraphViz(cluster_desc.at(graph), out);

  typename std::map<std::string, typename Objects::graph_t *>::iterator it_child;
  for (it_child = graph->children.begin(); it_child != graph->children.end(); it_child++)
    toGraphViz(it_child->second, clusters, edge_map, cluster_desc, node_desc, edge_desc, out, indent + "  ");

  std::vector<SgNode *>::iterator it_node;
  for (it_node = graph->ast_nodes.begin(); it_node != graph->ast_nodes.end(); it_node++) {
    toGraphViz(*it_node, node_desc.at(*it_node), out, indent + "  ");
  }

  std::set<std::pair<SgNode *, SgNode *> >::iterator it_edge;
  for (it_edge = edge_map.at(graph).begin(); it_edge != edge_map.at(graph).end(); it_edge++) {
    toGraphViz(*it_edge, edge_desc.at(*it_edge), out, indent + "  ");
  }

  out << "}" << std::endl;
}

template <class InheritedAttr_, class SynthesizedAttr_>
void TraversalWrapper<InheritedAttr_, SynthesizedAttr_>::toGraphViz(const std::string & tag, std::ostream & out) {
  typename Objects::graph_t * graph = getGraph(tag, NULL);

  std::vector<Objects::graph_t *> clusters;
  std::map<Objects::graph_t *, std::set<std::pair<SgNode *, SgNode *> > > edge_map;

  collectGraphElements(graph, clusters, edge_map, std::map<SgNode *, Objects::graph_t *>());

  Objects::graph_desc_t graph_desc;
  std::map<Objects::graph_t *, Objects::cluster_desc_t> cluster_desc;
  std::map<SgNode *, Objects::node_desc_t> node_desc;
  std::map<std::pair<SgNode *, SgNode *>, Objects::edge_desc_t> edge_desc;

  traversal.edit(graph_desc);
  editGraphElement(graph, clusters, edge_map, cluster_desc, node_desc, edge_desc);

  out << "digraph graph_" << graph->tag << " {" << std::endl;

  toGraphViz(graph_desc, out);

  typename std::map<std::string, typename Objects::graph_t *>::iterator it_child;
  for (it_child = graph->children.begin(); it_child != graph->children.end(); it_child++)
    toGraphViz(it_child->second, clusters, edge_map, cluster_desc, node_desc, edge_desc, out);

  std::vector<SgNode *>::iterator it_node;
  for (it_node = graph->ast_nodes.begin(); it_node != graph->ast_nodes.end(); it_node++) {
    toGraphViz(*it_node, node_desc.at(*it_node), out);
  }

  std::set<std::pair<SgNode *, SgNode *> >::iterator it_edge;
  for (it_edge = edge_map.at(graph).begin(); it_edge != edge_map.at(graph).end(); it_edge++) {
    toGraphViz(*it_edge, edge_desc.at(*it_edge), out);
  }

  out << "}" << std::endl;
}

template <class InheritedAttr_, class SynthesizedAttr_>
const typename TraversalWrapper<InheritedAttr_, SynthesizedAttr_>::InheritedAttr & TraversalWrapper<InheritedAttr_, SynthesizedAttr_>::getInheritedAttribute(SgNode * node) const {
  return inherited_attr_map.at(node);
}

template <class InheritedAttr_, class SynthesizedAttr_>
const typename TraversalWrapper<InheritedAttr_, SynthesizedAttr_>::VizAttr & TraversalWrapper<InheritedAttr_, SynthesizedAttr_>::getVizAttribute(SgNode * node) const {
  return inherited_attr_map.at(node).viz_attr;
}

template <class InheritedAttr_, class SynthesizedAttr_>
const typename TraversalWrapper<InheritedAttr_, SynthesizedAttr_>::SynthesizedAttr & TraversalWrapper<InheritedAttr_, SynthesizedAttr_>::getSynthesizedAttribute(SgNode * node) const {
  return synthesized_attr_map.at(node);
}

template <class InheritedAttr_, class SynthesizedAttr_>
typename TraversalWrapper<InheritedAttr_, SynthesizedAttr_>::VizAttr TraversalWrapper<InheritedAttr_, SynthesizedAttr_>::evaluateVizAttribute(
    SgNode * node, typename TraversalWrapper<InheritedAttr_, SynthesizedAttr_>::VizAttr attr
) {
  typename TraversalWrapper<InheritedAttr_, SynthesizedAttr_>::VizAttr res;
    res.node          = node;
    res.parent        = NULL;
    res.graph         = NULL;
    res.stopped_by    = NULL;
    res.skipped_since = NULL;

  assert(!(attr.stopped_by != NULL && attr.skipped_since != NULL));

  // Traversal is stopped
  if (attr.stopped_by != NULL) {
    // Does the user want to start a new graph for this subtree?
    res.graph = traversal.startOn(node);

    if (res.graph == NULL) {
      // Stay stopped
      res.stopped_by = attr.stopped_by;
      return res;
    }
  }

  // parent can be: NULL if new graph, the closest parent added to the graph, or simply the previous node
  if (attr.stopped_by != NULL)
    res.parent = NULL;
  else if (attr.skipped_since != NULL)
    res.parent = attr.skipped_since;
  else
    res.parent = attr.node;

  // This should happen when starting the traversal
  if (res.parent == NULL && res.graph == NULL) {
    assert(attr.graph != NULL);
    res.graph = attr.graph;
  }

  // Only one of the parent and graph can be filled
  assert((res.parent != NULL) xor (res.graph != NULL));

  // Does the user want to skip this node? (cannot skip a node if new graph)
  if (res.parent != NULL && traversal.skip(node)) {
    res.skipped_since = res.parent; // last node added to the graph
    res.parent = NULL;
    res.graph  = NULL;
    return res;
  }

  // Does the user want to stop after this node?
  if (traversal.stopAfter(node)) {
    res.stopped_by = node;
  }

  // Get graph from parent
  if (res.graph == NULL)
    res.graph = getVizAttribute(res.parent).graph;
  assert(res.graph != NULL);

  // Let the user create a cluster for the subtree if he wants. 
  res.graph = traversal.getSubgraph(node, res.graph);
  assert(res.graph != NULL);

  res.graph->ast_nodes.push_back(node);

  return res;
}

template <class InheritedAttr_, class SynthesizedAttr_>
typename TraversalWrapper<InheritedAttr_, SynthesizedAttr_>::InheritedAttr TraversalWrapper<InheritedAttr_, SynthesizedAttr_>::evaluateInheritedAttribute(SgNode * node, typename TraversalWrapper<InheritedAttr_, SynthesizedAttr_>::InheritedAttr attr) {
  InheritedAttr_ user_attr = traversal.evaluateInheritedAttribute(node, attr.user_attr);

  typename TraversalWrapper<InheritedAttr_, SynthesizedAttr_>::InheritedAttr res(user_attr);

  res.viz_attr = evaluateVizAttribute(node, attr.viz_attr);

  inherited_attr_map[node] = res;
  return res;
}

template <class InheritedAttr_, class SynthesizedAttr_>
typename TraversalWrapper<InheritedAttr_, SynthesizedAttr_>::SynthesizedAttr TraversalWrapper<InheritedAttr_, SynthesizedAttr_>::evaluateSynthesizedAttribute(SgNode * node, InheritedAttr attr, StackFrameVector<SynthesizedAttr> attrs) {
  SynthesizedAttr_ res = traversal.evaluateSynthesizedAttribute(node, attr.user_attr, attrs);
  synthesized_attr_map[node] = res;
  return res;
}

}

}
