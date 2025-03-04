#include <featureTests.h>
#ifdef ROSE_ENABLE_DEBUGGER_LINUX

static const char* purpose = "compares actual execution with known instructions";
static const char* description =
    "Reads instruction addresses from a file, the so-called \"expected\" addresses and and then executes the specimen "
    "and compares actual executed addresses with the expected addresses.  An actual executed address falls into one of "
    "three categories:  (1) the address is an expected address, or else (2) the address is not mapped, or else (3) the "
    "address not expected.\n\n"

    "One method of obtaining a list of expected addresses is to use the @man{recursiveDisassemble}{--help} tool's "
    "@s{list-instruction-addressses}{noerror} switch. Although this produces output that contains instruction sizes "
    "as well as addresses, @prop{programName} ignores the sizes.  This can be used to test whether a process executes any "
    "instructions that were not also disassembled, thereby testing some aspect of the disassembly quality.";

#include <rose.h>

#include <rose_getline.h>
#include <rose_strtoull.h>
#include <Rose/BinaryAnalysis/AddressInterval.h>
#include <Rose/BinaryAnalysis/Debugger/Linux.h>
#include <Rose/BinaryAnalysis/Partitioner2/EngineBinary.h>
#include <Rose/CommandLine/Parser.h>
#include <Sawyer/CommandLine.h>
#include <Sawyer/Message.h>
#include <Sawyer/ProgressBar.h>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;
using namespace StringUtility;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

static Sawyer::Message::Facility mlog;

typedef Sawyer::Container::Map<Address, size_t> AddressCounts;

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
parseCommandLine(int argc, char *argv[], const P2::EngineBinary::Ptr &engine, Settings &settings)
{
    using namespace Sawyer::CommandLine;

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

    // The parser is the same as that created by Engine::commandLineParser except we don't need any disassemler or partitioning
    // switches since this tool doesn't disassemble or partition.
    return Rose::CommandLine::createEmptyParser(purpose, description)
        .doc("Synopsis",
             "@prop{programName} [@v{switches}] @v{address_file} @v{specimen_name} @v{specimen_arguments}...")
        .with(tool)
        .with(Rose::CommandLine::genericSwitches())
        .doc(engine->specimenNameDocumentation())
        .with(engine->engineSwitches(engine->settings().engine))
        .with(engine->loaderSwitches(engine->settings().loader))
        .parse(argc, argv)
        .apply()
        .unreachedArgs();
}

// File has one address per line
static std::set<Address>
parseAddressFile(const std::string &fileName) {
    std::set<Address> retval;
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
isGoodAddr(const std::set<Address> &goodVas, const MemoryMap::Ptr &map, Address va) {
    return !map->at(va).exists() || goodVas.find(va)!=goodVas.end();
}

// returns number of good and bad addresses executed
static std::pair<size_t, size_t>
execute(const Settings &settings, const std::set<Address> &knownVas, const Debugger::Linux::Ptr &debugger,
        const MemoryMap::Ptr &map, AddressCounts &executed /*in,out*/) {
    Sawyer::ProgressBar<size_t> progress(mlog[MARCH], "instructions");
    std::ofstream trace;
    if (settings.trace)
        trace.open((numberToString(*debugger->processId()) + ".trace").c_str());
    size_t totalGood=0, totalBad=0;
    while (!debugger->isTerminated()) {
        Address eip = debugger->executionAddress(Debugger::ThreadId::unspecified());
        size_t addrSequence = ++executed.insertMaybe(eip, 0);
        bool goodAddr = isGoodAddr(knownVas, map, eip);

        if (goodAddr) {
            ++totalGood;
        } else {
            ++totalBad;
        }

        if (settings.trace)
            trace <<addrToString(eip) <<"\t" <<addrSequence <<"\t" <<(goodAddr?'1':'0') <<"\n";

        debugger->singleStep(Debugger::ThreadId::unspecified());
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
    Settings settings;
    auto engine = P2::EngineBinary::instance();
    std::vector<std::string> args = parseCommandLine(argc, argv, engine, settings);
    ASSERT_always_require2(args.size() >= 2, "incorrect usage; see --help");

    // Parse file containing instruction addresses
    std::string addrFileName = args[0];
    std::set<Address> knownVas = parseAddressFile(addrFileName);
    mlog[INFO] <<"parsed " <<plural(knownVas.size(), "unique addresses") <<"\n";

    // Load specimen natively and attach debugger
    Debugger::Linux::Specimen specimen(args);
    specimen.flags().set(Debugger::Linux::Flag::CLOSE_FILES);
    auto debugger = Debugger::Linux::instance(specimen);
    debugger->setBreakPoint(AddressInterval::whole());
    ASSERT_always_require(debugger->isAttached());
    ASSERT_always_forbid(debugger->isTerminated());
    pid_t pid = *debugger->processId();
    mlog[INFO] <<"child PID " <<pid <<"\n";

    // Get memory map.
    MemoryMap::Ptr map;
    if (MAP_ROSE==settings.mapSource) {
        map = engine->loadSpecimens(specimen.program().string());
    } else {
        map = MemoryMap::instance();
        map->insertProcess(pid, MemoryMap::Attach::NO);
    }
    map->dump(mlog[INFO]);

    // The addresses specified in the instruction address file must all be in memory that is mapped.
    for (Address va: knownVas) {
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
        for (const AddressCounts::Node &addrCount: executed.nodes()) {
            if (isGoodAddr(knownVas, map, addrCount.key()))
                std::cout <<"    " <<addrToString(addrCount.key()) <<"\t" <<addrCount.value() <<"\n";
        }
    }

    if (settings.showUnmapped) {
        std::cout <<"Unmapped addresses:\n";
        for (const AddressCounts::Node &addrCount: executed.nodes()) {
            if (!map->at(addrCount.key()).exists())
                std::cout <<"    " <<addrToString(addrCount.key()) <<"\t" <<addrCount.value() <<"\n";
        }
    }

    if (settings.showUnexpected) {
        std::cout <<"Unexpected addresses:\n";
        for (const AddressCounts::Node &addrCount: executed.nodes()) {
            if (!isGoodAddr(knownVas, map, addrCount.key()))
                std::cout <<"    " <<addrToString(addrCount.key()) <<"\t" <<addrCount.value() <<"\n";
        }
    }
}

#else

#include <rose.h>
#include <Rose/Diagnostics.h>

#include <iostream>
#include <cstring>

int main(int, char *argv[]) {
    ROSE_INITIALIZE;
    Sawyer::Message::Facility mlog;
    Rose::Diagnostics::initAndRegister(&mlog, "tool");
    mlog[Rose::Diagnostics::FATAL] <<argv[0] <<": this tool is not available in this ROSE configuration\n";

    for (char **arg = argv+1; *arg; ++arg) {
        if (!strcmp(*arg, "--no-error-if-disabled"))
            return 0;
    }
    return 1;
}

#endif
