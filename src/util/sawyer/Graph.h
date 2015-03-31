#ifndef Sawyer_Graph_H
#define Sawyer_Graph_H

#include <sawyer/Assert.h>
#include <sawyer/DefaultAllocator.h>
#include <sawyer/IndexedList.h>
#include <sawyer/Optional.h>                            // for Sawyer::Nothing
#include <sawyer/Sawyer.h>
#include <boost/range/iterator_range.hpp>
#include <ostream>
#if 1 /*DEBUGGING [Robb Matzke 2014-04-21]*/
#include <iomanip>
#endif

namespace Sawyer {
namespace Container {

/** Traits for graphs. */
template<class G>
struct GraphTraits {
    typedef typename G::EdgeNodeIterator EdgeNodeIterator;
    typedef typename G::EdgeValueIterator EdgeValueIterator;
    typedef typename G::VertexNodeIterator VertexNodeIterator;
    typedef typename G::VertexValueIterator VertexValueIterator;
    typedef typename G::VertexNode VertexNode;
    typedef typename G::EdgeNode EdgeNode;
    typedef typename G::VertexValue VertexValue;
    typedef typename G::EdgeValue EdgeValue;
};

template<class G>
struct GraphTraits<const G> {
    typedef typename G::ConstEdgeNodeIterator EdgeNodeIterator;
    typedef typename G::ConstEdgeValueIterator EdgeValueIterator;
    typedef typename G::ConstVertexNodeIterator VertexNodeIterator;
    typedef typename G::ConstVertexValueIterator VertexValueIterator;
    typedef const typename G::VertexNode VertexNode;
    typedef const typename G::EdgeNode EdgeNode;
    typedef const typename G::VertexValue VertexValue;
    typedef const typename G::EdgeValue EdgeValue;
};

/** %Graph containing user-defined vertices and edges.
 *
 * @section nodes Vertices and Edges
 *
 *  This container stores user-defined data at each vertex and edge, along with information about the connectivity of vertices
 *  through edges.  Semantics with respect to storing of user-defined data is similar to the STL's <code>std::list</code> type;
 *  namely, user values are copied into the container when they are inserted, and not copied thereafter.  Accessors return
 *  references to those values.  Edges are always directed and have source and target vertices. Self edges (an edge whose
 *  source and target vertex are the same) and parallel edges (two edges both having the same source vertex and both having the
 *  same target vertex) are supported.
 *
 *  Here's an example of declaring a graph that stores a string name for each vertex and a floating point weight for each edge:
 *
 * @code
 *  typedef Sawyer::Container::Graph<std::string, double> MyGraph;
 *  MyGraph graph;
 *  MyGraph::VertexNodeIterator v1 = graph.insertVertex("first vertex");
 *  MyGraph::VertexNodeIterator v2 = graph.insertVertex("second vertex");
 *  graph.insertEdge(v1, v2, 1.2); // v1 and v2 are the source and target vertices
 * @endcode
 *
 *  The term "Node" in the above iterators refers to the unit of storage for a vertex, which not only contains the user-defined
 *  value for the vertex, but also an ID number and connectivity information. Within this documentation, the term "vertex" is
 *  always used as the name of a graph component (i.e., a graph has vertices and edges), and the term "node" always refers to a
 *  unit of storage.
 *
 *  A graph doesn't necessarily need to store data at each vertex or edge. The vertex and node types default to @ref Nothing,
 *  which is similar to @c void.
 *
 * @section iterators Iterators
 *
 *  Vertices and edges are referenced via iterators, which are stable across insertion and erasure.  That is, an iterator will
 *  continue to point to the same vertex or edge even when other vertices or edges are added or removed from the graph.
 *  Iterators are the preferred mechanism for referring to a vertex or edge, and are lightweight objects.  Iterators, as their
 *  name suggests, are also used for iterating over a list of vertices or edges, and "end" iterators indicate the list
 *  termination--they point to one-past-the-end of the list. End iterators are generally specific to each list, so any two end
 *  iterators from two different lists will typically compare as unequal.
 *
 *  Iterators can refer to just the user-defined value, or the entire storage node. A storage node contains the user-defined
 *  value, an ID number, and graph connectivity information and can be implicitly converted to a value iterator.  Orthogonally,
 *  iterator's referent can be constant or mutable.  Vertex iterator names are:
 *
 *  @li @ref VertexValueIterator refers to user-defined mutable values
 *  @li @ref ConstVertexValueIterator refers to user-defined constant values
 *  @li @ref VertexNodeIterator refers to mutable vertex storage nodes
 *  @li @ref ConstVertexNodeIterator refers to constant vertex storage nodes
 *
 *  Edge iterators are similar.
 *
 *  The previous example (using vertex iterators to refer to newly-inserted vertices) should make more sense now.  Here's an
 *  example using iterators to actually iterate over something:
 *
 * @code
 *  std::cout <<"Vertex names:\n";
 *  for (MyGraph::ConstVertexNodeIterator vertex=graph.vertices().begin(); vertex!=graph.vertices().end(); ++vertex)
 *      std::cout <<"  " << vertex->value() <<"\n";
 * @endcode
 *
 *  The graph maintains a graph-wide list of vertices and edges, iterators to which are returned by the @ref vertices and @ref
 *  edges methods.  The @ref vertexValues and @ref edgeValues methods are related, but return iterators which, when
 *  dereferenced, return a reference to the user-defined value for that vertex or edge.  The "value" iterators are
 *  equality-comparable (<code>==</code> and <code>!=</code>) with their "node" iterator counterparts and implicitly
 *  constructed from them, but are unable to return information about vertex and edge connectivity (only user-defined values).
 *
 *  Here's a couple easier ways to do the same thing as the previous example:
 *
 * @code
 *  BOOST_FOREACH (const MyGraph::VertexNode &vertex, graph.vertices())
 *      std::cout <<"  " <<vertex.value() <<"\n";
 * @endcode
 *
 * @code
 *  BOOST_FOREACH (const std::string &name, graph.vertexValues())
 *      std::cout <<"  " <<name <<"\n";
 * @endcode
 *
 *  %Sawyer also defines numerous graph traversals that can traverse vertices or edges in certain orders by following the graph
 *  connectivity. See @ref Sawyer::Container::Algorithm::GraphTraversal.
 *
 * @section ids Identification Numbers
 *
 *  Vertices and edges are also given small, consecutive ID numbers beginning at zero--one set for vertices and another set for
 *  edges.  ID numbers are stable across insertion but not erasure. That is, if an edge is erased from the container then the
 *  ID numbers for other edges in the same container may change. Similarly for vertices.  An ID number can be converted to an
 *  iterator in constant time, and vice versa.  Inserting or erasing a vertex or edge is a constant-time operation.
 *
 *  Here's an example that lists all the edges in order of edge ID:
 *
 * @code
 *  for (size_t edgeId=0; edgeId<graph.nEdges(); ++edgeId) {
 *      MyGraph::ConstEdgeNodeIterator edge = graph.findEdge(edgeId);
 *      std::cout <<"Edge " <<edgeId
 *                <<" from vertex " <<edge->source()->id()
 *                <<" to vertex " <<edge->target()->id() <<"\n";
 *  }
 * @endcode
 *
 *  One very useful side effect of having small, consecutive identification numbers is that they can be used as constant time
 *  indexing into auxiliary tables.  For instance, here's how one might construct a table that contains hash values for all the
 *  vertex names:
 *
 * @code
 *  std::vector<unsigned long> vertexHashes(graph.nVertices());
 *  BOOST_FOREACH (const MyGraph::VertexNode &vertex, graph.vertices())
 *      vertexHashes[vertex.id()] = hash(vertex.value());
 * @endcode
 *
 * @section connectivity Graph connectivity
 *
 *  Each vertex has two additional edge lists: a list of incoming edges where this vertex serves as the edges' target, and a
 *  list of outgoing edges where this vertex serves as the edges' source.  The lists are returned by the @ref
 *  VertexNode::inEdges and @ref VertexNode::outEdges methods.  These lists are sublists of the graph-wide edge list and
 *  iterators are equality-comparable and return references to the same underlying edges.  However, the "end" iterators for
 *  these sublists are all distinct from one another and distinct from the graph-wide edge list. (footnote: Actually, the "end"
 *  iterators for the in-coming and out-going lists of a single vertex are equal to each other, but don't depend on this.)
 *
 *  Each edge has two methods, @ref EdgeNode::source and @ref EdgeNode::target that return iterators to the source and target
 *  vertices for that edge.
 *
 *  Here's an example similar to the previous edge ID iteration except it presents the graph in terms of vertices:
 *
 * @code
 *  BOOST_FOREACH (const MyGraph::VertexNode &vertex, graph.vertices()) {
 *      std::cout <<"vertex " <<vertex.id() <<"\n";
 *      BOOST_FOREACH (const MyGraph::EdgeNode &edge, vertex.outEdges()) {
 *          std::cout <<"  edge " <<edge.id() <<" to vertex " <<edge.target()->id() <<"\n";
 *      }
 *  }
 * @endcode
 *
 * @section bgl BGL Compatibility
 *
 *  The %Boost %Graph Library (<a href="http://www.boost.org/doc/libs/1_55_0/libs/graph/doc/index.html">BGL</a>) defines an API
 *  suitable for operating on a wide variety of graph implementations when the appropriate graph and property traits are
 *  defined.  In order to operate on a %Sawyer graph using the BGL API, the GraphBoost.h header file should be
 *  included.  See the @ref Sawyer::Boost name space for details.
 *
 *  The main philosophical difference between %Sawyer graphs and %Boost Graphs is how internal and external properties are
 *  stored. %Sawyer stores internal properties as user-defined value members within the vertex and edge storage nodes, and uses
 *  the small, contiguous vertex and edge ID numbers to look up vector-stored external properties in constant time. BGL graphs
 *  abstract internal and external properties to property maps (property maps are a separate part of the %Boost library but
 *  originated as part of BGL). The %Sawyer approach tends to be easier for users to understand because of its similarity to
 *  STL containers.
 *
 *  The <a
 *  href="https://github.com/matzke1/sawyer/blob/master/tests/Container/graphBoost.C"><tt>tests/Container/graphBoost.C</tt></a>
 *  file in the %Sawyer source tree exemplifies the differences between the %Sawyer and BGL approaches and gives examples of
 *  using the BGL API on %Sawyer graphs.
 *
 * @section allocators Custom allocators
 *
 *  Because a graph allocates memory in terms of vertex and edge nodes, and because these nodes can be quite small, a graph can
 *  often benefit by using a memory pool allocation scheme.  The third template argument provides the type for the allocator,
 *  and the graph constructors take an allocator argument which is copied into the graph.  The allocator must implement the
 *  @ref Sawyer::DefaultAllocator API (essentially an allocate and a deallocate method), which happens to use the normal C++
 *  global <code>new</code> and <code>delete</code> allocators.  Another possibility is @ref Sawyer::PoolAllocator.
 *
 * @code
 *  typedef Sawyer::Container::Graph<std::string, double, Sawyer::PoolAllocator> MyGraphFast;
 *  MyGraphFast graph; //uses a default-constructed pool allocator
 * @endcode
 *
 *  Here's a mechanism by which the same pool can be used by multiple graphs. A proxy is needed because allocators are copied
 *  by value, but we want all the graphs to share the same pool:
 *
 * @code
 *  Sawyer::PoolAllocator pool;
 *  typedef Sawyer::ProxyAllocator<Sawyer::PoolAllocator> PoolProxy;
 *  MyGraphFast g1(PoolProxy(pool));
 *  MyGraphFast g2(PoolProxy(pool));
 * @endcode
 *
 * @section complexity Complexity guarantees
 *
 *  Time complexity guarantees:
 *
 *  @li Vertex insertion:  amortized constant
 *  @li Edge insertion: amortized constant
 *  @li Vertex iterator dereference: constant
 *  @li Edge iterator dereference: constant
 *  @li Vertex erasure: O(|Ev|), where |Ev| is the number of edges incident to the vertex.
 *  @li Edge erasure: constant
 *  @li Vertex lookup by ID: constant
 *  @li Edge lookup by ID: constant
 *  @li %Graph vertex list: constant
 *  @li %Graph edge list: constant
 *  @li Vertex in/out edge list: constant
 *  @li Count graph vertices: constant
 *  @li Count graph edges: constant
 *  @li Count in/out edges: constant
 *  @li %Graph deletion: O(|V|+|E|)
 *  @li %Graph copy: O(|V|+|E|)
 *
 *  Insertion is amortized constant time due to a vector-based ID map that may require reallocation. */
template<class V = Nothing, class E = Nothing, class Alloc = DefaultAllocator>
class Graph {
public:
    typedef V VertexValue;                              /**< User-level data associated with vertices. */
    typedef E EdgeValue;                                /**< User-level data associated with edges. */
    typedef Alloc Allocator;                            /**< Allocator for vertex and edge nodes. */
    class VertexNode;                                   /**< All information about a vertex. User info plus connectivity info. */
    class EdgeNode;                                     /**< All information about an edge. User info plus connectivity info. */

private:
    enum EdgePhase { IN_EDGES=0, OUT_EDGES=1, N_PHASES=2 };
    typedef IndexedList<EdgeNode, Allocator> EdgeList;
    typedef IndexedList<VertexNode, Allocator> VertexList;

    template<class T>
    class VirtualList {
        VirtualList *next_[N_PHASES];
        VirtualList *prev_[N_PHASES];
        T *node_;
    public:
        VirtualList() {
            reset(NULL);
        }

        void reset(T* node) {
            node_ = node;
            for (size_t i=0; i<N_PHASES; ++i)
                next_[i] = prev_[i] = this;
        }

        bool isHead() const {
            return node_ == NULL;
        }

        bool isSingleton(EdgePhase phase) const {
            ASSERT_this();
            ASSERT_require(phase < N_PHASES);
            ASSERT_require((next_[phase]==this && prev_[phase]==this) || (next_[phase]!=this && prev_[phase]!=this));
            return next_[phase]==this;
        }

        bool isEmpty(EdgePhase phase) const {
            ASSERT_this();
            ASSERT_require(isHead());
            ASSERT_require((next_[phase]==this && prev_[phase]==this) || (next_[phase]!=this && prev_[phase]!=this));
            return next_[phase]==this;
        }

        void insert(EdgePhase phase, VirtualList *newNode) { // insert newNode before this
            ASSERT_this();
            ASSERT_require(phase < N_PHASES);
            ASSERT_not_null(newNode);
            ASSERT_forbid(newNode->isHead());
            ASSERT_require(newNode->isSingleton(phase)); // cannot be part of another sublist already
            prev_[phase]->next_[phase] = newNode;
            newNode->prev_[phase] = prev_[phase];
            prev_[phase] = newNode;
            newNode->next_[phase] = this;
        }

        void remove(EdgePhase phase) {                  // Remove this node from the list
            ASSERT_this();
            ASSERT_require(phase < N_PHASES);
            ASSERT_forbid(isHead());
            prev_[phase]->next_[phase] = next_[phase];
            next_[phase]->prev_[phase] = prev_[phase];
            next_[phase] = prev_[phase] = this;
        }

        VirtualList& next(EdgePhase phase) { return *next_[phase]; }
        const VirtualList& next(EdgePhase phase) const { return *next_[phase]; }
        VirtualList& prev(EdgePhase phase) { return *prev_[phase]; }
        const VirtualList& prev(EdgePhase phase) const { return *prev_[phase]; }

        T& dereference() {                              // Return the EdgeNode to which this VirtualList node belongs
            ASSERT_this();
            ASSERT_forbid(isHead());                    // list head contains no user-data
            return *(T*)this;                           // depends on VirtualList being at the beginning of EdgeNode
        }

        const T& dereference() const {
            ASSERT_this();
            ASSERT_forbid(isHead());
            return *(const T*)this;
        }

#if 1 /*DEBUGGING [Robb Matzke 2014-04-21]*/
        void dump(EdgePhase phase, std::ostream &o) const {
            const VirtualList *cur = this;
            o <<"  " <<std::setw(18) <<"Node"
              <<"\t" <<std::setw(18) <<"This"
              <<"\t" <<std::setw(18) <<"Next"
              <<"\t" <<std::setw(18) <<"Prev\n";
            do {
                o <<"  " <<std::setw(18) <<node_
                  <<"\t" <<std::setw(18) <<cur
                  <<"\t" <<std::setw(18) <<cur->next_[phase]
                  <<"\t" <<std::setw(18) <<cur->prev_[phase] <<"\n";
                cur = cur->next_[phase];
            } while (cur!=this && cur->next_[phase]!=cur);
        }
#endif
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Iterators
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:                                                 // public only for the sake of doxygen
    /** Base class for edge iterators. This isn't normally used directly by users except for its public methods inherited by
     *  subclasses. */
    template<class Derived, class Value, class Node, class BaseIter, class VList>
    class EdgeBaseIterator: public std::iterator<std::bidirectional_iterator_tag, Value> {
        EdgePhase phase_;                               // IN_EDGES, OUT_EDGES or N_PHASES (graph edges)
        BaseIter iter_;                                 // EdgeList::NodeIterator or EdgeList::ConstNodeIterator
        VList *vlist_;                                  // (const) VirtualList<EdgeNode> when phase_ is IN_EDGES or OUT_EDGES
    protected:
        friend class Graph;
        EdgeBaseIterator() {}
        EdgeBaseIterator(const EdgeBaseIterator &other): phase_(other.phase_), iter_(other.iter_), vlist_(other.vlist_) {}
        EdgeBaseIterator(const BaseIter &iter): phase_(N_PHASES), iter_(iter), vlist_(NULL) {}
        EdgeBaseIterator(EdgePhase phase, VList *vlist): phase_(phase), vlist_(vlist) {}
        template<class BaseIter2> EdgeBaseIterator(EdgePhase phase, const BaseIter2 &iter, VList *vlist)
            : phase_(phase), iter_(iter), vlist_(vlist) {}

        Node& dereference() const {
            return N_PHASES==phase_ ? iter_->value() : vlist_->dereference();
        }

    private:
        Derived* derived() { return static_cast<Derived*>(this); }
        const Derived* derived() const { return static_cast<const Derived*>(this); }

    public:
        /** Assignment. */
        Derived& operator=(const Derived &other) {
            phase_ = other.phase_;
            iter_ = other.iter_;
            vlist_ = other.vlist_;
            return *derived();
        }

        /** Increment.
         *
         *  Causes this iterator to advance to the next node of the edge list. This method should not be invoked on an
         *  iterator that points to the end of the list.
         *
         * @{ */
        Derived& operator++() {
            if (N_PHASES==phase_) {
                ++iter_;
            } else {
                vlist_ = &vlist_->next(phase_);
            }
            return *derived();
        }
        Derived operator++(int) {
            Derived old = *this;
            ++*this;
            return old;
        }
        /** @} */

        /** Decrement.
         *
         *  Causes this iterator to advance to the previous node of the edge list. This method should not be invoked on an
         *  iterator that points to the beginning of the list.
         *
         * @{ */
        Derived& operator--() {
            if (N_PHASES==phase_) {
                --iter_;
            } else {
                vlist_ = &vlist_->prev(phase_);
            }
            return *derived();
        }
        Derived operator--(int) {
            Derived old = *this;
            --*this;
            return old;
        }
        /** @} */

        /** Equality predicate.
         *
         *  Two iterators are equal if they point to the same node of an edge list, and unequal otherwise.  End iterators point
         *  to a virtual node one past the end of the list and are unique to each list or sublist.  One minor exception to this
         *  rule is that end iterators for the in-edge list and out-edge list of the same vertex are considered to be equal.
         *
         * @{ */
        template<class OtherIter>
        bool operator==(const OtherIter &other) const {
            Node *a = NULL;
            if (N_PHASES==phase_) {
                a = iter_.isAtEnd() ? NULL : &iter_->value();
            } else {
                a = vlist_->isHead() ? NULL : &vlist_->dereference();
            }
            Node *b = NULL;
            if (N_PHASES==other.phase_) {
                b = other.iter_.isAtEnd() ? NULL : &other.iter_->value();
            } else {
                b = other.vlist_->isHead() ? NULL : &other.vlist_->dereference();
            }
            return a == b;
        }
        template<class OtherIter>
        bool operator!=(const OtherIter &other) const {
            return !(*this==other);
        }
        /** @} */

        /** Iterator comparison. */
        bool operator<(const EdgeBaseIterator &other) const {
            Node *a = NULL;
            if (N_PHASES==phase_) {
                a = iter_.isAtEnd() ? NULL : &iter_->value();
            } else {
                a = vlist_->isHead() ? NULL : &vlist_->dereference();
            }
            Node *b = NULL;
            if (N_PHASES==other.phase_) {
                b = other.iter_.isAtEnd() ? NULL : &other.iter_->value();
            } else {
                b = other.vlist_->isHead() ? NULL : &other.vlist_->dereference();
            }
            return a < b;
        }
    };

    /** Base class for vertex iterators. */
    template<class Derived, class Value, class Node, class BaseIter>
    class VertexBaseIterator: public std::iterator<std::bidirectional_iterator_tag, Value> {
        BaseIter base_;                                 // VertexList::NodeIterator or VertexList::ConstNodeIterator
    protected:
        friend class Graph;
        VertexBaseIterator() {}
        VertexBaseIterator(const VertexBaseIterator &other): base_(other.base_) {}
        VertexBaseIterator(const BaseIter &base): base_(base) {}
        Node& dereference() const { return base_->value(); }
    public:
        /** Assignment. */
        Derived& operator=(const Derived &other) { base_ = other.base_; return *derived(); }

        /** Increment.
         *
         *  Causes this iterator to advance to the next node of the vertex list. This method should not be invoked on an
         *  iterator that points to the end of the list.
         *
         * @{ */
        Derived& operator++() { ++base_; return *derived(); }
        Derived operator++(int) { Derived old=*derived(); ++*this; return old; }
        /** @} */

        /** Decrement.
         *
         *  Causes this iterator to advance to the previous node of the vertex list. This method should not be invoked on an
         *  iterator that points to the beginning of the list.
         *
         * @{ */
        Derived& operator--() { --base_; return *derived(); }
        Derived operator--(int) { Derived old=*derived(); --*this; return old; }
        /** @} */

        /** Equality predicate.
         *
         *  Two iterators are equal if they point to the same node of the vertex list, and unequal otherwise.
         *
         * @{ */
        template<class OtherIter> bool operator==(const OtherIter &other) const { return base_ == other.base_; }
        template<class OtherIter> bool operator!=(const OtherIter &other) const { return base_ != other.base_; }
        /** @} */

        /** Iterator comparison. */
        bool operator<(const VertexBaseIterator &other) const { return base_ < other.base_; }

    private:
        Derived* derived() { return static_cast<Derived*>(this); }
        const Derived* derived() const { return static_cast<const Derived*>(this); }
    };

public:
    /** Bidirectional edge node iterator.
     *
     *  Iterates over the edge nodes in a list, returning the node (type @ref EdgeNode) when dereferenced.  Edge iterators are
     *  stable across insert and erase operations.  The difference between @ref EdgeNodeIterator and @ref ConstEdgeNodeIterator
     *  is that the latter returns const references when dereferenced.  An EdgeNodeIterator can be impliciatly converted to a
     *  @ref ConstEdgeNodeIterator, @ref EdgeValueIterator, or @ref ConstEdgeValueIterator. */
    class EdgeNodeIterator: public EdgeBaseIterator<EdgeNodeIterator, EdgeNode, EdgeNode, typename EdgeList::NodeIterator,
                                                    VirtualList<EdgeNode> > {
        typedef                    EdgeBaseIterator<EdgeNodeIterator, EdgeNode, EdgeNode, typename EdgeList::NodeIterator,
                                                    VirtualList<EdgeNode> > Super;
    public:
        typedef EdgeNode& Reference;
        typedef EdgeNode* Pointer;
        EdgeNodeIterator() {}
        EdgeNodeIterator(const EdgeNodeIterator &other): Super(other) {}
        EdgeNode& operator*() const { return this->dereference(); }
        EdgeNode* operator->() const { return &this->dereference(); }
    private:
        friend class Graph;
        EdgeNodeIterator(const typename EdgeList::NodeIterator &base): Super(base) {}
        EdgeNodeIterator(EdgePhase phase, VirtualList<EdgeNode> *vlist): Super(phase, vlist) {}
    };

    /** Bidirectional edge node iterator.
     *
     *  Iterates over the edge nodes in a list, returning the node (type @ref EdgeNode) when dereferenced.  Edge iterators are
     *  stable across insert and erase operations.  The difference between @ref EdgeNodeIterator and @ref ConstEdgeNodeIterator
     *  is that the latter returns const references when dereferenced.  A ConstEdgeNodeIterator can be implicitly converted to
     *  a @ref ConstEdgeValueIterator. */
    class ConstEdgeNodeIterator: public EdgeBaseIterator<ConstEdgeNodeIterator, const EdgeNode, const EdgeNode,
                                                         typename EdgeList::ConstNodeIterator,
                                                         const VirtualList<EdgeNode> > {
        typedef                         EdgeBaseIterator<ConstEdgeNodeIterator, const EdgeNode, const EdgeNode,
                                                         typename EdgeList::ConstNodeIterator,
                                                         const VirtualList<EdgeNode> > Super;
    public:
        typedef const EdgeNode& Reference;
        typedef const EdgeNode* Pointer;
        ConstEdgeNodeIterator() {}
        ConstEdgeNodeIterator(const ConstEdgeNodeIterator &other): Super(other) {}
        ConstEdgeNodeIterator(const EdgeNodeIterator &other): Super(other.phase_, other.iter_, other.vlist_) {}
        const EdgeNode& operator*() const { return this->dereference(); }
        const EdgeNode* operator->() const { return &this->dereference(); }
    private:
        friend class Graph;
        ConstEdgeNodeIterator(const typename EdgeList::ConstNodeIterator &base): Super(base) {}
        ConstEdgeNodeIterator(EdgePhase phase, const VirtualList<EdgeNode> *vlist): Super(phase, vlist) {}
    };
    /** @} */

    /** Bidirectional edge value iterator.
     *
     *  Iterates over the edge values in a list, returning the user-defined value (type @ref EdgeValue) when dereferenced.
     *  Edge iterators are stable across insert and erase operations.  The difference between @ref EdgeValueIterator and @ref
     *  ConstEdgeValueIterator is that the latter returns const references when dereferenced.  An EdgeValueIterator can be
     *  impliciatly converted to a @ref ConstEdgeValueIterator. */
    class EdgeValueIterator: public EdgeBaseIterator<EdgeValueIterator, EdgeValue, EdgeNode, typename EdgeList::NodeIterator,
                                                     VirtualList<EdgeNode> > {
        typedef                     EdgeBaseIterator<EdgeValueIterator, EdgeValue, EdgeNode, typename EdgeList::NodeIterator,
                                                     VirtualList<EdgeNode> > Super;
    public:
        typedef EdgeValue& Reference;
        typedef EdgeValue* Pointer;
        EdgeValueIterator() {}
        EdgeValueIterator(const EdgeValueIterator &other): Super(other) {}
        EdgeValueIterator(const EdgeNodeIterator &other): Super(other.phase_, other.iter_, other.vlist_) {}
        EdgeValue& operator*() const { return this->dereference().value(); }
        EdgeValue* operator->() const { return &this->dereference().value(); }
    private:
        friend class Graph;
        EdgeValueIterator(const typename EdgeList::NodeIterator &base): Super(base) {}
        EdgeValueIterator(EdgePhase phase, VirtualList<EdgeNode> *vlist): Super(phase, vlist) {}
    };

    /** Bidirectional edge value iterator.
     *
     *  Iterates over the edge values in a list, returning the user-defined value (type @ref EdgeValue) when dereferenced.
     *  Edge iterators are stable across insert and erase operations.  The difference between @ref EdgeValueIterator and @ref
     *  ConstEdgeValueIterator is that the latter returns const references when dereferenced. */
    class ConstEdgeValueIterator: public EdgeBaseIterator<ConstEdgeValueIterator, const EdgeValue, const EdgeNode,
                                                          typename EdgeList::ConstNodeIterator,
                                                          const VirtualList<EdgeNode> > {
        typedef                          EdgeBaseIterator<ConstEdgeValueIterator, const EdgeValue, const EdgeNode,
                                                          typename EdgeList::ConstNodeIterator,
                                                          const VirtualList<EdgeNode> > Super;
    public:
        typedef const EdgeValue& Reference;
        typedef const EdgeValue* Pointer;
        ConstEdgeValueIterator() {}
        ConstEdgeValueIterator(const ConstEdgeValueIterator &other): Super(other) {}
        ConstEdgeValueIterator(const EdgeValueIterator &other): Super(other.phase_, other.iter_, other.vlist_) {}
        ConstEdgeValueIterator(const EdgeNodeIterator &other): Super(other.phase_, other.iter_, other.vlist_) {}
        ConstEdgeValueIterator(const ConstEdgeNodeIterator &other): Super(other.phase_, other.iter_, other.vlist_) {}
        const EdgeValue& operator*() const { return this->dereference().value(); }
        const EdgeValue* operator->() const { return &this->dereference().value(); }
    private:
        friend class Graph;
        ConstEdgeValueIterator(const typename EdgeList::ConstNodeIterator &base): Super(base) {}
        ConstEdgeValueIterator(EdgePhase phase, const VirtualList<EdgeNode> *vlist): Super(phase, vlist) {}
    };

    /** Bidirectional vertex node iterator.
     *
     *  Iterates over the vertex nodes in a list, returning the node (type @ref VertexNode) when dereferenced.  Vertex
     *  iterators are stable across insert and erase operations.  The difference between @ref VertexNodeIterator and @ref
     *  ConstVertexNodeIterator is that the latter returns const references when dereferenced.  A VertexNodeIterator can be
     *  impliciatly converted to a @ref ConstVertexNodeIterator, @ref VertexValueIterator, or @ref ConstVertexValueIterator. */
    class VertexNodeIterator: public VertexBaseIterator<VertexNodeIterator, VertexNode, VertexNode,
                                                        typename VertexList::NodeIterator> {
        typedef                      VertexBaseIterator<VertexNodeIterator, VertexNode, VertexNode,
                                                        typename VertexList::NodeIterator> Super;
    public:
        typedef VertexNode& Reference;
        typedef VertexNode* Pointer;
        VertexNodeIterator() {}
        VertexNodeIterator(const VertexNodeIterator &other): Super(other) {}
        VertexNode& operator*() const { return this->dereference(); }
        VertexNode* operator->() const { return &this->dereference(); }
    private:
        friend class Graph;
        VertexNodeIterator(const typename VertexList::NodeIterator &base): Super(base) {}
    };

    /** Bidirectional vertex node iterator.
     *
     *  Iterates over the vertex nodes in a list, returning the node (type @ref VertexNode) when dereferenced.  Vertex
     *  iterators are stable across insert and erase operations.  The difference between @ref VertexNodeIterator and @ref
     *  ConstVertexNodeIterator is that the latter returns const references when dereferenced. */
    class ConstVertexNodeIterator: public VertexBaseIterator<ConstVertexNodeIterator, const VertexNode, const VertexNode,
                                                             typename VertexList::ConstNodeIterator> {
        typedef                           VertexBaseIterator<ConstVertexNodeIterator, const VertexNode, const VertexNode,
                                                             typename VertexList::ConstNodeIterator> Super;
    public:
        typedef const VertexNode& Reference;
        typedef const VertexNode* Pointer;
        ConstVertexNodeIterator() {}
        ConstVertexNodeIterator(const ConstVertexNodeIterator &other): Super(other) {}
        ConstVertexNodeIterator(const VertexNodeIterator &other): Super(other.base_) {}
        const VertexNode& operator*() const { return this->dereference(); }
        const VertexNode* operator->() const { return &this->dereference(); }
    private:
        friend class Graph;
        ConstVertexNodeIterator(const typename VertexList::ConstNodeIterator &base): Super(base) {}
    };
        
    /** Bidirectional vertex value iterator.
     *
     *  Iterates over the vertex values in a list, returning the user-defined value (type @ref VertexValue) when dereferenced.
     *  Vertex iterators are stable across insert and erase operations.  The difference between @ref VertexValueIterator and
     *  @ref ConstVertexValueIterator is that the latter returns const references when dereferenced.  A VertexValueIterator can
     *  be impliciatly converted to a @ref ConstVertexValueIterator. */
    class VertexValueIterator: public VertexBaseIterator<VertexValueIterator, VertexValue, VertexNode,
                                                         typename VertexList::NodeIterator> {
        typedef                       VertexBaseIterator<VertexValueIterator, VertexValue, VertexNode,
                                                         typename VertexList::NodeIterator> Super;
    public:
        typedef VertexValue& Reference;
        typedef VertexValue* Pointer;
        VertexValueIterator() {}
        VertexValueIterator(const VertexValueIterator &other): Super(other) {}
        VertexValueIterator(const VertexNodeIterator &other): Super(other.base_) {}
        VertexValue& operator*() const { return this->dereference().value(); }
        VertexValue* operator->() const { return &this->dereference().value(); }
    private:
        friend class Graph;
        VertexValueIterator(const typename VertexList::NodeIterator &base): Super(base) {}
    };

    /** Bidirectional vertex value iterator.
     *
     *  Iterates over the vertex values in a list, returning the user-defined value (type @ref VertexValue) when dereferenced.
     *  Vertex iterators are stable across insert and erase operations.  The difference between @ref VertexValueIterator and
     *  @ref ConstVertexValueIterator is that the latter returns const references when dereferenced. */
    class ConstVertexValueIterator: public VertexBaseIterator<ConstVertexValueIterator, const VertexValue, const VertexNode,
                                                              typename VertexList::ConstNodeIterator> {
        typedef                            VertexBaseIterator<ConstVertexValueIterator, const VertexValue, const VertexNode,
                                                              typename VertexList::ConstNodeIterator> Super;
    public:
        typedef const VertexValue& Reference;
        typedef const VertexValue* Pointer;
        ConstVertexValueIterator() {}
        ConstVertexValueIterator(const ConstVertexValueIterator &other): Super(other) {}
        ConstVertexValueIterator(const VertexValueIterator &other): Super(other.base_) {}
        ConstVertexValueIterator(const VertexNodeIterator &other): Super(other.base_) {}
        ConstVertexValueIterator(const ConstVertexNodeIterator &other): Super(other.base_) {}
        const VertexValue& operator*() const { return this->dereference().value(); }
        const VertexValue* operator->() const { return &this->dereference().value(); }
    private:
        friend class Graph;
        ConstVertexValueIterator(const typename VertexList::ConstNodeIterator &base): Super(base) {}
    };


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Storage nodes
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    /** Edge node.
     *
     *  These list nodes contain all information about an edge and are the objects returned (by reference) when an edge node
     *  iterator (@ref EdgeNodeIterator or @ref ConstEdgeNodeIterator) is dereferenced. */
    class EdgeNode {
        VirtualList<EdgeNode> edgeLists_;               // links for in- and out-edge sublists; MUST BE FIRST
        EdgeValue value_;                               // user-defined data for each edge
        typename EdgeList::NodeIterator self_;          // always points to itself so we can get to IndexedList::Node
        VertexNodeIterator source_, target_;            // starting and ending points of the edge are always required
    private:
        friend class Graph;
        EdgeNode(const EdgeValue &value, const VertexNodeIterator &source, const VertexNodeIterator &target)
            : value_(value), source_(source), target_(target) {}
    public:
        /** Unique edge ID number.
         *
         *  Edges within a graph are numbered consecutively starting at zero, and this method returns the edge's ID number. ID
         *  numbers are unrelated to the order in which edges are inserted, although in the absense of edge erasure they will
         *  be assigned consecutively.  Edge ID numbers are stable over insertion of vertices and edges and the erasure of
         *  vertices (provided no incident edges are erase), but are not stable over edge erasure.  In order to obtain
         *  constant-time edge erasure, after an edge is erased the largest-ID edge is renumbered to fill the gap.
         *
         *  Time complexity is constant. */
        const size_t& id() const { return self_->id(); }

        /** Source vertex.
         *
         *  All edges in a graph are directed edges, and this method returns an iterator (pointer) to the vertex that serves
         *  as the source of this edge.
         *
         *  Time complexity is constant.
         *
         * @{ */
        const VertexNodeIterator& source() { return source_; }
        ConstVertexNodeIterator source() const { return source_; }
        /** @} */

        /** Target vertex.
         *
         *  All edges in a graph are directed edges, and this method returns an iterator (pointer) to the vertex that serves as
         *  the destination of this edge.
         *
         *  Time complexity is constant.
         *
         * @{ */
        const VertexNodeIterator& target() { return target_; }
        ConstVertexNodeIterator target() const { return target_; }
        /** @} */

        /** User-defined value.
         *
         *  Each edge stores one user-defined value whose type is specified as the @p E parameter of the Graph template
         *  (a.k.a., the @ref EdgeValue type). This method returns a reference to that data, which was copied into the graph
         *  when this edge was inserted.  This is also the value that is returned when an edge value iterator (@ref
         *  EdgeValueIterator or @ref ConstEdgeValueIterator) is dereferenced.
         *
         *  Time complexity is constant.
         *
         * @{ */
        EdgeValue& value() { return value_; }
        const EdgeValue& value() const { return value_; }
        /** @} */

        /** Determines if edge is a self-edge.
         *
         *  Returns true if this edge is a self edge.  A self edge is an edge whose source and target vertices are the same
         *  vertex. */
        bool isSelfEdge() const {
            return source_ == target_;
        }
    };

    /** Vertex node.
     *
     *  These list nodes contain all information about a vertex and are the objects returned (by reference) when a vertex node
     *  iterator (@ref VertexNodeIterator or @ref ConstVertexNodeIterator) is dereferenced. */
    class VertexNode {
        VertexValue value_;                             // user data for this vertex
        typename VertexList::NodeIterator self_;        // always points to itself so we can get to IndexedList::Node
        VirtualList<EdgeNode> edgeLists_;               // this is the head node; points to the real edges
        size_t nInEdges_;                               // number of incoming edges
        size_t nOutEdges_;                              // number of outgoing edges
    private:
        friend class Graph;
        VertexNode(const VertexValue &value): value_(value), nInEdges_(0), nOutEdges_(0) {}
    public:
        /** Unique vertex ID number.
         *
         *  Vertices within a graph are numbered consecutively starting at zero, and this method returns the vertex's ID
         *  number. ID numbers are unrelated to the order in which vertices are inserted, although in the absense of vertex
         *  erasure they will be assigned consecutively.  Vertex ID numbers are stable over insertion of vertices and edges and
         *  the erasure of edges, but are not stable over vertex erasure.  In order to obtain constant-time vertex erasure (at
         *  least when it has no incident edges), after a vertex is erased the largest-ID vertex is renumbered to fill the
         *  gap.
         *
         *  Time complexity is constant. */
        const size_t& id() const { return self_->id(); }

        /** List of incoming edges.
         *
         *  Returns a sublist of edges whose target vertex is this vertex.  The return value is a pair of iterators which
         *  delineate the edges.  The traversal is in no particular order. Edge iterators are equality-comparable with one
         *  another even when the come from different sublists. See @ref EdgeNodeIterator for details.
         *
         *  Time complexity is constant.
         *
         * @{ */
        boost::iterator_range<EdgeNodeIterator> inEdges() {
            EdgeNodeIterator begin(IN_EDGES, &edgeLists_.next(IN_EDGES));
            EdgeNodeIterator end(IN_EDGES, &edgeLists_);
            return boost::iterator_range<EdgeNodeIterator>(begin, end);
        }
        boost::iterator_range<ConstEdgeNodeIterator> inEdges() const {
            ConstEdgeNodeIterator begin(IN_EDGES, &edgeLists_.next(IN_EDGES));
            ConstEdgeNodeIterator end(IN_EDGES, &edgeLists_);
            return boost::iterator_range<ConstEdgeNodeIterator>(begin, end);
        }
        /** @} */

        /** List of outgoing edges.
         *
         *  Returns a sublist of edges whose source vertex is this vertex.  The return value is a pair of iterators which
         *  delineate the edges.  The traversal is in no particular order. Edge iterators are equality-comparable with one
         *  another even when the come from different sublists. See @ref EdgeNodeIterator for details.
         *
         *  Time complexity is constant.
         *
         * @{ */
        boost::iterator_range<EdgeNodeIterator> outEdges() {
            EdgeNodeIterator begin(OUT_EDGES, &edgeLists_.next(OUT_EDGES));
            EdgeNodeIterator end(OUT_EDGES, &edgeLists_);
            return boost::iterator_range<EdgeNodeIterator>(begin, end);
        }
        boost::iterator_range<ConstEdgeNodeIterator> outEdges() const {
            ConstEdgeNodeIterator begin(OUT_EDGES, &edgeLists_.next(OUT_EDGES));
            ConstEdgeNodeIterator end(OUT_EDGES, &edgeLists_);
            return boost::iterator_range<ConstEdgeNodeIterator>(begin, end);
        }
        /** @} */

        /** Number of incoming edges.
         *
         *  Returns the in-degree of this vertex, the length of the list returned by @ref inEdges. */
        size_t nInEdges() const {
            return nInEdges_;
        }

        /** Number of outgoing edges.
         *
         *  Returns the out-degree of this vertex, the length of the list returned by @ref outEdges. */
        size_t nOutEdges() const {
            return nOutEdges_;
        }

        /** Number of incident edges.
         *
         *  Returns the total number of incident edges, the sum of @ref nInEdges and @ref nOutEdges.  Self-edges are counted
         *  two times: once for the source end, and once for the target end. */
        size_t degree() const {
            return nInEdges_ + nOutEdges_;
        }

        /** User-defined value.
         *
         *  Each vertex stores one user-defined value whose type is specified as the @p V parameter of the Graph template
         *  (a.k.a., the @ref VertexValue type). This method returns a reference to that data, which was copied into the graph
         *  when this vertex was inserted.  This is also the value that is returned when a vertex value iterator (@ref
         *  VertexValueIterator or @ref ConstVertexValueIterator) is dereferenced.
         *
         *  Time complexity is constant.
         *
         * @{ */
        VertexValue& value() { return value_; }
        const VertexValue& value() const { return value_; }
        /** @} */
    };

private:
    EdgeList edges_;                                    // all edges with integer ID numbers and O(1) insert/erase
    VertexList vertices_;                               // all vertices with integer ID numbers and O(1) insert/erase


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Initialization
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    /** Default constructor.
     *
     *  Creates an empty graph.
     *
     *  Time complexity is constant. */
    Graph(const Allocator &allocator = Allocator()): edges_(allocator), vertices_(allocator) {};

    /** Copy constructor.
     *
     *  Initializes this graph by copying all node and edge data from the @p other graph and initializing the same vertex
     *  connectivity.  Vertices and edges in this new graph will have the same ID numbers as the @p other graph, but the order
     *  of vertex and edges traversals is not expected to be the same.
     *
     *  The new graph's allocator is copy constructed from the source graph's allocator, which results in the new allocator
     *  having the same settings but sharing none of the original data.
     *
     *  Time complexity is linear in the total number of vertices and edges in @p other. */
    Graph(const Graph &other): edges_(other.edges_.allocator()), vertices_(other.vertices_.allocator()) {
        *this = other;
    }

    /** Copy constructor.
     *
     *  Initializes this graph by copying all node and edge data from the @p other graph and initializing the same vertex
     *  connectivity.  The vertices and edges of @p other must be convertible to the types of vertices and edges in this
     *  graph, and the will have the same ID numbers as in the @p other graph.  The order of vertex and edge traversals is not
     *  expected to be identical between the two graphs.
     *
     *  Time complexity is linear in the total number of vertices and edges in @p other. */
    template<class V2, class E2, class Alloc2>
    Graph(const Graph<V2, E2, Alloc2> &other, const Allocator &allocator = Allocator())
        : edges_(allocator), vertices_(allocator) {
        *this = other;
    }

    /** Assignment.
     *
     *  Causes this graph to look like @p other in that this graph will have copies of all the @p other vertex and edge data
     *  and the same vertex connectivity as @p other.  The vertices and edges will have the same ID numbers as in @p other.
     *  The order of vertex and edge traversals is not expected to be identical between the two graphs.
     *
     *  Time complexity is linear in the sum of the number of vertices and edges in this graph and @p other. */
    Graph& operator=(const Graph &other) {
        return operator=<V, E>(other);
    }
    
    /** Assignment.
     *
     *  Causes this graph to look like @p other in that this graph will have copies of all the @p other vertex and edge data
     *  and the same vertex connectivity as @p other.  The vertices and edges of @p other must be convertible to the types of
     *  vertices and edges in this graph, and they will have the same ID numbers as in @p other.  The order of vertex and edge
     *  traversals is not expected to be identical between the two graphs.
     *
     *  Time complexity is linear in the sum of the number of vertices and edges in this graph and @p other. */
    template<class V2, class E2, class Alloc2>
    Graph& operator=(const Graph<V2, E2, Alloc2> &other) {
        clear();
        for (size_t i=0; i<other.nVertices(); ++i) {
            typename Graph<V2, E2>::ConstVertexNodeIterator vertex = other.findVertex(i);
            VertexNodeIterator inserted SAWYER_ATTR_UNUSED = insertVertex(VertexValue(vertex->value()));
            ASSERT_require(inserted->id() == i);
        }
        for (size_t i=0; i<other.nEdges(); ++i) {
            typename Graph<V2, E2>::ConstEdgeNodeIterator edge = other.findEdge(i);
            VertexNodeIterator vsrc = findVertex(edge->source()->id());
            VertexNodeIterator vtgt = findVertex(edge->target()->id());
            insertEdge(vsrc, vtgt, EdgeValue(edge->value()));
        }
        return *this;
    }

    /** Allocator.
     *
     *  Returns the allocator used for vertices (and probably edges). */
    const Allocator& allocator() {
        return vertices_.allocator();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    /** Iterators for all vertices.
     *
     *  Returns a pair of vertex node iterators that deliniate the list of all vertices of this graph.  The traversal of this
     *  list is in no particular order.
     *
     *  Time complexity is constant.
     *
     * @{ */
    boost::iterator_range<VertexNodeIterator> vertices() {
        return boost::iterator_range<VertexNodeIterator>(VertexNodeIterator(vertices_.nodes().begin()),
                                                         VertexNodeIterator(vertices_.nodes().end()));
    }
    boost::iterator_range<ConstVertexNodeIterator> vertices() const {
        return boost::iterator_range<ConstVertexNodeIterator>(ConstVertexNodeIterator(vertices_.nodes().begin()),
                                                              ConstVertexNodeIterator(vertices_.nodes().end()));
    }
    /** @} */

    /** Iterators for all vertices.
     *
     *  Returns a pair of vertex value iterators that deliniate the list of all vertices of the graph.  The traversal of this
     *  list is in no particular order.
     *
     *  Although vertex node iterators are implicitly convertible to vertex value iterators, this method proves useful in
     *  conjuction with "foreach" loops:
     *
     *  @code
     *   Sawyer::Container::Graph<std::string, ...> graph = ...;
     *   BOOST_FOREACH (const std::string &vertexName, graph.vertexValues())
     *       std::cout <<"name = " <<*vertexName <<"\n";
     *  @endcode
     *
     *  Time complexity is constant.
     *
     *  @{ */
    boost::iterator_range<VertexValueIterator> vertexValues() {
        return boost::iterator_range<VertexValueIterator>(VertexValueIterator(vertices_.nodes().begin()),
                                                          VertexValueIterator(vertices_.nodes().end()));
    }
    boost::iterator_range<ConstVertexValueIterator> vertexValues() const {
        return boost::iterator_range<ConstVertexValueIterator>(ConstVertexValueIterator(vertices_.nodes().begin()),
                                                               ConstVertexValueIterator(vertices_.nodes().end()));
    }
    /** @} */

    /** Finds the vertex with specified ID number.
     *
     *  Returns a vertex node iterator for the vertex with the specified ID.  ID numbers are consecutive integers beginning at
     *  zero.  Do not call this method with an ID number greater than or equal to the number of vertices contained in this graph.
     *
     *  Time complexity is constant.
     *
     *  @{ */
    VertexNodeIterator findVertex(size_t id) {
        return VertexNodeIterator(vertices_.find(id));
    }
    ConstVertexNodeIterator findVertex(size_t id) const {
        return ConstVertexNodeIterator(vertices_.find(id));
    }
    /** @} */

    /** Iterators for all edges.
     *
     *  Returns a pair of edge node iterators that deliniate the list of all edges of this graph.  The traversal of this
     *  list is in no particular order.
     *
     *  Time complexity is constant.
     *
     *  @{ */
    boost::iterator_range<EdgeNodeIterator> edges() {
        return boost::iterator_range<EdgeNodeIterator>(EdgeNodeIterator(edges_.nodes().begin()),
                                                       EdgeNodeIterator(edges_.nodes().end()));
    }
    boost::iterator_range<ConstEdgeNodeIterator> edges() const {
        return boost::iterator_range<ConstEdgeNodeIterator>(ConstEdgeNodeIterator(edges_.nodes().begin()),
                                                            ConstEdgeNodeIterator(edges_.nodes().end()));
    }
    /** @} */

    /** Iterators for all edges.
     *
     *  Returns a pair of edge value iterators that deliniate the list of all edges of the graph.  The traversal of this
     *  list is in no particular order.
     *
     *  Although edge node iterators are implicitly convertible to edge value iterators, this method proves useful in
     *  conjuction with "foreach" loops:
     *
     *  @code
     *   Sawyer::Container::Graph<..., std::string> graph = ...;
     *   BOOST_FOREACH (const std::string &edgeName, graph.edgeValues())
     *       std::cout <<"name = " <<*edgeName <<"\n";
     *  @endcode
     *
     *  Time complexity is constant.
     *
     *  @{ */
    boost::iterator_range<EdgeValueIterator> edgeValues() {
        return boost::iterator_range<EdgeValueIterator>(EdgeValueIterator(edges_.nodes().begin()),
                                                        EdgeValueIterator(edges_.nodes().end()));
    }
    boost::iterator_range<ConstEdgeValueIterator> edgeValues() const {
        return boost::iterator_range<ConstEdgeValueIterator>(ConstEdgeValueIterator(edges_.nodes().begin()),
                                                             ConstEdgeValueIterator(edges_.nodes().end()));
    }
    /** @} */

    /** Finds the edge with specified ID number.
     *
     *  Returns an edge node iterator for the edge with the specified ID.  ID numbers are consecutive integers beginning at
     *  zero.  Do not call this method with an ID number greater than or equal to the number of edges contained in this graph.
     *
     *  Time complexity is constant.
     *
     *  @{ */
    EdgeNodeIterator findEdge(size_t id) {
        return EdgeNodeIterator(edges_.find(id));
    }
    ConstEdgeNodeIterator findEdge(size_t id) const {
        return ConstEdgeNodeIterator(edges_.find(id));
    }
    /** @} */

    /** Total number of vertices.
     *
     *  Returns the total number of vertices in the graph.  Vertex ID numbers are guaranteed to be less than this value and
     *  greater than or equal to zero.
     *
     *  Time complexity is constant. */
    size_t nVertices() const {
        return vertices_.size();
    }

    /** Total number of edges.
     *
     *  Returns the total number of edges in the graph.  Edge ID numbers are guaranteed to be less than this value and greater
     *  than or equal to zero.
     *
     *  Time complexity is constant. */
    size_t nEdges() const {
        return edges_.size();
    }

    /** True if graph is empty.
     *
     *  Returns true if this graph contains no vertices (and therefore no edges).
     *
     *  Time complexity is constant. */
    bool isEmpty() const {
        ASSERT_require(edges_.isEmpty() || !vertices_.isEmpty()); // existence of edges implies existence of vertices
        return vertices_.isEmpty();
    }

    /** Insert a new vertex.
     *
     *  Inserts a new vertex and copies @p value (if specified, or else default-constructed) into the vertex node.  Returns an
     *  iterator that points to the new vertex.  All other vertex iterators that were not already positioned at the
     *  one-past-last vertex will eventually traverse this new vertex; no iterators, vertex or edge, are invalidated.  The new
     *  vertex is given the higest vertex ID number; no other ID numbers, vertex or edge, change.
     *
     *  Time complexity is constant. */
    VertexNodeIterator insertVertex(const VertexValue &value = VertexValue()) {
        typename VertexList::NodeIterator inserted = vertices_.insert(vertices_.nodes().end(), VertexNode(value));
        inserted->value().self_ = inserted;
        inserted->value().edgeLists_.reset(NULL);       // this is a sublist head, no edge node
        return VertexNodeIterator(inserted);
    }

    /** Insert a new edge.
     *
     *  Inserts a new edge and copies @p value (if specified, or else default-constructed) into the edge node.  Returns an
     *  iterator that points to the new edge.  All other edge iterators that were not already positioned at the one-past-last
     *  edge will eventually traverse this new edge; no iterators, edge or vertex, are invalidated.  The new edge is given the
     *  highest edge ID number; no other ID numbers, edge or vertex, change.
     *
     *  Time complexity is constant. */
    EdgeNodeIterator insertEdge(const VertexNodeIterator &sourceVertex, const VertexNodeIterator &targetVertex,
                                const EdgeValue &value = EdgeValue()) {
        ASSERT_forbid(sourceVertex==vertices().end());
        ASSERT_forbid(targetVertex==vertices().end());
        typename EdgeList::NodeIterator inserted = edges_.insert(edges_.nodes().end(),
                                                                 EdgeNode(value, sourceVertex, targetVertex));
        inserted->value().self_ = inserted;
        inserted->value().edgeLists_.reset(&inserted->value());
        EdgeNodeIterator newEdge(inserted);
        sourceVertex->edgeLists_.insert(OUT_EDGES, &newEdge->edgeLists_);
        ++sourceVertex->nOutEdges_;
        targetVertex->edgeLists_.insert(IN_EDGES, &newEdge->edgeLists_);
        ++targetVertex->nInEdges_;
        return newEdge;
    }

    /** Erases an edge.
     *
     *  The edge specified by the iterator (which must not be a one-past-last iterator) is erased from the graph. The term
     *  "erasure" is Standard Template Library terminology for the withdrawal and deletion of an object from a container, and
     *  differs from the term "remove", which means to move an object to some near-the-end position in a container.  Any edge
     *  iterator that was pointing at the removed edge becomes invalid and should not be subsequently dereferenced,
     *  incremented, decremented, or compared; other iterators, edge and vertex, are unaffected.  The edge with the highest ID
     *  number will be given the ID of the edge that was removed in order to fill the gap left in the ID sequence.  This method
     *  returns an iterator for the edge following the one that was deleted (possibly the one-past-last iterator if the last
     *  edge was deleted).
     *
     *  Time complexity is constant. */
    EdgeNodeIterator eraseEdge(const EdgeNodeIterator &edge) {
        ASSERT_forbid(edge==edges().end());
        EdgeNodeIterator next = edge; ++next;           // advance before we delete edge
        --edge->source_->nOutEdges_;
        edge->edgeLists_.remove(OUT_EDGES);
        --edge->target_->nInEdges_;
        edge->edgeLists_.remove(IN_EDGES);
        edges_.eraseAt(edge->self_);                    // edge is now deleted
        return next;
    }

    /** Erases all edges connecting two vertices.
     *
     *  Given two vertex iterators, erase all edges whose source is the first vertex and whose target is the second vertex.
     *
     *  Time complexity is linear in the number of incoming or outgoing edges (whichever is smaller). */
    void eraseEdges(const VertexNodeIterator &source, const VertexNodeIterator &target) {
        ASSERT_forbid(source==vertices().end());
        ASSERT_forbid(target==vertices().end());
        if (source->nOutEdges() < target->nInEdges()) {
            EdgeNodeIterator iter = source->outEdges().begin();
            while (iter != source->outEdges().end()) {
                if (iter->target() == target) {
                    iter = eraseEdge(iter);
                } else {
                    ++iter;
                }
            }
        } else {
            EdgeNodeIterator iter = target->inEdges().begin();
            while (iter != target->inEdges().end()) {
                if (iter->source() == source) {
                    iter = eraseEdge(iter);
                } else {
                    ++iter;
                }
            }
        }
    }
    
    /** Erases a vertex and its incident edges.
     *
     *  The vertex specified by the iterator (which must not be a one-past-last iterator) is erased from the graph along with
     *  all edges that originate from or terminate at that vertex. The term "erasure" is Standard Template Library terminology
     *  for the withdrawal and deletion of an object from a container, and differs from the term "remove", which means to move
     *  an object to some near-the-end position in a container.  Any iterator that was pointing at the removed vertex or any of
     *  its incident edges becomes invalid and should not be subsequently dereferenced, incremented, decremented, or compared;
     *  other iterators, edge and vertex, are unaffected.  The vertex with the highest ID number will be given the ID of the
     *  vertex that was removed in order to fill the gap left in the ID sequence.  This method returns an iterator for the
     *  vertex following the one that was deleted (possibly the one-past-last iterator if the last vertex was deleted).
     *
     *  Time complexity is constant. */
    VertexNodeIterator eraseVertex(const VertexNodeIterator &vertex) {
        VertexNodeIterator next = vertex; ++next;       // advance before we delete vertex
        clearEdges(vertex);
        vertices_.eraseAt(vertex->self_);               // vertex is now deleted
        return next;
    }

    /** Erase all edges, but leave all vertices.
     *
     *  This method erases (withdraws and deletes) all edges but leaves all vertices. It is logically equivalent to calling
     *  @ref eraseEdge for each edge, but is more efficient.
     *
     *  Time complexity is linear in the number of edges erased. */
    void clearEdges() {
        for (VertexNodeIterator vertex=vertices().begin(); vertex!=vertices().end(); ++vertex) {
            vertex->inEdges().reset();
            vertex->outEdges().reset();
        }
        edges_.clear();
    }

    /** Erase all edges incident to a vertex.
     *
     *  This method erases (withdraws and deletes) all edges that are incident to the specified vertex. That is, all edges
     *  whose source or target is the vertex.  It is logically equivalent to calling @ref clearOutEdges followed by @ref
     *  clearInEdges, and has the same effects on iterators and edge ID numbers as erasing edges individually.
     *
     *  Time complexity is linear in the number of edges erased. */
    void clearEdges(const VertexNodeIterator &vertex) {
        clearOutEdges(vertex);
        clearInEdges(vertex);
    }

    /** Erase all edges emanating from a vertex.
     *
     *  This method erases (withdraws and deletes) all edges whose source is the specified vertex.  It has the same effects on
     *  iterators and edge ID numbers as erasing edges individually.
     *
     *  Time complexity is linear in the number of edges erased. */
    void clearOutEdges(const VertexNodeIterator &vertex) {
        ASSERT_forbid(vertex==vertices().end());
        for (EdgeNodeIterator edge=vertex->outEdges().begin(); edge!=vertex->outEdges().end(); /*void*/)
            edge = eraseEdge(edge);
    }

    /** Erase all edges targeting a vertex.
     *
     *  This method erases (withdraws and deletes) all edges whose target is the specified vertex.  It has the same effects on
     *  iterators and edge ID numbers as erasing edges individually.
     *
     *  Time complexity is linear in the number of edges erased. */
    void clearInEdges(const VertexNodeIterator &vertex) {
        ASSERT_forbid(vertex==vertices().end());
        for (EdgeNodeIterator edge=vertex->inEdges().begin(); edge!=vertex->inEdges().end(); /*void*/)
            edge = eraseEdge(edge);
    }

    /** Remove all vertices and edges.
     *
     *  This method has the same effect as erasing edges and vertices individually until the container is empty, but is more
     *  efficient.  All iterators to vertices and edges in this container become invalid and should not be dereferenced,
     *  incremented, decremented, or compared.
     *
     *  Time complexity is linear in the number of vertices and edges erased. */
    void clear() {
        edges_.clear();
        vertices_.clear();
    }
};

} // namespace
} // namespace

#endif
