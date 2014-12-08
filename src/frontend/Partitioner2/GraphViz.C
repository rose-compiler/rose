#include <sage3basic.h>

#include <AsmUnparser_compat.h>
#include <Diagnostics.h>
#include <Partitioner2/GraphViz.h>
#include <Partitioner2/Partitioner.h>
#include <sawyer/GraphTraversal.h>

using namespace rose::Diagnostics;
using namespace Sawyer::Container::Algorithm;

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

// Borrowed with permission from Robb's Beenav navigation software [Robb P. Matzke 2014-12-02]
GraphViz::HsvColor::HsvColor(const RgbColor &rgb) {
    double hi = std::max(std::max(rgb.r, rgb.g), rgb.b);
    double lo = std::min(std::min(rgb.r, rgb.g), rgb.b);
    v = (hi+lo)/2.0;
    if (hi==lo) {
        h = s = 0.0;
    } else {
        double delta = hi-lo;
        s = v > 0.5 ? delta / (2.0-hi-lo) : delta / (hi+lo);
        if (hi==rgb.r) {
            h = (rgb.g-rgb.b)/delta + (rgb.g<rgb.b ? 6 : 0);
        } else if (hi==rgb.g) {
            h = (rgb.b-rgb.r)/delta + 2;
        } else {
            h = (rgb.r-rgb.g)/delta + 4;
        }
        h /= 6.0;
    }
}

// Borrowed with permission from Robb's Beenav navigation software [Robb P. Matzke 2014-12-02]
static double
rgb_from_hue(double p, double q, double t)
{
    if (t<0)
        t += 1.0;
    if (t>1)
        t -= 1.0;
    if (t < 1/6.0)
        return p + (q-p) * 6 * t;
    if (t < 0.5)
        return q;
    if (t < 2/3.0)
        return p + (q-p) * (2/3.0 - t) * 6;
    return p;
}

// Borrowed with permission from Robb's Beenav navigation software [Robb P. Matzke 2014-12-02]
GraphViz::RgbColor::RgbColor(const HsvColor &hsv) {
    if (0.0==hsv.v) {
        r = g = b = 0;
    } else {
        double q = hsv.v < 0.5 ? hsv.v * (1+hsv.s) : hsv.v + hsv.s - hsv.v*hsv.s;
        double p = 2.0 * hsv.v - q;
        r = rgb_from_hue(p, q, hsv.h+1/3.0);
        g = rgb_from_hue(p, q, hsv.h);
        b = rgb_from_hue(p, q, hsv.h-1/3.0);
    }
}

GraphViz::HsvColor
GraphViz::HsvColor::invert() const {
    return HsvColor(h, s, (1-v));
}

GraphViz::RgbColor
GraphViz::RgbColor::invert() const {
    return HsvColor(*this).invert();
}

static double limitColor(double x) {
    return std::max(0.0, std::min(x, 1.0));
}

std::string
GraphViz::RgbColor::toString() const {
    std::stringstream ss;
    mfprintf(ss)("#%02x%02x%02x",
                 (unsigned)round(limitColor(r)*255),
                 (unsigned)round(limitColor(g)*255),
                 (unsigned)round(limitColor(b)*255));
    return ss.str();
}

std::string
GraphViz::HsvColor::toString() const {
    return RgbColor(*this).toString();
}

// Make an edge color from a background color. Backgrounds tend to be too light for edges, and inverting the color would be too
// dark to be distinguishable from black on such a fine line.
static GraphViz::RgbColor
makeEdgeColor(const GraphViz::RgbColor &bg) {
    GraphViz::HsvColor hsv = bg;
    hsv.s = 1.0;
    hsv.v = 0.5;
    return hsv;
}

std::string
GraphViz::labelEscape(const std::string &s) {
    std::string retval;
    for (size_t i=0; i<s.size(); ++i) {
        if ('\n'==s[i]) {
            retval += "\\n";
        } else if ('"'==s[i]) {
            retval += "\\\"";
        } else if ('\\'==s[i] && i+1<s.size() && strchr("\\0abtnvfr", s[i+1])) {
            retval += "\\";                             // seems to already be escaped
        } else if ('\\'==s[i]) {
            retval += "\\\\";
        } else {
            retval += s[i];
        }
    }
    return retval;
}

bool
GraphViz::isSelected(const Partitioner &partitioner, const ControlFlowGraph::ConstVertexNodeIterator &vertex) const {
    if (vertex == partitioner.cfg().vertices().end())
        return false;
    if (selected_.empty())
        return true;
    return vertex->id() < selected_.size() && selected_[vertex->id()];
}

void
GraphViz::select(const Partitioner &partitioner, const ControlFlowGraph::ConstVertexNodeIterator &vertex, bool b) {
    ASSERT_require(vertex != partitioner.cfg().vertices().end());
    if (vertex->id() >= selected_.size())
        selected_.resize(vertex->id()+1, false);
    selected_[vertex->id()] = b;
}

std::string
GraphViz::vertexLabel(const Partitioner &partitioner, const ControlFlowGraph::ConstVertexNodeIterator &vertex) const {
    ASSERT_require(vertex != partitioner.cfg().vertices().end());
    BasicBlock::Ptr bb;
    if (showInstructions_ && vertex->value().type() == V_BASIC_BLOCK && (bb = vertex->value().bblock())) {
        std::string s;
        BOOST_FOREACH (SgAsmInstruction *insn, vertex->value().bblock()->instructions())
            s += (s.empty()?"":"\n") + unparseInstructionWithAddress(insn);
        return StringUtility::cEscape(s);
    }
    return vertexLabelSimple(partitioner, vertex);
}

std::string
GraphViz::vertexLabelSimple(const Partitioner &partitioner, const ControlFlowGraph::ConstVertexNodeIterator &vertex) const {
    ASSERT_require(vertex != partitioner.cfg().vertices().end());
    switch (vertex->value().type()) {
        case V_BASIC_BLOCK:
            if (vertex->value().function() && vertex->value().function()->address() == vertex->value().address()) {
                return vertex->value().function()->printableName();
            } else if (BasicBlock::Ptr bb = vertex->value().bblock()) {
                return bb->printableName();
            } else {
                return StringUtility::addrToString(vertex->value().address());
            }
        case V_NONEXISTING:
            return "nonexisting";
        case V_UNDISCOVERED:
            return "undiscovered";
        case V_INDETERMINATE:
            return "indeterminate";
    }
}

std::string
GraphViz::vertexAttributes(const Partitioner &partitioner, const ControlFlowGraph::ConstVertexNodeIterator &vertex) const {
    ASSERT_require(vertex != partitioner.cfg().vertices().end());
    std::map<std::string, std::string> attr;
    attr["shape"] = "box";

    if (vertex->value().type() == V_BASIC_BLOCK) {
        attr["labeljust"] = "l";                        // doesn't seem to align left [Robb P. Matzke 2014-12-01]

        if (vertex->value().function() && vertex->value().function()->address() == vertex->value().address()) {
            attr["style"] = "filled";
            attr["fillcolor"] = "\"" + funcEnterColor_.toString() + "\"";
        } else if (BasicBlock::Ptr bb = vertex->value().bblock()) {
            if (partitioner.basicBlockIsFunctionReturn(bb)) {
                attr["style"] = "filled";
                attr["fillcolor"] = "\"" + funcReturnColor_.toString() + "\"";
            }
        }
    } else {
        attr["style"] = "filled";
        attr["fillcolor"] = "\"" + warningColor_.toString() + "\"";
    }

    std::string s;
    for (std::map<std::string, std::string>::iterator ai=attr.begin(); ai!=attr.end(); ++ai)
        s += (s.empty() ? "" : " ") + ai->first + "=" + ai->second;
    return s;
}

// dump vertex only if it isn't selected and hasn't been already dumped
size_t
GraphViz::dumpVertex(std::ostream &out, const Partitioner &partitioner,
                     const ControlFlowGraph::ConstVertexNodeIterator &vertex) const {
    size_t id = NO_ID;
    if (!vmap_.getOptional(vertex).assignTo(id) && isSelected(partitioner, vertex)) {
        id = vmap_.size();
        out <<id <<" [ label=\"" <<labelEscape(vertexLabel(partitioner, vertex)) <<"\" ";
        if (vertex->value().type() == V_BASIC_BLOCK)
            out <<"href=\"" <<StringUtility::addrToString(vertex->value().address()) <<"\" ";
        out <<vertexAttributes(partitioner, vertex) <<" ];\n";
        vmap_.insert(vertex, id);
    }
    return id;
}

// dump vertex regardless of whether it's selected (but not if it was already dumped)
size_t
GraphViz::dumpVertexInfo(std::ostream &out, const Partitioner &partitioner,
                         const ControlFlowGraph::ConstVertexNodeIterator &vertex) const {
    size_t id = NO_ID;
    if (!vmap_.getOptional(vertex).assignTo(id)) {
        id = vmap_.size();
        out <<id <<" [ label=\"" <<labelEscape(vertexLabelSimple(partitioner, vertex)) <<"\" ";
        if (vertex->value().type() == V_BASIC_BLOCK)
            out <<"href=\"" <<StringUtility::addrToString(vertex->value().address()) <<"\" ";
        out <<vertexAttributes(partitioner, vertex) <<" ];\n";
        vmap_.insert(vertex, id);
    }
    return id;
}

std::string
GraphViz::edgeLabel(const Partitioner &partitioner, const ControlFlowGraph::ConstEdgeNodeIterator &edge) const {
    ASSERT_require(edge != partitioner.cfg().edges().end());
    std::string s;
    switch (edge->value().type()) {
        case E_FUNCTION_CALL:
            s = "call";
            break;
        case E_FUNCTION_XFER:
            s = "xfer";
            break;
        case E_FUNCTION_RETURN:
            s = "return";
            break;
        case E_CALL_RETURN:
            s = "cret";
            if (edge->value().confidence() == ASSUMED)
                s += "\nassumed";
            break;
        case E_NORMAL: {
            // Normal edges don't get labels unless its intra-function, otherwise the graphs would be too noisy.
            if (edge->source()->value().type() == V_BASIC_BLOCK && edge->target()->value().type() == V_BASIC_BLOCK &&
                edge->source()->value().function() != edge->target()->value().function())
                s = "other";
            break;
        }
    }
    return s;
}

std::string
GraphViz::edgeAttributes(const Partitioner &partitioner, const ControlFlowGraph::ConstEdgeNodeIterator &edge) const {
    ASSERT_require(edge != partitioner.cfg().edges().end());
    std::map<std::string, std::string> attr;

    if (edge->value().type() == E_FUNCTION_RETURN) {
        attr["color"] = "\"" + makeEdgeColor(funcReturnColor_).toString() + "\"";
    } else if (edge->target() == partitioner.indeterminateVertex()) {
        attr["color"] = "\"" + makeEdgeColor(warningColor_).toString() + "\"";
    } else if (edge->value().type() == E_FUNCTION_CALL) {
        attr["color"] = "\"" + makeEdgeColor(funcEnterColor_).toString() + "\"";
    } else if (edge->source()->value().type() == V_BASIC_BLOCK && edge->target()->value().type() == V_BASIC_BLOCK &&
               edge->source()->value().bblock() &&
               edge->source()->value().bblock()->fallthroughVa() != edge->target()->value().address()) {
        attr["style"] = "dotted";                       // non-fallthrough edges
    }
    
    std::string s;
    for (std::map<std::string, std::string>::iterator ai=attr.begin(); ai!=attr.end(); ++ai)
        s += (s.empty() ? "" : " ") + ai->first + "=" + ai->second;
    return s;
}

bool
GraphViz::dumpEdge(std::ostream &out, const Partitioner &partitioner,
                   const ControlFlowGraph::ConstEdgeNodeIterator &edge) const {
    size_t sourceId=0, targetId=0;
    if (!showReturnEdges_ && edge->value().type() == E_FUNCTION_RETURN)
        return false;

    // Emit the target vertex if desired and if it wouldn't normally have been emitted.
    if (edge->target()->value().type()!=V_BASIC_BLOCK && vmap_.exists(edge->source()) && !vmap_.exists(edge->target())) {
        // Special vertex (indeterminate, undiscovered, etc)
        dumpVertexInfo(out, partitioner, edge->target());
    } else if (!selected_.empty() && isSelected(partitioner, edge->source()) && !isSelected(partitioner, edge->target()) &&
        !vmap_.exists(edge->target())) {
        // Outgoing neighbor of selected vertex
        dumpVertexInfo(out, partitioner, edge->target());
    } else if (!selected_.empty() && !isSelected(partitioner, edge->source()) && isSelected(partitioner, edge->target()) &&
        !vmap_.exists(edge->source())) {
        // Incoming neighbor of selected vertex
        dumpVertexInfo(out, partitioner, edge->source());
    }
    
    if (vmap_.getOptional(edge->source()).assignTo(sourceId) && vmap_.getOptional(edge->target()).assignTo(targetId)) {
        out <<sourceId <<" -> " <<targetId <<" [ label=\"" <<labelEscape(edgeLabel(partitioner, edge)) <<"\" "
            <<edgeAttributes(partitioner, edge) <<" ];\n";
        return true;
    }
    return false;
}

// Edges between any two basic blocks in the same function
void
GraphViz::dumpIntraFunction(std::ostream &out, const Partitioner &partitioner, const Function::Ptr &function) const {
    ASSERT_not_null(function);

    ControlFlowGraph::ConstVertexNodeIterator start = partitioner.findPlaceholder(function->address());
    if (start == partitioner.cfg().vertices().end())
        return;
    dumpVertex(out, partitioner, start);

    typedef DepthFirstForwardEdgeTraversal<const ControlFlowGraph> Traversal;
    for (Traversal t(partitioner.cfg(), start); t; ++t) {
        ControlFlowGraph::ConstVertexNodeIterator target = t.edge()->target();
        if (target->value().type() == V_BASIC_BLOCK && target->value().function() == function) {
            dumpVertex(out, partitioner, target);
            dumpEdge(out, partitioner, t.edge());
        }
    }
}

// Outgoing edges that wouldn't be emitted by dumpIntraFunction.
void
GraphViz::dumpInterFunctionOutEdges(std::ostream &out, const Partitioner &partitioner, const Function::Ptr &function) const {
    for (ControlFlowGraph::ConstEdgeNodeIterator edge=partitioner.cfg().edges().begin();
         edge!=partitioner.cfg().edges().end(); ++edge) {

        if ((edge->source()->value().type() == V_BASIC_BLOCK && edge->source()->value().function() == function) &&
            (edge->target()->value().type() != V_BASIC_BLOCK || edge->target()->value().function() != function)) {
            dumpEdge(out, partitioner, edge);
        }
    }
}

std::string
GraphViz::functionLabel(const Partitioner &partitioner, const Function::Ptr &function) const {
    ASSERT_not_null(function);
    return function->printableName();
}

std::string
GraphViz::functionAttributes(const Partitioner &partitioner, const Function::Ptr &function) const {
    ASSERT_not_null(function);
    return "style=filled fillcolor=\"" + subgraphColor().toString() + "\"";
}

// Dump function entry vertex regardless of whether it's selected (but not if already dumped)
size_t
GraphViz::dumpFunctionInfo(std::ostream &out, const Partitioner &partitioner,
                           const ControlFlowGraph::ConstVertexNodeIterator &vertex) const {
    ASSERT_require(vertex != partitioner.cfg().vertices().end());
    size_t id = NO_ID;
    if (!vmap_.getOptional(vertex).assignTo(id)) {
        id = vmap_.size();

        Function::Ptr function;
        if (vertex->value().type() == V_BASIC_BLOCK && (function = vertex->value().function()) &&
            function->address() == vertex->value().address()) {
            out <<id <<" [ label=\"" <<labelEscape(functionLabel(partitioner, function)) <<"\" "
                <<"href=\"" <<StringUtility::addrToString(function->address()) <<"\" "
                <<functionAttributes(partitioner, function) <<" ];\n";
        } else {
            out <<id <<" [ label=\"" <<labelEscape(vertexLabelSimple(partitioner, vertex)) <<"\" ";
            if (vertex->value().type() == V_BASIC_BLOCK)
                out <<"href=\"" <<StringUtility::addrToString(vertex->value().address()) <<"\" ";
            out <<vertexAttributes(partitioner, vertex) <<" ];\n";
        }
        vmap_.insert(vertex, id);
    }
    return id;
}
    
void
GraphViz::dumpFunctionCallees(std::ostream &out, const Partitioner &partitioner, const Function::Ptr &function) const {
    ASSERT_not_null(function);
    using namespace Sawyer::Container::Algorithm;

    ControlFlowGraph::ConstVertexNodeIterator start = partitioner.findPlaceholder(function->address());
    if (start == partitioner.cfg().vertices().end())
        return;

    typedef DepthFirstForwardGraphTraversal<const ControlFlowGraph> Traversal;
    for (Traversal t(partitioner.cfg(), start, ENTER_EVENTS); t; ++t) {
        if (t.event() == ENTER_VERTEX) {
            if (t.vertex()->value().type() != V_BASIC_BLOCK) {
                t.skipChildren();
            } else if (t.vertex()->value().function() != function) {
                dumpFunctionInfo(out, partitioner, t.vertex());
                t.skipChildren();
            }
        } else {
            ASSERT_require(t.event() == ENTER_EDGE);
            if ((t.edge()->value().type() == E_FUNCTION_CALL || t.edge()->value().type() == E_FUNCTION_XFER) &&
                t.edge()->target()->value().type() == V_BASIC_BLOCK && t.edge()->target()->value().function() != function) {
                dumpFunctionInfo(out, partitioner, t.edge()->target()); // non-recursive function call
                t.skipChildren();
            }
        }
    }
}

struct CallInfo {
    size_t nCalls;                                      // number of E_FUNCTION_CALL edges
    size_t nTransfers;                                  // number of E_FUNCTION_XFER edges
    size_t nOthers;                                     // number of edges with other labels
    CallInfo(): nCalls(0), nTransfers(0), nOthers(0) {}
};
    
void
GraphViz::dumpFunctionCallers(std::ostream &out, const Partitioner &partitioner, const Function::Ptr &callee) const {
    ASSERT_not_null(callee);

    ControlFlowGraph::ConstVertexNodeIterator calleeVertex = partitioner.findPlaceholder(callee->address());
    if (calleeVertex == partitioner.cfg().vertices().end())
        return;
    size_t calleeId = dumpVertex(out, partitioner, calleeVertex);
    if (calleeId == NO_ID)
        return;

    typedef Sawyer::Container::Map<size_t /*callerId*/, CallInfo> Calls;
    Calls calls;
    for (ControlFlowGraph::ConstEdgeNodeIterator edge = calleeVertex->inEdges().begin();
         edge!=calleeVertex->inEdges().end(); ++edge) {
        ASSERT_require(edge->source()->value().type() == V_BASIC_BLOCK);

        // Where is the call coming from? Use the call site's function entry if possible, otherwise the call site basic block.
        Function::Ptr caller;
        size_t callerId = NO_ID;
        if ((caller = edge->source()->value().function()) != callee) {
            if (caller) {
                ControlFlowGraph::ConstVertexNodeIterator callerVertex = partitioner.findPlaceholder(caller->address());
                ASSERT_require(callerVertex != partitioner.cfg().vertices().end());
                callerId = dumpFunctionInfo(out, partitioner, callerVertex);
            } else {
                // call is not coming from a known function; show the call site instead
                callerId = dumpVertexInfo(out, partitioner, edge->source());
            }
        }

        // Omit calls that are recursive since they'll be handled as intra-function edges
        if (callerId != NO_ID && caller != callee) {
            CallInfo &info = calls.insertMaybeDefault(callerId);
            switch (edge->value().type()) {
                case E_FUNCTION_CALL:
                    ++info.nCalls;
                    break;
                case E_FUNCTION_XFER:
                    ++info.nTransfers;
                    break;
                default:
                    ++info.nOthers;
                    break;
            }
        }
    }

    // Emit edges
    BOOST_FOREACH (const Calls::Node &call, calls.nodes()) {
        size_t callerId = call.key();
        const CallInfo &info = call.value();
        std::string label;
        if (info.nCalls)
            label += StringUtility::plural(info.nCalls, "calls");
        if (info.nTransfers)
            label += (label.empty()?"":"\n") + StringUtility::plural(info.nTransfers, "xfers");
        if (info.nOthers)
            label += (label.empty()?"":"\n") + StringUtility::plural(info.nOthers, "others");
        out <<callerId <<" -> " <<calleeId <<" [ label=\"" <<label <<"\" ];\n";
    }
}

void
GraphViz::dumpCfgAll(std::ostream &out, const Partitioner &partitioner) const {
    vmap_.clear();
    out <<"digraph CFG {\n";
    if (useFunctionSubgraphs_) {
        BOOST_FOREACH (const Function::Ptr &function, partitioner.functions()) {
            if (isSelected(partitioner, partitioner.findPlaceholder(function->address()))) {
                mfprintf(out)("\nsubgraph cluster_F%"PRIx64" {", function->address());
                out <<" label=\"" <<labelEscape(functionLabel(partitioner, function)) <<"\" "
                    <<functionAttributes(partitioner, function) <<";\n";
                dumpIntraFunction(out, partitioner, function);
                out <<"}\n";
            }
        }
        dumpInterFunctionOutEdges(out, partitioner, Function::Ptr());
    } else {
        for (ControlFlowGraph::ConstVertexNodeIterator vertex = partitioner.cfg().vertices().begin();
             vertex != partitioner.cfg().vertices().end(); ++vertex)
            dumpVertex(out, partitioner, vertex);
        for (ControlFlowGraph::ConstEdgeNodeIterator edge = partitioner.cfg().edges().begin();
             edge != partitioner.cfg().edges().end(); ++edge)
            dumpEdge(out, partitioner, edge);
    }
    out <<"}\n";
}

void
GraphViz::dumpCfgFunction(std::ostream &out, const Partitioner &partitioner, const Function::Ptr &function) const {
    ASSERT_not_null(function);
    vmap_.clear();
    out <<"digraph CFG {\n";
    out <<"# Function callees...\n";
    dumpFunctionCallees(out, partitioner, function);
    out <<"# Intra function nodes and edges...\n";
    dumpIntraFunction(out, partitioner, function);
    out <<"# Function callers nodes and edges...\n";
    dumpFunctionCallers(out, partitioner, function);
    out <<"# Outgoing inter-function edges...\n";
    dumpInterFunctionOutEdges(out, partitioner, function);
    out <<"}\n";
}

void
GraphViz::dumpCfgInterval(std::ostream &out, const Partitioner &partitioner, const AddressInterval &interval) {
    selected_.clear();
    selected_.resize(partitioner.cfg().nVertices(), false);
    BOOST_FOREACH (const ControlFlowGraph::VertexNode &vertex, partitioner.cfg().vertices())
        selected_[vertex.id()] = vertex.value().type() == V_BASIC_BLOCK && interval.isContaining(vertex.value().address());
    dumpCfgAll(out, partitioner);
}

void
GraphViz::dumpCallGraph(std::ostream &out, const Partitioner &partitioner) const {
    typedef FunctionCallGraph::Graph CG;
    FunctionCallGraph cg = partitioner.functionCallGraph(false); // parallel edges are compressed
    out <<"digraph CG {\n";

    BOOST_FOREACH (const CG::VertexNode &vertex, cg.graph().vertices()) {
        const Function::Ptr &function = vertex.value();
        out <<vertex.id() <<" [ label=\"" <<labelEscape(functionLabel(partitioner, function)) <<"\" "
            <<"href=\"" <<StringUtility::addrToString(function->address()) <<"\" "
            <<functionAttributes(partitioner, function) <<" ]\n";
    }

    BOOST_FOREACH (const CG::EdgeNode &edge, cg.graph().edges()) {
        std::string label;
        switch (edge.value().type()) {
            case E_FUNCTION_CALL: label = "calls";  break;
            case E_FUNCTION_XFER: label = "xfers";  break;
            default:              label = "others"; break;
        }
        label = StringUtility::plural(edge.value().count(), label);
        out <<edge.source()->id() <<" -> " <<edge.target()->id() <<" [ label=\"" <<label <<"\" ];\n";
    }
    
    out <<"}\n";
}

} // namespace
} // namespace
} // namespace
