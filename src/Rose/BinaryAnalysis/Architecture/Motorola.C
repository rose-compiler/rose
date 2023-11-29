#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/Motorola.h>

#include <Rose/BinaryAnalysis/Disassembler/M68k.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherM68k.h>
#include <Rose/BinaryAnalysis/Partitioner2/ModulesM68k.h>
#include <Rose/BinaryAnalysis/Unparser/M68k.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

Motorola::Motorola(const std::string &name)
    : Base(name, 4, ByteOrder::ORDER_MSB) {}

Motorola::~Motorola() {}

const CallingConvention::Dictionary&
Motorola::callingConventions() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    if (!callingConventions_.isCached()) {
        CallingConvention::Dictionary dict;

        // https://m680x0.github.io/doc/abi.html
        const RegisterDictionary::Ptr regdict = registerDictionary();
        const RegisterDescriptor SP = regdict->stackPointerRegister();

        auto cc = CallingConvention::Definition::instance(bitsPerWord(), "sysv", "m68k-sysv", regdict);

        //==== Address locations ====
        cc->instructionPointerRegister(regdict->instructionPointerRegister());
        cc->returnAddressLocation(ConcreteLocation(SP, 0));

        //==== Stack characteristics ====
        cc->stackPointerRegister(SP);
        cc->stackDirection(CallingConvention::StackDirection::GROWS_DOWN);
        cc->nonParameterStackSize(bytesPerWord()); // return address

        //====  Function parameters ====
        // All parameters are passed on the stack.
        cc->stackParameterOrder(CallingConvention::StackParameterOrder::RIGHT_TO_LEFT);
        cc->stackCleanup(CallingConvention::StackCleanup::BY_CALLER);

        //==== Other inputs ====


        //==== Return values ====
        cc->appendOutputParameter(regdict->findOrThrow("d0"));
        cc->appendOutputParameter(regdict->findOrThrow("a0"));
        cc->appendOutputParameter(regdict->findOrThrow("fp0"));
        cc->appendOutputParameter(SP);                  // final value is usually one word greater than initial value

        //====  Scratch registers ====
        // (i.e., modified, not callee-saved, not return registers
        cc->scratchRegisters().insert(regdict->findOrThrow("d0"));
        cc->scratchRegisters().insert(regdict->findOrThrow("d1"));
        cc->scratchRegisters().insert(regdict->findOrThrow("a0"));
        cc->scratchRegisters().insert(regdict->findOrThrow("a1"));
        cc->scratchRegisters().insert(regdict->findOrThrow("pc"));
        cc->scratchRegisters().insert(regdict->findOrThrow("ccr"));
        cc->scratchRegisters().insert(regdict->findOrThrow("fp0"));
        cc->scratchRegisters().insert(regdict->findOrThrow("fp1"));


        //==== Callee-saved registers
        // Everything else
        RegisterParts regParts = regdict->getAllParts() - cc->getUsedRegisterParts();
        std::vector<RegisterDescriptor> registers = regParts.extract(regdict);
        cc->calleeSavedRegisters().insert(registers.begin(), registers.end());

        dict.push_back(cc);

        callingConventions_ = dict;
    }

    return callingConventions_.get();
}

Unparser::Base::Ptr
Motorola::newUnparser() const {
    return Unparser::M68k::instance(shared_from_this());
}

InstructionSemantics::BaseSemantics::DispatcherPtr
Motorola::newInstructionDispatcher(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr &ops) const {
    return InstructionSemantics::DispatcherM68k::instance(shared_from_this(), ops);
}

std::vector<Partitioner2::FunctionPrologueMatcher::Ptr>
Motorola::functionPrologueMatchers(const Partitioner2::EnginePtr&) const {
    std::vector<Partitioner2::FunctionPrologueMatcher::Ptr> retval;
    retval.push_back(Partitioner2::ModulesM68k::MatchLink::instance());
    return retval;
}

std::vector<Partitioner2::BasicBlockCallback::Ptr>
Motorola::basicBlockCreationHooks(const Partitioner2::EnginePtr&) const {
    std::vector<Partitioner2::BasicBlockCallback::Ptr> retval;
    retval.push_back(Partitioner2::ModulesM68k::SwitchSuccessors::instance());
    return retval;
}

} // namespace
} // namespace
} // namespace

#endif
