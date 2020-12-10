static const char *purpose = "info about the ELF/PE container";
static const char *description =
    "Given a BAT state for a binary specimen, print all information about the ELF or PE container.\n\n"

    "This tool reads the binary analysis state file provided as a command-line positional argument, or standard input if "
    "the name is \"-\" (a single hyphen) no file name is specified. The standard input mode works only on those operating "
    "systems whose standard input is opened in binary mode, such as Unix-like systems.";

#include <rose.h>
#include <CommandLine.h>                                        // rose
#include <Partitioner2/Engine.h>                                // rose

#include <batSupport.h>
#include <Sawyer/Stopwatch.h>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

Sawyer::Message::Facility mlog;
SerialIo::Format stateFormat = SerialIo::BINARY;

// Parses the command-line and returns the name of the input file if any (the ROSE binary state).
boost::filesystem::path
parseCommandLine(int argc, char *argv[], P2::Engine &engine) {
    using namespace Sawyer::CommandLine;

    SwitchGroup gen = Rose::CommandLine::genericSwitches();
    gen.insert(Bat::stateFileFormatSwitch(stateFormat));

    Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
    parser.errorStream(mlog[FATAL]);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] [@v{rba-state}]");
    parser.with(gen);
    std::vector<std::string> input = parser.parse(argc, argv).apply().unreachedArgs();
    if (input.size() > 1) {
        mlog[FATAL] <<"incorrect usage; see --help\n";
        exit(1);
    }
    return input.empty() ? boost::filesystem::path("-") : input[0];
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");
    mlog.comment("querying information about ELF/PE containers");
    Bat::checkRoseVersionNumber(MINIMUM_ROSE_LIBRARY_VERSION, mlog[FATAL]);
    Bat::registerSelfTests();

    P2::Engine engine;
    boost::filesystem::path inputFileName = parseCommandLine(argc, argv, engine);
    P2::Partitioner partitioner = engine.loadPartitioner(inputFileName, stateFormat);

    BOOST_FOREACH (SgFile *file, SageInterface::generateFileList()) {
        if (SgBinaryComposite *binComp = isSgBinaryComposite(file)) {
            if (SgAsmGenericFileList *genFileList = binComp->get_genericFileList()) {
                BOOST_FOREACH (SgAsmGenericFile *asmFile, genFileList->get_files()) {
                    std::cout <<asmFile->format_name() <<"\n";

                    // A table describing the sections of the file
                    asmFile->dump(stdout);

                    // Detailed info about each section
                    const SgAsmGenericSectionPtrList &sections = asmFile->get_sections();
                    for (size_t i = 0; i < sections.size(); i++) {
                        printf("Section [%zd]:\n", i);
                        sections[i]->dump(stdout, "  ", -1);
                    }
                }
            }
        }
    }
}
