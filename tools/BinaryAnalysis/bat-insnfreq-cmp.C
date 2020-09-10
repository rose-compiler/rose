#if __cplusplus >= 201402L

static const char *purpose = "compare instruction frequencies";
static const char *description =
    "Reads instruction frequencies from files specified on the command-line, then compares the first file with "
    "all the remaining files. The files must have been created with @man{bat-insnfreq}{1}. Each line of output "
    "shows a database name and a percent match where higher values imply a better match.";

#include <rose.h>
#include <CommandLine.h>                                // rose
#include <Diagnostics.h>                                // rose

#include <batSupport.h>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <Sawyer/CommandLine.h>

using namespace Bat;
using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;

static Sawyer::Message::Facility mlog;
static size_t sensitivity = 1000;

static std::vector<boost::filesystem::path>
parseCommandLine(int argc, char *argv[]) {
    using namespace Sawyer::CommandLine;

    SwitchGroup generic = Rose::CommandLine::genericSwitches();

    Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
    parser.errorStream(mlog[FATAL]);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] @v{freq_files}...");

    SwitchGroup tool("Tool specific switches");
    tool.name("tool");
    tool.insert(Switch("sensitivity", 's')
                .argument("n", nonNegativeIntegerParser(sensitivity))
                .doc("Histogram comparison sensitivity, where @v{n} is a positive integer. Higher values result in more sensitive "
                     "comparisons between the histograms, which can be good if the training data is large and matches well with "
                     "the specimen being classified. The default sensitivity is " + boost::lexical_cast<std::string>(sensitivity) +
                     ".  Sensitivity is limited by the number of unique instruction mnemonics present in the training data, and "
                     "specifying a value larger than this will not further increase sensitivity."));

    std::vector<std::string> input = parser.with(generic).with(tool).parse(argc, argv).apply().unreachedArgs();
    if (input.size() < 2) {
        mlog[FATAL] <<"at least two frequency files must be specified\n";
        exit(1);
    }
    sensitivity = std::max(sensitivity, size_t{1});
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
    std::vector<InsnHistogram> aSplit = splitInsnHistogram(a, sensitivity);

    // Compare the first histogram with the rest
    std::vector<std::pair<boost::filesystem::path, double>> output;
    for (size_t i = 1; i < fileNames.size(); ++i) {
        InsnHistogram b = loadInsnHistogram(fileNames[i]);
        double diff = compareInsnHistograms(aSplit, b);
        output.push_back(std::make_pair(fileNames[i], diff));
    }

    // Produce output. Watch out: the data has the amount of difference, but we want to show the amount of similarity.
    std::sort(output.begin(), output.end(), [](auto &a, auto &b) { return a.second < b.second; });
    for (const auto &pair: output)
        std::cout <<(boost::format("%7.3f%%") % (100.0*(1 - pair.second))) <<"\t" <<pair.first <<"\n";
}


#else

// C++ comppiler is too old
#include <iostream>

int main(int argc, char *argv[]) {
    std::cerr <<argv[0] <<": this tool is disabled (your C++ compiler is too old)\n";
    return 1;
}

#endif
