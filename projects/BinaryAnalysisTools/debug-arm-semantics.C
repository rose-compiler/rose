#include <rose.h>

#include <boost/filesystem.hpp>
#include <Disassembler.h>
#include <MemoryMap.h>
#include <Partitioner2/Engine.h>
#include <rose_strtoull.h>
#include <TraceSemantics2.h>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;
namespace S2 = Rose::BinaryAnalysis::InstructionSemantics2;

static const rose_addr_t startingVa = 0;

static void
showUsageAndExit(int exitValue, const boost::filesystem::path arg0) {
    std::cerr <<"usage: " <<arg0.filename().string() <<" ISA BYTES...\n"
              <<"where ISA is one of \"a64\", \"a32\", or \"t32\" and BYTES are two or four hexadecimal "
              <<"arguments with or without leading \"0x\" specifying the machine instruction in big endian order\n";
    exit(exitValue);
}

static MemoryMap::Ptr
parseBytes(int argc, char *argv[]) {
    std::vector<uint8_t> bytes;
    for (int i = 1; i < argc; ++i) {
        uint8_t byte = rose_strtoull(argv[i], NULL, 16);
        bytes.push_back(byte);
    }
    std::reverse(bytes.begin(), bytes.end());

    auto map = MemoryMap::instance();
    map->insert(AddressInterval::baseSize(startingVa, bytes.size()),
                MemoryMap::Segment(MemoryMap::AllocatingBuffer::instance(bytes.size()),
                                   0, MemoryMap::READ_EXECUTE, "data"));
    map->at(startingVa).write(bytes);
    return map;
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;

    // Parse command-line
    MemoryMap::Ptr memory;
    if ((argc == 6 && (!strcmp(argv[1], "a64") || !strcmp(argv[1], "a32"))) ||
        (argc == 4 && !strcmp(argv[1], "t32"))) {
        memory = parseBytes(argc-1, argv+1);
    } else {
        showUsageAndExit(1, argv[0]);
    }
    std::string isa = argv[1];

    // Create the decoder and semantics
    P2::Engine engine;
    engine.settings().disassembler.isaName = isa;
    engine.memoryMap(memory);
    P2::Partitioner p = engine.createTunedPartitioner();
    auto symOps = S2::SymbolicSemantics::RiscOperators::promote(p.newOperators());
    symOps->trimThreshold(UNLIMITED);
    std::cout <<"expr size limit = " <<symOps->trimThreshold() <<"\n";
    auto ops = S2::TraceSemantics::RiscOperators::instance(symOps);
    S2::BaseSemantics::Dispatcher::Ptr cpu = p.newDispatcher(ops);

    // Decode and process the instruction
    size_t va = memory->hull().least();
    while (SgAsmInstruction *insn = p.instructionProvider()[va]) {
        std::cerr <<p.unparse(insn) <<"\n";
        if (cpu) {
            cpu->processInstruction(insn);
            std::cerr <<*ops->currentState();
        }
        va += insn->get_size();
    }
}
