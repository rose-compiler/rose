#include <rose.h>
#include <BinaryUnparserBase.h>
#include <CommandLine.h>
#include <Disassembler.h>
#include <Partitioner2/Engine.h>
#include <rose_getline.h>
#include <rose_strtoull.h>

#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/format.hpp>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;
namespace I2 = Rose::BinaryAnalysis::InstructionSemantics2;

bool runSemantics = false;

static std::string
parseCommandLine(int argc, char *argv[], P2::Engine &engine, BinaryAnalysis::Unparser::Settings &upSettings) {
    using namespace Sawyer::CommandLine;

    SwitchGroup output = BinaryAnalysis::Unparser::commandLineSwitches(upSettings);

    SwitchGroup tool("Tool-specific switches");
    tool.name("tool");
    Rose::CommandLine::insertBooleanSwitch(tool, "semantics", runSemantics, "Run instruction semantics.");

    // The ony switch we want is --isa=ISA.
    SwitchGroup disassembler = P2::Engine::disassemblerSwitches(engine.settings().disassembler);
    Switch isa = disassembler.getByName("isa");
    while (disassembler.nSwitches() > 0)
        disassembler.removeByIndex(0);
    disassembler.insert(isa);

    Parser p = Rose::CommandLine::createEmptyParser("testing", "Tests disassembler and semantics in various ways.");
    p.doc("Synopsis", "@prop{programName} [@v{switches}] @v{test_input}");
    p.with(Rose::CommandLine::genericSwitches());

    ParserResult cmdline = p.with(disassembler).with(output).with(tool).parse(argc, argv).apply();
    ASSERT_always_require(cmdline.unreachedArgs().size() == 1);
    return cmdline.unreachedArgs()[0];
}

static std::string
strip(std::string s) {
    size_t hash = s.find('#');
    if (hash != std::string::npos)
        s = s.substr(0, hash);
    return boost::trim_copy(s);
}

static std::vector<uint8_t>
parse(const std::string &input, rose_addr_t &va) {
    std::vector<uint8_t> retval;
    std::string str = strip(input);

    // Parse "va ADDR"
    if (boost::starts_with(str, "va ")) {
        char *rest = NULL;
        va = rose_strtoull(str.c_str()+3, &rest, 0);
        ASSERT_always_require(0 == errno);
        ASSERT_always_require(rest > str.c_str()+3);
        return retval;
    }
    
    // Parse: BYTE...
    const char *s = str.c_str();
    while (*s) {
        char *rest = NULL;
        uint64_t byte = rose_strtoull(s, &rest, 0);
        ASSERT_always_require(0 == errno);
        ASSERT_always_require(byte <= 0xff);
        ASSERT_always_require(rest > s);
        retval.push_back(byte);
        s = rest;
        while (isspace(*s)) ++s;
    }
    return retval;
}
            
int main(int argc, char *argv[]) {
    ROSE_INITIALIZE;

    BinaryAnalysis::Unparser::Settings upSettings;
    P2::Engine engine;
    std::string fileName = parseCommandLine(argc, argv, engine, upSettings);
    std::ifstream input(fileName.c_str());
    
    P2::mlog[WARN].disable(); // warnings about empty memory map
    P2::Partitioner partitioner = engine.createPartitioner();
    Disassembler *disassembler = engine.obtainDisassembler();
    ASSERT_always_not_null(disassembler);
    BinaryAnalysis::Unparser::BasePtr unparser = partitioner.unparser();
    ASSERT_always_not_null(unparser);
    unparser->settings(upSettings);

    rose_addr_t va = 0;
    for (size_t lineNum = 1; true; ++lineNum) {
        std::string line = rose_getline(input);
        if (line.empty())
            break;
        std::cout <<(boost::format("L%|-4u| ") % lineNum) << line;
        std::vector<uint8_t> bytes = parse(line, va /*in,out*/);
        if (bytes.empty())
            continue;

        // Build virtual memory with just this one line of bytes
        MemoryMap::Ptr map = MemoryMap::instance();
        map->insert(AddressInterval::baseSize(va, bytes.size()),
                    MemoryMap::Segment(MemoryMap::StaticBuffer::instance(&bytes[0], bytes.size()),
                                       0, MemoryMap::EXECUTABLE));

        // Disassemble the buffer
        SgAsmInstruction *insn = NULL;
        try {
            insn = disassembler->disassembleOne(map, va);
            std::cout <<(boost::format("L%|-4u| ") % lineNum);
            unparser->unparse(std::cout, partitioner, insn);
            std::cout <<"\n";
        } catch (const std::runtime_error &e) {
            std::cerr <<"exception: " <<e.what() <<"\n";
            std::cerr <<"input was: " <<StringUtility::addrToString(va) <<":";
            BOOST_FOREACH (uint8_t byte, bytes)
                std::cerr <<(boost::format(" 0x%|02X|") % (unsigned)byte);
            std::cerr <<"\n";
            exit(1);
        }

        if (runSemantics) {
            // Process instruction semantics
            I2::BaseSemantics::RiscOperatorsPtr ops = partitioner.newOperators();
            I2::BaseSemantics::DispatcherPtr cpu = partitioner.newDispatcher(ops);
            cpu->processInstruction(insn);
            std::ostringstream ss;
            ss <<*ops;
            std::cout <<StringUtility::prefixLines(ss.str(), "       ");
        }
    }
}
