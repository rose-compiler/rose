#include <rose.h>

#include <BinaryToSource.h>
#include <Diagnostics.h>
#include <Partitioner2/Engine.h>
#include <Sawyer/CommandLine.h>

using namespace Rose::Diagnostics;
using namespace Rose::BinaryAnalysis;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

// Global settings adjusted from the command-line
struct Settings {
    BinaryToSource::Settings generator;
};

static std::vector<std::string>
parseCommandLine(int argc, char *argv[], P2::Engine &engine, Settings &settings) {
    using namespace Sawyer::CommandLine;

    std::string purpose = "generates low-level source code from a binary specimen";
    std::string description =
        "This command generates a C source file from a binary specimen. The binary specimen is parsed, disassembled, "
        "and partitioned into functions, basic blocks, and instructions. These are then traversed to build C source "
        "code which is emitted to standard output.";

    Parser parser = engine.commandLineParser(purpose, description);

    SwitchGroup generator = BinaryToSource::commandLineSwitches(settings.generator);

    return parser.with(generator).parse(argc, argv).apply().unreachedArgs();
}

int
main(int argc, char *argv[]) {
    // Parse the command-line switches
    P2::Engine engine;
    Settings settings;
    std::vector<std::string> args = parseCommandLine(argc, argv, engine, settings);
    if (args.empty()) {
        mlog[FATAL] <<"no binary specimen specified; see --help\n";
        exit(1);
    }

    // Parse the binary specimen. We're not actually adding it to the AST.
    P2::Partitioner binary = engine.partition(args);

    // Process the binary to add its instructions to the source template
    BinaryToSource(settings.generator).generateSource(binary, std::cout);
}
