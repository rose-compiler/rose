static const char *purpose = "tests concolic testing";
static const char *description =
    "To be written. This program is currently only for testing some of the concolic testing framework and is not useful "
    "to users at this time.";

#include <rose.h>

#include <BinaryConcolic.h>                             // rose
#include <CommandLine.h>                                // rose
#include <Partitioner2/Engine.h>                        // rose
#include <Sawyer/CommandLine.h>
#include <Sawyer/Message.h>

using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

static Sawyer::Message::Facility mlog;

static std::vector<std::string>
parseCommandLine(int argc, char *argv[], P2::Engine &engine) {
    using namespace Sawyer::CommandLine;
    Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
    parser.groupNameSeparator("-");
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] @v{specimen_names}");
    parser.doc("Specimens", engine.specimenNameDocumentation());
    parser.with(engine.engineSwitches());
    parser.with(engine.loaderSwitches());
    parser.with(engine.disassemblerSwitches());
    parser.with(engine.partitionerSwitches());

    std::vector<std::string> specimen = parser.parse(argc, argv).apply().unreachedArgs();
    if (specimen.empty()) {
        mlog[FATAL] <<"no binary specimen specified; see --help\n";
        exit(1);
    }
    return specimen;
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Rose::Diagnostics::initAndRegister(&mlog, "tool");
    mlog.comment("concolic testing tool proper");

#if !defined(__linux__)
    mlog[INFO] <<"this test supported only for Linux operating systems\n";
    exit(0); // test succeeds since it does nothing
#endif

    P2::Engine engine;
    std::vector<std::string> specimen = parseCommandLine(argc, argv, engine);
    P2::Partitioner partitioner = engine.partition(specimen);

    // Find the starting address
    SgAsmInterpretation *interp = engine.interpretation();
    ASSERT_not_null(interp);
    SgAsmElfFileHeader *hdr = isSgAsmElfFileHeader(interp->get_headers()->get_headers().back());
    if (!hdr || hdr->get_exec_format()->get_purpose() != SgAsmExecutableFileFormat::PURPOSE_EXECUTABLE ||
        hdr->get_exec_format()->get_word_size() != 8 ||
        !hdr->get_sections_by_name(".interp").empty()) {
        mlog[FATAL] <<"only statically-linked 64-bit ELF executables are supported\n";
        exit(1);
    }
    rose_addr_t entryVa = hdr->get_entry_rva() + hdr->get_base_va();

    // Test concolic executor
    Concolic::ConcolicExecutor::Ptr concolicExecutor = Concolic::ConcolicExecutor::instance();
    concolicExecutor->run(partitioner, entryVa);
}
