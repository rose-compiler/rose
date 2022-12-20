#include <rose.h>
#include <AsmUnparser.h>
#include <Rose/Diagnostics.h>
#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>
#include <Sawyer/CommandLine.h>

using namespace Rose;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

int
main(int argc, char *argv[]) {
    std::string purpose = "tests Partitioner2";
    std::string description = "Parses, disassembles, and partitions the specimens given as command-line arguments.";
    P2::Engine *engine = P2::Engine::instance();
    engine->settings().partitioner.namingSyscalls = false;      // for consistent results w.r.t. the answer file since the system...
    engine->settings().partitioner.syscallHeader = "/dev/null"; // ...call mapping comes from run-time files.
    
    SgAsmBlock *gblock = engine->frontend(argc, argv, purpose, description);
    SgAsmInterpretation *interp = SageInterface::getEnclosingNode<SgAsmInterpretation>(gblock);
    ASSERT_not_null(interp);
    BinaryAnalysis::AsmUnparser().unparse(std::cout, interp);

    delete engine;
}
