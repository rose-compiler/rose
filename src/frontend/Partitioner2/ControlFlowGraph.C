#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"

#include <Partitioner2/ControlFlowGraph.h>
#include <Partitioner2/Partitioner.h>
#include <Sawyer/GraphTraversal.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CfgVertex
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

AddressIntervalSet
CfgVertex::addresses() const {
    AddressIntervalSet retval;
    switch (type()) {
        case V_BASIC_BLOCK:
            retval.insert(address());
            BOOST_FOREACH (SgAsmInstruction *insn, bblock()->instructions())
                retval.insert(AddressInterval::baseSize(insn->get_address(), insn->get_size()));
            break;
        case V_USER_DEFINED:
            retval.insert(address());
            break;
        case V_INDETERMINATE:
        case V_UNDISCOVERED:
        case V_NONEXISTING:
            break;
    }
    return retval;
}

Sawyer::Optional<rose_addr_t>
CfgVertex::optionalAddress() const {
    switch (type()) {
        case V_BASIC_BLOCK:
        case V_USER_DEFINED:
        case V_NONEXISTING:
            return address();
        default:
            return Sawyer::Nothing();
    }
}

Sawyer::Optional<rose_addr_t>
CfgVertex::optionalLastAddress() const {
    switch (type()) {
        case V_BASIC_BLOCK:
            if (bblock()->nInstructions() > 1) {
                return bblock()->instructions().back()->get_address();
            } else {
                return address();
            }
        case V_USER_DEFINED:
        case V_NONEXISTING:
            return address();
        default:
            return Sawyer::Nothing();
    }
}

Function::Ptr
CfgVertex::isEntryBlock() const {
    Function::Ptr retval;
    switch (type()) {
        case V_BASIC_BLOCK:
        case V_USER_DEFINED:
        case V_NONEXISTING:
            BOOST_FOREACH (const Function::Ptr &function, owningFunctions_.values()) {
                if (function->address() == address()) {
                    retval = function;
                    break;
                }
            }
            break;
        case V_INDETERMINATE:
        case V_UNDISCOVERED:
            break;
    }
    return retval;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Free functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
insertCfg(ControlFlowGraph &dst, const ControlFlowGraph &src, CfgVertexMap &vmap /*out*/) {
    BOOST_FOREACH (ControlFlowGraph::Vertex vertex, src.vertices())
        vmap.insert(src.findVertex(vertex.id()), dst.insertVertex(vertex.value()));
    BOOST_FOREACH (ControlFlowGraph::Edge edge, src.edges())
        dst.insertEdge(vmap.forward()[edge.source()], vmap.forward()[edge.target()], edge.value());
}

CfgConstEdgeSet
findBackEdges(const ControlFlowGraph &cfg, const ControlFlowGraph::ConstVertexIterator &begin) {
    using namespace Sawyer::Container::Algorithm;
    typedef DepthFirstForwardGraphTraversal<const ControlFlowGraph> Traversal;

    CfgConstEdgeSet backEdges;
    std::vector<bool> visitingVertex(cfg.nVertices(), false);
    for (Traversal t(cfg, begin, ENTER_VERTEX|LEAVE_VERTEX|ENTER_EDGE); t; ++t) {
        if (t.event() == ENTER_VERTEX) {
            visitingVertex[t.vertex()->id()] = true;
        } else if (t.event() == LEAVE_VERTEX) {
            visitingVertex[t.vertex()->id()] = false;
        } else if (t.event() == ENTER_EDGE) {
            if (visitingVertex[t.edge()->target()->id()])
                backEdges.insert(t.edge());
        }
    }
    return backEdges;
}

CfgConstEdgeSet
findCallEdges(const ControlFlowGraph::ConstVertexIterator &callSite) {
    CfgConstEdgeSet retval;
    for (ControlFlowGraph::ConstEdgeIterator ei=callSite->outEdges().begin(); ei!=callSite->outEdges().end(); ++ei) {
        if (ei->value().type() == E_FUNCTION_CALL)
            retval.insert(ei);
    }
    return retval;
}

CfgConstVertexSet
findCalledFunctions(const ControlFlowGraph &cfg, const ControlFlowGraph::ConstVertexIterator &callSite) {
    ASSERT_require2(cfg.isValidVertex(callSite), "callSite vertex must belong to the CFG");
    CfgConstVertexSet retval;
    BOOST_FOREACH (const ControlFlowGraph::Edge &edge, callSite->outEdges()) {
        if (edge.value().type() == E_FUNCTION_CALL)
            retval.insert(cfg.findVertex(edge.target()->id()));
    }
    return retval;
}

CfgConstEdgeSet
findCallReturnEdges(const Partitioner &partitioner, const ControlFlowGraph &cfg) {
    CfgConstEdgeSet retval;
    for (ControlFlowGraph::ConstEdgeIterator edge = cfg.edges().begin(); edge != cfg.edges().end(); ++edge) {
        if (edge->value().type() == E_CALL_RETURN)
            retval.insert(edge);
    }
    return retval;
}

CfgConstEdgeSet
findCallReturnEdges(const ControlFlowGraph::ConstVertexIterator &callSite) {
    CfgConstEdgeSet retval;
    for (ControlFlowGraph::ConstEdgeIterator ei=callSite->outEdges().begin(); ei!=callSite->outEdges().end(); ++ei) {
        if (ei->value().type() == E_CALL_RETURN)
            retval.insert(ei);
    }
    return retval;
}

CfgConstVertexSet
findFunctionReturns(const ControlFlowGraph &cfg, const ControlFlowGraph::ConstVertexIterator &beginVertex) {
    ASSERT_require2(cfg.isValidVertex(beginVertex), "beginVertex must belong to the CFG");
    CfgConstVertexSet endVertices;
    typedef Sawyer::Container::Algorithm::DepthFirstForwardEdgeTraversal<const ControlFlowGraph> Traversal;
    for (Traversal t(cfg, beginVertex); t; ++t) {
        if (t->value().type() == E_FUNCTION_RETURN) {
            endVertices.insert(t->source());
            t.skipChildren();                           // found a function return edge
        } else if (t->value().type() == E_FUNCTION_CALL) { // not E_FUNCTION_XFER
            t.skipChildren();                           // stay in this function
        }
    }
    return endVertices;
}

void
eraseEdges(ControlFlowGraph &graph, const CfgConstEdgeSet &toErase) {
    BOOST_FOREACH (const ControlFlowGraph::ConstEdgeIterator &edge, toErase.values()) {
        ASSERT_require(graph.isValidEdge(edge));
        graph.eraseEdge(edge);
    }
}

CfgConstVertexSet
findIncidentVertices(const CfgConstEdgeSet &edgeSet) {
    CfgConstVertexSet retval;
    BOOST_FOREACH (const ControlFlowGraph::ConstEdgeIterator &edge, edgeSet.values()) {
        retval.insert(edge->source());
        retval.insert(edge->target());
    }
    return retval;
}

CfgConstVertexSet
findDetachedVertices(const ControlFlowGraph &graph) {
    CfgConstVertexSet retval;
    for (ControlFlowGraph::ConstVertexIterator vertex=graph.vertices().begin(); vertex!=graph.vertices().end(); ++vertex) {
        if (0 == vertex->degree())
            retval.insert(vertex);
    }
    return retval;
}

CfgConstVertexSet
findDetachedVertices(const CfgConstVertexSet &vertices) {
    CfgConstVertexSet retval;
    BOOST_FOREACH (const ControlFlowGraph::ConstVertexIterator &vertex, vertices.values()) {
        if (0 == vertex->degree())
            retval.insert(vertex);
    }
    return retval;
}

CfgConstVertexSet
forwardMapped(const CfgConstVertexSet &vertices, const CfgVertexMap &vmap) {
    CfgConstVertexSet retval;
    BOOST_FOREACH (const ControlFlowGraph::ConstVertexIterator &vertex, vertices.values()) {
        if (vmap.forward().exists(vertex))
            retval.insert(vmap.forward()[vertex]);
    }
    return retval;
}

CfgConstVertexSet
reverseMapped(const CfgConstVertexSet &vertices, const CfgVertexMap &vmap) {
    CfgConstVertexSet retval;
    BOOST_FOREACH (const ControlFlowGraph::ConstVertexIterator &vertex, vertices.values()) {
        if (vmap.reverse().exists(vertex))
            retval.insert(vmap.reverse()[vertex]);
    }
    return retval;
}

Sawyer::Container::Map<Function::Ptr, CfgConstEdgeSet>
findFunctionReturnEdges(const Partitioner &partitioner) {
    return findFunctionReturnEdges(partitioner, partitioner.cfg());
}

Sawyer::Container::Map<Function::Ptr, CfgConstEdgeSet>
findFunctionReturnEdges(const Partitioner &partitioner, const ControlFlowGraph &cfg) {
    Sawyer::Container::Map<Function::Ptr, CfgConstEdgeSet> retval;
    for (ControlFlowGraph::ConstEdgeIterator edge = cfg.edges().begin(); edge != cfg.edges().end(); ++edge) {
        if (edge->value().type() == E_FUNCTION_RETURN) {
            if (BasicBlock::Ptr bblock = edge->source()->value().bblock()) {
                std::vector<Function::Ptr> functions = partitioner.functionsOwningBasicBlock(bblock);
                BOOST_FOREACH (const Function::Ptr &function, functions)
                    retval.insertMaybeDefault(function).insert(edge);
            }
        }
    }
    return retval;
}

void
expandFunctionReturnEdges(const Partitioner &partitioner, ControlFlowGraph &cfg/*in,out*/) {
    Sawyer::Container::Map<Function::Ptr, CfgConstEdgeSet> fre = findFunctionReturnEdges(partitioner, cfg);
    CfgConstEdgeSet edgesToErase;                       // erased after iterating

    CfgConstEdgeSet crEdges = findCallReturnEdges(partitioner, cfg);
    BOOST_FOREACH (const ControlFlowGraph::ConstEdgeIterator &crEdge, crEdges.values()) {
        ControlFlowGraph::ConstVertexIterator callSite = crEdge->source();
        ControlFlowGraph::ConstVertexIterator returnSite = crEdge->target();
        CfgConstEdgeSet callEdges = findCallEdges(callSite);
        BOOST_FOREACH (const ControlFlowGraph::ConstEdgeIterator &callEdge, callEdges.values()) {
            if (callEdge->target()->value().type() != V_BASIC_BLOCK)
                continue; // functionCallEdge is not a call to a known function, so ignore it

            BasicBlock::Ptr functionBlock = callEdge->target()->value().bblock();
            std::vector<Function::Ptr> functions = partitioner.functionsOwningBasicBlock(functionBlock);
            BOOST_FOREACH (const Function::Ptr &function, functions) {
                BOOST_FOREACH (const ControlFlowGraph::ConstEdgeIterator &oldReturnEdge, fre.getOrDefault(function).values()) {
                    edgesToErase.insert(oldReturnEdge);
                    cfg.insertEdge(oldReturnEdge->source(), returnSite, E_FUNCTION_RETURN);
                }
            }
        }
    }

    BOOST_FOREACH (ControlFlowGraph::ConstEdgeIterator edge, edgesToErase.values())
        cfg.eraseEdge(edge);
}

ControlFlowGraph
functionCfgByErasure(const ControlFlowGraph &gcfg, const Function::Ptr &function,
                     ControlFlowGraph::VertexIterator &entry/*out*/) {
    ASSERT_not_null(function);

    ControlFlowGraph fcfg = gcfg;
    entry = fcfg.vertices().end();

    // Start by erasing all vertices that aren't owned by the function, but keep the indeterminate vertex.
    ControlFlowGraph::VertexIterator vi = fcfg.vertices().begin();
    ControlFlowGraph::VertexIterator indet = fcfg.vertices().end();
    while (vi != fcfg.vertices().end()) {
        if (vi->value().type() == V_INDETERMINATE) {
            indet = vi++;
        } if (!vi->value().isOwningFunction(function)) {
            fcfg.eraseVertex(vi++);
        } else if (vi->value().type() == V_BASIC_BLOCK && vi->value().address() == function->address()) {
            entry = vi++;
        } else {
            ++vi;
        }
    }

    // If there's an indeterminate vertex, erase all the E_FUNCTION_RETURN edges.
    if (indet != fcfg.vertices().end()) {
        std::vector<ControlFlowGraph::EdgeIterator> toErase;
        for (ControlFlowGraph::EdgeIterator edge = indet->inEdges().begin(); edge != indet->inEdges().end(); ++edge) {
            if (edge->value().type() == E_FUNCTION_RETURN)
                toErase.push_back(edge);
        }
        BOOST_FOREACH (ControlFlowGraph::EdgeIterator &edge, toErase)
            fcfg.eraseEdge(edge);
    }

    // Erase the indeterminate vertex if appropriate
    if (indet != fcfg.vertices().end() && indet->degree() == 0)
        fcfg.eraseVertex(indet);

    return fcfg;
}

ControlFlowGraph
functionCfgByReachability(const ControlFlowGraph &gcfg, const Function::Ptr &function,
                          const ControlFlowGraph::ConstVertexIterator &gcfgEntry) {
    ASSERT_not_null(function);

    ControlFlowGraph fcfg;
    if (gcfg.isEmpty() || !gcfg.isValidVertex(gcfgEntry))
        return fcfg;

    // Copy all reachable vertices that belong to the function, plus copy the indeterminate vertex.
    typedef ControlFlowGraph::ConstVertexIterator ConstVertexIterator;
    typedef ControlFlowGraph::VertexIterator VertexIterator;
    typedef Sawyer::Container::GraphIteratorMap<ConstVertexIterator, VertexIterator> GlobalToFunction;
    GlobalToFunction g2f;
    typedef Sawyer::Container::Algorithm::DepthFirstForwardVertexTraversal<const ControlFlowGraph> Traversal;
    VertexIterator fcfgIndet = fcfg.vertices().end();
    for (Traversal t(gcfg, gcfgEntry); t; ++t) {
        if (t->value().type() == V_INDETERMINATE || t->value().isOwningFunction(function)) {
            VertexIterator vertex = fcfg.insertVertex(t->value());
            if (t->value().type() == V_INDETERMINATE)
                fcfgIndet = vertex;
            g2f.insert(gcfg.findVertex(t->id()), vertex);
        } else {
            t.skipChildren();
        }
    }
    
    // Copy all edges if both endpoint vertices exist in the function CFG. But omit E_FUNCTION_RETURN edges that go
    // to the indeterminate vertex.
    BOOST_FOREACH (const GlobalToFunction::Node &mapping, g2f.nodes()) {
        BOOST_FOREACH (const ControlFlowGraph::Edge &edge, mapping.key()->outEdges()) {
            if ((edge.value().type() != E_FUNCTION_RETURN || edge.target()->value().type() != V_INDETERMINATE) &&
                g2f.exists(edge.target()))
                fcfg.insertEdge(mapping.value(), g2f[edge.target()], edge.value());
        }
    }

    // If we copied the indeterminate vertex but it isn't reachable, remove it.
    if (fcfgIndet != fcfg.vertices().end() && fcfgIndet->nInEdges() == 0 && fcfgIndet->id() != 0)
        fcfg.eraseVertex(fcfgIndet);

    return fcfg;
}

bool
sortVerticesByAddress(const ControlFlowGraph::ConstVertexIterator &a,
                      const ControlFlowGraph::ConstVertexIterator &b) {
    const CfgVertex &av = a->value();
    const CfgVertex &bv = b->value();
    if (av.type() != bv.type() || av.type() != V_BASIC_BLOCK)
        return av.type() < bv.type();
    return av.address() < bv.address();
}

bool sortVerticesById(const ControlFlowGraph::ConstVertexIterator &a,
                      const ControlFlowGraph::ConstVertexIterator &b) {
    return a->id() < b->id();
}

bool
sortEdgesBySrc(const ControlFlowGraph::ConstEdgeIterator &a,
               const ControlFlowGraph::ConstEdgeIterator &b) {
    return sortVerticesByAddress(a->source(), b->source());
}

bool
sortEdgesByDst(const ControlFlowGraph::ConstEdgeIterator &a,
               const ControlFlowGraph::ConstEdgeIterator &b) {
    return sortVerticesByAddress(a->target(), b->target());
}

bool
sortEdgesById(const ControlFlowGraph::ConstEdgeIterator &a,
              const ControlFlowGraph::ConstEdgeIterator &b) {
    return a->id() < b->id();
}

std::ostream&
operator<<(std::ostream &out, const ControlFlowGraph::Vertex &x) {
    out <<Partitioner::vertexName(x);
    return out;
}

std::ostream&
operator<<(std::ostream &out, const ControlFlowGraph::Edge &x) {
    out <<Partitioner::edgeName(x);
    return out;
}

} // namespace
} // namespace
} // namespace

#endif
