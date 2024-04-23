#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/CallingConvention/BasicTypes.h>

#include <Rose/BinaryAnalysis/CallingConvention/Definition.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/SValue.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/RiscOperators.h>
#include <Rose/Diagnostics.h>

using namespace Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics;

namespace Rose {
namespace BinaryAnalysis {
namespace CallingConvention {

Sawyer::Message::Facility mlog;

void
initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::CallingConvention");
        mlog.comment("computing function calling conventions");
    }
}

SValue::Ptr
readArgument(const RiscOperators::Ptr &ops, const Definition::Ptr &ccDef, size_t argNumber) {
    ASSERT_not_null(ops);
    ASSERT_not_null(ccDef);
    ops->comment("reading function argument #" + boost::lexical_cast<std::string>(argNumber) +
                 " using calling convention " + ccDef->name());
    const size_t nBits = ccDef->bitsPerWord();
    SValue::Ptr retval;
    if (argNumber < ccDef->inputParameters().size()) {
        // Argument is explicit in the definition
        const ConcreteLocation &loc = ccDef->inputParameters()[argNumber];

        switch (loc.type()) {
            case ConcreteLocation::REGISTER:
                retval = ops->readRegister(loc.reg());
                break;

            case ConcreteLocation::RELATIVE: {
                const auto base = ops->readRegister(loc.reg());
                const auto offset = ops->signExtend(ops->number_(64, loc.offset()), base->nBits());
                const auto address = ops->add(base, offset);
                const auto dflt = ops->undefined_(nBits);
                retval = ops->readMemory(RegisterDescriptor(), address, dflt, ops->boolean_(true));
                break;
            }

            case ConcreteLocation::ABSOLUTE: {
                auto address = ops->number_(64, loc.address());
                const auto dflt = ops->undefined_(nBits);
                retval = ops->readMemory(RegisterDescriptor(), address, dflt, ops->boolean_(true));
                break;
            }

            case ConcreteLocation::NO_LOCATION:
                break;
        }

    } else {
        // Argument is at an implied stack location
        argNumber = argNumber - ccDef->inputParameters().size();
        switch (ccDef->stackParameterOrder()) {
            case StackParameterOrder::RIGHT_TO_LEFT:
                break;
            case StackParameterOrder::LEFT_TO_RIGHT:
                ASSERT_not_implemented("we need to know how many parameters were pushed");
            case StackParameterOrder::UNSPECIFIED:
                ASSERT_not_implemented("invalid stack paramter order");
        }
        int64_t stackOffset = ccDef->nonParameterStackSize() + argNumber * nBits/8;
        switch (ccDef->stackDirection()) {
            case StackDirection::GROWS_DOWN:
                break;
            case StackDirection::GROWS_UP:
                stackOffset = -stackOffset - nBits/8;
                break;
        }

        const RegisterDescriptor reg_sp = ccDef->stackPointerRegister();
        const auto base = ops->readRegister(reg_sp);
        const auto offset = ops->signExtend(ops->number_(64, stackOffset), base->nBits());
        const auto address = ops->add(base, offset);
        const auto dflt = ops->undefined_(nBits);
        retval = ops->readMemory(RegisterDescriptor(), address, dflt, ops->boolean_(true));
    }

    ASSERT_always_not_null2(retval, "invalid parameter location");
    ops->comment("argument value is " + retval->toString());
    return retval;
}

void
writeArgument(const RiscOperators::Ptr &ops, const Definition::Ptr &ccDef, size_t argNumber, const SValue::Ptr &value) {
    ASSERT_not_null(ops);
    ASSERT_not_null(ccDef);
    ops->comment("writing function argument #" + boost::lexical_cast<std::string>(argNumber) +
                 " using calling convention " + ccDef->name());
    const size_t nBits = ccDef->bitsPerWord();
    if (argNumber < ccDef->inputParameters().size()) {
        // Argument is explicit in the definition
        const ConcreteLocation &loc = ccDef->inputParameters()[argNumber];

        switch (loc.type()) {
            case ConcreteLocation::REGISTER:
                ops->writeRegister(loc.reg(), value);
                ops->comment("argument written");
                return;

            case ConcreteLocation::RELATIVE: {
                const auto base = ops->readRegister(loc.reg());
                const auto offset = ops->signExtend(ops->number_(64, loc.offset()), base->nBits());
                const auto address = ops->add(base, offset);
                ops->writeMemory(RegisterDescriptor(), address, value, ops->boolean_(true));
                ops->comment("argument written");
                return;
            }

            case ConcreteLocation::ABSOLUTE: {
                auto address = ops->number_(64, loc.address());
                ops->writeMemory(RegisterDescriptor(), address, value, ops->boolean_(true));
                ops->comment("argument written");
                return;
            }

            case ConcreteLocation::NO_LOCATION:
                break;
        }
        ASSERT_not_reachable("invalid parameter location type");

    } else {
        // Argument is at an implied stack location
        ASSERT_require(argNumber >= ccDef->inputParameters().size());
        argNumber = argNumber - ccDef->inputParameters().size();
        switch (ccDef->stackParameterOrder()) {
            case StackParameterOrder::RIGHT_TO_LEFT:
                break;
            case StackParameterOrder::LEFT_TO_RIGHT:
                ASSERT_not_implemented("we need to know how many parameters were pushed");
            case StackParameterOrder::UNSPECIFIED:
                ASSERT_not_implemented("invalid stack paramter order");
        }
        int64_t stackOffset = ccDef->nonParameterStackSize() + argNumber * nBits/8;
        switch (ccDef->stackDirection()) {
            case StackDirection::GROWS_DOWN:
                break;
            case StackDirection::GROWS_UP:
                stackOffset = -stackOffset - nBits/8;
                break;
        }

        const RegisterDescriptor reg_sp = ccDef->stackPointerRegister();
        const auto base = ops->readRegister(reg_sp);
        const auto offset = ops->signExtend(ops->number_(64, stackOffset), base->nBits());
        const auto address = ops->add(base, offset);
        ops->writeMemory(RegisterDescriptor(), address, value, ops->boolean_(true));
        ops->comment("argument written");
    }
}

SValue::Ptr
readReturnValue(const RiscOperators::Ptr &ops, const Definition::Ptr &ccDef) {
    ASSERT_not_null(ops);
    ASSERT_not_null(ccDef);
    ops->comment("reading function return value using calling convention " + ccDef->name());

    if (ccDef->outputParameters().empty())
        throw Exception("calling convention has no output parameters");

    // Assume that the first output parameter is the main integer return location.
    const ConcreteLocation &loc = ccDef->outputParameters()[0];
    SValue::Ptr retval;
    switch (loc.type()) {
        case ConcreteLocation::REGISTER:
            retval = ops->readRegister(loc.reg());
            break;

        case ConcreteLocation::RELATIVE: {
            const auto base = ops->readRegister(loc.reg());
            const auto offset = ops->signExtend(ops->number_(64, loc.offset()), base->nBits());
            const auto address = ops->add(base, offset);
            const auto dflt = ops->undefined_(loc.reg().nBits());
            retval = ops->readMemory(RegisterDescriptor(), address, dflt, ops->boolean_(true));
            break;
        }

        case ConcreteLocation::ABSOLUTE: {
            auto address = ops->number_(64, loc.address());
            const auto dflt = ops->undefined_(loc.reg().nBits());
            retval = ops->readMemory(RegisterDescriptor(), address, dflt, ops->boolean_(true));
            break;
        }

        case ConcreteLocation::NO_LOCATION:
            break;
    }
    ASSERT_always_not_null2(retval, "invalid parameter location type");
    ops->comment("return value is " + retval->toString());
    return retval;
}

void
writeReturnValue(const RiscOperators::Ptr &ops, const Definition::Ptr &ccDef, const SValue::Ptr &returnValue) {
    ASSERT_not_null(ops);
    ASSERT_not_null(ccDef);
    ASSERT_not_null(returnValue);
    ops->comment("writing function return value using calling convention " + ccDef->name());

    if (ccDef->outputParameters().empty())
        throw Exception("calling convention has no output parameters");

    // Assume that the first output parameter is the main integer return location.
    const ConcreteLocation &loc = ccDef->outputParameters()[0];
    switch (loc.type()) {
        case ConcreteLocation::REGISTER:
            ops->writeRegister(loc.reg(), returnValue);
            ops->comment("return value written");
            return;

        case ConcreteLocation::RELATIVE: {
            const auto base = ops->readRegister(loc.reg());
            const auto offset = ops->signExtend(ops->number_(64, loc.offset()), base->nBits());
            const auto address = ops->add(base, offset);
            ops->writeMemory(RegisterDescriptor(), address, returnValue, ops->boolean_(true));
            ops->comment("return value written");
            return;
        }

        case ConcreteLocation::ABSOLUTE: {
            auto address = ops->number_(64, loc.address());
            ops->writeMemory(RegisterDescriptor(), address, returnValue, ops->boolean_(true));
            ops->comment("return value written");
            return;
        }

        case ConcreteLocation::NO_LOCATION:
            break;
    }
    ASSERT_not_reachable("invalid parameter location type");
}

void
simulateFunctionReturn(const RiscOperators::Ptr &ops, const Definition::Ptr &ccDef) {
    ASSERT_not_null(ops);
    ASSERT_not_null(ccDef);
    ops->comment("simulating function return using calling convention " + ccDef->name());

    // Assume that the current stack pointer is the same as it was when this function was entered.
    const RegisterDescriptor SP = ccDef->stackPointerRegister();
    ASSERT_require(SP);
    const auto originalSp = ops->readRegister(SP);

    // How many input parameters to be popped from the stack by the callee?
    //
    // FIXME[Robb Matzke 2022-07-13]: This assumes that each parameter is one word, which is often not the case, but
    // we don't currently have any information about parameter sizes.
    int64_t nArgBytes = 0;
    if (StackCleanup::BY_CALLEE == ccDef->stackCleanup()) {
        for (const ConcreteLocation &loc: ccDef->inputParameters()) {
            if (ConcreteLocation::RELATIVE == loc.type())
                nArgBytes += ccDef->bitsPerWord() / 8;
        }
    }

    // How many other things need to be popped from the stack by the callee as part of returning?
    int64_t nNonArgBytes = ccDef->nonParameterStackSize();

    // Obtain the return address
    const ConcreteLocation &retVaLoc = ccDef->returnAddressLocation();
    SValue::Ptr retVa;
    switch (retVaLoc.type()) {
        case ConcreteLocation::REGISTER:
            retVa = ops->readRegister(retVaLoc.reg());
            break;

        case ConcreteLocation::RELATIVE: {
            const auto base = ops->readRegister(retVaLoc.reg());
            const auto offset = ops->signExtend(ops->number_(64, retVaLoc.offset()), base->nBits());
            const auto address = ops->add(base, offset);
            const auto dflt = ops->undefined_(ccDef->bitsPerWord());
            retVa = ops->readMemory(RegisterDescriptor(), address, dflt, ops->boolean_(true));
            break;
        }

        case ConcreteLocation::ABSOLUTE: {
            auto address = ops->number_(64, retVaLoc.address());
            const auto dflt = ops->undefined_(ccDef->bitsPerWord());
            retVa = ops->readMemory(RegisterDescriptor(), address, dflt, ops->boolean_(true));
            break;
        }

        case ConcreteLocation::NO_LOCATION:
            break;
    }
    ASSERT_not_null2(retVa, "unknown location for fuction return address");

    // Pop things from the stack
    SValue::Ptr newSp;
    switch (ccDef->stackDirection()) {
        case StackDirection::GROWS_DOWN:
            newSp = ops->add(originalSp, ops->number_(originalSp->nBits(), nArgBytes + nNonArgBytes));
            break;
        case StackDirection::GROWS_UP:
            newSp = ops->subtract(originalSp, ops->number_(originalSp->nBits(), nArgBytes + nNonArgBytes));
            break;
    }
    ASSERT_not_null2(newSp, "invalid stack growth direction");
    ops->writeRegister(SP, newSp);

    // Change the instruction pointer to be the return address.
    const RegisterDescriptor IP = ccDef->instructionPointerRegister();
    ops->writeRegister(IP, retVa);
    ops->comment("function return has been simulated");
}

} // namespace
} // namespace
} // namespace

#endif
