#ifndef Rose_BinaryAnalysis_Unparser_EdgeArrows_H
#define Rose_BinaryAnalysis_Unparser_EdgeArrows_H

#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include <Partitioner2/BasicTypes.h>
#include <Sawyer/Graph.h>
#include <Sawyer/Interval.h>
#include <Sawyer/IntervalMap.h>
#include <Sawyer/Map.h>
#include <StringUtility.h>
#include <ostream>
#include <vector>

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Basic block arrow information
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Analysis to generate arrows from one basic block to another.
 *
 * Oftentimes there's a desire to print arrows from one basic block to another to represent control flow, dependencies, other
 * other analysis results.  This class is meant to be a general-purpose way to compute the start end end of arrows using a
 * variety of layouts. The layout that's currently implemented assumes a line-oriented output where each basic block occupies
 * at least two lines. The first line of the basic block is for edges going into the block (where the block is the target of an
 * edge), and the last line is for edges leaving the block. */
class EdgeArrows {
public:
    /** Parts of the output. */
    enum OutputPart {
        FIRST_LINE,                                     /**< First line of a vertex when printed. */
        LAST_LINE,                                      /**< Last line of a vertex when printed. */
        MIDDLE_LINE,                                    /**< Lines in the middle of a printed vertex. */
        INTER_LINE                                      /**< Lines output between vertices. */
    };

    /** Vertex identification numbers. */
    typedef rose_addr_t VertexId;

    /** Graph relating vertices by their edges with vertex index. */
    typedef Sawyer::Container::Graph<VertexId, Sawyer::Nothing, VertexId> Graph;

    /** Arrow text. */
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

    /** On which side of the listing do the errors appear. */
    enum ArrowSide {
        LEFT,                                           /**< Arrows appear left of the listing and point to the right. */
        RIGHT                                           /**< Arrows appear right of the listing and point to the left. */
    };
    
private:
    // Location of a vertex in the output. Each vertex is assumed to occupy four lines: a first line for incoming edges,
    // a middle line representative of all lines of output between the first and last lines, a last line for outgoing
    // edges, and a separator line representative of all lines of output between vertices.
    typedef Sawyer::Container::Interval<size_t> OutputLocation;

    // Mapping from vertex ID to its location in the output.
    typedef Sawyer::Container::Map<VertexId, OutputLocation> VertexLocations;

    // Edge properties
    struct Arrow {
        OutputLocation location;                        // line numbers occupied by the arrow
        bool isForward;                                 // arrow points to a later vertex?

        Arrow()
            : isForward(true) {}

        Arrow(const OutputLocation &location, bool isForward)
            : location(location), isForward(isForward) {}

        bool operator==(const Arrow &other) {
            return location == other.location && isForward == other.isForward;
        }
    };
    
    // One column of arrows. Maps line numbers of output to line numbers of edges.
    typedef Sawyer::Container::IntervalMap<OutputLocation /*line number*/, Arrow> Column;

    // Columns of arrows
    typedef std::vector<Column> Columns;

private:
    OutputLocation outputHull_;                         // entire output
    VertexLocations vertexLocations_;                   // location of each vertex in the output
    Columns columns_;                                   // columns of arrows
    ArrowStyle arrowStyle_;                             // how to render arrows

public:
    EdgeArrows() {
        arrowStyle_ = asciiL3();                        // maximum portability but slightly less clarity than unicodeL2
    }
    
    /** Analyze connectivity in order to assign arrow locations.
     *
     *  Analyzes the connectivity of the vertices specified in the @p graph, and the optional vertex @p order in order to
     *  calculate the locations of the margin arrows.  The vertices of the graph are assumed to be output in the order listed
     *  in the @p order argument. Any vertices that are present in the @p graph but not in the @p order specification are
     *  sorted by ID and then appended to the @p order.
     *
     *  This does not clear previous layout information, and so it can be used to append to previous output. To discard
     *  previous layout information first, call @ref reset. */
    void computeLayout(const Graph&, const std::vector<VertexId> &order = std::vector<VertexId>());

    /** Compute arrow layout for a control flow graph for a function.
     *
     *  This produces arrows for intra-function CFG edges such that the arrows' sources and targets are basic blocks. That is,
     *  in the unparser output, arrows will point to the first instruction of a basic block and emanate from the last
     *  instruction.  This method assumes that the unparser emits a function's basic blocks in order of their starting address.
     *
     *  See also, @ref computeCfgEdgeLayout */
    void computeCfgBlockLayout(const Partitioner2::Partitioner&, const Partitioner2::FunctionPtr&);

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
    void computeCfgEdgeLayout(const Partitioner2::Partitioner&, const Partitioner2::FunctionPtr&);

    /** Endpoint ID for CFG edge arrows.
     *
     *  Returns the endpoint ID for the specified CFG edge. These are the IDs that correspond to those used by @ref
     *  computeCfgEdgeLayout.  The edge must be a valid edge, not an end iterator.
     *
     * @{ */
    static VertexId cfgEdgeSourceEndpoint(size_t edgeId);
    static VertexId cfgEdgeTargetEndpoint(size_t edgeId);
    /** @} */

    /** Reset the analysis.
     *
     *  This clears previous layout information but not rendering information. I.e., the arrows are all erased, but the arrow
     *  style and other properties are not affected. */
    void reset();

    /** Number of arrow columns.
     *
     *  This is the number of columns needed for the arrows measured in terms of arrows.
     *
     *  If you want the number of columns of characters (rather than arrows) you should multiply the return value of this
     *  function by @c arrowStyle.charactersPerColumn. */
    size_t nArrowColumns() const;

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

    /** Render a field of blank characters for all the columns. Use this to keep tabular output aligned when you're
     *  only reserving space for the arrow columns and not actually printing any arrows. */
    std::string renderBlank() const;

    /** Render arrow columsn for a vertex. */
    std::string render(VertexId, OutputPart) const;

    /** Number of arrows that emanate from the given vertex.
     *
     *  Normally you'd use the original graph to answer this question, but sometimes that graph is long gone. In those cases,
     *  you can use this slower function to find the number of arrows that originate from the specified vertex. */
    size_t nSources(VertexId) const;

    /** Number of arrows that point to the given vertex.
     *
     *  Normally you'd use the original graph to answer this question, but sometimes that graph is long gone. In those cases,
     *  you can use this slower function to find the number of arrows that point to the specified vertex. */
    size_t nTargets(VertexId) const;

    /** Print implementation-defined debugging information. */
    void debug(std::ostream&) const;
    
private:
    // Append a vertex to the output. This doesn't actually create any output, it just reserves space for it.
    void appendVertex(VertexId);

    // Is the length of arrow A less than the length of arrow B?
    static bool ascendingLength(const Arrow &a, const Arrow &b);

};

} // namespace
} // namespace
} // namespace

#endif
#endif
