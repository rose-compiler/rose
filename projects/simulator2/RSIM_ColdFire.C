#include <rose.h>
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR

#include "RSIM_ColdFire.h"
#include <Partitioner2/Engine.h>

using namespace rose;
using namespace rose::Diagnostics;

void
RSIM_ColdFire::init() {}

void
RSIM_ColdFire::updateExecutablePath() {
    // Nothing to do since this is raw hardware with no operating system.
}

SgAsmInterpretation*
RSIM_ColdFire::parseMainExecutable(RSIM_Process *process) {
    namespace P2 = rose::BinaryAnalysis::Partitioner2;
    using namespace Sawyer::CommandLine;

    // This is raw hardware, so assume that all the arguments are for loading the specimen.
    P2::Engine engine;
    Parser parser;
    parser
        .purpose("initializes ColdFire memory")
        .version(std::string(ROSE_SCM_VERSION_ID).substr(0, 8), ROSE_CONFIGURE_DATE)
        .chapter(1, "ROSE Command-line Tools")
        .doc("Synopsis", "@prop{programName} ... -- [@v{loader_switches}] @v{resources}")
        .doc("Description",
             "This part of the simulator command-line is responsible for configuring how @v{resources} are loaded into "
             "simulated FreeScale ColdFire system memory.  If switches are provided here they must be separated from "
             "simulator switches with a \"--\" to prevent the simulator itself from interpreting them.\n\n" +
             engine.specimenNameDocumentation())
        .with(Switch("help", 'h')
              .hidden(true)
              .action(showHelpAndExit(0)))
        .with(engine.loaderSwitches());
    std::vector<std::string> resources = parser.parse(exeArgs()).apply().unreachedArgs();
    engine.isaName("coldfire");
    MemoryMap map = engine.loadSpecimens(resources);
    process->mem_transaction_start("specimen main memory");
    process->get_memory() = map;                        // shallow copy, new segments point to same old data

    // The initial program counter is stored at address 4, the second entry in the interrupt vector.
    uint32_t initialIpBe = 0;
    if (!map.at(4).limit(sizeof initialIpBe).read((uint8_t*)&initialIpBe)) {
        mlog[FATAL] <<"failed to read initial program counter from address zero\n";
        exit(1);
    }
    uint32_t initialIp = ByteOrder::be_to_host(initialIpBe);
    process->entryPointOriginalVa(initialIp);
    process->entryPointStartVa(initialIp);

    process->disassembler(engine.obtainDisassembler());
    return engine.interpretation();                     // probably null since args not likely to be ELF or PE
}

bool
RSIM_ColdFire::isSupportedArch(SgAsmGenericHeader*) {
    return true;
}

PtRegs
RSIM_ColdFire::initialRegistersArch(RSIM_Process *process) {
    PtRegs regs;

    // Initial stack pointer is at address zero, four bytes big endian.
    uint32_t initialSpBe = 0;
    process->get_memory().at(0).limit(sizeof initialSpBe).read((uint8_t*)&initialSpBe);
    regs.a7 = ByteOrder::be_to_host(initialSpBe);

    return regs;
}

void
RSIM_ColdFire::loadSpecimenNative(RSIM_Process*, rose::BinaryAnalysis::Disassembler*, int existingPid) {
    ASSERT_not_reachable("native loading not possible for naked hardware");
}

void
RSIM_ColdFire::loadSpecimenArch(RSIM_Process*, SgAsmInterpretation*, const std::string &interpName) {
    // All the work was done by parseMainExecutable
}

void
RSIM_ColdFire::initializeStackArch(RSIM_Thread*, SgAsmGenericHeader *) {
    // Nothing to do for naked hardware
}

void
RSIM_ColdFire::initializeSimulatedOs(RSIM_Process*, SgAsmGenericHeader*) {
    // Nothing to do for naked hardware
}

#endif
