#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Architecture/Base.h>

#include <Rose/BinaryAnalysis/Architecture/Exception.h>
#include <Rose/BinaryAnalysis/CallingConvention/BasicTypes.h>
#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics.h>
#include <Rose/BinaryAnalysis/MemoryMap.h>
#include <Rose/BinaryAnalysis/Partitioner2/Modules.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <Rose/BinaryAnalysis/Unparser.h>
#include <Rose/StringUtility/Escape.h>

#include <sstream>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

Base::Base(const std::string &name, size_t bytesPerWord, ByteOrder::Endianness byteOrder)
    : name_(name), bytesPerWord_(bytesPerWord), byteOrder_(byteOrder) {}

Base::~Base() {}

Base::Ptr
Base::ptr() {
    return shared_from_this();
}

Base::ConstPtr
Base::constPtr() const {
    return shared_from_this();
}

const std::string&
Base::name() const {
    return name_;
}

const Sawyer::Optional<size_t>&
Base::registrationId() const {
    return registrationId_;
}

void
Base::registrationId(const Sawyer::Optional<size_t> &id) {
    ASSERT_require2(!id || 0 == registrationId_, "already registered");
    registrationId_ = id;
}

size_t
Base::bytesPerWord() const {
    return bytesPerWord_;
}

size_t
Base::bitsPerWord() const {
    return bytesPerWord() * 8;
}

ByteOrder::Endianness
Base::byteOrder() const {
    return byteOrder_;
}

RegisterDictionary::Ptr
Base::interruptDictionary() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    if (!interruptDictionary_.isCached())
        interruptDictionary_ = RegisterDictionary::instance("empty");

    return interruptDictionary_.get();
}

bool
Base::matchesName(const std::string &name) const {
    return name_ == name;
}

bool
Base::matchesHeader(SgAsmGenericHeader*) const {
    return false;
}

const CallingConvention::Dictionary&
Base::callingConventions() const {
    static const CallingConvention::Dictionary empty;
    return empty;
}

bool
Base::instructionsCanOverlap() const {
    ASSERT_require(bytesPerInstruction());
    return *instructionAlignment() < bytesPerInstruction().greatest();
}

InstructionSemantics::BaseSemantics::Dispatcher::Ptr
Base::newInstructionDispatcher(const InstructionSemantics::BaseSemantics::RiscOperators::Ptr&) const {
    return {};
}

Unparser::Base::Ptr
Base::insnUnparser() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);
    if (!insnToString_.isCached()) {
        auto settings = Unparser::Settings::minimal();
        settings.insn.address.showing = true;
        settings.insn.address.useLabels = false;
        settings.insn.address.fieldWidth = 1;
        settings.insn.bytes.showing = false;
        settings.insn.stackDelta.showing = false;
        settings.insn.mnemonic.fieldWidth = 1;
        settings.insn.operands.fieldWidth = 1;
        settings.insn.comment.showing = false;
        settings.insn.semantics.showing = false;
        insnToString_ = newUnparser();
        insnToString_.get()->settings() = settings;
    }
    return insnToString_.get();
}

Unparser::Base::Ptr
Base::newInstructionUnparser() const {
    return insnUnparser()->copy();
}

std::string
Base::toString(const SgAsmInstruction *insn) const {
    if (insn) {
        return insnUnparser()->unparse(const_cast<SgAsmInstruction*>(insn));
    } else {
        return "null";
    }
}

std::string
Base::toString(const SgAsmExpression *expr) const {
    if (expr) {
        return insnUnparser()->unparse(const_cast<SgAsmExpression*>(expr));
    } else {
        return "null";
    }
}

std::string
Base::toStringNoAddr(const SgAsmInstruction *insn) const {
    if (insn) {
        {
            static SAWYER_THREAD_TRAITS::Mutex mutex;
            SAWYER_THREAD_TRAITS::LockGuard lock(mutex);
            if (!insnToStringNoAddr_.isCached()) {
                auto settings = insnUnparser()->settings();
                settings.insn.address.showing = false;
                insnToStringNoAddr_ = newUnparser();
                insnToStringNoAddr_.get()->settings() = settings;
            }
        }
        return insnToStringNoAddr_.get()->unparse(const_cast<SgAsmInstruction*>(insn));
    } else {
        return "null";
    }
}

std::string
Base::toStringNoColor(const SgAsmInstruction *insn) const {
    if (insn) {
        {
            static SAWYER_THREAD_TRAITS::Mutex mutex;
            SAWYER_THREAD_TRAITS::LockGuard lock(mutex);
            if (!insnToStringNoColor_.isCached()) {
                auto settings = insnUnparser()->settings();
                settings.colorization.enabled = Color::Enabled::OFF;
                insnToStringNoColor_ = newUnparser();
                insnToStringNoColor_.get()->settings() = settings;
            }
        }
        return insnToStringNoColor_.get()->unparse(const_cast<SgAsmInstruction*>(insn));
    } else {
        return "null";
    }
}

std::string
Base::toStringNoAddrNoColor(const SgAsmInstruction *insn) const {
    if (insn) {
        {
            static SAWYER_THREAD_TRAITS::Mutex mutex;
            SAWYER_THREAD_TRAITS::LockGuard lock(mutex);
            if (!insnToStringNoAddrNoColor_.isCached()) {
                auto settings = insnUnparser()->settings();
                settings.insn.address.showing = false;
                settings.colorization.enabled = Color::Enabled::OFF;
                insnToStringNoAddrNoColor_ = newUnparser();
                insnToStringNoAddrNoColor_.get()->settings() = settings;
            }
        }
        return insnToStringNoAddrNoColor_.get()->unparse(const_cast<SgAsmInstruction*>(insn));
    } else {
        return "null";
    }
}

std::string
Base::instructionDescription(const SgAsmInstruction*) const {
    return {};
}

bool
Base::isControlTransfer(const SgAsmInstruction*) const {
    return false;
}

bool
Base::isFunctionCallFast(const std::vector<SgAsmInstruction*>&, Address*, Address*) const {
    return false;
}

bool
Base::isFunctionCallSlow(const std::vector<SgAsmInstruction*>& insns, Address *target, Address *ret) const {
    return isFunctionCallFast(insns, target, ret);
}

bool
Base::isFunctionReturnFast(const std::vector<SgAsmInstruction*>&) const {
    return false;
}

bool
Base::isFunctionReturnSlow(const std::vector<SgAsmInstruction*> &insns) const {
    return isFunctionReturnFast(insns);
}

Sawyer::Optional<Address>
Base::branchTarget(SgAsmInstruction*) const {
    return Sawyer::Nothing();
}

AddressSet
Base::getSuccessors(SgAsmInstruction*, bool &complete) const {
    complete = false;
    return AddressSet();
}

AddressSet
Base::getSuccessors(const std::vector<SgAsmInstruction*> &insns, bool &complete) const {
    return getSuccessors(insns, complete, MemoryMap::Ptr());
}

AddressSet
Base::getSuccessors(const std::vector<SgAsmInstruction*> &insns, bool &complete, const MemoryMapPtr&) const {
    if (insns.empty()) {
        complete = true;
        return AddressSet();
    } else {
        return getSuccessors(insns.back(), complete /*out*/);
    }
}

std::vector<Partitioner2::FunctionPrologueMatcher::Ptr>
Base::functionPrologueMatchers(const Partitioner2::EnginePtr&) const {
    return {};
}

std::vector<Partitioner2::BasicBlockCallback::Ptr>
Base::basicBlockCreationHooks(const Partitioner2::EnginePtr&) const {
    return {};
}

} // namespace
} // namespace
} // namespace

#endif
