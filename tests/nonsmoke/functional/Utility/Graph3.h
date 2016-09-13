///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
//                                              DO NOT USE
//
// This file is for demonstration purposes only.  See the README in this directory.  These are stub classes for Sage
// nodes. The declarations will have to be moved into src/ROSETTA before they can become part of ROSE.  The "3" will ultimately
// be dropped from the names.
//
// The main purpose of this file is to describe the Sage Graph Support (SGS) interface, note how it compares with the Boost
// Graph Library (BGL) interface, define the time complexity guarantees, and describe how complexity compares with BGL.
// 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ROSE_Graph3_H
#define ROSE_Graph3_H


/** Virtual base class for all Sage Graph Support (SGS) graphs.
 *
 *  General properties of SGS graphs:
 *  <ol>
 *  <li>A graph is a single, self-contained object of type Graph3 or one of its subclasses.</li>
 *  <li>A Graph3 object behaves like an STL container: it can be allocated in heap, stack or data.</li>
 *  <li>A Sage SgGraph node contains a Graph3 (TBD: contains graph, pointer, or reference?)</li>
 *  <li>SGS graph operations are defined as methods.  This is in contrast to BGL, where operations are defined by overloaded,
 *      global functions.  Defining them as methods is cleaner and results in better doxygen documentation. The SGS method
 *      names are the same as the BGL function names, but don't need the final BGL graph argument.</li>
 *  <li>SGS header files define the necessary BGL types and global functions so that SGS graphs are a drop-in replacement for
 *      similar BGL graphs.</li>
 *  <li>SGS graphs are assignable.  The C code "Graph2 *g1, *g2; ...; *g2=*g1;" results in graph g2 having a copy of the
 *      vertices and edges that were defined in g1.  Pointed-to objects are not copied (the copy is shallow), so if g1 had
 *      vertices or edges that pointed to other Sage nodes (see get_vertex_node() et al), then g2 will point to those same
 *      nodes.  This makes SGS graphs act like STL and BGL.</li>
 *  <li>Vertices of a graph are numbered consecutively starting at zero.  This allows a user (or subclass) to "carry along"
 *      additional vertex information in a vector and have constant-time lookups of that information.  Likewise, edges are
 *      numbered consecutively across the entire graph.  The disadvantage of doing it this way is that single-vertex and
 *      single-edge deletion is more expensive (the corresponding BGL operations are also expensive, just not a much so).<li>
 *  <li>One can iterate over all the edges (and vertices) of any SGS graph.  The iterators are automatically invalidated
 *      when the underlying graph is modified.<li>
 *  <li>SGS graphs are serializable and can be written to and read from a BinaryAST.  The serializing/deserializing
 *      functionality for connectivity (but not the Sage node pointers described below) is implemented within the base class so
 *      as to be useful in situations besides BinaryAST.</li>
 *  <li>Since having vertices that point to other Sage nodes is such a common occurrence (and a lesser extent for edges), all
 *      SGS graphs directly support this concept.  There is no need for users to "carry along" additional vertex- or
 *      edge-to-sage-node lookup tables.  These pointers are automatically serialized to/from the BinaryAST.</li>
 *  <li>For backward compatibility, we preserve the SgGraphNode (sic) and SgGraphEdge Sage node types, and by virtue of the
 *      previous bullet, any SGS graph vertex or edge can point to any of these objects.  These objects are sometimes useful
 *      for having a point to which Sage attributes can be attached and serialized to/from the BinaryAST.</li>
 *  </ol>
 *
 *  SGS graphs have some features that differ from BGL graphs:
 *  <ol>
 *  <li>Vertex and edges are both always identified by unsigned integers from zero to |V|-1 or |E|-1.</li>
 *  <li>When a vertex is removed, the highest numbered vertex is swapped with the vertex just removed. BGL graphs that number
 *      vertices change the names of (by subtracting one from) all the higher vertices.<li>
 *  <li>When an edge is removed, the highest numbered edge is swapped with the edge just removed.  BGL doesn't have an
 *      adjacency_list graph type that numbers edges across the entire graph.</li>
 *  </ol>
 *
 *  SGS graphs have a set of feature that BGL graphs lack: they have have integer edge descriptors, and at the same time, store
 *  a vertex adjacency list.  BGL can done one or the other, but not both at once.  SGS does both at once while still
 *  satisfying the complexity guarantees of both BGL interfaces.
 */
class Graph3 {
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Type Names
    //
    // Note that our type names follow the usual Sage scheme of capitalized camel case for type names.  BGL uses the STL scheme
    // of lower-case, underscore-separated names.  We could add template specializations to boost::graph_traits so that the BGL
    // style names are also available.
    //
    // IMHO, BGL's parameterization of the size types is pointless because one has to know so much about the underlying type to
    // use it safely, that he might as well just use the underlying type directly. (You have to know that it's unsigned, and
    // you have to know what size it will have under various compilers and compiler switches.)
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
protected:
    typedef size_t Descriptor;                                  // we use the same type for all vertex and edge descriptors
    static const Descriptor INVALID_ID = (Descriptor)(-1);      // returned by null_vertex() or null_edge()

public:
    /** Type of vertex names.  Vertices are identified across a graph by unsigned integers ranging from zero (inclusive) to the
     *  number of vertices (exclusive).  Vertex names are not changed when a new vertex is added; the name of the last vertex
     *  may change when a vertex is removed from a graph. Corresponds to boost::graph_traits<G>::vertex_descriptor. */
    typedef Descriptor VertexDescriptor;

    /** Type of edge names.  Edges are indentified across a graph by unsigned integers ranging from zero (inclusive) to the
     *  number of edges (exclusive).  Edge names are not changed when a new edge is added; the name of the last edge may change
     *  when an edge is removed from a graph.  Corresponds to boost::graph_traits<G>::edge_descriptor. */
    typedef Descriptor EdgeDescriptor;

protected:
    typedef std::pair<VertexDescriptor/*source*/, VertexDescriptor/*target*/> Edge;
    typedef std::vector<EdgeDescriptor> EdgeDescriptorList;

private:
    std::vector<Edge> p_edges;                                  // list of all edges indexed by EdgeDescriptor
    std::vector<EdgeDescriptorList> p_out_edges;                // out edges per vertex
    std::vector<SgNode*> p_vertex_nodes, p_edge_nodes;          // links to AST, indexed by vertex or edge descriptor

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Iterators
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    friend class DescriptorIterator;

    enum IteratorFamily {
        ITER_GRAPH_VERTICES,                                    // Iterators over all vertices of a graph.
        ITER_GRAPH_EDGES,                                       // Iterators over all edges of a graph.
        ITER_VERTEX_EDGES,                                      // Iterators over edges incident to a vertex.
        ITER_NFAMILIES                                          // MUST BE LAST
    };

    std::vector<size_t> iterator_serial;

    // Return a serial number unique to the graph/family pair
    size_t get_iterator_serial(IteratorFamily family) const {
        return iterator_serial[family];
    }

    // Invalidate certain types of iterators
    void invalidate_iterators(IteratorFamily family) {
        ++iterator_serial[family];
    }

    class DescriptorIterator {
    private:
        const Graph3 *graph;                                    // graph over which this iterator operates
        IteratorFamily family;                                  // kind of iterator
        size_t serial;                                          // serial number, unique to graph/family pair
        size_t end;                                             // stop when we hit this value
        Descriptor current;                                     // current iterator position
    public:
        DescriptorIterator()
            : graph(NULL), family(ITER_GRAPH_VERTICES), serial((size_t)-1), end(0), current(0) {}
        DescriptorIterator(const Graph3 *graph, IteratorFamily family, Descriptor current, Descriptor end)
            : graph(graph), family(family), serial(graph->get_iterator_serial(family)), end(end), current(current) {}
        bool is_valid() const {
            return graph && serial==graph->get_iterator_serial(family) && current<=end;
        }
        Descriptor operator*() const {
            assert(is_valid());
            return current;
        }
        DescriptorIterator& operator++() {
            assert(is_valid() && current!=end);
            ++current;
            return *this;
        }
        DescriptorIterator operator++(int) {
            assert(is_valid() && current!=end);
            DescriptorIterator i = *this;
            ++current;
            return i;
        }
        bool operator==(const DescriptorIterator &x) const {
            assert(is_valid() && x.is_valid());
            assert(graph==x.graph && family==x.family);
            return current==x.current;
        }
        bool operator!=(const DescriptorIterator &x) const {
            return !(*this==x);
        }
    };
public:

    /** Iterator to traverse all vertices of a graph.  Iterates over vertices in order of their descriptors.  Such an iterator
     *  is valid until a vertex is added to or removed from the graph. */
    class VertexIterator: public DescriptorIterator {
    public:
        VertexIterator(): DescriptorIterator() {}
        VertexIterator(DescriptorIterator i): DescriptorIterator(i) {}
    };

    /** Iterator to traverse all edges of a graph.  Iterates over edges in order of their descriptors.  Such an iterator is
     * valid until an edge is added to or removed from the graph. */
    class EdgeIterator: public DescriptorIterator {
    public:
        EdgeIterator(): DescriptorIterator() {}
        EdgeIterator(DescriptorIterator i): DescriptorIterator(i) {}
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Constructors, etc.
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Default construct an empty graph. */
    Graph3(): iterator_serial(ITER_NFAMILIES, 0) {}

    /** Copy constructor.  The new graph has it's own vertices and edges.  Pointers to SageIII IR nodes in the source graphs
     *  are copied to the new graph, thus the new graph and source graph point to the same IR nodes. */
    explicit Graph3(const Graph3 &g): iterator_serial(ITER_NFAMILIES, 0) {
        init_from_sgs(g);
    };

    /** Clear this graph and reinitialize it from the supplied BGL graph. */
    template<class BoostGraph> void init_from_bgl(const BoostGraph&);

    /** Clear this graph and reinitialize it from another SGS graph. This method's time complexity is linear in the number of
     * vertices and edges. */
    void init_from_sgs(const Graph3&);

    /** Clear this graph and reinitialize it from a serialized SGS graph.  Only vertex and edge connectivity is initialized
     *  since that's the only information emitted by the serialize() method.  In particular, pointers to SageII IR nodes are
     *  not initialized (they will all be null pointers). This method's time complexity is linear in the number of vertices and
     *  edges. */
    void init_from_serial(std::istream&);

    /** Reserve memory.  Reserves memory for the specified number of vertices and edges.  This does not reserve memory for the
     * out-edge adjacency lists since we don't know how out edges will be distributed across the graph. */
    virtual void reserve(size_t nvertices, size_t nedges) {
        p_out_edges.reserve(nvertices);
        p_vertex_nodes.reserve(nvertices);
        p_edges.reserve(nedges);
        p_edge_nodes.reserve(nedges);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  BGL-like, non-modifying accessor methods
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Returns an invalid vertex name.  All vertices within the graph have a name that is different than the name returned by
     *  this class method. */
    static VertexDescriptor null_vertex() {
        return INVALID_ID;
    }

    /** Returns an invalid edge name.  All edges within the graph have a name that is different than the name returned by this
     *  class method. */
    static EdgeDescriptor null_edge() {
        return INVALID_ID;
    }

    /** size_t */
    typedef size_t VerticesSizeType;

    /** Returns the number of vertices in the graph.  Returns in constant time. */
    VerticesSizeType num_vertices() const {
        return p_out_edges.size();
    }

    /** Returns the range of vertex iterators.  Returns a pair of iterators, the begin and end iterators, for all vertices in
     *  the graph.  These iterators, when dereferenced, give a VertexDescriptor. The iterators are valid until a vertex is
     *  added to or removed from the graph. Executes in constant time. */
    std::pair<VertexIterator, VertexIterator> vertices() const {
        return std::make_pair(VertexIterator(DescriptorIterator(this, ITER_GRAPH_VERTICES, 0,              num_vertices())),
                              VertexIterator(DescriptorIterator(this, ITER_GRAPH_VERTICES, num_vertices(), num_vertices())));
    }

    /** Returns the number of out-edges for a vertex.  Returns in constant time. */
    typedef size_t DegreeSizeType;
    DegreeSizeType out_degree(VertexDescriptor v) const {
        assert(vertex_exists(v));
        return p_out_edges[v].size();
    }

    /** Returns the range of iterators for out-edges of a vertex.  Returns a begin and end iterator for the out edges of the
     *  specified vertex.  These iterators, when dereferenced, give an EdgeDescriptor. */
    typedef EdgeDescriptorList::const_iterator OutEdgeIterator;
    std::pair<OutEdgeIterator, OutEdgeIterator> out_edges(VertexDescriptor v) const {
        assert(vertex_exists(v));
        return std::make_pair(p_out_edges[v].begin(), p_out_edges[v].end());
    }

    /** Returns the number of edges in a graph.  Executes in constant time. */
    typedef size_t EdgesSizeType;
    EdgesSizeType num_edges() const {
        return p_edges.size();
    }

    /** Returns the range of edge iterators.  Returns a pair of iterators, the begin and end iterators, for all edges in the
     *  graph.  These iterators, when dereferenced, give an EdgeDescriptor.  The iterators are valid until an edge is added to
     *  or removed from the graph. Executes in constant time. */
    std::pair<EdgeIterator, EdgeIterator> edges() const {
        return std::make_pair(EdgeIterator(DescriptorIterator(this, ITER_GRAPH_EDGES, 0,           num_edges())),
                              EdgeIterator(DescriptorIterator(this, ITER_GRAPH_EDGES, num_edges(), num_edges())));
    }

    /** Returns an edge's originating vertex.  The edge must exist in the graph. Executes in constant time. */
    VertexDescriptor source(EdgeDescriptor e) const {
        assert(edge_exists(e));
        VertexDescriptor v = p_edges[e].first;
        assert(vertex_exists(v));
        return v;
    }

    /** Returns an edge's destination vertex.  The edge must exist in the graph.  Executes in constant time. */
    VertexDescriptor target(EdgeDescriptor e) const {
        assert(edge_exists(e));
        VertexDescriptor v = p_edges[e].second;
        assert(vertex_exists(v));
        return v;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  BGL-like mutator methods
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Adds a new vertex. The returned descriptor of the new vertex is equal to the number of vertices in the graph before the
     *  new vertex was added. Existing vertex iterators become invalid. Executes in amortized constant time. */
    virtual VertexDescriptor add_vertex() {
        invalidate_iterators(ITER_GRAPH_VERTICES);
        invalidate_iterators(ITER_VERTEX_EDGES);
        VertexDescriptor v = num_vertices();
        p_out_edges.push_back(EdgeDescriptorList());
        return v;
    }

    /** Removes a vertex.  The vertex must exist and must not have incident edges (see clear_vertex()).  Execution time is
     *  constant (or maybe linear in the number of incident edges in the last vertex, depending on how std::swap()
     *  operates). */
    virtual void remove_vertex(VertexDescriptor v) {
        invalidate_iterators(ITER_GRAPH_VERTICES);
        invalidate_iterators(ITER_VERTEX_EDGES);
        assert(0==out_degree(v)); // also asserts v exists
        rename_last_vertex(num_vertices()-1, v);
    }

    /** Adds a new edge.  BGL requires that a pair of values be returned, with the second item indicating whether the edge was
     *  added.  Since SGS is able to store parallel edges, edges are always able to be added.  The returned edge descriptor is
     *  equal to the total number of edges in the graph before the new edge was added.  Existing edge iterators become
     *  invalid. Executes in amortized constant time. */
    virtual std::pair<EdgeDescriptor, bool> add_edge(VertexDescriptor source, VertexDescriptor target) {
        invalidate_iterators(ITER_GRAPH_EDGES);
        invalidate_iterators(ITER_VERTEX_EDGES);
        assert(vertex_exists(source) && vertex_exists(target));
        EdgeDescriptor e = num_edges();
        p_edges.push_back(Edge(source, target));
        p_out_edges[source].push_back(e);
        return std::make_pair(e, true);
    }

    /** Removes an edge. The edge must exist in the graph.  Execution time is linear in the number of edges incident to the
     *  source vertex of the edge being removed (i.e., O(|E|/|V|)). */
    virtual void remove_edge(EdgeDescriptor e) {
        invalidate_iterators(ITER_GRAPH_EDGES);
        invalidate_iterators(ITER_VERTEX_EDGES);
        VertexDescriptor src = source(e); // asserts e and src exist
        EdgeDescriptorList::iterator vei = std::find(p_out_edges[src].begin(), p_out_edges[src].end(), e);
        assert(vei!=p_out_edges[src].end());
        p_out_edges[src].erase(vei);
        rename_last_edge(num_edges()-1, e);
    }

    /** Removes all edges incident to two vertices.  Time complexity is O((|E|/|V|)^2).  Subclasses don't usually augment this
     *  method since it defined in terms of remove_edge(EdgeDescriptor). */
    virtual void remove_edge(VertexDescriptor source, VertexDescriptor target) {
        for (EdgeDescriptor e=0; e<p_edges.size(); ++e)
            if (p_edges[e].first==source && p_edges[e].second==target)
                remove_edge(e--);
    }

    /** Removes an edge.  The edge is specified via iterator.  Subclasses don't usually augment this method since it's defined
     *  in terms of remove_edge(EdgeDescriptor).  See remove_edge(EdgeDescriptor) for details. */
    virtual void remove_edge(EdgeIterator ei) {
        remove_edge(*ei);
    }

    /** Removes all edges incident to a vertex. */
    virtual void clear_vertex(VertexDescriptor v) {
        // FIXME: this could be faster by removing all out edges at once
        assert(vertex_exists(v));
        while (out_degree(v)>0)
            remove_edge(p_out_edges[v].front());
    }
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Non-BGL-like, non-modifying accessor methods
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Returns true iff the vertex is valid and exists.  Executes in constant time. */
    virtual bool vertex_exists(VertexDescriptor v) const {
        return v<num_vertices();
    }

    /** Returns true iff the edge is valid and exists. Executes in constant time. */
    virtual bool edge_exists(EdgeDescriptor e) const {
        return e<num_edges();
    }

    /** Returns true iff the graph has no ertices.  A graph having no vertices must necessarily have no edges.  Executes in
     *  constant time. */
    virtual bool empty() const {
        return 0==num_vertices(); // implies 0==num_edges()
    }

    /** Returns a BGL graph.  Constructs a BGL graph of the specified type from this SGS graph. */
    template<class BoostGraph> BoostGraph boost_graph() const;

    /** Serializes graph connectivity information.  The node count and a list of edges with their source and target vertex
     *  numbers is written to the specified stream.  See also, init_from_serial(). */
    void serialize(std::ostream &o) const;
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Non-BGL-like mutator methods
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Clears a graph.  Resets the graph to its initial, empty state.  Executes in constant time. */
    virtual void clear() {
        invalidate_iterators(ITER_GRAPH_VERTICES);
        invalidate_iterators(ITER_GRAPH_EDGES);
        invalidate_iterators(ITER_VERTEX_EDGES);
        p_edges.clear();
        p_out_edges.clear();
        p_vertex_nodes.clear();
        p_edge_nodes.clear();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Links to Sage Nodes
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    /** Returns the SageIII IR node associated with a vertex.  Vertices are born with a null IR pointer. Executes in constant
     *  time. */
    SgNode *get_vertex_node(VertexDescriptor v) const {
        assert(vertex_exists(v));
        return v<p_vertex_nodes.size() ? p_vertex_nodes[v] : NULL;
    }

    /** Returns the SageIII IR node associated with an edge.  Edges are born with a null IR pointer. Executes in constant time. */
    SgNode *get_edge_node(EdgeDescriptor e) const {
        assert(edge_exists(e));
        return e<p_edge_nodes.size() ? p_edge_nodes[e] : NULL;
    }

    /** Sets the SageIII IR node associated with a vertex.  The specified node pointer (possibly null) is associated with the
     *  specified vertex.  The association will exist even if the vertex is renamed due to remove_vertex() being called for
     *  some other vertex.  Executes in amortized constant time. */
    VertexDescriptor set_vertex_node(VertexDescriptor v, SgNode *node) {
        assert(vertex_exists(v));
        if (node) {
            if (v>=p_vertex_nodes.size())
                p_vertex_nodes.resize(v+1, NULL);
            p_vertex_nodes[v] = node;
        } else if (v<p_vertex_nodes.size()) {
            p_vertex_nodes[v] = NULL;
        }
        return v;
    }

    /** Sets the SageIII IR node associated with an edge.  The specified node pointer (possibly null) is associated with the
     *  specified edge.  The association will exist even if the edge is renamed due to remove_edge() being called for some
     *  other edge.  Executes in amortized constant time. */
    EdgeDescriptor set_edge_node(EdgeDescriptor e, SgNode *node) {
        assert(edge_exists(e));
        if (node) {
            if (e>=p_edge_nodes.size())
                p_edge_nodes.resize(e+1, NULL);
            p_edge_nodes[e] = node;
        } else if (e<p_edge_nodes.size()) {
            p_edge_nodes[e] = NULL;
        }
        return e;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Protected support functions
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

    /** Callback for vertex-removing operations.  When a vertex is removed, in order to keep the consecutive vertex numbering
     *  without requiring a linear time complexity rename operation, we simply rename the last existing vertex to fill the hole
     *  left by the vertex that is being removed.  This method is invoked to notify subclasses when a rename is necessary.
     *  Subclasses should augment (not replace) this method. */
    virtual void rename_last_vertex(VertexDescriptor last, VertexDescriptor v) {
        assert(vertex_exists(v) && vertex_exists(last) && last==num_vertices()-1);
        p_out_edges[v] = p_out_edges[last];
        p_out_edges.resize(last);

        if (p_vertex_nodes.size()>last) {
            p_vertex_nodes[v] = p_vertex_nodes[last];
            p_vertex_nodes.resize(last);
        } else if (p_vertex_nodes.size()>v) {
            p_vertex_nodes[v] = NULL;
        }
    }

    /** Callback for edge-removing operations.  When an edge is removed, in order to keep the consecutive edge numbering
     * without requiring a linear time complexity rename operation, we simply rename the last existing edge to fill the hole
     * left by the edge that is being removed.   This method is invoked to notify subclasses when a rename is
     * necessary. Subclasses should augment (not replace) this method. */
    virtual void rename_last_edge(EdgeDescriptor last, EdgeDescriptor e) {
        assert(edge_exists(e) && edge_exists(last) && last==num_edges()-1);
        p_edges[e] = p_edges[last];
        p_edges.resize(last);

        if (e!=last) {
            for (EdgeDescriptorList::iterator ei=p_out_edges[source(e)].begin(); ei!=p_out_edges[source(e)].end(); ++ei) {
                if (*ei==last)
                    *ei = e;
            }
        }

        if (p_edge_nodes.size()>last) {
            p_edge_nodes[e] = p_edge_nodes[last];
            p_edge_nodes.resize(last);
        } else if (p_edge_nodes.size()>e) {
            p_edge_nodes[e] = NULL;
        }
    }
};

/** Like Graph3 but addes efficient access to in-edges. */
class BidirectionalGraph3: public Graph3 {
protected:
    std::vector<EdgeDescriptorList> p_in_edges;                 // out edges per vertex

public:
    BidirectionalGraph3() {}
    explicit BidirectionalGraph3(const Graph3 &g): Graph3(g) {}

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  New methods not found in base class
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    DegreeSizeType in_degree(VertexDescriptor v) const {
        assert(vertex_exists(v));
        return p_in_edges[v].size();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Methods that augment the base class
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Documented in base class. */
    virtual void reserve(size_t nvertices, size_t nedges) {
        Graph3::reserve(nvertices, nedges);
        p_in_edges.reserve(nvertices);
    }

    /** Documented in base class. */
    virtual bool vertex_exists(VertexDescriptor v) const {
        return Graph3::vertex_exists(v) && v<p_in_edges.size();
    }

    /** Documented in base class. */
    virtual VertexDescriptor add_vertex() {
        VertexDescriptor v = Graph3::add_vertex();
        assert(v==p_in_edges.size());
        p_in_edges.push_back(EdgeDescriptorList());
        return v;
    }

    /** Documented in base class. */
    virtual void remove_vertex(VertexDescriptor v) {
        assert(0==in_degree(v));
        Graph3::remove_vertex(v);
    }

    /** Documented in base class. */
    virtual std::pair<EdgeDescriptor, bool> add_edge(VertexDescriptor source, VertexDescriptor target) {
        std::pair<EdgeDescriptor, bool> retval = Graph3::add_edge(source, target);
        if (retval.second)
            p_in_edges[target].push_back(retval.first);
        return retval;
    }

    /** Documented in base class. */
    virtual void remove_edge(EdgeDescriptor e) {
        VertexDescriptor tgt = target(e); // asserts e and tgt exist
        EdgeDescriptorList::iterator vei = std::find(p_in_edges[tgt].begin(), p_in_edges[tgt].end(), e);
        assert(vei!=p_in_edges[tgt].end());
        p_in_edges[tgt].erase(vei);
        Graph3::remove_edge(e);
    }

    /** Documented in base class. */
    virtual void clear() {
        p_in_edges.clear();
        Graph3::clear();
    }

protected:
    /** Documented in base class. */
    virtual void rename_last_vertex(VertexDescriptor last, VertexDescriptor v) {
        Graph3::rename_last_vertex(last, v);
        p_in_edges[v] = p_in_edges[last];
        p_in_edges.resize(last);
    }

    /** Documented in base class. */
    virtual void rename_last_edge(EdgeDescriptor last, EdgeDescriptor e) {
        Graph3::rename_last_edge(last, e);
        if (e!=last) {
            for (EdgeDescriptorList::iterator ei=p_in_edges[target(e)].begin(); ei!=p_in_edges[target(e)].end(); ++ei) {
                if (*ei==last)
                    *ei = e;
            }
        }
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The rest of this header file is to impart a BGL-like interface for Graph3 classes. See BGL documentation for details.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Graph3::VerticesSizeType
num_vertices(const Graph3 &g) {
    return g.num_vertices();
}

std::pair<Graph3::VertexIterator, Graph3::VertexIterator>
vertices(const Graph3 &g)
{
    return g.vertices();
}

Graph3::DegreeSizeType
out_degree(Graph3::VertexDescriptor v, const Graph3 &g)
{
    return g.out_degree(v);
}

std::pair<Graph3::OutEdgeIterator, Graph3::OutEdgeIterator>
out_edges(Graph3::VertexDescriptor v, const Graph3 &g)
{
    return g.out_edges(v);
}

Graph3::EdgesSizeType
num_edges(const Graph3 &g) {
    return g.num_edges();
}

std::pair<Graph3::EdgeIterator, Graph3::EdgeIterator>
edges(const Graph3 &g)
{
    return g.edges();
}

Graph3::VertexDescriptor
source(Graph3::EdgeDescriptor &e, const Graph3 &g)
{
    return g.source(e);
}

Graph3::VertexDescriptor
target(Graph3::EdgeDescriptor &e, const Graph3 &g)
{
    return g.target(e);
}

Graph3::VertexDescriptor
add_vertex(Graph3 &g) {
    return g.add_vertex();
}

void
remove_vertex(Graph3::VertexDescriptor v, Graph3 &g)
{
    g.remove_vertex(v);
}

std::pair<Graph3::EdgeDescriptor, bool>
add_edge(Graph3::VertexDescriptor v1, Graph3::VertexDescriptor v2, Graph3 &g) {
    return g.add_edge(v1, v2);
}

void
remove_edge(Graph3::EdgeDescriptor e, Graph3 &g)
{
    g.remove_edge(e);
}

void
remove_edge(Graph3::VertexDescriptor v1, Graph3::VertexDescriptor v2, Graph3 &g)
{
    g.remove_edge(v1, v2);
}

void
remove_edge(Graph3::EdgeIterator ei, Graph3 &g)
{
    g.remove_edge(ei);
}

void
clear_vertex(Graph3::VertexDescriptor v, Graph3 &g)
{
    g.clear_vertex(v);
}

#include <boost/graph/graph_traits.hpp>

namespace boost {
    template<>
    struct graph_traits<Graph3> {
        typedef Graph3::VertexDescriptor                        vertex_descriptor;
        typedef Graph3::EdgeDescriptor                          edge_descriptor;
        //                                                      adjacency_iterator;
        typedef Graph3::OutEdgeIterator                         out_edge_iterator;
        //                                                      in_edge_iterator;
        typedef Graph3::VertexIterator                          vertex_iterator;
        typedef Graph3::EdgeIterator                            edge_iterator;
        typedef directed_tag                                    directed_category;
        typedef allow_parallel_edge_tag                         edge_parallel_category;
        typedef incidence_graph_tag                             traversal_category; // but we're also edge_list_graph_tag!
        typedef Graph3::VerticesSizeType                        vertices_size_type;
        typedef Graph3::EdgesSizeType                           edges_size_type;
        typedef Graph3::DegreeSizeType                          degree_size_type;

        static Graph3::VertexDescriptor null_vertex() {
            return Graph3::null_vertex();
        }
    };

    template<>
    struct graph_traits<BidirectionalGraph3> {
        typedef BidirectionalGraph3::VertexDescriptor           vertex_descriptor;
        typedef BidirectionalGraph3::EdgeDescriptor             edge_descriptor;
        //                                                      adjacency_iterator;
        typedef BidirectionalGraph3::OutEdgeIterator            out_edge_iterator;
        //                                                      in_edge_iterator;
        typedef BidirectionalGraph3::VertexIterator             vertex_iterator;
        typedef BidirectionalGraph3::EdgeIterator               edge_iterator;
        typedef directed_tag                                    directed_category;
        typedef allow_parallel_edge_tag                         edge_parallel_category;
        typedef incidence_graph_tag                             traversal_category; // but we're also edge_list_graph_tag!
        typedef BidirectionalGraph3::VerticesSizeType           vertices_size_type;
        typedef BidirectionalGraph3::EdgesSizeType              edges_size_type;
        typedef BidirectionalGraph3::DegreeSizeType             degree_size_type;

        static BidirectionalGraph3::VertexDescriptor null_vertex() {
            return BidirectionalGraph3::null_vertex();
        }
    };
}
#endif
