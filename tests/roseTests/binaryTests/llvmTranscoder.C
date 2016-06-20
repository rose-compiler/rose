// Tests that a specimen can be converted to LLVM
#define __STDC_FORMAT_MACROS
#include "rose.h"
#include "LlvmSemantics2.h"
#include <Partitioner2/Engine.h>
#include <Sawyer/CommandLine.h>

using namespace Sawyer::Message::Common;
using namespace StringUtility;
using namespace SageInterface;
using namespace rose::BinaryAnalysis::InstructionSemantics2;
namespace P2 = rose::BinaryAnalysis::Partitioner2;

static Sawyer::Message::Facility mlog;

struct Settings {
    std::string llvmVersionString;
    int llvmVersion;                                    // set after command-line parsing by evaluating llvmVersionString

    Settings()
        : llvmVersion(0) {}
};

SgProject*
buildAst(int argc, char *argv[], Settings &settings) {
    using namespace Sawyer::CommandLine;
    P2::Engine engine;

    // Parse the commane-line
    Parser p = engine.commandLineParser("transcode to LLVM", "Convert an ELF/PE specimen to LLVM assembly for testing.");
    SwitchGroup tool("Tool-specific switches");
    tool.insert(Switch("llvm")
                .argument("version", anyParser(settings.llvmVersionString))
                .doc("Version number for LLVM.  The version number is a doublet or triplet of integers such as \"3.5\" or "
                     "\"3.5.0\" and indicates which dialect of assembly should be emitted. The LLVM assembly syntax, being "
                     "mostly an LLVM internal language, changes in incompatible ways between LLVM versions. This transcoder "
                     "supports only certain versions (e.g., 3.5.0 and 3.7.0 as of December 2015)."));

    std::vector<std::string> specimen = p.with(tool).parse(argc, argv).apply().unreachedArgs();
    if (specimen.empty()) {
        ::mlog[FATAL] <<"no binary specimen; see --help for usage\n";
        exit(1);
    }

    // Parse the LLVM version number specified on the command-line
    if (!settings.llvmVersionString.empty()) {
        const char *s = settings.llvmVersionString.c_str();
        char *rest = NULL;
        errno = 0;
        int a = strtol(s, &rest, 10), b = 0, c = 0;
        if ('.'==*rest && 0==errno) {
            b = strtol(rest+1, &rest, 10);
            if ('.'==*rest && 0==errno)
                c = strtol(rest+1, &rest, 10);
        }
        settings.llvmVersion = 1000000 * a + 1000 * b + c;
    }

    // Parse, load, disassemble, and partition the specimen.
    (void) engine.buildAst(specimen);
    SgProject *project = SageInterface::getProject();
    if (!project) {
        ::mlog[FATAL] <<"This tool only supports ELF/PE specimens.\n";
        exit(1);
    }
    
    return project;
}

int
main(int argc, char *argv[])
{
    ROSE_INITIALIZE;
    ::mlog = Sawyer::Message::Facility("tool");
    rose::Diagnostics::mfacilities.insertAndAdjust(::mlog);

    Settings settings;
    SgProject *project = buildAst(argc, argv, settings);
    std::vector<SgAsmInterpretation*> interps = querySubTree<SgAsmInterpretation>(project);
    std::cerr <<"found " <<plural(interps.size(), "binary interpretations") <<"\n";

    // A Transcoder is responsible for converting some part of an AST into LLVM assembly.
    LlvmSemantics::TranscoderPtr transcoder = LlvmSemantics::Transcoder::instanceX86();
    transcoder->quietErrors(true);                      // catch exceptions and emit an LLVM comment instead
    transcoder->llvmVersion(settings.llvmVersion);      // controls which dialect of assembly is produced

    // Emit some LLVM. There are lots of methods for doing this, but this is the easiest.
    if (!interps.empty())
        transcoder->transcodeInterpretation(interps.back(), std::cout);
    
    return 0;
}
