#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"
#include <Rose/BinaryAnalysis/InstructionProvider.h>

#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>

namespace Rose {
namespace BinaryAnalysis {

InstructionProvider::InstructionProvider()
    : useDisassembler_(false) {
    // Start off with a large map to reduce early rehashing. There will probably be a lot of instructions.
    insnMap_.rehash(1000000);
}

InstructionProvider::InstructionProvider(const Disassembler::Base::Ptr &disassembler, const MemoryMap::Ptr &map)
    : disassembler_(disassembler), memMap_(map), useDisassembler_(true) {
    ASSERT_not_null(disassembler);
    // Start off with a large map to reduce early rehashing. There will probably be a lot of instructions.
    insnMap_.rehash(1000000);
}

InstructionProvider::~InstructionProvider() {}

void
InstructionProvider::enableDisassembler(bool enable) {
    ASSERT_require(!enable || disassembler_);
    useDisassembler_ = enable;
}

SgAsmInstruction*
InstructionProvider::operator[](rose_addr_t va) const {
    SgAsmInstruction *insn = NULL;
    if (!insnMap_.getOptional(va).assignTo(insn)) {
        if (useDisassembler_ && memMap_->at(va).require(MemoryMap::EXECUTABLE).exists()) {
            try {
                insn = disassembler_->disassembleOne(memMap_, va);
            } catch (const Disassembler::Exception &e) {
                insn = disassembler_->makeUnknownInstruction(e);
                ASSERT_not_null(insn);
                ASSERT_require(insn->get_address()==va);
                if (0 == insn->get_size()) {
                    uint8_t byte;
                    if (1==memMap_->at(va).limit(1).require(MemoryMap::EXECUTABLE).read(&byte).size())
                        insn->set_raw_bytes(SgUnsignedCharList(1, byte));
                    ASSERT_require(insn->get_size()==1);
                }
            }
            ASSERT_not_null(insn);
            ASSERT_require(insn->get_address() == va);
            insnMap_.insert(va, insn);
        }
    }
    return insn;
}

void
InstructionProvider::insert(SgAsmInstruction *insn) {
    ASSERT_not_null(insn);
    insnMap_.insert(insn->get_address(), insn);
}

Disassembler::Base::Ptr
InstructionProvider::disassembler() const {
    return disassembler_;
}

RegisterDictionary::Ptr
InstructionProvider::registerDictionary() const {
    return disassembler_->registerDictionary();
}

const CallingConvention::Dictionary&
InstructionProvider::callingConventions() const {
    return disassembler_->callingConventions();
}

RegisterDescriptor
InstructionProvider::instructionPointerRegister() const {
    return disassembler_->instructionPointerRegister();
}

RegisterDescriptor
InstructionProvider::stackPointerRegister() const {
    return disassembler_->stackPointerRegister();
}

RegisterDescriptor
InstructionProvider::stackFrameRegister() const {
    return disassembler_->stackFrameRegister();
}

RegisterDescriptor
InstructionProvider::callReturnRegister() const {
    return disassembler_->callReturnRegister();
}

RegisterDescriptor
InstructionProvider::stackSegmentRegister() const {
    return disassembler_->stackSegmentRegister();
}

ByteOrder::Endianness
InstructionProvider::defaultByteOrder() const {
    return disassembler_->byteOrder();
}

size_t
InstructionProvider::wordSize() const {
    return 8 * disassembler_->wordSizeBytes();
}

size_t
InstructionProvider::instructionAlignment() const {
    return disassembler_->instructionAlignment();
}

InstructionSemantics::BaseSemantics::Dispatcher::Ptr
InstructionProvider::dispatcher() const {
    return disassembler_->dispatcher();
}

void
InstructionProvider::showStatistics() const {
    std::cout <<"Rose::BinaryAnalysis::InstructionProvider statistics:\n";
    std::cout <<"  instruction map:\n";
    std::cout <<"    size = " <<insnMap_.size() <<"\n";
    std::cout <<"    number of hash buckets = " <<insnMap_.nBuckets() <<"\n";
    std::cout <<"    load factor = " <<insnMap_.loadFactor() <<"\n";
}

} // namespace
} // namespace

#endif
