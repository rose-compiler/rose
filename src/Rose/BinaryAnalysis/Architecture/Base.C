#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/Base.h>

#include <Rose/BinaryAnalysis/Architecture/Exception.h>
#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <Rose/StringUtility/Escape.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

Base::Base() {}

Base::~Base() {}

const std::string&
Base::name() const {
    return name_;
}

void
Base::name(const std::string &s) {
    name_ = s;
}

const Disassembler::Base::Ptr&
Base::instructionDecoderFactory() const {
    return instructionDecoderFactory_;
}

void
Base::instructionDecoderFactory(const Disassembler::Base::Ptr &decoder) {
    instructionDecoderFactory_ = decoder;
}

Disassembler::Base::Ptr
Base::newInstructionDecoder() const {
    if (!instructionDecoderFactory_)
        return instructionDecoderFactory_->clone();
    throw NotFound("no instruction decoder for \"" + StringUtility::cEscape(name()) + "\"");
}

RegisterDictionary::Ptr
Base::registerDictionary() const {
    if (instructionDecoderFactory_)
        return instructionDecoderFactory_->registerDictionary();
    throw NotFound("no instruction decoder for \"" + StringUtility::cEscape(name()) + "\"");
}

size_t
Base::wordSizeBytes() const {
    if (instructionDecoderFactory_)
        return instructionDecoderFactory_->wordSizeBytes();
    throw NotFound("no instruction decoder for \"" + StringUtility::cEscape(name()) + "\"");
}

size_t
Base::wordSizeBits() const {
    return wordSizeBytes() * 8;
}

ByteOrder::Endianness
Base::byteOrder() const {
    if (instructionDecoderFactory_)
        return instructionDecoderFactory_->byteOrder();
    throw NotFound("no instruction decoder for \"" + StringUtility::cEscape(name()) + "\"");
}

size_t
Base::instructionAlignment() const {
    if (instructionDecoderFactory_)
        return instructionDecoderFactory_->instructionAlignment();
    throw NotFound("no instruction decoder for \"" + StringUtility::cEscape(name()) + "\"");
}

RegisterDescriptor
Base::instructionPointerRegister() const {
    if (instructionDecoderFactory_)
        return instructionDecoderFactory_->instructionPointerRegister();
    throw NotFound("no instruction decoder for \"" + StringUtility::cEscape(name()) + "\"");
}

RegisterDescriptor
Base::stackPointerRegister() const {
    if (instructionDecoderFactory_)
        return instructionDecoderFactory_->stackPointerRegister();
    throw NotFound("no instruction decoder for \"" + StringUtility::cEscape(name()) + "\"");
}

RegisterDescriptor
Base::stackFrameRegister() const {
    if (instructionDecoderFactory_)
        return instructionDecoderFactory_->stackFrameRegister();
    throw NotFound("no instruction decoder for \"" + StringUtility::cEscape(name()) + "\"");
}

RegisterDescriptor
Base::stackSegmentRegister() const {
    if (instructionDecoderFactory_)
        return instructionDecoderFactory_->stackSegmentRegister();
    throw NotFound("no instruction decoder for \"" + StringUtility::cEscape(name()) + "\"");
}

RegisterDescriptor
Base::callReturnRegister() const {
    if (instructionDecoderFactory_)
        return instructionDecoderFactory_->callReturnRegister();
    throw NotFound("no instruction decoder for \"" + StringUtility::cEscape(name()) + "\"");
}

InstructionSemantics::BaseSemantics::Dispatcher::Ptr
Base::newInstructionDispatcher(const InstructionSemantics::BaseSemantics::RiscOperators::Ptr &ops) const {
    return {};
}

} // namespace
} // namespace
} // namespace

#endif
