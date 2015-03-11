#include <rose.h>

#include <AsmUnparser_compat.h>
#include <Diagnostics.h>
#include <Partitioner2/Engine.h>
#include <ConcreteSemantics2.h>

using namespace rose;
using namespace rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;
using namespace rose::BinaryAnalysis::InstructionSemantics2;

Diagnostics::Facility mlog;

// Settings from the command-line
struct Settings {
    std::string isaName;                                // instruction set architecture name
    Sawyer::Optional<rose_addr_t> startVa;              // where to start executing
};
static Settings settings;

// Describe and parse the command-line
static Sawyer::CommandLine::ParserResult
parseCommandLine(int argc, char *argv[])
{
    using namespace Sawyer::CommandLine;

    //--------------------------- 
    Parser parser;
    parser
        .purpose("simulate execution using concrete semantics")
        .version(std::string(ROSE_SCM_VERSION_ID).substr(0, 8), ROSE_CONFIGURE_DATE)
        .chapter(1, "ROSE Command-line Tools")
        .doc("Synopsis", "@prop{programName} [@v{switches}] @v{specimen...}")
        .doc("Description",
             "Parses and loads the specimen and then executes its instructions in a concrete domain.")
        .doc("Specimens", Partitioner2::Engine::specimenNameDocumentation());
    
    //--------------------------- 
    SwitchGroup gen = CommandlineProcessing::genericSwitches();

    //---------------------------
    SwitchGroup tool("Tool switches");
    tool.insert(Switch("isa")
               .argument("architecture", anyParser(settings.isaName))
               .doc("Instruction set architecture. Specify \"list\" to see a list of possible ISAs."));

    tool.insert(Switch("start")
                .argument("address", nonNegativeIntegerParser(settings.startVa))
                .doc("Address at which to start executing. If no address is specified then execution starts at the "
                     "lowest address having execute permission."));

    return parser.with(gen).with(tool).parse(argc, argv).apply();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
    Diagnostics::initialize();
    ::mlog = Diagnostics::Facility("tool", Diagnostics::destination);
    Diagnostics::mfacilities.insertAndAdjust(::mlog);

    // Parse the command-line
    std::vector<std::string> specimenNames = parseCommandLine(argc, argv).unreachedArgs();
    Partitioner2::Engine engine;
    if (!settings.isaName.empty())
        engine.disassembler(Disassembler::lookup(settings.isaName));
    if (specimenNames.empty())
        throw std::runtime_error("no specimen specified; see --help");

    // Load specimen into memory
    MemoryMap map = engine.load(specimenNames);

    // Configure instruction semantics
    Partitioner2::Partitioner partitioner = engine.createTunedPartitioner();
    Disassembler *disassembler = engine.obtainDisassembler();
    const RegisterDictionary *regdict = disassembler->get_registers();
    if (disassembler->dispatcher() == NULL)
        throw std::runtime_error("no instruction semantics for this architecture");    
    BaseSemantics::RiscOperatorsPtr ops = InstructionSemantics2::ConcreteSemantics::RiscOperators::instance(regdict);
    BaseSemantics::DispatcherPtr cpu = disassembler->dispatcher()->create(ops);
    ConcreteSemantics::MemoryState::promote(ops->get_state()->get_memory_state())->memoryMap(map);

    // Find starting address
    rose_addr_t va = 0;
    if (settings.startVa) {
        va = *settings.startVa;
    } else if (settings.isaName == "coldfire") {
        // Use the interrupt vector to initialize the stack pointer and instruction pointer.
        uint32_t sp, ip;
        if (4 != map.at(0).limit(4).read((uint8_t*)&sp).size())
            throw std::runtime_error("cannot read stack pointer at address 0x00000000");
        ops->writeRegister(disassembler->stackPointerRegister(), ops->number_(32, ByteOrder::be_to_host(sp)));
        if (4 != map.at(4).limit(4).read((uint8_t*)&ip).size())
            throw std::runtime_error("cannot read instruction pointer at address 0x00000004");
        va = ByteOrder::be_to_host(ip);
    } else if (!map.atOrAfter(0).require(MemoryMap::EXECUTABLE).next().assignTo(va)) {
        throw std::runtime_error("no starting address specified and none marked executable");
    }
    ops->writeRegister(disassembler->instructionPointerRegister(), ops->number_(32, va));

    // Execute
    while (1) {
        va = ops->readRegister(disassembler->instructionPointerRegister())->get_number();
        SgAsmInstruction *insn = partitioner.instructionProvider()[va];
        SAWYER_MESG(::mlog[TRACE]) <<unparseInstructionWithAddress(insn, NULL, regdict) <<"\n";
        try {
            cpu->processInstruction(insn);
        } catch (const BaseSemantics::Exception &e) {
            ::mlog[WARN] <<e <<"\n";
        }
    }

    // std::cout <<"Final state:\n";
    // std::cout <<*ops->get_state();
}
