#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/SharedMemory.h>

#include <Rose/BinaryAnalysis/Concolic/Architecture.h>
#include <Rose/BinaryAnalysis/Concolic/Callback.h>
#include <Rose/BinaryAnalysis/Concolic/ConcolicExecutor.h>
#include <Rose/BinaryAnalysis/Concolic/ExecutionEvent.h>
#include <Rose/BinaryAnalysis/Concolic/InputVariables.h>
#include <Rose/BinaryAnalysis/Concolic/TestCase.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/SValue.h>
#include <Rose/BinaryAnalysis/SymbolicExpression.h>
#include <Rose/StringUtility/Diagnostics.h>

using namespace Sawyer::Message::Common;

namespace IS = Rose::BinaryAnalysis::InstructionSemantics;
namespace BS = IS::BaseSemantics;

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SharedMemoryContext
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SharedMemoryContext::SharedMemoryContext(const Architecture::Ptr &architecture, const ExecutionEvent::Ptr &sharedMemoryEvent)
    : phase(ConcolicPhase::REPLAY), architecture(architecture), ip(sharedMemoryEvent->instructionPointer()),
      accessedVas(sharedMemoryEvent->memoryLocation()), direction(IoDirection::READ), sharedMemoryEvent(sharedMemoryEvent),
      valueRead(sharedMemoryEvent->value()) {
    ASSERT_not_null(architecture);
    ASSERT_not_null(sharedMemoryEvent);
    ASSERT_require(sharedMemoryEvent->action() == ExecutionEvent::Action::OS_SHARED_MEMORY);
}

SharedMemoryContext::SharedMemoryContext(const Architecture::Ptr &architecture, const Emulation::RiscOperators::Ptr &ops,
                                         const ExecutionEvent::Ptr &sharedMemoryEvent)
    : phase(ConcolicPhase::EMULATION), architecture(architecture), ops(ops), ip(sharedMemoryEvent->instructionPointer()),
      accessedVas(sharedMemoryEvent->memoryLocation()), direction(IoDirection::READ), sharedMemoryEvent(sharedMemoryEvent),
      valueRead(sharedMemoryEvent->inputVariable()) {
    ASSERT_not_null(architecture);
    ASSERT_not_null(ops);
    ASSERT_not_null(sharedMemoryEvent);
}

SharedMemoryContext::SharedMemoryContext(const Architecture::Ptr &architecture, const Emulation::RiscOperators::Ptr &ops,
                                         Address accessingInstructionVa, Address accessedVa, const SymbolicExpression::Ptr &value)
    : phase(ConcolicPhase::EMULATION), architecture(architecture), ops(ops), ip(accessingInstructionVa),
      accessedVas(AddressInterval::baseSize(accessedVa, (value->nBits()+7)/8)), direction(IoDirection::WRITE),
      valueWritten(value) {
    ASSERT_not_null(architecture);
    ASSERT_not_null(ops);
    ASSERT_not_null(value);
}

SharedMemoryContext::~SharedMemoryContext() {}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SharedMemoryCallback
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static SAWYER_THREAD_TRAITS::Mutex registryMutex;
static std::vector<SharedMemoryCallback::Ptr> registry;
static boost::once_flag registryInitFlag = BOOST_ONCE_INIT;

static void
initRegistryHelper() {
    SAWYER_THREAD_TRAITS::LockGuard lock(registryMutex);
    registry.push_back(Callback::MemoryExit::factory());
    registry.push_back(Callback::MemoryInput::factory());
    registry.push_back(Callback::MemoryTime::factory());
}

static void
initRegistry() {
    boost::call_once(&initRegistryHelper, registryInitFlag);
}

SharedMemoryCallback::~SharedMemoryCallback() {}

SharedMemoryCallback::SharedMemoryCallback(const std::string &name)
    : name_(name) {
    ASSERT_forbid(name.empty());
}

SharedMemoryCallback::SharedMemoryCallback(const AddressInterval &where, const std::string &name)
    : registrationVas_(where), name_(name) {
    ASSERT_forbid(name.empty());
}

void
SharedMemoryCallback::registerFactory(const Ptr &factory) {
    ASSERT_not_null(factory);
    ASSERT_require(factory->isFactory());
    initRegistry();
    SAWYER_THREAD_TRAITS::LockGuard lock(registryMutex);
    registry.push_back(factory);
}

bool
SharedMemoryCallback::deregisterFactory(const Ptr &factory) {
    ASSERT_not_null(factory);
    ASSERT_require(factory->isFactory());
    initRegistry();
    SAWYER_THREAD_TRAITS::LockGuard lock(registryMutex);
    for (auto iter = registry.rbegin(); iter != registry.rend(); ++iter) {
        if (*iter == factory) {
            registry.erase(std::next(iter).base());
            return true;
        }
    }
    return false;
}

std::vector<SharedMemoryCallback::Ptr>
SharedMemoryCallback::registeredFactories() {
    initRegistry();
    std::vector<Ptr> retval;
    SAWYER_THREAD_TRAITS::LockGuard lock(registryMutex);
    retval.reserve(registry.size());
    for (const Ptr &factory: registry)
        retval.push_back(factory);
    return retval;
}

bool
SharedMemoryCallback::matchFactory(const Yaml::Node &config) const {
    ASSERT_require(isFactory());
    return config["driver"].as<std::string>() == name();
}

SharedMemoryCallback::Ptr
SharedMemoryCallback::forge(const AddressInterval &where, const Yaml::Node &config) {
    ASSERT_forbid(where.isEmpty());
    initRegistry();
    SAWYER_THREAD_TRAITS::LockGuard lock(registryMutex);
    for (auto factory = registry.rbegin(); factory != registry.rend(); ++factory) {
        if ((*factory)->matchFactory(config))
            return (*factory)->instanceFromFactory(where, config);
    }
    return {};
}

SharedMemoryCallback::Ptr
SharedMemoryCallback::instanceFromFactory(const AddressInterval&, const Yaml::Node&) const {
    ASSERT_not_reachable("subclasses that use factories must implement this method; optional otherwise");
}

bool
SharedMemoryCallback::isFactory() const {
    return registrationVas_.isEmpty();
}

const std::string&
SharedMemoryCallback::name() const {
    return name_;
}

void
SharedMemoryCallback::name(const std::string &s) {
    ASSERT_forbid(s.empty());
    name_ = s;
}

const AddressInterval&
SharedMemoryCallback::registrationVas() const {
    return registrationVas_;
}

void
SharedMemoryCallback::registrationVas(const AddressInterval &i) {
    registrationVas_ = i;
}

void
SharedMemoryCallback::hello(const SharedMemoryContext &ctx) const {
    Sawyer::Message::Stream out = mlog[WHERE] ? mlog[WHERE] : mlog[DEBUG];
    if (out) {
        switch (ctx.phase) {
            case ConcolicPhase::REPLAY:
                out <<"replaying ";
                break;
            case ConcolicPhase::EMULATION:
                out <<"calling ";
                break;
            case ConcolicPhase::POST_EMULATION:
                out <<"finishing ";
                break;
        }
        out <<"shared memory \"" <<name() <<"\" at instruction " <<StringUtility::addrToString(ctx.ip)
            <<", addresses " <<StringUtility::addrToString(ctx.accessedVas)
            <<" (" <<StringUtility::plural(ctx.accessedVas.size(), "bytes") <<")\n";
        if (mlog[DEBUG] && ConcolicPhase::REPLAY == ctx.phase) {
            ASSERT_not_null(ctx.sharedMemoryEvent);
            if (SymbolicExpression::Ptr value = ctx.sharedMemoryEvent->value()) {
                SAWYER_MESG(mlog[DEBUG]) <<"  value = " <<toString(value) <<"\n";
            } else {
                SAWYER_MESG(mlog[DEBUG]) <<"  no concrete value (treated as non-shared memory)\n";
            }
        }
    }
}

void
SharedMemoryCallback::normalRead(SharedMemoryContext &ctx) const {
    SAWYER_MESG(mlog[DEBUG]) <<"    canceled: this read will be treated as non-shared memory\n";
    ctx.ops->inputVariables()->deactivate(ctx.sharedMemoryEvent);
    ctx.sharedMemoryEvent->variable(SymbolicExpression::Ptr());
    ctx.valueRead = SymbolicExpression::Ptr();
}

void
SharedMemoryCallback::notAnInput(SharedMemoryContext &ctx) const {
    SAWYER_MESG(mlog[DEBUG]) <<"    this shared memory read will not be treated as a test case input\n";
    ctx.ops->inputVariables()->deactivate(ctx.sharedMemoryEvent);
}

void
SharedMemoryCallback::returns(SharedMemoryContext &ctx, const SymbolicExpression::Ptr &value) const {
    ASSERT_not_null(value);
    SAWYER_MESG(mlog[DEBUG]) <<"    returning " <<toString(value) <<"\n";
    if (value->isConstant()) {
        ctx.ops->inputVariables()->deactivate(ctx.sharedMemoryEvent);
        ctx.sharedMemoryEvent->variable(SymbolicExpression::Ptr());
        ctx.valueRead = value;
        SAWYER_MESG(mlog[DEBUG]) <<"    this shared memory read will not be treated as a test case input\n";
    } else {
        ctx.valueRead = value;
    }
}

void
SharedMemoryCallback::returns(SharedMemoryContext &ctx, const BS::SValue::Ptr &value) const {
    SymbolicExpression::Ptr expr = IS::SymbolicSemantics::SValue::promote(value)->get_expression();
    returns(ctx, expr);
}

SymbolicExpression::Ptr
SharedMemoryCallback::inputVariable(const SharedMemoryContext &ctx) const {
    return ctx.sharedMemoryEvent->inputVariable();
}

bool
SharedMemoryCallback::operator()(bool /*handled*/, SharedMemoryContext &ctx) {
    switch (ctx.phase) {
        case ConcolicPhase::REPLAY:
            playback(ctx);
            return true;
        case ConcolicPhase::EMULATION:
            handlePreSharedMemory(ctx);
            return true;
        case ConcolicPhase::POST_EMULATION:
            handlePostSharedMemory(ctx);
            return true;
    }
    return false;
}

} // namespace
} // namespace
} // namespace

#endif
