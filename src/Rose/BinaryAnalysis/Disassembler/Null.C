#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Disassembler/Null.h>

#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>

#include <SgAsmNullInstruction.h>
#include <SgAsmOperandList.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Disassembler {

Null::Null(const Architecture::Base::ConstPtr &arch)
    : Base(arch) {
    REG_IP = architecture()->registerDictionary()->instructionPointerRegister();
    REG_SP = architecture()->registerDictionary()->stackPointerRegister();
}

Null::~Null() {}

Null::Ptr
Null::instance(const Architecture::Base::ConstPtr &arch) {
    return Ptr(new Null(arch));
}

Base::Ptr
Null::clone() const {
    return Ptr(new Null(architecture()));
}

SgAsmInstruction*
Null::disassembleOne(const MemoryMap::Ptr &map, rose_addr_t va, AddressSet*) {
    uint8_t byte = 0;
    size_t nRead = map->at(va).limit(1).require(MemoryMap::EXECUTABLE).read(&byte).size();
    if (0 == nRead)
        throw Exception("short read", va);

    return makeUnknownInstruction(Exception("unknown", va,
                                            SgUnsignedCharList((const unsigned char*)&byte, (const unsigned char*)&byte+1),
                                            0));
}

SgAsmInstruction*
Null::makeUnknownInstruction(const Exception &e) {
#if 0 // [Robb Matzke 2024-08-21]: we need to set an instruction kind corresponding to the arch's unknown instruction
    SgAsmInstruction *insn = new SgAsmNullInstruction(e.ip, *architecture()->registrationId(), "unknown");
    SgAsmOperandList *operands = new SgAsmOperandList;
    insn->set_operandList(operands);
    operands->set_parent(insn);
    insn->set_rawBytes(e.bytes);
    return insn;
#else
    ASSERT_not_implemented("[Robb Matzke 2024-08-21]");
#endif
}

} // namespace
} // namespace
} // namespace

#endif
