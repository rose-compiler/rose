#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Unparser/EdgeArrows.h>

#include <Rose/BinaryAnalysis/Unparser/Base.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/ControlFlowGraph.h>
#include <Rose/BinaryAnalysis/Partitioner2/Function.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>

namespace P2 = Rose::BinaryAnalysis::Partitioner2;

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

std::pair<size_t, size_t>
EdgeArrows::minMaxRenderColumns() const {
    return {minRenderColumns_, maxRenderColumns_};
}

void
EdgeArrows::minMaxRenderColumns(const size_t minCols, const size_t maxCols) {
    ASSERT_require(minCols <= maxCols);
    minRenderColumns_ = minCols;
    maxRenderColumns_ = maxCols;
}

bool
EdgeArrows::columnIsRendered(const size_t columnIdx) const {
    return columnIdx < columns_.size() && columnIdx < maxRenderColumns_;
}

void
EdgeArrows::appendEndpoint(EndpointId endpoint) {
    // Each endpoint has three logical lines: first, middle, and end. There's also and inter-endpoint line that separates
    // each endpoint. The first endpoint starts at line 1 so it's convenient to get the prior inter-endpoint line by always
    // subtracting one from the first line of an endpoint, or adding one to the last line of an endpoint.
    OutputLocation location;
    if (outputHull_.isEmpty()) {
        location = OutputLocation::baseSize(1, 3);
    } else {
        location = OutputLocation::baseSize(outputHull_.greatest() + 2, 3);
    }
    endpointLocations_.insert(endpoint, location);
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
    endpointLocations_.clear();
    columns_.clear();
}

void
EdgeArrows::computeLayout(const Graph &graph, const std::vector<EndpointId> &ordered) {
    // Locations for the ordered endpoints.
    for (const EndpointId endpoint: ordered) {
        if (!endpointLocations_.exists(endpoint))
            appendEndpoint(endpoint);
    }
    
    // Locations for the un-ordered endpoints.
    std::vector<EndpointId> unordered;
    for (const EndpointId endpoint: graph.vertexValues()) {
        if (!endpointLocations_.exists(endpoint))
            unordered.push_back(endpoint);
    }
    std::sort(unordered.begin(), unordered.end());
    unordered.erase(std::unique(unordered.begin(), unordered.end()), unordered.end());
    for (const EndpointId endpoint: unordered)
        appendEndpoint(endpoint);
    
    // Obtain info about each edge/arrow so we can sort them by length. We want to process small arrows before large arrows in order
    // to pack them nicely into columns.
    std::vector<Arrow> arrows;
    arrows.reserve(graph.nEdges());
    for (const Graph::Edge &edge: graph.edges()) {
        size_t srcLine = endpointLocations_[edge.source()->value()].greatest(); // edges leave from the last line of the endpoint
        size_t dstLine = endpointLocations_[edge.target()->value()].least();    // and enter at the first line of the endpoint
        OutputLocation lines = OutputLocation::hull(srcLine, dstLine);
        arrows.push_back(Arrow(lines, srcLine < dstLine, edge.value()));
    }
    std::sort(arrows.begin(), arrows.end(), ascendingLength);

    // Process each arrow and assign it to the first column where it fits (i.e., doesn't conflict with any other arrow). If there is
    // no such column, append a new column.
    for (Arrow &arrow: arrows) {
        bool inserted = false;
        for (size_t i = 0; i < columns_.size() && !inserted; ++i) {
            if (!columns_[i].overlaps(arrow.location)) {
                arrow.columnIdx = i;
                columns_[i].insert(arrow.location, arrow);
                inserted = true;
            }
        }
        if (!inserted) {
            columns_.push_back(Column());
            arrow.columnIdx = columns_.size() - 1;
            columns_.back().insert(arrow.location, arrow);
        }
    }
}

void
EdgeArrows::computeCfgBlockLayout(const P2::Partitioner::ConstPtr &partitioner, const P2::Function::Ptr &function) {
    ASSERT_not_null(partitioner);
    ASSERT_not_null(function);

    // We use the basic block addresses as the endpoint IDs since the unparser will emit the basic blocks in order of their starting
    // address.
    Graph graph;
    for (Address sourceVa: function->basicBlockAddresses()) {
        P2::ControlFlowGraph::ConstVertexIterator vertex = partitioner->findPlaceholder(sourceVa);
        ASSERT_require(vertex != partitioner->cfg().vertices().end());
        for (const P2::ControlFlowGraph::Edge &edge: vertex->outEdges()) {
            if (edge.target()->value().type() == P2::V_BASIC_BLOCK) {
                Address targetVa = edge.target()->value().address();
                if (function->ownsBasicBlock(targetVa)) // don't use Partitioner::isIntraFunctionEdge; we want all self edges
                    graph.insertEdgeWithVertices(sourceVa, targetVa, edge.id());
            }
        }
    }
    computeLayout(graph, std::vector<EndpointId>());
}

void
EdgeArrows::computeCfgEdgeLayout(const P2::Partitioner::ConstPtr &partitioner, const P2::Function::Ptr &function) {
    ASSERT_not_null(function);

    // Process basic blocks in the order they're emitted by the unparser (i.e., by increasing starting address), and for each block
    // process it's incoming edges in the order the "predecessors" are emitted by the unparser.  After all incoming edges for the
    // block, process the outgoing edges in the order of the "successors" emitted by the unparser. Each CFG edge has two endpoints
    // that will be emitted by the unparser and the endpoint IDs are a function of the CFG edge ID.  Of course not all CFG edges
    // will appear in a single function, and we only care about those whose source and target are both in this function.
    std::vector<EndpointId> edgeEndpointIds;            // order that CFG edge endpoints are emitted by the unparser
    Graph graph;                                        // the arrows (edges) and their endpoints (vertices)

    for (Address sourceVa: function->basicBlockAddresses()) {
        P2::BasicBlock::Ptr bb = partitioner->basicBlockExists(sourceVa);
        ASSERT_not_null(bb);

        // Incoming edges for the basic block (and we do the arrow at the same time)
        std::vector<P2::ControlFlowGraph::ConstEdgeIterator> inEdges = Unparser::Base::orderedBlockPredecessors(partitioner, bb);
        for (P2::ControlFlowGraph::ConstEdgeIterator inEdge: inEdges) {
            edgeEndpointIds.push_back(edgeToTargetEndpoint(inEdge->id()));

            if (inEdge->target()->value().type() == P2::V_BASIC_BLOCK) {
                Address sourceVa = inEdge->source()->value().address();
                if (function->ownsBasicBlock(sourceVa)) // don't use Partitioner::isIntraFunctionEdge; we want all self edges
                    graph.insertEdgeWithVertices(edgeToSourceEndpoint(inEdge->id()), edgeToTargetEndpoint(inEdge->id()),
                                                 inEdge->id());
            }
        }

        // Followed by outgoing edges for the basic block
        std::vector<P2::ControlFlowGraph::ConstEdgeIterator> outEdges = Unparser::Base::orderedBlockSuccessors(partitioner, bb);
        for (P2::ControlFlowGraph::ConstEdgeIterator outEdge: outEdges)
            edgeEndpointIds.push_back(edgeToSourceEndpoint(outEdge->id()));
    }
    computeLayout(graph, edgeEndpointIds);
}

// class method
EdgeArrows::EndpointId
EdgeArrows::edgeToSourceEndpoint(const size_t edgeId) {
    return 2 * edgeId + 0;
}

// class method
EdgeArrows::EndpointId
EdgeArrows::edgeToTargetEndpoint(const size_t edgeId) {
    return 2 * edgeId + 1;
}

// class method
size_t
EdgeArrows::edgeFromEndpoint(const EndpointId endpoint) {
    return endpoint / 2;
}

// class method
bool
EdgeArrows::isSourceEndpoint(const EndpointId endpoint) {
    return 0 == endpoint % 2;
}

// class method
bool
EdgeArrows::isTargetEndpoint(const EndpointId endpoint) {
    return 1 == endpoint % 2;
}

// class method
EdgeArrows::EndpointId
EdgeArrows::otherEndpoint(const EndpointId endpoint) {
    return isSourceEndpoint(endpoint) ? endpoint + 1 : endpoint - 1;
}

size_t
EdgeArrows::nArrowColumns() const {
    return columns_.size();
}

size_t
EdgeArrows::nRenderColumns() const {
    const size_t need = columns_.size();
    return std::max(minRenderColumns_, std::min(need, maxRenderColumns_));
}

std::string
EdgeArrows::renderBlank() const {
    std::string retval;
    for (size_t i = 0; i < nRenderColumns(); ++i)
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

    out <<"endpoint locations:\n";
    for (const EndpointLocations::Node &node: endpointLocations_.nodes()) {
        out <<"  endpoint " <<node.key() <<" (" <<StringUtility::addrToString(node.key()) <<")"
            <<" at lines " <<node.value().least()
            <<" through " <<node.value().greatest() <<"\n";
    }

    out <<StringUtility::plural(columns_.size(), "columns") <<"\n";
    for (size_t i=0; i<columns_.size(); ++i) {
        out <<"  column #" <<i <<":\n";
        for (const Arrow &arrow: columns_[i].values()) {
            ASSERT_require(arrow.columnIdx == i);
            out <<"    " <<(arrow.isForward ? "forward" : "backward") <<" arrow id=" <<arrow.arrowId
                <<" at lines " <<arrow.location.least()
                <<" through " <<arrow.location.greatest() <<"\n";
        }
    }
}

bool
EdgeArrows::exists(const EndpointId endpoint) const {
    return endpointLocations_.exists(endpoint);
}

Sawyer::Optional<EdgeArrows::Arrow>
EdgeArrows::findArrow(const ArrowId arrowId) const {
    for (const Column &column: columns_) {
        for (const auto &node: column.nodes()) {
            if (node.value().arrowId == arrowId)
                return node.value();
        }
    }
    return {};
}

std::string
EdgeArrows::render(const EndpointId endpoint, const OutputPart part) const {
    std::string retval;

    // Find the line number.
    const OutputLocation &endpointLocation = endpointLocations_.getOrDefault(endpoint);
    if (endpointLocation.isEmpty())
        return renderBlank();                           // this is not a vertex we know about!
    size_t lineNumber = 0;
    switch (part) {
        case FIRST_LINE:
            lineNumber = endpointLocation.least();
            break;
        case MIDDLE_LINE:
            lineNumber = endpointLocation.least()+1;
            break;
        case LAST_LINE:
            lineNumber = endpointLocation.greatest();
            break;
        case INTER_LINE:
            lineNumber = endpointLocation.greatest()+1;
            break;
    }

    // Figure out which columns should be rendered, and the order in which to render them from left to right.
    std::vector<size_t> columnsToRender;
    for (size_t i = 0; i < columns_.size(); ++i) {
        if (columnIsRendered(i))
            columnsToRender.push_back(i);
    }
    if (arrowStyle_.pointsRight)
        std::reverse(columnsToRender.begin(), columnsToRender.end());

    // Emit extra blank columns at the beginning?
    if (arrowStyle_.pointsRight) {
        for (size_t i = columnsToRender.size(); i < minRenderColumns_; ++i)
            retval += arrowStyle_.blank;
    }

    // Render the arrow field across all the arrow columns.
    for (const size_t columnIdx: columnsToRender) {
        const Arrow &arrow = columns_[columnIdx].getOrDefault(lineNumber);
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

    // Emit extra Blank columns at the end?
    if (!arrowStyle_.pointsRight) {
        for (size_t i = columnsToRender.size(); i < minRenderColumns_; ++i)
            retval += arrowStyle_.blank;
    }

    return retval;
}

size_t
EdgeArrows::nSources(const EndpointId endpoint) const {
    const OutputLocation &endpointLocation = endpointLocations_.getOrDefault(endpoint);
    if (endpointLocation.isEmpty())
        return 0;
    const size_t lineNumber = endpointLocation.greatest();

    size_t retval = 0;
    for (size_t i = 0; i < columns_.size(); ++i) {
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
EdgeArrows::nTargets(const EndpointId endpoint) const {
    const OutputLocation &endpointLocation = endpointLocations_.getOrDefault(endpoint);
    if (endpointLocation.isEmpty())
        return 0;
    const size_t lineNumber = endpointLocation.least();

    size_t retval = 0;
    for (size_t i = 0; i < columns_.size(); ++i) {
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
            break;
        case UNICODE_2:
            switch (side) {
                case LEFT: return arrowStyle(unicodeL2());
                case RIGHT: return arrowStyle(unicodeR2());
            }
            break;
        case ASCII_1:
            switch (side) {
                case LEFT: return arrowStyle(asciiL1());
                case RIGHT: return arrowStyle(asciiR1());
            }
            break;
        case ASCII_2:
            switch (side) {
                case LEFT: return arrowStyle(asciiL2());
                case RIGHT: return arrowStyle(asciiR2());
            }
            break;
        case ASCII_3:
            switch (side) {
                case LEFT: return arrowStyle(asciiL3());
                case RIGHT: return arrowStyle(asciiR3());
            }
            break;
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

#endif
