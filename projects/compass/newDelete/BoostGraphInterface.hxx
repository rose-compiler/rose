#ifndef BOOSTGRAPHINTERFACE_HXX_
#define BOOSTGRAPHINTERFACE_HXX_


#include <boost/config.hpp>
#include <boost/graph/transpose_graph.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/iterator/counting_iterator.hpp>
#include <algorithm>
#include <boost/graph/visitors.hpp>


using namespace boost;

namespace BOOSTGraphInterface {

  //typedef std::pair<VirtualCFG::CFGNode, SgInitializedName*> tps_node;

  struct tps_node:public std::pair<VirtualCFG::CFGNode, SgInitializedName*> {
    typedef std::pair<VirtualCFG::CFGNode, SgInitializedName*> base;
    tps_node(VirtualCFG::CFGNode a, SgInitializedName* b):base(a,b){}      
    tps_node():base(VirtualCFG::CFGNode (0,0),0){}      
  };

inline std::ostream& operator << (std::ostream& o, tps_node node) {
    o << node.first.toString() << " " << node.second->get_name().str() ;
    return o;
}

typedef std::pair<tps_node, tps_node> tps_edge;

class deref_outEdgeVector ;
typedef transform_iterator<deref_outEdgeVector, counting_iterator<int> > tps_out_edge_iterator;

class tps_graph_interface {
public:
    virtual std::vector <tps_node> tps_out_edges(tps_node) const=0;
  virtual ~tps_graph_interface() {}
};

class tps_graph {
public:
    
    tps_graph( tps_graph_interface const* graph_interface=0):graph_interface(graph_interface) {
    }
    typedef tps_node vertex_descriptor;
    typedef tps_edge edge_descriptor;
    typedef void adjacency_iterator;
    typedef tps_out_edge_iterator out_edge_iterator;
    typedef void in_edge_iterator;
    typedef void edge_iterator;
    typedef void vertex_iterator;
    typedef int degree_size_type;
    typedef int vertices_size_type;
    typedef int edges_size_type;
    typedef directed_tag directed_category;
    typedef disallow_parallel_edge_tag edge_parallel_category;
    typedef incidence_graph_tag traversal_category;

    tps_graph_interface const* graph_interface;
};


class deref_outEdgeVector {
protected:
    tps_node vertex;
    tps_graph  g;
    
public:
    deref_outEdgeVector() {}
    deref_outEdgeVector(tps_node ver, tps_graph  graph):vertex(ver),g(graph) {
    }
    tps_edge operator()(int index) const {
        return make_pair(vertex, g.graph_interface->tps_out_edges(vertex)[index]);
    }
    typedef tps_edge result_type;
};




typedef boost::associative_property_map< std::map<tps_node, int> > colorMapType;
typedef boost::associative_property_map< std::map<tps_node, tps_node> > predMapType;

inline std::pair<tps_out_edge_iterator, tps_out_edge_iterator> out_edges(tps_node node, tps_graph  g) {
    return std::make_pair(tps_out_edge_iterator (counting_iterator<int>(0) ,deref_outEdgeVector (node,g)) ,
                          tps_out_edge_iterator (counting_iterator<int>(g.graph_interface->tps_out_edges(node).size()) ,deref_outEdgeVector (node,g)) );
}

inline int out_degree (tps_node node, tps_graph  g) {
    return g.graph_interface->tps_out_edges(node).size();

};


};


#endif /*BOOSTGRAPHINTERFACE_HXX_*/

