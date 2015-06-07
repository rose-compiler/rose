#include <rose.h>
#include <AsmUnparser.h>
#include <Diagnostics.h>
#include <Partitioner2/Engine.h>
#include <Sawyer/CommandLine.h>

using namespace rose;

int
main(int argc, char *argv[]) {
    std::string purpose = "tests Partitioner2";
    std::string description = "Parses, disassembles, and partitions the specimens given as command-line arguments.";
    SgAsmBlock *gblock = BinaryAnalysis::Partitioner2::Engine().frontend(argc, argv, purpose, description);
    SgAsmInterpretation *interp = SageInterface::getEnclosingNode<SgAsmInterpretation>(gblock);
    ASSERT_not_null(interp);
    BinaryAnalysis::AsmUnparser().unparse(std::cout, interp);
}
