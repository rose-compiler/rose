static const char *purpose = "unit tests for non-modifiying semantic state reads";
static const char *description =
    "Initializes a machine state, then reads something that exists and something that doesn't exist, each time checking that "
    "the state hasn't changed.";

#include <rose.h>
#include <BinarySmtSolver.h>
#include <CommandLine.h>
#include <Registers.h>
#include <SymbolicSemantics2.h>
#include <sstream>
#include <string>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Rose::BinaryAnalysis::InstructionSemantics2;
using namespace Sawyer::Message::Common;

Sawyer::Message::Facility mlog;

void
parseCommandLine(int argc, char *argv[]) {
    using namespace Sawyer::CommandLine;
    Parser p = Rose::CommandLine::createEmptyParser(purpose, description);
    p.errorStream(::mlog[FATAL]);
    p.doc("Synopsis", "@prop{programName} [@v{switches}]");
    SwitchGroup switches = Rose::CommandLine::genericSwitches();
    switches.name("");

    if (!p.with(switches).parse(argc, argv).apply().unreachedArgs().empty()) {
        ::mlog[FATAL] <<"incorrect usage; see --help\n";
        exit(1);
    }
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&::mlog, "tool");
    parseCommandLine(argc, argv);

    // Create the machine state
    const RegisterDictionary *registers = RegisterDictionary::dictionary_amd64();
    const RegisterDescriptor EAX = *registers->lookup("eax");
    SmtSolverPtr solver;
    BaseSemantics::RiscOperatorsPtr ops = SymbolicSemantics::RiscOperators::instance(registers, solver);
    ASSERT_always_not_null(ops);
    ops->currentState()->memoryState()->set_byteOrder(ByteOrder::ORDER_LSB);

    // Initialize the machine state with some writes and get the string representation.
    BaseSemantics::SValuePtr eax = ops->number_(32, 1234);
    ops->writeRegister(EAX, eax);
    BaseSemantics::SValuePtr addr0 = ops->number_(32, 0x1000);
    BaseSemantics::SValuePtr mem0 = ops->number_(8, 123);
    ops->writeMemory(RegisterDescriptor(), addr0, mem0, ops->boolean_(true));
    std::ostringstream s0;
    s0 <<*ops;
    
    // Peek at parts of the state that exist
    BaseSemantics::SValuePtr v1 = ops->peekRegister(EAX, ops->undefined_(32));
    ASSERT_always_not_null(v1);
    ASSERT_always_require(v1->must_equal(eax, solver));
    BaseSemantics::SValuePtr mem1 = ops->peekMemory(RegisterDescriptor(), addr0, ops->undefined_(8));
    ASSERT_always_not_null(mem1);
    ASSERT_always_require(mem1->must_equal(mem0, solver));
    std::ostringstream s1;
    s1 <<*ops;
    ASSERT_always_require2(s0.str() == s1.str(), s1.str());

    // Peek at parts of the state that don't exist
    const RegisterDescriptor EBX = *registers->lookup("ebx");
    BaseSemantics::SValuePtr ebx = ops->undefined_(32);
    BaseSemantics::SValuePtr v2 = ops->peekRegister(EBX, ebx);
    ASSERT_always_not_null(v2);
    ASSERT_always_require(v2->must_equal(ebx, solver));
    BaseSemantics::SValuePtr addr2 = ops->number_(32, 0x2000);
    BaseSemantics::SValuePtr mem2init = ops->undefined_(8);
    BaseSemantics::SValuePtr mem2 = ops->peekMemory(RegisterDescriptor(), addr2, mem2init);
    ASSERT_always_not_null(mem2);
    ASSERT_always_require(mem2->must_equal(mem2init, solver));
    std::ostringstream s2;
    s2 <<*ops;
    ASSERT_always_require2(s0.str() == s2.str(), s2.str());

    // Peek at parts of the state that partly exist.
    const RegisterDescriptor RAX = *registers->lookup("rax");
    BaseSemantics::SValuePtr zero64 = ops->number_(64, 0);
    BaseSemantics::SValuePtr v3 = ops->peekRegister(RAX, zero64);
    ASSERT_always_not_null(v3);
    ASSERT_always_require(v3->must_equal(ops->number_(64, 1234), solver));
    BaseSemantics::SValuePtr zero32 = ops->number_(32, 0);
    BaseSemantics::SValuePtr mem3ans = ops->number_(32, 123);
    BaseSemantics::SValuePtr mem3 = ops->peekMemory(RegisterDescriptor(), addr0, zero32);
    ASSERT_always_not_null(mem3);
    ASSERT_always_require(mem3->must_equal(mem3ans, solver));
    std::ostringstream s3;
    s3 <<*ops;
    ASSERT_always_require2(s0.str() == s3.str(), s3.str());

    std::cout <<s3.str();
}
