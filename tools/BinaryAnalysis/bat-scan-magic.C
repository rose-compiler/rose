const char *purpose = "looks for magic numbers in binaries";
const char *description =
    "Parses and loads the specimen, then looks for a magic number at each address.";

#include <batSupport.h>

#include <Rose/BinaryAnalysis/AddressInterval.h>
#include <Rose/BinaryAnalysis/MagicNumber.h>
#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/BinaryAnalysis/Partitioner2/Utility.h>
#include <Rose/CommandLine.h>
#include <Rose/Initialize.h>

#include <Sawyer/ProgressBar.h>

#include <boost/algorithm/string/predicate.hpp>

using namespace Rose;
using namespace Rose::Diagnostics;
using namespace Rose::BinaryAnalysis;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

struct Settings {
    AddressInterval limits;                             // limits for scanning (empty implies all addresses)
    size_t step = 1;                                    // amount by which to increment each time
    size_t maxBytes = 256;                              // number of bytes to check at one time
    SerialIo::Format stateFormat = SerialIo::BINARY;
};

static Sawyer::CommandLine::Parser
createSwitchParser(Settings &settings) {
    using namespace Sawyer::CommandLine;

    SwitchGroup gen = Rose::CommandLine::genericSwitches();
    gen.insert(Bat::stateFileFormatSwitch(settings.stateFormat));

    SwitchGroup tool("Switches specific to this tool");
    tool.name("tool");

    tool.insert(Switch("addresses")
                .argument("interval", BinaryAnalysis::Partitioner2::addressIntervalParser(settings.limits))
                .doc("Limits the scanning to the specified addresses. The default is to scan all addresses. " +
                     BinaryAnalysis::Partitioner2::AddressIntervalParser::docString()));
    tool.insert(Switch("address")                       // sometimes the singular form is better: --address=123
                .key("addresses")
                .argument("interval", BinaryAnalysis::Partitioner2::addressIntervalParser(settings.limits))
                .hidden(true));
    tool.insert(Switch("step")
                .argument("nbytes", nonNegativeIntegerParser(settings.step))
                .doc("Number of bytes to advance after each scan.  The default is " +
                     boost::lexical_cast<std::string>(settings.step) + "."));
    tool.insert(Switch("limit")
                .argument("nbytes", nonNegativeIntegerParser(settings.maxBytes))
                .doc("Maximum number of bytes to pass to the detection functions per call.  The default is " +
                     boost::lexical_cast<std::string>(settings.maxBytes) + ". Large values may occassionally be " +
                     "more accurate, but small values are faster.  The ROSE library's detector also has a hard-coded " +
                     "limit which will never be exceeded regardless of this setting."));

    Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
    parser.errorStream(mlog[FATAL]);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] @v{specimen}");
    parser.with(tool).with(gen);
    return parser;
}

static std::vector<std::string>
parseCommandLine(int argc, char *argv[], Sawyer::CommandLine::Parser &parser) {
    std::vector<std::string> specimen = parser.parse(argc, argv).apply().unreachedArgs();
    if (specimen.empty())
        specimen.push_back("-");
    return specimen;
}

static std::string
leadingBytes(const uint8_t *buf, size_t bufsize) {
    std::string retval;
    static const size_t nBytesToShow = 8;
    char s[8];
    for (size_t i=0; i<nBytesToShow; ++i) {
        if (i>=bufsize) {
            retval += "   ";
        } else if (isgraph(buf[i])) {
            sprintf(s, "  %c", buf[i]);
            retval += s;
        } else {
            sprintf(s, " %02x", (unsigned)buf[i]);
            retval += s;
        }
    }
    return retval;
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");
    mlog.comment("scanning for magic numbers");
    Bat::checkRoseVersionNumber(MINIMUM_ROSE_LIBRARY_VERSION, mlog[FATAL]);
    Bat::registerSelfTests();

    Settings settings;
    Sawyer::CommandLine::Parser switchParser = createSwitchParser(settings);
    auto engine = P2::Engine::forge(argc, argv, switchParser /*in,out*/);
    const std::vector<std::string> specimen = parseCommandLine(argc, argv, switchParser);

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

    BinaryAnalysis::MagicNumber analyzer;
    analyzer.maxBytesToCheck(settings.maxBytes);
    MemoryMap::Ptr map = partitioner->memoryMap();
    map->dump(mlog[INFO]);

    size_t step = std::max(size_t(1), settings.step);
    AddressInterval limits = settings.limits.isEmpty() ? map->hull() : (settings.limits & map->hull());
    Sawyer::Container::IntervalSet<AddressInterval> addresses(*map);
    addresses.intersect(limits);
    size_t nPositions = addresses.size() / step;
    mlog[INFO] <<"approximately " <<StringUtility::plural(nPositions, "positions") <<" to check\n";

    {
        Sawyer::ProgressBar<size_t> progress(nPositions, mlog[INFO], "positions");
        for (Address va=limits.least();
             va<=limits.greatest() && map->atOrAfter(va).next().assignTo(va);
             va+=step, ++progress) {
            std::string magicString = analyzer.identify(map, va);
            if (magicString!="data") {                  // runs home to Momma when it gets confused
                uint8_t buf[8];
                size_t nBytes = map->at(va).limit(sizeof buf).read(buf).size();
                std::cout <<StringUtility::addrToString(va) <<" |" <<leadingBytes(buf, nBytes) <<" | " <<magicString <<"\n";
            }
            if (va==limits.greatest())
                break;                                  // prevent overflow at top of address space
        }
    }
}
