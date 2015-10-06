#include <rose.h>

#include <BinaryToSource.h>
#include <Diagnostics.h>
#include <Partitioner2/Engine.h>
#include <Sawyer/CommandLine.h>

using namespace rose::Diagnostics;
using namespace rose::BinaryAnalysis;
namespace P2 = rose::BinaryAnalysis::Partitioner2;

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

    SwitchGroup tool("Tool-specific switches");
    tool.insert(Switch("trace-generation")
                .intrinsicValue(true, settings.generator.traceRiscOps)
                .doc("Cause the source generation phase to emit information about the basic RISC-like steps performed for "
                     "each instruction. This can preserve a developer's sanity because the C expressions often become large, "
                     "deeply nested, and not always intuitive about from whence each part came. The @s{no-trace-generation} "
                     "switch turns this off.  The default is to " + std::string(settings.generator.traceRiscOps?"":"not ") +
                     "show this information."));
    tool.insert(Switch("no-trace-generation")
                .key("trace-generation")
                .intrinsicValue(false, settings.generator.traceRiscOps)
                .hidden(true));

    tool.insert(Switch("trace-instructions")
                .intrinsicValue(true, settings.generator.traceInsnExecution)
                .doc("Cause the generated source to contain extra \"printf\" calls to emit each instruction as it is "
                     "processed. The @s{no-trace-instructions} switch turns this off. The default is to " +
                     std::string(settings.generator.traceInsnExecution?"":"not ") + "add these printf calls."));
    tool.insert(Switch("no-trace-instructions")
                .key("trace-instructions")
                .intrinsicValue(false, settings.generator.traceInsnExecution)
                .hidden(true));

    tool.insert(Switch("ip")
                .longName("instruction-pointer")
                .argument("address", nonNegativeIntegerParser(settings.generator.initialInstructionPointer))
                .doc("Initial value for the instruction pointer.  The default is to not initialize the instruction pointer."));

    tool.insert(Switch("sp")
                .longName("stack-pointer")
                .argument("address", nonNegativeIntegerParser(settings.generator.initialStackPointer))
                .doc("Initial value for the stack pointer. The default is to not initialize the stack pointer."));

    tool.insert(Switch("allocate-memory")
                .argument("size", nonNegativeIntegerParser(settings.generator.allocateMemoryArray))
                .doc("Causes the global \"mem\" array to be allocated instead of being declared \"extern\". The switch "
                     "argument is the amount of memory to allocate. If the argument is zero, then the memory array is "
                     "allocated to be just large enough to hold the value at the maximum initialized address. The default "
                     "is to not allocate the array."));

    return parser.with(tool).parse(argc, argv).apply().unreachedArgs();
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
