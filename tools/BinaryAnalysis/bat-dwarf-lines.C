static const char *purpose = "print information about DWARF line numbers";
static const char *description =
    "Given an ELF binary, show the mapping from address to source line number and the "
    "mapping from line numbers to addresses.";

#include <rose.h>
#include <batSupport.h>

#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/CommandLine.h>
#include <Rose/Diagnostics.h>

#include <iostream>
#include <string>
#include <vector>

using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

struct Settings {};

static Sawyer::Message::Facility mlog;

static std::vector<std::string>
parseCommandLine(int argc, char *argv[], P2::Engine &engine, Settings&) {
    using namespace Sawyer::CommandLine;

    Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] @v{specimen}");
    parser.doc("Specimens", engine.specimenNameDocumentation());
    parser.errorStream(mlog[FATAL]);
    parser.with(engine.engineSwitches());
    std::vector<std::string> args = parser.parse(argc, argv).apply().unreachedArgs();

    if (args.empty()) {
        mlog[FATAL] <<"incorrect usage; see --help\n";
        exit(1);
    }

    return args;
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Rose::Diagnostics::initAndRegister(&mlog, "tool");
    mlog.comment("listing DWARF line numbers");
    Bat::checkRoseVersionNumber(MINIMUM_ROSE_LIBRARY_VERSION, mlog[FATAL]);
    Bat::registerSelfTests();

    Settings settings;
    P2::EnginePtr engine = P2::Engine::forge();
    std::vector<std::string> specimen = parseCommandLine(argc, argv, *engine, settings);
    engine->parseContainers(specimen);

    SgProject *project = SageInterface::getProject();
    SourceLocations lineMapper;
    lineMapper.insertFromDebug(project);
    lineMapper.printSrcToAddr(std::cout);
    lineMapper.printAddrToSrc(std::cout);
}
