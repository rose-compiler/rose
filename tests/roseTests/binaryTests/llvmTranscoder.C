// Tests that a specimen can be converted to LLVM
#define __STDC_FORMAT_MACROS
#include "rose.h"
#include "LlvmSemantics2.h"

using namespace StringUtility;
using namespace SageInterface;
using namespace rose::BinaryAnalysis::InstructionSemantics2;

int
main(int argc, char *argv[])
{
    SgProject *project = frontend(argc, argv);
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
