#include <rose.h>                                       // must be first ROSE include
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/InstructionSemantics2/BaseSemantics.h>
#include <Rose/BinaryAnalysis/InstructionSemantics2/ConcreteSemantics.h>
#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>

using namespace Rose::BinaryAnalysis;
namespace BS = Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics;
namespace IS = Rose::BinaryAnalysis::InstructionSemantics2;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

static void
dumpMemory(const std::string &title, const MemoryMap::Ptr &memory) {
    uint8_t buf[4096];                                  // multiple of 16
    rose_addr_t va = 0;
    std::cout <<title <<"\n";
    HexdumpFormat fmt;
    fmt.prefix = "  ";
    while (AddressInterval where = memory->atOrAfter(va).limit(sizeof buf).read(buf)) {
        std::cout <<"  ";
        SgAsmExecutableFileFormat::hexdump(std::cout, where.least(), buf, where.size(), fmt);
        std::cout <<"\n";
        if (where.greatest() == memory->hull().greatest())
            break;                                      // avoid possible integer overflow in next stmt
        va = where.greatest() + 1;
    }
}

int main(int argc, char *argv[]) {
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
    auto ops = IS::ConcreteSemantics::RiscOperators::instance(partitioner.instructionProvider().registerDictionary());
    IS::ConcreteSemantics::MemoryState::promote(ops->currentState()->memoryState())->memoryMap(partitioner.memoryMap());

    // Show the initial state's memory map.
    auto origMemState = IS::ConcreteSemantics::MemoryState::promote(ops->currentState()->clone()->memoryState());
    dumpMemory("Original state before instructions", origMemState->memoryMap());

    // Process the instructions
    BS::Dispatcher::Ptr dispatcher = partitioner.newDispatcher(ops);
    for (SgAsmInstruction *insn: partitioner.instructionsOverlapping(AddressInterval::whole())) {
        std::cout <<"Processing instruction " <<partitioner.unparse(insn) <<"\n";
        dispatcher->processInstruction(insn);
    }

    // Show the original state (which should be unmodified from what we printed earlier) and the current state.
    dumpMemory("Original state after processing instructions", origMemState->memoryMap());
    auto curMemState = IS::ConcreteSemantics::MemoryState::promote(ops->currentState()->memoryState());
    dumpMemory("Current state after instructions", curMemState->memoryMap());
}

#else

int main() {}                                           // test skipped, automatically passes

#endif
