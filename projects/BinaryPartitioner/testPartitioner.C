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
using namespace rose::Diagnostics;

namespace P2 = Partitioner2;

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
    rose_addr_t disassembleVa;
    Settings(): startVa(0), disassembleVa(0) {}
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
    switches.insert(Switch("log")
                    .action(configureDiagnostics("log", Sawyer::Message::mfacilities))
                    .argument("config")
                    .whichValue(SAVE_ALL)
                    .doc("Configures diagnostics.  Use \"@s{log}=help\" and \"@s{log}=list\" to get started."));
    switches.insert(Switch("version", 'V')
                    .action(showVersionAndExit(version_message(), 0))
                    .doc("Shows version information for various ROSE components and then exits."));
    switches.insert(Switch("disassemble")
                    .argument("virtual-address", nonNegativeIntegerParser(settings.disassembleVa))
                    .doc("Address where disassembly starts.  The default is to use the first address of the file."));

    Parser parser;
    parser
        .purpose("tests new partitioner architecture")
        .doc("synopsis",
             "@prop{programName} [@v{switches}] @v{specimen_name}")
        .doc("description",
             "This program tests the new partitioner architecture by disassembling the specified raw file.");
    
    return parser.with(switches).parse(argc, argv).apply();
}

// Example of watching CFG changes
class CfgChangeWatcher: public P2::Partitioner::CfgAdjustmentCallback {
public:
    static Ptr instance() { return Ptr(new CfgChangeWatcher); }
    virtual bool operator()(bool enabled, const InsertionArgs &args) {
        std::cerr <<"+";
        if (P2::Partitioner::BasicBlock::Ptr bb = args.insertedVertex->value().bblock())
            std::cerr <<std::string(bb->nInsns(), '.');
        return enabled;
    }
    virtual bool operator()(bool enabled, const ErasureArgs &args) {
        std::cerr <<"-" <<std::string(args.erasedBlock->nInsns(), '.');
        return enabled;
    }
};

// Looks for m68k instruction prologues containing a LINK.W instruction whose first argument is A6
class MatchM68kLink: public P2::Partitioner::FunctionPrologueMatcher {
protected:
    rose_addr_t functionVa_;
public:
    MatchM68kLink(): functionVa_(0) {}
    static Ptr instance() { return Ptr(new MatchM68kLink); }
    virtual rose_addr_t functionVa() const {
        return functionVa_;
    }

    virtual bool match(P2::Partitioner *partitioner, rose_addr_t anchor) /*override*/ {
        static const RegisterDescriptor REG_A6(m68k_regclass_addr, 6, 0, 32);
        if (SgAsmM68kInstruction *insn = isSgAsmM68kInstruction(partitioner->discoverInstruction(anchor))) {
            const SgAsmExpressionPtrList &args = insn->get_operandList()->get_operands();
            if (insn->get_kind()==m68k_link && args.size()==2) {
                if (SgAsmDirectRegisterExpression *rre = isSgAsmDirectRegisterExpression(args[0])) {
                    if (rre->get_descriptor()==REG_A6) {
                        functionVa_ = anchor;
                        return true;
                    }
                }
            }
        }
        return false;
    }
};

class MatchX86Prologue: public P2::Partitioner::FunctionPrologueMatcher {
protected:
    rose_addr_t functionVa_;
public:
    MatchX86Prologue(): functionVa_(0) {}
    static Ptr instance() { return Ptr(new MatchX86Prologue); }
    virtual rose_addr_t functionVa() const {
        return functionVa_;
    }

    virtual bool match(P2::Partitioner *partitioner, rose_addr_t anchor) /*override*/ {
        // Look for optional MOV RDI, RDI; if found, advance the anchor
        do {
            SgAsmx86Instruction *insn = isSgAsmx86Instruction(partitioner->discoverInstruction(anchor));
            if (!insn || insn->get_kind()!=x86_mov)
                break;
            const SgAsmExpressionPtrList &opands = insn->get_operandList()->get_operands();
            if (opands.size()!=2)
                break;
            SgAsmRegisterReferenceExpression *rre = isSgAsmRegisterReferenceExpression(opands[0]);
            if (!rre ||
                rre->get_descriptor().get_major()!=x86_regclass_gpr ||
                rre->get_descriptor().get_minor()!=x86_gpr_di)
                break;
            rre = isSgAsmRegisterReferenceExpression(opands[1]);
            if (!rre ||
                rre->get_descriptor().get_major()!=x86_regclass_gpr ||
                rre->get_descriptor().get_minor()!=x86_gpr_di)
                break;
            anchor += insn->get_size();
        } while (0);

        // Look for PUSH RBP at the (adjusted) anchor address. It must not already be in the CFG
        SgAsmx86Instruction *insn = isSgAsmx86Instruction(partitioner->discoverInstruction(anchor));
        {
            if (partitioner->instructionExists(anchor))
                return false;
            if (!insn || insn->get_kind()!=x86_push)
                return false;
            const SgAsmExpressionPtrList &opands = insn->get_operandList()->get_operands();
            if (opands.size()!=1)
                return false;
            SgAsmRegisterReferenceExpression *rre = isSgAsmRegisterReferenceExpression(opands[0]);
            if (!rre ||
                rre->get_descriptor().get_major()!=x86_regclass_gpr ||
                rre->get_descriptor().get_minor()!=x86_gpr_bp)
                return false;
        }

        // Look for MOV RBP,RSP following the PUSH. It must not already be in the CFG
        {
            rose_addr_t moveVa = insn->get_address() + insn->get_size();
            if (partitioner->instructionExists(moveVa))
                return false;
            insn = isSgAsmx86Instruction(partitioner->discoverInstruction(moveVa));
            if (!insn || insn->get_kind()!=x86_mov)
                return false;
            const SgAsmExpressionPtrList &opands = insn->get_operandList()->get_operands();
            if (opands.size()!=2)
                return false;
            SgAsmRegisterReferenceExpression *rre = isSgAsmRegisterReferenceExpression(opands[0]);
            if (!rre ||
                rre->get_descriptor().get_major()!=x86_regclass_gpr ||
                rre->get_descriptor().get_minor()!=x86_gpr_bp)
                return false;
            rre = isSgAsmRegisterReferenceExpression(opands[1]);
            if (!rre ||
                rre->get_descriptor().get_major()!=x86_regclass_gpr ||
                rre->get_descriptor().get_minor()!=x86_gpr_sp)
                return false;
        }

        functionVa_ = anchor;                               // address of the PUSH
        return true;
    }
};

int main(int argc, char *argv[])
{
    // Do this explicitly since the partitioner is not yet part of librose
    Diagnostics::initialize();
    P2::Partitioner::initDiagnostics();

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
    P2::Partitioner partitioner(disassembler, map);
    partitioner.functionPrologueMatchers().push_back(MatchM68kLink::instance());
    partitioner.functionPrologueMatchers().push_back(MatchX86Prologue::instance());
    partitioner.cfgAdjustmentCallbacks().append(CfgChangeWatcher::instance());

    // A simple disassembler
    rose_addr_t prologueVa = std::max(settings.disassembleVa, settings.startVa);
    partitioner.insertPlaceholder(prologueVa);
    P2::Partitioner::ControlFlowGraph::VertexNodeIterator worklist = partitioner.undiscoveredVertex();
    while (1) {
        if (worklist->nInEdges() > 0) {
            // Disassemble recursively by following undiscovered basic blocks in the CFG
            P2::Partitioner::ControlFlowGraph::VertexNodeIterator placeholder = worklist->inEdges().begin()->source();
            std::cerr <<"\n  processing " <<StringUtility::addrToString(placeholder->value().address());
            P2::Partitioner::BasicBlock::Ptr bb = partitioner.discoverBasicBlock(placeholder);
            partitioner.insertBasicBlock(placeholder, bb);
        } else if (partitioner.nextFunctionPrologue(prologueVa).assignTo(prologueVa)) {
            // We found another function prologue, so disassemble recursively at that location
            std::cerr <<"\nFound function prologue at " <<StringUtility::addrToString(prologueVa);
            partitioner.insertPlaceholder(prologueVa++);
        } else {
            break;                                      // all done
        }
    }
    std::cerr <<"\n";

    std::cout <<"Final control flow graph:\n";
    partitioner.dumpCfg(std::cout, "  ");

    exit(0);
}
