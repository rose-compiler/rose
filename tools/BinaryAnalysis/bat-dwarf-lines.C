static const char *purpose = "print information about DWARF line numbers";
static const char *description =
    "Given an ELF binary, show the mapping from address to source line number and the "
    "mapping from line numbers to addresses.";

#include <rose.h>
#include <batSupport.h>

#include <Rose/BinaryAnalysis/Partitioner2/EngineBinary.h>
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
parseCommandLine(int argc, char *argv[], const P2::EngineBinary::Ptr &engine, Settings&) {
    using namespace Sawyer::CommandLine;
    ASSERT_not_null(engine);

    Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] @v{specimen}");
    parser.errorStream(mlog[FATAL]);
    parser.with(Rose::CommandLine::genericSwitches());
    engine->addToParser(parser);
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
    auto engine = P2::EngineBinary::instance();
    std::vector<std::string> specimen = parseCommandLine(argc, argv, engine, settings);
    engine->parseContainers(specimen);

    SgProject *project = SageInterface::getProject();
    SourceLocations lineMapper;
    lineMapper.insertFromDebug(project);
    lineMapper.printSrcToAddr(std::cout);
    lineMapper.printAddrToSrc(std::cout);
}
