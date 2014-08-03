#include <rose.h>
#include <rosePublicConfig.h>
#include <DisassemblerArm.h>
#include <DisassemblerPowerpc.h>
#include <DisassemblerMips.h>
#include <DisassemblerX86.h>
#include <DisassemblerM68k.h>

#include <SymbolicSemantics2.h>
#include <TraceSemantics2.h>
#include <DispatcherM68k.h>

#include <map>
#include <sawyer/Assert.h>
#include <sawyer/CommandLine.h>
#include <string>

using namespace rose;
using namespace rose::BinaryAnalysis::InstructionSemantics2;
using namespace rose::BinaryAnalysis;

// Round X up to the next multiple of ALIGNMENT
static rose_addr_t
alignUp(rose_addr_t x, rose_addr_t alignment)
{
    return alignment>1 ? ((x+alignment-1)/alignment)*alignment : x;
}

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
    rose_addr_t alignment;
    Settings(): startVa(0), alignment(1) {}
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
    switches.insert(Switch("alignment")
                    .argument("align", nonNegativeIntegerParser(settings.alignment))
                    .doc("Alignment for instructions.  The default is 1 (no alignment).  Values larger than one will "
                         "cause each candidate address to be rounded up to the next multiple of @v{align}.  If this "
                         "rounding up causes addresses after a valid instruction to be skipped then a warning is printed. "
                         "No warning is printed if the alignment skips addresses after a disassembly failure."));
    switches.insert(Switch("version", 'V')
                    .action(showVersionAndExit(version_message(), 0))
                    .doc("Shows version information for various ROSE components and then exits."));

    Parser parser;
    parser
        .purpose("disassembles files one address at a time")
        .doc("synopsis",
             "@prop{programName} [@v{switches}] @v{specimen_name}")
        .doc("description",
             "This program is a very simple disassembler that tries to disassemble an instruction at each address of "
             "the specimen file.");
    
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
    disassembler->set_protection(MemoryMap::MM_PROT_READ); // we map the file read-only, so disassemble that part

    // Open the file that needs to be disassembled
    if (positionalArgs.empty())
        throw std::runtime_error("no file name specified; see --help");
    if (positionalArgs.size()>1)
        throw std::runtime_error("too many files specified; see --help");
    std::string specimenName = positionalArgs[0];
    MemoryMap map;
    if (!map.insert_file(specimenName, settings.startVa))
        throw std::runtime_error("problem reading file: " + specimenName);
    map.dump(std::cerr);                                // debugging so the user can see the map

    // Obtain an unparser suitable for this disassembler
    AsmUnparser unparser;
    unparser.set_registers(disassembler->get_registers());

#if 0 // [Robb P. Matzke 2014-07-29]
    BaseSemantics::RiscOperatorsPtr ops = SymbolicSemantics::RiscOperators::instance(disassembler->get_registers());
    ops = TraceSemantics::RiscOperators::instance(ops);
    BaseSemantics::DispatcherPtr dispatcher = DispatcherM68k::instance(ops);
    dispatcher->get_state()->get_memory_state()->set_byteOrder(ByteOrder::ORDER_MSB);
#endif

    // Disassemble at each valid address, and show disassembly errors
    rose_addr_t va = 0;
    while (map.next(va).assignTo(va)) {
        va = alignUp(va, settings.alignment);
        try {
            SgAsmInstruction *insn = disassembler->disassembleOne(&map, va);
            ASSERT_not_null(insn);
            unparser.unparse(std::cout, insn);

#if 0 // [Robb P. Matzke 2014-07-29]
            if (SgAsmM68kInstruction *insnM68k = isSgAsmM68kInstruction(insn)) {
                switch (insnM68k->get_kind()) {
                    case m68k_cpusha:
                    case m68k_cpushl:
                    case m68k_cpushp:
                        std::cout <<"    No semantics yet for privileged instructions\n";
                        break;
                        
                    case m68k_fbeq:
                    case m68k_fbne:
                    case m68k_fboge:
                    case m68k_fbogt:
                    case m68k_fbule:
                    case m68k_fbult:
                    case m68k_fcmp:
                    case m68k_fdabs:
                    case m68k_fdadd:
                    case m68k_fddiv:
                    case m68k_fdiv:
                    case m68k_fdmove:
                    case m68k_fdmul:
                    case m68k_fdneg:
                    case m68k_fdsqrt:
                    case m68k_fdsub:
                    case m68k_fintrz:
                    case m68k_fmove:
                    case m68k_fmovem:
                    case m68k_fsadd:
                    case m68k_fsdiv:
                    case m68k_fsmove:
                    case m68k_fsmul:
                    case m68k_fsneg:
                    case m68k_fssub:
                    case m68k_ftst:
                        std::cout <<"    No semantics yet for floating-point instructions\n";
                        break;

                    case m68k_nbcd:
                    case m68k_rtm:
                    case m68k_movep:
                        std::cout <<"    No semantics yet for this odd instruction\n";
                        break;

                    default:
                        ops->get_state()->clear();
                        dispatcher->processInstruction(insn);
                        std::ostringstream ss;
                        ss <<*dispatcher->get_state();
                        std::cout <<StringUtility::prefixLines(ss.str(), "    ") <<"\n";
                        break;
                }
            }
#endif

            va += insn->get_size();
            if (0 != va % settings.alignment)
                std::cerr <<StringUtility::addrToString(va) <<": invalid alignment\n";
#if 0 // [Robb P. Matzke 2014-06-19]: broken
            deleteAST(insn);
#endif
        } catch (const Disassembler::Exception &e) {
            std::cerr <<StringUtility::addrToString(va) <<": " <<e.what() <<"\n";
            ++va;
        }
    }

    exit(0);
}
