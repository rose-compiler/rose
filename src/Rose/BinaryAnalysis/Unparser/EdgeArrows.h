#ifndef ROSE_BinaryAnalysis_Unparser_EdgeArrows_H
#define ROSE_BinaryAnalysis_Unparser_EdgeArrows_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/Partitioner2/BasicTypes.h>
#include <Sawyer/Graph.h>
#include <Sawyer/Interval.h>
#include <Sawyer/IntervalMap.h>
#include <Sawyer/Map.h>
#include <Rose/StringUtility.h>
#include <ROSE_DEPRECATED.h>

#include <ostream>
#include <vector>

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Basic block arrow information
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Renders textual margin arrows.
 *
 *  This class is responsible for calculating the layout of textual arrows in a left or right margin of assembly output, and then
 *  later rendering those arrows.
 *
 *  The implementation is general in that it doesn't know what purpose the arrows serve--it only knows that an arrow has two
 *  endpoints called herein the "source" and "target". However, it assumes that an arrow spans lines of output and its main body
 *  is therefore rendered as a vertical lines, although its two endpoints might have horizontal components.
 *
 *  This API has a few high-level layout functions for situations that are common in the ROSE library, and these are written in
 *  terms of a more general layout function. At the more general level, the desired arrows are represented by a graph whose vertices
 *  represent the end points and whose edges represent the desired arrows connecting the endpoints. Arrows may share endpoints in
 *  this descriptive graph.  Each endpoint has a unique ID number. */
class EdgeArrows {
public:
    /** Parts of the output.
     *
     *  Arrows are rendered one line at a time, although it is simpler to think of the output in terms of the things to which the
     *  arrows point (e.g., the basic blocks in a control flow graph). Each pointee is assumed to have zero or more lines describing
     *  arrow targets (e.g., describing from whence CFG execution flow comes), followed by zero or more non-endpoint lines (e.g.,
     *  the instructions themselves), followed by zero or more lines describing arrow sources (e.g., describing to where execution
     *  flow goes next).  Pointees might be separated from one another in the output by zero or more other lines. */
    enum OutputPart {
        FIRST_LINE,                                     /**< An arrow target line. */
        LAST_LINE,                                      /**< An arrow source line. */
        MIDDLE_LINE,                                    /**< Non-arrow lines between target and source lines for a pointee. */
        INTER_LINE                                      /**< Lines between sources of this pointee and targets of next pointee. */
    };

    /** Endpoint identification number.
     *
     *  These serve a user-specified purpose to identify the things to/from which arrows point. For instance, they might be the
     *  identification numbers for basic blocks in a control flow graph, or they might be identification numbers for lines
     *  describing CFG edge sources and targets. */
    using EndpointId = size_t;

    /** Arrow identification number.
     *
     *  This is a user-specified arrow identification number. Most users will probably arrange for arrow IDs to be the same as
     *  the ID numbers for edges in a control flow graph. */
    using ArrowId = size_t;

    /** Graph relating endpoints.
     *
     *  The vertices of this graph represent the endpoints of the arrows. The arrows themselves are represented by the directed
     *  edges in this graph. An edge from endpoint A to endpoint B means that there should be an arrow that originates from endpoint
     *  A and points to endpoint B in the output. */
    using Graph = Sawyer::Container::Graph<EndpointId, ArrowId, EndpointId, ArrowId>;

    /** Arrow text.
     *
     *  Represents the text to output for different parts of an arrow. */
    struct ArrowStyle {
        size_t charactersPerColumn;                     /**< Number of characters per arrow column. */
        bool pointsRight;                               /**< Arrows point right? Otherwise left. */
        std::string sourceThenDown;                     /**< Text for an arrow leaving a vertex and turning downward. */
        std::string sourceThenUp;                       /**< Text for an arrow leaving a vertex and turning upward. */
        std::string verticalLine;                       /**< Text for the vertical line part of an arrow. */
        std::string downToTarget;                       /**< Text for an arrow comming from above into a vertex. */
        std::string upToTarget;                         /**< Text for an arrow comming from below into a vertex. */
        std::string blank;                              /**< Text when there's lack of any arrow. */
        std::string sourceHorizontal;                   /**< Text for the horizontal source of an arrow. */
        std::string targetHorizontal;                   /**< Text for the horizontal target of an arrow. */
    };

    /** Arrow style presets. */
    enum ArrowStylePreset {
        UNICODE_1,                                      /**< Single-character Unicode arrows. */
        UNICODE_2,                                      /**< Two-character Unicode arrows. Best readability vs. size. */
        ASCII_1,                                        /**< Single-character ASCII-art arrows. Not very readable. */
        ASCII_2,                                        /**< Two-character ASCII-art arrows. Somewhat readable. */
        ASCII_3                                         /**< Threee-character ASCII-art arrows. Best readability for ASCII. */
    };

    /** Specifies on which side of the output the arrows appear. */
    enum ArrowSide {
        LEFT,                                           /**< Arrows appear left of the main output and point to the right. */
        RIGHT                                           /**< Arrows appear right of the main output and point to the left. */
    };
    
private:
    // Logical location of an arrow endpoint in the output. Each endpoint is assumed to occupy four logical lines, although each
    // logical line may consist of multiple physical lines of output). They are:
    //
    //    1. a first line for the endpoint,
    //
    //    2. a middle line representative of all lines between #1 and #3
    //
    //    3. a last line for the endpoint
    //
    //    4. a separator line representative of all lines of output between #4 of this endpoint and #1 of the next endpoint.
    using OutputLocation = Sawyer::Container::Interval<size_t>;

    // Mapping from an endpoint to its logical location in the output.
    using EndpointLocations = Sawyer::Container::Map<EndpointId, OutputLocation>;

    // Arrow properties
    struct Arrow {
        OutputLocation location;                        // line numbers occupied by the arrow
        bool isForward;                                 // arrow points to a later vertex?
        ArrowId arrowId;                                // ID for this arrow
        size_t columnIdx = INVALID_INDEX;               // column of arrows where this arrow appears (after its been assigned)

        Arrow()
            : isForward(true) {}

        Arrow(const OutputLocation &location, const bool isForward, const ArrowId arrowId)
            : location(location), isForward(isForward), arrowId(arrowId) {}

        bool operator==(const Arrow &other) {
            return location == other.location && isForward == other.isForward && arrowId == other.arrowId;
        }
    };
    
    // One column of arrows. Maps line numbers of output to line numbers of edges.
    using Column = Sawyer::Container::IntervalMap<OutputLocation /*line number*/, Arrow>;

    // Columns of arrows
    using Columns = std::vector<Column>;

private:
    OutputLocation outputHull_;                         // entire output
    EndpointLocations endpointLocations_;               // logical location of each endpoint in the output
    Columns columns_;                                   // columns of arrows
    ArrowStyle arrowStyle_;                             // how to render arrows
    size_t minRenderColumns_ = 0;                       // minimum number of arrow columns to render
    size_t maxRenderColumns_ = UNLIMITED;               // maximum number of arrow columns to render

public:
    EdgeArrows() {
        arrowStyle_ = asciiL3();                        // maximum portability but slightly less clarity than unicodeL2
    }

    /** Property: Minimum and maximum number of columns to render.
     *
     *  The render columns will emit at least the specified minumum number of arrow columns, but not more than the maximum. If there
     *  are fewer columns than the minimum, then extra empty columns are rendered; if there are more than the maximum number of
     *  columns then some columns are not rendered.
     *
     *  The minimum must be less than or equal to the maximum.
     *
     * @{ */
    std::pair<size_t, size_t> minMaxRenderColumns() const;
    void minMaxRenderColumns(size_t minColumns, size_t maxColumns);
    /** @} */

    /** Predicate: Whether the specified arrow column is rendered.
     *
     *  Arrows belong to logical columns, and the renderer can be set to show a minimum and maximum number of columns. This
     *  predicate returns true if the specified logical column would be shown.  Columns are printed in different orders depending on
     *  whether they appear left or right of the things to which they point, and this predicate takes that into account. */
    bool columnIsRendered(size_t columnIdx) const;

    /** Analyze connectivity in order to assign arrow locations.
     *
     *  Analyzes the connectivity of the endpoints specified in the @p graph, and the optional endpoint @p order to calculate the
     *  locations of the margin arrows.  The endpoints in the graph are assumed to be output in the order listed in the @p order
     *  argument. Any endpoints that are present in the @p graph but not in the @p order specification are sorted by @ref EndpointId
     *  and then appended to the @p order.
     *
     *  This does not clear previous layout information, and so it can be used to append to previous output. To discard previous
     *  layout information first, call @ref reset. */
    void computeLayout(const Graph&, const std::vector<EndpointId> &order);

    /** Compute arrow layout for a control flow graph for a function.
     *
     *  This produces arrows for intra-function CFG edges such that the arrows' sources and targets are basic blocks. That is, in
     *  the output, arrows will point to the first instruction of a basic block and emanate from the last instruction.  This method
     *  assumes that the unparser emits a function's basic blocks in order of their starting address.
     *
     *  See also, @ref computeCfgEdgeLayout */
    void computeCfgBlockLayout(const Partitioner2::PartitionerConstPtr&, const Partitioner2::FunctionPtr&);

    /** Compute arrow layout for a control flow graph for a function.
     *
     *  This produces arrows for intra-function CFG edges such that the arrows' sources and targets are the "predecessor" and
     *  "successor" lines emitted by the unparser before and after each basic block. That is, each arrow points to its corresponding
     *  "predecessor" line, and emanates from its corresponding "successor" line.  This method assumes that the unparser emits a
     *  function's basic blocks in order of their starting address, and emits the "predecessor" and "successor" lines in the order
     *  they appear in the partitioner control flow graph (from @c vertex.inEdges and @c vertex.outEdges, respectively). It also
     *  assumes that "predecessors" are listed before "successors".
     *
     *  See also, @ref computeCfgBlockLayout */
    void computeCfgEdgeLayout(const Partitioner2::PartitionerConstPtr&, const Partitioner2::FunctionPtr&);

    /** Arrow endpoint IDs related to edge or vertex IDs.
     *
     *  These functions generate arrow endpoint IDs (`EndpointId`) from non-arrow IDs (`size_t`) and vice versa. They're purely here
     *  for convenience--you can number your endpoints however you like as long as unique endpoints have unique IDs.
     *
     *  The @ref computeCfgEdgeLayout uses the numbering scheme implemented by these functions.
     *
     * @{ */
    static EndpointId toSourceEndpoint(size_t cfgEdgeOrVertexId);
    static EndpointId toTargetEndpoint(size_t cfgEdgeOrVertexId);
    static size_t fromEndpoint(EndpointId);
    static bool isSourceEndpoint(EndpointId);
    static bool isTargetEndpoint(EndpointId);
    static EndpointId otherEndpoint(EndpointId);
    /** @} */

    // [Robb Matzke 2025-04-04]: Deprecated
    static EndpointId edgeToSourceEndpoint(size_t edgeId) ROSE_DEPRECATED("use toSourceEndpoint");
    static EndpointId edgeToTargetEndpoint(size_t edgeId) ROSE_DEPRECATED("use toTargetEndpoint");
    static size_t edgeFromEndpoint(EndpointId) ROSE_DEPRECATED("use fromEndpoint");

    /** Tests whether endpoint ID is known.
     *
     *  Returns true if the specified endpoint is part of the edge arrow information, false if the ID is unknown. */
    bool exists(EndpointId) const;

    /** Finds the arrow information for the specified edge. */
    Sawyer::Optional<Arrow> findArrow(ArrowId) const;

    /** Reset the analysis.
     *
     *  This clears previous layout information but not rendering information. I.e., the arrows are all erased, but the arrow
     *  style and other properties are not affected. */
    void reset();

    /** Number of arrow columns.
     *
     *  This is the number of logical columns needed for the arrows. An arrow occupies a single logical column, but depending on the
     *  arrow style, the rendered arrow might occupy multiple physical columns of output. The return value is not limited by the
     *  @ref minMaxRenderColumns property.
     *
     *  If you want the number of columns of characters (rather than arrows) you should multiply the return value of this
     *  function by @c arrowStyle.charactersPerColumn. */
    size_t nArrowColumns() const;

    /** Number of columns to render.
     *
     *  This is the number of logical arrow columns actually rendered. The return value is a function of @ref nArrowColumns and @ref
     *  minMaxRenderColumns. Each logical column may refer to multiple physical columns depending on the arrow style. */
    size_t nRenderColumns() const;

    /** Property: Information about how to render an arrow.
     *
     * @{ */
    const ArrowStyle& arrowStyle() const { return arrowStyle_; }
    void arrowStyle(const ArrowStyle &t) { arrowStyle_ = t; }
    /** @} */

    /** Set the arrow style to a preset value. */
    void arrowStyle(ArrowStylePreset, ArrowSide);

    /** Arrow rendering styles.
     *
     *  These functions have names that follow a regular pattern. The first part of the name describes the character set used
     *  for the arrows. The letters "R" and "L" indicate whether the arrows are to the left or right of the vertices to which
     *  they refer (opposite the direction the arrows point). The number indicates the number of characters per column of
     *  arrows.  The wider formats are easier to read but take up more output space.
     *
     *  To set the arrow style, use code like this:
     *
     * @code
     *   arrows.arrowStyle(unicodeL2());
     * @endcode
     *
     * @{ */
    static ArrowStyle unicodeL1();
    static ArrowStyle unicodeR1();
    static ArrowStyle unicodeL2();
    static ArrowStyle unicodeR2();
    static ArrowStyle asciiL1();
    static ArrowStyle asciiR1();
    static ArrowStyle asciiL2();
    static ArrowStyle asciiR2();
    static ArrowStyle asciiL3();
    static ArrowStyle asciiR3();
    static ArrowStyle asciiL4();
    static ArrowStyle asciiR4();
    /** @} */

    /** Render a field of blank characters for all the columns.
     *
     *  Use this to keep tabular output aligned when you're only reserving space for the arrow columns and not actually printing any
     *  arrows. */
    std::string renderBlank() const;

    /** Render arrow columns for an endpoint. */
    std::string render(EndpointId, OutputPart) const;

    /** Number of arrows that emanate from the given endpoint.
     *
     *  Normally you'd use the original graph to answer this question, but sometimes that graph is long gone. In those cases,
     *  you can use this slower function to find the number of arrows that originate from the specified endpoint. */
    size_t nSources(EndpointId) const;

    /** Number of arrows that point to the given endpoint.
     *
     *  Normally you'd use the original graph to answer this question, but sometimes that graph is long gone. In those cases,
     *  you can use this slower function to find the number of arrows that point to the specified endpoint. */
    size_t nTargets(EndpointId) const;

    /** Print implementation-defined debugging information.
     *
     * @{ */
    void debug(std::ostream&) const;
    void debugGraph(std::ostream&, const Graph&) const;
    void debugLines(std::ostream&, const std::vector<EndpointId>&) const;
    /** @} */

private:
    // Append an endpoint to the output. This doesn't actually create any output, it just reserves space for it.
    void appendEndpoint(EndpointId);

    // Is the length of arrow A less than the length of arrow B?
    static bool ascendingLength(const Arrow &a, const Arrow &b);

};

} // namespace
} // namespace
} // namespace

#endif
#endif
