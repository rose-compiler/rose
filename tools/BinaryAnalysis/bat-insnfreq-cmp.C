static const char *purpose = "compare instruction frequencies";
static const char *description =
    "Reads instruction frequencies from files specified on the command-line, then compares the first file with "
    "all the remaining files. The files must have been created with @sa{bat-insnfreq}(1).";

#include <rose.h>
#include <CommandLine.h>                                // rose
#include <Diagnostics.h>                                // rose

#include <batSupport.h>
#include <boost/filesystem.hpp>
#include <Sawyer/CommandLine.h>

using namespace Bat;
using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;

static Sawyer::Message::Facility mlog;

static std::vector<boost::filesystem::path>
parseCommandLine(int argc, char *argv[]) {
    using namespace Sawyer::CommandLine;

    SwitchGroup generic = Rose::CommandLine::genericSwitches();

    Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
    parser.errorStream(mlog[FATAL]);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] @v{freq_files}...");

    std::vector<std::string> input = parser.parse(argc, argv).apply().unreachedArgs();
    if (input.size() < 2) {
        mlog[FATAL] <<"at least two frequency files must be specified\n";
        exit(1);
    }
    return std::vector<boost::filesystem::path>(input.begin(), input.end());
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");
    mlog.comment("instruction frequencies");
    Bat::checkRoseVersionNumber(MINIMUM_ROSE_LIBRARY_VERSION, mlog[FATAL]);
    Bat::registerSelfTests();
    std::vector<boost::filesystem::path> fileNames = parseCommandLine(argc, argv);

    // Read the first histogram
    InsnHistogram a = loadInsnHistogram(fileNames[0]);
    std::vector<InsnHistogram> aQuartiles = splitInsnHistogram(a, 4);

    // Compare the first histogram with the rest
    std::vector<std::pair<boost::filesystem::path, double>> output;
    for (size_t i = 1; i < fileNames.size(); ++i) {
        InsnHistogram b = loadInsnHistogram(fileNames[i]);
        double diff = compareInsnHistograms(aQuartiles, b);
        output.push_back(std::make_pair(fileNames[i], diff));
    }

    // Produce output
    std::sort(output.begin(), output.end(), [](auto &a, auto &b) { return a.second < b.second; });
    for (const auto &pair: output)
        std::cout <<(boost::format("%7.3f%%") % (100.0*pair.second)) <<"\t" <<pair.first <<"\n";
}
