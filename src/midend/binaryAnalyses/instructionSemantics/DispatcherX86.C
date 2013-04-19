#include "sage3basic.h"
#include "BaseSemantics2.h"
#include "DispatcherX86.h"
#include "integerOps.h"

namespace BinaryAnalysis {
namespace InstructionSemantics2 {

/*******************************************************************************************************************************
 *                                      Support functions
 *******************************************************************************************************************************/

static inline size_t asm_type_width(SgAsmType* ty) {
  switch (ty->variantT()) {
    case V_SgAsmTypeByte: return 8;
    case V_SgAsmTypeWord: return 16;
    case V_SgAsmTypeDoubleWord: return 32;
    case V_SgAsmTypeQuadWord: return 64;
    default: {std::cerr << "Unhandled type " << ty->class_name() << " in asm_type_width" << std::endl; abort();}
  }
}

// FIXME: Use Register dictionary support
/* Returns the segment register corresponding to the specified register reference address expression. */
static inline X86SegmentRegister getSegregFromMemoryReference(SgAsmMemoryReferenceExpression* mr) {
    X86SegmentRegister segreg = x86_segreg_none;
    SgAsmx86RegisterReferenceExpression* seg = isSgAsmx86RegisterReferenceExpression(mr->get_segment());
    if (seg) {
        ROSE_ASSERT(seg->get_descriptor().get_major() == x86_regclass_segment);
        segreg = (X86SegmentRegister)(seg->get_descriptor().get_minor());
    } else {
        ROSE_ASSERT(!"Bad segment expr");
    }
    if (segreg == x86_segreg_none) segreg = x86_segreg_ds;
    return segreg;
}

/*******************************************************************************************************************************
 *                                      Functors that handle individual x86 instructions kinds
 *******************************************************************************************************************************/

// An intermediate class that reduces the amount of typing in all that follows.  Its process() method does some up-front
// checking, dynamic casting, and pointer dereferencing and then calls the p() method that does the real work.
class P: public BaseSemantics::InsnProcessor {
public:
    typedef DispatcherX86 *D;
    typedef BaseSemantics::RiscOperators *Ops;
    typedef SgAsmx86Instruction *I;
    typedef const SgAsmExpressionPtrList &A;
    virtual void p(D, Ops, I, A) = 0;

    virtual void process(const BaseSemantics::DispatcherPtr &dispatcher_, SgAsmInstruction *insn_) /*override*/ {
        DispatcherX86Ptr dispatcher = DispatcherX86::promote(dispatcher_);
        BaseSemantics::RiscOperatorsPtr operators = dispatcher->get_operators();
        SgAsmx86Instruction *insn = isSgAsmx86Instruction(insn_);
        assert(insn!=NULL && insn==operators->get_insn());
        operators->writeRegister(dispatcher->REG_EIP, operators->add(operators->number_(32, insn->get_address()),
                                                                     operators->number_(32, insn->get_size())));
        SgAsmExpressionPtrList &operands = insn->get_operandList()->get_operands();
        p(dispatcher.get(), operators.get(), insn, operands);
    }

    void assert_args(I insn, A args, size_t nargs) {
        if (args.size()!=nargs) {
            std::string mesg = "instruction must have " + StringUtility::numberToString(nargs) + "argument" + (1==nargs?"":"s");
            throw BaseSemantics::Exception(mesg, insn);
        }
    }
};

// ASCII adjust after addition
struct IP_aaa: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        BaseSemantics::SValuePtr nybble = ops->extract(ops->readRegister(d->REG_AL), 0, 4);
        BaseSemantics::SValuePtr incAh = ops->or_(ops->readRegister(d->REG_AF), d->greaterOrEqualToTen(nybble));
        ops->writeRegister(d->REG_AX, 
                           ops->concat(ops->add(ops->ite(incAh, ops->number_(4, 6), ops->number_(4, 0)),
                                                ops->extract(ops->readRegister(d->REG_AL), 0, 4)),
                                       ops->concat(ops->number_(4, 0),
                                                   ops->add(ops->ite(incAh, ops->number_(8, 1), ops->number_(8, 0)),
                                                            ops->readRegister(d->REG_AH)))));
        ops->writeRegister(d->REG_OF, ops->undefined_(1));
        ops->writeRegister(d->REG_SF, ops->undefined_(1));
        ops->writeRegister(d->REG_ZF, ops->undefined_(1));
        ops->writeRegister(d->REG_PF, ops->undefined_(1));
        ops->writeRegister(d->REG_AF, incAh);
        ops->writeRegister(d->REG_CF, incAh);
    }
};

// ASCII adjust AX before division
struct IP_aad: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        BaseSemantics::SValuePtr al = ops->readRegister(d->REG_AL);
        BaseSemantics::SValuePtr ah = ops->readRegister(d->REG_AH);
        BaseSemantics::SValuePtr divisor = d->read(args[0], 8);
        BaseSemantics::SValuePtr newAl = ops->add(al, ops->extract(ops->unsignedMultiply(ah, divisor), 0, 8));
        ops->writeRegister(d->REG_AX, ops->concat(newAl, ops->number_(8, 0)));
        ops->writeRegister(d->REG_OF, ops->undefined_(1));
        ops->writeRegister(d->REG_AF, ops->undefined_(1));
        ops->writeRegister(d->REG_CF, ops->undefined_(1));
        d->setFlagsForResult(newAl);
    }
};

// ASCII adjust AX after multiply
struct IP_aam: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        BaseSemantics::SValuePtr al = ops->readRegister(d->REG_AL);
        BaseSemantics::SValuePtr divisor = d->read(args[0], 8);
        BaseSemantics::SValuePtr newAh = ops->unsignedDivide(al, divisor);
        BaseSemantics::SValuePtr newAl = ops->unsignedModulo(al, divisor);
        ops->writeRegister(d->REG_AX, ops->concat(newAl, newAh));
        ops->writeRegister(d->REG_OF, ops->undefined_(1));
        ops->writeRegister(d->REG_AF, ops->undefined_(1));
        ops->writeRegister(d->REG_CF, ops->undefined_(1));
        d->setFlagsForResult(newAl);
    }
};

// ASCII adjust AL after subtraction
struct IP_aas: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        BaseSemantics::SValuePtr nybble = ops->extract(ops->readRegister(d->REG_AL), 0, 4);
        BaseSemantics::SValuePtr decAh = ops->or_(ops->readRegister(d->REG_AF), d->greaterOrEqualToTen(nybble));
        ops->writeRegister(d->REG_AX, 
                           ops->concat(ops->add(ops->ite(decAh, ops->number_(4, -6), ops->number_(4, 0)),
                                                ops->extract(ops->readRegister(d->REG_AL), 0, 4)),
                                       ops->concat(ops->number_(4, 0),
                                                   ops->add(ops->ite(decAh, ops->number_(8, -1), ops->number_(8, 0)),
                                                            ops->readRegister(d->REG_AH)))));
        ops->writeRegister(d->REG_OF, ops->undefined_(1));
        ops->writeRegister(d->REG_SF, ops->undefined_(1));
        ops->writeRegister(d->REG_ZF, ops->undefined_(1));
        ops->writeRegister(d->REG_PF, ops->undefined_(1));
        ops->writeRegister(d->REG_AF, decAh);
        ops->writeRegister(d->REG_CF, decAh);
    }
};

// Add with carry
struct IP_adc: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        size_t nbits = asm_type_width(args[0]->get_type());
        BaseSemantics::SValuePtr result = d->doAddOperation(d->read(args[0], nbits), d->read(args[1], nbits),
                                                            false, ops->readRegister(d->REG_CF));
        d->write(args[0], result);
    }
};

// Add two values
struct IP_add: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        size_t nbits = asm_type_width(args[0]->get_type());
        BaseSemantics::SValuePtr result = d->doAddOperation(d->read(args[0], nbits), d->read(args[1], nbits),
                                                            false, ops->boolean_(false));
        d->write(args[0], result);
    }
};

// Bitwise AND
struct IP_and: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        size_t nbits = asm_type_width(args[0]->get_type());
        BaseSemantics::SValuePtr result = ops->and_(d->read(args[0], nbits), d->read(args[1], nbits));
        d->setFlagsForResult(result);
        d->write(args[0], result);
        ops->writeRegister(d->REG_OF, ops->boolean_(false));
        ops->writeRegister(d->REG_AF, ops->undefined_(1));
        ops->writeRegister(d->REG_CF, ops->boolean_(false));
    }
};

// Bit scan forward or reverse
struct IP_bitscan: P {
    const X86InstructionKind kind;
    IP_bitscan(X86InstructionKind k): kind(k) {
        assert(x86_bsf==k || x86_bsr==k);
    }
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        ops->writeRegister(d->REG_OF, ops->undefined_(1));
        ops->writeRegister(d->REG_SF, ops->undefined_(1));
        ops->writeRegister(d->REG_AF, ops->undefined_(1));
        ops->writeRegister(d->REG_PF, ops->undefined_(1));
        ops->writeRegister(d->REG_CF, ops->undefined_(1));
        size_t nbits = asm_type_width(args[0]->get_type());
        BaseSemantics::SValuePtr op = d->read(args[1], nbits);
        BaseSemantics::SValuePtr isZero = ops->equalToZero(op);
        ops->writeRegister(d->REG_ZF, isZero);
        BaseSemantics::SValuePtr bitno;
        assert(insn->get_kind()==kind);
        switch (kind) {
            case x86_bsf: bitno = ops->leastSignificantSetBit(op); break;
            case x86_bsr: bitno = ops->mostSignificantSetBit(op); break;
            default: assert(!"instruction kind not handled"); abort();
        }
        BaseSemantics::SValuePtr result = ops->ite(isZero, d->read(args[0], nbits), bitno);
        d->write(args[0], result);
    }
};

// Bit test instructions: BT, BTR
struct IP_bittest: P {
    const X86InstructionKind kind;
    IP_bittest(X86InstructionKind k): kind(k) {
        assert(x86_bt==k || x86_btr==k || x86_bts==k);
    }
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        assert(insn->get_kind()==kind);
        // All flags except CF are undefined
        ops->writeRegister(d->REG_OF, ops->undefined_(1));
        ops->writeRegister(d->REG_SF, ops->undefined_(1));
        ops->writeRegister(d->REG_ZF, ops->undefined_(1));
        ops->writeRegister(d->REG_AF, ops->undefined_(1));
        ops->writeRegister(d->REG_PF, ops->undefined_(1));
                
        if (isSgAsmMemoryReferenceExpression(args[0]) && isSgAsmx86RegisterReferenceExpression(args[1])) {
            // Special case allowing multi-word offsets into memory
            BaseSemantics::SValuePtr addr = d->readEffectiveAddress(args[0]);
            size_t nbits = asm_type_width(args[1]->get_type());
            BaseSemantics::SValuePtr bitnum = ops->signExtend(d->read(args[1], nbits), 32);
            BaseSemantics::SValuePtr adjustedAddr = ops->add(addr, ops->signExtend(ops->extract(bitnum, 3, 32), 32));
            X86SegmentRegister sr = getSegregFromMemoryReference(isSgAsmMemoryReferenceExpression(args[0]));
            BaseSemantics::SValuePtr val = d->readMemory(sr, adjustedAddr, ops->boolean_(true), 8);
            BaseSemantics::SValuePtr bitval = ops->extract(ops->rotateRight(val, ops->extract(bitnum, 0, 3)), 0, 1);
            ops->writeRegister(d->REG_CF, bitval);
            BaseSemantics::SValuePtr result;
            switch (kind) {
                case x86_btr:
                    result = ops->and_(val, ops->invert(ops->rotateLeft(ops->number_(8, 1), ops->extract(bitnum, 0, 3))));
                    break;
                case x86_bts:
                    result = ops->or_(val, ops->rotateLeft(ops->number_(8, 1), ops->extract(bitnum, 0, 3)));
                    break;
                default:
                    assert(!"instruction kind not handled");
                    abort();
            }
            d->writeMemory(sr, adjustedAddr, result, ops->boolean_(true));
        } else {
            // Simple case
            size_t nbits = asm_type_width(args[0]->get_type());
            BaseSemantics::SValuePtr op0 = d->read(args[0], nbits);
            BaseSemantics::SValuePtr bitnum = ops->extract(d->read(args[1], nbits), 0, 32==nbits?5:4);
            BaseSemantics::SValuePtr bitval = ops->extract(ops->rotateRight(op0, bitnum), 0, 1);
            ops->writeRegister(d->REG_CF, bitval);
            BaseSemantics::SValuePtr result;
            switch (kind) {
                case x86_btr:
                    result = ops->and_(op0, ops->invert(ops->rotateLeft(ops->number_(nbits, 1), bitnum)));
                    break;
                case x86_bts:
                    result = ops->or_(op0, ops->rotateLeft(ops->number_(nbits, 1), bitnum));
                    break;
                default:
                    assert(!"instruction kind not handled");
                    abort();
            }
            d->write(args[0], result);
        }
    }
};

// Byte swap (reverse byte order)
struct IP_bswap: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        size_t nbits = asm_type_width(args[0]->get_type());
        BaseSemantics::SValuePtr op0 = d->read(args[0], nbits);
        BaseSemantics::SValuePtr result = ops->extract(op0, 0, 8);
        for (size_t startbit=8; startbit<nbits; startbit+=8)
            result = ops->concat(ops->extract(op0, startbit, startbit+8), result);
        d->write(args[0], result);
    }
};

// Call procedure
struct IP_call: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        if (insn->get_addressSize() != x86_insnsize_32 || insn->get_operandSize() != x86_insnsize_32)
            throw BaseSemantics::Exception("size not implemented", insn);
        BaseSemantics::SValuePtr oldSp = ops->readRegister(d->REG_ESP);
        BaseSemantics::SValuePtr newSp = ops->add(oldSp, ops->number_(32, -4));
        d->writeMemory(x86_segreg_ss, newSp, ops->readRegister(d->REG_EIP), ops->boolean_(true));
        ops->writeRegister(d->REG_EIP, ops->filterCallTarget(d->read(args[0], 32)));
        ops->writeRegister(d->REG_ESP, newSp);
    }
};

// Sign extend EAX into EDX:EAX
struct IP_cdq: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->writeRegister(d->REG_EDX, ops->extract(ops->signExtend(ops->readRegister(d->REG_EAX), 64), 32, 64));
    }
};

// Sign extend AL to AX
struct IP_cbw: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->writeRegister(d->REG_AX, ops->signExtend(ops->readRegister(d->REG_AL), 16));
    }
};

// Clear carry flag
struct IP_clc: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->writeRegister(d->REG_CF, ops->boolean_(false));
    }
};

// Clear direction flag
struct IP_cld: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->writeRegister(d->REG_DF, ops->boolean_(false));
    }
};

// Complement carry flag
struct IP_cmc: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->writeRegister(d->REG_CF, ops->invert(ops->readRegister(d->REG_CF)));
    }
};

// Conditional move
struct IP_cmovcc: P {
    const X86InstructionKind kind;
    IP_cmovcc(X86InstructionKind k): kind(k) {
        assert(x86_cmovne==k || x86_cmove==k || x86_cmovno==k || x86_cmovo==k || x86_cmovpo==k || x86_cmovpe==k ||
               x86_cmovns==k || x86_cmovs==k || x86_cmovae==k || x86_cmovb==k || x86_cmovbe==k || x86_cmova==k ||
               x86_cmovle==k || x86_cmovg==k || x86_cmovge==k || x86_cmovl==k);
    }
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        assert(insn->get_kind()==kind);
        size_t nbits = asm_type_width(args[0]->get_type());
        assert(16==nbits || 32==nbits);
        BaseSemantics::SValuePtr cond = d->flagsCombo(kind);
        d->write(args[0], ops->ite(cond, d->read(args[1], nbits), d->read(args[0], nbits)));
    }                                                                                                                          \
};
        
// Compare two values
struct IP_cmp: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        size_t nbits = asm_type_width(args[0]->get_type());
        (void) d->doAddOperation(d->read(args[0], nbits), ops->invert(d->read(args[1], nbits)), true, ops->boolean_(false));
    }
};

// Compare strings
struct IP_cmpstrings: P {
    const X86RepeatPrefix repeat;
    const size_t nbits;
    const size_t nbytes;
    IP_cmpstrings(X86RepeatPrefix repeat, size_t nbits): repeat(repeat), nbits(nbits), nbytes(nbits/8) {
        assert(8==nbits || 16==nbits || 32==nbits);
    }
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        if (insn->get_addressSize() != x86_insnsize_32)
            throw BaseSemantics::Exception("size not implemented", insn);
        BaseSemantics::SValuePtr in_loop = d->repEnter(repeat);
        X86SegmentRegister sr = insn->get_segmentOverride()==x86_segreg_none ? x86_segreg_ds : insn->get_segmentOverride();
        BaseSemantics::SValuePtr si_value = d->readMemory(sr, ops->readRegister(d->REG_ESI), in_loop, nbits);
        BaseSemantics::SValuePtr di_value = d->readMemory(x86_segreg_es, ops->readRegister(d->REG_EDI), in_loop, nbits);
        d->doAddOperation(si_value, ops->invert(di_value), true, ops->boolean_(false), in_loop);
        BaseSemantics::SValuePtr step = ops->ite(ops->readRegister(d->REG_DF),
                                                 ops->number_(32, -nbytes), ops->number_(32, nbytes));
        ops->writeRegister(d->REG_ESI,
                           ops->ite(in_loop,
                                    ops->add(ops->readRegister(d->REG_ESI), step),
                                    ops->readRegister(d->REG_ESI)));
        ops->writeRegister(d->REG_EDI,
                           ops->ite(in_loop,
                                    ops->add(ops->readRegister(d->REG_EDI), step),
                                    ops->readRegister(d->REG_EDI)));
        if (x86_repeat_none!=repeat)
            d->repLeave(repeat, in_loop, insn->get_address());
    }
};

// Compare and exchange
struct IP_cmpxchg: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        size_t nbits = asm_type_width(args[0]->get_type());
        BaseSemantics::SValuePtr op0 = d->read(args[0], nbits);
        RegisterDescriptor A = d->REG_EAX;
        A.set_nbits(nbits);
        BaseSemantics::SValuePtr oldA = ops->readRegister(A);
        (void) d->doAddOperation(oldA, ops->invert(op0), true, ops->boolean_(false));
        BaseSemantics::SValuePtr zf = ops->readRegister(d->REG_ZF);
        d->write(args[0], ops->ite(zf, d->read(args[1], nbits), op0));
        ops->writeRegister(A, ops->ite(zf, oldA, op0));
    }
};

// CPU identification
struct IP_cpuid: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->cpuid();
    }
};

// Sign extend AX into DX:AX
struct IP_cwd: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->writeRegister(d->REG_DX, ops->extract(ops->signExtend(ops->readRegister(d->REG_AX), 32), 16, 32));
    }
};

// Sign extend AX to EAX
struct IP_cwde: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->writeRegister(d->REG_EAX, ops->signExtend(ops->readRegister(d->REG_AX), 32)); 
    }
};

// Decrement
struct IP_dec: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        size_t nbits = asm_type_width(args[0]->get_type());
        BaseSemantics::SValuePtr result = d->doIncOperation(d->read(args[0], nbits), true, false);
        d->write(args[0], result);
    }
};

// Halt
struct IP_hlt: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->hlt();
        ops->writeRegister(d->REG_EIP, ops->number_(32, insn->get_address()));
    }
};

// division instructions: IDIV, DIV
struct IP_divide: P {
    const X86InstructionKind kind;
    IP_divide(X86InstructionKind k): kind(k) {
        assert(x86_div==k || x86_idiv==k);
    }
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        size_t nbits = asm_type_width(args[0]->get_type());
        RegisterDescriptor regA = d->REG_AX; regA.set_nbits(8==nbits ? 16 : nbits);
        RegisterDescriptor regD = d->REG_DX; regD.set_nbits(8==nbits ? 16 : nbits);
        BaseSemantics::SValuePtr op0;
        if (8==nbits) {
            ops->readRegister(regA);
        } else {
            ops->concat(ops->readRegister(regA), ops->readRegister(regD));
        }
        BaseSemantics::SValuePtr op1 = d->read(args[0], nbits);
        BaseSemantics::SValuePtr divResult, modResult;
        assert(insn->get_kind()==kind);
        switch (kind) {
            case x86_idiv:
                divResult = ops->signedDivide(op0, op1);
                modResult = ops->signedModulo(op0, op1);
                break;
            case x86_div:
                divResult = ops->unsignedDivide(op0, op1);
                modResult = ops->unsignedModulo(op0, op1);
                break;
            default:
                assert(!"instruction kind not handled");
                abort();
        }
        if (8==nbits) {
            ops->writeRegister(regA, ops->concat(ops->extract(divResult, 0, 8), modResult));
        } else {
            ops->writeRegister(regA, ops->extract(divResult, 0, nbits));
            ops->writeRegister(regD, modResult);
        }
        ops->writeRegister(d->REG_SF, ops->undefined_(1));
        ops->writeRegister(d->REG_ZF, ops->undefined_(1));
        ops->writeRegister(d->REG_AF, ops->undefined_(1));
        ops->writeRegister(d->REG_PF, ops->undefined_(1));
        ops->writeRegister(d->REG_CF, ops->undefined_(1));
        ops->writeRegister(d->REG_OF, ops->undefined_(1));
    }
};

// Restore x86 FPU control word
struct IP_fldcw: P {
    void p(D d, Ops ops, I insn, A args) {
        // FIXME: this is a dummy version that should be replaced later
        assert_args(insn, args, 1);
        (void) d->read(args[0], 16); // To catch access control violations
    }
};

// Store x87 FPU control word
struct IP_fnstcw: P {
    void p(D d, Ops ops, I insn, A args) {
        // FIXME: this is a dummy version that should be replaced later
        assert_args(insn, args, 1);
        d->write(args[0], ops->number_(16, 0x37f));
    }
};

// Signed multiply
struct IP_imul: P {
    void p(D d, Ops ops, I insn, A args) {
        size_t nbits = asm_type_width(args[0]->get_type());
        BaseSemantics::SValuePtr result;
        RegisterDescriptor reg0 = d->REG_AX;
        reg0.set_nbits(nbits);
        if (8==nbits) {
            assert_args(insn, args, 1);
            BaseSemantics::SValuePtr op0 = ops->readRegister(reg0);
            BaseSemantics::SValuePtr op1 = d->read(args[0], nbits);
            result = ops->signedMultiply(op0, op1);
            ops->writeRegister(d->REG_AX, result);
        } else {
            if (args.size()<1 || args.size()>3)
                throw BaseSemantics::Exception("instruction must have 1, 2, or 3 arguments", insn);
            BaseSemantics::SValuePtr op0 = 1==args.size() ? ops->readRegister(reg0) : d->read(args[args.size()-2], nbits);
            BaseSemantics::SValuePtr op1 = d->read(args[args.size()-1], nbits);
            result = ops->signedMultiply(op0, op1);
            if (1==args.size()) {
                RegisterDescriptor reg1 = d->REG_DX;
                reg1.set_nbits(nbits);
                ops->writeRegister(reg0, ops->extract(result, 0, nbits));
                ops->writeRegister(reg1, ops->extract(result, nbits, 2*nbits));
            } else {
                d->write(args[0], ops->extract(result, 0, nbits));
            }
        }
        BaseSemantics::SValuePtr carry = ops->invert(ops->or_(ops->equalToZero(ops->invert(ops->extract(result, 7, nbits))),
                                                              ops->equalToZero(ops->extract(result, 7, nbits))));
        ops->writeRegister(d->REG_CF, carry);
        ops->writeRegister(d->REG_OF, carry);
        ops->writeRegister(d->REG_SF, ops->undefined_(1));
        ops->writeRegister(d->REG_ZF, ops->undefined_(1));
        ops->writeRegister(d->REG_AF, ops->undefined_(1));
        ops->writeRegister(d->REG_PF, ops->undefined_(1));
    }
};

// Increment
struct IP_inc: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        size_t nbits = asm_type_width(args[0]->get_type());
        BaseSemantics::SValuePtr result = d->doIncOperation(d->read(args[0], nbits), false, false);
        d->write(args[0], result);
    }
};

// Call to interrupt procedure
struct IP_int: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        SgAsmByteValueExpression *bv = isSgAsmByteValueExpression(args[0]);
        if (!bv)
            throw BaseSemantics::Exception("operand must be a byte value expression", insn);
        ops->interrupt(bv->get_value());
    }
};

// Jump
struct IP_jmp: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        ops->writeRegister(d->REG_EIP, ops->filterIndirectJumpTarget(d->read(args[0], 32)));
    }
};

// Jump if condition is met
struct IP_jcc: P {
    const X86InstructionKind kind;
    IP_jcc(X86InstructionKind k): kind(k) {
        assert(x86_jne==k || x86_je==k || x86_jno==k || x86_jo==k || x86_jns==k || x86_js==k ||
               x86_jpo==k || x86_jpe==k || x86_jae==k || x86_jb==k || x86_jbe==k || x86_ja==k ||
               x86_jl==k || x86_jge==k || x86_jle==k || x86_jg==k || x86_jcxz==k || x86_jecxz==k);
    }
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        assert(insn->get_kind()==kind);
        BaseSemantics::SValuePtr cond = d->flagsCombo(kind);
        ops->writeRegister(d->REG_EIP,
                           ops->ite(cond, d->read(args[0], 32), ops->readRegister(d->REG_EIP)));
    }
};

// Load effective address
struct IP_lea: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        d->write(args[0], d->readEffectiveAddress(args[1]));
    }
};

// High-level procedure exit
struct IP_leave: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        if (insn->get_addressSize() != x86_insnsize_32 || insn->get_operandSize() != x86_insnsize_32)
            throw BaseSemantics::Exception("size not implemented", insn);
        ops->writeRegister(d->REG_ESP, ops->readRegister(d->REG_EBP));
        BaseSemantics::SValuePtr oldSp = ops->readRegister(d->REG_ESP);
        BaseSemantics::SValuePtr newSp = ops->add(oldSp, ops->number_(32, 4));
        ops->writeRegister(d->REG_EBP, d->readMemory(x86_segreg_ss, oldSp, ops->boolean_(true), 32));
        ops->writeRegister(d->REG_ESP, newSp);
    }
};

// Read time-stamp counter
struct IP_rdtsc: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        BaseSemantics::SValuePtr tsc = ops->rdtsc();
        ops->writeRegister(d->REG_EAX, ops->extract(tsc, 0, 32));
        ops->writeRegister(d->REG_EDX, ops->extract(tsc, 32, 64));
    }
};

// Load string
struct IP_loadstring: P {
    const X86RepeatPrefix repeat;
    const size_t nbits;
    const size_t nbytes;
    IP_loadstring(X86RepeatPrefix repeat, size_t nbits): repeat(repeat), nbits(nbits), nbytes(nbits/8) {
        assert(8==nbits || 16==nbits || 32==nbits);
    }
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        if (insn->get_addressSize() != x86_insnsize_32)
            throw BaseSemantics::Exception("size not implemented", insn);
        RegisterDescriptor regA = d->REG_EAX; regA.set_nbits(nbits);
        BaseSemantics::SValuePtr in_loop = d->repEnter(repeat);
        X86SegmentRegister sr = insn->get_segmentOverride()==x86_segreg_none ? x86_segreg_ds : insn->get_segmentOverride();
        BaseSemantics::SValuePtr value = d->readMemory(sr, ops->readRegister(d->REG_ESI), in_loop, nbits);
        ops->writeRegister(regA, value);
        BaseSemantics::SValuePtr step = ops->ite(ops->readRegister(d->REG_DF),
                                                 ops->number_(32, -nbytes), ops->number_(32, nbytes));
        ops->writeRegister(d->REG_ESI, ops->add(ops->readRegister(d->REG_ESI), step));
    }
};

// Loop according to ECX counter: LOOP, LOOPNZ, LOOPZ
struct IP_loop: P {
    const X86InstructionKind kind;
    IP_loop(X86InstructionKind k): kind(k) {
        assert(x86_loop==k || x86_loopnz==k || x86_loopz==k);
    }
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        if (insn->get_addressSize() != x86_insnsize_32 || insn->get_operandSize() != x86_insnsize_32)
            throw BaseSemantics::Exception("size not implemented", insn);
        BaseSemantics::SValuePtr oldCx = ops->readRegister(d->REG_ECX);
        BaseSemantics::SValuePtr newCx = ops->add(ops->number_(32, -1), oldCx);
        ops->writeRegister(d->REG_ECX, newCx);
        BaseSemantics::SValuePtr doLoop;
        assert(insn->get_kind()==kind);
        switch (kind) {
            case x86_loop:
                doLoop = ops->invert(ops->equalToZero(newCx));
                break;
            case x86_loopnz:
                doLoop = ops->and_(ops->invert(ops->equalToZero(newCx)), ops->invert(ops->readRegister(d->REG_ZF)));
                break;
            case x86_loopz:
                doLoop = ops->and_(ops->invert(ops->equalToZero(newCx)), ops->readRegister(d->REG_ZF));
                break;
            default:
                assert(!"instruction type not handled");
                abort();
        }
        ops->writeRegister(d->REG_EIP, ops->ite(doLoop, d->read(args[0], 32), ops->readRegister(d->REG_EIP)));
    }
};

// Move
struct IP_mov: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        size_t nbits = asm_type_width(args[0]->get_type());
        d->write(args[0], d->read(args[1], nbits));
    }
};

// Move data from string to string
struct IP_movestring: P {
    const X86RepeatPrefix repeat;
    const size_t nbits;
    const size_t nbytes;
    IP_movestring(X86RepeatPrefix repeat, size_t nbits): repeat(repeat), nbits(nbits), nbytes(nbits/8) {
        assert(8==nbits || 16==nbits || 32==nbits);
        assert(x86_repeat_none==repeat || x86_repeat_repe==repeat);
    }
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        if (insn->get_addressSize() != x86_insnsize_32)
            throw BaseSemantics::Exception("size not implemented", insn);
        BaseSemantics::SValuePtr in_loop = d->repEnter(repeat);
        BaseSemantics::SValuePtr value = d->readMemory(x86_segreg_es, ops->readRegister(d->REG_EDI), in_loop, nbits);
        d->writeMemory(x86_segreg_es, ops->readRegister(d->REG_EDI), value, in_loop);
        BaseSemantics::SValuePtr step = ops->ite(ops->readRegister(d->REG_DF),
                                                 ops->number_(32, -nbytes), ops->number_(32, nbytes));
        ops->writeRegister(d->REG_ESI,
                           ops->add(ops->readRegister(d->REG_ESI),
                                    ops->ite(in_loop, step, ops->number_(32, 0))));
        ops->writeRegister(d->REG_EDI,
                           ops->add(ops->readRegister(d->REG_EDI),
                                    ops->ite(in_loop, step, ops->number_(32, 0))));
        if (x86_repeat_none!=repeat)
            d->repLeave(repeat, in_loop, insn->get_address());
    }
};

// Move with sign extend
struct IP_movsx: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        switch (asm_type_width(args[0]->get_type())) {
            case 16: {
                BaseSemantics::SValuePtr op1 = d->read(args[1], 8);
                BaseSemantics::SValuePtr result = ops->signExtend(op1, 16);
                d->write(args[0], result);
                break;
            }
            case 32: {
                size_t width1 = asm_type_width(args[1]->get_type());
                switch (width1) {
                    case 8:
                    case 16: {
                        BaseSemantics::SValuePtr op1 = d->read(args[1], width1);
                        BaseSemantics::SValuePtr result = ops->signExtend(op1, 32);
                        d->write(args[0], result);
                        break;
                    }
                    default:
                        throw BaseSemantics::Exception("size not implemented", insn);
                }
                break;
            }
            default: {
                throw BaseSemantics::Exception("size not implemented", insn);
                break;
            }
        }
    }
};

// Move with zero extend
struct IP_movzx: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        size_t dst_nbits = asm_type_width(args[0]->get_type());
        size_t src_nbits = asm_type_width(args[1]->get_type());
        if (dst_nbits==src_nbits) {
            d->write(args[0], d->read(args[1], src_nbits));
        } else {
            assert(dst_nbits>src_nbits);
            d->write(args[0], ops->concat(d->read(args[1], src_nbits), ops->number_(dst_nbits-src_nbits, 0)));
        }
    }
};

// Unsigned multiply
struct IP_mul: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        size_t nbits = asm_type_width(args[0]->get_type());
        RegisterDescriptor reg0 = d->REG_AX; reg0.set_nbits(nbits);
        RegisterDescriptor reg1 = d->REG_DX; reg1.set_nbits(nbits);
        BaseSemantics::SValuePtr op0 = ops->readRegister(reg0);
        BaseSemantics::SValuePtr op1 = d->read(args[0], nbits);
        BaseSemantics::SValuePtr result = ops->unsignedMultiply(op0, op1);
        if (8==nbits) {
            ops->writeRegister(d->REG_AX, result);
        } else {
            ops->writeRegister(reg0, ops->extract(result, 0, nbits));
            ops->writeRegister(reg1, ops->extract(result, nbits, 2*nbits));
        }
        BaseSemantics::SValuePtr carry = ops->invert(ops->equalToZero(ops->extract(result, nbits, 2*nbits)));
        ops->writeRegister(d->REG_CF, carry);
        ops->writeRegister(d->REG_OF, carry);
        ops->writeRegister(d->REG_SF, ops->undefined_(1));
        ops->writeRegister(d->REG_ZF, ops->undefined_(1));
        ops->writeRegister(d->REG_AF, ops->undefined_(1));
        ops->writeRegister(d->REG_PF, ops->undefined_(1));
    }
};

// Two's complement negation
struct IP_neg: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        size_t nbits = asm_type_width(args[0]->get_type());
        BaseSemantics::SValuePtr result = d->doAddOperation(ops->number_(nbits, 0), ops->invert(d->read(args[0], nbits)),
                                                            true, ops->boolean_(false));
        d->write(args[0], result);
    }
};

// No operation
struct IP_nop: P {
    void p(D d, Ops ops, I insn, A args) {}
};

// Invert bits
struct IP_not: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        size_t nbits = asm_type_width(args[0]->get_type());
        BaseSemantics::SValuePtr result = ops->invert(d->read(args[0], nbits));
        d->write(args[0], result);
    }
};

// Bitwise OR
struct IP_or: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        size_t nbits = asm_type_width(args[0]->get_type());
        BaseSemantics::SValuePtr result = ops->or_(d->read(args[0], nbits), d->read(args[1], nbits));
        d->setFlagsForResult(result);
        d->write(args[0], result);
        ops->writeRegister(d->REG_OF, ops->boolean_(false));
        ops->writeRegister(d->REG_AF, ops->undefined_(1));
        ops->writeRegister(d->REG_CF, ops->boolean_(false));
    }
};

// Pop from stack
struct IP_pop: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        if (insn->get_addressSize() != x86_insnsize_32 || insn->get_operandSize() != x86_insnsize_32)
            throw BaseSemantics::Exception("size not implemented", insn);
        BaseSemantics::SValuePtr oldSp = ops->readRegister(d->REG_ESP);
        BaseSemantics::SValuePtr newSp = ops->add(oldSp, ops->number_(32, 4));
        ops->writeRegister(d->REG_ESP, newSp);
        d->write(args[0], d->readMemory(x86_segreg_ss, oldSp, ops->boolean_(true), 32));
    }
};

// Pop all general-purpose registers
struct IP_popad: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        if (insn->get_addressSize() != x86_insnsize_32)
            throw BaseSemantics::Exception("size not implemented", insn);
        BaseSemantics::SValuePtr oldSp = ops->readRegister(d->REG_ESP);
        BaseSemantics::SValuePtr newSp = ops->add(oldSp, ops->number_(32, 32));
        ops->writeRegister(d->REG_EDI,
                           d->readMemory(x86_segreg_ss, oldSp, ops->boolean_(true), 32));
        ops->writeRegister(d->REG_ESI,
                           d->readMemory(x86_segreg_ss, ops->add(oldSp, ops->number_(32, 4)), ops->boolean_(true), 32));
        ops->writeRegister(d->REG_EBP,
                           d->readMemory(x86_segreg_ss, ops->add(oldSp, ops->number_(32, 8)), ops->boolean_(true), 32));
        ops->writeRegister(d->REG_EBX,
                           d->readMemory(x86_segreg_ss, ops->add(oldSp, ops->number_(32, 16)), ops->boolean_(true), 32));
        ops->writeRegister(d->REG_EDX,
                           d->readMemory(x86_segreg_ss, ops->add(oldSp, ops->number_(32, 20)), ops->boolean_(true), 32));
        ops->writeRegister(d->REG_ECX,
                           d->readMemory(x86_segreg_ss, ops->add(oldSp, ops->number_(32, 24)), ops->boolean_(true), 32));
        ops->writeRegister(d->REG_EAX,
                           d->readMemory(x86_segreg_ss, ops->add(oldSp, ops->number_(32, 28)), ops->boolean_(true), 32));
        (void) d->readMemory(x86_segreg_ss, ops->add(oldSp, ops->number_(32, 12)), ops->boolean_(true), 32);
        ops->writeRegister(d->REG_ESP, newSp);
    }
};

// Push onto stack
struct IP_push: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        if (insn->get_addressSize() != x86_insnsize_32 || insn->get_operandSize() != x86_insnsize_32)
            throw BaseSemantics::Exception("size not implemented", insn);
        BaseSemantics::SValuePtr oldSp = ops->readRegister(d->REG_ESP);
        BaseSemantics::SValuePtr newSp = ops->add(oldSp, ops->number_(32, -4));
        d->writeMemory(x86_segreg_ss, newSp, d->read(args[0], 32), ops->boolean_(true));
        ops->writeRegister(d->REG_ESP, newSp);
    }
};

// Push all general-purpose registers
struct IP_pushad: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        if (insn->get_addressSize() != x86_insnsize_32)
            throw BaseSemantics::Exception("size not implemented", insn);
        BaseSemantics::SValuePtr oldSp = ops->readRegister(d->REG_ESP);
        BaseSemantics::SValuePtr newSp = ops->add(oldSp, ops->number_(32, -32));
        d->writeMemory(x86_segreg_ss, newSp,
                       ops->readRegister(d->REG_EDI), ops->boolean_(true));
        d->writeMemory(x86_segreg_ss, ops->add(newSp, ops->number_(32, 4)),
                       ops->readRegister(d->REG_ESI), ops->boolean_(true));
        d->writeMemory(x86_segreg_ss, ops->add(newSp, ops->number_(32, 8)),
                       ops->readRegister(d->REG_EBP), ops->boolean_(true));
        d->writeMemory(x86_segreg_ss, ops->add(newSp, ops->number_(32, 12)),
                       oldSp, ops->boolean_(true));
        d->writeMemory(x86_segreg_ss, ops->add(newSp, ops->number_(32, 16)),
                       ops->readRegister(d->REG_EBX), ops->boolean_(true));
        d->writeMemory(x86_segreg_ss, ops->add(newSp, ops->number_(32, 20)),
                       ops->readRegister(d->REG_EDX), ops->boolean_(true));
        d->writeMemory(x86_segreg_ss, ops->add(newSp, ops->number_(32, 24)),
                       ops->readRegister(d->REG_ECX), ops->boolean_(true));
        d->writeMemory(x86_segreg_ss, ops->add(newSp, ops->number_(32, 28)),
                       ops->readRegister(d->REG_EAX), ops->boolean_(true));
        ops->writeRegister(d->REG_ESP, newSp);
    }
};

// Push EFLAGS onto the stack
struct IP_pushfd: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        if (insn->get_addressSize() != x86_insnsize_32)
            throw BaseSemantics::Exception("size not implemented", insn);
        BaseSemantics::SValuePtr oldSp = ops->readRegister(d->REG_ESP);
        BaseSemantics::SValuePtr newSp = ops->add(oldSp, ops->number_(32, -4));
        d->writeMemory(x86_segreg_ss, newSp, ops->readRegister(d->REG_EFLAGS), ops->boolean_(true));
        ops->writeRegister(d->REG_ESP, newSp);
    }
};

// Return from procedure
struct IP_ret: P {
    void p(D d, Ops ops, I insn, A args) {
        if (args.size()>1)
            throw BaseSemantics::Exception("instruction must have zero or one operand", insn);
        if (insn->get_addressSize() != x86_insnsize_32 || insn->get_operandSize() != x86_insnsize_32)
            throw BaseSemantics::Exception("size not implemented", insn);
        BaseSemantics::SValuePtr extraBytes = (args.size()==1 ? d->read(args[0], 32) : ops->number_(32, 0));
        BaseSemantics::SValuePtr oldSp = ops->readRegister(d->REG_ESP);
        BaseSemantics::SValuePtr newSp = ops->add(oldSp, ops->add(ops->number_(32, 4), extraBytes));
        ops->writeRegister(d->REG_EIP, ops->filterReturnTarget(d->readMemory(x86_segreg_ss, oldSp, ops->boolean_(true), 32)));
        ops->writeRegister(d->REG_ESP, newSp);
    }
};

// Rotate left
struct IP_rol: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        size_t nbits = asm_type_width(args[0]->get_type());
        BaseSemantics::SValuePtr op = d->read(args[0], nbits);
        BaseSemantics::SValuePtr shiftCount = ops->extract(d->read(args[1], 8), 0, 5);
        BaseSemantics::SValuePtr result = ops->rotateLeft(op, shiftCount);
        ops->writeRegister(d->REG_CF, ops->ite(ops->equalToZero(shiftCount),
                                               ops->readRegister(d->REG_CF),
                                               ops->extract(result, 0, 1)));
        ops->writeRegister(d->REG_OF, ops->ite(ops->equalToZero(shiftCount),
                                               ops->readRegister(d->REG_OF),
                                               ops->xor_(ops->extract(result, 0, 1),
                                                         ops->extract(result, nbits-1, nbits))));
        d->write(args[0], result);
    }
};

// Rotate right
struct IP_ror: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        size_t nbits = asm_type_width(args[0]->get_type());
        BaseSemantics::SValuePtr op = d->read(args[0], nbits);
        BaseSemantics::SValuePtr shiftCount = ops->extract(d->read(args[1], 8), 0, 5);
        BaseSemantics::SValuePtr result = ops->rotateRight(op, shiftCount);
        ops->writeRegister(d->REG_CF, ops->ite(ops->equalToZero(shiftCount),
                                               ops->readRegister(d->REG_CF),
                                               ops->extract(result, nbits-1, nbits)));
        ops->writeRegister(d->REG_OF, ops->ite(ops->equalToZero(shiftCount),
                                               ops->readRegister(d->REG_OF),
                                               ops->xor_(ops->extract(result, nbits-2, nbits-1),
                                                         ops->extract(result, nbits-1, nbits))));
        d->write(args[0], result);
    }
};

// Integer subtraction with borrow
struct IP_sbb: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        size_t nbits = asm_type_width(args[0]->get_type());
        BaseSemantics::SValuePtr result = d->doAddOperation(d->read(args[0], nbits), ops->invert(d->read(args[1], nbits)),
                                                            true, ops->readRegister(d->REG_CF));
        d->write(args[0], result);
    }
};

// Scan string
struct IP_scanstring: P {
    const X86RepeatPrefix repeat;
    const size_t nbits;
    const size_t nbytes;
    IP_scanstring(X86RepeatPrefix repeat, size_t nbits): repeat(repeat), nbits(nbits), nbytes(nbits/8) {
        assert(8==nbits || 16==nbits || 32==nbits);
    }
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        if (insn->get_addressSize() != x86_insnsize_32)
            throw BaseSemantics::Exception("size not implemented", insn);
        BaseSemantics::SValuePtr in_loop = d->repEnter(repeat);
        RegisterDescriptor regA = d->REG_EAX; regA.set_nbits(nbits);
        BaseSemantics::SValuePtr value = d->readMemory(x86_segreg_es, ops->readRegister(d->REG_EDI), in_loop, nbits);
        d->doAddOperation(ops->readRegister(regA), ops->invert(value), true, ops->boolean_(false), in_loop);
        BaseSemantics::SValuePtr step = ops->ite(ops->readRegister(d->REG_DF),
                                                 ops->number_(32, -nbytes), ops->number_(32, nbytes));
        ops->writeRegister(d->REG_EDI, 
                           ops->ite(in_loop,
                                    ops->add(ops->readRegister(d->REG_EDI), step),
                                    ops->readRegister(d->REG_EDI)));
        if (x86_repeat_none!=repeat)
            d->repLeave(repeat, in_loop, insn->get_address());
    }
};

// Set byte on condition
struct IP_setcc: P {
    const X86InstructionKind kind;
    IP_setcc(X86InstructionKind k): kind(k) {
        assert(x86_setne==k || x86_sete==k || x86_setno==k || x86_seto==k || x86_setpo==k || x86_setpe==k ||
               x86_setns==k || x86_sets==k || x86_setae==k || x86_setb==k || x86_setbe==k || x86_seta==k ||
               x86_setle==k || x86_setg==k || x86_setge==k || x86_setl==k);
    }
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        assert(insn->get_kind()==kind);
        BaseSemantics::SValuePtr cond = d->flagsCombo(kind);
        d->write(args[0], ops->concat(cond, ops->number_(7, 0)));
    }
};

// Shift instructions: SHL, SAR, and SHR
struct IP_shift: P {
    const X86InstructionKind kind;
    IP_shift(X86InstructionKind k): kind(k) {
        assert(x86_shr==k || x86_sar==k || x86_shl==k || x86_shld==k || x86_shrd==k);
    }
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        assert(insn->get_kind()==kind);
        size_t nbits = asm_type_width(args[0]->get_type());
        size_t shiftSignificantBits = nbits <= 32 ? 5 : 7;
        BaseSemantics::SValuePtr result = d->doShiftOperation(insn->get_kind(), d->read(args[0], nbits),
                                                              ops->undefined_(nbits), d->read(args[1], 8),
                                                              shiftSignificantBits);
        d->write(args[0], result);
    }
};

// Set carry flag
struct IP_stc: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->writeRegister(d->REG_CF, ops->boolean_(true));
    }
};

// Set direction flag
struct IP_std: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->writeRegister(d->REG_DF, ops->boolean_(true));
    }
};

// Store string
struct IP_storestring: P {
    const X86RepeatPrefix repeat;
    const size_t nbits;
    const size_t nbytes;
    IP_storestring(X86RepeatPrefix repeat, size_t nbits): repeat(repeat), nbits(nbits), nbytes(nbits/8) {
        assert(8==nbits || 16==nbits || 32==nbits);
    }
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        if (insn->get_addressSize()!=x86_insnsize_32)
            throw BaseSemantics::Exception("address size must be 32 bits", insn);
        BaseSemantics::SValuePtr in_loop = d->repEnter(repeat);
        RegisterDescriptor regA = d->REG_EAX; regA.set_nbits(nbits);
        d->writeMemory(x86_segreg_es, ops->readRegister(d->REG_EDI), ops->readRegister(regA), in_loop);
        BaseSemantics::SValuePtr step = ops->ite(ops->readRegister(d->REG_DF),
                                                 ops->number_(32, -nbytes), ops->number_(32, nbytes));

        /* Update DI */
        ops->writeRegister(d->REG_EDI,
                           ops->ite(in_loop,
                                    ops->add(ops->readRegister(d->REG_EDI), step),
                                    ops->readRegister(d->REG_EDI)));
        if (x86_repeat_none!=repeat)
            d->repLeave(repeat, in_loop, insn->get_address());
    }
};

// Subtract two values
struct IP_sub: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        size_t nbits = asm_type_width(args[0]->get_type());
        BaseSemantics::SValuePtr result = d->doAddOperation(d->read(args[0], nbits),
                                                            ops->invert(d->read(args[1], nbits)),
                                                            true, ops->boolean_(false));
        d->write(args[0], result);
    }
};

// Fast system call
struct IP_sysenter: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->sysenter();
    }
};

// Same as x86_and except doesn't write result back to machine state
struct IP_test: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        size_t nbits = asm_type_width(args[0]->get_type());
        BaseSemantics::SValuePtr result = ops->and_(d->read(args[0], nbits), d->read(args[1], nbits));
        d->setFlagsForResult(result);
        ops->writeRegister(d->REG_OF, ops->boolean_(false));
        ops->writeRegister(d->REG_AF, ops->undefined_(1));
        ops->writeRegister(d->REG_CF, ops->boolean_(false));
    }
};

// Exchange and add
struct IP_xadd: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        size_t nbits = asm_type_width(args[0]->get_type());
        BaseSemantics::SValuePtr result = d->doAddOperation(d->read(args[0], nbits), d->read(args[1], nbits),
                                                            false, ops->boolean_(false));
        d->write(args[1], d->read(args[0], nbits));
        d->write(args[0], result);
    }
};

// Exchange
struct IP_xchg: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        size_t nbits = asm_type_width(args[0]->get_type());
        BaseSemantics::SValuePtr temp = d->read(args[1], nbits);
        d->write(args[1], d->read(args[0], nbits));
        d->write(args[0], temp);
    }
};

// Bitwise XOR
struct IP_xor: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        size_t nbits = asm_type_width(args[0]->get_type());
        BaseSemantics::SValuePtr result = ops->xor_(d->read(args[0], nbits), d->read(args[1], nbits));
        d->setFlagsForResult(result);
        d->write(args[0], result);
        ops->writeRegister(d->REG_OF, ops->boolean_(false));
        ops->writeRegister(d->REG_AF, ops->undefined_(1));
        ops->writeRegister(d->REG_CF, ops->boolean_(false));
    }
};

/*******************************************************************************************************************************
 *                                      DispatcherX86
 *******************************************************************************************************************************/

void
DispatcherX86::iproc_init()
{
    iproc_set(x86_aaa,          new IP_aaa);
    iproc_set(x86_aad,          new IP_aad);
    iproc_set(x86_aam,          new IP_aam);
    iproc_set(x86_aas,          new IP_aas);
    iproc_set(x86_adc,          new IP_adc);
    iproc_set(x86_add,          new IP_add);
    iproc_set(x86_and,          new IP_and);
    iproc_set(x86_bsf,          new IP_bitscan(x86_bsf));
    iproc_set(x86_bsr,          new IP_bitscan(x86_bsr));
    iproc_set(x86_bswap,        new IP_bswap);
    iproc_set(x86_bt,           new IP_bittest(x86_bt));
    iproc_set(x86_btr,          new IP_bittest(x86_btr));
    iproc_set(x86_bts,          new IP_bittest(x86_bts));
    iproc_set(x86_call,         new IP_call);
    iproc_set(x86_cbw,          new IP_cbw);
    iproc_set(x86_cdq,          new IP_cdq);
    iproc_set(x86_clc,          new IP_clc);
    iproc_set(x86_cld,          new IP_cld);
    iproc_set(x86_cmc,          new IP_cmc);
    iproc_set(x86_cmova,        new IP_cmovcc(x86_cmova));
    iproc_set(x86_cmovae,       new IP_cmovcc(x86_cmovae));
    iproc_set(x86_cmovb,        new IP_cmovcc(x86_cmovb));
    iproc_set(x86_cmovbe,       new IP_cmovcc(x86_cmovbe));
    iproc_set(x86_cmove,        new IP_cmovcc(x86_cmove));
    iproc_set(x86_cmovg,        new IP_cmovcc(x86_cmovg));
    iproc_set(x86_cmovge,       new IP_cmovcc(x86_cmovge));
    iproc_set(x86_cmovl,        new IP_cmovcc(x86_cmovl));
    iproc_set(x86_cmovle,       new IP_cmovcc(x86_cmovle));
    iproc_set(x86_cmovne,       new IP_cmovcc(x86_cmovne));
    iproc_set(x86_cmovno,       new IP_cmovcc(x86_cmovno));
    iproc_set(x86_cmovns,       new IP_cmovcc(x86_cmovns));
    iproc_set(x86_cmovo,        new IP_cmovcc(x86_cmovo));
    iproc_set(x86_cmovpe,       new IP_cmovcc(x86_cmovpe));
    iproc_set(x86_cmovpo,       new IP_cmovcc(x86_cmovpo));
    iproc_set(x86_cmovs,        new IP_cmovcc(x86_cmovs));
    iproc_set(x86_cmp,          new IP_cmp);
    iproc_set(x86_cmpsb,        new IP_cmpstrings(x86_repeat_none, 8));
    iproc_set(x86_cmpsw,        new IP_cmpstrings(x86_repeat_none, 16));
    iproc_set(x86_cmpsd,        new IP_cmpstrings(x86_repeat_none, 32)); // FIXME: also a floating point instruction
    iproc_set(x86_cmpxchg,      new IP_cmpxchg);
    iproc_set(x86_cpuid,        new IP_cpuid);
    iproc_set(x86_cwd,          new IP_cwd);
    iproc_set(x86_cwde,         new IP_cwde);
    iproc_set(x86_dec,          new IP_dec);
    iproc_set(x86_div,          new IP_divide(x86_div));
    iproc_set(x86_fldcw,        new IP_fldcw);
    iproc_set(x86_fnstcw,       new IP_fnstcw);
    iproc_set(x86_hlt,          new IP_hlt);
    iproc_set(x86_idiv,         new IP_divide(x86_idiv));
    iproc_set(x86_imul,         new IP_imul);
    iproc_set(x86_inc,          new IP_inc);
    iproc_set(x86_int,          new IP_int);
    iproc_set(x86_ja,           new IP_jcc(x86_ja));
    iproc_set(x86_jae,          new IP_jcc(x86_jae));
    iproc_set(x86_jb,           new IP_jcc(x86_jb));
    iproc_set(x86_jbe,          new IP_jcc(x86_jbe));
    iproc_set(x86_jcxz,         new IP_jcc(x86_jcxz));
    iproc_set(x86_je,           new IP_jcc(x86_je));
    iproc_set(x86_jecxz,        new IP_jcc(x86_jecxz));
    iproc_set(x86_jg,           new IP_jcc(x86_jg));
    iproc_set(x86_jge,          new IP_jcc(x86_jge));
    iproc_set(x86_jl,           new IP_jcc(x86_jl));
    iproc_set(x86_jle,          new IP_jcc(x86_jle));
    iproc_set(x86_jmp,          new IP_jmp);
    iproc_set(x86_jne,          new IP_jcc(x86_jne));
    iproc_set(x86_jno,          new IP_jcc(x86_jno));
    iproc_set(x86_jns,          new IP_jcc(x86_jns));
    iproc_set(x86_jo,           new IP_jcc(x86_jo));
    iproc_set(x86_jpe,          new IP_jcc(x86_jpe));
    iproc_set(x86_jpo,          new IP_jcc(x86_jpo));
    iproc_set(x86_js,           new IP_jcc(x86_js));
    iproc_set(x86_lea,          new IP_lea);
    iproc_set(x86_leave,        new IP_leave);
    iproc_set(x86_lodsb,        new IP_loadstring(x86_repeat_none, 8));
    iproc_set(x86_lodsw,        new IP_loadstring(x86_repeat_none, 16));
    iproc_set(x86_lodsd,        new IP_loadstring(x86_repeat_none, 32));
    iproc_set(x86_loop,         new IP_loop(x86_loop));
    iproc_set(x86_loopnz,       new IP_loop(x86_loopnz));
    iproc_set(x86_loopz,        new IP_loop(x86_loopz));
    iproc_set(x86_mov,          new IP_mov);
    iproc_set(x86_movsb,        new IP_movestring(x86_repeat_none, 8));
    iproc_set(x86_movsw,        new IP_movestring(x86_repeat_none, 16));
    iproc_set(x86_movsd,        new IP_movestring(x86_repeat_none, 32));
    iproc_set(x86_movsx,        new IP_movsx);
    iproc_set(x86_movzx,        new IP_movzx);
    iproc_set(x86_mul,          new IP_mul);
    iproc_set(x86_neg,          new IP_neg);
    iproc_set(x86_nop,          new IP_nop);
    iproc_set(x86_not,          new IP_not);
    iproc_set(x86_or,           new IP_or);
    iproc_set(x86_pop,          new IP_pop);
    iproc_set(x86_popad,        new IP_popad);
    iproc_set(x86_push,         new IP_push);
    iproc_set(x86_pushad,       new IP_pushad);
    iproc_set(x86_pushfd,       new IP_pushfd);
    iproc_set(x86_rdtsc,        new IP_rdtsc);
    iproc_set(x86_rep_lodsb,    new IP_loadstring(x86_repeat_repe, 8));
    iproc_set(x86_rep_lodsw,    new IP_loadstring(x86_repeat_repe, 16));
    iproc_set(x86_rep_lodsd,    new IP_loadstring(x86_repeat_repe, 32));
    iproc_set(x86_rep_movsb,    new IP_movestring(x86_repeat_repe, 8));
    iproc_set(x86_rep_movsw,    new IP_movestring(x86_repeat_repe, 16));
    iproc_set(x86_rep_movsd,    new IP_movestring(x86_repeat_repe, 32));
    iproc_set(x86_rep_stosb,    new IP_storestring(x86_repeat_repe, 8));
    iproc_set(x86_rep_stosw,    new IP_storestring(x86_repeat_repe, 16));
    iproc_set(x86_rep_stosd,    new IP_storestring(x86_repeat_repe, 32));
    iproc_set(x86_repe_cmpsb,   new IP_cmpstrings(x86_repeat_repe, 8));
    iproc_set(x86_repe_cmpsw,   new IP_cmpstrings(x86_repeat_repe, 16));
    iproc_set(x86_repe_cmpsd,   new IP_cmpstrings(x86_repeat_repe, 32));
    iproc_set(x86_repe_scasb,   new IP_scanstring(x86_repeat_repe, 8));
    iproc_set(x86_repe_scasw,   new IP_scanstring(x86_repeat_repe, 16));
    iproc_set(x86_repe_scasd,   new IP_scanstring(x86_repeat_repe, 32));
    iproc_set(x86_repne_cmpsb,  new IP_cmpstrings(x86_repeat_repne, 8));
    iproc_set(x86_repne_cmpsw,  new IP_cmpstrings(x86_repeat_repne, 16));
    iproc_set(x86_repne_cmpsd,  new IP_cmpstrings(x86_repeat_repne, 32));
    iproc_set(x86_repne_scasb,  new IP_scanstring(x86_repeat_repne, 8));
    iproc_set(x86_repne_scasw,  new IP_scanstring(x86_repeat_repne, 16));
    iproc_set(x86_repne_scasd,  new IP_scanstring(x86_repeat_repne, 32));
    iproc_set(x86_ret,          new IP_ret);
    iproc_set(x86_rol,          new IP_rol);
    iproc_set(x86_ror,          new IP_ror);
    iproc_set(x86_sar,          new IP_shift(x86_sar));
    iproc_set(x86_sbb,          new IP_sbb);
    iproc_set(x86_scasb,        new IP_scanstring(x86_repeat_none, 8));
    iproc_set(x86_scasw,        new IP_scanstring(x86_repeat_none, 16));
    iproc_set(x86_scasd,        new IP_scanstring(x86_repeat_none, 32));
    iproc_set(x86_seta,         new IP_setcc(x86_seta));
    iproc_set(x86_setae,        new IP_setcc(x86_setae));
    iproc_set(x86_setb,         new IP_setcc(x86_setb));
    iproc_set(x86_setbe,        new IP_setcc(x86_setbe));
    iproc_set(x86_sete,         new IP_setcc(x86_sete));
    iproc_set(x86_setg,         new IP_setcc(x86_setg));
    iproc_set(x86_setge,        new IP_setcc(x86_setge));
    iproc_set(x86_setl,         new IP_setcc(x86_setl));
    iproc_set(x86_setle,        new IP_setcc(x86_setle));
    iproc_set(x86_setne,        new IP_setcc(x86_setne));
    iproc_set(x86_setno,        new IP_setcc(x86_setno));
    iproc_set(x86_setns,        new IP_setcc(x86_setns));
    iproc_set(x86_seto,         new IP_setcc(x86_seto));
    iproc_set(x86_setpe,        new IP_setcc(x86_setpe));
    iproc_set(x86_setpo,        new IP_setcc(x86_setpo));
    iproc_set(x86_sets,         new IP_setcc(x86_sets));
    iproc_set(x86_shl,          new IP_shift(x86_shl));
    iproc_set(x86_shld,         new IP_shift(x86_shld));
    iproc_set(x86_shr,          new IP_shift(x86_shr));
    iproc_set(x86_shrd,         new IP_shift(x86_shrd));
    iproc_set(x86_stc,          new IP_stc);
    iproc_set(x86_std,          new IP_std);
    iproc_set(x86_stosb,        new IP_storestring(x86_repeat_none, 8));
    iproc_set(x86_stosw,        new IP_storestring(x86_repeat_none, 16));
    iproc_set(x86_stosd,        new IP_storestring(x86_repeat_none, 32));
    iproc_set(x86_sub,          new IP_sub);
    iproc_set(x86_sysenter,     new IP_sysenter);
    iproc_set(x86_test,         new IP_test);
    iproc_set(x86_xadd,         new IP_xadd);
    iproc_set(x86_xchg,         new IP_xchg);
    iproc_set(x86_xor,          new IP_xor);
}

void
DispatcherX86::regcache_init()
{
    if (regdict) {
        REG_EAX = findRegister("eax", 32);
        REG_EBX = findRegister("ebx", 32);
        REG_ECX = findRegister("ecx", 32);
        REG_EDX = findRegister("edx", 32);
        REG_EDI = findRegister("edi", 32);
        REG_EIP = findRegister("eip", 32);
        REG_ESI = findRegister("esi", 32);
        REG_ESP = findRegister("esp", 32);
        REG_EBP = findRegister("ebp", 32);

        REG_AX  = findRegister("ax", 16);
        REG_CX  = findRegister("cx", 16);
        REG_DX  = findRegister("dx", 16);

        REG_AL  = findRegister("al", 8);
        REG_AH  = findRegister("ah", 8);

        REG_EFLAGS= findRegister("eflags", 32);
        REG_AF  = findRegister("af", 1);
        REG_CF  = findRegister("cf", 1);
        REG_DF  = findRegister("df", 1);
        REG_OF  = findRegister("of", 1);
        REG_PF  = findRegister("pf", 1);
        REG_SF  = findRegister("sf", 1);
        REG_ZF  = findRegister("zf", 1);
    }
}

void
DispatcherX86::set_register_dictionary(const RegisterDictionary *regdict)
{
    BaseSemantics::Dispatcher::set_register_dictionary(regdict);
    regcache_init();
}

void
DispatcherX86::write(SgAsmExpression *e, const BaseSemantics::SValuePtr &value)
{
    assert(e!=NULL && value!=NULL);
    switch (e->variantT()) {
        case V_SgAsmx86RegisterReferenceExpression: {
            SgAsmx86RegisterReferenceExpression* rre = isSgAsmx86RegisterReferenceExpression(e);
            operators->writeRegister(rre->get_descriptor(), value);
            break;
        }
        case V_SgAsmMemoryReferenceExpression: {
            writeMemory(getSegregFromMemoryReference(isSgAsmMemoryReferenceExpression(e)),
                        readEffectiveAddress(e), value, operators->boolean_(true));
            break;
        }
        default: {
            assert(!"not implemented");
            abort();
        }
    }
}

BaseSemantics::SValuePtr
DispatcherX86::read(SgAsmExpression *e, size_t nbits)
{
    assert(e!=NULL);
    BaseSemantics::SValuePtr retval;
    switch (e->variantT()) {
        case V_SgAsmx86RegisterReferenceExpression: {
            SgAsmx86RegisterReferenceExpression* rre = isSgAsmx86RegisterReferenceExpression(e);
            retval = operators->readRegister(rre->get_descriptor());
            break;
        }
        case V_SgAsmBinaryAdd: {
            retval = operators->add(read(isSgAsmBinaryAdd(e)->get_lhs(), nbits), read(isSgAsmBinaryAdd(e)->get_rhs(), nbits));
            break;
        }
        case V_SgAsmBinaryMultiply: {
            SgAsmByteValueExpression* rhs = isSgAsmByteValueExpression(isSgAsmBinaryMultiply(e)->get_rhs());
            if (!rhs)
                throw BaseSemantics::Exception("byte value expression expected", get_insn());
            SgAsmExpression* lhs = isSgAsmBinaryMultiply(e)->get_lhs();
            retval = operators->extract(operators->unsignedMultiply(read(lhs, nbits), read(rhs, nbits)), 0, nbits);
            break;
        }
        case V_SgAsmMemoryReferenceExpression: {
            retval = readMemory(getSegregFromMemoryReference(isSgAsmMemoryReferenceExpression(e)),
                                readEffectiveAddress(e), operators->boolean_(true), nbits);
            break;
        }
        case V_SgAsmByteValueExpression:
        case V_SgAsmWordValueExpression:
        case V_SgAsmDoubleWordValueExpression:
        case V_SgAsmQuadWordValueExpression: {
            uint64_t val = SageInterface::getAsmSignedConstant(isSgAsmValueExpression(e));
            retval = operators->number_(nbits, val & IntegerOps::genMask<uint64_t>(nbits));
            break;
        }
        default: {
            assert(!"not implemented");
            abort();
        }
    }
    assert(retval!=NULL);
    assert(retval->get_width()==nbits);
    return retval;
}

BaseSemantics::SValuePtr
DispatcherX86::readEffectiveAddress(SgAsmExpression *expr)
{
    assert(isSgAsmMemoryReferenceExpression(expr));
    return read32(isSgAsmMemoryReferenceExpression(expr)->get_address());
}

BaseSemantics::SValuePtr
DispatcherX86::readMemory(X86SegmentRegister segreg, const BaseSemantics::SValuePtr &addr,
                          const BaseSemantics::SValuePtr& cond, size_t nbits)
{
    assert(cond!=NULL && cond->get_width()==1);
    BaseSemantics::SValuePtr retval = operators->readMemory(segreg, addr, cond, nbits);
    assert(retval!=NULL);
    assert(retval->get_width()==nbits);
    return retval;
}

void
DispatcherX86::writeMemory(X86SegmentRegister segreg, const BaseSemantics::SValuePtr &addr,
                           const BaseSemantics::SValuePtr &value, const BaseSemantics::SValuePtr &cond)
{
    assert(addr!=NULL && value!=NULL);
    assert(cond!=NULL && cond->get_width()==1);
    operators->writeMemory(segreg, addr, value, cond);
}

void
DispatcherX86::setFlagsForResult(const BaseSemantics::SValuePtr &result)
{
    size_t width = result->get_width();
    operators->writeRegister(REG_PF, parity(operators->extract(result, 0, 8)));
    operators->writeRegister(REG_SF, operators->extract(result, width-1, width));
    operators->writeRegister(REG_ZF, operators->equalToZero(result));
}

void
DispatcherX86::setFlagsForResult(const BaseSemantics::SValuePtr &result, const BaseSemantics::SValuePtr &cond)
{
    assert(cond->get_width()==1);
    BaseSemantics::SValuePtr lo_byte = operators->extract(result, 0, 8);
    BaseSemantics::SValuePtr signbit = operators->extract(result, result->get_width()-1, result->get_width());
    operators->writeRegister(REG_PF, operators->ite(cond, parity(lo_byte), operators->readRegister(REG_PF)));
    operators->writeRegister(REG_SF, operators->ite(cond, signbit, operators->readRegister(REG_SF)));
    operators->writeRegister(REG_ZF, operators->ite(cond, operators->equalToZero(result), operators->readRegister(REG_ZF)));
}

BaseSemantics::SValuePtr
DispatcherX86::parity(const BaseSemantics::SValuePtr &v)
{
    assert(v->get_width()==8);
    BaseSemantics::SValuePtr p01 = operators->xor_(operators->extract(v, 0, 1), operators->extract(v, 1, 2));
    BaseSemantics::SValuePtr p23 = operators->xor_(operators->extract(v, 2, 3), operators->extract(v, 3, 4));
    BaseSemantics::SValuePtr p45 = operators->xor_(operators->extract(v, 4, 5), operators->extract(v, 5, 6));
    BaseSemantics::SValuePtr p67 = operators->xor_(operators->extract(v, 6, 7), operators->extract(v, 7, 8));
    BaseSemantics::SValuePtr p0123 = operators->xor_(p01, p23);
    BaseSemantics::SValuePtr p4567 = operators->xor_(p45, p67);
    BaseSemantics::SValuePtr pall = operators->xor_(p0123, p4567);
    return operators->invert(pall);
}

BaseSemantics::SValuePtr
DispatcherX86::invertMaybe(const BaseSemantics::SValuePtr &value, bool maybe)
{
    return maybe ? operators->invert(value) : value;
}

BaseSemantics::SValuePtr
DispatcherX86::greaterOrEqualToTen(const BaseSemantics::SValuePtr &w)
{
    size_t nbits = w->get_width();
    BaseSemantics::SValuePtr carries;
    operators->addWithCarries(w, number_(nbits, 6), operators->boolean_(false), carries/*out*/);
    return operators->extract(carries, nbits-1, nbits);
}

BaseSemantics::SValuePtr
DispatcherX86::flagsCombo(X86InstructionKind k)
{
    switch (k) {
        case x86_jne:
        case x86_setne:
        case x86_cmovne:
            return operators->invert(operators->readRegister(REG_ZF));
        case x86_je:
        case x86_sete:
        case x86_cmove:
            return operators->readRegister(REG_ZF);
        case x86_jno:
        case x86_setno:
        case x86_cmovno:
            return operators->invert(operators->readRegister(REG_OF));
        case x86_jo:
        case x86_seto:
        case x86_cmovo:
            return operators->readRegister(REG_OF);
        case x86_jns:
        case x86_setns:
        case x86_cmovns:
            return operators->invert(operators->readRegister(REG_SF));
        case x86_js:
        case x86_sets:
        case x86_cmovs:
            return operators->readRegister(REG_SF);
        case x86_jpo:
        case x86_setpo:
        case x86_cmovpo:
            return operators->invert(operators->readRegister(REG_PF));
        case x86_jpe:
        case x86_setpe:
        case x86_cmovpe:
            return operators->readRegister(REG_PF);
        case x86_jae:
        case x86_setae:
        case x86_cmovae:
            return operators->invert(operators->readRegister(REG_CF));
        case x86_jb:
        case x86_setb:
        case x86_cmovb:
            return operators->readRegister(REG_CF);
        case x86_jbe:
        case x86_setbe:
        case x86_cmovbe:
            return operators->or_(operators->readRegister(REG_CF),
                                  operators->readRegister(REG_ZF));
        case x86_ja:
        case x86_seta:
        case x86_cmova:
            return operators->and_(operators->invert(operators->readRegister(REG_CF)),
                                   operators->invert(operators->readRegister(REG_ZF)));
        case x86_jl:
        case x86_setl:
        case x86_cmovl:
            return operators->xor_(operators->readRegister(REG_SF),
                                   operators->readRegister(REG_OF));
        case x86_jge:
        case x86_setge:
        case x86_cmovge:
            return operators->invert(operators->xor_(operators->readRegister(REG_SF),
                                                     operators->readRegister(REG_OF)));
        case x86_jle:
        case x86_setle:
        case x86_cmovle:
            return operators->or_(operators->readRegister(REG_ZF),
                                  operators->xor_(operators->readRegister(REG_SF),
                                                  operators->readRegister(REG_OF)));
        case x86_jg:
        case x86_setg:
        case x86_cmovg:
            return operators->and_(operators->invert(operators->xor_(operators->readRegister(REG_SF),
                                                                     operators->readRegister(REG_OF))),
                                   operators->invert(operators->readRegister(REG_ZF)));
        case x86_jcxz:
            return operators->equalToZero(operators->readRegister(REG_CX));
        case x86_jecxz:
            return operators->equalToZero(operators->readRegister(REG_ECX));
        default:
            assert(!"instruction kind not handled");
            abort();
        }
}

BaseSemantics::SValuePtr
DispatcherX86::repEnter(X86RepeatPrefix repeat)
{
    if (repeat==x86_repeat_none)
        return operators->boolean_(true);
    BaseSemantics::SValuePtr ecx = operators->readRegister(REG_ECX);
    BaseSemantics::SValuePtr in_loop = operators->invert(operators->equalToZero(ecx));
    return in_loop;
}

void
DispatcherX86::repLeave(X86RepeatPrefix repeat_prefix, const BaseSemantics::SValuePtr &in_loop, rose_addr_t insn_va)
{
    assert(in_loop!=NULL && in_loop->get_width()==1);

    // conditionally decrement the ECX register
    BaseSemantics::SValuePtr new_ecx = operators->add(operators->readRegister(REG_ECX),
                                                      operators->ite(in_loop,
                                                                     operators->number_(32, -1),
                                                                     operators->number_(32, 0)));
    operators->writeRegister(REG_ECX, new_ecx);
    BaseSemantics::SValuePtr nonzero_ecx = operators->invert(operators->equalToZero(new_ecx));

    // determine whether we should repeat the instruction.
    BaseSemantics::SValuePtr again;
    switch (repeat_prefix) {
        case x86_repeat_none:
            again = operators->boolean_(false);
            break;
        case x86_repeat_repe:
            again = operators->and_(operators->and_(in_loop, nonzero_ecx),
                                    operators->readRegister(REG_ZF));

            break;
        case x86_repeat_repne:
            again = operators->and_(operators->and_(in_loop, nonzero_ecx),
                                    operators->invert(operators->readRegister(REG_ZF)));
            break;
    }
    operators->writeRegister(REG_EIP,
                             operators->ite(again,
                                            operators->number_(32, insn_va),    // repeat
                                            operators->readRegister(REG_EIP))); // exit loop
}

BaseSemantics::SValuePtr
DispatcherX86::doAddOperation(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b,
                              bool invertCarries, const BaseSemantics::SValuePtr &carryIn)
{
    assert(a->get_width()==b->get_width());
    assert(1==carryIn->get_width());
    size_t nbits = a->get_width();
    BaseSemantics::SValuePtr carries;
    BaseSemantics::SValuePtr result = operators->addWithCarries(a, b, invertMaybe(carryIn, invertCarries), carries/*out*/);
    setFlagsForResult(result);
    BaseSemantics::SValuePtr sign = operators->extract(carries, nbits-1, nbits);
    BaseSemantics::SValuePtr ofbit = operators->extract(carries, nbits-2, nbits-1);
    operators->writeRegister(REG_AF, invertMaybe(operators->extract(carries, 3, 4), invertCarries));
    operators->writeRegister(REG_CF, invertMaybe(sign, invertCarries));
    operators->writeRegister(REG_OF, operators->xor_(sign, ofbit));
    return result;
}

BaseSemantics::SValuePtr
DispatcherX86::doAddOperation(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b,
                              bool invertCarries, const BaseSemantics::SValuePtr &carryIn,
                              const BaseSemantics::SValuePtr &cond)
{
    assert(a->get_width()==b->get_width());
    assert(1==carryIn->get_width());
    assert(cond!=NULL && cond->get_width()==1);
    size_t nbits = a->get_width();
    BaseSemantics::SValuePtr carries;
    BaseSemantics::SValuePtr result = operators->addWithCarries(a, b, invertMaybe(carryIn, invertCarries), carries/*out*/);
    setFlagsForResult(result, cond);
    BaseSemantics::SValuePtr sign = operators->extract(carries, nbits-1, nbits);
    BaseSemantics::SValuePtr ofbit = operators->extract(carries, nbits-2, nbits-1);
    operators->writeRegister(REG_AF,
                             operators->ite(cond,
                                            invertMaybe(operators->extract(carries, 3, 4), invertCarries),
                                            operators->readRegister(REG_AF)));
    operators->writeRegister(REG_CF,
                             operators->ite(cond,
                                            invertMaybe(sign, invertCarries),
                                            operators->readRegister(REG_CF)));
    operators->writeRegister(REG_OF,
                             operators->ite(cond,
                                            operators->xor_(sign, ofbit),
                                            operators->readRegister(REG_OF)));
    return result;
}
    


/* Does increment (decrement with DEC set), and adjusts condition flags. */
BaseSemantics::SValuePtr
DispatcherX86::doIncOperation(const BaseSemantics::SValuePtr &a, bool dec, bool setCarry)
{
    size_t nbits = a->get_width();
    BaseSemantics::SValuePtr carries;
    BaseSemantics::SValuePtr result = operators->addWithCarries(a, number_(nbits, dec?-1:1), operators->boolean_(false),
                                                                carries/*out*/);
    setFlagsForResult(result);
    BaseSemantics::SValuePtr sign = operators->extract(carries, nbits-1, nbits);
    BaseSemantics::SValuePtr ofbit = operators->extract(carries, nbits-2, nbits-1);
    operators->writeRegister(REG_AF, invertMaybe(operators->extract(carries, 3, 4), dec));
    operators->writeRegister(REG_OF, operators->xor_(sign, ofbit));
    if (setCarry)
        operators->writeRegister(REG_CF, invertMaybe(sign, dec));
    return result;
}

/** Implements the SHR, SAR, SHL, SAL, SHRD, and SHLD instructions for various operand sizes.  The shift amount is always 8
 *  bits wide in the instruction, but the semantics mask off all but the low-order bits, keeping 5 bits in 32-bit mode and
 *  7 bits in 64-bit mode (indicated by the shiftSignificantBits template argument).  The semantics of SHL and SAL are
 *  identical (in fact, ROSE doesn't even define x86_sal). The @p source_bits argument contains the bits to be shifted into
 *  the result and is used only for SHRD and SHLD instructions. */
BaseSemantics::SValuePtr
DispatcherX86::doShiftOperation(X86InstructionKind kind, const BaseSemantics::SValuePtr &operand,
                                const BaseSemantics::SValuePtr &source_bits, const BaseSemantics::SValuePtr &total_shift,
                                size_t shiftSignificantBits)
{
    assert(x86_shr==kind || x86_sar==kind || x86_shl==kind || x86_shld==kind || x86_shrd==kind);
    assert(operand->get_width()==source_bits->get_width());
    assert(total_shift->get_width()==8 && shiftSignificantBits<8);

    // The 8086 does not mask the shift count; processors starting with the 80286 (including virtual-8086 mode) do
    // mask.  The effect (other than timing) is the same either way.
    BaseSemantics::SValuePtr maskedShiftCount = operators->extract(total_shift, 0, shiftSignificantBits);
    BaseSemantics::SValuePtr isZeroShiftCount = operators->equalToZero(maskedShiftCount);

    // isLargeShift is true if the (unmasked) amount by which to shift is greater than or equal to the size in
    // bits of the destination operand.
    BaseSemantics::SValuePtr extraShiftCount = operators->extract(total_shift, shiftSignificantBits, 8);
    BaseSemantics::SValuePtr isLargeShift = operators->invert(operators->equalToZero(extraShiftCount));

    // isOneBitShift is true if the (masked) amount by which to shift is equal to one.
    uintmax_t m = IntegerOps::genMask<uintmax_t>(shiftSignificantBits);
    BaseSemantics::SValuePtr mask = number_(shiftSignificantBits, m); // -1 in modulo arithmetic
    BaseSemantics::SValuePtr isOneBitShift = operators->equalToZero(operators->add(maskedShiftCount, mask));

    // Do the actual shift, according to instruction kind.
    BaseSemantics::SValuePtr result;
    switch (kind) {
        case x86_shr:
            result = operators->shiftRight(operand, maskedShiftCount);
            break;
        case x86_sar:
            result = operators->shiftRightArithmetic(operand, maskedShiftCount);
            break;
        case x86_shl:
            result = operators->shiftLeft(operand, maskedShiftCount);
            break;
        case x86_shrd:
            result = operators->ite(isLargeShift,
                                    undefined_(operand->get_width()),
                                    operators->or_(operators->shiftRight(operand, maskedShiftCount),
                                                   operators->ite(isZeroShiftCount,
                                                                  number_(operand->get_width(), 0),
                                                                  operators->shiftLeft(source_bits,
                                                                                       operators->negate(maskedShiftCount)))));
            break;
        case x86_shld:
            result = operators->ite(isLargeShift,
                                    undefined_(operand->get_width()),
                                    operators->or_(operators->shiftLeft(operand, maskedShiftCount),
                                                   operators->ite(isZeroShiftCount,
                                                                  number_(operand->get_width(), 0),
                                                                  operators->shiftRight(source_bits,
                                                                                        operators->negate(maskedShiftCount)))));
            break;
        default:
            assert(!"instruction not handled");
            abort();
    }
    assert(operand->get_width()==result->get_width());
    BaseSemantics::SValuePtr originalSign = operators->extract(operand, operand->get_width()-1, operand->get_width());
    BaseSemantics::SValuePtr resultSign = operators->extract(result, result->get_width()-1, result->get_width());

    // The AF flag is undefined if a shift occurs.  The documentation for SHL, SHR, and SAR are somewhat ambiguous about
    // this, but the documentation for SHLD and SHRD is more specific.  We assume that both sets of shift instructions
    // behave the same way.
    operators->writeRegister(REG_AF,
                             operators->ite(isZeroShiftCount,
                                            operators->readRegister(REG_AF),
                                            undefined_(1)));

    // What is the last bit shifted off the operand?  If we're right shifting by N bits, then the original operand N-1 bit
    // is what should make it into the final CF; if we're left shifting by N bits then we need bit operand->get_width()-N.
    BaseSemantics::SValuePtr bitPosition;
    if (x86_shr==kind || x86_sar==kind) {
        bitPosition = operators->add(maskedShiftCount, mask);
    } else {
        bitPosition = operators->add(number_(shiftSignificantBits, operand->get_width() & m), // probably zero modulo
                                     operators->add(operators->invert(maskedShiftCount),
                                                    number_(shiftSignificantBits, 1)));
    }
    BaseSemantics::SValuePtr shifted_off = operators->extract(operators->shiftRight(operand, bitPosition), 0, 1);

    // New carry flag value.  From the Intel manual, the CF flag is "undefined for SHL and SHR [and SAL] instructions where
    // the count is greater than or equal to the size (in bits) of the destination operand", and "if the count is 0, the
    // flags are not affected."  The manual is silent about the value of CF for large SAR shifts, so we use the original
    // sign bit, matching the pseudo-code in the manual.
    BaseSemantics::SValuePtr newCF = operators->ite(isZeroShiftCount,
                                                    operators->readRegister(REG_CF),
                                                    operators->ite(isLargeShift,
                                                                   (x86_sar==kind ? originalSign : undefined_(1)),
                                                                   shifted_off));
    operators->writeRegister(REG_CF, newCF);

    // Ajust the overflow flag.  From the Intel manual for the SHL, SHR, and SAR instructions, "The OF flag is affected
    // only on 1-bit shifts.  For left shifts, the OF flag is set to 0 if the most-significant bit of the result is the
    // same as the CF flag (that is, the top two bits of the original operand were the same); otherwise, it is set to 1.
    // For the SAR instruction, the OF flag is cleared for all 1-bit shifts.  For the SHR instruction, the OF flag is set
    // to the most-significant bit of the original operand."  Later, it states that "the OF flag is affected only for 1-bit
    // shifts; otherwise it is undefined."  We're assuming that the statement "if the count is 0, then the flags are not
    // affected" takes precedence. For SHLD and SHRD it says, "for a 1-bit shift, the OF flag is set if a sign changed
    // occurred; otherwise it is cleared. For shifts greater than 1 bit, the OF flag is undefined."
    BaseSemantics::SValuePtr newOF;
    switch (kind) {
        case x86_shr:
            newOF = operators->ite(isOneBitShift,
                                   originalSign,
                                   operators->ite(isZeroShiftCount, 
                                                  operators->readRegister(REG_OF),
                                                  undefined_(1)));
            break;
        case x86_sar:
            newOF = operators->ite(isOneBitShift,
                                   operators->boolean_(false),
                                   operators->ite(isZeroShiftCount,
                                                  operators->readRegister(REG_OF),
                                                  undefined_(1)));
            break;
        case x86_shl:
        case x86_shld:
        case x86_shrd:
            newOF = operators->ite(isOneBitShift,
                                   operators->xor_(originalSign, resultSign),
                                   operators->ite(isZeroShiftCount,
                                                  operators->readRegister(REG_OF),
                                                  undefined_(1)));
            break;
        default: // to shut up compiler warnings even though we would have aborted by now.
            abort();
    }
    operators->writeRegister(REG_OF, newOF);

    // Result flags SF, ZF, and PF are set according to the result, but are unchanged if the shift count is zero.
    setFlagsForResult(result, operators->invert(isZeroShiftCount));
    return result;
}

} // namespace
} // namespace
