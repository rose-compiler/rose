static const char *purpose = "info about the ELF/PE container";
static const char *description =
    "Given a binary specimen, print all information about the ELF or PE container.";

#include <rose.h>                                       // Needed only for `#include <sageInterface.h>` below

#include <batSupport.h>

#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/CommandLine.h>
#include <Rose/Initialize.h>

#include <sageInterface.h>                              // ROSE

#include <boost/algorithm/string/predicate.hpp>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

Sawyer::Message::Facility mlog;

struct Settings {
    SerialIo::Format stateFormat = SerialIo::BINARY;
};

Sawyer::CommandLine::Parser
createSwitchParser(Settings &settings) {
    using namespace Sawyer::CommandLine;

    SwitchGroup gen = Rose::CommandLine::genericSwitches();
    gen.insert(Bat::stateFileFormatSwitch(settings.stateFormat));

    Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
    parser.errorStream(mlog[FATAL]);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] [@v{specimen}]");
    parser.with(gen);
    return parser;
}

// Parses the command-line and returns the positional arguments describing the specimen.
static std::vector<std::string>
parseCommandLine(int argc, char *argv[], Sawyer::CommandLine::Parser &parser) {
    std::vector<std::string> specimen = parser.parse(argc, argv).apply().unreachedArgs();
    if (specimen.empty())
        specimen.push_back("-");
    return specimen;
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");
    mlog.comment("querying information about ELF/PE containers");
    Bat::checkRoseVersionNumber(MINIMUM_ROSE_LIBRARY_VERSION, mlog[FATAL]);
    Bat::registerSelfTests();

    // Parse command-line
    Settings settings;
    Sawyer::CommandLine::Parser switchParser = createSwitchParser(settings);
    auto engine = P2::Engine::forge(argc, argv, switchParser /*in,out*/);
    std::vector<std::string> specimen = parseCommandLine(argc, argv, switchParser);

    // Ingest specimen
    P2::Partitioner::Ptr partitioner;
    if (specimen.size() == 1 && (specimen[0] == "-" || boost::ends_with(specimen[0], ".rba"))) {
        try {
            partitioner = P2::Partitioner::instanceFromRbaFile(specimen[0], settings.stateFormat);
        } catch (const std::exception &e) {
            mlog[FATAL] <<"cannot load partitioner from " <<specimen[0] <<": " <<e.what() <<"\n";
            exit(1);
        }
    } else {
        partitioner = engine->partition(specimen);
    }
    ASSERT_not_null(partitioner);

    // Print the AST
    for (SgFile *file: SageInterface::generateFileList()) {
        if (SgBinaryComposite *binComp = isSgBinaryComposite(file)) {
            if (SgAsmGenericFileList *genFileList = binComp->get_genericFileList()) {
                for (SgAsmGenericFile *asmFile: genFileList->get_files()) {
                    std::cout <<asmFile->formatName() <<"\n";

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
