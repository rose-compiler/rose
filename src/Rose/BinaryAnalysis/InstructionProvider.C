#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/InstructionProvider.h>

#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>

#include <SgAsmInstruction.h>

namespace Rose {
namespace BinaryAnalysis {

InstructionProvider::InstructionProvider() {
    // Start off with a large map to reduce early rehashing. There will probably be a lot of instructions.
    insnMap_.rehash(1000000);
}

InstructionProvider::InstructionProvider(const Architecture::Base::ConstPtr &arch, const MemoryMap::Ptr &map)
    : architecture_(arch), memMap_(map) {
    ASSERT_not_null(architecture_);
    if (map) {
        disassembler_ = arch->newInstructionDecoder();
        ASSERT_not_null(disassembler_);
    }

    // Start off with a large map to reduce early rehashing. There will probably be a lot of instructions.
    insnMap_.rehash(1000000);
}

InstructionProvider::~InstructionProvider() {}

InstructionProvider::Ptr
InstructionProvider::instance(const Architecture::Base::ConstPtr &arch, const MemoryMap::Ptr &map) {
    return Ptr(new InstructionProvider(arch, map));
}

bool
InstructionProvider::isDisassemblerEnabled() const {
    return memMap_ != nullptr;
}

SgAsmInstruction*
InstructionProvider::operator[](rose_addr_t va) const {
    SgAsmInstruction *insn = nullptr;
    if (!insnMap_.getOptional(va).assignTo(insn)) {
        if (memMap_ && memMap_->at(va).require(MemoryMap::EXECUTABLE).exists()) {
            ASSERT_not_null(disassembler_);
            try {
                insn = disassembler_->disassembleOne(memMap_, va);
            } catch (const Disassembler::Exception &e) {
                insn = disassembler_->makeUnknownInstruction(e);
                ASSERT_not_null(insn);
                ASSERT_require(insn->get_address()==va);
                if (0 == insn->get_size()) {
                    uint8_t byte;
                    if (1==memMap_->at(va).limit(1).require(MemoryMap::EXECUTABLE).read(&byte).size())
                        insn->set_rawBytes(SgUnsignedCharList(1, byte));
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
    return architecture_->registerDictionary();
}

const CallingConvention::Dictionary&
InstructionProvider::callingConventions() const {
    return architecture_->callingConventions();
}

RegisterDescriptor
InstructionProvider::instructionPointerRegister() const {
    return architecture_->registerDictionary()->instructionPointerRegister();
}

RegisterDescriptor
InstructionProvider::stackPointerRegister() const {
    return architecture_->registerDictionary()->stackPointerRegister();
}

RegisterDescriptor
InstructionProvider::stackFrameRegister() const {
    return architecture_->registerDictionary()->stackFrameRegister();
}

RegisterDescriptor
InstructionProvider::callReturnRegister() const {
    return architecture_->registerDictionary()->callReturnRegister();
}

RegisterDescriptor
InstructionProvider::stackSegmentRegister() const {
    return architecture_->registerDictionary()->stackSegmentRegister();
}

ByteOrder::Endianness
InstructionProvider::defaultByteOrder() const {
    return architecture_->byteOrder();
}

size_t
InstructionProvider::wordSize() const {
    return architecture_->bitsPerWord();
}

size_t
InstructionProvider::instructionAlignment() const {
    ASSERT_not_null(disassembler_);
    return disassembler_->instructionAlignment();
}

InstructionSemantics::BaseSemantics::Dispatcher::Ptr
InstructionProvider::dispatcher(const InstructionSemantics::BaseSemantics::RiscOperators::Ptr &ops) const {
    return architecture_->newInstructionDispatcher(ops);
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
