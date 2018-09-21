#include <rose.h>
#include <AsmUnparser.h>
#include <Diagnostics.h>
#include <Partitioner2/Engine.h>
#include <Sawyer/CommandLine.h>

using namespace Rose;

int
main(int argc, char *argv[]) {
    std::string purpose = "tests Partitioner2";
    std::string description = "Parses, disassembles, and partitions the specimens given as command-line arguments.";
    BinaryAnalysis::Partitioner2::Engine engine;
    engine.namingSystemCalls(false);                    // for consistent results w.r.t. the answer file since the system...
    engine.systemCallHeader("/dev/null");               // ...call mapping comes from run-time files.
    
    SgAsmBlock *gblock = engine.frontend(argc, argv, purpose, description);
    SgAsmInterpretation *interp = SageInterface::getEnclosingNode<SgAsmInterpretation>(gblock);
    ASSERT_not_null(interp);
    BinaryAnalysis::AsmUnparser().unparse(std::cout, interp);
}
