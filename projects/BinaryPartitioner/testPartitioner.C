#include <rose.h>
#include <rosePublicConfig.h>
#include <DisassemblerArm.h>
#include <DisassemblerPowerpc.h>
#include <DisassemblerMips.h>
#include <DisassemblerX86.h>
#include <DisassemblerM68k.h>

#include <map>
#include <sawyer/Assert.h>
#include <sawyer/CommandLine.h>
#include <string>

#include "Partitioner2.h"

using namespace rose;
using namespace rose::BinaryAnalysis;


static Disassembler *
getDisassembler(const std::string &name)
{
    if (0==name.compare("list")) {
        std::cout <<"The following ISAs are supported:\n"
                  <<"  amd64\n"
                  <<"  arm\n"
                  <<"  coldfire\n"
                  <<"  i386\n"
                  <<"  m68040\n"
                  <<"  mips\n"
                  <<"  ppc\n";
        exit(0);
    } else if (0==name.compare("arm")) {
        return new DisassemblerArm();
    } else if (0==name.compare("ppc")) {
        return new DisassemblerPowerpc();
    } else if (0==name.compare("mips")) {
        return new DisassemblerMips();
    } else if (0==name.compare("i386")) {
        return new DisassemblerX86(4);
    } else if (0==name.compare("amd64")) {
        return new DisassemblerX86(8);
    } else if (0==name.compare("m68040")) {
        return new DisassemblerM68k(m68k_68040);
    } else if (0==name.compare("coldfire")) {
        return new DisassemblerM68k(m68k_freescale_emacb);
    } else {
        throw std::runtime_error("invalid ISA name \""+name+"\"; use --isa=list");
    }
}

// Convenient struct to hold settings from the command-line all in one place.
struct Settings {
    std::string isaName;
    rose_addr_t startVa;
    Settings(): startVa(0) {}
};

// Describe and parse the command-line
static Sawyer::CommandLine::ParserResult
parseCommandLine(int argc, char *argv[], Settings &settings)
{
    using namespace Sawyer::CommandLine;
    SwitchGroup switches;
    switches.insert(Switch("help", 'h')
                    .doc("Show this documentation.")
                    .action(showHelpAndExit(0)));
    switches.insert(Switch("isa")
                    .argument("architecture", anyParser(settings.isaName))
                    .doc("Instruction set architecture. Specify \"list\" to see a list of possible ISAs."));
    switches.insert(Switch("at")
                    .argument("virtual-address", nonNegativeIntegerParser(settings.startVa))
                    .doc("The first byte of the file is mapped at the specified @v{virtual-address}, which defaults "
                         "to " + StringUtility::addrToString(settings.startVa) + "."));
    switches.insert(Switch("version", 'V')
                    .action(showVersionAndExit(version_message(), 0))
                    .doc("Shows version information for various ROSE components and then exits."));

    Parser parser;
    parser
        .purpose("tests new partitioner architecture")
        .doc("synopsis",
             "@prop{programName} [@v{switches}] @v{specimen_name}")
        .doc("description",
             "This program tests the new partitioner architecture by disassembling the specified raw file.");
    
    return parser.with(switches).parse(argc, argv).apply();
}

int main(int argc, char *argv[])
{
    // Parse the command-line
    Settings settings;
    Sawyer::CommandLine::ParserResult cmdline = parseCommandLine(argc, argv, settings);
    std::vector<std::string> positionalArgs = cmdline.unreachedArgs();

    // Obtain a disassembler (do this before opening the specimen so "--isa=list" has a chance to run)
    Disassembler *disassembler = getDisassembler(settings.isaName);
    ASSERT_not_null(disassembler);

    // Open the file that needs to be disassembled and map it with read and execute permission
    if (positionalArgs.empty())
        throw std::runtime_error("no file name specified; see --help");
    if (positionalArgs.size()>1)
        throw std::runtime_error("too many files specified; see --help");
    std::string specimenName = positionalArgs[0];
    MemoryMap map;
    size_t nBytesMapped = map.insert_file(specimenName, settings.startVa);
    if (0==nBytesMapped)
        throw std::runtime_error("problem reading file: " + specimenName);
    map.mprotect(AddressInterval::baseSize(settings.startVa, nBytesMapped), MemoryMap::MM_PROT_RX);
    map.dump(std::cerr);                                // debugging so the user can see the map


    // Create the partitioner
    namespace P2 = Partitioner2;
    P2::Partitioner partitioner(disassembler, map);

    // Recursive disassembly
    partitioner.insertPlaceholder(settings.startVa);    // disassembly starts here
    P2::Partitioner::ControlFlowGraph::VertexNodeIterator worklist = partitioner.undiscoveredVertex();
    while (worklist->nInEdges() > 0) {
        P2::Partitioner::ControlFlowGraph::VertexNodeIterator placeholder = worklist->inEdges().begin()->source();
        P2::Partitioner::BasicBlock::Ptr bb = partitioner.discoverBasicBlock(placeholder);
        partitioner.insertBasicBlock(placeholder, bb);
    }

    exit(0);
}
