static const char *purpose = "analyze a binary specimen";
static const char *description =
    "This tool loads and parses a binary specimen, then disassembles its instructions and partitions them into "
    "basic blocks, functions, and data.  Various analyses are performed during the partitioning and additional analysis "
    "are performed after partitioning. The specimen can be binary containers such as ELF or PE, raw files "
    "such as memory dumps, encoded binary data such as Motorola S-Records, a running process, a process paused "
    "in a debugger, or an executable that can be run natively. See the \"Specimens\" section for details.  This tool "
    "is cross platform, allowing specimens to be disassemble which are not native to the platform on which this tool "
    "is running.\n\n"

    "The output from this tool is a representation of the tool's final state, which can be used as input for a variety "
    "of other tools.";

#include <rose.h>
#include <batSupport.h>

#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/BestMapAddress.h>
#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/CommandLine.h>
#include <Rose/Diagnostics.h>

#include <Sawyer/Stopwatch.h>

#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace Rose;
using namespace Sawyer::Message::Common;
using namespace Rose::BinaryAnalysis;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

namespace {

Sawyer::Message::Facility mlog;

// Tool-specific command-line settings
struct Settings {
    boost::filesystem::path outputFileName;
    SerialIo::Format stateFormat;
    bool doRemap;
    bool skipOutput;

    Settings(): outputFileName("-"), stateFormat(SerialIo::BINARY), doRemap(false), skipOutput(false) {}
};

// Build a command line parser without running it
Sawyer::CommandLine::Parser
buildSwitchParser(Settings &settings) {
    using namespace Sawyer::CommandLine;

    SwitchGroup tool("Tool specific switches");
    tool.name("tool");

    CommandLine::insertBooleanSwitch(tool, "remap", settings.doRemap,
                                     "Attempt to rearrange the executable segments of the original memory map to create a new map "
                                     "that maximizes the number of function call targets that line up with function entry addresses. "
                                     "This is normally most useful for specimens that have no address information, as is often the "
                                     "case with firmware where the boot loader, which is perhaps not available during analysis, is "
                                     "responsible for choosing the final addresses.");

    CommandLine::insertBooleanSwitch(tool, "skip-output", settings.skipOutput,
                                     "Skip the output step even if @s{output} is specified. This is mainly for performance testing.");

    SwitchGroup output("Output switches");
    output.insert(Switch("output", 'o')
                  .argument("filename", anyParser(settings.outputFileName))
                  .doc("Send binary state information to the specified file instead of standard output. The output becomes "
                       "the input for subsequent binary analysis tools. This tool will refuse to write binary data to standard "
                       "output if it appears to be the terminal; if you really need that, pipe this tool's output through "
                       "@man{cat}{1}.  Specifying the output name \"-\" (a single hyphen) is the explicit way of saying that "
                       "output should be sent to standard output."));

    output.insert(Bat::stateFileFormatSwitch(settings.stateFormat));

    Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] @v{specimen}");
    parser.errorStream(mlog[FATAL]);
    parser.with(Rose::CommandLine::genericSwitches());
    parser.with(tool);
    parser.with(output);

    return parser;
}

// Rewrites the global CFG as the program is disassembled.
class IpRewrite: public P2::BasicBlockCallback {
public:
    typedef std::pair<P2::Semantics::SValue::Ptr, P2::Semantics::SValue::Ptr> SValueSValue;
private:
    std::vector<SValueSValue> rewrites_;

protected:
    IpRewrite(const P2::Partitioner::ConstPtr &partitioner, const std::vector<rose_addr_t> &vaPairs) {
        ASSERT_require(vaPairs.size() % 2 == 0);
        const RegisterDescriptor REG_IP = partitioner->instructionProvider().instructionPointerRegister();
        InstructionSemantics::BaseSemantics::RiscOperators::Ptr ops = partitioner->newOperators();
        for (size_t i=0; i < vaPairs.size(); i += 2) {
            P2::Semantics::SValue::Ptr oldVal = P2::Semantics::SValue::promote(ops->number_(REG_IP.nBits(), vaPairs[i+0]));
            P2::Semantics::SValue::Ptr newVal = P2::Semantics::SValue::promote(ops->number_(REG_IP.nBits(), vaPairs[i+1]));
            rewrites_.push_back(std::make_pair(oldVal, newVal));
        }
    }

public:
    static Ptr instance(const P2::Partitioner::ConstPtr &partitioner, const std::vector<rose_addr_t> &vaPairs) {
        return Ptr(new IpRewrite(partitioner, vaPairs));
    }

    virtual bool operator()(bool chain, const Args &args) {
        if (chain && !rewrites_.empty()) {
            std::vector<P2::BasicBlock::Successor> succs;
            if (args.bblock->successors().isCached())
                succs = args.bblock->successors().get();
            bool modified = false;
            for (size_t i=0; i<succs.size(); ++i) {
                for (const SValueSValue &rewrite: rewrites_) {
                    if (succs[i].expr()->mustEqual(rewrite.first)) {
                        succs[i] = P2::BasicBlock::Successor(rewrite.second, succs[i].type(), succs[i].confidence());
                        modified = true;
                        break;
                    }
                }
            }
            if (modified)
                args.bblock->successors() = succs;
        }
        return chain;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");
    mlog.comment("pre-analyzing binary specimens");
    try {
        Bat::checkRoseVersionNumber(MINIMUM_ROSE_LIBRARY_VERSION, mlog[FATAL]);
        Bat::registerSelfTests();

        // Build command line parser (without any knowledge of engine settings), choose the appropriate partitioner engine type, and
        // parse and apply the command-line to the settings, engine, etc. using the appropriately modified command-line parser.
        Settings settings;
        auto parser = buildSwitchParser(settings);
        P2::Engine::Ptr engine = P2::Engine::forge(argc, argv, parser/*in,out*/);
        std::vector<std::string> specimen = parser.parse(argc, argv).apply().unreachedArgs();
        mlog[INFO] <<"using the " <<engine->name() <<" partitioning engine\n";

        if (specimen.empty()) {
            mlog[FATAL] <<"no binary specimen specified; see --help\n";
            exit(1);
        }

        Bat::checkRbaOutput(settings.outputFileName, mlog);

        MemoryMap::Ptr map = engine->loadSpecimens(specimen);
        map->dump(mlog[INFO]);

        //TODO: should be skipped for JVM (throw error if jvm)
        if (settings.doRemap) {
            P2::Engine::Settings settings = engine->settings();
            settings.partitioner.doingPostAnalysis = false;
            if (settings.disassembler.isaName.empty()) {
                Disassembler::Base::Ptr disassembler = engine->architecture()->newInstructionDecoder();
                if (!disassembler) {
                    mlog[FATAL] <<"no disassembler found and none specified\n";
                    exit(1);
                }
                settings.disassembler.isaName = disassembler->name();
            }
            MemoryMap::Ptr newMap = BestMapAddress::align(map, settings);
            mlog[INFO] <<"Remapped addresses:\n";
            newMap->dump(mlog[INFO]);
            engine->memoryMap(newMap);
        }

        P2::Partitioner::Ptr partitioner;
        if (engine->settings().disassembler.doDisassemble) {
            const std::string archName = engine->architecture()->name(); // temp var so exceptions don't interfere with output
            mlog[INFO] <<"using the " <<archName <<" architecture\n";
            partitioner = engine->partition(specimen);
        } else {
            partitioner = engine->createPartitioner();
            engine->runPartitionerInit(partitioner);
            engine->runPartitionerFinal(partitioner);
        }

    #if 0 // DEBUGGING [Robb Matzke 2018-10-24]
        partitioner->showStatistics();                      // debugging
    #endif

        if (!settings.skipOutput) {
            partitioner->basicBlockDropSemantics();
            partitioner->saveAsRbaFile(settings.outputFileName, settings.stateFormat);
        }
    } catch (const std::exception &e) {
        mlog[FATAL] <<e.what() <<"\n";
        exit(1);
    }
}
