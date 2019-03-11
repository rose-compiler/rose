static const char *purpose = "disassemble a binary specimen";
static const char *description =
    "Disassemble the binary specimen described on the command-line in order to generate "
    "an assembly listing oriented toward human consumption. The output listing is not "
    "intended to be reassembled.";

#include <rose/rose.h>                                  // must be first!
#include <rose/rosePublicConfig.h>

#if !defined(ROSE_BUILD_BINARY_ANALYSIS_SUPPORT)
int main(int argc, char *argv[]) {
    std::cout <<"ROSE was not configured with binary analysis support\n";
    return 0; // a normal tool would probably fail, but we dont' want to fail during testing
}
#else

#include <rose/BinaryUnparserBase.h>
#include <rose/CommandLine.h>
#include <rose/Partitioner2/Engine.h>

using namespace Rose;
using namespace Sawyer::Message::Common;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

namespace {

Sawyer::Message::Facility mlog;
BinaryAnalysis::Unparser::Settings unparserSettings;

// Parse command-line and return the positional arguments which describe the specimen.
std::vector<std::string>
parseCommandLine(int argc, char *argv[], P2::Engine &engine) {
    using namespace Sawyer::CommandLine;

    Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] @v{specimen}...");
    parser.doc("Specimens", engine.specimenNameDocumentation());
    parser.errorStream(mlog[FATAL]);
    parser.with(engine.engineSwitches());
    parser.with(engine.loaderSwitches());
    parser.with(engine.disassemblerSwitches());
    parser.with(engine.partitionerSwitches());
    parser.with(BinaryAnalysis::Unparser::commandLineSwitches(unparserSettings));

    std::vector<std::string> specimen = parser.parse(argc, argv).apply().unreachedArgs();
    if (specimen.empty()) {
        mlog[FATAL] <<"no binary specimen specified; see --help\n";
        exit(1);
    }
    return specimen;
}

} // namespace
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");

    P2::Engine engine;
    std::vector<std::string> specimen = parseCommandLine(argc, argv, engine);
    P2::Partitioner partitioner = engine.partition(specimen);

    BinaryAnalysis::Unparser::Base::Ptr unparser = partitioner.unparser();
    unparser->settings() = unparserSettings;
    unparser->unparse(std::cout, partitioner);
}

#endif
