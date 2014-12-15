#ifndef ROSE_Partitioner2_GraphViz_H
#define ROSE_Partitioner2_GraphViz_H

#include <ostream>
#include <Partitioner2/ControlFlowGraph.h>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

class GraphViz {
public:
    struct HsvColor;

    struct RgbColor {
        double r, g, b;                                 // assume alpha is always 1.0
        RgbColor(double r, double g, double b): r(r), g(g), b(b) {}
        RgbColor(uint8_t r, uint8_t g, uint8_t b): r(r/255.0), g(g/255.0), b(b/255.0) {}
        RgbColor(const HsvColor&);                      // implicit
        std::string toString() const;                   // returns HTML color spec like #56abff
        RgbColor invert() const;                        // invert value in HSV space
    };

    struct HsvColor {
        double h, s, v;                                 // always in the range [0..1]
        HsvColor(double h, double s, double v): h(h), s(s), v(v) {}
        HsvColor(const RgbColor&);                      // implicit
        std::string toString() const;                   // returns HTML color spec like #56abff
        HsvColor invert() const;                        // invert value without changing hue or saturation
    };

    typedef Sawyer::Container::Map<std::string, std::string> Attributes;
    
private:
    typedef Sawyer::Container::Map<ControlFlowGraph::ConstVertexNodeIterator, size_t> VMap;
    std::vector<bool> selected_;                        // which vertices to select for output
    bool useFunctionSubgraphs_;                         // should called functions be shown as subgraphs?
    bool showReturnEdges_;                              // show E_FUNCTION_RETURN edges?
    bool showInstructions_;                             // show instructions or only block address?
    bool showInstructionAddresses_;                     // if instructions are shown, show addresses too?
    bool showInNeighbors_;                              // show neighbors for incoming edges to selected vertices?
    bool showOutNeighbors_;                             // show neighbors for outgoing edges to selected vertices?
    HsvColor subgraphColor_;                            // background color for function subgraphs
    HsvColor funcEnterColor_;                           // background color for function entrance blocks
    HsvColor funcReturnColor_;                          // background color for function return blocks
    HsvColor warningColor_;                             // background color for special nodes and warnings
    Attributes defaultNodeAttributes_;                  // default attributes for graph nodes (CFG vertices and other)
    Attributes defaultEdgeAttributes_;                  // default attributes for graph edges

    mutable VMap vmap_;                                 // maps CFG vertices to GraphViz vertex IDs (modified when dumping)

public:
    GraphViz()
        : useFunctionSubgraphs_(true), showReturnEdges_(true), showInstructions_(false), showInstructionAddresses_(true),
          showInNeighbors_(true),
          showOutNeighbors_(true),
          subgraphColor_(0, 0, 0.95),                   // light grey
          funcEnterColor_(0.33, 1.0, 0.9),              // light green
          funcReturnColor_(0.67, 1.0, 0.9),             // light blue
          warningColor_(0, 1.0, 0.80)                   // light red
        {}

    /** Property: use function subgraphs.
     *
     *  When this property is true, functions will be organized into subgraphs in the GraphViz output, otherwise no subgraphs
     *  are used and all Graphviz vertices will be in a single top-level graph.
     *
     * @{ */
    bool useFunctionSubgraphs() const { return useFunctionSubgraphs_; }
    void useFunctionSubgraphs(bool b) { useFunctionSubgraphs_ = b; }
    /** @} */

    /** Property: show function return edges.
     *
     *  When this property is true, edges of type @ref E_FUNCTION_RETURN are included in the graph, otherwise they are
     *  omitted.
     *
     * @{ */
    bool showReturnEdges() const { return showReturnEdges_; }
    void showReturnEdges(bool b) { showReturnEdges_ = b; }
    /** @} */

    /** Property: show basic block instructions.
     *
     *  When true the GraphViz vertex labels will be disassembled instructions, otherwise the labels will be only an address.
     *  The @ref vertexLabel method is responsible for querying this property and doing the appropriate thing.
     *
     * @{ */
    bool showInstructions() const { return showInstructions_; }
    void showInstructions(bool b) { showInstructions_ = b; }
    /** @} */

    /** Property: show instruction addresses.
     *
     *  When true and basic block instructions are shown, show the address of each instruction.
     *
     * @{ */
    bool showInstructionAddresses() const { return showInstructionAddresses_; }
    void showInstructionAddresses(bool b) { showInstructionAddresses_ = b; }
    /** @} */

    /** Property: default graph node attributes.
     *
     *  Attributes that should apply to all graph nodes.
     *
     * @{ */
    Attributes& defaultNodeAttributes() { return defaultNodeAttributes_; }
    const Attributes& defaultNodeAttributes() const { return defaultNodeAttributes_; }
    /** @} */

    /** Property: default graph edge attributes.
     *
     *  Attributes that should apply to all graph edges.
     *
     * @{ */
    Attributes& defaultEdgeAttributes() { return defaultEdgeAttributes_; }
    const Attributes& defaultEdgeAttributes() const { return defaultEdgeAttributes_; }
    /** @} */

    /** Restricts selection of vertices.
     *
     *  If the selection vector is initialized then only vertices marked as being selected are produced in the output. The
     *  default is to not suppress the output of any vertex.
     *
     *  The @ref selectAll method resets the selection vector, after which @ref selected will return an empty vector and @ref
     *  isSelected will always return true.
     *
     * @sa showInNeighbors @ref showOutNeighbors
     *
     * @{ */
    const std::vector<bool> selected() const { return selected_; }
    void selected(const std::vector<bool> &v) { selected_ = v; }
    void selectAll() { selected_.clear(); }
    bool isSelected(const Partitioner&, const ControlFlowGraph::ConstVertexNodeIterator&) const;
    void select(const Partitioner&, const ControlFlowGraph::ConstVertexNodeIterator&, bool selected=true);
    /** @} */

    /** Property: show unselected outgoing neighbors.
     *
     *  When true and only a subset of the vertices are selected for output, emit a placeholder vertex for edges that originate
     *  at a selected vertex and terminate at an unselected vertex. Otherwise such edges (and their target vertex) are not
     *  emitted to the GraphViz file.
     *
     * @{ */
    bool showOutNeighbors() const { return showOutNeighbors_; }
    void showOutNeighbors(bool b) { showOutNeighbors_ = b; }
    /** @} */

    /** Property: show unselected incoming neighbors.
     *
     *  When true and only a subset of the vertices are selected for output, emit a placeholder vertex for edges that originate
     *  at an unselected vertex and terminate at a selected vertex. Otherwise such edges (and their source vertex) are not
     *  emitted to the GraphViz file.
     *
     * @{ */
    bool showInNeighbors() const { return showInNeighbors_; }
    void showInNeighbors(bool b) { showInNeighbors_ = b; }
    /** @} */

    /** Property: color to use for function subgraph background.
     *
     * @{ */
    const HsvColor& subgraphColor() const { return subgraphColor_; }
    void subgraphColor(const HsvColor &bg) { subgraphColor_ = bg; }
    /** @} */

    /** Property: color to use for background of function entrance nodes.
     *
     *  Edges to these nodes are created by converting the background color to a foreground color by inverting the value
     *  in HSV space (keeping the same hue and saturation).
     *
     * @{ */
    const HsvColor& funcEnterColor() const { return funcEnterColor_; }
    void funcEnterColor(const HsvColor &bg) { funcEnterColor_ = bg; }
    /** @} */

    /** Property: color to use for background of function return nodes.
     *
     * @{ */
    const HsvColor& funcReturnColor() const { return funcReturnColor_; }
    void funcReturnColor(const HsvColor &bg) { funcReturnColor_ = bg; }
    /** @} */

    /** Property: color to use for background of special nodes and for warnings.
     *
     * @{ */
    const HsvColor& warningColor() const { return warningColor_; }
    void warningColor(const HsvColor &bg) { warningColor_ = bg; }
    /** @} */

    /** Dump entire control flow graph.
     *
     *  If a vertex selection vector has been supplied (i.e., @ref selected returns a non-empty vector) then only those
     *  vertices that are selected will be emitted, and their neighbors if the @ref showInNeighbors and/or @ref
     *  showOutNeighbors properties is set. */
    void dumpCfgAll(std::ostream&, const Partitioner&) const;

    /** Dump control flow graph for one function. */
    void dumpCfgFunction(std::ostream&, const Partitioner&, const Function::Ptr&) const;

    /** Dump vertices having certain addresses.
     *
     *  Produces a GraphViz file containing the basic blocks that begin within a certain address interval and optionally
     *  abbreviated nodes for neighboring vertices depending on the @ref showInNeighbors and @ref showOutNeighbors
     *  properties. Upon return the list of selected vertices returned by the @ref selected and @ref isSelected methods will be
     *  those vertices whose starting address is within the specified interval. */
    void dumpCfgInterval(std::ostream&, const Partitioner&, const AddressInterval&);

    /** Dump function call graph.
     *
     *  Produces a function call graph by emitting a GraphViz dot file to the specified stream. */
    void dumpCallGraph(std::ostream&, const Partitioner&) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Utility methods
protected:
    // Invalid identification number
    static const size_t NO_ID = -1;

    // Escape things that need to be escaped for GraphViz double quoted values.
    static std::string quotedEscape(const std::string&);

    // Escape things that need to be escaped for GraphViz HTML strings
    static std::string htmlEscape(const std::string&);

    // Escape some value for GraphViz
    static std::string escape(const std::string&);

    // Convert attributes to strings
    static std::string toString(const Attributes&);

    // True if s forms a valid GraphViz ID
    static bool isId(const std::string &s);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Vertex methods
protected:
    // Complete label for vertex, including double quotes or angle brackets with proper escaping of contents. May render to
    // more than one line.
    virtual std::string vertexLabel(const Partitioner&, const ControlFlowGraph::ConstVertexNodeIterator&) const;

    // Simple, short label including double quotes or angle brackets with proper escaping of contents. Usually a single line,
    // or maybe two but not much more.
    virtual std::string vertexLabelSimple(const Partitioner&, const ControlFlowGraph::ConstVertexNodeIterator&) const;

    // GraphViz attributes for a vertex
    virtual Attributes vertexAttributes(const Partitioner&, const ControlFlowGraph::ConstVertexNodeIterator&) const;

    // Emit a single vertex if it hasn't already been emitted and if it's a selected vertex (all vertices are selected by
    // default; see select())
    virtual size_t dumpVertex(std::ostream&, const Partitioner&, const ControlFlowGraph::ConstVertexNodeIterator&) const;

    // Emit a single vertex if it hasn't already been emitted, regardless of whether it's selected.  The vertex label will be
    // the simple, short format.  This method is used to emit vertices that are neighbors to selected vertices.
    virtual size_t dumpVertexInfo(std::ostream&, const Partitioner&, const ControlFlowGraph::ConstVertexNodeIterator&) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Edge methods
protected:
    // Label for edge including double quotes or angle brackets with proper escaping of contents.
    virtual std::string edgeLabel(const Partitioner&, const ControlFlowGraph::ConstEdgeNodeIterator&) const;

    // GraphViz attributes for an edge.
    virtual Attributes edgeAttributes(const Partitioner&, const ControlFlowGraph::ConstEdgeNodeIterator&) const;

    // Emit an edge if the edge hasn't already been emited and if its incident vertices are already emitted.
    virtual bool dumpEdge(std::ostream&, const Partitioner&, const ControlFlowGraph::ConstEdgeNodeIterator&) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Function methods
protected:
    // Label for function including double quotes or angle brackets with proper escaping of contents.
    virtual std::string functionLabel(const Partitioner&, const Function::Ptr&) const;

    // GraphViz attributes for function vertex.
    virtual Attributes functionAttributes(const Partitioner&, const Function::Ptr&) const;

    // Emit a function GraphViz node unless one has already been emitted. The node is emitted regardless of whether the vertex
    // address is selected, therefore this method can be used to emit neighbors of selected vertices.
    virtual size_t dumpFunctionInfo(std::ostream&, const Partitioner&, const ControlFlowGraph::ConstVertexNodeIterator&) const;

    // Emit GraphViz nodes and edges for functions that call the specified function.  The source vertices are the caller entry
    // vertices (dumped with dumpFunctionInfo()) rather than the actual call sites.
    virtual void dumpFunctionCallers(std::ostream&, const Partitioner&, const Function::Ptr &) const;

    // Emit GraphViz nodes for functions that the specified function calls.  This uses dumpFunctionInfo().
    virtual void dumpFunctionCallees(std::ostream&, const Partitioner&, const Function::Ptr &) const;

    // Emit selected function vertices by calling dumpVertex(). Emit intra-function edges by calling emitEdge().
    virtual void dumpIntraFunction(std::ostream&, const Partitioner&, const Function::Ptr&) const;

    // Emit edges that originate in the specified function but terminate outside the function.  No new vertices are created.
    virtual void dumpInterFunctionOutEdges(std::ostream&, const Partitioner&, const Function::Ptr&) const;
    
};

} // namespace
} // namespace
} // namespace

#endif
