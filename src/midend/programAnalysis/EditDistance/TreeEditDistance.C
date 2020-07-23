#include "sage3basic.h"
#include "Diagnostics.h"
#include <EditDistance/TreeEditDistance.h>

#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <sstream>
#include <string>
#include <vector>

namespace Rose {
namespace EditDistance {
namespace TreeEditDistance {

using namespace Diagnostics;

std::ostream&
operator<<(std::ostream &out, const Edit &x) {
    x.print(out);
    return out;
}

// A pre-order traversal that looks only at nodes that belong to the specified containingFile (if any) and whose depth in the
// sub-AST are between minDepth and maxDepth, inclusive.  Each such node is appended to the nodesTraced vector and its depth is
// appended to the nodeDepths vector.
struct SequenceGenerationTraversal: SgTopDownBottomUpProcessing<size_t, Sawyer::Nothing> {
    std::vector<SgNode*> &nodesTraced;                  // AST nodes in the order they were visited
    std::vector<size_t> &nodeDepths;                    // depth of nodes within subtree in the order they were visited
    SgFile *containingFile;                             // optional constraint: node must belong to this file
    size_t minDepth, maxDepth;                          // constraints: limits nodes by their depth under the subtree

    SequenceGenerationTraversal(std::vector<SgNode*> &nodesTraced /*out*/, std::vector<size_t> &nodeDepths /*out*/,
                                SgFile *containingFile, size_t minDepth, size_t maxDepth)
        : nodesTraced(nodesTraced), nodeDepths(nodeDepths),
          containingFile(containingFile), minDepth(minDepth), maxDepth(maxDepth) {
        ASSERT_require(minDepth <= maxDepth);
        ASSERT_require(nodesTraced.size() == nodeDepths.size());
    }

    size_t evaluateInheritedAttribute(SgNode *node, size_t depth) ROSE_OVERRIDE {
        ASSERT_always_forbid(isSgProject(node));
        Sg_File_Info *nodeInfo = node->get_file_info();
        bool trace = (depth >= minDepth && depth <= maxDepth &&
                      (!containingFile || (nodeInfo && nodeInfo->isSameFile(containingFile))));
        if (trace) {
            nodesTraced.push_back(node);
            nodeDepths.push_back(depth);
        } else if (depth > maxDepth) {
            // too bad ROSE doesn't have a way to short circuit when we've reached the maximum depth!
        }
        return depth+1;
    }

    Sawyer::Nothing evaluateSynthesizedAttribute(SgNode*, size_t depth, SubTreeSynthesizedAttributes) ROSE_OVERRIDE {
        return Sawyer::Nothing();
    }
};

// Generates a pre-order list of selected vertices and their depths in the subtree.
static std::vector<SgNode*>
generateTraversalList(SgNode *ast, std::vector<size_t> &nodeDepths /*out*/,
                      SgFile *containingFile=NULL, size_t minDepth=0, size_t maxDepth=size_t(-1)) {
    nodeDepths.clear();
    std::vector<SgNode*> nodesTraced;
    SequenceGenerationTraversal traversal(nodesTraced /*out*/, nodeDepths /*out*/, containingFile, minDepth, maxDepth);
    traversal.traverse(ast, 0);
    return traversal.nodesTraced;
}

// Convertes 2d row-major coordinates to 1d index.
class Coord2d {
    size_t nRows_, nCols_;
public:
    Coord2d(size_t nRows, size_t nCols): nRows_(nRows), nCols_(nCols) {}
    std::pair<size_t, size_t> size2d() const { return std::make_pair(nRows_, nCols_); }
    size_t size() const { return nRows_ * nCols_; }
    size_t index(size_t row, size_t col) const { return row*nCols_ + col; }
    std::pair<size_t, size_t> index2d(size_t idx) const { return std::make_pair(idx/nCols_, idx%nCols_); }
};

Analysis&
Analysis::setTree1(SgNode *ast, SgFile *file/*=NULL*/) {
    ASSERT_not_null(ast);
    ast1_ = ast;
    nodes1_ = generateTraversalList(ast, depths1_/*out*/, file);
    nodes1_.insert(nodes1_.begin(), NULL);
    depths1_.insert(depths1_.begin(), 0);
    return *this;
}

Analysis&
Analysis::setTree2(SgNode *ast, SgFile *file/*=NULL*/) {
    ASSERT_not_null(ast);
    ast2_ = ast;
    nodes2_ = generateTraversalList(ast, depths2_/*out*/, file);
    nodes2_.insert(nodes2_.begin(), NULL);
    depths2_.insert(depths2_.begin(), 0);
    return *this;
}

// Compute results and store them in this object for subsequent queries
Analysis&
Analysis::compute(SgNode *sourceAst, SgNode *targetAst, SgFile *sourceFile/*=NULL*/, SgFile *targetFile/*=NULL*/) {
    setTree1(sourceAst, sourceFile);
    setTree2(targetAst, targetFile);
    return compute();
}

Analysis&
Analysis::compute(SgNode *targetAst, SgFile *targetFile/*=NULL*/) {
    setTree2(targetAst, targetFile);
    return compute();
}

Analysis&
Analysis::compute() {
    ASSERT_forbid(nodes1_.empty());
    ASSERT_forbid(nodes2_.empty());

    // The two ordered sets of AST nodes (nodes1_ and nodes2_) were augmented by prepending nil into both so that their sizes
    // are n1+1 and n2+1 respectively.  The vertices of the graph over which Djikstra's shortest path is computed is the
    // Cartesian product {nil, nodes1} X {nil, nodes2} giving (n1+1)(n2+1) graph vertices in total.
    size_t n1 = nodes1_.size()-1;                       // n1 and n2 are the number of nodes; sizes before we
    size_t n2 = nodes2_.size()-1;                       // prepend nil to these sets (see below).
    Coord2d matrix(n1+1, n2+1);                         // no data, but convenient for converting between 1d and 2d
    graph_ = Graph(matrix.size());                      // graph with initial vertices

    // Boundary edges: downward edges along the right side of the matrix represent deletion, and right-facing edges along the
    // bottom side of the matrix represent insertion.
    for (size_t i=0; i<n1; ++i)
        boost::add_edge(matrix.index(i, n2), matrix.index(i+1, n2), deletionCost_, graph_);
    for (size_t j=0; j<n2; ++j)
        boost::add_edge(matrix.index(n1, j), matrix.index(n1, j+1), insertionCost_, graph_);

    // Internal edges: down edges represent deletion, right edges represent insertion, and diagonal edges represent substitution.
    for (size_t i=0; i<n1; ++i) {
        for (size_t j=0; j<n2; ++j) {
            if (depths1_[i+1] >= depths2_[j+1])
                boost::add_edge(matrix.index(i, j), matrix.index(i+1, j), deletionCost_, graph_);
            if (depths1_[i+1] <= depths2_[j+1])
                boost::add_edge(matrix.index(i, j), matrix.index(i, j+1), insertionCost_, graph_);
            if (depths1_[i+1] == depths2_[j+1] &&
                (!substitutionPredicate_ || (*substitutionPredicate_)(nodes1_[i+1], nodes2_[j+1])))
                boost::add_edge(matrix.index(i, j), matrix.index(i+1, j+1), substitutionCost_, graph_);
        }
    }

    totalCost_ = std::vector<double>(matrix.size(), 0.0);
    predecessors_ = std::vector<Vertex>(matrix.size(), boost::graph_traits<Graph>::null_vertex());
    boost::dijkstra_shortest_paths(graph_, 0, boost::predecessor_map(&predecessors_[0]).distance_map(&totalCost_[0]));
    return *this;
}

// Emit the graph to a GraphViz file
void
Analysis::emitGraphViz(std::ostream &out) const {
    out <<"digraph \"Edge Graph\" {\n";
    Coord2d matrix(nodes1_.size(), nodes2_.size());

    // Vertices
    {
        boost::graph_traits<Graph>::vertex_iterator vi, vi_end;
        for (boost::tie(vi, vi_end)=boost::vertices(graph_); vi!=vi_end; ++vi) {
            size_t i, j;
            boost::tie(i, j) = matrix.index2d(*vi);
            out <<*vi <<" [ label=\"(" <<i <<"," <<j <<")=" <<*vi;
            if (i>0)
                out <<"\\n" <<nodes1_[i]->class_name();
            if (j>0)
                out <<"\\n" <<nodes2_[j]->class_name();
            out <<"\" ];\n";
        }
    }

    // GraphViz doesn't support a strict matrix layout where each row is a list of columns in the same order for each row
    // unless we choose coordinates ourselves for each vertex.  We don't want to do that work, so we'll add invisible edges
    // to try to convince dot to do something as close as possible to what we want.
    for (size_t i=0; i<nodes1_.size(); ++i) {
        for (size_t j=0; j<nodes2_.size(); ++j) {
            if (i+1<nodes1_.size())
                out <<matrix.index(i, j) <<" -> " <<matrix.index(i+1, j) <<" [ dir=none style=invis ];\n";
            if (j+1<nodes2_.size())
                out <<matrix.index(i, j) <<" -> " <<matrix.index(i, j+1) <<" [ dir=none style=invis constraint=false ];\n";
        }
    }

    // Edges representing possible edit actions
    {
        boost::graph_traits<Graph>::edge_iterator ei, ei_end;
        for (boost::tie(ei, ei_end)=boost::edges(graph_); ei!=ei_end; ++ei) { // copied from projects // blame quinlan
            Vertex vs = boost::source(*ei, graph_);
            Vertex vt = boost::target(*ei, graph_);
            size_t si, sj, ti, tj;                      // source and target row and column numbers
            boost::tie(si, sj) = matrix.index2d(vs);
            boost::tie(ti, tj) = matrix.index2d(vt);
            out <<vs <<" -> " <<vt <<" [ dir=none color=";
            if (si+1==ti && sj+1==tj) {
                out <<"blue";                           // diagonal edge: substitution
            } else if (si+1==ti && sj==tj) {
                out <<"red";                            // down edge: deletion
            } else if (si==ti && sj+1==tj) {
                out <<"green";                          // right edge: insertion
            } else {
                ASSERT_not_reachable("invalid graph edge");
            }
            out <<" style=bold constraint=false ];\n";
        }
    }

    // Edges representing actual edit actions
    {
        Vertex current = matrix.size()-1;
        while (current != 0) {
            Vertex predecessor = predecessors_[current];
            out <<current <<" -> " <<predecessor <<" [ color=black style=bold constraint=false ];\n";
            current = predecessor;
        }
    }

    out <<"}\n";
}

double
Analysis::cost() const {
    ASSERT_forbid(totalCost_.empty());
    Coord2d matrix(nodes1_.size(), nodes2_.size());
    return totalCost_[matrix.size()-1];
}

double
Analysis::relativeCost() const {
    return cost() / std::max(nodes1_.size(), nodes2_.size());
}

Edits
Analysis::edits() const {
    Edits edits;
    if (totalCost_.empty())
        return edits;
    Stream debug(mlog[DEBUG]);
    debug <<"TreeEditDistance::edits() called: "
          <<" source=(" <<ast1_->class_name() <<"*)" <<ast1_ <<", "
          <<" target=(" <<ast2_->class_name() <<"*)" <<ast2_ <<"\n"
          <<"  individual edits (in reverse order):\n";
    Coord2d matrix(nodes1_.size(), nodes2_.size());
    size_t vertex = matrix.size()-1;
    while (vertex!=0) {
        size_t i, j, pi, pj;
        boost::tie(i, j) = matrix.index2d(vertex);
        size_t predecessor = predecessors_[vertex];
        boost::tie(pi, pj) = matrix.index2d(predecessor);
        if (pi+1 == i && pj+1 == j) {                   // diagonal edge representing substitution
            edits.push_back(Edit(SUBSTITUTE, nodes1_[i], nodes2_[j], substitutionCost_));
            debug <<"    subst";
        } else if (pi==i && pj+1==j) {                  // right-facing edge representing insertion
            edits.push_back(Edit(INSERT, NULL, nodes2_[j], insertionCost_));
            debug <<"    insert";
        } else if (pi+1==i && pj==j) {                  // downward edge representing deletion
            edits.push_back(Edit(DELETE, nodes1_[i], NULL, deletionCost_));
            debug <<"    delete";
        } else {
            ASSERT_not_reachable("not a properly formed path");
        }
        debug <<" from vertex (" <<pi <<"," <<pj <<")=" <<predecessor <<" to vertex (" <<i <<"," <<j <<")=" <<vertex <<"\n";
        vertex = predecessor;
    }
    std::reverse(edits.begin(), edits.end());
    debug <<"  TreeEditDistance::edits() finished; returning " <<StringUtility::plural(edits.size(), "edits") <<"\n";
    return edits;
}

std::pair<size_t, size_t>
Analysis::graphSize() const {
    return std::make_pair(boost::num_vertices(graph_), boost::num_edges(graph_));
}

void
Edit::print(std::ostream &out) const {
    switch (editType) {
        case INSERT:
            out <<"insert (" <<targetNode->class_name() <<"*)" <<targetNode;
            break;
        case DELETE:
            out <<"delete (" <<sourceNode->class_name() <<"*)" <<sourceNode;
            break;
        case SUBSTITUTE:
            out <<"subst  (" <<sourceNode->class_name() <<"*)" <<sourceNode
                <<" -> (" <<targetNode->class_name() <<"*)" <<targetNode;
            break;
    }
    out <<" cost=" <<cost;
}
    
} // namespace
} // namespace
} // namespace
