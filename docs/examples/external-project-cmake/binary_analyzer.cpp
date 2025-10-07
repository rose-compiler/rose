// ROSE Binary Analysis Example - List Synthesized Variables
// 
// This example demonstrates using ROSE's binary analysis capabilities
// to analyze executable files and extract information about variables.
// 
// Based on tools/BinaryAnalysis/bat-lsv.C from the ROSE repository.
// 
// Note: This program is only built if ROSE was configured with binary analysis support.

#ifndef HAVE_ROSE_BINARY_ANALYSIS
#error "This example requires ROSE with binary analysis support"
#endif

#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/BinaryAnalysis/Variables.h>
#include <Rose/CommandLine.h>
#include <Rose/Diagnostics.h>
#include <Rose/Initialize.h>

#include <boost/algorithm/string/predicate.hpp>
#include <iostream>
#include <string>
#include <vector>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

static const char *purpose = "list synthesized variables";
static const char *description =
    "Analyzes a binary specimen to synthesize information about source-level "
    "variables, and then displays that information. This demonstrates ROSE's "
    "ability to recover high-level variable information from executables.";

static Sawyer::Message::Facility mlog;

static Sawyer::CommandLine::Parser
createSwitchParser() {
    using namespace Sawyer::CommandLine;

    SwitchGroup generic = Rose::CommandLine::genericSwitches();

    Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
    parser.errorStream(mlog[FATAL]);
    parser.with(generic);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] @v{specimen}");
    return parser;
}

static std::vector<std::string>
parseCommandLine(int argc, char *argv[], Sawyer::CommandLine::Parser &parser) {
    std::vector<std::string> specimen = parser.parse(argc, argv).apply().unreachedArgs();
    if (specimen.empty()) {
        mlog[FATAL] << "no specimen specified; see --help\n";
        exit(1);
    }
    return specimen;
}

int main(int argc, char *argv[]) {
    // Initialize ROSE
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");
    mlog.comment("listing info about variables");

    std::cout << "ROSE Binary Variable Analyzer" << std::endl;
    std::cout << "ROSE Version: " << ROSE_PACKAGE_VERSION << std::endl;
    std::cout << "==============================" << std::endl;

    // Parse command line
    Sawyer::CommandLine::Parser switchParser = createSwitchParser();
    auto engine = P2::Engine::forge(argc, argv, switchParser /*in,out*/);
    const std::vector<std::string> specimen = parseCommandLine(argc, argv, switchParser);

    std::cout << "Analyzing specimen: " << specimen[0] << std::endl;

    try {
        // Create partitioner from the specimen
        P2::Partitioner::ConstPtr partitioner = engine->partition(specimen);
        if (!partitioner) {
            mlog[FATAL] << "failed to create partitioner\n";
            return 1;
        }

        std::cout << "Successfully partitioned binary" << std::endl;

        // Analyze to find global variables
        auto analyzer = Variables::VariableFinder::instance();
        std::cout << "Searching for global variables..." << std::endl;
        Variables::GlobalVariables gvars = analyzer->findGlobalVariables(partitioner);

        // Display results
        std::cout << "\nFound " << gvars.size() << " global variable(s):\n" << std::endl;
        for (const Variables::GlobalVariable &gvar: gvars.values()) {
            std::cout << gvar << "\n";
        }

        if (gvars.isEmpty()) {
            std::cout << "(No global variables found - this is normal for stripped binaries)\n";
        }

        std::cout << "\nAnalysis complete!" << std::endl;

    } catch (const std::exception& e) {
        mlog[FATAL] << "error during analysis: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
