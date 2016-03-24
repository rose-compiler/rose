// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




// Algorithms for Sawyer::Container::Graph

#ifndef Sawyer_GraphAlgorithm_H
#define Sawyer_GraphAlgorithm_H

#include <Sawyer/Sawyer.h>
#include <Sawyer/DenseIntegerSet.h>
#include <Sawyer/GraphTraversal.h>
#include <Sawyer/Message.h>
#include <Sawyer/Set.h>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <set>
#include <vector>

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
    std::set<typename Graph::ConstEdgeIterator> edgesToErase;

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
                    edgesToErase.insert(t.edge());
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

    BOOST_FOREACH (const typename Graph::ConstEdgeIterator &edge, edgesToErase)
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
template<class Graph>
class CsiEquivalence {
public:
    /** Isomorphism of two vertices.
     *
     *  Given a pair of vertices, one from each of two graphs, return true if the vertices could be an isomorphic pair in
     *  common subgraph isomorphism algorithms.  This default implementation always returns true. */
    bool mu(const Graph &g1, const typename Graph::ConstVertexIterator &v1,
            const Graph &g2, const typename Graph::ConstVertexIterator &v2) const {
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
    bool nu(const Graph &g1, typename Graph::ConstVertexIterator i1, typename Graph::ConstVertexIterator i2,
            const std::vector<typename Graph::ConstEdgeIterator> &edges1,
            const Graph &g2, typename Graph::ConstVertexIterator j1, typename Graph::ConstVertexIterator j2,
            const std::vector<typename Graph::ConstEdgeIterator> &edges2) const {
        return true;
    }
};

/** Functor called for each common subgraph isomorphism solution.
 *
 *  This functor is called whenever a solution is found for the common subgraph isomorphism problem. It takes two graph
 *  arguments and two vectors of vertex IDs, one for each graph.  The vectors are parallel: @p x[i] in @p g1 is isomorphic to
 *  @p y[i] in @p g2 for all @p i.
 *
 *  Users routinely write their own solution handler. This one serves only as an example and prints the solution vectors to
 *  standard output. */
template<class Graph>
class CsiShowSolution {
    size_t n;
public:
    CsiShowSolution(): n(0) {}

    /** Functor.
     *
     *  The vector @p x contains vertex IDs from graph @p g1, and @p y contains IDs from @p g2. Both vectors will always be the
     *  same length.  This implementation prints the vectors @p w and @p y to standard output. */
    void operator()(const Graph &g1, const std::vector<size_t> &x, const Graph &g2, const std::vector<size_t> &y) {
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
    }
};

/** Common subgraph isomorphism solver.
 *
 *  Finds subgraphs of two given graphs where the subgraphs are isomorphic to one another. Each time a solution is found, the
 *  @p SolutionProcessor is invoked. Users will typically provide their own solution process since the default processor, @ref
 *  CsiShowSolution, only prints the solutions to standard output.  The solver assumes that any vertex in the first graph can
 *  be isomorphic to any vertex of the second graph unless the user provides his own equivalence predicate. The default
 *  predicate, @ref CsiEquivalence, allows any vertex to be isomorphic to any other vertex (the solver additionally requires
 *  that the two subgraphs of any solution have the same number of edges). Providing an equivalence predicate can substantially
 *  reduce the search space, as can limiting the minimum size of solutions.
 *
 *  To use this class, instantiate an instance and specify the two graphs to be compared (they can both be the same graph if
 *  desired), the solution handler callback, and the vertex equivalence predicate. Then, if necessary, make adjustements to the
 *  callback and/or predicate. Finally, invoke the @ref run method. The graphs must not be modified between the time this
 *  solver is created and the @ref run method returns.*/
template<class Graph,
         class SolutionProcessor = CsiShowSolution<Graph>,
         class EquivalenceP = CsiEquivalence<Graph> >
class CommonSubgraphIsomorphism {
    const Graph &g1, &g2;                               // the two whole graphs being compared
    DenseIntegerSet<size_t> v, w;                       // available vertices of g1 and g2, respectively
    std::vector<size_t> x, y;                           // selected vertices of g1 and g2, which defines vertex mapping
    mutable Message::Stream debug;                      // debugging output
    DenseIntegerSet<size_t> vNotX;                      // X erased from V

    SolutionProcessor solutionProcessor_;               // functor to call for each solution
    EquivalenceP equivalenceP_;                         // predicates to determine if two vertices can be equivalent
    size_t minimumSolutionSize_;                        // size of smallest permitted solutions
    bool monotonicallyIncreasing_;                      // size of solutions increases
    bool findingCommonSubgraphs_;                       // solutions are subgraphs of both graphs or only second graph?

    class Vam {                                         // Vertex Availability Map
        typedef std::vector<size_t> TargetVertices;     // target vertices in no particular order
        typedef std::vector<TargetVertices> Map;        // map from source vertex to available target vertices
        Map map_;
    public:
        // Set to initial emtpy state
        void clear() {
            map_.clear();
        }

        // Predicate to determine whether vam is empty. Since we never remove items from the VAM and we only add rows if we're
        // adding a column, this is equivalent to checking whether the map has any rows.
        bool isEmpty() const {
            return map_.empty();
        }

        // Insert the pair (i,j) into the mapping. Assumes this pair isn't already present.
        void insert(size_t i, size_t j) {
            if (i >= map_.size())
                map_.resize(i+1);
            map_[i].push_back(j);
        }

        // Given a vertex i in G1, return the number of vertices j in G2 where i and j can be equivalent.
        size_t size(size_t i) const {
            return i < map_.size() ? map_[i].size() : size_t(0);
        }

        // Given a vertex i in G1, return those vertices j in G2 where i and j can be equivalent.
        const std::vector<size_t>& get(size_t i) const {
            static const std::vector<size_t> empty;
            return i < map_.size() ? map_[i] : empty;
        }

        // Print for debugging
        void print(std::ostream &out, const std::string &prefix = "vam") const {
            if (isEmpty()) {
                out <<prefix <<" is empty\n";
            } else {
                for (size_t i=0; i<map_.size(); ++i) {
                    if (!map_[i].empty()) {
                        out <<prefix <<"[" <<i <<"] -> {";
                        BOOST_FOREACH (size_t j, map_[i])
                            out <<" " <<j;
                        out <<" }\n";
                    }
                }
            }
        }
    };

public:
    /** Construct a solver.
     *
     *  Constructs a solver that will find subgraphs of @p g1 and @p g2 that are isomorpic to one another. The graphs must not
     *  be modified between the call to this constructor and the return of its @ref run method.
     *
     *  The solution processor and equivalence predicated are copied into this solver. If the size of these objects is an
     *  issue, then they can be created with default constructors when the solver is created, and then modified afterward by
     *  obtaining a reference to the copies that are part of the solver.
     *
     *  A debug stream can be provided, in which case large amounts of debug traces are emitted. The default is to use
     *  %Sawyer's main debug stream which is normally disabled, thus no output is produced. Debug output can be selectively
     *  turned on and off by enabling or disabling the stream at any time during the analysis.
     *
     *  The default solution processor, @ref CsiShowSolution, prints the solutions to standard output. The default vertex
     *  equivalence predicate, @ref CsiEquivalence, allows any vertex in graph @p g1 to be isomorphic to any vertex in graph @p
     *  g2. The solver additionally constrains the two sugraphs of any solution to have the same number of edges (that's the
     *  essence of subgraph isomorphism and cannot be overridden by the vertex isomorphism predicate). */
    CommonSubgraphIsomorphism(const Graph &g1, const Graph &g2,
                              Message::Stream &debug = Message::mlog[Message::DEBUG],
                              SolutionProcessor solutionProcessor = SolutionProcessor(), 
                              EquivalenceP equivalenceP = EquivalenceP())
        : g1(g1), g2(g2), v(g1.nVertices()), w(g2.nVertices()), debug(debug), vNotX(g1.nVertices()),
          solutionProcessor_(solutionProcessor), equivalenceP_(equivalenceP), minimumSolutionSize_(1),
          monotonicallyIncreasing_(false), findingCommonSubgraphs_(true) {}

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
     * @{ */
    size_t minimumSolutionSize() const { return minimumSolutionSize_; }
    void minimumSolutionSize(size_t n) { minimumSolutionSize_ = n; }
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
        Vam vam;                                        // this is the only per-recursion local state
        initializeVam(vam);
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
    // Print contents of a container. This is only used for debugging.
    template<class ForwardIterator>
    void printContainer(std::ostream &out, const std::string &prefix, ForwardIterator begin, const ForwardIterator &end,
                        const char *ends = "[]", bool doSort = false) const {
        ASSERT_require(ends && 2 == strlen(ends));
        if (doSort) {
            std::vector<size_t> sorted(begin, end);
            std::sort(sorted.begin(), sorted.end());
            printContainer(out, prefix, sorted.begin(), sorted.end(), ends, false);
        } else {
            out <<prefix <<ends[0];
            while (begin != end) {
                out <<" " <<*begin;
                ++begin;
            }
            out <<" " <<ends[1] <<"\n";
        }
    }

    template<class ForwardIterator>
    void printContainer(std::ostream &out, const std::string &prefix, const boost::iterator_range<ForwardIterator> &range,
                        const char *ends = "[]", bool doSort = false) const {
        printContainer(out, prefix, range.begin(), range.end(), ends, doSort);
    }

    // Initialize VAM so to indicate which source vertices (v of g1) map to which target vertices (w of g2) based only on the
    // vertex comparator.  We also handle self edges here.
    void initializeVam(Vam &vam) const {
        vam.clear();
        BOOST_FOREACH (size_t i, v.values()) {
            typename Graph::ConstVertexIterator v1 = g1.findVertex(i);
            BOOST_FOREACH (size_t j, w.values()) {
                typename Graph::ConstVertexIterator w1 = g2.findVertex(j);
                std::vector<typename Graph::ConstEdgeIterator> selfEdges1, selfEdges2;
                findEdges(g1, i, i, selfEdges1 /*out*/);
                findEdges(g2, j, j, selfEdges2 /*out*/);
                if (selfEdges1.size() == selfEdges2.size() &&
                    equivalenceP_.mu(g1, g1.findVertex(i), g2, g2.findVertex(j)) &&
                    equivalenceP_.nu(g1, v1, v1, selfEdges1, g2, w1, w1, selfEdges2))
                    vam.insert(i, j);
            }
        }
    }

    // Can the solution (stored in X and Y) be extended by adding another (i,j) pair of vertices where i is an element of the
    // set of available vertices of graph1 (vNotX) and j is a vertex of graph2 that is equivalent to i according to the
    // user-provided equivalence predicate. Furthermore, is it even possible to find a solution along this branch of discovery
    // which is large enough for the user? By eliminating entire branch of the search space we can drastically decrease the
    // time it takes to search, and the larger the required solution the more branches we can eliminate.
    bool isSolutionPossible(const Vam &vam) const {
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
        SAWYER_MESG(debug) <<"  extending solution with (i,j) = (" <<i <<"," <<j <<")\n";
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
        std::vector<typename Graph::ConstEdgeIterator> edges1, edges2;
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
        typename Graph::ConstVertexIterator v1 = g1.findVertex(i), v2 = g1.findVertex(iUnused);
        typename Graph::ConstVertexIterator w1 = g2.findVertex(j), w2 = g2.findVertex(jUnused);
        return equivalenceP_.nu(g1, v1, v2, edges1, g2, w1, w2, edges2);
    }

    // Create a new VAM from an existing one. The (i,j) pairs of the new VAM will form a subset of the specified VAM.
    Vam refine(const Vam &vam) const {
        Vam refined;
        BOOST_FOREACH (size_t i, vNotX.values()) {
            BOOST_FOREACH (size_t j, vam.get(i)) {
                if (j != y.back()) {
                    SAWYER_MESG(debug) <<"  refining with edges " <<x.back() <<" <--> " <<i <<" in G1"
                                       <<" and " <<y.back() <<" <--> " <<j <<" in G2";
                    if (edgesAreSuitable(x.back(), i, y.back(), j)) {
                        SAWYER_MESG(debug) <<": inserting (" <<i <<", " <<j <<")\n";
                        refined.insert(i, j);
                    } else {
                        SAWYER_MESG(debug) <<": non-equivalent edges\n";
                    }
                }
            }
        }
        refined.print(debug, "  refined");
        return refined;
    }

    // Show some debugging inforamtion
    void showState(const Vam &vam, const std::string &what, size_t level) const {
        debug <<what <<" " <<level <<":\n";
        printContainer(debug, "  v - x = ", vNotX.values(), "{}", true);
        printContainer(debug, "  x = ", x.begin(), x.end());
        printContainer(debug, "  y = ", y.begin(), y.end());
        vam.print(debug, "  vam");
    }

    // The main recursive function. It works by extending the current solution by one pair for all combinations of such pairs
    // that are permissible according to the vertex equivalence predicate and not already part of the solution and then
    // recursively searching the remaining space.  This analysis class acts as a state machine whose data structures are
    // advanced and retracted as the space is searched. The VAM is the only part of the state that needs to be stored on a
    // stack since changes to it could not be easily undone during the retract phase.
    void recurse(const Vam &vam, size_t level = 0) {
        if (debug)
            showState(vam, "entering state", level);        // debugging
        if (isSolutionPossible(vam)) {
            size_t i = pickVertex(vam);
            SAWYER_MESG(debug) <<"  picked i = " <<i <<"\n";
            std::vector<size_t> jCandidates = vam.get(i);
            BOOST_FOREACH (size_t j, jCandidates) {
                extendSolution(i, j);
                Vam refined = refine(vam);
                recurse(refined, level+1);
                retractSolution();
                if (debug)
                    showState(vam, "back to state", level);
            }

            // Try again after removing vertex i from consideration
            if (findingCommonSubgraphs_) {
                SAWYER_MESG(debug) <<"  removing i=" <<i <<" from consideration\n";
                v.erase(i);
                ASSERT_require(vNotX.exists(i));
                vNotX.erase(i);
                recurse(vam, level+1);
                v.insert(i);
                vNotX.insert(i);
                if (debug)
                    showState(vam, "restored i=" + boost::lexical_cast<std::string>(i) + " back to state", level);
            }
        } else if ((findingCommonSubgraphs_ && x.size() >= minimumSolutionSize_) ||
                   (!findingCommonSubgraphs_ && x.size() == g1.nVertices())) {
            ASSERT_require(x.size() == y.size());
            if (debug) {
                printContainer(debug, "  found soln x = ", x.begin(), x.end());
                printContainer(debug, "  found soln y = ", y.begin(), y.end());
            }
            if (monotonicallyIncreasing_)
                minimumSolutionSize_ = x.size();
            solutionProcessor_(g1, x, g2, y);
        }
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
template<class Graph, class SolutionProcessor>
void findCommonIsomorphicSubgraphs(const Graph &g1, const Graph &g2, SolutionProcessor solutionProcessor) {
    CommonSubgraphIsomorphism<Graph, SolutionProcessor> csi(g1, g2, Message::mlog[Message::DEBUG], solutionProcessor);
    csi.run();
}

template<class Graph, class SolutionProcessor, class EquivalenceP>
void findCommonIsomorphicSubgraphs(const Graph &g1, const Graph &g2, const Message::Stream &debug,
                                   SolutionProcessor solutionProcessor, EquivalenceP equivalenceP) {
    CommonSubgraphIsomorphism<Graph, SolutionProcessor, EquivalenceP> csi(g1, g2, debug, solutionProcessor, equivalenceP);
    csi.run();
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
template<class Graph, class SolutionProcessor>
void findIsomorphicSubgraphs(const Graph &g1, const Graph &g2, SolutionProcessor solutionProcessor) {
    CommonSubgraphIsomorphism<Graph, SolutionProcessor> csi(g1, g2, Message::mlog[Message::DEBUG], solutionProcessor);
    csi.findingCommonSubgraphs(false);
    csi.run();
}

template<class Graph, class SolutionProcessor, class EquivalenceP>
void findIsomorphicSubgraphs(const Graph &g1, const Graph &g2, const Message::Stream &debug,
                             SolutionProcessor solutionProcessor, EquivalenceP equivalenceP) {
    CommonSubgraphIsomorphism<Graph, SolutionProcessor, EquivalenceP> csi(g1, g2, debug, solutionProcessor, equivalenceP);
    csi.findingCommonSubgraphs(false);
    csi.run();
}
/** @} */

// Used internally by findMaximumCommonIsomorphicSubgraphs
template<class Graph>
class MaximumIsomorphicSubgraphs {
    std::vector<std::pair<std::vector<size_t>, std::vector<size_t> > > solutions_;
public:
    void operator()(const Graph &g1, const std::vector<size_t> &x, const Graph &g2, const std::vector<size_t> &y) {
        if (!solutions_.empty() && x.size() > solutions_.front().first.size())
            solutions_.clear();
        solutions_.push_back(std::make_pair(x, y));
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
template<class Graph>
std::vector<std::pair<std::vector<size_t>, std::vector<size_t> > >
findMaximumCommonIsomorphicSubgraphs(const Graph &g1, const Graph &g2) {
    CommonSubgraphIsomorphism<Graph, MaximumIsomorphicSubgraphs<Graph> > csi(g1, g2, Message::mlog[Message::DEBUG]);
    csi.monotonicallyIncreasing(true);
    csi.run();
    return csi.solutionProcessor().solutions();
}

template<class Graph, class EquivalenceP>
std::vector<std::pair<std::vector<size_t>, std::vector<size_t> > >
findMaximumCommonIsomorphicSubgraphs(const Graph &g1, const Graph &g2, EquivalenceP equivalenceP) {
    CommonSubgraphIsomorphism<Graph, MaximumIsomorphicSubgraphs<Graph> >
        csi(g1, g2, Message::mlog[Message::DEBUG], equivalenceP);
    csi.monotonicallyIncreasing(true);
    csi.run();
    return csi.solutionProcessor().solutions();
}
/** @} */

} // namespace
} // namespace
} // namespace

#endif
