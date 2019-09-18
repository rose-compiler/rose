// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




// Algorithms for Sawyer::Container::Graph

#ifndef Sawyer_GraphAlgorithm_H
#define Sawyer_GraphAlgorithm_H

#include <Sawyer/Sawyer.h>
#include <Sawyer/DenseIntegerSet.h>
#include <Sawyer/GraphTraversal.h>
#include <Sawyer/Set.h>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <list>
#include <set>
#include <vector>

// If non-zero, then use a stack-based pool allocation strategy that tuned for multi-threading when searching for isomorphic
// subgraphs. The non-zero value defines the size of the large heap blocks requested from the standard runtime system and is a
// multiple of the size of the number of vertices in the second of the two graphs accessed by the search.
#ifndef SAWYER_VAM_STACK_ALLOCATOR
#define SAWYER_VAM_STACK_ALLOCATOR 2                    // Use a per-thread, stack based allocation if non-zero
#endif
#if SAWYER_VAM_STACK_ALLOCATOR
#include <Sawyer/StackAllocator.h>
#endif

// If defined, perform extra checks in the subgraph isomorphism "VAM" ragged array.
//#define SAWYER_VAM_EXTRA_CHECKS

namespace Sawyer {
namespace Container {
namespace Algorithm {

/** Determines if the any edges of a graph form a cycle.
 *
 *  Returns true if any cycle is found, false if the graph contains no cycles. */
template<class Graph>
bool
graphContainsCycle(const Graph &g) {
    std::vector<bool> visited(g.nVertices(), false);    // have we seen this vertex already?
    std::vector<bool> onPath(g.nVertices(), false);     // is a vertex on the current path of edges?
    for (size_t rootId = 0; rootId < g.nVertices(); ++rootId) {
        if (visited[rootId])
            continue;
        visited[rootId] = true;
        ASSERT_require(!onPath[rootId]);
        onPath[rootId] = true;
        for (DepthFirstForwardGraphTraversal<const Graph> t(g, g.findVertex(rootId), ENTER_EDGE|LEAVE_EDGE); t; ++t) {
            size_t targetId = t.edge()->target()->id();
            if (t.event() == ENTER_EDGE) {
                if (onPath[targetId])
                    return true;                        // must be a back edge forming a cycle
                onPath[targetId] = true;
                if (visited[targetId]) {
                    t.skipChildren();
                } else {
                    visited[targetId] = true;
                }
            } else {
                ASSERT_require(t.event() == LEAVE_EDGE);
                ASSERT_require(onPath[targetId]);
                onPath[targetId] = false;
            }
        }
        ASSERT_require(onPath[rootId]);
        onPath[rootId] = false;
    }
    return false;
}

/** Break cycles of a graph arbitrarily.
 *
 *  Modifies the argument in place to remove edges that cause cycles.  Edges are not removed in any particular order.  Returns
 *  the number of edges that were removed. */
template<class Graph>
size_t
graphBreakCycles(Graph &g) {
    std::vector<bool> visited(g.nVertices(), false);    // have we seen this vertex already?
    std::vector<unsigned char> onPath(g.nVertices(), false);  // is a vertex on the current path of edges? 0, 1, or 2
    Map<size_t, typename Graph::ConstEdgeIterator> edgesToErase;

    for (size_t rootId = 0; rootId < g.nVertices(); ++rootId) {
        if (visited[rootId])
            continue;
        visited[rootId] = true;
        ASSERT_require(!onPath[rootId]);
        onPath[rootId] = true;
        for (DepthFirstForwardGraphTraversal<const Graph> t(g, g.findVertex(rootId), ENTER_EDGE|LEAVE_EDGE); t; ++t) {
            size_t targetId = t.edge()->target()->id();
            if (t.event() == ENTER_EDGE) {
                if (onPath[targetId]) {
                    edgesToErase.insert(t.edge()->id(), t.edge());
                    t.skipChildren();
                }
                ++onPath[targetId];
                if (visited[targetId]) {
                    t.skipChildren();
                } else {
                    visited[targetId] = true;
                }
            } else {
                ASSERT_require(t.event() == LEAVE_EDGE);
                ASSERT_require(onPath[targetId]);
                --onPath[targetId];
            }
        }
        ASSERT_require(onPath[rootId]==1);
        onPath[rootId] = 0;
    }

    BOOST_FOREACH (const typename Graph::ConstEdgeIterator &edge, edgesToErase.values())
        g.eraseEdge(edge);
    return edgesToErase.size();
}

/** Test whether a graph is connected.
 *
 *  Returns true if a graph is connected and false if not. This is a special case of findConnectedComponents but is faster for
 *  graphs that are not connected since this algorithm only needs to find one connected component instead of all connected
 *  components.
 *
 *  Time complexity is O(|V|+|E|).
 *
 *  @sa graphFindConnectedComponents. */
template<class Graph>
bool
graphIsConnected(const Graph &g) {
    if (g.isEmpty())
        return true;
    std::vector<bool> seen(g.nVertices(), false);
    size_t nSeen = 0;
    DenseIntegerSet<size_t> worklist(g.nVertices());
    worklist.insert(0);
    while (!worklist.isEmpty()) {
        size_t id = *worklist.values().begin();
        worklist.erase(id);

        if (seen[id])
            continue;
        seen[id] = true;
        ++nSeen;

        typename Graph::ConstVertexIterator v = g.findVertex(id);
        BOOST_FOREACH (const typename Graph::Edge &e, v->outEdges()) {
            if (!seen[e.target()->id()])                // not necessary, but saves some work
                worklist.insert(e.target()->id());
        }
        BOOST_FOREACH (const typename Graph::Edge &e, v->inEdges()) {
            if (!seen[e.source()->id()])                // not necessary, but saves some work
                worklist.insert(e.source()->id());
        }
    }
    return nSeen == g.nVertices();
}

/** Find all connected components of a graph.
 *
 *  Finds all connected components of a graph and numbers them starting at zero. The provided vector is initialized to hold the
 *  results with the vector serving as a map from vertex ID number to connected component number.  Returns the number of
 *  conencted components.
 *
 *  Time complexity is O(|V|+|E|).
 *
 *  @sa @ref graphIsConnected. */
template<class Graph>
size_t
graphFindConnectedComponents(const Graph &g, std::vector<size_t> &components /*out*/) {
    static const size_t NOT_SEEN(-1);
    size_t nComponents = 0;
    components.clear();
    components.resize(g.nVertices(), NOT_SEEN);
    DenseIntegerSet<size_t> worklist(g.nVertices());
    for (size_t rootId = 0; rootId < g.nVertices(); ++rootId) {
        if (components[rootId] != NOT_SEEN)
            continue;
        ASSERT_require(worklist.isEmpty());
        worklist.insert(rootId);
        while (!worklist.isEmpty()) {
            size_t id = *worklist.values().begin();
            worklist.erase(id);

            ASSERT_require(components[id]==NOT_SEEN || components[id]==nComponents);
            if (components[id] != NOT_SEEN)
                continue;
            components[id] = nComponents;

            typename Graph::ConstVertexIterator v = g.findVertex(id);
            BOOST_FOREACH (const typename Graph::Edge &e, v->outEdges()) {
                if (components[e.target()->id()] == NOT_SEEN) // not necessary, but saves some work
                    worklist.insert(e.target()->id());
            }
            BOOST_FOREACH (const typename Graph::Edge &e, v->inEdges()) {
                if (components[e.source()->id()] == NOT_SEEN) // not necesssary, but saves some work
                    worklist.insert(e.source()->id());
            }
        }
        ++nComponents;
    }
    return nComponents;
}

/** Create a subgraph.
 *
 *  Creates a new graph by copying an existing graph, but copying only those vertices whose ID numbers are specified.  All
 *  edges between the specified vertices are copied.  The @p vertexIdVector should have vertex IDs that are part of graph @p g
 *  and no ID number should occur more than once in that vector.
 *
 *  The ID numbers of the vertices in the returned subgraph are equal to the corresponding index into the @p vertexIdVector for
 *  the super-graph. */
template<class Graph>
Graph
graphCopySubgraph(const Graph &g, const std::vector<size_t> &vertexIdVector) {
    Graph retval;

    // Insert vertices
    typedef typename Graph::ConstVertexIterator VIter;
    typedef Map<size_t, VIter> Id2Vertex;
    Id2Vertex resultVertices;
    for (size_t i=0; i<vertexIdVector.size(); ++i) {
        ASSERT_forbid2(resultVertices.exists(vertexIdVector[i]), "duplicate vertices not allowed");
        VIter rv = retval.insertVertex(g.findVertex(vertexIdVector[i])->value());
        ASSERT_require(rv->id() == i);                  // some analyses depend on this
        resultVertices.insert(vertexIdVector[i], rv);
    }

    // Insert edges
    for (size_t i=0; i<vertexIdVector.size(); ++i) {
        typename Graph::ConstVertexIterator gSource = g.findVertex(vertexIdVector[i]);
        typename Graph::ConstVertexIterator rSource = resultVertices[vertexIdVector[i]];
        BOOST_FOREACH (const typename Graph::Edge &e, gSource->outEdges()) {
            typename Graph::ConstVertexIterator rTarget = retval.vertices().end();
            if (resultVertices.getOptional(e.target()->id()).assignTo(rTarget))
                retval.insertEdge(rSource, rTarget, e.value());
        }
    }
    return retval;
}

/** Erase parallel edges.
 *
 *  Given a graph, erase all but one parallel edge between any two vertices. Parallel edges are defined as any two edges where
 *  both have the same source vertex, and both have the same target vertex, and both have equal values. Edge values must be
 *  equality comparable but need not be less-than comparable. */
template<class Graph>
void
graphEraseParallelEdges(Graph &g) {
    BOOST_FOREACH (const typename Graph::Vertex &src, g.vertices()) {
        if (src.nOutEdges() > 1) {
            Map<typename Graph::ConstVertexIterator /*target*/, std::vector<typename Graph::ConstEdgeIterator> > edgesByTarget;
            typename Graph::ConstEdgeIterator nextEdge = src.outEdges().begin();
            while (nextEdge != src.outEdges().end()) {
                typename Graph::ConstEdgeIterator curEdge = nextEdge++;
                std::vector<typename Graph::ConstEdgeIterator> &prevEdges = edgesByTarget.insertMaybeDefault(curEdge->target());
                bool erased = false;
                BOOST_FOREACH (typename Graph::ConstEdgeIterator prevEdge, prevEdges) {
                    if (curEdge->value() == prevEdge->value()) {
                        g.eraseEdge(curEdge);
                        erased = true;
                        break;
                    }
                }
                if (!erased)
                    prevEdges.push_back(curEdge);
            }
        }
    }
}

/** Number vertices according to their height from the leaves.
 *
 *  This function treats the input graph as a dependency graph where an edge from vertex V1 to V2 means that V1 depends on
 *  V2. It then numbers the vertices (after breaking cycles arbitrarily) giving each vertex a distinct number based on its
 *  height in the tree. The caller will then typically processes the vertices according to increasing vertex numbers in order
 *  to minimize the number of dependencies during the processing. */
template<class Graph>
std::vector<size_t>
graphDependentOrder(Graph &g) {
    size_t height = 1;
    std::vector<size_t> retval(g.nVertices(), 0);
    for (size_t root = 0; root < retval.size(); ++root) {
        if (0 == retval[root]) {
            std::vector<size_t /*vertexId*/> stack;
            stack.reserve(retval.size());
            stack.push_back(root);
            while (!stack.empty()) {
                size_t vid = stack.back();
                BOOST_FOREACH (const typename Graph::Edge &edge, g.findVertex(vid)->outEdges()) {
                    size_t target = edge.target()->id();
                    if (0 == retval[target]) {
                        retval[target] = 1;
                        stack.push_back(target);
                    }
                }
                if (stack.back() == vid) {
                    stack.pop_back();
                    retval[vid] = ++height;
                }
            }
        }
    }
    return retval;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Common subgraph isomorphism (CSI)
// Loosely based on the algorithm presented by Evgeny B. Krissinel and Kim Henrick
// "Common subgraph isomorphism detection by backtracking search"
// European Bioinformatics Institute, Genome Campus, Hinxton, Cambridge CB10 1SD, UK
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Vertex equivalence for common subgraph isomorphism.
 *
 *  Determines when a pair of vertices, one from each of two graphs, can be considered isomorphic. This class serves as both a
 *  model for those wishing to write their own formulation of equivalence, and as the default implementation when none is
 *  provided by the user. */
template<class GraphA, class GraphB>
class CsiEquivalence {
public:
    /** Isomorphism of two vertices.
     *
     *  Given a pair of vertices, one from each of two graphs, return true if the vertices could be an isomorphic pair in
     *  common subgraph isomorphism algorithms.  This default implementation always returns true. */
    bool mu(const GraphA &g1, const typename GraphA::ConstVertexIterator &v1,
            const GraphB &g2, const typename GraphB::ConstVertexIterator &v2) const {
        SAWYER_ARGUSED(g1);                             // Leave formal arg names in declaration because they're important
        SAWYER_ARGUSED(v1);                             // documentation for this function.
        SAWYER_ARGUSED(g2);
        SAWYER_ARGUSED(v2);
        return true;
    }

    /** Isomorphism of vertices based on incident edges.
     *
     *  Given two pairs of vertices, (@p i1, @p i2) and (@p j1, @p j2), one from each of two graphs @p g1 and @p g2, and given
     *  the two sets of edges that connect the vertices of each pair (in both directions), determine whether vertices @p i2 and
     *  @p j2 are isomorphic. The pair (@p i1, @p j1) is already part of a common subgraph isomorphism solution. Vertices @p i2
     *  and @p j2 are already known to satisfy the @ref mu predicate and have the appropriate number of edges for inclusion
     *  into the solution.
     *
     *  This default implementation always returns true. */
    bool nu(const GraphA &g1, typename GraphA::ConstVertexIterator i1, typename GraphA::ConstVertexIterator i2,
            const std::vector<typename GraphA::ConstEdgeIterator> &edges1,
            const GraphB &g2, typename GraphB::ConstVertexIterator j1, typename GraphB::ConstVertexIterator j2,
            const std::vector<typename GraphB::ConstEdgeIterator> &edges2) const {
        SAWYER_ARGUSED(g1);                             // Leave formal argument names in declaration because they're
        SAWYER_ARGUSED(i1);                             // important documentation for this function.
        SAWYER_ARGUSED(i2);
        SAWYER_ARGUSED(edges1);
        SAWYER_ARGUSED(g2);
        SAWYER_ARGUSED(j1);
        SAWYER_ARGUSED(j2);
        SAWYER_ARGUSED(edges2);
        return true;
    }

    /** Called at each step during the algorithm.
     *
     *  This method is called each time the algorithm tests a potential solution. It can be used to report progress or
     *  terminate the search after some number of iterations.  The argument is the number of vertices in the potential
     *  solution. */
    void progress(size_t /*size*/) {}
};

/** How the CSI algorith should proceed. */
enum CsiNextAction {
    CSI_CONTINUE,                                       /**< Continue searching for more solutions. */
    CSI_ABORT                                           /**< Return to caller without further searching. */
};

/** Functor called for each common subgraph isomorphism solution.
 *
 *  This functor is called whenever a solution is found for the common subgraph isomorphism problem. Users routinely write
 *  their own solution handler. This one serves only as an example and prints the solution vectors to standard output.
 *
 *  A solution processor takes two graph arguments and two vectors of vertex IDs, one for each graph.  The vectors are
 *  parallel: @p x[i] in @p g1 is isomorphic to @p y[i] in @p g2 for all @p i.
 *
 *  A solution processor returns a code that indicates whether the algorithm should search for additional solutions or return
 *  to its caller. Throwing an exception from the solution processor is another valid way to return from the algorithm,
 *  although it may be slower than returning the abort code. */
template<class GraphA, class GraphB>
class CsiShowSolution {
    size_t n;
public:
    CsiShowSolution(): n(0) {}

    /** Functor.
     *
     *  The vector @p x contains vertex IDs from graph @p g1, and @p y contains IDs from @p g2. Both vectors will always be the
     *  same length.  This implementation prints the vectors @p w and @p y to standard output. See the class definition for
     *  more information. */
    CsiNextAction operator()(const GraphA &g1, const std::vector<size_t> &x, const GraphB &g2, const std::vector<size_t> &y) {
        ASSERT_require(x.size() == y.size());
        std::cout <<"Common subgraph isomorphism solution #" <<n <<" found:\n"
                  <<"  x = [";
        BOOST_FOREACH (size_t i, x)
            std::cout <<" " <<i;
        std::cout <<" ]\n"
                  <<"  y = [";
        BOOST_FOREACH (size_t j, y)
            std::cout <<" " <<j;
        std::cout <<" ]\n";
        ++n;
        return CSI_CONTINUE;
    }
};

/** Common subgraph isomorphism solver.
 *
 *  Finds subgraphs of two given graphs where the subgraphs are isomorphic to one another.
 *
 *  The solver assumes that any vertex in the first graph can be isomorphic to any vertex of the second graph unless the user
 *  provides his own equivalence predicate. The default predicate, @ref CsiEquivalence, allows any vertex to be isomorphic to
 *  any other vertex (the solver additionally requires that the two subgraphs of any solution have the same number of
 *  edges). Providing an equivalence predicate can substantially reduce the search space, as can limiting the minimum size of
 *  solutions.
 *
 *  Each time a solution is found, the @p SolutionProcessor is invoked. Users will typically provide their own solution
 *  processor since the default processor, @ref CsiShowSolution, only prints the solutions to standard output. The solution
 *  processor is only called for complete solutions when the end of a search path is reached; it is not called for intermediate
 *  solutions.  The processor can return a code that indicates how the algorithm should proceed. See @ref CsiShowSolution for
 *  more information.
 *
 *  To use this class, instantiate an instance and specify the two graphs to be compared (they can both be the same graph if
 *  desired), the solution handler callback, and the vertex equivalence predicate. Then, if necessary, make adjustements to the
 *  callback and/or predicate. Finally, invoke the @ref run method. The graphs must not be modified between the time this
 *  solver is created and the @ref run method returns.
 *
 *  The following functions are convenient wrappers around this class: @ref findCommonIsomorphicSubgraphs, @ref
 *  findFirstCommonIsomorphicSubgraph, @ref findIsomorphicSubgraphs, @ref findMaximumCommonIsomorphicSubgraphs. */
template<class GraphA, class GraphB,
         class SolutionProcessor = CsiShowSolution<GraphA, GraphB>,
         class EquivalenceP = CsiEquivalence<GraphA, GraphB> >
class CommonSubgraphIsomorphism {
    const GraphA &g1;                                   // The first graph being compared (i.e., needle)
    const GraphB &g2;                                   // the second graph being compared (i.e., haystack)
    DenseIntegerSet<size_t> v, w;                       // available vertices of g1 and g2, respectively
    std::vector<size_t> x, y;                           // selected vertices of g1 and g2, which defines vertex mapping
    DenseIntegerSet<size_t> vNotX;                      // X erased from V

    SolutionProcessor solutionProcessor_;               // functor to call for each solution
    EquivalenceP equivalenceP_;                         // predicates to determine if two vertices can be equivalent
    size_t minimumSolutionSize_;                        // size of smallest permitted solutions
    size_t maximumSolutionSize_;                        // size of largest permitted solutions
    bool monotonicallyIncreasing_;                      // size of solutions increases
    bool findingCommonSubgraphs_;                       // solutions are subgraphs of both graphs or only second graph?

    // The Vam is a ragged 2d array, but using std::vector<std::vector<T>> is not efficient in a multithreaded environment
    // because std::allocator<> and Boost pool allocators don't (yet) have a mode that allows each thread to be largely
    // independent of other threads. The best we can do with std::allocator is about 30 threads running 100% on my box having
    // 72 hardware threads.  But we can use the following facts about the Vam:
    //
    //   (1) VAMs are constructed and destroyed in a stack-like manner.
    //   (2) We always know the exact size of the major axis (number or rows) before the VAM is created.
    //   (3) The longest row of a new VAM will not be longer than the longest row of the VAM from which it is derived.
    //   (4) The number of rows will never be more than the number of vertices in g1.
    //   (5) The longest row will never be longer than the number of vertices in g2.
    //   (6) VAMs are never shared between threads
#if SAWYER_VAM_STACK_ALLOCATOR
    typedef StackAllocator<size_t> VamAllocator;
#else
    struct VamAllocator {
        explicit VamAllocator(size_t) {}
    };
#endif
    VamAllocator vamAllocator_;

    // Essentially a ragged array having a fixed number of rows and each row can be a different length.  The number of rows is
    // known at construction time, and the rows are extended one at a time starting with the first and working toward the
    // last. Accessing any element is a constant-time operation.
    class Vam {                                         // Vertex Availability Map
        VamAllocator &allocator_;
#if SAWYER_VAM_STACK_ALLOCATOR
        std::vector<size_t*> rows_;
        std::vector<size_t> rowSize_;
        size_t *lowWater_;                              // first element allocated
#else
        std::vector<std::vector<size_t> > rows_;
#endif
        size_t lastRowStarted_;
#ifdef SAWYER_VAM_EXTRA_CHECKS
        size_t maxRows_, maxCols_;
#endif

    public:
        // Construct the VAM and reserve enough space for the indicated number of rows.
        explicit Vam(VamAllocator &allocator)
            : allocator_(allocator),
#if SAWYER_VAM_STACK_ALLOCATOR
              lowWater_(NULL),
#endif
              lastRowStarted_((size_t)(-1)) {
        }

        // Destructor assumes this is the top VAM in the allocator stack.
        ~Vam() {
#if SAWYER_VAM_STACK_ALLOCATOR
            if (lowWater_ != NULL) {
                ASSERT_require(!rows_.empty());
                allocator_.revert(lowWater_);
            } else {
                ASSERT_require((size_t)std::count(rowSize_.begin(), rowSize_.end(), 0) == rows_.size()); // all rows empty
            }
#endif
        }

        // Reserve space for specified number of rows.
        void reserveRows(size_t nrows) {
            rows_.reserve(nrows);
#if SAWYER_VAM_STACK_ALLOCATOR
            rowSize_.reserve(nrows);
#endif
#ifdef SAWYER_VAM_EXTRA_CHECKS
            maxRows_ = nrows;
            maxCols_ = 0;
#endif
        }

        // Start a new row.  You can only insert elements into the most recent row.
        void startNewRow(size_t i, size_t maxColumns) {
#ifdef SAWYER_VAM_EXTRA_CHECKS
            ASSERT_require(i < maxRows_);
            maxCols_ = maxColumns;
#endif
#if SAWYER_VAM_STACK_ALLOCATOR
            if (i >= rows_.size()) {
                rows_.resize(i+1, NULL);
                rowSize_.resize(i+1, 0);
            }
            ASSERT_require(rows_[i] == NULL);           // row was already started
            rows_[i] = allocator_.reserve(maxColumns);
#else
            if (i >= rows_.size())
                rows_.resize(i+1);
#endif
            lastRowStarted_ = i;
        }

        // Push a new element onto the end of the current row.
        void push(size_t i, size_t x) {
            ASSERT_require(i == lastRowStarted_);
            ASSERT_require(i < rows_.size());
#ifdef SAWYER_VAM_EXTRA_CHECKS
            ASSERT_require(size(i) < maxCols_);
#endif
#if SAWYER_VAM_STACK_ALLOCATOR
            size_t *ptr = allocator_.allocNext();
            if (lowWater_ == NULL)
                lowWater_ = ptr;
#ifdef SAWYER_VAM_EXTRA_CHECKS
            ASSERT_require(ptr == rows_[i] + rowSize_[i]);
#endif
            ++rowSize_[i];
            *ptr = x;
#else
            rows_[i].push_back(x);
#endif
        }

        // Given a vertex i in G1, return the number of vertices j in G2 where i and j can be equivalent.
        size_t size(size_t i) const {
#if SAWYER_VAM_STACK_ALLOCATOR
            return i < rows_.size() ? rowSize_[i] : size_t(0);
#else
            return i < rows_.size() ? rows_[i].size() : size_t(0);
#endif
        }

        // Given a vertex i in G1, return those vertices j in G2 where i and j can be equivalent.
        // This isn't really a proper iterator, but we can't return std::vector<size_t>::const_iterator on macOS because it's
        // constructor-from-pointer is private.
        boost::iterator_range<const size_t*> get(size_t i) const {
            static const size_t empty = 911; /*arbitrary*/
            if (i < rows_.size() && size(i) > 0) {
#if SAWYER_VAM_STACK_ALLOCATOR
                return boost::iterator_range<const size_t*>(rows_[i], rows_[i] + rowSize_[i]);
#else
                return boost::iterator_range<const size_t*>(&rows_[i][0], &rows_[i][0] + rows_[i].size());
#endif
            }
            return boost::iterator_range<const size_t*>(&empty, &empty);
        }
    };

public:
    /** Construct a solver.
     *
     *  Constructs a solver that will find subgraphs of @p g1 and @p g2 that are isomorpic to one another. The graphs must not
     *  be modified between the call to this constructor and the return of its @ref run method.
     *
     *  The solution processor and equivalence predicate are copied into this solver. If the size of these objects is an
     *  issue, then they can be created with default constructors when the solver is created, and then modified afterward by
     *  obtaining a reference to the copies that are part of the solver.
     *
     *  The default solution processor, @ref CsiShowSolution, prints the solutions to standard output. The default vertex
     *  equivalence predicate, @ref CsiEquivalence, allows any vertex in graph @p g1 to be isomorphic to any vertex in graph @p
     *  g2. The solver additionally constrains the two sugraphs of any solution to have the same number of edges (that's the
     *  essence of subgraph isomorphism and cannot be overridden by the vertex isomorphism predicate). */
    CommonSubgraphIsomorphism(const GraphA &g1, const GraphB &g2,
                              SolutionProcessor solutionProcessor = SolutionProcessor(),
                              EquivalenceP equivalenceP = EquivalenceP())
        : g1(g1), g2(g2), v(g1.nVertices()), w(g2.nVertices()), vNotX(g1.nVertices()), solutionProcessor_(solutionProcessor),
          equivalenceP_(equivalenceP), minimumSolutionSize_(1), maximumSolutionSize_(-1), monotonicallyIncreasing_(false),
          findingCommonSubgraphs_(true), vamAllocator_(SAWYER_VAM_STACK_ALLOCATOR * g2.nVertices()) {}

private:
    CommonSubgraphIsomorphism(const CommonSubgraphIsomorphism&) {
        ASSERT_not_reachable("copy constructor makes no sense");
    }

    CommonSubgraphIsomorphism& operator=(const CommonSubgraphIsomorphism&) {
        ASSERT_not_reachable("assignment operator makes no sense");
    }

public:
    /** Property: minimum allowed solution size.
     *
     *  Determines the minimum size of solutions for which the solution processor callback is invoked. This minimum can be
     *  changed at any time before or during the analysis and is useful when looking for the largest isomorphic subgraphs. Not
     *  only does this property control when the solution processor is invoked, but it's also used to limit the search
     *  space--specifying a large minimum size causes the analysis to run faster.
     *
     *  Decreasing the minimum solution size during a run will not cause solutions that were smaller than its previous value to
     *  be found if those solutions have already been skipped or pruned from the search space.
     *
     *  The default minimum is one, which means that the trivial solution of two empty subgraphs is not reported to the
     *  callback.
     *
     *  See also, @ref maximumSolutionSize.
     *
     * @{ */
    size_t minimumSolutionSize() const { return minimumSolutionSize_; }
    void minimumSolutionSize(size_t n) { minimumSolutionSize_ = n; }
    /** @} */

    /** Property: maximum allowed solution size.
     *
     *  Determines the maximum size of solutions for which the solution processor callback is invoked. The maximum can be
     *  changed any time before or during the analysis. Once a maximum solution is found along some search path, the remainder
     *  of the search path is discarded.
     *
     *  Increasing the maximum solution size during a run will not cause solutions that were larger than its previous value to
     *  be found if those solutions have already been skipped.
     *
     *  The default maximum is larger than both graphs, which means all solutions will be found.
     *
     *  See also, @ref minimumSolutionSize.
     *
     *  @{ */
    size_t maximumSolutionSize() const { return maximumSolutionSize_; }
    void maximumSolutionSize(size_t n) { maximumSolutionSize_ = n; }
    /** @} */

    /** Property: monotonically increasing solution size.
     *
     *  If true, then each solution reported to the solution processor will be at least as large as the previous
     *  solution. Setting this property will result in more efficient behavior than culling solutions in the solution processor
     *  because in the former situation the solver can eliminate branches of the search space.  This property is useful when
     *  searching for the largest isomorphic subgraph.
     *
     * @{ */
    bool monotonicallyIncreasing() const { return monotonicallyIncreasing_; }
    void monotonicallyIncreasing(bool b) { monotonicallyIncreasing_ = b; }
    /** @} */

    /** Property: reference to the solution callback.
     *
     *  Returns a reference to the callback that will process each solution. The callback can be changed at any time between
     *  construction of this analysis and the return from its @ref run method.
     *
     * @{ */
    SolutionProcessor& solutionProcessor() { return solutionProcessor_; }
    const SolutionProcessor& solutionProcessor() const { return solutionProcessor_; }
    /** @} */

    /** Property: reference to the vertex equivalence predicate.
     *
     *  Returns a reference to the predicate that determines whether a vertex from one graph can be isomorphic to a vertex of
     *  the other graph. Solutions will only contain pairs of vertices for which the predicate returns true.
     *
     *  Changing the predicate during the @ref run method may or may not have the desired effect. This is because the return
     *  value from the predicate (at least it's @ref CsiEquivalence::mu "mu" method) is computed up front and cached.
     *
     * @{ */
    EquivalenceP& equivalencePredicate() { return equivalenceP_; }
    const EquivalenceP& equivalencePredicate() const { return equivalenceP_; }
    /** @} */

    /** Property: find common subgraphs.
     *
     *  This property controls whether the solver finds subgraphs of both specified graphs, or requires that the entire first
     *  graph is a subgraph of the second. When true (the default) the solver finds solutions to the "common subgraph
     *  isomorphism" problem; when false it finds solutions to the "subgraph isomorphism" problem.
     *
     *  When this property is false the @ref minimumSolutionSize is ignored; all solutions will be equal in size to the number
     *  of vertices in the first graph.
     *
     * @{ */
    bool findingCommonSubgraphs() const { return findingCommonSubgraphs_; }
    void findingCommonSubgraphs(bool b) { findingCommonSubgraphs_ = b; }
    /** @} */

    /** Perform the common subgraph isomorphism analysis.
     *
     *  Runs the common subgraph isomorphism analysis from beginning to end, invoking the constructor-supplied solution
     *  processor for each solution that's found to be large enough (see @ref minimumSolutionSize).  The solutions are not
     *  detected in any particular order.
     *
     *  The graphs provided to the analysis constructor on which this analysis runs must not be modified between when the
     *  analysis is created and this method returns. Actually, it is permissible to modify the contents of the graphs, just not
     *  their connectivity. I.e., changing the values stored at vertices and edges is fine, but inserting or erasing vertices
     *  or edges is not.
     *
     *  The @ref run method may be called multiple times and will always start from the beginning. If the solution processor
     *  determines that the analysis is not required to complete then it may throw an exception. The @ref reset method can be
     *  called afterward to delete memory used by the analysis (memory usage is not large to begin with), although this is not
     *  necessary since the destructor does not leak memory. */
    void run() {
        reset();
        Vam vam = initializeVam();
        recurse(vam);
    }

    /** Releases memory used by the analysis.
     *
     *  Releases memory that's used by the analysis, returning the analysis to its just-constructed state.  This method is
     *  called implicitly at the beginning of each @ref run. */
    void reset() {
        v.insertAll();
        w.insertAll();
        x.clear();
        y.clear();
        vNotX.insertAll();
    }

private:
    // Maximum value+1 or zero.
    template<typename Container>
    static size_t maxPlusOneOrZero(const Container &container) {
        if (container.isEmpty())
            return 0;
        size_t retval = 0;
        BOOST_FOREACH (size_t val, container.values())
            retval = std::max(retval, val);
        return retval+1;
    }

    // Initialize VAM so to indicate which source vertices (v of g1) map to which target vertices (w of g2) based only on the
    // vertex comparator.  We also handle self edges here.
    Vam initializeVam() {
        Vam vam(vamAllocator_);
        vam.reserveRows(maxPlusOneOrZero(v));
        BOOST_FOREACH (size_t i, v.values()) {
            typename GraphA::ConstVertexIterator v1 = g1.findVertex(i);
            vam.startNewRow(i, w.size());
            BOOST_FOREACH (size_t j, w.values()) {
                typename GraphB::ConstVertexIterator w1 = g2.findVertex(j);
                std::vector<typename GraphA::ConstEdgeIterator> selfEdges1;
                std::vector<typename GraphB::ConstEdgeIterator> selfEdges2;
                findEdges(g1, i, i, selfEdges1 /*out*/);
                findEdges(g2, j, j, selfEdges2 /*out*/);
                if (selfEdges1.size() == selfEdges2.size() &&
                    equivalenceP_.mu(g1, g1.findVertex(i), g2, g2.findVertex(j)) &&
                    equivalenceP_.nu(g1, v1, v1, selfEdges1, g2, w1, w1, selfEdges2))
                    vam.push(i, j);
            }
        }
        return vam;
    }

    // Can the solution (stored in X and Y) be extended by adding another (i,j) pair of vertices where i is an element of the
    // set of available vertices of graph1 (vNotX) and j is a vertex of graph2 that is equivalent to i according to the
    // user-provided equivalence predicate. Furthermore, is it even possible to find a solution along this branch of discovery
    // which is falls between the minimum and maximum sizes specified by the user?  By eliminating entire branch of the search
    // space we can drastically decrease the time it takes to search, and the larger the required solution the more branches we
    // can eliminate.
    bool isSolutionPossible(const Vam &vam) const {
        if (findingCommonSubgraphs_ && x.size() >= maximumSolutionSize_)
            return false;                               // any further soln on this path would be too large
        size_t largestPossibleSolution = x.size();
        BOOST_FOREACH (size_t i, vNotX.values()) {
            if (vam.size(i) > 0) {
                ++largestPossibleSolution;
                if ((findingCommonSubgraphs_ && largestPossibleSolution >= minimumSolutionSize_) ||
                    (!findingCommonSubgraphs_ && largestPossibleSolution >= g1.nVertices()))
                    return true;
            }
        }
        return false;
    }

    // Choose some vertex i from G1 which is still available (i.e., i is a member of vNotX) and for which there is a vertex
    // equivalence of i with some available j from G2 (i.e., the pair (i,j) is present in the VAM).  The recursion terminates
    // quickest if we return the row of the VAM that has the fewest vertices in G2.
    size_t pickVertex(const Vam &vam) const {
        // FIXME[Robb Matzke 2016-03-19]: Perhaps this can be made even faster. The step that initializes the VAM
        // (initializeVam or refine) might be able to compute and store the shortest row so we can retrieve it here in constant
        // time.  Probably not worth the work though since loop this is small compared to the overall analysis.
        size_t shortestRowLength(-1), retval(-1);
        BOOST_FOREACH (size_t i, vNotX.values()) {
            size_t vs = vam.size(i);
            if (vs > 0 && vs < shortestRowLength) {
                shortestRowLength = vs;
                retval = i;
            }
        }
        ASSERT_require2(retval != size_t(-1), "cannot be reached if isSolutionPossible returned true");
        return retval;
    }

    // Extend the current solution by adding vertex i from G1 and vertex j from G2. The VAM should be adjusted separately.
    void extendSolution(size_t i, size_t j) {
        ASSERT_require(x.size() == y.size());
        ASSERT_require(std::find(x.begin(), x.end(), i) == x.end());
        ASSERT_require(std::find(y.begin(), y.end(), j) == y.end());
        ASSERT_require(vNotX.exists(i));
        x.push_back(i);
        y.push_back(j);
        vNotX.erase(i);
    }

    // Remove the last vertex mapping from a solution. The VAM should be adjusted separately.
    void retractSolution() {
        ASSERT_require(x.size() == y.size());
        ASSERT_require(!x.empty());
        size_t i = x.back();
        ASSERT_forbid(vNotX.exists(i));
        x.pop_back();
        y.pop_back();
        vNotX.insert(i);
    }

    // Find all edges that have the specified source and target vertices.  This is usually zero or one edge, but can be more if
    // the graph contains parallel edges.
    template<class Graph>
    void
    findEdges(const Graph &g, size_t sourceVertex, size_t targetVertex,
              std::vector<typename Graph::ConstEdgeIterator> &result /*in,out*/) const {
        BOOST_FOREACH (const typename Graph::Edge &candidate, g.findVertex(sourceVertex)->outEdges()) {
            if (candidate.target()->id() == targetVertex)
                result.push_back(g.findEdge(candidate.id()));
        }
    }

    // Given that we just extended a solution by adding the vertex pair (i, j), decide whether there's a
    // possible equivalence between vertex iUnused of G1 and vertex jUnused of G2 based on the edge(s) between i and iUnused
    // and between j and jUnused.
    bool edgesAreSuitable(size_t i, size_t iUnused, size_t j, size_t jUnused) const {
        ASSERT_require(i != iUnused);
        ASSERT_require(j != jUnused);

        // The two subgraphs in a solution must have the same number of edges.
        std::vector<typename GraphA::ConstEdgeIterator> edges1;
        std::vector<typename GraphB::ConstEdgeIterator> edges2;
        findEdges(g1, i, iUnused, edges1 /*out*/);
        findEdges(g2, j, jUnused, edges2 /*out*/);
        if (edges1.size() != edges2.size())
            return false;
        findEdges(g1, iUnused, i, edges1 /*out*/);
        findEdges(g2, jUnused, j, edges2 /*out*/);
        if (edges1.size() != edges2.size())
            return false;

        // If there are no edges, then assume that iUnused and jUnused could be isomorphic. We already know they satisfy the mu
        // constraint otherwise they wouldn't even be in the VAM.
        if (edges1.empty() && edges2.empty())
            return true;

        // Everything looks good to us, now let the user weed out certain pairs of vertices based on their incident edges.
        typename GraphA::ConstVertexIterator v1 = g1.findVertex(i), v2 = g1.findVertex(iUnused);
        typename GraphB::ConstVertexIterator w1 = g2.findVertex(j), w2 = g2.findVertex(jUnused);
        return equivalenceP_.nu(g1, v1, v2, edges1, g2, w1, w2, edges2);
    }

    // Create a new VAM from an existing one. The (i,j) pairs of the new VAM will form a subset of the specified VAM.
    void refine(const Vam &vam, Vam &refined /*out*/) {
        refined.reserveRows(maxPlusOneOrZero(vNotX));
        BOOST_FOREACH (size_t i, vNotX.values()) {
            size_t rowLength = vam.size(i);
            refined.startNewRow(i, rowLength);
            BOOST_FOREACH (size_t j, vam.get(i)) {
                if (j != y.back() && edgesAreSuitable(x.back(), i, y.back(), j))
                    refined.push(i, j);
            }
        }
    }

    // The Goldilocks predicate. Returns true if the solution is a valid size, false if it's too small or too big.
    bool isSolutionValidSize() const {
        if (findingCommonSubgraphs_) {
            return x.size() >= minimumSolutionSize_ && x.size() <= maximumSolutionSize_;
        } else {
            return x.size() == g1.nVertices();
        }
    }

    // The main recursive function. It works by extending the current solution by one pair for all combinations of such pairs
    // that are permissible according to the vertex equivalence predicate and not already part of the solution and then
    // recursively searching the remaining space.  This analysis class acts as a state machine whose data structures are
    // advanced and retracted as the space is searched. The VAM is the only part of the state that needs to be stored on a
    // stack since changes to it could not be easily undone during the retract phase.
    CsiNextAction recurse(const Vam &vam, size_t level = 0) {
        equivalenceP_.progress(level);
        if (isSolutionPossible(vam)) {
            size_t i = pickVertex(vam);
            BOOST_FOREACH (size_t j, vam.get(i)) {
                extendSolution(i, j);
                Vam refined(vamAllocator_);
                refine(vam, refined);
                if (recurse(refined, level+1) == CSI_ABORT)
                    return CSI_ABORT;
                retractSolution();
            }

            // Try again after removing vertex i from consideration
            if (findingCommonSubgraphs_) {
                v.erase(i);
                ASSERT_require(vNotX.exists(i));
                vNotX.erase(i);
                if (recurse(vam, level+1) == CSI_ABORT)
                    return CSI_ABORT;
                v.insert(i);
                vNotX.insert(i);
            }
        } else if (isSolutionValidSize()) {
            ASSERT_require(x.size() == y.size());
            if (monotonicallyIncreasing_)
                minimumSolutionSize_ = x.size();
            if (solutionProcessor_(g1, x, g2, y) == CSI_ABORT)
                return CSI_ABORT;
        }
        return CSI_CONTINUE;
    }
};

/** Find common isomorphic subgraphs.
 *
 *  Given two graphs find subgraphs of each that are isomorphic to each other.
 *
 *  Each solution causes an invocation of the @p solutionProcessor, which is a functor that takes four arguments: a const
 *  reference to the first graph, a const vector of @c size_t which is the ID numbers of the first graph's vertices selected to
 *  be in the subgraph, and the same two arguments for the second graph. Regardless of the graph sizes, the two vectors are
 *  always parallel--they contain the matching pairs of vertices. The solutions are processed in no particular order.
 *
 *  The @p equivalenceP is an optional predicate to determine when a pair of vertices, one from each graph, can be
 *  isomorphic. The subgraph solutions given by the two parallel vectors passed to the solution processor callback will contain
 *  only pairs of vertices for which this predicate returns true.
 *
 *  This function is only a convenient wrapper around the @ref CommonSubgraphIsomorphism class.
 *
 *  @sa @ref CommonSubgraphIsomorphism, @ref findIsomorphicSubgraphs, @ref findMaximumCommonIsomorphicSubgraphs.
 *
 *  @includelineno graphIso.C
 *
 * @{ */
template<class GraphA, class GraphB, class SolutionProcessor>
void findCommonIsomorphicSubgraphs(const GraphA &g1, const GraphB &g2, SolutionProcessor solutionProcessor) {
    CommonSubgraphIsomorphism<GraphA, GraphB, SolutionProcessor> csi(g1, g2, solutionProcessor);
    csi.run();
}

template<class GraphA, class GraphB, class SolutionProcessor, class EquivalenceP>
void findCommonIsomorphicSubgraphs(const GraphA &g1, const GraphB &g2,
                                   SolutionProcessor solutionProcessor, EquivalenceP equivalenceP) {
    CommonSubgraphIsomorphism<GraphA, GraphB, SolutionProcessor, EquivalenceP> csi(g1, g2, solutionProcessor, equivalenceP);
    csi.run();
}
/** @} */

// Used by findFirstCommonIsomorphicSubgraph
template<class GraphA, class GraphB>
class FirstIsomorphicSubgraph {
    std::pair<std::vector<size_t>, std::vector<size_t> > solution_;
public:
    CsiNextAction operator()(const GraphA &/*g1*/, const std::vector<size_t> &x,
                             const GraphB &/*g2*/, const std::vector<size_t> &y) {
        solution_ = std::make_pair(x, y);
        return CSI_ABORT;
    }

    const std::pair<std::vector<size_t>, std::vector<size_t> >& solution() const {
        return solution_;
    }
};

/** Determine whether a common subgraph exists.
 *
 *  Given two graphs, try to find any common isomorphic subgraph which is at least the specified size and return as soon as one
 *  is found. The return value is a pair of parallel vectors of vertex id numbers that relate the two subgraphs.  The return
 *  value is empty if no common isomorphic subgraph could be found.
 *
 * @{ */
template<class GraphA, class GraphB>
std::pair<std::vector<size_t>, std::vector<size_t> >
findFirstCommonIsomorphicSubgraph(const GraphA &g1, const GraphB &g2, size_t minimumSize) {
    CommonSubgraphIsomorphism<GraphA, GraphB, FirstIsomorphicSubgraph<GraphA, GraphB> > csi(g1, g2);
    csi.minimumSolutionSize(minimumSize);
    csi.maximumSolutionSize(minimumSize);               // to avoid going further than necessary
    csi.run();
    return csi.solutionProcessor().solution();
}


template<class GraphA, class GraphB, class EquivalenceP>
std::pair<std::vector<size_t>, std::vector<size_t> >
findFirstCommonIsomorphicSubgraph(const GraphA &g1, const GraphB &g2, size_t minimumSize, EquivalenceP equivalenceP) {
    CommonSubgraphIsomorphism<GraphA, GraphB, FirstIsomorphicSubgraph<GraphA, GraphB>, EquivalenceP>
        csi(g1, g2, FirstIsomorphicSubgraph<GraphA, GraphB>(), equivalenceP);
    csi.minimumSolutionSize(minimumSize);
    csi.maximumSolutionSize(minimumSize);               // to avoid going further than necessary
    csi.run();
    return csi.solutionProcessor().solution();
}

/** @} */

/** Find an isomorphic subgraph.
 *
 *  Given a smaller graph, @p g1, and a larger graph, @p g2, find all subgraphs of the larger graph that are isomorphic to the
 *  smaller graph.  If the @p g1 is larger than @p g2 then no solutions will be found since no subgraph of @p g2 can have
 *  enough vertices to be isomorphic to @p g1.
 *
 *  This function's behavior is identical to @ref findCommonIsomorphicSubgraphs except in one regard: the size of the vertex ID
 *  vectors passed to the solution processor will always be the same size as the number of vertices in @p g1.
 *
 *  This function is only a convenient wrapper around the @ref CommonSubgraphIsomorphism class.
 *
 *  @sa @ref CommonSubgraphIsomorphism, @ref findCommonIsomorphicSubgraphs, @ref findMaximumCommonIsomorphicSubgraphs.
 *
 *  @includelineno graphIso.C
 *
 * @{ */
template<class GraphA, class GraphB, class SolutionProcessor>
void findIsomorphicSubgraphs(const GraphA &g1, const GraphB &g2, SolutionProcessor solutionProcessor) {
    CommonSubgraphIsomorphism<GraphA, GraphB, SolutionProcessor> csi(g1, g2, solutionProcessor);
    csi.findingCommonSubgraphs(false);
    csi.run();
}

template<class GraphA, class GraphB, class SolutionProcessor, class EquivalenceP>
void findIsomorphicSubgraphs(const GraphA &g1, const GraphB &g2,
                             SolutionProcessor solutionProcessor, EquivalenceP equivalenceP) {
    CommonSubgraphIsomorphism<GraphA, GraphB, SolutionProcessor, EquivalenceP> csi(g1, g2, solutionProcessor, equivalenceP);
    csi.findingCommonSubgraphs(false);
    csi.run();
}
/** @} */

// Used internally by findMaximumCommonIsomorphicSubgraphs
template<class GraphA, class GraphB>
class MaximumIsomorphicSubgraphs {
    std::vector<std::pair<std::vector<size_t>, std::vector<size_t> > > solutions_;
public:
    CsiNextAction operator()(const GraphA &/*g1*/, const std::vector<size_t> &x,
                             const GraphB &/*g2*/, const std::vector<size_t> &y) {
        if (!solutions_.empty() && x.size() > solutions_.front().first.size())
            solutions_.clear();
        solutions_.push_back(std::make_pair(x, y));
        return CSI_CONTINUE;
    }

    const std::vector<std::pair<std::vector<size_t>, std::vector<size_t> > > &solutions() const {
        return solutions_;
    }
};

/** Find maximum common isomorphic subgraphs.
 *
 *  Given two graphs, find the largest possible isomorphic subgraph of those two graphs. The return value is a vector of pairs
 *  of vectors with each pair of vectors representing one solution.  For any pair of vectors, the first vector contains the
 *  IDs of vertices selected to be in a subgraph of the first graph, and the second vector contains the ID numbers of vertices
 *  selected to be in a subgraph of the second graph. These two vectors are parallel and represent isomorphic pairs of
 *  vertices.  The length of the vector-of-pairs is the number of solutions found; the lengths of all other vectors are equal
 *  to each other and represent the size of the (maximum) subgraph.
 *
 *  The @p equivalenceP is an optional predicate to determine when a pair of vertices, one from each graph, can be
 *  isomorphic. The subgraph solutions returned as pairs of parallel vectors will contain only pairs of vertices for which this
 *  predicate returns true.
 *
 *  This function is only a convenient wrapper around the @ref CommonSubgraphIsomorphism class.
 *
 *  @sa @ref CommonSubgraphIsomorphism, @ref findCommonIsomorphicSubgraphs, @ref findIsomorphicSubgraphs.
 *
 *  @includelineno graphIso.C
 *
 * @{ */
template<class GraphA, class GraphB>
std::vector<std::pair<std::vector<size_t>, std::vector<size_t> > >
findMaximumCommonIsomorphicSubgraphs(const GraphA &g1, const GraphB &g2) {
    CommonSubgraphIsomorphism<GraphA, GraphB, MaximumIsomorphicSubgraphs<GraphA, GraphB> > csi(g1, g2);
    csi.monotonicallyIncreasing(true);
    csi.run();
    return csi.solutionProcessor().solutions();
}

template<class GraphA, class GraphB, class EquivalenceP>
std::vector<std::pair<std::vector<size_t>, std::vector<size_t> > >
findMaximumCommonIsomorphicSubgraphs(const GraphA &g1, const GraphB &g2, EquivalenceP equivalenceP) {
    CommonSubgraphIsomorphism<GraphA, GraphB, MaximumIsomorphicSubgraphs<GraphA, GraphB>, EquivalenceP >
        csi(g1, g2, MaximumIsomorphicSubgraphs<GraphA, GraphB>(), equivalenceP);
    csi.monotonicallyIncreasing(true);
    csi.run();
    return csi.solutionProcessor().solutions();
}
/** @} */

/** Find immediate pre- or post-dominators.
 *
 *  Given a graph, find the immediate pre- or post-dominator of each vertex, if any, and return them as a vector. The vector,
 *  indexed by vertex ID, contains either a pointer (vertex iterator) to the dominator vertex, or no pointer (end vertex
 *  iterator) if the vertex has no dominator.
 *
 *  The algorithm employed here is loosely based on an algorithm from Rice University known to be O(n^2) where n is the number
 *  of vertices in the control flow subgraph connected to the start vertex.  According to the Rice paper, their algorithm
 *  outperforms Lengauer-Tarjan on typicall control flow graphs even though asymptotically, Lengauer-Tarjan is better.  The
 *  Rice algorithm is also much simpler.
 *
 *  I've added a few minor optimizations:
 *  @li Reverse post-order depth-first search is calculated once rather than each time through the loop.  Rice's analysis
 *      indicates that they also made this optimization, although their listed algorithm does not show it.
 *  @li The first processed predecessor of the vertex under consideration is determined in the same loop that processes
 *      the other predecessors, while in the listed algorithm this was a separate operation.
 *  @li Self loops in the control flow graph are not processed, since they don't contribute to the dominance relation.
 *  @li Undefined state for <code>idom(x)</code> is represented by <code>idom(x)==x</code>.
 *  @li Nodes are labeled in reverse order from Rice, but traversed in the same order.  This simplifies the code a bit
 *      because the vertices are traversed according to the "flowlist" vector, and the index into the "flowlist" vector
 *      can serve as the node label.
 *  @li Back edges in the flowlist are ignored since their dominator must be along a forward edge.
 *
 *  The set of dominators of vertex @em v, namely <em>dom(v)</em>, is represented as a linked list stored as an array indexed
 *  by vertex number. That is
 *
 * @code
 *  dom(v) = { v, idom(v), idom(idom(v)), ..., start }
 * @endcode
 *
 *  is stored in the @c idom array as:
 *
 * @code
 *  dom(v) = { v, idom[v], idom[idom[v]], ..., start }
 * @endcode
 *
 * This representation, combined with the fact that: <em>a ELEMENT_OF dom(v) implies dom(a) SUBSET_OF dom(v)</em>
 *
 * allows us to perform intersection by simply walking the two sorted lists until we find an element in common, and including
 * that element an all subsequent elements in the intersection result.  The @c idom array uses the flow-list vertex numbering
 * produced by a post-order visitor of a depth-first search, and the nodes are processed from highest to lowest. */
template<class Direction, class Graph>
std::vector<typename GraphTraits<Graph>::VertexIterator>
graphDirectedDominators(Graph &g, typename GraphTraits<Graph>::VertexIterator root) {
    typedef typename GraphTraits<Graph>::VertexIterator VertexIterator;
    typedef typename GraphTraits<Graph>::EdgeIterator EdgeIterator;
    typedef typename GraphTraits<Graph>::Edge Edge;
    static const size_t NO_ID = (size_t)(-1);

    ASSERT_require(g.isValidVertex(root));

    // List of vertex IDs in the best order for data-flow. I.e., the reverse of the post-fix, depth-first traversal following
    // forwared or reverse edges (depending on the Direction template argument).  A useful fact is that disregarding back
    // edges, the predecessors of the vertex represented by flowlist[i] all appear earlier in flowlist.
    GraphTraversal<Graph, DepthFirstTraversalTag, Direction> traversal(g, LEAVE_VERTEX);
    traversal.start(root);
    std::vector<size_t> flowlist = graphReachableVertices(traversal);
    std::reverse(flowlist.begin(), flowlist.end());

    // The inverse mapping of the flowlist. I.e.., since flowlist maps an index to a vertex ID, rflowlist maps a vertex ID back
    // to the flowlist index. If vertex v is not reachable from the root, then rflowlist[v] == NO_ID.
    std::vector<size_t> rflowlist(g.nVertices(), NO_ID);
    for (size_t i=0; i<flowlist.size(); ++i)
        rflowlist[flowlist[i]] = i;

    // Initialize the idom vector. idom[i]==i implies idom[i] is unknown
    std::vector<size_t> idom(flowlist.size());
    for (size_t i=0; i<flowlist.size(); ++i)
        idom[i] = i;

    bool changed = true;
    while (changed) {
        changed = false;                                // assume no change, prove otherwise
        for (size_t vertex_i=0; vertex_i < flowlist.size(); ++vertex_i) {
            VertexIterator vertex = g.findVertex(flowlist[vertex_i]);

            // Test idom invariant
#ifndef SAWYER_NDEBUG
            for (size_t i=0; i<idom.size(); ++i)
                ASSERT_require(idom[i] <= i);
#endif

            // The root vertex has no immediate dominator.
            // FIXME[Robb P Matzke 2017-06-23]: why not just start iterating with vertex_i=1?
            if (vertex == root)
                continue;

            // Try to choose a new idom for this vertex by processing each of its predecessors. The dominator of the current
            // vertex is a function of the dominators of its predecessors.
            size_t newIdom = idom[vertex_i];

            boost::iterator_range<EdgeIterator> edges = previousEdges<EdgeIterator>(vertex, Direction());
            for (EdgeIterator edge=edges.begin(); edge!=edges.end(); ++edge) {
                VertexIterator predecessor = previousVertex<VertexIterator>(edge, Direction());
                size_t predecessor_i = rflowlist[predecessor->id()];
                if (NO_ID == predecessor_i)
                    continue;                           // predecessor is not reachable from root, so does't contribute
                if (predecessor_i == vertex_i)
                    continue;                           // ignore self edges: V cannot be its own immediate dominator
                if (predecessor_i > vertex_i)
                    continue;                           // ignore back edges; the idom will also be found along a forward edge

                // The predecessor might be our dominator, so merge it with what we already have
                if (newIdom == vertex_i) {
                    newIdom = predecessor_i;
                } else {
                    size_t tmpIdom = predecessor_i;
                    while (newIdom != tmpIdom) {
                        while (newIdom > tmpIdom)
                            newIdom = idom[newIdom];
                        while (tmpIdom > newIdom)
                            tmpIdom = idom[tmpIdom];
                    }
                }
            }

            if (idom[vertex_i] != newIdom) {
                idom[vertex_i] = newIdom;
                changed = true;
            }
        }
    }

    // Build the result relation
    std::vector<VertexIterator> retval;
    retval.resize(g.nVertices(), g.vertices().end());
    for (size_t i=0; i<flowlist.size(); ++i) {
        if (idom[i] != i)
            retval[flowlist[i]] = g.findVertex(flowlist[idom[i]]);
    }
    return retval;
}

/** Find immediate pre-dominators.
 *
 *  Given a graph, find the immediate pre-dominator of each vertex, if any, and return them as a vector. The vector,
 *  indexed by vertex ID, contains either a pointer (vertex iterator) to the dominator vertex, or no pointer (end vertex
 *  iterator) if the vertex has no dominator.
 *
 *  See also, @ref graphPostDominators and @ref graphDirectedDominators. */
template<class Graph>
std::vector<typename GraphTraits<Graph>::VertexIterator>
graphDominators(Graph &g, typename GraphTraits<Graph>::VertexIterator root) {
    return graphDirectedDominators<ForwardTraversalTag>(g, root);
}

/** Find immediate post-dominators.
 *
 *  Given a graph, find the immediate post-dominator of each vertex, if any, and return them as a vector. The vector,
 *  indexed by vertex ID, contains either a pointer (vertex iterator) to the dominator vertex, or no pointer (end vertex
 *  iterator) if the vertex has no dominator.
 *
 *  See also, @ref graphDominators and @ref graphDirectedDominators. */
template<class Graph>
std::vector<typename GraphTraits<Graph>::VertexIterator>
graphPostDominators(Graph &g, typename GraphTraits<Graph>::VertexIterator root) {
    return graphDirectedDominators<ReverseTraversalTag>(g, root);
}

} // namespace
} // namespace
} // namespace

#endif
