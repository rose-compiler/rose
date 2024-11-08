static const char *purpose = "count instructions";
static const char *description =
    "Scans the memory regions marked as executable and does a linear disassembly. The instruction mnemonics are listed along "
    "with the number of times they occur.";

#include <batSupport.h>

#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/CommandLine.h>
#include <Rose/Diagnostics.h>
#include <Rose/Initialize.h>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>

using namespace Bat;
using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

static Sawyer::Message::Facility mlog;

struct Settings {
    SerialIo::Format stateFormat = SerialIo::BINARY;
    boost::filesystem::path saveAs, augmentFrom;
};

static Sawyer::CommandLine::Parser
createSwitchParser(Settings &settings) {
    using namespace Sawyer::CommandLine;

    SwitchGroup generic = Rose::CommandLine::genericSwitches();
    generic.insert(Bat::stateFileFormatSwitch(settings.stateFormat));
    generic.insert(Switch("output", 'o')
                   .argument("file", anyParser(settings.saveAs))
                   .doc("Save output in the specified file, overwriting the file if it already existed. The default "
                        "is to print the output as a textual table to standard output."));

    generic.insert(Switch("input", 'i')
                   .argument("file", anyParser(settings.augmentFrom))
                   .doc("Initialize the histogram with data from the specified file. This can be used to accumulate "
                        "instruction frequencies across multiple specimens."));

    Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
    parser.errorStream(mlog[FATAL]);
    parser.with(generic);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] [@v{specimen}...]");
    return parser;
}

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
    mlog.comment("instruction frequencies");
    Bat::checkRoseVersionNumber(MINIMUM_ROSE_LIBRARY_VERSION, mlog[FATAL]);
    Bat::registerSelfTests();

    // Parse the command-line
    Settings settings;
    Sawyer::CommandLine::Parser switchParser = createSwitchParser(settings);
    auto engine = P2::Engine::forge(argc, argv, switchParser /*in,out*/);
    std::vector<std::string> specimen = parseCommandLine(argc, argv, switchParser);

    // Initialize the histogram.
    InsnHistogram histogram;
    if (!settings.augmentFrom.empty()) {
        if (!boost::filesystem::exists(settings.augmentFrom) && settings.augmentFrom == settings.saveAs) {
            // As a special case, it's not an error if the augment-from file does not exist but would be created as the output
            // of this tool. This is so that this tool can be used in a shell "for" loop like:
            //    rm result.dat
            //    for f in *.rba; do
            //        bat-insnfreq -a result.dat -o result.dat "$f"
            //    done
        } else {
            try {
                histogram = loadInsnHistogram(settings.augmentFrom);
            } catch (const SerialIo::Exception &e) {
                mlog[FATAL] <<e.what() <<"\n";
                exit(1);
            }
        }
    }

    // Ingest the specimen
    P2::Partitioner::ConstPtr partitioner;
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

    // Compute the histogram
    MemoryMap::Ptr map = partitioner->memoryMap();
    ASSERT_not_null(map);
    mergeInsnHistogram(histogram, computeInsnHistogram(partitioner->instructionProvider(), map));

    // Emit results
    if (!settings.saveAs.empty()) {
        saveInsnHistogram(histogram, settings.saveAs);
    } else {
        printInsnHistogram(histogram, std::cout);
    }
}
