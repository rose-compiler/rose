static const char *purpose = "count instructions";
static const char *description =
    "Scans the memory regions marked as executable and does a linear disassembly. The instruction mnemonics are listed along "
    "with the number of times they occur.";

#include <rose.h>
#include <CommandLine.h>                                // rose
#include <Diagnostics.h>                                // rose
#include <Partitioner2/Engine.h>                        // rose
#include <Partitioner2/Partitioner.h>                   // rose

#include <batSupport.h>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <Sawyer/CommandLine.h>

using namespace Bat;
using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

static Sawyer::Message::Facility mlog;
static SerialIo::Format stateFormat = SerialIo::BINARY;
static boost::filesystem::path saveAs, augmentFrom;

static std::vector<boost::filesystem::path>
parseCommandLine(int argc, char *argv[]) {
    using namespace Sawyer::CommandLine;

    SwitchGroup generic = Rose::CommandLine::genericSwitches();
    generic.insert(Bat::stateFileFormatSwitch(stateFormat));
    generic.insert(Switch("output", 'o')
                   .argument("file", anyParser(saveAs))
                   .doc("Save output in the specified file, overwriting the file if it already existed. The default "
                        "is to print the output as a textual table to standard output."));

    generic.insert(Switch("input", 'i')
                   .argument("file", anyParser(augmentFrom))
                   .doc("Initialize the histogram with data from the specified file. This can be used to accumulate "
                        "instruction frequencies across multiple specimens."));

    Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
    parser.errorStream(mlog[FATAL]);
    parser.with(generic);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] [@v{rba_files}...]");

    std::vector<std::string> input = parser.parse(argc, argv).apply().unreachedArgs();
    return std::vector<boost::filesystem::path>(input.begin(), input.end());
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");
    mlog.comment("instruction frequencies");
    Bat::checkRoseVersionNumber(MINIMUM_ROSE_LIBRARY_VERSION, mlog[FATAL]);
    Bat::registerSelfTests();
    std::vector<boost::filesystem::path> rbaFiles = parseCommandLine(argc, argv);

    // Initialize the histogram.
    InsnHistogram histogram;
    if (!augmentFrom.empty()) {
        if (!boost::filesystem::exists(augmentFrom) && augmentFrom == saveAs) {
            // As a special case, it's not an error if the augment-from file does not exist but would be created as the output
            // of this tool. This is so that this tool can be used in a shell "for" loop like:
            //    rm result.dat
            //    for f in *.rba; do
            //        bat-insnfreq -a result.dat -o result.dat "$f"
            //    done
        } else {
            try {
                histogram = loadInsnHistogram(augmentFrom);
            } catch (const SerialIo::Exception &e) {
                mlog[FATAL] <<e.what() <<"\n";
                exit(1);
            }
        }
    }

    // Compute the histogram
    for (const boost::filesystem::path &rbaFile: rbaFiles) {
        P2::Engine engine;
        P2::Partitioner partitioner = engine.loadPartitioner(rbaFile, stateFormat);
        MemoryMap::Ptr map = partitioner.memoryMap();
        ASSERT_not_null(map);
        mergeInsnHistogram(histogram, computeInsnHistogram(partitioner.instructionProvider(), map));
    }
    
    // Emit results
    if (!saveAs.empty()) {
        saveInsnHistogram(histogram, saveAs);
    } else {
        printInsnHistogram(histogram, std::cout);
    }
}
