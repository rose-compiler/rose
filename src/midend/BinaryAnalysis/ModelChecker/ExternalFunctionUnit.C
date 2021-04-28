#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <BinaryAnalysis/ModelChecker/ExternalFunctionUnit.h>

#include <BaseSemanticsRiscOperators.h>
#include <BinaryAnalysis/ModelChecker/SemanticCallbacks.h>
#include <BinaryAnalysis/ModelChecker/Settings.h>
#include <BinaryAnalysis/ModelChecker/Tag.h>
#include <boost/format.hpp>
#include <Partitioner2/Function.h>

using namespace Sawyer::Message::Common;
namespace BS = Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics;
namespace CC = Rose::BinaryAnalysis::CallingConvention;
namespace IS = Rose::BinaryAnalysis::InstructionSemantics2;
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
ExternalFunctionUnit::printSteps(const Settings::Ptr &settings, std::ostream &out, const std::string &prefix,
                                 size_t stepOrigin, size_t maxSteps) const {
    if (maxSteps > 0) {
        if (sourceLocation())
            out <<prefix <<"from " <<sourceLocation() <<"\n";

        out <<(boost::format("%s#%-6d no definition available\n")
               %prefix
               %stepOrigin);
    }
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

BS::SValuePtr
ExternalFunctionUnit::readLocation(const BS::DispatcherPtr &cpu, const CC::ParameterLocation &param, const BS::SValuePtr &dflt) {
    ASSERT_not_null(cpu);
    BS::RiscOperatorsPtr ops = cpu->operators();

    switch (param.type()) {
        case CC::ParameterLocation::REGISTER:
            return ops->readRegister(param.reg());

        case CC::ParameterLocation::STACK: {
            const RegisterDescriptor baseReg = param.reg();
            BS::SValuePtr base = ops->readRegister(baseReg);
            BS::SValuePtr offset = ops->number_(baseReg.nBits(), BitOps::signExtend(param.offset(), baseReg.nBits()));
            BS::SValuePtr stackVa = ops->add(base, offset);
            return ops->readMemory(RegisterDescriptor(), stackVa, dflt, ops->boolean_(true));
        }

        case CC::ParameterLocation::ABSOLUTE: {
            size_t nBits = cpu->stackPointerRegister().nBits();
            BS::SValuePtr va = ops->number_(nBits, param.address());
            ops->readMemory(RegisterDescriptor(), va, dflt, ops->boolean_(true));
            break;
        }

        case CC::ParameterLocation::NO_LOCATION:
            break;
    }
    ASSERT_not_reachable("invalid parameter location");
}

void
ExternalFunctionUnit::writeLocation(const BS::DispatcherPtr &cpu, const CC::ParameterLocation &param, const BS::SValuePtr &value) {
    ASSERT_not_null(cpu);
    BS::RiscOperatorsPtr ops = cpu->operators();

    switch (param.type()) {
        case CC::ParameterLocation::REGISTER:
            return ops->writeRegister(param.reg(), value);

        case CC::ParameterLocation::STACK: {
            const RegisterDescriptor baseReg = param.reg();
            BS::SValuePtr base = ops->readRegister(baseReg);
            BS::SValuePtr offset = ops->number_(baseReg.nBits(), BitOps::signExtend(param.offset(), baseReg.nBits()));
            BS::SValuePtr stackVa = ops->add(base, offset);
            return ops->writeMemory(RegisterDescriptor(), stackVa, value, ops->boolean_(true));
        }

        case CC::ParameterLocation::ABSOLUTE: {
            size_t nBits = cpu->stackPointerRegister().nBits();
            BS::SValuePtr va = ops->number_(nBits, param.address());
            ops->writeMemory(RegisterDescriptor(), va, value, ops->boolean_(true));
            break;
        }

        case CC::ParameterLocation::NO_LOCATION:
            break;
    }
    ASSERT_not_reachable("invalid parameter location");
}

void
ExternalFunctionUnit::clearReturnValues(const BS::DispatcherPtr &cpu) {
    ASSERT_not_null(cpu);
    BS::RiscOperatorsPtr ops = cpu->operators();
    const RegisterDictionary *regDict = cpu->registerDictionary();

    // Set return value locations to unknown values. Using calling convention analysis is the right way,
    // but if that's not available fall back to the stupid closed list of dynamic casts.
    if (CC::Definition::Ptr ccDefn = function_->callingConventionDefinition()) {
        for (const CC::ParameterLocation &param: ccDefn->outputParameters()) {
            size_t nBits = 0;
            switch (param.type()) {
                case CC::ParameterLocation::REGISTER:
                    if (param.reg() != cpu->stackPointerRegister() &&
                        param.reg() != cpu->stackFrameRegister() &&
                        param.reg() != cpu->callReturnRegister()) {
                        nBits = param.reg().nBits();
                    }
                    break;
                case CC::ParameterLocation::STACK:
                case CC::ParameterLocation::ABSOLUTE:
                    nBits = cpu->stackPointerRegister().nBits();
                    break;
                case CC::ParameterLocation::NO_LOCATION:
                    ASSERT_not_reachable("invalid parameter location");
            }
            if (nBits != 0) {
                if (mlog[DEBUG]) {
                    mlog[DEBUG] <<"    clearing output value ";
                    param.print(mlog[DEBUG], regDict);
                    mlog[DEBUG] <<" (" <<ccDefn->name() <<" calling convention)\n";
                }
                BS::SValuePtr undef = ops->undefined_(nBits);
                writeLocation(cpu, param, undef);
            }
        }
    } else {
        std::vector<RegisterDescriptor> retRegs;
        if (boost::dynamic_pointer_cast<IS::DispatcherX86>(cpu)) {
            RegisterDescriptor r;
            if ((r = regDict->find("rax")) || (r = regDict->find("eax")) || (r = regDict->find("ax")))
                retRegs.push_back(r);
        } else if (boost::dynamic_pointer_cast<IS::DispatcherM68k>(cpu)) {
            // FIXME[Robb Matzke 2021-04-15]: m68k also typically has other return registers
            if (RegisterDescriptor r = regDict->find("d0"))
                retRegs.push_back(r);
        } else if (boost::dynamic_pointer_cast<IS::DispatcherPowerpc>(cpu)) {
            if (RegisterDescriptor r = regDict->find("r3"))
                retRegs.push_back(r);
            if (RegisterDescriptor r = regDict->find("r4"))
                retRegs.push_back(r);
#ifdef ROSE_ENABLE_ASM_AARCH32
        } else if (boost::dynamic_pointer_cast<IS::DispatcherAarch32>(cpu)) {
            if (RegisterDescriptor r = regDict->find("r0"))
                retRegs.push_back(r);
#endif
#ifdef ROSE_ENABLE_ASM_AARCH64
        } else if (boost::dynamic_pointer_cast<IS::DispatcherAarch64>(cpu)) {
            if (RegisterDescriptor r = regDict->find("x0"))
                retRegs.push_back(r);
#endif
        } else {
            ASSERT_not_implemented("function return value register is not implemented for this ISA/ABI");
        }

        for (RegisterDescriptor reg: retRegs) {
            RegisterNames regNames(regDict);
            SAWYER_MESG(mlog[DEBUG]) <<"    clearing return value " <<regNames(reg) <<" (hard coded)\n";
            BS::SValuePtr undef = ops->undefined_(reg.nBits());
            ops->writeRegister(reg, undef);
        }
    }
}

void
ExternalFunctionUnit::simulateReturn(const BS::DispatcherPtr &cpu) {
    ASSERT_not_null(cpu);
    BS::RiscOperatorsPtr ops = cpu->operators();

    // FIXME[Robb Matzke 2021-04-02]: dynamic casting the CPU to each known architecture is certainly not the right
    // way to do this! These operations should be abstracted in the BaseSemantics::Dispatcher class as virtual functions.
    // Most of this has been copied from the FeasiblePath class.
    if (boost::dynamic_pointer_cast<IS::DispatcherPowerpc>(cpu)) {
        // PowerPC calling convention stores the return address in the link register (LR)
        const RegisterDescriptor LR = cpu->callReturnRegister();
        ASSERT_forbid(LR.isEmpty());
        BS::SValuePtr returnTarget = ops->readRegister(LR, ops->undefined_(LR.nBits()));
        ops->writeRegister(cpu->instructionPointerRegister(), returnTarget);

    } else if (boost::dynamic_pointer_cast<IS::DispatcherX86>(cpu) ||
               boost::dynamic_pointer_cast<IS::DispatcherM68k>(cpu)) {
        // x86, amd64, and m68k store the return address at the top of the stack
        const RegisterDescriptor SP = cpu->stackPointerRegister();
        ASSERT_forbid(SP.isEmpty());
        BS::SValuePtr stackPointer = ops->readRegister(SP, ops->undefined_(SP.nBits()));
        BS::SValuePtr returnTarget = ops->readMemory(RegisterDescriptor(), stackPointer,
                                                     ops->undefined_(stackPointer->nBits()),
                                                     ops->boolean_(true));
        ops->writeRegister(cpu->instructionPointerRegister(), returnTarget);
        stackPointer = ops->add(stackPointer, ops->number_(stackPointer->nBits(), stackPointer->nBits()/8));
        ops->writeRegister(cpu->stackPointerRegister(), stackPointer);
#ifdef ROSE_ENABLE_ASM_AARCH64
    } else if (boost::dynamic_pointer_cast<IS::DispatcherAarch64>(cpu)) {
        // Return address is in the link register, lr
        const RegisterDescriptor LR = cpu->callReturnRegister();
        ASSERT_forbid(LR.isEmpty());
        BS::SValuePtr returnTarget = ops->readRegister(LR, ops->undefined_(LR.nBits()));
        ops->writeRegister(cpu->instructionPointerRegister(), returnTarget);
#endif
#ifdef ROSE_ENABLE_ASM_AARCH32
    } else if (boost::dynamic_pointer_cast<IS::DispatcherAarch32>(cpu)) {
        // Return address is in the link register, lr
        const RegisterDescriptor LR = cpu->callReturnRegister();
        ASSERT_forbid(LR.isEmpty());
        BS::SValuePtr returnTarget = ops->readRegister(LR, ops->undefined_(LR.nBits()));
        ops->writeRegister(cpu->instructionPointerRegister(), returnTarget);
#endif
    }
}

std::vector<Tag::Ptr>
ExternalFunctionUnit::execute(const Settings::Ptr &settings, const SemanticCallbacks::Ptr &semantics,
                              const BS::RiscOperatorsPtr &ops) {
    ASSERT_not_null(settings);
    ASSERT_not_null(semantics);
    ASSERT_not_null(ops);
    std::vector<Tag::Ptr> tags;
    BS::DispatcherPtr cpu = semantics->createDispatcher(ops);
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
