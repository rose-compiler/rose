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

size_t
GraphViz::dumpVertex(std::ostream &out, const Partitioner &partitioner,
                     const ControlFlowGraph::ConstVertexNodeIterator &vertex) const {
    size_t id = -1;
    if (!vmap_.getOptional(vertex).assignTo(id) && isSelected(partitioner, vertex)) {
        id = vmap_.size();
        out <<id <<" [ label=\"" <<labelEscape(vertexLabel(partitioner, vertex)) <<"\" "
            <<vertexAttributes(partitioner, vertex) <<" ];\n";
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
            s = "call-ret";
            break;
        case E_NORMAL:
            break;
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
        size_t targetId = vmap_.size();
        vmap_.insert(edge->target(), targetId);
        out <<targetId <<" [ label=\"" <<labelEscape(vertexLabelSimple(partitioner, edge->target())) <<"\" "
            <<vertexAttributes(partitioner, edge->target()) <<" ];\n";
    } else if (!selected_.empty() && isSelected(partitioner, edge->source()) &&!isSelected(partitioner, edge->target()) &&
        !vmap_.exists(edge->target())) {
        // Neighbor of selected vertex
        size_t targetId = vmap_.size();
        vmap_.insert(edge->target(), targetId);
        out <<targetId <<" [ label=\"" <<labelEscape(vertexLabelSimple(partitioner, edge->target())) <<"\" ];\n"; // noattrs
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

// Edges that wouldn't be emitted by dumpIntraFunction
void
GraphViz::dumpInterFunctionEdges(std::ostream &out, const Partitioner &partitioner, const Function::Ptr &function) const {
    for (ControlFlowGraph::ConstEdgeNodeIterator edge=partitioner.cfg().edges().begin();
         edge!=partitioner.cfg().edges().end(); ++edge) {
        if (function && (edge->source()->value().type() != V_BASIC_BLOCK || edge->source()->value().function() != function)) {
            continue;                                   // not originating from desired function
        } else if (edge->target()->value().type() != V_BASIC_BLOCK) {
            dumpEdge(out, partitioner, edge);           // special edge
        } else if (edge->source()->value().function() != edge->target()->value().function()) {
            dumpEdge(out, partitioner, edge);           // inter-function edge
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

size_t
GraphViz::dumpFunctionInfo(std::ostream &out, const Partitioner &partitioner,
                           const ControlFlowGraph::ConstVertexNodeIterator &vertex) const {
    ASSERT_require(vertex != partitioner.cfg().vertices().end());
    size_t id = 0;
    if (!vmap_.getOptional(vertex).assignTo(id)) {
        id = vmap_.size();

        Function::Ptr function;
        if (vertex->value().type() == V_BASIC_BLOCK && (function = vertex->value().function())) {
            out <<id <<" [ label=\"" <<labelEscape(functionLabel(partitioner, function)) <<"\" "
                <<functionAttributes(partitioner, function) <<" ];\n";
        } else {
            out <<id <<" [ label=\"" <<labelEscape(vertexLabel(partitioner, vertex)) <<"\" "
                <<vertexAttributes(partitioner, vertex) <<" ];\n";
        }
        vmap_.insert(vertex, id);
    }
    return id;
}
    
void
GraphViz::dumpFunctionCallees(std::ostream &out, const Partitioner &partitioner, const Function::Ptr &function) const {
    ASSERT_not_null(function);

    ControlFlowGraph::ConstVertexNodeIterator start = partitioner.findPlaceholder(function->address());
    if (start == partitioner.cfg().vertices().end())
        return;

    typedef DepthFirstForwardEdgeTraversal<const ControlFlowGraph> Traversal;
    for (Traversal t(partitioner.cfg(), start); t; ++t) {
        if (t.edge()->value().type() == E_FUNCTION_CALL || t.edge()->value().type() == E_FUNCTION_XFER) {
            t.skipChildren();
            dumpFunctionInfo(out, partitioner, t.edge()->target());
        }
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
        dumpInterFunctionEdges(out, partitioner, Function::Ptr());
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
    dumpFunctionCallees(out, partitioner, function);
    dumpIntraFunction(out, partitioner, function);
    dumpInterFunctionEdges(out, partitioner, function);
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
