#include <featureTests.h>
#ifdef ROSE_ENABLE_MODEL_CHECKER
#include <Rose/BinaryAnalysis/ModelChecker/ExternalFunctionUnit.h>

#include <Rose/As.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherAarch32.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherAarch64.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherM68k.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherPowerpc.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherX86.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/RiscOperators.h>
#include <Rose/BinaryAnalysis/Partitioner2/Function.h>
#include <Rose/BinaryAnalysis/ModelChecker/SemanticCallbacks.h>
#include <Rose/BinaryAnalysis/ModelChecker/Settings.h>
#include <Rose/BinaryAnalysis/ModelChecker/Tag.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <Rose/BinaryAnalysis/RegisterNames.h>
#include <Rose/Sarif/Location.h>
#include <Rose/StringUtility/Escape.h>
#include <Rose/StringUtility/NumberToString.h>

#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

using namespace Sawyer::Message::Common;
namespace BS = Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics;
namespace CC = Rose::BinaryAnalysis::CallingConvention;
namespace IS = Rose::BinaryAnalysis::InstructionSemantics;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

ExternalFunctionUnit::ExternalFunctionUnit(const P2::Function::Ptr &function, const SourceLocation &sloc)
    : ExecutionUnit(sloc), function_(function) {}

ExternalFunctionUnit::~ExternalFunctionUnit() {}

ExternalFunctionUnit::Ptr
ExternalFunctionUnit::instance(const P2::Function::Ptr &function, const SourceLocation &sloc) {
    ASSERT_not_null(function);
    return Ptr(new ExternalFunctionUnit(function, sloc));
}

P2::Function::Ptr
ExternalFunctionUnit::function() const {
    // No lock necessary since the function pointer is read-only
    ASSERT_not_null(function_);
    return function_;
}

std::string
ExternalFunctionUnit::printableName() const {
    // No lock necessary since the function pointer is read-only
    return "extern " + function_->printableName();
}

void
ExternalFunctionUnit::printSteps(const Settings::Ptr&, std::ostream &out, const std::string &prefix, size_t stepOrigin,
                                 size_t maxSteps) const {
    if (maxSteps > 0) {
        if (sourceLocation())
            out <<prefix <<"from " <<sourceLocation() <<"\n";

        out <<(boost::format("%s#%-6d no definition available\n")
               %prefix
               %stepOrigin);
    }
}

void
ExternalFunctionUnit::toYamlHeader(const Settings::Ptr&, std::ostream &out, const std::string &prefix1) const {
    out <<prefix1 <<"vertex-type: extern-function\n";
    if (auto va = address()) {
        std::string prefix(prefix1.size(), ' ');
        out <<prefix <<"vertex-address: " <<StringUtility::addrToString(*va) <<"\n";
    }
}

void
ExternalFunctionUnit::toYamlSteps(const Settings::Ptr&, std::ostream &out, const std::string &prefix1,
                                  size_t /*stepOrigin*/, size_t maxSteps) const {
    if (maxSteps > 0) {
        out <<prefix1 <<"definition: none available\n";

        if (sourceLocation()) {
            std::string prefix(prefix1.size(), ' ');
            out <<prefix <<"    source-file: " <<StringUtility::yamlEscape(sourceLocation().fileName().string()) <<"\n"
                <<prefix <<"    source-line: " <<sourceLocation().line() <<"\n";
            if (sourceLocation().column())
                out <<prefix <<"    source-column: " <<*sourceLocation().column() <<"\n";
        }
    }
}

std::vector<Sarif::Location::Ptr>
ExternalFunctionUnit::toSarif(const size_t maxSteps) const {
    std::vector<Sarif::Location::Ptr> retval;

    if (maxSteps > 0) {
        retval.push_back(Sarif::Location::instance("file:///proc/self/mem", address().orElse(0), "extern function"));

        if (const auto sloc = sourceLocation())
            retval.push_back(Sarif::Location::instance(sloc));
    }

    return retval;
}

size_t
ExternalFunctionUnit::nSteps() const {
    return 1;
}

Sawyer::Optional<rose_addr_t>
ExternalFunctionUnit::address() const {
    // No lock necessary since the function pointer is read-only
    ASSERT_not_null(function_);
    return function_->address();
}

BS::SValue::Ptr
ExternalFunctionUnit::readLocation(const BS::Dispatcher::Ptr &cpu, const ConcreteLocation &param,
                                   const BS::SValue::Ptr &dflt) {
    ASSERT_not_null(cpu);
    BS::RiscOperators::Ptr ops = cpu->operators();

    switch (param.type()) {
        case ConcreteLocation::REGISTER:
            return ops->readRegister(param.reg());

        case ConcreteLocation::RELATIVE: {
            const RegisterDescriptor baseReg = param.reg();
            BS::SValue::Ptr base = ops->readRegister(baseReg);
            BS::SValue::Ptr offset = ops->number_(baseReg.nBits(), BitOps::signExtend(param.offset(), baseReg.nBits()));
            BS::SValue::Ptr stackVa = ops->add(base, offset);
            return ops->readMemory(RegisterDescriptor(), stackVa, dflt, ops->boolean_(true));
        }

        case ConcreteLocation::ABSOLUTE: {
            size_t nBits = cpu->stackPointerRegister().nBits();
            BS::SValue::Ptr va = ops->number_(nBits, param.address());
            ops->readMemory(RegisterDescriptor(), va, dflt, ops->boolean_(true));
            break;
        }

        case ConcreteLocation::NO_LOCATION:
            break;
    }
    ASSERT_not_reachable("invalid parameter location");
}

void
ExternalFunctionUnit::writeLocation(const BS::Dispatcher::Ptr &cpu, const ConcreteLocation &param,
                                    const BS::SValue::Ptr &value) {
    ASSERT_not_null(cpu);
    BS::RiscOperators::Ptr ops = cpu->operators();

    switch (param.type()) {
        case ConcreteLocation::REGISTER:
            return ops->writeRegister(param.reg(), value);

        case ConcreteLocation::RELATIVE: {
            const RegisterDescriptor baseReg = param.reg();
            BS::SValue::Ptr base = ops->readRegister(baseReg);
            BS::SValue::Ptr offset = ops->number_(baseReg.nBits(), BitOps::signExtend(param.offset(), baseReg.nBits()));
            BS::SValue::Ptr stackVa = ops->add(base, offset);
            return ops->writeMemory(RegisterDescriptor(), stackVa, value, ops->boolean_(true));
        }

        case ConcreteLocation::ABSOLUTE: {
            size_t nBits = cpu->stackPointerRegister().nBits();
            BS::SValue::Ptr va = ops->number_(nBits, param.address());
            ops->writeMemory(RegisterDescriptor(), va, value, ops->boolean_(true));
            break;
        }

        case ConcreteLocation::NO_LOCATION:
            break;
    }
    ASSERT_not_reachable("invalid parameter location");
}

void
ExternalFunctionUnit::clearReturnValues(const BS::Dispatcher::Ptr &cpu) {
    ASSERT_not_null(cpu);
    BS::RiscOperators::Ptr ops = cpu->operators();
    RegisterDictionary::Ptr regDict = cpu->registerDictionary();

    // Set return value locations to unknown values. Using calling convention analysis is the right way,
    // but if that's not available fall back to the stupid closed list of dynamic casts.
    if (CC::Definition::Ptr ccDefn = function_->callingConventionDefinition()) {
        for (const ConcreteLocation &param: ccDefn->outputParameters()) {
            size_t nBits = 0;
            switch (param.type()) {
                case ConcreteLocation::REGISTER:
                    if (param.reg() != cpu->stackPointerRegister() &&
                        param.reg() != cpu->stackFrameRegister() &&
                        param.reg() != cpu->callReturnRegister()) {
                        nBits = param.reg().nBits();
                    }
                    break;
                case ConcreteLocation::RELATIVE:
                case ConcreteLocation::ABSOLUTE:
                    nBits = cpu->stackPointerRegister().nBits();
                    break;
                case ConcreteLocation::NO_LOCATION:
                    ASSERT_not_reachable("invalid parameter location");
            }
            if (nBits != 0) {
                if (mlog[DEBUG]) {
                    mlog[DEBUG] <<"    clearing output value ";
                    param.print(mlog[DEBUG], regDict);
                    mlog[DEBUG] <<" (" <<ccDefn->name() <<" calling convention)\n";
                }
                BS::SValue::Ptr undef = ops->undefined_(nBits);
                writeLocation(cpu, param, undef);
            }
        }
    } else {
        std::vector<RegisterDescriptor> retRegs;
        if (as<IS::DispatcherX86>(cpu)) {
            RegisterDescriptor r;
            if ((r = regDict->find("rax")) || (r = regDict->find("eax")) || (r = regDict->find("ax")))
                retRegs.push_back(r);
        } else if (as<IS::DispatcherM68k>(cpu)) {
            // FIXME[Robb Matzke 2021-04-15]: m68k also typically has other return registers
            if (RegisterDescriptor r = regDict->find("d0"))
                retRegs.push_back(r);
        } else if (as<IS::DispatcherPowerpc>(cpu)) {
            if (RegisterDescriptor r = regDict->find("r3"))
                retRegs.push_back(r);
            if (RegisterDescriptor r = regDict->find("r4"))
                retRegs.push_back(r);
#ifdef ROSE_ENABLE_ASM_AARCH32
        } else if (as<IS::DispatcherAarch32>(cpu)) {
            if (RegisterDescriptor r = regDict->find("r0"))
                retRegs.push_back(r);
#endif
#ifdef ROSE_ENABLE_ASM_AARCH64
        } else if (as<IS::DispatcherAarch64>(cpu)) {
            if (RegisterDescriptor r = regDict->find("x0"))
                retRegs.push_back(r);
#endif
        } else {
            ASSERT_not_implemented("function return value register is not implemented for this ISA/ABI");
        }

        for (RegisterDescriptor reg: retRegs) {
            RegisterNames regNames(regDict);
            SAWYER_MESG(mlog[DEBUG]) <<"    clearing return value " <<regNames(reg) <<" (hard coded)\n";
            BS::SValue::Ptr undef = ops->undefined_(reg.nBits());
            ops->writeRegister(reg, undef);
        }
    }
}

void
ExternalFunctionUnit::simulateReturn(const BS::Dispatcher::Ptr &cpu) {
    ASSERT_not_null(cpu);
    BS::RiscOperators::Ptr ops = cpu->operators();

    // FIXME[Robb Matzke 2021-04-02]: dynamic casting the CPU to each known architecture is certainly not the right
    // way to do this! These operations should be abstracted in the BaseSemantics::Dispatcher class as virtual functions.
    // Most of this has been copied from the FeasiblePath class.
    if (as<IS::DispatcherPowerpc>(cpu)) {
        // PowerPC calling convention stores the return address in the link register (LR)
        const RegisterDescriptor LR = cpu->callReturnRegister();
        ASSERT_forbid(LR.isEmpty());
        BS::SValue::Ptr returnTarget = ops->readRegister(LR, ops->undefined_(LR.nBits()));
        ops->writeRegister(cpu->instructionPointerRegister(), returnTarget);

    } else if (as<IS::DispatcherX86>(cpu) || as<IS::DispatcherM68k>(cpu)) {
        // x86, amd64, and m68k store the return address at the top of the stack
        const RegisterDescriptor SP = cpu->stackPointerRegister();
        ASSERT_forbid(SP.isEmpty());
        BS::SValue::Ptr stackPointer = ops->readRegister(SP, ops->undefined_(SP.nBits()));
        BS::SValue::Ptr returnTarget = ops->readMemory(RegisterDescriptor(), stackPointer,
                                                     ops->undefined_(stackPointer->nBits()),
                                                     ops->boolean_(true));
        ops->writeRegister(cpu->instructionPointerRegister(), returnTarget);
        stackPointer = ops->add(stackPointer, ops->number_(stackPointer->nBits(), stackPointer->nBits()/8));
        ops->writeRegister(cpu->stackPointerRegister(), stackPointer);
#ifdef ROSE_ENABLE_ASM_AARCH64
    } else if (as<IS::DispatcherAarch64>(cpu)) {
        // Return address is in the link register, lr
        const RegisterDescriptor LR = cpu->callReturnRegister();
        ASSERT_forbid(LR.isEmpty());
        BS::SValue::Ptr returnTarget = ops->readRegister(LR, ops->undefined_(LR.nBits()));
        ops->writeRegister(cpu->instructionPointerRegister(), returnTarget);
#endif
#ifdef ROSE_ENABLE_ASM_AARCH32
    } else if (as<IS::DispatcherAarch32>(cpu)) {
        // Return address is in the link register, lr
        const RegisterDescriptor LR = cpu->callReturnRegister();
        ASSERT_forbid(LR.isEmpty());
        BS::SValue::Ptr returnTarget = ops->readRegister(LR, ops->undefined_(LR.nBits()));
        ops->writeRegister(cpu->instructionPointerRegister(), returnTarget);
#endif
    }
}

std::vector<Tag::Ptr>
ExternalFunctionUnit::execute(const Settings::Ptr &settings, const SemanticCallbacks::Ptr &semantics,
                              const BS::RiscOperators::Ptr &ops) {
    ASSERT_not_null(settings);
    ASSERT_not_null(semantics);
    ASSERT_not_null(ops);
    std::vector<Tag::Ptr> tags;
    BS::Dispatcher::Ptr cpu = semantics->createDispatcher(ops);
    BS::Formatter fmt;
    fmt.set_line_prefix("      ");

    SAWYER_MESG_FIRST(mlog[WHERE], mlog[TRACE], mlog[DEBUG]) <<"  executing " <<printableName() <<"\n";
    if (mlog[DEBUG] && settings->showInitialStates)
        mlog[DEBUG] <<"    initial state\n" <<(*ops->currentState() + fmt);

    clearReturnValues(cpu);
    simulateReturn(cpu);

    if (mlog[DEBUG] && settings->showFinalStates)
        mlog[DEBUG] <<"    final state\n" <<(*ops->currentState() + fmt);

    return {};
}

} // namespace
} // namespace
} // namespace

#endif
