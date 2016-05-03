// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




// Algorithms for Sawyer::Container::Graph

#ifndef Sawyer_GraphAlgorithm_H
#define Sawyer_GraphAlgorithm_H

#include <Sawyer/Sawyer.h>
#include <Sawyer/GraphTraversal.h>
#include <Sawyer/Message.h>
#include <Sawyer/Set.h>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Common subgraph isomorphism (CSI)
// Loosely based on the algorithm presented by Evgeny B. Krissinel and Kim Henrick
// "Common subgraph isomorphism detection by backtracking search"
// European Bioinformatics Institute, Genome Campus, Hinxton, Cambridge CB10 1SD, UK
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class Graph>
class CommonSubgraphIsomorphism {
    typedef std::vector<std::vector<size_t> > Vmm;      // vertex matching matrix

    const Graph &g1, &g2;
    Message::Stream debug;                              // optional debugging stream
    Set<size_t> v, w;                                   // vertex IDs from g1 and g2, respectively
    std::vector<size_t> x, y;                           // vertex equivalence maps
    size_t n0;                                          // minimum size of desired matches
    size_t nmax;                                        // maximum size of matched subsets (num vertices)
public:
    CommonSubgraphIsomorphism(const Graph &g1, const Graph &g2, Message::Stream debug = Message::mlog[Message::DEBUG])
        : g1(g1), g2(g2), debug(debug), n0(1), nmax(0) {
        // Initialize modifiable vertex sets
        for (size_t i=0; i<g1.nVertices(); ++i)
            v.insert(i);
        for (size_t i=0; i<g2.nVertices(); ++i)
            w.insert(i);
    }

    // This is usually user-defined
    bool vertexComparator(const typename Graph::ConstVertexIterator &v1, const typename Graph::ConstVertexIterator &v2) {
        return true;
    }

    // This is usually user-defined
    bool edgeComparator(const std::vector<typename Graph::ConstEdgeIterator> &edges1,
                        const std::vector<typename Graph::ConstEdgeIterator> &edges2) {
        return true;
    }

    // This is usually user-defined
    void output(const std::vector<size_t> &x, const std::vector<size_t> &y) {
        std::cout <<"match found:\n"
                  <<"  x= {";
        BOOST_FOREACH (size_t i, x)
            std::cout <<" " <<i;
        std::cout <<" }\n"
                  <<"  y= {";
        BOOST_FOREACH (size_t j, y)
            std::cout <<" " <<j;
        std::cout <<" }\n";
    }

    void vmmPush(Vmm &d /*in,out*/, size_t i, size_t value) {
        if (i >= d.size())
            d.resize(i+1);
        d[i].push_back(value);
    }

    size_t vmmSize(const Vmm &d, size_t i) {
        return i < d.size() ? d[i].size() : 0;
    }

    const std::vector<size_t>& vmmRow(const Vmm &d, size_t i) {
        static const std::vector<size_t> empty;
        return i < d.size() ? d[i] : empty;
    }

    void vmmPrint(std::ostream &out, const std::string &indent, const std::string &title, const Vmm &d) {
        for (size_t i=0; i<d.size(); ++i) {
            if (!d[i].empty()) {
                out <<indent <<title <<"[" <<i <<"] = {";
                BOOST_FOREACH (size_t j, d[i])
                    out <<" " <<j;
                out <<" }\n";
            }
        }
    }

    void printVector(std::ostream &out, const std::string &indent, const std::string &title, const std::vector<size_t> &v) {
        out <<indent <<title <<"[";
        BOOST_FOREACH (size_t i, v)
            out <<" " <<i;
        out <<" ]\n";
    }
    
    void vmmInit(Vmm &d /*out*/) {
        d.clear();
        BOOST_FOREACH (size_t i, v.values()) {
            BOOST_FOREACH (size_t j, w.values()) {
                if (vertexComparator(g1.findVertex(i), g2.findVertex(j)))
                    vmmPush(d, i, j);
            }
        }
    }

    size_t pickVertex(const Vmm &d) {
        Set<size_t> lv = v;
        for (size_t k=0; k<x.size(); ++k)
            lv.erase(x[k]);
        ASSERT_require(!lv.isEmpty());
#if 1
        return lv.least();
#else
        BOOST_FOREACH (size_t i, lv.values()) {
            if (vmmSize(d, i) == 0)
                continue;
            bool satisfied = true;
            BOOST_FOREACH (size_t j, lv.values()) {
                if (vmmSize(d, j) < vmmSize(d, i)) {
                    satisfied = false;
                    break;
                }
            }
            if (satisfied)
                return i;
        }
#endif
        ASSERT_not_reachable("Not sure what to do here");
    }

    bool isExtendable(const Vmm &d) {
        size_t q = x.size();
        size_t s = q;
        Set<size_t> lv = v;

        // We might not need this step. Perhaps the only d_i rows that are not empty are those where i is not in x
        for (size_t i=0; i<q; ++i)
            lv.erase(x[i]);

        BOOST_FOREACH (size_t i, lv.values()) {
            if (vmmSize(d, i) > 0)
                ++s;
        }
        return s >= std::max(n0, nmax) && s > q;
    }

    const std::vector<size_t>& getMappableVertices(size_t i, const Vmm &d) {
        return vmmRow(d, i);
    }

    std::vector<typename Graph::ConstEdgeIterator> findEdges(const Graph &g, size_t source, size_t target) {
        std::vector<typename Graph::ConstEdgeIterator> retval;
        BOOST_FOREACH (const typename Graph::Edge &candidate, g.findVertex(source)->outEdges()) {
            if (candidate.target()->id() == target)
                retval.push_back(g.findEdge(candidate.id()));
        }
        return retval;
    }

    // We just added source1 and source2 to the subgraphs. Target1 and target2 are unmapped vertices that we could consider
    // next.
    bool suitableEdges(size_t source1, size_t target1, size_t source2, size_t target2) {
        ASSERT_require(source1 != target1);
        if (source2 == target2)
            return false;

        std::vector<typename Graph::ConstEdgeIterator> edges1 = findEdges(g1, source1, target1);
        std::vector<typename Graph::ConstEdgeIterator> edges2 = findEdges(g2, source2, target2);

        // Both subgraphs must have the same number of edges.
        if (edges2.size() != edges1.size())
            return false;

        return edgeComparator(edges1, edges2);
    }

    Vmm refine(const Vmm &d) {
        ASSERT_require(x.size() == y.size());
        Set<size_t> lv = v;
        for (size_t i=0; i<x.size(); ++i)
            lv.erase(x[i]);

        Vmm d1;
        BOOST_FOREACH (size_t i, lv.values()) {
            BOOST_FOREACH (size_t j, d[i]) {
                if (suitableEdges(x.back(), i, y.back(), j))
                    vmmPush(d1, i, j);
            }
        }
        return d1;
    }

    void backtrack(const Vmm &d, size_t level=0) {
        std::string indent;
        static const size_t maxLevel = 9, indentAmount = 2;
        if (debug) {
            if (level > maxLevel) {
                indent = std::string(indentAmount*(maxLevel+1), ' ') + "L" + boost::lexical_cast<std::string>(level) + ": ";
            } else {
                indent = std::string(indentAmount*level, ' ');
            }
            debug <<indent <<"at level " <<level <<":\n";
            printVector(debug, indent, "  x = ", x);
            printVector(debug, indent, "  y = ", y);
            vmmPrint(debug, indent, "  d", d);
        }

        if (isExtendable(d)) {
            int i = pickVertex(d);
            std::vector<size_t> z = getMappableVertices(i, d);
            if (debug) {
                debug <<indent <<"  pick v[" <<i <<"] mappable to any of w";
                printVector(debug, "", "", z);
            }

            BOOST_FOREACH (size_t j, z) {
                SAWYER_MESG(debug) <<indent <<"  trying v[" <<i <<"] mapped to w[" <<j <<"]\n";
                x.push_back(i);
                y.push_back(j);
                Vmm d1 = refine(d);
                backtrack(d1, level+1);
                x.pop_back();
                y.pop_back();
            }

            SAWYER_MESG(debug) <<indent <<"  backtracking by removing v[" <<i <<"] from consideration\n";
            v.erase(i);
            backtrack(d, level+1);
            v.insert(i);
        } else {
            nmax = std::max(nmax, x.size());
            if (debug) {
                printVector(debug, indent, "  solution x = ", x);
                printVector(debug, indent, "  solution y = ", y);
                debug <<indent <<"  n0 = " <<n0 <<", nmax = " <<nmax <<"\n";
            }
            output(x, y);
        }
    }

    void run() {
        Vmm d;
        vmmInit(d);
        backtrack(d);
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
        csi(g1, g2, Message::mlog[Message::DEBUG], MaximumIsomorphicSubgraphs<Graph>(), equivalenceP);
    csi.monotonicallyIncreasing(true);
    csi.run();
    return csi.solutionProcessor().solutions();
}
/** @} */

} // namespace
} // namespace
} // namespace

#endif
