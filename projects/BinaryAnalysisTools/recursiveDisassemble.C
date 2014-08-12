#include <rose.h>
#include <rosePublicConfig.h>
#include <AsmUnparser.h>
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
    std::string isaName;                                // instruction set architecture name
    rose_addr_t mapVa;                                  // where to map the specimen in virtual memory
    bool doListCfg;                                     // list the control flow graph
    bool doListAum;                                     // list the address usage map
    bool doListAsm;                                     // produce an assembly-like listing with AsmUnparser
    bool doListFunctionAddresses;                       // list function entry addresses
    bool doShowStats;                                   // show some statistics
    bool doListUnused;                                  // list unused addresses
    Settings()
        : mapVa(0), doListCfg(false), doListAum(false), doListAsm(true), doListFunctionAddresses(false),
          doShowStats(false), doListUnused(false) {}
};

// Describe and parse the command-line
static Sawyer::CommandLine::ParserResult
parseCommandLine(int argc, char *argv[], Settings &settings)
{
    using namespace Sawyer::CommandLine;

    // Generic switches
    SwitchGroup gen;
    gen.doc("General switches:");
    gen.insert(Switch("help", 'h')
               .doc("Show this documentation.")
               .action(showHelpAndExit(0)));
    gen.insert(Switch("log", 'L')
               .action(configureDiagnostics("log", Sawyer::Message::mfacilities))
               .argument("config")
               .whichValue(SAVE_ALL)
               .doc("Configures diagnostics.  Use \"@s{log}=help\" and \"@s{log}=list\" to get started."));
    gen.insert(Switch("version", 'V')
               .action(showVersionAndExit(version_message(), 0))
               .doc("Shows version information for various ROSE components and then exits."));

    // Switches for disassembly
    SwitchGroup dis;
    dis.doc("Switches for disassembly:");
    dis.insert(Switch("isa")
               .argument("architecture", anyParser(settings.isaName))
               .doc("Instruction set architecture. Specify \"list\" to see a list of possible ISAs."));
    dis.insert(Switch("map")
               .argument("virtual-address", nonNegativeIntegerParser(settings.mapVa))
               .doc("The first byte of the file is mapped at the specified @v{virtual-address}, which defaults "
                    "to " + StringUtility::addrToString(settings.mapVa) + "."));

    // Switches for output
    SwitchGroup out;
    out.doc("Switches that affect output:");
    out.insert(Switch("list-asm")
               .intrinsicValue(true, settings.doListAsm)
               .doc("Produce an assembly listing.  This is the default; it can be turned off with @s{no-list-asm}."));
    out.insert(Switch("no-list-asm")
               .key("list-asm")
               .intrinsicValue(false, settings.doListAsm)
               .hidden(true));

    out.insert(Switch("list-aum")
               .intrinsicValue(true, settings.doListAum)
               .doc("Emit a listing of the address usage map after the CFG is discovered.  The @s{no-list-aum} switch is "
                    "the inverse."));
    out.insert(Switch("no-list-aum")
               .key("list-aum")
               .intrinsicValue(false, settings.doListAum)
               .hidden(true));

    out.insert(Switch("list-cfg")
               .intrinsicValue(true, settings.doListCfg)
               .doc("Emit a listing of the CFG after it is discovered."));
    out.insert(Switch("no-list-cfg")
               .key("list-cfg")
               .intrinsicValue(false, settings.doListCfg)
               .hidden(true));

    out.insert(Switch("list-function-entries")
               .intrinsicValue(true, settings.doListFunctionAddresses)
               .doc("Produce a listing of function entry addresses, one address per line in hexadecimal format. Each address "
                    "is followed by the word \"existing\" or \"missing\" depending on whether a non-empty basic block exists "
                    "in the CFG for the function entry address.  The listing is disabled with @s{no-list-function-entries}."));
    out.insert(Switch("no-list-function-entries")
               .key("list-function-entries")
               .intrinsicValue(false, settings.doListFunctionAddresses)
               .hidden(true));

    out.insert(Switch("list-unused")
               .intrinsicValue(true, settings.doListUnused)
               .doc("Produce a listing of all specimen addresses that are not represented in the control flow graph. This "
                    "listing can be disabled with @s{no-list-unused}."));
    out.insert(Switch("no-list-unused")
               .key("list-unused")
               .intrinsicValue(false, settings.doListUnused)
               .hidden(true));

    out.insert(Switch("show-stats")
               .intrinsicValue(true, settings.doShowStats)
               .doc("Emit some information about how much of the input was disassembled."));
    out.insert(Switch("no-show-stats")
               .key("show-stats")
               .intrinsicValue(false, settings.doShowStats)
               .hidden(true));
    

    Parser parser;
    parser
        .purpose("tests new partitioner architecture")
        .doc("synopsis",
             "@prop{programName} [@v{switches}] @v{specimen_name}")
        .doc("description",
             "This program tests the new partitioner architecture by disassembling the specified raw file.");
    
    return parser.with(gen).with(dis).with(out).parse(argc, argv).apply();
}

// Example of watching CFG changes
class CfgChangeWatcher: public P2::Partitioner::CfgAdjustmentCallback {
public:
    static Ptr instance() { return Ptr(new CfgChangeWatcher); }
    virtual bool operator()(bool enabled, const InsertionArgs &args) {
#if 0 // [Robb P. Matzke 2014-08-05]
        std::cerr <<"+";
        if (P2::Partitioner::BasicBlock::Ptr bb = args.insertedVertex->value().bblock())
            std::cerr <<std::string(bb->nInsns(), '.');
#endif
        return enabled;
    }
    virtual bool operator()(bool enabled, const ErasureArgs &args) {
#if 0 // [Robb P. Matzke 2014-08-05]
        std::cerr <<"-" <<std::string(args.erasedBlock->nInsns(), '.');
#endif
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
        if (anchor & 1)
            return false;                               // m68k instructions must be 16-bit aligned
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
    disassembler->set_progress_reporting(-1.0);         // turn it off

    // Open the file that needs to be disassembled and map it with read and execute permission
    if (positionalArgs.empty())
        throw std::runtime_error("no file name specified; see --help");
    if (positionalArgs.size()>1)
        throw std::runtime_error("too many files specified; see --help");
    std::string specimenName = positionalArgs[0];
    MemoryMap map;
    size_t nBytesMapped = map.insert_file(specimenName, settings.mapVa);
    if (0==nBytesMapped)
        throw std::runtime_error("problem reading file: " + specimenName);
    map.mprotect(AddressInterval::baseSize(settings.mapVa, nBytesMapped), MemoryMap::MM_PROT_RX);
    map.dump(std::cerr);                                // debugging so the user can see the map


    // Create the partitioner
    P2::Partitioner partitioner(disassembler, map);
    partitioner.functionPrologueMatchers().push_back(MatchM68kLink::instance());
    partitioner.functionPrologueMatchers().push_back(MatchX86Prologue::instance());
    partitioner.cfgAdjustmentCallbacks().append(CfgChangeWatcher::instance());

    // A simple disassembler
    rose_addr_t prologueVa = 0;
    P2::Partitioner::ControlFlowGraph::VertexNodeIterator worklist = partitioner.undiscoveredVertex();
    while (1) {
        if (worklist->nInEdges() > 0) {
            // Disassemble recursively by following undiscovered basic blocks in the CFG
            P2::Partitioner::ControlFlowGraph::VertexNodeIterator placeholder = worklist->inEdges().begin()->source();
            partitioner.mlog[WHERE] <<"\n  processing block " <<partitioner.vertexName(*placeholder) <<"\n";
            P2::Partitioner::BasicBlock::Ptr bb = partitioner.discoverBasicBlock(placeholder);
            partitioner.insertBasicBlock(placeholder, bb);
        } else if (partitioner.nextFunctionPrologue(prologueVa).assignTo(prologueVa)) {
            // We found another function prologue, so disassemble recursively at that location
            partitioner.mlog[WHERE] <<"\nFound function prologue at " <<StringUtility::addrToString(prologueVa) <<"\n";
            partitioner.insertFunction(P2::Partitioner::Function::instance(prologueVa));
        } else {
            break;                                      // all done
        }
    }

    // Find all the function entry points. The entry points are the function prologues we found above, and any basic block
    // which is a target of a function call edge.
    P2::Partitioner::Functions functions = partitioner.discoverFunctionEntryVertices();

    // Discover blocks for functions, and (re)insert the functions into the CFG
    std::vector<P2::Partitioner::Function::Ptr> badFunctions;
    BOOST_FOREACH (const P2::Partitioner::Function::Ptr &function, functions.values()) {
        partitioner.mlog[WHERE] <<"Discovering blocks for function "
                                <<StringUtility::addrToString(function->address()) <<"\n";
        if (function->isFrozen()) {
            // If the function is is the CFG then it is frozen and we can't adjust its connectivity to blocks.  Therefore we
            // first have to remove the function from the CFG, operate on it while it is thawed, then insert it again when
            // we're done.
            partitioner.eraseFunction(function);
        }
        if (partitioner.discoverFunctionBasicBlocks(function, NULL, NULL)) {
            badFunctions.push_back(function);
        } else {
            partitioner.insertFunction(function);
        }
    }

    // Try each failed function again, but this time report the errors, and maybe insert them into the CFG anyway
    BOOST_FOREACH (const P2::Partitioner::Function::Ptr &function, badFunctions) {
        P2::Partitioner::EdgeList inwardConflictEdges, outwardConflictEdges;
        if (partitioner.discoverFunctionBasicBlocks(function, &inwardConflictEdges, &outwardConflictEdges)) {
            partitioner.mlog[WARN] <<"discovery for function " <<StringUtility::addrToString(function->address())
                                   <<" had " <<StringUtility::plural(inwardConflictEdges.size(), "inward conflicts")
                                   <<" and " <<StringUtility::plural(outwardConflictEdges.size(), "outward conflicts") <<"\n";

            BOOST_FOREACH (const P2::Partitioner::ControlFlowGraph::EdgeNodeIterator &edge, inwardConflictEdges) {
                using namespace P2;                     // to pick up operator<< for *edge
                partitioner.mlog[WARN] <<"  inward conflict " <<*edge
                                       <<" from " <<partitioner.functionName(edge->source()->value().function())
                                       <<"\n";
            }
            BOOST_FOREACH (const P2::Partitioner::ControlFlowGraph::EdgeNodeIterator &edge, outwardConflictEdges) {
                using namespace P2;                     // to pick up operator<< for *edge
                partitioner.mlog[WARN] <<"  outward conflict " <<*edge
                                       <<" to " <<partitioner.functionName(edge->target()->value().function())
                                       <<"\n";
            }

            // Forcibly insert the target vertex for inward-conflicting edges and try again.
            BOOST_FOREACH (P2::Partitioner::ControlFlowGraph::EdgeNodeIterator &edge, inwardConflictEdges)
                function->insertBasicBlock(edge->target()->value().address());
            if (0==partitioner.discoverFunctionBasicBlocks(function, NULL, NULL)) {
                partitioner.mlog[WARN] <<"  conflicts have been overridden\n";
                partitioner.insertFunction(function);
            }
        }
    }

    // Show the results as requested
    if (settings.doShowStats) {
        std::cout <<"CFG contains " <<StringUtility::plural(partitioner.nFunctions(), "functions") <<"\n";
        std::cout <<"CFG contains " <<StringUtility::plural(partitioner.nBasicBlocks(), "basic blocks") <<"\n";
        std::cout <<"CFG contains " <<StringUtility::plural(partitioner.nInstructions(), "instructions") <<"\n";
        std::cout <<"CFG contains " <<StringUtility::plural(partitioner.nBytes(), "bytes") <<"\n";
        std::cout <<"Instruction cache contains "
                  <<StringUtility::plural(partitioner.instructionProvider().nCached(), "instructions") <<"\n";
        std::cout <<"Specimen contains " <<StringUtility::plural(nBytesMapped, "bytes") <<"\n";
        std::cout <<"CFG covers " <<(100.0*partitioner.nBytes()/nBytesMapped) <<"% of specimen\n";
        std::cout <<"Specimen not covered by CFG: " <<StringUtility::plural(nBytesMapped-partitioner.nBytes(), "bytes") <<"\n";
    }

    if (settings.doListCfg) {
        std::cout <<"Final control flow graph:\n";
        partitioner.dumpCfg(std::cout, "  ", true);
    }

    if (settings.doListAum) {
        std::cout <<"Final address usage map:\n";
        partitioner.aum().print(std::cout, "  ");
    }
    
    if (settings.doListUnused) {
        AddressInterval addressSpace = AddressInterval::baseSize(settings.mapVa, nBytesMapped);
        Sawyer::Container::IntervalSet<AddressInterval> unusedAddresses = partitioner.aum().unusedExtent(addressSpace);
        BOOST_FOREACH (const AddressInterval &unused, unusedAddresses.nodes()) {
            std::cout <<unused;
            std::cout <<"\t" <<StringUtility::plural(unused.size(), "bytes") <<"\n";
        }
    }

    if (settings.doListFunctionAddresses) {
        BOOST_FOREACH (rose_addr_t functionVa, partitioner.functions().keys()) {
            P2::Partitioner::BasicBlock::Ptr bb = partitioner.basicBlockExists(functionVa);
            std::cout <<StringUtility::addrToString(functionVa) <<": "
                      <<(bb && !bb->isEmpty() ? "exists" : "missing") <<"\n";
        }
    }

    // Build the AST and unparse it.
    if (settings.doListAsm) {
        SgAsmBlock *globalBlock = partitioner.buildAst();
        AsmUnparser().unparse(std::cout, globalBlock);
    }
    
    exit(0);
}
