#include <rose.h>
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR

#include "RSIM_Pentium4.h"
#include <Partitioner2/Engine.h>

using namespace rose;
using namespace rose::Diagnostics;

void
RSIM_Pentium4::init() {}

void
RSIM_Pentium4::updateExecutablePath() {
    // Nothing to do since this is raw hardware with no operating system.
}

SgAsmInterpretation*
RSIM_Pentium4::parseMainExecutable(RSIM_Process *process) {
    namespace P2 = rose::BinaryAnalysis::Partitioner2;
    using namespace Sawyer::CommandLine;

    // This is raw hardware, so assume that all the arguments are for loading the specimen.
    P2::Engine engine;
    Parser parser;
    parser
        .purpose("initializes Pentium4 memory")
        .version(std::string(ROSE_SCM_VERSION_ID).substr(0, 8), ROSE_CONFIGURE_DATE)
        .chapter(1, "ROSE Command-line Tools")
        .doc("Synopsis", "@prop{programName} ... -- [@v{loader_switches}] @v{resources}")
        .doc("Description",
             "This part of the simulator command-line is responsible for configuring how @v{resources} are loaded into "
             "simulated Intel Pentium4 system memory.  If switches are provided here they must be separated from "
             "simulator switches with a \"--\" to prevent the simulator itself from interpreting them.")
        .with(Switch("help", 'h')
              .hidden(true)
              .action(showHelpAndExit(0)))
        .with(engine.loaderSwitches());

    SwitchGroup pentium4("Pentium4-specific switches");
    pentium4.insert(Switch("eip")
                    .argument("address", nonNegativeIntegerParser(initialEip_))
                    .doc("Initial value for the instruction pointer register. The default is " +
                         StringUtility::addrToString(initialEip_) + "."));
    pentium4.insert(Switch("esp")
                    .argument("address", nonNegativeIntegerParser(initialEsp_))
                    .doc("Initial value for the stack pointer register. The default is " +
                         StringUtility::addrToString(initialEsp_) + "."));

    std::vector<std::string> resources = parser.with(pentium4).parse(exeArgs()).apply().unreachedArgs();
    engine.isaName("i386");
    MemoryMap map = engine.loadSpecimens(resources);
    process->mem_transaction_start("specimen main memory");
    process->get_memory() = map;                        // shallow copy, new segments point to same old data

    // The initial program counter is stored at address 4, the second entry in the interrupt vector.
    process->entryPointOriginalVa(initialEip_);
    process->entryPointStartVa(initialEip_);

    process->disassembler(engine.obtainDisassembler());
    return engine.interpretation();                     // probably null since args not likely to be ELF or PE
}

bool
RSIM_Pentium4::isSupportedArch(SgAsmGenericHeader*) {
    return true;
}

PtRegs
RSIM_Pentium4::initialRegistersArch(RSIM_Process *process) {
    PtRegs regs;
    regs.sp = initialEsp_;                              // high end of stack, exclusive
    regs.flags = 2;                                     // flag bit 1 is set, although this is a reserved bit
    regs.cs = 0x23;
    regs.ds = 0x2b;
    regs.es = 0x2b;
    regs.ss = 0x2b;
    return regs;
}

void
RSIM_Pentium4::loadSpecimenNative(RSIM_Process*, rose::BinaryAnalysis::Disassembler*, int existingPid) {
    ASSERT_not_reachable("native loading not possible for naked hardware");
}

void
RSIM_Pentium4::loadSpecimenArch(RSIM_Process*, SgAsmInterpretation*, const std::string &interpName) {
    // All the work was done by parseMainExecutable
}

void
RSIM_Pentium4::initializeStackArch(RSIM_Thread*, SgAsmGenericHeader *) {
    // Nothing to do for naked hardware
}

void
RSIM_Pentium4::initializeSimulatedOs(RSIM_Process*, SgAsmGenericHeader*) {
    // Nothing to do for naked hardware
}

void
RSIM_Pentium4::threadCreated(RSIM_Thread *t) {
    t->operators()->allocateOnDemand(true);
}

#endif
