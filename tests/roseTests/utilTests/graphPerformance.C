/* Tests how well a graph performs at various operations. */
#include "rose.h"
#include "Graph3.h"

#include <algorithm>
#include <boost/graph/adjacency_list.hpp>
#include <boost/lexical_cast.hpp>
#include <Sawyer/CommandLine.h>
#include <Sawyer/GraphBoost.h>
#include <Sawyer/PoolAllocator.h>
#include <Sawyer/Stopwatch.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

using namespace rose;

#define MAX_VERTICES (2*1000*1000)
#define MAX_EDGES (4*MAX_VERTICES)
#define MAX_COUNT (boost::integer_traits<size_t>::const_max - 1000)

static size_t nruns = 1;                                // total number of runs per test

struct Totals {
    std::vector<uint64_t> nIters;
    std::vector<double> elapsed;
    Totals() {}
    Totals(const Totals &other): nIters(other.nIters), elapsed(other.elapsed) {}
    Totals(uint64_t nIters, double elapsed) {
        this->nIters.push_back(nIters);
        this->elapsed.push_back(elapsed);
    }

    size_t size() const {
        assert(nIters.size()==elapsed.size());
        return nIters.size();
    }

    Totals& operator+=(const Totals &other) {
        for (size_t i=0; i<other.size(); ++i) {
            nIters.push_back(other.nIters[i]);
            elapsed.push_back(other.elapsed[i]);
        }
        return *this;
    }

    // Data points that are valid
    Totals valid() const {
        Totals retval;
        for (size_t i=0; i<nIters.size(); ++i) {
            double rate = nIters[i] / elapsed[i];
            if (!isnan(rate))
                retval += Totals(nIters[i], elapsed[i]);
        }
        return retval;
    }
    
    Totals noOutliers() const {
        // Consider only rates that are numbers
        Totals t = valid();
        if (0==t.size())
            return t;

        // Find the average rate, but only over the rates that are valid numbers
        double rate_ave = 0.0;
        for (size_t i=0; i<t.size(); ++i)
            rate_ave += t.nIters[i] / t.elapsed[i];
        rate_ave /= t.size();

        // Standard deviation
        double sigma = 0.0;
        for (size_t i=0; i<t.size(); ++i) {
            double diff = (t.nIters[i] / t.elapsed[i]) - rate_ave;
            sigma += diff * diff;
        }
        sigma = sqrt(sigma / t.size());

        // Throw away anything more than 2sigma from the average
        Totals retval;
        for (size_t i=0; i<t.size(); ++i) {
            if (fabs(t.nIters[i] / t.elapsed[i] - rate_ave) < 2*sigma)
                retval += Totals(t.nIters[i], t.elapsed[i]);
        }
        return retval;
    }

    double average() const {
        Totals t = noOutliers();
        double rate_sum = 0.0;
        for (size_t i=0; i<t.size(); ++i)
            rate_sum += t.nIters[i] / t.elapsed[i];
        return rate_sum / t.size();
    }

    double maximum() const {
        double best = 0.0;
        for (size_t i=0; i<size(); ++i) {
            double rate = nIters[i] / elapsed[i];
            if (!isnan(rate))
                best = std::max(best, rate);
        }
        return best;
    }
};

/******************************************************************************************************************************
 *                                      Deadman alarm
 ******************************************************************************************************************************/

// volatile needed because -O3 will optimize away some reads and thus never realize that a signal handler has set the value.
static volatile sig_atomic_t had_alarm = -1;

// signal handler
static void
alarm_handler(int)
{
    had_alarm = 1;
}

static void
cancel_deadman()
{
    alarm(0);
    had_alarm = 0;
}

static void
start_deadman(int seconds)
{
    alarm(0);
    if (had_alarm<0) {
        struct sigaction sa;
        sa.sa_handler = alarm_handler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = SA_RESTART;
        int status __attribute__((unused)) = sigaction(SIGALRM, &sa, NULL);
        assert(status>=0);
    }
    cancel_deadman();
    alarm(seconds);
}

/*******************************************************************************************************************************
 *                                      Graph size
 *******************************************************************************************************************************/

typedef std::pair<size_t, size_t> GraphSize;

template<class GraphType>
static GraphSize
yagi_size(const GraphType &g)
{
    return GraphSize(g.num_vertices(), g.num_edges());
}

template<class GraphType>
static GraphSize
sgl_size(const GraphType &g)
{
    return GraphSize(g.nVertices(), g.nEdges());
}

template<class GraphType>
static GraphSize
bgl_size(GraphType &g)
{
    return GraphSize(boost::num_vertices(g), boost::num_edges(g));
}

template<class GraphType>
static GraphSize
sage_size(const GraphType &g)
{
    return GraphSize(g->numberOfGraphNodes(), g->numberOfGraphEdges());
}

/*******************************************************************************************************************************
 *                                      Report
 *******************************************************************************************************************************/

static std::string
pretty(uint64_t v)
{
    std::string t, s = boost::lexical_cast<std::string>(v);
    for (size_t i=0; i<s.size(); ++i) {
        if (0 == i % 3 && i > 0)
            t += ",";
        t += s[s.size()-(i+1)];
    }
    std::reverse(t.begin(), t.end());
    return t;
}

static void
report_separator()
{
    std::cout <<"  " <<std::string(16, '-')
              <<"  " <<std::string(13, '-') <<"-" <<std::string(13, '-')
              <<"  " <<std::string(13, '-') <<"---" <<std::string(6, '-')
              <<"  " <<std::string(13, '-') <<"--------\n";
}

static void
report_head(const std::string &title)
{
    std::cout <<"\n" <<title <<"...\n";
    printf("  %16s  %13s %13s  %13s   %6s  %13s\n",
           "", "Vertices", "Edges", "Iterations", "Time", "Rate");
    report_separator();
}

static Totals
report(const std::string &title, const GraphSize gsize, size_t count, const Sawyer::Stopwatch &t, const std::string &units)
{
    printf("  %-16s (%13s %13s) %13s / %6.3f  ",
           title.c_str(),
           pretty(gsize.first).c_str(), pretty(gsize.second).c_str(),
           pretty(count).c_str(), t.report());
    double rate = count / t.report();
    if (rate >= 10e9) {
        printf("%13g", rate);
    } else {
        uint64_t irate = rate;
        printf("%13s", pretty(irate).c_str());
    }
    printf(" %s\n", units.c_str());
    return Totals(count, t.report());
}

static void
report_totals(const Totals &totals)
{
    if (totals.size()>1) {
        std::cout <<std::string(64, ' ') <<"Average: ";
        double rate = totals.average();
        if (rate >= 10e9) {
            printf("%13g\n", rate);
        } else {
            uint64_t irate = rate;
            printf("%13s\n", pretty(irate).c_str());
        }

        std::cout <<std::string(64, ' ') <<"Maximum: ";
        double best = totals.maximum();
        if (best >= 10e9) {
            printf("%13g\n", best);
        } else {
            uint64_t irate = best;
            printf("%13s\n", pretty(irate).c_str());
        }
    }
}
    
/******************************************************************************************************************************
 *                                      Graph utils
 ******************************************************************************************************************************/
template<typename T>
static void
shuffle(std::vector<T> &v)
{
    size_t sz = v.size();
    for (size_t i=0; i<sz; ++i)
        std::swap(v[i], v[rand()%sz]);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class GraphType>
static typename GraphType::VertexDescriptor
yagi_random_vertex(const GraphType &g)
{
    size_t n = g.num_vertices();
    assert(n>0);
    return rand() % n;
}

template<class GraphType>
static typename GraphType::VertexIterator
sgl_random_vertex(GraphType &g)
{
    size_t n = g.nVertices();
    assert(n>0);
    size_t id = rand() % n;
    return g.findVertex(id);
}

template<class GraphType>
static std::vector<typename boost::graph_traits<GraphType>::vertex_descriptor>
bgl_vertex_vector(GraphType &g)
{
    typename boost::graph_traits<GraphType>::vertex_iterator vi, vi_end;
    boost::tie(vi, vi_end) = boost::vertices(g);
    std::vector<typename boost::graph_traits<GraphType>::vertex_descriptor> retval;
    for (/*void*/; vi!=vi_end; ++vi)
        retval.push_back(*vi);
    return retval;
}

template<class GraphType>
static typename boost::graph_traits<GraphType>::vertex_descriptor
bgl_random_vertex(const GraphType &g, const std::vector<typename boost::graph_traits<GraphType>::vertex_descriptor> &vertices)
{
    size_t n = vertices.size();
    assert(n>0);
    return vertices[rand() % n];
}

template<class GraphType>
static std::vector<SgGraphNode*>
sage_vertex_vector(GraphType *g)
{
    std::set<SgGraphNode*> vset = g->computeNodeSet();
    return std::vector<SgGraphNode*>(vset.begin(), vset.end());
}

template<class GraphType>
static SgGraphNode *
sage_random_vertex(const GraphType &g, const std::vector<SgGraphNode*> &vertices)
{
    size_t n = vertices.size();
    assert(n>0);
    return vertices[rand() % n];
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class GraphType>
static typename GraphType::EdgeDescriptor
yagi_random_edge(const GraphType &g)
{
    size_t n = g.num_edges();
    assert(n>0);
    return rand() % n;
}

template<class GraphType>
static typename GraphType::EdgeIterator
sgl_random_edge(GraphType &g)
{
    size_t n = g.nEdges();
    assert(n>0);
    size_t id = rand() % n;
    return g.findEdge(id);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Build a graph with up to max_vertices (added in about vertex_time_limit seconds) and some edges.  The number of edges is
// calculated edge_to_vertex_ratio.
template<class GraphType>
static void
yagi_random_graph(GraphType &g, size_t max_vertices, int vertex_time_limit, double edge_to_vertex_ratio)
{
    start_deadman(vertex_time_limit);
    for (size_t i=0; i<max_vertices && !had_alarm; ++i)
        g.add_vertex();

    for (size_t i=g.num_vertices()*edge_to_vertex_ratio; i>0; --i)
        g.add_edge(yagi_random_vertex(g), yagi_random_vertex(g));
}

template<class GraphType>
static void
sgl_random_graph(GraphType &g, size_t max_vertices, int vertex_time_limit, double edge_to_vertex_ratio)
{
    start_deadman(vertex_time_limit);
    for (size_t i=0; i<max_vertices && !had_alarm; ++i)
        g.insertVertex(0);

    for (size_t i=g.nVertices()*edge_to_vertex_ratio; i>0; --i)
        g.insertEdge(sgl_random_vertex(g), sgl_random_vertex(g), 0);
}

template<class GraphType>
void
bgl_random_graph(GraphType &g, size_t max_vertices, int vertex_time_limit, double edge_to_vertex_ratio)
{
    typedef typename boost::graph_traits<GraphType>::vertex_descriptor VertexDescriptor;

    start_deadman(vertex_time_limit);
    for (size_t i=0; i<max_vertices && !had_alarm; ++i)
        boost::add_vertex(g);

    std::vector<VertexDescriptor> vertices = bgl_vertex_vector(g);
    for (size_t i=boost::num_vertices(g)*edge_to_vertex_ratio; i>0; --i)
        boost::add_edge(bgl_random_vertex(g, vertices), bgl_random_vertex(g, vertices), g);
}

template<class GraphType>
void
sage_random_graph(GraphType *g, size_t max_vertices, int vertex_time_limit, double edge_to_vertex_ratio)
{
    start_deadman(vertex_time_limit);
    for (size_t i=0; i<max_vertices && !had_alarm; ++i)
        g->addNode(new SgGraphNode);

    std::vector<SgGraphNode*> vertices = sage_vertex_vector(g);
    for (size_t i=g->numberOfGraphNodes()*edge_to_vertex_ratio; i>0; --i)
        g->addEdge(sage_random_vertex(g, vertices), sage_random_vertex(g, vertices));
}
    


/******************************************************************************************************************************
 *                                      Timing tests
 ******************************************************************************************************************************/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class GraphType>
Totals
yagi_time_add_vertex()
{
    GraphType g;
    start_deadman(2);
    Sawyer::Stopwatch t;
    while (!had_alarm && g.num_vertices()<MAX_VERTICES)
        g.add_vertex();
    t.stop();
    return report("add vertex", yagi_size(g), g.num_vertices(), t, "verts/s");
}

template<class GraphType>
Totals
sgl_time_add_vertex()
{
    GraphType g;
    start_deadman(2);
    Sawyer::Stopwatch t;
    while (!had_alarm && g.nVertices()<MAX_VERTICES)
        g.insertVertex(0);
    t.stop();
    return report("add vertex", sgl_size(g), g.nVertices(), t, "verts/s");
}

template<class GraphType>
Totals
bgl_time_add_vertex()
{
    GraphType g;
    start_deadman(2);
    Sawyer::Stopwatch t;
    while (!had_alarm && boost::num_vertices(g)<MAX_VERTICES)
        boost::add_vertex(g);
    t.stop();
    return report("add vertex", bgl_size(g), boost::num_vertices(g), t, "verts/s");
}

template<class GraphType>
Totals
sage_time_add_vertex()
{
    GraphType *g = new GraphType;
    start_deadman(2);
    Sawyer::Stopwatch t;
    while (!had_alarm && g->numberOfGraphNodes()<MAX_VERTICES)
        g->addNode(new SgGraphNode);
    t.stop();
    return report("add vertex", sage_size(g), g->numberOfGraphNodes(), t, "verts/s");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  The add-edge tests also test the performance for counting edges in the graph.
//
template<class GraphType>
Totals
yagi_time_add_edge()
{
    GraphType g;
    start_deadman(2);
    while (!had_alarm && g.num_vertices()<MAX_VERTICES)
        g.add_vertex();

    start_deadman(2);
    Sawyer::Stopwatch t;
    while (!had_alarm && g.num_edges()<MAX_EDGES) {
        typename GraphType::VertexDescriptor v1 = yagi_random_vertex(g);
        typename GraphType::VertexDescriptor v2 = yagi_random_vertex(g);
        g.add_edge(v1, v2);
    }
    t.stop();
    return report("add edge", yagi_size(g), g.num_edges(), t, "edges/s");
}

template<class GraphType>
Totals
sgl_time_add_edge()
{
    GraphType g;
    start_deadman(2);
    while (!had_alarm && g.nVertices()<MAX_VERTICES)
        g.insertVertex(0);

    start_deadman(2);
    Sawyer::Stopwatch t;
    while (!had_alarm && g.nEdges()<MAX_EDGES) {
        typename GraphType::VertexIterator v1 = sgl_random_vertex(g);
        typename GraphType::VertexIterator v2 = sgl_random_vertex(g);
        g.insertEdge(v1, v2, 0);
    }
    t.stop();
    return report("add edge", sgl_size(g), g.nEdges(), t, "edges/s");
}

template<class GraphType>
Totals
bgl_time_add_edge()
{
    GraphType g;
    start_deadman(2);
    while (!had_alarm && boost::num_vertices(g)<MAX_VERTICES)
        boost::add_vertex(g);

    // Build a list of vertices that we can index in constant time, regardless of the BGL graph type
    typedef typename boost::graph_traits<GraphType>::vertex_descriptor VertexDescriptor;
    std::vector<VertexDescriptor> vertices = bgl_vertex_vector(g);
    assert(vertices.size()==boost::num_vertices(g));

    // The actual test
    start_deadman(2);
    Sawyer::Stopwatch t;
    while (!had_alarm && boost::num_edges(g)<MAX_EDGES) {
        typename boost::graph_traits<GraphType>::vertex_descriptor v1 = bgl_random_vertex(g, vertices);
        typename boost::graph_traits<GraphType>::vertex_descriptor v2 = bgl_random_vertex(g, vertices);
        boost::add_edge(v1, v2, g);
    }
    t.stop();
    return report("add edge", bgl_size(g), boost::num_edges(g), t, "edges/s");
}

template<class GraphType>
Totals
sage_time_add_edge()
{
    // Insert vertices
    GraphType *g = new GraphType;
    start_deadman(2);
    while (!had_alarm && g->numberOfGraphNodes()<MAX_VERTICES)
        g->addNode(new SgGraphNode);

    // Build a list of vertices that we can index in constant time.
    std::vector<SgGraphNode*> vertices = sage_vertex_vector(g);

    // The actual test
    start_deadman(2);
    Sawyer::Stopwatch t;
    while (!had_alarm && g->numberOfGraphEdges()<MAX_EDGES) {
        SgGraphNode *v1 = sage_random_vertex(g, vertices);
        SgGraphNode *v2 = sage_random_vertex(g, vertices);
        g->addEdge(v1, v2);
    }
    t.stop();
    return report("add edge", sage_size(g), g->numberOfGraphEdges(), t, "edges/s");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class GraphType>
Totals
yagi_time_vertex_traversal()
{
    GraphType g;
    yagi_random_graph(g, MAX_VERTICES, 2, 4.0);
    size_t niter=0;
    start_deadman(2);
    Sawyer::Stopwatch t;
    while (!had_alarm && niter<MAX_COUNT) {
        std::pair<typename GraphType::VertexIterator, typename GraphType::VertexIterator> vi_pair = g.vertices();
        while (vi_pair.first!=vi_pair.second && !had_alarm) {
            ++niter;
            ++vi_pair.first;
        }
    }
    t.stop();
    return report("vert iter", yagi_size(g), niter, t, "verts/s");
}

template<class GraphType>
Totals
sgl_time_vertex_traversal()
{
    GraphType g;
    sgl_random_graph(g, MAX_VERTICES, 2, 4.0);
    size_t niter=0;
    start_deadman(2);
    Sawyer::Stopwatch t;
    while (!had_alarm && niter<MAX_COUNT) {
        boost::iterator_range<typename GraphType::VertexIterator> vi_pair = g.vertices();
        for (typename GraphType::VertexIterator iter=vi_pair.begin(); iter!=vi_pair.end() && !had_alarm; ++iter)
            ++niter;
    }
    t.stop();
    return report("vert iter", sgl_size(g), niter, t, "verts/s");
}

template<class GraphType>
Totals
bgl_time_vertex_traversal()
{
    GraphType g;
    bgl_random_graph(g, MAX_VERTICES, 2, 4.0);
    size_t niter=0;
    start_deadman(2);
    Sawyer::Stopwatch t;
    while (!had_alarm && niter<MAX_COUNT) {
        typename boost::graph_traits<GraphType>::vertex_iterator vi, vi_end;
        for (boost::tie(vi, vi_end)=boost::vertices(g); vi!=vi_end && !had_alarm; ++vi)
            ++niter;
    }
    t.stop();
    return report("vert iter", bgl_size(g), niter, t, "verts/s");
}

template<class GraphType>
Totals
sage_time_vertex_traversal()
{
    GraphType *g = new GraphType;
    sage_random_graph(g, MAX_VERTICES, 2, 4.0);
    size_t niter=0;
    start_deadman(2);
    Sawyer::Stopwatch t;
    while (!had_alarm && niter<MAX_COUNT) {
        std::set<SgGraphNode*> vertex_set = g->computeNodeSet();
        for (std::set<SgGraphNode*>::iterator vi=vertex_set.begin(); vi!=vertex_set.end() && !had_alarm; ++vi)
            ++niter;
    }
    t.stop();
    return report("vert iter", sage_size(g), niter, t, "verts/s");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class GraphType>
Totals
yagi_time_edge_traversal()
{
    GraphType g;
    yagi_random_graph(g, MAX_VERTICES, 2, 4.0);
    size_t niter=0;
    start_deadman(2);
    Sawyer::Stopwatch t;
    while (!had_alarm && niter<MAX_COUNT) {
        typename GraphType::EdgeIterator ei, ei_end;
        for (boost::tie(ei, ei_end) = g.edges(); ei!=ei_end && !had_alarm; ++ei)
            ++niter;
    }
    t.stop();
    return report("edge iter", yagi_size(g), niter, t, "edges/s");
}

template<class GraphType>
Totals
sgl_time_edge_traversal()
{
    GraphType g;
    sgl_random_graph(g, MAX_VERTICES, 2, 4.0);
    size_t niter=0;
    start_deadman(2);
    Sawyer::Stopwatch t;
    while (!had_alarm && niter<MAX_COUNT) {
        boost::iterator_range<typename GraphType::EdgeIterator> edges = g.edges();
        for (typename GraphType::EdgeIterator edge=edges.begin(); edge!=edges.end() && !had_alarm; ++edge)
            ++niter;
    }
    t.stop();
    return report("edge iter", sgl_size(g), niter, t, "edges/s");
}

template<class GraphType>
Totals
bgl_time_edge_traversal()
{
    GraphType g;
    bgl_random_graph(g, MAX_VERTICES, 2, 4.0);
    size_t niter=0;
    start_deadman(2);
    Sawyer::Stopwatch t;
    while (!had_alarm && niter<MAX_COUNT) {
        typename boost::graph_traits<GraphType>::edge_iterator ei, ei_end;
        for (boost::tie(ei, ei_end) = boost::edges(g); ei!=ei_end && !had_alarm; ++ei)
            ++niter;
    }
    t.stop();
    return report("edge iter", bgl_size(g), niter, t, "edges/s");
}

template<class GraphType>
Totals
sage_time_edge_traversal()
{
    GraphType *g = new GraphType;
    sage_random_graph(g, MAX_VERTICES, 2, 4.0);
    size_t niter=0;
    start_deadman(2);
    Sawyer::Stopwatch t;
    while (!had_alarm && niter<MAX_COUNT) {
        // SgGraph doesn't have a graph-wide edge iterator, so we have to iterator over the vertices and then each edge.
        std::set<SgGraphNode*> vertex_set = g->computeNodeSet();
        for (std::set<SgGraphNode*>::iterator vi=vertex_set.begin(); vi!=vertex_set.end() && !had_alarm; ++vi) {
            std::set<SgGraphEdge*> edge_set = g->computeEdgeSet(*vi);
            for (std::set<SgGraphEdge*>::iterator ei=edge_set.begin(); ei!=edge_set.end() && !had_alarm; ++ei)
                ++niter;
        }
    }
    t.stop();
    return report("edge iter", sage_size(g), niter, t, "edges/s");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class GraphType>
Totals
yagi_time_remove_vertex()
{
    GraphType g;
    start_deadman(2);
    while (!had_alarm && g.num_vertices()<MAX_VERTICES)
        g.add_vertex();
    GraphSize gsize = yagi_size(g);
    size_t nv_orig = gsize.first;

    start_deadman(2);
    Sawyer::Stopwatch t;
    for (size_t i=0; i<nv_orig && !had_alarm; ++i)
        g.remove_vertex(yagi_random_vertex(g));
    t.stop();
    size_t nremoved = nv_orig - g.num_vertices();
    return report("vert erase", gsize, nremoved, t, "verts/s");
}

template<class GraphType>
Totals
sgl_time_remove_vertex()
{
    GraphType g;
    start_deadman(2);
    while (!had_alarm && g.nVertices()<MAX_VERTICES)
        g.insertVertex(0);
    GraphSize gsize = sgl_size(g);
    size_t nv_orig = gsize.first;

    start_deadman(2);
    Sawyer::Stopwatch t;
    for (size_t i=0; i<nv_orig && !had_alarm; ++i)
        g.eraseVertex(sgl_random_vertex(g));
    t.stop();
    size_t nremoved = nv_orig - g.nVertices();
    return report("vert erase", gsize, nremoved, t, "verts/s");
}

template<class GraphType>
Totals
bgl_time_remove_vertex()
{
#if 1
    // Boost 1.56 with gcc-4.9.4-c++11 tries to invoke deleted copy constructor for boost::detail::stored_edge_property<...>
    // when the edges are stored as setS, therefore we can either specialize this function to not test that particular kind of
    // boost::adjacency_list, or just disable this test for all boost graphs. I'm choosing the latter because why bother
    // testing performance when we already know its bad, and why would ROSE generic graph algorithms want to ever delete a
    // vertex knowing that performance is bad and that they have to specialize based on graph type.
    std::cout <<"  remove vertex:   not implemented for this graph type\n";
#else
    GraphType g;
    start_deadman(2);
    while (!had_alarm && boost::num_vertices(g)<MAX_VERTICES)
        boost::add_vertex(g);
    GraphSize gsize = bgl_size(g);
    size_t nv_orig = gsize.first;

    // Vertex removal is SO SLOW that we need more time to measure it accurately.  To quote from chapter 11 "Performance
    // Guidelines" of the the "Boost Graph Library: User Guide and Reference Manual" book,
    //     Other variations [than list-list] of adjacency list perform horribly on this operation because its
    //     implementation is not of constant time complexity.
    start_deadman(6);
    Sawyer::Stopwatch t;
    for (size_t i=0; i<nv_orig && !had_alarm; ++i) {
        // Note that this won't work generically.  It's only good for graphs where vertices are numbered sequentially across
        // the entire graph.
        typename boost::graph_traits<GraphType>::vertex_descriptor v = rand() % boost::num_vertices(g);
        boost::remove_vertex(v, g);
    }
    t.stop();
    size_t nremoved = nv_orig - boost::num_vertices(g);
    return report("vert erase", gsize, nremoved, t, "verts/s");
#endif
}

template<class GraphType>
Totals
sage_time_remove_vertex()
{
    std::cout <<"  remove vertex:    " <<"not implemented for this graph type\n";
    return Totals();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class GraphType>
static Totals
yagi_time_remove_edge()
{
    GraphType g;
    yagi_random_graph(g, MAX_VERTICES, 2, 4.0);
    GraphSize gsize = yagi_size(g);

    start_deadman(2);
    Sawyer::Stopwatch t;
    size_t ne_orig = g.num_edges();
    for (size_t i=0; i<ne_orig && !had_alarm; ++i)
        g.remove_edge(yagi_random_edge(g));
    t.stop();
    size_t nremoved = ne_orig - g.num_edges();
    return report("edge erase", gsize, nremoved, t, "edges/s");
}

template<class GraphType>
static Totals
sgl_time_remove_edge()
{
    GraphType g;
    sgl_random_graph(g, MAX_VERTICES, 2, 4.0);
    GraphSize gsize = sgl_size(g);

    start_deadman(2);
    Sawyer::Stopwatch t;
    size_t ne_orig = g.nEdges();
    for (size_t i=0; i<ne_orig && !had_alarm; ++i)
        g.eraseEdge(sgl_random_edge(g));
    t.stop();
    size_t nremoved = ne_orig - g.nEdges();
    return report("edge erase", gsize, nremoved, t, "edges/s");
}

template<class GraphType>
static Totals
bgl_time_remove_edge()
{
    std::cout <<"  remove edge:      " <<"interface is not suitable for generic timing\n";
    return Totals();
}

template<class GraphType>
static Totals
sage_time_remove_edge()
{
    std::cout <<"  remove edge:      " <<"not implemented for this graph type\n";
    return Totals();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class GraphType>
static Totals
yagi_time_clear_vertex()
{
    GraphType g;
    yagi_random_graph(g, MAX_VERTICES, 2, 4.0);
    std::vector<typename GraphType::VertexDescriptor> vertices;
    for (size_t i=0; i<g.num_vertices(); ++i)
        vertices.push_back(i);
    shuffle(vertices);

    GraphSize gsize = yagi_size(g);
    size_t ncleared;

    start_deadman(6);           // vertex clearing is SO SLOW that we need more time (see bgl_time_remove_vertex())
    Sawyer::Stopwatch t;
    for (ncleared=0; ncleared<gsize.first && !had_alarm; ++ncleared)
        g.clear_vertex(vertices[ncleared]);
    t.stop();
    size_t ne_removed = gsize.second - g.num_edges();
    report("vert clear", gsize, ncleared, t, "verts/s");
    return report("vert clear", gsize, ne_removed, t, "edges/s");
}

template<class GraphType>
static Totals
sgl_time_clear_vertex()
{
    GraphType g;
    sgl_random_graph(g, MAX_VERTICES, 2, 4.0);
    std::vector<typename GraphType::VertexIterator> vertices;
    for (size_t i=0; i<g.nVertices(); ++i)
        vertices.push_back(g.findVertex(i));
    shuffle(vertices);

    GraphSize gsize = sgl_size(g);
    size_t ncleared;

    start_deadman(6);           // vertex clearing is SO SLOW that we need more time (see bgl_time_remove_vertex())
    Sawyer::Stopwatch t;
    for (ncleared=0; ncleared<gsize.first && !had_alarm; ++ncleared)
        g.clearEdges(vertices[ncleared]);
    t.stop();
    size_t ne_removed = gsize.second - g.nEdges();
    report("vert clear", gsize, ncleared, t, "verts/s");
    return report("vert clear", gsize, ne_removed, t, "edges/s");
}

template<class GraphType>
static Totals
bgl_time_clear_vertex()
{
    GraphType g;
    bgl_random_graph(g, MAX_VERTICES, 2, 4.0);
    std::vector<typename boost::graph_traits<GraphType>::vertex_descriptor> vertices = bgl_vertex_vector(g);
    shuffle(vertices);

    GraphSize gsize = bgl_size(g);
    size_t ncleared;

    start_deadman(2);
    Sawyer::Stopwatch t;
    for (ncleared=0; ncleared<gsize.first && !had_alarm; ++ncleared)
        boost::clear_vertex(vertices[ncleared], g);
    t.stop();
    size_t ne_removed = gsize.second - boost::num_edges(g);
    report("vert clear", gsize, ncleared, t, "verts/s");
    return report("vert clear", gsize, ne_removed, t, "edges/s");
}

template<class GraphType>
static Totals
sage_time_clear_vertex()
{
    std::cout <<"  clear vertex:     " <<"not implemented for this graph type\n";
    return Totals();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class GraphType>
static void
yagi_test_all(const std::string &title)
{
    report_head(title);

    Totals totals;
    for (size_t i=0; i<nruns; ++i)
        totals += yagi_time_add_vertex<GraphType>();
    report_totals(totals);

    totals = Totals();
    for (size_t i=0; i<nruns; ++i)
        totals += yagi_time_add_edge<GraphType>();
    report_totals(totals);

    totals = Totals();
    for (size_t i=0; i<nruns; ++i)
        totals += yagi_time_vertex_traversal<GraphType>();
    report_totals(totals);

    totals = Totals();
    for (size_t i=0; i<nruns; ++i)
        totals += yagi_time_edge_traversal<GraphType>();
    report_totals(totals);

    totals = Totals();
    for (size_t i=0; i<nruns; ++i)
        totals += yagi_time_remove_vertex<GraphType>();
    report_totals(totals);

    totals = Totals();
    for (size_t i=0; i<nruns; ++i)
        totals += yagi_time_remove_edge<GraphType>();
    report_totals(totals);

    totals = Totals();
    for (size_t i=0; i<nruns; ++i)
        totals += yagi_time_clear_vertex<GraphType>();
    report_totals(totals);
}

template<class GraphType>
static void
sgl_test_all(const std::string &title)
{
    report_head(title);

    Totals totals;
    for (size_t i=0; i<nruns; ++i)
        totals += sgl_time_add_vertex<GraphType>();
    report_totals(totals);

    totals = Totals();
    for (size_t i=0; i<nruns; ++i)
        totals += sgl_time_add_edge<GraphType>();
    report_totals(totals);

    totals = Totals();
    for (size_t i=0; i<nruns; ++i)
        totals += sgl_time_vertex_traversal<GraphType>();
    report_totals(totals);

    totals = Totals();
    for (size_t i=0; i<nruns; ++i)
        totals += sgl_time_edge_traversal<GraphType>();
    report_totals(totals);

    totals = Totals();
    for (size_t i=0; i<nruns; ++i)
        totals += sgl_time_remove_vertex<GraphType>();
    report_totals(totals);

    totals = Totals();
    for (size_t i=0; i<nruns; ++i)
        totals += sgl_time_remove_edge<GraphType>();
    report_totals(totals);

    totals = Totals();
    for (size_t i=0; i<nruns; ++i)
        totals += sgl_time_clear_vertex<GraphType>();
    report_totals(totals);
}


template<class GraphType>
static void
bgl_test_all(const std::string &title)
{
    report_head(title);

    Totals totals;
    for (size_t i=0; i<nruns; ++i)
        totals += bgl_time_add_vertex<GraphType>();
    report_totals(totals);

    totals = Totals();
    for (size_t i=0; i<nruns; ++i)
        totals += bgl_time_add_edge<GraphType>();
    report_totals(totals);

    totals = Totals();
    for (size_t i=0; i<nruns; ++i)
        totals += bgl_time_vertex_traversal<GraphType>();
    report_totals(totals);

    totals = Totals();
    for (size_t i=0; i<nruns; ++i)
        totals += bgl_time_edge_traversal<GraphType>();
    report_totals(totals);

    totals = Totals();
    for (size_t i=0; i<nruns; ++i)
        totals += bgl_time_remove_vertex<GraphType>();
    report_totals(totals);

    totals = Totals();
    for (size_t i=0; i<nruns; ++i)
        totals += bgl_time_remove_edge<GraphType>();
    report_totals(totals);

    totals = Totals();
    for (size_t i=0; i<nruns; ++i)
        totals += bgl_time_clear_vertex<GraphType>();
    report_totals(totals);
}

template<class GraphType>
static void
sage_test_all(const std::string &title)
{
    report_head(title);

    Totals totals;
    for (size_t i=0; i<nruns; ++i)
        totals += sage_time_add_vertex<GraphType>();
    report_totals(totals);

    totals = Totals();
    for (size_t i=0; i<nruns; ++i)
        totals += sage_time_add_edge<GraphType>();
    report_totals(totals);

    totals = Totals();
    for (size_t i=0; i<nruns; ++i)
        totals += sage_time_vertex_traversal<GraphType>();
    report_totals(totals);

    totals = Totals();
    for (size_t i=0; i<nruns; ++i)
        totals += sage_time_edge_traversal<GraphType>();
    report_totals(totals);

    totals = Totals();
    for (size_t i=0; i<nruns; ++i)
        totals += sage_time_remove_vertex<GraphType>();
    report_totals(totals);

    totals = Totals();
    for (size_t i=0; i<nruns; ++i)
        totals += sage_time_remove_edge<GraphType>();
    report_totals(totals);

    totals = Totals();
    for (size_t i=0; i<nruns; ++i)
        totals += sage_time_clear_vertex<GraphType>();
    report_totals(totals);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void run_sage() {
    sage_test_all<SgGraph>("Sage: SgGraph");
}

static void run_sawyer() {
    // "int" could be "void" for this test when supported
    typedef Sawyer::Container::Graph<int, int> sgl1;
    sgl_test_all<sgl1>("Sawyer Graph");
}

static void run_sawyer_pool() {
    // "int" could be "void" for this test when supported
    typedef Sawyer::Container::Graph<int, int, Sawyer::UnsynchronizedPoolAllocator> sgl1;
    sgl_test_all<sgl1>("Sawyer Graph using memory pools");
}

static void run_sawyer_bgl() {
    // "int" could be "void" for this test when supported
    typedef Sawyer::Container::Graph<int, int> sgl1;
    bgl_test_all<sgl1>("Sawyer Graph w/BGL interface");
}

static void run_sawyer_bgl_pool() {
    // "int" could be "void" for this test when supported
    typedef Sawyer::Container::Graph<int, int, Sawyer::UnsynchronizedPoolAllocator> sgl1;
    bgl_test_all<sgl1>("Sawyer Graph w/BGL interface using memory pools");
}

static void run_bgl_vec_vec() {
    typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS> bgl1;
    bgl_test_all<bgl1>("BGL directed: V=vec E=vec");
}

static void run_bgl_vec_list() {
    typedef boost::adjacency_list<boost::listS, boost::vecS, boost::directedS> bgl2;
    bgl_test_all<bgl2>("BGL directed: V=vec E=list");
}

static void run_bgl_vec_set() {
    typedef boost::adjacency_list<boost::setS, boost::vecS, boost::directedS> bgl3;
    bgl_test_all<bgl3>("BGL directed: V=vec E=set");
}

static void run_yagi() {
    yagi_test_all<Graph3>("YAGI: Graph3");
}

static void run_yagi_bidir() {
    yagi_test_all<BidirectionalGraph3>("YAGI: BidirectionalGraph3");
}

static void run_yagi_bgl() {
#if 0 /* [Robb P. Matzke 2014-04-23] */
    bgl_test_all<Graph3>("YAGI Graph3 using BGL interface");
#else
    std::cerr <<"yagi-bgl: disabled because Graph3 functions are in the wrong namespace\n";
#endif
}

static void run_yagi_bgl_bidir() {
#if 0 /* [Robb P. Matzke 2014-04-23] */
            bgl_test_all<BidirectionalGraph3>("YAGI BidirectionalGraph3 using BGL interface");
#else
    std::cerr <<"yagi-bgl-bidir: disabled because Graph3 functions are in the wrong namespace\n";
#endif
}

static void run_sage_inc() {
#if 0 // [Robb P. Matzke 2014-04-23]: don't work because tests use SgGraph API rather than subclass APIs
    sage_test_all<SgIncidenceDirectedGraph>("Sage: SgIncidenceDirectedGraph");
#else
    std::cerr <<"sage-inc: disabled because test harness uses SgGraph API rather than API in its subclasses\n";
#endif
}

static void run_sage_inc_bidir() {
#if 0 // [Robb P. Matzke 2014-04-23]: don't work because tests use SgGraph API rather than subclass APIs
    sage_test_all<SgBidirectionalGraph>("Sage: SgBidirectionalGraph");
#else
    std::cerr <<"sage-inc-bidir: disabled because test harness uses SgGraph API rather than API in its subclasses\n";
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef Sawyer::Container::Map<std::string /*name*/, void(*)()/*tester*/> TestDictionary;
static TestDictionary testDictionary;
    
static void showHelpAndExit(const Sawyer::CommandLine::ParserResult &cmdline) {
    cmdline.parser().emitDocumentationToPager();
    exit(0);
}

static void listTestsAndExit(const Sawyer::CommandLine::ParserResult&) {
    BOOST_FOREACH (const std::string &name, testDictionary.keys())
        std::cout <<name <<"\n";
    exit(0);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int
main(int argc, char *argv[])
{
    // Dictionary of possible tests
    testDictionary.insert("sage",               run_sage);
    testDictionary.insert("sawyer",             run_sawyer);
    testDictionary.insert("sawyer-pool",        run_sawyer_pool);
    testDictionary.insert("sawyer-bgl",         run_sawyer_bgl);
    testDictionary.insert("sawyer-bgl-pool",    run_sawyer_bgl_pool);
    testDictionary.insert("bgl-vec-vec",        run_bgl_vec_vec);
    testDictionary.insert("bgl-vec-list",       run_bgl_vec_list);
    testDictionary.insert("bgl-vec-set",        run_bgl_vec_set);
    testDictionary.insert("yagi",               run_yagi);
    testDictionary.insert("yagi-bidir",         run_yagi_bidir);
    testDictionary.insert("yagi-bgl",           run_yagi_bgl);
    testDictionary.insert("yagi-bgl-bidir",     run_yagi_bgl_bidir);
    testDictionary.insert("sage-inc",           run_sage_inc);
    testDictionary.insert("sage-inc-bidir",     run_sage_inc_bidir);

    // Parse command-line
    using Sawyer::CommandLine::Switch;
    Sawyer::CommandLine::SwitchGroup switches;
    switches.insert(Switch("help", 'h')
                    .action(userAction(showHelpAndExit))
                    .doc("Shows this documentation."));
    switches.insert(Switch("iterations", 'N')
                    .argument("n", Sawyer::CommandLine::integerParser(nruns))
                    .doc("Run the tests @v{N} times each."));
    switches.insert(Switch("list", 'L')
                    .action(userAction(listTestsAndExit))
                    .doc("List the available tests and exit."));
    Sawyer::CommandLine::Parser parser;
    parser.with(switches);
    parser.purpose("tests performance of various graph implementations");
    parser.doc("Synopsis", "@prop{programName} [@v{switches}...] @v{test_names}...");
    Sawyer::CommandLine::ParserResult cmdline = parser.parse(argc, argv).apply();

    // Run each specified test
    BOOST_FOREACH (const std::string &testName, cmdline.unreachedArgs()) {
        if (testName == "all") {
            BOOST_FOREACH (void(*func)(), testDictionary.values())
                func();
        } else if (void(*func)() = testDictionary.getOrElse(testName, NULL)) {
            func();
        } else {
            std::cerr <<"unknown graph type: " <<testName <<"\n"
                      <<"use --list to see a list of types\n";
        }
    }

    return 0;
}
