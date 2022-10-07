#include <rose.h>                                       // must be first ROSE include
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/ConcreteSemantics.h>
#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <sstream>

using namespace Rose::BinaryAnalysis;
namespace BS = Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics;
namespace IS = Rose::BinaryAnalysis::InstructionSemantics;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

static void
dumpMemory(std::ostream &out, const MemoryMap::Ptr &memory) {
    uint8_t buf[4096];                                  // multiple of 16
    rose_addr_t va = 0;
    HexdumpFormat fmt;
    fmt.prefix = "  ";
    while (AddressInterval where = memory->atOrAfter(va).limit(sizeof buf).read(buf)) {
        out <<"  ";
        SgAsmExecutableFileFormat::hexdump(out, where.least(), buf, where.size(), fmt);
        out <<"\n";
        if (where.greatest() == memory->hull().greatest())
            break;                                      // avoid possible integer overflow in next stmt
        va = where.greatest() + 1;
    }
}

int main() {
    ROSE_INITIALIZE;

    // Disassemble a small example specimen
    P2::Engine engine;
    engine.isaName("i386");
    engine.functionStartingVas().push_back(0x10000000);
    P2::Partitioner partitioner = engine.partition("data:0x10000000=rx::"      // i386 assembly:
                                                   "0xb8 0 0 0x10 0 "          //   mov eax, 0x100000
                                                   "0xbb 0xef 0xbe 0xad 0xde " //   mov ebx, 0xdeadbeef
                                                   "0x89 0x18");               //   mov dword ptr [eax], ebx

    // Create the instruction semantics and give it an initial MemoryMap containing the instructions we parsed above
    auto ops = IS::ConcreteSemantics::RiscOperators::instanceFromRegisters(partitioner.instructionProvider().registerDictionary());
    IS::ConcreteSemantics::MemoryState::promote(ops->currentState()->memoryState())->memoryMap(partitioner.memoryMap());

    // Show the initial state's memory map.
    auto origMemState = IS::ConcreteSemantics::MemoryState::promote(ops->currentState()->clone()->memoryState());
    std::ostringstream memBeforeRunning;
    dumpMemory(memBeforeRunning, origMemState->memoryMap());
    std::cout <<"Original state before instructions\n" <<memBeforeRunning.str();

    // Process the instructions
    BS::Dispatcher::Ptr dispatcher = partitioner.newDispatcher(ops);
    for (SgAsmInstruction *insn: partitioner.instructionsOverlapping(AddressInterval::whole())) {
        std::cout <<"Processing instruction " <<partitioner.unparse(insn) <<"\n";
        dispatcher->processInstruction(insn);
    }

    // Show the original state again, and the current state.
    std::ostringstream memAfterRunning;
    dumpMemory(memAfterRunning, origMemState->memoryMap());
    std::cout <<"Original state after processing instructions\n" <<memAfterRunning.str();

    std::ostringstream memCurrent;
    auto curMemState = IS::ConcreteSemantics::MemoryState::promote(ops->currentState()->memoryState());
    dumpMemory(memCurrent, curMemState->memoryMap());
    std::cout <<"Current state after instructions\n" <<memCurrent.str();

    // The original state should not have been modified by executing instructions because we cloned it. The instructions
    // should have modified only the current state.
    ASSERT_always_require2(memBeforeRunning.str() == memAfterRunning.str(),
                           "initial saved memory was modified by executing instructions");
    ASSERT_always_require2(memBeforeRunning.str() != memCurrent.str(),
                           "executing instructions should have changed the current memory state");
}

#else

int main() {}                                           // test skipped, automatically passes

#endif
