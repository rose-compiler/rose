#include <rose.h>

#include <boost/filesystem.hpp>
#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/MemoryMap.h>
#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>
#include <rose_strtoull.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/TraceSemantics.h>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;
namespace S2 = Rose::BinaryAnalysis::InstructionSemantics;

static void
showUsageAndExit(int exitValue, const boost::filesystem::path arg0) {
    std::cerr <<"usage: " <<arg0.filename().string() <<" ISA BYTES...\n"
              <<"where ISA is one of \"a64\", \"a32\", or \"t32\" and BYTES are two or four hexadecimal "
              <<"arguments with or without leading \"0x\" specifying the machine instruction in big endian order\n";
    exit(exitValue);
}

// The first argument is an optional starting address of the form "=N" where N is a hexadecimal value without the leading "0x"
// and the bytes are hexidecimal without the leading "0x".
static MemoryMap::Ptr
parseBytes(size_t nWords, char *words[]) {
    rose_addr_t va = 0;
    std::vector<uint8_t> bytes;
    bytes.reserve(nWords);
    size_t i = 0;
    if (nWords > 0 && words[0] && '=' == words[0][0]) {
        va = rose_strtoull(words[0]+1, nullptr, 16);
        ++i;
    }
    for (/*void*/; i < nWords; ++i) {
        uint8_t byte = rose_strtoull(words[i], nullptr, 16);
        bytes.push_back(byte);
    }
    std::reverse(bytes.begin(), bytes.end());

    auto map = MemoryMap::instance();
    map->insert(AddressInterval::baseSize(va, bytes.size()),
                MemoryMap::Segment(MemoryMap::AllocatingBuffer::instance(bytes.size()),
                                   0, MemoryMap::READ_EXECUTE, "instructions"));
    map->at(va).write(bytes);
    return map;
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;

    // Parse command-line
    if (argc < 3)
        showUsageAndExit(1, argv[0]);
    std::string isa = argv[1];
    MemoryMap::Ptr memory = parseBytes(argc-2, argv+2); // exclude program name and ISA name

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
