// Example of using rose::BinaryAnalysis::PointerDetection to find pointer variables in a binary specimen.
#include <rose.h>

#include <BinaryPointerDetection.h>
#include <boost/foreach.hpp>
#include <Partitioner2/Engine.h>
#include <Partitioner2/Partitioner.h>
#include <rose_strtoull.h>
#include <Sawyer/CommandLine.h>
#include <string>
#include <vector>

using namespace rose::Diagnostics;
using namespace rose::BinaryAnalysis;
namespace P2 = rose::BinaryAnalysis::Partitioner2;

struct Settings {
    std::vector<std::string> functionNames;
};

static std::vector<std::string>
parseCommandLine(int argc, char *argv[], Partitioner2::Engine &engine, Settings &settings) {
    using namespace Sawyer::CommandLine;

    std::string purpose = "finds memory variables that are used as pointers";
    std::string description =
        "This tool disassembles the binary specimen and examines each function to find memory locations that are "
        "used as pointers, and whether they're pointers to code or data. It's primary purpose is to test the "
        "rose::BinaryAnalysis::PointerDetection analysis.";

    SwitchGroup tool("Tool-specific switches");
    tool.insert(Switch("function")
                .argument("name_or_address", anyParser(settings.functionNames))
                .whichValue(SAVE_ALL)
                .doc("Names or addresses of functions that should be analyzed. This switch may appear more than once if "
                     "more than one function should be analyzed. Results will be presented in this same order. "
                     "If no names or addresses are specified then all functions are analyzed."));

    return engine.commandLineParser(purpose, description).with(tool).parse(argc, argv).apply().unreachedArgs();
}

static bool
shouldAnalyze(const Settings &settings, const P2::Function::Ptr &function) {
    if (settings.functionNames.empty())
        return true;
    BOOST_FOREACH (const std::string &name_or_address, settings.functionNames) {
        if (function->name() == name_or_address)
            return true;
        char *rest = NULL;
        errno = 0;
        rose_addr_t va = rose_strtoull(name_or_address.c_str(), &rest, 0);
        if (0 == errno && !*rest && va == function->address())
            return true;
    }
    return false;
}

int
main(int argc, char *argv[]) {
    Settings settings;
    P2::Engine engine;
    std::vector<std::string> specimen = parseCommandLine(argc, argv, engine, settings);
    if (specimen.empty()) {
        mlog[FATAL] <<"no binary specimen specified; see --help\n";
        exit(1);
    }
    P2::Partitioner partitioner = engine.partition(specimen);

    BOOST_FOREACH (const P2::Function::Ptr &function, partitioner.functions()) {
        if (shouldAnalyze(settings, function)) {
            std::cout <<"\nPointer detection analysis for " <<function->printableName() <<"\n";

            // Display the function's instructions for easy reference
            std::cout <<"  Instructions:\n";
            BOOST_FOREACH (rose_addr_t bbVa, function->basicBlockAddresses()) {
                if (P2::BasicBlock::Ptr bb = partitioner.basicBlockExists(bbVa)) {
                    BOOST_FOREACH (SgAsmInstruction *insn, bb->instructions()) {
                        std::cout <<"    " <<unparseInstructionWithAddress(insn) <<"\n";
                    }
                }
            }

            // Run the analysis
            PointerDetection::Analysis pda(engine.disassembler());
            pda.analyzeFunction(partitioner, function);

            // Show the results
            if (!pda.hasResults()) {
                std::cout <<"  Analysis failed (perhaps not a well-formed function?)\n";
            } else if (!pda.didConverge()) {
                std::cout <<"  Analysis did not converge\n";
            } else {
                static const bool sort = true;
                std::cout <<"  Code pointers:\n";
                BOOST_FOREACH (const SymbolicExpr::Ptr &addr, pda.codePointers(sort))
                    std::cout <<"    " <<*addr <<"\n";
                std::cout <<"  Data pointers:\n";
                BOOST_FOREACH (const SymbolicExpr::Ptr &addr, pda.dataPointers(sort))
                    std::cout <<"    " <<*addr <<"\n";
            }
        }
    }
}
