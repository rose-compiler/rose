#ifndef ROSE_Partitioner2_GraphViz_H
#define ROSE_Partitioner2_GraphViz_H

#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include <ostream>
#include <BinaryNoOperation.h>
#include <boost/regex.hpp>
#include <Color.h>
#include <DwarfLineMapper.h>
#include <Partitioner2/ControlFlowGraph.h>
#include <Partitioner2/FunctionCallGraph.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

/** Support for generating and reading GraphViz output. */
namespace GraphViz {

/** GraphViz attributes.
 *
 *  Attributes are name/value pairs where the valid names are defined in the GraphViz language. */
typedef Sawyer::Container::Map<std::string, std::string> Attributes;

/** Convert attributes to GraphViz language string. */
ROSE_DLL_API std::string toString(const Attributes&);

/** Escape characters that need to be escaped within GraphViz double quoted literals. */
ROSE_DLL_API std::string quotedEscape(const std::string&);

/** Escape characters that need to be escaped within GraphViz HTML literals. */
ROSE_DLL_API std::string htmlEscape(const std::string&);

/** Escape some value for GraphViz.
 *
 *  The returned string will include double quote or angle-brackets as necessary depending on the input string. */
ROSE_DLL_API std::string escape(const std::string&);

/** Append a value to an existing string.
 *
 *  Appends @p newStuff to the end of @p oldStuff taking into account that @p oldStuff is already quoted and escaped. The @p
 *  newStuff should not be quoted or escaped.  This is useful for appending additional information to a label. The @p separator
 *  is escaped and inserted between the @p oldStuff and @p newStuff if @p oldStuff is not empty. Returns a new string that is
 *  also quoted and escaped. */
ROSE_DLL_API std::string concatenate(const std::string &oldStuff, const std::string &newStuff, const std::string &separator="");

/** Determins if a string is a valid GraphViz ID.
 *
 *  True if s forms a valid GraphViz ID.  ID strings do not need special quoting in the GraphViz language. */
ROSE_DLL_API bool isId(const std::string &s);

/** An invalid identification number. */
ROSE_DLL_API extern const size_t NO_ID;


/** Organizational information.
 *
 *  The organization determines which vertices, edges, and subgraphs are selected for output and also gives them labels and
 *  attributes.  Generally speaking, the GraphViz object will update labels and attributes automatically only when
 *  transitioning from an unselected to selected state. */
class ROSE_DLL_API Organization {
private:
    bool isSelected_;
    std::string name_;                                  // name used by GraphViz to identify this object
    std::string label_;                                 // label shown in rendered graph, includes delimiters, "" or <>
    Attributes attributes_;
    std::string subgraph_;
public:
    /** Default constructor.
     *
     *  Constructs an organization that selects the object (vertex, edge, or subgraph) and gives it an empty label and no
     *  attributes. */
    Organization(): isSelected_(true) {}

    /** Select or deselect object. */
    void select(bool b=true) { isSelected_ = b; }

    /** Determines whether an object is selected.
     *
     *  An object that is not selected will not appear in the output, and objects that are selected may appear in the
     *  output. Being selected is not sufficient to appear in the output. For instance, a selected edge will only appear if
     *  both incident vertices are also selected, and a selected subgraph will appear only if it has at least one selected
     *  vertex. */
    bool isSelected() const { return isSelected_; }

    /** Name for object.
     *
     *  This is the name used by GraphViz to denote this object.  If the name is empty, then an ID number is used instead.
     *
     * @{ */
    const std::string &name() const { return name_; }
    void name(const std::string &s) { name_ = s; }
    /** @} */

    /** Label for object.
     *
     *  The object label should either be empty, or must be a properly delimited and escaped value for the GraphViz
     *  language.  The accessor will always return a properly-delimited string if the value is empty.
     *
     * @{ */
    const std::string& label() const {
        static std::string empty = "\"\"";
        return label_.empty() ? empty : label_;
    }
    void label(const std::string &s) { label_ = s; }
    /** @} */

    /** Attributes for object.
     *
     *  Attributes are name/value pairs defined by the GraphViz language.
     *
     * @{ */
    const Attributes& attributes() const { return attributes_; }
    Attributes& attributes() { return attributes_; }
    void attributes(const Attributes &a) { attributes_ = a; }
    /** @} */

    /** Subgraph for object.
     *
     *  A vertex may belong to a subgraph. Subgraphs have names that must be valid GraphViz identifiers without the
     *  "cluster_" prefix.  An empty name means no subgraph.
     *
     * @{ */
    const std::string& subgraph() const { return subgraph_; }
    void subgraph(const std::string &s) { subgraph_ = s; }
    /** @} */
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      BaseEmitter
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Base class for generating GraphViz output.
 *
 *  The @p G template parameter is the type of graph for which output is being generated.  @p G must be a @c
 *  Sawyer::Container::Graph type. */
template<class G>
class BaseEmitter {
public:
    typedef G Graph;

    /** Organizational information for vertices. */
    typedef std::vector<Organization> VertexOrganization;

    /** Organizational information for edges. */
    typedef std::vector<Organization> EdgeOrganization;

    /** Organizational information for subgraphs. */
    typedef Sawyer::Container::Map<std::string, Organization> SubgraphOrganization;

protected:
    struct PseudoEdge {
        typename G::ConstVertexIterator src, dst;
        std::string label;
        Attributes attributes;
        PseudoEdge(const typename G::ConstVertexIterator &src, const typename G::ConstVertexIterator &dst,
                   const std::string &label)
            : src(src), dst(dst), label(label) {}
    };

    typedef Sawyer::Container::Map<size_t, size_t> VMap;// maps graph vertex ID to graphviz node ID

protected:
    Graph graph_;                                       // graph being emitted
    VertexOrganization vertexOrganization_;             // which vertices are selected for output
    EdgeOrganization edgeOrganization_;                 // which edges are selected for output
    SubgraphOrganization subgraphOrganization_;         // which subgraphs are selected for output
    Attributes defaultGraphAttributes_;                 // default attributes for the graph as a whole
    Attributes defaultNodeAttributes_;                  // default attributes for graph nodes (CFG vertices and other)
    Attributes defaultEdgeAttributes_;                  // default attributes for graph edges
    std::list<PseudoEdge> pseudoEdges_;                 // extra edges not present in the CFG but needed in the GraphViz
    Color::HSV subgraphColor_;                          // background color for function subgraphs

public:
    /** Default constructor.
     *
     *  Construct a GraphViz emitter having an empty graph. The @ref graph method can be called to give the emitter a new graph
     *  later. */
    BaseEmitter(): subgraphColor_(0, 0, 0.95) {}
    
    /** Constructor.
     *
     *  Construct a generator for the specified graph.  The graph is copied into this generator. */
    explicit BaseEmitter(const Graph &g)
        : subgraphColor_(0, 0, 0.95) {
        graph(g);
    }

    /** Reset the graph. */
    void graph(const Graph &g) {
        graph_ = g;
        vertexOrganization_.clear();
        vertexOrganization_.resize(g.nVertices());
        edgeOrganization_.clear();
        edgeOrganization_.resize(g.nEdges());
        subgraphOrganization_.clear();
        pseudoEdges_.clear();
    }

    /** Property: default graph attributes.
     *
     *  Attributes that should apply to the graph as a whole.
     *
     * @{ */
    Attributes& defaultGraphAttributes() {
        return defaultGraphAttributes_;
    }
    const Attributes& defaultGraphAttributes() const {
        return defaultGraphAttributes_;
    }
    /** @} */

    /** Property: default graph node attributes.
     *
     *  Attributes that should apply to all graph nodes.
     *
     * @{ */
    Attributes& defaultNodeAttributes() {
        return defaultNodeAttributes_;
    }
    const Attributes& defaultNodeAttributes() const {
        return defaultNodeAttributes_;
    }
    /** @} */

    /** Property: default graph edge attributes.
     *
     *  Attributes that should apply to all graph edges.
     *
     * @{ */
    Attributes& defaultEdgeAttributes() {
        return defaultEdgeAttributes_;
    }
    const Attributes& defaultEdgeAttributes() const {
        return defaultEdgeAttributes_;
    }
    /** @} */

    /** Property: color to use for function subgraph background.
     *
     * @{ */
    const Color::HSV& subgraphColor() const { return subgraphColor_; }
    void subgraphColor(const Color::HSV &bg) { subgraphColor_ = bg; }
    /** @} */

    /** Property: Controls which vertices are to appear in the output, and how.
     *
     *  Each vertex of the graph has an entry in this table, and the entry describes such things as whether the vertex will be
     *  present in the GraphViz file, which subgraph (if any) it will belong to, its label, and other attributes.
     *
     *  Most GraphViz-emitting methods modify this information and then call the basic @ref emit method. In general, if
     *  the user supplies a label or attribute prior to such calls then that information is used instead of calculating new
     *  information.
     *
     * @{ */
    const VertexOrganization& vertexOrganization() const {
        return vertexOrganization_;
    }
    VertexOrganization& vertexOrganization() {
        return vertexOrganization_;
    }
    const Organization& vertexOrganization(size_t vertexId) const {
        ASSERT_require(vertexId < vertexOrganization_.size());
        return vertexOrganization_[vertexId];
    }
    Organization& vertexOrganization(size_t vertexId) {
        ASSERT_require(vertexId < vertexOrganization_.size());
        return vertexOrganization_[vertexId];
    }
    const Organization& vertexOrganization(const typename G::ConstVertexIterator &vertex) const {
        return vertexOrganization(vertex->id());
    }
    const Organization& vertexOrganization(const typename G::Vertex &vertex) const {
        return vertexOrganization(vertex.id());
    }
    Organization& vertexOrganization(const typename G::ConstVertexIterator &vertex) {
        return vertexOrganization(vertex->id());
    }
    Organization& vertexOrganization(const typename G::Vertex &vertex) {
        return vertexOrganization(vertex.id());
    }
    /** @} */

    /** Property: Controls which edges are to appear in the output, and how.
     *
     *  Each edge of the CFG has an entry in this table, and the entry describes such things as whether the edge will be
     *  present in the GraphViz file, which subgraph (if any) it belongs to, its label, and other attributes.
     *
     *  Most GraphViz-emitting methods modify this information and then call the basic @ref emit method. In general, if
     *  the user supplies a label or attribute prior to such calls then that information is used instead of calculating new
     *  information.
     *
     * @{ */
    const EdgeOrganization& edgeOrganization() const {
        return edgeOrganization_;
    }
    EdgeOrganization& edgeOrganization() {
        return edgeOrganization_;
    }
    const Organization& edgeOrganization(size_t edgeId) const {
        ASSERT_require(edgeId < edgeOrganization_.size());
        return edgeOrganization_[edgeId];
    }
    Organization& edgeOrganization(size_t edgeId) {
        ASSERT_require(edgeId < edgeOrganization_.size());
        return edgeOrganization_[edgeId];
    }
    const Organization& edgeOrganization(const typename G::ConstEdgeIterator &edge) const {
        return edgeOrganization(edge->id());
    }
    const Organization& edgeOrganization(const typename G::Edge &edge) const {
        return edgeOrganization(edge.id());
    }
    Organization& edgeOrganization(const typename G::ConstEdgeIterator &edge) {
        return edgeOrganization(edge->id());
    }
    Organization& edgeOrganization(const typename G::Edge &edge) {
        return edgeOrganization(edge.id());
    }
    /** @} */

    /** Property: Controls which subgraphs appear in the output, and how.
     *
     *  Each subgraph has a distinct name consisting of only letters, numbers, and underscores. The table contains information
     *  about whether the subgraph is selected for output, its label, and other attributes.
     *
     *  Most GraphViz-emitting methods modify this information and then call the basic @ref emit method. In general, if
     *  the user supplies a label or attribute prior to such calls then that information is used instead of calculating new
     *  information.
     *
     * @{ */
    const SubgraphOrganization& subgraphOrganization() const {
        return subgraphOrganization_;
    }
    SubgraphOrganization& subgraphOrganization() {
        return subgraphOrganization_;
    }
    const Organization& subgraphOrganization(const std::string &name) const {
        return subgraphOrganization_.getOrDefault(name);
    }
    Organization& subgraphOrganization(const std::string &name) {
        return subgraphOrganization_.insertMaybeDefault(name);
    }
    /** @} */

    /** Causes all vertices and edges to be selected.
     *
     *  Causes all vertices and edges to be selected as the core part of the graph. If @p b is false then all vertices and
     *  edges are deselected instead. */
    void selectAll(bool b=true) {
        selectAllVertices(b);
        selectAllEdges(b);
    }

    /** Deselects all vertices and edges. */
    void selectNone() {
        pseudoEdges_.clear();
        selectAllEdges(false);
        selectAllVertices(false);
    }

    /** Causes all vertices to be selected.
     *  
     *  Causes all vertices to be selected as the core part of the graph. If @p b is false then all vertices are deselected
     *  instead. */
    void selectAllVertices(bool b=true) {
        BOOST_FOREACH (Organization &org, vertexOrganization_)
            org.select(b);
    }

    /** Causes all edges to be selected.
     *  
     *  Causes all edges to be selected as the core part of the graph. If @p b is false then all edges are deselected
     *  instead. */
    void selectAllEdges(bool b=true) {
        BOOST_FOREACH (Organization &org, edgeOrganization_)
            org.select(b);
    }

    /** Deselect all but one parallel edge.
     *
     *  For parallel edges between a pair of vertices, all but one is deselected.  Which one remains selected is arbitrary. */
    void deselectParallelEdges();
    
    /** Dump selected vertices, edges, and subgraphs.
     *
     *  This is the most basic emitter that produces an entire GraphViz file.  The graph will contain the selected vertices and
     *  edges organized into subgraphs according to the vertex, edge, and subgraph organization information. */
    virtual void emit(std::ostream&) const;

protected:
    /** Emit a single vertex if it hasn't been emitted already.
     *
     *  In any case, returns the GraphViz ID number for the vertex. */
    size_t emitVertex(std::ostream&, const typename G::ConstVertexIterator&, const Organization&, const VMap&) const;

    /** Emit a single edge.  The vertices must have been emitted already. */
    void emitEdge(std::ostream&, const typename G::ConstEdgeIterator&, const Organization&, const VMap&) const;

};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                              Base generator for Partitioner2::ControlFlowGraph
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Creates GraphViz files from Partitioner data.
 *
 *  This class operates in these steps:
 *
 *  @li Step 1: Configure properties for this object
 *  @li Step 2: Choose which vertices and edges should be displayed
 *  @li Step 3: Set labels and attributes
 *  @li Step 4: Emit GraphViz output
 *
 *  Step 2 (choosing vertices and edges) is accomplished via @ref vertexOrganization and @ref edgeOrganization methods and the
 *  @ref Organization::select method invoked on their return values. Step 3 (setting labels and other attributes) is performed
 *  the same way, but with @ref Organization::label and @ref Organization::attributes methods. This class also has a number of
 *  "select" methods that perform steps 2 and 3 at the same time; these methods adjust labels and attributes only when
 *  selecting a previously unselected vertex, edge, or subgraph--otherwise they use the label and attributes that were
 *  previously specified.  Step 4 is normally achieved by invoking the @ref emit method, but the class also defines other
 *  emitters that combine a "select" method of the same name with @ref emit.
 *
 * @code
 *  Partitioner partitioner = ...;
 *  Function::Ptr f1=..., f2=...;
 *  GraphViz::CfgEmitter gv(partitioner);
 *  gv.showInstructions(true);
 *  gv.useFunctionSubgraphs(true);
 *  gv.selectFunctionGraph(f1);
 *  gv.emit(std::cout);
 * @endcode */
class ROSE_DLL_API CfgEmitter: public BaseEmitter<ControlFlowGraph> {
    const Partitioner &partitioner_;
    bool useFunctionSubgraphs_;                         // should called functions be shown as subgraphs?
    bool showReturnEdges_;                              // show E_FUNCTION_RETURN edges?
    bool showInstructions_;                             // show instructions or only block address?
    bool showInstructionAddresses_;                     // if instructions are shown, show addresses too?
    bool showInstructionStackDeltas_;                   // show stack deltas for instructions
    bool showInNeighbors_;                              // show neighbors for incoming edges to selected vertices?
    bool showOutNeighbors_;                             // show neighbors for outgoing edges to selected vertices?
    bool strikeNoopSequences_;                          // render no-op sequences in a different style
    Color::HSV funcEnterColor_;                         // background color for function entrance blocks
    Color::HSV funcReturnColor_;                        // background color for function return blocks
    Color::HSV warningColor_;                           // background color for special nodes and warnings
    DwarfLineMapper srcMapper_;                         // maps addresses to source code (optional)
    static unsigned long versionDate_;                  // date code from "dot -V", like 20100126
    NoOperation noOpAnalysis_;

public:
    /** Constructor.
     *
     *  Constructs a GraphViz emitter that uses the control flow graph (and possibly other data structures) from the specified
     *  partitioner, or the provided control flow graph which must be compatible with the partitioner. The GraphViz object will
     *  hold a reference to the partitioner, therefore the partitioner should not be deleted before the GraphViz object.
     *
     * @{ */
    explicit CfgEmitter(const Partitioner&);
    CfgEmitter(const Partitioner&, const ControlFlowGraph&);
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties

    /** Property: partitioner.
     *
     *  The partitioner that's being used, set when this emitter was constructed. */
    const Partitioner& partitioner() { return partitioner_; }

    /** Property: use function subgraphs.
     *
     *  When this property is true, GraphViz-emitting methods that adjust the vertex, edge, and/or subgraph organization will
     *  create new subgraphs for each function that has selected vertices. The subgraphs will correspond to functions. Vertices
     *  that don't appear in any function will likewise not belong to any subgraph.
     *
     * @{ */
    bool useFunctionSubgraphs() const { return useFunctionSubgraphs_; }
    void useFunctionSubgraphs(bool b) { useFunctionSubgraphs_ = b; }
    /** @} */

    /** Property: show basic block instructions.
     *
     *  When true, vertex labels computed by the GraphViz-emitting methods (as opposed to labels set by the user), will contain
     *  the disassembled instructions.  Otherwise such labels will consist of only the basic block starting address.  This
     *  property is expected to be honored by the @ref vertexLabel virtual method.
     *
     * @{ */
    bool showInstructions() const { return showInstructions_; }
    void showInstructions(bool b) { showInstructions_ = b; }
    /** @} */

    /** Property: show instruction addresses.
     *
     *  When true and basic block instructions are shown (@ref showInstructions), then the address of each instruction is also
     *  rendered.  This property is expected to be honored by the @ref vertexLabel virtual method.
     *
     * @{ */
    bool showInstructionAddresses() const { return showInstructionAddresses_; }
    void showInstructionAddresses(bool b) { showInstructionAddresses_ = b; }
    /** @} */

    /** Property: show instruction stack deltas.
     *
     *  When true and basic block instructions are shown (@ref showInstructions), then each instruction will be preceeded by a
     *  two-digit (or more) hexadecimal stack offset. The stack delta appears after the address (if any).  This property is
     *  expected to be honored by the @ref vertexLabel virtual method.
     *
     * @{ */
    bool showInstructionStackDeltas() const { return showInstructionStackDeltas_; }
    void showInstructionStackDeltas(bool b) { showInstructionStackDeltas_ = b; }
    /** @} */

    /** Property: strike no-op sequences.
     *
     *  Those instructions that are part of a no-op sequence are rendered in a different font (such as strike-through). When
     *  two or more sequences overlap, the largest sequence is struck and all overlapping sequences are not processed. For
     *  nested squences, this causes all instructions to be struck; for overlapping but non-nested sequences, only the largest
     *  one is struck.
     *
     * @{ */
    bool strikeNoopSequences() const { return strikeNoopSequences_; }
    void strikeNoopSequences(bool b) { strikeNoopSequences_ = b; }
    /** @} */


    /** Property: color to use for background of function entrance nodes.
     *
     *  Edges to these nodes are created by converting the background color to a foreground color by inverting the value
     *  in HSV space (keeping the same hue and saturation).
     *
     * @{ */
    const Color::HSV& funcEnterColor() const { return funcEnterColor_; }
    void funcEnterColor(const Color::HSV &bg) { funcEnterColor_ = bg; }
    /** @} */

    /** Property: color to use for background of function return nodes.
     *
     * @{ */
    const Color::HSV& funcReturnColor() const { return funcReturnColor_; }
    void funcReturnColor(const Color::HSV &bg) { funcReturnColor_ = bg; }
    /** @} */

    /** Property: color to use for background of special nodes and for warnings.
     *
     * @{ */
    const Color::HSV& warningColor() const { return warningColor_; }
    void warningColor(const Color::HSV &bg) { warningColor_ = bg; }
    /** @} */

    /** Property: show outgoing edges to neighbor vertices.
     *
     *  If set, then a function selector will also select inter-function edges originating from the selected function, and the
     *  incident vertices for those edges.  Most of the time the incident vertices will be entry points for called functions.
     *
     * @{ */
    bool showOutNeighbors() const { return showOutNeighbors_; }
    void showOutNeighbors(bool b) { showOutNeighbors_ = b; }
    /** @} */

    /** Property: show incoming edges from neighbor vertices.
     *
     *  If set, then a function selector will also select inter-function edges targeting the selected function, and the
     *  incident vertices for those edges. Under certain (usual) circumstances, all incident vertices from the same function
     *  are collapsed into a single function vertex and the parallel edges are replaced with a single edge having a label
     *  indicating the number of original edges and their types.
     *
     * @{ */
    bool showInNeighbors() const { return showInNeighbors_; }
    void showInNeighbors(bool b) { showInNeighbors_ = b; }
    /** @} */

    /** Property: show function return edges.
     *
     *  If this property is false, then selector functions that normally add @ref E_FUNCTION_RETURN edges to the output will
     *  omit those edges instead by calling @ref deselectReturnEdges.
     *
     * @{ */
    bool showReturnEdges() const { return showReturnEdges_; }
    void showReturnEdges(bool b) { showReturnEdges_ = b; }
    /** @} */

    /** Property: Address-to-source mapping.
     *
     *  If an address to source mapping is provided then source location information will be shown in each vertex.
     *
     * @{ */
    const DwarfLineMapper& srcMapper() const { return srcMapper_; }
    DwarfLineMapper& srcMapper() { return srcMapper_; }
    void srcMapper(const DwarfLineMapper &mapper) { srcMapper_ = mapper; }
    /** @} */


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Organization

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Low-level vertex and edge selection

    /** Selects vertices in some interval.
     *
     *  Selects all vertices whose starting address falls within the specified interval, then selects all edges that connect
     *  any two selected vertices (regardless of whether those vertices were just selected by this method or were previously
     *  selected). */
    void selectInterval(const AddressInterval&);

    /** Select vertices and intra-function edges for one function. */
    void selectIntraFunction(const Function::Ptr&);

    /** Select outgoing edges to neighboring vertices.
     *
     *  These edges typically represent calls to other functions. */
    void selectFunctionCallees(const Function::Ptr&);

    /** Select incoming edges from neighboring vertices.
     *
     *  These edges typically represent calls from other functions.  When possible, multiple edges from the same function will
     *  be replaced with a single pseudo-edge coming from that function's entry vertex. */
    void selectFunctionCallers(const Function::Ptr&);

    /** Deselect all function return edges.
     *
     *  Any edge of type @ref E_FUNCTION_RETURN is deselected. */
    void deselectReturnEdges();

    /** Deselect a vertex if it has no selected incident edges. */
    void deselectUnusedVertex(ControlFlowGraph::ConstVertexIterator);

    /** Deselect vertices of specified type if they have no selected incident edges. */
    void deselectUnusedVertexType(VertexType);

    /** Select neighboring vertices.
     *
     *  Selects vertices that are neighbors of selected vertices, and the edges that connect them. */
    void selectNeighbors(bool selectInEdges=true, bool selectOutEdges=true);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // High-level selectors

    /** Selects graph elements for whole-graph output.
     *
     *  (Re)selects all vertices and edges and gives them default names and attributes according to certain properties defined
     *  in this object.  The user then has a chance to make adjustments to the organization before calling @ref
     *  emitSelectedGraph.  The @ref emitWholeGraph does the selection and emitting in one step. */
    CfgEmitter& selectWholeGraph();

    /** Selects the CFG for one function.
     *
     *  Selects all vertices and edges that are part of the specified function.  Additionally, any inter-function edges to/from
     *  this function and their incident vertices are also selected according to @ref selectFunctionCallees and @ref
     *  selectFunctionCallers. */
    CfgEmitter& selectFunctionGraph(const Function::Ptr&);

    /** Selects vertices that start within some interval.
     *
     *  Selects all vertices whose starting address falls within the specified interval, plus all edges whose incident vertices
     *  are selected.  Additionally, neighboring vertices and connective edges are optionally added depending on @ref
     *  showOutNeighbors and @ref showInNeighbors. */
    CfgEmitter& selectIntervalGraph(const AddressInterval &interval);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // GraphViz emitters
    
    /** Dump entire control flow graph.
     *
     *  This is a convenient wrapper around @ref selectWholeGraph and @ref emit. */
    void emitWholeGraph(std::ostream &out) {
        selectWholeGraph().emit(out);
    }

    /** Dump control flow graph for one function.
     *
     *  This is a convenient wrapper around @ref selectFunctionGraph and @ref emit. */
    void emitFunctionGraph(std::ostream &out, const Function::Ptr &function) {
        selectFunctionGraph(function).emit(out);
    }

    /** Dump control flow graph for some address interval.
     *
     *  This is a convenient wrapper around @ref selectIntervalGraph and @ref emit. */
    void emitIntervalGraph(std::ostream &out, const AddressInterval &interval) {
        selectIntervalGraph(interval).emit(out);
    }
    

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Low-level emitters

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Utilities

    /** Returns true if the edge spans two different functions.
     *
     * @{ */
    static bool isInterFunctionEdge(const ControlFlowGraph::Edge&);
    static bool isInterFunctionEdge(const ControlFlowGraph::ConstEdgeIterator &e) { return isInterFunctionEdge(*e); }
    /** @} */

    /** First function that owns a vertex.
     *
     *  Returns the first of possibly many functions that own a vertex. "First" is defined as the function listed first in the
     *  set returned by @ref CfgVertex::owningFunctions.  Returns null if there are no owning functions.
     *
     * @{ */
    static Function::Ptr firstOwningFunction(const ControlFlowGraph::Vertex&);
    static Function::Ptr firstOwningFunction(const ControlFlowGraph::ConstVertexIterator &v) {
        return firstOwningFunction(*v);
    }
    /** @} */

    /** Functions that own a vertex.
     *
     *  Returns a set of pointers to the functions that own the specified vertex. Usually a vertex is owned by either zero or
     *  one function.
     *
     *  @{ */
    static FunctionSet owningFunctions(const ControlFlowGraph::Vertex&);
    static FunctionSet owningFunctions(const ControlFlowGraph::ConstVertexIterator &v) { return owningFunctions(*v); }
    /** @} */

    /** Assign vertices and edges to subgraphs.
     *
     *  Each vertex is assigned to a subgraph, one subgraph per function. If a vertex is owned by more than one function then
     *  the "first" function is used, where the definition of "first" is quite arbitrary. */
    void assignFunctionSubgraphs();


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Formatting: these are expected to be overridden by subclasses

    /** Source location for vertex.
     *
     *  Returns a string indicating the source code location for a vertex.  If no information is available then an empty string
     *  is returned. */
    virtual std::string sourceLocation(const ControlFlowGraph::ConstVertexIterator&) const;

    /** Label for CFG vertex.
     *
     *  Returns the simple label for a CFG vertex.  The simple label is usually just an address rather than instructions, etc.
     *  The returned lable must include the delimiting double quotes or angle brackets and have proper escaping of contents.
     *
     *  @{ */
    virtual std::string vertexLabel(const ControlFlowGraph::ConstVertexIterator&) const;
    std::string vertexLabel(const ControlFlowGraph::Vertex&) const;
    /** @} */

    /** Detailed label for CFG vertex.
     *
     *  The detailed label can be a multi-line value and must include delimiting double quotes or angle brackets with proper
     *  escaping of the content.  This method should honor the @ref showInstructionAddresses and @ref
     *  showInstructionStackDeltas properties.
     *
     *  @{ */
    virtual std::string vertexLabelDetailed(const ControlFlowGraph::ConstVertexIterator&) const;
    std::string vertexLabelDetailed(const ControlFlowGraph::Vertex&) const;
    /** @} */

    /** Attributes for a CFG vertex.
     *
     * @{ */
    virtual Attributes vertexAttributes(const ControlFlowGraph::ConstVertexIterator&) const;
    Attributes vertexAttributes(const ControlFlowGraph::Vertex&) const;
    /** @} */

    /** Label for CFG edge.
     *
     *  The returned lable must include the delimiting double quotes or angle brackets and have  proper escaping of contents.
     *
     *  @{ */
    virtual std::string edgeLabel(const ControlFlowGraph::ConstEdgeIterator&) const;
    std::string edgeLabel(const ControlFlowGraph::Edge&) const;
    /** @} */

    /** Attributes for a CFG edge.
     *
     *  @{ */
    virtual Attributes edgeAttributes(const ControlFlowGraph::ConstEdgeIterator&) const;
    Attributes edgeAttributes(const ControlFlowGraph::Edge&) const;
    /** @} */

    /** Label for function vertex.
     *
     *  The returned label must include the delimiting double quotes or angle brackets and have proper escaping of contents. */
    virtual std::string functionLabel(const Function::Ptr&) const;

    /** Attributes for function vertex. */
    virtual Attributes functionAttributes(const Function::Ptr&) const;

private:
    void init();

    // Give GraphViz identifying names to some vertices. The names assigned by this method are used internally by GraphViz, but
    // encoding some information into thse names (instead of using integers) is useful mainly if we try to parse the GraphViz
    // output later --  it gives us a way to relate GraphViz's vertex identifiers back to the original ROSE CFG vertices.
    void nameVertices();
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                              Base emitter for Partitioner2::FunctionCallGraph
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Emits a function call graph. */
class ROSE_DLL_API CgEmitter: public BaseEmitter<FunctionCallGraph::Graph> {
    FunctionCallGraph cg_;
    Color::HSV functionHighlightColor_;                 // highlight certain functions
    boost::regex highlightNameMatcher_;                 // which functions to highlight
public:
    explicit CgEmitter(const Partitioner &partitioner);
    CgEmitter(const Partitioner &partitioner, const FunctionCallGraph &cg);
    virtual std::string functionLabel(const Function::Ptr&) const;
    virtual Attributes functionAttributes(const Function::Ptr&) const;
    virtual void emitCallGraph(std::ostream &out) const;
    virtual const FunctionCallGraph& callGraph() const { return cg_; }
    virtual void callGraph(const FunctionCallGraph &cg);
    virtual void highlight(const boost::regex&);
private:
    // Give GraphViz identifying names to some vertices. The names assigned by this method are used internally by GraphViz, but
    // encoding some information into thse names (instead of using integers) is useful mainly if we try to parse the GraphViz
    // output later --  it gives us a way to relate GraphViz's vertex identifiers back to the original ROSE CFG vertices.
    void nameVertices();
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                              Callgraph emitter with inlined imports
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Emits a modified function call graph.
 *
 *  The function call graph is modified by removing all vertices whose function names match a user-specified pattern and
 *  compensating by listing the names of removed functions in the vertices of the callers.  This is a little bit like inlining,
 *  thus the name of the class. */
class ROSE_DLL_API CgInlinedEmitter: public CgEmitter {
    boost::regex nameMatcher_;
    typedef std::vector<Function::Ptr> InlinedFunctions;
    typedef Sawyer::Container::Map<Function::Ptr, InlinedFunctions> Inlines;
    Inlines inlines_;
public:
    CgInlinedEmitter(const Partitioner &partitioner, const boost::regex &nameMatcher);
    CgInlinedEmitter(const Partitioner &partitioner, const FunctionCallGraph &cg, const boost::regex &nameMatcher);
    virtual const FunctionCallGraph& callGraph() const ROSE_OVERRIDE { return CgEmitter::callGraph(); }
    virtual void callGraph(const FunctionCallGraph&) ROSE_OVERRIDE;
    virtual std::string functionLabel(const Function::Ptr&) const ROSE_OVERRIDE;
    virtual bool shouldInline(const Function::Ptr&) const;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Reading layout position information from "dot"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Two dimensional display plane coordinate. */
struct Coordinate {
    double x;                                           /**< Distance from left. */
    double y;                                           /**< Distance from up. */ 
};

/** Position of a vertex. */
struct VertexPosition {
    std::string name;                                   /**< Name of vertex as known to GraphViz. */
    Coordinate center;                                  /**< Center of vertex in display plane units. */
    double width;                                       /**< Horizontal size of vertex. */
    double height;                                      /**< Vertical size of vertex. */
};

/** Position of an edge.
 *
 *  GraphViz represents edge positions as B-splines. These are apparently a sequence quadratic Bezier curve segments, which can
 *  be drawn with a sliding window of length four coordinates and delta of one coordinate. The spline will therefore always
 *  have at least four coorindates.
 *
 *  Since GraphViz identifies edges by their endpoints, there is no support for being able to resolve GraphViz edges back to
 *  their corresponding Sawyer edges when edges are parallel. */
struct EdgePosition {
    std::vector<Coordinate> spline;                     /**< Control points for the edge B-spline. See @ref EdgePosition. */
};

/** A graph with positioned vertices and edges. */
typedef Sawyer::Container::Graph<VertexPosition, EdgePosition> PositionGraph;

/** Constructs graph positions from a file.
 *
 *  The input must have the same syntax as the output from the GraphViz "dot -Tplain" command. */
PositionGraph readPositions(std::istream&);


    

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Class template method implementations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class G>
size_t
BaseEmitter<G>::emitVertex(std::ostream &out, const typename G::ConstVertexIterator &vertex,
                           const Organization &org, const VMap &vmap) const {
    size_t id = NO_ID;
    if (org.isSelected() && !vmap.getOptional(vertex->id()).assignTo(id)) {
        id = vmap.size();
        std::string name = org.name();
        if (name.empty())
            name = StringUtility::numberToString(id);
        out <<name <<" [ label=" <<org.label() <<" ";
        out <<toString(org.attributes()) <<" ];\n";
    }
    return id;
}

template<class G>
void
BaseEmitter<G>::emitEdge(std::ostream &out, const typename G::ConstEdgeIterator &edge, const Organization &org,
                         const VMap &vmap) const {
    ASSERT_require2(vmap.exists(edge->source()->id()), "edge source vertex has not yet been emitted");
    ASSERT_require2(vmap.exists(edge->target()->id()), "edge target vertex has not yet been emitted");

    size_t sourceId = edge->source()->id();
    std::string sourceName = vertexOrganization(sourceId).name();
    if (sourceName.empty())
        sourceName = StringUtility::numberToString(vmap[sourceId]);

    size_t targetId = edge->target()->id();
    std::string targetName = vertexOrganization(targetId).name();
    if (targetName.empty())
        targetName = StringUtility::numberToString(vmap[targetId]);

    out <<sourceName <<" -> " <<targetName <<" [ label=" <<org.label() <<" " <<toString(org.attributes()) <<" ];\n";
}

template<class G>
void
BaseEmitter<G>::emit(std::ostream &out) const {
    VMap vmap;                                          // GraphViz node ID for each graph vertex (modified by emit)

    out <<"digraph CFG {\n";
    out <<" graph [ " <<toString(defaultGraphAttributes_) <<" ];\n";
    out <<" node  [ " <<toString(defaultNodeAttributes_) <<" ];\n";
    out <<" edge  [ " <<toString(defaultEdgeAttributes_) <<" ];\n";

    typedef std::map<std::string /*subgraph name*/, std::string/*subgraph content*/> Subgraphs;
    Subgraphs subgraphs;

    // Emit vertices to subgraphs
    for (typename G::ConstVertexIterator vertex=graph_.vertices().begin(); vertex!=graph_.vertices().end(); ++vertex) {
        const Organization &org = vertexOrganization(vertex);
        if (org.isSelected() && !vmap.exists(vertex->id())) {
            std::ostringstream ss;
            size_t gvid = emitVertex(ss, vertex, org, vmap);
            vmap.insert(vertex->id(), gvid);
            subgraphs[org.subgraph()] += ss.str();
        }
    }

    // Emit edges to subgraphs
    for (typename G::ConstEdgeIterator edge=graph_.edges().begin(); edge!=graph_.edges().end(); ++edge) {
        const Organization &org = edgeOrganization(edge);
        if (org.isSelected() &&
            vertexOrganization(edge->source()).isSelected() && vertexOrganization(edge->target()).isSelected()) {
            std::ostringstream ss;
            emitEdge(ss, edge, org, vmap);
            subgraphs[org.subgraph()] += ss.str();
        }
    }

    // Emit named subgraphs to output
    BOOST_FOREACH (const Subgraphs::value_type &node, subgraphs) {
        const std::string &subgraphName = node.first;
        const std::string &subgraphContent = node.second;
        if (!subgraphName.empty()) {
            out <<"\nsubgraph cluster_" <<subgraphName <<" {"
                <<" label=" <<subgraphOrganization(subgraphName).label() <<" "
                <<toString(subgraphOrganization(subgraphName).attributes()) <<"\n"
                <<subgraphContent
                <<"}\n";
        }
    }

    // Emit unnamed subgraph content without a surrounding subgraph construct (i.e., global graph)
    Subgraphs::iterator unnamedSubgraph = subgraphs.find("");
    if (unnamedSubgraph != subgraphs.end())
        out <<unnamedSubgraph->second;

    // Emit pseudo edges
    BOOST_FOREACH (const PseudoEdge &edge, pseudoEdges_) {
        if (vertexOrganization(edge.src).isSelected() && vertexOrganization(edge.dst).isSelected()) {
            std::string sourceName = vertexOrganization(edge.src).name();
            std::string targetName = vertexOrganization(edge.dst).name();
            out <<(sourceName.empty() ? StringUtility::numberToString(vmap[edge.src->id()]) : sourceName)
                <<" -> "
                <<(targetName.empty() ? StringUtility::numberToString(vmap[edge.dst->id()]) : targetName)
                <<" [ label=" <<escape(edge.label) <<" ];\n";
        }
    }
    
    out <<"}\n";
}

template<class G>
void
BaseEmitter<G>::deselectParallelEdges() {
    BOOST_FOREACH (const typename G::Vertex &src, graph_.vertices()) {
        if (vertexOrganization(src).isSelected()) {
            std::set<size_t> targets;
            BOOST_FOREACH (const typename G::Edge &edge, src.outEdges()) {
                if (edgeOrganization(edge).isSelected() && !targets.insert(edge.target()->id()).second)
                    edgeOrganization(edge).select(false);
            }
        }
    }
}

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
