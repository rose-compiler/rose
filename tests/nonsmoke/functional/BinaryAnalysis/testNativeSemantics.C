#include <rose.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS) && defined(ROSE_ENABLE_DEBUGGER_LINUX)

#include <Rose/BinaryAnalysis/Debugger/Linux.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/NativeSemantics.h>

using namespace Rose::BinaryAnalysis;
namespace IS = Rose::BinaryAnalysis::InstructionSemantics;
namespace BaseSemantics = IS::BaseSemantics;
namespace NativeSemantics = IS::NativeSemantics;

int main(int argc, char *argv[]) {
    ASSERT_require(argc >= 2);

    Debugger::Linux::Specimen exe(argv[1]);
    exe.arguments(std::vector<std::string>(argv+2, argv+argc));

    NativeSemantics::Dispatcher::Ptr cpu = NativeSemantics::Dispatcher::instance(exe);
    ASSERT_not_null(cpu);
    BaseSemantics::RiscOperators::Ptr ops = cpu->operators();

    const RegisterDescriptor IP = cpu->instructionPointerRegister();

    while (true) {
        rose_addr_t va = ops->peekRegister(IP, ops->undefined_(IP.nBits()))->toUnsigned().get();
        //std::cerr <<Rose::StringUtility::addrToString(va) <<"\n";
        cpu->processInstruction(va);
    }
}

#else

#include <iostream>

int main(int, char *argv[]) {
    std::cout <<argv[0] <<": not tested for this configuration of ROSE\n";
}

#endif
