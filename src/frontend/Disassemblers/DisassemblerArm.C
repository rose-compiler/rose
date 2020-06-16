#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_A64

#include <sage3basic.h>
#include <BitOps.h>
#include <DisassemblerArm.h>
#include <BinaryUnparserArm.h>

using namespace Rose::Diagnostics;

namespace Rose {
namespace BinaryAnalysis {

Disassembler*
DisassemblerArm::clone() const {
    return new DisassemblerArm(*this);
}

bool
DisassemblerArm::canDisassemble(SgAsmGenericHeader *header) const {
    SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();
    return (isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_ARM_Family;
}

void
DisassemblerArm::init() {
    // Warning: the "mode" constants are not orthogonal with each other or the "arch" values.
    cs_arch arch = (cs_arch)arch_;
    cs_mode mode = (cs_mode)modes_.vector();

    // ROSE disassembler properties, and choose a somewhat descriptive name (at least something better than "ARM").
    std::string name;
    switch (arch_) {
        case Architecture::ARCH_ARM:
#if 0
            name = "a32";
            wordSizeBytes(4);
            byteOrder(ByteOrder::ORDER_LSB);
            registerDictionary(RegisterDictionary::dictionary_arm7()); // only a default
            callingConventions(CallingConvention::dictionaryArm());
#else
            ASSERT_not_reachable("AArch32 is not implemented yet");
#endif
            break;
        case Architecture::ARCH_ARM64:
            name = "a64";
            wordSizeBytes(8);
            byteOrder(ByteOrder::ORDER_LSB);
            registerDictionary(RegisterDictionary::dictionary_aarch64());
            callingConventions(CallingConvention::dictionaryArm64());
            break;
    }
    if (name.empty())
        throw Exception("invalid ARM architecture");
    if (modes_.isSet(Mode::MODE_THUMB))
        name += "_thumb"; // according to capston: "Thumb, Thumb-2", whatever that comma means, but apparently not ThumbEE/Jazelle
    if (modes_.isSet(Mode::MODE_MCLASS))
        name += "_microprocessor"; // apparently the "microprocessor profile for Cortex processors"
    if (modes_.isSet(Mode::MODE_V8))
        name += "_a32"; // capstone: "ARMv8 A32 encodings for ARM"
    this->name(name);

    // Architecture independent ROSE disassembler properties
    REG_IP = registerDictionary()->findOrThrow("pc");
    REG_SP = registerDictionary()->findOrThrow("sp");

    // Build the Capstone context object, which must be explicitly closed in the destructor.
    if (CS_ERR_OK != cs_open(arch, mode, &capstone_))
        throw Exception("capstone cs_open failed");
    capstoneOpened_ = true;
    if (CS_ERR_OK != cs_option(capstone_, CS_OPT_DETAIL, CS_OPT_ON))
        throw Exception("capstone cs_option failed");
}

DisassemblerArm::~DisassemblerArm() {
    if (capstoneOpened_) {
        cs_err err = cs_close(&capstone_);
        ASSERT_always_require2(CS_ERR_OK == err, "capstone cs_close failed");
    }
}

Unparser::BasePtr
DisassemblerArm::unparser() const {
    return Unparser::Arm::instance();
}

SgAsmInstruction*
DisassemblerArm::disassembleOne(const MemoryMap::Ptr &map, rose_addr_t va, AddressSet *successors/*=NULL*/) {
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
    if (va & 0x3)
        throw Exception("instruction pointer not word aligned", va);
    if (ARCH_ARM == arch_ && va > 0xfffffffc)
        throw Exception("instruction pointer out of range", va);
    uint8_t bytes[4];                                   // largest possible instruction is 4 bytes
    size_t nRead = map->at(va).limit(sizeof bytes).require(MemoryMap::EXECUTABLE).read(bytes).size();
    if (0 == nRead)
        throw Exception("short read", va);

    // Disassemble the instruction with capstone
    r.nInsns = cs_disasm(capstone_, bytes, nRead, va, 1, &r.csi);
    if (0 == r.nInsns) {
#if 1 // DEBGUGGING: show the disassembly string from capstone itself
        std::cerr <<"ROBB: capstone disassembly:"
                  <<" " <<StringUtility::addrToString(va) <<":"
                  <<" " <<StringUtility::toHex2(bytes[0], 8, false, false).substr(2)
                  <<" " <<StringUtility::toHex2(bytes[1], 8, false, false).substr(2)
                  <<" " <<StringUtility::toHex2(bytes[2], 8, false, false).substr(2)
                  <<" " <<StringUtility::toHex2(bytes[3], 8, false, false).substr(2)
                  <<" unknown\n";
#endif
        return makeUnknownInstruction(Exception("unable to decode instruction", va, SgUnsignedCharList(bytes+0, bytes+nRead), 0));
    }

    ASSERT_require(1 == r.nInsns);
    ASSERT_not_null(r.csi);
    ASSERT_require(r.csi->address == va);
    ASSERT_not_null(r.csi->detail);

    // Convert disassembled capstone instruction to ROSE AST
    SgAsmInstruction *retval = nullptr;
    if (Architecture::ARCH_ARM == arch_) {
        //const cs_arm &detail = r.csi->detail->arm;
        ASSERT_not_implemented("ARM (32-bit) disassembler is not implemented");
    } else if (Architecture::ARCH_ARM64 == arch_) {
        const cs_arm64 &detail = r.csi->detail->arm64;
#if 1 // DEBGUGGING: show the disassembly string from capstone itself
        std::cerr <<"ROBB: capstone disassembly:"
                  <<" " <<StringUtility::addrToString(va) <<":"
                  <<" " <<StringUtility::toHex2(bytes[0], 8, false, false).substr(2)
                  <<" " <<StringUtility::toHex2(bytes[1], 8, false, false).substr(2)
                  <<" " <<StringUtility::toHex2(bytes[2], 8, false, false).substr(2)
                  <<" " <<StringUtility::toHex2(bytes[3], 8, false, false).substr(2)
                  <<" " <<r.csi->mnemonic <<" " <<r.csi->op_str <<"\n";
#endif
        auto operands = new SgAsmOperandList;
        for (uint8_t i = 0; i < detail.op_count; ++i) {
            auto operand = makeOperand(*r.csi, detail.operands[i]);
            ASSERT_not_null(operand);
            ASSERT_not_null(operand->get_type());
            operands->get_operands().push_back(operand);
            operand->set_parent(operands);
        }
        wrapPrePostIncrement(operands, detail);
        auto insn = new SgAsmArm64Instruction(va, r.csi->mnemonic, (Arm64InstructionKind)r.csi->id, detail.cc);
        insn->set_raw_bytes(SgUnsignedCharList(r.csi->bytes+0, r.csi->bytes+r.csi->size));
        insn->set_operandList(operands);
        insn->set_condition(detail.cc);
        operands->set_parent(insn);
        retval = insn;
    } else {
        ASSERT_not_reachable("invalid ARM architecture");
    }
    ASSERT_not_null(retval);

    /* Note successors if necessary */
    if (successors) {
        bool complete;
        *successors |= retval->getSuccessors(complete/*out*/);
    }

    return retval;
}

SgAsmInstruction*
DisassemblerArm::makeUnknownInstruction(const Exception &e) {
    SgAsmArm64Instruction *insn = new SgAsmArm64Instruction(e.ip, "unknown", ARM64_INS_INVALID, ARM64_CC_INVALID);
    SgAsmOperandList *operands = new SgAsmOperandList();
    insn->set_operandList(operands);
    operands->set_parent(insn);
    insn->set_raw_bytes(e.bytes);
    return insn;
}

SgAsmExpression*
DisassemblerArm::makeOperand(const cs_insn &insn, const cs_arm64_op &op) {
    SgAsmExpression *retval = nullptr;

    switch (op.type) {
        case ARM64_OP_INVALID:
            ASSERT_not_reachable("invalid operand type from Capstone");

        case ARM64_OP_REG: {    // register operand
            RegisterDescriptor reg = makeRegister(op.reg);
            retval = new SgAsmDirectRegisterExpression(reg);
            retval->set_type(registerType(op.reg, op.vas));
            retval = extendOperand(retval, op.ext, retval->get_type(), op.shift.type, op.shift.value);
            break;
        }

        case ARM64_OP_IMM:      // immediate operand
            // Capstone doesn't have any types. It just stores all immediate values in a 64 bit field and assumes that the
            // instruction semantics and unparser will only use that part of the field that is applicable for this instruction
            // and argument type.
            retval = new SgAsmIntegerValueExpression(op.imm, SageBuilderAsm::buildTypeU64());
            retval = extendOperand(retval, op.ext, retval->get_type(), op.shift.type, op.shift.value);
            break;

        case ARM64_OP_MEM: {    // memory operand
            auto base = new SgAsmDirectRegisterExpression(makeRegister(op.mem.base));
            base->set_type(registerType(op.mem.base, ARM64_VAS_INVALID));
            SgAsmExpression *addr = base;
            SgAsmType *u64 = SageBuilderAsm::buildTypeU64();

            if (op.mem.index != ARM64_REG_INVALID) {
                auto index = new SgAsmDirectRegisterExpression(makeRegister(op.mem.index));
                index->set_type(registerType(op.mem.index, ARM64_VAS_INVALID));

                if (op.mem.disp != 0) {
                    auto disp = new SgAsmIntegerValueExpression(op.mem.disp, u64);
                    switch (op.shift.type) {
                        case ARM64_SFT_INVALID:
                            addr = SageBuilderAsm::buildAddExpression(addr, SageBuilderAsm::buildAddExpression(index, disp, u64), u64);
                            break;
                        case ARM64_SFT_LSL:
                            addr = SageBuilderAsm::buildLslExpression(addr, disp, u64);
                            break;
                        case ARM64_SFT_MSL: // Same as LSL but filling low bits with ones
                            addr = SageBuilderAsm::buildMslExpression(addr, disp, u64);
                            break;
                        case ARM64_SFT_LSR:
                            addr = SageBuilderAsm::buildLsrExpression(addr, disp, u64);
                            break;
                        case ARM64_SFT_ASR:
                            addr = SageBuilderAsm::buildAsrExpression(addr, disp, u64);
                            break;
                        case ARM64_SFT_ROR:
                            addr = SageBuilderAsm::buildRorExpression(addr, disp, u64);
                            break;
                    }
                } else {
                    addr = SageBuilderAsm::buildAddExpression(addr, index, u64);
                }
            } else if (op.mem.disp != 0) {
                auto disp = new SgAsmIntegerValueExpression(op.mem.disp, u64);
                addr = SageBuilderAsm::buildAddExpression(addr, disp, u64);
            }
            addr = extendOperand(addr, op.ext, addr->get_type(), op.shift.type, op.shift.value);

            auto mre = new SgAsmMemoryReferenceExpression;
            mre->set_address(addr);
            mre->set_type(typeForMemoryRead(insn));
            retval = mre;
            break;
        }

        case ARM64_OP_FP:       // floating-point operand
            retval = SageBuilderAsm::buildValueFloat(op.fp, SageBuilderAsm::buildIeee754Binary64());
            break;

        case ARM64_OP_CIMM:     // C-immediate
            retval = new SgAsmArm64CImmediateOperand(op.sys);
            retval->set_type(SageBuilderAsm::buildTypeU(1)); // FIXME: not sure what the type should be, but probably not this
            break;

        case ARM64_OP_REG_MRS:  // MRS register operand
        case ARM64_OP_REG_MSR:  // MSR register operand
            retval = new SgAsmArm64SysMoveOperand(op.reg);
            retval->set_type(SageBuilderAsm::buildTypeU(1)); // FIXME: not sure what the type should be, but probably not this
            break;

        case ARM64_OP_PSTATE:   // PState operand
            break;

        case ARM64_OP_SYS:      // SYS operand for IC/DC/AT/TLBI
            retval = new SgAsmArm64AtOperand((Arm64AtOperation)op.sys);
            retval->set_type(SageBuilderAsm::buildTypeU(1)); // FIXME: not sure what the type should be, but probably not this
            break;

        case ARM64_OP_PREFETCH: // prefetch operand
            retval = new SgAsmArm64PrefetchOperand(op.prefetch);
            retval->set_type(SageBuilderAsm::buildTypeU(1)); // FIXME: not sure what the type should be, but probably not this
            break;

        case ARM64_OP_BARRIER:  // memory barrier operand for ISB/DMB/DSB instruction
            break;
    }
    ASSERT_not_null(retval);
    ASSERT_not_null(retval->get_type());
    return retval;
}

SgAsmExpression*
DisassemblerArm::extendOperand(SgAsmExpression *expr, arm64_extender extender, SgAsmType *dstType, arm64_shifter shifter,
                               unsigned shiftAmount) const {
    ASSERT_not_null(expr);
    ASSERT_not_null(expr->get_type());

    if (extender != ARM64_EXT_INVALID) {
        ASSERT_require(isSgAsmIntegerType(expr->get_type()));
        ASSERT_require(isSgAsmIntegerType(dstType));

        // First we need to truncate the incoming value to the specified input size.
        SgAsmIntegerType *tmpType = nullptr;
        switch (extender) {
            case ARM64_EXT_INVALID:
                ASSERT_not_reachable("invalid extender function");
            case ARM64_EXT_UXTB:
                tmpType = SageBuilderAsm::buildTypeU8();
                break;
            case ARM64_EXT_SXTB:
                tmpType = SageBuilderAsm::buildTypeI8();
                break;
            case ARM64_EXT_UXTH:
                tmpType = SageBuilderAsm::buildTypeU16();
                break;
            case ARM64_EXT_SXTH:
                tmpType = SageBuilderAsm::buildTypeI16();
                break;
            case ARM64_EXT_UXTW:
                tmpType = SageBuilderAsm::buildTypeU32();
                break;
            case ARM64_EXT_SXTW:
                tmpType = SageBuilderAsm::buildTypeI32();
                break;
            case ARM64_EXT_UXTX:
                tmpType = SageBuilderAsm::buildTypeU64();
                break;
            case ARM64_EXT_SXTX:
                tmpType = SageBuilderAsm::buildTypeI64();
                break;
        }
        if (tmpType->get_nBits() != expr->get_type()->get_nBits())
            expr = SageBuilderAsm::buildTruncateExpression(expr, tmpType);

        // Now that we have the intermediate type of the correct size, extend it to the final size. If the intermediate
        // expression is already the same as the destination size then there's nothing to do.
        if (tmpType->get_nBits() != dstType->get_nBits()) {
            switch (extender) {
                case ARM64_EXT_UXTB:
                case ARM64_EXT_UXTH:
                case ARM64_EXT_UXTW:
                case ARM64_EXT_UXTX:
                    expr = SageBuilderAsm::buildUnsignedExtendExpression(expr, dstType);
                    break;
                default:
                    expr = SageBuilderAsm::buildSignedExtendExpression(expr, dstType);
                    break;
            }
        }
    }

    if (shifter != ARM64_SFT_INVALID && shiftAmount != 0) {
        ASSERT_require(shiftAmount < dstType->get_nBits());
        auto amount = SageBuilderAsm::buildValueU8(shiftAmount);
        switch (shifter) {
            case ARM64_SFT_LSL:
                expr = SageBuilderAsm::buildLslExpression(expr, amount);
                break;
            case ARM64_SFT_MSL:
                expr = SageBuilderAsm::buildMslExpression(expr, amount);
                break;
            case ARM64_SFT_LSR:
                expr = SageBuilderAsm::buildLsrExpression(expr, amount);
                break;
            case ARM64_SFT_ASR:
                expr = SageBuilderAsm::buildAsrExpression(expr, amount);
                break;
            case ARM64_SFT_ROR:
                expr = SageBuilderAsm::buildRorExpression(expr, amount);
                break;
            case ARM64_SFT_INVALID:
                ASSERT_not_reachable("invalid shifter");
        }
    }

    ASSERT_not_null(expr);
    ASSERT_not_null(expr->get_type());
    ASSERT_require(expr->get_type() == dstType);
    return expr;
}

void
DisassemblerArm::wrapPrePostIncrement(SgAsmOperandList *operands, const cs_arm64 &cs_detail) {
    ASSERT_not_null(operands);

    if (cs_detail.writeback) {
        // This A64 instruction must have a memory reference operand and is either pre-update or post-update.  A pre-update
        // instruction will have both the register and the increment within the memory address expression, such as:
        //
        //    Opcode:      95 9d 87 ad
        //    ARM syntax:  stp q21, q7, [x12, #0xf0]!
        //    ROSE syntax: stp q21, q7, u128 [x12 + 0xf0]   (before the transformation)
        //    ROSE syntax: stp q21, q7, u128 [x12 += 0xf0]  (after the transformation)
        //
        // Semantically, the above means that the value in register x12 is added to the constant 0xf0 resulting in the first
        // (lowest) address where data is written to memory.
        //
        // The post-increment operations is slightly different in that Capstone disassembles the expression as two separate
        // instruction operands. For instance:
        //
        //    Opcode:       95 9d 87 ac
        //    ARM syntax:   stp q21, q7, [x12], #0xf0
        //    ROSE syntax:  stp q21, q7, u128 [x12], 0xf0             (before the transformation)
        //    ROSE syntax:  stp q21, q7, u128 [x12 then x12 += 0xf0]  (after the transofmration)
        //
        // Semantically, the value stored originally in x12 is used as the memory address, and then the register is incremented
        // by 0xf0. ROSE combines these two arguments into a single expression containing a SgAsmBinaryAddPostupdate node.
        for (size_t i = 0; i < operands->get_operands().size(); ++i) {
            if (auto mre = isSgAsmMemoryReferenceExpression(operands->get_operands()[i])) {
                if (i + 1 == operands->get_operands().size()) {
                    // The memory reference expression is the last operand, therefore this is a pre-increment. The address must
                    // be an addition expression whose lhs is a register reference (or just a register expression, in which
                    // case the pre-increment is a no-op).  Replace the SgAsmBinaryAdd with a SgAsmBinaryAddPreupdate node.
                    // First unlink things from the tree...
                    SgAsmBinaryAdd *add = isSgAsmBinaryAdd(mre->get_address());
                    if (!add) {
                        ASSERT_require(isSgAsmDirectRegisterExpression(mre->get_address()));
                        return; // if the offset is zero then we have already removed it and the pre-increment is a no-op.
                    }
                    SgAsmType *type = add->get_type();
                    ASSERT_not_null(type);
                    SgAsmExpression *lhs = add->get_lhs();
                    ASSERT_require(isSgAsmDirectRegisterExpression(lhs));
                    add->set_lhs(NULL);
                    lhs->set_parent(NULL);
                    SgAsmExpression *rhs = add->get_rhs();
                    add->set_rhs(NULL);
                    rhs->set_parent(NULL);
                    mre->set_address(NULL);
                    add->set_parent(NULL);
#if 0 // FIXME: Deleting parts of an AST is unsafe because ROSE has no formal ownership rules for its nodes.
                    delete add;
#else // instead, we'll just drop it
                    add = nullptr;
#endif
                    // Construct the replacement and link it into the tree.
                    auto newNode = SageBuilderAsm::buildAddPreupdateExpression(lhs, rhs, type);
                    mre->set_address(newNode);
                    newNode->set_parent(mre);
                    return;

                } else {
                    // The memory reference must be a post-increment, in which case it must the the penultimate argument. The
                    // address must be a register reference. First unlink things from the tree...
                    ASSERT_require(i + 2 == operands->get_operands().size());
                    SgAsmExpression *lhs = mre->get_address();
                    mre->set_address(NULL);
                    lhs->set_parent(NULL);
                    SgAsmExpression *rhs = operands->get_operands()[i+1];
                    operands->get_operands().erase(operands->get_operands().begin() + i + 1);
                    rhs->set_parent(NULL);

                    // Construct the replacement and link it into the tree
                    auto newNode = SageBuilderAsm::buildAddPostupdateExpression(lhs, rhs, lhs->get_type());
                    mre->set_address(newNode);
                    newNode->set_parent(mre);
                    return;
                }
            }
        }
        ASSERT_not_reachable("writeback was set but there is no memory reference operand");
    }
}

SgAsmType*
DisassemblerArm::typeForMemoryRead(const cs_insn &insn) {
    using namespace ::Rose::BitOps;
    using Kind = ::Rose::BinaryAnalysis::Arm64InstructionKind;
    // FIXME: This is for little endian. Will need to be swapped for big endian when we implement that.
    uint32_t code = (insn.bytes[0] << 0) | (insn.bytes[1] << 8) | (insn.bytes[2] << 16) | (insn.bytes[3] << 24);

    switch ((Kind)insn.id) {
        //--------------------------------------------------------------------------------------------------------
        //case Kind::ARM64_INS_LDADDB:    // atomic add on byte in memory                       -- not in capstone
        //case Kind::ARM64_INS_LDADDAB:   // atomic add on byte in memorym                      -- not in capstone
        //case Kind::ARM64_INS_LDADDALB:  // atomic add on byte in memory                       -- not in capstone
        //case Kind::ARM64_INS_LDADDLB:   // atomic add on byte in memory                       -- not in capstone
        //case Kind::ARM64_INS_LDAPRB:    // load-acquire RCpc register byte                    -- not in capstone
        //case Kind::ARM64_INS_LDAPURB:   // load-acquire RCpc register byte unscaled           -- not in capstone
        //case Kind::ARM64_INS_LDAPURSB:  // load-acquire RCpc regiater signed byte unscaled    -- not in capstone
        case Kind::ARM64_INS_LDARB:     // load-acquire register byte
        case Kind::ARM64_INS_LDAXRB:    // load-acquire exclusive register byte
        //case Kind::ARM64_INS_LDCLRB:    // atomic bit clear on byte in memory                 -- not in capstone
        //case Kind::ARM64_INS_LDCLRAB:   // atomic bit clear on byte in memory                 -- not in capstone
        //case Kind::ARM64_INS_LDCLRALB:  // atomic bit clear on byte in memory                 -- not in capstone
        //case Kind::ARM64_INS_LDCLRLB:   // atomic bit clear on byte in memory                 -- not in capstone
        //case Kind::ARM64_INS_LDEORB:    // atomic exclusive OR on byte in memory              -- not in capstone
        //case Kind::ARM64_INS_LDEORAB:   // atomic exclusive OR on byte in memory              -- not in capstone
        //case Kind::ARM64_INS_LDEORALB:  // atomic exclusive OR on byte in memory              -- not in capstone
        //case Kind::ARM64_INS_LDEORLB:   // atomic exclusive OR on byte in memory              -- not in capstone
        //case Kind::ARM64_INS_LDLARB:    // load LOAcquire register byte                       -- not in capstone
        case Kind::ARM64_INS_LDRB:      // load register byte
        case Kind::ARM64_INS_LDRSB:     // load register signed byte
        //case Kind::ARM64_INS_LDSETB:    // atomic bit set on byte in memory                   -- not in capstone
        //case Kind::ARM64_INS_LDSETAB:   // atomic bit set on byte in memory                   -- not in capstone
        //case Kind::ARM64_INS_LDSETALB:  // atomic bit set on byte in memory                   -- not in capstone
        //case Kind::ARM64_INS_LDSETLB:   // atomic bit set on byte in memory                   -- not in capstone
        //case Kind::ARM64_INS_LDSMAXB:   // atomic signed maximum on byte in memory            -- not in capstone
        //case Kind::ARM64_INS_LDSMAXAB:  // atomic signed maximum on byte in memory            -- not in capstone
        //case Kind::ARM64_INS_LDSMAXALB: // atomic signed maximum on byte in memory            -- not in capstone
        //case Kind::ARM64_INS_LDSMAXLB:  // atomic signed maximum on byte in memory            -- not in capstone
        //case Kind::ARM64_INS_LDSMINB:   // atomic signed minimum on byte in memory            -- not in capstone
        //case Kind::ARM64_INS_LDSMINAB:  // atomic signed minimum on byte in memory            -- not in capstone
        //case Kind::ARM64_INS_LDSMINALB: // atomic signed minimum on byte in memory            -- not in capstone
        //case Kind::ARM64_INS_LDSMINLB:  // atomic signed minimum on byte in memory            -- not in capstone
        case Kind::ARM64_INS_LDTRB:     // load register byte unprivileged
        case Kind::ARM64_INS_LDTRSB:    // load register signed byte unprivileged
        //case Kind::ARM64_INS_LDUMAXB:   // atomic unsigned maximum on byte in memory          -- not in capstone
        //case Kind::ARM64_INS_LDUMAXAB:  // atomic unsigned maximum on byte in memory          -- not in capstone
        //case Kind::ARM64_INS_LDUMAXALB: // atomic unsigned maximum on byte in memory          -- not in capstone
        //case Kind::ARM64_INS_LDUMAXLB:  // atomic unsigned maximum on byte in memory          -- not in capstone
        //case Kind::ARM64_INS_LDUMINB:   // atomic unsigned minimum on byte in memory          -- not in capstone
        //case Kind::ARM64_INS_LDUMINAB:  // atomic unsigned minimum on byte in memory          -- not in capstone
        //case Kind::ARM64_INS_LDUMINALB: // atomic unsigned minimum on byte in memory          -- not in capstone
        //case Kind::ARM64_INS_LDUMINLB:  // atomic unsigned minimum on byte in memory          -- not in capstone
        case Kind::ARM64_INS_LDURB:     // load register byte unscaled
        case Kind::ARM64_INS_LDURSB:    // load register signed byte unscaled
        case Kind::ARM64_INS_LDXRB:     // load exclusive register byte
        case Kind::ARM64_INS_PRFM:      // prefetch memory
        case Kind::ARM64_INS_PRFUM:     // prefetch memory, unscaled offset
        //case Kind::ARM64_INS_STADDB:    // atomic add on byte in memory                       -- not in capstone
        //case Kind::ARM64_INS_STADDLB:   // atomic add on byte in memory                       -- not in capstone
        //case Kind::ARM64_INS_STCLRB:    // atomic bit clear on byte in memory                 -- not in capstone
        //case Kind::ARM64_INS_STCLRLB:   // atomic bit clear on byte in memory                 -- not in capstone
        //case Kind::ARM64_INS_STEORB:    // atomic exclusive OR on byte in memory              -- not in capstone
        //case Kind::ARM64_INS_STEORLB:   // atomic exclusive OR on byte in memory              -- not in capstone
        //case Kind::ARM64_INS_STLLRB:    // store LORelease register byte                      -- not in capstone
        case Kind::ARM64_INS_STLRB:     // store-release register byte
        //case Kind::ARM64_INS_STLURB:    // store-release register byte                        -- not in capstone
        case Kind::ARM64_INS_STLXRB:    // store-release exclusive register byte
        case Kind::ARM64_INS_STRB:      // store register byte
        //case Kind::ARM64_INS_STSETB:    // atomic bit set on byte in memory                   -- not in capstone
        //case Kind::ARM64_INS_STSETLB:   // atomic bit set on byte in memory                   -- not in capstone
        //case Kind::ARM64_INS_STSMAXB:   // atomic signed maximum on byte in memory            -- not in capstone
        //case Kind::ARM64_INS_STSMAXLB:  // atomic signed maximum on byte in memory            -- not in capstone
        //case Kind::ARM64_INS_STSMINB:   // atomic signed minimum on byte in memory            -- not in capstone
        //case Kind::ARM64_INS_STSMINLB:  // atomic signed minimum on byte in memory            -- not in capstone
        case Kind::ARM64_INS_STTRB:     // store register byte unprivileged
        //case Kind::ARM64_INS_STUMAXB:   // atomic unsigned maximum on byte in memory          -- not in capstone
        //case Kind::ARM64_INS_STUMAXLB:  // atomic unsigned maximum on byte in memory          -- not in capstone
        //case Kind::ARM64_INS_STUMINB:   // atomic unsigned minimum on byte in memory          -- not in capstone
        //case Kind::ARM64_INS_STUMINLB:  // atomic unsigned minimum on byte in memory          -- not in capstone
        case Kind::ARM64_INS_STURB:     // store register byte unscaled
        case Kind::ARM64_INS_STXRB:     // store exclusive register byte
            return SageBuilderAsm::buildTypeU(8);

        //--------------------------------------------------------------------------------------------------------
        //case Kind::ARM64_INS_LDADDH:    // atomic add on halfword in memory                   -- not in capstone
        //case Kind::ARM64_INS_LDADDAH:   // atomic add on halfword in memory                   -- not in capstone
        //case Kind::ARM64_INS_LDADDALH:  // atomic add on halfword in memory                   -- not in capstone
        //case Kind::ARM64_INS_LDADDLH:   // atomic add on halfword in memory                   -- not in capstone
        //case Kind::ARM64_INS_LDAPRH:    // load-acquire RCpc register halfword                -- not in capstone
        //case Kind::ARM64_INS_LDAPURH:   // load-acquire RCpc register halfword unscaled       -- not in capstone
        //case Kind::ARM64_INS_LDAPURSH:  // load-acquire RCpc register signed halfword unscaled-- not in capstone
        case Kind::ARM64_INS_LDARH:     // load-acquire register halfword
        case Kind::ARM64_INS_LDAXRH:    // load-acquire exclusive register halfword
        //case Kind::ARM64_INS_LDCLRH:    // atomic bit clear on halfword in memory             -- not in capstone
        //case Kind::ARM64_INS_LDCLRAH:   // atomic bit clear on halfword in memory             -- not in capstone
        //case Kind::ARM64_INS_LDCLRALH:  // atomic bit clear on halfword in memory             -- not in capstone
        //case Kind::ARM64_INS_LDCLRLH:   // atomic bit clear on halfword in memory             -- not in capstone
        //case Kind::ARM64_INS_LDEORH:    // atomic exclusive OR on halfword in memory          -- not in capstone
        //case Kind::ARM64_INS_LDEORAH:   // atomic exclusive OR on halfword in memory          -- not in capstone
        //case Kind::ARM64_INS_LDEORALH:  // atomic exclusive OR on halfword in memory          -- not in capstone
        //case Kind::ARM64_INS_LDEORLH:   // atomic exclusive OR on halfword in memory          -- not in capstone
        //case Kind::ARM64_INS_LDLARH:    // load LOAcquire register halfword                   -- not in capstone
        case Kind::ARM64_INS_LDRH:      // load register halfword
        case Kind::ARM64_INS_LDRSH:     // load register signed halfword
        //case Kind::ARM64_INS_LDSETH:    // atomic bit set on halfword in memory               -- not in capstone
        //case Kind::ARM64_INS_LDSETAH:   // atomic bit set on halfword in memory               -- not in capstone
        //case Kind::ARM64_INS_LDSETALH:  // atomic bit set on halfword in memory               -- not in capstone
        //case Kind::ARM64_INS_LDSETLH:   // atomic bit set on halfword in memory               -- not in capstone
        //case Kind::ARM64_INS_LDSMAXH:   // atomic signed maximum on halfword in memory        -- not in capstone
        //case Kind::ARM64_INS_LDSMAXAH:  // atomic signed maximum on halfword in memory        -- not in capstone
        //case Kind::ARM64_INS_LDSMAXALH: // atomic signed maximum on halfword in memory        -- not in capstone
        //case Kind::ARM64_INS_LDSMAXLH:  // atomic signed maximum on halfword in memory        -- not in capstone
        //case Kind::ARM64_INS_LDSMINH:   // atomic signed minimum on halfword in memory        -- not in capstone
        //case Kind::ARM64_INS_LDSMINAH:  // atomic signed minimum on halfword in memory        -- not in capstone
        //case Kind::ARM64_INS_LDSMINALH: // atomic signed minimum on halfword in memory        -- not in capstone
        //case Kind::ARM64_INS_LDSMINLH:  // atomic signed minimum on halfword in memory        -- not in capstone
        case Kind::ARM64_INS_LDTRH:     // load register halfword unprivileged
        case Kind::ARM64_INS_LDTRSH:    // load register signed halfword unprivileged
        //case Kind::ARM64_INS_LDUMAXH:   // atomic unsigned maximum on halfword in memory      -- not in capstone
        //case Kind::ARM64_INS_LDUMAXAH:  // atomic unsigned maximum on halfword in memory      -- not in capstone
        //case Kind::ARM64_INS_LDUMAXALH: // atomic unsigned maximum on halfword in memory      -- not in capstone
        //case Kind::ARM64_INS_LDUMAXLH:  // atomic unsigned maximum on halfword in memory      -- not in capstone
        //case Kind::ARM64_INS_LDUMINH:   // atomic unsigned minimum on halfword in memory      -- not in capstone
        //case Kind::ARM64_INS_LDUMINAH:  // atomic unsigned minimum on halfword in memory      -- not in capstone
        //case Kind::ARM64_INS_LDUMINALH: // atomic unsigned minimum on halfword in memory      -- not in capstone
        //case Kind::ARM64_INS_LDUMINLH:  // atomic unsigned minimum on halfword in memory      -- not in capstone
        case Kind::ARM64_INS_LDURH:     // load register halfword unscaled
        case Kind::ARM64_INS_LDURSH:    // load register signed halfword unscaled
        case Kind::ARM64_INS_LDXRH:     // load exclusive register halfword
        //case Kind::ARM64_INS_LDADDH:    // atomic add on halfword in memory                   -- not in capstone
        //case Kind::ARM64_INS_LDADDLH:   // atomic add on halfword in memory                   -- not in capstone
        //case Kind::ARM64_INS_STCLRH:    // atomic bit clear on halfword in memory             -- not in capstone
        //case Kind::ARM64_INS_STCLRLH:   // atomic bit clear on halfword in memory             -- not in capstone
        //case Kind::ARM64_INS_STEORH:    // atomic exclusive OR on halfword in memory          -- not in capstone
        //case Kind::ARM64_INS_STEORLH:   // atomic exclusive OR on halfword in memory          -- not in capstone
        //case Kind::ARM64_INS_STLLRH:    // store LORelease register halfword                  -- not in capstone
        case Kind::ARM64_INS_STLRH:     // store-release register halfword
        //case Kind::ARM64_INS_STLURH:    // store-release reigster halfword                    -- not in capstone
        case Kind::ARM64_INS_STLXRH:    // store-release exclusive register halfword
        case Kind::ARM64_INS_STRH:      // store register halfword
        //case Kind::ARM64_INS_STSETH:    // atomic bit set on halfword in memory               -- not in capstone
        //case Kind::ARM64_INS_STSETLH:   // atomic bit set on halfword in memory               -- not in capstone
        //case Kind::ARM64_INS_STSMAXH:   // atomic signed maximum on halfword in memory        -- not in capstone
        //case Kind::ARM64_INS_STSMAXLH:  // atomic signed maximum on halfword in memory        -- not in capstone
        //case Kind::ARM64_INS_STSMINH:   // atomic signed minimum on halfword in memory        -- not in capstone
        //case Kind::ARM64_INS_STSMINLH:  // atomic signed minimum on halfword in memory        -- not in capstone
        case Kind::ARM64_INS_STTRH:     // store register halfword unprivileged
        //case Kind::ARM64_INS_STUMAXH:   // atomic unsigned maximum on halfword in memory      -- not in capstone
        //case Kind::ARM64_INS_STUMAXLH:  // atomic unsigned maximum on halfword in memory      -- not in capstone
        //case Kind::ARM64_INS_STUMINH:   // atomic unsigned minimum on halfword in memory      -- not in capstone
        //case Kind::ARM64_INS_STUMINLH:  // atomic unsigned minimum on halfword in memory      -- not in capstone
        case Kind::ARM64_INS_STURH:     // store register halfword unscaled
        case Kind::ARM64_INS_STXRH:     // store exclusive register halfword
            return SageBuilderAsm::buildTypeU(16);

        //--------------------------------------------------------------------------------------------------------
        case Kind::ARM64_INS_LDRSW:     // load register signed word
        //case Kind::ARM64_INS_LDAPURSW:  // load-acquire RCpc register signed word unscaled    -- not in capstone
        case Kind::ARM64_INS_LDTRSW:    // load register signed word unprivileged
        case Kind::ARM64_INS_LDURSW:    // laod register signed word
            return SageBuilderAsm::buildTypeU(32);

        //--------------------------------------------------------------------------------------------------------
        case Kind::ARM64_INS_LDPSW:     // load pair of registers signed word
        //case Kind::ARM64_INS_LDRAA:     // load register with pointer authentication          -- not in capstone
        //case Kind::ARM64_INS_LDRAB:     // load register with pointer authentication          -- not in capstone
            return SageBuilderAsm::buildTypeU(64); // two 32-bit words

        //--------------------------------------------------------------------------------------------------------
        //case Kind::ARM64_INS_LDADD:     // atomic add on word or doubleword in memory         -- not in capstone
        //case Kind::ARM64_INS_LDADDA:    // atomic add on word or doubleword in memory         -- not in capstone
        //case Kind::ARM64_INS_LDADDAL:   // atomic add on word or doubleword in memory         -- not in capstone
        //case Kind::ARM64_INS_LDADDL:    // atomic add on word or doubleword in memory         -- not in capstone
        //case Kind::ARM64_INS_LDALR:     // load LOAcquire register                            -- not in capstone
        //case Kind::ARM64_INS_LDAPR:     // load-acquire RCpc register                         -- not in capstone
        //case Kind::ARM64_INS_LDAPUR:    // load-acquire RCpe register unscaled                -- not in capstone
        case Kind::ARM64_INS_LDAR:      // load-acquire register
        case Kind::ARM64_INS_LDAXR:     // load-acquire exclusive register
        //case Kind::ARM64_INS_LDCLR:     // atomic bit clear on word or doubleword in memory   -- not in capstone
        //case Kind::ARM64_INS_LDCLRA:    // atomic bit clear on word or doubleword in memory   -- not in capstone
        //case Kind::ARM64_INS_LDCLRAL:   // atomic bit clear on word or doubleword in memory   -- not in capstone
        //case Kind::ARM64_INS_LDCLRL:    // atomic bit clear on word or doubleword in memory   -- not in capstone
        //case Kind::ARM64_INS_LDEOR:     // atomic exclusive OR on word or doubleword in memory-- not in capstone
        //case Kind::ARM64_INS_LDEORA:    // atomic exclusive OR on word or doubleword in memory-- not in capstone
        //case Kind::ARM64_INS_LDEORAL:   // atomic exclusive OR on word or doubleword in memory-- not in capstone
        //case Kind::ARM64_INS_LDEORL:    // atomic exclusive OR on word or doubleword in memory-- not in capstone
        //case Kind::ARM64_INS_LDSET:     // atomic bit set on word or doubleword in memory     -- not in capstone
        //case Kind::ARM64_INS_LDSETA:    // atomic bit set on word or doubleword in memory     -- not in capstone
        //case Kind::ARM64_INS_LDSETAL:   // atomic bit set on word or doubleword in memory     -- not in capstone
        //case Kind::ARM64_INS_LDSETL:    // atomic bit set on word or doubleword in memory     -- not in capstone
        //case Kind::ARM64_INS_LDSMAX:    // atomic signed maximum on word or doubleword in memory-- not in capstone
        //case Kind::ARM64_INS_LDSMAXA:   // atomic signed maximum on word or doubleword in memory-- not in capstone
        //case Kind::ARM64_INS_LDSMAXAL:  // atomic signed maximum on word or doubleword in memory-- not in capstone
        //case Kind::ARM64_INS_LDSMAXL:   // atomic signed maximum on word or doubleword in memory-- not in capstone
        //case Kind::ARM64_INS_LDSMIN:    // atomic signed minimum on word or doubleword in memory-- not in capstone
        //case Kind::ARM64_INS_LDSMINA:   // atomic signed minimum on word or doubleword in memory-- not in capstone
        //case Kind::ARM64_INS_LDSMINAL:  // atomic signed minimum on word or doubleword in memory-- not in capstone
        //case Kind::ARM64_INS_LDSMINL:   // atomic signed minimum on word or doubleword in memory-- not in capstone
        case Kind::ARM64_INS_LDTR:      // load register unprivileged
        //case Kind::ARM64_INS_LDUMAX:    // atomic unsigned maximum on word or doubleword in memory-- not in capstone
        //case Kind::ARM64_INS_LDUMAXA:   // atomic unsigned maximum on word or doubleword in memory-- not in capstone
        //case Kind::ARM64_INS_LDUMAXAL:  // atomic unsigned maximum on word or doubleword in memory-- not in capstone
        //case Kind::ARM64_INS_LDUMAXL:   // atomic unsigned maximum on word or doubleword in memory-- not in capstone
        //case Kind::ARM64_INS_LDUMIN:    // atomic unsigned minimum on word or doubleword in memory-- not in capstone
        //case Kind::ARM64_INS_LDUMINA:   // atomic unsigned minimum on word or doubleword in memory-- not in capstone
        //case Kind::ARM64_INS_LDUMINAL:  // atomic unsigned minimum on word or doubleword in memory-- not in capstone
        //case Kind::ARM64_INS_LDUMINL:   // atomic unsigned minimum on word or doubleword in memory-- not in capstone
        case Kind::ARM64_INS_LDXR:      // load exclusive register
        //case Kind::ARM64_INS_STADD:     // atomic add on word or doubleword in memory         -- not in capstone
        //case Kind::ARM64_INS_STADDL:    // atomic add on word or doubleword in memory         -- not in capstone
        //case Kind::ARM64_INS_STCLR:     // atomci bit clear on word or doubleword in memory   -- not in capstone
        //case Kind::ARM64_INS_STCLRL:    // atomic bit clear on word or doubleword in memory   -- not in capstone
        //case Kind::ARM64_INS_STEOR:     // atomic exclusive OR on word or doubleword in memory-- not in capstone
        //case Kind::ARM64_INS_STEORL:    // atomic exclusive OR on word or doubleword in memory-- not in capstone
        //case Kind::ARM64_INS_STLLR:     // store LORelease register                           -- not in capstone
        case Kind::ARM64_INS_STLR:      // store-release register
        //case Kind::ARM64_INS_STLUR:     // store-release register unscaled                    -- not in capstone
        case Kind::ARM64_INS_STLXR:     // store-release exclusive register
        //case Kind::ARM64_INS_STSET:     // atomic bit set on word or doubleword in memory     -- not in capstone
        //case Kind::ARM64_INS_STSETL:    // atomic bit set on word or doubleword in memory     -- not in capstone
        //case Kind::ARM64_INS_STSMAX:    // atomic signed maximum on word or doubleword in memory-- not in capstone
        //case Kind::ARM64_INS_STSMAXL:   // atomic signed maximum on word or doubleword in memory-- not in capstone
        //case Kind::ARM64_INS_STSMIN:    // atomic signed minimum on word or doubleword in memory-- not in capstone
        //case Kind::ARM64_INS_STSMINL:   // atomic signed minimum on word or doubleword in memory-- not in capstone
        case Kind::ARM64_INS_STTR:      // store register
        //case Kind::ARM64_INS_STUMAX:    // atomic unsigned maximum on word or doubleword in memory-- not in capstone
        //case Kind::ARM64_INS_STUMAXL:   // atomic unsigned maximum on word or doubleword in memory-- not in capstone
        //case Kind::ARM64_INS_STUMIN:    // atomic unsigned minimum on word or doubleword in memory-- not in capstone
        //case Kind::ARM64_INS_STUMINL:   // atomic unsigned minimum on word or doubleword in memory-- not in capstone
        case Kind::ARM64_INS_STXR:      // store exclusive register
            return SageBuilderAsm::buildTypeU(bit(code, 30) ? 64 : 32);

        //--------------------------------------------------------------------------------------------------------
        case Kind::ARM64_INS_LD1:       // load (multiple or single) 1-element structures
        case Kind::ARM64_INS_LD1R:      // load one single-element structure and replicate to all lanes
        case Kind::ARM64_INS_LD2:       // load (multiple or single) 2-element structures
        case Kind::ARM64_INS_LD2R:      // load single 2-element structure and replicate to all lanes
        case Kind::ARM64_INS_LD3:       // load (multiple or single) 3-element structures
        case Kind::ARM64_INS_LD3R:      // load single 3-element structure and replicate to all lanes
        case Kind::ARM64_INS_LD4:       // load (multiple or single) 4-element structures
        case Kind::ARM64_INS_LD4R:      // load single 4-element structure and replicate to all lanes
        case Kind::ARM64_INS_LDAXP:     // load-acquire exclusive pair of registers
        case Kind::ARM64_INS_LDXP:      // load exclusive pair of registers
        case Kind::ARM64_INS_ST1:       // store (multiple or single) 1-element structures
        case Kind::ARM64_INS_ST2:       // store (multiple or single) 2-element structures
        case Kind::ARM64_INS_ST3:       // store (multiple or single) 3-element structures
        case Kind::ARM64_INS_ST4:       // store (multiple or single) 4-element structures
        case Kind::ARM64_INS_STLXP:    // store-release exclusive pair of registers
        case Kind::ARM64_INS_STXP:      // store exclusive pair of registers
            return SageBuilderAsm::buildTypeU(bit(code, 30) ? 128 : 64);

        //--------------------------------------------------------------------------------------------------------
        case Kind::ARM64_INS_LDNP:      // load pair of registers with non-temporal hint
        case Kind::ARM64_INS_LDP:       // load pair of registers
        case Kind::ARM64_INS_STNP:      // store pair of registers with non-temporal hint
        case Kind::ARM64_INS_STP: {     // store pair of registers
            if (bits(code, 22, 29) == 0b10110001 || // LDNP (SIMD&FP)
                bits(code, 22, 29) == 0b10110011 || // LDP (SIMD&FP) Post-index
                bits(code, 22, 29) == 0b10110111 || // LDP (SIMD&FP) Pre-index
                bits(code, 22, 29) == 0b10110101 || // LDP (SIMD&FP) Signed offset
                bits(code, 22, 29) == 0b10110000 || // STNP (SIMD&FP)
                bits(code, 22, 29) == 0b10110010 || // STP (SIMD&FP) Post-index
                bits(code, 22, 29) == 0b10110110 || // STP (SIMD&FP) Pre-index
                bits(code, 22, 29) == 0b10110100) { // STP (SIMD&FP) Signed offset
                uint32_t opc = bits(code, 30, 31);
                return SageBuilderAsm::buildTypeU(8 << (2 + opc));
            } else if (bits(code, 22, 30) == 0b010100001 || // LDNP
                       bits(code, 22, 30) == 0b010100011 || // LDP Post-index
                       bits(code, 22, 30) == 0b010100111 || // LDP Pre-index
                       bits(code, 22, 30) == 0b010100101 || // LDP Signed offset
                       bits(code, 22, 30) == 0b010100000 || // STNP
                       bits(code, 22, 30) == 0b010100010 || // STP Post-index
                       bits(code, 22, 30) == 0b010100110 || // STP Pre-index
                       bits(code, 22, 30) == 0b010100100) { // STP Signed offset
                return SageBuilderAsm::buildTypeU(bit(code, 31) ? 64 : 32);
            } else {
                ASSERT_not_reachable("invalid opcode");
            }
        }

        //--------------------------------------------------------------------------------------------------------
        case Kind::ARM64_INS_LDR:       // load register
        case Kind::ARM64_INS_LDUR:      // load register unscaled
        case Kind::ARM64_INS_STR:       // store register
        case Kind::ARM64_INS_STUR: {    // store register unscaled offset
            if ((bits(code, 24, 29) == 0b111100 && bits(code, 21, 22) == 0b10 && bits(code, 10, 11) == 0b01) || // LDR (immediate, SIMD&FP) Post-index
                (bits(code, 24, 29) == 0b111100 && bits(code, 21, 22) == 0b10 && bits(code, 10, 11) == 0b11) || // LDR (immediate, SIMD&FP) Pre-index
                (bits(code, 24, 29) == 0b111101 && bit(code, 22)) || // LDR (immediate, SIMD&FP) Unsigned offset
                (bits(code, 24, 29) == 0b111100 && bits(code, 21, 22) == 0b11 && bits(code, 10, 11) == 0b10) || // LDR (register, SIMD&FP)
                (bits(code, 24, 29) == 0b111100 && bits(code, 21, 22) == 0b10 && bits(code, 10, 11) == 0b00) || // LDUR (SIMD&FP)
                (bits(code, 24, 29) == 0b111100 && bits(code, 21, 22) == 0b00 && bits(code, 10, 11) == 0b01) || // STR (immediate, SIMD&FP) Post-index
                (bits(code, 24, 29) == 0b111100 && bits(code, 21, 22) == 0b00 && bits(code, 10, 11) == 0b11) || // STR (immediate, SIMD&FP) Pre-index
                (bits(code, 24, 29) == 0b111101 && !bit(code, 22)) || // STR (immediate, SIMD&FP) Unsigned offset
                (bits(code, 24, 29) == 0b111100 && bits(code, 21, 22) == 0b01 && bits(code, 10, 11) == 0b10) || // STR (register, SIMD&FP)
                (bits(code, 24, 29) == 0b111100 && bits(code, 21, 22) == 0b00 && bits(code, 10, 11) == 0b00)) { // STUR (SIMD&FP)
                if (bits(code, 30, 31) == 0b00 && !bit(code, 23)) {
                    return SageBuilderAsm::buildTypeU(8);
                } else if (bits(code, 30, 31) == 0b01 && !bit(code, 23)) {
                    return SageBuilderAsm::buildTypeU(16);
                } else if (bits(code, 30, 31) == 0b10 && !bit(code, 23)) {
                    return SageBuilderAsm::buildTypeU(32);
                } else if (bits(code, 30, 31) == 0b11 && !bit(code, 23)) {
                    return SageBuilderAsm::buildTypeU(64);
                } else {
                    ASSERT_require(bits(code, 30, 31) == 0b00);
                    ASSERT_require(bit(code, 23));
                    return SageBuilderAsm::buildTypeU(128);
                }
            } else if (( bit(code, 31) && bits(code, 21, 29) == 0b111000010 && bits(code, 10, 11) == 0b01) || // LDR (immediate) Post-index
                       ( bit(code, 31) && bits(code, 21, 29) == 0b111000010 && bits(code, 10, 11) == 0b11) || // LDR (immediate) Pre-index
                       ( bit(code, 31) && bits(code, 22, 29) == 0b11100101) || // LDR (immediate) unsigned offset
                       (!bit(code, 31) && bits(code, 24, 29) == 0b011000) || // LDR (literal)
                       ( bit(code, 31) && bits(code, 21, 29) == 0b111000011 && bits(code, 10, 11) == 0b10) || // LDR (register)
                       ( bit(code, 31) && bits(code, 21, 29) == 0b111000010 && bits(code, 10, 11) == 0b00) || // LDUR
                       ( bit(code, 31) && bits(code, 21, 29) == 0b111000000 && bits(code, 10, 11) == 0b01) || // STR (immediate) Post-index
                       ( bit(code, 31) && bits(code, 21, 29) == 0b111000000 && bits(code, 10, 11) == 0b11) || // STR (immediate) Pre-index
                       ( bit(code, 31) && bits(code, 22, 29) == 0b11100100) || // STR (immediate) unsigned offset
                       ( bit(code, 31) && bits(code, 21, 29) == 0b111000001 && bits(code, 10, 11) == 0b10) || // STR (register)
                       ( bit(code, 31) && bits(code, 21, 29) == 0b111000000 && bits(code, 10, 11) == 0b00)) { // STUR
                if (bit(code, 30) == 0) {
                    return SageBuilderAsm::buildTypeU(32);
                } else {
                    return SageBuilderAsm::buildTypeU(64);
                }
            } else if (bits(code, 24, 29) == 0b011100) { // LDR (literal, SIMD&FP)
                if (bits(code, 30, 31) == 0b00) {
                    return SageBuilderAsm::buildTypeU(32);
                } else if (bits(code, 30, 31) == 0b01) {
                    return SageBuilderAsm::buildTypeU(64);
                } else {
                    ASSERT_require(bits(code, 30, 31) == 0b10);
                    return SageBuilderAsm::buildTypeU(128);
                }
            } else {
                ASSERT_not_reachable("invalid opcode");
            }
            break;
        }

        //--------------------------------------------------------------------------------------------------------
        default:
            ASSERT_not_reachable("memory read instruction not handled");
    }
}

SgAsmType*
DisassemblerArm::registerType(arm64_reg reg, arm64_vas arrangement) {
    SgAsmType *type = nullptr;
    switch (arrangement) {
        case ARM64_VAS_INVALID:
            type = SageBuilderAsm::buildTypeU(makeRegister(reg).nBits());
            break;
        case ARM64_VAS_8B:
            type = SageBuilderAsm::buildTypeVector(8, SageBuilderAsm::buildTypeU8());
            break;
        case ARM64_VAS_16B:
            type = SageBuilderAsm::buildTypeVector(16, SageBuilderAsm::buildTypeU8());
            break;
        case ARM64_VAS_4H:
            type = SageBuilderAsm::buildTypeVector(4, SageBuilderAsm::buildTypeU16());
            break;
        case ARM64_VAS_8H:
            type = SageBuilderAsm::buildTypeVector(8, SageBuilderAsm::buildTypeU16());
            break;
        case ARM64_VAS_2S:
            type = SageBuilderAsm::buildTypeVector(2, SageBuilderAsm::buildTypeU32());
            break;
        case ARM64_VAS_4S:
            type = SageBuilderAsm::buildTypeVector(4, SageBuilderAsm::buildTypeU32());
            break;
        case ARM64_VAS_1D:
            type = SageBuilderAsm::buildTypeU(64);
            break;
        case ARM64_VAS_2D:
            type = SageBuilderAsm::buildTypeVector(2, SageBuilderAsm::buildTypeU64());
            break;
        case ARM64_VAS_1Q:
            type = SageBuilderAsm::buildTypeU(128);
            break;
    }
    ASSERT_not_null(type);
    return type;
}

RegisterDescriptor
DisassemblerArm::makeRegister(arm64_reg reg) {
    ASSERT_not_null(registerDictionary());
    const RegisterDictionary &dict = *registerDictionary();
    RegisterDescriptor retval;

    switch (reg) {
        case ARM64_REG_INVALID:
        case ARM64_REG_ENDING:
            ASSERT_not_reachable("invalid register from Capstone");
        case ARM64_REG_X29:
            retval = dict.find("x29");
            break;
        case ARM64_REG_X30:
            retval = dict.find("x30");
            break;
        case ARM64_REG_NZCV:
            retval = dict.find("nzcv");
            break;
        case ARM64_REG_SP:
            retval = dict.find("sp");
            break;
        case ARM64_REG_WSP:
            retval = dict.find("wsp");
            break;
        case ARM64_REG_WZR:
            retval = dict.find("wzr");
            break;
        case ARM64_REG_XZR:
            retval = dict.find("xzr");
            break;
        case ARM64_REG_B0:
            retval = dict.find("b0");
            break;
        case ARM64_REG_B1:
            retval = dict.find("b1");
            break;
        case ARM64_REG_B2:
            retval = dict.find("b2");
            break;
        case ARM64_REG_B3:
            retval = dict.find("b3");
            break;
        case ARM64_REG_B4:
            retval = dict.find("b4");
            break;
        case ARM64_REG_B5:
            retval = dict.find("b5");
            break;
        case ARM64_REG_B6:
            retval = dict.find("b6");
            break;
        case ARM64_REG_B7:
            retval = dict.find("b7");
            break;
        case ARM64_REG_B8:
            retval = dict.find("b8");
            break;
        case ARM64_REG_B9:
            retval = dict.find("b9");
            break;
        case ARM64_REG_B10:
            retval = dict.find("b10");
            break;
        case ARM64_REG_B11:
            retval = dict.find("b11");
            break;
        case ARM64_REG_B12:
            retval = dict.find("b12");
            break;
        case ARM64_REG_B13:
            retval = dict.find("b13");
            break;
        case ARM64_REG_B14:
            retval = dict.find("b14");
            break;
        case ARM64_REG_B15:
            retval = dict.find("b15");
            break;
        case ARM64_REG_B16:
            retval = dict.find("b16");
            break;
        case ARM64_REG_B17:
            retval = dict.find("b17");
            break;
        case ARM64_REG_B18:
            retval = dict.find("b18");
            break;
        case ARM64_REG_B19:
            retval = dict.find("b19");
            break;
        case ARM64_REG_B20:
            retval = dict.find("b20");
            break;
        case ARM64_REG_B21:
            retval = dict.find("b21");
            break;
        case ARM64_REG_B22:
            retval = dict.find("b22");
            break;
        case ARM64_REG_B23:
            retval = dict.find("b23");
            break;
        case ARM64_REG_B24:
            retval = dict.find("b24");
            break;
        case ARM64_REG_B25:
            retval = dict.find("b25");
            break;
        case ARM64_REG_B26:
            retval = dict.find("b26");
            break;
        case ARM64_REG_B27:
            retval = dict.find("b27");
            break;
        case ARM64_REG_B28:
            retval = dict.find("b28");
            break;
        case ARM64_REG_B29:
            retval = dict.find("b29");
            break;
        case ARM64_REG_B30:
            retval = dict.find("b30");
            break;
        case ARM64_REG_B31:
            retval = dict.find("b31");
            break;
        case ARM64_REG_D0:
            retval = dict.find("d0");
            break;
        case ARM64_REG_D1:
            retval = dict.find("d1");
            break;
        case ARM64_REG_D2:
            retval = dict.find("d2");
            break;
        case ARM64_REG_D3:
            retval = dict.find("d3");
            break;
        case ARM64_REG_D4:
            retval = dict.find("d4");
            break;
        case ARM64_REG_D5:
            retval = dict.find("d5");
            break;
        case ARM64_REG_D6:
            retval = dict.find("d6");
            break;
        case ARM64_REG_D7:
            retval = dict.find("d7");
            break;
        case ARM64_REG_D8:
            retval = dict.find("d8");
            break;
        case ARM64_REG_D9:
            retval = dict.find("d9");
            break;
        case ARM64_REG_D10:
            retval = dict.find("d10");
            break;
        case ARM64_REG_D11:
            retval = dict.find("d11");
            break;
        case ARM64_REG_D12:
            retval = dict.find("d12");
            break;
        case ARM64_REG_D13:
            retval = dict.find("d13");
            break;
        case ARM64_REG_D14:
            retval = dict.find("d14");
            break;
        case ARM64_REG_D15:
            retval = dict.find("d15");
            break;
        case ARM64_REG_D16:
            retval = dict.find("d16");
            break;
        case ARM64_REG_D17:
            retval = dict.find("d17");
            break;
        case ARM64_REG_D18:
            retval = dict.find("d18");
            break;
        case ARM64_REG_D19:
            retval = dict.find("d19");
            break;
        case ARM64_REG_D20:
            retval = dict.find("d20");
            break;
        case ARM64_REG_D21:
            retval = dict.find("d21");
            break;
        case ARM64_REG_D22:
            retval = dict.find("d22");
            break;
        case ARM64_REG_D23:
            retval = dict.find("d23");
            break;
        case ARM64_REG_D24:
            retval = dict.find("d24");
            break;
        case ARM64_REG_D25:
            retval = dict.find("d25");
            break;
        case ARM64_REG_D26:
            retval = dict.find("d26");
            break;
        case ARM64_REG_D27:
            retval = dict.find("d27");
            break;
        case ARM64_REG_D28:
            retval = dict.find("d28");
            break;
        case ARM64_REG_D29:
            retval = dict.find("d29");
            break;
        case ARM64_REG_D30:
            retval = dict.find("d30");
            break;
        case ARM64_REG_D31:
            retval = dict.find("d31");
            break;
        case ARM64_REG_H0:
            retval = dict.find("h0");
            break;
        case ARM64_REG_H1:
            retval = dict.find("h1");
            break;
        case ARM64_REG_H2:
            retval = dict.find("h2");
            break;
        case ARM64_REG_H3:
            retval = dict.find("h3");
            break;
        case ARM64_REG_H4:
            retval = dict.find("h4");
            break;
        case ARM64_REG_H5:
            retval = dict.find("h5");
            break;
        case ARM64_REG_H6:
            retval = dict.find("h6");
            break;
        case ARM64_REG_H7:
            retval = dict.find("h7");
            break;
        case ARM64_REG_H8:
            retval = dict.find("h8");
            break;
        case ARM64_REG_H9:
            retval = dict.find("h9");
            break;
        case ARM64_REG_H10:
            retval = dict.find("h10");
            break;
        case ARM64_REG_H11:
            retval = dict.find("h11");
            break;
        case ARM64_REG_H12:
            retval = dict.find("h12");
            break;
        case ARM64_REG_H13:
            retval = dict.find("h13");
            break;
        case ARM64_REG_H14:
            retval = dict.find("h14");
            break;
        case ARM64_REG_H15:
            retval = dict.find("h15");
            break;
        case ARM64_REG_H16:
            retval = dict.find("h16");
            break;
        case ARM64_REG_H17:
            retval = dict.find("h17");
            break;
        case ARM64_REG_H18:
            retval = dict.find("h18");
            break;
        case ARM64_REG_H19:
            retval = dict.find("h19");
            break;
        case ARM64_REG_H20:
            retval = dict.find("h20");
            break;
        case ARM64_REG_H21:
            retval = dict.find("h21");
            break;
        case ARM64_REG_H22:
            retval = dict.find("h22");
            break;
        case ARM64_REG_H23:
            retval = dict.find("h23");
            break;
        case ARM64_REG_H24:
            retval = dict.find("h24");
            break;
        case ARM64_REG_H25:
            retval = dict.find("h25");
            break;
        case ARM64_REG_H26:
            retval = dict.find("h26");
            break;
        case ARM64_REG_H27:
            retval = dict.find("h27");
            break;
        case ARM64_REG_H28:
            retval = dict.find("h28");
            break;
        case ARM64_REG_H29:
            retval = dict.find("h29");
            break;
        case ARM64_REG_H30:
            retval = dict.find("h30");
            break;
        case ARM64_REG_H31:
            retval = dict.find("h31");
            break;
        case ARM64_REG_Q0:
            retval = dict.find("q0");
            break;
        case ARM64_REG_Q1:
            retval = dict.find("q1");
            break;
        case ARM64_REG_Q2:
            retval = dict.find("q2");
            break;
        case ARM64_REG_Q3:
            retval = dict.find("q3");
            break;
        case ARM64_REG_Q4:
            retval = dict.find("q4");
            break;
        case ARM64_REG_Q5:
            retval = dict.find("q5");
            break;
        case ARM64_REG_Q6:
            retval = dict.find("q6");
            break;
        case ARM64_REG_Q7:
            retval = dict.find("q7");
            break;
        case ARM64_REG_Q8:
            retval = dict.find("q8");
            break;
        case ARM64_REG_Q9:
            retval = dict.find("q9");
            break;
        case ARM64_REG_Q10:
            retval = dict.find("q10");
            break;
        case ARM64_REG_Q11:
            retval = dict.find("q11");
            break;
        case ARM64_REG_Q12:
            retval = dict.find("q12");
            break;
        case ARM64_REG_Q13:
            retval = dict.find("q13");
            break;
        case ARM64_REG_Q14:
            retval = dict.find("q14");
            break;
        case ARM64_REG_Q15:
            retval = dict.find("q15");
            break;
        case ARM64_REG_Q16:
            retval = dict.find("q16");
            break;
        case ARM64_REG_Q17:
            retval = dict.find("q17");
            break;
        case ARM64_REG_Q18:
            retval = dict.find("q18");
            break;
        case ARM64_REG_Q19:
            retval = dict.find("q19");
            break;
        case ARM64_REG_Q20:
            retval = dict.find("q20");
            break;
        case ARM64_REG_Q21:
            retval = dict.find("q21");
            break;
        case ARM64_REG_Q22:
            retval = dict.find("q22");
            break;
        case ARM64_REG_Q23:
            retval = dict.find("q23");
            break;
        case ARM64_REG_Q24:
            retval = dict.find("q24");
            break;
        case ARM64_REG_Q25:
            retval = dict.find("q25");
            break;
        case ARM64_REG_Q26:
            retval = dict.find("q26");
            break;
        case ARM64_REG_Q27:
            retval = dict.find("q27");
            break;
        case ARM64_REG_Q28:
            retval = dict.find("q28");
            break;
        case ARM64_REG_Q29:
            retval = dict.find("q29");
            break;
        case ARM64_REG_Q30:
            retval = dict.find("q30");
            break;
        case ARM64_REG_Q31:
            retval = dict.find("q31");
            break;
        case ARM64_REG_S0:
            retval = dict.find("s0");
            break;
        case ARM64_REG_S1:
            retval = dict.find("s1");
            break;
        case ARM64_REG_S2:
            retval = dict.find("s2");
            break;
        case ARM64_REG_S3:
            retval = dict.find("s3");
            break;
        case ARM64_REG_S4:
            retval = dict.find("s4");
            break;
        case ARM64_REG_S5:
            retval = dict.find("s5");
            break;
        case ARM64_REG_S6:
            retval = dict.find("s6");
            break;
        case ARM64_REG_S7:
            retval = dict.find("s7");
            break;
        case ARM64_REG_S8:
            retval = dict.find("s8");
            break;
        case ARM64_REG_S9:
            retval = dict.find("s9");
            break;
        case ARM64_REG_S10:
            retval = dict.find("s10");
            break;
        case ARM64_REG_S11:
            retval = dict.find("s11");
            break;
        case ARM64_REG_S12:
            retval = dict.find("s12");
            break;
        case ARM64_REG_S13:
            retval = dict.find("s13");
            break;
        case ARM64_REG_S14:
            retval = dict.find("s14");
            break;
        case ARM64_REG_S15:
            retval = dict.find("s15");
            break;
        case ARM64_REG_S16:
            retval = dict.find("s16");
            break;
        case ARM64_REG_S17:
            retval = dict.find("s17");
            break;
        case ARM64_REG_S18:
            retval = dict.find("s18");
            break;
        case ARM64_REG_S19:
            retval = dict.find("s19");
            break;
        case ARM64_REG_S20:
            retval = dict.find("s20");
            break;
        case ARM64_REG_S21:
            retval = dict.find("s21");
            break;
        case ARM64_REG_S22:
            retval = dict.find("s22");
            break;
        case ARM64_REG_S23:
            retval = dict.find("s23");
            break;
        case ARM64_REG_S24:
            retval = dict.find("s24");
            break;
        case ARM64_REG_S25:
            retval = dict.find("s25");
            break;
        case ARM64_REG_S26:
            retval = dict.find("s26");
            break;
        case ARM64_REG_S27:
            retval = dict.find("s27");
            break;
        case ARM64_REG_S28:
            retval = dict.find("s28");
            break;
        case ARM64_REG_S29:
            retval = dict.find("s29");
            break;
        case ARM64_REG_S30:
            retval = dict.find("s30");
            break;
        case ARM64_REG_S31:
            retval = dict.find("s31");
            break;
        case ARM64_REG_W0:
            retval = dict.find("w0");
            break;
        case ARM64_REG_W1:
            retval = dict.find("w1");
            break;
        case ARM64_REG_W2:
            retval = dict.find("w2");
            break;
        case ARM64_REG_W3:
            retval = dict.find("w3");
            break;
        case ARM64_REG_W4:
            retval = dict.find("w4");
            break;
        case ARM64_REG_W5:
            retval = dict.find("w5");
            break;
        case ARM64_REG_W6:
            retval = dict.find("w6");
            break;
        case ARM64_REG_W7:
            retval = dict.find("w7");
            break;
        case ARM64_REG_W8:
            retval = dict.find("w8");
            break;
        case ARM64_REG_W9:
            retval = dict.find("w9");
            break;
        case ARM64_REG_W10:
            retval = dict.find("w10");
            break;
        case ARM64_REG_W11:
            retval = dict.find("w11");
            break;
        case ARM64_REG_W12:
            retval = dict.find("w12");
            break;
        case ARM64_REG_W13:
            retval = dict.find("w13");
            break;
        case ARM64_REG_W14:
            retval = dict.find("w14");
            break;
        case ARM64_REG_W15:
            retval = dict.find("w15");
            break;
        case ARM64_REG_W16:
            retval = dict.find("w16");
            break;
        case ARM64_REG_W17:
            retval = dict.find("w17");
            break;
        case ARM64_REG_W18:
            retval = dict.find("w18");
            break;
        case ARM64_REG_W19:
            retval = dict.find("w19");
            break;
        case ARM64_REG_W20:
            retval = dict.find("w20");
            break;
        case ARM64_REG_W21:
            retval = dict.find("w21");
            break;
        case ARM64_REG_W22:
            retval = dict.find("w22");
            break;
        case ARM64_REG_W23:
            retval = dict.find("w23");
            break;
        case ARM64_REG_W24:
            retval = dict.find("w24");
            break;
        case ARM64_REG_W25:
            retval = dict.find("w25");
            break;
        case ARM64_REG_W26:
            retval = dict.find("w26");
            break;
        case ARM64_REG_W27:
            retval = dict.find("w27");
            break;
        case ARM64_REG_W28:
            retval = dict.find("w28");
            break;
        case ARM64_REG_W29:
            retval = dict.find("w29");
            break;
        case ARM64_REG_W30:
            retval = dict.find("w30");
            break;
        case ARM64_REG_X0:
            retval = dict.find("x0");
            break;
        case ARM64_REG_X1:
            retval = dict.find("x1");
            break;
        case ARM64_REG_X2:
            retval = dict.find("x2");
            break;
        case ARM64_REG_X3:
            retval = dict.find("x3");
            break;
        case ARM64_REG_X4:
            retval = dict.find("x4");
            break;
        case ARM64_REG_X5:
            retval = dict.find("x5");
            break;
        case ARM64_REG_X6:
            retval = dict.find("x6");
            break;
        case ARM64_REG_X7:
            retval = dict.find("x7");
            break;
        case ARM64_REG_X8:
            retval = dict.find("x8");
            break;
        case ARM64_REG_X9:
            retval = dict.find("x9");
            break;
        case ARM64_REG_X10:
            retval = dict.find("x10");
            break;
        case ARM64_REG_X11:
            retval = dict.find("x11");
            break;
        case ARM64_REG_X12:
            retval = dict.find("x12");
            break;
        case ARM64_REG_X13:
            retval = dict.find("x13");
            break;
        case ARM64_REG_X14:
            retval = dict.find("x14");
            break;
        case ARM64_REG_X15:
            retval = dict.find("x15");
            break;
        case ARM64_REG_X16:
            retval = dict.find("x16");
            break;
        case ARM64_REG_X17:
            retval = dict.find("x17");
            break;
        case ARM64_REG_X18:
            retval = dict.find("x18");
            break;
        case ARM64_REG_X19:
            retval = dict.find("x19");
            break;
        case ARM64_REG_X20:
            retval = dict.find("x20");
            break;
        case ARM64_REG_X21:
            retval = dict.find("x21");
            break;
        case ARM64_REG_X22:
            retval = dict.find("x22");
            break;
        case ARM64_REG_X23:
            retval = dict.find("x23");
            break;
        case ARM64_REG_X24:
            retval = dict.find("x24");
            break;
        case ARM64_REG_X25:
            retval = dict.find("x25");
            break;
        case ARM64_REG_X26:
            retval = dict.find("x26");
            break;
        case ARM64_REG_X27:
            retval = dict.find("x27");
            break;
        case ARM64_REG_X28:
            retval = dict.find("x28");
            break;
        case ARM64_REG_V0:
            retval = dict.find("v0");
            break;
        case ARM64_REG_V1:
            retval = dict.find("v1");
            break;
        case ARM64_REG_V2:
            retval = dict.find("v2");
            break;
        case ARM64_REG_V3:
            retval = dict.find("v3");
            break;
        case ARM64_REG_V4:
            retval = dict.find("v4");
            break;
        case ARM64_REG_V5:
            retval = dict.find("v5");
            break;
        case ARM64_REG_V6:
            retval = dict.find("v6");
            break;
        case ARM64_REG_V7:
            retval = dict.find("v7");
            break;
        case ARM64_REG_V8:
            retval = dict.find("v8");
            break;
        case ARM64_REG_V9:
            retval = dict.find("v9");
            break;
        case ARM64_REG_V10:
            retval = dict.find("v10");
            break;
        case ARM64_REG_V11:
            retval = dict.find("v11");
            break;
        case ARM64_REG_V12:
            retval = dict.find("v12");
            break;
        case ARM64_REG_V13:
            retval = dict.find("v13");
            break;
        case ARM64_REG_V14:
            retval = dict.find("v14");
            break;
        case ARM64_REG_V15:
            retval = dict.find("v15");
            break;
        case ARM64_REG_V16:
            retval = dict.find("v16");
            break;
        case ARM64_REG_V17:
            retval = dict.find("v17");
            break;
        case ARM64_REG_V18:
            retval = dict.find("v18");
            break;
        case ARM64_REG_V19:
            retval = dict.find("v19");
            break;
        case ARM64_REG_V20:
            retval = dict.find("v20");
            break;
        case ARM64_REG_V21:
            retval = dict.find("v21");
            break;
        case ARM64_REG_V22:
            retval = dict.find("v22");
            break;
        case ARM64_REG_V23:
            retval = dict.find("v23");
            break;
        case ARM64_REG_V24:
            retval = dict.find("v24");
            break;
        case ARM64_REG_V25:
            retval = dict.find("v25");
            break;
        case ARM64_REG_V26:
            retval = dict.find("v26");
            break;
        case ARM64_REG_V27:
            retval = dict.find("v27");
            break;
        case ARM64_REG_V28:
            retval = dict.find("v28");
            break;
        case ARM64_REG_V29:
            retval = dict.find("v29");
            break;
        case ARM64_REG_V30:
            retval = dict.find("v30");
            break;
        case ARM64_REG_V31:
            retval = dict.find("v31");
            break;
    }
    ASSERT_require(retval);
    return retval;
}

#if 0
SgAsmArmInstruction *
DisassemblerArm::makeInstructionWithoutOperands(uint32_t address, const std::string& mnemonic, int condPos,
                                                ArmInstructionKind kind, ArmInstructionCondition cond, uint32_t insn)
{
    SgAsmArmInstruction* instruction = new SgAsmArmInstruction(address, mnemonic, kind, cond, condPos);
    ASSERT_not_null(instruction);

    SgAsmOperandList* operands = new SgAsmOperandList();
    instruction->set_operandList(operands);
    operands->set_parent(instruction);

    SgUnsignedCharList bytes(4, '\0');
    for (int i = 0; i < 4; ++i) {
        bytes[i] = (insn >> (8 * i)) & 0xFF; /* Force little-endian */
    }   

    instruction->set_raw_bytes(bytes);

    return instruction;
}

// Creates a general-purpose register reference expression.
SgAsmRegisterReferenceExpression *
DisassemblerArm::makeRegister(uint8_t reg) const
{
    ASSERT_not_null(registerDictionary());
    ASSERT_require(reg<16);
    std::string name = "r" + StringUtility::numberToString(reg);
    const RegisterDescriptor *rdesc = registerDictionary()->lookup(name);
    ASSERT_not_null(rdesc);
    SgAsmRegisterReferenceExpression* r = new SgAsmDirectRegisterExpression(*rdesc);
    return r;
}

// Create a reference to one of the program status registers. If @p useSPSR is true then create a reference to the saved
// program status register, otherwise create a reference to the current program status register. The @p fields is a 4-bit mask
// indicating which individual fields of the register are selected.
//
//  The field bits are
//     0x01 => c   control field mask bit
//     0x02 => x   extension field mask bit
//     0x04 => s   status field mask bit
//     0x08 => f   flags field mask bit
SgAsmRegisterReferenceExpression *
DisassemblerArm::makePsrFields(bool useSPSR, uint8_t fields) const
{
    ASSERT_not_null(registerDictionary());
    std::string name = useSPSR ? "spsr" : "cpsr";
    const RegisterDescriptor *rdesc = registerDictionary()->lookup(name);
    ASSERT_not_null(rdesc);
    SgAsmDirectRegisterExpression *r = new SgAsmDirectRegisterExpression(*rdesc);
    if (fields!=0)
        r->set_psr_mask(fields);
    return r;
}

SgAsmRegisterReferenceExpression *
DisassemblerArm::makePsr(bool useSPSR) const
{
    return makePsrFields(useSPSR, 0);
}


SgAsmExpression *
DisassemblerArm::makeRotatedImmediate() const
{
    uint8_t rsField = (insn >> 8) & 15;
    uint8_t rotateCount = rsField * 2;
    uint32_t immRaw = insn & 0xFF;
    if (rotateCount == 0) {
        return SageBuilderAsm::buildValueU32(immRaw);
    } else {
        return SageBuilderAsm::buildValueU32((immRaw >> rotateCount) | (immRaw << (32 - rotateCount)));
    }
}

SgAsmExpression *
DisassemblerArm::makeShifterField() const
{
    bool i = (insn >> 25) & 1;
    uint8_t rsField = (insn >> 8) & 15;
    uint8_t rmField = insn & 15;
    uint8_t shiftCount = (insn >> 7) & 31;
    uint8_t shiftCountOr32 = shiftCount == 0 ? 32 : shiftCount;
    if (i) {
        return makeRotatedImmediate();
    } else if ((insn & 0xFF0) == 0) {
        return makeRegister(rmField);
    } else if ((insn & 0x070) == 0) {
        return SageBuilderAsm::buildLslExpression(makeRegister(rmField), SageBuilderAsm::buildValueU8(shiftCount));
    } else if ((insn & 0x0F0) == 0x010) {
        return SageBuilderAsm::buildLslExpression(makeRegister(rmField), makeRegister(rsField));
    } else if ((insn & 0x070) == 0x020) {
        return SageBuilderAsm::buildLsrExpression(makeRegister(rmField), SageBuilderAsm::buildValueU8(shiftCountOr32));
    } else if ((insn & 0x0F0) == 0x030) {
        return SageBuilderAsm::buildLsrExpression(makeRegister(rmField), makeRegister(rsField));
    } else if ((insn & 0x070) == 0x040) {
        return SageBuilderAsm::buildAsrExpression(makeRegister(rmField), SageBuilderAsm::buildValueU8(shiftCountOr32));
    } else if ((insn & 0x0F0) == 0x050) {
        return SageBuilderAsm::buildAsrExpression(makeRegister(rmField), makeRegister(rsField));
    } else if ((insn & 0xFF0) == 0x060) {
        return SageBuilderAsm::buildRrxExpression(makeRegister(rmField));
    } else if ((insn & 0x070) == 0x060) {
        return SageBuilderAsm::buildRorExpression(makeRegister(rmField), SageBuilderAsm::buildValueU8(shiftCount));
    } else if ((insn & 0x0F0) == 0x070) {
        return SageBuilderAsm::buildRorExpression(makeRegister(rmField), makeRegister(rsField));
    } else {
        throw ExceptionArm("bad shifter field", this, 25);
    }
}

#define MAKE_INSN0(Mne, CondPos) (makeInstructionWithoutOperands(ip, #Mne, (CondPos), arm_##Mne, cond, insn))
#define MAKE_INSN1(Mne, CondPos, Op1) (SageBuilderAsm::appendOperand(MAKE_INSN0(Mne, CondPos), (Op1)))
#define MAKE_INSN2(Mne, CondPos, Op1, Op2) (SageBuilderAsm::appendOperand(MAKE_INSN1(Mne, CondPos, Op1), (Op2)))
#define MAKE_INSN3(Mne, CondPos, Op1, Op2, Op3) (SageBuilderAsm::appendOperand(MAKE_INSN2(Mne, CondPos, Op1, Op2), (Op3)))
#define MAKE_INSN4(Mne, CondPos, Op1, Op2, Op3, Op4) (SageBuilderAsm::appendOperand(MAKE_INSN3(Mne, CondPos, Op1, Op2, Op3), (Op4)))

SgAsmArmInstruction *
DisassemblerArm::makeDataProcInstruction(uint8_t opcode, bool s, SgAsmExpression* rn, SgAsmExpression* rd,
                                         SgAsmExpression* rhsOperand)
{
    switch ((s ? 16 : 0) | opcode) {
        case 0x00: return MAKE_INSN3(and, 3, rd, rn, rhsOperand);
        case 0x01: return MAKE_INSN3(eor, 3, rd, rn, rhsOperand);
        case 0x02: return MAKE_INSN3(sub, 3, rd, rn, rhsOperand);
        case 0x03: return MAKE_INSN3(rsb, 3, rd, rn, rhsOperand);
        case 0x04: return MAKE_INSN3(add, 3, rd, rn, rhsOperand);
        case 0x05: return MAKE_INSN3(adc, 3, rd, rn, rhsOperand);
        case 0x06: return MAKE_INSN3(sbc, 3, rd, rn, rhsOperand);
        case 0x07: return MAKE_INSN3(rsc, 3, rd, rn, rhsOperand);
        case 0x08: ASSERT_not_reachable("Not a data processing insn");
        case 0x09: ASSERT_not_reachable("Not a data processing insn");
        case 0x0A: ASSERT_not_reachable("Not a data processing insn");
        case 0x0B: ASSERT_not_reachable("Not a data processing insn");
        case 0x0C: return MAKE_INSN3(orr, 3, rd, rn, rhsOperand);
        case 0x0D: return MAKE_INSN2(mov, 3, rd, rhsOperand);
        case 0x0E: return MAKE_INSN3(bic, 3, rd, rn, rhsOperand);
        case 0x0F: return MAKE_INSN2(mvn, 3, rd, rhsOperand);
        case 0x10: return MAKE_INSN3(ands, 3, rd, rn, rhsOperand);
        case 0x11: return MAKE_INSN3(eors, 3, rd, rn, rhsOperand);
        case 0x12: return MAKE_INSN3(subs, 3, rd, rn, rhsOperand);
        case 0x13: return MAKE_INSN3(rsbs, 3, rd, rn, rhsOperand);
        case 0x14: return MAKE_INSN3(adds, 3, rd, rn, rhsOperand);
        case 0x15: return MAKE_INSN3(adcs, 3, rd, rn, rhsOperand);
        case 0x16: return MAKE_INSN3(sbcs, 3, rd, rn, rhsOperand);
        case 0x17: return MAKE_INSN3(rscs, 3, rd, rn, rhsOperand);
        case 0x18: return MAKE_INSN2(tst, 3, rn, rhsOperand);
        case 0x19: return MAKE_INSN2(teq, 3, rn, rhsOperand);
        case 0x1A: return MAKE_INSN2(cmp, 3, rn, rhsOperand);
        case 0x1B: return MAKE_INSN2(cmn, 3, rn, rhsOperand);
        case 0x1C: return MAKE_INSN3(orrs, 3, rd, rn, rhsOperand);
        case 0x1D: return MAKE_INSN2(movs, 3, rd, rhsOperand);
        case 0x1E: return MAKE_INSN3(bics, 3, rd, rn, rhsOperand);
        case 0x1F: return MAKE_INSN2(mvns, 3, rd, rhsOperand);
        default: ASSERT_not_reachable("invalid opcode " + StringUtility::addrToString(opcode));
    }
// DQ (11/29/2009): Avoid MSVC warning.
   return NULL;
}

SgAsmIntegerValueExpression *
DisassemblerArm::makeSplit8bitOffset() const
{
    int32_t val = ((insn >> 4) & 0xF0) | (insn & 0xF);
    val <<= 24;
    val >>= 24; // Arithmetic shift to copy highest bit of immediate
    return SageBuilderAsm::buildValueU32((uint32_t)val);
}

SgAsmIntegerValueExpression *
DisassemblerArm::makeBranchTarget() const
{
    int32_t val = insn & 0xFFFFFF;
    val <<= 8;
    val >>= 6; // Arithmetic shift to copy highest bit of immediate
    uint32_t targetAddr = ip + 8 + val;
    return SageBuilderAsm::buildValueU32(targetAddr);
}

SgAsmExpression *
DisassemblerArm::decodeMemoryAddress(SgAsmExpression* rn) const
{
    bool bit25 = (insn >> 25) & 1;
    bool p = (insn >> 24) & 1;
    bool u = (insn >> 23) & 1;
    bool w = (insn >> 21) & 1;
    SgAsmExpression* offset = bit25 ? makeShifterField() : SageBuilderAsm::buildValueU32(insn & 0xFFFU);
    switch ((p ? 4 : 0) | (u ? 2 : 0) | (w ? 1 : 0)) {
      case 0: return SageBuilderAsm::buildSubtractPostupdateExpression(rn, offset);
      case 1: return SageBuilderAsm::buildSubtractPostupdateExpression(rn, offset); // T suffix
      case 2: return SageBuilderAsm::buildAddPostupdateExpression(rn, offset);
      case 3: return SageBuilderAsm::buildAddPostupdateExpression(rn, offset); // T suffix
      case 4: return SageBuilderAsm::buildSubtractExpression(rn, offset);
      case 5: return SageBuilderAsm::buildSubtractPreupdateExpression(rn, offset);
      case 6: return SageBuilderAsm::buildAddExpression(rn, offset);
      case 7: return SageBuilderAsm::buildAddPreupdateExpression(rn, offset);
      default: ASSERT_not_reachable("invalid memory address specification");
    }
    // DQ (11/29/2009): Avoid MSVC warning.
    return NULL;
}

SgAsmArmInstruction *
DisassemblerArm::decodeMediaInstruction() const
{
    mlog[DEBUG] << "ARM media instructions not supported: " << StringUtility::intToHex(insn) << "\n";
    throw ExceptionArm("media instruction not supported", this);
}

SgAsmArmInstruction *
DisassemblerArm::decodeMultiplyInstruction() const
{
    SgAsmExpression* rn = makeRegister((insn >> 16) & 15);
    SgAsmExpression* rd = makeRegister((insn >> 12) & 15);
    SgAsmExpression* rs = makeRegister((insn >> 8) & 15);
    SgAsmExpression* rm = makeRegister(insn & 15);
    switch ((insn >> 20) & 15) {
        case 0x0: return MAKE_INSN3(mul, 3, rn, rm, rs);
        case 0x1: return MAKE_INSN3(muls, 3, rn, rm, rs);
        case 0x2: return MAKE_INSN4(mla, 3, rn, rm, rs, rd);
        case 0x3: return MAKE_INSN4(mlas, 3, rn, rm, rs, rd);
        case 0x4: return MAKE_INSN4(umaal, 3, rd, rn, rm, rs);
        case 0x5: throw ExceptionArm("bad bits in decodeMultiplyInstruction (0x5)", this, 20);
        case 0x6: throw ExceptionArm("bad bits in decodeMultiplyInstruction (0x6)", this, 20);
        case 0x7: throw ExceptionArm("bad bits in decodeMultiplyInstruction (0x7)", this, 20);
        case 0x8: return MAKE_INSN4(umull, 5, rd, rn, rm, rs);
        case 0x9: return MAKE_INSN4(umulls, 5, rd, rn, rm, rs);
        case 0xA: return MAKE_INSN4(umlal, 5, rd, rn, rm, rs);
        case 0xB: return MAKE_INSN4(umlals, 5, rd, rn, rm, rs);
        case 0xC: return MAKE_INSN4(smull, 5, rd, rn, rm, rs);
        case 0xD: return MAKE_INSN4(smulls, 5, rd, rn, rm, rs);
        case 0xE: return MAKE_INSN4(smlal, 5, rd, rn, rm, rs);
        case 0xF: return MAKE_INSN4(smlals, 5, rd, rn, rm, rs);
        default: ASSERT_not_reachable("invalid multiply instruction");
    }
    // DQ (11/29/2009): Avoid MSVC warning.
    return NULL;
}

SgAsmArmInstruction *
DisassemblerArm::decodeExtraLoadStores() const
{
    bool bit5 = (insn >> 5) & 1;
    bool bit6 = (insn >> 6) & 1;
    bool bit20 = (insn >> 20) & 1;
    bool bit21 = (insn >> 21) & 1;
    bool bit22 = (insn >> 22) & 1;
    bool bit23 = (insn >> 23) & 1;
    bool bit24 = (insn >> 24) & 1;
    SgAsmExpression* rn = makeRegister((insn >> 16) & 15);
    SgAsmExpression* rd = makeRegister((insn >> 12) & 15);
    SgAsmExpression* offset = bit22 ? (SgAsmExpression*)makeSplit8bitOffset() : makeRegister(insn & 15);
    SgAsmExpression* addr = NULL;
    switch ((bit24 ? 4 : 0) | (bit23 ? 2 : 0) | (bit21 ? 1 : 0)) {
        case 0: addr = SageBuilderAsm::buildSubtractPostupdateExpression(rn, offset); break;
        case 1: throw ExceptionArm("bad bits in decodeExtraLoadStores (1)", this, 21);
        case 2: addr = SageBuilderAsm::buildAddPostupdateExpression(rn, offset); break;
        case 3: throw ExceptionArm("bad bits in decodeExtraLoadStores (3)", this, 21);
        case 4: addr = SageBuilderAsm::buildSubtractExpression(rn, offset); break;
        case 5: addr = SageBuilderAsm::buildSubtractPreupdateExpression(rn, offset); break;
        case 6: addr = SageBuilderAsm::buildAddExpression(rn, offset); break;
        case 7: addr = SageBuilderAsm::buildAddPreupdateExpression(rn, offset); break;
        default: ASSERT_not_reachable("invalid extra load stores");
    }
    SgAsmExpression* memref = SageBuilderAsm::buildMemoryReferenceExpression(addr);
    uint8_t lsh = (bit20 ? 4 : 0) | (bit6 ? 2 : 0) | (bit5 ? 1 : 0);
    switch (lsh) {
        case 0:
            /* Should have been handled in multiply code above */
            throw ExceptionArm("bad bits in decodeExtraLoadStores (0)", this, 5);
        case 1: return MAKE_INSN2(strh, 3, rd, memref);
        case 2: return MAKE_INSN2(ldrd, 3, rd, memref);
        case 3: return MAKE_INSN2(strd, 3, rd, memref);
        case 4:
            /* Should have been handled in multiply code above */
            throw ExceptionArm("bad bits in decodeExtraLoadStores (4)", this, 5);
        case 5: return MAKE_INSN2(ldruh, 3, rd, memref);
        case 6: return MAKE_INSN2(ldrsb, 3, rd, memref);
        case 7: return MAKE_INSN2(ldrsh, 3, rd, memref);
        default: ASSERT_not_reachable("invalid lsh value " + StringUtility::numberToString(lsh));
    }
    // DQ (11/29/2009): Avoid MSVC warning.
    return NULL;
}

SgAsmArmInstruction *
DisassemblerArm::decodeMiscInstruction() const
{
    bool bit7 = (insn >> 7) & 1;
    bool bit21 = (insn >> 21) & 1;
    bool bit22 = (insn >> 22) & 1;
    if (!bit7) {
      switch ((insn >> 4) & 7) {
        case 0: {
          if (bit21) {
            SgAsmRegisterReferenceExpression* rm = makeRegister(insn & 15);
            bool useSPSR = bit22;
            uint8_t mask = (insn >> 16) & 15;
            SgAsmRegisterReferenceExpression* psr = makePsrFields(useSPSR, mask);
            return MAKE_INSN2(msr, 3, psr, rm);
          } else {
            bool useSPSR = bit22;
            SgAsmRegisterReferenceExpression* rd = makeRegister((insn >> 12) & 15);
            SgAsmRegisterReferenceExpression* psr = makePsr(useSPSR);
            return MAKE_INSN2(mrs, 3, rd, psr);
          }
        }
        case 1: {
          if (bit22) {
            SgAsmRegisterReferenceExpression* rd = makeRegister((insn >> 12) & 15);
            SgAsmRegisterReferenceExpression* rm = makeRegister(insn & 15);
            return MAKE_INSN2(clz, 3, rd, rm);
          } else {
            return MAKE_INSN1(bx, 2, makeRegister(insn & 15));
          }
        }
        case 2: return MAKE_INSN1(bxj, 3, makeRegister(insn & 15));
        case 3: return MAKE_INSN1(blx, 3, makeRegister(insn & 15));
        case 4: throw ExceptionArm("bad bits in decodeMiscInstruction (4)", this, 4);
        case 5: {
          SgAsmRegisterReferenceExpression* rd = makeRegister((insn >> 12) & 15);
          SgAsmRegisterReferenceExpression* rn = makeRegister((insn >> 16) & 15);
          SgAsmRegisterReferenceExpression* rm = makeRegister(insn & 15);
          uint8_t op = (insn >> 21) & 3;
          switch (op) {
            case 0: return MAKE_INSN3(qadd, 4, rd, rm, rn);
            case 1: return MAKE_INSN3(qsub, 4, rd, rm, rn);
            case 2: return MAKE_INSN3(qdadd, 5, rd, rm, rn);
            case 3: return MAKE_INSN3(qdsub, 5, rd, rm, rn);
            default: ASSERT_not_reachable("invalid op " + StringUtility::numberToString(op));
          }
        }
        case 6: throw ExceptionArm("bad bits in decodeMiscInstruction (6)", this, 4);
        case 7: {
          uint16_t imm1 = (insn >> 8) & 0xFFF;
          uint16_t imm2 = insn & 0xF;
          uint16_t imm = (imm1 << 4) | imm2;
          return MAKE_INSN1(bkpt, 4, SageBuilderAsm::buildValueU16(imm));
        }
        default: ASSERT_not_reachable("invalid miscellaneous instruction");
      }
    } else { // bit 7 set -- signed mul
      SgAsmRegisterReferenceExpression* rd = makeRegister((insn >> 16) & 15);
      SgAsmRegisterReferenceExpression* rn = makeRegister((insn >> 12) & 15);
      SgAsmRegisterReferenceExpression* rs = makeRegister((insn >> 8) & 15);
      SgAsmRegisterReferenceExpression* rm = makeRegister(insn & 15);
      uint8_t op = (insn >> 21) & 3;
      bool y = (insn >> 6) & 1;
      bool x = (insn >> 5) & 1;
      switch ((op << 2) | (x ? 2 : 0) | (y ? 1 : 0)) {
        case 0x0: return MAKE_INSN4(smlabb, 6, rd, rm, rs, rn);
        case 0x1: return MAKE_INSN4(smlabt, 6, rd, rm, rs, rn);
        case 0x2: return MAKE_INSN4(smlatb, 6, rd, rm, rs, rn);
        case 0x3: return MAKE_INSN4(smlatt, 6, rd, rm, rs, rn);
        case 0x4: return MAKE_INSN4(smlawb, 6, rd, rm, rs, rn);
        case 0x5: return MAKE_INSN4(smlawt, 6, rd, rm, rs, rn);
        case 0x6: return MAKE_INSN4(smluwb, 6, rd, rm, rs, rn);
        case 0x7: return MAKE_INSN4(smluwt, 6, rd, rm, rs, rn);
        case 0x8: return MAKE_INSN4(smlalbb, 7, rn, rd, rm, rs);
        case 0x9: return MAKE_INSN4(smlalbt, 7, rn, rd, rm, rs);
        case 0xA: return MAKE_INSN4(smlaltb, 7, rn, rd, rm, rs);
        case 0xB: return MAKE_INSN4(smlaltt, 7, rn, rd, rm, rs);
        case 0xC: return MAKE_INSN3(smulbb, 6, rd, rm, rs);
        case 0xD: return MAKE_INSN3(smulbt, 6, rd, rm, rs);
        case 0xE: return MAKE_INSN3(smultb, 6, rd, rm, rs);
        case 0xF: return MAKE_INSN3(smultt, 6, rd, rm, rs);
        default: ASSERT_not_reachable("invalid miscellaneous instruction op");
      }
    }
    // DQ (11/29/2009): Avoid MSVC warning.
    return NULL;
}

SgAsmArmInstruction *
DisassemblerArm::disassemble()
{
      uint8_t condField = (insn >> 28) & 0xF;
      bool bit4 = (insn >> 4) & 1;
      bool bit7 = (insn >> 7) & 1;
      bool bit9 = (insn >> 9) & 1;
      bool bit16 = (insn >> 16) & 1;
      bool bit20 = (insn >> 20) & 1;
      bool bit21 = (insn >> 21) & 1;
      bool bit22 = (insn >> 22) & 1;
      bool bit24 = (insn >> 24) & 1;
      bool bit25 = (insn >> 25) & 1;
      bool bit4_and_bit7 = bit4 && bit7;
      if (condField != 15 || !decodeUnconditionalInstructions) {
        // Normal instructions (or arm_cond_nv instructions if they are not treated specially)
        cond = (ArmInstructionCondition)(condField + 1);
        uint8_t dataProcOpcode = (insn >> 21) & 15;
        bool dpIsSpecial = (insn & 0x01900000) == 0x01000000;
        switch ((insn >> 26) & 3) {
          case 0: { // Data processing, misc, multiplies, undefined, move imm to status reg
            if ((insn & 0x0F0000F0U) == 0x00000090U) { // Multiplies
              return decodeMultiplyInstruction();
            } else if (bit4_and_bit7 && !bit25) {
              return decodeExtraLoadStores();
            } else if (dpIsSpecial && bit25) {
              if (bit21) {
                SgAsmExpression* imm = makeRotatedImmediate();
                bool useSPSR = bit22;
                uint8_t mask = (insn >> 16) & 15;
                SgAsmRegisterReferenceExpression* psr = makePsrFields(useSPSR, mask);
                return MAKE_INSN2(msr, 3, psr, imm);
              } else {
                  throw ExceptionArm("bad bit21", this, 26);
              }
            } else if (dpIsSpecial && !bit25) {
              return decodeMiscInstruction();
            } else { // !dpIsSpecial && !bit4_and_bit7
              SgAsmExpression* rn = makeRegister((insn >> 16) & 15);
              SgAsmExpression* rd = makeRegister((insn >> 12) & 15);
              SgAsmExpression* shifterField = makeShifterField();
              return makeDataProcInstruction(dataProcOpcode, bit20, rn, rd, shifterField);
            }
          }
          case 1: { // Load-store, media, undefined
            if (!bit4 || !bit25) {
              SgAsmExpression* rn = makeRegister((insn >> 16) & 15);
              SgAsmExpression* memref = SageBuilderAsm::buildMemoryReferenceExpression(decodeMemoryAddress(rn));
              SgAsmExpression* rd = makeRegister((insn >> 12) & 15);
              bool isLoad = bit20;
              bool isByte = bit22;
              bool isTranslated = !bit24 && bit21;
              switch ((isTranslated ? 4 : 0) | (isLoad ? 2 : 0) | (isByte ? 1 : 0)) {
                case 0: return MAKE_INSN2(str, 3, rd, memref);
                case 1: return MAKE_INSN2(strb, 3, rd, memref);
                case 2: return MAKE_INSN2(ldr, 3, rd, memref);
                case 3: return MAKE_INSN2(ldrb, 3, rd, memref);
                case 4: return MAKE_INSN2(strt, 3, rd, memref);
                case 5: return MAKE_INSN2(strbt, 3, rd, memref);
                case 6: return MAKE_INSN2(ldrt, 3, rd, memref);
                case 7: return MAKE_INSN2(ldrbt, 3, rd, memref);
                default: ASSERT_not_reachable("invalid bits");
              }
            } else if ((insn & 0x0FF000F0U) == 0x07F000F0U) {
              return MAKE_INSN0(undefined, 9);
            } else {
              return decodeMediaInstruction();
            }
          }
          case 2: { // Load-store multiple, branches
            if (!bit25) {
              SgAsmExpression* rn = makeRegister((insn >> 16) & 15);
              SgAsmExprListExp* regs = SageBuilderAsm::buildExprListExpression();
              for (int i = 0; i < 16; ++i) {
                if ((insn >> i) & 1) {
                  SgAsmRegisterReferenceExpression* reg = makeRegister(i);
                  regs->get_expressions().push_back(reg);
                  reg->set_parent(regs);
                }
              }
#if 0 // [Robb P Matzke 2015-09-16]: 'base' is set but not used.
              bool bit23 = (insn >> 23) & 1;
              SgAsmExpression* base = rn;
              if (bit21) { // w
                SgAsmExpression* offset = SageBuilderAsm::buildValueU8(regs->get_expressions().size() * 4);
                if (bit23) { // u
                    base = SageBuilderAsm::buildAddPostupdateExpression(rn, offset);
                } else {
                    base = SageBuilderAsm::buildSubtractPostupdateExpression(rn, offset);
                }
              }
#endif

           // DQ (11/29/2009): This is a MSVC warning: warning C4805: '|' : unsafe mix of type 'uint32_t' and type 'bool' in operation
              switch (((insn >> 21) & 62) | bit20) { // p, u, s, l
                case 0x0: return MAKE_INSN2(stmda, 3, rn, regs);
                case 0x1: return MAKE_INSN2(ldmda, 3, rn, regs);
                case 0x2: return MAKE_INSN2(stmda, 3, rn, SageBuilderAsm::buildArmSpecialRegisterList(regs));
                case 0x3: return MAKE_INSN2(ldmda, 3, rn, SageBuilderAsm::buildArmSpecialRegisterList(regs));
                case 0x4: return MAKE_INSN2(stmia, 3, rn, regs);
                case 0x5: return MAKE_INSN2(ldmia, 3, rn, regs);
                case 0x6: return MAKE_INSN2(stmia, 3, rn, SageBuilderAsm::buildArmSpecialRegisterList(regs));
                case 0x7: return MAKE_INSN2(ldmia, 3, rn, SageBuilderAsm::buildArmSpecialRegisterList(regs));
                case 0x8: return MAKE_INSN2(stmdb, 3, rn, regs);
                case 0x9: return MAKE_INSN2(ldmdb, 3, rn, regs);
                case 0xA: return MAKE_INSN2(stmdb, 3, rn, SageBuilderAsm::buildArmSpecialRegisterList(regs));
                case 0xB: return MAKE_INSN2(ldmdb, 3, rn, SageBuilderAsm::buildArmSpecialRegisterList(regs));
                case 0xC: return MAKE_INSN2(stmib, 3, rn, regs);
                case 0xD: return MAKE_INSN2(ldmib, 3, rn, regs);
                case 0xE: return MAKE_INSN2(stmib, 3, rn, SageBuilderAsm::buildArmSpecialRegisterList(regs));
                case 0xF: return MAKE_INSN2(ldmib, 3, rn, SageBuilderAsm::buildArmSpecialRegisterList(regs));
                default: ASSERT_not_reachable("invalid bits");
              }
            } else {
              SgAsmExpression* target = makeBranchTarget();
              if ((insn >> 24) & 1) {
                return MAKE_INSN1(bl, 2, target);
              } else {
                return MAKE_INSN1(b, 1, target);
              }
            }
          }
          case 3: {
            if ((insn & 0x0F000000U) == 0x0F000000U) {
              return MAKE_INSN1(swi, 3, SageBuilderAsm::buildValueU32(insn & 0x00FFFFFFU));
            } else {
                mlog[DEBUG] << "Coprocessor not supported 0x" << StringUtility::intToHex(insn) << "\n";
                throw ExceptionArm("coprocessor not supported", this, 26);
            }
          }
          default: ASSERT_not_reachable("invalid bits");
        }
      } else { // Unconditional instructions
        cond = arm_cond_al;
        uint16_t opcode1 = (insn >> 20) & 0xFF;

        // DQ (8/30/2008): Unused value removed to avoid compiler warning.
        // uint16_t opcode2 = (insn >> 4) & 0xF;

        switch (opcode1) {
          case 0x10: {
            if (bit16) {
              return MAKE_INSN1(setend, 6, SageBuilderAsm::buildValueU8(bit9));
            } else {
                throw ExceptionArm("CPS not supported", this);
            }
          }
          default: {
              mlog[DEBUG] << "Cannot handle too many unconditional instructions: " << StringUtility::intToHex(insn) << "\n";
              throw ExceptionArm("too many unconditional instructions", this, 32);
          }
        }
      }

      ASSERT_not_reachable("fell off end of disassemble");
      // DQ (11/29/2009): Avoid MSVC warning.
      return NULL;
}
#endif

} // namespace
} // namespace

#endif
