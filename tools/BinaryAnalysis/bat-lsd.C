static const char *purpose = "lists static data blocks";
static const char *description =
    "Given a BAT state for a binary specimen, list information about all of the static data blocks.";

#include <rose.h>

#include <Rose/BinaryAnalysis/Hexdump.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/DataBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/CommandLine.h>
#include <Rose/Diagnostics.h>

#include <batSupport.h>
#include <boost/algorithm/string/trim.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>
#include <Sawyer/CommandLine.h>
#include <Sawyer/Stopwatch.h>
#include <string>
#include <vector>

using namespace Rose;
using namespace Sawyer::Message::Common;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

namespace {

Sawyer::Message::Facility mlog;
Rose::BinaryAnalysis::SerialIo::Format format = Rose::BinaryAnalysis::SerialIo::BINARY;

struct Settings {
    bool printingData;                                  // print the actual data

    Settings()
        : printingData(true) {}
};

// Parses the command-line and returns the name of the input file, if any.
boost::filesystem::path
parseCommandLine(int argc, char *argv[], Settings &settings) {
    using namespace Sawyer::CommandLine;

    SwitchGroup gen = Rose::CommandLine::genericSwitches();
    gen.insert(Bat::stateFileFormatSwitch(format));

    SwitchGroup tool("Tool-specific switches");
    CommandLine::insertBooleanSwitch(tool, "data", settings.printingData,
                                     "Output the data represented by the static data block.");

    Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
    parser.errorStream(mlog[FATAL]);
    parser.with(tool);
    parser.with(gen);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] [@v{BAT-input}]");

    std::vector<std::string> input = parser.parse(argc, argv).apply().unreachedArgs();
    if (input.size() > 1) {
        mlog[FATAL] <<"incorrect usage; see --help\n";
        exit(1);
    }
    return input.empty() ? std::string("-") : input[0];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");
    mlog.comment("listing info about basic blocks");
    Bat::checkRoseVersionNumber(MINIMUM_ROSE_LIBRARY_VERSION, mlog[FATAL]);
    Bat::registerSelfTests();

    Settings settings;
    boost::filesystem::path inputFileName = parseCommandLine(argc, argv, settings);
    P2::Partitioner::Ptr partitioner;
    try {
        partitioner = P2::Partitioner::instanceFromRbaFile(inputFileName, format);
    } catch (const std::exception &e) {
        mlog[FATAL] <<"cannot load partitioner from " <<inputFileName <<": " <<e.what() <<"\n";
        exit(1);
    }

    P2::AddressUsers allUsers = partitioner->aum().overlapping(partitioner->aum().hull());
    for (const P2::AddressUser &user: allUsers.addressUsers()) {
        if (P2::DataBlock::Ptr db = user.dataBlock()) {
            // Header
            std::cout <<db->printableName() <<"\n";
            std::string comment = boost::trim_copy(db->comment());
            if (!db->comment().empty())
                std::cout <<StringUtility::prefixLines(db->comment(), "  ") <<"\n";
            std::cout <<"  type: " <<db->type()->toString() <<"\n";

            // Ownership information
            if (db->attachedFunctionOwners().empty()) {
                std::cout <<"  not owned by any functions\n";
            } else {
                for (const P2::Function::Ptr &func: db->attachedFunctionOwners())
                    std::cout <<"  owned by " <<func->printableName() <<"\n";
            }

            if (db->attachedBasicBlockOwners().empty()) {
                std::cout<<"  not owned by any basic blocks\n";
            } else {
                for (const P2::BasicBlock::Ptr &bb: db->attachedBasicBlockOwners())
                    std::cout <<"  owned by " <<bb->printableName() <<"\n";
            }

            // Data
            std::cout <<"  data: " <<StringUtility::plural(db->size(), "bytes") <<"\n";
            if (settings.printingData) {
                std::cout <<"    ";
                std::vector<uint8_t> data = db->read(partitioner->memoryMap());
                BinaryAnalysis::HexdumpFormat fmt;
                fmt.prefix = "    ";
                BinaryAnalysis::hexdump(std::cout, db->address(), &data[0], db->size(), fmt);
                std::cout <<"\n";
            }
        }
    }
}
