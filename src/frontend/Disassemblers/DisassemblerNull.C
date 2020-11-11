#include <featureTests.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include <sage3basic.h>
#include <DisassemblerNull.h>
#include <BinaryUnparserNull.h>

namespace Rose {
namespace BinaryAnalysis {

DisassemblerNull::DisassemblerNull() {
    name("null");
    wordSizeBytes(1);
    byteOrder(ByteOrder::ORDER_LSB);
    registerDictionary(RegisterDictionary::dictionary_null());

    REG_IP = registerDictionary()->findOrThrow("pc");
    REG_SP = registerDictionary()->findOrThrow("sp");
}

DisassemblerNull::~DisassemblerNull() {}

Disassembler*
DisassemblerNull::clone() const {
    return new DisassemblerNull;
}

bool
DisassemblerNull::canDisassemble(SgAsmGenericHeader*) const {
    return false;
}

Unparser::BasePtr
DisassemblerNull::unparser() const {
    return Unparser::Null::instance();
}

SgAsmInstruction*
DisassemblerNull::disassembleOne(const MemoryMap::Ptr &map, rose_addr_t va, AddressSet*) {
    uint8_t byte = 0;
    size_t nRead = map->at(va).limit(1).require(MemoryMap::EXECUTABLE).read(&byte).size();
    if (0 == nRead)
        throw Exception("short read", va);

    return makeUnknownInstruction(Exception("unknown", va,
                                            SgUnsignedCharList((const unsigned char*)&byte, (const unsigned char*)&byte+1),
                                            0));
}

SgAsmInstruction*
DisassemblerNull::makeUnknownInstruction(const Exception &e) {
    SgAsmInstruction *insn = new SgAsmNullInstruction(e.ip, "unknown");
    SgAsmOperandList *operands = new SgAsmOperandList;
    insn->set_operandList(operands);
    operands->set_parent(insn);
    insn->set_raw_bytes(e.bytes);
    return insn;
}

} // namespace
} // namespace

#endif
