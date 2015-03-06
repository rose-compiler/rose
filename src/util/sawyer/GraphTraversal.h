#ifndef Sawyer_GraphTraversal_H
#define Sawyer_GraphTraversal_H

#include <sawyer/Graph.h>

#include <sawyer/BitVector.h>
#include <list>
#include <vector>

namespace Sawyer {
namespace Container {

/** Algorithms that operate on container classes. */
namespace Algorithm {

/** Events returned by a graph traversal.
 *
 *  The graph traversal @ref GraphTraversal::advance "advance" method single steps through a traversal and returns control to
 *  the caller whenever a desired stopping point is reached.  The stopping points are specified by a bit mask of these event
 *  type constants.
 *
 *  Besides the enumerated constants, the following @c unsigned bit masks are also defined:
 *
 *  @li @c VERTEX_EVENTS: all events related to vertices, namley @ref ENTER_VERTEX, @ref LEAVE_VERTEX, and @ref
 *      DISCOVER_VERTEX.
 *  @li @c EDGE_EVENTS: all events related to edges, namely @ref ENTER_EDGE and @ref LEAVE_EDGE.
 *  @li @c ENTER_EVENTS: all events for entering a node, namely @ref ENTER_VERTEX and @ref ENTER_EDGE.
 *  @li @c LEAVE_EVENTS: all events for leaving a node, namely @ref LEAVE_VERTEX and @ref LEAVE_EDGE.
 *  @li @c ENTER_LEAVE_EVENTS: the union of @c ENTER_EVENTS and @c LEAVE_EVENTS.
 *  @li @c ALL_EVENTS: just like it sounds. */
enum TraversalEvent {
    NO_EVENT        = 0,      /**< No event. */
    ENTER_VERTEX    = 0x0001, /**< Vertex entered for first time.  The traversal will return the vertex just entered and the
                               *   edge by which it was entered. If the vertex was entered due to being set explicitly as the
                               *   current vertex, then the edge will be an end iterator. */
    ENTER_EDGE      = 0x0002, /**< Edge is entered.  Edges are entered after their originating vertex is entered and possibly
                               *   before discovering the vertex at the far end of the edge.  We use terms "originating" and
                               *   "far end" because traversals can flow in the natural direction of the edge (from source to
                               *   target) or in reverse (from target to source).  When stopped at this event, the traversal
                               *   will return the entered edge along with the vertex from which it was entered. If the edge
                               *   was set explicitly as a traversal position then the vertex will be an end iterator. */
    DISCOVER_VERTEX = 0x0004, /**< Neighboring vertex discovered for the first time.  Vertex discover happens after an edge is
                               *   entered.  When stopped at this event, the traversal will return the vertex that is being
                               *   discovered and the edge by which it was discovered. If the vertex was explicitly set as the
                               *   traversal's current position then the edge will be an end iterator. */
    LEAVE_EDGE      = 0x0008, /**< Leaving edge. The traversal eventually leaves all edges that were entered. In a
                               *   breadth-first search the traversal leaves an entered edge before entering any other edge,
                               *   while in a depth-first traversal the stack of entered edges may become quite deep.  A
                               *   traversal stopped at this event returns the edge which is being left and the vertex from
                               *   which the edge was originally entered. If the edge was set explicitly then the vertex will
                               *   be an end iterator. */
    LEAVE_VERTEX    = 0x0010, /**< Leaving vertex.  The traversal eventually leaves all vertices that were entered. A vertex is
                               *   left after entering and leaving all the edges that originate (for the purpose of traversal)
                               *   from the vertex.  In a breadth-first search the traversal leaves an entered vertex before
                               *   entering any other vertex, while in a depth-first search the stack of entered vertices may
                               *   become quite deep.  A traversal stopped at this event returns the vertex which is being left
                               *   and the edge by which the vertex was originally entered.  If the vertex was set as an
                               *   explicit traversal position then the edge will be an end iterator. */
    FOLLOW_EDGE     = 0x0020, // Internal: current edge was followed to find neighbor vertex
};

// Event sets (doxygen doesn't pick these up, so they're documented in the TraversalEvent enum
static const unsigned VERTEX_EVENTS = ENTER_VERTEX | DISCOVER_VERTEX | LEAVE_VERTEX;
static const unsigned EDGE_EVENTS = ENTER_EDGE | LEAVE_EDGE;
static const unsigned ENTER_EVENTS = ENTER_VERTEX | ENTER_EDGE;
static const unsigned LEAVE_EVENTS = LEAVE_VERTEX | LEAVE_EDGE;
static const unsigned ENTER_LEAVE_EVENTS = ENTER_EVENTS | LEAVE_EVENTS;
static const unsigned ALL_EVENTS = VERTEX_EVENTS | EDGE_EVENTS;

/** Event name from constant. */
SAWYER_EXPORT std::string traversalEventName(TraversalEvent);

/** Direction tag for forward traversals. */
class ForwardTraversalTag {};

/** Direction tag for reverse traversals. */
class ReverseTraversalTag {};

/** Order tag for depth-first traversals. */
class DepthFirstTraversalTag {};

/** Order tag for breadth-first traversals. */
class BreadthFirstTraversalTag {};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Base class for graph traversals.
 *
 *  Graph traversals are similar to single-pass, forward iterators in that a traversal object points to a particular node
 *  (vertex or edge) in a graph and supports dereference and increment operations.  The dereference operator returns a
 *  reference to the node and the increment operator advances the traversal object so it points to the next node.
 *
 *  Traversals come in a variety of dimensions:
 *
 *  @li A graph traversal can traverse over a constant graph or a mutable graph. When traversing over constant graphs, the
 *      traversal object returns constant nodes (vertices or edges depending on the traversal kind). When traversing over a
 *      mutable graph it returns mutable nodes which are permitted to be modified during the traversal. The connectivity of a
 *      graph should not be modified while a traversal is in progress; i.e., traversals are unstable over insertion and erasure
 *      and the traversal will have undefined behavior.
 *
 *  @li A graph traversal can be depth-first or breadth-first.  A depth-first traversal will follow edges immediately, quickly
 *      moving away from an initial vertex as far as possible before eventually backtracking to follow another edge.  A
 *      breadth-first traversal will follow each of the edges from a single vertex before moving to another vertex.
 *
 *  @li A graph traversal can be in a forward or reverse direction.  Forward traversals flow along edges in their natural
 *      direction from source to target.  Reverse traversals are identical in every respect except they flow backward across
 *      edges.
 *
 *  Regardless of the type of traversal, it will normally visit each reachable vertex and edge exactly one time. However, the
 *  user may adjust the visited state of vertices and edges during the traversal, may change the current position of the
 *  traversal, and may cause the traversal to skip over parts of a graph.
 *
 *  The basic traversal advancing operation is the @ref advance method upon which other advancing methods and operators are
 *  built.  When a traversal is constructed the user (or a subclass) supplies a set of events that are of interest and the @ref
 *  advance method will return control when any of those event points are reached.  For instance, in a traversal that's being
 *  used to calculate which vertices are reachable from an initial vertex the user is probably only interested in @ref
 *  ENTER_VERTEX events.  When control is returned to the user the @ref event, @ref vertex, and @ref edge methods can be used
 *  to get information about why and where the traversal stopped.  The @ref isAtEnd method will indicate whether the traversal
 *  is completed.
 *
 *  The following subclasses are implemented. Their names follow the pattern @em Order, @em
 *  Direction, @em Visiter. For instance "DepthFirstForwardEdgeTraversal" visits nodes in a "DepthFirst" order, follows edges
 *  in their natural forward direction (from source to target), stops only at @ref ENTER_EDGE events, and returns edges
 *  when dereferenced.  The orders are "DepthFirst" or "BreadthFirst".  The directions are "Forward" and "Reverse". The
 *  visitors are "Edge" (only @ref ENTER_EDGE events), "Vertex" (only @ref ENTER_VERTEX events), and "Graph" (user specified
 *  events defaulting to all events).
 *
 *  @li DepthFirstForwardGraphTraversal
 *  @li DepthFirstReverseGraphTraversal
 *  @li BreadthFirstForwardGraphTraversal
 *  @li BreadthFirstReverseGraphTraversal
 *  @li DepthFirstForwardEdgeTraversal
 *  @li DepthFirstReverseEdgeTraversal
 *  @li BreadthFirstForwardEdgeTraversal
 *  @li BreadthFirstReverseEdgeTraversal
 *  @li DepthFirstForwardVertexTraversal
 *  @li DepthFirstReverseVertexTraversal
 *  @li BreadthFirstForwardVertexTraversal
 *  @li BreadthFirstReverseVertexTraversal
 *
 *  Although graph traversals have functionality similar to iterators, there are some important differences to keep in mind:
 *
 *  @li Iterators are lightweight objects typically a few bytes that can be passed around efficiently by value, whereas a
 *      traversal's size is linear with respect to the size of the graph over which it traverses.  The reason for this is that
 *      a traversal must keep track of which nodes have been visited.
 *  @li Advancing an iterator takes constant time but advancing a traversal can take longer.
 *  @li Iterators can be incremented and decremented to move the pointer forward and backward along a list of nodes, but
 *      traversals can only be advanced in one direction per traversal object.
 *  @li Iterating requires two iterators--the current position and an end position; traversing requires only
 *      one traversal object which keeps track of its own state.
 *  @li Iterators are copyable and comparable, traversals are not.
 *  @li %Graph iterators are insert and erase stable, traversals are not.
 *  @li Iterators do not follow connectivity, but that's the main point for traversals.
 *  @li Iterators are a core feature of a graph. Traversals are implemented in terms of iterators and don't provide any
 *      functionality that isn't already available to the user through the Graph API.
 *
 * @section example Examples
 *
 *  The examples in this section assume the following context:
 *
 * @code
 *  #include <sawyer/GraphTraversal.h>
 *
 *  using namespace Sawyer::Container;
 *  using namespace Sawyer::Container::Algorithm;
 *
 *  typedef ... MyVertexType;   // User data for vertices; any default-constructable, copyable type
 *  typedef ... MyEdgeType;     // Ditto for edges. These can be Sawyer::Nothing if desired.
 *
 *  typedef Graph<MyVertexType, MyEdgeType> MyGraph;
 *  MyGraph graph;
 * @endcode
 *
 *  This example shows how to build a reachability vector.  The vector is indexed by vertex ID number and is true when the
 *  vertex is reachable from the starting vertex by following edges in their natural direction.  The starting point is a
 *  vertex, but it could just as well have been an edge that points to the first vertex.  One could also create edge
 *  reachability vectors by using an edge traversal instead.  The @ref GraphVertexTraversal::next "next" method returns the
 *  current vertex and advances the traversal to the next vertex.
 *
 * @code
 *  MyGraph::VertexNodeIterator startingVertex = ....;
 *  std::vector<bool> reachable(graph.nVertices(), false);
 *
 *  DepthFirstForwardVertexTraversal<MyGraph> traversal(graph, startingVertex);
 *  while (traversal.hasNext())
 *      reachable[traversal.next().id()] = true;
 * @endcode
 *
 *  A more typical C++ iterator paradigm can be used instead. However, traversals are not comparable and there is no "end"
 *  traversal like there is for iterators.  Evaluating a traversal in Boolean context returns true unless the traversal is at
 *  the end.  Here's the same loop again:
 *
 * @code
 *  typedef DepthFirstForwardVertexTraversal<MyGraph> Traversal;
 *  for (Traversal traversal(graph, start); traversal; ++traversal)
 *      reachable[traversal->id()] = true;
 * @endcode
 *
 *  The traversals whose names end with "GraphTraversal", such as @ref DepthFirstForwardGraphTraversal, can be used if one
 *  needs to gain control more frequently than just entering vertices.  For instance, one can count the depth of a vertex in a
 *  depth-first traversal by getting control on both @ref ENTER_VERTEX and @ref LEAVE_VERTEX events, like this:
 *
 * @code
 *  typedef DepthFirstForwardGraphTraversal<MyGraph> Traversal;
 *  size_t depth = 0;
 *  std::vector<size_t> depthOfVertex(graph.nVertices(), 0);
 *
 *  for (Traversal t(graph, start, ENTER_VERTEX|LEAVE_VERTEX); t; ++t) {
 *      if (t.event()==ENTER_VERTEX) {
 *          ++depth;
 *      } else {
 *          depthOfVertex[t.vertex()->id()] = --depth;
 *      }
 *  }
 * @endcode
 *
 *  All traversals support skipping over parts of the graph by using the @ref skipChildren method. If this method is called
 *  during an @ref ENTER_VERTEX event then no edges of that vertex are visited, and if called during an @ref ENTER_EDGE event,
 *  then no neighbor vertex is discovered at the far end of the edge (although it might be discovered by other edges).  For
 *  instance, the following code computes a vertex reachability vector for a control flow graph but does not follow edges that
 *  are marked as function calls.  The @c value method returns a reference to a @c MyEdgeType (see typedef example above) which
 *  we assume has a @c isFunctionCall method.
 *
 * @code
 *  MyGraph::VertexNodeIterator startingVertex = ...;
 *  std::vector<bool> reachable(graph.nVertices(), false);
 *  typedef DepthFirstForwardGraphTraversal<MyGraph> Traversal;
 *
 *  for (Traversal t(graph, startingVertex, ENTER_EVENTS); t; ++t) {
 *      if (t.event() == ENTER_VERTEX) {
 *          reachable[t.vertex()->id()] = true;
 *      } else if (t.edge()->value().isFunctionCall()) {
 *          ASSERT_require(t.event() == ENTER_EDGE);
 *          t.skipChildren();
 *      }
 *  }
 * @endcode
 *
 *  Traversals are suitable for using in generic functions as well.  For instance, the next example is a generic function that
 *  prints a graph using a depth-first traversal.  It operates on const or non-const graphs for demonstration purposes; in real
 *  life one might advertise that the function doesn't modify the graph by having an explicit const qualifier. The @ref
 *  GraphTraits is used to obtain the appropriate const/non-const vertex iterator type.
 *
 * @code
 *  template<class Graph>
 *  void printGraph(std::ostream &out, Graph &graph,
 *                  typename GraphTraits<Graph>::VertexNodeIterator start) {
 *      for (DepthFirstForwardGraphTraversal t(graph, start, ENTER_EVENTS); t; ++t) {
 *          if (t.event() == ENTER_VERTEX) {
 *              out <<"vertex " <<t->vertex()->id() <<"\t= " <<t->vertex()->value() <<"\n";
 *          } else {
 *              ASSERT_require(t.event() == ENTER_EDGE);
 *              out <<"edge   " <<t->edge()->id() <<"\t= " <<t->edge()->value() <<"\n";
 *          }
 *      }
 *  }
 * @endcode
 *
 *  The following example shows one way to construct a new graph from an existing graph.  Although graphs can be copy
 *  constructed from related graphs as long as the destination graph's vertices and edges can be copy constructed from the
 *  source graph's vertices and edges, this is not always the situation encountered. One often needs to construct a new graph
 *  whose edges and vertices cannot be copy constructed, where certain edges or vertices should not be copied, or where certain
 *  extra vertices or edges need to be inserted.  A combination of traversal and vertex lookup tables can be convenient in this
 *  situation.  For instance, consider a program control flow graph (CFG) where each vertex represents a sequence of machine
 *  instructions and each edge is a possible transfer of control.  Assume that the source graph has three edge types: INTERFUNC
 *  is an inter-function edge such as a function call, INTRAFUNC are function-internal edges, and ADJUST is a special kind of
 *  INTRAFUNC edge.  We want to create a new graph that contains only vertices that belong to a single function, and any ADJUST
 *  edge in the source should result in an edge-vertex-edge in the destination where the vertex is marked as ADJUST. The
 *  destination graph edges carry no information and thus cannot be copy-constructed from the source graph's edges.
 *
 * @code
 *   typedef Graph<CfgVertex, CfgEdge> Cfg;
 *   typedef Graph<DfVertex> DfCfg;
 *   typedef DepthFirstGraphTraversal<Cfg> Traversal;
 *
 *   Cfg cfg = ...;
 *   Cfg::VertexNodeIterator startVertex = ...;
 *
 *   DfCfg dfCfg;
 *   Map<size_t, DfCfg::VertexNodeIterator> vmap;
 *   
 *   for (Traversal t(cfg, startVertex, ENTER_EVENTS|LEAVE_EDGE); t; ++t) {
 *       if (t.event() == ENTER_VERTEX) {
 *           // Insert each vertex before we visit any edge going into that vertex
 *           DfCfg::VertexNodeIterator v = dfCfg.insertVertex(NORMAL);
 *           vmap.insert(t.vertex()->id(), v);
 *       } else if (t.event() == ENTER_EDGE && t.edge()->value().type() == INTERFUNC) {
 *           // Don't traverse edges that cross function boundaries
 *           t.skipChildren();
 *       } else if (vmap.exists(t.edge()->source()->id()) && vmap.exists(t.edge()->target()->id())) {
 *           // Insert an edge provided we have both of its endpoints
 *           DfCfg::VertexNodeIterator source = vmap[t.edge()->source()->id()];
 *           DfCfg::VertexNodeIterator target = vmap[t.edge()->target()->id()];
 *           if (t.edge()->value().type() == ADJUST) {
 *               DfCfg::VertexNodeIterator v = dfCfg.insertVertex(ADJUST);
 *               dfCfg.insertEdge(source, v);
 *               dfCfg.insertEdge(v,target);
 *           } else {
 *               dfCfg.insertEdge(source,target);
 *           }
 *       }
 *   }
 * @endcode */
template<class Graph, class Order=DepthFirstTraversalTag, class Direction=ForwardTraversalTag>
class GraphTraversal {
public:
    /** Const or non-const vertex node iterator. */
    typedef typename GraphTraits<Graph>::VertexNodeIterator VertexNodeIterator;

    /** Const or non-const edge node iterator. */
    typedef typename GraphTraits<Graph>::EdgeNodeIterator EdgeNodeIterator;

private:
    Graph &graph_;

protected:
    // Work that needs to be performed.  The current item is always at the front of this list. Depth-first subclasses will push
    // new work onto the front of the list and breadth-first subclasses will push it onto the end.  When the list becomes empty
    // then the iterator has reached its end state.
    struct Work {
        TraversalEvent event;                           // last event returned from this work item
        EdgeNodeIterator fromEdge;                      // edge that brought us to this vertex (or edges().end())
        VertexNodeIterator vertex;                      // vertex being visited
        EdgeNodeIterator edge;                          // edge being visited
        EdgeNodeIterator endEdge;                       // end edge for this vertex
        bool followEdge;                                // follow edge to discover the neighbor vertex?
        Work(EdgeNodeIterator fromEdge, VertexNodeIterator vertex, const boost::iterator_range<EdgeNodeIterator> &nextEdges)
            : event(DISCOVER_VERTEX), fromEdge(fromEdge), vertex(vertex), edge(nextEdges.begin()), endEdge(nextEdges.end()),
              followEdge(true) {}
    };
    typedef std::list<Work> WorkList;
    WorkList workList_;

private:
    unsigned significantEvents_;                        // events for which advance() will return to the caller
    BitVector verticesDiscovered_;                      // vertices that are (or have been) on the work list
    BitVector edgesVisited_;                            // edges that have been visited
    Optional<Work> latestDiscovered_;                   // most recent work discovered

protected:
    GraphTraversal(Graph &graph, unsigned significantEvents)
        : graph_(graph), significantEvents_(significantEvents),
          verticesDiscovered_(graph.nVertices()), edgesVisited_(graph.nEdges()) {}
        
    // Current work item.
    Work& current() {
        ASSERT_forbid(workList_.empty());
        return workList_.front();
    }
    const Work& current() const {
        ASSERT_forbid(workList_.empty());
        return workList_.front();
    }

    // True if event is significant to the user.
    bool isSignificant(TraversalEvent event) const {
        return 0 != (event & significantEvents_);
    }

    // Mark a vertex as being discovered.  A vertex so marked will not be added to the work list, but will remain in the
    // worklist if it is already present.
    void setDiscovered(VertexNodeIterator vertex, bool isDiscovered=true) {
        ASSERT_require(vertex != graph_.vertices().end());
        verticesDiscovered_.setValue(vertex->id(), isDiscovered);
    }

    // Mark an edge as having been entered.  An edge so marked will not be entered again.
    void setVisited(EdgeNodeIterator edge, bool isVisited=true) {
        ASSERT_require(edge != graph_.edges().end());
        edgesVisited_.setValue(edge->id(), isVisited);
    }

    // Reset to an inital empty state.
    void clear() {
        verticesDiscovered_.clear();
        edgesVisited_.clear();
        latestDiscovered_ = Nothing();
        workList_.clear();
    }

    // Initialize traversal to start at the specified vertex.
    void start(VertexNodeIterator startVertex) {
        ASSERT_forbid(startVertex == graph_.vertices().end());
        clear();
        Work newWork(graph_.edges().end(), startVertex, nextEdges(startVertex, Direction()));
        enqueue(newWork, Order());
        setDiscovered(startVertex);
        latestDiscovered_ = newWork;
        while (!isAtEnd() && !isSignificant(event()))
            advance();
    }

    // Initialize traversal to start at the specified edge.
    void start(EdgeNodeIterator startEdge) {
        ASSERT_forbid(startEdge == graph_.edges().end());
        clear();
        EdgeNodeIterator endEdge = startEdge; ++endEdge;
        boost::iterator_range<EdgeNodeIterator> edges(startEdge, endEdge);
        Work newWork(graph_.edges().end(), graph_.vertices().end(), edges);
        enqueue(newWork, Order());
        setVisited(startEdge);
        workList_.front().event = ENTER_EDGE;
        while (!isAtEnd() && !isSignificant(event()))
            advance();
    }

public:
    /** Current event on which traversal is stopped.
     *
     *  See @ref TraversalEvent for a complete description of possible events and the vertex and edge values at those events. */
    TraversalEvent event() const {
        if (isAtEnd())
            throw std::runtime_error("attempt to dereference traversal at end");
        return FOLLOW_EDGE==current().event ? DISCOVER_VERTEX : current().event;
    }

    /** Vertex to which traversal is pointing.
     *
     *  See @ref TraversalEvent for details about which vertex is returned for various events. */
    VertexNodeIterator vertex() const {
        switch (event()) {
            case NO_EVENT:
                return graph_.vertices().end();
            case DISCOVER_VERTEX:
                ASSERT_require(latestDiscovered_);
                return latestDiscovered_->vertex;
            case ENTER_VERTEX:
            case ENTER_EDGE:
            case LEAVE_VERTEX:
            case LEAVE_EDGE:
                return current().vertex;
            default:
                break;
        }
        ASSERT_not_reachable("invalid state");
    }

    /** Edge to which traversal is pointing.
     *
     *  See @ref TraversalEvent for details about which edge is returned for various events. */
    EdgeNodeIterator edge() const {
        switch (event()) {
            case NO_EVENT:
                return graph_.edges().end();
            case ENTER_VERTEX:
                return current().fromEdge;
            case DISCOVER_VERTEX:
                ASSERT_require(latestDiscovered_);
                return latestDiscovered_->fromEdge;
            case LEAVE_VERTEX:
                return current().fromEdge;
            case ENTER_EDGE:
                return current().edge;
            case LEAVE_EDGE:
                return current().edge;
            default:
                break;
        }
        ASSERT_not_reachable("invalid state");
    }

    /** Returns true when traversal reaches the end.
     *
     *  A traversal for which isAtEnd returns true does not point to a meaningful vertex or edge and should not be advanced
     *  further.  The isAtEnd return value is the inverse of what @ref hasNext returns. */
    bool isAtEnd() const {
        return workList_.empty();
    }

    /** Returns true when a traversal can be advanced.
     *
     *  The hasNext return value is the inverse of what @ref isAtEnd returns. This predicate is often used with the @c next
     *  method in subclasses that have one:
     *
     * @code
     *  Traversal t(graph, start);
     *  while (t.hasNext())
     *      f(t.next());
     * @endcode
     *
     *  An iterator evaluated in a Boolean context will return a value equivalent to what hasNext returns:
     *
     * @code
     *  for (Traversal t(graph, start); t; ++t) ...
     * @endcode */
    bool hasNext() const {
        return !isAtEnd();
    }

    /** Advance traversl to next interesting event.
     *
     *  The traversal advances until it reaches a state that is deemed to be interesting to the user according to the list of
     *  event types supplied when the traversal was created.  It then returns the event that was reached.  Most subclasses
     *  define the list of interesting events themselves.  For instance, an edge traversal will probably only be interested in
     *  @ref ENTER_EDGE events.
     *
     *  Returns a valid event type on success, or @ref NO_EVENT when the traversal reaches the end state. */
    TraversalEvent advance() {
        while (1) {
            if (workList_.empty())
                return NO_EVENT;
            
            // After leaving an edge advance to the next edge if any. The state is set to ENTER_VERTEX so that it looks like
            // we're entering the vertex again, but this time at a different initial edge.
            if (current().event == LEAVE_EDGE) {
                if (workList_.front().edge != workList_.front().endEdge)
                    ++workList_.front().edge;
                current().event = ENTER_VERTEX;
            }

            // Leave the vertex if we're all done processing edges (or if it has none)
            if (current().event == ENTER_VERTEX) {
                while (workList_.front().edge != workList_.front().endEdge && isVisited(workList_.front().edge))
                    ++workList_.front().edge;
                if (workList_.front().edge == workList_.front().endEdge) {
                    current().event = LEAVE_VERTEX;
                    if (isSignificant(LEAVE_VERTEX) && workList_.front().vertex != graph_.vertices().end())
                        return LEAVE_VERTEX;
                }
            }
            
            // All done with the current work?
            if (current().event == LEAVE_VERTEX) {
                workList_.pop_front();
                if (workList_.empty())
                    return NO_EVENT;                    // end of traversal
            }

            // We've left the vertex; now leave the edge that got us to that vertex (if any).
            if (current().event == LEAVE_VERTEX) {
                current().event = LEAVE_EDGE;
                if (isSignificant(LEAVE_EDGE) && current().edge != graph_.edges().end())
                    return LEAVE_EDGE;
                continue;
            }

            // Enter a discovered vertex
            if (current().event == DISCOVER_VERTEX) {
                current().event = ENTER_VERTEX;
                if (isSignificant(ENTER_VERTEX))
                    return ENTER_VERTEX;
                continue;
            }

            // Visit the edge after entering the vertex or returning from another edge
            if (current().event == ENTER_VERTEX || current().event == LEAVE_EDGE) {
                current().event = ENTER_EDGE;
                setVisited(current().edge);
                if (isSignificant(ENTER_EDGE))
                    return ENTER_EDGE;
            }
            
            // Discover the neighbor vertex at the other end of this edge
            if (current().event == ENTER_EDGE) {
                current().event = FOLLOW_EDGE; // never escapes to the user
                if (current().followEdge) {
                    VertexNodeIterator neighbor = nextVertex(workList_.front().edge, Direction());
                    if (!isDiscovered(neighbor)) {
                        Work newWork(workList_.front().edge, neighbor, nextEdges(neighbor, Direction()));
                        enqueue(newWork, Order());
                        setDiscovered(neighbor);
                        latestDiscovered_ = newWork;
                        if (isSignificant(DISCOVER_VERTEX))
                            return DISCOVER_VERTEX;
                    }
                } else {
                    current().followEdge = true;
                }
            }

            // Leave the edge
            if (current().event == FOLLOW_EDGE) {
                current().event = LEAVE_EDGE;
                if (isSignificant(LEAVE_EDGE))
                    return LEAVE_EDGE;
            }
        }
    }

    /** Causes traversal to skip children.
     *
     *  If the current event is @ref ENTER_VERTEX then no edges will be followed out of the current vertex.  If the current
     *  event is @ref ENTER_EDGE then no vertex will be discovered from the current edge (but it might still be discovered by
     *  some other edge).  This method cannot be called in any other state. */
    void skipChildren() {
        switch (event()) {
            case NO_EVENT:
                throw std::runtime_error("GraphTraversal::skipChildren called at end of traversal");
            case ENTER_VERTEX:
                current().edge = current().endEdge;
                return;
            case ENTER_EDGE:
                current().followEdge = false;
                return;
            case DISCOVER_VERTEX:
            case LEAVE_VERTEX:
            case LEAVE_EDGE:
            case FOLLOW_EDGE:
                throw std::runtime_error("GraphTraversal::skipChildren cannot be called from " +
                                         traversalEventName(event()) + " event");
        }
        ASSERT_not_reachable("invalid state: event=" + traversalEventName(event()));
    }

    /** Allow a vertex to be discovered again.
     *
     *  Under normal operation, once a vertex is discovered all of its incoming or outgoing edges (depending on the traversal
     *  direction) are inserted into the worklist. This discovery normally happens once per vertex. However, the vertex can be
     *  forgotten so that if it's ever discovered by some other edge its incoming or outgoing edges will be inserted into the
     *  worklist again.  Calling @ref allowRediscovery each time a traversal leaves a vertex during a depth-first traversal
     *  will result in a traversal that finds all non-cyclic paths, possibly visiting some vertices more than once. */
    void allowRediscovery(VertexNodeIterator vertex) {
        if (vertex != graph_.vertices().end()) {
            setDiscovered(vertex, false);
            boost::iterator_range<EdgeNodeIterator> edges = nextEdges(vertex, Direction());
            for (EdgeNodeIterator iter=edges.begin(); iter!=edges.end(); ++iter)
                setVisited(iter, false);
        }
    }
    
    /** True if the vertex has been discovered.
     *
     *  Returns true if the specified vertex has ever existed on the work list.  Vertices are normally discovered between the
     *  @ref ENTER_EDGE and @ref LEAVE_EDGE events for the edge that flows into the vertex, and are only discovered once
     *  regardless of how many edges flow into them. */
    bool isDiscovered(VertexNodeIterator vertex) const {
        if (vertex == graph_.vertices().end())
            return false;
        return verticesDiscovered_.get(vertex->id());
    }

    /** True if the edge has been entered.
     *
     *  Returns true if the traversal has ever returned a @ref ENTER_EDGE event for the specified edge. */
    bool isVisited(EdgeNodeIterator edge) const {
        if (edge == graph_.edges().end())
            return false;
        return edgesVisited_.get(edge->id());
    }

    // The following trickery is to allow things like "if (x)" to work but without having an implicit
    // conversion to bool which would cause no end of other problems. This is fixed in C++11.
private:
    typedef void(GraphTraversal::*unspecified_bool)() const;
    void this_type_does_not_support_comparisons() const {}
public:
    /** Type for Boolean context.
     *
     *  Implicit conversion to a type that can be used in a boolean context such as an <code>if</code> or <code>while</code>
     *  statement.  For instance:
     *
     *  @code
     *   typedef DepthFirstForwardVertexTraversal<MyGraph> Traversal;
     *   for (Traversal t(graph, startVertex); t; ++t)
     *       reachable[t->id()] = true;
     *  @endcode */
    operator unspecified_bool() const {
        return isAtEnd() ? 0 : &GraphTraversal::this_type_does_not_support_comparisons;
    }

private:
    // Returns the next vertex in traversal order when given an edge.  Forward-flowing subclasses will return edge->target()
    // and reverse-flowing subclasses will return edge->source().
    VertexNodeIterator nextVertex(EdgeNodeIterator edge, ForwardTraversalTag) { return edge->target(); }
    VertexNodeIterator nextVertex(EdgeNodeIterator edge, ReverseTraversalTag) { return edge->source(); }

    // Returns edges that leave a vertex for the purpose of traversal.  Forward-flowing subclasses will return
    // vertex->outEdges() and reverse-flowing subclasses will return vertex->inEdges().
    boost::iterator_range<EdgeNodeIterator> nextEdges(VertexNodeIterator vertex, ForwardTraversalTag) { return vertex->outEdges(); }
    boost::iterator_range<EdgeNodeIterator> nextEdges(VertexNodeIterator vertex, ReverseTraversalTag) { return vertex->inEdges(); }

    // Adds new work to the list.  Depth-first subclasses will push work onto the front of the list and breadth-first
    // subclasses will push it onto the end.
    void enqueue(const Work &work, DepthFirstTraversalTag) { workList_.push_front(work); }
    void enqueue(const Work &work, BreadthFirstTraversalTag) { workList_.push_back(work); }
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                      Subclasses {Order}{Direction}GraphTraversal
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Depth-first, forward traversal for all event types.
 *
 *  This traversal follows edges in their natural order from source to target visiting children before siblings.  The advancing
 *  methods and operators stop at the next event that matches a list of significant event types supplied as a constructor
 *  argument.
 *
 * @sa GraphTraversal */
template<class Graph>
class DepthFirstForwardGraphTraversal: public GraphTraversal<Graph, DepthFirstTraversalTag, ForwardTraversalTag> {
public:
    /** Start traversal at specified vertex. */
    DepthFirstForwardGraphTraversal(Graph &graph, typename GraphTraits<Graph>::VertexNodeIterator startVertex,
                                    unsigned significantEvents=ALL_EVENTS)
        : GraphTraversal<Graph, DepthFirstTraversalTag, ForwardTraversalTag>(graph, significantEvents) {
        this->start(startVertex);
    }

    /** Start traversal at specified edge. */
    DepthFirstForwardGraphTraversal(Graph &graph, typename GraphTraits<Graph>::EdgeNodeIterator startEdge,
                                    unsigned significantEvents=ALL_EVENTS)
        : GraphTraversal<Graph, DepthFirstTraversalTag, ForwardTraversalTag>(graph, significantEvents) {
        this->start(startEdge);
    }

    /** Advance traversal to next event. */
    DepthFirstForwardGraphTraversal& operator++() {
        this->advance();
        return *this;
    }
};

/** Depth-first, reverse traversal for all event types.
 *
 *  This traversal follows edges in their reverse order from target to source visiting children before siblings.  The advancing
 *  methods and operators stop at the next event that matches a list of significant event types supplied as a constructor
 *  argument.
 *
 * @sa GraphTraversal */
template<class Graph>
class DepthFirstReverseGraphTraversal: public GraphTraversal<Graph, DepthFirstTraversalTag, ReverseTraversalTag> {
public:
    /** Start traversal at specified vertex. */
    DepthFirstReverseGraphTraversal(Graph &graph, typename GraphTraits<Graph>::VertexNodeIterator startVertex,
                                    unsigned significantEvents=ALL_EVENTS)
        : GraphTraversal<Graph, DepthFirstTraversalTag, ReverseTraversalTag>(graph, significantEvents) {
        this->start(startVertex);
    }

    /** Start traversal at specified edge. */
    DepthFirstReverseGraphTraversal(Graph &graph, typename GraphTraits<Graph>::EdgeNodeIterator startEdge,
                                    unsigned significantEvents=ALL_EVENTS)
        : GraphTraversal<Graph, DepthFirstTraversalTag, ReverseTraversalTag>(graph, significantEvents) {
        this->start(startEdge);
    }

    /** Advance traversal to next event. */
    DepthFirstReverseGraphTraversal& operator++() {
        this->advance();
        return *this;
    }
};

/** Breadth-first, forward traversal for all event types.
 *
 *  This traversal follows edges in their natural order from source to target visiting siblings before children.  The advancing
 *  methods and operators stop at the next event that matches a list of significant event types supplied as a constructor
 *  argument.
 *
 * @sa GraphTraversal */
template<class Graph>
class BreadthFirstForwardGraphTraversal: public GraphTraversal<Graph, BreadthFirstTraversalTag, ForwardTraversalTag> {
public:
    /** Start traversal at specified vertex. */
    BreadthFirstForwardGraphTraversal(Graph &graph, typename GraphTraits<Graph>::VertexNodeIterator startVertex,
                                      unsigned significantEvents=ALL_EVENTS)
        : GraphTraversal<Graph, BreadthFirstTraversalTag, ForwardTraversalTag>(graph, significantEvents) {
        this->start(startVertex);
    }

    /** Start traversal at specified edge. */
    BreadthFirstForwardGraphTraversal(Graph &graph, typename GraphTraits<Graph>::EdgeNodeIterator startEdge,
                                      unsigned significantEvents=ALL_EVENTS)
        : GraphTraversal<Graph, BreadthFirstTraversalTag, ForwardTraversalTag>(graph, significantEvents) {
        this->start(startEdge);
    }

    /** Advance traversal to next event. */
    BreadthFirstForwardGraphTraversal& operator++() {
        this->advance();
        return *this;
    }
};

/** Breadth-first, reverse traversal for all event types.
 *
 *  This traversal follows edges in their reverse order from target to source visiting siblings before children.  The advancing
 *  methods and operators stop at the next event that matches a list of significant event types supplied as a constructor
 *  argument.
 *
 * @sa GraphTraversal */
template<class Graph>
class BreadthFirstReverseGraphTraversal: public GraphTraversal<Graph, BreadthFirstTraversalTag, ReverseTraversalTag> {
public:
    /** Start traversal at specified vertex. */
    BreadthFirstReverseGraphTraversal(Graph &graph, typename GraphTraits<Graph>::VertexNodeIterator startVertex,
                                      unsigned significantEvents=ALL_EVENTS)
        : GraphTraversal<Graph, BreadthFirstTraversalTag, ReverseTraversalTag>(graph, significantEvents) {
        this->start(startVertex);
    }

    /** Start traversal at specified edge. */
    BreadthFirstReverseGraphTraversal(Graph &graph, typename GraphTraits<Graph>::EdgeNodeIterator startEdge,
                                      unsigned significantEvents=ALL_EVENTS)
        : GraphTraversal<Graph, BreadthFirstTraversalTag, ReverseTraversalTag>(graph, significantEvents) {
        this->start(startEdge);
    }

    /** Advance traversal to next event. */
    BreadthFirstReverseGraphTraversal& operator++() {
        this->advance();
        return *this;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                      Subclasses {Order}{Direction}VertexTraversal
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Base class for graph vertex traversals.
 *
 *  Vertex traversals increment their current position stopping only when a vertex is entered. They have dereference ("*") and
 *  arrow ("->") operators and a @ref next method that return vertex nodes from the graph. */
template<class Graph, class Order, class Direction>
class GraphVertexTraversal: public GraphTraversal<Graph, Order, Direction> {
protected:
    explicit GraphVertexTraversal(Graph &graph): GraphTraversal<Graph, Order, Direction>(graph, ENTER_VERTEX) {}

public:
    /** Return reference to current vertex. */
    typename GraphTraits<Graph>::VertexNodeIterator::Reference operator*() {
        return *this->vertex();
    }

    /** Return pointer to current vertex. */
    typename GraphTraits<Graph>::VertexNodeIterator::Pointer operator->() {
        return &*this->vertex();
    }

    /** Return reference to current vertex and advance. */
    typename GraphTraits<Graph>::VertexNodeIterator::Reference next() {
        typename GraphTraits<Graph>::VertexNodeIterator::Reference retval = this->vertex();
        this->advance();
        return retval;
    }
};

/** Depth-first, forward traversal for vertices.
 *
 *  This traversal follows edges in their natural order from source to target visiting child vertices before siblings. The
 *  advancing methods and operators (@ref advance, @ref next, etc.) stop only at vertex enter events (@ref
 *  ENTER_VERTEX). Dereferencing the traversal object returns a graph vertex node. */
template<class Graph>
class DepthFirstForwardVertexTraversal: public GraphVertexTraversal<Graph, DepthFirstTraversalTag, ForwardTraversalTag> {
public:
    /** Start traversal at specified vertex. */
    DepthFirstForwardVertexTraversal(Graph &graph, typename GraphTraits<Graph>::VertexNodeIterator startVertex)
        : GraphVertexTraversal<Graph, DepthFirstTraversalTag, ForwardTraversalTag>(graph) {
        this->start(startVertex);
    }

    /** Start traversal at specified edge. */
    DepthFirstForwardVertexTraversal(Graph &graph, typename GraphTraits<Graph>::EdgeNodeIterator startEdge)
        : GraphVertexTraversal<Graph, DepthFirstTraversalTag, ForwardTraversalTag>(graph) {
        this->start(startEdge);
    }

    /** Advance traversal to next event. */
    DepthFirstForwardVertexTraversal& operator++() {
        this->advance();
        return *this;
    }
};

/** Depth-first, reverse traversal for vertices.
 *
 *  This traversal follows edges in their reverse order from target to source visiting child vertices before siblings. The
 *  advancing methods and operators (@ref advance, @ref next, etc.) stop only at vertex enter events (@ref
 *  ENTER_VERTEX). Dereferencing the traversal object returns a graph vertex node. */
template<class Graph>
class DepthFirstReverseVertexTraversal: public GraphVertexTraversal<Graph, DepthFirstTraversalTag, ReverseTraversalTag> {
public:
    /** Start traversal at specified vertex. */
    DepthFirstReverseVertexTraversal(Graph &graph, typename GraphTraits<Graph>::VertexNodeIterator startVertex)
        : GraphVertexTraversal<Graph, DepthFirstTraversalTag, ReverseTraversalTag>(graph) {
        this->start(startVertex);
    }

    /** Start traversal at specified edge. */
    DepthFirstReverseVertexTraversal(Graph &graph, typename GraphTraits<Graph>::EdgeNodeIterator startEdge)
        : GraphVertexTraversal<Graph, DepthFirstTraversalTag, ReverseTraversalTag>(graph) {
        this->start(startEdge);
    }

    /** Advance traversal to next event. */
    DepthFirstReverseVertexTraversal& operator++() {
        this->advance();
        return *this;
    }
};

/** Breadth-first, forward traversal for vertices.
 *
 *  This traversal follows edges in their natural order from source to target visiting sibling vertices before children. The
 *  advancing methods and operators (@ref advance, @ref next, etc.) stop only at vertex enter events (@ref
 *  ENTER_VERTEX). Dereferencing the traversal object returns a graph vertex node. */
template<class Graph>
class BreadthFirstForwardVertexTraversal: public GraphVertexTraversal<Graph, BreadthFirstTraversalTag, ForwardTraversalTag> {
public:
    /** Start traversal at specified vertex. */
    BreadthFirstForwardVertexTraversal(Graph &graph, typename GraphTraits<Graph>::VertexNodeIterator startVertex)
        : GraphVertexTraversal<Graph, BreadthFirstTraversalTag, ForwardTraversalTag>(graph) {
        this->start(startVertex);
    }

    /** Start traversal at specified edge. */
    BreadthFirstForwardVertexTraversal(Graph &graph, typename GraphTraits<Graph>::EdgeNodeIterator startEdge)
        : GraphVertexTraversal<Graph, BreadthFirstTraversalTag, ForwardTraversalTag>(graph) {
        this->start(startEdge);
    }

    /** Advance traversal to next event. */
    BreadthFirstForwardVertexTraversal& operator++() {
        this->advance();
        return *this;
    }
};

/** Breadth-first, reverse traversal for vertices.
 *
 *  This traversal follows edges in their reverse order from target to source visiting sibling vertices before children. The
 *  advancing methods and operators (@ref advance, @ref next, etc.) stop only at vertex enter events (@ref
 *  ENTER_VERTEX). Dereferencing the traversal object returns a graph vertex node. */
template<class Graph>
class BreadthFirstReverseVertexTraversal: public GraphVertexTraversal<Graph, BreadthFirstTraversalTag, ReverseTraversalTag> {
public:
    /** Start traversal at specified vertex. */
    BreadthFirstReverseVertexTraversal(Graph &graph, typename GraphTraits<Graph>::VertexNodeIterator startVertex)
        : GraphVertexTraversal<Graph, BreadthFirstTraversalTag, ReverseTraversalTag>(graph) {
        this->start(startVertex);
    }

    /** Start traversal at specified edge. */
    BreadthFirstReverseVertexTraversal(Graph &graph, typename GraphTraits<Graph>::EdgeNodeIterator startEdge)
        : GraphVertexTraversal<Graph, BreadthFirstTraversalTag, ReverseTraversalTag>(graph) {
        this->start(startEdge);
    }

    /** Advance traversal to next event. */
    BreadthFirstReverseVertexTraversal& operator++() {
        this->advance();
        return *this;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                      Subclasses {Order}{Direction}EdgeTraversal
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Base class for graph edge traversals.
 *
 *  Edge traversals increment their current position stopping only when an edge is entered. They have dereference ("*") and
 *  arrow ("->") operators and a @ref next method that return edge nodes from the graph. */
template<class Graph, class Order, class Direction>
class GraphEdgeTraversal: public GraphTraversal<Graph, Order, Direction> {
protected:
    explicit GraphEdgeTraversal(Graph &graph): GraphTraversal<Graph, Order, Direction>(graph, ENTER_EDGE) {}

public:
    /** Return reference to current edge. */
    typename GraphTraits<Graph>::EdgeNodeIterator::Reference operator*() {
        return *this->edge();
    }

    /** Return pointer to current edge. */
    typename GraphTraits<Graph>::EdgeNodeIterator::Pointer operator->() {
        return &*this->edge();
    }

    /** Return reference to current edge and advance. */
    typename GraphTraits<Graph>::EdgeNodeIterator::Reference next() {
        typename GraphTraits<Graph>::EdgeNodeIterator::Reference retval = this->vertex();
        this->advance();
        return retval;
    }
};

/** Depth-first, forward traversal for edges.
 *
 *  This traversal follows edges in their natural order from source to target visiting child edges before siblings. The
 *  advancing methods and operators (@ref advance, @ref next, etc.) stop only at edge enter events (@ref
 *  ENTER_EDGE). Dereferencing the traversal object returns a graph edge node. */
template<class Graph>
class DepthFirstForwardEdgeTraversal: public GraphEdgeTraversal<Graph, DepthFirstTraversalTag, ForwardTraversalTag> {
public:
    /** Start traversal at specified vertex. */
    DepthFirstForwardEdgeTraversal(Graph &graph, typename GraphTraits<Graph>::VertexNodeIterator startVertex)
        : GraphEdgeTraversal<Graph, DepthFirstTraversalTag, ForwardTraversalTag>(graph) {
        this->start(startVertex);
    }

    /** Start traversal at specified edge. */
    DepthFirstForwardEdgeTraversal(Graph &graph, typename GraphTraits<Graph>::EdgeNodeIterator startEdge)
        : GraphEdgeTraversal<Graph, DepthFirstTraversalTag, ForwardTraversalTag>(graph) {
        this->start(startEdge);
    }

    /** Advance traversal to next event. */
    DepthFirstForwardEdgeTraversal& operator++() {
        this->advance();
        return *this;
    }
};
    
/** Depth-first, reverse traversal for edges.
 *
 *  This traversal follows edges in their reverse order from target to source visiting child edges before siblings. The
 *  advancing methods and operators (@ref advance, @ref next, etc.) stop only at edge enter events (@ref
 *  ENTER_EDGE). Dereferencing the traversal object returns a graph edge node. */
template<class Graph>
class DepthFirstReverseEdgeTraversal: public GraphEdgeTraversal<Graph, DepthFirstTraversalTag, ReverseTraversalTag> {
public:
    /** Start traversal at specified vertex. */
    DepthFirstReverseEdgeTraversal(Graph &graph, typename GraphTraits<Graph>::VertexNodeIterator startVertex)
        : GraphEdgeTraversal<Graph, DepthFirstTraversalTag, ReverseTraversalTag>(graph) {
        this->start(startVertex);
    }

    /** Start traversal at specified edge. */
    DepthFirstReverseEdgeTraversal(Graph &graph, typename GraphTraits<Graph>::EdgeNodeIterator startEdge)
        : GraphEdgeTraversal<Graph, DepthFirstTraversalTag, ReverseTraversalTag>(graph) {
        this->start(startEdge);
    }

    /** Advance traversal to next event. */
    DepthFirstReverseEdgeTraversal& operator++() {
        this->advance();
        return *this;
    }
};
        
/** Breadth-first, forward traversal for edges.
 *
 *  This traversal follows edges in their natural order from source to target visiting sibling edges before children. The
 *  advancing methods and operators (@ref advance, @ref next, etc.) stop only at edge enter events (@ref
 *  ENTER_EDGE). Dereferencing the traversal object returns a graph edge node. */
template<class Graph>
class BreadthFirstForwardEdgeTraversal: public GraphEdgeTraversal<Graph, BreadthFirstTraversalTag, ForwardTraversalTag> {
public:
    /** Start traversal at specified vertex. */
    BreadthFirstForwardEdgeTraversal(Graph &graph, typename GraphTraits<Graph>::VertexNodeIterator startVertex)
        : GraphEdgeTraversal<Graph, BreadthFirstTraversalTag, ForwardTraversalTag>(graph) {
        this->start(startVertex);
    }

    /** Start traversal at specified edge. */
    BreadthFirstForwardEdgeTraversal(Graph &graph, typename GraphTraits<Graph>::EdgeNodeIterator startEdge)
        : GraphEdgeTraversal<Graph, BreadthFirstTraversalTag, ForwardTraversalTag>(graph) {
        this->start(startEdge);
    }

    /** Advance traversal to next event. */
    BreadthFirstForwardEdgeTraversal& operator++() {
        this->advance();
        return *this;
    }
};
    
/** Breadth-first, reverse traversal for edges.
 *
 *  This traversal follows edges in their reverse order from target to source visiting sibling edges before children. The
 *  advancing methods and operators (@ref advance, @ref next, etc.) stop only at edge enter events (@ref
 *  ENTER_EDGE). Dereferencing the traversal object returns a graph edge node. */
template<class Graph>
class BreadthFirstReverseEdgeTraversal: public GraphEdgeTraversal<Graph, BreadthFirstTraversalTag, ReverseTraversalTag> {
public:
    /** Start traversal at specified vertex. */
    BreadthFirstReverseEdgeTraversal(Graph &graph, typename GraphTraits<Graph>::VertexNodeIterator startVertex)
        : GraphEdgeTraversal<Graph, BreadthFirstTraversalTag, ReverseTraversalTag>(graph) {
        this->start(startVertex);
    }

    /** Start traversal at specified edge. */
    BreadthFirstReverseEdgeTraversal(Graph &graph, typename GraphTraits<Graph>::EdgeNodeIterator startEdge)
        : GraphEdgeTraversal<Graph, BreadthFirstTraversalTag, ReverseTraversalTag>(graph) {
        this->start(startEdge);
    }

    /** Advance traversal to next event. */
    BreadthFirstReverseEdgeTraversal& operator++() {
        this->advance();
        return *this;
    }
};

} // namespace
} // namespace
} // namespace

#endif
