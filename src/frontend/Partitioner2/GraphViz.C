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

std::string
GraphViz::RgbColor::toString() const {
    std::stringstream ss;
    mfprintf(ss)("#%02x%02x%02x", (unsigned)round(r*255), (unsigned)round(g*255), (unsigned)round(b*255));
    return ss.str();
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
GraphViz::vertexLabel(const Partitioner &partitioner, const ControlFlowGraph::ConstVertexNodeIterator &vertex) {
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
GraphViz::vertexLabelSimple(const Partitioner &partitioner, const ControlFlowGraph::ConstVertexNodeIterator &vertex) {
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
GraphViz::vertexAttributes(const Partitioner &partitioner, const ControlFlowGraph::ConstVertexNodeIterator &vertex) {
    ASSERT_require(vertex != partitioner.cfg().vertices().end());
    std::map<std::string, std::string> attr;
    attr["shape"] = "box";

    if (vertex->value().type() == V_BASIC_BLOCK) {
        attr["labeljust"] = "l";                        // doesn't seem to align left [Robb P. Matzke 2014-12-01]

        if (vertex->value().function() && vertex->value().function()->address() == vertex->value().address()) {
            attr["style"] = "filled";
            attr["fillcolor"] = "\"#d4ffc8\"";          // function entrance blocks are light green
        } else if (BasicBlock::Ptr bb = vertex->value().bblock()) {
            if (partitioner.basicBlockIsFunctionReturn(bb)) {
                attr["style"] = "filled";
                attr["fillcolor"] = "\"#c8e8ff\"";      // function returns are light blue (unless they are entrance also)
            }
        }
    } else {
        attr["style"] = "filled";
        attr["fillcolor"] = "\"#ff9a9a\"";              // light red to indicate warnings
    }

    std::string s;
    for (std::map<std::string, std::string>::iterator ai=attr.begin(); ai!=attr.end(); ++ai)
        s += (s.empty() ? "" : " ") + ai->first + "=" + ai->second;
    return s;
}

size_t
GraphViz::dumpVertex(std::ostream &out, const Partitioner &partitioner, const ControlFlowGraph::ConstVertexNodeIterator &vertex) {
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
GraphViz::edgeLabel(const Partitioner &partitioner, const ControlFlowGraph::ConstEdgeNodeIterator &edge) {
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
GraphViz::edgeAttributes(const Partitioner &partitioner, const ControlFlowGraph::ConstEdgeNodeIterator &edge) {
    ASSERT_require(edge != partitioner.cfg().edges().end());
    std::map<std::string, std::string> attr;

    if (edge->value().type() == E_FUNCTION_RETURN) {
        attr["color"] = "\"#889ead\"";                  // function returns are blue
    } else if (edge->target() == partitioner.indeterminateVertex()) {
        attr["color"] = "red";                          // red for warning: successor is unknown
    } else if (edge->value().type() == E_FUNCTION_CALL) {
        attr["color"] = "\"#809b79\"";                  // function calls are light green (same as entrance vertices)
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
GraphViz::dumpEdge(std::ostream &out, const Partitioner &partitioner, const ControlFlowGraph::ConstEdgeNodeIterator &edge) {
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
GraphViz::dumpIntraFunction(std::ostream &out, const Partitioner &partitioner, const Function::Ptr &function) {
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
GraphViz::dumpInterFunctionEdges(std::ostream &out, const Partitioner &partitioner, const Function::Ptr &function) {
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
GraphViz::functionLabel(const Partitioner &partitioner, const Function::Ptr &function) {
    ASSERT_not_null(function);
    return function->printableName();
}

std::string
GraphViz::functionAttributes(const Partitioner &partitioner, const Function::Ptr &function) {
    ASSERT_not_null(function);
    return "style=filled fillcolor=\"" + subgraphColor().toString() + "\"";
}

size_t
GraphViz::dumpFunctionInfo(std::ostream &out, const Partitioner &partitioner,
                           const ControlFlowGraph::ConstVertexNodeIterator &vertex) {
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
GraphViz::dumpFunctionCallees(std::ostream &out, const Partitioner &partitioner, const Function::Ptr &function) {
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
GraphViz::dumpCfgAll(std::ostream &out, const Partitioner &partitioner) {
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
GraphViz::dumpCfgFunction(std::ostream &out, const Partitioner &partitioner, const Function::Ptr &function) {
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
GraphViz::dumpCallGraph(std::ostream &out, const Partitioner &partitioner) {
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
