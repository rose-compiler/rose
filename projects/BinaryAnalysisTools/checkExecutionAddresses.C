#include <rose.h>

#include <boost/foreach.hpp>
#include <rose_getline.h>
#include <rose_strtoull.h>
#include <BinaryDebugger.h>
#include <Partitioner2/Engine.h>
#include <Sawyer/CommandLine.h>
#include <Sawyer/Message.h>
#include <Sawyer/ProgressBar.h>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;
using namespace StringUtility;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

static Sawyer::Message::Facility mlog;

typedef Sawyer::Container::Map<rose_addr_t, size_t> AddressCounts;

enum MapSource { MAP_NATIVE, MAP_ROSE };

struct Settings {
    std::string addressFile;                            // name of file containing expected addresses
    bool trace;                                         // generate a *.trace file?
    MapSource mapSource;                                // how to create the memory map
    bool showExpected;                                  // show totals for expected addresses
    bool showUnexpected;                                // show totals for unexpected addresses
    bool showUnmapped;                                  // show totals for unmapped addresses
    Settings(): trace(false), mapSource(MAP_ROSE), showExpected(false), showUnexpected(false), showUnmapped(false) {}
};

// Describe and parse the command-line
static std::vector<std::string>
parseCommandLine(int argc, char *argv[], P2::Engine &engine, Settings &settings)
{
    using namespace Sawyer::CommandLine;

    std::string purpose = "compares actual execution with known instructions";
    std::string description = 
        "Reads instruction addresses from a file, the so-called \"expected\" addresses and and then executes the specimen "
        "and compares actual executed addresses with the expected addresses.  An actual executed address falls into one of "
        "three categories:  (1) the address is an expected address, or else (2) the address is not mapped, or else (3) the "
        "address not expected.\n\n"

        "One method of obtaining a list of expected addresses is to use the @man{recursiveDisassemble}{--help} tool's "
        "@s{list-instruction-addressses}{noerror} switch. Although this produces output that contains instruction sizes "
        "as well as addresses, @prop{programName} ignores the sizes.  This can be used to test whether a process executes any "
        "instructions that were not also disassembled, thereby testing some aspect of the disassembly quality.";

    // The parser is the same as that created by Engine::commandLineParser except we don't need any disassemler or partitioning
    // switches since this tool doesn't disassemble or partition.
    Parser parser;
    parser
        .purpose(purpose)
        .version(std::string(ROSE_SCM_VERSION_ID).substr(0, 8), ROSE_CONFIGURE_DATE)
        .chapter(1, "ROSE Command-line Tools")
        .doc("Synopsis",
             "@prop{programName} [@v{switches}] @v{address_file} @v{specimen_name} @v{specimen_arguments}...")
        .doc("Description", description)
        .doc("Specimens", engine.specimenNameDocumentation())
        .with(engine.engineSwitches())
        .with(engine.loaderSwitches());
    
    SwitchGroup tool("Tool specific switches");
    tool.name("tool");
    tool.insert(Switch("map")
                .argument("how", enumParser(settings.mapSource)
                          ->with("native", MAP_NATIVE)
                          ->with("rose", MAP_ROSE))
                .doc("Specifies how the memory map should be obtained, where @v{how} is either \"native\" to obtain the map "
                     "from a running process, or \"rose\" to obtain the map by parsing the specimen container with ROSE.  When "
                     "obtained natively the map may contain addresses that were not visible to the original disassembler. When "
                     "obtained from ROSE the map might not be identical to map actually used by the process. The default is \"" +
                     std::string(MAP_ROSE==settings.mapSource?"rose":"native") + "\"."));

    tool.insert(Switch("trace")
                .intrinsicValue(true, settings.trace)
                .doc("When @s{trace} is specified each execution address is printed to a file named \"@v{pid}.trace\" where "
                     "@v{pid} is the process ID of the specimen.  Each line of the file will contain the following "
                     "space-separated fields:"
                     "@bullet{The hexadecimal address of the instruction that was executed.}"
                     "@bullet{The number of times this address has been executed so far.}"
                     "@bullet{The letter '1' or '0' to indicate whether the address known (from the @v{address_file}) or not.}"
                     "The @s{no-trace} switch disables tracing. The default is to " + std::string(settings.trace?"":"not ") +
                     "produce a trace."));
    tool.insert(Switch("no-trace")
                .key("trace")
                .intrinsicValue(false, settings.trace)
                .hidden(true));

    tool.insert(Switch("show-expected")
                .intrinsicValue(true, settings.showExpected)
                .doc("List addresses that were expected and show how many times each was executed.  The output will be one line "
                     "per address, containing a hexadecimal address and a decimal count separated by white space.  The "
                     "@s{no-show-expected} switch turns this listing off.  The default is to " +
                     std::string(settings.showExpected?"":"not ") + "show this information."));
    tool.insert(Switch("no-show-expected")
                .key("show-expected")
                .intrinsicValue(false, settings.showExpected)
                .hidden(true));
    
    tool.insert(Switch("show-unexpected")
                .intrinsicValue(true, settings.showUnexpected)
                .doc("List the addresses that were executed where no instruction was expected.  The output will be one line per "
                     "address, containing a hexadecimal address and the number of times the address was executed separated by "
                     "white space.  The @s{no-show-unexpected} switch turns this listing off.  The default is to " +
                     std::string(settings.showUnexpected?"":"not ") + "show this information."));
    tool.insert(Switch("no-show-unexpected")
                .key("show-unexpected")
                .intrinsicValue(false, settings.showUnexpected)
                .hidden(true));

    tool.insert(Switch("show-unmapped")
                .intrinsicValue(true, settings.showUnmapped)
                .doc("List addresses that were executed but are not present in the memory map.  These are probably instructions "
                     "that belong to the dynamic linker, dynamically-linked libraries, or virtual dynamic shared objects.  The "
                     "@s{no-show-unmapped} switch turns this listing off.  The default is to " +
                     std::string(settings.showUnmapped?"":"not ") + "show this information."));
    tool.insert(Switch("no-show-unmapped")
                .key("show-unmapped")
                .intrinsicValue(false, settings.showUnmapped)
                .hidden(true));

    return parser.with(tool).parse(argc, argv).apply().unreachedArgs();
}

// File has one address per line
static std::set<rose_addr_t>
parseAddressFile(const std::string &fileName) {
    std::set<rose_addr_t> retval;
    FILE *file = fopen(fileName.c_str(), "r");
    ASSERT_always_require2(file!=NULL, "cannot open address file for reading: " + fileName + ": " + strerror(errno));
    char *buf = NULL;
    size_t bufsz = 0;
    while (rose_getline(&buf, &bufsz, file)>0)
        retval.insert(rose_strtoull(buf, NULL, 0));
    free(buf);
    fclose(file);
    return retval;
}

static inline bool
isGoodAddr(const std::set<rose_addr_t> &goodVas, const MemoryMap::Ptr &map, rose_addr_t va) {
    return !map->at(va).exists() || goodVas.find(va)!=goodVas.end();
}

// returns number of good and bad addresses executed
static std::pair<size_t, size_t>
execute(const Settings &settings, const std::set<rose_addr_t> &knownVas, const Debugger::Ptr &debugger,
        const MemoryMap::Ptr &map, AddressCounts &executed /*in,out*/) {
    Sawyer::ProgressBar<size_t> progress(mlog[MARCH], "instructions");
    std::ofstream trace;
    if (settings.trace)
        trace.open((numberToString(debugger->isAttached()) + ".trace").c_str());
    size_t totalGood=0, totalBad=0;
    while (!debugger->isTerminated()) {
        rose_addr_t eip = debugger->executionAddress();
        size_t addrSequence = ++executed.insertMaybe(eip, 0);
        bool goodAddr = isGoodAddr(knownVas, map, eip);

        if (goodAddr) {
            ++totalGood;
        } else {
            ++totalBad;
        }

        if (settings.trace)
            trace <<addrToString(eip) <<"\t" <<addrSequence <<"\t" <<(goodAddr?'1':'0') <<"\n";

        debugger->singleStep();
        ++progress;
    }
    return std::make_pair(totalGood, totalBad);
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");
    Sawyer::ProgressBarSettings::minimumUpdateInterval(0.2); // more fluid spinner

    // Parse command-line
    P2::Engine engine;
    Settings settings;
    std::vector<std::string> args = parseCommandLine(argc, argv, engine, settings);
    ASSERT_always_require2(args.size() >= 2, "incorrect usage; see --help");

    // Parse file containing instruction addresses
    std::string addrFileName = args[0];
    std::set<rose_addr_t> knownVas = parseAddressFile(addrFileName);
    mlog[INFO] <<"parsed " <<plural(knownVas.size(), "unique addresses") <<"\n";

    // Load specimen natively and attach debugger
    Debugger::Specimen specimen(args);
    specimen.flags().set(Debugger::CLOSE_FILES);
    Debugger::Ptr debugger = Debugger::instance(specimen);
    debugger->setBreakpoint(AddressInterval::whole());
    ASSERT_always_require(debugger->isAttached());
    ASSERT_always_forbid(debugger->isTerminated());
    pid_t pid = debugger->isAttached();
    mlog[INFO] <<"child PID " <<pid <<"\n";

    // Get memory map.
    MemoryMap::Ptr map;
    if (MAP_ROSE==settings.mapSource) {
        map = engine.loadSpecimens(specimen.program().native());
    } else {
        map = MemoryMap::instance();
        map->insertProcess(pid, MemoryMap::Attach::NO);
    }
    map->dump(mlog[INFO]);

    // The addresses specified in the instruction address file must all be in memory that is mapped.
    BOOST_FOREACH (rose_addr_t va, knownVas) {
        ASSERT_always_require2(map->at(va).require(MemoryMap::EXECUTABLE).exists(),
                               "given address " + addrToString(va) + " is not mapped or lacks execute permission");
    }

    // Some fine tuning for the memory map.  We only need the executable sections, so throw everything else away. Also if the
    // map came from an ELF file parsed by ROSE then it will probably have a ".plt" section that is executable.  It is very
    // likely that the process will execute instructions here that we don't know about, and can't know about since we didn't do
    // any dynamic linking.  Therefore, simply remove these sections from the map.
    map->require(MemoryMap::EXECUTABLE).keep();
    map->substr("(.plt)").prune();

    // Single step the process and see if each mapped execution address is also an instruction address. Keep track of which
    // addresses were executed.
    AddressCounts executed;
    std::pair<size_t, size_t> total = execute(settings, knownVas, debugger, map, executed /*in,out*/);

    // Results
    std::cout <<"child process " <<debugger->howTerminated() <<"\n"
              <<"executed " <<plural(total.first, "expected and unmapped instructions") <<"\n"
              <<"executed " <<plural(total.second, "unexpected instructions") <<"\n";

    if (settings.showExpected) {
        std::cout <<"Expected addresses:\n";
        BOOST_FOREACH (const AddressCounts::Node &addrCount, executed.nodes()) {
            if (isGoodAddr(knownVas, map, addrCount.key()))
                std::cout <<"    " <<addrToString(addrCount.key()) <<"\t" <<addrCount.value() <<"\n";
        }
    }

    if (settings.showUnmapped) {
        std::cout <<"Unmapped addresses:\n";
        BOOST_FOREACH (const AddressCounts::Node &addrCount, executed.nodes()) {
            if (!map->at(addrCount.key()).exists())
                std::cout <<"    " <<addrToString(addrCount.key()) <<"\t" <<addrCount.value() <<"\n";
        }
    }

    if (settings.showUnexpected) {
        std::cout <<"Unexpected addresses:\n";
        BOOST_FOREACH (const AddressCounts::Node &addrCount, executed.nodes()) {
            if (!isGoodAddr(knownVas, map, addrCount.key()))
                std::cout <<"    " <<addrToString(addrCount.key()) <<"\t" <<addrCount.value() <<"\n";
        }
    }
}
