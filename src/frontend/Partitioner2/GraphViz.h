#ifndef ROSE_Partitioner2_GraphViz_H
#define ROSE_Partitioner2_GraphViz_H

#include <ostream>
#include <Partitioner2/ControlFlowGraph.h>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

class GraphViz {
public:
    struct RgbColor {
        double r, g, b;
        RgbColor(double red, double green, double blue)
            : r(red), g(green), b(blue) {}
        RgbColor(uint8_t red, uint8_t green, uint8_t blue)
            : r(red/255.0), g(green/255.0), b(blue/255.0) {}
        std::string toString() const;
    };

private:
    typedef Sawyer::Container::Map<ControlFlowGraph::ConstVertexNodeIterator, size_t> VMap;
    VMap vmap_;                                         // maps CFG vertices to GraphViz vertex IDs
    std::vector<bool> selected_;                        // which vertices to select for output
    bool useFunctionSubgraphs_;                         // should called functions be shown as subgraphs?
    bool showReturnEdges_;                              // show E_FUNCTION_RETURN edges?
    bool showInstructions_;                             // show instructions or only block address?
    bool showNeighbors_;                                // show neighbors for un-selected vertices?
    RgbColor subgraphColor_;                            // color for function subgraphs

public:
    GraphViz()
        : useFunctionSubgraphs_(true), showReturnEdges_(true), showInstructions_(false), showNeighbors_(true),
          subgraphColor_(0.9, 0.9, 0.9) {}

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

    /** Restricts selection of vertices.
     *
     *  If the selection vector is initialized then only vertices marked as being selected are produced in the output. The
     *  default is to not suppress the output of any vertex.
     *
     *  The @ref selectAll method resets the selection vector, after which @ref selected will return an empty vector and @ref
     *  isSelected will always return true.
     *
     * @sa showNeighbors
     *
     * @{ */
    const std::vector<bool> selected() const { return selected_; }
    void selected(const std::vector<bool> &v) { selected_ = v; }
    void selectAll() { selected_.clear(); }
    bool isSelected(const Partitioner&, const ControlFlowGraph::ConstVertexNodeIterator&) const;
    void select(const Partitioner&, const ControlFlowGraph::ConstVertexNodeIterator&, bool selected=true);
    /** @} */

    /** Property: show unselected neighbors.
     *
     *  When true and only a subset of the vertices are selected for output, emit a placeholder vertex for edges that originate
     *  at a selected vertex and terminate at an unselected vertex. Otherwise such edges (and their target vertex) are not
     *  emitted to the GraphViz file.
     *
     * @{ */
    bool showNeighbors() const { return showNeighbors_; }
    void showNeighbors(bool b) { showNeighbors_ = b; }
    /** @} */

    /** Property: color to use for function subgraph background.
     *
     * @{ */
    RgbColor subgraphColor() const { return subgraphColor_; }
    void subgraphColor(const RgbColor &bg) { subgraphColor_ = bg; }
    /** @} */

    /** Dump entire control flow graph.
     *
     *  If a vertex selection vector has been supplied (i.e., @ref selected returns a non-empty vector) then only those
     *  vertices that are selected will be emitted, and their neighbors if the @ref showNeighbors property is set. */
    void dumpCfgAll(std::ostream&, const Partitioner&);

    /** Dump control flow graph for one function. */
    void dumpCfgFunction(std::ostream&, const Partitioner&, const Function::Ptr&);

    /** Dump vertices having certain addresses.
     *
     *  Produces a GraphViz file containing the basic blocks that begin within a certain address interval and optionally
     *  abbreviated nodes for neighboring vertices depending on the @ref showNeighbors property. Upon return the list of
     *  selected vertices returned by the @ref selected and @ref isSelected methods will be those vertices whose starting
     *  address is within the specified interval. */
    void dumpCfgInterval(std::ostream&, const Partitioner&, const AddressInterval&);

    /** Dump function call graph.
     *
     *  Produces a function call graph by emitting a GraphViz dot file to the specified stream. */
    void dumpCallGraph(std::ostream&, const Partitioner&);

protected:
    static std::string labelEscape(const std::string&);

    virtual std::string vertexLabel(const Partitioner&, const ControlFlowGraph::ConstVertexNodeIterator&);
    virtual std::string vertexLabelSimple(const Partitioner&, const ControlFlowGraph::ConstVertexNodeIterator&);
    virtual std::string vertexAttributes(const Partitioner&, const ControlFlowGraph::ConstVertexNodeIterator&);
    virtual size_t dumpVertex(std::ostream&, const Partitioner&, const ControlFlowGraph::ConstVertexNodeIterator&);

    virtual std::string edgeLabel(const Partitioner&, const ControlFlowGraph::ConstEdgeNodeIterator&);
    virtual std::string edgeAttributes(const Partitioner&, const ControlFlowGraph::ConstEdgeNodeIterator&);
    virtual bool dumpEdge(std::ostream&, const Partitioner&, const ControlFlowGraph::ConstEdgeNodeIterator&);

    virtual std::string functionLabel(const Partitioner&, const Function::Ptr&);
    virtual std::string functionAttributes(const Partitioner&, const Function::Ptr&);
    virtual size_t dumpFunctionInfo(std::ostream&, const Partitioner&, const ControlFlowGraph::ConstVertexNodeIterator&);

    virtual void dumpFunctionCallees(std::ostream&, const Partitioner&, const Function::Ptr &);
    virtual void dumpIntraFunction(std::ostream&, const Partitioner&, const Function::Ptr&);
    virtual void dumpInterFunctionEdges(std::ostream&, const Partitioner&, const Function::Ptr&);
    
};

} // namespace
} // namespace
} // namespace

#endif
