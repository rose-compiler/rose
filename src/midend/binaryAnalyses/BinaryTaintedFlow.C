#include "BinaryTaintedFlow.h"

namespace BinaryAnalysis {

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
    BOOST_FOREACH (VarTaintList::value_type &node, taints_) {
        if (node.first.mustAlias(variable))
            return node.second;
    }
    throw std::runtime_error("variable not found");
}

bool
TaintedFlow::State::setIfExists(const DataFlow::Variable &variable, Taintedness taint) {
    BOOST_FOREACH (VarTaintList::value_type &node, taints_) {
        if (node.first.mustAlias(variable)) {
            node.second = taint;
            return true;
        }
    }
    return false;
}

bool
TaintedFlow::State::merge(const StatePtr &other) {
    bool changed = false;
    BOOST_FOREACH (const VarTaintList::value_type &otherNode, other->taints_) {
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
    BOOST_FOREACH (const VarTaintList::value_type &node, taints_) {
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

TaintedFlow::StatePtr
TaintedFlow::TransferFunction::operator()(size_t cfgVertex, const StatePtr &in) {
    const DataFlow::Graph &dfg = index_[cfgVertex]; // data flow for this basic block
    StatePtr out = in->copy();

    for (size_t edgeId=0; edgeId<dfg.nEdges(); ++edgeId) {
        // We're taking a shortcut here and assuming that data flow edge sequence number == edge ID. This will be true
        // since we inserted the edges in the order of their sequence numbers, but only if we haven't erased any edges
        // since then.
        const DataFlow::Graph::EdgeNode &edge = *dfg.findEdge(edgeId);
        ASSERT_require(edge.id()==edge.value().sequence);

        Taintedness srcTaint = out->lookup(edge.source()->value());
        Taintedness &dstTaint = out->lookup(edge.target()->value());
        if (edge.value().edgeType == DataFlow::Graph::EdgeValue::CLOBBER) {
            dstTaint = srcTaint;
        } else {
            dstTaint = merge(dstTaint, srcTaint);
        }
    }
    return out;
}

std::ostream &
operator<<(std::ostream &out, const TaintedFlow::State &state) {
    state.print(out);
    return out;
}

} // namespace
