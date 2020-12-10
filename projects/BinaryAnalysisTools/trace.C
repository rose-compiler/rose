static const char *purpose = "demonstrates how to use Sawyer::Trace";
static const char *description =
    "Parses and disassembles an executable to obtain a global control flow graph (CFG).  The CFG is constructed such that each "
    "vertex represents an instruction rather than a basic block in order to simplify this demo, but a production analysis would "
    "probably use basic blocks since that would be more efficient.  This demo then runs the executable with the specified "
    "command-line arguments to obtain an execution trace and the trace is immediately mapped onto the CFG.  Finally, the demo "
    "prints some interesting information about the trace:"

    "@numbered{A list of instructions in the CFG that were never executed. This is a simple code-coverage.}"

    "@numbered{A list of executed instructions that do not appear in the CFG.  This is the inverse idea of the previous "
    "bullet and indicates where ROSE's static analysis failed to find executable instructions.}"

    "@numbered{A list of instructions that have multiple successors in the CFG but only one successor in the trace. These "
    "are branches that were never taken or always taken.}"

    "@numbered{A list of instructions having multiple trace successors, sorted by decreasing burstiness.  These represent "
    "branches where hints controlling speculative execution might improve performance.}";

#include <rose.h>                                       // MUST BE FIRST

#include <BinaryDebugger.h>                             // ROSE header
#include <boost/algorithm/string/join.hpp>
#include <Diagnostics.h>                                // ROSE header
#include <Partitioner2/Engine.h>                        // ROSE header
#include <Sawyer/Trace.h>
#include <StringUtility.h>                              // ROSE header

using namespace Rose;
using namespace Sawyer::Message::Common;                // INFO, WARN, ERROR, FATAL, etc.
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

Sawyer::Message::Facility mlog;

// Index fields for an instruction. We want to index an instruction just by its starting virtual address.
class InstructionKey {
    rose_addr_t startVa_;
public:
    InstructionKey(): startVa_(0) {}
    /*implicit*/ InstructionKey(rose_addr_t va): startVa_(va) {}
    explicit InstructionKey(const SgAsmInstruction *insn): startVa_(insn->get_address()) {}
    bool operator<(const InstructionKey &other) const { return startVa_ < other.startVa_; }
};

// A control flow graph whose vertices are instructions that are indexed by their starting address.
typedef Sawyer::Container::Graph<SgAsmInstruction*, Sawyer::Nothing, InstructionKey> InsnCfg;

// An instruction trace whose labels are the ID numbers of a CFG. We could have just as easily used labels which are the CFG
// vertex pointers (i.e., InsnCfg::VertexIterator) but doing so would require using a O(log n) index, whereas integer IDs can
// use an O(1) index.  It's also possible to use a hash map for the index, which would give amortized O(1) lookups even for
// vertex pointers.
typedef Sawyer::Container::Trace<size_t /*vertex ID*/, Sawyer::Container::TraceVectorIndexTag> Trace;

// Trace info per instruction. We could have also attached this info to the instruction ASTs as an attribute, but we want to
// use it to build a sorted table for output instead.
struct InsnTraceInfo {
    SgAsmInstruction *insn;                             // the instruction's AST
    double burstiness;                                  // 0..1 "burstiness" of the trace; see Trace documentation
    size_t nHits;                                       // number of times the trace hits this instruction

    InsnTraceInfo()
        : insn(NULL), burstiness(NAN), nHits(0) {}

    InsnTraceInfo(SgAsmInstruction *insn, double burstiness, size_t nHits)
        : insn(insn), burstiness(burstiness), nHits(nHits) {}

    bool operator<(const InsnTraceInfo &other) const {
        return burstiness < other.burstiness;
    }
};

// Test whether a control flow graph vertex is an existing basic block.  Binary CFG vertices can also represent unknown
// addresses for branches whose target could not be determined statically, addresses where no attempt has been made to discover
// instructions, addresses where instructions could not be created because there's no memory mapped there, etc. This is a
// super-predicate: besides returning a truth value, it returns something useful for true.
P2::BasicBlock::Ptr isBasicBlock(const P2::ControlFlowGraph::Vertex &v) {
    return v.value().type()==P2::V_BASIC_BLOCK && v.value().bblock() != NULL && !v.value().bblock()->isEmpty() ?
                  v.value().bblock() : P2::BasicBlock::Ptr();
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");

    // Parse the command-line to configure the partitioner engine, obtain the executable and its arguments, and generate a man
    // page, adjust global settings, etc. This demo tool has no switches of its own, which makes this even easier. For a
    // production tool, it's probably better to obtain the parser and register only those switches we need (e.g., no need for
    // AST generation switches since we skip that step), to set it up to use our own diagnostic stream instead of exceptions,
    // and to adjust this tool's synopsis in the documentation.  Examples of all of these can be found in other demos.
    P2::Engine engine;
    engine.doingPostAnalysis(false);                    // no need for any post-analysis phases (user can override on cmdline)
    BinaryAnalysis::Debugger::Specimen specimen;
    try {
        std::vector<std::string> tmp = engine.parseCommandLine(argc, argv, purpose, description).unreachedArgs();
        if (tmp.empty()) {
            mlog[FATAL] <<"no executable specified\n";
            exit(1);
        }
        specimen.program(tmp[0]);
        tmp.erase(tmp.begin());
        specimen.arguments(tmp);
        specimen.flags().set(BinaryAnalysis::Debugger::CLOSE_FILES);
    } catch (const std::runtime_error &e) {
        mlog[FATAL] <<"invalid command-line: " <<e.what() <<"\n";
        exit(1);
    }

    // Since we'll be tracing this program's execution, we might as well disassemble the process's memory directly. That way we
    // don't have to worry about ROSE mapping the specimen to the same virtual address as the kernel (which might be using
    // address randomization). We can stop short of generating the AST because we won't need it.
    BinaryAnalysis::Debugger::Ptr debugger = BinaryAnalysis::Debugger::instance(specimen);
    std::string specimenResourceName = "proc:noattach:" + StringUtility::numberToString(debugger->isAttached());
    P2::Partitioner partitioner = engine.partition(specimenResourceName);
    partitioner.memoryMap()->dump(std::cerr);           // show the memory map as a debugging aid

    // Create a global control flow graph whose vertices are instructions from a global CFG whose verts are mostly basic
    // blocks.
    InsnCfg insnCfg;
    const P2::ControlFlowGraph &bbCfg = partitioner.cfg();
    BOOST_FOREACH (const P2::ControlFlowGraph::Vertex &bbVert, bbCfg.vertices()) {
        if (P2::BasicBlock::Ptr bb = isBasicBlock(bbVert)) {
            const std::vector<SgAsmInstruction*> &insns = bb->instructions();

            // Each basic block has one or more instructions that need to be inserted into our instruction control flow graph
            // with edges from each instruction to the next.  The insertEdgeWithVertices automatically inserts missing
            // vertices, and doesn't insert vertices that already exist, making it convenient for this type of construction.
            for (size_t i=1; i<insns.size(); ++i)
                insnCfg.insertEdgeWithVertices(insns[i-1], insns[i]);

            // The final instruction of this block needs to flow into each of the initial instructions of the successor basic
            // blocks. Be careful that the successors are actually existing basic blocks.  Note that in ROSE's global CFG, a
            // function call has at least two successors: the function being called (normal edges), and the address to which
            // the function returns ("callret" edges). There are other types of edges too, but we want only the normal edges.
            BOOST_FOREACH (const P2::ControlFlowGraph::Edge &bbEdge, bbVert.outEdges()) {
                if (bbEdge.value().type() == P2::E_NORMAL) {
                    if (P2::BasicBlock::Ptr target = isBasicBlock(*bbEdge.target()))
                        insnCfg.insertEdgeWithVertices(insns.back(), target->instructions()[0]);
                }
            }
        }
    }
    mlog[INFO] <<"block CFG: "
               <<StringUtility::plural(bbCfg.nVertices(), "vertices", "vertex") <<", "
               <<StringUtility::plural(bbCfg.nEdges(), "edges") <<"\n";
    mlog[INFO] <<"insn CFG:  "
               <<StringUtility::plural(insnCfg.nVertices(), "vertices", "vertex") <<", "
               <<StringUtility::plural(insnCfg.nEdges(), "edges") <<"\n";
    
    // Run the executable to obtain a trace.  We use the instruction pointer to look up a SgAsmInstruction in the insnCfg and
    // thus map the trace onto the instruction CFG.
    mlog[INFO] <<"running subordinate to obtain trace: " <<specimen <<"\n";
    std::set<rose_addr_t> missingAddresses;
    Trace trace;
    while (!debugger->isTerminated()) {
        // Find the instruction CFG vertex corresponding to the current execution address. It could be that the execution
        // address doesn't exist in the CFG, and this can be caused by a number of things including failure of ROSE to
        // statically find the address, dynamic libraries that weren't loaded statically, etc.
        rose_addr_t va = debugger->executionAddress();
        InsnCfg::ConstVertexIterator vertex = insnCfg.findVertexKey(va);
        if (!insnCfg.isValidVertex(vertex)) {
            missingAddresses.insert(va);
        } else {
            trace.append(vertex->id());
        }
        debugger->singleStep();
    }
    mlog[INFO] <<"subordinate " <<debugger->howTerminated() <<"\n";
    mlog[INFO] <<"trace length: " <<StringUtility::plural(trace.size(), "instructions") <<"\n";
    Diagnostics::mfprintf(mlog[INFO])("overall burstiness: %6.2f%%\n", 100.0 * trace.burstiness());
    mlog[INFO] <<"distinct executed addresses missing from CFG: " <<missingAddresses.size() <<"\n";

    // Print a list of CFG vertices that were never reached.  We use std::cout rather than diagnostics because this is one of
    // the main outputs of this demo. The "if" condition is constant time.
    BOOST_FOREACH (const InsnCfg::Vertex &vertex, insnCfg.vertices()) {
        if (!trace.exists(vertex.id()))
            std::cout <<"not executed: " <<partitioner.unparse(vertex.value()) <<"\n";
    }

    // Print list of addresses that were executed but did not appear in the CFG
    BOOST_FOREACH (rose_addr_t va, missingAddresses)
        std::cout <<"missing address: " <<StringUtility::addrToString(va) <<"\n";

    // Print those branch instructions that were executed by the trace but always took the same branch.  Just to mix things up,
    // I'll iterate over the trace labels this time instead of the CFG vertices.  Remember, the labels are the integer IDs of
    // the CFG vertices. The "if" condition executes in constant time, as does the next line.
    for (size_t i = 0; i < trace.nLabels(); ++i) {
        if (insnCfg.findVertex(i)->nOutEdges() > 1 && trace.successors(i).size() == 1) {
            SgAsmInstruction *successor = insnCfg.findVertex(*trace.successorSet(i).begin())->value();
            std::cout <<"single flow: " <<partitioner.unparse(insnCfg.findVertex(i)->value())
                      <<" --> " <<partitioner.unparse(successor) <<"\n";
        }
    }

    // Get a list of executed instructions that are branch points and sort them by their burstiness.  The "if" condition is
    // constant time.
    std::vector<InsnTraceInfo> info;
    BOOST_FOREACH (const InsnCfg::Vertex &vertex, insnCfg.vertices()) {
        if (vertex.nOutEdges() > 1 && trace.exists(vertex.id()))
            info.push_back(InsnTraceInfo(vertex.value(), trace.burstiness(vertex.id()), trace.size(vertex.id())));
    }
    std::sort(info.begin(), info.end());
    std::reverse(info.begin(), info.end());
    BOOST_FOREACH (const InsnTraceInfo &record, info) {
        Diagnostics::mfprintf(std::cout)("burstiness %6.2f%% %5zu hits at %s\n",
                                         100.0*record.burstiness, record.nHits,
                                         partitioner.unparse(record.insn).c_str());
    }
}
