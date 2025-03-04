#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_AARCH32
#include <Rose/BinaryAnalysis/Disassembler/Aarch32.h>

#include <Rose/AST/Traversal.h>
#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <Rose/BitOps.h>

#include <SgAsmAarch32Coprocessor.h>
#include <SgAsmAarch32Instruction.h>
#include <SgAsmBinaryAdd.h>
#include <SgAsmBinaryAsr.h>
#include <SgAsmBinaryConcat.h>
#include <SgAsmBinaryLsl.h>
#include <SgAsmBinaryLsr.h>
#include <SgAsmBinaryPostupdate.h>
#include <SgAsmBinaryRor.h>
#include <SgAsmBinaryPreupdate.h>
#include <SgAsmBinarySubtract.h>
#include <SgAsmByteOrder.h>
#include <SgAsmDirectRegisterExpression.h>
#include <SgAsmFloatType.h>
#include <SgAsmFloatValueExpression.h>
#include <SgAsmIntegerType.h>
#include <SgAsmIntegerValueExpression.h>
#include <SgAsmMemoryReferenceExpression.h>
#include <SgAsmRegisterNames.h>

#include <Cxx_GrammarDowncast.h>
#include <SageBuilderAsm.h>

using namespace Rose::Diagnostics;

namespace Rose {
namespace BinaryAnalysis {
namespace Disassembler {

Aarch32::Aarch32(const Architecture::Base::ConstPtr &arch, Modes modes)
    : Base(arch), modes_(modes) {
    // ROSE disassembler properties, and choose a somewhat descriptive name (at least something better than "ARM").
    if (modes_.isSet(Mode::THUMB)) {
        instructionAlignment_ = 2;
    } else {
        instructionAlignment_ = 4;
    }

    // Architecture independent ROSE disassembler properties
    REG_IP = architecture()->registerDictionary()->instructionPointerRegister();
    REG_SP = architecture()->registerDictionary()->stackPointerRegister();
    REG_SF = architecture()->registerDictionary()->stackFrameRegister();
    REG_LINK = architecture()->registerDictionary()->callReturnRegister();
    ASSERT_require(REG_IP);
    ASSERT_require(REG_SP);
    ASSERT_require(REG_SF);
    ASSERT_require(REG_LINK);
}

Aarch32::Ptr
Aarch32::instanceA32(const Architecture::Base::ConstPtr &arch) {
    return Ptr(new Aarch32(arch, Modes(Mode::ARM32)));
}

Aarch32::Ptr
Aarch32::instanceT32(const Architecture::Base::ConstPtr &arch) {
    return Ptr(new Aarch32(arch, Modes(Mode::THUMB)));
}

Aarch32::Ptr
Aarch32::instance(const Architecture::Base::ConstPtr &arch) {
    return Ptr(new Aarch32(arch));
}

Base::Ptr
Aarch32::clone() const {
    return Ptr(new Aarch32(*this));
}

void
Aarch32::openCapstone() {
    // Build the Capstone context object, which must be explicitly closed in the destructor. Furthermore, since capstone is a
    // shared library, it might not be possible to call cs_close (in the Aarch32 destructor) after libcapstone has been shut
    // down during program termination. For instance, if you store any static pointers to Aarch32 objects that have open
    // capstone connections, then its likely that the C++ runtime will close the capstone library before calling the
    // destructors for those Aarch32 objects.

    // Warning: the "mode" constants are not orthogonal with each other or the "arch" values.
    cs_mode mode = (cs_mode)modes_.vector();

    if (CS_ERR_OK != cs_open(CS_ARCH_ARM /*i.e., AArch32*/, mode, &capstone_))
        throw Exception("capstone cs_open failed");
    capstoneOpened_ = true;
    if (CS_ERR_OK != cs_option(capstone_, CS_OPT_DETAIL, CS_OPT_ON))
        throw Exception("capstone cs_option failed");
}

Aarch32::~Aarch32() {
    if (capstoneOpened_) {
        cs_err err = cs_close(&capstone_);
        ASSERT_always_require2(CS_ERR_OK == err, "capstone cs_close failed");
    }
}

uint32_t
Aarch32::bytesToWord(size_t nBytes, const uint8_t *bytes) {
    ASSERT_require(nBytes <= 4);
    uint32_t retval = 0;
    switch (byteOrder()) {
        case ByteOrder::ORDER_LSB:
            for (size_t i = 0; i < nBytes; ++i)
                retval |= uint32_t(bytes[i]) << (8*i);
            break;
        case ByteOrder::ORDER_MSB:
            for (size_t i = 0; i < nBytes; ++i)
                retval |= uint32_t(bytes[nBytes - (i+1)]) << (8*i);
            break;
        default:
            ASSERT_not_reachable("invalid byte order");
    }
    return retval;
}

SgAsmInstruction*
Aarch32::disassembleOne(const MemoryMap::Ptr &map, Address va, AddressSet *successors/*=nullptr*/) {
    openCapstone();

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
    ASSERT_require(sizeof bytes >= instructionAlignment_);
    const size_t nRead = map->at(va).limit(instructionAlignment_).require(MemoryMap::EXECUTABLE).read(bytes).size();
    if (0 == nRead)
        throw Exception("short read", va);
    uint32_t word = bytesToWord(nRead, bytes);

    // Disassemble the instruction with capstone
    r.nInsns = cs_disasm(capstone_, bytes, nRead, va, 1, &r.csi);
    if (0 == r.nInsns)
        return makeUnknownInstruction(Exception("unable to decode instruction", va, SgUnsignedCharList(bytes+0, bytes+nRead), 0));
    ASSERT_require(1 == r.nInsns);
    ASSERT_not_null(r.csi);
    ASSERT_require(r.csi->address == va);
    ASSERT_require(r.csi->detail);

    //--------------------------------------------------------------------------------
    // Initial work-arounds for Capstone bugs
    //--------------------------------------------------------------------------------
    if (((arm_insn)r.csi->id == ARM_INS_RFEDA || (arm_insn)r.csi->id == ARM_INS_RFEDB ||
         (arm_insn)r.csi->id == ARM_INS_RFEIA || (arm_insn)r.csi->id == ARM_INS_RFEIB)) {
        if (BitOps::bits(word, 0, 15) != 0x0a00 /*A1*/ && BitOps::bits(word, 0, 15) != 0xc000 /*T1,T2*/) {
            // Capstone mis-decodes random data as this instruction. E.g., the word 0xf8364650 gets decoded as "rfeda #3!", which
            // doesn't even make any sense. Similarly, 0xf9b4f001 is "rfeib #2!".
            return isSgAsmAarch32Instruction(makeUnknownInstruction(Exception("unable to decode instruction", va,
                                                                              SgUnsignedCharList(bytes+0, bytes+nRead), 0)));
        } else if (r.csi->detail->arm.op_count == 1 && r.csi->detail->arm.operands[0].type == ARM_OP_IMM) {
            // Even when bits [0,15] are correct (0x0a00 or 0xc000), Capstone decodes invalid opcodes as this instruction but
            // with an argument like "#X!" where X is an integer. E.g., 0xf8bac000 is invalid (bits [25,31] are the A1 decoding
            // which requires bits [0,15] to be 0x0a00, not 0xc000.
            return isSgAsmAarch32Instruction(makeUnknownInstruction(Exception("unable to decode instruction", va,
                                                                              SgUnsignedCharList(bytes+0, bytes+nRead), 0)));
        }
    } else if ((arm_insn)r.csi->id == ARM_INS_STMDB) {
        if (r.csi->detail->arm.op_count < 2) {
            // When M=1 and register_list=0 capstone returns only one argument. This is always a two-argument instruction.
            return isSgAsmAarch32Instruction(makeUnknownInstruction(Exception("unable to decode instruction", va,
                                                                              SgUnsignedCharList(bytes+0, bytes+nRead), 0)));
        }
    }

    //--------------------------------------------------------------------------------
    // Convert disassembled Capstone instruction to ROSE AST
    //--------------------------------------------------------------------------------
    const cs_arm &detail = r.csi->detail->arm;
#if 0 // DEBGUGGING: show the disassembly string from capstone itself
    std::cerr <<"ROBB: capstone disassembly:" <<" " <<StringUtility::addrToString(va) <<":";
    for (size_t i = 0; i < r.csi->size; ++i)
        std::cerr <<" " <<StringUtility::toHex2(bytes[i], 8, false, false).substr(2);
    std::cerr <<" " <<r.csi->mnemonic <<" " <<r.csi->op_str <<"\n";
#endif

    SgAsmAarch32Instruction *retval = nullptr;
    try {
        auto operands = new SgAsmOperandList;
        for (uint8_t i = 0; i < detail.op_count; ++i) {
            SgAsmExpression *operand = makeOperand(*r.csi, detail.operands[i]);
            ASSERT_not_null(operand);
            ASSERT_not_null(operand->get_type());
            operands->get_operands().push_back(operand);
            operand->set_parent(operands);
        }
        auto insn = new SgAsmAarch32Instruction(va, *architecture()->registrationId(), (Aarch32InstructionKind)r.csi->id,
                                                detail.cc);
        insn->set_rawBytes(SgUnsignedCharList(r.csi->bytes, r.csi->bytes + r.csi->size));
        insn->set_updatesFlags(detail.update_flags);
        insn->set_operandList(operands);
        operands->set_parent(insn);
        wrapPrePostIncrement(insn, *r.csi, word);

        // What registers are written. Note: POP has a bug which is handled below. This bug probably occurs for any instruction that
        // writes to more than one register where the registers are specified in a single argument.
        for (uint8_t i = 0; i < r.csi->detail->regs_write_count; ++i) {
            if (ARM_REG_PC == r.csi->detail->regs_write[i]) {
                insn->set_writesToIp(true);
                break;
            }
        }
        retval = isSgAsmAarch32Instruction(insn);
    } catch (const Exception &e) {
        auto insn = makeUnknownInstruction(Exception(e.what(), va, SgUnsignedCharList(bytes+0, bytes+nRead), 0));
        retval = isSgAsmAarch32Instruction(insn);
    }
    ASSERT_not_null(retval);

    // Note successors if necessary
    if (successors) {
        bool complete = false;
        *successors |= architecture()->getSuccessors(retval, complete /*out*/);
    }

    //--------------------------------------------------------------------------------
    // Some assembly aliases are self evident in the non-alias version. For instance,
    // since nearly all instructions can shift and rotate arguments, the mnemonics for
    // doing so are still self-evident with the move instruction (consider ROSE syntax
    // for the alias "ASR R2, ASR(R1, 3)" versus the original "MOV R2, ASR(R1, 3)").
    //--------------------------------------------------------------------------------
    if ((arm_insn)r.csi->id == ARM_INS_ASR ||
        (arm_insn)r.csi->id == ARM_INS_LSL ||
        (arm_insn)r.csi->id == ARM_INS_LSR ||
        (arm_insn)r.csi->id == ARM_INS_ROR) {
        retval->set_kind(ARM_INS_MOV);
    }

    //--------------------------------------------------------------------------------
    // Additional Capstone bug workarounds not already handled.
    //--------------------------------------------------------------------------------
    if ((arm_insn)r.csi->id == ARM_INS_ADC) {
        // This instruction updates status flags, but Capstone says it doesn't.
        retval->set_updatesFlags((bytes[2] & 0x10) != 0);
    } else if ((arm_insn)r.csi->id == ARM_INS_SBC) {
        // This instruction updates status flags, but Capstone says it doesn't.
        retval->set_updatesFlags((bytes[2] & 0x10) != 0);
    } else if ((arm_insn)r.csi->id == ARM_INS_B ||
               (arm_insn)r.csi->id == ARM_INS_CBZ ||
               (arm_insn)r.csi->id == ARM_INS_CBNZ) {
        // These instruction writes to the instruction pointer register, but Capstone says they don't.
        retval->set_writesToIp(true);
    } else if ((arm_insn)r.csi->id == ARM_INS_POP) {
        // The r.csi->detail->regs_write[] array is wrong for this instruction.
        const RegisterDescriptor REG_PC(aarch32_regclass_gpr, aarch32_gpr_pc, 0, 32);
        for (size_t i = 0; i < retval->nOperands(); ++i) {
            if (auto rre = isSgAsmDirectRegisterExpression(retval->operand(i))) {
                if (rre->get_descriptor() == REG_PC) {
                    retval->set_writesToIp(true);
                    break;
                }
            }
        }
    } else if ((arm_insn)r.csi->id == ARM_INS_ADD && retval->nOperands() == 4) {
        // This instruction is only supposed to have three arguments. The imm12 part of the encoding is broken into two parts:
        // the low-order 8 bits are rotate right by twice the high-order 4 bits. Obviously, if the low-order 8 bits are zero
        // then it doesn't matter what the high-order 4 bits are--the result is always zero. Therefore capstone returns a
        // 4-argument version where the usual third argument (the rotate right result) is replaced by zero and a new fourth
        // argument is added, which is the shift amount (twice the imm12<11,8> field). Capstone also creates the four-argument
        // version in other cases when the result is not zero.
        ASSERT_require(isSgAsmIntegerValueExpression(retval->operand(2)));
        ASSERT_require(isSgAsmIntegerValueExpression(retval->operand(3)));
        SgAsmExpression *rotateAmount = retval->operand(2);
        SgAsmExpression *baseAmount = retval->operand(3);
        retval->get_operandList()->get_operands()[2] = nullptr; rotateAmount->set_parent(nullptr);
        retval->get_operandList()->get_operands()[3] = nullptr; baseAmount->set_parent(nullptr);

        SgAsmExpression *shifted = SageBuilderAsm::buildRorExpression(baseAmount, rotateAmount);
        retval->get_operandList()->get_operands()[2] = shifted; shifted->set_parent(retval->get_operandList());
        retval->get_operandList()->get_operands().resize(3);
    }

    commentIpRelative(retval);
    ASSERT_not_null(retval);
    return retval;
}

void
Aarch32::commentIpRelative(SgAsmInstruction *insn) {
    ASSERT_not_null(insn);

    const RegisterDescriptor REG_PC = RegisterDescriptor(aarch32_regclass_gpr, aarch32_gpr_pc, 0, 32);
    const Address fallThroughVa = insn->get_address() + insn->get_size();

    AST::Traversal::forwardPre<SgAsmBinaryAdd>(insn, [&REG_PC, &fallThroughVa](SgAsmBinaryAdd *add) {
        SgAsmDirectRegisterExpression *reg = NULL;
        SgAsmIntegerValueExpression *ival = NULL;
        if ((reg = isSgAsmDirectRegisterExpression(add->get_lhs()))) {
            ival = isSgAsmIntegerValueExpression(add->get_rhs());
        } else if ((reg = isSgAsmDirectRegisterExpression(add->get_rhs()))) {
            ival = isSgAsmIntegerValueExpression(add->get_lhs());
        }

        if (reg && ival && reg->get_descriptor() == REG_PC) {
            const Address offset = ival->get_absoluteValue();
            const Address va = (fallThroughVa + offset + 4) & BitOps::lowMask<Address>(32);
            const std::string vaStr = "absolute=" + StringUtility::addrToString(va, 32);
            std::string comment = add->get_comment();
            comment = comment.empty() ? vaStr : vaStr + "," + comment;
            add->set_comment(comment);
        }
    });
}

SgAsmInstruction*
Aarch32::makeUnknownInstruction(const Exception &e) {
    SgAsmAarch32Instruction *insn = new SgAsmAarch32Instruction(e.ip, *architecture()->registrationId(), ARM_INS_INVALID,
                                                                ARM_CC_AL);
    SgAsmOperandList *operands = new SgAsmOperandList();
    insn->set_operandList(operands);
    operands->set_parent(insn);
    insn->set_rawBytes(e.bytes);
    insn->set_condition(Aarch32InstructionCondition::ARM_CC_AL);
    return insn;
}

SgAsmExpression*
Aarch32::makeOperand(const cs_insn &insn, const cs_arm_op &op) {
    SgAsmExpression *retval = nullptr;
    SgAsmType *u32 = SageBuilderAsm::buildTypeU32();
    bool shiftedOrRotated = false;

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

        case ARM_OP_IMM: {
            uint64_t val = (uint64_t)op.imm;
            if (op.subtracted)
                val = (~val) + 1;
            retval = new SgAsmIntegerValueExpression(val, u32);
            break;
        }

        case ARM_OP_MEM: {
            RegisterDescriptor reg = makeRegister(op.mem.base);
            auto base = new SgAsmDirectRegisterExpression(reg);
            base->set_type(registerType(reg));
            SgAsmExpression *addr = base;

            if (op.mem.index != ARM_REG_INVALID) {
                RegisterDescriptor indexReg = makeRegister(op.mem.index);
                SgAsmExpression *index = new SgAsmDirectRegisterExpression(indexReg);
                index->set_type(registerType(indexReg));
                index = shiftOrRotate(index, op);
                shiftedOrRotated = true;

                if (op.mem.disp != 0) {
                    ASSERT_not_implemented("[Robb Matzke 2021-01-04]");
                } else if (op.subtracted) {
                    addr = SageBuilderAsm::buildSubtractExpression(addr, index, u32);
                } else {
                    addr = SageBuilderAsm::buildAddExpression(addr, index, u32);
                }
            } else if (op.mem.disp != 0) {
                auto disp = new SgAsmIntegerValueExpression(op.mem.disp, u32);
                addr = SageBuilderAsm::buildAddExpression(addr, disp, u32);
            }

            if (!shiftedOrRotated) {
                addr = shiftOrRotate(addr, op);
                shiftedOrRotated = true;
            }
            auto mre = new SgAsmMemoryReferenceExpression;
            mre->set_address(addr); addr->set_parent(mre);
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
            switch (op.setend) {
                case ARM_SETEND_BE:
                    retval = new SgAsmByteOrder(ByteOrder::ORDER_MSB);
                    break;
                case ARM_SETEND_LE:
                    retval = new SgAsmByteOrder(ByteOrder::ORDER_LSB);
                    break;
                case ARM_SETEND_INVALID:
                    ASSERT_not_reachable("invalid endianness = " + boost::lexical_cast<std::string>(op.setend));
            }
            retval->set_type(SageBuilderAsm::buildTypeU1()); // the type doesn't really matter here
            break;

        case ARM_OP_SYSREG: {
            retval = makeSystemRegister((arm_sysreg)op.reg);
            break;
        }
    }

    // Adjust for shift and rotate
    if (!shiftedOrRotated)
        retval = shiftOrRotate(retval, op);

    ASSERT_not_null(retval);
    ASSERT_not_null(retval->get_type());
    return retval;
}

SgAsmExpression*
Aarch32::shiftOrRotate(SgAsmExpression *baseExpr, const cs_arm_op &op) {
    ASSERT_not_null(baseExpr);
    if (op.shift.type != ARM_SFT_INVALID && op.shift.value != 0) {
        SgAsmType *u32 = SageBuilderAsm::buildTypeU32();
        switch (op.shift.type) {
            case ARM_SFT_INVALID:
                return baseExpr;

            case ARM_SFT_ASR: {                         // shift with immediate const
                SgAsmExpression *amount = new SgAsmIntegerValueExpression(op.shift.value, u32);
                return SageBuilderAsm::buildAsrExpression(baseExpr, amount, u32);
            }
            case ARM_SFT_LSL: {                         // shift with immediate const
                SgAsmExpression *amount = new SgAsmIntegerValueExpression(op.shift.value, u32);
                return SageBuilderAsm::buildLslExpression(baseExpr, amount, u32);
            }
            case ARM_SFT_LSR: {                         // shift with immediate const
                SgAsmExpression *amount = new SgAsmIntegerValueExpression(op.shift.value, u32);
                return SageBuilderAsm::buildLsrExpression(baseExpr, amount, u32);
            }
            case ARM_SFT_ROR: {                         // shift with immediate const
                SgAsmExpression *amount = new SgAsmIntegerValueExpression(op.shift.value, u32);
                return SageBuilderAsm::buildRorExpression(baseExpr, amount, u32);
            }
            case ARM_SFT_RRX: {                         // rotate right with extend
                RegisterDescriptor carryReg = architecture()->registerDictionary()->find("cpsr_c");
                SgAsmExpression *carry = new SgAsmDirectRegisterExpression(carryReg);
                carry->set_type(registerType(carryReg));
                SgAsmExpression *wide = SageBuilderAsm::buildConcatExpression(carry, baseExpr);
                SgAsmExpression *amount = new SgAsmIntegerValueExpression(op.shift.value, u32);
                return SageBuilderAsm::buildRorExpression(wide, amount, wide->get_type());
            }
            case ARM_SFT_ASR_REG: {                     // shift with register
                RegisterDescriptor reg = makeRegister(static_cast<arm_reg>(op.shift.value));
                SgAsmExpression *amount = new SgAsmDirectRegisterExpression(reg);
                amount->set_type(registerType(reg));
                return SageBuilderAsm::buildAsrExpression(baseExpr, amount, u32);
            }
            case ARM_SFT_LSL_REG: {                     // shift with register
                RegisterDescriptor reg = makeRegister(static_cast<arm_reg>(op.shift.value));
                SgAsmExpression *amount = new SgAsmDirectRegisterExpression(reg);
                amount->set_type(registerType(reg));
                return SageBuilderAsm::buildLslExpression(baseExpr, amount, u32);
            }
            case ARM_SFT_LSR_REG: {                     // shift with register
                RegisterDescriptor reg = makeRegister(static_cast<arm_reg>(op.shift.value));
                SgAsmExpression *amount = new SgAsmDirectRegisterExpression(reg);
                amount->set_type(registerType(reg));
                return SageBuilderAsm::buildLsrExpression(baseExpr, amount, u32);
            }
            case ARM_SFT_ROR_REG: {                     // shift with register
                RegisterDescriptor reg = makeRegister(static_cast<arm_reg>(op.shift.value));
                SgAsmExpression *amount = new SgAsmDirectRegisterExpression(reg);
                amount->set_type(registerType(reg));
                return SageBuilderAsm::buildRorExpression(baseExpr, amount, u32);
            }
            case ARM_SFT_RRX_REG: {                     // shift with register
                RegisterDescriptor carryReg = architecture()->registerDictionary()->find("cpsr_c");
                SgAsmExpression *carry = new SgAsmDirectRegisterExpression(carryReg);
                carry->set_type(registerType(carryReg));
                SgAsmExpression *wide = SageBuilderAsm::buildConcatExpression(carry, baseExpr);

                RegisterDescriptor reg = makeRegister(static_cast<arm_reg>(op.shift.value));
                SgAsmExpression *amount = new SgAsmDirectRegisterExpression(reg);
                amount->set_type(registerType(reg));

                return SageBuilderAsm::buildRorExpression(wide, amount, wide->get_type());
            }
            default:
                ASSERT_not_reachable("invalid shift type: " + boost::lexical_cast<std::string>(op.shift.type));
        }
    } else {
        return baseExpr;
    }
}

SgAsmType*
Aarch32::registerType(RegisterDescriptor reg) {
    return SageBuilderAsm::buildTypeU(reg.nBits());
}

RegisterDescriptor
Aarch32::makeCoprocRegister(int registerNumber) {
    return RegisterDescriptor(aarch32_regclass_coproc, registerNumber, 0, 32);
}

RegisterDescriptor
Aarch32::makeRegister(arm_reg reg) {
    RegisterDictionary::Ptr dict = architecture()->registerDictionary();
    RegisterDescriptor retval;

    switch (reg) {
        case ARM_REG_INVALID:
        case ARM_REG_ENDING:
            ASSERT_not_reachable("invalid register from Capstone");
        case ARM_REG_APSR:
        case ARM_REG_CPSR:                              // alias for APSR
            retval = dict->find("apsr");
            break;
        case ARM_REG_APSR_NZCV:
            retval = dict->find("apsr_nzcv");
            break;
        case ARM_REG_LR:
            retval = dict->find("lr");
            break;
        case ARM_REG_PC:
            retval = dict->find("pc");
            break;
        case ARM_REG_SP:
            retval = dict->find("sp");
            break;
        case ARM_REG_R0:
            retval = dict->find("r0");
            break;
        case ARM_REG_R1:
            retval = dict->find("r1");
            break;
        case ARM_REG_R2:
            retval = dict->find("r2");
            break;
        case ARM_REG_R3:
            retval = dict->find("r3");
            break;
        case ARM_REG_R4:
            retval = dict->find("r4");
            break;
        case ARM_REG_R5:
            retval = dict->find("r5");
            break;
        case ARM_REG_R6:
            retval = dict->find("r6");
            break;
        case ARM_REG_R7:
            retval = dict->find("r7");
            break;
        case ARM_REG_R8:
            retval = dict->find("r8");
            break;
        case ARM_REG_R9:
            retval = dict->find("r9");
            break;
        case ARM_REG_R10:
            retval = dict->find("r10");
            break;
        case ARM_REG_R11:
            retval = dict->find("r11");
            break;
        case ARM_REG_R12:
            retval = dict->find("r12");
            break;
        case ARM_REG_Q0:
            retval = dict->find("q0");
            break;
        case ARM_REG_Q1:
            retval = dict->find("q1");
            break;
        case ARM_REG_Q2:
            retval = dict->find("q2");
            break;
        case ARM_REG_Q3:
            retval = dict->find("q3");
            break;
        case ARM_REG_Q4:
            retval = dict->find("q4");
            break;
        case ARM_REG_Q5:
            retval = dict->find("q5");
            break;
        case ARM_REG_Q6:
            retval = dict->find("q6");
            break;
        case ARM_REG_Q7:
            retval = dict->find("q7");
            break;
        case ARM_REG_Q8:
            retval = dict->find("q8");
            break;
        case ARM_REG_Q9:
            retval = dict->find("q9");
            break;
        case ARM_REG_Q10:
            retval = dict->find("q10");
            break;
        case ARM_REG_Q11:
            retval = dict->find("q11");
            break;
        case ARM_REG_Q12:
            retval = dict->find("q12");
            break;
        case ARM_REG_Q13:
            retval = dict->find("q13");
            break;
        case ARM_REG_Q14:
            retval = dict->find("q14");
            break;
        case ARM_REG_Q15:
            retval = dict->find("q15");
            break;
        case ARM_REG_D0:
            retval = dict->find("d0");
            break;
        case ARM_REG_D1:
            retval = dict->find("d1");
            break;
        case ARM_REG_D2:
            retval = dict->find("d2");
            break;
        case ARM_REG_D3:
            retval = dict->find("d3");
            break;
        case ARM_REG_D4:
            retval = dict->find("d4");
            break;
        case ARM_REG_D5:
            retval = dict->find("d5");
            break;
        case ARM_REG_D6:
            retval = dict->find("d6");
            break;
        case ARM_REG_D7:
            retval = dict->find("d7");
            break;
        case ARM_REG_D8:
            retval = dict->find("d8");
            break;
        case ARM_REG_D9:
            retval = dict->find("d9");
            break;
        case ARM_REG_D10:
            retval = dict->find("d10");
            break;
        case ARM_REG_D11:
            retval = dict->find("d11");
            break;
        case ARM_REG_D12:
            retval = dict->find("d12");
            break;
        case ARM_REG_D13:
            retval = dict->find("d13");
            break;
        case ARM_REG_D14:
            retval = dict->find("d14");
            break;
        case ARM_REG_D15:
            retval = dict->find("d15");
            break;
        case ARM_REG_D16:
            retval = dict->find("d16");
            break;
        case ARM_REG_D17:
            retval = dict->find("d17");
            break;
        case ARM_REG_D18:
            retval = dict->find("d18");
            break;
        case ARM_REG_D19:
            retval = dict->find("d19");
            break;
        case ARM_REG_D20:
            retval = dict->find("d20");
            break;
        case ARM_REG_D21:
            retval = dict->find("d21");
            break;
        case ARM_REG_D22:
            retval = dict->find("d22");
            break;
        case ARM_REG_D23:
            retval = dict->find("d23");
            break;
        case ARM_REG_D24:
            retval = dict->find("d24");
            break;
        case ARM_REG_D25:
            retval = dict->find("d25");
            break;
        case ARM_REG_D26:
            retval = dict->find("d26");
            break;
        case ARM_REG_D27:
            retval = dict->find("d27");
            break;
        case ARM_REG_D28:
            retval = dict->find("d28");
            break;
        case ARM_REG_D29:
            retval = dict->find("d29");
            break;
        case ARM_REG_D30:
            retval = dict->find("d30");
            break;
        case ARM_REG_D31:
            retval = dict->find("d31");
            break;
        case ARM_REG_S0:
            retval = dict->find("s0");
            break;
        case ARM_REG_S1:
            retval = dict->find("s1");
            break;
        case ARM_REG_S2:
            retval = dict->find("s2");
            break;
        case ARM_REG_S3:
            retval = dict->find("s3");
            break;
        case ARM_REG_S4:
            retval = dict->find("s4");
            break;
        case ARM_REG_S5:
            retval = dict->find("s5");
            break;
        case ARM_REG_S6:
            retval = dict->find("s6");
            break;
        case ARM_REG_S7:
            retval = dict->find("s7");
            break;
        case ARM_REG_S8:
            retval = dict->find("s8");
            break;
        case ARM_REG_S9:
            retval = dict->find("s9");
            break;
        case ARM_REG_S10:
            retval = dict->find("s10");
            break;
        case ARM_REG_S11:
            retval = dict->find("s11");
            break;
        case ARM_REG_S12:
            retval = dict->find("s12");
            break;
        case ARM_REG_S13:
            retval = dict->find("s13");
            break;
        case ARM_REG_S14:
            retval = dict->find("s14");
            break;
        case ARM_REG_S15:
            retval = dict->find("s15");
            break;
        case ARM_REG_S16:
            retval = dict->find("s16");
            break;
        case ARM_REG_S17:
            retval = dict->find("s17");
            break;
        case ARM_REG_S18:
            retval = dict->find("s18");
            break;
        case ARM_REG_S19:
            retval = dict->find("s19");
            break;
        case ARM_REG_S20:
            retval = dict->find("s20");
            break;
        case ARM_REG_S21:
            retval = dict->find("s21");
            break;
        case ARM_REG_S22:
            retval = dict->find("s22");
            break;
        case ARM_REG_S23:
            retval = dict->find("s23");
            break;
        case ARM_REG_S24:
            retval = dict->find("s24");
            break;
        case ARM_REG_S25:
            retval = dict->find("s25");
            break;
        case ARM_REG_S26:
            retval = dict->find("s26");
            break;
        case ARM_REG_S27:
            retval = dict->find("s27");
            break;
        case ARM_REG_S28:
            retval = dict->find("s28");
            break;
        case ARM_REG_S29:
            retval = dict->find("s29");
            break;
        case ARM_REG_S30:
            retval = dict->find("s30");
            break;
        case ARM_REG_S31:
            retval = dict->find("s31");
            break;
        case ARM_REG_SPSR:
            // When we're decoding an instruction to produce ROSE's AST we don't know what interrupt handling state the
            // processor will be in when the instruction is eventually executed. Since the instruction has a single encoding
            // for all the banked SPSR registers, we use a placeholder SPSR register that will be resolved to the correct
            // hardware SPSR_* register when the instruction is executed.
            retval = dict->find("spsr");
            break;
        case ARM_REG_FPSID:
            retval = dict->find("fpsid");
            break;
        case ARM_REG_FPSCR:
            retval = dict->find("fpscr");
            break;
        case ARM_REG_FPSCR_NZCV:
            retval = dict->find("fpscr_nzcv");
            break;
        case ARM_REG_FPEXC:
            retval = dict->find("fpexc");
            break;
        case ARM_REG_FPINST:
            retval = dict->find("fpinst");
            break;
        case ARM_REG_FPINST2:
            retval = dict->find("fpinst2");
            break;
        case ARM_REG_MVFR0:
            retval = dict->find("mvfr0");
            break;
        case ARM_REG_MVFR1:
            retval = dict->find("mvfr1");
            break;
        case ARM_REG_MVFR2:
            retval = dict->find("mvfr2");
            break;
        case ARM_REG_ITSTATE:
            retval = dict->find("itstate");
            break;
    }

    ASSERT_require2(retval, "reg=" + boost::lexical_cast<std::string>(reg));
    return retval;
}

SgAsmExpression*
Aarch32::makeSystemRegister(arm_sysreg capreg) {
    RegisterDictionary::Ptr dict = architecture()->registerDictionary();
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
            reg = dict->find("spsr_control");
            auto regExpr = new SgAsmDirectRegisterExpression(reg);
            regExpr->set_type(registerType(reg));
            regList->get_registers().push_back(regExpr);
            regExpr->set_parent(regList);
        }
        if ((capreg & ARM_SYSREG_SPSR_X) != 0) {
            reg = dict->find("spsr_extension");
            auto regExpr = new SgAsmDirectRegisterExpression(reg);
            regExpr->set_type(registerType(reg));
            regList->get_registers().push_back(regExpr);
            regExpr->set_parent(regList);
        }
        if ((capreg & ARM_SYSREG_SPSR_S) != 0) {
            reg = dict->find("spsr_status");
            auto regExpr = new SgAsmDirectRegisterExpression(reg);
            regExpr->set_type(registerType(reg));
            regList->get_registers().push_back(regExpr);
            regExpr->set_parent(regList);
        }
        if ((capreg & ARM_SYSREG_SPSR_F) != 0) {
            reg = dict->find("spsr_flags");
            auto regExpr = new SgAsmDirectRegisterExpression(reg);
            regExpr->set_type(registerType(reg));
            regList->get_registers().push_back(regExpr);
            regExpr->set_parent(regList);
        }
        retval = regList;
    } else if ((capreg & (ARM_SYSREG_CPSR_C | ARM_SYSREG_CPSR_X | ARM_SYSREG_CPSR_S | ARM_SYSREG_CPSR_F)) != 0 &&
               BitOps::nSet((unsigned)capreg) > 1) {
        auto regList = new SgAsmRegisterNames;
        if ((capreg & ARM_SYSREG_CPSR_C) != 0) {
            reg = dict->find("cpsr_control");
            auto regExpr = new SgAsmDirectRegisterExpression(reg);
            regExpr->set_type(registerType(reg));
            regList->get_registers().push_back(regExpr);
            regExpr->set_parent(regList);
        }
        if ((capreg & ARM_SYSREG_CPSR_X) != 0) {
            reg = dict->find("cpsr_extension");
            auto regExpr = new SgAsmDirectRegisterExpression(reg);
            regExpr->set_type(registerType(reg));
            regList->get_registers().push_back(regExpr);
            regExpr->set_parent(regList);
        }
        if ((capreg & ARM_SYSREG_CPSR_S) != 0) {
            reg = dict->find("cpsr_status");
            auto regExpr = new SgAsmDirectRegisterExpression(reg);
            regExpr->set_type(registerType(reg));
            regList->get_registers().push_back(regExpr);
            regExpr->set_parent(regList);
        }
        if ((capreg & ARM_SYSREG_CPSR_F) != 0) {
            reg = dict->find("cpsr_flags");
            auto regExpr = new SgAsmDirectRegisterExpression(reg);
            regExpr->set_type(registerType(reg));
            regList->get_registers().push_back(regExpr);
            regExpr->set_parent(regList);
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
                reg = dict->find("spsr_control");
                break;
            case ARM_SYSREG_SPSR_X:
                reg = dict->find("spsr_extension");
                break;
            case ARM_SYSREG_SPSR_S:
                reg = dict->find("spsr_status");
                break;
            case ARM_SYSREG_SPSR_F:
                reg = dict->find("spsr_flags");
                break;

            case ARM_SYSREG_CPSR_C:
                reg = dict->find("cpsr_control");
                break;
            case ARM_SYSREG_CPSR_X:
                reg = dict->find("cpsr_extension");
                break;
            case ARM_SYSREG_CPSR_S:
                reg = dict->find("cpsr_status");
                break;
            case ARM_SYSREG_CPSR_F:
                reg = dict->find("cpsr_flags");
                break;
            case ARM_SYSREG_APSR:
                reg = dict->find("apsr");
                break;
            case ARM_SYSREG_APSR_NZCVQ:
                reg = dict->find("apsr_nzcvq");
                break;
            case ARM_SYSREG_APSR_NZCVQG: {                  // undocumented in capstone, but perhaps N, Z, C, V, Q, and GE
                // These are discontiguous parts of the register, so we need to create a list of register parts.
                RegisterDescriptor reg = dict->find("apsr_nzcvq");
                auto nzcvq = new SgAsmDirectRegisterExpression(reg);
                nzcvq->set_type(registerType(reg));

                reg = dict->find("apsr_ge");
                auto ge = new SgAsmDirectRegisterExpression(reg);
                ge->set_type(registerType(reg));

                auto regList = new SgAsmRegisterNames;
                regList->get_registers().push_back(nzcvq);
                nzcvq->set_parent(regList);
                regList->get_registers().push_back(ge);
                ge->set_parent(regList);
                retval = regList;
                break;
            }

            case ARM_SYSREG_APSR_G:
                ASSERT_not_implemented("apsr_g");           // undocumented

            // IAPSR (not sure what this is)
            case ARM_SYSREG_IAPSR:
                reg = dict->find("iapsr");
                break;
            case ARM_SYSREG_IAPSR_G:
                ASSERT_not_implemented("iapsr_g");          // undocumented
            case ARM_SYSREG_IAPSR_NZCVQG:
                ASSERT_not_implemented("iapsr_nzcvqg");     // undocumented
            case ARM_SYSREG_IAPSR_NZCVQ:
                ASSERT_not_implemented("iapsr*");           // undocumented

            // EAPSR (not sure what this is)
            case ARM_SYSREG_EAPSR:
                reg = dict->find("eapsr");
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
                reg = dict->find("ipsr");
                break;
            case ARM_SYSREG_EPSR:
                reg = dict->find("epsr");
                break;
            case ARM_SYSREG_IEPSR:
                reg = dict->find("iepsr");
                break;
            case ARM_SYSREG_MSP:
                reg = dict->find("msp");
                break;
            case ARM_SYSREG_PSP:
                reg = dict->find("psp");
                break;
            case ARM_SYSREG_PRIMASK:
                reg = dict->find("primask");
                break;
            case ARM_SYSREG_BASEPRI:
                ASSERT_not_implemented("basepri");          // undocumented
            case ARM_SYSREG_BASEPRI_MAX:
                ASSERT_not_implemented("basepri_max");      // undocumented
            case ARM_SYSREG_FAULTMASK:
                ASSERT_not_implemented("faultmask");        // undocumented
            case ARM_SYSREG_CONTROL:
                reg = dict->find("control");
                break;

            // Banked registers
            case ARM_SYSREG_R8_USR:
                reg = dict->find("r8_usr");
                break;
            case ARM_SYSREG_R9_USR:
                reg = dict->find("r9_usr");
                break;
            case ARM_SYSREG_R10_USR:
                reg = dict->find("r10_usr");
                break;
            case ARM_SYSREG_R11_USR:
                reg = dict->find("r11_usr");
                break;
            case ARM_SYSREG_R12_USR:
                reg = dict->find("r12_usr");
                break;
            case ARM_SYSREG_SP_USR:
                reg = dict->find("sp_usr");
                break;
            case ARM_SYSREG_LR_USR:
                reg = dict->find("lr_usr");
                break;
            case ARM_SYSREG_R8_FIQ:
                reg = dict->find("r8_fiq");
                break;
            case ARM_SYSREG_R9_FIQ:
                reg = dict->find("r9_fiz");
                break;
            case ARM_SYSREG_R10_FIQ:
                reg = dict->find("r10_fiq");
                break;
            case ARM_SYSREG_R11_FIQ:
                reg = dict->find("r11_fiq");
                break;
            case ARM_SYSREG_R12_FIQ:
                reg = dict->find("r12_fiq");
                break;
            case ARM_SYSREG_SP_FIQ:
                reg = dict->find("sp_fiq");
                break;
            case ARM_SYSREG_LR_FIQ:
                reg = dict->find("lr_fiq");
                break;
            case ARM_SYSREG_LR_IRQ:
                reg = dict->find("lr_irq");
                break;
            case ARM_SYSREG_SP_IRQ:
                reg = dict->find("sp_irq");
                break;
            case ARM_SYSREG_LR_SVC:
                reg = dict->find("lr_svc");
                break;
            case ARM_SYSREG_SP_SVC:
                reg = dict->find("sp_svc");
                break;
            case ARM_SYSREG_LR_ABT:
                reg = dict->find("lr_abt");
                break;
            case ARM_SYSREG_SP_ABT:
                reg = dict->find("sp_abt");
                break;
            case ARM_SYSREG_LR_UND:
                reg = dict->find("lr_und");
                break;
            case ARM_SYSREG_SP_UND:
                reg = dict->find("sp_und");
                break;
            case ARM_SYSREG_LR_MON:
                reg = dict->find("lr_mon");
                break;
            case ARM_SYSREG_SP_MON:
                reg = dict->find("sp_mon");
                break;
            case ARM_SYSREG_ELR_HYP:
                reg = dict->find("sp_hyp");
                break;
            case ARM_SYSREG_SP_HYP:
                reg = dict->find("sp_hyp");
                break;
            case ARM_SYSREG_SPSR_FIQ:
                reg = dict->find("spsr_fiq");
                break;
            case ARM_SYSREG_SPSR_IRQ:
                reg = dict->find("spsr_irq");
                break;
            case ARM_SYSREG_SPSR_SVC:
                reg = dict->find("spsr_svc");
                break;
            case ARM_SYSREG_SPSR_ABT:
                reg = dict->find("spsr_abt");
                break;
            case ARM_SYSREG_SPSR_UND:
                reg = dict->find("spsr_und");
                break;
            case ARM_SYSREG_SPSR_MON:
                reg = dict->find("spsr_mon");
                break;
            case ARM_SYSREG_SPSR_HYP:
                reg = dict->find("spsr_hyp");
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
Aarch32::typeForMemoryRead(const cs_insn &insn) {
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
Aarch32::opcode(const cs_insn &insn) {
    ASSERT_require(insn.size <= 4);
    uint32_t retval = 0;
    for (size_t i = 0; i < insn.size; ++i)
        retval |= uint32_t(insn.bytes[i]) << (8 * i);
    return retval;
}

RegisterDescriptor
Aarch32::subRegister(RegisterDescriptor reg, int idx) {
    // This isn't really complete yet, just stubbed out to return something that doesn't crash ROSE.
    if (idx >= 0)
        mlog[WARN] <<"register indexing not supported yet for A32/T32\n";
    return reg;
}

void
Aarch32::wrapPrePostIncrement(SgAsmAarch32Instruction *insn, const cs_insn &csInsn, const uint32_t word) {
    ASSERT_not_null(insn);
    SgAsmOperandList *operands = insn->get_operandList();
    ASSERT_not_null(operands);
    size_t nOperands = operands->get_operands().size();
    ASSERT_not_null(csInsn.detail);
    const cs_arm &arm = csInsn.detail->arm;

    if ((arm_insn)csInsn.id == ARM_INS_LDRBT && nOperands == 3) {
        // Apparent capstone bug: arm.writeback is false but should be true.
    } else if ((arm_insn)csInsn.id == ARM_INS_LDRSBT && nOperands == 3) {
        // Apparent capstone bug: arm.writeback is false but should be true.
    } else if ((arm_insn)csInsn.id == ARM_INS_LDRT && nOperands == 3) {
        // Apparent capstone bug: arm.writeback is false but should be true.
    } else if ((arm_insn)csInsn.id == ARM_INS_LDRHT && nOperands == 3) {
        // Apparent capstone bug: arm.writeback is false but should be true.
    } else if ((arm_insn)csInsn.id == ARM_INS_LDRSHT && nOperands == 3) {
        // Apparent capstone bug: arm.writeback is false but should be true.
    } else if ((arm_insn)csInsn.id == ARM_INS_STRT && nOperands == 3) {
        // Apparent capstone bug: arm.writeback is false but should be true.
    } else if ((arm_insn)csInsn.id == ARM_INS_STRBT && nOperands == 3) {
        // Apparent capstone bug: arm.writeback is false but should be true.
    } else if ((arm_insn)csInsn.id == ARM_INS_STRHT && nOperands == 3) {
        // Apparent capstone bug: arm.writeback is false but should be true.
    } else if (!arm.writeback) {
        return;
    }

    const arm_insn id = (arm_insn)csInsn.id;
    switch (id) {
        case ARM_INS_FLDMDBX:
        case ARM_INS_FLDMIAX:
        case ARM_INS_FSTMDBX:
        case ARM_INS_FSTMIAX:
        case ARM_INS_LDM:
        case ARM_INS_LDMDA:
        case ARM_INS_LDMDB:
        case ARM_INS_LDMIB:
        case ARM_INS_STM:
        case ARM_INS_STMDA:
        case ARM_INS_STMDB:
        case ARM_INS_STMIB:
        case ARM_INS_VLDMDB:
        case ARM_INS_VLDMIA:
        case ARM_INS_VSTMDB:
        case ARM_INS_VSTMIA: {
            // First argument is the register that will be updated. The remaining variable number of arguments are the
            // registers read/written to memory. The first register is decremented according to how many other registers are
            // specified.
            ASSERT_require2(arm.op_count > 1, "insn = " + StringUtility::addrToString(word));

            // Does the first argument register also appear again in the rest of the arguments?
            size_t nBytesTransferred = 0;
            bool readWriteSame = false;
            const RegisterDescriptor firstReg = isSgAsmDirectRegisterExpression(operands->get_operands()[0])->get_descriptor();
            for (size_t i = 1; i < operands->get_operands().size(); ++i) {
                const RegisterDescriptor otherReg = isSgAsmDirectRegisterExpression(operands->get_operands()[i])->get_descriptor();
                nBytesTransferred += (otherReg.nBits() + 7) / 8;
                if (firstReg == otherReg)
                    readWriteSame = true;
            }

            // Unlink the register (eventual lhs of the postupdate) from the AST.
            ASSERT_require2(nOperands == arm.op_count, "insn = " + StringUtility::addrToString(word));
            auto lhs = isSgAsmDirectRegisterExpression(operands->get_operands()[0]); // the register
            ASSERT_not_null2(lhs, "insn = " + StringUtility::addrToString(word));
            operands->get_operands()[0] = nullptr;  // illegal, but just temporary
            lhs->set_parent(nullptr);

            // Calculate the expression to be assigned to the first argument register.
            SgAsmExpression *newValue = nullptr;
            if (readWriteSame &&
                (id == ARM_INS_FLDMDBX ||
                 id == ARM_INS_FLDMIAX ||
                 id == ARM_INS_LDM ||
                 id == ARM_INS_LDMDA ||
                 id == ARM_INS_LDMDB ||
                 id == ARM_INS_LDMIB ||
                 id == ARM_INS_VLDMDB ||
                 id == ARM_INS_VLDMIA)) {
                // If the same register is being loaded from memory and updated, then its final value is unknown.
                const RegisterDescriptor unknown = architecture()->registerDictionary()->findOrThrow("unknown");
                newValue = new SgAsmDirectRegisterExpression(unknown);
                newValue->set_type(SageBuilderAsm::buildTypeU32());
            } else if (id == ARM_INS_FLDMDBX ||
                       id == ARM_INS_FSTMDBX ||
                       id == ARM_INS_LDMDA ||
                       id == ARM_INS_LDMDB ||
                       id == ARM_INS_STMDA ||
                       id == ARM_INS_STMDB ||
                       id == ARM_INS_VLDMDB ||
                       id == ARM_INS_VSTMDB) {
                // The new value will be the register's old value minus the number of bytes transferred to/from memory.
                auto oldValue = new SgAsmDirectRegisterExpression(lhs->get_descriptor());
                oldValue->set_type(lhs->get_type());
                auto inc = new  SgAsmIntegerValueExpression(-nBytesTransferred, oldValue->get_type());
                newValue = SageBuilderAsm::buildAddExpression(oldValue, inc);
            } else if (id == ARM_INS_FLDMIAX ||
                       id == ARM_INS_FSTMIAX ||
                       id == ARM_INS_LDM ||
                       id == ARM_INS_LDMIB ||
                       id == ARM_INS_STM ||
                       id == ARM_INS_STMIB ||
                       id == ARM_INS_VLDMIA ||
                       id == ARM_INS_VSTMIA) {
                // The new value will be the register's old value plus the number of bytes transferred to/from memory.
                auto oldValue = new SgAsmDirectRegisterExpression(lhs->get_descriptor());
                oldValue->set_type(lhs->get_type());
                auto inc = new SgAsmIntegerValueExpression(nBytesTransferred, oldValue->get_type());
                newValue = SageBuilderAsm::buildAddExpression(oldValue, inc);
            } else {
                ASSERT_not_reachable("instruction not handled: id = " + boost::lexical_cast<std::string>(id) +
                                     " insn = " + StringUtility::addrToString(word));
            }

            // Create the post-update expression and link it into the AST. The update is always a post-update regardless of
            // whether the mnemonic says "before" or "after".
            SgAsmExpression *postUpdate = SageBuilderAsm::buildPostupdateExpression(lhs, newValue);
            operands->get_operands()[0] = postUpdate;
            postUpdate->set_parent(operands);

            return;
        }

        case ARM_INS_RFEIA:
        case ARM_INS_RFEIB: {
            // Last argument (a register) is incremented by 8.
            ASSERT_require2(operands->get_operands().size() >= 1, "insn = " + StringUtility::addrToString(word));
            ASSERT_require2(isSgAsmDirectRegisterExpression(operands->get_operands().back()),
                            "insn = " + StringUtility::addrToString(word));
            auto last = isSgAsmDirectRegisterExpression(operands->get_operands().back());

            // Unlink from AST
            operands->get_operands().back() = nullptr; last->set_parent(nullptr); // invalid, but temporary

            // Create the new expression
            auto sumLhs = new SgAsmDirectRegisterExpression(last->get_descriptor());
            sumLhs->set_type(last->get_type());
            SgAsmIntegerValueExpression *sumRhs = SageBuilderAsm::buildValueU32(8);
            SgAsmBinaryAdd *sum = SageBuilderAsm::buildAddExpression(sumLhs, sumRhs);
            SgAsmExpression *postUpdate = SageBuilderAsm::buildPostupdateExpression(last, sum);

            // Link new expression into AST
            operands->get_operands().back() = postUpdate; postUpdate->set_parent(operands);
            return;
        }

        case ARM_INS_RFEDA:
        case ARM_INS_RFEDB: {
            // Last argument (a register) is decremented by 8.
            ASSERT_require2(operands->get_operands().size() >= 1, "insn = " + StringUtility::addrToString(word));
            ASSERT_require2(isSgAsmDirectRegisterExpression(operands->get_operands().back()),
                            "insn = " + StringUtility::addrToString(word));
            auto last = isSgAsmDirectRegisterExpression(operands->get_operands().back());

            // Unlink from AST
            operands->get_operands().back() = nullptr; last->set_parent(nullptr); // invalid, but temporary

            // Create the new expression
            auto sumLhs = new SgAsmDirectRegisterExpression(last->get_descriptor());
            sumLhs->set_type(last->get_type());
            SgAsmIntegerValueExpression *sumRhs = SageBuilderAsm::buildValueU32(-8);
            SgAsmBinaryAdd *sum = SageBuilderAsm::buildAddExpression(sumLhs, sumRhs);
            SgAsmExpression *postUpdate = SageBuilderAsm::buildPostupdateExpression(last, sum);

            // Link new expression into AST
            operands->get_operands().back() = postUpdate; postUpdate->set_parent(operands);
            return;
        }

        case ARM_INS_POP:
            // The update for these commands is handled in the main body of the instruction's semantics instead
            // of a pre- or post-update operation.
            return;

        default:
            // If the penultimate argument is a memory access whose address is a register and the final argument is not a
            // memory access, then this is a post-increment operation. The memory address and last argument are summed in a
            // post-update expression to create a new address expression, and the last argument is discarded.
            if (nOperands >= 2) {
                auto mre = isSgAsmMemoryReferenceExpression(operands->get_operands()[nOperands-2]);
                auto addr = mre ? isSgAsmDirectRegisterExpression(mre->get_address()) : nullptr;
                auto last = operands->get_operands()[nOperands-1];
                if (addr && !isSgAsmMemoryReferenceExpression(last)) {
                    // Unlink things from the AST
                    mre->set_address(nullptr); addr->set_parent(nullptr);
                    operands->get_operands().resize(nOperands - 1); last->set_parent(nullptr);

                    // Create the rhs of the post-update, i.e., the memory address plus the former last argument
                    auto oldValue = new SgAsmDirectRegisterExpression(addr->get_descriptor());
                    oldValue->set_type(addr->get_type());
                    SgAsmBinaryAdd *newValue = SageBuilderAsm::buildAddExpression(oldValue, last);

                    // Create the post-update expression and link it into the AST.
                    auto newAddrExpr = SageBuilderAsm::buildPostupdateExpression(addr, newValue);
                    mre->set_address(newAddrExpr);
                    newAddrExpr->set_parent(mre);
                    return;
                }
            }

            // We didn't find the previous pattern. Therefore, look for an argument which is a memory reference whose address
            // is a register plus (or minus) some expression. This is a pre-update situation, so replace the addition (or
            // subtraction) with a pre-update operation.  In other words, if the argument looks like this:
            //
            //     ..... [ REG + INC ] .....
            //
            // then replace it with this:
            //
            //     ..... [ REG after REG = REG + INC ] .....
            for (size_t i = 0; i < nOperands; ++i) {
                auto mre = isSgAsmMemoryReferenceExpression(operands->get_operands()[i]);
                SgAsmBinaryExpression *addr = mre ?
                                              (isSgAsmBinaryAdd(mre->get_address()) ?
                                               isSgAsmBinaryExpression(isSgAsmBinaryAdd(mre->get_address())) :
                                               isSgAsmBinaryExpression(isSgAsmBinarySubtract(mre->get_address()))) :
                                              nullptr;
                SgAsmDirectRegisterExpression *addrLhs = addr ? isSgAsmDirectRegisterExpression(addr->get_lhs()) : nullptr;

                if (addr && addrLhs) {
                    // unlink things from the AST
                    mre->set_address(nullptr);  addr->set_parent(nullptr);

                    // create a second REG node to act as the lhs of the pre-update
                    auto lhs = new SgAsmDirectRegisterExpression(addrLhs->get_descriptor());
                    lhs->set_type(addrLhs->get_type());

                    // Create the pre-update and link it into the AST as the memory reference address.
                    auto preUpdate = SageBuilderAsm::buildPreupdateExpression(lhs, addr);
                    mre->set_address(preUpdate); preUpdate->set_parent(mre);
                    return;
                }
            }

            // If the memory reference expression has an address that's simply a register, then we do nothing. This is because
            // an instruction such as "stc2l p0, c15, [r8, #-0]!" (0xfd68f000) has been simplified already to remove the "-0"
            // term.
            for (size_t i = 0; i < nOperands; ++i) {
                auto mre = isSgAsmMemoryReferenceExpression(operands->get_operands()[i]);
                auto reg = mre ? isSgAsmDirectRegisterExpression(mre->get_address()) : nullptr;
                if (reg)
                    return;
            }

            break;
    }
    ASSERT_not_reachable("no pre/post update replacement performed for insn = " + StringUtility::addrToString(word));
}

} // namespace
} // namespace
} // namespace

#endif
