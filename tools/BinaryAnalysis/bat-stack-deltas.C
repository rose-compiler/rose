static const char *purpose = "shows stack deltas";
static const char *description =
    "Prints information from the stack delta analysis, which attempts to determine how the stack pointer varies "
    "throughout the execution of a function, and how the frame pointer (if any) varies with respect to the stack "
    "pointer.";

#include <batSupport.h>

#include <Rose/BinaryAnalysis/AddressInterval.h>
#include <Rose/BinaryAnalysis/AddressIntervalSet.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/CommandLine.h>
#include <Rose/Diagnostics.h>
#include <Rose/Initialize.h>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;
namespace BaseSemantics = Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics;

namespace {

Sawyer::Message::Facility mlog;

enum StackDeltaDomain {
    SD_FUNCTION,                                        // show stack deltas per function
    SD_BASIC_BLOCK,                                     // show stack deltas per basic block
    SD_INSTRUCTION                                      // show stack deltas per instruction
};

struct Settings {
    StackDeltaDomain domain = SD_INSTRUCTION;
    bool emitNan = true;
    SerialIo::Format stateFormat = SerialIo::BINARY;
    std::set<std::string> functionNames;
    std::set<Address> addresses;
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

Sawyer::CommandLine::Parser
createSwitchParser(Settings &settings) {
    using namespace Sawyer::CommandLine;

    SwitchGroup sel("Selection switches");
    sel.name("sel");

    sel.insert(Switch("function", 'f')
               .argument("name_or_address", listParser(anyParser(settings.functionNames), ","))
               .explosiveLists(true)
               .whichValue(SAVE_ALL)
               .doc("Restricts output to the specified functions. The @v{name_or_address} can be the name of a function as "
                    "a string or the entry address for the function as a decimal, octal, hexadecimal or binary number. "
                    "If a value is ambiguous, it's first treated as a name and if no function has that name it's then "
                    "treated as an address. This switch may occur multiple times and multiple comma-separated values may "
                    "be specified per occurrence."));

    sel.insert(Switch("containing", 'a')
               .argument("addresses", listParser(nonNegativeIntegerParser(settings.addresses), ","))
               .explosiveLists(true)
               .whichValue(SAVE_ALL)
               .doc("Restricts output to functions that contain one of the specified addresses. This switch may occur "
                    "multiple times and multiple comma-separate addresses may be specified per occurrence."));

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

    SwitchGroup generic = Rose::CommandLine::genericSwitches();
    generic.insert(Bat::stateFileFormatSwitch(settings.stateFormat));

    Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
    parser.errorStream(mlog[FATAL]);
    parser.with(sel);
    parser.with(output);
    parser.with(generic);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] [@v{specimen}]");
    return parser;
}

std::vector<std::string>
parseCommandLine(int argc, char *argv[], Sawyer::CommandLine::Parser &parser) {
    std::vector<std::string> specimen = parser.parse(argc, argv).apply().unreachedArgs();
    if (specimen.empty())
        specimen.push_back("-");
    return specimen;
}

void
insertDeltaPairs(StackDeltaMap &sdmap /*in,out*/, const AddressIntervalSet &intervals,
                 const BaseSemantics::SValue::Ptr &preDelta, const BaseSemantics::SValue::Ptr &postDelta) {
    StackDeltaPair sdpair;
    if (preDelta)
        preDelta->toSigned().assignTo(sdpair.preDelta);
    if (postDelta)
        postDelta->toSigned().assignTo(sdpair.postDelta);

    if (sdpair.preDelta || sdpair.postDelta) {
        for (const AddressInterval &interval: intervals.intervals())
            sdmap.insert(interval, sdpair);
    }
}

void
insertDeltaPairs(StackDeltaMap &sdmap /*in,out*/, const AddressInterval &interval,
                 const BaseSemantics::SValue::Ptr &preDelta, const BaseSemantics::SValue::Ptr &postDelta) {
    StackDeltaPair sdpair;
    if (preDelta)
        preDelta->toSigned().assignTo(sdpair.preDelta);
    if (postDelta)
        postDelta->toSigned().assignTo(sdpair.postDelta);

    if (sdpair.preDelta || sdpair.postDelta)
        sdmap.insert(interval, sdpair);
}

void
insertDeltaPairs(StackDeltaMap &sdmap /*in,out*/, const AddressIntervalSet &intervals,
                 const BaseSemantics::SValue::Ptr &postDelta) {
    StackDeltaPair sdpair;
    sdpair.preDelta = 0;
    if (postDelta)
        postDelta->toSigned().assignTo(sdpair.postDelta);

    for (const AddressInterval &interval: intervals.intervals())
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
    Sawyer::CommandLine::Parser switchParser = createSwitchParser(settings);
    auto engine = P2::Engine::forge(argc, argv, switchParser /*in,out*/);
    const std::vector<std::string> specimen = parseCommandLine(argc, argv, switchParser);

    P2::Partitioner::Ptr partitioner;
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

    // Select the functions to analyze
    std::vector<P2::Function::Ptr> selectedFunctions;
    if (!settings.functionNames.empty() || !settings.addresses.empty()) {
        selectedFunctions = Bat::selectFunctionsByNameOrAddress(partitioner->functions(), settings.functionNames, mlog[WARN]);
        for (const P2::Function::Ptr &f: Bat::selectFunctionsContainingInstruction(partitioner, settings.addresses))
            P2::insertUnique(selectedFunctions, f, P2::sortFunctionsByAddress);
    } else {
        selectedFunctions = partitioner->functions();
    }
    if (selectedFunctions.empty())
        mlog[WARN] <<"no matching functions found\n";

    // Accumulate deltas in a map
    Sawyer::Stopwatch timer;
    mlog[INFO] <<"calculating stack deltas";
    StackDeltaMap sdmap;
    for (const P2::Function::Ptr &function: selectedFunctions) {
        partitioner->functionStackDelta(function);
        if (SD_FUNCTION == settings.domain) {
            BaseSemantics::SValue::Ptr postDelta = partitioner->functionStackDelta(function);
            insertDeltaPairs(sdmap, partitioner->functionBasicBlockExtent(function), postDelta);
        } else {
            for (const Address bbva: function->basicBlockAddresses()) {
                if (P2::BasicBlock::Ptr bb = partitioner->basicBlockExists(bbva)) {
                    if (SD_BASIC_BLOCK == settings.domain) {
                        BaseSemantics::SValue::Ptr preDelta = partitioner->basicBlockStackDeltaIn(bb, function);
                        BaseSemantics::SValue::Ptr postDelta = partitioner->basicBlockStackDeltaOut(bb, function);
                        insertDeltaPairs(sdmap, partitioner->basicBlockInstructionExtent(bb), preDelta, postDelta);
                    } else {
                        ASSERT_require(SD_INSTRUCTION == settings.domain);
                        const BinaryAnalysis::StackDelta::Analysis &sda = function->stackDeltaAnalysis();
                        for (SgAsmInstruction *insn: bb->instructions()) {
                            BaseSemantics::SValue::Ptr preDelta = sda.instructionInputStackDeltaWrtFunction(insn);
                            BaseSemantics::SValue::Ptr postDelta = sda.instructionOutputStackDeltaWrtFunction(insn);
                            insertDeltaPairs(sdmap, partitioner->instructionExtent(insn), preDelta, postDelta);
                        }
                    }
                }
            }
        }
    }
    mlog[INFO] <<"; took " <<timer <<"\n";
    
    // Print results
    for (const StackDeltaMap::Node &node: sdmap.nodes()) {
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
