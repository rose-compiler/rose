#include <rose.h>
#include <Partitioner2/Engine.h>
#include <StaticSemantics2.h>


using namespace rose;
using namespace rose::BinaryAnalysis;
namespace P2 = rose::BinaryAnalysis::Partitioner2;

int
main(int argc, char *argv[]) {
    std::string purpose = "tests StaticSemantics";
    std::string description =
        "ROSE has two forms of instruction semantics. The most general is that which is part of the Semantics2 API "
        "and which provides semantic lookup tables, arbitrary domains, and virtual machine states. But users coming "
        "from other platforms might be more accustomed to having semantics represented statically as part of an "
        "abstract syntax tree. ROSE can build these static semantics ASTs from its StaticSemantics domain in the "
        "Semantics2 API, and that is what this program tests.";
    P2::Engine engine;
    SgAsmBlock *gblock = engine.frontend(argc, argv, purpose, description);
    InstructionSemantics2::StaticSemantics::attachInstructionSemantics(gblock, engine.disassembler());

#if 1 // DEBUGGING [Robb P. Matzke 2015-06-08]
    generateDOT(*SageInterface::getProject());
#endif
}
