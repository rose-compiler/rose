#include <rose.h>
#include <RSIM_Private.h>
#ifdef ROSE_ENABLE_SIMULATOR                            // protects this whole file

#include <RSIM_Linux32.h>

using namespace rose::BinaryAnalysis;

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

        std::string rawFileName = namePrefix + StringUtility::addrToString(interval.least()) + ".raw";
        std::ofstream rawFile(rawFileName.c_str());
        uint8_t buf[8192];
        for (rose_addr_t va=interval.least(); va<=interval.greatest(); va+=sizeof buf) {
            size_t nread = mm.at(va).limit(sizeof buf).read(buf).size();
            rawFile.write((const char*)buf, nread);
            if (nread < sizeof buf)
                break;                                  // might be an error, with nread==0
        }

        index <<"map:" <<StringUtility::addrToString(interval.least()) <<"=";
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

// Effectively a breakpoint at the program original entry point (after the dynamic linker has a chance to be emulated), at
// which time we dump all the specimen's memory to files.
class MemoryDumper: public RSIM_Callbacks::InsnCallback {
public:
    rose_addr_t breakpoint;
    bool triggered;

    MemoryDumper(rose_addr_t breakpoint): breakpoint(breakpoint), triggered(false) {}

    virtual MemoryDumper* clone() { return this; }      // all threads share same object

    virtual bool operator()(bool enabled, const Args &args) {
        if (enabled && !triggered && args.insn->get_address() == breakpoint) {
            triggered = true;
            dumpAllMemory(args.thread->get_process()->get_simulator(), "x-");
        }
        return enabled;
    }
};

// Example of accumulating info about where instructions are executed.  We use AddressIntervalSet to track which addresses are
// accessed; it's slower than std::vector<bool> but will behave better for programs that have executable segments widely
// scattered in memory. We could have used std::set<rose_addr_t> but the STL's set-theoretic API is not so nice.
class InstructionCoverageTracker: public RSIM_Callbacks::InsnCallback {
public:
    AddressIntervalSet coverage;                       // addresses that were executed (entire insns, not just first addr)

    virtual InstructionCoverageTracker* clone() { return this; } // all threads share same object

    virtual bool operator()(bool enabled, const Args &args) {
        if (enabled)
            coverage.insert(AddressInterval::baseSize(args.insn->get_address(), args.insn->get_size()));
        return enabled;
    }
};

class MemoryAccessTracker: public RSIM_Callbacks::MemoryCallback {
public:
    AddressIntervalSet readCoverage, writeCoverage;

    virtual MemoryAccessTracker* clone() { return this; } // all threads share same object

    virtual bool operator()(bool enabled, const Args &args) {
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int
main(int argc, char *argv[], char *envp[]) {

    // Configure the simulator
    RSIM_Linux32 sim;
    int n = sim.configure(argc, argv, envp);
    InstructionCoverageTracker *insnTracker = new InstructionCoverageTracker;
    sim.install_callback(insnTracker);
    MemoryAccessTracker *dataTracker = new MemoryAccessTracker;
    sim.install_callback(dataTracker);

    // Load the specimen to create the initial process and main thread (which does not start executing yet)
    sim.exec(argc-n, argv+n);

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

    // Deregister signal handlers if we activated them above.  This step isn't strictly necessary here since the specimen has
    // already terminated and we're about to do the same.
    if (doActivate)
        sim.deactivate();

    // Print instruction coverage info
    {
        const AddressIntervalSet &r = dataTracker->readCoverage;
        const AddressIntervalSet &w = dataTracker->writeCoverage;
        const AddressIntervalSet &x = insnTracker->coverage;
        using namespace StringUtility;
        std::string units = "distinct addresses";

        std::cout <<"read:  " <<plural(r.size(), units) <<"\n";
        std::cout <<"write: " <<plural(w.size(), units) <<"\n";
        std::cout <<"exec:  " <<plural(x.size(), units) <<"\n";

        std::cout <<"write & exec: " <<plural((w & x).size(), units) <<"\n";
        std::cout <<"write & !read: " <<plural((w - r).size(), units) <<"\n";
        std::cout <<"exec & !read:  " <<plural((x - r).size(), units) <<" (should be zero)\n";
    }

    // Describe specimen termination and then exit ourselves with the samem status.
    sim.describe_termination(stderr);
    sim.terminate_self();                               // probably doesn't return
    return 0;
}

#else
int main(int, char *argv[])
{
    std::cerr <<argv[0] <<": not supported on this platform" <<std::endl;
    return 0;
}
#endif
