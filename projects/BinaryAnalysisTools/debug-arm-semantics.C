#include <rose.h>

#include <Disassembler.h>
#include <MemoryMap.h>
#include <Partitioner2/Engine.h>
#include <rose_strtoull.h>
#include <TraceSemantics2.h>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;
namespace S2 = Rose::BinaryAnalysis::InstructionSemantics2;

static MemoryMap::Ptr
parseBytes(int argc, char *argv[]) {
    std::vector<uint8_t> bytes;
    for (int i = 1; i < argc; ++i) {
        uint8_t byte = rose_strtoull(argv[i], NULL, 16);
        bytes.push_back(byte);
    }
    std::reverse(bytes.begin(), bytes.end());

    auto map = MemoryMap::instance();
    map->insert(AddressInterval::baseSize(0, bytes.size()),
                MemoryMap::Segment(MemoryMap::AllocatingBuffer::instance(bytes.size()),
                                   0, MemoryMap::READ_EXECUTE, "data"));
    map->at(0).write(bytes);
    return map;
}

int
main(int argc, char *argv[]) {
    MemoryMap::Ptr memory = parseBytes(argc, argv);
    P2::Engine engine;
    engine.settings().disassembler.isaName = "a64";
    engine.memoryMap(memory);
    P2::Partitioner p = engine.createTunedPartitioner();
    auto ops = S2::TraceSemantics::RiscOperators::instance(p.newOperators());
    S2::BaseSemantics::Dispatcher::Ptr cpu = p.newDispatcher(ops);

    size_t va = memory->hull().least();
    while (SgAsmInstruction *insn = p.instructionProvider()[va]) {
        std::cerr <<p.unparse(insn) <<"\n";
        cpu->processInstruction(insn);
        std::cerr <<*ops->currentState();
        va += insn->get_size();
    }
}
