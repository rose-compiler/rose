#include <rose.h>
#include <NativeSemantics.h>

using namespace Rose::BinaryAnalysis;
namespace IS = Rose::BinaryAnalysis::InstructionSemantics2;
namespace BaseSemantics = IS::BaseSemantics;
namespace NativeSemantics = IS::NativeSemantics;

int main(int argc, char *argv[]) {
    ASSERT_require(argc >= 2);

    Debugger::Specimen exe(argv[1]);
    exe.arguments(std::vector<std::string>(argv+2, argv+argc));

    NativeSemantics::DispatcherPtr cpu = NativeSemantics::Dispatcher::instance(exe);
    ASSERT_not_null(cpu);
    BaseSemantics::RiscOperatorsPtr ops = cpu->get_operators();

    const RegisterDescriptor IP = cpu->instructionPointerRegister();

    while (true) {
        rose_addr_t va = ops->peekRegister(IP, ops->undefined_(IP.nBits()))->get_number();
        //std::cerr <<Rose::StringUtility::addrToString(va) <<"\n";
        cpu->processInstruction(va);
    }
}

