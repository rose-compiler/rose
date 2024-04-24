#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/CallingConvention/Definition.h>

#include <Rose/Affirm.h>
#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/CallingConvention/StoragePool.h>

namespace Rose {
namespace BinaryAnalysis {
namespace CallingConvention {

Definition::Definition() {}

Definition::Definition(const std::string &name, const std::string &comment, const Architecture::Base::ConstPtr &arch)
    : name_(name), comment_(comment), architecture_(arch), returnValueAllocator_(Allocator::instance()),
      argumentValueAllocator_(Allocator::instance()) {
    ASSERT_not_null(arch);
}

Definition::~Definition() {}

Definition::Ptr
Definition::instance(const std::string &name, const std::string &comment, const Architecture::Base::ConstPtr &arch) {
    return Ptr(new Definition(name, comment, arch));
}

Architecture::Base::ConstPtr
Definition::architecture() const {
    Architecture::Base::ConstPtr arch = architecture_.lock();
    ASSERT_not_null(arch);
    return arch;
}

RegisterDictionary::Ptr
Definition::registerDictionary() const {
    return notnull(architecture()->registerDictionary());
}

size_t
Definition::bitsPerWord() const {
    return bitsPerWord_.orElse(architecture()->bitsPerWord());
}

void
Definition::bitsPerWord(const Sawyer::Optional<size_t> &x) {
    bitsPerWord_ = x;
}

void
Definition::appendInputParameter(const ConcreteLocation &newLocation) {
#ifndef NDEBUG
    for (const ConcreteLocation &existingLocation: inputParameters_)
        ASSERT_forbid(newLocation == existingLocation);
#endif
    inputParameters_.push_back(newLocation);
}

void
Definition::appendOutputParameter(const ConcreteLocation &newLocation) {
#ifndef NDEBUG
    for (const ConcreteLocation &existingLocation: outputParameters_)
        ASSERT_forbid(newLocation == existingLocation);
#endif
    outputParameters_.push_back(newLocation);
}

RegisterDescriptor
Definition::stackPointerRegister() const {
    return stackPointerRegister_ ? stackPointerRegister_ : architecture()->registerDictionary()->stackPointerRegister();
}

void
Definition::stackPointerRegister(const RegisterDescriptor x) {
    stackPointerRegister_ = x;
}

ByteOrder::Endianness
Definition::byteOrder() const {
    return architecture()->byteOrder();
}

const Alignment&
Definition::stackAlignment() const {
    return stackAlignment_;
}

void
Definition::stackAlignment(const Alignment &x) {
    stackAlignment_ = x;
}

RegisterParts
Definition::outputRegisterParts() const {
    RegisterParts retval;
    for (const ConcreteLocation &loc: outputParameters_) {
        if (loc.type() == ConcreteLocation::REGISTER)
            retval.insert(loc.reg());
    }
    return retval;
}

RegisterParts
Definition::inputRegisterParts() const {
    RegisterParts retval;
    for (const ConcreteLocation &loc: inputParameters_) {
        if (loc.type() == ConcreteLocation::REGISTER)
            retval.insert(loc.reg());
    }
    for (const ConcreteLocation &loc: nonParameterInputs_) {
        if (loc.type() == ConcreteLocation::REGISTER)
            retval.insert(loc.reg());
    }
    return retval;
}

RegisterParts
Definition::scratchRegisterParts() const {
    RegisterParts retval;
    for (RegisterDescriptor reg: scratchRegisters_)
        retval.insert(reg);
    return retval;
}

RegisterParts
Definition::calleeSavedRegisterParts() const {
    RegisterParts retval;
    for (RegisterDescriptor reg: calleeSavedRegisters_)
        retval.insert(reg);
    return retval;
}

RegisterParts
Definition::getUsedRegisterParts() const {
    RegisterParts retval = inputRegisterParts();
    retval |= outputRegisterParts();
    retval.insert(stackPointerRegister());
    if (thisParameter_.type() == ConcreteLocation::REGISTER)
        retval.insert(thisParameter_.reg());
    retval |= calleeSavedRegisterParts();
    retval |= scratchRegisterParts();
    return retval;
}

Allocator::Ptr
Definition::returnValueAllocator() const {
    return notnull(returnValueAllocator_);
}

void
Definition::returnValueAllocator(const Allocator::Ptr &allocator) {
    ASSERT_not_null(allocator);
    returnValueAllocator_ = allocator;
}

Allocator::Ptr
Definition::argumentValueAllocator() const {
    return notnull(argumentValueAllocator_);
}

void
Definition::argumentValueAllocator(const Allocator::Ptr &allocator) {
    ASSERT_not_null(allocator);
    argumentValueAllocator_ = allocator;
}

void
Definition::print(std::ostream &out) const {
    print(out, RegisterDictionary::Ptr());
}

void
Definition::print(std::ostream &out, const RegisterDictionary::Ptr &regDictOverride/*=NULL*/) const {
    using namespace StringUtility;
    ASSERT_require(regDictOverride || registerDictionary());
    RegisterDictionary::Ptr regDict = regDictOverride ? regDictOverride : registerDictionary();
    RegisterNames regNames(regDict);

    out <<cEscape(name_);
    if (!comment_.empty())
        out <<" (" <<cEscape(comment_) <<")";
    out <<" = {" <<bitsPerWord() <<"-bit words";

    if (instructionPointerRegister_)
        out <<", instructionAddressLocation=" <<regNames(instructionPointerRegister_);

    if (returnAddressLocation_.isValid()) {
        out <<", returnAddress=";
        returnAddressLocation_.print(out, regDict);
    }

    if (!inputParameters_.empty()) {
        out <<", input-parameters={";
        for (const ConcreteLocation &loc: inputParameters_) {
            out <<" ";
            loc.print(out, regDict ? regDict : loc.registerDictionary());
        }
        out <<" }";
    }

    if (!nonParameterInputs_.empty()) {
        out <<", non-parameter-inputs={";
        for (const ConcreteLocation &loc: nonParameterInputs_) {
            out <<" ";
            loc.print(out, regDict ? regDict : loc.registerDictionary());
        }
        out <<" }";
    }

    if (stackParameterOrder_ != StackParameterOrder::UNSPECIFIED) {
        out <<", implied={";
        switch (stackParameterOrder_) {
            case StackParameterOrder::LEFT_TO_RIGHT: out <<" left-to-right"; break;
            case StackParameterOrder::RIGHT_TO_LEFT: out <<" right-to-left"; break;
            case StackParameterOrder::UNSPECIFIED: ASSERT_not_reachable("invalid stack parameter order");
        }

        out <<" " <<regNames(stackPointerRegister()) <<"-based stack";

        switch (stackCleanup_) {
            case StackCleanup::BY_CALLER: out <<" cleaned up by caller"; break;
            case StackCleanup::BY_CALLEE: out <<" cleaned up by callee"; break;
            case StackCleanup::UNSPECIFIED: out <<" with UNSPECIFIED cleanup"; break;
        }
        out <<" }";
    }

    if (nonParameterStackSize_ > 0)
        out <<", " <<nonParameterStackSize_ <<"-byte return";

    if (stackParameterOrder_ != StackParameterOrder::UNSPECIFIED || nonParameterStackSize_ > 0) {
        switch (stackDirection_) {
            case StackDirection::GROWS_UP: out <<", upward-growing stack"; break;
            case StackDirection::GROWS_DOWN: out <<", downward-growing stack"; break;
        }
    }

    if (thisParameter_.isValid()) {
        out <<", this=";
        thisParameter_.print(out, regDict ? regDict : thisParameter_.registerDictionary());
    }

    if (!outputParameters_.empty()) {
        out <<", outputs={";
        for (const ConcreteLocation &loc: outputParameters_) {
            out <<" ";
            loc.print(out, regDict ? regDict : loc.registerDictionary());
        }
        out <<" }";
    }

    if (!scratchRegisters_.empty()) {
        out <<", scratch={";
        for (RegisterDescriptor loc: scratchRegisters_)
            out <<" " <<regNames(loc);
        out <<" }";
    }

    if (!calleeSavedRegisters_.empty()) {
        out <<", saved={";
        for (RegisterDescriptor loc: calleeSavedRegisters_)
            out <<" " <<regNames(loc);
        out <<" }";
    }
}

std::ostream&
operator<<(std::ostream &out, const Definition &x) {
    x.print(out);
    return out;
}

} // namespace
} // namespace
} // namespace

#endif
