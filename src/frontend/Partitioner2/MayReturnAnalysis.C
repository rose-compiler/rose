#include "sage3basic.h"
#include <Partitioner2/Partitioner.h>

#include <Sawyer/GraphTraversal.h>
#include <Sawyer/ProgressBar.h>

using namespace Rose::Diagnostics;

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Internal stuff
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static std::string
toString(boost::logic::tribool tb,
         const std::string &yes="yes", const std::string &no="no", const std::string &unknown="unknown") {
    if (tb == true) {
        return yes;
    } else if (tb == false) {
        return no;
    } else {
        return unknown;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Public methods
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
Partitioner::basicBlockMayReturnReset() const {
    BOOST_FOREACH (const ControlFlowGraph::VertexValue &vertex, cfg_.vertexValues()) {
        if (vertex.type() == V_BASIC_BLOCK) {
            if (BasicBlock::Ptr bblock = vertex.bblock())
                bblock->mayReturn().clear();
        }
    }
}

Sawyer::Optional<bool>
Partitioner::basicBlockOptionalMayReturn(const BasicBlock::Ptr &bb) const {
    ASSERT_not_null(bb);

    bool retval;
    if (bb->mayReturn().getOptional().assignTo(retval))
        return retval;                                  // already cached
    ControlFlowGraph::ConstVertexIterator startVertex = findPlaceholder(bb->address());
    if (startVertex != cfg_.vertices().end())
        return basicBlockOptionalMayReturn(startVertex); // full CFG-based analysis
    
    if (basicBlockIsFunctionReturn(bb)) {
        bb->mayReturn() = true;
        return true;
    }

    BasicBlock::Successors successors;
    if (!bb->successors().getOptional().assignTo(successors))
        return Sawyer::Nothing();

    // If the basic block has any significant successor that may return then this basic block may return.
    bool successorIsIndeterminate=false, calleeMayReturn=false, calleeIsIndeterminate=false;
    std::vector<ControlFlowGraph::ConstVertexIterator> callReturnSuccessors;
    BOOST_FOREACH (const BasicBlock::Successor &edge, successors) {
        if (!edge.expr()->is_number()) {
            successorIsIndeterminate = true;            // sucessor vertex is (or would be) the indeterminate vertex
            continue;
        }
        rose_addr_t successorVa = edge.expr()->get_number();
        if (successorVa == bb->address())
            continue;                                   // a self edge is not significant to the analysis result
        ControlFlowGraph::ConstVertexIterator successorVertex = findPlaceholder(successorVa);
        if (successorVertex == cfg_.vertices().end()) {
            successorIsIndeterminate = true;            // a not-present successor has indeterminate may-return property
            continue;
        }

        bool b;
        if (edge.type() == E_CALL_RETURN) {
            callReturnSuccessors.push_back(successorVertex); // skip for now; they need to be processed last
        } else if (edge.type() == E_FUNCTION_CALL) {
            if (!calleeMayReturn) {
                if (!basicBlockOptionalMayReturn(successorVertex).assignTo(b)) {
                    calleeIsIndeterminate = true;
                } else if (b) {
                    calleeMayReturn = true;
                }
            }
        } else {
            // This is a significant edge
            if (!basicBlockOptionalMayReturn(successorVertex).assignTo(b)) {
                successorIsIndeterminate = true;
            } else if (b) {
                bb->mayReturn() = true;
                return true;                            // bb may return if any significant successor may return
            }
        }
    }

    // Call-return edges can be processed now.
    if (calleeMayReturn) {
        BOOST_FOREACH (const ControlFlowGraph::ConstVertexIterator &successor, callReturnSuccessors) {
            bool b;
            if (!basicBlockOptionalMayReturn(successor).assignTo(b)) {
                successorIsIndeterminate = true;
            } else if (b) {
                bb->mayReturn() = true;
                return true;                            // call-ret is a significant successor that may return
            }
        }
    }

    // If there is no call-return edge and a callee is positive or indeterminate then this block is also indeterminate.
    if (callReturnSuccessors.empty() && (calleeMayReturn || calleeIsIndeterminate))
        return Sawyer::Nothing();

    // None of the significant successors has a positive may-return property.  If they were all negative (no indeterminates)
    // then we can say that this block does not return.
    if (!successorIsIndeterminate) {
        bb->mayReturn() = false;
        return false;
    }
    
    return Sawyer::Nothing();
}

Sawyer::Optional<bool>
Partitioner::basicBlockOptionalMayReturn(const ControlFlowGraph::ConstVertexIterator &start) const {
    ASSERT_require(start != cfg_.vertices().end());

    // Return a cached value if there is one
    if (start->value().type() == V_BASIC_BLOCK) {
        if (BasicBlock::Ptr bblock = start->value().bblock()) {
            bool b;
            if (bblock->mayReturn().getOptional().assignTo(b))
                return b;
        }
    }
    
    // Do the hard work
    std::vector<MayReturnVertexInfo> vertexInfo(cfg_.nVertices());
    return basicBlockOptionalMayReturn(start, vertexInfo);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Internal stuff
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Internal: Is edge significant for may-return analysis?
// + Function call edges are never significant (a return from a function call does not mean that the calling function also
//   returns.
// + A self edge is not significant; it doesn't change the result of the analysis
// + Call-ret edges are significant only if at least one of the sibling function calls returns or if any of the sibling
//   function calls has an indeterminate may-return.
// + Other edges are significant, including E_FUNCTION_XFER edges.
bool
Partitioner::mayReturnIsSignificantEdge(const ControlFlowGraph::ConstEdgeIterator &edge,
                                        std::vector<MayReturnVertexInfo> &vertexInfo) const {
    if (edge == cfg_.edges().end())
        return false;
    if (edge->value().type() == E_FUNCTION_CALL)
        return false;
    if (edge->target() == edge->source())
        return false;
    if (edge->value().type() == E_CALL_RETURN) {
        boost::logic::tribool tb = mayReturnDoesCalleeReturn(edge->source(), vertexInfo);
        return tb || boost::logic::indeterminate(tb) ? true : false;
    }
    return true;
}

// Internal: Run the may-return analysis on the callees and return true if any of them have a positive or indeterminate
// may-return property.
boost::logic::tribool
Partitioner::mayReturnDoesCalleeReturn(const ControlFlowGraph::ConstVertexIterator &caller,
                                       std::vector<MayReturnVertexInfo> &vertexInfo) const {
    ASSERT_require(caller != cfg_.vertices().end());
    if (!vertexInfo[caller->id()].processedCallees) {
        vertexInfo[caller->id()].anyCalleesReturn = false;
        bool hasPositiveCallee = false, hasIndeterminateCallee = false;
        BOOST_FOREACH (const ControlFlowGraph::Edge &edge, caller->outEdges()) {
            if (edge.value().type() == E_FUNCTION_CALL) {
                ControlFlowGraph::ConstVertexIterator callee = edge.target();
                bool mayReturn = false;
                if (!basicBlockOptionalMayReturn(callee, vertexInfo).assignTo(mayReturn)) {
                    hasIndeterminateCallee = true;
                } else if (mayReturn) {
                    hasPositiveCallee = true;
                    break;
                }
            }
        }
        if (hasPositiveCallee) {
            vertexInfo[caller->id()].anyCalleesReturn = true;
        } else if (hasIndeterminateCallee) {
            vertexInfo[caller->id()].anyCalleesReturn = boost::logic::indeterminate;
        }
        vertexInfo[caller->id()].processedCallees = true;
    }
    return vertexInfo[caller->id()].anyCalleesReturn;
}

// Internal: Returns non-negative (positive or indeterminate) if any significant successor of this vertex has non-negative
// may-return.  Simulates a phantom call-return edge if necessary since such edges might not yet be present in the CFG.  For
// instance, if vertex V calls vertex U but V has no call-return edge then a phantom one will be created unless U has negative
// may-return. The phantom call-return edge is assumed to point to a phantom vertex with indeterminate may-return.
boost::logic::tribool
Partitioner::mayReturnDoesSuccessorReturn(const ControlFlowGraph::ConstVertexIterator &vertex,
                                          std::vector<MayReturnVertexInfo> &vertexInfo) const {
    ASSERT_require(vertex->value().type() == V_BASIC_BLOCK);

    bool hasIndeterminateSuccessor = false; // does any significant successor have indeterminate may-return?
    bool isFunctionCall = false;            // does vertex have any function call edges?
    bool hasIndeterminateCallee = false;    // does any callee have indeterminate may-return?
    bool hasPositiveCallee = false;         // does any callee have positive may-return
    bool hasCallReturnEdge = false;         // does vertex have any call-return edges?

    for (ControlFlowGraph::ConstEdgeIterator edge = vertex->outEdges().begin();
         edge != vertex->outEdges().end(); ++edge) {
        ControlFlowGraph::ConstVertexIterator successor = edge->target();
        if (mayReturnIsSignificantEdge(edge, vertexInfo)) {
            // Significant successors
            if (edge->value().type() == E_CALL_RETURN)
                hasCallReturnEdge = true;

            if (vertexInfo[successor->id()].result) {
                return true;
            } else if (boost::logic::indeterminate(vertexInfo[successor->id()].result)) {
                hasIndeterminateSuccessor = true;
            }
        } else if (edge->value().type() == E_FUNCTION_CALL) {
            // Callees
            isFunctionCall = true;
            bool mayReturn = false;
            if (!basicBlockOptionalMayReturn(successor, vertexInfo).assignTo(mayReturn)) {
                hasIndeterminateCallee = true;
            } else if (mayReturn) {
                hasPositiveCallee = true;
            }
        }
    }

    if (hasIndeterminateSuccessor)
        return boost::logic::indeterminate;

    if (isFunctionCall && !hasCallReturnEdge && (hasIndeterminateCallee || hasPositiveCallee))
        return boost::logic::indeterminate; // assume phantom call-return edge is indeterminate

    return false;
}

// Internal: returns true if "start" could return to its caller, false if it cannot ever return, and nothing if we can't
// decide yet.
Sawyer::Optional<bool>
Partitioner::basicBlockOptionalMayReturn(const ControlFlowGraph::ConstVertexIterator &start,
                                         std::vector<MayReturnVertexInfo> &vertexInfo) const {
    using namespace Sawyer::Container::Algorithm;
    Sawyer::Message::Stream debug(mlog[DEBUG]);

    static size_t depth = 0;
    struct Depth {
        Depth() { ++depth; }
        ~Depth() { --depth; }
    } depthObserver;

    ASSERT_require(start != cfg_.vertices().end());
    SAWYER_MESG(debug) <<"[" <<depth <<"] basicBlockMayReturn(" <<vertexName(start) <<") ...\n";

    typedef DepthFirstForwardGraphTraversal<const ControlFlowGraph> Traversal;
    for (Traversal t(cfg_, start, ENTER_EDGE|ENTER_VERTEX|LEAVE_VERTEX); t; ++t) {
        switch (t.event()) {
            case ENTER_EDGE: {
                // The call to mayReturnIsSignificantEdge will be recursive if t.vertex is a function call
                if (!mayReturnIsSignificantEdge(t.edge(), vertexInfo)) {
                    SAWYER_MESG(debug) <<"[" <<depth <<"]   skipping edge " <<edgeName(t.edge()) <<"\n";
                    t.skipChildren();
                } else {
                    SAWYER_MESG(debug) <<"[" <<depth <<"]   following edge " <<edgeName(t.edge()) <<"\n";
                }
                break;
            }

            case ENTER_VERTEX: {
                SAWYER_MESG(debug) <<"[" <<depth <<"]   enter vertex " <<vertexName(t.vertex())
                                   <<" via " <<edgeNameSrc(t.edge()) <<"\n";

                // Recursion termination
                switch (vertexInfo[t.vertex()->id()].state) {
                    case MayReturnVertexInfo::INIT:
                        vertexInfo[t.vertex()->id()].state = MayReturnVertexInfo::CALCULATING;
                        break;
                    case MayReturnVertexInfo::CALCULATING:
                        SAWYER_MESG(debug) <<"[" <<depth <<"]     recursive query; returning nothing\n";
                        t.skipChildren();
                        return Sawyer::Nothing();
                    case MayReturnVertexInfo::FINISHED:
                        SAWYER_MESG(debug) <<"[" <<depth <<"]     already calculated: may-return is "
                                           <<toString(vertexInfo[t.vertex()->id()].result) <<"\n";
                        t.skipChildren();
                        if (vertexInfo[t.vertex()->id()].result)
                            return true;
                        if (!vertexInfo[t.vertex()->id()].result)
                            return false;
                        return Sawyer::Nothing();
                }

                // Can we get the may-return value immediately, or do we need to follow outgoing edges first?
                if (t.vertex()->value().type() == V_BASIC_BLOCK) {
                    BasicBlock::Ptr bb = t.vertex()->value().bblock();

                    // Properties of functions that own this block.
                    Function::Ptr isBlackListed, isWhiteListed, isDynamicLinked;
                    BOOST_FOREACH (const Function::Ptr &function, t.vertex()->value().owningFunctions().values()) {
                        if (function->address() == t.vertex()->value().address()) {
                            bool b = false;
                            if (config_.functionMayReturn(function).assignTo(b)) {
                                if (b) {
                                    isWhiteListed = function;
                                } else {
                                    isBlackListed = function;
                                }
                            } else if (boost::ends_with(function->name(), "@plt") ||
                                       boost::ends_with(function->name(), "@iat")) {
                                isDynamicLinked = function;
                            }
                        }
                    }

                    if (isWhiteListed && isBlackListed) {
                        // Block is owned by functions that are both white and black listed for may-return. Assume white.
                        SAWYER_MESG(debug) <<"[" <<depth <<"]     block "
                                           <<StringUtility::addrToString(t.vertex()->value().address())
                                           <<" belongs to white- and black-listed functions (assuming white).\n";
                        SAWYER_MESG(debug) <<"[" <<depth <<"]         example white: " <<isWhiteListed->printableName() <<"\n"
                                           <<"[" <<depth <<"]         example black: " <<isBlackListed->printableName() <<"\n";
                        vertexInfo[t.vertex()->id()].result = true;
                        t.skipChildren();
                    } else if (isWhiteListed) {
                        // Block is whitelisted by some owning function.
                        SAWYER_MESG(debug) <<"[" <<depth <<"]     " <<isWhiteListed->printableName() <<" is whitelisted\n";
                        vertexInfo[t.vertex()->id()].result = true;
                        t.skipChildren();
                    } else if (isBlackListed) {
                        // Block is blacklisted by some owning function.
                        SAWYER_MESG(debug) <<"[" <<depth <<"]     " <<isBlackListed->printableName() <<" is blacklisted\n";
                        vertexInfo[t.vertex()->id()].result = false;
                        t.skipChildren();
                    } else if (isDynamicLinked) {
                        // Dynamically linked functions return or not by definition
                        bool b = assumeFunctionsReturn_;
                        SAWYER_MESG(debug) <<"[" <<depth <<"]    " <<isDynamicLinked->printableName()
                                           <<" is " <<(b?"whitelisted":"blacklisted") <<" by /@plt$/ pattern\n";
                        vertexInfo[t.vertex()->id()].result = b;
                        t.skipChildren();
                    } else if (t.vertex()->nOutEdges()==1 && t.vertex()->outEdges().begin()->target()==nonexistingVertex_) {
                        // Non-existing vertex returns or not by definition
                        SAWYER_MESG(debug) <<"[" <<depth <<"]    " <<vertexName(t.vertex())
                                           <<(assumeFunctionsReturn_?"returns":"does not return")
                                           <<" by virtue of not existing\n";
                        vertexInfo[t.vertex()->id()].result = assumeFunctionsReturn_;
                        t.skipChildren();
                    } else if (bb && bb->mayReturn().isCached()) {
                        // Basic block may-return is already calculated
                        bool b = bb->mayReturn().get();
                        SAWYER_MESG(debug) <<"[" <<depth <<"]     already cached: may-return is " <<(b?"yes":"no") <<"\n";
                        vertexInfo[t.vertex()->id()].result = b;
                        t.skipChildren();
                    } else if (bb && basicBlockIsFunctionReturn(bb)) {
                        // This is a function return statement, so it obviously returns
                        SAWYER_MESG(debug) <<"[" <<depth <<"]     block is a function return; may-return is yes\n";
                        bb->mayReturn() = true;
                        vertexInfo[t.vertex()->id()].result = true;
                        t.skipChildren();
                    } else if (bb && basicBlockIsFunctionCall(bb)) {
                        // Function calls handled by recursively computing may-return in the callee
                        SAWYER_MESG(debug) <<"[" <<depth <<"]     block is a function call; process callees...\n";
                        boost::logic::tribool tb = mayReturnDoesCalleeReturn(t.vertex(), vertexInfo);
                        SAWYER_MESG(debug) <<"[" <<depth <<"]     callees for " <<vertexName(t.vertex()) <<" processed"
                                           <<"; " <<(tb || boost::logic::indeterminate(tb)?"at least one":"none")
                                           <<" may return\n";
                    } else if (t.vertex()->nOutEdges()==1 && t.vertex()->outEdges().begin()->target()==indeterminateVertex_ &&
                               assumeFunctionsReturn_) {
                        // Indeterminate successor is likely a permanent condition
                        SAWYER_MESG(debug) <<"[" <<depth <<"]    " <<vertexName(t.vertex())
                                           <<" returns by virtue of having only an indeterminate successor\n";
                        vertexInfo[t.vertex()->id()].result = assumeFunctionsReturn_;
                        t.skipChildren();
                    } else if (!addressIsExecutable(t.vertex()->value().address())) {
                        // Non-executable address is perhaps not possible, but such returns or not by definition
                        SAWYER_MESG(debug) <<"[" <<depth <<"]     " <<vertexName(t.vertex())
                                           <<(assumeFunctionsReturn_?"returns":"does not return")
                                           <<" by virtue of being at a non-executable address\n";
                        vertexInfo[t.vertex()->id()].result = assumeFunctionsReturn_;
                        t.skipChildren();
                    } else if (BasicBlock::Ptr bb = t.vertex()->value().bblock()) {
                        // FIXME[Robb P. Matzke 2014-12-17]: Need to handle graph cycles, but for now we'll just say that the
                        // may-return for a cycle is indeterminate.  E.g., see test7_no in i386-may-return-tests.
                        SAWYER_MESG(debug) <<"[" <<depth <<"]     must process vertex successors first...\n";
                    }
                }
                break;
            }
                    
            case LEAVE_VERTEX: {
                ASSERT_require(vertexInfo[t.vertex()->id()].state == MayReturnVertexInfo::CALCULATING);
                SAWYER_MESG(debug) <<"[" <<depth <<"]   leaving vertex " <<vertexName(t.vertex()) <<"\n";
                if (t.vertex()->value().type() == V_BASIC_BLOCK) {
                    if (boost::logic::indeterminate(vertexInfo[t.vertex()->id()].result)) {
                        // This vertex has positive may-return if any of its significant successors have positive
                        // may-return. Otherwise it has indeterminate may-return if any of its significant successors is
                        // indeterminate. Otherwise, it has negative may-return.
                        SAWYER_MESG(debug) <<"[" <<depth <<"]     calling mayReturnDoesSuccessorReturn...\n";
                        boost::logic::tribool tb = mayReturnDoesSuccessorReturn(t.vertex(), vertexInfo);
                        vertexInfo[t.vertex()->id()].result = tb;
                        SAWYER_MESG(debug) <<"[" <<depth <<"]     mayReturnDoesSuccessorReturn = " <<toString(tb) <<"\n";
                    }
                    if (BasicBlock::Ptr bblock = t.vertex()->value().bblock()) {
                        boost::logic::tribool tb = vertexInfo[t.vertex()->id()].result;
                        if (tb) {
                            bblock->mayReturn() = true;
                        } else if (!tb) {
                            bblock->mayReturn() = false;
                        } else {
                            bblock->mayReturn().clear();
                        }
                    }
                }
                vertexInfo[t.vertex()->id()].state = MayReturnVertexInfo::FINISHED;
                SAWYER_MESG(debug) <<"[" <<depth <<"]   leaving vertex " <<vertexName(t.vertex())
                                   <<"; may-return is " <<toString(vertexInfo[t.vertex()->id()].result) <<"\n";
                break;
            }

            default:
                ASSERT_not_reachable("event should not occur");
        }
    }

    if (vertexInfo[start->id()].result) {
        return true;
    } else if (!vertexInfo[start->id()].result) {
        return false;
    } else {
        return Sawyer::Nothing();
    }
}

Sawyer::Optional<bool>
Partitioner::functionOptionalMayReturn(const Function::Ptr &function) const {
    ASSERT_not_null(function);
    ControlFlowGraph::ConstVertexIterator entryVertex = findPlaceholder(function->address());
    if (entryVertex != cfg_.vertices().end() && entryVertex->value().type() == V_BASIC_BLOCK)
        return basicBlockOptionalMayReturn(entryVertex);
    return Sawyer::Nothing();
}

void
Partitioner::allFunctionMayReturn() const {
    using namespace Sawyer::Container::Algorithm;
    FunctionCallGraph cg = functionCallGraph(AllowParallelEdges::NO);
    size_t nFunctions = cg.graph().nVertices();
    std::vector<bool> visited(nFunctions, false);
    Sawyer::ProgressBar<size_t> progress(nFunctions, mlog[MARCH], "may-return analysis");
    progress.suffix(" functions");
    for (size_t cgVertexId=0; cgVertexId<nFunctions; ++cgVertexId) {
        if (!visited[cgVertexId]) {
            typedef DepthFirstForwardGraphTraversal<const FunctionCallGraph::Graph> Traversal;
            for (Traversal t(cg.graph(), cg.graph().findVertex(cgVertexId), ENTER_VERTEX|LEAVE_VERTEX); t; ++t) {
                if (t.event() == ENTER_VERTEX) {
                    if (visited[t.vertex()->id()])
                        t.skipChildren();
                } else if (!visited[t.vertex()->id()]) {
                    ASSERT_require(t.event() == LEAVE_VERTEX);
                    functionOptionalMayReturn(t.vertex()->value());
                    visited[t.vertex()->id()] = true;

                    // Update progress reports
                    ++progress;
                    updateProgress("may-return", progress.ratio());
                }
            }
        }
    }
}

} // namespace
} // namespace
} // namespace
