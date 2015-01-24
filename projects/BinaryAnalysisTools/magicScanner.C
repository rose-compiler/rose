#include <rose.h>

#include <boost/algorithm/string/trim.hpp>
#include <FileSystem.h>
#include <Partitioner2/Engine.h>
#include <Partitioner2/Utility.h>
#include <sawyer/CommandLine.h>
#include <sawyer/ProgressBar.h>

using namespace rose;
using namespace rose::Diagnostics;

struct Settings {
    AddressInterval limits;                             // limits for scanning (empty implies all addresses)
    size_t step;                                        // amount by which to increment each time
    Settings(): step(1) {}
};

static Sawyer::CommandLine::ParserResult
parseCommandLine(int argc, char *argv[], Settings &settings /*in,out*/) {
    using namespace Sawyer::CommandLine;
    Parser parser;

    parser
        .purpose("looks for magic numbers in binary specimens")
        .version(std::string(ROSE_SCM_VERSION_ID).substr(0, 8), ROSE_CONFIGURE_DATE)
        .chapter(1, "ROSE Command-line Tools")
        .doc("synopsis", "@prop{programName} [@v{switches}] @v{specimen_names}")
        .doc("description", "Parses and loads the specimen, then looks for magic numbers at each address.")
        .doc("Specimens", BinaryAnalysis::Partitioner2::Engine::specimenNameDocumentation());

    SwitchGroup gen = CommandlineProcessing::genericSwitches();

    SwitchGroup tool("Switches specific to this tool");
    tool.insert(Switch("addresses")
                .argument("interval", BinaryAnalysis::Partitioner2::addressIntervalParser(settings.limits))
                .doc("Limits the scanning to the specified addresses. The default is to scan all addresses. " +
                     BinaryAnalysis::Partitioner2::AddressIntervalParser::docString()));
    tool.insert(Switch("address")                       // sometimes the singular form is better: --address=123
                .key("addresses")
                .argument("interval", BinaryAnalysis::Partitioner2::addressIntervalParser(settings.limits))
                .hidden());
    tool.insert(Switch("step")
                .argument("nbytes", nonNegativeIntegerParser(settings.step))
                .doc("Number of bytes to advance after each scan.  The default is " +
                     boost::lexical_cast<std::string>(settings.step) + "."));

    return parser.with(gen).with(tool).parse(argc, argv).apply();
}

static std::string
scanMagicNumbers(const FileSystem::Path &fileName) {
    std::string cmd = "file " + fileName.string();
    if (FILE *f = popen(cmd.c_str(), "r")) {
        char line[1024];
        if (fgets(line, sizeof line, f)) {
            pclose(f);
            return boost::trim_right_copy(std::string(line).substr(fileName.string().size()+2));
        }
        pclose(f);
    }
    return "";
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
    Diagnostics::initialize();

    Settings settings;
    Sawyer::CommandLine::ParserResult cmdline = parseCommandLine(argc, argv, settings /*in,out*/);
    std::vector<std::string> specimenNames = cmdline.unreachedArgs();

    FileSystem::Path tempFileName = boost::filesystem::unique_path("/tmp/rose-%%%%-%%%%-%%%%-%%%%");

    MemoryMap map = BinaryAnalysis::Partitioner2::Engine().load(specimenNames);
    map.dump(mlog[INFO]);

    size_t step = std::max(size_t(1), settings.step);
    AddressInterval limits = settings.limits.isEmpty() ? map.hull() : (settings.limits & map.hull());
    Sawyer::Container::IntervalSet<AddressInterval> addresses(map);
    addresses.intersect(limits);
    size_t nPositions = addresses.size() / step;
    mlog[INFO] <<"approximately " <<StringUtility::plural(nPositions, "positions") <<" to check\n";

    {
        Sawyer::ProgressBar<size_t> progress(nPositions, mlog[INFO], "positions");
        for (rose_addr_t va=limits.least();
             va<=limits.greatest() && map.atOrAfter(va).next().assignTo(va);
             va+=step, ++progress) {
            uint8_t buffer[1024];
            size_t nBytes = map.at(va).limit(sizeof buffer).read(buffer).size();
            std::ofstream(tempFileName.c_str()).write((const char*)buffer, nBytes);
            std::string magicString = scanMagicNumbers(tempFileName);
            if (magicString!="data")                    // "file" runs home to Momma when it has nothing else to do
                std::cout <<StringUtility::addrToString(va) <<" |" <<leadingBytes(buffer, nBytes) <<" | " <<magicString <<"\n";
            if (va==limits.greatest())
                break;                                  // prevent overflow at top of address space
        }
    }

    boost::filesystem::remove(tempFileName);
}
