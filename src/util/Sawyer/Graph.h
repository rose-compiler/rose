// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#ifndef Sawyer_Graph_H
#define Sawyer_Graph_H

#include <Sawyer/Assert.h>
#include <Sawyer/DefaultAllocator.h>
#include <Sawyer/Exception.h>
#include <Sawyer/IndexedList.h>
#include <Sawyer/Map.h>
#include <Sawyer/Optional.h>                            // for Sawyer::Nothing
#include <Sawyer/Sawyer.h>
#include <boost/range/iterator_range.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/unordered_map.hpp>
#include <ostream>
#if 1 /*DEBUGGING [Robb Matzke 2014-04-21]*/
#include <iomanip>
#endif

namespace Sawyer {
namespace Container {

/** @defgroup sawyer_indexed_graph_examples Examples of indexed graph features
 *  @ingroup sawyer_examples
 *
 *  Examples for using indexing features of @ref Graph. */

/** @defgroup sawyer_indexed_graph_example_1 Indexing string vertices
 *  @ingroup sawyer_indexed_graph_examples
 *
 *  Demo of indexing graph vertices that are strings.
 *
 *  This demo creates a graph whose vertices are unique city names (@c std::string) and whose edges are the time in hours it
 *  takes to travel from the source to the target city by train, including layovers (double).  We want the vertices to be
 *  indexed so we don't have to keep track of them ourselves--we want to be able to look up a vertex in logarithmic time.
 *
 *  @snippet indexedGraphDemo.C demo1 */

/** @defgroup sawyer_indexed_graph_example_2 Using only part of a vertex as the key
 *  @ingroup sawyer_indexed_graph_examples
 *
 *  Demo of using only part of a vertex as the lookup key.
 *
 *  This demo creates a graph that stores multiple things at each vertex, only two of which are used to compute the unique
 *  vertex key. Each vertex is an airline flight and the edges represent layovers at airports.
 *
 *  @snippet indexedGraphDemo.C demo2 */

/** @defgroup sawyer_indexed_graph_example_3 Using a hash-based lookup
 *  @ingroup sawyer_indexed_graph_examples
 *
 *  Demo of using a hash-based mechanism for the vertex index.
 *
 *  By default, %Sawyer uses a balanced binary tree for the index which guarantees logarithmic insert, erase, and lookup
 *  times. However, the library also directly supports a hash-based index with O(N) time but which is nominally constant time
 *  if a good hash algorithm is used.
 *
 *  Each graph vertex is information about a person and the edges represent relationships although for simplicity this example
 *  doesn't actually store any information about the kinds of relationships.
 *
 *  @snippet indexedGraphDemo.C demo3 */

/** @defgroup sawyer_indexed_graph_example_4 Using a custom index type
 *  @ingroup sawyer_indexed_graph_examples
 *
 *  Demo showing how to define your own index type.
 *
 *  In this example the goal is to have vertices that are labeled with small, stable integers and provide O(1) index
 *  operations. We do that by using a @c std::vector as the index, and the vertex labels are indexes into the vector.
 *
 *  @snippet indexedGraphDemo.C demo4 */

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Special vertex and edge key types.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Type of vertex key for graphs that do not index their vertices.
 *
 *  This is the default vertex key type, @p VKey argument, for the @ref Graph template. In order to index graph vertices you
 *  must provide at least the vertex key type which must have a copy constructor and a constructor that takes a vertex value
 *  argument. Depending on the index type obtained from @ref GraphIndexTraits, this key type may need additional functionality
 *  such as a default constructor and a less-than operator or hashing function. */
template<class VertexValue>
class GraphVertexNoKey {
public:
    GraphVertexNoKey() {}
    explicit GraphVertexNoKey(const VertexValue&) {}
};

/** Type of edge key for graphs that do not index their edges.
 *
 *  This is the default edge key type, @p EKey argument, for the @ref Graph template. In order to index graph edges you must
 *  provide at least the index key type which must have a copy constructor and a constructor that takes an edge value
 *  argument. Depending on the index type obtained from @ref GraphIndexTraits, this key type may need additional functionality
 *  such as a default constructor and a less-than operator or hashing function. */
template<class EdgeValue>
class GraphEdgeNoKey {
public:
    GraphEdgeNoKey() {}
    explicit GraphEdgeNoKey(const EdgeValue&) {}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Special vertex and edge indexing types.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Fake index for graphs that don't have an index.
 *
 *  This is the index type used when a vertex or edge index is not required. It has no storage and constant lookup times.  The
 *  API of the index is documented here.  See @ref GraphIndexTraits for information about how to override the index type for a
 *  graph. */
template<class VertexOrEdgeKey, class VertexOrEdgeConstIterator>
class GraphVoidIndex {
public:
    /** Erase all data from this index.
     *
     *  This resets the index to the same state as if it were default constructed. */
    void clear() {}

    /** Insert a new element into the map.
     *
     *  Inserts a mapping from a vertex or edge key to a vertex or edge const iterator regardless of whether the key already
     *  exists in the map. The index should never contain duplicate keys, so if the same key is inserted twice the second one
     *  overrides the first one. In other words, inserting a key/iterator pair into an index behaves exactly the same whether
     *  the index is implemented as an @c std::map from key to value or an array indexed by a key. */
    void insert(const VertexOrEdgeKey&, const VertexOrEdgeConstIterator&) {}

    /** Erase an element from the map.
     *
     *  Erases the mapping for the specified key. If the key does not exist in the index then nothing happens. */
    void erase(const VertexOrEdgeKey&) {}

    /** Look up iterator for vertex or edge key.
     *
     *  Given a vertex or edge key, return the graph vertex or edge const iterator for that key.  If the key does not
     *  exist in the index then return nothing. */
    Optional<VertexOrEdgeConstIterator> lookup(const VertexOrEdgeKey&) const {
        return Nothing();
    }
};

/** Map based index is the default index type when indexes are present.
 *
 *  This index has O(log N) insert, erase, and lookup times. The key type must have a less-than operator.
 *
 *  The semantics for the methods of this class are documented in the @ref GraphVoidIndex class. */
template<class VertexOrEdgeKey, class VertexOrEdgeConstIterator>
class GraphBimapIndex {
    Map<VertexOrEdgeKey, VertexOrEdgeConstIterator> map_;
public:
    /** Erase all data from this index.
     *
     *  See @ref GraphVoidIndex::clear. */
    void clear() {
        map_.clear();
    }

    /** Insert a new element into the map.
     *
     *  See @ref GraphVoidIndex::insert. */
    void insert(const VertexOrEdgeKey &key, const VertexOrEdgeConstIterator &iter) {
        map_.insert(key, iter);                         // Unlike std::map, Sawyer's "insert" always inserts
    }

    /** Erase an element from the map.
     *
     *  See @ref GraphVoidIndex::erase. */
    void erase(const VertexOrEdgeKey &key) {
        map_.erase(key);
    }

    /** Lookup iterator for vertex or edge key.
     *
     *  See @ref GraphVoidIndex::lookup. */
    Optional<VertexOrEdgeConstIterator> lookup(const VertexOrEdgeKey &key) const {
        return map_.getOptional(key);
    }
};

/** Hash-based indexing.
 *
 *  This index has O(N) insert, erase, and lookup times, although nominally the times are constant because the index uses
 *  hashing.  The vertex or edge keys must have a hash function appropriate for <code>boost::unordered_map</code> and an
 *  equality operator.
 *
 *  The semantics for each of the class methods are documented in @ref GraphVoidIndex. */
template<class VertexOrEdgeKey, class VertexOrEdgeConstIterator>
class GraphHashIndex {
    typedef boost::unordered_map<VertexOrEdgeKey, VertexOrEdgeConstIterator> Map;
    Map map_;
public:
    /** Erase all data from this index.
     *
     *  See @ref GraphVoidIndex::clear. */
    void clear() {
        map_.clear();
    }

    /** Insert a new element into the map.
     *
     *  See @ref GraphVoidIndex::insert. */
    void insert(const VertexOrEdgeKey &key, const VertexOrEdgeConstIterator &iter) {
        map_[key] = iter;
    }

    /** Erase an element from the map.
     *
     *  See @ref GraphVoidIndex::erase. */
    void erase(const VertexOrEdgeKey &key) {
        map_.erase(key);
    }

    /** Lookup iterator for vertex or edge key.
     *
     *  See @ref GraphVoidIndex::lookup. */
    Optional<VertexOrEdgeConstIterator> lookup(const VertexOrEdgeKey &key) const {
        typename Map::const_iterator found = map_.find(key);
        if (found == map_.end())
            return Nothing();
        return found->second;
    }
};

/** Traits for vertex and edge indexing.
 *
 *  By partly specializing this class template a user can define the type of index used for a particular vertex or edge key
 *  type.   Sawyer already defines specializations for graphs that have no vertex or edge keys.  The @c
 *  SAWYER_GRAPH_INDEXING_SCHEME_1 and @c SAWYER_GRAPH_INDEXING_SHCEME_2 macros can be used at the global scope to add partial
 *  specializations for users that aren't keen to use C++ templates. They both take two arguments: the key type and the index
 *  type. They differ only in whether the index type template takes one argument (a vertex or edge iterator type) or two (the
 *  key type and an iterator type). */
template<class VertexOrEdgeKey, class VertexOrEdgeConstIterator>
struct GraphIndexTraits {
    /** Type of index to use for the specified key type. */
    typedef GraphBimapIndex<VertexOrEdgeKey, VertexOrEdgeConstIterator> Index;
};

// Partial specialization for when there is no vertex index
template<class VertexValue, class ConstVertexIterator>
struct GraphIndexTraits<GraphVertexNoKey<VertexValue>, ConstVertexIterator> {
    typedef GraphVoidIndex<GraphVertexNoKey<VertexValue>, ConstVertexIterator> Index;
};

// Partial specialization for when there is no edge index.
template<class EdgeValue, class ConstEdgeIterator>
struct GraphIndexTraits<GraphEdgeNoKey<EdgeValue>, ConstEdgeIterator> {
    typedef GraphVoidIndex<GraphEdgeNoKey<EdgeValue>, ConstEdgeIterator> Index;
};

// A #define so users that don't understand C++ templates can still get by. See GraphIndexTraits doc for details.
// Must be used at global scope.
#define SAWYER_GRAPH_INDEXING_SCHEME_1(KEY_TYPE, INDEX_TYPE)                                                                   \
    namespace Sawyer {                                                                                                         \
        namespace Container {                                                                                                  \
            template<class VertexOrEdgeConstIterator>                                                                          \
            struct GraphIndexTraits<KEY_TYPE, VertexOrEdgeConstIterator> {                                                     \
                typedef INDEX_TYPE<VertexOrEdgeConstIterator> Index;                                                           \
            };                                                                                                                 \
        }                                                                                                                      \
    }

#define SAWYER_GRAPH_INDEXING_SCHEME_2(KEY_TYPE, INDEX_TYPE)                                                                   \
    namespace Sawyer {                                                                                                         \
        namespace Container {                                                                                                  \
            template<class VertexOrEdgeConstIterator>                                                                          \
            struct GraphIndexTraits<KEY_TYPE, VertexOrEdgeConstIterator> {                                                     \
                typedef INDEX_TYPE<KEY_TYPE, VertexOrEdgeConstIterator> Index;                                                 \
            };                                                                                                                 \
        }                                                                                                                      \
    }

    
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Graph traits
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Traits for graphs. */
template<class G>
struct GraphTraits {
    /** Const or non-const edge iterator. */
    typedef typename G::EdgeIterator EdgeIterator;

    /** Const or non-const edge value iterator. */
    typedef typename G::EdgeValueIterator EdgeValueIterator;

    /** Const or non-const vertex iterator. */
    typedef typename G::VertexIterator VertexIterator;

    /** Const or non-const vertex value iterator. */
    typedef typename G::VertexValueIterator VertexValueIterator;

    /** Vertex type including user type and connectivity. */
    typedef typename G::Vertex Vertex;

    /** Edge type including user type and connectivity. */
    typedef typename G::Edge Edge;

    /** User-defined vertex type without connectivity information. */
    typedef typename G::VertexValue VertexValue;

    /** User-defined edge type without connectivity information. */
    typedef typename G::EdgeValue EdgeValue;
};

// GraphTraits specialization for const graphs.
template<class G>
struct GraphTraits<const G> {
    typedef typename G::ConstEdgeIterator EdgeIterator;
    typedef typename G::ConstEdgeValueIterator EdgeValueIterator;
    typedef typename G::ConstVertexIterator VertexIterator;
    typedef typename G::ConstVertexValueIterator VertexValueIterator;
    typedef const typename G::Vertex Vertex;
    typedef const typename G::Edge Edge;
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
 *  MyGraph::VertexIterator v1 = graph.insertVertex("first vertex");
 *  MyGraph::VertexIterator v2 = graph.insertVertex("second vertex");
 *  graph.insertEdge(v1, v2, 1.2); // v1 and v2 are the source and target vertices
 * @endcode
 *
 *  This example shows a few features of the design: First, like STL containers, there is a clear separation of concerns
 *  related to managing the storage and connectivity versus managing the user-defined data stored in the container.  Just as an
 *  STL container like <code>std::list</code> is reponsible for managing the list's vertices and the linear connectivity
 *  between those vertices, %Sawyer is responsible for managing the vertex storage and the connectivity (edges) between the
 *  vertices, and the user is responsible for their data (the first two graph template arguments).
 *
 *  Another feature of the design is that iterators serve a dual purpose. Just like an <code>int*</code> pointing into an array
 *  of integers can be used as a pointer to a single element or incremented to iterate through elements, %Sawyer graph
 *  iterators are both pointers to a particular vertex or edge and at other times incremented to iterate over vertices and
 *  edges. Oftentimes incrementing an iterator that's being used as a pointer doesn't really make much sense, just as
 *  incrementing an <code>int*</code> for an array implementation of a lattice/heap/tree/etc might not make much sense.
 *
 *  In this documentation, the term "node" refers to the unit of storage for an edge or vertex, which contains the user-defined
 *  value for the edge or vertex, plus an ID number and connectivity information. Within this documentation, the term "vertex"
 *  is always used as the name of a graph component (i.e., a graph has vertices and edges), and the term "node" always refers
 *  to a unit of storage.
 *
 *  A graph doesn't necessarily need to store data at each vertex or edge. The vertex and edge types default to @ref Nothing,
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
 *  @li @ref VertexIterator refers to mutable vertex storage nodes
 *  @li @ref ConstVertexIterator refers to constant vertex storage nodes
 *
 *  A const-iterator points to information that is const qualified. Const-iterators can be converted to non-const iterators in
 *  linear time if one has the non-const graph available:
 *
 * @code
 *  MyGraph graph = ...
 *  MyGraph::ConstVertexIterator constVertex = ...; // not the end iterator
 *  MyGraph::VertexIterator vertex = graph.findVertex(constVertex->id());
 * @endcode
 *
 *  Edge iterators are similar.
 *
 *  The previous example (using vertex iterators to refer to newly-inserted vertices) should make more sense now.  Here's an
 *  example using iterators to actually iterate over something:
 *
 * @code
 *  std::cout <<"Vertex names:\n";
 *  for (MyGraph::ConstVertexIterator vertex=graph.vertices().begin(); vertex!=graph.vertices().end(); ++vertex)
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
 *  BOOST_FOREACH (const MyGraph::Vertex &vertex, graph.vertices())
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
 *      MyGraph::ConstEdgeIterator edge = graph.findEdge(edgeId);
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
 *  BOOST_FOREACH (const MyGraph::Vertex &vertex, graph.vertices())
 *      vertexHashes[vertex.id()] = hash(vertex.value());
 * @endcode
 *
 * @section connectivity Graph connectivity
 *
 *  Each vertex has two additional edge lists: a list of incoming edges where this vertex serves as the edges' target, and a
 *  list of outgoing edges where this vertex serves as the edges' source.  The lists are returned by the @ref
 *  Vertex::inEdges and @ref Vertex::outEdges methods.  These lists are sublists of the graph-wide edge list and
 *  iterators are equality-comparable and return references to the same underlying edges.  However, the "end" iterators for
 *  these sublists are all distinct from one another and distinct from the graph-wide edge list. (footnote: Actually, the "end"
 *  iterators for the in-coming and out-going lists of a single vertex are equal to each other, but don't depend on this.)
 *
 *  Each edge has two methods, @ref Edge::source and @ref Edge::target that return iterators to the source and target
 *  vertices for that edge.
 *
 *  Here's an example similar to the previous edge ID iteration except it presents the graph in terms of vertices:
 *
 * @code
 *  BOOST_FOREACH (const MyGraph::Vertex &vertex, graph.vertices()) {
 *      std::cout <<"vertex " <<vertex.id() <<"\n";
 *      BOOST_FOREACH (const MyGraph::Edge &edge, vertex.outEdges()) {
 *          std::cout <<"  edge " <<edge.id() <<" to vertex " <<edge.target()->id() <<"\n";
 *      }
 *  }
 * @endcode
 *
 * @section graph_indexing Indexing
 *
 *  Users should make every effort to use iterators or ID numbers to point to vertices and edges since these have constant-time
 *  performance. However, a drawback of iterators is that if you have a data structure that contains a graph and some iterators
 *  your copy constructor will need to update all its iterators so they point to the copied graph rather than the original
 *  graph. On the other hand, the drawback of using ID numbers is that erasing vertices and edges might change the ID numbers
 *  of other vertices and edges.  Therefore, graphs also optionally provide a vertex index and/or edge index that can be used
 *  to look up a vertex or edge if you know its key. Keys are anything you want as long as they're computed from the value
 *  stored in a vertex or edge. By default, graphs have neither vertex nor edge indexes--they must be enabled by supplying
 *  extra template arguments.  A graph that has an index does not have the same level of performance as a graph without an
 *  index.
 *
 *  Indexing works the same for vertices and edges, although it's most commonly used with vertices. By default, the graph
 *  implements a vertex index as a balanced binary tree that maps keys to vertices in O(log) time.  Therefore the key
 *  type must have a copy constructor and a less-than operator. The library also implements hash-based indexing, in which case
 *  the key must satisfy the requirements for <code>boost::unordered_map</code> instead.
 *
 *  In addition, regardless of what kind of index the graph uses, all keys must be (explicitly) constructable from a vertex
 *  value.  In practice it's often the case that the vertex values and the keys are the same type. For instance, if a vertex
 *  stores <code>std::string</code> then the key can also be an <code>std::string</code> since that type has all the properties
 *  we need.  Distinguishing between vertex value type and vertex key types allows the graph to store larger data structures at
 *  the vertices, a small part of which becomes the key. In fact, the key need not be a particular data member of the value as
 *  long as the key generator always produces the same key for the same data.
 *
 *  For example, lets say the vertex type is information about a city and that it's quite large.
 *
 * @code
 *  struct City {
 *      std::string name;
 *      const State *state;
 *      unsigned population;
 *      std::vector<std::string> zipCodes;
 *      std::vector<std::string> areaCodes;
 *      PhoneBook phoneBook;
 *      // Pretend there are lots more...
 *  };
 * @endcode
 *
 *  Every city has a unique name + state pair and we'd like to be able to look up cities in the graph by that pair. So we
 *  define a lookup key for cities:
 *
 * @code
 *  class CityKey {
 *      std::string key_;
 *  public:
 *      CityKey(const City &city) {
 *          key_ = city.name + ", " + city.state->abbreviation();
 *      }
 *
 *      bool operator<(const CityKey &other) const {
 *          return key_ < other.key_;
 *      }
 *  };
 * @endcode
 *
 *  Lets say that the edges between cities represent things like travel time by various modes and we keep this information in a
 *  type named @c TravelTime. Here's how our graph would be declared:
 *
 * @code
 *  typedef Sawyer::Container::Graph<City, TravelTime, CityKey> CityGraph;
 * @endcode
 *
 *  The only new thing we added is the third template argument, which this causes the graph to contain a vertex index and gives
 *  us the ability to look up a city by value or key. The following two lines assume you've added the appropriate constructors
 *  to the @c City and @c CityKey types.
 *
 * @code
 *  CityGraph::VertexIterator boston = cityGraph.findVertexValue(City("boston", MA));
 *  CityGraph::VertexIterator boston = cityGraph.findVertexKey(CityKey("boston", MA));
 * @endcode
 *
 *  The other thing a vertex index does is prevent us from adding two vertices having the same key--we'd get an @ref
 *  Exception::AlreadyExists error.  And finally a word of warning: if you define a vertex key and index then the values you
 *  store at each vertex (at least the parts from which a key is created) <em>must not change</em>, since doing so will give
 *  the vertex a new key without ever updating the index. The only way to change the key fields of a vertex is to insert a new
 *  vertex and erase the old one. This happens to be how many indexed containers work, including @c std::unordered_map. The
 *  same is true for edges.
 *
 *  Some indexed graph demos can be found @ref sawyer_indexed_graph_examples "here".
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
 *  STL containers and its much lighter use of C++ templates in its public API.
 *
 *  The <a
 *  href="https://github.com/matzke1/Sawyer/blob/master/tests/Container/graphBoost.C"><tt>tests/Container/graphBoost.C</tt></a>
 *  file in the %Sawyer source tree exemplifies the differences between the %Sawyer and BGL approaches and gives examples of
 *  using the BGL API on %Sawyer graphs.
 *
 * @section allocators Custom allocators
 *
 *  Because a graph allocates memory in terms of vertex and edge nodes, and because these nodes can be quite small, a graph can
 *  often benefit by using a memory pool allocation scheme.  The third template argument provides the type for the allocator,
 *  and the graph constructors take an allocator argument which is copied into the graph.  The allocator must implement the
 *  @ref Sawyer::DefaultAllocator API (essentially an allocate and a deallocate method), which happens to use the normal C++
 *  global <code>new</code> and <code>delete</code> allocators.  A couple possibilities are @ref
 *  Sawyer::SynchronizedPoolAllocator and @ref Sawyer::UnsynchronizedPoolAllocator.
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
 *  Time complexity guarantees for graphs without indexes:
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
template<class V = Nothing, class E = Nothing,
         class VKey = GraphVertexNoKey<V>, class EKey = GraphEdgeNoKey<E>,
         class Alloc = DefaultAllocator>
class Graph {
public:
    typedef V VertexValue;                              /**< User-level data associated with vertices. */
    typedef E EdgeValue;                                /**< User-level data associated with edges. */
    typedef VKey VertexKey;                             /**< Type for looking up a vertex. */
    typedef EKey EdgeKey;                               /**< Type for looking up an edge. */
    typedef Alloc Allocator;                            /**< Allocator for vertex and edge nodes. */
    class Vertex;                                       /**< All information about a vertex. User info plus connectivity info. */
    class Edge;                                         /**< All information about an edge. User info plus connectivity info. */

private:
    enum EdgePhase { IN_EDGES=0, OUT_EDGES=1, N_PHASES=2 };
    typedef IndexedList<Edge, Allocator> EdgeList;
    typedef IndexedList<Vertex, Allocator> VertexList;

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
            ASSERT_require(phase < N_PHASES);
            ASSERT_require((next_[phase]==this && prev_[phase]==this) || (next_[phase]!=this && prev_[phase]!=this));
            return next_[phase]==this;
        }

        bool isEmpty(EdgePhase phase) const {
            ASSERT_require(isHead());
            ASSERT_require((next_[phase]==this && prev_[phase]==this) || (next_[phase]!=this && prev_[phase]!=this));
            return next_[phase]==this;
        }

        void insert(EdgePhase phase, VirtualList *newNode) { // insert newNode before this
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

        T& dereference() {                              // Return the Edge to which this VirtualList node belongs
            ASSERT_forbid(isHead());                    // list head contains no user-data
            return *(T*)this;                           // depends on VirtualList being at the beginning of Edge
        }

        const T& dereference() const {
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
        VList *vlist_;                                  // (const) VirtualList<Edge> when phase_ is IN_EDGES or OUT_EDGES
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
            Derived old = *derived();
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
            Derived old = *derived();
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

        /** True if iterator doesn't point to anything. */
        bool isEmpty() const {
            if (N_PHASES == phase_) {
                return iter_.isAtEnd();
            } else {
                return vlist_->isHead();
            }
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

        /** True if iterator doesn't point to anything. */
        bool isEmpty() const {
            return base_.base() == NULL;
        }

    private:
        Derived* derived() { return static_cast<Derived*>(this); }
        const Derived* derived() const { return static_cast<const Derived*>(this); }
    };

public:
    /** Bidirectional edge node iterator.
     *
     *  Iterates over the edge nodes in a list, returning the @ref Edge when dereferenced.  Edge iterators are
     *  stable across insert and erase operations.  The difference between @ref EdgeIterator and @ref ConstEdgeIterator
     *  is that the latter returns const references when dereferenced.  An EdgeIterator can be impliciatly converted to a
     *  @ref ConstEdgeIterator, @ref EdgeValueIterator, or @ref ConstEdgeValueIterator. */
    class EdgeIterator: public EdgeBaseIterator<EdgeIterator, Edge, Edge, typename EdgeList::NodeIterator,
                                                VirtualList<Edge> > {
        typedef                EdgeBaseIterator<EdgeIterator, Edge, Edge, typename EdgeList::NodeIterator,
                                                VirtualList<Edge> > Super;
    public:
        typedef Edge& Reference;
        typedef Edge* Pointer;
        EdgeIterator() {}
        EdgeIterator(const EdgeIterator &other): Super(other) {}
        Edge& operator*() const { return this->dereference(); }
        Edge* operator->() const { return &this->dereference(); }
    private:
        friend class Graph;
        EdgeIterator(const typename EdgeList::NodeIterator &base): Super(base) {}
        EdgeIterator(EdgePhase phase, VirtualList<Edge> *vlist): Super(phase, vlist) {}
    };

    /** Bidirectional edge node iterator.
     *
     *  Iterates over the edge nodes in a list, returning the @ref Edge when dereferenced.  Edge iterators are
     *  stable across insert and erase operations.  The difference between @ref EdgeIterator and @ref ConstEdgeIterator
     *  is that the latter returns const references when dereferenced.  A ConstEdgeIterator can be implicitly converted to
     *  a @ref ConstEdgeValueIterator. */
    class ConstEdgeIterator: public EdgeBaseIterator<ConstEdgeIterator, const Edge, const Edge,
                                                     typename EdgeList::ConstNodeIterator,
                                                     const VirtualList<Edge> > {
        typedef                     EdgeBaseIterator<ConstEdgeIterator, const Edge, const Edge,
                                                     typename EdgeList::ConstNodeIterator,
                                                     const VirtualList<Edge> > Super;
    public:
        typedef const Edge& Reference;
        typedef const Edge* Pointer;
        ConstEdgeIterator() {}
        ConstEdgeIterator(const ConstEdgeIterator &other): Super(other) {}
        ConstEdgeIterator(const EdgeIterator &other): Super(other.phase_, other.iter_, other.vlist_) {}
        const Edge& operator*() const { return this->dereference(); }
        const Edge* operator->() const { return &this->dereference(); }
    private:
        friend class Graph;
        ConstEdgeIterator(const typename EdgeList::ConstNodeIterator &base): Super(base) {}
        ConstEdgeIterator(EdgePhase phase, const VirtualList<Edge> *vlist): Super(phase, vlist) {}
    };
    /** @} */

    /** Bidirectional edge value iterator.
     *
     *  Iterates over the edge values in a list, returning the user-defined value (type @ref EdgeValue) when dereferenced.
     *  Edge iterators are stable across insert and erase operations.  The difference between @ref EdgeValueIterator and @ref
     *  ConstEdgeValueIterator is that the latter returns const references when dereferenced.  An EdgeValueIterator can be
     *  impliciatly converted to a @ref ConstEdgeValueIterator. */
    class EdgeValueIterator: public EdgeBaseIterator<EdgeValueIterator, EdgeValue, Edge, typename EdgeList::NodeIterator,
                                                     VirtualList<Edge> > {
        typedef                     EdgeBaseIterator<EdgeValueIterator, EdgeValue, Edge, typename EdgeList::NodeIterator,
                                                     VirtualList<Edge> > Super;
    public:
        typedef EdgeValue& Reference;
        typedef EdgeValue* Pointer;
        EdgeValueIterator() {}
        EdgeValueIterator(const EdgeValueIterator &other): Super(other) {}
        EdgeValueIterator(const EdgeIterator &other): Super(other.phase_, other.iter_, other.vlist_) {}
        EdgeValue& operator*() const { return this->dereference().value(); }
        EdgeValue* operator->() const { return &this->dereference().value(); }
    private:
        friend class Graph;
        EdgeValueIterator(const typename EdgeList::NodeIterator &base): Super(base) {}
        EdgeValueIterator(EdgePhase phase, VirtualList<Edge> *vlist): Super(phase, vlist) {}
    };

    /** Bidirectional edge value iterator.
     *
     *  Iterates over the edge values in a list, returning the user-defined value (type @ref EdgeValue) when dereferenced.
     *  Edge iterators are stable across insert and erase operations.  The difference between @ref EdgeValueIterator and @ref
     *  ConstEdgeValueIterator is that the latter returns const references when dereferenced. */
    class ConstEdgeValueIterator: public EdgeBaseIterator<ConstEdgeValueIterator, const EdgeValue, const Edge,
                                                          typename EdgeList::ConstNodeIterator,
                                                          const VirtualList<Edge> > {
        typedef                          EdgeBaseIterator<ConstEdgeValueIterator, const EdgeValue, const Edge,
                                                          typename EdgeList::ConstNodeIterator,
                                                          const VirtualList<Edge> > Super;
    public:
        typedef const EdgeValue& Reference;
        typedef const EdgeValue* Pointer;
        ConstEdgeValueIterator() {}
        ConstEdgeValueIterator(const ConstEdgeValueIterator &other): Super(other) {}
        ConstEdgeValueIterator(const EdgeValueIterator &other): Super(other.phase_, other.iter_, other.vlist_) {}
        ConstEdgeValueIterator(const EdgeIterator &other): Super(other.phase_, other.iter_, other.vlist_) {}
        ConstEdgeValueIterator(const ConstEdgeIterator &other): Super(other.phase_, other.iter_, other.vlist_) {}
        const EdgeValue& operator*() const { return this->dereference().value(); }
        const EdgeValue* operator->() const { return &this->dereference().value(); }
    private:
        friend class Graph;
        ConstEdgeValueIterator(const typename EdgeList::ConstNodeIterator &base): Super(base) {}
        ConstEdgeValueIterator(EdgePhase phase, const VirtualList<Edge> *vlist): Super(phase, vlist) {}
    };

    /** Bidirectional vertex node iterator.
     *
     *  Iterates over the vertex nodes in a list, returning the @ref Vertex when dereferenced.  Vertex
     *  iterators are stable across insert and erase operations.  The difference between @ref VertexIterator and @ref
     *  ConstVertexIterator is that the latter returns const references when dereferenced.  A VertexIterator can be
     *  impliciatly converted to a @ref ConstVertexIterator, @ref VertexValueIterator, or @ref ConstVertexValueIterator. */
    class VertexIterator: public VertexBaseIterator<VertexIterator, Vertex, Vertex,
                                                    typename VertexList::NodeIterator> {
        typedef                  VertexBaseIterator<VertexIterator, Vertex, Vertex,
                                                    typename VertexList::NodeIterator> Super;
    public:
        typedef Vertex& Reference;
        typedef Vertex* Pointer;
        VertexIterator() {}
        VertexIterator(const VertexIterator &other): Super(other) {}
        Vertex& operator*() const { return this->dereference(); }
        Vertex* operator->() const { return &this->dereference(); }
    private:
        friend class Graph;
        VertexIterator(const typename VertexList::NodeIterator &base): Super(base) {}
    };

    /** Bidirectional vertex node iterator.
     *
     *  Iterates over the vertex nodes in a list, returning the @ref Vertex when dereferenced.  Vertex
     *  iterators are stable across insert and erase operations.  The difference between @ref VertexIterator and @ref
     *  ConstVertexIterator is that the latter returns const references when dereferenced. */
    class ConstVertexIterator: public VertexBaseIterator<ConstVertexIterator, const Vertex, const Vertex,
                                                         typename VertexList::ConstNodeIterator> {
        typedef                       VertexBaseIterator<ConstVertexIterator, const Vertex, const Vertex,
                                                         typename VertexList::ConstNodeIterator> Super;
    public:
        typedef const Vertex& Reference;
        typedef const Vertex* Pointer;
        ConstVertexIterator() {}
        ConstVertexIterator(const ConstVertexIterator &other): Super(other) {}
        ConstVertexIterator(const VertexIterator &other): Super(other.base_) {}
        const Vertex& operator*() const { return this->dereference(); }
        const Vertex* operator->() const { return &this->dereference(); }
    private:
        friend class Graph;
        ConstVertexIterator(const typename VertexList::ConstNodeIterator &base): Super(base) {}
    };
        
    /** Bidirectional vertex value iterator.
     *
     *  Iterates over the vertex values in a list, returning the user-defined value (type @ref VertexValue) when dereferenced.
     *  Vertex iterators are stable across insert and erase operations.  The difference between @ref VertexValueIterator and
     *  @ref ConstVertexValueIterator is that the latter returns const references when dereferenced.  A VertexValueIterator can
     *  be impliciatly converted to a @ref ConstVertexValueIterator. */
    class VertexValueIterator: public VertexBaseIterator<VertexValueIterator, VertexValue, Vertex,
                                                         typename VertexList::NodeIterator> {
        typedef                       VertexBaseIterator<VertexValueIterator, VertexValue, Vertex,
                                                         typename VertexList::NodeIterator> Super;
    public:
        typedef VertexValue& Reference;
        typedef VertexValue* Pointer;
        VertexValueIterator() {}
        VertexValueIterator(const VertexValueIterator &other): Super(other) {}
        VertexValueIterator(const VertexIterator &other): Super(other.base_) {}
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
    class ConstVertexValueIterator: public VertexBaseIterator<ConstVertexValueIterator, const VertexValue, const Vertex,
                                                              typename VertexList::ConstNodeIterator> {
        typedef                            VertexBaseIterator<ConstVertexValueIterator, const VertexValue, const Vertex,
                                                              typename VertexList::ConstNodeIterator> Super;
    public:
        typedef const VertexValue& Reference;
        typedef const VertexValue* Pointer;
        ConstVertexValueIterator() {}
        ConstVertexValueIterator(const ConstVertexValueIterator &other): Super(other) {}
        ConstVertexValueIterator(const VertexValueIterator &other): Super(other.base_) {}
        ConstVertexValueIterator(const VertexIterator &other): Super(other.base_) {}
        ConstVertexValueIterator(const ConstVertexIterator &other): Super(other.base_) {}
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
     *  iterator (@ref EdgeIterator or @ref ConstEdgeIterator) is dereferenced. */
    class Edge {
        VirtualList<Edge> edgeLists_;                   // links for in- and out-edge sublists; MUST BE FIRST
        EdgeValue value_;                               // user-defined data for each edge
        typename EdgeList::NodeIterator self_;          // always points to itself so we can get to IndexedList::Node
        VertexIterator source_, target_;                // starting and ending points of the edge are always required
    private:
        friend class Graph;
        Edge(const EdgeValue &value, const VertexIterator &source, const VertexIterator &target)
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
        const VertexIterator& source() { return source_; }
        ConstVertexIterator source() const { return source_; }
        /** @} */

        /** Target vertex.
         *
         *  All edges in a graph are directed edges, and this method returns an iterator (pointer) to the vertex that serves as
         *  the destination of this edge.
         *
         *  Time complexity is constant.
         *
         * @{ */
        const VertexIterator& target() { return target_; }
        ConstVertexIterator target() const { return target_; }
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
     *  iterator (@ref VertexIterator or @ref ConstVertexIterator) is dereferenced. */
    class Vertex {
        VertexValue value_;                             // user data for this vertex
        typename VertexList::NodeIterator self_;        // always points to itself so we can get to IndexedList::Node
        VirtualList<Edge> edgeLists_;                   // this is the head node; points to the real edges
        size_t nInEdges_;                               // number of incoming edges
        size_t nOutEdges_;                              // number of outgoing edges
    private:
        friend class Graph;
        Vertex(const VertexValue &value): value_(value), nInEdges_(0), nOutEdges_(0) {}
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
         *  another even when the come from different sublists. See @ref EdgeIterator for details.
         *
         *  Time complexity is constant.
         *
         * @{ */
        boost::iterator_range<EdgeIterator> inEdges() {
            EdgeIterator begin(IN_EDGES, &edgeLists_.next(IN_EDGES));
            EdgeIterator end(IN_EDGES, &edgeLists_);
            return boost::iterator_range<EdgeIterator>(begin, end);
        }
        boost::iterator_range<ConstEdgeIterator> inEdges() const {
            ConstEdgeIterator begin(IN_EDGES, &edgeLists_.next(IN_EDGES));
            ConstEdgeIterator end(IN_EDGES, &edgeLists_);
            return boost::iterator_range<ConstEdgeIterator>(begin, end);
        }
        /** @} */

        /** List of outgoing edges.
         *
         *  Returns a sublist of edges whose source vertex is this vertex.  The return value is a pair of iterators which
         *  delineate the edges.  The traversal is in no particular order. Edge iterators are equality-comparable with one
         *  another even when the come from different sublists. See @ref EdgeIterator for details.
         *
         *  Time complexity is constant.
         *
         * @{ */
        boost::iterator_range<EdgeIterator> outEdges() {
            EdgeIterator begin(OUT_EDGES, &edgeLists_.next(OUT_EDGES));
            EdgeIterator end(OUT_EDGES, &edgeLists_);
            return boost::iterator_range<EdgeIterator>(begin, end);
        }
        boost::iterator_range<ConstEdgeIterator> outEdges() const {
            ConstEdgeIterator begin(OUT_EDGES, &edgeLists_.next(OUT_EDGES));
            ConstEdgeIterator end(OUT_EDGES, &edgeLists_);
            return boost::iterator_range<ConstEdgeIterator>(begin, end);
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
    typedef typename GraphIndexTraits<VertexKey, ConstVertexIterator>::Index VertexIndex;
    typedef typename GraphIndexTraits<EdgeKey, ConstEdgeIterator>::Index EdgeIndex;

    EdgeList edges_;                                    // all edges with integer ID numbers and O(1) insert/erase
    VertexList vertices_;                               // all vertices with integer ID numbers and O(1) insert/erase
    EdgeIndex edgeIndex_;                               // optional mapping between EdgeValue and ConstEdgeIterator
    VertexIndex vertexIndex_;                           // optional mapping between VertexValue and ConstVertexIterator

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Serialization
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    friend class boost::serialization::access;

    struct SerializableEdge {
        size_t srcId, tgtId;
        EdgeValue value;

        SerializableEdge()
            : srcId(-1), tgtId(-1) {}

        SerializableEdge(size_t srcId, size_t tgtId, const EdgeValue &value)
            : srcId(srcId), tgtId(tgtId), value(value) {}

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_NVP(srcId);
            s & BOOST_SERIALIZATION_NVP(tgtId);
            s & BOOST_SERIALIZATION_NVP(value);
        }
    };

    template<class S>
    void save(S &s, const unsigned /*version*/) const {
        size_t nv = nVertices();
        s <<BOOST_SERIALIZATION_NVP(nv);
        for (size_t i=0; i<nv; ++i)
            s <<boost::serialization::make_nvp("vertex", findVertex(i)->value());
                                               
        size_t ne = nEdges();
        s <<BOOST_SERIALIZATION_NVP(ne);
        for (size_t i=0; i<ne; ++i) {
            ConstEdgeIterator edge = findEdge(i);
            SerializableEdge se(edge->source()->id(), edge->target()->id(), edge->value());
            s <<BOOST_SERIALIZATION_NVP(se);
        }
    }

    template<class S>
    void load(S &s, const unsigned /*version*/) {
        clear();
        size_t nv = 0;
        s >>BOOST_SERIALIZATION_NVP(nv);
        for (size_t i=0; i<nv; ++i) {
            VertexValue vv;
            s >>boost::serialization::make_nvp("vertex", vv);
            insertVertex(vv);
        }

        size_t ne = 0;
        s >>BOOST_SERIALIZATION_NVP(ne);
        for (size_t i=0; i<ne; ++i) {
            SerializableEdge se;
            s >>BOOST_SERIALIZATION_NVP(se);
            ASSERT_require(se.srcId < nv && se.tgtId < nv);
            insertEdge(findVertex(se.srcId), findVertex(se.tgtId), se.value);
        }
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER();


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
    Graph(const Graph &other)
        : edges_(other.edges_.allocator()), vertices_(other.vertices_.allocator()) {
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
    template<class V2, class E2, class VKey2, class EKey2, class Alloc2>
    Graph(const Graph<V2, E2, VKey2, EKey2, Alloc2> &other, const Allocator &allocator = Allocator())
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
     *  Time complexity is linear in the sum of the number of vertices and edges in this graph and @p other.
     *
     *  <b>Warning:</b> Assignment is not currently exception safe. If an exception occurs (e.g., OOM) then the destination
     *  graph could be left in an inconsistent state. */
    template<class V2, class E2, class VKey2, class EKey2, class Alloc2>
    Graph& operator=(const Graph<V2, E2, VKey2, EKey2, Alloc2> &other) {
        clear();
        for (size_t i=0; i<other.nVertices(); ++i) {
            typename Graph<V2, E2, VKey2, EKey2, Alloc2>::ConstVertexIterator vertex = other.findVertex(i);
            VertexIterator inserted SAWYER_ATTR_UNUSED = insertVertex(VertexValue(vertex->value()));
            ASSERT_require(inserted->id() == i);
        }
        for (size_t i=0; i<other.nEdges(); ++i) {
            typename Graph<V2, E2, VKey2, EKey2, Alloc2>::ConstEdgeIterator edge = other.findEdge(i);
            VertexIterator vsrc = findVertex(edge->source()->id());
            VertexIterator vtgt = findVertex(edge->target()->id());
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
    boost::iterator_range<VertexIterator> vertices() {
        return boost::iterator_range<VertexIterator>(VertexIterator(vertices_.nodes().begin()),
                                                     VertexIterator(vertices_.nodes().end()));
    }
    boost::iterator_range<ConstVertexIterator> vertices() const {
        return boost::iterator_range<ConstVertexIterator>(ConstVertexIterator(vertices_.nodes().begin()),
                                                          ConstVertexIterator(vertices_.nodes().end()));
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
     *  See also @ref findVertexValue and @ref findVertexKey.
     *
     *  @{ */
    VertexIterator findVertex(size_t id) {
        return VertexIterator(vertices_.find(id));
    }
    ConstVertexIterator findVertex(size_t id) const {
        return ConstVertexIterator(vertices_.find(id));
    }
    /** @} */

    /** Finds a vertex given its key.
     *
     *  Finds a vertex having the specified key and returns an itertor pointing to it, or the end iterator if such a vertex
     *  does not exist. The end iterator is always returned for graphs that have no vertex index.
     *
     *  Time complexity depends on the vertex index type, but is usually logarithmic in the number of vertices.
     *
     *  See also @ref findVertex and @ref findVertexValue.
     *
     * @{ */
    VertexIterator findVertexKey(const VertexKey &key) {
        if (Optional<ConstVertexIterator> ov = vertexIndex_.lookup(key))
            return findVertex((*ov)->id());
        return vertices().end();
    }
    ConstVertexIterator findVertexKey(const VertexKey &key) const {
        return vertexIndex_.lookup(key).orElse(vertices().end());
    }
    /** @} */

    /** Finds a vertex given its value.
     *
     *  Finds a vertex having the specified value and returns an iterator pointing to it, or the end iterator if such a vertex
     *  does not exist.  The end iterator is always returned for graphs that have no vertex index.  This method is just a
     *  wrapper around a vertex key constructor followed by a call to @ref findVertexKey for the convenience of the user that
     *  doesn't what to remember how to construct a key.
     *
     *  See also @ref findVertex and @ref findVertexKey.
     *
     * @{ */
    VertexIterator findVertexValue(const VertexValue &value) {
        return findVertexKey(VertexKey(value));
    }
    ConstVertexIterator findVertexValue(const VertexValue &value) const {
        return findVertexKey(VertexKey(value));
    }
    /** @} */

    /** Determines whether the vertex iterator is valid.
     *
     *  Returns true if and only if the specified iterator is not this graph's end iterator and the iterator points to a vertex
     *  in this graph. */
    bool isValidVertex(const ConstVertexIterator &vertex) const {
        return vertex!=vertices().end() && vertex->id()<nVertices() && vertex==findVertex(vertex->id());
    }

    /** Iterators for all edges.
     *
     *  Returns a pair of edge node iterators that deliniate the list of all edges of this graph.  The traversal of this
     *  list is in no particular order.
     *
     *  Time complexity is constant.
     *
     *  @{ */
    boost::iterator_range<EdgeIterator> edges() {
        return boost::iterator_range<EdgeIterator>(EdgeIterator(edges_.nodes().begin()),
                                                   EdgeIterator(edges_.nodes().end()));
    }
    boost::iterator_range<ConstEdgeIterator> edges() const {
        return boost::iterator_range<ConstEdgeIterator>(ConstEdgeIterator(edges_.nodes().begin()),
                                                        ConstEdgeIterator(edges_.nodes().end()));
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
     *  See also @ref findEdgeValue and @ref findEdgeKey.
     *
     *  @{ */
    EdgeIterator findEdge(size_t id) {
        return EdgeIterator(edges_.find(id));
    }
    ConstEdgeIterator findEdge(size_t id) const {
        return ConstEdgeIterator(edges_.find(id));
    }
    /** @} */

    /** Finds an edge given its key.
     *
     *  Finds an edge having the specified key and returns an iterator pointing to it, or the end iterator if such a vertex
     *  does not exist. The end iterator is always returned for graphs that have no edge index.
     *
     *  Time complexity depends on the edge index type, but is usually logarithmic in the number of edges.
     *
     *  See also @ref findEdge and @ref findEdgeValue.
     *
     * @{ */
    EdgeIterator findEdgeKey(const EdgeKey &key) {
        if (Optional<ConstEdgeIterator> oe = edgeIndex_.lookup(key))
            return findEdge((*oe)->id());
        return edges().end();
    }
    ConstEdgeIterator findEdgeKey(const EdgeKey &key) const {
        return edgeIndex_.lookup(key).orElse(edges().end());
    }
    /** @} */

    /** Finds an edge given its value.
     *
     *  Finds an edge having the specified value and returns an iterator pointing to it, or the end iterator if such an edge
     *  does not exist.  The end iterator is always returned for graphs that have no edge index.  This method is just a
     *  wrapper around an edge key constructor followed by a call to @ref findEdgeKey for the convenience of the user that
     *  doesn't what to remember how to construct a key.
     *
     *  See also @ref findEdge and @ref findEdgeKey.
     *
     * @{ */
    EdgeIterator findEdgeValue(const EdgeValue &value) {
        return findEdgeKey(EdgeKey(value));
    }
    ConstEdgeIterator findEdgeValue(const EdgeValue &value) const {
        return findEdgeValue(EdgeKey(value));
    }
    /** @} */

    /** Determines whether the edge iterator is valid.
     *
     *  Returns true if and only if the specified iterator is not this graph's end iterator and the iterator points to an edge
     *  in this graph. */
    bool isValidEdge(const ConstEdgeIterator &edge) const {
        return edge!=edges().end() && edge->id()<nEdges() && edge==findEdge(edge->id());
    }

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
     *  If this graph has a vertex index and a vertex with the same key already exists then an @ref Exception::AlreadyExists is
     *  thrown. See also @ref insertVertexMaybe.
     *
     *  Time complexity is constant for graphs without a vertex index. Looking up the vertex in the index has time complexity
     *  depending on the type of index (usually logarithmic in the number of vertices). */
    VertexIterator insertVertex(const VertexValue &value = VertexValue()) {
        return insertVertexImpl(value, true /*strict*/);
    }

    /** Optionally insert a new vertex.
     *
     *  Same as @ref insertVertex except if this graph has a vertex index and a vertex already exists with the same key then a
     *  new vertex is not inserted and the iterator of the existing vertex is returned instead.  This function always inserts a
     *  new vertex for graphs that do not have a vertex index.
     *
     *  Time complexity is constant for graphs without a vertex index. Looking up the vertex in the index has time complexity
     *  depending on the type of index (usually logarithmic in the number of vertices). */
    VertexIterator insertVertexMaybe(const VertexValue &value) {
        return insertVertexImpl(value, false /*non-strict*/);
    }
    
    /** Insert a new edge.
     *
     *  Inserts a new edge and copies @p value (if specified, or else default-constructed) into the edge node.  Returns an
     *  iterator that points to the new edge.  All other edge iterators that were not already positioned at the one-past-last
     *  edge will eventually traverse this new edge; no iterators, edge or vertex, are invalidated.  The new edge is given the
     *  highest edge ID number; no other ID numbers, edge or vertex, change.
     *
     *  If this graph has an edge index and an edge with the same key already exists then an @ref Exception::AlreadyExists is
     *  thrown. See also @ref insertEdgeMaybe.
     *
     *  Time complexity is constant for graphs without an edge index. Looking up the edge in the index has time complexity
     *  depending on the type of index (usually logirithmic in the number of edges).
     *
     * @{ */
    EdgeIterator insertEdge(const VertexIterator &sourceVertex, const VertexIterator &targetVertex,
                            const EdgeValue &value = EdgeValue()) {
        return insertEdgeImpl(sourceVertex, targetVertex, value, true /*strict*/);
    }
    EdgeIterator insertEdge(const ConstVertexIterator &sourceVertex, const ConstVertexIterator &targetVertex,
                            const EdgeValue &value = EdgeValue()) {
        ASSERT_require(isValidVertex(sourceVertex));
        ASSERT_require(isValidVertex(targetVertex));
        return insertEdge(findVertex(sourceVertex->id()), findVertex(targetVertex->id()), value);
    }
    /** @} */

    /** Optionally insert a new edge.
     *
     *  Same as @ref insertEdge except if this graph has an edge index and an edge already exists with the same key then a new
     *  edge is not inserted and the iterator of the existing edge is returned instead.  This function always inserts a new
     *  edge for graphs that do not have an edge index.
     *
     *  Time complexity is constant for graphs without an edge index. Looking up the edge in the index has time complexity
     *  depending on the type of index (usually logirithmic in the number of edges).
     *
     * @{ */
    EdgeIterator insertEdgeMaybe(const VertexIterator &sourceVertex, const VertexIterator &targetVertex,
                                 const EdgeValue &value = EdgeValue()) {
        return insertEdgeImpl(sourceVertex, targetVertex, value, false /*non-strict*/);
    }
    EdgeIterator insertEdgeMaybe(const ConstVertexIterator &sourceVertex, const ConstVertexIterator &targetVertex,
                                 const EdgeValue &value = EdgeValue()) {
        ASSERT_require(isValidVertex(sourceVertex));
        ASSERT_require(isValidVertex(targetVertex));
        return insertEdgeMaybe(findVertex(sourceVertex->id()), findVertex(targetVertex->id()), value);
    }
    /** @} */

    /** Insert an edge and its vertex end points.
     *
     *  Invoke @ref insertVertexMaybe for both given vertex values, and then invokes @ref insertEdge to connect the two
     *  vertices with an edge. */
    EdgeIterator insertEdgeWithVertices(const VertexValue &sourceValue, const VertexValue &targetValue,
                                        const EdgeValue &edgeValue = EdgeValue()) {
        VertexIterator source = insertVertexMaybe(sourceValue);
        VertexIterator target = insertVertexMaybe(targetValue);
        return insertEdge(source, target, edgeValue);
    }

    /** Erases an edge.
     *
     *  The edge specified by the iterator (which must not be a one-past-last iterator) is erased from the graph. The term
     *  "erasure" is Standard Template Library terminology for the withdrawal and deletion of an object from a container, and
     *  differs from the term "remove", which means to move an object to some near-the-end position in a container.  Any edge
     *  iterator that was pointing at the erased edge becomes invalid and should not be subsequently dereferenced,
     *  incremented, decremented, or compared; other iterators, edge and vertex, are unaffected.  The edge with the highest ID
     *  number will be given the ID of the edge that was erased in order to fill the gap left in the ID sequence.  This method
     *  returns an iterator for the edge following the one that was erased (possibly the one-past-last iterator if the last
     *  edge was erased).
     *
     *  Time complexity is constant unless the graph has an edge index, in which case time complexity is dependent on the index
     *  type (usually logarithmic in the number of edges).
     *
     * @{ */
    EdgeIterator eraseEdge(const EdgeIterator &edge) {
        ASSERT_require(isValidEdge(edge));
        EdgeIterator next = edge; ++next;               // advance before we delete edge
        edgeIndex_.erase(EdgeKey(edge->value()));
        --edge->source_->nOutEdges_;
        edge->edgeLists_.remove(OUT_EDGES);
        --edge->target_->nInEdges_;
        edge->edgeLists_.remove(IN_EDGES);
        edges_.eraseAt(edge->self_);                    // edge is now deleted
        return next;
    }
    EdgeIterator eraseEdge(const ConstEdgeIterator &edge) {
        ASSERT_require(isValidEdge(edge));
        return eraseEdge(findEdge(edge->id()));
    }
    /** @} */

    /** Erases and edge and possibly vertices.
     *
     *  Erases the specified edge. If this results in the source vertex having no incoming or outgoing edges then the source
     *  vertex is also erased. Similarly for the target vertex when the edge is not a self edge.  Erasing of the vertices and
     *  edges has the semantics of @ref eraseVertex and @ref eraseEdges, including the affects on iterators and ID numbers, and
     *  time complexity.
     *
     *  Returns an iterator for the edge following the one that was erased (possibly the on-past-last iterator if the last edge
     *  was erased). */
    EdgeIterator eraseEdgeWithVertices(const EdgeIterator &edge) {
        ASSERT_require(isValidEdge(edge));
        VertexIterator source = edge->source();
        VertexIterator target = edge->target();
        EdgeIterator retval = eraseEdge(edge);
        if (source == target) {
            if (source->degree() == 0)
                eraseVertex(source);
        } else {
            if (source->degree() == 0)
                eraseVertex(source);
            if (target->degree() == 0)
                eraseVertex(target);
        }
        return retval;
    }

    /** Erases all edges connecting two vertices.
     *
     *  Given two vertex iterators, erase all edges whose source is the first vertex and whose target is the second vertex.
     *
     *  For graphs without an edge index, time complexity is linear in the number of incoming or outgoing edges (whichever is
     *  smaller). If an edge index is present then time complexity depends on the type of edge index (most indexes have
     *  logarithmic lookup time).
     *
     * @{ */
    void eraseEdges(const VertexIterator &source, const VertexIterator &target) {
        ASSERT_require(isValidVertex(source));
        ASSERT_require(isValidVertex(target));
        if (source->nOutEdges() < target->nInEdges()) {
            EdgeIterator iter = source->outEdges().begin();
            while (iter != source->outEdges().end()) {
                if (iter->target() == target) {
                    iter = eraseEdge(iter);
                } else {
                    ++iter;
                }
            }
        } else {
            EdgeIterator iter = target->inEdges().begin();
            while (iter != target->inEdges().end()) {
                if (iter->source() == source) {
                    iter = eraseEdge(iter);
                } else {
                    ++iter;
                }
            }
        }
    }
    void eraseEdges(const ConstVertexIterator &source, const ConstVertexIterator &target) {
        ASSERT_require(isValidVertex(source));
        ASSERT_require(isValidVertex(target));
        eraseEdges(findVertex(source->id()), findVertex(target->id()));
    }
    /** @} */
    
    /** Erases a vertex and its incident edges.
     *
     *  The vertex specified by the iterator (which must not be a one-past-last iterator) is erased from the graph along with
     *  all edges that originate from or terminate at that vertex. The term "erasure" is Standard Template Library terminology
     *  for the withdrawal and deletion of an object from a container, and differs from the term "remove", which means to move
     *  an object to some near-the-end position in a container.  Any iterator that was pointing at the erased vertex or any of
     *  its incident edges becomes invalid and should not be subsequently dereferenced, incremented, decremented, or compared;
     *  other iterators, edge and vertex, are unaffected.  The vertex with the highest ID number will be given the ID of the
     *  vertex that was erased in order to fill the gap left in the ID sequence.  This method returns an iterator for the
     *  vertex following the one that was erased (possibly the one-past-last iterator if the last vertex was erased).
     *
     *  For a vertex with no incident edges, time complexity is constant unless the graph has a vertex index in which case it
     *  depends on the type of index (most vertex indexes have logarithmic lookup/erase time).  If the vertex being erased has
     *  incoming or outgoing edges then the implementation also calls @ref eraseEdges.
     *
     * @{ */
    VertexIterator eraseVertex(const VertexIterator &vertex) {
        ASSERT_require(isValidVertex(vertex));
        VertexIterator next = vertex; ++next;       // advance before we delete vertex
        clearEdges(vertex);
        vertexIndex_.erase(VertexKey(vertex->value()));
        vertices_.eraseAt(vertex->self_);               // vertex is now deleted
        return next;
    }
    VertexIterator eraseVertex(const ConstVertexIterator &vertex) {
        ASSERT_require(isValidVertex(vertex));
        return eraseVertex(findVertex(vertex->id()));
    }
    /** @} */

    /** Erase all edges, but leave all vertices.
     *
     *  This method erases (withdraws and deletes) all edges but leaves all vertices. It is logically equivalent to calling
     *  @ref eraseEdge for each edge, but is more efficient.
     *
     *  Time complexity is linear in the number of edges erased. */
    void clearEdges() {
        for (VertexIterator vertex=vertices().begin(); vertex!=vertices().end(); ++vertex) {
            vertex->inEdges().reset();
            vertex->outEdges().reset();
        }
        edges_.clear();
        edgeIndex_.clear();
    }

    /** Erase all edges incident to a vertex.
     *
     *  This method erases (withdraws and deletes) all edges that are incident to the specified vertex. That is, all edges
     *  whose source or target is the vertex.  It is logically equivalent to calling @ref clearOutEdges followed by @ref
     *  clearInEdges, and has the same effects on iterators and edge ID numbers as erasing edges individually.
     *
     *  Time complexity is linear in the number of edges erased, multiplied by the time complexity for edge index lookups if
     *  any. Most edge indexes have logarithmic lookup time.
     *
     * @{ */
    void clearEdges(const VertexIterator &vertex) {
        clearOutEdges(vertex);
        clearInEdges(vertex);
    }
    void clearEdges(const ConstVertexIterator &vertex) {
        clearOutEdges(vertex);
        clearInEdges(vertex);
    }
    /** @} */

    /** Erase all edges emanating from a vertex.
     *
     *  This method erases (withdraws and deletes) all edges whose source is the specified vertex.  It has the same effects on
     *  iterators and edge ID numbers as erasing edges individually.
     *
     *  Time complexity is linear in the number of edges erased, multiplied by the time complexity for edge index lookups if
     *  any. Most edge indexes have logarithmic lookup time.
     *
     * @{ */
    void clearOutEdges(const VertexIterator &vertex) {
        ASSERT_forbid(vertex==vertices().end());
        for (EdgeIterator edge=vertex->outEdges().begin(); edge!=vertex->outEdges().end(); /*void*/)
            edge = eraseEdge(edge);
    }
    void clearOutEdges(const ConstVertexIterator &vertex) {
        ASSERT_forbid(vertex==vertices().end());
        clearOutEdges(findVertex(vertex->id()));
    }
    /** @} */

    /** Erase all edges targeting a vertex.
     *
     *  This method erases (withdraws and deletes) all edges whose target is the specified vertex.  It has the same effects on
     *  iterators and edge ID numbers as erasing edges individually.
     *
     *  Time complexity is linear in the number of edges erased, multiplied by the time complexity for edge index lookups if
     *  any. Most edge indexes have logarithmic lookup time..
     *
     * @{ */
    void clearInEdges(const VertexIterator &vertex) {
        ASSERT_forbid(vertex==vertices().end());
        for (EdgeIterator edge=vertex->inEdges().begin(); edge!=vertex->inEdges().end(); /*void*/)
            edge = eraseEdge(edge);
    }
    void clearInEdges(const ConstVertexIterator &vertex) {
        ASSERT_forbid(vertex==vertices().end());
        clearInEdges(findVertex(vertex->id()));
    }
    /** @} */

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
        edgeIndex_.clear();
        vertexIndex_.clear();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Internal implementation details
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    VertexIterator insertVertexImpl(const VertexValue &value, bool strict) {
        const VertexKey key(value);
        if (Optional<ConstVertexIterator> found = vertexIndex_.lookup(key)) {
            if (strict)
                throw Exception::AlreadyExists("cannot insert duplicate vertex when graph vertices are indexed");
            return findVertex((*found)->id());
        }
        typename VertexList::NodeIterator inserted = vertices_.insert(vertices_.nodes().end(), Vertex(value));
        inserted->value().self_ = inserted;
        inserted->value().edgeLists_.reset(NULL);       // this is a sublist head, no edge node
        VertexIterator retval = VertexIterator(inserted);
        vertexIndex_.insert(key, retval);
        return retval;
    }

    EdgeIterator insertEdgeImpl(const VertexIterator &sourceVertex, const VertexIterator &targetVertex,
                                const EdgeValue &value, bool strict) {
        const EdgeKey key(value);
        ASSERT_require(isValidVertex(sourceVertex));
        ASSERT_require(isValidVertex(targetVertex));
        if (Optional<ConstEdgeIterator> found = edgeIndex_.lookup(key)) {
            if (strict)
                throw Exception::AlreadyExists("cannot insert duplicate edge when graph edges are indexed");
            return findEdge((*found)->id());
        }
        typename EdgeList::NodeIterator inserted = edges_.insert(edges_.nodes().end(), Edge(value, sourceVertex, targetVertex));
        inserted->value().self_ = inserted;
        inserted->value().edgeLists_.reset(&inserted->value());
        EdgeIterator newEdge(inserted);
        sourceVertex->edgeLists_.insert(OUT_EDGES, &newEdge->edgeLists_);
        ++sourceVertex->nOutEdges_;
        targetVertex->edgeLists_.insert(IN_EDGES, &newEdge->edgeLists_);
        ++targetVertex->nInEdges_;
        edgeIndex_.insert(key, newEdge);
        return newEdge;
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated stuff
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    // Deprecated [Robb Matzke 2015-03-28]: to be removed on or after 2015-09-28
    typedef Edge EdgeNode SAWYER_DEPRECATED("use Edge instead");
    typedef Vertex VertexNode SAWYER_DEPRECATED("use Vertex instead");
    typedef EdgeIterator EdgeNodeIterator SAWYER_DEPRECATED("use EdgeIterator instead");
    typedef ConstEdgeIterator ConstEdgeNodeIterator SAWYER_DEPRECATED("use ConstEdgeIterator instead");
    typedef VertexIterator VertexNodeIterator SAWYER_DEPRECATED("use VertexIterator instead");
    typedef ConstVertexIterator ConstVertexNodeIterator SAWYER_DEPRECATED("use ConstVertexIterator instead");
};

} // namespace
} // namespace

#endif
