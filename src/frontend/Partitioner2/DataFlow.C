#include "sage3basic.h"

#include <Partitioner2/DataFlow.h>
#include <Partitioner2/GraphViz.h>
#include <Partitioner2/Partitioner.h>
#include <sawyer/GraphTraversal.h>

using namespace Sawyer::Container;
using namespace Sawyer::Container::Algorithm;

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {
namespace DataFlow {


// private class
class DfCfgBuilder {
public:
    const Partitioner &partitioner;
    const ControlFlowGraph &cfg;                                 // global control flow graph
    const ControlFlowGraph::ConstVertexNodeIterator startVertex; // where to start in the global CFG
    DfCfg dfCfg;                                                 // dataflow control flow graph we are building
    InterproceduralPredicate &interproceduralPredicate;          // returns true when a call should be inlined

    // maps CFG vertex ID to dataflow vertex
    typedef Sawyer::Container::Map<ControlFlowGraph::ConstVertexNodeIterator, DfCfg::VertexNodeIterator> VertexMap;

    // Info about one function call
    struct CallFrame {
        VertexMap vmap;
        DfCfg::VertexNodeIterator functionReturnVertex;
        bool wasFaked;
        CallFrame(DfCfg &dfCfg): functionReturnVertex(dfCfg.vertices().end()), wasFaked(false) {}
    };

    typedef std::list<CallFrame> CallStack;             // we use a list since there's no default constructor for an iterator
    CallStack callStack;
    size_t maxCallStackSize;
    

    DfCfgBuilder(const Partitioner &partitioner, const ControlFlowGraph &cfg,
                 const ControlFlowGraph::ConstVertexNodeIterator &startVertex, InterproceduralPredicate &predicate)
        : partitioner(partitioner), cfg(cfg), startVertex(startVertex), interproceduralPredicate(predicate),
          maxCallStackSize(4) {}
    
    typedef DepthFirstForwardGraphTraversal<const ControlFlowGraph> CfgTraversal;

    DfCfg::VertexNodeIterator findVertex(const ControlFlowGraph::ConstVertexNodeIterator cfgVertex) {
        ASSERT_require(!callStack.empty());
        CallFrame &callFrame = callStack.back();
        return callFrame.vmap.getOrElse(cfgVertex, dfCfg.vertices().end());
    }

    bool isValidVertex(const DfCfg::VertexNodeIterator &dfVertex) {
        return dfVertex != dfCfg.vertices().end();
    }

    DfCfg::VertexNodeIterator insertVertex(const DfCfgVertex &dfVertex,
                                           const ControlFlowGraph::ConstVertexNodeIterator &cfgVertex) {
        ASSERT_require(!callStack.empty());
        CallFrame &callFrame = callStack.back();
        ASSERT_require(cfgVertex != cfg.vertices().end());
        ASSERT_require(!callFrame.vmap.exists(cfgVertex));
        DfCfg::VertexNodeIterator dfVertexIter = dfCfg.insertVertex(dfVertex);
        callFrame.vmap.insert(cfgVertex, dfVertexIter);
        return dfVertexIter;
    }

    DfCfg::VertexNodeIterator insertVertex(const DfCfgVertex &dfVertex) {
        return dfCfg.insertVertex(dfVertex);
    }

    DfCfg::VertexNodeIterator insertVertex(DfCfgVertex::Type type) {
        return insertVertex(DfCfgVertex(type));
    }

    DfCfg::VertexNodeIterator insertVertex(DfCfgVertex::Type type, const ControlFlowGraph::ConstVertexNodeIterator &cfgVertex) {
        return insertVertex(DfCfgVertex(type), cfgVertex);
    }

    // Insert basic block if it hasn't been already
    DfCfg::VertexNodeIterator findOrInsertBasicBlockVertex(const ControlFlowGraph::ConstVertexNodeIterator &cfgVertex) {
        ASSERT_require(!callStack.empty());
        CallFrame &callFrame = callStack.back();
        ASSERT_require(cfgVertex != cfg.vertices().end());
        ASSERT_require(cfgVertex->value().type() == V_BASIC_BLOCK);
        DfCfg::VertexNodeIterator retval = dfCfg.vertices().end();
        if (!callFrame.vmap.getOptional(cfgVertex).assignTo(retval)) {
            BasicBlock::Ptr bblock = cfgVertex->value().bblock();
            ASSERT_not_null(bblock);
            retval = insertVertex(DfCfgVertex(bblock), cfgVertex);

            // All function return basic blocks will point only to the special FUNCRET vertex.
            if (partitioner.basicBlockIsFunctionReturn(bblock)) {
                if (!isValidVertex(callFrame.functionReturnVertex))
                    callFrame.functionReturnVertex = insertVertex(DfCfgVertex::FUNCRET);
                dfCfg.insertEdge(retval, callFrame.functionReturnVertex);
            }
        }
        return retval;
    }

    // Returns the dfCfg vertex for a CALL return-to vertex, creating it if necessary.  There might be none, in which case the
    // vertex end iterator is returned.
    DfCfg::VertexNodeIterator findOrInsertCallReturnVertex(const ControlFlowGraph::ConstVertexNodeIterator &cfgVertex) {
        ASSERT_require(cfgVertex != cfg.vertices().end());
        ASSERT_require(cfgVertex->value().type() == V_BASIC_BLOCK);
        DfCfg::VertexNodeIterator retval = dfCfg.vertices().end();
        BOOST_FOREACH (const ControlFlowGraph::EdgeNode &edge, cfgVertex->outEdges()) {
            if (edge.value().type() == E_CALL_RETURN) {
                ASSERT_require(edge.target()->value().type() == V_BASIC_BLOCK);
                ASSERT_require2(retval == dfCfg.vertices().end(),
                                edge.target()->value().bblock()->printableName() + " has multiple call-return edges");
                retval = findOrInsertBasicBlockVertex(edge.target());
            }
        }
        return retval;
    }

    // top-level build function.
    DfCfgBuilder& build() {
        callStack.push_back(CallFrame(dfCfg));
        for (CfgTraversal t(cfg, startVertex, ENTER_EVENTS|LEAVE_EDGE); t; ++t) {
            if (t.event() == ENTER_VERTEX) {
                if (t.vertex()->value().type() == V_BASIC_BLOCK) {
                    findOrInsertBasicBlockVertex(t.vertex());
                    if (partitioner.basicBlockIsFunctionReturn(t.vertex()->value().bblock()))
                        t.skipChildren();               // we're handling return successors explicitly
                } else {
                    insertVertex(DfCfgVertex::INDET, t.vertex());
                }
            } else {
                ASSERT_require(t.event()==ENTER_EDGE || t.event()==LEAVE_EDGE);
                ControlFlowGraph::ConstEdgeNodeIterator edge = t.edge();

                if (edge->value().type() == E_CALL_RETURN) {
                    // Do nothing; we handle call-return edges as part of function calls.

                } else if (edge->value().type() == E_FUNCTION_CALL) {
                    if (t.event() == ENTER_EDGE) {
                        DfCfg::VertexNodeIterator callFrom = findVertex(edge->source());
                        ASSERT_require(isValidVertex(callFrom));
                        callStack.push_back(CallFrame(dfCfg));
                        if (callStack.size() <= maxCallStackSize && edge->target()->value().type()==V_BASIC_BLOCK &&
                            interproceduralPredicate(cfg, edge)) {
                            // Incorporate the call into the dfCfg
                            DfCfg::VertexNodeIterator callTo = findOrInsertBasicBlockVertex(edge->target());
                            ASSERT_require(isValidVertex(callTo));
                            dfCfg.insertEdge(callFrom, callTo);
                        } else {
                            callStack.back().wasFaked = true;
                            t.skipChildren();
                        }
                    } else {
                        ASSERT_require(t.event() == LEAVE_EDGE);
                        ASSERT_require(callStack.size()>1);

                        if (!callStack.back().wasFaked) {
                            // Wire up the return from the called function back to the return-to point in the caller.
                            DfCfg::VertexNodeIterator returnFrom = callStack.back().functionReturnVertex;
                            callStack.pop_back();
                            DfCfg::VertexNodeIterator returnTo = findOrInsertCallReturnVertex(edge->source());
                            if (isValidVertex(returnFrom) && isValidVertex(returnTo))
                                dfCfg.insertEdge(returnFrom, returnTo);
                            ASSERT_require(!callStack.empty());
                        } else {
                            // Build the faked-call vertex and wire it up so the CALL goes to the faked-call vertex, which then
                            // flows to the CALL's return-point.
                            callStack.pop_back();
                            Function::Ptr callee;
                            if (edge->target()->value().type() == V_BASIC_BLOCK)
                                callee = edge->target()->value().function();
                            DfCfg::VertexNodeIterator dfSource = findVertex(edge->source());
                            ASSERT_require(isValidVertex(dfSource));
                            DfCfg::VertexNodeIterator faked = insertVertex(DfCfgVertex(callee));
                            dfCfg.insertEdge(dfSource, faked);
                            DfCfg::VertexNodeIterator returnTo = findOrInsertCallReturnVertex(edge->source());
                            if (isValidVertex(returnTo))
                                dfCfg.insertEdge(faked, returnTo);
                        }
                    }
                    
                } else {
                    // Generic edges
                    if (t.event() == LEAVE_EDGE) {
                        DfCfg::VertexNodeIterator dfSource = findVertex(edge->source());
                        ASSERT_require(isValidVertex(dfSource));
                        DfCfg::VertexNodeIterator dfTarget = findVertex(edge->target()); // the called function
                        if (isValidVertex(dfTarget))
                            dfCfg.insertEdge(dfSource, dfTarget);
                    }
                }
            }
        }
        return *this;
    }
};
        
DfCfg
buildDfCfg(const Partitioner &partitioner,
           const ControlFlowGraph &cfg, const ControlFlowGraph::ConstVertexNodeIterator &startVertex,
           InterproceduralPredicate &predicate) {
    return DfCfgBuilder(partitioner, cfg, startVertex, predicate).build().dfCfg;
}

void
dumpDfCfg(std::ostream &out, const DfCfg &dfCfg) {
    out <<"digraph dfCfg {\n";

    BOOST_FOREACH (const DfCfg::VertexNode &vertex, dfCfg.vertices()) {
        out <<vertex.id() <<" [ label=";
        switch (vertex.value().type()) {
            case DfCfgVertex::BBLOCK:
                out <<"\"" <<vertex.value().bblock()->printableName() <<"\"";
                break;
            case DfCfgVertex::FAKED_CALL:
                if (Function::Ptr callee = vertex.value().callee()) {
                    out <<"<fake call to " <<GraphViz::htmlEscape(vertex.value().callee()->printableName()) <<">";
                } else {
                    out <<"\"fake call to indeterminate function\"";
                }
                break;
            case DfCfgVertex::FUNCRET:
                out <<"\"function return\"";
                break;
            case DfCfgVertex::INDET:
                out <<"\"indeterminate\"";
                break;
        }
        out <<" ];\n";
    }

    BOOST_FOREACH (const DfCfg::EdgeNode &edge, dfCfg.edges()) {
        out <<edge.source()->id() <<" -> " <<edge.target()->id() <<";\n";
    }
    
    out <<"}\n";
}


} // namespace
} // namespace
} // namespace
} // namespace
