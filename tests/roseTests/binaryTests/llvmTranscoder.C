// Tests that a specimen can be converted to LLVM
#define __STDC_FORMAT_MACROS
#include "rose.h"
#include "LlvmSemantics2.h"
#include <Partitioner2/Engine.h>

using namespace StringUtility;
using namespace SageInterface;
using namespace rose::BinaryAnalysis::InstructionSemantics2;
namespace P2 = rose::BinaryAnalysis::Partitioner2;

int
main(int argc, char *argv[])
{
#if 0
    // Partitioner version 1: This doesn't always do a good job finding basic blocks, in which case the LLVM output
    // may contain branches to undefined labels.
    SgProject *project = frontend(argc, argv);
#else
    // Partitioner version 2: Does a better job of finding the basic blocks.
    P2::Engine engine;
    engine.doingPostAnalysis(false);
    engine.frontend(argc, argv, "testing llvm", "Generates LLVM assembly on standard output.");
    SgProject *project = SageInterface::getProject();
    ASSERT_not_null2(project, "specimen must be an ELF or PE container for this test");
#endif

    std::vector<SgAsmInterpretation*> interps = querySubTree<SgAsmInterpretation>(project);
    std::cerr <<"found " <<plural(interps.size(), "binary interpretations") <<"\n";

    // A Transcoder is responsible for converting some part of an AST into LLVM assembly.
    LlvmSemantics::TranscoderPtr transcoder = LlvmSemantics::Transcoder::instanceX86();
    transcoder->quietErrors(true);                      // catch exceptions and emit an LLVM comment instead

    // Emit some LLVM. There are lots of methods for doing this, but this is the easiest.
    if (!interps.empty())
        transcoder->transcodeInterpretation(interps.back(), std::cout);
    
    return 0;
}
