#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Disassembler/Null.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <Rose/BinaryAnalysis/Unparser/Null.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Disassembler {

Null::Null() {
    name("null");
    wordSizeBytes(1);
    byteOrder(ByteOrder::ORDER_LSB);
    registerDictionary(RegisterDictionary::instanceNull());

    REG_IP = registerDictionary()->findOrThrow("pc");
    REG_SP = registerDictionary()->findOrThrow("sp");
}

Null::~Null() {}

Null::Ptr
Null::instance() {
    return Ptr(new Null);
}

Base::Ptr
Null::clone() const {
    return Ptr(new Null);
}

bool
Null::canDisassemble(SgAsmGenericHeader*) const {
    return false;
}

Unparser::BasePtr
Null::unparser() const {
    return Unparser::Null::instance();
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
    SgAsmInstruction *insn = new SgAsmNullInstruction(e.ip, "unknown");
    SgAsmOperandList *operands = new SgAsmOperandList;
    insn->set_operandList(operands);
    operands->set_parent(insn);
    insn->set_rawBytes(e.bytes);
    return insn;
}

} // namespace
} // namespace
} // namespace

#endif
