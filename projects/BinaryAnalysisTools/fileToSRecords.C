#include <rose.h>
#include <SRecord.h>

#include <sawyer/CommandLine.h>
#include <sawyer/Message.h>

using namespace rose;
using namespace Sawyer::Message::Common;

static Diagnostics::Facility mlog("tool");              // further initialization in main()

struct Options {
    size_t addrSize;                                    // bytes per address (2, 3, or 4)
    rose_addr_t startVa;                                // address where file is loaded
    Options(): addrSize(4), startVa(0) {}
};

static Sawyer::CommandLine::ParserResult
parseCommandLine(int argc, char *argv[], Options &opts) {
    using namespace Sawyer::CommandLine;
    SwitchGroup generic = CommandlineProcessing::genericSwitches();

    SwitchGroup tool("Tool-specific switches");
    tool.insert(Switch("addrsize")
                .argument("nbytes", nonNegativeIntegerParser(opts.addrSize))
                .doc("Number of bytes in which to encode addresses.  The default is " +
                     StringUtility::numberToString(opts.addrSize) + ".  The program will fail if it encounters an "
                     "address that is two wide to represent in the specified number of bytes."));
    tool.insert(Switch("at")
                .argument("address", nonNegativeIntegerParser(opts.startVa))
                .doc("Virtual address at which the file is loaded.  The default is " +
                     StringUtility::addrToString(opts.startVa)));

    Parser parser;
    parser.errorStream(mlog[FATAL]);
    parser.purpose("load file into memory and produce Motorola S-records");
    parser.version(std::string(ROSE_SCM_VERSION_ID).substr(0, 8), ROSE_CONFIGURE_DATE);
    parser.chapter(1, "ROSE Command-line Tools");
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] @v{file}");
    parser.doc("Description",
               "Loads the @v{file} into memory at the address specified with the @s{at} switch and then produces "
               "Motorola S-Records on standard output.");

    return parser.with(generic).with(tool).parse(argc, argv).apply();
}

int
main(int argc, char *argv[]) {
    // Initialization
    Diagnostics::initialize();
    mlog.initStreams(rose::Diagnostics::destination);
    Diagnostics::mfacilities.insertAndAdjust(mlog);

    // Parse command-line
    Options opts;
    std::vector<std::string> positionalArgs = parseCommandLine(argc, argv, opts).unreachedArgs();
    if (positionalArgs.size()!=1) {
        mlog[FATAL] <<"exactly one binary file must be specified\n";
        if (positionalArgs.empty())
            mlog[INFO] <<"specify \"--help\" for usage information\n";
        exit(1);
    }
    if (2!=opts.addrSize && 3!=opts.addrSize && 4!=opts.addrSize) {
        mlog[FATAL] <<"address size must be 2, 3, or 4 bytes\n";
        exit(1);
    }

    // Open input file
    mlog[WHERE] <<"loading file into memory\n";
    std::string inputFileName = positionalArgs[0];
    MemoryMap map;
    if (!map.insertFile(inputFileName, opts.startVa)) {
        mlog[FATAL] <<"problem reading file: " <<inputFileName <<"\n";
        exit(1);
    }
    map.dump(mlog[INFO]);

    // Produce output
    Stream where(mlog[WHERE] <<"emitting S-Records");
    size_t nrecs = BinaryAnalysis::SRecord::dump(map, std::cout, opts.addrSize);
    where <<"; " <<StringUtility::plural(nrecs, "records") <<"\n";
    return 0;
}
