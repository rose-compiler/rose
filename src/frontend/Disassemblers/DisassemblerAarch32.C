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
            return !modes_.isSet(Mode::THUMB);
        } else if (header->get_exec_format()->get_word_size() == 2) {
            return modes_.isSet(Mode::THUMB);
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
    if (modes_.isSet(Mode::THUMB)) {
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

    if (modes_.isSet(Mode::MCLASS))
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
DisassemblerAarch32::disassembleOne(const MemoryMap::Ptr &map, rose_addr_t va, AddressSet *successors/*=nullptr*/) {
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

    // Disassemble the instruction with capstone
    r.nInsns = cs_disasm(capstone_, bytes, nRead, va, 1, &r.csi);
    if (0 == r.nInsns)
        return makeUnknownInstruction(Exception("unable to decode instruction", va, SgUnsignedCharList(bytes+0, bytes+nRead), 0));
    ASSERT_require(1 == r.nInsns);
    ASSERT_not_null(r.csi);
    ASSERT_require(r.csi->address == va);
    ASSERT_require(r.csi->detail);

    // Convert disassembled Capstone instruction to ROSE AST
    SgAsmInstruction *retval = nullptr;
    const cs_arm &detail = r.csi->detail->arm;
#if 1 // DEBGUGGING: show the disassembly string from capstone itself
    std::cerr <<"ROBB: capstone disassembly:" <<" " <<StringUtility::addrToString(va) <<":";
    for (size_t i = 0; i < r.csi->size; ++i)
        std::cerr <<" " <<StringUtility::toHex2(bytes[i], 8, false, false).substr(2);
    std::cerr <<" " <<r.csi->mnemonic <<" " <<r.csi->op_str <<"\n";
#endif

    try {
        auto operands = new SgAsmOperandList;
        for (uint8_t i = 0; i < detail.op_count; ++i) {
            SgAsmExpression *operand = makeOperand(*r.csi, detail.operands[i]);
            ASSERT_not_null(operand);
            ASSERT_not_null(operand->get_type());
            operands->get_operands().push_back(operand);
            operand->set_parent(operands);
        }
        wrapPrePostIncrement(operands, detail);
        auto insn = new SgAsmAarch32Instruction(va, r.csi->mnemonic, (Aarch32InstructionKind)r.csi->id, detail.cc);
        insn->set_raw_bytes(SgUnsignedCharList(r.csi->bytes, r.csi->bytes + r.csi->size));
        insn->set_updatesFlags(detail.update_flags);
        insn->set_operandList(operands);
        operands->set_parent(insn);
        retval = insn;
    } catch (const Exception &e) {
        retval = makeUnknownInstruction(Exception(e.what(), va, SgUnsignedCharList(bytes+0, bytes+nRead), 0));
    }

    // Note successors if necessary
    if (successors) {
        bool complete = false;
        *successors |= retval->getSuccessors(complete /*out*/);
    }

    return retval;
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

SgAsmExpression*
DisassemblerAarch32::makeOperand(const cs_insn &insn, const cs_arm_op &op) {
    SgAsmExpression *retval = nullptr;
    SgAsmType *u32 = SageBuilderAsm::buildTypeU32();

    switch (op.type) {
        case ARM_OP_INVALID:
            ASSERT_not_reachable("invalid operand type from Capstone");

        case ARM_OP_REG: {
            // FIXME[Robb Matzke 2021-01-05]: how do we know whether op.reg is an arm_reg or an arm_sysreg?
            RegisterDescriptor reg = makeRegister(static_cast<arm_reg>(op.reg));
            reg = subRegister(reg, op.vector_index);
            retval = new SgAsmDirectRegisterExpression(reg);
            retval->set_type(registerType(reg));
            break;
        }

        case ARM_OP_IMM:
            retval = new SgAsmIntegerValueExpression(op.imm, u32);
            break;

        case ARM_OP_MEM: {
            RegisterDescriptor reg = makeRegister(op.mem.base);
            auto base = new SgAsmDirectRegisterExpression(reg);
            base->set_type(registerType(reg));
            SgAsmExpression *addr = base;

            if (op.mem.index != ARM_REG_INVALID) {
                RegisterDescriptor indexReg = makeRegister(op.mem.index);
                auto index = new SgAsmDirectRegisterExpression(indexReg);
                index->set_type(registerType(indexReg));
                if (op.mem.disp != 0) {
                    auto disp = new SgAsmIntegerValueExpression(op.mem.disp, u32);
                    if (op.mem.lshift != 0) {
                        ASSERT_not_implemented("[Robb Matzke 2021-01-04]");
                    }
                } else {
                    addr = SageBuilderAsm::buildAddExpression(addr, index, u32);
                }
            } else if (op.mem.disp != 0) {
                auto disp = new SgAsmIntegerValueExpression(op.mem.disp, u32);
                addr = SageBuilderAsm::buildAddExpression(addr, disp, u32);
            }
            auto mre = new SgAsmMemoryReferenceExpression;
            mre->set_address(addr);
            mre->set_type(typeForMemoryRead(insn));
            retval = mre;
            break;
        }

        case ARM_OP_FP:
            retval = SageBuilderAsm::buildValueFloat(op.fp, SageBuilderAsm::buildIeee754Binary64());
            break;

        case ARM_OP_CIMM: {
            RegisterDescriptor reg = makeCoprocRegister(op.reg);
            retval = new SgAsmDirectRegisterExpression(reg);
            retval->set_type(registerType(reg));
            break;
        }

        case ARM_OP_PIMM:
            retval = new SgAsmAarch32Coprocessor(op.reg);
            retval->set_type(SageBuilderAsm::buildTypeU8());
            break;

        case ARM_OP_SETEND:
            ASSERT_not_implemented("[Robb Matzke 2021-01-04]");

        case ARM_OP_SYSREG: {
            retval = makeSystemRegister((arm_sysreg)op.reg);
            break;
        }
    }

    // Adjust for shift and rotate
    if (op.shift.type != ARM_SFT_INVALID && op.shift.value != 0) {
        switch (op.shift.type) {
            case ARM_SFT_INVALID:
                ASSERT_not_reachable("filtered out above");
            case ARM_SFT_ASR: {                         // shift with immediate const
                auto value = new SgAsmIntegerValueExpression(op.shift.value, u32);
                retval = SageBuilderAsm::buildAsrExpression(retval, value, u32);
                break;
            }
            case ARM_SFT_LSL: {                         // shift with immediate const
                auto value = new SgAsmIntegerValueExpression(op.shift.value, u32);
                retval = SageBuilderAsm::buildLslExpression(retval, value, u32);
                break;
            }
            case ARM_SFT_LSR: {                         // shift with immediate const
                auto value = new SgAsmIntegerValueExpression(op.shift.value, u32);
                retval = SageBuilderAsm::buildLsrExpression(retval, value, u32);
                break;
            }
            case ARM_SFT_ROR: {                         // shift with immediate const
                auto value = new SgAsmIntegerValueExpression(op.shift.value, u32);
                retval = SageBuilderAsm::buildRorExpression(retval, value, u32);
                break;
            }
            case ARM_SFT_RRX: {                         // rotate right with extend
                RegisterDescriptor carryReg = registerDictionary()->find("cpsr_c");
                auto carry = new SgAsmDirectRegisterExpression(carryReg);
                carry->set_type(registerType(carryReg));
                retval = SageBuilderAsm::buildConcatExpression(carry, retval);
                auto value = new SgAsmIntegerValueExpression(op.shift.value, u32);
                retval = SageBuilderAsm::buildRorExpression(retval, value, retval->get_type());
                break;
            }
            case ARM_SFT_ASR_REG: {                     // shift with register
                RegisterDescriptor reg = makeRegister(static_cast<arm_reg>(op.shift.value));
                auto value = new SgAsmDirectRegisterExpression(reg);
                value->set_type(registerType(reg));
                retval = SageBuilderAsm::buildAsrExpression(retval, value, u32);
                break;
            }
            case ARM_SFT_LSL_REG: {                     // shift with register
                RegisterDescriptor reg = makeRegister(static_cast<arm_reg>(op.shift.value));
                auto value = new SgAsmDirectRegisterExpression(reg);
                value->set_type(registerType(reg));
                retval = SageBuilderAsm::buildLslExpression(retval, value, u32);
                break;
            }
            case ARM_SFT_LSR_REG: {                     // shift with register
                RegisterDescriptor reg = makeRegister(static_cast<arm_reg>(op.shift.value));
                auto value = new SgAsmDirectRegisterExpression(reg);
                value->set_type(registerType(reg));
                retval = SageBuilderAsm::buildLsrExpression(retval, value, u32);
                break;
            }
            case ARM_SFT_ROR_REG: {                     // shift with register
                RegisterDescriptor reg = makeRegister(static_cast<arm_reg>(op.shift.value));
                auto value = new SgAsmDirectRegisterExpression(reg);
                value->set_type(registerType(reg));
                retval = SageBuilderAsm::buildRorExpression(retval, value, u32);
                break;
            }
            case ARM_SFT_RRX_REG: {                     // shift with register
                RegisterDescriptor carryReg = registerDictionary()->find("cpsr_c");
                auto carry = new SgAsmDirectRegisterExpression(carryReg);
                carry->set_type(registerType(carryReg));
                retval = SageBuilderAsm::buildConcatExpression(carry, retval);

                RegisterDescriptor reg = makeRegister(static_cast<arm_reg>(op.shift.value));
                auto value = new SgAsmDirectRegisterExpression(reg);
                value->set_type(registerType(reg));

                retval = SageBuilderAsm::buildRorExpression(retval, value, retval->get_type());
                break;
            }
        }
    }

    ASSERT_not_null(retval);
    ASSERT_not_null(retval->get_type());
    return retval;
}

SgAsmType*
DisassemblerAarch32::registerType(RegisterDescriptor reg) {
    return SageBuilderAsm::buildTypeU(reg.nBits());
}

RegisterDescriptor
DisassemblerAarch32::makeCoprocRegister(int registerNumber) {
    return RegisterDescriptor(aarch32_regclass_coproc, registerNumber, 0, 32);
}

RegisterDescriptor
DisassemblerAarch32::makeRegister(arm_reg reg) {
    ASSERT_not_null(registerDictionary());
    const RegisterDictionary &dict = *registerDictionary();
    RegisterDescriptor retval;

    switch (reg) {
        case ARM_REG_INVALID:
        case ARM_REG_ENDING:
            ASSERT_not_reachable("invalid register from Capstone");
        case ARM_REG_APSR:
        case ARM_REG_CPSR:                              // alias for APSR
            retval = dict.find("apsr");
            break;
        case ARM_REG_APSR_NZCV:
            retval = dict.find("apsr_nzcv");
            break;
        case ARM_REG_LR:
            retval = dict.find("lr");
            break;
        case ARM_REG_PC:
            retval = dict.find("pc");
            break;
        case ARM_REG_SP:
            retval = dict.find("sp");
            break;
        case ARM_REG_R0:
            retval = dict.find("r0");
            break;
        case ARM_REG_R1:
            retval = dict.find("r1");
            break;
        case ARM_REG_R2:
            retval = dict.find("r2");
            break;
        case ARM_REG_R3:
            retval = dict.find("r3");
            break;
        case ARM_REG_R4:
            retval = dict.find("r4");
            break;
        case ARM_REG_R5:
            retval = dict.find("r5");
            break;
        case ARM_REG_R6:
            retval = dict.find("r6");
            break;
        case ARM_REG_R7:
            retval = dict.find("r7");
            break;
        case ARM_REG_R8:
            retval = dict.find("r8");
            break;
        case ARM_REG_R9:
            retval = dict.find("r9");
            break;
        case ARM_REG_R10:
            retval = dict.find("r10");
            break;
        case ARM_REG_R11:
            retval = dict.find("r11");
            break;
        case ARM_REG_R12:
            retval = dict.find("r12");
            break;
        case ARM_REG_Q0:
            retval = dict.find("q0");
            break;
        case ARM_REG_Q1:
            retval = dict.find("q1");
            break;
        case ARM_REG_Q2:
            retval = dict.find("q2");
            break;
        case ARM_REG_Q3:
            retval = dict.find("q3");
            break;
        case ARM_REG_Q4:
            retval = dict.find("q4");
            break;
        case ARM_REG_Q5:
            retval = dict.find("q5");
            break;
        case ARM_REG_Q6:
            retval = dict.find("q6");
            break;
        case ARM_REG_Q7:
            retval = dict.find("q7");
            break;
        case ARM_REG_Q8:
            retval = dict.find("q8");
            break;
        case ARM_REG_Q9:
            retval = dict.find("q9");
            break;
        case ARM_REG_Q10:
            retval = dict.find("q10");
            break;
        case ARM_REG_Q11:
            retval = dict.find("q11");
            break;
        case ARM_REG_Q12:
            retval = dict.find("q12");
            break;
        case ARM_REG_Q13:
            retval = dict.find("q13");
            break;
        case ARM_REG_Q14:
            retval = dict.find("q14");
            break;
        case ARM_REG_Q15:
            retval = dict.find("q15");
            break;
        case ARM_REG_D0:
            retval = dict.find("d0");
            break;
        case ARM_REG_D1:
            retval = dict.find("d1");
            break;
        case ARM_REG_D2:
            retval = dict.find("d2");
            break;
        case ARM_REG_D3:
            retval = dict.find("d3");
            break;
        case ARM_REG_D4:
            retval = dict.find("d4");
            break;
        case ARM_REG_D5:
            retval = dict.find("d5");
            break;
        case ARM_REG_D6:
            retval = dict.find("d6");
            break;
        case ARM_REG_D7:
            retval = dict.find("d7");
            break;
        case ARM_REG_D8:
            retval = dict.find("d8");
            break;
        case ARM_REG_D9:
            retval = dict.find("d9");
            break;
        case ARM_REG_D10:
            retval = dict.find("d10");
            break;
        case ARM_REG_D11:
            retval = dict.find("d11");
            break;
        case ARM_REG_D12:
            retval = dict.find("d12");
            break;
        case ARM_REG_D13:
            retval = dict.find("d13");
            break;
        case ARM_REG_D14:
            retval = dict.find("d14");
            break;
        case ARM_REG_D15:
            retval = dict.find("d15");
            break;
        case ARM_REG_D16:
            retval = dict.find("d16");
            break;
        case ARM_REG_D17:
            retval = dict.find("d17");
            break;
        case ARM_REG_D18:
            retval = dict.find("d18");
            break;
        case ARM_REG_D19:
            retval = dict.find("d19");
            break;
        case ARM_REG_D20:
            retval = dict.find("d20");
            break;
        case ARM_REG_D21:
            retval = dict.find("d21");
            break;
        case ARM_REG_D22:
            retval = dict.find("d22");
            break;
        case ARM_REG_D23:
            retval = dict.find("d23");
            break;
        case ARM_REG_D24:
            retval = dict.find("d24");
            break;
        case ARM_REG_D25:
            retval = dict.find("d25");
            break;
        case ARM_REG_D26:
            retval = dict.find("d26");
            break;
        case ARM_REG_D27:
            retval = dict.find("d27");
            break;
        case ARM_REG_D28:
            retval = dict.find("d28");
            break;
        case ARM_REG_D29:
            retval = dict.find("d29");
            break;
        case ARM_REG_D30:
            retval = dict.find("d30");
            break;
        case ARM_REG_D31:
            retval = dict.find("d31");
            break;
        case ARM_REG_S0:
            retval = dict.find("s0");
            break;
        case ARM_REG_S1:
            retval = dict.find("s1");
            break;
        case ARM_REG_S2:
            retval = dict.find("s2");
            break;
        case ARM_REG_S3:
            retval = dict.find("s3");
            break;
        case ARM_REG_S4:
            retval = dict.find("s4");
            break;
        case ARM_REG_S5:
            retval = dict.find("s5");
            break;
        case ARM_REG_S6:
            retval = dict.find("s6");
            break;
        case ARM_REG_S7:
            retval = dict.find("s7");
            break;
        case ARM_REG_S8:
            retval = dict.find("s8");
            break;
        case ARM_REG_S9:
            retval = dict.find("s9");
            break;
        case ARM_REG_S10:
            retval = dict.find("s10");
            break;
        case ARM_REG_S11:
            retval = dict.find("s11");
            break;
        case ARM_REG_S12:
            retval = dict.find("s12");
            break;
        case ARM_REG_S13:
            retval = dict.find("s13");
            break;
        case ARM_REG_S14:
            retval = dict.find("s14");
            break;
        case ARM_REG_S15:
            retval = dict.find("s15");
            break;
        case ARM_REG_S16:
            retval = dict.find("s16");
            break;
        case ARM_REG_S17:
            retval = dict.find("s17");
            break;
        case ARM_REG_S18:
            retval = dict.find("s18");
            break;
        case ARM_REG_S19:
            retval = dict.find("s19");
            break;
        case ARM_REG_S20:
            retval = dict.find("s20");
            break;
        case ARM_REG_S21:
            retval = dict.find("s21");
            break;
        case ARM_REG_S22:
            retval = dict.find("s22");
            break;
        case ARM_REG_S23:
            retval = dict.find("s23");
            break;
        case ARM_REG_S24:
            retval = dict.find("s24");
            break;
        case ARM_REG_S25:
            retval = dict.find("s25");
            break;
        case ARM_REG_S26:
            retval = dict.find("s26");
            break;
        case ARM_REG_S27:
            retval = dict.find("s27");
            break;
        case ARM_REG_S28:
            retval = dict.find("s28");
            break;
        case ARM_REG_S29:
            retval = dict.find("s29");
            break;
        case ARM_REG_S30:
            retval = dict.find("s30");
            break;
        case ARM_REG_S31:
            retval = dict.find("s31");
            break;
        case ARM_REG_SPSR:
            // When we're decoding an instruction to produce ROSE's AST we don't know what interrupt handling state the
            // processor will be in when the instruction is eventually executed. Since the instruction has a single encoding
            // for all the banked SPSR registers, we use a placeholder SPSR register that will be resolved to the correct
            // hardware SPSR_* register when the instruction is executed.
            retval = dict.find("spsr");
            break;
        case ARM_REG_FPSID:
            retval = dict.find("fpsid");
            break;
        case ARM_REG_FPSCR:
            retval = dict.find("fpscr");
            break;
        case ARM_REG_FPSCR_NZCV:
            retval = dict.find("fpscr_nzcv");
            break;
        case ARM_REG_FPEXC:
            retval = dict.find("fpexc");
            break;
        case ARM_REG_FPINST:
            retval = dict.find("fpinst");
            break;
        case ARM_REG_FPINST2:
            retval = dict.find("fpinst2");
            break;
        case ARM_REG_MVFR0:
            retval = dict.find("mvfr0");
            break;
        case ARM_REG_MVFR1:
            retval = dict.find("mvfr1");
            break;
        case ARM_REG_MVFR2:
            retval = dict.find("mvfr2");
            break;
        case ARM_REG_ITSTATE:
            retval = dict.find("itstate");
            break;
    }

    ASSERT_require2(retval, "reg=" + boost::lexical_cast<std::string>(reg));
    return retval;
}

SgAsmExpression*
DisassemblerAarch32::makeSystemRegister(arm_sysreg capreg) {
    ASSERT_not_null(registerDictionary());
    const RegisterDictionary &dict = *registerDictionary();
    SgAsmExpression *retval = nullptr;
    RegisterDescriptor reg;

    // The ARM_SYSREG_SPSR_* and ARM_SYSREG_CPSR_* enumerators can be OR'd to represent possibly discontiguous regions of the
    // hardware register. This can't be described by a single RegisterDescriptor or SgAsmDirectRegisterReference
    // expression. Therefore, we need to wrap the parts in a list (SgAsmRegisterNames).
    //
    // Furthermore, when we're decoding an instruction to produce ROSE's AST we don't know what interrupt handling state the
    // processor will be in when the instruction is eventually executed. Since the instruction has a single encoding for all
    // the banked SPSR registers, we use a placeholder SPSR register that will be resolved to the correct hardware SPSR_*
    // register when the instruction is executed.
    if ((capreg & (ARM_SYSREG_SPSR_C | ARM_SYSREG_SPSR_X | ARM_SYSREG_SPSR_S | ARM_SYSREG_SPSR_F)) != 0 &&
        BitOps::nSet((unsigned)capreg) > 1) {
        auto regList = new SgAsmRegisterNames;
        if ((capreg & ARM_SYSREG_SPSR_C) != 0) {
            reg = dict.find("spsr_control");
            auto regExpr = new SgAsmDirectRegisterExpression(reg);
            regExpr->set_type(registerType(reg));
            regList->get_registers().push_back(regExpr);
        }
        if ((capreg & ARM_SYSREG_SPSR_X) != 0) {
            reg = dict.find("spsr_extension");
            auto regExpr = new SgAsmDirectRegisterExpression(reg);
            regExpr->set_type(registerType(reg));
            regList->get_registers().push_back(regExpr);
        }
        if ((capreg & ARM_SYSREG_SPSR_S) != 0) {
            reg = dict.find("spsr_status");
            auto regExpr = new SgAsmDirectRegisterExpression(reg);
            regExpr->set_type(registerType(reg));
            regList->get_registers().push_back(regExpr);
        }
        if ((capreg & ARM_SYSREG_SPSR_F) != 0) {
            reg = dict.find("spsr_flags");
            auto regExpr = new SgAsmDirectRegisterExpression(reg);
            regExpr->set_type(registerType(reg));
            regList->get_registers().push_back(regExpr);
        }
        retval = regList;
    } else if ((capreg & (ARM_SYSREG_CPSR_C | ARM_SYSREG_CPSR_X | ARM_SYSREG_CPSR_S | ARM_SYSREG_CPSR_F)) != 0 &&
               BitOps::nSet((unsigned)capreg) > 1) {
        auto regList = new SgAsmRegisterNames;
        if ((capreg & ARM_SYSREG_CPSR_C) != 0) {
            reg = dict.find("cpsr_control");
            auto regExpr = new SgAsmDirectRegisterExpression(reg);
            regExpr->set_type(registerType(reg));
            regList->get_registers().push_back(regExpr);
        }
        if ((capreg & ARM_SYSREG_CPSR_X) != 0) {
            reg = dict.find("cpsr_extension");
            auto regExpr = new SgAsmDirectRegisterExpression(reg);
            regExpr->set_type(registerType(reg));
            regList->get_registers().push_back(regExpr);
        }
        if ((capreg & ARM_SYSREG_CPSR_S) != 0) {
            reg = dict.find("cpsr_status");
            auto regExpr = new SgAsmDirectRegisterExpression(reg);
            regExpr->set_type(registerType(reg));
            regList->get_registers().push_back(regExpr);
        }
        if ((capreg & ARM_SYSREG_CPSR_F) != 0) {
            reg = dict.find("cpsr_flags");
            auto regExpr = new SgAsmDirectRegisterExpression(reg);
            regExpr->set_type(registerType(reg));
            regList->get_registers().push_back(regExpr);
        }
        retval = regList;
    } else {
        switch (capreg) {
            // The register can be invalid for certain encodings. For instance, the a64 encoding 0x05f22741 is a msrlo instruction whose
            // first operand is "UNPREDICTABLE". Capstone decodes this as ARM_SYSREG_INVALID, which we turn into an exception that's
            // caught above and turned into an "unknown" instruction.
            case ARM_SYSREG_INVALID:
                throw Exception("unpredictable system register");

            // when we're decoding an instruction to produce ROSE's AST we don't know what interrupt handling state the
            // processor will be in when the instruction is eventually executed. Since the instruction has a single encoding
            // for all the banked SPSR registers, we use a placeholder SPSR register that will be resolved to the correct
            // hardware SPSR_* register when the instruction is executed.
            case ARM_SYSREG_SPSR_C:
                reg = dict.find("spsr_control");
                break;
            case ARM_SYSREG_SPSR_X:
                reg = dict.find("spsr_extension");
                break;
            case ARM_SYSREG_SPSR_S:
                reg = dict.find("spsr_status");
                break;
            case ARM_SYSREG_SPSR_F:
                reg = dict.find("spsr_flags");
                break;

            case ARM_SYSREG_CPSR_C:
                reg = dict.find("cpsr_control");
                break;
            case ARM_SYSREG_CPSR_X:
                reg = dict.find("cpsr_extension");
                break;
            case ARM_SYSREG_CPSR_S:
                reg = dict.find("cpsr_status");
                break;
            case ARM_SYSREG_CPSR_F:
                reg = dict.find("cpsr_flags");
                break;
            case ARM_SYSREG_APSR:
                reg = dict.find("apsr");
                break;
            case ARM_SYSREG_APSR_NZCVQ:
                reg = dict.find("apsr_nzcvq");
                break;
            case ARM_SYSREG_APSR_NZCVQG: {                  // undocumented in capstone, but perhaps N, Z, C, V, Q, and GE
                // These are discontiguous parts of the register, so we need to create a list of register parts.
                RegisterDescriptor reg = dict.find("apsr_nzcvq");
                auto nzcvq = new SgAsmDirectRegisterExpression(reg);
                nzcvq->set_type(registerType(reg));

                reg = dict.find("apsr_ge");
                auto ge = new SgAsmDirectRegisterExpression(reg);
                ge->set_type(registerType(reg));

                auto regList = new SgAsmRegisterNames;
                regList->get_registers().push_back(nzcvq);
                regList->get_registers().push_back(ge);
                retval = regList;
                break;
            }

            case ARM_SYSREG_APSR_G:
                ASSERT_not_implemented("apsr_g");           // undocumented

            // IAPSR (not sure what this is)
            case ARM_SYSREG_IAPSR:
                reg = dict.find("iapsr");
                break;
            case ARM_SYSREG_IAPSR_G:
                ASSERT_not_implemented("iapsr_g");          // undocumented
            case ARM_SYSREG_IAPSR_NZCVQG:
                ASSERT_not_implemented("iapsr_nzcvqg");     // undocumented
            case ARM_SYSREG_IAPSR_NZCVQ:
                ASSERT_not_implemented("iapsr*");           // undocumented

            // EAPSR (not sure what this is)
            case ARM_SYSREG_EAPSR:
                reg = dict.find("eapsr");
                break;
            case ARM_SYSREG_EAPSR_G:
                ASSERT_not_implemented("eapsr_g");          // undocumented
            case ARM_SYSREG_EAPSR_NZCVQG:
                ASSERT_not_implemented("eapsr_nzcvqg");     // undocumented
            case ARM_SYSREG_EAPSR_NZCVQ:
                ASSERT_not_implemented("eapsr_nzcvq");      // undocumented

            // XPSR (not sure what this is)
            case ARM_SYSREG_XPSR:
            case ARM_SYSREG_XPSR_G:
            case ARM_SYSREG_XPSR_NZCVQG:
            case ARM_SYSREG_XPSR_NZCVQ:
                ASSERT_not_implemented("xpsr");             // undocumented

            // Other registers with unclear purposes
            case ARM_SYSREG_IPSR:
                reg = dict.find("ipsr");
                break;
            case ARM_SYSREG_EPSR:
                reg = dict.find("epsr");
                break;
            case ARM_SYSREG_IEPSR:
                reg = dict.find("iepsr");
                break;
            case ARM_SYSREG_MSP:
                reg = dict.find("msp");
                break;
            case ARM_SYSREG_PSP:
                reg = dict.find("psp");
                break;
            case ARM_SYSREG_PRIMASK:
                reg = dict.find("primask");
                break;
            case ARM_SYSREG_BASEPRI:
                ASSERT_not_implemented("basepri");          // undocumented
            case ARM_SYSREG_BASEPRI_MAX:
                ASSERT_not_implemented("basepri_max");      // undocumented
            case ARM_SYSREG_FAULTMASK:
                ASSERT_not_implemented("faultmask");        // undocumented
            case ARM_SYSREG_CONTROL:
                reg = dict.find("control");
                break;

            // Banked registers
            case ARM_SYSREG_R8_USR:
                reg = dict.find("r8_usr");
                break;
            case ARM_SYSREG_R9_USR:
                reg = dict.find("r9_usr");
                break;
            case ARM_SYSREG_R10_USR:
                reg = dict.find("r10_usr");
                break;
            case ARM_SYSREG_R11_USR:
                reg = dict.find("r11_usr");
                break;
            case ARM_SYSREG_R12_USR:
                reg = dict.find("r12_usr");
                break;
            case ARM_SYSREG_SP_USR:
                reg = dict.find("sp_usr");
                break;
            case ARM_SYSREG_LR_USR:
                reg = dict.find("lr_usr");
                break;
            case ARM_SYSREG_R8_FIQ:
                reg = dict.find("r8_fiq");
                break;
            case ARM_SYSREG_R9_FIQ:
                reg = dict.find("r9_fiz");
                break;
            case ARM_SYSREG_R10_FIQ:
                reg = dict.find("r10_fiq");
                break;
            case ARM_SYSREG_R11_FIQ:
                reg = dict.find("r11_fiq");
                break;
            case ARM_SYSREG_R12_FIQ:
                reg = dict.find("r12_fiq");
                break;
            case ARM_SYSREG_SP_FIQ:
                reg = dict.find("sp_fiq");
                break;
            case ARM_SYSREG_LR_FIQ:
                reg = dict.find("lr_fiq");
                break;
            case ARM_SYSREG_LR_IRQ:
                reg = dict.find("lr_irq");
                break;
            case ARM_SYSREG_SP_IRQ:
                reg = dict.find("sp_irq");
                break;
            case ARM_SYSREG_LR_SVC:
                reg = dict.find("lr_svc");
                break;
            case ARM_SYSREG_SP_SVC:
                reg = dict.find("sp_svc");
                break;
            case ARM_SYSREG_LR_ABT:
                reg = dict.find("lr_abt");
                break;
            case ARM_SYSREG_SP_ABT:
                reg = dict.find("sp_abt");
                break;
            case ARM_SYSREG_LR_UND:
                reg = dict.find("lr_und");
                break;
            case ARM_SYSREG_SP_UND:
                reg = dict.find("sp_und");
                break;
            case ARM_SYSREG_LR_MON:
                reg = dict.find("lr_mon");
                break;
            case ARM_SYSREG_SP_MON:
                reg = dict.find("sp_mon");
                break;
            case ARM_SYSREG_ELR_HYP:
                reg = dict.find("sp_hyp");
                break;
            case ARM_SYSREG_SP_HYP:
                reg = dict.find("sp_hyp");
                break;
            case ARM_SYSREG_SPSR_FIQ:
                reg = dict.find("spsr_fiq");
                break;
            case ARM_SYSREG_SPSR_IRQ:
                reg = dict.find("spsr_irq");
                break;
            case ARM_SYSREG_SPSR_SVC:
                reg = dict.find("spsr_svc");
                break;
            case ARM_SYSREG_SPSR_ABT:
                reg = dict.find("spsr_abt");
                break;
            case ARM_SYSREG_SPSR_UND:
                reg = dict.find("spsr_und");
                break;
            case ARM_SYSREG_SPSR_MON:
                reg = dict.find("spsr_mon");
                break;
            case ARM_SYSREG_SPSR_HYP:
                reg = dict.find("spsr_hyp");
                break;

            default:
                ASSERT_not_reachable("register not handled; reg = " + boost::lexical_cast<std::string>(capreg));
        }
    }

    if (!retval) {
        ASSERT_require(reg);
        retval = new SgAsmDirectRegisterExpression(reg);
    }
    if (!retval->get_type()) {
        ASSERT_require(reg);
        retval->set_type(registerType(reg));
    }

    return retval;
}

SgAsmType*
DisassemblerAarch32::typeForMemoryRead(const cs_insn &insn) {
    using namespace Rose::BitOps;
    using Kind = ::Rose::BinaryAnalysis::Aarch32InstructionKind;
    uint32_t code = opcode(insn);

    switch (static_cast<Kind>(insn.id)) {
        case Kind::ARM_INS_LDA:
        case Kind::ARM_INS_LDAEX:
        case Kind::ARM_INS_LDR:
        case Kind::ARM_INS_LDREX:
        case Kind::ARM_INS_LDRT:
        case Kind::ARM_INS_STL:
        case Kind::ARM_INS_STLEX:
        case Kind::ARM_INS_STR:
        case Kind::ARM_INS_STREX:
        case Kind::ARM_INS_STRT:
        case Kind::ARM_INS_SWP:
            return SageBuilderAsm::buildTypeU32();

        case Kind::ARM_INS_LDAB:
        case Kind::ARM_INS_LDAEXB:
        case Kind::ARM_INS_LDRB:
        case Kind::ARM_INS_LDRBT:
        case Kind::ARM_INS_LDREXB:
        case Kind::ARM_INS_LDRSB:
        case Kind::ARM_INS_LDRSBT:
        case Kind::ARM_INS_STLB:
        case Kind::ARM_INS_STLEXB:
        case Kind::ARM_INS_STRB:
        case Kind::ARM_INS_STRBT:
        case Kind::ARM_INS_STREXB:
        case Kind::ARM_INS_SWPB:
            return SageBuilderAsm::buildTypeU8();

        case Kind::ARM_INS_LDAEXH:
        case Kind::ARM_INS_LDAH:
        case Kind::ARM_INS_LDREXH:
        case Kind::ARM_INS_LDRH:
        case Kind::ARM_INS_LDRHT:
        case Kind::ARM_INS_LDRSH:
        case Kind::ARM_INS_LDRSHT:
        case Kind::ARM_INS_STLEXH:
        case Kind::ARM_INS_STLH:
        case Kind::ARM_INS_STREXH:
        case Kind::ARM_INS_STRH:
        case Kind::ARM_INS_STRHT:
            return SageBuilderAsm::buildTypeU16();

        case Kind::ARM_INS_LDAEXD:
        case Kind::ARM_INS_LDRD:
        case Kind::ARM_INS_LDREXD:
        case Kind::ARM_INS_STLEXD:
        case Kind::ARM_INS_STRD:
        case Kind::ARM_INS_STREXD:
            return SageBuilderAsm::buildTypeU64();

        case Kind::ARM_INS_LDM:
        case Kind::ARM_INS_LDMDA:
        case Kind::ARM_INS_LDMDB:
        case Kind::ARM_INS_LDMIB:
        case Kind::ARM_INS_POP:
        case Kind::ARM_INS_PUSH:
        case Kind::ARM_INS_STM:
        case Kind::ARM_INS_STMDA:
        case Kind::ARM_INS_STMDB:
        case Kind::ARM_INS_STMIB:
        case Kind::ARM_INS_VLDMDB:
        case Kind::ARM_INS_VLDMIA:
        case Kind::ARM_INS_VSTMDB:
        case Kind::ARM_INS_VSTMIA:
            ASSERT_not_implemented("kind=" + boost::lexical_cast<std::string>(insn.id));

        case Kind::ARM_INS_VLD1:
        case Kind::ARM_INS_VLD2:
        case Kind::ARM_INS_VLD3:
        case Kind::ARM_INS_VLD4:
        case Kind::ARM_INS_VST1:
        case Kind::ARM_INS_VST2:
        case Kind::ARM_INS_VST3:
        case Kind::ARM_INS_VST4:
            return SageBuilderAsm::buildTypeU(insn.detail->arm.vector_size); // "scalar size for vector instruction"

        case Kind::ARM_INS_LDC:
        case Kind::ARM_INS_LDC2:
        case Kind::ARM_INS_LDC2L:
        case Kind::ARM_INS_LDCL:
        case Kind::ARM_INS_STC:
        case Kind::ARM_INS_STC2:
        case Kind::ARM_INS_STC2L:
        case Kind::ARM_INS_STCL:
            // These access an unknown amount of memory based on the coprocessor. The best we can do here is to say that access
            // at least one byte.
            return SageBuilderAsm::buildTypeU32();

        case Kind::ARM_INS_PLD:
        case Kind::ARM_INS_PLDW:
        case Kind::ARM_INS_PLI:
            // These are only hints that memory will be accessed sometime in the near future. The type doesn't actually matter.
            return SageBuilderAsm::buildTypeU32();

        case Kind::ARM_INS_VLDR:
        case Kind::ARM_INS_VSTR:
            switch (bits(code, 8, 9)) {                 // size field for both A32 and T32
                case 1:
                    return SageBuilderAsm::buildTypeU16();
                case 2:
                    return SageBuilderAsm::buildTypeU32();
                case 3:
                    return SageBuilderAsm::buildTypeU64();
                default:
                    ASSERT_require("invalid size field in bits 8 and 9: 0b00");
            }

        default:
            ASSERT_not_reachable("memory read instruction not handled");
    }
}

uint32_t
DisassemblerAarch32::opcode(const cs_insn &insn) {
    ASSERT_require(insn.size <= 4);
    uint32_t retval = 0;
    for (size_t i = 0; i < insn.size; ++i)
        retval |= uint32_t(insn.bytes[i]) << (8 * i);
    return retval;
}

RegisterDescriptor
DisassemblerAarch32::subRegister(RegisterDescriptor reg, int idx) {
    // This isn't really complete yet, just stubbed out to return something that doesn't crash ROSE.
    if (idx >= 0)
        mlog[WARN] <<"register indexing not supported yet for A32/T32\n";
    return reg;
}

void
DisassemblerAarch32::wrapPrePostIncrement(SgAsmOperandList *operands, const cs_arm &insn) {
}

} // namespace
} // namespace

#endif
