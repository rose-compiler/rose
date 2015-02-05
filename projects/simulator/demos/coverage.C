#include <rose.h>
#include <RSIM_Private.h>
#ifdef ROSE_ENABLE_SIMULATOR                            // protects this whole file

#include <AsmFunctionIndex.h>
#include <AsmUnparser.h>
#include <Diagnostics.h>
#include <RSIM_Linux32.h>

// Use partitoner version 2?  It's faster, better quality results, and has analysis-oriented data structures. As of Dec 2014
// it's still under development and might require Robb's m68k branch for the latest features.
#define USE_PARTITIONER_2

#ifdef USE_PARTITIONER_2
#include <Partitioner2/Engine.h>                        // New API for partitioning instructions into functions
#endif

using namespace rose;
using namespace StringUtility;                          // part of ROSE, contains addrToString and plural
using namespace rose::BinaryAnalysis;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Supporting functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Example of dumping all specimen memory to files.  Since recent ROSE tools understand the "map:" schema for loading
// raw data into memory maps, that's what we'll generate here.  We'll generate one output file for each memory segment, plus a
// file that contains mapping information.  It so happens that the mapping info file is a list of command-line arguments, and
// thus can be specified to other tools with the "@index" format.
static void
dumpAllMemory(RSIM_Simulator *sim, const std::string &namePrefix) {
    std::string indexName = namePrefix + "index";
    std::ofstream index(indexName.c_str());

    sim->get_process()->get_memory().dump(std::cerr, "Dumping: ");

    const MemoryMap &mm = sim->get_process()->get_memory();
    BOOST_FOREACH (const MemoryMap::Node &node, mm.nodes()) {
        const AddressInterval &interval = node.key();
        const MemoryMap::Segment &segment = node.value();

        // Be careful about reading from memory that is mapped without read access since this will result in a segfault to the
        // simulator. True memory which lacks read access will also lack the READABLE bit in the simulated memory, but not
        // necessarily vice versa.  We're taking the easy way out here.
        if (0 == (segment.accessibility() & MemoryMap::READABLE))
            continue;

        std::string rawFileName = namePrefix + addrToString(interval.least()) + ".raw";
        std::ofstream rawFile(rawFileName.c_str());
        uint8_t buf[8192];
        for (rose_addr_t va=interval.least(); va<=interval.greatest(); va+=sizeof buf) {
            size_t nread = mm.at(va).limit(sizeof buf).read(buf).size();
            rawFile.write((const char*)buf, nread);
            if (nread < sizeof buf)
                break;                                  // might be an error, with nread==0
        }

        index <<"map:" <<addrToString(interval.least()) <<"=";
        if (0 != (segment.accessibility() & MemoryMap::READABLE))
            index <<"r";
        if (0 != (segment.accessibility() & MemoryMap::WRITABLE))
            index <<"w";
        if (0 != (segment.accessibility() & MemoryMap::EXECUTABLE))
            index <<"x";
        index <<"::" <<rawFileName <<"\n";
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Callbacks
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Effectively a breakpoint at the program original entry point (after the dynamic linker has a chance to be emulated), at
// which time we dump all the specimen's memory to files. The clone method is used to create a new instance of this callback
// whenever a new process or thread is created.  We're not supporting multi-threaded apps in this demo, so all our callbacks
// will just return the same object.
class MemoryDumper: public RSIM_Callbacks::InsnCallback {
public:
    rose_addr_t breakpoint;
    bool triggered;

    MemoryDumper(rose_addr_t breakpoint): breakpoint(breakpoint), triggered(false) {}

    MemoryDumper* clone() ROSE_OVERRIDE { return this; } // all threads share same object

    bool operator()(bool enabled, const Args &args) ROSE_OVERRIDE {
        if (enabled && !triggered && args.insn->get_address() == breakpoint) {
            triggered = true;
            dumpAllMemory(args.thread->get_process()->get_simulator(), "x-begin-");
        }
        return enabled;
    }
};

// Example of accumulating info about where instructions are executed.
class InstructionCoverageTracker: public RSIM_Callbacks::InsnCallback {
public:
    AddressIntervalSet &coverage;                      // addresses that were executed (entire insns, not just first addr)

    InstructionCoverageTracker(AddressIntervalSet &s): coverage(s) {}

    InstructionCoverageTracker* clone() ROSE_OVERRIDE { return this; } // all threads share same object

    bool operator()(bool enabled, const Args &args) ROSE_OVERRIDE {
        if (enabled)
            coverage.insert(AddressInterval::baseSize(args.insn->get_address(), args.insn->get_size()));
        return enabled;
    }
};

// Called each time the specimen reads or writes memory, including reading memory to obtain the instructions to executed.  The
// callback args will allow you to distinguish between reading an instruction and reading other memory if you need that.
class MemoryAccessTracker: public RSIM_Callbacks::MemoryCallback {
public:
    AddressIntervalSet &readCoverage, &writeCoverage;

    MemoryAccessTracker(AddressIntervalSet &r, AddressIntervalSet &w): readCoverage(r), writeCoverage(w) {}

    MemoryAccessTracker* clone() ROSE_OVERRIDE { return this; } // all threads share same object

    bool operator()(bool enabled, const Args &args) ROSE_OVERRIDE {
        if (enabled) {
            if (args.how == MemoryMap::READABLE) {      // includes instruction fetch; see docs for other possibilities
                readCoverage.insert(AddressInterval::baseSize(args.va, args.nbytes));
            } else if (args.how == MemoryMap::WRITABLE) {
                writeCoverage.insert(AddressInterval::baseSize(args.va, args.nbytes));
            }
        }
        return enabled;
    }
};

// Callback to spit out the names of all files that are opened with the open system call. You can get the list of 32-bit
// syscall numbers from the top of RSIM_Linux32.C.  The open system call is #5 and you can see what arguments it takes by
// looking at the syscall_open_enter function, looking at the output from the strace tool, or running "x86sim --debug=syscall".
// It takes a string and one or two bit vectors.
class OpenSyscallTracker: public RSIM_Callbacks::SyscallCallback {
public:
    OpenSyscallTracker* clone() ROSE_OVERRIDE { return this; }

    bool operator()(bool enabled, const Args &args) ROSE_OVERRIDE {
        if (enabled && args.callno == 5 /*open*/) {

            // If this callback is invoked after the syscall then we can get the syscall result from the EAX register and
            // report the file name only if the open was successful. Whether it's invoked before or after depends on how it was
            // registered with the simulator -- if you do it both ways then you'll probably want two callback objects with some
            // extra state.
            const RegisterDescriptor &EAX = args.thread->policy.reg_eax;
            int32_t retval = args.thread->policy.readRegister<32>(EAX).known_value();
            if (retval >= 0) {
                RSIM_Process *proc = args.thread->get_process();
                uint32_t filename_va=args.thread->syscall_arg(0);
                std::string filename = proc->read_string(filename_va);

                // We just print the string without regard for messing up the specimen's standard error stream!
                std::cerr <<"specimen successfully opened \"" <<cEscape(filename) <<"\"\n";
            }
        }
        return enabled;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int
main(int argc, char *argv[], char *envp[]) {
    AddressIntervalSet r, w, x;                         // Accumulates read, write, and executed memory addresses

    //--------------------
    // Do the simulation
    //--------------------

    // Configure the simulator
    RSIM_Linux32 sim;
    int n = sim.configure(argc, argv, envp);
    sim.install_callback(new RSIM_Tools::UnhandledInstruction); // handles some common insns missing from ROSE
    InstructionCoverageTracker *insnTracker = new InstructionCoverageTracker(x);
    sim.install_callback(insnTracker);
    MemoryAccessTracker *dataTracker = new MemoryAccessTracker(r, w);
    sim.install_callback(dataTracker);
    sim.install_callback(new OpenSyscallTracker, RSIM_Callbacks::AFTER); // invoke after the syscall returns

    // Load the specimen to create the initial process and main thread (which does not start executing yet). Some binary
    // containers have more than one interpretation and we might need this for later.
    sim.exec(argc-n, argv+n);
    SgAsmInterpretation *interp = sim.get_process()->get_interpretation();

    // Set up signal handlers, etc.  This is optional and can be commented to make debugging easier.
    static const bool doActivate = false;
    if (doActivate)
        sim.activate();

    // Arrange to dump all memory when we reach the OEP (this happens after the dynamic linker is emulated if there is one).
    // This callback needs to be explicitly installed on the main thread. The callbacks above could be installed on the
    // simulator because they end up being copied into all new threads, but by now the main thread has already been created.
    sim.get_process()->get_main_thread()->install_callback(new MemoryDumper(sim.get_process()->get_ep_orig_va()));

    // Allow main thread to start running.  This returns when the simulated process terminates.
    sim.main_loop();
    sim.describe_termination(stdout);

    // Deregister signal handlers if we activated them above.  This step isn't strictly necessary here since the specimen has
    // already terminated and we're about to do the same.
    if (doActivate)
        sim.deactivate();


    //------------------
    // Print some stats
    //------------------

    std::string units = "distinct addresses";
    std::cout <<"read:  " <<plural(r.size(), units) <<"\n";
    std::cout <<"write: " <<plural(w.size(), units) <<"\n";
    std::cout <<"exec:  " <<plural(x.size(), units) <<"\n";
    std::cout <<"write & exec: " <<plural((w & x).size(), units) <<"\n";
    std::cout <<"write & !read: " <<plural((w - r).size(), units) <<"\n";
    std::cout <<"exec & !read:  " <<plural((x - r).size(), units) <<" (should be zero)\n";

    // Example of printing the intervals.  AddressInterval is a typedef for Sawyer::Container::Interval<rose_addr_t> which is
    // documented at the Sawyer webiste [http://github.com/matzke1/sawyer]. You could also use "std::cout <<interval" if you're
    // not choosy about the format.
    AddressIntervalSet wx = w & x;                      // hold a result for sake of BOOST_FOREACH
    BOOST_FOREACH (const AddressInterval &interval, wx.intervals())
        std::cout <<addrToString(interval.least()) <<"\t" <<addrToString(interval.greatest()) <<"\n";

    // Dump memory again so we can analyze later
    dumpAllMemory(&sim, "x-end-");

    //------------- 
    // Disassemble
    //-------------

#ifdef USE_PARTITIONER_2
    // Using version two of the partitioner API.  Version 2 is about 3x faster than version 1 and usually gives better results.
    // Since is currently being developed [Dec 2014] you may need Robb's m68k branch for the latest features.
    namespace P2 = rose::BinaryAnalysis::Partitioner2;

    // The new partitioner is experimental [Dec 2014] and may spit out an aweful lot of warnings that usually aren't too
    // interesting.  They can be turned off like this:
    Diagnostics::mfacilities.control("rose::BinaryAnalysis::Partitioner2(!warn)");

    // We could disassemble the final memory image at this point, which gives different results than disassembling the
    // executable itself.  For instance, the final memory will contain all the dynamically linked functions, final data
    // structures, etc.  The specimen has completed at this point, so we might as well mark all its memory as read-only; this
    // will cause the partitioner to treat it as immutable so things like "JMP [x]" where x is an address will result in a
    // known CFG successor, and will be good for things like dynamically linked functions, but perhaps not so good for other
    // stuff. Try it both ways.
    MemoryMap mm = sim.get_process()->get_memory();
    mm.any().changeAccess(0, MemoryMap::WRITABLE);      // adds nothing, removes writable

    // The new partioning API uses a customizable Engine to drive a Partitioner final class.
    P2::Engine engine;                                  // use the engine supplied by ROSE
    engine.memoryMap(mm);                               // use this memory rather than the one from the ELF container
    engine.postPartitionAnalyses(false);                // no need for this expensive final step
    P2::Partitioner partitioner = engine.partition(interp);

    // We've already parsed the ELF container and created an AST, but we haven't yet created the part of the AST representing
    // functions, basic blocks, instructions, and expressions.  Most user-level analysis is written in terms of the AST, so
    // we'll create one here for demo purposes.  The global block will contain children which are functions (SgAsmFunction),
    // whose children are basic blocks (SgAsmBlock), whose children are instructions (SgAsmInstruction).
    SgAsmBlock *gblock = engine.buildAst(partitioner);

    // Since we're not destroying the partitioner, we should make sure its progress message is cleaned up.
    P2::mlog[Diagnostics::MARCH] <<"done partitioning\n";
#else
    // The simulator uses Partitioner version 1, so there's a simple wrapper for it.
    SgAsmBlock *gblock = sim.get_process()->disassemble();
#endif

    // Print the disassembly. Since we dumped memory, you could also use any number of other tools to disassemble. The
    // $ROSE_SRC/projects/BinaryAnalysisTools has some good stuff, like recursiveDisassemble and bROwSE.  Try running them with
    // "--help" for details.  For instance,
    //   $ recursiveDisassemble --isa=i386 @x-index
    if (0)
        AsmUnparser().unparse(std::cout, gblock);

    // Print a list of functions we found.
    if (0)
        std::cout <<AsmFunctionIndex(gblock);

    // Number of AST nodes
    struct AstSize: AstSimpleProcessing {
        size_t nNodes;
        AstSize(): nNodes(0) {}
        void visit(SgNode *) { ++nNodes; }
    } sizer;
    sizer.traverse(gblock, preorder);
    std::cout <<"Interpretation global block contains " <<plural(sizer.nNodes, "nodes") <<"\n";
    

    //---------- 
    // Analysis
    //----------

    // Build a set containing all known instruction addresses.
    AddressIntervalSet knownInsns;
#ifdef USE_PARTITIONER_2
    BOOST_FOREACH (const P2::BasicBlock::Ptr &bb, partitioner.basicBlocks())
        knownInsns.insertMultiple(partitioner.basicBlockInstructionExtent(bb));
#else // the following also works with the new partitioner
    class GetInstructionIntervals: public AstSimpleProcessing {
    public:
        AddressIntervalSet &knownInsns;
        GetInstructionIntervals(AddressIntervalSet &knownInsns): knownInsns(knownInsns) {}
        void visit(SgNode *node) {
            if (SgAsmInstruction *insn = isSgAsmInstruction(node))
                knownInsns.insert(AddressInterval::baseSize(insn->get_address(), insn->get_size()));
        }
    } visitor(knownInsns);
    visitor.traverse(gblock, preorder);
#endif

    std::cout <<"Instruction coverage info:\n"
              <<"  Addresses disassembled:  "       <<knownInsns.size() <<"\n"
              <<"  Addresses executed:      "       <<x.size() <<"\n"
              <<"  Executed but not disassembled: " <<(x-knownInsns).size() <<"\n"
              <<"  Disassembled but not executed: " <<(knownInsns-x).size() <<"\n";

    // Terminate ourselves with the same status as the specimen.
    sim.terminate_self();
}

#else
int main(int, char *argv[])
{
    std::cerr <<argv[0] <<": not supported on this platform" <<std::endl;
}
#endif
