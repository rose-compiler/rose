static const char *purpose = "shows stack deltas";
static const char *description =
    "Given a BAT state for a binary specimen, list all function or instruction stack deltas present in that state. The name "
    "of the state file is specified as a command argument, and standard input is used if the name is \"-\" (a single hpyhen) "
    "or unspecified.";

#include <rose.h>
#include <CommandLine.h>                                // rose
#include <Diagnostics.h>                                // rose
#include <Partitioner2/Engine.h>                        // rose
#include <Partitioner2/Partitioner.h>                   // rose

#include <batSupport.h>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <fstream>
#include <iostream>
#include <Sawyer/CommandLine.h>
#include <Sawyer/Stopwatch.h>
#include <string>
#include <vector>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;
namespace BaseSemantics = Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics;

namespace {

Sawyer::Message::Facility mlog;

enum StackDeltaDomain {
    SD_FUNCTION,                                        // show stack deltas per function
    SD_BASIC_BLOCK,                                     // show stack deltas per basic block
    SD_INSTRUCTION                                      // show stack deltas per instruction
};

struct Settings {
    StackDeltaDomain domain;
    bool emitNan;
    SerialIo::Format stateFormat;
    Settings(): domain(SD_INSTRUCTION), emitNan(true), stateFormat(SerialIo::BINARY) {}
};

struct StackDeltaPair {
    Sawyer::Optional<boost::int64_t> preDelta, postDelta;

    StackDeltaPair() {}
    StackDeltaPair(boost::int64_t preDelta, boost::int64_t postDelta)
        : preDelta(preDelta), postDelta(postDelta) {}
    bool operator==(const StackDeltaPair &other) const {
        return preDelta.isEqual(other.preDelta) && postDelta.isEqual(other.postDelta);
    }
};

typedef Sawyer::Container::IntervalMap<AddressInterval, StackDeltaPair> StackDeltaMap;

// Parses the command-line and returns the name of the input file, if any.
boost::filesystem::path
parseCommandLine(int argc, char *argv[], Settings &settings) {
    using namespace Sawyer::CommandLine;

    SwitchGroup generic = Rose::CommandLine::genericSwitches();
    generic.insert(Bat::stateFileFormatSwitch(settings.stateFormat));

    SwitchGroup output("Output switches");
    output.insert(Switch("domain")
                  .argument("entity", enumParser(settings.domain)
                            ->with("function", SD_FUNCTION)
                            ->with("basic-block", SD_BASIC_BLOCK)
                            ->with("instruction", SD_INSTRUCTION))
                  .doc("Specifies the resolution for printing results. This tool can show pre- and post-deltas for "
                       "functions as a whole, basic-blocks as a whole, or instructions.  All domains use the same "
                       "output format, namely an interval's least and greatest address and the pre- and post-deltas "
                       "for the stack pointer. Deltas are always relative to the value of the stack pointer at the "
                       "beginning of a function.  The default domain is \"" +
                       std::string(SD_FUNCTION==settings.domain ? "function" :
                                   (SD_BASIC_BLOCK==settings.domain ? "basic-block" : "instruction")) + "\"."));

    Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
    parser.errorStream(mlog[FATAL]);
    parser.with(generic);
    parser.with(output);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] [@v{BAT-input}]");

    std::vector<std::string> input = parser.parse(argc, argv).apply().unreachedArgs();
    if (input.size() > 1) {
        mlog[FATAL] <<"incorrect usage; see --help\n";
        exit(1);
    }
    return input.empty() ? std::string("-") : input[0];
}

boost::int64_t
toSigned(const BaseSemantics::SValuePtr &expr) {
    ASSERT_not_null(expr);
    ASSERT_require(expr->is_number());
    ASSERT_require(expr->get_width() <= 64);
    return IntegerOps::signExtend2<boost::uint64_t>(expr->get_number(), expr->get_width(), 64);
}

void
insertDeltaPairs(StackDeltaMap &sdmap /*in,out*/, const AddressIntervalSet &intervals,
                 const BaseSemantics::SValuePtr &preDelta, const BaseSemantics::SValuePtr &postDelta) {
    StackDeltaPair sdpair;
    if (preDelta && preDelta->is_number() && preDelta->get_width() <= 64)
        sdpair.preDelta = toSigned(preDelta);
    if (postDelta && postDelta->is_number() && postDelta->get_width() <= 64)
        sdpair.postDelta = toSigned(postDelta);

    if (sdpair.preDelta || sdpair.postDelta) {
        BOOST_FOREACH (const AddressInterval &interval, intervals.intervals())
            sdmap.insert(interval, sdpair);
    }
}

void
insertDeltaPairs(StackDeltaMap &sdmap /*in,out*/, const AddressInterval &interval,
                 const BaseSemantics::SValuePtr &preDelta, const BaseSemantics::SValuePtr &postDelta) {
    StackDeltaPair sdpair;
    if (preDelta && preDelta->is_number() && preDelta->get_width() <= 64)
        sdpair.preDelta = toSigned(preDelta);
    if (postDelta && postDelta->is_number() && postDelta->get_width() <= 64)
        sdpair.postDelta = toSigned(postDelta);

    if (sdpair.preDelta || sdpair.postDelta)
        sdmap.insert(interval, sdpair);
}

void
insertDeltaPairs(StackDeltaMap &sdmap /*in,out*/, const AddressIntervalSet &intervals,
                 const BaseSemantics::SValuePtr &postDelta) {
    StackDeltaPair sdpair;
    sdpair.preDelta = 0;
    if (postDelta && postDelta->is_number() && postDelta->get_width() <= 64)
        sdpair.postDelta = toSigned(postDelta);

    BOOST_FOREACH (const AddressInterval &interval, intervals.intervals())
        sdmap.insert(interval, sdpair);
}
    
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");
    mlog.comment("analyzing stack pointer behavior");
    Bat::checkRoseVersionNumber(MINIMUM_ROSE_LIBRARY_VERSION, mlog[FATAL]);
    Bat::registerSelfTests();

    Settings settings;
    boost::filesystem::path inputFileName = parseCommandLine(argc, argv, settings);
    P2::Engine engine;
    P2::Partitioner partitioner = engine.loadPartitioner(inputFileName, settings.stateFormat);

    // Accumulate deltas in a map
    Sawyer::Stopwatch timer;
    mlog[INFO] <<"calculating stack deltas";
    StackDeltaMap sdmap;
    BOOST_FOREACH (const P2::Function::Ptr &function, partitioner.functions()) {
        if (SD_FUNCTION == settings.domain) {
            BaseSemantics::SValuePtr postDelta = partitioner.functionStackDelta(function);
            insertDeltaPairs(sdmap, partitioner.functionBasicBlockExtent(function), postDelta);
        } else {
            BOOST_FOREACH (const rose_addr_t bbva, function->basicBlockAddresses()) {
                if (P2::BasicBlock::Ptr bb = partitioner.basicBlockExists(bbva)) {
                    if (SD_BASIC_BLOCK == settings.domain) {
                        BaseSemantics::SValuePtr preDelta = partitioner.basicBlockStackDeltaIn(bb, function);
                        BaseSemantics::SValuePtr postDelta = partitioner.basicBlockStackDeltaOut(bb, function);
                        insertDeltaPairs(sdmap, partitioner.basicBlockInstructionExtent(bb), preDelta, postDelta);
                    } else {
                        ASSERT_require(SD_INSTRUCTION == settings.domain);
                        const BinaryAnalysis::StackDelta::Analysis &sda = function->stackDeltaAnalysis();
                        BOOST_FOREACH (SgAsmInstruction *insn, bb->instructions()) {
                            BaseSemantics::SValuePtr preDelta = sda.instructionInputStackDeltaWrtFunction(insn);
                            BaseSemantics::SValuePtr postDelta = sda.instructionOutputStackDeltaWrtFunction(insn);
                            insertDeltaPairs(sdmap, partitioner.instructionExtent(insn), preDelta, postDelta);
                        }
                    }
                }
            }
        }
    }
    mlog[INFO] <<"; took " <<timer <<" seconds\n";
    
    // Print results
    BOOST_FOREACH (const StackDeltaMap::Node &node, sdmap.nodes()) {
        std::cout <<StringUtility::addrToString(node.key().least()) <<"\t"
                  <<StringUtility::addrToString(node.key().greatest()) <<"\t";
        if (node.value().preDelta) {
            std::cout <<*node.value().preDelta <<"\t";
        } else {
            std::cout <<"nan\t";
        }
        if (node.value().postDelta) {
            std::cout <<*node.value().postDelta <<"\n";
        } else {
            std::cout <<"nan\n";
        }
    }
}
