// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#include <Sawyer/GraphAlgorithm.h>
#include <Sawyer/Message.h>
#include <Sawyer/Stopwatch.h>
#include <Sawyer/Synchronization.h>
#include <boost/assign/list_of.hpp>

using namespace Sawyer::Message::Common;
using namespace Sawyer::Container::Algorithm;
using Sawyer::Message::mlog;

typedef Sawyer::Container::Graph<std::string, std::string> Graph;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Test support functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define check(COND, MESG) ASSERT_always_require2(COND, MESG)

static void
heading(const std::string &title) {
    std::cerr <<std::string(20, '=') <<" " <<title <<"\n";
}
static void
heading(const std::string &title, bool allowDisconnectedSubgraphs) {
    std::cerr <<std::string(20, '=') <<" " <<title <<(allowDisconnectedSubgraphs ? " (disconnected)" : " (connected)") <<"\n";
}

class details {
    std::ostringstream ss_;
public:
    operator std::string() { return "unit test failed\n" + ss_.str(); }

    details() {}

    details& operator<<(const std::string &s) {
        ss_ <<s;
        return *this;
    }

    details& operator<<(size_t n) {
        ss_ <<n;
        return *this;
    }

    details& operator<<(const std::vector<size_t> &v) {
        ss_ <<"[";
        BOOST_FOREACH (size_t i, v)
            ss_ <<" " <<i;
        ss_ <<" ]";
        return *this;
    }

    details& operator<<(const Graph &g) {
        BOOST_FOREACH (const Graph::Vertex &vertex, g.vertices()) {
            ss_ <<"  " <<vertex.value() <<"\t--> {";
            BOOST_FOREACH (const Graph::Edge &edge, vertex.outEdges())
                ss_ <<" " <<edge.target()->value();
            ss_ <<" }\n";
        }
        return *this;
    }
};

// Sort x and at the same time y since changing the order of the vertices makes no difference to the solution.
static void
sort(std::vector<size_t> &x, std::vector<size_t> &y) {
    for (size_t i=0; i+1<x.size(); ++i) {
        for (size_t j=i+1; j<x.size(); ++j) {
            if (x[i] > x[j]) {
                std::swap(x[i], x[j]);
                std::swap(y[i], y[j]);
            }
        }
    }
}

class SolutionChecker {
public:
    typedef std::vector<size_t> VertexIds;
    typedef std::pair<VertexIds, VertexIds> Solution;
    typedef std::vector<Solution> Solutions;

private:
    Solutions solns_;
    bool allowDisconnectedSubgraphs_;

public:
    SolutionChecker()
        : allowDisconnectedSubgraphs_(true) {}

    void allowDisconnectedSubgraphs(bool b) {
        allowDisconnectedSubgraphs_ = b;
    }

    void insert(const VertexIds &x, const VertexIds &y) {
        ASSERT_require2(x.size() == y.size(), "test harness issue");
        solns_.push_back(std::make_pair(x, y));
    }

    bool isSolutionExpected(VertexIds x, VertexIds y) {
        sort(x, y);
        for (size_t i=0; i<solns_.size(); ++i) {
            if (x.size() == solns_[i].first.size() &&
                std::equal(x.begin(), x.end(), solns_[i].first.begin()) &&
                std::equal(y.begin(), y.end(), solns_[i].second.begin())) {
                solns_.erase(solns_.begin()+i);
                return true;
            }
        }
        return false;
    }

    void operator()(const Graph &g1, const VertexIds &x, const Graph &g2, const VertexIds &y) {
        check(x.size() == y.size(), details() <<x.size() <<", " <<y.size());
        if (!allowDisconnectedSubgraphs_) {
            Graph g1sub = graphCopySubgraph(g1, x);
            if (!graphIsConnected(g1sub))
                return;
            Graph g2sub = graphCopySubgraph(g2, y);
            if (!graphIsConnected(g2sub))
                return;
        }
        check(isSolutionExpected(x, y), details() <<"x = " <<x <<", y = " <<y);
    }

    void checkMissing() {
        BOOST_FOREACH (const Solution &soln, solns_) {
            std::cerr <<"Solution not found by the analysis:\n"
                      <<"  x = [";
            BOOST_FOREACH (size_t i, soln.first)
                std::cerr <<" " <<i;
            std::cerr <<" ]\n"
                      <<"  y = [";
            BOOST_FOREACH (size_t j, soln.second)
                std::cerr <<" " <<j;
            std::cerr <<" ]\n";
        }
        check(solns_.empty(), details() <<solns_.size() <<" missing solutions");
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Tests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Test isomorphism of two empty graphs. The only solution should be the empty graphs, which is not considered a valid solution
// since the default minimum solution size is one vertex.
static void
testEmptyGraphs() {
    heading("empty graphs");
    Graph g1, g2;

    CommonSubgraphIsomorphism<Graph, SolutionChecker> csi(g1, g2);
    SolutionChecker &s = csi.solutionProcessor();

    csi.run();
    s.checkMissing();
}

// Test isomorphism of two graphs with two vertices and no edges. There are two full solutions, one which maps vertices with
// the same ID numbers and the other having the vertices swapped. Plus four partial solutions that have only one vertex from
// each graph.
static void
testVertexGraphs(bool allowDisconnectedSubgraphs) {
    heading("graphs with no edges", allowDisconnectedSubgraphs);
    Graph g1, g2;

    g1.insertVertex("v0");
    g1.insertVertex("v1");

    g2.insertVertex("w0");
    g2.insertVertex("w1");

    CommonSubgraphIsomorphism<Graph, SolutionChecker> csi(g1, g2);
    SolutionChecker &s = csi.solutionProcessor();
    s.allowDisconnectedSubgraphs(allowDisconnectedSubgraphs);

    if (allowDisconnectedSubgraphs) {
        s.insert(boost::assign::list_of(0)(1),
                 boost::assign::list_of(0)(1));

        s.insert(boost::assign::list_of(0)(1),
                 boost::assign::list_of(1)(0));
    }

    s.insert(boost::assign::list_of(0),
             boost::assign::list_of(0));

    s.insert(boost::assign::list_of(0),
             boost::assign::list_of(1));

    s.insert(boost::assign::list_of(1),
             boost::assign::list_of(0));

    s.insert(boost::assign::list_of(1),
             boost::assign::list_of(1));
    
    csi.run();
    s.checkMissing();
}

// Test isomorphism when the graphs both have two vertices connected by one edge. There's only one solution of size two, and
// all four possibilities of size 1.
//
//       v0  ---> v1
//
//       w0  ---> w1
//
static void
testOneEdge(bool allowDisconnectedSubgraphs) {
    heading("graphs with one edge", allowDisconnectedSubgraphs);
    Graph g1, g2;

    Graph::ConstVertexIterator v0 = g1.insertVertex("v0");
    Graph::ConstVertexIterator v1 = g1.insertVertex("v1");
    g1.insertEdge(v0, v1, "e01");

    Graph::ConstVertexIterator w0 = g2.insertVertex("w0");
    Graph::ConstVertexIterator w1 = g2.insertVertex("w1");
    g2.insertEdge(w0, w1, "f01");

    CommonSubgraphIsomorphism<Graph, SolutionChecker> csi(g1, g2);
    SolutionChecker &s = csi.solutionProcessor();
    s.allowDisconnectedSubgraphs(allowDisconnectedSubgraphs);

    s.insert(boost::assign::list_of(0)(1),
             boost::assign::list_of(0)(1));

    s.insert(boost::assign::list_of(0),
             boost::assign::list_of(0));

    s.insert(boost::assign::list_of(0),
             boost::assign::list_of(1));

    s.insert(boost::assign::list_of(1),
             boost::assign::list_of(0));

    s.insert(boost::assign::list_of(1),
             boost::assign::list_of(1));
    
    csi.run();
    s.checkMissing();
}

// Test graphs with three edges
//
//      v0 ----> v1 ----> v2
//
//      w0 ----> w1 ----> w2
//
static void
testTwoLinearEdges(bool allowDisconnectedSubgraphs) {
    heading("graphs with two edges", allowDisconnectedSubgraphs);
    Graph g1, g2;

    Graph::ConstVertexIterator v0 = g1.insertVertex("v0");
    Graph::ConstVertexIterator v1 = g1.insertVertex("v1");
    Graph::ConstVertexIterator v2 = g1.insertVertex("v2");
    g1.insertEdge(v0, v1, "e01");
    g1.insertEdge(v1, v2, "e12");

    Graph::ConstVertexIterator w0 = g2.insertVertex("w0");
    Graph::ConstVertexIterator w1 = g2.insertVertex("w1");
    Graph::ConstVertexIterator w2 = g2.insertVertex("w2");
    g2.insertEdge(w0, w1, "f01");
    g2.insertEdge(w1, w2, "f12");

    CommonSubgraphIsomorphism<Graph, SolutionChecker> csi(g1, g2);
    SolutionChecker &s = csi.solutionProcessor();
    s.allowDisconnectedSubgraphs(allowDisconnectedSubgraphs);

    //---------------------
    // Solutions of size 3
    //---------------------

    // 0,1,2
    s.insert(boost::assign::list_of(0)(1)(2),
             boost::assign::list_of(0)(1)(2));

    //--------------------
    // Solution of size 2
    //--------------------

    // 0,1
    s.insert(boost::assign::list_of(0)(1),
             boost::assign::list_of(0)(1));
    s.insert(boost::assign::list_of(0)(1),
             boost::assign::list_of(1)(2));

    // 1,2 (same as 0,1)
    s.insert(boost::assign::list_of(1)(2),
             boost::assign::list_of(0)(1));
    s.insert(boost::assign::list_of(1)(2),
             boost::assign::list_of(1)(2));

    // 0,2
    if (allowDisconnectedSubgraphs) {
        s.insert(boost::assign::list_of(0)(2),
                 boost::assign::list_of(0)(2));
        s.insert(boost::assign::list_of(0)(2),
                 boost::assign::list_of(2)(0));
    }

    //--------------------
    // Solutions of size 1
    //--------------------
    s.insert(boost::assign::list_of(0),
             boost::assign::list_of(0));
    s.insert(boost::assign::list_of(0),
             boost::assign::list_of(1));
    s.insert(boost::assign::list_of(0),
             boost::assign::list_of(2));
    s.insert(boost::assign::list_of(1),
             boost::assign::list_of(0));
    s.insert(boost::assign::list_of(1),
             boost::assign::list_of(1));
    s.insert(boost::assign::list_of(1),
             boost::assign::list_of(2));
    s.insert(boost::assign::list_of(2),
             boost::assign::list_of(0));
    s.insert(boost::assign::list_of(2),
             boost::assign::list_of(1));
    s.insert(boost::assign::list_of(2),
             boost::assign::list_of(2));

    csi.run();
    s.checkMissing();
}

// Test isomorphism when the graphs both have two fully connected vertices.
//
//          ___________
//         /           V
//      v0            v1
//         ^._________/
//
//          ___________
//         /           V
//      w0            w1
//         ^._________/
//
// This has both solutions of size two, and all four solutions of size one.
static void
testTwoCircularEdges(bool allowDisconnectedSubgraphs) {
    heading("graphs with two edges", allowDisconnectedSubgraphs);
    Graph g1, g2;

    Graph::ConstVertexIterator v0 = g1.insertVertex("v0");
    Graph::ConstVertexIterator v1 = g1.insertVertex("v1");
    g1.insertEdge(v0, v1, "e01");
    g1.insertEdge(v1, v0, "e10");

    Graph::ConstVertexIterator w0 = g2.insertVertex("w0");
    Graph::ConstVertexIterator w1 = g2.insertVertex("w1");
    g2.insertEdge(w0, w1, "f01");
    g2.insertEdge(w1, w0, "f10");

    CommonSubgraphIsomorphism<Graph, SolutionChecker> csi(g1, g2);
    SolutionChecker &s = csi.solutionProcessor();
    s.allowDisconnectedSubgraphs(allowDisconnectedSubgraphs);

    s.insert(boost::assign::list_of(0)(1),
             boost::assign::list_of(0)(1));

    s.insert(boost::assign::list_of(0)(1),
             boost::assign::list_of(1)(0));

    s.insert(boost::assign::list_of(0),
             boost::assign::list_of(0));

    s.insert(boost::assign::list_of(0),
             boost::assign::list_of(1));

    s.insert(boost::assign::list_of(1),
             boost::assign::list_of(0));

    s.insert(boost::assign::list_of(1),
             boost::assign::list_of(1));
    
    csi.run();
    s.checkMissing();
}

// Test parallel edges
//
//          __________
//         /          \    .
//        /            V
//      v0            v1             w0 ------> w1
//        \            ^
//         \__________/
//
// This should have no solutions of length 2 since that would create subgraphs with different numbers of edges. All four
// solutions of length one should be present in the output.
static void
testParallelEdges(bool allowDisconnectedSubgraphs) {
    heading("test parallel edges", allowDisconnectedSubgraphs);
    Graph g1, g2;
    
    Graph::ConstVertexIterator v0 = g1.insertVertex("v0");
    Graph::ConstVertexIterator v1 = g1.insertVertex("v1");
    g1.insertEdge(v0, v1, "e01a");
    g1.insertEdge(v0, v1, "e01b");

    Graph::ConstVertexIterator w0 = g2.insertVertex("w0");
    Graph::ConstVertexIterator w1 = g2.insertVertex("w1");
    g2.insertEdge(w0, w1, "f01");

    CommonSubgraphIsomorphism<Graph, SolutionChecker> csi(g1, g2);
    SolutionChecker &s = csi.solutionProcessor();
    s.allowDisconnectedSubgraphs(allowDisconnectedSubgraphs);

    s.insert(boost::assign::list_of(0),
             boost::assign::list_of(0));

    s.insert(boost::assign::list_of(0),
             boost::assign::list_of(1));

    s.insert(boost::assign::list_of(1),
             boost::assign::list_of(0));

    s.insert(boost::assign::list_of(1),
             boost::assign::list_of(1));

    csi.run();
    s.checkMissing();
}

// Test isomorphism of two graphs where each graph has two vertices, one of which has a self-edge.
//
//             _                              _
//            / \                            / \   .
//           v   |                          v   |
//          v0   |   v1                    w0   |     w1
//            \_/                            \_/
static void
testSelfEdges(bool allowDisconnectedSubgraphs) {
    heading("graph with self-edges", allowDisconnectedSubgraphs);
    Graph g1, g2;

    Graph::ConstVertexIterator v0 = g1.insertVertex("v0");
    g1.insertVertex("v1");
    g1.insertEdge(v0, v0, "e00");

    Graph::ConstVertexIterator w0 = g2.insertVertex("w0");
    g2.insertVertex("w1");
    g2.insertEdge(w0, w0, "f00");

    CommonSubgraphIsomorphism<Graph, SolutionChecker> csi(g1, g2);
    SolutionChecker &s = csi.solutionProcessor();
    s.allowDisconnectedSubgraphs(allowDisconnectedSubgraphs);

    if (allowDisconnectedSubgraphs) {
        s.insert(boost::assign::list_of(0)(1),
                 boost::assign::list_of(0)(1));
    }

    s.insert(boost::assign::list_of(0),
             boost::assign::list_of(0));

    s.insert(boost::assign::list_of(1),
             boost::assign::list_of(1));

    csi.run();
    s.checkMissing();
}

// A test that's a bit larger
//
//
//                                 [ 5 ] <----- [ 6 ]
//                                   ^            ^
//                                   \           /
//                                     \       /
//                                       \   /
//                      [ 4 ]            [ 4 ]
//                        ^                ^
//                        |                |
//                        |                |
//         [ 2 ] -----> [ 3 ]            [ 2 ] <----- [ 3 ]
//           ^            ^                ^            ^
//            \          /                  \          /
//              \      /                      \      /
//                \  /                          \  /
//                [ 1 ]                         [ 1 ]
//                  ^                             ^
//                  |                             |
//                  |                             |
//                [ 0 ]                         [ 0 ]
//
static void
testLarger() {
    heading("slightly larger test");
    Graph g1, g2;
    Graph::VertexIterator v0 = g1.insertVertex("v0");
    Graph::VertexIterator v1 = g1.insertVertex("v1");
    Graph::VertexIterator v2 = g1.insertVertex("v2");
    Graph::VertexIterator v3 = g1.insertVertex("v3");
    Graph::VertexIterator v4 = g1.insertVertex("v4");

    g1.insertEdge(v0, v1, "e01");
    g1.insertEdge(v1, v2, "e12");
    g1.insertEdge(v1, v3, "e13");
    g1.insertEdge(v2, v3, "e23");
    g1.insertEdge(v3, v4, "e34");

    Graph::VertexIterator w0 = g2.insertVertex("w0");
    Graph::VertexIterator w1 = g2.insertVertex("w1");
    Graph::VertexIterator w2 = g2.insertVertex("w2");
    Graph::VertexIterator w3 = g2.insertVertex("w3");
    Graph::VertexIterator w4 = g2.insertVertex("w4");
    Graph::VertexIterator w5 = g2.insertVertex("w5");
    Graph::VertexIterator w6 = g2.insertVertex("w6");

    g2.insertEdge(w0, w1, "f01");
    g2.insertEdge(w1, w2, "f12");
    g2.insertEdge(w1, w3, "f13");
    g2.insertEdge(w3, w2, "f32");
    g2.insertEdge(w2, w4, "f24");
    g2.insertEdge(w4, w5, "f45");
    g2.insertEdge(w4, w6, "f46");
    g2.insertEdge(w6, w5, "f65");

    CommonSubgraphIsomorphism<Graph, SolutionChecker> csi(g1, g2);
    SolutionChecker &s = csi.solutionProcessor();
    s.allowDisconnectedSubgraphs(false);                // keeps our solution list a lot smaller

    //---------------------
    // Solutions of size 5
    //---------------------

    // 0,1,2,3,4
    s.insert(boost::assign::list_of(0)(1)(2)(3)(4),
             boost::assign::list_of(0)(1)(3)(2)(4));

    //----------------------
    // Solutions of size 4
    //----------------------

    // 0,1,2,3
    s.insert(boost::assign::list_of(0)(1)(2)(3),
             boost::assign::list_of(0)(1)(3)(2));
    s.insert(boost::assign::list_of(0)(1)(2)(3),
             boost::assign::list_of(2)(4)(6)(5));

    // 0,1,3,4
    s.insert(boost::assign::list_of(0)(1)(3)(4),
             boost::assign::list_of(0)(1)(2)(4));
    s.insert(boost::assign::list_of(0)(1)(3)(4),
             boost::assign::list_of(1)(2)(4)(5));
    s.insert(boost::assign::list_of(0)(1)(3)(4),
             boost::assign::list_of(1)(2)(4)(6));
    s.insert(boost::assign::list_of(0)(1)(3)(4),
             boost::assign::list_of(3)(2)(4)(5));
    s.insert(boost::assign::list_of(0)(1)(3)(4),
             boost::assign::list_of(3)(2)(4)(6));

    // 1,2,3,4
    s.insert(boost::assign::list_of(1)(2)(3)(4),
             boost::assign::list_of(1)(3)(2)(4));

    //---------------------
    // Solutions of size 3
    //---------------------

    // 0,1,2
    s.insert(boost::assign::list_of(0)(1)(2),
             boost::assign::list_of(0)(1)(2));
    s.insert(boost::assign::list_of(0)(1)(2),
             boost::assign::list_of(0)(1)(3));
    s.insert(boost::assign::list_of(0)(1)(2),
             boost::assign::list_of(1)(2)(4));
    s.insert(boost::assign::list_of(0)(1)(2),
             boost::assign::list_of(2)(4)(5));
    s.insert(boost::assign::list_of(0)(1)(2),
             boost::assign::list_of(2)(4)(6));
    s.insert(boost::assign::list_of(0)(1)(2),
             boost::assign::list_of(3)(2)(4));

    // 0,1,3 (same as 0,1,2)
    s.insert(boost::assign::list_of(0)(1)(3),
             boost::assign::list_of(0)(1)(2));
    s.insert(boost::assign::list_of(0)(1)(3),
             boost::assign::list_of(0)(1)(3));
    s.insert(boost::assign::list_of(0)(1)(3),
             boost::assign::list_of(1)(2)(4));
    s.insert(boost::assign::list_of(0)(1)(3),
             boost::assign::list_of(2)(4)(5));
    s.insert(boost::assign::list_of(0)(1)(3),
             boost::assign::list_of(2)(4)(6));
    s.insert(boost::assign::list_of(0)(1)(3),
             boost::assign::list_of(3)(2)(4));

    // 1,2,3
    s.insert(boost::assign::list_of(1)(2)(3),
             boost::assign::list_of(1)(3)(2));
    s.insert(boost::assign::list_of(1)(2)(3),
             boost::assign::list_of(4)(6)(5));

    // 1,3,4 (same as 0,1,2)
    s.insert(boost::assign::list_of(1)(3)(4),
             boost::assign::list_of(0)(1)(2));
    s.insert(boost::assign::list_of(1)(3)(4),
             boost::assign::list_of(0)(1)(3));
    s.insert(boost::assign::list_of(1)(3)(4),
             boost::assign::list_of(1)(2)(4));
    s.insert(boost::assign::list_of(1)(3)(4),
             boost::assign::list_of(2)(4)(5));
    s.insert(boost::assign::list_of(1)(3)(4),
             boost::assign::list_of(2)(4)(6));
    s.insert(boost::assign::list_of(1)(3)(4),
             boost::assign::list_of(3)(2)(4));

    // 2,3,4 (same as 0,1,2)
    s.insert(boost::assign::list_of(2)(3)(4),
             boost::assign::list_of(0)(1)(2));
    s.insert(boost::assign::list_of(2)(3)(4),
             boost::assign::list_of(0)(1)(3));
    s.insert(boost::assign::list_of(2)(3)(4),
             boost::assign::list_of(1)(2)(4));
    s.insert(boost::assign::list_of(2)(3)(4),
             boost::assign::list_of(2)(4)(5));
    s.insert(boost::assign::list_of(2)(3)(4),
             boost::assign::list_of(2)(4)(6));
    s.insert(boost::assign::list_of(2)(3)(4),
             boost::assign::list_of(3)(2)(4));

    csi.minimumSolutionSize(3);
    csi.run();
    s.checkMissing();

    // Verify that findMaximumCommonIsomorphicSubgraphs works.
    heading("slightly larger test (findMaximumCommonIsomorphicSubgraphs)");
    std::vector<std::pair<std::vector<size_t>, std::vector<size_t> > > maxSubgraphs =
        findMaximumCommonIsomorphicSubgraphs(g1, g2);
    check(maxSubgraphs.size() == 1, "should be only one solution of maximal size");
    check(maxSubgraphs[0].first.size() == 5, "max solution should have 5 vertices");
    check(maxSubgraphs[0].second.size() == 5, "max solution should have 5 vertices");
    sort(maxSubgraphs[0].first, maxSubgraphs[0].second);
    check(maxSubgraphs[0].first[0] == 0 && maxSubgraphs[0].second[0] == 0, "wrong solution");
    check(maxSubgraphs[0].first[1] == 1 && maxSubgraphs[0].second[1] == 1, "wrong solution");
    check(maxSubgraphs[0].first[2] == 2 && maxSubgraphs[0].second[2] == 3, "wrong solution");
    check(maxSubgraphs[0].first[3] == 3 && maxSubgraphs[0].second[3] == 2, "wrong solution");
    check(maxSubgraphs[0].first[4] == 4 && maxSubgraphs[0].second[4] == 4, "wrong solution");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Performance testing

struct SolutionCounter {
    size_t nSolutions;
    size_t largestSolution;

    SolutionCounter(): nSolutions(0), largestSolution(0) {}

    void operator()(const Graph &g1, const std::vector<size_t> &g1VertIds,
                    const Graph &g2, const std::vector<size_t> &g2VertIds) {
        ++nSolutions;
        largestSolution = std::max(largestSolution, g1VertIds.size());
    }
};

// Just some stupid way of saying which vertices of g1 can be equivalent to g2
class Equivalence: public CsiEquivalence<Graph> {
public:
    bool mu(const Graph &g1, const Graph::ConstVertexIterator &v1,
            const Graph &g2, const Graph::ConstVertexIterator &v2) const {
        int allowedDelta = (g1.nVertices() + g2.nVertices()) / 8;
        int distance = abs((int)v1->id() - (int)v2->id());
        return distance <= allowedDelta;
    }
};

static void
testRandomGraphs(size_t maxVerts, size_t vertDelta, double edgeRatio) {
    heading("random graphs");
    for (size_t nVertices = 5; nVertices < maxVerts; nVertices += vertDelta) {
        size_t nEdges = round(edgeRatio * nVertices);

        // Build a random graph from scratch so edges are evenly distributed across the whole graph
        Graph g;
        for (size_t i=0; i<nVertices; ++i)
            g.insertVertex("v" + boost::lexical_cast<std::string>(i));
        for (size_t i=0; i<nEdges; ++i) {
            Graph::ConstVertexIterator v1 = g.findVertex(Sawyer::fastRandomIndex(nVertices));
            Graph::ConstVertexIterator v2 = g.findVertex(Sawyer::fastRandomIndex(nVertices));
            g.insertEdge(v1, v2, "e" + boost::lexical_cast<std::string>(i));
        }
        std::cerr <<"|V| = " <<nVertices <<", |E| = " <<nEdges <<"\n";
        
        CommonSubgraphIsomorphism<Graph, SolutionCounter, Equivalence> csi(g, g);
#if 0 // [Robb Matzke 2016-03-24]
        csi.minimumSolutionSize(nVertices-2);
#else
        csi.monotonicallyIncreasing(true);
#endif
        Sawyer::Stopwatch stopwatch;
        std::cerr <<"  starting...\n";
        csi.run();
        std::cerr <<"  " <<csi.solutionProcessor().nSolutions <<" solutions in " <<stopwatch <<" seconds\n";
        if (csi.solutionProcessor().nSolutions > 0)
            std::cerr <<"  largest solution had " <<csi.solutionProcessor().largestSolution <<" vertices\n";
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int main() {
    Sawyer::initializeLibrary();
#if 0 // DEBUGGING [Robb Matzke 2016-03-17]
    Sawyer::Message::mlog[DEBUG].enable();
#endif

    testEmptyGraphs();
    testVertexGraphs(true);
    testVertexGraphs(false);
    testOneEdge(true);
    testOneEdge(false);
    testTwoLinearEdges(true);
    testTwoLinearEdges(false);
    testTwoCircularEdges(true);
    testTwoCircularEdges(false);
    testSelfEdges(true);
    testSelfEdges(false);
    testParallelEdges(true);
    testParallelEdges(false);
    testLarger();
    testRandomGraphs(25, 1, 1.2);
}
