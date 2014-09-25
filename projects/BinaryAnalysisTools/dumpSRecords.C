#include "rose.h"
#include "MemoryMap.h"
#include "SRecord.h"

#include <sawyer/CommandLine.h>
#include <sawyer/Message.h>

using namespace rose;
using namespace Sawyer::Message::Common;

static Diagnostics::Facility mlog("tool");              // further initialization in main()

struct Options {
    bool quiet;                                         // don't do the hexdump to stdout
    std::string prefix;                                 // if non-empty, generate binary files
    Options(): quiet(false) {}
};

static Sawyer::CommandLine::ParserResult
parseCommandLine(int argc, char *argv[], Options &opts) {
    using namespace Sawyer::CommandLine;
    SwitchGroup generic = CommandlineProcessing::genericSwitches();

    SwitchGroup tool("Tool-specific switches");
    tool.insert(Switch("quiet", 'q')
                .intrinsicValue(true, opts.quiet)
                .doc("Suppresses the hexdump output."));
    tool.insert(Switch("binary", 'b')
                .argument("prefix", anyParser(opts.prefix))
                .doc("Causes files to be created that contain the raw memory from loading the S-Records. The file "
                     "names are constructed from the specified @v{prefix} and an 8-character hexadecimal string "
                     "which is the starting address for a contiguous region of memory."));

    Parser parser;
    parser.errorStream(mlog[FATAL]);
    parser.purpose("load and list Motorola S-Records");
    parser.version(std::string(ROSE_SCM_VERSION_ID).substr(0, 8), ROSE_CONFIGURE_DATE);
    parser.chapter(1, "ROSE Command-line Tools");
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] @v{SRecord_File}");
    parser.doc("Description",
               "Reads the ASCII @v{SRecord_File} specified on the command-line, loads it into memory, and then produces "
               "a hexdump and/or binary files for the resulting memory.");

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
        mlog[FATAL] <<"exactly one S-Record file must be specified\n";
        if (positionalArgs.empty())
            mlog[INFO] <<"specify \"--help\" for usage information\n";
        exit(1);
    }

    // Open input file
    std::string inputFileName = positionalArgs[0];
    std::ifstream input(inputFileName.c_str());
    if (!input.good()) {
        mlog[FATAL] <<"cannot open input file: " <<inputFileName <<"\n";
        exit(1);
    }

    // Read input file and parse S-Records
    Sawyer::Message::Stream where(mlog[WHERE] <<"parsing S-Records from " <<inputFileName);
    std::vector<BinaryAnalysis::SRecord> srecs = BinaryAnalysis::SRecord::parse(input);
    where <<"; " <<StringUtility::plural(srecs.size(), "records") <<"\n";
    for (size_t i=0; i<srecs.size(); ++i) {
        if (!srecs[i].error().empty()) {
            mlog[ERROR] <<inputFileName <<":" <<(i+1) <<": " <<srecs[i].error() <<"\n";
        } else if (srecs[i].type() == BinaryAnalysis::SRecord::SREC_HEADER) {
            std::string str(srecs[i].data().begin(), srecs[i].data().end());
            std::cout <<"Header (line " <<(i+1) <<"): " <<StringUtility::cEscape(str) <<"\n";
        }
    }

    // Load S-Records into memory
    mlog[WHERE] <<"loading S-Records into memory map\n";
    MemoryMap map;
    rose_addr_t startAddr = BinaryAnalysis::SRecord::load(srecs, map, true/*create*/, MemoryMap::READABLE|MemoryMap::EXECUTABLE);
    std::cout <<"Memory map:\n";
    map.dump(std::cout, "    ");
    if (startAddr)
        std::cout <<"Starting address: " <<StringUtility::addrToString(startAddr) <<"\n";

    // Create binary files
    if (!opts.prefix.empty()) {
        mlog[WHERE] <<"dumping memory to binary files\n";
        BOOST_FOREACH (const MemoryMap::Node &node, map.nodes()) {
            const AddressInterval &interval = node.key();
            const MemoryMap::Segment &segment = node.value();
            std::string outputName = opts.prefix + StringUtility::addrToString(interval.least()).substr(2);// skip "0x"
            mlog[TRACE] <<"  dumping " <<StringUtility::plural(interval.size(), "bytes") <<" to " <<outputName <<"\n";
            std::ofstream output(outputName.c_str());
            const char *data = (const char*)segment.buffer()->data();
            if (data)
                output.write(data, interval.size());
            if (!output.good())
                mlog[ERROR] <<"cannot create output file: " <<outputName <<"\n";
        }
    }
    
    // Make the hexdump
    if (!opts.quiet) {
        mlog[WHERE] <<"dumping memory in hexdump format\n";
        HexdumpFormat fmt;
        fmt.numeric_fmt_special[0x00] = " .";           // make zeros less obtrusive
        fmt.numeric_fmt_special[0xff] = "##";           // make 0xff more obtrusive
        BOOST_FOREACH (const MemoryMap::Node &node, map.nodes()) {
            const AddressInterval &interval = node.key();
            const MemoryMap::Segment &segment = node.value();
            const unsigned char *data = (const unsigned char*)segment.buffer()->data();
            if (data) {
                std::cout <<"\n\n";
                SgAsmExecutableFileFormat::hexdump(std::cout, interval.least(), data, interval.size(), fmt);
            }
        }
    }

    return 0;
}
