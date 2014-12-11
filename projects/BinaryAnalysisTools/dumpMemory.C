#define BOOST_FILESYSTEM_VERSION 3

#include "rose.h"

#include "SRecord.h"

#include <Partitioner2/Engine.h>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem.hpp>
#include <sawyer/CommandLine.h>
#include <sawyer/Message.h>

using namespace rose;
using namespace Sawyer::Message::Common;
using namespace rose::BinaryAnalysis;
namespace P2 = rose::BinaryAnalysis::Partitioner2;

static Diagnostics::Facility mlog;                      // further initialization in main()

struct Settings {
    bool showAsHex;                                     // show memory in hexdump format
    bool showAsSRecords;                                // show memory in Motorola S-Record format
    bool showAsBinary;                                  // show memory as raw bytes (output prefix required)
    bool showMap;                                       // show memory mapping information
    std::string outputPrefix;                           // file name prefix for output, or send it to standard output
    AddressInterval where;                              // addresses that should be dumped
    Settings()
        : showAsHex(false), showAsSRecords(false), showAsBinary(false), showMap(false), where(AddressInterval::whole()) {}
};

static Sawyer::CommandLine::ParserResult
parseCommandLine(int argc, char *argv[], Settings &settings/*out*/) {
    using namespace Sawyer::CommandLine;
    SwitchGroup generic = CommandlineProcessing::genericSwitches();

    SwitchGroup fmt("Format switches");
    fmt.insert(Switch("hexdump")
               .intrinsicValue(true, settings.showAsHex)
               .doc("Dump the specimen memory as ASCII text using an output format similar to the @man{hexdump}(1) command."));

    fmt.insert(Switch("srecords")
               .intrinsicValue(true, settings.showAsSRecords)
               .doc("Dump the specimen memory as Motorola S-Records."));

    fmt.insert(Switch("binary")
               .intrinsicValue(true, settings.showAsBinary)
               .doc("Dump the specimen memory as raw binary files.  The @s{prefix} switch is required for this format since "
                    "otherwise there is no way to indicate the addresses that correspond to the dumped data.  Each segment "
                    "will be output to a file ending with \"<@v{prefix}><@v{address}>.raw\", where @v{prefix} is specified with "
                    "the @s{prefix} switch and @v{address} is the starting address for the segment.  An additional "
                    "\"<@v{prefix}>.load\" file is created that can be used to load the raw files back into another ROSE "
                    "command, usualyl by specifying \"@@v{prefix}.load\" at the end of its command-line."));

    SwitchGroup out("Output switches");
    out.insert(Switch("map")
               .intrinsicValue(true, settings.showMap)
               .doc("Show information about the memory map on standard output.  If no output formats are specified then the "
                    "memory map is displayed regardless of whether the @s{map} switch is present."));

    out.insert(Switch("prefix")
               .argument("string", anyParser(settings.outputPrefix))
               .doc("Causes output to be emitted to a set of files whose names all begin with the specified @v{string}. "
                    "When this switch is absent or the @v{string} is empty then output will be sent to standard output, but "
                    "not all output formats permit standard output."));

    SwitchGroup misc("Other switches");
    misc.insert(Switch("where")
                .argument("interval", P2::addressIntervalParser(settings.where))
                .doc("Specifies the addresses that should be dumped. The default is to dump all mapped addresses. " +
                     P2::AddressIntervalParser::docString() + "  The specified interval may include addresses "
                     "that aren't mapped and which are silently ignored."));

    Parser parser;
    parser
        .errorStream(mlog[FATAL])
        .purpose("dump specimen memory")
        .version(std::string(ROSE_SCM_VERSION_ID).substr(0, 8), ROSE_CONFIGURE_DATE)
        .chapter(1, "ROSE Command-line Tools")
        .doc("Synopsis", "@prop{programName} [@v{switches}] @v{specimen}...")
        .doc("Description",
             "Parses and/or loads the @v{specimen} into ROSE's analysis memory and then dumps that memory in a variety of "
             "formats selected with @v{switches}.")
        .doc("Specimens", P2::Engine::specimenNameDocumentation());

    return parser.with(generic).with(fmt).with(out).with(misc).parse(argc, argv).apply();
}

class Dumper {
public:
    void operator()(const Settings &settings, const MemoryMap &map, const AddressInterval &dataInterval, std::ostream &stream) {
        MemoryMap::ConstNodeIterator inode = map.at(dataInterval.least()).nodes().begin();
        ASSERT_forbid(inode == map.nodes().end());
        const AddressInterval &segmentInterval = inode->key();
        ASSERT_require(segmentInterval.isContaining(dataInterval));
        const MemoryMap::Segment &segment = inode->value();
        if (const uint8_t *data = segment.buffer()->data()) {
            rose_addr_t bufferOffset = segment.offset() + dataInterval.least() - segmentInterval.least();
            ASSERT_require(segment.buffer()->available(bufferOffset) >= dataInterval.size());
            formatData(stream, segmentInterval, segment, dataInterval, data+bufferOffset);
        }
    }

    virtual void formatData(std::ostream&, const AddressInterval &segmentInterval, const MemoryMap::Segment&,
                            const AddressInterval &dataInterval, const uint8_t *data) = 0;
};

class HexDumper: public Dumper {
    HexdumpFormat fmt_;
public:
    virtual void formatData(std::ostream &stream, const AddressInterval &segmentInterval, const MemoryMap::Segment &segment,
                            const AddressInterval &dataInterval, const uint8_t *data) ROSE_OVERRIDE {
        rose_addr_t va = dataInterval.least();
        rose_addr_t nRemain = dataInterval.size();

        if (dataInterval.least() == segmentInterval.least())
            stream <<"Dumping segment " <<segmentInterval <<" \"" <<StringUtility::cEscape(segment.name()) <<"\"\n";
        if (dataInterval != segmentInterval)
            stream <<"Data for addresses " <<dataInterval <<":\n";

        // Hexdumps are typically aligned so the first byte on each line is aligned on a 16-byte address, so print
        // out some stuff to get the rest aligned if necessary.
        rose_addr_t nLeader = std::min(16 - va % 16, nRemain);
        if (nLeader != 16) {
            SgAsmExecutableFileFormat::hexdump(stream, va, data, nLeader, fmt_);
            va += nLeader;
            data += nLeader;
            nRemain -= nLeader;
            stream <<"\n";
        }

        SgAsmExecutableFileFormat::hexdump(stream, va, data, nRemain, fmt_);
        stream <<"\n";
    }
};

class SRecordDumper: public Dumper {
public:
    virtual void formatData(std::ostream &stream, const AddressInterval &segmentInterval, const MemoryMap::Segment &segment,
                            const AddressInterval &dataInterval, const uint8_t *data) ROSE_OVERRIDE {
        MemoryMap map;
        map.insert(dataInterval, MemoryMap::Segment::staticInstance(data, dataInterval.size(), MemoryMap::READABLE));
        SRecord::dump(map, stream, 4);
    }
};

class BinaryDumper: public Dumper {
public:
    virtual void formatData(std::ostream &stream, const AddressInterval &segmentInterval, const MemoryMap::Segment &segment,
                            const AddressInterval &dataInterval, const uint8_t *data) ROSE_OVERRIDE {
        stream.write((const char*)data, dataInterval.size());
        if (!stream.good()) {
            std::ostringstream mesg;
            mesg <<"write failed for virtual addresses " <<dataInterval <<" in segment " <<segmentInterval
                 <<" \"" <<StringUtility::cEscape(segment.name()) <<"\"";
            throw std::runtime_error(mesg.str());
        }
    }
};

int
main(int argc, char *argv[]) {
    // Initialization
    Diagnostics::initialize();
    mlog = Sawyer::Message::Facility("tool", Diagnostics::destination);
    Diagnostics::mfacilities.insertAndAdjust(mlog);

    // Parse command-line
    Settings settings;
    std::vector<std::string> specimenNames = parseCommandLine(argc, argv, settings).unreachedArgs();
    if (!settings.showAsHex && !settings.showAsSRecords && !settings.showAsBinary) {
        mlog[WARN] <<"no output format selected; see --help\n";
        settings.showMap = true;
    }
    if (settings.showAsBinary && settings.outputPrefix.empty())
        throw std::runtime_error("the --prefix switch is required when --binary is specified");

    // Parse and load the specimen, but do not disassemble or partition.
    P2::Engine engine;
    MemoryMap map = engine.load(specimenNames);
    if (settings.showMap)
        map.dump(std::cout);

    std::ofstream binaryIndex;
    if (settings.showAsBinary) {
        binaryIndex.open((settings.outputPrefix + "load").c_str());
        if (!binaryIndex.good()) {
            throw std::runtime_error("cannot open binary index file \"" +
                                     StringUtility::cEscape(settings.outputPrefix) + "load\"");
        }
    }

    // Dump the output
    if (!settings.where.isEmpty()) {
        rose_addr_t va = settings.where.least();
        while (AddressInterval interval = map.atOrAfter(va).singleSegment().available()) {
            interval = interval.intersection(settings.where);
            ASSERT_forbid(interval.isEmpty());
            MemoryMap::ConstNodeIterator inode = map.at(interval.least()).nodes().begin();
            const AddressInterval &segmentInterval = inode->key();
            const MemoryMap::Segment &segment = inode->value();
            mlog[WHERE] <<"dumping segment " <<segmentInterval <<" \"" <<StringUtility::cEscape(segment.name()) <<"\"\n";

            if (settings.outputPrefix.empty()) {
                if (settings.showAsHex)
                    HexDumper()(settings, map, interval, std::cout);
                if (settings.showAsSRecords)
                    SRecordDumper()(settings, map, interval, std::cout);
                ASSERT_forbid2(settings.showAsBinary, "binary output is never emitted to standard output");
            } else {
                std::string outputName = settings.outputPrefix + StringUtility::addrToString(interval.least()).substr(2);
                if (settings.showAsHex) {
                    std::ofstream output((outputName+".txt").c_str());
                    if (!output.good())
                        throw std::runtime_error("cannot create \"" + outputName + ".txt\"");
                    HexDumper()(settings, map, interval, output);
                }
                if (settings.showAsSRecords) {
                    std::ofstream output((outputName+".srec").c_str());
                    if (!output.good())
                        throw std::runtime_error("cannot create \"" + outputName + ".srec\"");
                    SRecordDumper()(settings, map, interval, output);
                }
                if (settings.showAsBinary) {
                    std::ofstream output((outputName+".raw").c_str());
                    if (!output.good())
                        throw std::runtime_error("cannot create \"" + outputName + ".raw\"");
                    BinaryDumper()(settings, map, interval, output);
                    binaryIndex <<"# Segment " <<segmentInterval <<" \"" <<StringUtility::cEscape(segment.name()) <<"\"\n"
                                <<"map:" <<StringUtility::addrToString(segmentInterval.least())
                                <<"+" <<StringUtility::addrToString(segmentInterval.size())
                                <<"=" <<(0!=(segment.accessibility() & MemoryMap::READABLE) ? "r" : "")
                                <<(0!=(segment.accessibility() & MemoryMap::WRITABLE) ? "w" : "")
                                <<(0!=(segment.accessibility() & MemoryMap::EXECUTABLE) ? "x" : "")
                                <<"::" <<outputName <<".raw\n";
                }
            }
            if (interval.greatest() == settings.where.greatest())
                break;                                  // to prevent possible overflow
            va = interval.greatest() + 1;
        }
    }
}
