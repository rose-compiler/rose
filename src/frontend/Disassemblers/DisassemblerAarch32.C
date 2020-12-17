#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_AARCH32

#include <sage3basic.h>
#include <BitOps.h>
#include <DisassemblerAarch32.h>
#include <BinaryUnparserAarch32.h>
#include <DispatcherAarch32.h>

using namespace Rose::Diagnostics;

namespace Rose {
namespace BinaryAnalysis {

Disassembler*
DisassemblerAarch32::clone() const {
    return new DisassemblerAarch32(*this);
}

bool
DisassemblerAarch32::canDisassemble(SgAsmGenericHeader *header) const {
    SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();
    if ((isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_ARM_Family) {
        if (header->get_exec_format()->get_word_size() == 4) {
            // FIXME[Robb Matzke 2020-12-18]: valid iff we're decoding A32 instructions
            ASSERT_not_implemented("[Robb Matzke 2020-12-23]");
        } else if (header->get_exec_format()->get_word_size() == 2) {
            // FIXME[Robb Matzke 2020-12-18]: valid iff we're decoding T32 instructions
            ASSERT_not_implemented("[Robb Matzke 2020-12-23]");
        }
    }
    return false;
}

void
DisassemblerAarch32::init() {
    // Warning: the "mode" constants are not orthogonal with each other or the "arch" values.
    cs_mode mode = (cs_mode)modes_.vector();

    // ROSE disassembler properties, and choose a somewhat descriptive name (at least something better than "ARM").
    std::string name;
    if (modes_.isSet(Mode::MODE_THUMB)) {
        name = "t32";
        instructionAlignment_ = 2;
    } else {
        name = "a32";
        instructionAlignment_ = 4;
    }
    wordSizeBytes(4);
    byteOrder(ByteOrder::ORDER_LSB);
    registerDictionary(RegisterDictionary::dictionary_aarch32());
    callingConventions(CallingConvention::dictionaryAarch32());

    if (modes_.isSet(Mode::MODE_MCLASS))
        name += "_microprocessor"; // apparently the "microprocessor profile for Cortex processors"
    this->name(name);

    // Architecture independent ROSE disassembler properties
    REG_IP = registerDictionary()->findOrThrow("pc");
    REG_SP = registerDictionary()->findOrThrow("sp");
    REG_LINK = registerDictionary()->findOrThrow("lr");

    // Build the Capstone context object, which must be explicitly closed in the destructor.
    if (CS_ERR_OK != cs_open(CS_ARCH_ARM /*i.e., AArch32*/, mode, &capstone_))
        throw Exception("capstone cs_open failed");
    capstoneOpened_ = true;
    if (CS_ERR_OK != cs_option(capstone_, CS_OPT_DETAIL, CS_OPT_ON))
        throw Exception("capstone cs_option failed");
}

DisassemblerAarch32::~DisassemblerAarch32() {
    if (capstoneOpened_) {
        cs_err err = cs_close(&capstone_);
        ASSERT_always_require2(CS_ERR_OK == err, "capstone cs_close failed");
    }
}

Unparser::BasePtr
DisassemblerAarch32::unparser() const {
    return Unparser::Aarch32::instance();
}

SgAsmInstruction*
DisassemblerAarch32::disassembleOne(const MemoryMap::Ptr &map, rose_addr_t va, AddressSet *successors/*=NULL*/) {
    // Resources that must be explicitly reclaimed before returning.
    struct Resources {
        cs_insn *csi = nullptr;
        size_t nInsns = 0;
        ~Resources() {
            if (csi)
                cs_free(csi, nInsns);
        }
    } r;

    // Read the encoded instruction bytes into a temporary buffer to be used by capstone
    if (va % instructionAlignment_ != 0)
        throw Exception("instruction pointer not aligned", va);
    if (va > 0xfffffffc)
        throw Exception("instruction pointer out of range", va);
    uint8_t bytes[4];                                   // largest possible instruction is 4 bytes
    ASSERT_require(sizeof bytes <= instructionAlignment_);
    size_t nRead = map->at(va).limit(instructionAlignment_).require(MemoryMap::EXECUTABLE).read(bytes).size();
    if (0 == nRead)
        throw Exception("short read", va);

    ASSERT_not_implemented("[Robb Matzke 2020-12-22]");
}

SgAsmInstruction*
DisassemblerAarch32::makeUnknownInstruction(const Exception &e) {
    SgAsmAarch32Instruction *insn = new SgAsmAarch32Instruction(e.ip, "unknown", ARM_INS_INVALID);
    SgAsmOperandList *operands = new SgAsmOperandList();
    insn->set_operandList(operands);
    operands->set_parent(insn);
    insn->set_raw_bytes(e.bytes);
    return insn;
}

} // namespace
} // namespace

#endif
