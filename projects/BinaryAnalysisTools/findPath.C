#include <rose.h>
#include <rose_strtoull.h>
#include <Diagnostics.h>
#include <Partitioner2/Engine.h>
#include <sawyer/GraphTraversal.h>

using namespace rose;
using namespace rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;
namespace P2 = Partitioner2;

Diagnostics::Facility mlog;

// Settings from the command-line
struct Settings {
    std::string isaName;                                // instruction set architecture name
    std::string beginVertex;                            // address or function name where paths should begin
    std::string endVertex;                              // address of function name where paths should end
    std::vector<rose_addr_t> avoidVertices;             // vertices to avoid in any path
    std::vector<rose_addr_t> avoidEdges;                // edges to avoid in any path (even number of vertex addresses)
    bool showInstructions;                              // show instructions in paths
    Settings(): showInstructions(false) {}
};

// Describe and parse the command-line
static Sawyer::CommandLine::ParserResult
parseCommandLine(int argc, char *argv[], Settings &settings)
{
    using namespace Sawyer::CommandLine;

    //--------------------------- 
    Parser parser;
    parser
        .purpose("finds paths in control flow graph")
        .version(std::string(ROSE_SCM_VERSION_ID).substr(0, 8), ROSE_CONFIGURE_DATE)
        .chapter(1, "ROSE Command-line Tools")
        .doc("synopsis",
             "@prop{programName} --begin=@v{va} --end=@v{va} [@v{switches}] @v{specimen}")
        .doc("description", "FIXME[Robb P. Matzke 2014-10-15]: NOT WRITTEN YET")
        .doc("Specimens", P2::Engine::specimenNameDocumentation());
    
    //--------------------------- 
    SwitchGroup gen = CommandlineProcessing::genericSwitches();

    //--------------------------- 
    SwitchGroup dis("Disassembly switches");
    dis.insert(Switch("isa")
               .argument("architecture", anyParser(settings.isaName))
               .doc("Instruction set architecture. Specify \"list\" to see a list of possible ISAs."));

    //--------------------------- 
    SwitchGroup cfg("Control flow graph switches");
    cfg.insert(Switch("begin")
               .argument("name_or_va", anyParser(settings.beginVertex))
               .doc("Address or function name where paths will begin.  An address can be the address of an instruction, "
                    "basic block, or function in decimal, octal, or hexadecimal.  Anything that's not parsable as an "
                    "address is assumed to be a function name and paths will start at the entry point to that function."));

    cfg.insert(Switch("end")
               .argument("va", anyParser(settings.endVertex))
               .doc("Address or function name where paths will end.  An address can be the address of an instruction, "
                    "basic block, or function in decimal, octal, or hexadecimal.  Anything that's not parsable as an "
                    "address is assumed to be a function name and paths will start at the entry point to that function."));

    cfg.insert(Switch("avoid-vertex")
               .argument("name_or_va", listParser(nonNegativeIntegerParser(settings.avoidVertices)))
               .whichValue(SAVE_ALL)
               .explosiveLists(true)
               .doc("A vertex that should be excluded from any path.  This switch may appear as often as desired in "
                    "order to avoid multiple vertices, or vertex addresses can be specified in a comma-separated "
                    "list.  The addresses can be any combination of instruction addresses, basic block addresses, "
                    "or function entry addresses."));

    cfg.insert(Switch("avoid-edge")
               .argument("edge", listParser(nonNegativeIntegerParser(settings.avoidEdges))->exactly(2))
               .whichValue(SAVE_ALL)
               .explosiveLists(true)
               .doc("Edge where path search will end. Edges are specified as a pair of addresses separated by a comma: "
                    "the source address and the target address.  The addresses are any combination of instruction "
                    "addresses, basic block addresses, or function addresses. This switch may appear as often as "
                    "desired to avoid multiple edges."));

    //--------------------------- 
    SwitchGroup out("Output switches");
    cfg.insert(Switch("show-instructions")
               .intrinsicValue(true, settings.showInstructions)
               .doc("Cause instructions to be listed as part of each path."));

    return parser.with(gen).with(dis).with(cfg).with(out).parse(argc, argv).apply();
}

// Return CFG basic block vertex that contains specified instruction address, or the end vertex if none found.
P2::ControlFlowGraph::ConstVertexNodeIterator
vertexForInstruction(const P2::Partitioner &partitioner, rose_addr_t insnVa) {
    if (P2::BasicBlock::Ptr bblock = partitioner.basicBlockContainingInstruction(insnVa))
        return partitioner.findPlaceholder(bblock->address());
    return partitioner.cfg().vertices().end();
}

P2::ControlFlowGraph::ConstVertexNodeIterator
vertexForInstruction(const P2::Partitioner &partitioner, const std::string &nameOrVa) {
    const char *s = nameOrVa.c_str();
    char *rest;
    errno = 0;
    rose_addr_t va = rose_strtoull(s, &rest, 0);
    if (*rest || errno!=0) {
        size_t nFound = 0;
        BOOST_FOREACH (const P2::Function::Ptr &function, partitioner.functions()) {
            if (function->name() == nameOrVa) {
                va = function->address();
                ++nFound;
            }
        }
        if (0==nFound)
            return partitioner.cfg().vertices().end();
        if (nFound > 1)
            throw std::runtime_error("vertex \""+StringUtility::cEscape(nameOrVa)+"\" is ambiguous");
    }
    return vertexForInstruction(partitioner, va);
}

// Convert a pair of addresses to a CFG edge.  The source address can be a basic block or last instruction of the basic
// block.
P2::ControlFlowGraph::ConstEdgeNodeIterator
edgeForInstructions(const P2::Partitioner &partitioner, rose_addr_t sourceVa, rose_addr_t targetVa) {
    P2::ControlFlowGraph::ConstVertexNodeIterator source = vertexForInstruction(partitioner, sourceVa);
    P2::ControlFlowGraph::ConstVertexNodeIterator target = vertexForInstruction(partitioner, targetVa);
    if (source == partitioner.cfg().vertices().end() || target == partitioner.cfg().vertices().end())
        return partitioner.cfg().edges().end();         // sourceVa or targetVa is not an instruction starting address
    if (source->value().address() != sourceVa && source->value().bblock()->instructions().back()->get_address() != sourceVa)
        return partitioner.cfg().edges().end();         // sourceVa is not a basic block initial or terminating instruction
    if (target->value().address() != targetVa)
        return partitioner.cfg().edges().end();         // targetVa is not a basic block initial instruction
    for (P2::ControlFlowGraph::ConstEdgeNodeIterator edge=source->outEdges().begin(); edge!=source->outEdges().end(); ++edge) {
        if (edge->target() == target)
            return edge;
    }
    return partitioner.cfg().edges().end();
}

P2::ControlFlowGraph::ConstEdgeNodeIterator
edgeForInstructions(const P2::Partitioner &partitioner, const std::vector<rose_addr_t> &endPoints) {
    ASSERT_require(endPoints.size()==2);
    return edgeForInstructions(partitioner, endPoints[0], endPoints[1]);
}

#if 0 // [Robb P. Matzke 2015-02-06]
// Returns a Boolean vector indicating whether an edge is significant.  An edge is significant if it appears on some
// path that originates at the beginVertex and reaches the endEdge.
std::vector<bool>
significantEdges(const P2::Partitioner &partitioner,
                 P2::ControlFlowGraph::ConstVertexNodeIterator beginVertex,
                 P2::ControlFlowGraph::ConstEdgeNodeIterator endEdge) {
    using namespace Sawyer::Container::Algorithm;

    // Mark edges that are reachable with a forward traversal from the starting vertex
    std::vector<bool> forwardReachable(partitioner.cfg().nEdges(), false);
    typedef DepthFirstForwardEdgeTraversal<const P2::ControlFlowGraph> ForwardTraversal;
    for (ForwardTraversal t(partitioner.cfg(), beginVertex); t; ++t)
        forwardReachable[t->id()] = true;

    // Mark edges that are reachable with a backward traversal from the ending edge
    std::vector<bool> significant(partitioner.cfg().nEdges(), false);
    typedef DepthFirstReverseEdgeTraversal<const P2::ControlFlowGraph> ReverseTraversal;
    for (ReverseTraversal t(partitioner.cfg(), endEdge); t; ++t)
        significant[t->id()] = forwardReachable[t->id()];

#if 1 // DEBUGGING [Robb P. Matzke 2014-10-24]
    for (size_t i=0; i<significant.size(); ++i) {
        const P2::ControlFlowGraph::EdgeNode &edge = *partitioner.cfg().findEdge(i);
        if (significant[i] &&
            edge.source()->value().type()==P2::V_BASIC_BLOCK &&
            edge.target()->value().type()==P2::V_BASIC_BLOCK) {
            std::cerr <<"edge " <<StringUtility::addrToString(edge.source()->value().address())
                      <<":" <<StringUtility::addrToString(edge.target()->value().address())
                      <<"<" <<i <<"> is significant\n";
        }
    }
#endif

    return significant;
}
#endif

typedef std::vector<P2::ControlFlowGraph::ConstVertexNodeIterator> CfgPath;

// Print a CFG path
void
showPath(std::ostream &out, const CfgPath &path, const Settings &settings) {
    out <<"Path:\n";
    BOOST_FOREACH (const P2::ControlFlowGraph::ConstVertexNodeIterator &vertex, path) {
        if (vertex->value().type() == P2::V_INDETERMINATE) {
            out <<"  indeterminate\n";
        } else {
            out <<"  " <<StringUtility::addrToString(vertex->value().address());
            if (P2::Function::Ptr function = vertex->value().function())
                out <<" in " <<function->printableName();
            out <<"\n";
            if (settings.showInstructions) {
                if (P2::BasicBlock::Ptr bblock = vertex->value().bblock()) {
                    BOOST_FOREACH (SgAsmInstruction *insn, bblock->instructions())
                        out <<"    " <<unparseInstructionWithAddress(insn) <<"\n";
                }
            }
        }
    }
}

// Find shortest paths from the beginVertex to the target of the avoidEdge without going through avoidEdge.
void
findPaths(const P2::ControlFlowGraph &cfg,
          const P2::ControlFlowGraph::ConstVertexNodeIterator &beginVertex,
          const P2::ControlFlowGraph::ConstVertexNodeIterator &endVertex,
          const std::set<P2::ControlFlowGraph::ConstVertexNodeIterator> &avoidVertices,
          const std::set<P2::ControlFlowGraph::ConstEdgeNodeIterator> &avoidEdges,
          const Settings &settings) {
    using namespace Sawyer::Container::Algorithm;

    if (avoidVertices.find(beginVertex) != avoidVertices.end())
        return;                                         // all paths would start with a vertex-to-be-avoided
    if (avoidVertices.find(endVertex) != avoidVertices.end())
        return;                                         // all paths would end with a vertex-to-be-avoided

    CfgPath path;
    path.push_back(beginVertex);
    typedef DepthFirstForwardGraphTraversal<const P2::ControlFlowGraph> Traversal;
    for (Traversal t(cfg, beginVertex, EDGE_EVENTS); t; ++t) {
        if (t.event() == ENTER_EDGE) {
            path.push_back(t.edge()->target());
            if (avoidEdges.find(t.edge()) != avoidEdges.end()) {
                t.skipChildren();                       // do not follow this edge
            } else if (avoidVertices.find(t.edge()->target()) != avoidVertices.end()) {
                t.skipChildren();                       // do not follow this edge
            } else if (t.edge()->target() == endVertex) {
                showPath(std::cout, path, settings);
                t.skipChildren();                       // found; no need to go further
            }
        } else {
            ASSERT_require(t.event() == LEAVE_EDGE);
            ASSERT_require(!path.empty());
            ASSERT_require(path.back() == t.edge()->target());
            path.pop_back();
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
    Diagnostics::initialize();
    mlog = Diagnostics::Facility("tool", Diagnostics::destination);
    Diagnostics::mfacilities.insertAndAdjust(mlog);

    // Parse the command-line
    Settings settings;
    std::vector<std::string> specimenNames = parseCommandLine(argc, argv, settings).unreachedArgs();
    P2::Engine engine;
    if (!settings.isaName.empty())
        engine.disassembler(Disassembler::lookup(settings.isaName));
    if (specimenNames.empty())
        throw std::runtime_error("no specimen specified; see --help");

    // Disassemble and partition
    P2::Partitioner partitioner = engine.partition(specimenNames);

    // Find origin and destination vertices for path searching
    P2::ControlFlowGraph::ConstVertexNodeIterator cfgBegin = vertexForInstruction(partitioner, settings.beginVertex);
    if (cfgBegin == partitioner.cfg().vertices().end())
        throw std::runtime_error("no --begin vertex at " + settings.beginVertex);
    P2::ControlFlowGraph::ConstVertexNodeIterator cfgEnd = vertexForInstruction(partitioner, settings.endVertex);
    if (cfgEnd == partitioner.cfg().vertices().end())
        throw std::runtime_error("no --end vertex at " + settings.endVertex);

    // Which vertices should be avoided
    std::set<P2::ControlFlowGraph::ConstVertexNodeIterator> avoidVertices;
    BOOST_FOREACH (rose_addr_t va, settings.avoidVertices) {
        P2::ControlFlowGraph::ConstVertexNodeIterator vertex = vertexForInstruction(partitioner, va);
        if (vertex == partitioner.cfg().vertices().end())
            throw std::runtime_error("no vertex at " + StringUtility::addrToString(va));
        avoidVertices.insert(vertex);
    }

    // Which edges should be avoided
    std::set<P2::ControlFlowGraph::ConstEdgeNodeIterator> avoidEdges;
    ASSERT_require(0 == settings.avoidEdges.size() % 2);
    for (size_t i=0; i<settings.avoidEdges.size(); i+=2) {
        rose_addr_t sourceVa = settings.avoidEdges[i+0];
        rose_addr_t targetVa = settings.avoidEdges[i+1];
        P2::ControlFlowGraph::ConstEdgeNodeIterator edge = edgeForInstructions(partitioner, sourceVa, targetVa);
        if (edge == partitioner.cfg().edges().end()) {
            throw std::runtime_error("no edge from " + StringUtility::addrToString(sourceVa) +
                                     " to " + StringUtility::addrToString(targetVa));
        }
        avoidEdges.insert(edge);
    }

    // Show the configuration
    mlog[INFO] <<"start at vertex " <<partitioner.vertexName(cfgBegin) <<"\n";
    mlog[INFO] <<"end at vertex " <<partitioner.vertexName(cfgEnd) <<"\n";
    if (!avoidVertices.empty()) {
        mlog[INFO] <<"avoiding the following vertices:";
        BOOST_FOREACH (const P2::ControlFlowGraph::ConstVertexNodeIterator &vertex, avoidVertices)
            mlog[INFO] <<" " <<partitioner.vertexName(vertex) <<";";
        mlog[INFO] <<"\n";
    }
    if (!avoidEdges.empty()) {
        mlog[INFO] <<"avoiding the following edges:";
        BOOST_FOREACH (const P2::ControlFlowGraph::ConstEdgeNodeIterator &edge, avoidEdges)
            mlog[INFO] <<" " <<partitioner.edgeName(edge) <<";";
        mlog[INFO] <<"\n";
    }

    // Do the real work
    findPaths(partitioner.cfg(), cfgBegin, cfgEnd, avoidVertices, avoidEdges, settings);
}
