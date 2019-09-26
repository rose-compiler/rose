#include <sage3basic.h>

#include <BinaryEdgeArrows.h>
#include <BinaryUnparserBase.h>
#include <Partitioner2/ControlFlowGraph.h>
#include <Partitioner2/Function.h>
#include <Partitioner2/Partitioner.h>

namespace P2 = Rose::BinaryAnalysis::Partitioner2;

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

void
EdgeArrows::appendVertex(VertexId vertex) {
    // Each vertex has three logical lines: first, middle, and end. There's also and inter-vertex line that separates
    // each vertex. The first vertex starts at line 1 so it's convenient to get the prior inter-vertex line by always
    // subtracting one from the first line of a vertex, or adding one to the last line of a vertex.
    OutputLocation location;
    if (outputHull_.isEmpty()) {
        location = OutputLocation::baseSize(1, 3);
    } else {
        location = OutputLocation::baseSize(outputHull_.greatest() + 2, 3);
    }
    vertexLocations_.insert(vertex, location);
    outputHull_ = outputHull_.hull(location);
}

bool
EdgeArrows::ascendingLength(const Arrow &a, const Arrow &b) {
    ASSERT_forbid(a.location.isEmpty());
    ASSERT_forbid(b.location.isEmpty());
    return a.location.size() < b.location.size();
}

void
EdgeArrows::reset() {
    outputHull_ = OutputLocation();
    vertexLocations_.clear();
    columns_.clear();
}

void
EdgeArrows::computeLayout(const Graph &graph, const std::vector<VertexId> &ordered) {
    // Locations for the ordered vertices.
    BOOST_FOREACH (VertexId v, ordered) {
        if (!vertexLocations_.exists(v))
            appendVertex(v);
    }
    
    // Locations for the un-ordered vertices.
    std::vector<VertexId> unordered;
    BOOST_FOREACH (VertexId v, graph.vertexValues()) {
        if (!vertexLocations_.exists(v))
            unordered.push_back(v);
    }
    std::sort(unordered.begin(), unordered.end());
    unordered.erase(std::unique(unordered.begin(), unordered.end()), unordered.end());
    BOOST_FOREACH (VertexId v, unordered)
        appendVertex(v);
    
    // Obtain info about each edge so we can sort them by length. We want to process small edges
    // before large edges in order to pack them nicely into columns.
    std::vector<Arrow> arrows;
    arrows.reserve(graph.nEdges());
    BOOST_FOREACH (const Graph::Edge &edge, graph.edges()) {
        size_t srcLine = vertexLocations_[edge.source()->value()].greatest(); // edges leave from the last line
        size_t dstLine = vertexLocations_[edge.target()->value()].least(); // and enter at the first line
        OutputLocation lines = OutputLocation::hull(srcLine, dstLine);
        arrows.push_back(Arrow(lines, srcLine < dstLine));
    }
    std::sort(arrows.begin(), arrows.end(), ascendingLength);

    // Process each arrow and assign it to the first layer where it fits (i.e., doesn't conflict with any other arrow). If
    // there is no such layer, append a new layer.
    BOOST_FOREACH (const Arrow &arrow, arrows) {
        bool inserted = false;
        for (size_t i = 0; i < columns_.size() && !inserted; ++i) {
            if (!columns_[i].isOverlapping(arrow.location)) {
                columns_[i].insert(arrow.location, arrow);
                inserted = true;
            }
        }

        if (!inserted) {
            columns_.push_back(Column());
            columns_.back().insert(arrow.location, arrow);
        }
    }
}

void
EdgeArrows::computeCfgBlockLayout(const P2::Partitioner &partitioner, const P2::Function::Ptr &function) {
    ASSERT_not_null(function);

    // We use the basic block addresses as the arrow vertex names since the unparser will emit the basic blocks in order of
    // their starting address.
    Graph graph;                                        // the arrows (edges) and their endpoints (vertices)
    BOOST_FOREACH (rose_addr_t sourceVa, function->basicBlockAddresses()) {
        P2::ControlFlowGraph::ConstVertexIterator vertex = partitioner.findPlaceholder(sourceVa);
        ASSERT_require(vertex != partitioner.cfg().vertices().end());
        BOOST_FOREACH (const P2::ControlFlowGraph::Edge &edge, vertex->outEdges()) {
            if (edge.target()->value().type() == P2::V_BASIC_BLOCK) {
                rose_addr_t targetVa = edge.target()->value().address();
                if (function->ownsBasicBlock(targetVa)) // don't use Partitioner::isIntraFunctionEdge; we want all self edges
                    graph.insertEdgeWithVertices(sourceVa, targetVa);
            }
        }
    }
    computeLayout(graph);
}


void
EdgeArrows::computeCfgEdgeLayout(const P2::Partitioner &partitioner, const P2::Function::Ptr &function) {
    ASSERT_not_null(function);

    // Process basic blocks in the order they're emitted by the unparser (i.e., by increasing starting address), and for each
    // block process it's incoming edges in the order the "predecessors" are emitted by the unparser.  After all incoming edges
    // for the block, process the outgoing edges in the order of the "successors" emitted by the unparser. Each CFG edge has
    // two endpoints that will be emitted by the unparser, so for the arrow graph, we use vertex IDs that are twice the CFG
    // edge ID plus either 0 for targets or 1 for sources.  Of course not all CFG edges will appear in a single function; we
    // only care about those whose source and target are both in this function.
    std::vector<VertexId> edgeEndpointIds;              // order that CFG edge endpoints are emitted by the unparser
    Graph graph;                                        // the arrows (edges) and their endpoints (vertices)

    BOOST_FOREACH (rose_addr_t sourceVa, function->basicBlockAddresses()) {
        P2::BasicBlock::Ptr bb = partitioner.basicBlockExists(sourceVa);
        ASSERT_not_null(bb);

        // Incoming edges for the basic block (and we do the arrow at the same time)
        std::vector<P2::ControlFlowGraph::ConstEdgeIterator> inEdges = Unparser::Base::orderedBlockPredecessors(partitioner, bb);
        BOOST_FOREACH (P2::ControlFlowGraph::ConstEdgeIterator inEdge, inEdges) {
            edgeEndpointIds.push_back(cfgEdgeTargetEndpoint(inEdge->id()));

            if (inEdge->target()->value().type() == P2::V_BASIC_BLOCK) {
                rose_addr_t sourceVa = inEdge->source()->value().address();
                if (function->ownsBasicBlock(sourceVa)) // don't use Partitioner::isIntraFunctionEdge; we want all self edges
                    graph.insertEdgeWithVertices(cfgEdgeSourceEndpoint(inEdge->id()), cfgEdgeTargetEndpoint(inEdge->id()));
            }
        }

        // Followed by outgoing edges for the basic block
        std::vector<P2::ControlFlowGraph::ConstEdgeIterator> outEdges = Unparser::Base::orderedBlockSuccessors(partitioner, bb);
        BOOST_FOREACH (P2::ControlFlowGraph::ConstEdgeIterator outEdge, outEdges)
            edgeEndpointIds.push_back(cfgEdgeSourceEndpoint(outEdge->id()));
    }
    computeLayout(graph, edgeEndpointIds);
}

// class method
EdgeArrows::VertexId
EdgeArrows::cfgEdgeSourceEndpoint(size_t edgeId) {
    return 2 * edgeId + 0;
}

// class method
EdgeArrows::VertexId
EdgeArrows::cfgEdgeTargetEndpoint(size_t edgeId) {
    return 2 * edgeId + 1;
}

size_t
EdgeArrows::nArrowColumns() const {
    return columns_.size();
}


std::string
EdgeArrows::renderBlank() const {
    std::string retval;
    for (size_t i=0; i<columns_.size(); ++i)
        retval += arrowStyle_.blank;
    return retval;
}

void
EdgeArrows::debug(std::ostream &out) const {
    out <<"output hull = ";
    if (outputHull_.isEmpty()) {
        out <<"empty\n";
    } else {
        out <<"lines [" <<outputHull_.least() <<", " <<outputHull_.greatest() <<"]\n";
    }

    out <<"vertex locations:\n";
    BOOST_FOREACH (const VertexLocations::Node &node, vertexLocations_.nodes()) {
        out <<"  vertex " <<node.key() <<" (" <<StringUtility::addrToString(node.key()) <<")"
            <<" at lines " <<node.value().least()
            <<" through " <<node.value().greatest() <<"\n";
    }

    out <<StringUtility::plural(columns_.size(), "columns") <<"\n";
    for (size_t i=0; i<columns_.size(); ++i) {
        out <<"  column #" <<i <<":\n";
        BOOST_FOREACH (const Arrow &arrow, columns_[i].values()) {
            out <<"    " <<(arrow.isForward ? "forward" : "backward") <<" arrow "
                <<" at lines " <<arrow.location.least()
                <<" through " <<arrow.location.greatest() <<"\n";
        }
    }
}

std::string
EdgeArrows::render(VertexId v, OutputPart part) const {
    // Find the line number.
    const OutputLocation &vertexLocation = vertexLocations_.getOrDefault(v);
    if (vertexLocation.isEmpty())
        return renderBlank();                           // this is not a vertex we know about!
    size_t lineNumber = 0;
    switch (part) {
        case FIRST_LINE:
            lineNumber = vertexLocation.least();
            break;
        case MIDDLE_LINE:
            lineNumber = vertexLocation.least()+1;
            break;
        case LAST_LINE:
            lineNumber = vertexLocation.greatest();
            break;
        case INTER_LINE:
            lineNumber = vertexLocation.greatest()+1;
            break;
    }

    // Render the arrow field across all the arrow columns.
    std::string retval;
    for (size_t i=0; i<columns_.size(); ++i) {
        size_t colnum = arrowStyle_.pointsRight ? columns_.size() - (i+1) : i;
        const Arrow &arrow = columns_[colnum].getOrDefault(lineNumber);

        if (arrow.location.isEmpty()) {
            retval += arrowStyle_.blank;

        } else if (lineNumber == arrow.location.least()) {
            // We're at the top of an arrow
            if (arrow.isForward) {
                retval += arrowStyle_.sourceThenDown;
            } else {
                retval += arrowStyle_.upToTarget;
            }

        } else if (lineNumber == arrow.location.greatest()) {
            // We're at the bottom of an arrow
            if (arrow.isForward) {
                retval += arrowStyle_.downToTarget;
            } else {
                retval += arrowStyle_.sourceThenUp;
            }

        } else {
            // We're in the middle of an arrow
            retval += arrowStyle_.verticalLine;
        }
    }

    return retval;
}

size_t
EdgeArrows::nSources(VertexId v) const {
    const OutputLocation &vertexLocation = vertexLocations_.getOrDefault(v);
    if (vertexLocation.isEmpty())
        return 0;
    size_t lineNumber = vertexLocation.greatest();

    size_t retval = 0;
    for (size_t i=0; i<columns_.size(); ++i) {
        size_t colnum = arrowStyle_.pointsRight ? columns_.size() - (i+1) : i;
        const Arrow &arrow = columns_[colnum].getOrDefault(lineNumber);
        if (!arrow.location.isEmpty()) {
            if (lineNumber == arrow.location.least() && arrow.isForward) {
                ++retval;
            } else if (lineNumber == arrow.location.greatest() && !arrow.isForward) {
                ++retval;
            }
        }
    }
    return retval;
}

size_t
EdgeArrows::nTargets(VertexId v) const {
    const OutputLocation &vertexLocation = vertexLocations_.getOrDefault(v);
    if (vertexLocation.isEmpty())
        return 0;
    size_t lineNumber = vertexLocation.least();

    size_t retval = 0;
    for (size_t i=0; i<columns_.size(); ++i) {
        size_t colnum = arrowStyle_.pointsRight ? columns_.size() - (i+1) : i;
        const Arrow &arrow = columns_[colnum].getOrDefault(lineNumber);
        if (!arrow.location.isEmpty()) {
            if (lineNumber == arrow.location.least() && !arrow.isForward) {
                ++retval;
            } else if (lineNumber == arrow.location.greatest() && arrow.isForward) {
                ++retval;
            }
        }
    }
    return retval;
}

void
EdgeArrows::arrowStyle(ArrowStylePreset preset, ArrowSide side) {
    switch (preset) {
        case UNICODE_1:
            switch (side) {
                case LEFT: return arrowStyle(unicodeL1());
                case RIGHT: return arrowStyle(unicodeR1());
            }
        case UNICODE_2:
            switch (side) {
                case LEFT: return arrowStyle(unicodeL2());
                case RIGHT: return arrowStyle(unicodeR2());
            }
        case ASCII_1:
            switch (side) {
                case LEFT: return arrowStyle(asciiL1());
                case RIGHT: return arrowStyle(asciiR1());
            }
        case ASCII_2:
            switch (side) {
                case LEFT: return arrowStyle(asciiL2());
                case RIGHT: return arrowStyle(asciiR2());
            }
        case ASCII_3:
            switch (side) {
                case LEFT: return arrowStyle(asciiL3());
                case RIGHT: return arrowStyle(asciiR3());
            }
    }
    ASSERT_not_reachable("invalid arrow preset or side");
}

EdgeArrows::ArrowStyle
EdgeArrows::unicodeL1() {
    ArrowStyle a;
    a.charactersPerColumn = 1;
    a.pointsRight = true;
    a.sourceThenDown   = "\u250c";
    a.verticalLine     = "\u2502";
    a.downToTarget     = "\u21b3";
    a.upToTarget       = "\u21b1";
    a.sourceThenUp     = "\u2514";
    a.sourceHorizontal = "\u2500";
    a.targetHorizontal = "\u25b6";
    a.blank            = " ";
    return a;
}

EdgeArrows::ArrowStyle
EdgeArrows::unicodeR1() {
    ArrowStyle a;
    a.charactersPerColumn = 1;
    a.pointsRight = false;
    a.sourceThenDown   = "\u2510";
    a.verticalLine     = "\u2502";
    a.downToTarget     = "\u21b2";
    a.upToTarget       = "\u21b0";
    a.sourceThenUp     = "\u2518";
    a.sourceHorizontal = "\u2500";
    a.targetHorizontal = "\u25c0";
    a.blank            = " ";
    return a;
}

EdgeArrows::ArrowStyle
EdgeArrows::unicodeL2() {
    ArrowStyle a;
    a.charactersPerColumn = 2;
    a.pointsRight = true;
    a.sourceThenDown   = "\u250c\u2500";
    a.verticalLine     = "\u2502 ";
    a.downToTarget     = "\u2514\u25b6";
    a.upToTarget       = "\u250c\u25b6";
    a.sourceThenUp     = "\u2514\u2500";
    a.sourceHorizontal = "\u2500\u2500";
    a.targetHorizontal = "\u2500\u25b6";
    a.blank            = "  ";
    return a;
}

EdgeArrows::ArrowStyle
EdgeArrows::unicodeR2() {
    ArrowStyle a;
    a.charactersPerColumn = 2;
    a.pointsRight = false;
    a.sourceThenDown   = "\u2500\u2510";
    a.verticalLine     = " \u2502";
    a.downToTarget     = "\u25c0\u2518";
    a.upToTarget       = "\u25c0\u2510";
    a.sourceThenUp     = "\u2500\u2518";
    a.sourceHorizontal = "\u2500\u2500";
    a.targetHorizontal = "\u25c0\u2500";
    a.blank            = "  ";
    return a;
}

EdgeArrows::ArrowStyle
EdgeArrows::asciiL1() {
    ArrowStyle a;
    a.charactersPerColumn = 1;
    a.pointsRight = true;
    a.sourceThenDown   = ".";
    a.verticalLine     = "|";
    a.downToTarget     = ">";
    a.upToTarget       = ">";
    a.sourceThenUp     = "'";
    a.sourceHorizontal = "-";
    a.targetHorizontal = ">";
    a.blank            = " ";
    return a;
}

EdgeArrows::ArrowStyle
EdgeArrows::asciiR1() {
    ArrowStyle a;
    a.charactersPerColumn = 1;
    a.pointsRight = false;
    a.sourceThenDown   = ".";
    a.verticalLine     = "|";
    a.downToTarget     = "<";
    a.upToTarget       = "<";
    a.sourceThenUp     = "'";
    a.sourceHorizontal = "-";
    a.targetHorizontal = "<";
    a.blank            = " ";
    return a;
}

EdgeArrows::ArrowStyle
EdgeArrows::asciiL2() {
    ArrowStyle a;
    a.charactersPerColumn = 2;
    a.pointsRight = true;
    a.sourceThenDown   = ".-";
    a.verticalLine     = "| ";
    a.downToTarget     = "`>";
    a.upToTarget       = ".>";
    a.sourceThenUp     = "`-";
    a.sourceHorizontal = "--";
    a.targetHorizontal = "->";
    a.blank            = "  ";
    return a;
}

EdgeArrows::ArrowStyle
EdgeArrows::asciiR2() {
    ArrowStyle a;
    a.charactersPerColumn = 2;
    a.pointsRight = false;
    a.sourceThenDown   = "-.";
    a.verticalLine     = " |";
    a.downToTarget     = "<'";
    a.upToTarget       = "<.";
    a.sourceThenUp     = "-'";
    a.sourceHorizontal = "--";
    a.targetHorizontal = "<-";
    a.blank            = "  ";
    return a;
}

EdgeArrows::ArrowStyle
EdgeArrows::asciiL3() {
    ArrowStyle a;
    a.charactersPerColumn = 3;
    a.pointsRight = true;
    a.sourceThenDown   = ".--";
    a.verticalLine     = "|  ";
    a.downToTarget     = "`->";
    a.upToTarget       = ".->";
    a.sourceThenUp     = "`--";
    a.sourceHorizontal = "---";
    a.targetHorizontal = "-->";
    a.blank            = "   ";
    return a;
}

EdgeArrows::ArrowStyle
EdgeArrows::asciiR3() {
    ArrowStyle a;
    a.charactersPerColumn = 3;
    a.pointsRight = false;
    a.sourceThenDown   = "--.";
    a.verticalLine     = "  |";
    a.downToTarget     = "<-'";
    a.upToTarget       = "<-.";
    a.sourceThenUp     = "--'";
    a.sourceHorizontal = "---";
    a.targetHorizontal = "<--";
    a.blank            = "   ";
    return a;
}

EdgeArrows::ArrowStyle
EdgeArrows::asciiL4() {
    ArrowStyle a;
    a.charactersPerColumn = 3;
    a.pointsRight = true;
    a.sourceThenDown   = " .--";
    a.verticalLine     = " |  ";
    a.downToTarget     = " `->";
    a.upToTarget       = " .->";
    a.sourceThenUp     = " `--";
    a.sourceHorizontal = "----";
    a.targetHorizontal = "--->";
    a.blank            = "    ";
    return a;
}

EdgeArrows::ArrowStyle
EdgeArrows::asciiR4() {
    ArrowStyle a;
    a.charactersPerColumn = 4;
    a.pointsRight = false;
    a.sourceThenDown   = "--. ";
    a.verticalLine     = "  | ";
    a.downToTarget     = "<-' ";
    a.upToTarget       = "<-. ";
    a.sourceThenUp     = "--' ";
    a.sourceHorizontal = "----";
    a.targetHorizontal = "<---";
    a.blank            = "    ";
    return a;
}

} // namespace
} // namespace
} // namespace
