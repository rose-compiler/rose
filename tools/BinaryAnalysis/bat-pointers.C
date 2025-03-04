static const char *purpose = "find pointer variables";
static const char *description =
    "Detects variables that are used as pointers.";

#include <batSupport.h>

#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/BinaryAnalysis/PointerDetection.h>
#include <Rose/CommandLine.h>
#include <Rose/Initialize.h>

#include <Sawyer/ThreadWorkers.h>

#include <boost/algorithm/string/predicate.hpp>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

Sawyer::Message::Facility mlog;

struct Settings {
    std::set<std::string> functionNames;
    std::set<Address> addresses;
    SerialIo::Format stateFormat = SerialIo::BINARY;
};

static Sawyer::CommandLine::Parser
createSwitchParser(Settings &settings) {
    using namespace Sawyer::CommandLine;

    SwitchGroup gen = Rose::CommandLine::genericSwitches();
    gen.insert(Bat::stateFileFormatSwitch(settings.stateFormat));

    SwitchGroup sel("Selection switches");
    sel.name("sel");

    sel.insert(Switch("function", 'f')
               .argument("name_or_address", listParser(anyParser(settings.functionNames), ","))
               .explosiveLists(true)
               .whichValue(SAVE_ALL)
               .doc("Restricts output to the specified functions. The @v{name_or_address} can be the name of a function as "
                    "a string or the entry address for the function as a decimal, octal, hexadecimal or binary number. "
                    "If a value is ambiguous, it's first treated as a name and if no function has that name it's then "
                    "treated as an address. This switch may occur multiple times and multiple comma-separated values may "
                    "be specified per occurrence."));

    sel.insert(Switch("containing", 'a')
               .argument("addresses", listParser(nonNegativeIntegerParser(settings.addresses), ","))
               .explosiveLists(true)
               .whichValue(SAVE_ALL)
               .doc("Restricts output to functions that contain one of the specified addresses. This switch may occur "
                    "multiple times and multiple comma-separate addresses may be specified per occurrence."));

    Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
    parser.errorStream(mlog[FATAL]);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] [@v{specimen}]");
    parser.with(sel).with(gen);
    return parser;
}

static std::vector<std::string>
parseCommandLine(int argc, char *argv[], Sawyer::CommandLine::Parser &parser) {
    std::vector<std::string> specimen = parser.parse(argc, argv).apply().unreachedArgs();
    if (specimen.empty())
        specimen.push_back("-");
    return specimen;
}

static void
processFunction(const P2::Partitioner::ConstPtr &partitioner, const P2::Function::Ptr &function) {
    ASSERT_not_null(function);
    BinaryAnalysis::PointerDetection::Analysis analyzer(partitioner->instructionProvider().disassembler());
    analyzer.analyzeFunction(partitioner, function);
    if (analyzer.hasResults()) {
        std::cout <<"data pointers in " <<function->printableName() <<"\n";
        for (const PointerDetection::PointerDescriptor &ptr: analyzer.dataPointers()) {
            ASSERT_not_null(ptr.pointerVa);
            std::cout <<"  " <<(ptr.nBits/8) <<"-byte pointer variable located at " <<*ptr.pointerVa <<"\n";
            for (const PointerDetection::PointerDescriptor::Access &access: ptr.pointerAccesses) {
                std::cout <<"    instruction " <<StringUtility::addrToString(access.insnVa);
                switch (access.direction) {
                    case PointerDetection::PointerDescriptor::READ:
                        std::cout <<" reads  pointer value ";
                        break;
                    case PointerDetection::PointerDescriptor::WRITE:
                        std::cout <<" writes pointer value ";
                        break;
                }
                std::cout <<*access.value <<"\n";
            }
            for (const PointerDetection::PointerDescriptor::Access &access: ptr.dereferences) {
                std::cout <<"    instruction " <<StringUtility::addrToString(access.insnVa);
                switch (access.direction) {
                    case PointerDetection::PointerDescriptor::READ:
                        std::cout <<" dereferences to read  pointee value ";
                        break;
                    case PointerDetection::PointerDescriptor::WRITE:
                        std::cout <<" dereferences to write pointee value ";
                        break;
                }
                std::cout <<*access.value <<"\n";
            }
        }
    }
}

int
main(int argc, char *argv[]) {
    // Initialize
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");
    mlog.comment("find pointer variables");
    Bat::checkRoseVersionNumber(MINIMUM_ROSE_LIBRARY_VERSION, mlog[FATAL]);
    Bat::registerSelfTests();

    // Parse command-line
    Settings settings;
    Sawyer::CommandLine::Parser switchParser = createSwitchParser(settings);
    auto engine = P2::Engine::forge(argc, argv, switchParser /*in,out*/);
    const std::vector<std::string> specimen = parseCommandLine(argc, argv, switchParser);

    // Ingest specimen
    P2::Partitioner::ConstPtr partitioner;
    if (specimen.size() == 1 && (specimen[0] == "-" || boost::ends_with(specimen[0], ".rba"))) {
        try {
            partitioner = P2::Partitioner::instanceFromRbaFile(specimen[0], settings.stateFormat);
        } catch (const std::exception &e) {
            mlog[FATAL] <<"cannot load partitioner from " <<specimen[0] <<": " <<e.what() <<"\n";
            exit(1);
        }
    } else {
        partitioner = engine->partition(specimen);
    }
    ASSERT_not_null(partitioner);

    // Select functions to analyze
    std::vector<P2::Function::Ptr> selectedFunctions;
    if (!settings.functionNames.empty() || !settings.addresses.empty()) {
        selectedFunctions = Bat::selectFunctionsByNameOrAddress(partitioner->functions(), settings.functionNames, mlog[WARN]);
        for (const P2::Function::Ptr &function: Bat::selectFunctionsContainingInstruction(partitioner, settings.addresses))
            P2::insertUnique(selectedFunctions, function, P2::sortFunctionsByAddress);
        if (selectedFunctions.empty())
            mlog[WARN] <<"no matching functions found\n";
    } else {
        selectedFunctions = partitioner->functions();
        if (selectedFunctions.empty())
            mlog[WARN] <<"specimen contains no functions\n";
    }

    // Analyze all selected functions. We can do this in parallel, assigning functions to workers in any order.
    Sawyer::Container::Graph<P2::Function::Ptr> work;
    for (const P2::Function::Ptr &function: selectedFunctions)
        work.insertVertex(function);
    Sawyer::ProgressBar<size_t> progress(selectedFunctions.size(), mlog[MARCH], "analyzing");
    progress.suffix(" functions");
    Sawyer::workInParallel(work, CommandLine::genericSwitchArgs.threads,
                           [&partitioner, &progress](size_t, const P2::Function::Ptr &function) {
                               ++progress;
                               processFunction(partitioner, function);
                           });
}
