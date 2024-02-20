#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/Base.h>

#include <Rose/BinaryAnalysis/Architecture/Exception.h>
#include <Rose/BinaryAnalysis/CallingConvention.h>
#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics.h>
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

const std::string&
Base::name() const {
    return name_;
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

InstructionSemantics::BaseSemantics::Dispatcher::Ptr
Base::newInstructionDispatcher(const InstructionSemantics::BaseSemantics::RiscOperators::Ptr&) const {
    return {};
}

std::string
Base::toString(const SgAsmInstruction *insn) const {
    if (insn) {
        {
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
        }
        return insnToString_.get()->unparse(const_cast<SgAsmInstruction*>(insn));
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
                auto settings = Unparser::Settings::minimal();
                settings.insn.address.showing = false;
                settings.insn.address.useLabels = false;
                settings.insn.address.fieldWidth = 1;
                settings.insn.bytes.showing = false;
                settings.insn.stackDelta.showing = false;
                settings.insn.mnemonic.fieldWidth = 1;
                settings.insn.operands.fieldWidth = 1;
                settings.insn.comment.showing = false;
                settings.insn.semantics.showing = false;
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
Base::instructionDescription(const SgAsmInstruction*) const {
    return {};
}

bool
Base::isFunctionCallFast(const std::vector<SgAsmInstruction*>&, rose_addr_t*, rose_addr_t*) const {
    return false;
}

bool
Base::isFunctionCallSlow(const std::vector<SgAsmInstruction*>& insns, rose_addr_t *target, rose_addr_t *ret) const {
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

Sawyer::Optional<rose_addr_t>
Base::branchTarget(SgAsmInstruction*) const {
    return Sawyer::Nothing();
}

AddressSet
Base::getSuccessors(SgAsmInstruction*, bool &complete) const {
    complete = false;
    return AddressSet();
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
