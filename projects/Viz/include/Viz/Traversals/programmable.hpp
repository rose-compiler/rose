
#include "Viz/Traversals/traversal.hpp"

#ifndef __ROSE_VIZ_TRAVERSALS_PROGRAMMABLE_HPP__
#define __ROSE_VIZ_TRAVERSALS_PROGRAMMABLE_HPP__

namespace Rose {

namespace Viz {

namespace Traversals {

template <class InheritedAttr_, class SynthesizedAttr_>
struct Programmable : Traversal<InheritedAttr_, SynthesizedAttr_> {
  typedef InheritedAttr_ InheritedAttr;
  typedef SynthesizedAttr_ SynthesizedAttr;

  class Program {
    public:
      enum style_e   { e_none, e_filled, e_invisible };
      enum rankdir_e { e_top_bottom, e_bottom_top, e_left_right, e_right_left};

    public:
      Program();

      void open(const std::string & file);

      void setTitle(const std::string & title);

     protected:
       std::map<std::string, VariantT> name_variant_map;
       std::map<VariantT, std::string> variant_name_map;
       std::map<VariantT, VariantT> parent_variant_map;

     protected:
       static std::string toString(style_e style);
       static std::string toString(rankdir_e rankdir);

       static style_e styleFromString(const std::string & str);
       static rankdir_e rankDirFromString(const std::string & str);

  /// Graph
    protected:
      std::string graph_title;
      std::string graph_color;
      style_e     graph_style;
      rankdir_e   graph_rankdir;
      float       graph_ranksep;

    public:
      const std::string getGraphLabel  () const;
      const std::string getGraphColor  () const;
      const std::string getGraphStyle  () const;
      const std::string getGraphRankDir() const;
      const float       getGraphRankSep() const;

  /// Cluster
    protected:
      std::map<std::string, std::string> cluster_label_map;
      std::map<std::string, std::string> cluster_color_map;
      std::map<std::string, style_e>     cluster_style_map;
      std::map<std::string, float>       cluster_pen_width_map;

      std::string cluster_default_label;
      std::string cluster_default_color;
      style_e     cluster_default_style;
      float       cluster_default_pen_width;

    public:
      const std::string getClusterLabel   (Objects::graph_t * graph) const;
      const std::string getClusterColor   (Objects::graph_t * graph) const;
      const std::string getClusterStyle   (Objects::graph_t * graph) const;
      const float       getClusterPenWidth(Objects::graph_t * graph) const;

  /// Edge
    protected:
      std::string edge_default_label;
      std::string edge_default_color;
      style_e     edge_default_style;
      int         edge_default_min_len;
      float       edge_default_pen_width;
      bool        edge_default_constraint;

    public:
      const std::string getEdgeLabel     (SgNode * node, SgNode * parent) const;
      const std::string getEdgeColor     (SgNode * node, SgNode * parent) const;
      const std::string getEdgeStyle     (SgNode * node, SgNode * parent) const;
      const int         getEdgeMinLen    (SgNode * node, SgNode * parent) const;
      const float       getEdgePenWidth  (SgNode * node, SgNode * parent) const;
      const bool        getEdgeConstraint(SgNode * node, SgNode * parent) const;

  /// Node
    protected:
      std::map<VariantT, std::string> node_label_map;
      std::map<VariantT, std::string> node_color_map;
      std::map<VariantT, style_e>     node_style_map;
      std::map<VariantT, std::string> node_shape_map;
      std::map<VariantT, std::string> node_fill_color_map;
      std::map<VariantT, float>       node_pen_width_map;

    public:
      const std::string getNodeLabel    (SgNode * node, const InheritedAttr & inhr_attr, const SynthesizedAttr & synth_attr) const;
      const std::string getNodeColor    (SgNode * node, const InheritedAttr & inhr_attr, const SynthesizedAttr & synth_attr) const;
      const std::string getNodeStyle    (SgNode * node, const InheritedAttr & inhr_attr, const SynthesizedAttr & synth_attr) const;
      const std::string getNodeShape    (SgNode * node, const InheritedAttr & inhr_attr, const SynthesizedAttr & synth_attr) const;
      const std::string getNodeFillColor(SgNode * node, const InheritedAttr & inhr_attr, const SynthesizedAttr & synth_attr) const;
      const float       getNodePenWidth (SgNode * node, const InheritedAttr & inhr_attr, const SynthesizedAttr & synth_attr) const;
  };

  const Program & program;

  Programmable(const Program & program_);

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

#include "Viz/Traversals/programmable.tpp"

#endif /* __ROSE_VIZ_TRAVERSALS_PROGRAMMABLE_HPP__ */

