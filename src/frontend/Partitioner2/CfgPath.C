#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include <sage3basic.h>
#include <Partitioner2/CfgPath.h>

#include <Combinatorics.h>
#include <Diagnostics.h>
#include <Partitioner2/Partitioner.h>
#include <Sawyer/GraphTraversal.h>

using namespace Rose::Diagnostics;

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

bool
CfgPath::isConnected() const {
    if (!isEmpty()) {
        ControlFlowGraph::ConstVertexIterator vertex = frontVertex();
        BOOST_FOREACH (const ControlFlowGraph::ConstEdgeIterator &edge, edges_) {
            if (edge->source() != vertex)
                return false;
            vertex = edge->target();
        }
    }
    return true;
}

CfgPath::Vertices
CfgPath::vertices() const {
    Vertices retval;
    if (!isEmpty()) {
        retval.push_back(frontVertex());
        BOOST_FOREACH (const Edges::value_type &edge, edges_)
            retval.push_back(edge->target());
    }
    return retval;
}

void
CfgPath::pushBack(const std::vector<ControlFlowGraph::ConstEdgeIterator> &edges) {
#ifndef NDEBUG
    ASSERT_forbid(edges.empty());
    ControlFlowGraph::ConstVertexIterator srcVertex = edges[0]->source();
    BOOST_FOREACH (ControlFlowGraph::ConstEdgeIterator edge, edges) {
        ASSERT_require(edge->source() == srcVertex);
    }
    ASSERT_require(isEmpty() || srcVertex == backVertex());
#endif

    if (isEmpty()) {
        frontVertex_ = edges[0]->source();
        vertexAttributes_.push_back(Attributes());
    }

    edgeOrders_.push_back(edges);
    std::reverse(edgeOrders_.back().begin(), edgeOrders_.back().end());
    edges_.push_back(edgeOrders_.back().back());
    edgeOrders_.back().pop_back();
    edgeAttributes_.push_back(Attributes());
    vertexAttributes_.push_back(Attributes());

    ASSERT_require(edgeAttributes_.size() == edges_.size());
    ASSERT_require(edgeOrders_.size() == edges_.size());
    ASSERT_require(vertexAttributes_.size() == 1 + edges_.size());
}

void
CfgPath::pushBack(ControlFlowGraph::ConstEdgeIterator edge) {
    ASSERT_require2(!edge.isEmpty(), "cannot be an end iterator");
    std::vector<ControlFlowGraph::ConstEdgeIterator> edges;
    for (/*void*/; !edge.isEmpty(); ++edge)
        edges.push_back(edge);
    pushBack(edges);
}

void
CfgPath::pushFront(const std::vector<ControlFlowGraph::ConstEdgeIterator> &edges) {
#ifndef NDEBUG
    ASSERT_forbid(edges.empty());
    ControlFlowGraph::ConstVertexIterator tgtVertex = edges[0]->target();
    BOOST_FOREACH (ControlFlowGraph::ConstEdgeIterator edge, edges) {
        ASSERT_require(edge->target() == tgtVertex);
    }
    ASSERT_require(isEmpty() || tgtVertex == frontVertex());
#endif

    if (isEmpty())
        vertexAttributes_.push_back(Attributes());
    frontVertex_ = edges[0]->source();
    edgeOrders_.insert(edgeOrders_.begin(), edges);
    std::reverse(edgeOrders_[0].begin(), edgeOrders_[0].end());
    edges_.insert(edges_.begin(), edgeOrders_[0].back());
    edgeOrders_[0].pop_back();
    edgeAttributes_.insert(edgeAttributes_.begin(), Attributes());
    vertexAttributes_.insert(vertexAttributes_.begin(), Attributes());

    ASSERT_require(edgeAttributes_.size() == edges_.size());
    ASSERT_require(edgeOrders_.size() == edges_.size());
    ASSERT_require(vertexAttributes_.size() == 1 + edges_.size());
}

void
CfgPath::pushFront(ControlFlowGraph::ConstEdgeIterator edge) {
    ASSERT_require2(!edge.isEmpty(), "cannot be an end iterator");
    std::vector<ControlFlowGraph::ConstEdgeIterator> edges;
    for (/*void*/; !edge.isEmpty(); ++edge)
        edges.push_back(edge);
    pushFront(edges);
}

void
CfgPath::popBack() {
    ASSERT_forbid(isEmpty());
    ASSERT_forbid(isEmpty());
    if (edges_.empty()) {
        // Erasing the starting vertex; then the path will be empty
        frontVertex_ = Sawyer::Nothing();
        vertexAttributes_.clear();
    } else {
        edges_.pop_back();
        edgeOrders_.pop_back();
        vertexAttributes_.pop_back();
        edgeAttributes_.pop_back();
    }

    ASSERT_require(edgeAttributes_.size() == edges_.size());
    ASSERT_require(vertexAttributes_.size() == 1 + edges_.size());
}

std::vector<ControlFlowGraph::ConstEdgeIterator>
CfgPath::backtrack() {
    ASSERT_require(edgeOrders_.size() == edges_.size());
    ASSERT_require(edgeAttributes_.size() == edges_.size());
    ASSERT_require(vertexAttributes_.size() == 1 + edges_.size());

    std::vector<ControlFlowGraph::ConstEdgeIterator> removedEdges;
    while (!edges_.empty()) {
        ControlFlowGraph::ConstEdgeIterator edgeToRemove = edges_.back();
        removedEdges.push_back(edgeToRemove);

        if (!edgeOrders_.back().empty()) {
            // Another edge available at this position in the path
            edges_.back() = edgeOrders_.back().back();
            edgeOrders_.back().pop_back();
            edgeAttributes_.back() = Attributes();
            vertexAttributes_.back() = Attributes();

            ASSERT_require(edgeOrders_.size() == edges_.size());
            ASSERT_require(edgeAttributes_.size() == edges_.size());
            ASSERT_require(vertexAttributes_.size() == 1 + edges_.size());
            return removedEdges;
        } else {
            // No more sibling edges available, so backtrack
            edges_.pop_back();
            edgeOrders_.pop_back();
            vertexAttributes_.pop_back();
            edgeAttributes_.pop_back();
        }
    }

    clear();
    return removedEdges;
}

CfgPath::Attributes&
CfgPath::vertexAttributes(size_t index) {
    ASSERT_require(vertexAttributes_.size() == 1 + edges_.size());
    ASSERT_require(index < vertexAttributes_.size());
    return vertexAttributes_[index];
}

const CfgPath::Attributes&
CfgPath::vertexAttributes(size_t index) const {
    ASSERT_require(vertexAttributes_.size() == 1 + edges_.size());
    ASSERT_require(index < vertexAttributes_.size());
    return vertexAttributes_[index];
}

CfgPath::Attributes&
CfgPath::edgeAttributes(size_t index) {
    ASSERT_require(edgeAttributes_.size() == edges_.size());
    ASSERT_require(index < edgeAttributes_.size());
    return edgeAttributes_[index];
}

const CfgPath::Attributes&
CfgPath::edgeAttributes(size_t index) const {
    ASSERT_require(edgeAttributes_.size() == edges_.size());
    ASSERT_require(index < edgeAttributes_.size());
    return edgeAttributes_[index];
}

size_t
CfgPath::nVisits(const ControlFlowGraph::ConstVertexIterator &vertex) const {
    size_t retval = 0;
    if (!isEmpty()) {
        if (frontVertex() == vertex)
            ++retval;
        BOOST_FOREACH (const ControlFlowGraph::ConstEdgeIterator &edge, edges_) {
            if (edge->target() == vertex)
                ++retval;
        }
    }
    return retval;
}

size_t
CfgPath::nVisits(const ControlFlowGraph::ConstEdgeIterator &edge) const {
    size_t retval = 0;
    BOOST_FOREACH (const ControlFlowGraph::ConstEdgeIterator &e, edges_) {
        if (e == edge)
            ++retval;
    }
    return retval;
}

size_t
CfgPath::nCalls(const Function::Ptr &function) const {
    size_t retval = 0;
    BOOST_FOREACH (const ControlFlowGraph::ConstEdgeIterator &edge, edges_) {
        if (edge->value().type() == E_FUNCTION_CALL) {
            if (!function) {
                ++retval;
            } else if (edge->target()->value().type() == V_BASIC_BLOCK &&
                       edge->target()->value().isOwningFunction(function)) {
                ++retval;
            }
        }
    }
    return retval;
}

size_t
CfgPath::nReturns(const Function::Ptr &function) const {
    size_t retval = 0;
    BOOST_FOREACH (const ControlFlowGraph::ConstEdgeIterator &edge, edges_) {
        if (edge->value().type() == E_FUNCTION_RETURN) {
            if (!function) {
                ++retval;
            } else if (edge->source()->value().type() == V_BASIC_BLOCK &&
                       edge->source()->value().isOwningFunction(function)) {
                ++retval;
            }
        }
    }
    return retval;
}

ssize_t
CfgPath::callDepth(const Function::Ptr &function) const {
    ssize_t depth = 0;
    BOOST_FOREACH (const ControlFlowGraph::ConstEdgeIterator &edge, edges_) {
        if (edge->value().type() == E_FUNCTION_CALL) {
            if (!function) {
                ++depth;
            } else if (edge->target()->value().type() == V_BASIC_BLOCK &&
                       edge->target()->value().isOwningFunction(function)) {
                ++depth;
            }
        } else if (edge->value().type() == E_FUNCTION_RETURN) {
            if (!function) {
                --depth;
            } else if (edge->source()->value().type() == V_BASIC_BLOCK &&
                       edge->source()->value().isOwningFunction(function)) {
                --depth;
            }
        }
    }
    return depth;
}

size_t
CfgPath::maxCallDepth(const Function::Ptr &function) const {
    ssize_t depth = 0;
    ssize_t retval = 0;
    BOOST_FOREACH (const ControlFlowGraph::ConstEdgeIterator &edge, edges_) {
        if (edge->value().type() == E_FUNCTION_CALL) {
            if (!function) {
                ++depth;
            } else if (edge->target()->value().type() == V_BASIC_BLOCK &&
                       edge->target()->value().isOwningFunction(function)) {
                ++depth;
            }
        } else if (edge->value().type() == E_FUNCTION_RETURN) {
            if (!function) {
                --depth;
            } else if (edge->source()->value().type() == V_BASIC_BLOCK &&
                       edge->source()->value().isOwningFunction(function)) {
                --depth;
            }
        }
        retval = std::max(retval, depth);
    }
    return retval;
}

std::vector<ControlFlowGraph::ConstEdgeIterator>
CfgPath::truncate(const CfgConstEdgeSet &toRemove) {
    for (Edges::iterator ei=edges_.begin(); ei!=edges_.end(); ++ei) {
        if (toRemove.exists(*ei)) {
            // Remove path edges from here to the end
            std::vector<ControlFlowGraph::ConstEdgeIterator> removedEdges(ei, edges_.end());
            std::reverse(removedEdges.begin(), removedEdges.end());
            edges_.erase(ei, edges_.end());

            edgeOrders_.resize(edges_.size());
            edgeAttributes_.resize(edges_.size());
            vertexAttributes_.resize(1 + edges_.size());

            return removedEdges;
        }
    }
    return std::vector<ControlFlowGraph::ConstEdgeIterator>();
}

std::vector<ControlFlowGraph::ConstEdgeIterator>
CfgPath::truncate(const ControlFlowGraph::ConstEdgeIterator &edge) {
    CfgConstEdgeSet toRemove;
    toRemove.insert(edge);
    return truncate(toRemove);
}

std::pair<size_t, size_t>
CfgPath::lastInsnIndex(SgAsmInstruction *needle) const {
    Sawyer::Optional<size_t> lastInsnIdx, lastVertexIdx;
    Vertices verts = vertices();
    for (size_t vertexIdx=0, insnIdx=0; vertexIdx < verts.size(); ++vertexIdx) {
        ControlFlowGraph::ConstVertexIterator vertex = verts[vertexIdx];
        if (vertex->value().type() == V_BASIC_BLOCK) {
            BasicBlock::Ptr bb = vertex->value().bblock();
            BOOST_FOREACH (SgAsmInstruction *insn, bb->instructions()) {
                if (insn->get_address() == needle->get_address()) {
                    lastInsnIdx = insnIdx;
                    lastVertexIdx = vertexIdx;
                }
                ++insnIdx;
            }
        } else {
            ++insnIdx;
        }
    }
    ASSERT_require(lastInsnIdx);
    ASSERT_require(lastVertexIdx);
    return std::make_pair(*lastVertexIdx, *lastInsnIdx);
}

uint64_t
CfgPath::hash() const {
    Combinatorics::HasherSha256Builtin hasher;
    BOOST_FOREACH (const ControlFlowGraph::ConstVertexIterator &vertex, vertices()) {
        hasher.insert((uint64_t)vertex->value().type());
        if (Sawyer::Optional<rose_addr_t> addr = vertex->value().optionalAddress())
            hasher.insert(*addr);
    }
    uint64_t retval = 0;
    BOOST_FOREACH (uint8_t byte, hasher.digest())
        retval = ((retval << 8) | byte) ^ ((retval >> 56) & 0xff);
    return retval;
}

uint64_t
CfgPath::hash(SgAsmInstruction *lastInsn) const {
    std::pair<size_t, size_t> lastIndices = lastInsnIndex(lastInsn);
    size_t lastVertexIdx = lastIndices.first;
    size_t lastInsnIdx = lastIndices.second;
    Vertices verts = vertices();
    ASSERT_require(lastVertexIdx < verts.size());
    Combinatorics::HasherSha256Builtin hasher;
    for (size_t vertIdx = 0, insnIdx = 0; vertIdx <= lastVertexIdx; ++vertIdx) {
        hasher.insert((uint64_t)verts[vertIdx]->value().type());
        if (verts[vertIdx]->value().type() == V_BASIC_BLOCK) {
            BOOST_FOREACH (SgAsmInstruction *insn, verts[vertIdx]->value().bblock()->instructions()) {
                if (insnIdx++ <= lastInsnIdx)
                    hasher.insert(insn->get_address());
            }
        } else if (Sawyer::Optional<rose_addr_t> addr = verts[vertIdx]->value().optionalAddress()) {
            hasher.insert(*addr);
            ++insnIdx;
        } else {
            ++insnIdx;
        }
    }

    uint64_t retval = 0;
    BOOST_FOREACH (uint8_t byte, hasher.digest())
        retval = ((retval << 8) | byte) ^ ((retval >> 56) & 0xff);
    return retval;
}

void
CfgPath::print(std::ostream &out) const {
    if (isEmpty()) {
        out <<"empty";
    } else if (edges_.empty()) {
        out <<"vertex " <<Partitioner::vertexName(*frontVertex());
    } else {
        out <<StringUtility::plural(edges_.size(), "edges") <<":";
        BOOST_FOREACH (const ControlFlowGraph::ConstEdgeIterator &edge, edges_)
            out <<" " <<Partitioner::edgeName(*edge);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Utility functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::ostream&
operator<<(std::ostream &out, const CfgPath &path) {
    path.print(out);
    return out;
}

std::vector<bool>
findPathEdges(const ControlFlowGraph &graph, const CfgConstVertexSet &beginVertices,
              const CfgConstVertexSet &avoidVertices, const CfgConstEdgeSet &avoidEdges,
              bool avoidCallsAndReturns) {
    using namespace Sawyer::Container::Algorithm;

    // Find all edges that are reachable with a forward traversal from any starting vertex, avoiding certain vertices and edges.
    std::vector<bool> retval(graph.nEdges(), false);
    BOOST_FOREACH (const ControlFlowGraph::ConstVertexIterator &beginVertex, beginVertices.values()) {
        ASSERT_require(graph.isValidVertex(beginVertex));
        typedef DepthFirstForwardGraphTraversal<const ControlFlowGraph> ForwardTraversal;
        for (ForwardTraversal t(graph, beginVertex, ENTER_EVENTS); t; ++t) {
            switch (t.event()) {
                case ENTER_VERTEX:
                    if (avoidVertices.exists(t.vertex()))
                        t.skipChildren();
                    break;
                case ENTER_EDGE:
                    if (avoidCallsAndReturns &&
                        (t.edge()->value().type() == E_FUNCTION_CALL || t.edge()->value().type() == E_FUNCTION_RETURN)) {
                        t.skipChildren();
                    } else if (avoidEdges.exists(t.edge())) {
                        t.skipChildren();
                    } else {
                        retval[t.edge()->id()] = true;
                    }
                    break;
                default:
                    break;
            }
        }
    }
    return retval;
}

std::vector<bool>
findPathEdges(const ControlFlowGraph &graph, const CfgConstVertexSet &beginVertices,
              const CfgConstVertexSet &endVertices, const CfgConstVertexSet &avoidVertices,
              const CfgConstEdgeSet &avoidEdges, bool avoidCallsAndReturns) {
    using namespace Sawyer::Container::Algorithm;

    // Mark edges that are reachable with a forward traversal from any starting vertex, avoiding certain vertices and edges.
    std::vector<bool> forwardReachable = findPathEdges(graph, beginVertices, avoidVertices, avoidEdges, avoidCallsAndReturns);

    // Mark edges that are reachable with a backward traversal from any ending vertex, avoiding certain vertices and edges.
    std::vector<bool> significant(graph.nEdges(), false);
    BOOST_FOREACH (const ControlFlowGraph::ConstVertexIterator &endVertex, endVertices.values()) {
        ASSERT_require(graph.isValidVertex(endVertex));
        typedef DepthFirstReverseGraphTraversal<const ControlFlowGraph> ReverseTraversal;
        for (ReverseTraversal t(graph, endVertex, ENTER_EVENTS); t; ++t) {
            switch (t.event()) {
                case ENTER_VERTEX:
                    if (avoidVertices.exists(t.vertex()))
                        t.skipChildren();
                    break;
                case ENTER_EDGE:
                    if (avoidCallsAndReturns &&
                        (t.edge()->value().type() == E_FUNCTION_CALL || t.edge()->value().type() == E_FUNCTION_RETURN)) {
                        t.skipChildren();
                    } else if (avoidEdges.exists(t.edge())) {
                        t.skipChildren();
                    } else if (forwardReachable[t.edge()->id()]) {
                        significant[t.edge()->id()] = true;
                    }
                    break;
                default:
                    break;
            }
        }
    }
    return significant;
}

CfgConstEdgeSet
findPathReachableEdges(const ControlFlowGraph &graph,
                       const CfgConstVertexSet &beginVertices, const CfgConstVertexSet &endVertices,
                       const CfgConstVertexSet &avoidVertices, const CfgConstEdgeSet &avoidEdges,
                       bool avoidCallsAndReturns) {
    CfgConstEdgeSet retval;
    std::vector<bool> goodEdges = findPathEdges(graph, beginVertices, endVertices, avoidVertices, avoidEdges,
                                                avoidCallsAndReturns);
    ASSERT_require(goodEdges.size() == graph.nEdges());
    for (size_t i=0; i<graph.nEdges(); ++i) {
        if (goodEdges[i])
            retval.insert(graph.findEdge(i));
    }
    return retval;
}

CfgConstEdgeSet
findPathUnreachableEdges(const ControlFlowGraph &graph,
                         const CfgConstVertexSet &beginVertices, const CfgConstVertexSet &endVertices,
                         const CfgConstVertexSet &avoidVertices, const CfgConstEdgeSet &avoidEdges,
                         bool avoidCallsAndReturns) {
    CfgConstEdgeSet retval;
    std::vector<bool> goodEdges = findPathEdges(graph, beginVertices, endVertices, avoidVertices, avoidEdges,
                                                avoidCallsAndReturns);
    ASSERT_require(goodEdges.size() == graph.nEdges());
    for (size_t i=0; i<graph.nEdges(); ++i) {
        if (!goodEdges[i])
            retval.insert(graph.findEdge(i));
    }
    return retval;
}

size_t
eraseUnreachablePaths(ControlFlowGraph &graph /*in,out*/, CfgConstVertexSet &beginVertices /*in,out*/,
                      CfgConstVertexSet &endVertices /*in,out*/, CfgVertexMap &vmap /*in,out*/, CfgPath &path /*in,out*/) {
    size_t origPathSize = path.nEdges();
    if (beginVertices.empty() || endVertices.empty()) {
        graph.clear();
        vmap.clear();
        path.clear();
        return origPathSize;
    }

    // Erase unreachable edges from the graph and path
    CfgConstVertexSet avoidVertices;
    CfgConstEdgeSet avoidEdges;
    CfgConstEdgeSet badEdges = findPathUnreachableEdges(graph, beginVertices, endVertices, avoidVertices, avoidEdges, false);
    CfgConstVertexSet incidentVertices = findIncidentVertices(badEdges);
    path.truncate(badEdges);
    eraseEdges(graph, badEdges);

    // This might leave some vertices having no incident edges, so remove them since they can't participate on any path. Avoid
    // scanning the entire graph by considering only vertices that are incident to the edges we just removed. We also need to
    // check the beginVertices and endVertices because they might not have had incident edges, yet we'll wwant to remove them.
    CfgConstVertexSet badVertices;
    BOOST_FOREACH (const ControlFlowGraph::ConstVertexIterator &vertex, incidentVertices.values()) {
        if (vertex->degree() == 0 &&
            (!beginVertices.exists(vertex) || !endVertices.exists(vertex)))
            badVertices.insert(vertex);
    }
    BOOST_FOREACH (const ControlFlowGraph::ConstVertexIterator &vertex, beginVertices.values()) {
        if (vertex->degree() == 0 && !endVertices.exists(vertex))
            badVertices.insert(vertex);
    }
    BOOST_FOREACH (const ControlFlowGraph::ConstVertexIterator &vertex, endVertices.values()) {
        if (vertex->degree() == 0 && !beginVertices.exists(vertex))
            badVertices.insert(vertex);
    }
    BOOST_FOREACH (const ControlFlowGraph::ConstVertexIterator &vertex, badVertices.values()) {
        beginVertices.erase(vertex);
        endVertices.erase(vertex);
        vmap.eraseTarget(vertex);
        graph.eraseVertex(vertex);
    }

    if (beginVertices.empty())
        path.clear();

    return origPathSize - path.nEdges();
}

void
findPaths(const ControlFlowGraph &cfg, ControlFlowGraph &paths /*out*/, CfgVertexMap &vmap /*out*/,
          const CfgConstVertexSet &beginVertices,
          const CfgConstVertexSet &avoidVertices, const CfgConstEdgeSet &avoidEdges,
          bool avoidCallsAndReturns) {
    paths.clear();
    vmap.clear();

    // Find all edges and insert them along with incident vertices.
    std::vector<bool> goodEdges = findPathEdges(cfg, beginVertices, avoidVertices, avoidEdges, avoidCallsAndReturns);
    BOOST_FOREACH (const ControlFlowGraph::Edge &edge, cfg.edges()) {
        if (goodEdges[edge.id()]) {
            if (!vmap.forward().exists(edge.source())) {
                ControlFlowGraph::VertexIterator newPathTarget = paths.insertVertex(edge.source()->value());
                vmap.insert(edge.source(), newPathTarget);
            }
            if (!vmap.forward().exists(edge.target())) {
                ControlFlowGraph::VertexIterator newPathTarget = paths.insertVertex(edge.target()->value());
                vmap.insert(edge.target(), newPathTarget);
            }
            paths.insertEdge(vmap.forward()[edge.source()], vmap.forward()[edge.target()], edge.value());
        }
    }

    // Make sure begin vertices are present. They were probably inserted above except if the vertex has no incident edges, in
    // which case the begin vertex should be inserted only if it can be a singleton path.
    BOOST_FOREACH (const ControlFlowGraph::ConstVertexIterator &beginVertex, beginVertices.values()) {
        if (!vmap.forward().exists(beginVertex) &&                      // not inserted above
            !avoidVertices.exists(beginVertex)) {                       // not an avoided vertex
            ControlFlowGraph::VertexIterator newPathTarget = paths.insertVertex(beginVertex->value());
            vmap.insert(beginVertex, newPathTarget);
        }
    }
}

void
findPaths(const ControlFlowGraph &cfg, ControlFlowGraph &paths /*out*/, CfgVertexMap &vmap /*out*/,
          const CfgConstVertexSet &beginVertices, const CfgConstVertexSet &endVertices,
          const CfgConstVertexSet &avoidVertices, const CfgConstEdgeSet &avoidEdges,
          bool avoidCallsAndReturns) {
    paths.clear();
    vmap.clear();

    // Find all edges and insert them along with incident vertices.
    std::vector<bool> goodEdges = findPathEdges(cfg, beginVertices, endVertices, avoidVertices, avoidEdges, avoidCallsAndReturns);
    BOOST_FOREACH (const ControlFlowGraph::Edge &edge, cfg.edges()) {
        if (goodEdges[edge.id()]) {
            if (!vmap.forward().exists(edge.source())) {
                ControlFlowGraph::VertexIterator newPathTarget = paths.insertVertex(edge.source()->value());
                vmap.insert(edge.source(), newPathTarget);
            }
            if (!vmap.forward().exists(edge.target())) {
                ControlFlowGraph::VertexIterator newPathTarget = paths.insertVertex(edge.target()->value());
                vmap.insert(edge.target(), newPathTarget);
            }
            paths.insertEdge(vmap.forward()[edge.source()], vmap.forward()[edge.target()], edge.value());
        }
    }

    // Make sure begin vertices are present. They were probably inserted above except if the vertex has no incident edges, in
    // which case the begin vertex should be inserted only if it can be a singleton path.
    BOOST_FOREACH (const ControlFlowGraph::ConstVertexIterator &beginVertex, beginVertices.values()) {
        if (!vmap.forward().exists(beginVertex) &&                      // not inserted above
            endVertices.exists(beginVertex) &&                          // can be a singleton path
            !avoidVertices.exists(beginVertex)) {                       // not an avoided vertex
            ControlFlowGraph::VertexIterator newPathTarget = paths.insertVertex(beginVertex->value());
            vmap.insert(beginVertex, newPathTarget);
        }
    }
}

void
findFunctionPaths(const ControlFlowGraph &srcCfg, ControlFlowGraph &paths /*out*/, CfgVertexMap &vmap /*out*/,
                  const CfgConstVertexSet &beginVertices,
                  const CfgConstVertexSet &avoidVertices, const CfgConstEdgeSet &avoidEdges) {
    return findPaths(srcCfg, paths, vmap, beginVertices, avoidVertices, avoidEdges, true);
}

void
findFunctionPaths(const ControlFlowGraph &srcCfg, ControlFlowGraph &paths /*out*/, CfgVertexMap &vmap /*out*/,
                  const CfgConstVertexSet &beginVertices, const CfgConstVertexSet &endVertices,
                  const CfgConstVertexSet &avoidVertices, const CfgConstEdgeSet &avoidEdges) {
    return findPaths(srcCfg, paths, vmap, beginVertices, endVertices, avoidVertices, avoidEdges, true);
}

void
findInterFunctionPaths(const ControlFlowGraph &srcCfg, ControlFlowGraph &paths /*out*/, CfgVertexMap &vmap /*out*/,
                       const CfgConstVertexSet &beginVertices,
                       const CfgConstVertexSet &avoidVertices, const CfgConstEdgeSet &avoidEdges) {
    return findPaths(srcCfg, paths, vmap, beginVertices, avoidVertices, avoidEdges, false);
}

void
findInterFunctionPaths(const ControlFlowGraph &srcCfg, ControlFlowGraph &paths /*out*/, CfgVertexMap &vmap /*out*/,
                       const CfgConstVertexSet &beginVertices, const CfgConstVertexSet &endVertices,
                       const CfgConstVertexSet &avoidVertices, const CfgConstEdgeSet &avoidEdges) {
    return findPaths(srcCfg, paths, vmap, beginVertices, endVertices, avoidVertices, avoidEdges, false);
}

bool
inlineMultipleCallees(ControlFlowGraph &paths /*in,out*/, const ControlFlowGraph::ConstVertexIterator &pathsCallSite,
                      const ControlFlowGraph &cfg, const ControlFlowGraph::ConstVertexIterator &cfgCallSite,
                      const CfgConstVertexSet &cfgAvoidVertices, const CfgConstEdgeSet &cfgAvoidEdges,
                      std::vector<ControlFlowGraph::ConstVertexIterator> *newVertices /*=NULL*/) {
    bool somethingInserted = false;
    CfgConstEdgeSet cfgCallEdges = findCallEdges(cfgCallSite);
    BOOST_FOREACH (const ControlFlowGraph::ConstEdgeIterator &cfgCallEdge, cfgCallEdges.values()) {
        if (inlineOneCallee(paths /*in,out*/, pathsCallSite, cfg, cfgCallEdge->target(), cfgAvoidVertices,
                            cfgAvoidEdges, newVertices))
            somethingInserted = true;
    }
    return somethingInserted;
}

bool
inlineOneCallee(ControlFlowGraph &paths /*in,out*/, const ControlFlowGraph::ConstVertexIterator &pathsCallSite,
                const ControlFlowGraph &cfg, const ControlFlowGraph::ConstVertexIterator &cfgCallTarget,
                const CfgConstVertexSet &cfgAvoidVertices, const CfgConstEdgeSet &cfgAvoidEdges,
                std::vector<ControlFlowGraph::ConstVertexIterator> *newVertices /*=NULL*/) {
    ASSERT_require(paths.isValidVertex(pathsCallSite));
    ASSERT_require(cfg.isValidVertex(cfgCallTarget));
    ASSERT_require2(pathsCallSite->value().type() == V_BASIC_BLOCK, "only basic blocks can call functions");

    // Most functions either don't return or have a single return target.  Functions like longjmp might have multiple return
    // targets. We need to know the return targets in the paths graph because we will create new edges from the inlined
    // function's return sites to each return target.
    CfgConstVertexSet pathsReturnTargets;
    BOOST_FOREACH (const ControlFlowGraph::Edge &edge, pathsCallSite->outEdges()) {
        if (edge.value().type() == E_CALL_RETURN)
            pathsReturnTargets.insert(edge.target());
    }

    // If this is a call to some indeterminate function, just copy another indeterminate vertex into the paths graph. Normally
    // a CFG will have only one indeterminate vertex and it will have no outgoing edges, but the paths graph is different. We
    // need separate indeterminate vertices so that each has its own function-return edge(s).
    if (cfgCallTarget->value().type() == V_INDETERMINATE) {
        ControlFlowGraph::ConstVertexIterator indet = paths.insertVertex(CfgVertex(V_INDETERMINATE));
        if (newVertices)
            newVertices->push_back(indet);
        paths.insertEdge(pathsCallSite, indet, CfgEdge(E_FUNCTION_CALL));
        BOOST_FOREACH (const ControlFlowGraph::ConstVertexIterator &returnTarget, pathsReturnTargets.values())
            paths.insertEdge(indet, returnTarget, CfgEdge(E_FUNCTION_RETURN));
        SAWYER_MESG(mlog[DEBUG]) <<"insertCalleePaths: indeterminate function call from "
                                 <<pathsCallSite->value().bblock()->printableName() <<"\n";
        return true;
    }

    // Call to a normal function.
    ASSERT_require2(cfgCallTarget->value().type() == V_BASIC_BLOCK, "non-basic block callees not implemented yet");

    // Find all paths through the callee that return and avoid certain vertices and edges.
    CfgVertexMap vmap1;                                 // relates CFG to calleePaths
    CfgConstVertexSet cfgCallTargets; cfgCallTargets.insert(cfgCallTarget);
    CfgConstVertexSet cfgReturns = findFunctionReturns(cfg, cfgCallTarget);
    ControlFlowGraph calleePaths;
    findFunctionPaths(cfg, calleePaths, vmap1, cfgCallTargets, cfgReturns, cfgAvoidVertices, cfgAvoidEdges);
    if (calleePaths.isEmpty()) {
        if (mlog[DEBUG]) {
            std::string calleePrintableName;
            if (cfgCallTarget->value().nOwningFunctions() > 1) {
                BOOST_FOREACH (const Function::Ptr &function, cfgCallTarget->value().owningFunctions().values()) {
                    if (!function->name().empty()) {
                        calleePrintableName = function->printableName();
                        break;
                    }
                }
            } else if (1 == cfgCallTarget->value().nOwningFunctions()) {
                calleePrintableName = (*cfgCallTarget->value().owningFunctions().values().begin())->printableName();
            } else {
                calleePrintableName = cfgCallTarget->value().bblock()->printableName();
            }
            mlog[DEBUG] <<"insertCalleePaths: " <<calleePrintableName <<" has no paths to insert\n";
        }
        return false;
    }

    // Insert the callee into the paths CFG
    CfgVertexMap vmap2;                            // relates calleePaths to paths
    insertCfg(paths, calleePaths, vmap2);
    if (newVertices) {
        BOOST_FOREACH (ControlFlowGraph::ConstVertexIterator vertex, vmap2.forward().values()) {
            ASSERT_require(paths.isValidVertex(vertex));
            newVertices->push_back(vertex);
        }
    }
    CfgVertexMap vmap(vmap1, vmap2);                    // composite map from the CFG to paths graph

    // Make an edge from call site to the entry block of the callee in the paths graph
    if (vmap.forward().exists(cfgCallTarget)) {
        ControlFlowGraph::ConstVertexIterator pathStart = vmap.forward()[cfgCallTarget];
        paths.insertEdge(pathsCallSite, pathStart, CfgEdge(E_FUNCTION_CALL));
    }

    // Make edges from the callee's return statements back to the return targets in the caller
    BOOST_FOREACH (const ControlFlowGraph::ConstVertexIterator &cfgReturnSite, cfgReturns.values()) {
        if (vmap.forward().exists(cfgReturnSite)) {
            ControlFlowGraph::ConstVertexIterator pathsReturnSite = vmap.forward()[cfgReturnSite];
            BOOST_FOREACH (const ControlFlowGraph::ConstVertexIterator &pathsReturnTarget, pathsReturnTargets.values())
                paths.insertEdge(pathsReturnSite, pathsReturnTarget, CfgEdge(E_FUNCTION_RETURN));
        }
    }
    return true;
}

Inliner::HowInline
Inliner::ShouldInline::operator()(const Partitioner &partitioner, const ControlFlowGraph::ConstEdgeIterator cfgCallEdge,
                                  const ControlFlowGraph &paths, const ControlFlowGraph::ConstVertexIterator &pathsCallSite,
                                  size_t callDepth) {
    ASSERT_require(partitioner.cfg().isValidEdge(cfgCallEdge));
    ASSERT_require(paths.isValidVertex(pathsCallSite));
    return callDepth <= maxCallDepth_ ? INLINE_NORMAL : INLINE_NONE;
}

// class method
ControlFlowGraph::ConstVertexIterator
Inliner::pathToCfg(const Partitioner &partitioner, const ControlFlowGraph::ConstVertexIterator &pathVertex) {
    switch (pathVertex->value().type()) {
        case V_BASIC_BLOCK:
            return partitioner.findPlaceholder(pathVertex->value().address());
        case V_INDETERMINATE:
            return partitioner.indeterminateVertex();
        case V_NONEXISTING:
            return partitioner.nonexistingVertex();
        case V_UNDISCOVERED:
            return partitioner.undiscoveredVertex();
        case V_USER_DEFINED:
            return partitioner.cfg().vertices().end();
    }
    ASSERT_not_reachable("invalid vertex type");
}

// class method
bool
Inliner::isFunctionCall(const Partitioner &partitioner, const ControlFlowGraph::ConstVertexIterator &pathVertex) {
    ControlFlowGraph::ConstVertexIterator cfgVertex = pathToCfg(partitioner, pathVertex);
    ASSERT_require(partitioner.cfg().isValidVertex(cfgVertex));
    BOOST_FOREACH (ControlFlowGraph::Edge edge, cfgVertex->outEdges()) {
        if (edge.value().type() == E_FUNCTION_CALL)
            return true;
    }
    return false;
}

void
Inliner::reset(const Partitioner &partitioner, const CfgConstVertexSet &cfgBeginVertices,
              const CfgConstVertexSet &cfgEndVertices, const CfgConstVertexSet &cfgAvoidVertices,
              const CfgConstEdgeSet &cfgAvoidEdges) {

    paths_.clear();
    vmap_.clear();
    workList_.clear();
    if (!shouldInline_)
        shouldInline_ = ShouldInline::instance();

    // Find top-level paths. These paths don't traverse into function calls unless they must do so in order to reach an ending
    // vertex.
    findInterFunctionPaths(partitioner.cfg(), paths_/*out*/, vmap_/*out*/,
                           cfgBeginVertices, cfgEndVertices, cfgAvoidVertices, cfgAvoidEdges);
    if (paths_.isEmpty()) {
        pathsBeginVertices_.clear();
        pathsEndVertices_.clear();
        return;
    }
    pathsBeginVertices_ = forwardMapped(cfgBeginVertices, vmap_);
    pathsEndVertices_ = forwardMapped(cfgEndVertices, vmap_);

    // Set up the initial worklist
    for (ControlFlowGraph::ConstVertexIterator vert=paths_.vertices().begin(); vert!=paths_.vertices().end(); ++vert) {
        if (isFunctionCall(partitioner, vert) && !findCallReturnEdges(vert).empty())
            workList_.push_back(CallSite(vert, 0));
    }
}

void
Inliner::inlinePaths(const Partitioner &partitioner, const CfgConstVertexSet &cfgBeginVertices,
                     const CfgConstVertexSet &cfgEndVertices, const CfgConstVertexSet &cfgAvoidVertices,
                     const CfgConstEdgeSet &cfgAvoidEdges) {

    reset(partitioner, cfgBeginVertices, cfgEndVertices, cfgAvoidVertices, cfgAvoidEdges);

    // When finding paths through a called function, avoid the usual vertices and edges, but also avoid those vertices that
    // mark the end of paths. We want paths that go all the way from the entry block of the called function to its returning
    // blocks.
    CfgConstVertexSet calleeCfgAvoidVertices = cfgAvoidVertices;
    calleeCfgAvoidVertices.insert(cfgEndVertices);

    while (!workList_.empty()) {
        CallSite work = workList_.back();
        workList_.pop_back();
        ControlFlowGraph::ConstVertexIterator cfgVertex = pathToCfg(partitioner, work.pathsVertex);
        CfgConstEdgeSet callEdges = findCallEdges(cfgVertex);
        BOOST_FOREACH (const ControlFlowGraph::ConstEdgeIterator &cfgCallEdge, callEdges.values()) {
            HowInline how = (*shouldInline_)(partitioner, cfgCallEdge, paths_, work.pathsVertex, work.callDepth);
            switch (how) {
                case INLINE_NONE:
                    break;
                case INLINE_NORMAL: {
                    std::vector<ControlFlowGraph::ConstVertexIterator> insertedVertices;
                    inlineOneCallee(paths_, work.pathsVertex, partitioner.cfg(), cfgCallEdge->target(),
                                    calleeCfgAvoidVertices, cfgAvoidEdges, &insertedVertices);
                    BOOST_FOREACH (const ControlFlowGraph::ConstVertexIterator &vertex, insertedVertices) {
                        if (isFunctionCall(partitioner, vertex) && !findCallReturnEdges(vertex).empty())
                            workList_.push_back(CallSite(vertex, work.callDepth+1));
                    }
                    break;
                }
                case INLINE_USER: {
                    CfgVertex userdef(V_USER_DEFINED);
                    ControlFlowGraph::ConstVertexIterator callee = cfgCallEdge->target();
                    if (callee->value().type() == V_BASIC_BLOCK)
                        userdef.address(callee->value().address());
                    ControlFlowGraph::VertexIterator userVertex = paths_.insertVertex(userdef);
                    paths_.insertEdge(work.pathsVertex, userVertex);
                    CfgConstEdgeSet callReturnEdges = findCallReturnEdges(work.pathsVertex);
                    BOOST_FOREACH (const ControlFlowGraph::ConstEdgeIterator &cre, callReturnEdges.values())
                        paths_.insertEdge(userVertex, cre->target(), E_FUNCTION_RETURN);
                    break;
                }
            }
        }
        eraseEdges(paths_, findCallReturnEdges(work.pathsVertex));
    }

    // Remove edges and vertices that cannot be part of any path
    CfgConstEdgeSet pathsUnreachableEdges = findPathUnreachableEdges(paths_, pathsBeginVertices_, pathsEndVertices_);
    CfgConstVertexSet pathsIncidentVertices = findIncidentVertices(pathsUnreachableEdges);
    CfgConstVertexSet pathsBadVertices;
    eraseEdges(paths_, pathsUnreachableEdges);
    BOOST_FOREACH (const ControlFlowGraph::ConstVertexIterator &pathVertex, pathsIncidentVertices.values()) {
        if (pathVertex->degree() == 0 && (!pathsBeginVertices_.exists(pathVertex) || !pathsEndVertices_.exists(pathVertex)))
            pathsBadVertices.insert(pathVertex);
    }
    BOOST_FOREACH (const ControlFlowGraph::ConstVertexIterator &pathVertex, pathsBeginVertices_.values()) {
        if (pathVertex->degree() == 0 && !pathsEndVertices_.exists(pathVertex))
            pathsBadVertices.insert(pathVertex);
    }
    BOOST_FOREACH (const ControlFlowGraph::ConstVertexIterator &pathVertex, pathsEndVertices_.values()) {
        if (pathVertex->degree() == 0 && !pathsBeginVertices_.exists(pathVertex))
            pathsBadVertices.insert(pathVertex);
    }
    BOOST_FOREACH (const ControlFlowGraph::ConstVertexIterator &pathVertex, pathsBadVertices.values()) {
        pathsBeginVertices_.erase(pathVertex);
        pathsEndVertices_.erase(pathVertex);
        vmap_.eraseTarget(pathVertex);
        paths_.eraseVertex(pathVertex);
    }
}

} // namespace
} // namespace
} // namespace

#endif
