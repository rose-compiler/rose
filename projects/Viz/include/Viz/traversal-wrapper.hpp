
#ifndef __ROSE_VIZ_TRAVERSAL_WRAPPER_HPP__
#define __ROSE_VIZ_TRAVERSAL_WRAPPER_HPP__

#include "Viz/objects.hpp"
#include "Viz/Traversals/traversal.hpp"

#include "sage3basic.h"

#include <string>
#include <sstream>
#include <map>
#include <utility>

namespace Rose {

namespace Viz {

template <class InheritedAttr>
struct VizAttrWrapper {
    struct viz_attr_t {
      SgNode * node;
      SgNode * parent;
      Objects::graph_t * graph;

      SgNode * stopped_by;
      SgNode * skipped_since;
    };

    viz_attr_t viz_attr;
    InheritedAttr user_attr;

    VizAttrWrapper();
    VizAttrWrapper(const InheritedAttr & user_attr_);
};

template <class InheritedAttr_, class SynthesizedAttr_>
class TraversalWrapper : public AstTopDownBottomUpProcessing<
                           VizAttrWrapper<InheritedAttr_>,
                           SynthesizedAttr_
                         > {
  public:
    typedef InheritedAttr_   UserInheritedAttr;
    typedef SynthesizedAttr_ UserSynthesizedAttr;

    typedef VizAttrWrapper<UserInheritedAttr> InheritedAttr;
    typedef SynthesizedAttr_                  SynthesizedAttr;

    typedef typename InheritedAttr::viz_attr_t VizAttr;

  protected:
    Traversals::Traversal<UserInheritedAttr, UserSynthesizedAttr> & traversal;
    std::map<SgNode *, InheritedAttr> inherited_attr_map;
    std::map<SgNode *, SynthesizedAttr> synthesized_attr_map;
    Objects::graph_t * universe_graph;

  public:
    TraversalWrapper(Traversals::Traversal<UserInheritedAttr, UserSynthesizedAttr> & traversal_);
    ~TraversalWrapper();

    Objects::graph_t * getGraph(const std::string & tag, Objects::graph_t * parent = NULL);

    Objects::graph_t * mergeGraph(Objects::graph_t * g1, Objects::graph_t * g2, bool fuse = false);

    Objects::graph_t * ancestorGraph(Objects::graph_t * g1, Objects::graph_t * g2);

    InheritedAttr makeAttribute(const std::string & tag);
    InheritedAttr makeAttribute(const std::string & tag, const UserInheritedAttr & user_attr);

    void collectGraphElements(
           Objects::graph_t * graph,
           std::vector<Objects::graph_t *> & graphs,
           std::map<Objects::graph_t *, std::set<std::pair<SgNode *, SgNode *> > > & edge_map,
           std::map<SgNode *, Objects::graph_t *> node_graph_map
    );

    void editGraphElement(
           Objects::graph_t * graph,
           const std::vector<Objects::graph_t *> & clusters,
           const std::map<Objects::graph_t *, std::set<std::pair<SgNode *, SgNode *> > > & edge_map,
           std::map<Objects::graph_t *, Objects::cluster_desc_t> & cluster_desc,
           std::map<SgNode *, Objects::node_desc_t> & node_desc,
           std::map<std::pair<SgNode *, SgNode *>, Objects::edge_desc_t> & edge_desc
         );

    static std::string getNodeName(SgNode * node);

    void toGraphViz(const Objects::graph_desc_t & desc, std::ostream & out, std::string indent = std::string("  "));

    void toGraphViz(const Objects::cluster_desc_t & desc, std::ostream & out, std::string indent = std::string("  "));

    void toGraphViz(SgNode * node, const Objects::node_desc_t & desc, std::ostream & out, std::string indent = std::string("  "));

    void toGraphViz(std::pair<SgNode *, SgNode *> edge, const Objects::edge_desc_t & desc, std::ostream & out, std::string indent = std::string("  "));

    void toGraphViz(
           Objects::graph_t * graph,
           const std::vector<Objects::graph_t *> & clusters,
           const std::map<Objects::graph_t *, std::set<std::pair<SgNode *, SgNode *> > > & edge_map,
           const std::map<Objects::graph_t *, Objects::cluster_desc_t> & cluster_desc,
           const std::map<SgNode *, Objects::node_desc_t> & node_desc,
           const std::map<std::pair<SgNode *, SgNode *>, Objects::edge_desc_t> & edge_desc,
           std::ostream & out, std::string indent = std::string("  ")
         );

    void toGraphViz(const std::string & tag, std::ostream & out);

  protected:
    const InheritedAttr & getInheritedAttribute(SgNode *) const;
    const VizAttr & getVizAttribute(SgNode *) const;

    const SynthesizedAttr & getSynthesizedAttribute(SgNode *) const;

    InheritedAttr evaluateInheritedAttribute(SgNode * node, InheritedAttr attr);

    SynthesizedAttr evaluateSynthesizedAttribute(SgNode * node, InheritedAttr attr, StackFrameVector<SynthesizedAttr> attrs);

    VizAttr evaluateVizAttribute(SgNode * node, VizAttr);
};

}

}

#include "Viz/traversal-wrapper.tpp"

#endif /* __ROSE_VIZ_TRAVERSAL_WRAPPER_HPP__ */

