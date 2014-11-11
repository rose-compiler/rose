#include "sage3basic.h"
#include <Partitioner2/Partitioner.h>

#include <sawyer/GraphTraversal.h>

using namespace rose::Diagnostics;

namespace rose {
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

void
Partitioner::setMayReturnWhitelisted(const std::string &functionName, bool state) {
    if (state) {
        mayReturnList_.insert(functionName, true);
    } else {
        mayReturnList_.erase(functionName);
    }
}

void
Partitioner::setMayReturnBlacklisted(const std::string &functionName, bool state) {
    if (state) {
        mayReturnList_.insert(functionName, false);
    } else {
        mayReturnList_.erase(functionName);
    }
}

void
Partitioner::adjustMayReturnList(const std::string &functionName, boost::tribool state) {
    if (state==true || state==false) {
        mayReturnList_.insert(functionName, state);
    } else {
        mayReturnList_.erase(functionName);
    }
}

bool
Partitioner::isMayReturnWhitelisted(const std::string &functionName) const {
    return mayReturnList_.getOrElse(functionName, false);
}

bool
Partitioner::isMayReturnBlacklisted(const std::string &functionName) const {
    return !mayReturnList_.getOrElse(functionName, true);
}

bool
Partitioner::isMayReturnListed(const std::string &functionName, bool dflt) const {
    if (dflt) { // return false iff name is blacklisted
        return mayReturnList_.getOrElse(functionName, true);
    } else {    // return true iff name is whitelisted
        return mayReturnList_.getOrElse(functionName, false);
    }
}

Sawyer::Optional<bool>
Partitioner::basicBlockOptionalMayReturn(const BasicBlock::Ptr &bb, boost::logic::tribool dflt,
                                         bool recompute) const {
    ASSERT_not_null(bb);
    if (recompute)
        bb->mayReturn().clear();

    bool retval;
    if (bb->mayReturn().getOptional().assignTo(retval))
        return retval;                                  // already cached
    ControlFlowGraph::ConstVertexNodeIterator startVertex = findPlaceholder(bb->address());
    if (startVertex != cfg_.vertices().end())
        return basicBlockOptionalMayReturn(startVertex, dflt, recompute); // full CFG-based analysis
    
    if (basicBlockIsFunctionReturn(bb)) {
        bb->mayReturn() = true;
        return true;
    }

    BasicBlock::Successors successors;
    if (!bb->successors().getOptional().assignTo(successors))
        return Sawyer::Nothing();

    // If the basic block has any significant successor that may return then this basic block may return.
    bool successorIsIndeterminate=false, calleeMayReturn=false, calleeIsIndeterminate=false;
    std::vector<ControlFlowGraph::ConstVertexNodeIterator> callReturnSuccessors;
    BOOST_FOREACH (const BasicBlock::Successor &edge, successors) {
        if (!edge.expr()->is_number()) {
            successorIsIndeterminate = true;            // sucessor vertex is (or would be) the indeterminate vertex
            continue;
        }
        rose_addr_t successorVa = edge.expr()->get_number();
        if (successorVa == bb->address())
            continue;                                   // a self edge is not significant to the analysis result
        ControlFlowGraph::ConstVertexNodeIterator successorVertex = findPlaceholder(successorVa);
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
        BOOST_FOREACH (const ControlFlowGraph::ConstVertexNodeIterator &successor, callReturnSuccessors) {
            bool b;
            if (!basicBlockOptionalMayReturn(successor).assignTo(b)) {
                successorIsIndeterminate = true;
            } else if (b) {
                bb->mayReturn() = true;
                return true;                            // call-ret is a significant successor that may return
            }
        }
    }

    // None of the significant successors has a positive may-return property.  If they were all negative (no indeterminates)
    // then we can say that this block does not return.
    if (!successorIsIndeterminate) {
        bb->mayReturn() = false;
        return false;
    }
    
    return Sawyer::Nothing();
}

Sawyer::Optional<bool>
Partitioner::basicBlockOptionalMayReturn(const ControlFlowGraph::ConstVertexNodeIterator &start,
                                         boost::logic::tribool dflt, bool recompute) const {
    ASSERT_require(start != cfg_.vertices().end());

    // Return a cached value if there is one
    if (!recompute && start->value().type() == V_BASIC_BLOCK) {
        if (BasicBlock::Ptr bblock = start->value().bblock()) {
            bool b;
            if (bblock->mayReturn().getOptional().assignTo(b))
                return b;
        }
    }
    
    // Do the hard work
    std::vector<MayReturnVertexInfo> vertexInfo(cfg_.nVertices());
    return basicBlockOptionalMayReturn(start, dflt, recompute, vertexInfo);
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
Partitioner::mayReturnIsSignificantEdge(const ControlFlowGraph::ConstEdgeNodeIterator &edge,
                                        boost::logic::tribool dflt, bool recompute,
                                        std::vector<MayReturnVertexInfo> &vertexInfo) const {
    if (edge == cfg_.edges().end())
        return false;
    if (edge->value().type() == E_FUNCTION_CALL)
        return false;
    if (edge->target() == edge->source())
        return false;
    if (edge->value().type() == E_CALL_RETURN) {
        boost::logic::tribool tb = mayReturnDoesCalleeReturn(edge->source(), dflt, recompute, vertexInfo);
        return tb || boost::logic::indeterminate(tb);
    }
    return true;
}

// Internal: Run the may-return analysis on the callees and return true if any of them have a positive or indeterminate
// may-return property.
boost::logic::tribool
Partitioner::mayReturnDoesCalleeReturn(const ControlFlowGraph::ConstVertexNodeIterator &caller,
                                       boost::logic::tribool dflt, bool recompute,
                                       std::vector<MayReturnVertexInfo> &vertexInfo) const {
    ASSERT_require(caller != cfg_.vertices().end());
    if (!vertexInfo[caller->id()].processedCallees) {
        vertexInfo[caller->id()].anyCalleesReturn = false;
        bool hasPositiveCallee = false, hasIndeterminateCallee = false;
        BOOST_FOREACH (const ControlFlowGraph::EdgeNode &edge, caller->outEdges()) {
            if (edge.value().type() == E_FUNCTION_CALL) {
                ControlFlowGraph::ConstVertexNodeIterator callee = edge.target();
                bool mayReturn = false;
                if (!basicBlockOptionalMayReturn(callee, dflt, recompute, vertexInfo).assignTo(mayReturn)) {
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
Partitioner::mayReturnDoesSuccessorReturn(const ControlFlowGraph::ConstVertexNodeIterator &vertex,
                                          boost::logic::tribool dflt, bool recompute,
                                          std::vector<MayReturnVertexInfo> &vertexInfo) const {
    ASSERT_require(vertex->value().type() == V_BASIC_BLOCK);

    bool hasIndeterminateSuccessor = false; // does any significant successor have indeterminate may-return?
    bool isFunctionCall = false;            // does vertex have any function call edges?
    bool hasIndeterminateCallee = false;    // does any callee have indeterminate may-return?
    bool hasPositiveCallee = false;         // does any callee have positive may-return
    bool hasCallReturnEdge = false;         // does vertex have any call-return edges?

    for (ControlFlowGraph::ConstEdgeNodeIterator edge = vertex->outEdges().begin();
         edge != vertex->outEdges().end(); ++edge) {

        ControlFlowGraph::ConstVertexNodeIterator successor = edge->target();
        if (mayReturnIsSignificantEdge(edge, dflt, recompute, vertexInfo)) {
            // Significant successors
            if (vertexInfo[successor->id()].result) {
                return true;
            } else if (boost::logic::indeterminate(vertexInfo[successor->id()].result)) {
                hasIndeterminateSuccessor = true;
            }
        } else if (edge->value().type() == E_FUNCTION_CALL) {
            // Callees
            isFunctionCall = true;
            bool mayReturn = false;
            if (!basicBlockOptionalMayReturn(successor, dflt, recompute, vertexInfo).assignTo(mayReturn)) {
                hasIndeterminateCallee = true;
            } else if (mayReturn) {
                hasPositiveCallee = true;
            }
        } else if (edge->value().type() == E_CALL_RETURN) {
            hasCallReturnEdge = true;
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
Partitioner::basicBlockOptionalMayReturn(const ControlFlowGraph::ConstVertexNodeIterator &start,
                                         boost::logic::tribool dflt, bool recompute,
                                         std::vector<MayReturnVertexInfo> &vertexInfo) const {
    using namespace Sawyer::Container::Algorithm;
    Sawyer::Message::Stream debug(mlog[DEBUG]);

    static size_t depth = 0;
    struct Depth {
        Depth() { ++depth; }
        ~Depth() { --depth; }
    } depthObserver;

    ASSERT_require(start != cfg_.vertices().end());
    SAWYER_MESG(debug) <<"[" <<depth <<"] basicBlockMayReturn(" <<vertexName(start)
                       <<", dflt=" <<toString(dflt, "true", "false", "indeterminate")
                       <<", recompute=" <<(recompute?"true":"false") <<") ...\n";

    typedef DepthFirstForwardGraphTraversal<const ControlFlowGraph> Traversal;
    for (Traversal t(cfg_, start, ENTER_EDGE|ENTER_VERTEX|LEAVE_VERTEX); t; ++t) {
        switch (t.event()) {
            case ENTER_EDGE: {
                // The call to mayReturnIsSignificantEdge will be recursive if t.vertex is a function call
                if (!mayReturnIsSignificantEdge(t.edge(), dflt, recompute, vertexInfo)) {
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
                    Function::Ptr function = t.vertex()->value().function();
                    if (function)
                        SAWYER_MESG(debug) <<"[" <<depth <<"]     block is owned by " <<function->printableName() <<"\n";
                    bool functionListed = false;        // function's blacklisted or whitelisted value if listed
                    if (function && function->address()==t.vertex()->value().address() &&
                        mayReturnList_.getOptional(function->name()).assignTo(functionListed)) {
                        SAWYER_MESG(debug) <<"[" <<depth <<"]     " <<function->printableName()
                                           <<" is " <<(functionListed?"whitelisted":"blacklisted") <<"\n";
                        if (BasicBlock::Ptr bb = t.vertex()->value().bblock())
                            bb->mayReturn() = functionListed;
                        vertexInfo[t.vertex()->id()].result = functionListed;
                        t.skipChildren();
                    } else if (BasicBlock::Ptr bb = t.vertex()->value().bblock()) {
                        if (recompute)
                            bb->mayReturn().clear();
                        if (bb->mayReturn().isCached()) {
                            bool b = bb->mayReturn().get();
                            SAWYER_MESG(debug) <<"[" <<depth <<"]     already cached: may-return is " <<(b?"yes":"no") <<"\n";
                            vertexInfo[t.vertex()->id()].result = b;
                            t.skipChildren();
                        } else if (basicBlockIsFunctionReturn(bb)) {
                            SAWYER_MESG(debug) <<"[" <<depth <<"]     block is a function return; may-return is yes\n";
                            bb->mayReturn() = true;
                            vertexInfo[t.vertex()->id()].result = true;
                            t.skipChildren();
                        } else if (basicBlockIsFunctionCall(bb)) {
                            SAWYER_MESG(debug) <<"[" <<depth <<"]     block is a function call; process callees...\n";
                            boost::logic::tribool tb = mayReturnDoesCalleeReturn(t.vertex(), dflt, recompute, vertexInfo);
                            SAWYER_MESG(debug) <<"[" <<depth <<"]     callees for " <<vertexName(t.vertex()) <<" processed"
                                               <<"; " <<(tb || boost::logic::indeterminate(tb)?"at least one":"none")
                                               <<" may return\n";
                        } else {
                            SAWYER_MESG(debug) <<"[" <<depth <<"]     must process vertex successors first...\n";
                        }
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
                        boost::logic::tribool tb = mayReturnDoesSuccessorReturn(t.vertex(), dflt, recompute, vertexInfo);
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

} // namespace
} // namespace
} // namespace
