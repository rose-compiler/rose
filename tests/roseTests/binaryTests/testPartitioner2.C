#include <rose.h>
#include <AsmUnparser.h>
#include <Diagnostics.h>
#include <Partitioner2/Engine.h>
#include <sawyer/CommandLine.h>

using namespace rose;
namespace P2 = rose::BinaryAnalysis::Partitioner2;

std::vector<std::string>
parseCommandLine(int argc, char *argv[]) {
    return Sawyer::CommandLine::Parser()
        .purpose("tests Partitioner2")
        .version(std::string(ROSE_SCM_VERSION_ID).substr(0, 8), ROSE_CONFIGURE_DATE)
        .chapter(1, "ROSE Command-line Tools")
        .doc("synopsis",
             "@prop{programName} [@v{switches}] @v{specimen_names}")
        .doc("description",
             "Parses, disassembles and partitions the specimens given as positional arguments on the command-line.")
        .doc("Specimens", P2::Engine::specimenNameDocumentation())
        .with(CommandlineProcessing::genericSwitches())
        .parse(argc, argv)
        .apply()
        .unreachedArgs();
}

int
main(int argc, char *argv[]) {
    Diagnostics::initialize();

    std::vector<std::string> specimenNames = parseCommandLine(argc, argv);
    P2::Engine engine;
    P2::Partitioner partitioner = engine.partition(specimenNames);
    SgAsmBlock *gblock = engine.buildAst(partitioner);
    SgAsmInterpretation *interp = engine.interpretation();
    interp->set_global_block(gblock);

    BinaryAnalysis::AsmUnparser().unparse(std::cout, interp);
}
