#include "rose.h"
#include "MemoryMap.h"
#include "SRecord.h"

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem.hpp>
#include <sawyer/CommandLine.h>
#include <sawyer/Message.h>

using namespace rose;
using namespace Sawyer::Message::Common;

static Diagnostics::Facility mlog;                      // further initialization in main()

struct Settings {
    bool doHexDump;                                     // show a hexdump of the initialized memory
    std::string binaryPrefix;                           // if non-empty, generate binary files
    Settings(): doHexDump(false) {}
};

static Sawyer::CommandLine::ParserResult
parseCommandLine(int argc, char *argv[], Settings &settings/*out*/) {
    using namespace Sawyer::CommandLine;
    SwitchGroup generic = CommandlineProcessing::genericSwitches();

    SwitchGroup tool("Tool-specific switches");

    tool.insert(Switch("hexdump")
                .intrinsicValue(true, settings.doHexDump)
                .doc("Show a hexdump of the initialized memory, or turn off this feature with the @s{no-hexdump} switch. "
                     "The default is to " + std::string(settings.doHexDump?"":"not ") + "produce this output."));
    tool.insert(Switch("no-hexdump")
                .key("hexdump")
                .intrinsicValue(false, settings.doHexDump)
                .hidden(true));

    tool.insert(Switch("binary")
                .argument("prefix", anyParser(settings.binaryPrefix))
                .doc("Produce a binary file containing the data for each initialized segment of memory.  If a string is "
                     "specified then the string is used as the first part of the file name, and if the string contains a "
                     "percent character ('%') then that character is replaced with the non-path part of the specimen "
                     "file. The hexadecimal address of each memory segment is appended to the prefix.  The default is to "
                     "not produce binary files, which happens when the prefix string is empty."));

    Parser parser;
    parser
        .errorStream(mlog[FATAL])
        .purpose("load and list Motorola S-Records")
        .version(std::string(ROSE_SCM_VERSION_ID).substr(0, 8), ROSE_CONFIGURE_DATE)
        .chapter(1, "ROSE Command-line Tools")
        .doc("Synopsis", "@prop{programName} [@v{switches}] @v{SRecord_File}")
        .doc("Description",
             "Reads the ASCII @v{SRecord_File} specified on the command-line, loads it into memory, and optionally produces "
             "a hexdump and/or binary files for the resulting memory.");

    return parser.with(generic).with(tool).parse(argc, argv).apply();
}

int
main(int argc, char *argv[]) {
    // Initialization
    Diagnostics::initialize();
    mlog = Sawyer::Message::Facility("tool", Diagnostics::destination);
    Diagnostics::mfacilities.insertAndAdjust(mlog);

    // Parse command-line
    Settings settings;
    std::vector<std::string> positionalArgs = parseCommandLine(argc, argv, settings).unreachedArgs();
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
    if (!settings.binaryPrefix.empty()) {
        if (boost::contains(settings.binaryPrefix, "%")) {
            std::string inputFileBaseName = boost::filesystem::path(inputFileName).filename().native();
            boost::replace_first(settings.binaryPrefix, "%", inputFileBaseName);
        }
        mlog[WHERE] <<"dumping memory to binary files\n";
        BOOST_FOREACH (const MemoryMap::Node &node, map.nodes()) {
            const AddressInterval &interval = node.key();
            const MemoryMap::Segment &segment = node.value();
            std::string outputName = settings.binaryPrefix + StringUtility::addrToString(interval.least()).substr(2);// skip "0x"
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
    if (settings.doHexDump) {
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
