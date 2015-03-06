
#include "jsonxx.h"

extern void build_sage_name_variant_map(std::map<std::string, VariantT> & name_variant_map);
extern void build_sage_variant_name_map(std::map<VariantT, std::string> & variant_name_map);
extern void build_sage_parent_variant_map(std::map<VariantT, VariantT> & parent_variant_map);

namespace Rose {

namespace Viz {

namespace Traversals {

template <class InheritedAttr, class SynthesizedAttr>
Programmable<InheritedAttr, SynthesizedAttr>::Program::Program() :
  name_variant_map(), variant_name_map(), parent_variant_map(),
  graph_title(""), graph_color("black"), graph_style(e_none), graph_rankdir(e_top_bottom), graph_ranksep(1),
  cluster_label_map(), cluster_color_map(), cluster_style_map(), cluster_pen_width_map(),
  cluster_default_label(""), cluster_default_color("black"), cluster_default_style(e_none), cluster_default_pen_width(1),
  edge_default_label(""), edge_default_color("black"), edge_default_style(e_none), edge_default_min_len(1), edge_default_pen_width(1), edge_default_constraint(true),
  node_label_map(), node_color_map(), node_style_map(), node_shape_map(), node_fill_color_map(), node_pen_width_map()
{
  node_label_map.insert(std::pair<VariantT, std::string>((VariantT)SgNode::static_variant, ""));
  node_color_map.insert(std::pair<VariantT, std::string>((VariantT)SgNode::static_variant, "black"));
  node_style_map.insert(std::pair<VariantT, style_e>((VariantT)SgNode::static_variant, e_filled));
  node_shape_map.insert(std::pair<VariantT, std::string>((VariantT)SgNode::static_variant, "box"));
  node_fill_color_map.insert(std::pair<VariantT, std::string>((VariantT)SgNode::static_variant, "white"));
  node_pen_width_map.insert(std::pair<VariantT, float>((VariantT)SgNode::static_variant, 1));

  build_sage_name_variant_map(name_variant_map);
  build_sage_variant_name_map(variant_name_map);
  build_sage_parent_variant_map(parent_variant_map);
}

template <class InheritedAttr, class SynthesizedAttr>
void Programmable<InheritedAttr, SynthesizedAttr>::Program::open(const std::string & file) {
  std::ifstream viz_file;
  viz_file.open(file.c_str());
  assert(viz_file.is_open());

  jsonxx::Object viz;
  viz.parse(viz_file);

  jsonxx::Object & graph = viz.get<jsonxx::Object>("graph");
  graph_color   = graph.get<jsonxx::String>("color");
  graph_style   = styleFromString(graph.get<jsonxx::String>("style"));
  graph_rankdir = rankDirFromString(graph.get<jsonxx::String>("rankdir"));
  graph_ranksep = graph.get<jsonxx::Number>("ranksep");

  jsonxx::Object & edge = viz.get<jsonxx::Object>("edge");
  jsonxx::Object & edge_default = edge.get<jsonxx::Object>("default");
  edge_default_label      = edge_default.get<jsonxx::String >("label");
  edge_default_color      = edge_default.get<jsonxx::String >("color");
  edge_default_style      = styleFromString(edge_default.get<jsonxx::String >("style"));
  edge_default_min_len    = edge_default.get<jsonxx::Number >("min_len");
  edge_default_pen_width  = edge_default.get<jsonxx::Number >("pen_width");
  edge_default_constraint = edge_default.get<jsonxx::Boolean>("constraint");

  jsonxx::Object & cluster = viz.get<jsonxx::Object>("cluster");
  jsonxx::Object & cluster_default = edge.get<jsonxx::Object>("default");
  cluster_default_label     = cluster_default.get<jsonxx::String >("label");
  cluster_default_color     = cluster_default.get<jsonxx::String >("color");
  cluster_default_style     = styleFromString(cluster_default.get<jsonxx::String >("style"));
  cluster_default_pen_width = cluster_default.get<jsonxx::Number >("pen_width");

  const std::map<std::string, jsonxx::Value *> & cluster_map = cluster.get<jsonxx::Object>("map").kv_map();
  std::map<std::string, jsonxx::Value *>::const_iterator it_cluster;
  for (it_cluster = cluster_map.begin(); it_cluster != cluster_map.end(); it_cluster++) {
    jsonxx::Object & cluster_map_element = it_cluster->second->get<jsonxx::Object>();
    cluster_label_map[it_cluster->first]     = cluster_map_element.get<jsonxx::String >("label");
    cluster_color_map[it_cluster->first]     = cluster_map_element.get<jsonxx::String >("color");
    cluster_style_map[it_cluster->first]     = styleFromString(cluster_map_element.get<jsonxx::String >("style"));
    cluster_pen_width_map[it_cluster->first] = cluster_map_element.get<jsonxx::Number >("pen_width");
  }

  jsonxx::Object & node = viz.get<jsonxx::Object>("node");
  const std::map<std::string, jsonxx::Value *> & node_map = node.get<jsonxx::Object>("map").kv_map();
  std::map<std::string, jsonxx::Value *>::const_iterator it_node;
  for (it_node = node_map.begin(); it_node != node_map.end(); it_node++) {
    VariantT variant = name_variant_map[it_node->first];
    jsonxx::Object & node_map_element = it_node->second->get<jsonxx::Object>();
    node_label_map[variant]      = node_map_element.get<jsonxx::String >("label");
    node_color_map[variant]      = node_map_element.get<jsonxx::String >("color");
    node_style_map[variant]      = styleFromString(node_map_element.get<jsonxx::String >("style"));
    node_shape_map[variant]      = node_map_element.get<jsonxx::String >("shape");
    node_fill_color_map[variant] = node_map_element.get<jsonxx::String >("fill_color");
    node_pen_width_map[variant]  = node_map_element.get<jsonxx::Number >("pen_width");
  }
}

template <class InheritedAttr, class SynthesizedAttr>
void Programmable<InheritedAttr, SynthesizedAttr>::Program::setTitle(const std::string & title) {
  graph_title = title;
}

template <class InheritedAttr, class SynthesizedAttr>
std::string Programmable<InheritedAttr, SynthesizedAttr>::Program::toString(style_e style) {
  switch (style) {
    case e_none: return "";
    case e_filled: return "filled";
    case e_invisible: return "invis";
    default: assert(false);
  }
}

template <class InheritedAttr, class SynthesizedAttr>
std::string Programmable<InheritedAttr, SynthesizedAttr>::Program::toString(rankdir_e rankdir) {
  switch (rankdir) {
    case e_top_bottom: return "TB";
    case e_bottom_top: return "BT";
    case e_left_right: return "LR";
    case e_right_left: return "RL";
    default: assert(false);
  }
}

template <class InheritedAttr, class SynthesizedAttr>
typename Programmable<InheritedAttr, SynthesizedAttr>::Program::style_e Programmable<InheritedAttr, SynthesizedAttr>::Program::styleFromString(const std::string & str) {
  if      (str == "") return e_none;
  else if (str == "filled") return e_filled;
  else if (str == "invis" || str == "invisible") return e_invisible;
  else assert(false);
}

template <class InheritedAttr, class SynthesizedAttr>
typename Programmable<InheritedAttr, SynthesizedAttr>::Program::rankdir_e Programmable<InheritedAttr, SynthesizedAttr>::Program::rankDirFromString(const std::string & str) {
  if      (str == "TB") return e_top_bottom;
  else if (str == "BT") return e_bottom_top;
  else if (str == "LR") return e_left_right;
  else if (str == "RL") return e_right_left;
  else assert(false);
}

template <class InheritedAttr, class SynthesizedAttr>
const std::string Programmable<InheritedAttr, SynthesizedAttr>::Program::getGraphLabel() const {
  return graph_title;
}

template <class InheritedAttr, class SynthesizedAttr>
const std::string Programmable<InheritedAttr, SynthesizedAttr>::Program::getGraphColor() const {
  return graph_color;
}

template <class InheritedAttr, class SynthesizedAttr>
const std::string Programmable<InheritedAttr, SynthesizedAttr>::Program::getGraphStyle() const {
  return toString(graph_style);
}

template <class InheritedAttr, class SynthesizedAttr>
const std::string Programmable<InheritedAttr, SynthesizedAttr>::Program::getGraphRankDir() const {
  return toString(graph_rankdir);
}

template <class InheritedAttr, class SynthesizedAttr>
const float Programmable<InheritedAttr, SynthesizedAttr>::Program::getGraphRankSep() const {
  return graph_ranksep;
}

template <class InheritedAttr, class SynthesizedAttr>
const std::string Programmable<InheritedAttr, SynthesizedAttr>::Program::getClusterLabel(Objects::graph_t * graph) const {
  int pos = graph->tag.find_first_of('_');
  std::string prefix = graph->tag.substr(0, pos);
  std::map<std::string, std::string>::const_iterator it = cluster_label_map.find(prefix);
  if (it == cluster_label_map.end())
    return cluster_default_label;
  return it->second;
}

template <class InheritedAttr, class SynthesizedAttr>
const std::string Programmable<InheritedAttr, SynthesizedAttr>::Program::getClusterColor(Objects::graph_t * graph) const {
  int pos = graph->tag.find_first_of('_');
  std::string prefix = graph->tag.substr(0, pos);
  std::map<std::string, std::string>::const_iterator it = cluster_color_map.find(prefix);
  if (it == cluster_color_map.end())
    return cluster_default_color;
  return it->second;
}

template <class InheritedAttr, class SynthesizedAttr>
const std::string Programmable<InheritedAttr, SynthesizedAttr>::Program::getClusterStyle(Objects::graph_t * graph) const {
  style_e style = cluster_default_style;
  int pos = graph->tag.find_first_of('_');
  std::string prefix = graph->tag.substr(0, pos);
  typename std::map<std::string, style_e>::const_iterator it = cluster_style_map.find(prefix);
  if (it != cluster_style_map.end())
    style = it->second;
  return toString(style);
}

template <class InheritedAttr, class SynthesizedAttr>
const float Programmable<InheritedAttr, SynthesizedAttr>::Program::getClusterPenWidth(Objects::graph_t * graph) const {
  int pos = graph->tag.find_first_of('_');
  std::string prefix = graph->tag.substr(0, pos);
  std::map<std::string, float>::const_iterator it = cluster_pen_width_map.find(prefix);
  if (it == cluster_pen_width_map.end())
    return cluster_default_pen_width;
  return it->second;
}

template <class InheritedAttr, class SynthesizedAttr>
const std::string Programmable<InheritedAttr, SynthesizedAttr>::Program::getEdgeLabel(SgNode * node, SgNode * parent) const {
  return edge_default_label;
}

template <class InheritedAttr, class SynthesizedAttr>
const std::string Programmable<InheritedAttr, SynthesizedAttr>::Program::getEdgeColor(SgNode * node, SgNode * parent) const {
  return edge_default_color;
}

template <class InheritedAttr, class SynthesizedAttr>
const std::string Programmable<InheritedAttr, SynthesizedAttr>::Program::getEdgeStyle(SgNode * node, SgNode * parent) const {
  return toString(edge_default_style);
}

template <class InheritedAttr, class SynthesizedAttr>
const int Programmable<InheritedAttr, SynthesizedAttr>::Program::getEdgeMinLen(SgNode * node, SgNode * parent) const {
  return edge_default_min_len;
}

template <class InheritedAttr, class SynthesizedAttr>
const float Programmable<InheritedAttr, SynthesizedAttr>::Program::getEdgePenWidth(SgNode * node, SgNode * parent) const {
  return edge_default_pen_width;
}

template <class InheritedAttr, class SynthesizedAttr>
const bool Programmable<InheritedAttr, SynthesizedAttr>::Program::getEdgeConstraint(SgNode * node, SgNode * parent) const {
  return edge_default_constraint;
}

template <class InheritedAttr, class SynthesizedAttr>
const std::string Programmable<InheritedAttr, SynthesizedAttr>::Program::getNodeLabel(SgNode * node, const InheritedAttr & inhr_attr, const SynthesizedAttr & synth_attr) const {
  VariantT variant = node->variantT();
  std::map<VariantT, std::string>::const_iterator it = node_label_map.find(variant);
  while (it == node_label_map.end() && variant != (VariantT)SgNode::static_variant) {
    variant = parent_variant_map.at(variant);
    it = node_label_map.find(variant);
  }
  assert(it != node_label_map.end());
  std::string label = it->second;
  if (!label.empty() && label[0] == '@') {
    // TODO programmatic display
  }
  return label;
}

template <class InheritedAttr, class SynthesizedAttr>
const std::string Programmable<InheritedAttr, SynthesizedAttr>::Program::getNodeColor(SgNode * node, const InheritedAttr & inhr_attr, const SynthesizedAttr & synth_attr) const {
  VariantT variant = node->variantT();
//std::cerr << "[getNodeColor] node->class_name() = " << node->class_name() << std::endl;
  std::map<VariantT, std::string>::const_iterator it = node_color_map.find(variant);
  while (it == node_color_map.end() && variant != (VariantT)SgNode::static_variant) {
  //std::cerr << "[getNodeColor] variant = " << variant << std::endl;
  //std::cerr << "[getNodeColor] node = " << variant_name_map.at(variant) << std::endl;
    variant = parent_variant_map.at(variant);
    it = node_color_map.find(variant);
  }
  assert(it != node_color_map.end());
  return it->second;
}

template <class InheritedAttr, class SynthesizedAttr>
const std::string Programmable<InheritedAttr, SynthesizedAttr>::Program::getNodeStyle(SgNode * node, const InheritedAttr & inhr_attr, const SynthesizedAttr & synth_attr) const {
  VariantT variant = node->variantT();
//std::cerr << "[getNodeStyle] node->class_name() = " << node->class_name() << std::endl;
  typename std::map<VariantT, style_e>::const_iterator it = node_style_map.find(variant);
  while (it == node_style_map.end() && variant != (VariantT)SgNode::static_variant) {
  //std::cerr << "[getNodeStyle] variant = " << variant << std::endl;
  //std::cerr << "[getNodeStyle] node = " << variant_name_map.at(variant) << std::endl;
    variant = parent_variant_map.at(variant);
    it = node_style_map.find(variant);
  }
  assert(it != node_style_map.end());
  return toString(it->second);
}

template <class InheritedAttr, class SynthesizedAttr>
const std::string Programmable<InheritedAttr, SynthesizedAttr>::Program::getNodeShape(SgNode * node, const InheritedAttr & inhr_attr, const SynthesizedAttr & synth_attr) const {
  VariantT variant = node->variantT();
//std::cerr << "[getNodeShape] node->class_name() = " << node->class_name() << std::endl;
  std::map<VariantT, std::string>::const_iterator it = node_shape_map.find(variant);
  while (it == node_shape_map.end() && variant != (VariantT)SgNode::static_variant) {
  //std::cerr << "[getNodeShape] variant = " << variant << std::endl;
  //std::cerr << "[getNodeShape] node = " << variant_name_map.at(variant) << std::endl;
    variant = parent_variant_map.at(variant);
    it = node_shape_map.find(variant);
  }
  assert(it != node_shape_map.end());
  return it->second;
}

template <class InheritedAttr, class SynthesizedAttr>
const std::string Programmable<InheritedAttr, SynthesizedAttr>::Program::getNodeFillColor(SgNode * node, const InheritedAttr & inhr_attr, const SynthesizedAttr & synth_attr) const {
  VariantT variant = node->variantT();
//std::cerr << "[getNodeFillColor] node->class_name() = " << node->class_name() << std::endl;
  std::map<VariantT, std::string>::const_iterator it = node_fill_color_map.find(variant);
  while (it == node_fill_color_map.end() && variant != (VariantT)SgNode::static_variant) {
  //std::cerr << "[getNodeFillColor] variant = " << variant << std::endl;
  //std::cerr << "[getNodeFillColor] node = " << variant_name_map.at(variant) << std::endl;
    variant = parent_variant_map.at(variant);
    it = node_fill_color_map.find(variant);
  }
  assert(it != node_fill_color_map.end());
  return it->second;
}

template <class InheritedAttr, class SynthesizedAttr>
const float Programmable<InheritedAttr, SynthesizedAttr>::Program::getNodePenWidth(SgNode * node, const InheritedAttr & inhr_attr, const SynthesizedAttr & synth_attr) const {
  VariantT variant = node->variantT();
//std::cerr << "[getNodePenWidth] node->class_name() = " << node->class_name() << std::endl;
  std::map<VariantT, float>::const_iterator it = node_pen_width_map.find(variant);
  while (it == node_pen_width_map.end() && variant != (VariantT)SgNode::static_variant) {
  //std::cerr << "[getNodePenWidth] variant = " << variant << std::endl;
  //std::cerr << "[getNodePenWidth] node = " << variant_name_map.at(variant) << std::endl;
    variant = parent_variant_map.at(variant);
    it = node_pen_width_map.find(variant);
  }
  assert(it != node_pen_width_map.end());
  return it->second;
}

template <class InheritedAttr, class SynthesizedAttr>
Programmable<InheritedAttr, SynthesizedAttr>::Programmable(const Program & program_) :
  Traversal<InheritedAttr, SynthesizedAttr>(),
  program(program_)
{}

// Implements virtuals

template <class InheritedAttr, class SynthesizedAttr>
InheritedAttr Programmable<InheritedAttr, SynthesizedAttr>::evaluateInheritedAttribute(SgNode * node, InheritedAttr attr) {
  InheritedAttr res;
  return res;
}

template <class InheritedAttr, class SynthesizedAttr>
SynthesizedAttr Programmable<InheritedAttr, SynthesizedAttr>::evaluateSynthesizedAttribute(SgNode * node, InheritedAttr attr, StackFrameVector<SynthesizedAttr> attrs) {
  SynthesizedAttr res;
  return res;
}

template <class InheritedAttr, class SynthesizedAttr>
bool Programmable<InheritedAttr, SynthesizedAttr>::stopAfter(SgNode *) {
  return false;
}

template <class InheritedAttr, class SynthesizedAttr>
bool Programmable<InheritedAttr, SynthesizedAttr>::skip(SgNode *) {
  return false;
}

template <class InheritedAttr, class SynthesizedAttr>
Objects::graph_t * Programmable<InheritedAttr, SynthesizedAttr>::startOn(SgNode *) {
  return NULL;
}

template <class InheritedAttr, class SynthesizedAttr>
Objects::graph_t * Programmable<InheritedAttr, SynthesizedAttr>::getSubgraph(SgNode * node, Objects::graph_t * graph) {
  return graph;
}

template <class InheritedAttr, class SynthesizedAttr>
void Programmable<InheritedAttr, SynthesizedAttr>::edit(SgNode * node, Objects::node_desc_t & desc, const InheritedAttr & inhr_attr, const SynthesizedAttr & synth_attr) const {
  desc.label     = program.getNodeLabel(node, inhr_attr, synth_attr);
  desc.color     = program.getNodeColor(node, inhr_attr, synth_attr);
  desc.style     = program.getNodeStyle(node, inhr_attr, synth_attr);
  desc.shape     = program.getNodeShape(node, inhr_attr, synth_attr);
  desc.fillcolor = program.getNodeFillColor(node, inhr_attr, synth_attr);
  desc.penwidth  = program.getNodePenWidth(node, inhr_attr, synth_attr);
}

template <class InheritedAttr, class SynthesizedAttr>
void Programmable<InheritedAttr, SynthesizedAttr>::edit(SgNode * node, SgNode * parent, Objects::edge_desc_t & desc) const {
  desc.label      = program.getEdgeLabel(node, parent);
  desc.color      = program.getEdgeColor(node, parent);
  desc.style      = program.getEdgeStyle(node, parent);
  desc.minlen     = program.getEdgeMinLen(node, parent);
  desc.penwidth   = program.getEdgePenWidth(node, parent);
  desc.constraint = program.getEdgeConstraint(node, parent);
}

template <class InheritedAttr, class SynthesizedAttr>
void Programmable<InheritedAttr, SynthesizedAttr>::edit(Objects::graph_t * graph, Objects::cluster_desc_t & desc) const {
  desc.label    = program.getClusterLabel(graph);
  desc.color    = program.getClusterColor(graph);
  desc.style    = program.getClusterStyle(graph);
  desc.penwidth = program.getClusterPenWidth(graph);
}

template <class InheritedAttr, class SynthesizedAttr>
void Programmable<InheritedAttr, SynthesizedAttr>::edit(Objects::graph_desc_t & desc) const {
  desc.label   = program.getGraphLabel();
  desc.color   = program.getGraphColor();
  desc.style   = program.getGraphStyle();
  desc.rankdir = program.getGraphRankDir();
  desc.ranksep = program.getGraphRankSep();
}

}

}

}

