#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include <sage3basic.h>

#include <AsmUnparser_compat.h>
#include <CommandLine.h>
#include <Diagnostics.h>
#include <Partitioner2/GraphViz.h>
#include <Partitioner2/Partitioner.h>
#include <Sawyer/GraphTraversal.h>
#include <SymbolicSemantics2.h>
#ifdef _MSC_VER
#define popen _popen
#define pclose _pclose
#endif

using namespace Rose::Diagnostics;
using namespace Sawyer::Container::Algorithm;

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {
namespace GraphViz {


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Utilities
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const size_t NO_ID = -1;

// Make an edge color from a background color. Backgrounds tend to be too light for edges, and inverting the color would be too
// dark to be distinguishable from black on such a fine line.
static Color::HSV
makeEdgeColor(const Color::HSV &bg) {
    return Color::HSV(bg.h(), 1.0, 0.5, 1.0);
}

ROSE_DLL_API std::string
toString(const Attributes &attrs) {
    std::string retval;
    BOOST_FOREACH (const Attributes::Node &attr, attrs.nodes())
        retval += (retval.empty()?"":" ") + escape(attr.key()) + "=" + escape(attr.value());
    return retval;
}

ROSE_DLL_API std::string
quotedEscape(const std::string &s) {
    std::string retval;
    for (size_t i=0; i<s.size(); ++i) {
        if ('\n'==s[i]) {
            retval += "\\n";
        } else if ('"'==s[i]) {
            retval += "\\\"";
        } else {
            retval += s[i];
        }
    }
    return retval;
}

ROSE_DLL_API std::string
htmlEscape(const std::string &s) {
    std::string retval;
    for (size_t i=0; i<s.size(); ++i) {
        if ('\n'==s[i]) {
            retval += "<br/>";
        } else if ('<'==s[i]) {
            retval += "&lt;";
        } else if ('>'==s[i]) {
            retval += "&gt;";
        } else if ('&'==s[i]) {
            retval += "&amp;";
        } else {
            retval += s[i];
        }
    }
    return retval;
}

ROSE_DLL_API bool
isId(const std::string &s) {
    if (s.empty())
        return false;
    if (isalpha(s[0])) {
        BOOST_FOREACH (char ch, s) {
            if (!isalnum(ch) || '_'==ch)
                return false;
        }
        return true;
    }
    if (isdigit(s[0])) {
        BOOST_FOREACH (char ch, s) {
            if (!isdigit(ch))
                return false;
        }
        return true;
    }
    return false;
}

ROSE_DLL_API std::string
escape(const std::string &s) {
    if (s.empty())
        return "\"\"";
    if (isId(s))
        return s;
    if ('<'==s[0]) {
        int depth = 0;
        BOOST_FOREACH (char ch, s) {
            if ('<'==ch) {
                ++depth;
            } else if ('>'==ch) {
                if (--depth < 0)
                    break;
            }
        }
        if (0==depth)
            return "<" + htmlEscape(s) + ">";
    }
    return "\"" + quotedEscape(s) + "\"";
}

ROSE_DLL_API std::string
concatenate(const std::string &oldStuff, const std::string &newStuff, const std::string &separator) {
    if (oldStuff.empty())
        return "\"" + quotedEscape(newStuff) + "\"";
    if ('"'==oldStuff[0] && '"'==oldStuff[oldStuff.size()-1])
        return oldStuff.substr(0, oldStuff.size()-1) + quotedEscape(separator) + quotedEscape(newStuff) + "\"";
    if ('<'==oldStuff[0] && '>'==oldStuff[oldStuff.size()-1])
        return oldStuff.substr(0, oldStuff.size()-1) + htmlEscape(separator) + htmlEscape(newStuff) + ">";
    return "\"" + quotedEscape(oldStuff) + quotedEscape(separator) + quotedEscape(newStuff) + "\"";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      CfgEmitter
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned long CfgEmitter::versionDate_ = 0;

CfgEmitter::CfgEmitter(const Partitioner &partitioner)
    : BaseEmitter<ControlFlowGraph>(partitioner.cfg()), partitioner_(partitioner), useFunctionSubgraphs_(true),
      showReturnEdges_(true), showInstructions_(false), showInstructionAddresses_(true), showInstructionStackDeltas_(true),
      showInNeighbors_(true), showOutNeighbors_(true), strikeNoopSequences_(false),
      funcEnterColor_(0.33, 1.0, 0.9),              // light green
      funcReturnColor_(0.67, 1.0, 0.9),             // light blue
      warningColor_(0, 1.0, 0.80)                   // light red
    {
        init();
    }

CfgEmitter::CfgEmitter(const Partitioner &partitioner, const ControlFlowGraph &g)
    : BaseEmitter<ControlFlowGraph>(g), partitioner_(partitioner), useFunctionSubgraphs_(true),
      showReturnEdges_(true), showInstructions_(false), showInstructionAddresses_(true), showInstructionStackDeltas_(true),
      showInNeighbors_(true), showOutNeighbors_(true), strikeNoopSequences_(false),
      funcEnterColor_(0.33, 1.0, 0.9),              // light green
      funcReturnColor_(0.67, 1.0, 0.9),             // light blue
      warningColor_(0, 1.0, 0.80)                   // light red
    {
        init();
    }

void
CfgEmitter::init() {
    using namespace Rose::BinaryAnalysis::InstructionSemantics2;

    // Class initialization
    if (0 == versionDate_) {
        FILE *dot = popen("dot -V 2>&1", "r");
        if (dot) {
            char buffer[256];
            if (size_t n = fread(buffer, 1, sizeof(buffer)-1, dot)) {
                // The full string is something like this: dot - graphviz version 2.26.3 (20100126.1600)
                buffer[n] = '\0';
                if (char *ltparen = strchr(buffer, '('))
                    versionDate_ = strtoul(ltparen+1, NULL, 0);
            }
            pclose(dot);
        }
        if (versionDate_ == 0)
            versionDate_ = 1;                           // something low, and other than zero
    }

    // Instance initialization
    if (BaseSemantics::DispatcherPtr cpu = partitioner_.instructionProvider().dispatcher()) {
        SmtSolverPtr solver = SmtSolver::instance(Rose::CommandLine::genericSwitchArgs.smtSolver);
        const RegisterDictionary *regdict = partitioner_.instructionProvider().registerDictionary();
        size_t addrWidth = partitioner_.instructionProvider().instructionPointerRegister().nBits();
        BaseSemantics::RiscOperatorsPtr ops = SymbolicSemantics::RiscOperators::instance(regdict, solver);
        noOpAnalysis_ = NoOperation(cpu->create(ops, addrWidth, regdict));
        noOpAnalysis_.initialStackPointer(0xdddd0001); // optional; odd prevents false positives for stack aligning instructions
    }
    nameVertices();
}

void
CfgEmitter::nameVertices() {
    BOOST_FOREACH (const ControlFlowGraph::Vertex &vertex, graph_.vertices()) {
        if (vertexOrganization(vertex.id()).name().empty()) {
            switch (vertex.value().type()) {
                case V_BASIC_BLOCK:
                    vertexOrganization(vertex.id()).name("V_" + StringUtility::addrToString(vertex.value().address()));
                    break;
                case V_NONEXISTING:
                    vertexOrganization(vertex.id()).name("nonexisting");
                    break;
                case V_INDETERMINATE:
                    vertexOrganization(vertex.id()).name("indeterminate");
                    break;
                case V_UNDISCOVERED:
                    vertexOrganization(vertex.id()).name("undiscovered");
                    break;
                case V_USER_DEFINED:
                    // use an ID number, the default
                    break;
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------
//                                      CfgEmitter selectors
//----------------------------------------------------------------------------------------------------------------------------

CfgEmitter&
CfgEmitter::selectWholeGraph() {
    subgraphOrganization().clear();
    selectNone();
    selectAll();

    BOOST_FOREACH (const ControlFlowGraph::Vertex &vertex, graph_.vertices()) {
        vertexOrganization(vertex).label(vertexLabelDetailed(vertex));
        vertexOrganization(vertex).attributes(vertexAttributes(vertex));
    }

    BOOST_FOREACH (const ControlFlowGraph::Edge &edge, graph_.edges()) {
        edgeOrganization(edge).label(edgeLabel(edge));
        edgeOrganization(edge).attributes(edgeAttributes(edge));
    }

    if (!showReturnEdges())
        deselectReturnEdges();
    if (useFunctionSubgraphs())
        assignFunctionSubgraphs();

    // We can't deselect these if the graph isn't a patritioner.cfg() because they might not be present.
    deselectUnusedVertexType(V_UNDISCOVERED);
    deselectUnusedVertexType(V_INDETERMINATE);
    deselectUnusedVertexType(V_NONEXISTING);

    return *this;
}

CfgEmitter&
CfgEmitter::selectFunctionGraph(const Function::Ptr &function) {
    ASSERT_not_null(function);
    subgraphOrganization().clear();
    selectNone();

    selectIntraFunction(function);
    if (showOutNeighbors())
        selectFunctionCallees(function);
    if (showInNeighbors())
        selectFunctionCallers(function);
    if (!showReturnEdges())
        deselectReturnEdges();
    if (useFunctionSubgraphs())
        assignFunctionSubgraphs();

    return *this;
}

CfgEmitter&
CfgEmitter::selectIntervalGraph(const AddressInterval &interval) {
    subgraphOrganization().clear();
    selectNone();
    selectInterval(interval);
    selectNeighbors(showInNeighbors(), showOutNeighbors());
    if (!showReturnEdges())
        deselectReturnEdges();
    if (useFunctionSubgraphs())
        assignFunctionSubgraphs();

    return *this;
}


//----------------------------------------------------------------------------------------------------------------------------
//                                      Low-level selectors
//----------------------------------------------------------------------------------------------------------------------------

void
CfgEmitter::selectInterval(const AddressInterval &interval) {
    BOOST_FOREACH (const ControlFlowGraph::Vertex &vertex, graph_.vertices()) {
        if (vertex.value().type() == V_BASIC_BLOCK && interval.isContaining(vertex.value().address())) {
            Organization &org = vertexOrganization(vertex);
            org.select();
            org.label(vertexLabelDetailed(vertex));
            org.attributes(vertexAttributes(vertex));
        }
    }

    BOOST_FOREACH (const ControlFlowGraph::Edge &edge, graph_.edges()) {
        if (vertexOrganization(edge.source()).isSelected() && vertexOrganization(edge.target()).isSelected()) {
            Organization &org = edgeOrganization(edge);
            if (!org.isSelected()) {
                org.select();
                org.label(edgeLabel(edge));
                org.attributes(edgeAttributes(edge));
            }
        }
    }
}

void
CfgEmitter::selectIntraFunction(const Function::Ptr &function) {
    // Use an iteration rather than a traversal because we want all vertices that belong to the function, including those
    // not reachable from the entry vertex.
    BOOST_FOREACH (const ControlFlowGraph::Vertex &vertex, graph_.vertices()) {
        if (vertex.value().isOwningFunction(function)) {
            if (!vertexOrganization(vertex).isSelected()) {
                vertexOrganization(vertex).select();
                vertexOrganization(vertex).label(vertexLabelDetailed(vertex));
                vertexOrganization(vertex).attributes(vertexAttributes(vertex));
            }
            BOOST_FOREACH (const ControlFlowGraph::Edge &edge, vertex.outEdges()) {
                if (!edgeOrganization(edge).isSelected() && partitioner_.isEdgeIntraProcedural(edge, function)) {
                    edgeOrganization(edge).select();
                    edgeOrganization(edge).label(edgeLabel(edge));
                    edgeOrganization(edge).attributes(edgeAttributes(edge));
                }
            }
        }
    }
}

void
CfgEmitter::selectFunctionCallees(const Function::Ptr &function) {
    // Use an iteration rather than a traversal because we want to consider all vertices that belong to the function, including
    // those not reachable from the entry vertex.
    BOOST_FOREACH (const ControlFlowGraph::Vertex &vertex, graph_.vertices()) {
        if (vertexOrganization(vertex).isSelected() && vertex.value().isOwningFunction(function)) {
            BOOST_FOREACH (const ControlFlowGraph::Edge &edge, vertex.outEdges()) {
                if (partitioner_.isEdgeInterProcedural(edge, function)) {
                    if (!edgeOrganization(edge).isSelected()) {
                        edgeOrganization(edge).select();
                        edgeOrganization(edge).label(edgeLabel(edge));
                        edgeOrganization(edge).attributes(edgeAttributes(edge));
                    }

                    Organization &tgt = vertexOrganization(edge.target());
                    if (!tgt.isSelected()) {
                        tgt.select();

                        if (Function::Ptr callee = edge.target()->value().isEntryBlock()) {
                            tgt.label(functionLabel(callee));
                            tgt.attributes(functionAttributes(callee));
                        } else {
                            tgt.label(vertexLabel(edge.target()));
                            tgt.attributes(vertexAttributes(edge.target()));
                        }
                    }
                }
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
CfgEmitter::selectFunctionCallers(const Function::Ptr &callee) {
    // Use an iteration rather than a traversal because we want to consider all vertices that belong to the function, including
    // those not reachable from the entry vertex.
    BOOST_FOREACH (const ControlFlowGraph::Vertex &vertex, graph_.vertices()) {
        if (vertexOrganization(vertex).isSelected() && vertex.value().isOwningFunction(callee)) {

            // Are there edges coming into this vertex from outside this function?
            typedef Sawyer::Container::Map<rose_addr_t /*caller*/, CallInfo> Callers;
            Callers callers;
            BOOST_FOREACH (const ControlFlowGraph::Edge &interEdge, vertex.inEdges()) {
                if (partitioner_.isEdgeInterProcedural(interEdge, Function::Ptr(), callee) &&
                    !edgeOrganization(interEdge).isSelected()) {

                    // Is the call coming from a block that belongs to a function, or from some random non-owned block?
                    if (Function::Ptr caller = firstOwningFunction(interEdge.source())) {
                        CallInfo callInfo = callers.getOptional(caller->address()).orDefault();
                        if (interEdge.value().type() == E_FUNCTION_CALL) {
                            ++callInfo.nCalls;
                        } else if (interEdge.value().type() == E_FUNCTION_XFER) {
                            ++callInfo.nTransfers;
                        } else {
                            ++callInfo.nOthers;
                        }
                        callers.insert(caller->address(), callInfo);
                    } else {
                        Organization &src = vertexOrganization(interEdge.source());
                        if (!src.isSelected()) {
                            src.select();
                            src.label(vertexLabel(interEdge.source()));
                            src.attributes(vertexAttributes(interEdge.source()));
                        }
                        edgeOrganization(interEdge).select();
                        edgeOrganization(interEdge).label(edgeLabel(interEdge));
                        edgeOrganization(interEdge).attributes(edgeAttributes(interEdge));
                    }
                }
            }
            
            // Organize the calls to this vertex from a function-as-a-whole
            BOOST_FOREACH (const Callers::Node &callNode, callers.nodes()) {
                Function::Ptr callerFunc = partitioner_.functionExists(callNode.key());
                ASSERT_not_null(callerFunc);
                ControlFlowGraph::ConstVertexIterator caller = partitioner_.findPlaceholder(callerFunc->address());
                ASSERT_require(caller != graph_.vertices().end());

                Organization &org = vertexOrganization(caller);
                if (!org.isSelected()) {
                    org.select();
                    org.label(functionLabel(callerFunc));
                    org.attributes(functionAttributes(callerFunc));
                }

                const CallInfo &callInfo = callNode.value();
                std::string label;
                if (callInfo.nCalls)
                    label += StringUtility::plural(callInfo.nCalls, "calls");
                if (callInfo.nTransfers)
                    label += (label.empty()?"":"\n") + StringUtility::plural(callInfo.nTransfers, "xfers");
                if (callInfo.nOthers)
                    label += (label.empty()?"":"\n") + StringUtility::plural(callInfo.nOthers, "others");

                pseudoEdges_.push_back(PseudoEdge(caller, graph_.findVertex(vertex.id()), label));
            }
        }
    }
}

void
CfgEmitter::deselectReturnEdges() {
    BOOST_FOREACH (const ControlFlowGraph::Edge &edge, graph_.edges()) {
        if (edgeOrganization(edge).isSelected() && edge.value().type() == E_FUNCTION_RETURN) {
            // If we're removing the last edge to a vertex then remove the vertex also.
            edgeOrganization(edge).select(false);
            size_t nSelectedIncomingEdges = 0;
            BOOST_FOREACH (const ControlFlowGraph::Edge &inEdge, edge.target()->inEdges()) {
                if (edgeOrganization(inEdge).isSelected())
                    ++nSelectedIncomingEdges;
            }
            if (0==nSelectedIncomingEdges)
                vertexOrganization(edge.target()).select(false);
        }
    }
}

void
CfgEmitter::deselectUnusedVertexType(VertexType type) {
    BOOST_FOREACH (const ControlFlowGraph::Vertex &vertex, graph_.vertices()) {
        if (vertex.value().type() == type)
            deselectUnusedVertex(graph_.findVertex(vertex.id()));
    }
}

void
CfgEmitter::deselectUnusedVertex(ControlFlowGraph::ConstVertexIterator vertex) {
    bool isUsed = false;
    BOOST_FOREACH (const ControlFlowGraph::Edge &edge, vertex->inEdges()) {
        if (edgeOrganization(edge).isSelected()) {
            isUsed = true;
            break;
        }
    }
    if (!isUsed) {
        BOOST_FOREACH (const ControlFlowGraph::Edge &edge, vertex->outEdges()) {
            if (edgeOrganization(edge).isSelected()) {
                isUsed = true;
                break;
            }
        }
    }

    if (!isUsed)
        vertexOrganization(vertex).select(false);
}
    
void
CfgEmitter::selectNeighbors(bool selectIn, bool selectOut) {
    if (!selectIn && !selectOut)
        return;

    std::vector<ControlFlowGraph::ConstVertexIterator> needed;
    BOOST_FOREACH (const ControlFlowGraph::Edge &edge, graph_.edges()) {
        if ((selectOut && vertexOrganization(edge.source()).isSelected() && !vertexOrganization(edge.target()).isSelected()) ||
            (selectIn  && !vertexOrganization(edge.source()).isSelected() && vertexOrganization(edge.target()).isSelected())) {
            if (!edgeOrganization(edge).isSelected()) {
                edgeOrganization(edge).select();
                edgeOrganization(edge).label(edgeLabel(edge));
                edgeOrganization(edge).attributes(edgeAttributes(edge));
            }
            if (!vertexOrganization(edge.source()).isSelected())
                needed.push_back(edge.source());
            if (!vertexOrganization(edge.target()).isSelected())
                needed.push_back(edge.target());
        }
    }

    BOOST_FOREACH (const ControlFlowGraph::ConstVertexIterator &vertex, needed) {
        Organization &org = vertexOrganization(vertex);
        if (!org.isSelected()) {
            org.select();
            org.label(vertexLabel(vertex));
            org.attributes(vertexAttributes(vertex));
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------
//                                      CfgEmitter utilities
//----------------------------------------------------------------------------------------------------------------------------

// class method
Function::Ptr
CfgEmitter::firstOwningFunction(const ControlFlowGraph::Vertex &v) {
    FunctionSet owners = owningFunctions(v);
    return owners.isEmpty() ? Function::Ptr() : *owners.values().begin();
}

// class method
FunctionSet
CfgEmitter::owningFunctions(const ControlFlowGraph::Vertex &v) {
    if (v.value().type() != V_BASIC_BLOCK)
        return FunctionSet();
    return v.value().owningFunctions();
}

// class method
bool
CfgEmitter::isInterFunctionEdge(const ControlFlowGraph::Edge &edge) {
    if (edge.value().type() == E_FUNCTION_CALL || edge.value().type() == E_FUNCTION_XFER)
        return true;
    if (edge.source() == edge.target())
        return false;

    // When the source and/or target blocks of an edge are owned by multiple functions it can be ambiguous whether the edge is
    // inter- or intra- function.  For the purposes of the GraphViz output, each vertex is assigned to at most one function
    // and the edge classification is based on this assignment.
    return firstOwningFunction(edge.source()) != firstOwningFunction(edge.target());
}

void
CfgEmitter::assignFunctionSubgraphs() {
    BOOST_FOREACH (const ControlFlowGraph::Vertex &vertex, graph_.vertices()) {
        Organization &org = vertexOrganization(vertex);
        Function::Ptr function;
        if (org.isSelected() && org.subgraph().empty() && (function=firstOwningFunction(vertex))) {
            std::string subgraphName = StringUtility::addrToString(function->address());
            if (!subgraphOrganization().exists(subgraphName)) {
                Organization org;
                org.label(functionLabel(function));
                org.attributes(functionAttributes(function));
                subgraphOrganization().insert(subgraphName, org);
            }
            vertexOrganization(vertex).subgraph(subgraphName);
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------
//                                      CfgEmitter formatting
//----------------------------------------------------------------------------------------------------------------------------

std::string
CfgEmitter::sourceLocation(const ControlFlowGraph::ConstVertexIterator &vertex) const {
    ASSERT_require(graph_.isValidVertex(vertex));
    if (vertex->value().type() != V_BASIC_BLOCK)
        return "";
    SourceLocation srcInfo = srcMapper_.get(vertex->value().address());
    if (srcInfo.isEmpty())
        return "";
    boost::filesystem::path fileName = srcInfo.fileName().filename();
    return fileName.string() + ":" + StringUtility::numberToString(srcInfo.line());
}

std::string
CfgEmitter::vertexLabel(const ControlFlowGraph::Vertex &v) const {
    return vertexLabel(graph_.findVertex(v.id()));
}

std::string
CfgEmitter::vertexLabel(const ControlFlowGraph::ConstVertexIterator &vertex) const {
    std::string srcLoc = sourceLocation(vertex);
    if (!srcLoc.empty())
        srcLoc = htmlEscape(srcLoc) + "<br align=\"left\"/>";
    switch (vertex->value().type()) {
        case V_BASIC_BLOCK: {
            // If this basic block is the entry point to at least one function, show the function name as the basic block's
            // label.
            bool foundEntryPoint = false;
            FunctionSet functions = owningFunctions(vertex);
            if (!functions.isEmpty()) {
                BOOST_FOREACH (const Function::Ptr &function, functions.values()) {
                    if (function->address() == vertex->value().address()) {
                        srcLoc += htmlEscape(function->printableName()) + "<br align=\"left\"/>";
                        foundEntryPoint = true;
                    }
                }
                if (foundEntryPoint)
                    return "<" + srcLoc + ">";
            }

            // If this basic block has a name (i.e., an address) then use it as the label.
            if (BasicBlock::Ptr bb = vertex->value().bblock())
                return "<" + srcLoc + htmlEscape(bb->printableName()) + ">";

            // Last resort, use the address stored in the CFG (not sure if this code is even reachable)
            return "<" + srcLoc + StringUtility::addrToString(vertex->value().address()) + ">";
        }
        case V_NONEXISTING:
            return "\"nonexisting\"";
        case V_UNDISCOVERED:
            return "\"undiscovered\"";
        case V_INDETERMINATE:
            return "\"indeterminate\"";
        case V_USER_DEFINED:
            return "\"user defined\"";
    }
    ASSERT_not_reachable("invalid vertex type");
}

std::string
CfgEmitter::vertexLabelDetailed(const ControlFlowGraph::Vertex &v) const {
    return vertexLabelDetailed(graph_.findVertex(v.id()));
}

std::string
CfgEmitter::vertexLabelDetailed(const ControlFlowGraph::ConstVertexIterator &vertex) const {
    BasicBlock::Ptr bb;
    if (showInstructions_ && vertex->value().type() == V_BASIC_BLOCK && (bb = vertex->value().bblock())) {
        const std::vector<SgAsmInstruction*> insns = bb->instructions();

        // Decide which instructions are part of a no-op sequence and which sequences should be struck out in the bb label.
        std::vector<bool> isPartOfNoopSequence(insns.size(), false);
        if (strikeNoopSequences_ && !isPartOfNoopSequence.empty()) {
            NoOperation::IndexIntervals noopSequences = noOpAnalysis_.findNoopSubsequences(insns);
            noopSequences = NoOperation::largestEarliestNonOverlapping(noopSequences);
            BOOST_FOREACH (const NoOperation::IndexInterval &where, noopSequences) {
                for (size_t i=where.least(); i<=where.greatest(); ++i)
                    isPartOfNoopSequence[i] = true;
            }
        }

        // Source code position for this BB if known.
        std::string srcLoc = sourceLocation(vertex);
        if (!srcLoc.empty())
            srcLoc = htmlEscape(srcLoc) + "<br align=\"left\"/>";
        std::string s = srcLoc;

        // Instructions for this BB.
        for (size_t i=0; i<insns.size(); ++i) {
            SgAsmInstruction *insn = insns[i];

            if (showInstructionAddresses_)
                s += StringUtility::addrToString(insn->get_address()).substr(2) + " ";

            if (showInstructionStackDeltas_) {
                int64_t delta = insn->get_stackDeltaIn();
                if (delta != SgAsmInstruction::INVALID_STACK_DELTA) {
                    // Stack delta as a two-character hexadecimal, but show a '+' sign when it's positive and nothing
                    // when it's negative (negative is the usual case for most architectures).
                    char buf[64];
                    if (delta <= 0) {
                        sprintf(buf, "%02" PRIx64 " ", -delta);
                    } else {
                        sprintf(buf, "+%" PRIx64 " ", delta);
                    }
                    s += buf;
                } else {
                    s += " ?? ";
                }
            }

            if (isPartOfNoopSequence[i]) {
                if (versionDate_ >= 20130915) {
                    // Strike out each insn of the no-op sequence
                    s += "<s>" + htmlEscape(unparseInstruction(insn)) + "</s><br align=\"left\"/>";
                } else {
                    // Put the no-op in parentheses because we graphViz doesn't have strike-through capability
                    s += "no-op (" + htmlEscape(unparseInstruction(insn)) + ")<br align=\"left\"/>"; 
                }
            } else {
                s += htmlEscape(unparseInstruction(insn)) + "<br align=\"left\"/>";
            }
        }

        if (s.empty())
            s = "(no insns)";
        return "<" + s + ">";
    }
    return vertexLabel(vertex);
}

Attributes
CfgEmitter::vertexAttributes(const ControlFlowGraph::Vertex &v) const {
    return vertexAttributes(graph_.findVertex(v.id()));
}

Attributes
CfgEmitter::vertexAttributes(const ControlFlowGraph::ConstVertexIterator &vertex) const {
    ASSERT_require(vertex != graph_.vertices().end());
    Attributes attr;
    attr.insert("shape", "box");

    if (vertex->value().type() == V_BASIC_BLOCK) {
        BasicBlock::Ptr bblock = vertex->value().bblock();
        ASSERT_not_null(bblock);
        attr.insert("fontname", "Courier");

        bool isEntryBlock = vertex->value().isEntryBlock();
        bool isFunctionReturn = partitioner_.basicBlockIsFunctionReturn(bblock);
        bool isShared = vertex->value().nOwningFunctions() > 1;

        std::vector<std::string> styles;
        if (isEntryBlock) {
            styles.push_back("filled");
            attr.insert("fillcolor", funcEnterColor_.toHtml());
        } else if (isFunctionReturn) {
            styles.push_back("filled");
            attr.insert("fillcolor", funcReturnColor_.toHtml());
        }
        if (isShared)
            styles.push_back("dashed");

        if (!styles.empty())
            attr.insert("style", StringUtility::join(", ", styles));

        attr.insert("href", StringUtility::addrToString(vertex->value().address()));

    } else {
        attr.insert("style", "filled");
        attr.insert("fillcolor", warningColor_.toHtml());
    }

    return attr;
}

std::string
CfgEmitter::edgeLabel(const ControlFlowGraph::Edge &e) const {
    return edgeLabel(graph_.findEdge(e.id()));
}

std::string
CfgEmitter::edgeLabel(const ControlFlowGraph::ConstEdgeIterator &edge) const {
    ASSERT_require(edge != graph_.edges().end());
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
                s += "\\nassumed";
            break;
        case E_NORMAL: {
            // Normal edges don't get labels unless it's inter-function, otherwise the graphs would be too noisy.
            if (isInterFunctionEdge(edge))
                s = "other";
            break;
        }
        case E_USER_DEFINED:
            s = "user";
            break;
    }
    return "\"" + s + "\"";
}

Attributes
CfgEmitter::edgeAttributes(const ControlFlowGraph::Edge &e) const {
    return edgeAttributes(graph_.findEdge(e.id()));
}

Attributes
CfgEmitter::edgeAttributes(const ControlFlowGraph::ConstEdgeIterator &edge) const {
    ASSERT_require(edge != graph_.edges().end());
    Attributes attr;

    if (edge->value().type() == E_FUNCTION_RETURN) {
        attr.insert("color", makeEdgeColor(funcReturnColor_).toHtml());
    } else if (edge->target() == partitioner_.indeterminateVertex()) {
        attr.insert("color", makeEdgeColor(warningColor_).toHtml());
    } else if (edge->value().type() == E_FUNCTION_CALL) {
        attr.insert("color", makeEdgeColor(funcEnterColor_).toHtml());
    }

    // Fall-through edges are less important, so make them dotted.
    if (edge->source()->value().type() ==V_BASIC_BLOCK && edge->target()->value().type() == V_BASIC_BLOCK &&
        edge->source()->value().bblock() &&
        edge->source()->value().bblock()->fallthroughVa() == edge->target()->value().address()) {
        attr.insert("style", "dotted");                 // fall-through edge
    }
    return attr;
}

std::string
CfgEmitter::functionLabel(const Function::Ptr &function) const {
    if (function)
        return "\"" + quotedEscape(function->printableName()) + "\"";
    return "\"\"";
}

Attributes
CfgEmitter::functionAttributes(const Function::Ptr &function) const {
    ASSERT_not_null(function);
    Attributes attr;
    attr.insert("style", "filled");
    attr.insert("fillcolor", subgraphColor().toHtml());
    attr.insert("href", StringUtility::addrToString(function->address()));
    return attr;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Function call graph
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CgEmitter::CgEmitter(const Partitioner &partitioner)
    : functionHighlightColor_(0.15, 1.0, 0.75), highlightNameMatcher_("^\\001$") {
    callGraph(partitioner.functionCallGraph(AllowParallelEdges::NO));
}

CgEmitter::CgEmitter(const Partitioner& /*for consistency and future expansion*/, const FunctionCallGraph &cg)
    : functionHighlightColor_(0.15, 1.0, 0.75), highlightNameMatcher_("^\\001$") {
    callGraph(cg);
}

void
CgEmitter::callGraph(const FunctionCallGraph &cg) {
    cg_ = cg;
    graph(cg_.graph());
    nameVertices();
}

void
CgEmitter::nameVertices() {
    BOOST_FOREACH (const FunctionCallGraph::Graph::Vertex &vertex, graph_.vertices()) {
        const Function::Ptr &function = vertex.value();
        vertexOrganization(vertex.id()).name("V_" + StringUtility::addrToString(function->address()));
    }
}

std::string
CgEmitter::functionLabel(const Function::Ptr &function) const {
    if (function)
        return "\"" + quotedEscape(function->printableName()) + "\"";
    return "\"\"";
}

void
CgEmitter::highlight(const boost::regex &re) {
    highlightNameMatcher_ = re;
}

Attributes
CgEmitter::functionAttributes(const Function::Ptr &function) const {
    ASSERT_not_null(function);
    Attributes attr;
    attr.insert("style", "filled");
    if (boost::regex_search(function->name(), highlightNameMatcher_)) {
        attr.insert("fillcolor", functionHighlightColor_.toHtml());
    } else {
        attr.insert("fillcolor", subgraphColor().toHtml());
    }
    return attr;
}

void
CgEmitter::emitCallGraph(std::ostream &out) const {
    typedef FunctionCallGraph::Graph CG;
    out <<"digraph CG {\n";
    out <<" graph [ " <<toString(defaultGraphAttributes_) <<" ];\n";
    out <<" node  [ " <<toString(defaultNodeAttributes_) <<" ];\n";
    out <<" edge  [ " <<toString(defaultEdgeAttributes_) <<" ];\n";

    BOOST_FOREACH (const CG::Vertex &vertex, graph_.vertices()) {
        const Function::Ptr &function = vertex.value();

        std::string vertexName = vertexOrganization(vertex.id()).name();
        if (vertexName.empty())
            vertexName = StringUtility::numberToString(vertex.id());

        out <<vertexName <<" [ label=" <<functionLabel(function) <<" "
            <<"href=\"" <<StringUtility::addrToString(function->address()) <<"\" "
            <<toString(functionAttributes(function)) <<" ]\n";
    }

    BOOST_FOREACH (const CG::Edge &edge, graph_.edges()) {
        std::string label;
        switch (edge.value().type()) {
            case E_FUNCTION_CALL: label = "calls";  break;
            case E_FUNCTION_XFER: label = "xfers";  break;
            default:              label = "others"; break;
        }
        label = StringUtility::plural(edge.value().count(), label);

        std::string sourceName = vertexOrganization(edge.source()->id()).name();
        if (sourceName.empty())
            sourceName = StringUtility::numberToString(edge.source()->id());

        std::string targetName = vertexOrganization(edge.target()->id()).name();
        if (targetName.empty())
            targetName = StringUtility::numberToString(edge.target()->id());

        out <<sourceName <<" -> " <<targetName <<" [ label=\"" <<label <<"\" ];\n";
    }
    
    out <<"}\n";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Function callgraph with inlined functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CgInlinedEmitter::CgInlinedEmitter(const Partitioner &partitioner, const boost::regex &nameMatcher)
    : CgEmitter(partitioner), nameMatcher_(nameMatcher) {
    callGraph(partitioner.functionCallGraph(AllowParallelEdges::NO));
}

CgInlinedEmitter::CgInlinedEmitter(const Partitioner &partitioner, const FunctionCallGraph &cg, const boost::regex &nameMatcher)
    : CgEmitter(partitioner), nameMatcher_(nameMatcher) {
    callGraph(cg);
}

void
CgInlinedEmitter::callGraph(const FunctionCallGraph &fullCg) {
    FunctionCallGraph cg;                               // the call graph with some calls removed
    inlines_.clear();

    // Insert all vertices that will be needed.
    BOOST_FOREACH (const FunctionCallGraph::Graph::Vertex &vertex, fullCg.graph().vertices()) {
        Function::Ptr function = vertex.value();
        if (!shouldInline(function) || fullCg.nCallees(function)>0) {
            cg.insertFunction(function);
            inlines_.insert(function, InlinedFunctions());
        }
    }

    // Insert call edges
    BOOST_FOREACH (const FunctionCallGraph::Graph::Edge &edge, fullCg.graph().edges()) {
        Function::Ptr caller = edge.source()->value();
        Function::Ptr callee = edge.target()->value();
        if (shouldInline(callee)) {
            insertUnique(inlines_[caller], callee, sortFunctionsByAddress);
        } else {
            ASSERT_require(inlines_.exists(callee));
            cg.insertCall(caller, callee, edge.value().type(), edge.value().count());
        }
    }
    CgEmitter::callGraph(cg);
}

bool
CgInlinedEmitter::shouldInline(const Function::Ptr &function) const {
    return boost::regex_search(function->name(), nameMatcher_);
}

std::string
CgInlinedEmitter::functionLabel(const Function::Ptr &function) const {
    ASSERT_not_null(function);
    std::string s = htmlEscape(function->printableName()) + "<br align=\"left\"/>";
    BOOST_FOREACH (const Function::Ptr &inlined, inlines_[function])
        s += "  " + htmlEscape(inlined->printableName()) + "<br align=\"left\"/>";
    return "<" + s + ">";
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Grap with positioned vertices and edges
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


PositionGraph
readPositions(std::istream &in) {
    PositionGraph retval;
    Sawyer::Container::Map<std::string /*vertex name*/, PositionGraph::VertexIterator> vertexMap;

    size_t lineNumber = 0;
    char buf[8192];
    while (in) {
        ++lineNumber;
        in.getline(buf, sizeof buf);

        // Split the line into space-separated words, ignoring quotes, comments, HTML tags, etc.
        std::vector<std::string> words;
        char *s = buf;                                  // start
        while (s) {
            while (isspace(*s))
                ++s;
            if (!*s)
                break;
            char *t = s;                                // terminate
            while (t && !isspace(*t))
                ++t;
            words.push_back(std::string(s, t));
            s = t;
        }
        if (words.empty())
            continue;

        // Parse the words (ignore everything after the coordinates)
        if (1 == lineNumber) {
            if (words.size() < 4 || words[0] != "graph")
                throw std::runtime_error("expected \"graph SF DX DY\" at start of input");

        } else if (words[0] == "node") {
            if (words.size() < 6) {
                throw std::runtime_error("expected \"node NAME X Y DX DY\" at line " +
                                         StringUtility::numberToString(lineNumber));
            }
            VertexPosition pos;
            pos.name = words[1];
            pos.center.x = boost::lexical_cast<double>(words[2]);
            pos.center.y = boost::lexical_cast<double>(words[3]);
            pos.width = boost::lexical_cast<double>(words[4]);
            pos.height = boost::lexical_cast<double>(words[5]);
            ASSERT_forbid2(vertexMap.exists(pos.name), "duplicate vertex name");
            vertexMap.insert(pos.name, retval.insertVertex(pos));

        } else if (words[0] == "edge") {
            if (words.size() < 12) {
                throw std::runtime_error("expected \"edge SOURCE TARGET N SPLINE...\" at line " +
                                         StringUtility::numberToString(lineNumber));
            }
            EdgePosition pos;
            std::string &sourceVertexName = words[1];
            std::string &targetVertexName = words[2];
            size_t nControlPoints = boost::lexical_cast<size_t>(words[3]);
            for (size_t i=0; i<nControlPoints; ++i) {
                Coordinate cp;
                cp.x = boost::lexical_cast<double>(words[2*i+3]);
                cp.y = boost::lexical_cast<double>(words[2*i+4]);
                pos.spline.push_back(cp);
            }
            ASSERT_require(vertexMap.exists(sourceVertexName));
            ASSERT_require(vertexMap.exists(targetVertexName));
            retval.insertEdge(vertexMap[sourceVertexName], vertexMap[targetVertexName], pos);

        } else if (words[0] == "stop") {
            break;
        }
    }
    return retval;
}

} // namespace
} // namespace
} // namespace
} // namespace

#endif
