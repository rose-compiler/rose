#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/Powerpc.h>

#include <Rose/BinaryAnalysis/Disassembler/Powerpc.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherPowerpc.h>
#include <Rose/BinaryAnalysis/Partitioner2/ModulesPowerpc.h>
#include <Rose/BinaryAnalysis/Unparser/Powerpc.h>

#include <boost/lexical_cast.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

Powerpc::Powerpc(size_t bytesPerWord, ByteOrder::Endianness byteOrder)
    : Base("ppc" + boost::lexical_cast<std::string>(8*bytesPerWord) +
           (ByteOrder::ORDER_MSB == byteOrder ? "-be" : (ByteOrder::ORDER_LSB == byteOrder ? "-el" : "")),
           bytesPerWord, byteOrder) {}

Powerpc::~Powerpc() {}

const CallingConvention::Dictionary&
Powerpc::callingConventions() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    if (!callingConventions_.isCached()) {
        CallingConvention::Dictionary dict;
        dict.push_back(cc_ibm(bitsPerWord()));
        callingConventions_ = dict;
    }

    return callingConventions_.get();
}

Disassembler::Base::Ptr
Powerpc::newInstructionDecoder() const {
    return Disassembler::Powerpc::instance(shared_from_this());
}

Unparser::Base::Ptr
Powerpc::newUnparser() const {
    return Unparser::Powerpc::instance(shared_from_this());
}

InstructionSemantics::BaseSemantics::DispatcherPtr
Powerpc::newInstructionDispatcher(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr &ops) const {
    return InstructionSemantics::DispatcherPowerpc::instance(shared_from_this(), ops);
}

std::vector<Partitioner2::FunctionPrologueMatcher::Ptr>
Powerpc::functionPrologueMatchers(const Partitioner2::EnginePtr&) const {
    std::vector<Partitioner2::FunctionPrologueMatcher::Ptr> retval;
    retval.push_back(Partitioner2::ModulesPowerpc::MatchStwuPrologue::instance());
    return retval;
}

CallingConvention::Definition::Ptr
Powerpc::cc_ibm(size_t bitsPerWord) const {
    // See https://www.ibm.com/support/knowledgecenter/en/ssw_aix_72/com.ibm.aix.alangref/idalangref_reg_use_conv.htm
    RegisterDictionary::Ptr regdict = registerDictionary();
    auto cc = CallingConvention::Definition::instance(bitsPerWord, "IBM",
                                                      "powerpc-" + boost::lexical_cast<std::string>(bitsPerWord) + " ibm",
                                                      regdict);

    const RegisterDescriptor SP = regdict->stackPointerRegister();

    //==== Address locations ====
    cc->instructionPointerRegister(regdict->instructionPointerRegister());
    cc->returnAddressLocation(ConcreteLocation(regdict->callReturnRegister()));

    //==== Stack characteristics ====
    cc->stackPointerRegister(SP);
    cc->stackDirection(CallingConvention::StackDirection::GROWS_DOWN);
    cc->nonParameterStackSize(0);                       // return address is in link register

    //==== Function parameters ====
    cc->appendInputParameter(regdict->findOrThrow("r3"));
    cc->appendInputParameter(regdict->findOrThrow("r4"));
    cc->appendInputParameter(regdict->findOrThrow("r5"));
    cc->appendInputParameter(regdict->findOrThrow("r6"));
    cc->appendInputParameter(regdict->findOrThrow("r7"));
    cc->appendInputParameter(regdict->findOrThrow("r8"));
    cc->appendInputParameter(regdict->findOrThrow("r9"));
    cc->appendInputParameter(regdict->findOrThrow("r10"));

    cc->appendInputParameter(regdict->findOrThrow("f1"));
    cc->appendInputParameter(regdict->findOrThrow("f2"));
    cc->appendInputParameter(regdict->findOrThrow("f3"));
    cc->appendInputParameter(regdict->findOrThrow("f4"));
    cc->appendInputParameter(regdict->findOrThrow("f5"));
    cc->appendInputParameter(regdict->findOrThrow("f6"));
    cc->appendInputParameter(regdict->findOrThrow("f7"));
    cc->appendInputParameter(regdict->findOrThrow("f8"));
    cc->appendInputParameter(regdict->findOrThrow("f9"));
    cc->appendInputParameter(regdict->findOrThrow("f10"));
    cc->appendInputParameter(regdict->findOrThrow("f11"));
    cc->appendInputParameter(regdict->findOrThrow("f12"));
    cc->appendInputParameter(regdict->findOrThrow("f13"));

    // Stack is generally not used for passing arguments
    cc->stackParameterOrder(CallingConvention::StackParameterOrder::RIGHT_TO_LEFT);
    cc->stackCleanup(CallingConvention::StackCleanup::BY_CALLER);

    //==== Return values ====
    cc->appendOutputParameter(regdict->findOrThrow("r3")); // primary return
    cc->appendOutputParameter(regdict->findOrThrow("r4")); // secondary return

    cc->appendOutputParameter(regdict->findOrThrow("f1"));
    cc->appendOutputParameter(regdict->findOrThrow("f2"));
    cc->appendOutputParameter(regdict->findOrThrow("f3"));
    cc->appendOutputParameter(regdict->findOrThrow("f4"));

    //==== Scratch registers ====
    // function arguments that are not return values, plus others.
    cc->scratchRegisters().insert(regdict->findOrThrow("r0"));
    cc->scratchRegisters().insert(regdict->findOrThrow("r5"));
    cc->scratchRegisters().insert(regdict->findOrThrow("r6"));
    cc->scratchRegisters().insert(regdict->findOrThrow("r7"));
    cc->scratchRegisters().insert(regdict->findOrThrow("r8"));
    cc->scratchRegisters().insert(regdict->findOrThrow("r9"));
    cc->scratchRegisters().insert(regdict->findOrThrow("r10"));
    cc->scratchRegisters().insert(regdict->findOrThrow("r11"));
    cc->scratchRegisters().insert(regdict->findOrThrow("r12"));

    cc->scratchRegisters().insert(regdict->findOrThrow("f0"));
    cc->scratchRegisters().insert(regdict->findOrThrow("f5"));
    cc->scratchRegisters().insert(regdict->findOrThrow("f6"));
    cc->scratchRegisters().insert(regdict->findOrThrow("f7"));
    cc->scratchRegisters().insert(regdict->findOrThrow("f8"));
    cc->scratchRegisters().insert(regdict->findOrThrow("f9"));
    cc->scratchRegisters().insert(regdict->findOrThrow("f10"));
    cc->scratchRegisters().insert(regdict->findOrThrow("f11"));
    cc->scratchRegisters().insert(regdict->findOrThrow("f12"));
    cc->scratchRegisters().insert(regdict->findOrThrow("f13"));

    cc->scratchRegisters().insert(regdict->findOrThrow("cr"));
    cc->scratchRegisters().insert(regdict->findOrThrow("fpscr"));
    cc->scratchRegisters().insert(regdict->instructionPointerRegister());

    //==== Callee-saved registers ====
    // Everything else
    RegisterParts regParts = regdict->getAllParts() - cc->getUsedRegisterParts();
    std::vector<RegisterDescriptor> registers = regParts.extract(regdict);
    cc->calleeSavedRegisters().insert(registers.begin(), registers.end());

    return cc;
}

} // namespace
} // namespace
} // namespace

#endif
