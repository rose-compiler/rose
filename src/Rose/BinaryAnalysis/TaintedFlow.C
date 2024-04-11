#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/TaintedFlow.h>

#include <stringify.h>                                  // rose

#include <sstream>

namespace Rose {
namespace BinaryAnalysis {

Sawyer::Message::Facility TaintedFlow::mlog;

void
TaintedFlow::initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Rose::Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::TaintedFlow");
        mlog.comment("analyzing based on tainted-flow");
    }
}

TaintedFlow::Taintedness
TaintedFlow::merge(Taintedness a, Taintedness b) {
    if (a==b) {
        return a;
    } else if (TOP==a || TOP==b) {
        return TOP;
    } else if (BOTTOM==a) {
        return b;
    } else if (BOTTOM==b) {
        return a;
    } else {
        ASSERT_require((TAINTED==a && NOT_TAINTED==b) || (NOT_TAINTED==a && TAINTED==b));
        return TOP;
    }
}

TaintedFlow::Taintedness &
TaintedFlow::State::lookup(const DataFlow::Variable &variable) {
    for (VarTaintList::value_type &node: taints_) {
        if (node.first.mustAlias(variable))
            return node.second;
    }
    throw std::runtime_error("variable not found");
}

bool
TaintedFlow::State::setIfExists(const DataFlow::Variable &variable, Taintedness taint) {
    for (VarTaintList::value_type &node: taints_) {
        if (node.first.mustAlias(variable)) {
            node.second = taint;
            return true;
        }
    }
    return false;
}

bool
TaintedFlow::State::merge(const State::Ptr &other) {
    bool changed = false;
    for (const VarTaintList::value_type &otherNode: other->taints_) {
        Taintedness otherTaint = otherNode.second;
        Taintedness &myTaint = lookup(otherNode.first);
        Taintedness newTaint = TaintedFlow::merge(myTaint, otherTaint);
        if (myTaint != newTaint)
            changed = true;
        myTaint = newTaint;
    }
    return changed;
}

void
TaintedFlow::State::print(std::ostream &out) const {
    for (const VarTaintList::value_type &node: taints_) {
        DataFlow::Variable var = node.first;
        Taintedness taint = node.second;
        switch (taint) {
            case BOTTOM:      out <<"  bottom   "; break;
            case NOT_TAINTED: out <<"  no-taint "; break;
            case TAINTED:     out <<"  tainted  "; break;
            case TOP:         out <<"  top      "; break;
        }
        out <<var <<"\n";
    }
}

TaintedFlow::State::Ptr
TaintedFlow::TransferFunction::operator()(size_t cfgVertex, const State::Ptr &in) {
    using namespace Diagnostics;

    const DataFlow::Graph &dfg = index_[cfgVertex]; // data flow for this basic block
    State::Ptr out = in->copy();

    Stringifier taintednessStr(stringifyBinaryAnalysisTaintedFlowTaintedness);
    Stringifier edgeTypeStr(stringifyBinaryAnalysisInstructionSemanticsDataFlowSemanticsDataFlowEdgeEdgeType);

    mlog[TRACE] <<"transfer function for CFG vertex " <<cfgVertex <<"\n";

    for (size_t edgeId=0; edgeId<dfg.nEdges(); ++edgeId) {
        // We're taking a shortcut here and assuming that data flow edge sequence number == edge ID. This will be true
        // since we inserted the edges in the order of their sequence numbers, but only if we haven't erased any edges
        // since then.
        const DataFlow::Graph::Edge &edge = *dfg.findEdge(edgeId);
        ASSERT_require(edge.id()==edge.value().sequence);

        const DataFlow::Variable &srcVariable = edge.source()->value();
        Taintedness srcTaint = out->lookup(srcVariable);

        if (mlog[DEBUG]) {
            mlog[DEBUG] <<"  xfer: flow from " <<srcVariable <<" (" <<taintednessStr(srcTaint) <<")\n";
            mlog[DEBUG] <<"  xfer: flow to   " <<edge.target()->value()
                        <<" (" <<taintednessStr(out->lookup(edge.target()->value())) <<")\n";
        }

        switch (approximation_) {
            case UNDER_APPROXIMATE: {
                Taintedness &dstTaint = out->lookup(edge.target()->value());
                if (edge.value().edgeType == DataFlow::Graph::EdgeValue::CLOBBER) {
                    dstTaint = srcTaint;
                } else {
                    dstTaint = merge(dstTaint, srcTaint);
                }
                if (mlog[DEBUG])
                    mlog[DEBUG] <<"  xfer:   " <<edgeTypeStr(edge.value().edgeType) <<" to " <<taintednessStr(dstTaint) <<"\n";
                break;
            }

            case OVER_APPROXIMATE: {
                State::Ptr tmp = out->copy();
                for (VariableTaint &varTaint: tmp->variables()) {
                    DataFlow::Variable &dstVariable = varTaint.first;
                    Taintedness &dstTaint = varTaint.second;
                    if (dstVariable.mustAlias(edge.target()->value(), smtSolver_)) {
                        if (edge.value().edgeType == DataFlow::Graph::EdgeValue::CLOBBER) {
                            dstTaint = srcTaint;
                        } else {
                            dstTaint = merge(dstTaint, srcTaint);
                        }
                        if (mlog[DEBUG]) {
                            mlog[DEBUG] <<"  xfer:   mustAlias " <<dstVariable <<"\n"
                                        <<"  xfer:   " <<edgeTypeStr(edge.value().edgeType)
                                        <<" to " <<taintednessStr(dstTaint) <<"\n";
                        }
                    } else if (dstVariable.mayAlias(edge.target()->value(), smtSolver_)) {
                        dstTaint = merge(dstTaint, srcTaint);
                        if (mlog[DEBUG]) {
                            mlog[DEBUG] <<"  xfer:   mayAlias " <<dstVariable <<"\n"
                                        <<"  xfer:   AUGMENT to " <<taintednessStr(dstTaint) <<"\n";
                        }
                    }
                }
                out = tmp;
                break;
            }
        }
    }
    if (mlog[DEBUG])
        mlog[DEBUG] <<"state after transfer function:\n" <<*out;
    return out;
}

std::string
TaintedFlow::TransferFunction::toString(const State::Ptr &state) {
    if (!state)
        return "null state";
    std::ostringstream ss;
    ss <<*state;
    return ss.str();
}

std::ostream &
operator<<(std::ostream &out, const TaintedFlow::State &state) {
    state.print(out);
    return out;
}

} // namespace
} // namespace

#endif
