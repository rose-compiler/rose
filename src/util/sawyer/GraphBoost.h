// Boost Graph Library (BGL) interface around Sawyer::Container::Graph
#ifndef Sawyer_GraphBoost_H
#define Sawyer_GraphBoost_H

#include <sawyer/Graph.h>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/properties.hpp>

namespace Sawyer {

/** Boost Graph Library API for Sawyer::Container::Boost.
 *
 *  Including this header into a program will allow a Sawyer::Container::Graph to be accessed via the Boost Graph Library (BGL)
 *  API. %Sawyer non-const graphs implement the following BGL concepts: MutablePropertyGraph, MutableGraph, PropertyMapGraph,
 *  BidirectionalGraph, IncidenceGraph, EdgeListGraph, VertexListGraph, and Graph.  Const graphs implement the same concepts
 *  except MutablePropertyGraph and MutableGraph.
 *
 *  Vertex and edge iterators in the BGL domain map to VertexOuterIterator and EdgeOuterIterator types, which are defined in
 *  this namespace and have implicit conversions from Sawyer::Container::Graph::VertexNodeIterator and
 *  Sawyer::Container::Graph::EdgeNodeIterator. There are also const versions.  These outer iterators produce BGL
 *  vertex_descriptor or edge_descriptor when they're dereferenced, and these descriptors are simply the %Sawyer vertex and
 *  edge ID numbers.
 *
 *  The BGL API allows graphs to store any number of vertex and edge properties internally, but %Sawyer graphs take a more
 *  STL-like approach allowing the user to specify arbitrary types for the vertices and edges. This namespace declares
 *  vertex_value_t and edge_value_t tags to access the user-supplied vertex and edge values.  The vertex_id_t and edge_id_t
 *  tags can be used to access the ID properties that %Sawyer automatically maintains for vertices and edges.  BGL external
 *  properties have no such restrictions and can be used exactly as advertised in the BGL documentation. */
namespace Boost {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Iterators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// BGL vertex and edge iterators, when dereferenced, yield a BGL vertex descriptor; BGL vertex descriptors are used to look up
// property values in property maps.  Since we want O(1) lookup times for properties, we'll use Sawyer vertex and edge ID
// numbers as BGL vertex and edge descriptors.  Therefore, we need to provide iterators which when dereferenced return a Sawyer
// vertex or edge ID.
//
// Another reason for using Sawyer vertex and edge ID numbers as BGL vertex and edge descriptors is that BGL expects there to
// exist a single vertex descriptor that represents "no vertex".  We can use (size_t)(-1) for this, but we would not have been
// able to use Sawyer "end" iterators since the end of one list is not equal to the end of another list.

template<class V, class E>
class VertexOuterIterator: public std::iterator<std::bidirectional_iterator_tag, const size_t> {
private:
    typedef typename Sawyer::Container::Graph<V, E>::VertexNodeIterator BaseIter;
    BaseIter base_;
public:
    VertexOuterIterator() {}
    VertexOuterIterator(const VertexOuterIterator &other): base_(other.base_) {}
    explicit VertexOuterIterator(const BaseIter &base): base_(base) {}
    VertexOuterIterator& operator=(const VertexOuterIterator &other) { base_ = other.base_; return *this; }
    VertexOuterIterator& operator++() { ++base_; return *this; }
    VertexOuterIterator& operator--() { --base_; return *this; }
    VertexOuterIterator operator++(int) { VertexOuterIterator old = *this; ++base_; return old; }
    VertexOuterIterator operator--(int) { VertexOuterIterator old = *this; --base_; return old; }
    bool operator==(const VertexOuterIterator &other) const { return base_ == other.base_; }
    bool operator!=(const VertexOuterIterator &other) const { return base_ != other.base_; }
    const size_t& operator*() const { return base_->id(); }
    // size_t* operator->() const; //no methods defined on size_t, so not needed
};

template<class V, class E>
class ConstVertexOuterIterator: public std::iterator<std::bidirectional_iterator_tag, const size_t> {
private:
    typedef typename Sawyer::Container::Graph<V, E>::ConstVertexNodeIterator BaseIter;
    BaseIter base_;
public:
    ConstVertexOuterIterator() {}
    ConstVertexOuterIterator(const ConstVertexOuterIterator &other): base_(other.base_) {}
    ConstVertexOuterIterator(const VertexOuterIterator<V, E> &other): base_(other.base_) {}
    explicit ConstVertexOuterIterator(const BaseIter &base): base_(base) {}
    ConstVertexOuterIterator& operator=(const ConstVertexOuterIterator &other) { base_ = other.base_; return *this; }
    ConstVertexOuterIterator& operator++() { ++base_; return *this; }
    ConstVertexOuterIterator& operator--() { --base_; return *this; }
    ConstVertexOuterIterator operator++(int) { ConstVertexOuterIterator old = *this; ++base_; return old; }
    ConstVertexOuterIterator operator--(int) { ConstVertexOuterIterator old = *this; --base_; return old; }
    bool operator==(const ConstVertexOuterIterator &other) const { return base_ == other.base_; }
    bool operator!=(const ConstVertexOuterIterator &other) const { return base_ != other.base_; }
    const size_t& operator*() const { return base_->id(); }
    // size_t* operator->() const; //no methods defined on size_t, so not needed
};

template<class V, class E>
class EdgeOuterIterator: public std::iterator<std::bidirectional_iterator_tag, const size_t> {
private:
    typedef typename Sawyer::Container::Graph<V, E>::EdgeNodeIterator BaseIter;
    BaseIter base_;
public:
    EdgeOuterIterator() {}
    EdgeOuterIterator(const EdgeOuterIterator &other): base_(other.base_) {}
    explicit EdgeOuterIterator(const BaseIter &base): base_(base) {}
    EdgeOuterIterator& operator=(const EdgeOuterIterator &other) { base_ = other.base_; return *this; }
    EdgeOuterIterator& operator++() { ++base_; return *this; }
    EdgeOuterIterator& operator--() { --base_; return *this; }
    EdgeOuterIterator operator++(int) { EdgeOuterIterator old = *this; ++base_; return old; }
    EdgeOuterIterator operator--(int) { EdgeOuterIterator old = *this; --base_; return old; }
    bool operator==(const EdgeOuterIterator &other) const { return base_ == other.base_; }
    bool operator!=(const EdgeOuterIterator &other) const { return base_ != other.base_; }
    const size_t& operator*() const { return base_->id(); }
    // size_t* operator->() const; //no methods defined on size_t, so not needed
};

template<class V, class E>
class ConstEdgeOuterIterator: public std::iterator<std::bidirectional_iterator_tag, const size_t> {
private:
    typedef typename Sawyer::Container::Graph<V, E>::ConstEdgeNodeIterator BaseIter;
    BaseIter base_;
public:
    ConstEdgeOuterIterator() {}
    ConstEdgeOuterIterator(const ConstEdgeOuterIterator &other): base_(other.base_) {}
    ConstEdgeOuterIterator(const EdgeOuterIterator<V, E> &other): base_(other.base_) {}
    explicit ConstEdgeOuterIterator(const BaseIter &base): base_(base) {}
    ConstEdgeOuterIterator& operator=(const ConstEdgeOuterIterator &other) { base_ = other.base_; return *this; }
    ConstEdgeOuterIterator& operator++() { ++base_; return *this; }
    ConstEdgeOuterIterator& operator--() { --base_; return *this; }
    ConstEdgeOuterIterator operator++(int) { ConstEdgeOuterIterator old = *this; ++base_; return old; }
    ConstEdgeOuterIterator operator--(int) { ConstEdgeOuterIterator old = *this; --base_; return old; }
    bool operator==(const ConstEdgeOuterIterator &other) const { return base_ == other.base_; }
    bool operator!=(const ConstEdgeOuterIterator &other) const { return base_ != other.base_; }
    const size_t& operator*() const { return base_->id(); }
    // size_t* operator->() const; //no methods defined on size_t, so not needed
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Internal properties
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// External properties can use the BGL mechanisms that are already defined, but we need something special for internal
// properties. A BGL property map is simply a class that provides a lookup mechanism when given a vertex or edge descriptor, so
// all we need is a class that will look up the user-supplied value for a vertex or edge.

// User-defined value attached to a vertex.
template<class Graph>
class VertexNodePropertyMap {
    Graph &graph_;
public:
    typedef typename Graph::VertexValue ValueType;
    VertexNodePropertyMap(Graph &graph): graph_(graph) {}
    ValueType get(size_t vertexId) const {
        return graph_.findVertex(vertexId)->value();
    }
    void put(size_t vertexId, const ValueType &value) {
        graph_.findVertex(vertexId)->value() = value;
    }
    ValueType& at(size_t vertexId) {
        return graph_.findVertex(vertexId)->value();
    }
    const ValueType& at(size_t vertexId) const {
        return graph_.findVertex(vertexId)->value();
    }
};

// Const user-defined value attached to a vertex.
template<class Graph>
class ConstVertexNodePropertyMap {
    const Graph &graph_;
public:
    typedef typename Graph::VertexValue ValueType;
    ConstVertexNodePropertyMap(const Graph &graph): graph_(graph) {}
    ValueType get(size_t vertexId) const {
        return graph_.findVertex(vertexId)->value();
    }
    const ValueType& at(size_t vertexId) const {
        return graph_.findVertex(vertexId)->value();
    }
};

// User-defined value attached to an edge. Graph is any non-const Sawyer::Container::Graph<>
template<class Graph>
class EdgeNodePropertyMap {
    Graph &graph_;
public:
    typedef typename Graph::EdgeValue ValueType;
    EdgeNodePropertyMap(Graph &graph): graph_(graph) {}
    ValueType get(size_t edgeId) const {
        return graph_.findEdge(edgeId)->value();
    }
    void put(size_t edgeId, const ValueType &value) {
        graph_.findEdge(edgeId)->value() = value;
    }
    ValueType& at(size_t edgeId) {
        return graph_.findEdge(edgeId)->value();
    }
    const ValueType& at(size_t edgeId) const {
        return graph_.findEdge(edgeId)->value();
    }
};

// Const user-defined value attached to a vertex. Graph is any const Sawyer::Container::Graph<>
template<class Graph>
class ConstEdgeNodePropertyMap {
    const Graph &graph_;
public:
    typedef typename Graph::EdgeValue ValueType;
    ConstEdgeNodePropertyMap(const Graph &graph): graph_(graph) {}
    ValueType get(size_t edgeId) const {
        return graph_.findEdge(edgeId)->value();
    }
    const ValueType& at(size_t edgeId) const {
        return graph_.findEdge(edgeId)->value();
    }
};

// The ID (index) associated with each vertex.  This is a read-only property since ID numbers are managed by the graph.
template<class Graph>
class ConstVertexIdPropertyMap {
    const Graph &graph_;
public:
    ConstVertexIdPropertyMap(const Graph &graph): graph_(graph) {}
    size_t get(size_t vertexId) const {
        return vertexId;
    }
    const size_t& at(size_t vertexId) const {
        return graph_.findVertex(vertexId)->id();
    }
};

// The ID (index) associated with each edge.  This is a read-only property since ID numbers are managed by the graph.
template<class Graph>
class ConstEdgeIdPropertyMap {
    const Graph &graph_;
public:
    ConstEdgeIdPropertyMap(const Graph &graph): graph_(graph) {}
    size_t get(size_t edgeId) const {
        return edgeId;
    }
    const size_t& at(size_t edgeId) const {
        return graph_.findEdge(edgeId)->id();
    }
};

// Tags for the user-defined vertex or edge value property. These are to access the user-defined data associated with each
// vertex are value corresponding to the "V" and "E" template parameters for Sawyer::Container::Graph
struct vertex_value_t {
    typedef boost::vertex_property_tag kind;
};
struct edge_value_t {
    typedef boost::edge_property_tag kind;
};

struct vertex_id_t {
    typedef boost::vertex_property_tag kind;
};

struct edge_id_t {
    typedef boost::edge_property_tag kind;
};

} // namespace
} // namespace


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace boost {


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      BGL trait classes
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 0 // [Robb P. Matzke 2014-05-23]: Temporarily disabled because it matches too much.
// Including this file with boost/wave/util/cpp_include_paths.hpp causes problems because code like this:
//     using boost::multi_index::get;
//     return get<from>(pragma_once_files).find(filename) != pragma_once_files.end();
// in wave matches these property_traits. E.g.,
//     boost::property_traits<Sawyer::Boost::VertexNodePropertyMap<boost::wave::util::from> >
// and of course boost::wave::util::from has no VertexValue type.  Someone with more boost property map and template
// programming experience is needed.  For now, users should instantiate their own Sawyer::Boost::VertexNodePropertyMap<> class
// and use it to make their own specialization of boost::property_traits<>.

template<class Graph>
struct property_traits<Sawyer::Boost::VertexNodePropertyMap<Graph> > {
    typedef typename Graph::VertexValue value_type;
    typedef typename Graph::VertexValue &reference;
    typedef size_t key_type;                            // vertex ID number
    typedef boost::read_write_property_map_tag category;
};

template<class Graph>
struct property_traits<Sawyer::Boost::ConstVertexNodePropertyMap<Graph> > {
    typedef typename Graph::VertexValue value_type;
    typedef typename Graph::VertexValue const &reference;
    typedef size_t key_type;                            // vertex ID number
    typedef boost::readable_property_map_tag category;
};

template<class Graph>
struct property_traits<Sawyer::Boost::EdgeNodePropertyMap<Graph> > {
    typedef typename Graph::EdgeValue value_type;
    typedef typename Graph::EdgeValue &reference;
    typedef size_t key_type;                            // edge ID number
    typedef boost::read_write_property_map_tag category;
};

template<class Graph>
struct property_traits<Sawyer::Boost::ConstEdgeNodePropertyMap<Graph> > {
    typedef typename Graph::EdgeValue value_type;
    typedef typename Graph::EdgeValue const &reference;
    typedef size_t key_type;                            // edge ID number
    typedef boost::readable_property_map_tag category;
};

template<class Graph>
struct property_traits<Sawyer::Boost::ConstVertexIdPropertyMap<Graph> > {
    typedef size_t value_type;
    typedef const size_t &reference;
    typedef size_t key_type;                            // vertex ID number
    typedef boost::readable_property_map_tag category;
};

template<class Graph>
struct property_traits<Sawyer::Boost::ConstEdgeIdPropertyMap<Graph> > {
    typedef size_t value_type;
    typedef const size_t &reference;
    typedef size_t key_type;                            // vertex ID number
    typedef boost::readable_property_map_tag category;
};

#endif


template<class Graph>
struct property_map<Graph, Sawyer::Boost::vertex_value_t> {
    typedef Sawyer::Boost::VertexNodePropertyMap<Graph> type;
    typedef Sawyer::Boost::ConstVertexNodePropertyMap<const Graph> const_type;
};

template<class Graph>
struct property_map<Graph, Sawyer::Boost::edge_value_t> {
    typedef Sawyer::Boost::EdgeNodePropertyMap<Graph> type;
    typedef Sawyer::Boost::ConstEdgeNodePropertyMap<const Graph> const_type;
};

template<class Graph>
struct property_map<Graph, Sawyer::Boost::vertex_id_t> {
    typedef Sawyer::Boost::ConstVertexIdPropertyMap<const Graph> type;
    typedef Sawyer::Boost::ConstVertexIdPropertyMap<const Graph> const_type;
};

template<class Graph>
struct property_map<Graph, Sawyer::Boost::edge_id_t> {
    typedef Sawyer::Boost::ConstEdgeIdPropertyMap<const Graph> type;
    typedef Sawyer::Boost::ConstEdgeIdPropertyMap<const Graph> const_type;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Access members of property maps
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//--- vertex value ---

template<class Graph>
typename Graph::VertexValue&
get(Sawyer::Boost::VertexNodePropertyMap<Graph> &pmap, size_t key) {
    return pmap.at(key);
}

template<class Graph>
const typename Graph::VertexValue&
get(const Sawyer::Boost::ConstVertexNodePropertyMap<Graph> &pmap, size_t key) {
    return pmap.at(key);
}

template<class Graph>
void
put(Sawyer::Boost::VertexNodePropertyMap<Graph> &pmap, size_t key, const typename Graph::VertexValue &value) {
    pmap.at(key) = value;
}

//--- edge value ---

template<class Graph>
typename Graph::EdgeValue&
get(Sawyer::Boost::EdgeNodePropertyMap<Graph> &pmap, size_t key) {
    return pmap.at(key);
}

template<class Graph>
typename Graph::EdgeValue&
get(const Sawyer::Boost::ConstEdgeNodePropertyMap<Graph> &pmap, size_t key) {
    return pmap.at(key);
}

template<class Graph>
void
put(Sawyer::Boost::EdgeNodePropertyMap<Graph> &pmap, size_t key,
    const typename Graph::EdgeValue &value) {
    pmap.at(key) = value;
}

//---  vertex and edge ID (indices) ---

template<class Graph>
const size_t&
get(const Sawyer::Boost::ConstVertexIdPropertyMap<Graph> &pmap, size_t key) {
    return pmap.at(key);
}

template<class Graph>
const size_t&
get(const Sawyer::Boost::ConstEdgeIdPropertyMap<Graph> &pmap, size_t key) {
    return pmap.at(key);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Graph traits
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class V, class E>
struct graph_traits<Sawyer::Container::Graph<V, E> > {
    typedef bidirectional_graph_tag traversal_category;

    // Graph concepts
    typedef size_t vertex_descriptor;
    typedef size_t edge_descriptor;
    typedef directed_tag directed_category;
    typedef allow_parallel_edge_tag edge_parallel_category;
    static size_t null_vertex() { return (size_t)(-1); }

    // VertexListGraph concepts
    typedef Sawyer::Boost::VertexOuterIterator<V, E> vertex_iterator;
    typedef size_t vertices_size_type;

    // EdgeListGraph concepts
    typedef Sawyer::Boost::EdgeOuterIterator<V, E> edge_iterator;
    typedef size_t edges_size_type;

    // IncidenceGraph concepts
    typedef Sawyer::Boost::EdgeOuterIterator<V, E> out_edge_iterator;
    typedef size_t degree_size_type;

    // BidirectionalGraph concepts
    typedef Sawyer::Boost::EdgeOuterIterator<V, E> in_edge_iterator;

    // MutablePropertyGraph concepts
    typedef typename Sawyer::Container::Graph<V, E>::VertexValue vertex_property_type;
    typedef typename Sawyer::Container::Graph<V, E>::EdgeValue edge_property_type;
};

template<class V, class E>
struct graph_traits<const Sawyer::Container::Graph<V, E> > {
    typedef bidirectional_graph_tag traversal_category;

    // Graph concepts
    typedef size_t vertex_descriptor;
    typedef size_t edge_descriptor;
    typedef directed_tag directed_category;
    typedef allow_parallel_edge_tag edge_parallel_category;

    // VertexListGraph concepts
    typedef Sawyer::Boost::ConstVertexOuterIterator<V, E> vertex_iterator;
    typedef size_t vertices_size_type;

    // EdgeListGraph concepts
    typedef Sawyer::Boost::ConstEdgeOuterIterator<V, E> edge_iterator;
    typedef size_t edges_size_type;

    // IncidenceGraph concepts
    typedef Sawyer::Boost::ConstEdgeOuterIterator<V, E> out_edge_iterator;
    typedef size_t degree_size_type;

    // BidirectionalGraph concepts
    typedef Sawyer::Boost::ConstEdgeOuterIterator<V, E> in_edge_iterator;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Graph concepts
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// BGL has a global entity that indicates no-vertex, but Sawyer doesn't--it has STL-like end() iterators.
template<class V, class E>
typename graph_traits<Sawyer::Container::Graph<V, E> >::vertex_descriptor
null_vertex() {
    return (size_t)(-1);
}

template<class V, class E>
typename graph_traits<const Sawyer::Container::Graph<V, E> >::vertex_descriptor
null_vertex() {
    return (size_t)(-1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      VertexListGraph concepts
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class V, class E>
std::pair<typename graph_traits<Sawyer::Container::Graph<V, E> >::vertex_iterator,
          typename graph_traits<Sawyer::Container::Graph<V, E> >::vertex_iterator>
vertices(Sawyer::Container::Graph<V, E> &graph) {
    return std::make_pair(Sawyer::Boost::VertexOuterIterator<V, E>(graph.vertices().begin()),
                          Sawyer::Boost::VertexOuterIterator<V, E>(graph.vertices().end()));
}

template<class V, class E>
std::pair<typename graph_traits<const Sawyer::Container::Graph<V, E> >::vertex_iterator,
          typename graph_traits<const Sawyer::Container::Graph<V, E> >::vertex_iterator>
vertices(const Sawyer::Container::Graph<V, E> &graph) {
    return std::make_pair(Sawyer::Boost::ConstVertexOuterIterator<V, E>(graph.vertices().begin()),
                          Sawyer::Boost::ConstVertexOuterIterator<V, E>(graph.vertices().end()));
}

template<class V, class E>
typename graph_traits<Sawyer::Container::Graph<V, E> >::vertices_size_type
num_vertices(Sawyer::Container::Graph<V, E> &graph) {
    return graph.nVertices();
}

template<class V, class E>
typename graph_traits<const Sawyer::Container::Graph<V, E> >::vertices_size_type
num_vertices(const Sawyer::Container::Graph<V, E> &graph) {
    return graph.nVertices();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      EdgeListGraph concepts
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class V, class E>
std::pair<typename graph_traits<Sawyer::Container::Graph<V, E> >::edge_iterator,
          typename graph_traits<Sawyer::Container::Graph<V, E> >::edge_iterator>
edges(Sawyer::Container::Graph<V, E> &graph) {
    return std::make_pair(Sawyer::Boost::EdgeOuterIterator<V, E>(graph.edges().begin()),
                          Sawyer::Boost::EdgeOuterIterator<V, E>(graph.edges().end()));
}

template<class V, class E>
std::pair<typename graph_traits<const Sawyer::Container::Graph<V, E> >::edge_iterator,
          typename graph_traits<const Sawyer::Container::Graph<V, E> >::edge_iterator>
edges(const Sawyer::Container::Graph<V, E> &graph) {
    return std::make_pair(Sawyer::Boost::ConstEdgeOuterIterator<V, E>(graph.edges().begin()),
                          Sawyer::Boost::ConstEdgeOuterIterator<V, E>(graph.edges().end()));
}

template<class V, class E>
typename graph_traits<Sawyer::Container::Graph<V, E> >::edges_size_type
num_edges(Sawyer::Container::Graph<V, E> &graph) {
    return graph.nEdges();
}

template<class V, class E>
typename graph_traits<const Sawyer::Container::Graph<V, E> >::edges_size_type
num_edges(const Sawyer::Container::Graph<V, E> &graph) {
    return graph.nEdges();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      IncidenceGraph concepts
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class V, class E>
typename graph_traits<Sawyer::Container::Graph<V, E> >::vertex_descriptor
source(typename graph_traits<Sawyer::Container::Graph<V, E> >::edge_descriptor edge,
       Sawyer::Container::Graph<V, E> &graph) {
    return graph.findEdge(edge)->source()->id();
}

template<class V, class E>
typename graph_traits<const Sawyer::Container::Graph<V, E> >::vertex_descriptor
source(typename graph_traits<const Sawyer::Container::Graph<V, E> >::edge_descriptor edge,
       const Sawyer::Container::Graph<V, E> &graph) {
    return graph.findEdge(edge)->source()->id();
}

template<class V, class E>
typename graph_traits<Sawyer::Container::Graph<V, E> >::vertex_descriptor
target(typename graph_traits<Sawyer::Container::Graph<V, E> >::edge_descriptor edge,
       Sawyer::Container::Graph<V, E> &graph) {
    return graph.findEdge(edge)->target()->id();
}

template<class V, class E>
typename graph_traits<const Sawyer::Container::Graph<V, E> >::vertex_descriptor
target(typename graph_traits<const Sawyer::Container::Graph<V, E> >::edge_descriptor edge,
       const Sawyer::Container::Graph<V, E> &graph) {
    return graph.findEdge(edge)->target()->id();
}

template<class V, class E>
std::pair<typename graph_traits<Sawyer::Container::Graph<V, E> >::out_edge_iterator,
          typename graph_traits<Sawyer::Container::Graph<V, E> >::out_edge_iterator>
out_edges(typename graph_traits<Sawyer::Container::Graph<V, E> >::vertex_descriptor vertex,
          Sawyer::Container::Graph<V, E> &graph) {
    typename Sawyer::Container::Graph<V, E>::VertexNodeIterator v = graph.findVertex(vertex);
    return std::make_pair(Sawyer::Boost::EdgeOuterIterator<V, E>(v->outEdges().begin()),
                          Sawyer::Boost::EdgeOuterIterator<V, E>(v->outEdges().end()));
}

template<class V, class E>
std::pair<typename graph_traits<const Sawyer::Container::Graph<V, E> >::out_edge_iterator,
          typename graph_traits<const Sawyer::Container::Graph<V, E> >::out_edge_iterator>
out_edges(typename graph_traits<const Sawyer::Container::Graph<V, E> >::vertex_descriptor vertex,
          const Sawyer::Container::Graph<V, E> &graph) {
    typename Sawyer::Container::Graph<V, E>::ConstVertexNodeIterator v = graph.findVertex(vertex);
    return std::make_pair(Sawyer::Boost::ConstEdgeOuterIterator<V, E>(v->outEdges().begin()),
                          Sawyer::Boost::ConstEdgeOuterIterator<V, E>(v->outEdges().end()));
}

template<class V, class E>
typename graph_traits<Sawyer::Container::Graph<V, E> >::degree_size_type
out_degree(typename graph_traits<Sawyer::Container::Graph<V, E> >::vertex_descriptor vertex,
           Sawyer::Container::Graph<V, E> &graph) {
    return graph.findVertex(vertex)->nOutEdges();
}

template<class V, class E>
typename graph_traits<const Sawyer::Container::Graph<V, E> >::degree_size_type
out_degree(typename graph_traits<const Sawyer::Container::Graph<V, E> >::vertex_descriptor vertex,
           const Sawyer::Container::Graph<V, E> &graph) {
    return graph.findVertex(vertex)->nOutEdges();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      BidirectionalGraph concepts
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class V, class E>
std::pair<typename graph_traits<Sawyer::Container::Graph<V, E> >::in_edge_iterator,
          typename graph_traits<Sawyer::Container::Graph<V, E> >::in_edge_iterator>
in_edges(typename graph_traits<Sawyer::Container::Graph<V, E> >::vertex_descriptor vertex,
         Sawyer::Container::Graph<V, E> &graph) {
    typename Sawyer::Container::Graph<V, E>::VertexNodeIterator v = graph.findVertex(vertex);
    return std::make_pair(Sawyer::Boost::EdgeOuterIterator<V, E>(v->inEdges().begin()),
                          Sawyer::Boost::EdgeOuterIterator<V, E>(v->inEdges().end()));
}

template<class V, class E>
std::pair<typename graph_traits<const Sawyer::Container::Graph<V, E> >::in_edge_iterator,
          typename graph_traits<const Sawyer::Container::Graph<V, E> >::in_edge_iterator>
in_edges(typename graph_traits<const Sawyer::Container::Graph<V, E> >::vertex_descriptor vertex,
         const Sawyer::Container::Graph<V, E> &graph) {
    typename Sawyer::Container::Graph<V, E>::ConstVertexNodeIterator v = graph.findVertex(vertex);
    return std::make_pair(Sawyer::Boost::ConstEdgeOuterIterator<V, E>(v->inEdges().begin()),
                          Sawyer::Boost::ConstEdgeOuterIterator<V, E>(v->inEdges().end()));
}

template<class V, class E>
typename graph_traits<Sawyer::Container::Graph<V, E> >::degree_size_type
in_degree(typename graph_traits<Sawyer::Container::Graph<V, E> >::vertex_descriptor vertex,
          Sawyer::Container::Graph<V, E> &graph) {
    return graph.findVertex(vertex)->nInEdges();
}

template<class V, class E>
typename graph_traits<const Sawyer::Container::Graph<V, E> >::degree_size_type
in_degree(typename graph_traits<const Sawyer::Container::Graph<V, E> >::vertex_descriptor vertex,
          const Sawyer::Container::Graph<V, E> &graph) {
    return graph.findVertex(vertex)->nInEdges();
}

template<class V, class E>
typename graph_traits<Sawyer::Container::Graph<V, E> >::degree_size_type
degree(typename graph_traits<Sawyer::Container::Graph<V, E> >::vertex_descriptor vertex,
       Sawyer::Container::Graph<V, E> &graph) {
    return in_degree(vertex) + out_degree(vertex);
}

template<class V, class E>
typename graph_traits<const Sawyer::Container::Graph<V, E> >::degree_size_type
degree(typename graph_traits<const Sawyer::Container::Graph<V, E> >::vertex_descriptor vertex,
       const Sawyer::Container::Graph<V, E> &graph) {
    return in_degree(vertex) + out_degree(vertex);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      PropertyMapGraph concepts
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class V, class E>
typename property_map<Sawyer::Container::Graph<V, E>, Sawyer::Boost::vertex_value_t>::type
get(Sawyer::Boost::vertex_value_t, Sawyer::Container::Graph<V, E> &graph) {
    return Sawyer::Boost::VertexNodePropertyMap<Sawyer::Container::Graph<V, E> >(graph);
}

template<class V, class E>
typename property_map<const Sawyer::Container::Graph<V, E>, Sawyer::Boost::vertex_value_t>::type
get(Sawyer::Boost::vertex_value_t, const Sawyer::Container::Graph<V, E> &graph) {
    return Sawyer::Boost::ConstVertexNodePropertyMap<const Sawyer::Container::Graph<V, E> >(graph);
}

template<class V, class E>
typename property_map<Sawyer::Container::Graph<V, E>, Sawyer::Boost::edge_value_t>::type
get(Sawyer::Boost::edge_value_t, Sawyer::Container::Graph<V, E> &graph) {
    return Sawyer::Boost::EdgeNodePropertyMap<Sawyer::Container::Graph<V, E> >(graph);
}

template<class V, class E>
typename property_map<const Sawyer::Container::Graph<V, E>, Sawyer::Boost::edge_value_t>::type
get(Sawyer::Boost::edge_value_t, const Sawyer::Container::Graph<V, E> &graph) {
    return Sawyer::Boost::ConstEdgeNodePropertyMap<const Sawyer::Container::Graph<V, E> >(graph);
}

template<class V, class E>
typename property_map<const Sawyer::Container::Graph<V, E>, Sawyer::Boost::vertex_id_t>::type
get(Sawyer::Boost::vertex_id_t, const Sawyer::Container::Graph<V, E> &graph) {
    return Sawyer::Boost::ConstVertexIdPropertyMap<const Sawyer::Container::Graph<V, E> >(graph);
}

template<class V, class E>
typename property_map<const Sawyer::Container::Graph<V, E>, Sawyer::Boost::edge_id_t>::type
get(Sawyer::Boost::edge_id_t, const Sawyer::Container::Graph<V, E> &graph) {
    return Sawyer::Boost::ConstEdgeIdPropertyMap<const Sawyer::Container::Graph<V, E> >(graph);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      MutableGraph concepts
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class V, class E>
std::pair<typename graph_traits<Sawyer::Container::Graph<V, E> >::edge_descriptor, bool>
add_edge(typename graph_traits<Sawyer::Container::Graph<V, E> >::vertex_descriptor source,
         typename graph_traits<Sawyer::Container::Graph<V, E> >::vertex_descriptor target,
         Sawyer::Container::Graph<V, E> &graph) {
    typename Sawyer::Container::Graph<V, E>::VertexNodeIterator src=graph.findVertex(source), tgt=graph.findVertex(target);
    typename Sawyer::Container::Graph<V, E>::EdgeNodeIterator newEdge = graph.insertEdge(src, tgt, E());
    return std::make_pair(newEdge->id(), true);
}

template<class V, class E>
void
remove_edge(typename graph_traits<Sawyer::Container::Graph<V, E> >::vertex_descriptor source,
            typename graph_traits<Sawyer::Container::Graph<V, E> >::vertex_descriptor target,
            Sawyer::Container::Graph<V, E> &graph) {
    typename Sawyer::Container::Graph<V, E>::VertexNodeIterator src=graph.findVertex(source), tgt=graph.findVertex(target);
    graph.eraseEdges(src, tgt);
}

template<class V, class E>
void
remove_edge(typename graph_traits<Sawyer::Container::Graph<V, E> >::edge_descriptor edge,
            Sawyer::Container::Graph<V, E> &graph) {
    graph.eraseEdge(graph.findEdge(edge));
}

template<class Predicate, class V, class E>
void
remove_edge_if(Predicate predicate,
               Sawyer::Container::Graph<V, E> &graph) {
    typename Sawyer::Container::Graph<V, E>::EdgeNodeIterator edge = graph.edges().begin();
    while (edge != graph.edges().end()) {
        if (predicate(edge->id())) {
            edge = graph.eraseEdge(edge);
        } else {
            ++edge;
        }
    }
}

template<class Predicate, class V, class E>
void
remove_out_edge_if(typename graph_traits<Sawyer::Container::Graph<V, E> >::vertex_descriptor vertex,
                   Predicate predicate,
                   Sawyer::Container::Graph<V, E> &graph) {
    typename Sawyer::Container::Graph<V, E>::VertexNodeIterator v = graph.findVertex(vertex);
    typename Sawyer::Container::Graph<V, E>::EdgeNodeIterator edge = v->outEdges().begin();
    while (edge != v->outEdges().end()) {
        if (predicate(edge->id())) {
            edge = graph.eraseEdge(edge);
        } else {
            ++edge;
        }
    }
}

template<class Predicate, class V, class E>
void
remove_in_edge_if(typename graph_traits<Sawyer::Container::Graph<V, E> >::vertex_descriptor vertex,
                  Predicate predicate,
                  Sawyer::Container::Graph<V, E> &graph) {
    typename Sawyer::Container::Graph<V, E>::VertexNodeIterator v = graph.findVertex(vertex);
    typename Sawyer::Container::Graph<V, E>::EdgeNodeIterator edge = v->inEdges().begin();
    while (edge != v->inEdges().end()) {
        if (predicate(edge->id())) {
            edge = graph.eraseEdge(edge);
        } else {
            ++edge;
        }
    }
}

template<class V, class E>
typename graph_traits<Sawyer::Container::Graph<V, E> >::vertex_descriptor
add_vertex(Sawyer::Container::Graph<V, E> &graph) {
    return graph.insertVertex(V())->id();
}

template<class V, class E>
void
clear_vertex(typename graph_traits<Sawyer::Container::Graph<V, E> >::vertex_descriptor vertex,
             Sawyer::Container::Graph<V, E> &graph) {
    graph.clearEdges(graph.findVertex(vertex));
}

template<class V, class E>
void
remove_vertex(typename graph_traits<Sawyer::Container::Graph<V, E> >::vertex_descriptor vertex,
              Sawyer::Container::Graph<V, E> &graph) {
    graph.eraseVertex(graph.findVertex(vertex));
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      MutablePropertyGraph concepts
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
template<class V, class E>
std::pair<typename graph_traits<Sawyer::Container::Graph<V, E> >::edge_descriptor, bool>
add_edge(typename graph_traits<Sawyer::Container::Graph<V, E> >::vertex_descriptor source,
         typename graph_traits<Sawyer::Container::Graph<V, E> >::vertex_descriptor target,
         typename graph_traits<Sawyer::Container::Graph<V, E> >::edge_property_type const &pval,
         Sawyer::Container::Graph<V, E> &graph) {
    typename Sawyer::Container::Graph<V, E>::VertexNodeIterator src=graph.findVertex(source), tgt=graph.findVertex(target);
    typename Sawyer::Container::Graph<V, E>::EdgeNodeIterator newEdge = graph.insertEdge(src, tgt, pval);
    return std::make_pair(newEdge->id(), true);
}

template<class V, class E>
typename graph_traits<Sawyer::Container::Graph<V, E> >::vertex_descriptor
add_vertex(const typename graph_traits<Sawyer::Container::Graph<V, E> >::vertex_property_type &pval,
           Sawyer::Container::Graph<V, E> &graph) {
    return graph.insertVertex(pval)->id();
}

} // namespace
#endif
