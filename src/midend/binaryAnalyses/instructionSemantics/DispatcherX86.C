#include "sage3basic.h"
#include "BaseSemantics2.h"
#include "DispatcherX86.h"
#include "integerOps.h"

namespace rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {

/*******************************************************************************************************************************
 *                                      Support functions
 *******************************************************************************************************************************/

static inline size_t asm_type_width(SgAsmType* ty) {
    ASSERT_not_null(ty);
    return ty->get_nBits();
}

/*******************************************************************************************************************************
 *                                      Functors that handle individual x86 instructions kinds
 *******************************************************************************************************************************/

namespace X86 {

// An intermediate class that reduces the amount of typing in all that follows.  Its process() method does some up-front
// checking, dynamic casting, and pointer dereferencing and then calls the p() method that does the real work.
class P: public BaseSemantics::InsnProcessor {
public:
    typedef DispatcherX86 *D;
    typedef BaseSemantics::RiscOperators *Ops;
    typedef SgAsmX86Instruction *I;
    typedef const SgAsmExpressionPtrList &A;
    virtual void p(D, Ops, I, A) = 0;

    virtual void process(const BaseSemantics::DispatcherPtr &dispatcher_, SgAsmInstruction *insn_) ROSE_OVERRIDE {
        DispatcherX86Ptr dispatcher = DispatcherX86::promote(dispatcher_);
        BaseSemantics::RiscOperatorsPtr operators = dispatcher->get_operators();
        SgAsmX86Instruction *insn = isSgAsmX86Instruction(insn_);
        ASSERT_require(insn!=NULL && insn==operators->get_insn());
        size_t nBits = dispatcher->REG_anyIP.get_nbits();
        operators->writeRegister(dispatcher->REG_anyIP, operators->add(operators->number_(nBits, insn->get_address()),
                                                                       operators->number_(nBits, insn->get_size())));
        SgAsmExpressionPtrList &operands = insn->get_operandList()->get_operands();
        check_arg_width(dispatcher.get(), insn, operands);
        p(dispatcher.get(), operators.get(), insn, operands);
    }

    void assert_args(I insn, A args, size_t nargs) {
        if (args.size()!=nargs) {
            std::string mesg = "instruction must have " + StringUtility::plural(nargs, "arguments");
            throw BaseSemantics::Exception(mesg, insn);
        }
    }

    // This is here because we don't fully support 64-bit mode yet, and a few of the support functions will fail in bad ways.
    // E.g., "jmp ds:[rip+0x200592]" will try to read32() the argument and then fail an assertion because it isn't 32 bits wide.
    // Note that even 32-bit x86 architectures might have registers that are larger than 32 bits (e.g., xmm registers on a
    // Pentium4). Therefore, we consult the register dictionary and only fail if the operand is larger than 32 bits and
    // contains a register which isn't part of the dictionary.
    void check_arg_width(D d, I insn, A args) {
        struct T1: AstSimpleProcessing {
            D d;
            I insn;
            size_t argWidth;
            T1(D d, I insn, size_t argWidth): d(d), insn(insn), argWidth(argWidth) {}
            void visit(SgNode *node) {
                if (SgAsmRegisterReferenceExpression *rre = isSgAsmRegisterReferenceExpression(node)) {
                    const RegisterDictionary *regdict = d->get_register_dictionary();
                    ASSERT_not_null(regdict);
                    if (regdict->lookup(rre->get_descriptor()).empty())
                        throw BaseSemantics::Exception(StringUtility::numberToString(argWidth) +
                                                       "-bit operands not supported for " +
                                                       regdict->get_architecture_name(),
                                                       insn);
                }
            }
        };
        for (size_t i=0; i<args.size(); ++i) {
            size_t nbits = asm_type_width(args[i]->get_type());
            if (nbits > 32)
                T1(d, insn, nbits).traverse(args[i], preorder);
        }
    }
};

// ASCII adjust after addition
struct IP_aaa: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        if (d->processorMode() == x86_insnsize_16) {
            throw BaseSemantics::Exception("16-bit processor not implemented", insn);
        } else if (d->processorMode() == x86_insnsize_32) {
            if (insn->get_lockPrefix()) {
                ops->interrupt(x86_exception_ud, 0);
            } else {
                BaseSemantics::SValuePtr nybble = ops->extract(d->readRegister(d->REG_AL), 0, 4);
                BaseSemantics::SValuePtr incAh = ops->or_(d->readRegister(d->REG_AF), d->greaterOrEqualToTen(nybble));
                ops->writeRegister(d->REG_AX, 
                                   ops->concat(ops->add(ops->ite(incAh, ops->number_(4, 6), ops->number_(4, 0)),
                                                        ops->extract(d->readRegister(d->REG_AL), 0, 4)),
                                               ops->concat(ops->number_(4, 0),
                                                           ops->add(ops->ite(incAh, ops->number_(8, 1), ops->number_(8, 0)),
                                                                    d->readRegister(d->REG_AH)))));
                ops->writeRegister(d->REG_OF, ops->undefined_(1));
                ops->writeRegister(d->REG_SF, ops->undefined_(1));
                ops->writeRegister(d->REG_ZF, ops->undefined_(1));
                ops->writeRegister(d->REG_PF, ops->undefined_(1));
                ops->writeRegister(d->REG_AF, incAh);
                ops->writeRegister(d->REG_CF, incAh);
            }
        } else {
            ASSERT_require(d->processorMode() == x86_insnsize_64);
            ops->interrupt(x86_exception_ud, 0);
        }
    }
};

// ASCII adjust AX before division
struct IP_aad: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        if (d->processorMode() == x86_insnsize_16) {
            throw BaseSemantics::Exception("16-bit processor not implemented", insn);
        } else if (d->processorMode() == x86_insnsize_32) {
            if (insn->get_lockPrefix()) {
                ops->interrupt(x86_exception_ud, 0);
            } else {
                BaseSemantics::SValuePtr al = d->readRegister(d->REG_AL);
                BaseSemantics::SValuePtr ah = d->readRegister(d->REG_AH);
                BaseSemantics::SValuePtr divisor = d->read(args[0], 8);
                BaseSemantics::SValuePtr newAl = ops->add(al, ops->extract(ops->unsignedMultiply(ah, divisor), 0, 8));
                ops->writeRegister(d->REG_AX, ops->concat(newAl, ops->number_(8, 0)));
                ops->writeRegister(d->REG_OF, ops->undefined_(1));
                ops->writeRegister(d->REG_AF, ops->undefined_(1));
                ops->writeRegister(d->REG_CF, ops->undefined_(1));
                d->setFlagsForResult(newAl);
            }
        } else {
            ASSERT_require(d->processorMode() == x86_insnsize_64);
            ops->interrupt(x86_exception_ud, 0);
        }
    }
};

// ASCII adjust AX after multiply
// AAM                  -- implied immediate value is 0x0a and stored explicitly as an argument
// AAM ib               -- immediate values other than 0x0a
struct IP_aam: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        if (d->processorMode() == x86_insnsize_16) {
            throw BaseSemantics::Exception("16-bit processor not implemented", insn);
        } else if (d->processorMode() == x86_insnsize_32) {
            BaseSemantics::SValuePtr divisor = d->read(args[0], 8);
            if (insn->get_lockPrefix()) {
                ops->interrupt(x86_exception_ud, 0);
            } else if (divisor->is_number() && divisor->get_number()==0) {
                ops->interrupt(x86_exception_de, 0);
            } else {
                BaseSemantics::SValuePtr al = d->readRegister(d->REG_AL);
                BaseSemantics::SValuePtr newAh = ops->unsignedDivide(al, divisor);
                BaseSemantics::SValuePtr newAl = ops->unsignedModulo(al, divisor);
                ops->writeRegister(d->REG_AX, ops->concat(newAl, newAh));
                ops->writeRegister(d->REG_OF, ops->undefined_(1));
                ops->writeRegister(d->REG_AF, ops->undefined_(1));
                ops->writeRegister(d->REG_CF, ops->undefined_(1));
                d->setFlagsForResult(newAl);
            }
        } else {
            ASSERT_require(d->processorMode() == x86_insnsize_64);
            ops->interrupt(x86_exception_ud, 0);
        }
    }
};

// ASCII adjust AL after subtraction
struct IP_aas: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        if (d->processorMode() == x86_insnsize_16) {
            throw BaseSemantics::Exception("16-bit processor not implemented", insn);
        } else if (d->processorMode() == x86_insnsize_32) {
            if (insn->get_lockPrefix()) {
                ops->interrupt(x86_exception_ud, 0);
            } else {
                BaseSemantics::SValuePtr nybble = ops->extract(d->readRegister(d->REG_AL), 0, 4);
                BaseSemantics::SValuePtr decAh = ops->or_(d->readRegister(d->REG_AF), d->greaterOrEqualToTen(nybble));
                ops->writeRegister(d->REG_AX, 
                                   ops->concat(ops->add(ops->ite(decAh, ops->number_(4, -6), ops->number_(4, 0)),
                                                        ops->extract(d->readRegister(d->REG_AL), 0, 4)),
                                               ops->concat(ops->number_(4, 0),
                                                           ops->add(ops->ite(decAh, ops->number_(8, -1), ops->number_(8, 0)),
                                                                    d->readRegister(d->REG_AH)))));
                ops->writeRegister(d->REG_OF, ops->undefined_(1));
                ops->writeRegister(d->REG_SF, ops->undefined_(1));
                ops->writeRegister(d->REG_ZF, ops->undefined_(1));
                ops->writeRegister(d->REG_PF, ops->undefined_(1));
                ops->writeRegister(d->REG_AF, decAh);
                ops->writeRegister(d->REG_CF, decAh);
            }
        } else {
            ASSERT_require(d->processorMode() == x86_insnsize_64);
            ops->interrupt(x86_exception_ud, 0);
        }
    }
};

// Add with carry
struct IP_adc: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix() && !isSgAsmMemoryReferenceExpression(args[0])) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr result = d->doAddOperation(d->read(args[0]), d->read(args[1]),
                                                                false, d->readRegister(d->REG_CF));
            d->write(args[0], result);
        }
    }
};

// Add two values
struct IP_add: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix() && !isSgAsmMemoryReferenceExpression(args[0])) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr result = d->doAddOperation(d->read(args[0]), d->read(args[1]), false, ops->boolean_(false));
            d->write(args[0], result);
        }
    }
};

// Bitwise AND
struct IP_and: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix() && !isSgAsmMemoryReferenceExpression(args[0])) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr a = d->read(args[0]);
            BaseSemantics::SValuePtr b = d->read(args[1]);
            if (b->get_width() < a->get_width())
                b = ops->signExtend(b, a->get_width());
            BaseSemantics::SValuePtr result = ops->and_(a, b);
            d->setFlagsForResult(result);
            d->write(args[0], result);
            ops->writeRegister(d->REG_OF, ops->boolean_(false));
            ops->writeRegister(d->REG_AF, ops->undefined_(1));
            ops->writeRegister(d->REG_CF, ops->boolean_(false));
        }
    }
};

// Bit scan forward or reverse
struct IP_bitscan: P {
    const X86InstructionKind kind;
    IP_bitscan(X86InstructionKind k): kind(k) {
        ASSERT_require(x86_bsf==k || x86_bsr==k);
    }
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            ops->writeRegister(d->REG_OF, ops->undefined_(1));
            ops->writeRegister(d->REG_SF, ops->undefined_(1));
            ops->writeRegister(d->REG_AF, ops->undefined_(1));
            ops->writeRegister(d->REG_PF, ops->undefined_(1));
            ops->writeRegister(d->REG_CF, ops->undefined_(1));
            size_t nbits = asm_type_width(args[0]->get_type());
            BaseSemantics::SValuePtr src = d->read(args[1], nbits);
            BaseSemantics::SValuePtr isZero = ops->equalToZero(src);
            ops->writeRegister(d->REG_ZF, isZero);
            BaseSemantics::SValuePtr bitno;
            ASSERT_require(insn->get_kind()==kind);
            switch (kind) {
                case x86_bsf: bitno = ops->leastSignificantSetBit(src); break;
                case x86_bsr: bitno = ops->mostSignificantSetBit(src); break;
                default: ASSERT_not_reachable("instruction kind not handled");
            }
            BaseSemantics::SValuePtr result = ops->ite(isZero, ops->undefined_(nbits), bitno);
            d->write(args[0], result);
        }
    }
};

// Bit test instructions: BT, BTR
struct IP_bittest: P {
    const X86InstructionKind kind;
    IP_bittest(X86InstructionKind k): kind(k) {
        ASSERT_require(x86_bt==k || x86_btr==k || x86_bts==k || x86_btc==k);
    }
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        ASSERT_require(insn->get_kind()==kind);
        if (insn->get_lockPrefix() && (x86_bt==kind || !isSgAsmMemoryReferenceExpression(args[0]))) {
            ops->interrupt(x86_exception_ud, 0);
        } else if (SgAsmMemoryReferenceExpression *mre = isSgAsmMemoryReferenceExpression(args[0])) {
            const size_t addrSize = d->REG_anyIP.get_nbits();
            BaseSemantics::SValuePtr bitBase = ops->unsignedExtend(d->effectiveAddress(mre), addrSize);
            BaseSemantics::SValuePtr bitOffset = ops->signExtend(d->read(args[1]), addrSize);

            // Byte offset from bitBase address is signed. If negative, the bit is at the previous memory address. E.g.:
            //   if bitBase==100 && bitOffset==+13 then cf = mem[101] & (1<<5);
            //   if bitBase==100 && bitOffset==-13 then cf = mem[99] & (1<<5)
            // Notice that the bit offset within a byte is always positive (i.e., |bitOffset|%8) and that in the second
            // example (for bitOffset==-13) the bit offset within the memory byte is 5, not 3.  This information comes from
            // the Intel Instruction Set Reference and has not been tested by us. [Robb P. Matzke 2015-02-24]
            BaseSemantics::SValuePtr byteOffset = ops->shiftRightArithmetic(bitOffset, ops->number_(8, 3));
            BaseSemantics::SValuePtr addr = ops->add(bitBase, byteOffset);
            BaseSemantics::SValuePtr byte = ops->readMemory(d->segmentRegister(mre), addr, ops->undefined_(8),
                                                            ops->boolean_(true));
            BaseSemantics::SValuePtr shiftAmount =
                ops->ite(ops->extract(bitOffset, bitOffset->get_width()-1, bitOffset->get_width()),
                         ops->extract(ops->negate(bitOffset), 0, 3), // bitOffset is negative
                         ops->extract(bitOffset, 0, 3));             // bitOffset is positive
            BaseSemantics::SValuePtr bit = ops->extract(ops->shiftRight(byte, shiftAmount), 0, 1);
            switch (kind) {
                case x86_bt:                            // test only
                    break;
                case x86_btr:                           // clear bit
                    byte = ops->and_(byte, ops->invert(ops->shiftLeft(ops->number_(8, 1), shiftAmount)));
                    ops->writeMemory(d->segmentRegister(mre), addr, byte, ops->boolean_(true));
                    break;
                case x86_bts:                           // set bit
                    byte = ops->or_(byte, ops->shiftLeft(ops->number_(8, 1), shiftAmount));
                    ops->writeMemory(d->segmentRegister(mre), addr, byte, ops->boolean_(true));
                    break;
                case x86_btc:                           // complement bit
                    byte = ops->xor_(byte, ops->shiftLeft(ops->number_(8, 1), shiftAmount));
                    ops->writeMemory(d->segmentRegister(mre), addr, byte, ops->boolean_(true));
                    break;
                default:
                    ASSERT_not_reachable("instruction kind not handled");
            }
            ops->writeRegister(d->REG_CF, bit);
            ops->writeRegister(d->REG_OF, ops->undefined_(1));
            ops->writeRegister(d->REG_SF, ops->undefined_(1));
            ops->writeRegister(d->REG_ZF, ops->undefined_(1));
            ops->writeRegister(d->REG_AF, ops->undefined_(1));
            ops->writeRegister(d->REG_PF, ops->undefined_(1));
        } else {
            BaseSemantics::SValuePtr bits = d->read(args[0]);
            BaseSemantics::SValuePtr bitOffset = d->read(args[1]);
            size_t log2modulo;
            switch (bits->get_width()) {
                case 16: log2modulo = 4; break;
                case 32: log2modulo = 5; break;
                case 64: log2modulo = 6; break;
                default: ASSERT_not_reachable("invalid width for first operand");
            }
            ASSERT_require(bitOffset->get_width() >= log2modulo);
            BaseSemantics::SValuePtr shiftAmount = ops->extract(bitOffset, 0, log2modulo);
            BaseSemantics::SValuePtr bit = ops->extract(ops->shiftRight(bits, shiftAmount), 0, 1);
            switch (kind) {
                case x86_bt:                            // test only
                    break;
                case x86_btr:                           // clear bit
                    bits = ops->and_(bits, ops->invert(ops->shiftLeft(ops->number_(bits->get_width(), 1), shiftAmount)));
                    d->write(args[0], bits);
                    break;
                case x86_bts:                           // set bit
                    bits = ops->or_(bits, ops->shiftLeft(ops->number_(bits->get_width(), 1), shiftAmount));
                    d->write(args[0], bits);
                    break;
                case x86_btc:                           // complement bit
                    bits = ops->xor_(bits, ops->shiftLeft(ops->number_(bits->get_width(), 1), shiftAmount));
                    d->write(args[0], bits);
                    break;
                default:
                    ASSERT_not_reachable("instruction kind not handled");
            }
            ops->writeRegister(d->REG_CF, bit);
            ops->writeRegister(d->REG_OF, ops->undefined_(1));
            ops->writeRegister(d->REG_SF, ops->undefined_(1));
            ops->writeRegister(d->REG_ZF, ops->undefined_(1));
            ops->writeRegister(d->REG_AF, ops->undefined_(1));
            ops->writeRegister(d->REG_PF, ops->undefined_(1));
        }
    }
};

// Byte swap (reverse byte order)
struct IP_bswap: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        size_t nbits = asm_type_width(args[0]->get_type());
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else if (16 == nbits) {
            // Intel ref manual says "When the BSWAP instruction references a 16-bit register, the result is
            // undefined".
            d->write(args[0], ops->undefined_(16));
        } else {
            BaseSemantics::SValuePtr op0 = d->read(args[0], nbits);
            BaseSemantics::SValuePtr result = ops->extract(op0, 0, 8);
            for (size_t startbit=8; startbit<nbits; startbit+=8)
                result = ops->concat(ops->extract(op0, startbit, startbit+8), result);
            d->write(args[0], result);
        }
    }
};

// Call procedure
struct IP_call: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr targetVa = ops->unsignedExtend(d->read(args[0]), d->REG_anyIP.get_nbits());
            
            // Push return address onto stack
            size_t nBytesPush = d->REG_anyIP.get_nbits() >> 3;
            BaseSemantics::SValuePtr oldSp = d->readRegister(d->REG_anySP);
            BaseSemantics::SValuePtr newSp = ops->add(oldSp, ops->number_(d->REG_anySP.get_nbits(), -nBytesPush));
            ops->writeMemory(d->REG_SS, newSp, d->readRegister(d->REG_anyIP), ops->boolean_(true));
            ops->writeRegister(d->REG_anySP, newSp);;
            
            // Branch
            ops->writeRegister(d->REG_anyIP, ops->filterCallTarget(targetVa));
        }
    }
};

// Sign extend AL to AX
struct IP_cbw: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            ops->writeRegister(d->REG_AX, ops->signExtend(d->readRegister(d->REG_AL), 16));
        }
    }
};

// Sign extend EAX into EDX:EAX
struct IP_cdq: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->writeRegister(d->REG_EDX, ops->extract(ops->signExtend(d->readRegister(d->REG_EAX), 64), 32, 64));
    }
};

// Sign extend EAX to RAX
struct IP_cdqe: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            ops->writeRegister(d->REG_RAX, ops->signExtend(d->readRegister(d->REG_EAX), 64));
        }
    }
};

// Clear carry flag
struct IP_clc: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            ops->writeRegister(d->REG_CF, ops->boolean_(false));
        }
    }
};

// Clear direction flag
struct IP_cld: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            ops->writeRegister(d->REG_DF, ops->boolean_(false));
        }
    }
};

// Flush cache line
struct IP_clflush: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            // read memory for its side effects (e.g., page faults), but don't do anything else since semantics has no cache
            // lines.
            (void) d->read(args[0]);
        }
    }
};

// Complement carry flag
struct IP_cmc: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            ops->writeRegister(d->REG_CF, ops->invert(d->readRegister(d->REG_CF)));
        }
    }
};

// Conditional move
struct IP_cmovcc: P {
    const X86InstructionKind kind;
    IP_cmovcc(X86InstructionKind k): kind(k) {
        ASSERT_require(x86_cmovne==k || x86_cmove==k || x86_cmovno==k || x86_cmovo==k || x86_cmovpo==k || x86_cmovpe==k ||
                       x86_cmovns==k || x86_cmovs==k || x86_cmovae==k || x86_cmovb==k || x86_cmovbe==k || x86_cmova==k ||
                       x86_cmovle==k || x86_cmovg==k || x86_cmovge==k || x86_cmovl==k);
    }
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        ASSERT_require(insn->get_kind()==kind);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            d->write(args[0], ops->ite(d->flagsCombo(kind), d->read(args[1]), d->read(args[0])));
        }
    }                                                                                                                          \
};
        
// Compare two values
struct IP_cmp: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr a = d->read(args[0]);
            BaseSemantics::SValuePtr b = d->read(args[1]);
            if (b->get_width() < a->get_width())
                b = ops->signExtend(b, a->get_width());
            ASSERT_require(a->get_width() == b->get_width());

            // Compute a-b for its status register side effects
            (void) d->doAddOperation(a, ops->invert(b), true, ops->boolean_(false));
        }
    }
};

// Compare strings
// CMPSD is also a floating-point instruction when it has two operands
struct IP_cmpstrings: P {
    const X86RepeatPrefix repeat;
    const size_t nbits;
    const size_t nbytes;
    IP_cmpstrings(X86RepeatPrefix repeat, size_t nbits): repeat(repeat), nbits(nbits), nbytes(nbits/8) {
        ASSERT_require(8==nbits || 16==nbits || 32==nbits || 64==nbits);
    }
    void p(D d, Ops ops, I insn, A args) {
        if (insn->get_kind()==x86_cmpsd && args.size() == 2) {
            // This is a floating point instruction: compare scalar double-precision floating-point values
            throw BaseSemantics::Exception("no dispatch ability for instruction", insn);
        }

        assert_args(insn, args, 0);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            // The disassembler produces CMPB, CMPW, CMPD, or CMPQ without any arguments.
            BaseSemantics::SValuePtr inLoop = d->repEnter(repeat);

            // Get the addresses for the two values to read and compare.
            RegisterDescriptor reg1, reg2;
            switch (insn->get_addressSize()) {
                case x86_insnsize_16:
                    reg1 = d->REG_SI;
                    reg2 = d->REG_DI;
                    break;
                case x86_insnsize_32:
                    reg1 = d->REG_ESI;
                    reg2 = d->REG_EDI;
                    break;
                case x86_insnsize_64:
                    reg1 = d->REG_RSI;
                    reg2 = d->REG_RDI;
                    break;
                default:
                    ASSERT_not_reachable("invalid instruction address size");
            }
            BaseSemantics::SValuePtr addr1 = d->readRegister(reg1);
            BaseSemantics::SValuePtr addr2 = d->readRegister(reg2);

            // Adjust address width depending on how memory is accessed.
            if (size_t addrWidth = d->addressWidth()) {
                if (addr1->get_width() < addrWidth) {
                    addr1 = ops->signExtend(addr1, addrWidth);
                } else if (addr1->get_width() > addrWidth) {
                    addr1 = ops->unsignedExtend(addr1, addrWidth);
                }
                if (addr2->get_width() < addrWidth) {
                    addr2 = ops->signExtend(addr2, addrWidth);
                } else if (addr2->get_width() > addrWidth) {
                    addr2 = ops->unsignedExtend(addr2, addrWidth);
                }
            }
            ASSERT_require(addr1->get_width() == addr2->get_width());
            
            // Read the two values from memory.
            RegisterDescriptor sr(x86_regclass_segment,
                                  insn->get_segmentOverride()!=x86_segreg_none ? insn->get_segmentOverride() : x86_segreg_ds,
                                  0, 16);
            BaseSemantics::SValuePtr val1 = ops->readMemory(sr, addr1, ops->undefined_(nbits), inLoop);
            BaseSemantics::SValuePtr val2 = ops->readMemory(sr, addr2, ops->undefined_(nbits), inLoop);

            // Compare values and set status flags.
            (void) d->doAddOperation(val1, ops->invert(val2), true, ops->boolean_(false), inLoop);

            // Adjust the address registers
            BaseSemantics::SValuePtr step = ops->ite(d->readRegister(d->REG_DF),
                                                     ops->number_(reg1.get_nbits(), -nbytes),
                                                     ops->number_(reg1.get_nbits(), +nbytes));
            ops->writeRegister(reg1, ops->ite(inLoop, ops->add(ops->readRegister(reg1), step), ops->readRegister(reg1)));
            ops->writeRegister(reg2, ops->ite(inLoop, ops->add(ops->readRegister(reg1), step), ops->readRegister(reg2)));

            // Adjust instruction pointer register to either repeat the instruction or fall through
            if (x86_repeat_none!=repeat)
                d->repLeave(repeat, inLoop, insn->get_address());
        }
    }
};

// Compare and exchange
struct IP_cmpxchg: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix() && !isSgAsmMemoryReferenceExpression(args[0])) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            size_t nbits = asm_type_width(args[0]->get_type());
            ASSERT_require(asm_type_width(args[1]->get_type()) == nbits);
            BaseSemantics::SValuePtr op0 = d->read(args[0]);
            RegisterDescriptor A = d->REG_AX;
            A.set_nbits(nbits);
            BaseSemantics::SValuePtr oldA = d->readRegister(A);
            (void) d->doAddOperation(oldA, ops->invert(op0), true, ops->boolean_(false));
            BaseSemantics::SValuePtr zf = d->readRegister(d->REG_ZF);
            d->write(args[0], ops->ite(zf, d->read(args[1]), op0));
            ops->writeRegister(A, ops->ite(zf, oldA, op0));
        }
    }
};

// CPU identification
struct IP_cpuid: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            ops->cpuid();
        }
    }
};

// Sign extend AX into DX:AX
struct IP_cwd: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        ops->writeRegister(d->REG_DX, ops->extract(ops->signExtend(d->readRegister(d->REG_AX), 32), 16, 32));
    }
};

// Sign extend AX to EAX
struct IP_cwde: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            ops->writeRegister(d->REG_EAX, ops->signExtend(d->readRegister(d->REG_AX), 32));
        }
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
        ops->writeRegister(d->REG_anyIP, ops->number_(d->REG_anyIP.get_nbits(), insn->get_address()));
    }
};

// division instructions: IDIV, DIV
struct IP_divide: P {
    const X86InstructionKind kind;
    IP_divide(X86InstructionKind k): kind(k) {
        ASSERT_require(x86_div==k || x86_idiv==k);
    }
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        size_t nbits = asm_type_width(args[0]->get_type());
        RegisterDescriptor regA = d->REG_AX; regA.set_nbits(8==nbits ? 16 : nbits);
        RegisterDescriptor regD = d->REG_DX; regD.set_nbits(8==nbits ? 16 : nbits);
        BaseSemantics::SValuePtr op0;
        if (8==nbits) {
            op0 = d->readRegister(regA);
        } else {
            op0 = ops->concat(d->readRegister(regA), d->readRegister(regD));
        }
        BaseSemantics::SValuePtr op1 = d->read(args[0], nbits);
        BaseSemantics::SValuePtr divResult, modResult;
        ASSERT_require(insn->get_kind()==kind);
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
                ASSERT_not_reachable("instruction kind not handled");
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

// Load floating-point value
struct IP_fld: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        size_t nbits = asm_type_width(args[0]->get_type());
        if (80!=nbits)
            throw BaseSemantics::Exception(StringUtility::numberToString(nbits)+"-bit FP values not supported yet", insn);
        BaseSemantics::SValuePtr fp = d->read(args[0], nbits);
        d->pushFloatingPoint(fp);
    }
};

// Restore x86 FPU control word
struct IP_fldcw: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        ops->writeRegister(d->REG_FPCTL, d->read(args[0], 16));
    }
};

// Store x87 FPU control word
struct IP_fnstcw: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        d->write(args[0], d->readRegister(d->REG_FPCTL));
    }
};

// Store x87 FPU status word
struct IP_fnstsw: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        d->write(args[0], d->readRegister(d->REG_FPSTATUS));
    }
};

// Store x87 floating point value
struct IP_fst: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        size_t nbits = asm_type_width(args[0]->get_type());
        if (80!=nbits)
            throw BaseSemantics::Exception(StringUtility::numberToString(nbits)+"-bit FP values not supported yet", insn);
        d->write(args[0], d->readFloatingPointStack(0));
    }
};

// Store x87 floating point value and pop fp stack
struct IP_fstp: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        size_t nbits = asm_type_width(args[0]->get_type());
        if (80!=nbits)
            throw BaseSemantics::Exception(StringUtility::numberToString(nbits)+"-bit FP values not supported yet", insn);
        d->write(args[0], d->readFloatingPointStack(0));
        d->popFloatingPoint();
    }
};

// Signed multiply
struct IP_imul: P {
    void p(D d, Ops ops, I insn, A args) {
        size_t nbits = asm_type_width(args[0]->get_type());
        size_t nbits_last = asm_type_width(args.back()->get_type());
        BaseSemantics::SValuePtr result;
        RegisterDescriptor reg0 = d->REG_AX;
        reg0.set_nbits(nbits);
        if (8==nbits) {
            assert_args(insn, args, 1);
            BaseSemantics::SValuePtr op0 = d->readRegister(reg0);
            BaseSemantics::SValuePtr op1 = d->read(args[0], nbits);
            result = ops->signedMultiply(op0, op1);
            ops->writeRegister(d->REG_AX, result);
        } else {
            if (args.size()<1 || args.size()>3)
                throw BaseSemantics::Exception("instruction must have 1, 2, or 3 arguments", insn);
            BaseSemantics::SValuePtr op0 = 1==args.size() ? d->readRegister(reg0) : d->read(args[args.size()-2], nbits);
            BaseSemantics::SValuePtr op1 = ops->signExtend(d->read(args.back(), nbits_last), nbits);
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
        ASSERT_require(result->get_width() == 2*nbits);

        // For the one-operand form of the instruction, the CF and OF flags are set only when bits are carried into the upper
        // half of the result; for the two- and three-operand forms, the CF and OF flags are set only when the signed result
        // must be truncated to fit in the destination operand size.  The CF and OF flags are cleared otherwise.
        BaseSemantics::SValuePtr carry;
        if (1==args.size()) {
            // carry set when high-order bits are not all zero
            carry = ops->invert(ops->equalToZero(ops->extract(result, nbits, 2*nbits)));
        } else {
            // carry set when high-order bits are not all equal to the low-half sign bit.  In other words, when high-half bits
            // are not all clear or not all set or when the high-half sign bit is not equal to the low-half sign bit.
            BaseSemantics::SValuePtr lh_signbit = ops->extract(result, nbits-1, nbits);
            BaseSemantics::SValuePtr hh_signbit = ops->extract(result, 2*nbits-1, 2*nbits);
            BaseSemantics::SValuePtr hh = ops->extract(result, nbits, 2*nbits);
            BaseSemantics::SValuePtr hh_allsame = ops->or_(ops->equalToZero(hh), ops->equalToZero(ops->invert(hh)));
            BaseSemantics::SValuePtr signsame = ops->equalToZero(ops->xor_(lh_signbit, hh_signbit));
            carry = ops->invert(ops->and_(hh_allsame, signsame));
        }

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
        SgAsmIntegerValueExpression *bv = isSgAsmIntegerValueExpression(args[0]);
        if (!bv)
            throw BaseSemantics::Exception("operand must be a byte value expression", insn);
        ops->interrupt(0, bv->get_value());
    }
};

// Jump
struct IP_jmp: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        ops->writeRegister(d->REG_anyIP, ops->filterIndirectJumpTarget(d->read(args[0], d->REG_anyIP.get_nbits())));
    }
};

// Jump if condition is met
struct IP_jcc: P {
    const X86InstructionKind kind;
    IP_jcc(X86InstructionKind k): kind(k) {
        ASSERT_require(x86_jne==k || x86_je==k || x86_jno==k || x86_jo==k || x86_jns==k || x86_js==k ||
                       x86_jpo==k || x86_jpe==k || x86_jae==k || x86_jb==k || x86_jbe==k || x86_ja==k ||
                       x86_jl==k || x86_jge==k || x86_jle==k || x86_jg==k || x86_jcxz==k || x86_jecxz==k);
    }
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        ASSERT_require(insn->get_kind()==kind);
        BaseSemantics::SValuePtr cond = d->flagsCombo(kind);
        ops->writeRegister(d->REG_anyIP,
                           ops->ite(cond, d->read(args[0], d->REG_anyIP.get_nbits()), d->readRegister(d->REG_anyIP)));
    }
};

// Load effective address
struct IP_lea: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        size_t nbits = asm_type_width(args[0]->get_type());
        d->write(args[0], ops->unsignedExtend(d->effectiveAddress(args[1]), nbits));
    }
};

// High-level procedure exit
struct IP_leave: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        if (insn->get_addressSize() != x86_insnsize_32 || insn->get_operandSize() != x86_insnsize_32)
            throw BaseSemantics::Exception("size not implemented", insn);
        ops->writeRegister(d->REG_ESP, d->readRegister(d->REG_EBP));
        BaseSemantics::SValuePtr oldSp = d->readRegister(d->REG_ESP);
        BaseSemantics::SValuePtr newSp = ops->add(oldSp, ops->number_(32, 4));
        ops->writeRegister(d->REG_EBP, ops->readMemory(d->REG_SS, oldSp, ops->undefined_(32), ops->boolean_(true)));
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
        ASSERT_require(8==nbits || 16==nbits || 32==nbits);
    }
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        if (insn->get_addressSize() != x86_insnsize_32)
            throw BaseSemantics::Exception("size not implemented", insn);
        RegisterDescriptor regA = d->REG_EAX; regA.set_nbits(nbits);
        BaseSemantics::SValuePtr in_loop = d->repEnter(repeat);
        RegisterDescriptor sr(x86_regclass_segment,
                              insn->get_segmentOverride()!=x86_segreg_none ? insn->get_segmentOverride() : x86_segreg_ds,
                              0, 16);
        BaseSemantics::SValuePtr value = ops->readMemory(sr, d->readRegister(d->REG_ESI), ops->undefined_(nbits), in_loop);
        ops->writeRegister(regA, value);
        BaseSemantics::SValuePtr step = ops->ite(d->readRegister(d->REG_DF),
                                                 ops->number_(32, -nbytes), ops->number_(32, nbytes));
        ops->writeRegister(d->REG_ESI, ops->add(d->readRegister(d->REG_ESI), step));
    }
};

// Loop according to ECX counter: LOOP, LOOPNZ, LOOPZ
struct IP_loop: P {
    const X86InstructionKind kind;
    IP_loop(X86InstructionKind k): kind(k) {
        ASSERT_require(x86_loop==k || x86_loopnz==k || x86_loopz==k);
    }
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        if (insn->get_addressSize() != x86_insnsize_32 || insn->get_operandSize() != x86_insnsize_32)
            throw BaseSemantics::Exception("size not implemented", insn);
        BaseSemantics::SValuePtr oldCx = d->readRegister(d->REG_ECX);
        BaseSemantics::SValuePtr newCx = ops->add(ops->number_(32, -1), oldCx);
        ops->writeRegister(d->REG_ECX, newCx);
        BaseSemantics::SValuePtr doLoop;
        ASSERT_require(insn->get_kind()==kind);
        switch (kind) {
            case x86_loop:
                doLoop = ops->invert(ops->equalToZero(newCx));
                break;
            case x86_loopnz:
                doLoop = ops->and_(ops->invert(ops->equalToZero(newCx)), ops->invert(d->readRegister(d->REG_ZF)));
                break;
            case x86_loopz:
                doLoop = ops->and_(ops->invert(ops->equalToZero(newCx)), d->readRegister(d->REG_ZF));
                break;
            default:
                ASSERT_not_reachable("instruction type not handled");
        }
        ops->writeRegister(d->REG_anyIP, ops->ite(doLoop,
                                                  d->read(args[0], d->REG_anyIP.get_nbits()),
                                                  d->readRegister(d->REG_anyIP)));
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

// Move source to destination with truncation or zero extend
struct IP_move_extend: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        size_t nbitsSrc = asm_type_width(args[1]->get_type());
        size_t nbitsDst = asm_type_width(args[0]->get_type());
        d->write(args[0], ops->unsignedExtend(d->read(args[1], nbitsSrc), nbitsDst));
    }
};

// Move aligned double quadword
struct IP_movdqa: P {
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
        ASSERT_require(8==nbits || 16==nbits || 32==nbits);
        ASSERT_require(x86_repeat_none==repeat || x86_repeat_repe==repeat);
    }
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        if (insn->get_addressSize() != x86_insnsize_32)
            throw BaseSemantics::Exception("size not implemented", insn);
        BaseSemantics::SValuePtr in_loop = d->repEnter(repeat);
        BaseSemantics::SValuePtr value = ops->readMemory(d->REG_ES, d->readRegister(d->REG_EDI),
                                                         ops->undefined_(nbits), in_loop);
        ops->writeMemory(d->REG_ES, d->readRegister(d->REG_EDI), value, in_loop);
        BaseSemantics::SValuePtr step = ops->ite(d->readRegister(d->REG_DF),
                                                 ops->number_(32, -nbytes), ops->number_(32, nbytes));
        ops->writeRegister(d->REG_ESI,
                           ops->add(d->readRegister(d->REG_ESI),
                                    ops->ite(in_loop, step, ops->number_(32, 0))));
        ops->writeRegister(d->REG_EDI,
                           ops->add(d->readRegister(d->REG_EDI),
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
            ASSERT_require(dst_nbits>src_nbits);
            d->write(args[0], ops->concat(d->read(args[1], src_nbits), ops->number_(dst_nbits-src_nbits, 0)));
        }
    }
};

// Unsigned multiply
struct IP_mul: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        size_t nbits = asm_type_width(args[0]->get_type());
        RegisterDescriptor reg0 = d->REG_AX; reg0.set_nbits(nbits);
        RegisterDescriptor reg1 = d->REG_DX; reg1.set_nbits(nbits);
        BaseSemantics::SValuePtr op0 = d->readRegister(reg0);
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

// Packed align right
struct IP_palignr: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        size_t nbits = asm_type_width(args[0]->get_type());
        if (64!=nbits && 128!=nbits) {
            throw BaseSemantics::Exception("first operand must be 64 or 128 bits wide (actual " +
                                           StringUtility::plural(nbits, "bits") + ")", insn);
        }
        if (args[0]->get_type() != args[1]->get_type())
            throw BaseSemantics::Exception("first and second operands must be the same type", insn);
        SgAsmIntegerValueExpression *nBytes = isSgAsmIntegerValueExpression(args[2]);
        if (!nBytes)
            throw BaseSemantics::Exception("third operand must be a byte value expression (imm8)", insn);
        BaseSemantics::SValuePtr wide = ops->concat(d->read(args[0], nbits), d->read(args[1], nbits));
        BaseSemantics::SValuePtr result = ops->extract(ops->shiftRight(wide, d->number_(8, nBytes->get_value() * 8)),
                                                       0, nbits);
        d->write(args[0], result);
    }
};

// Pop from stack
struct IP_pop: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);

        // Assuming the address size attribute of the stack segment is set to 32 bits.
        size_t operandWidth = asm_type_width(args[0]->get_type());
        if (16!=operandWidth && 32!=operandWidth)
            throw BaseSemantics::Exception(StringUtility::numberToString(operandWidth)+"-bit operand not supported for POP",
                                           insn);
        
        // Increment the stack pointer before writing to args[0] just in case args[0] is ESP-relative
        size_t stackDelta = operandWidth / 8;
        BaseSemantics::SValuePtr oldSp = d->readRegister(d->REG_ESP);
        BaseSemantics::SValuePtr newSp = ops->add(oldSp, ops->number_(32, stackDelta));
        ops->writeRegister(d->REG_ESP, newSp);

        // Read from stack and write to args[0]
        d->write(args[0], ops->readMemory(d->REG_SS, oldSp, ops->undefined_(operandWidth), ops->boolean_(true)));
    }
};

// Pop all general-purpose registers
struct IP_popad: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        if (insn->get_addressSize() != x86_insnsize_32)
            throw BaseSemantics::Exception("size not implemented", insn);
        BaseSemantics::SValuePtr oldSp = d->readRegister(d->REG_ESP);
        BaseSemantics::SValuePtr newSp = ops->add(oldSp, ops->number_(32, 32));
        ops->writeRegister(d->REG_EDI,
                           ops->readMemory(d->REG_SS, oldSp, ops->undefined_(32), ops->boolean_(true)));
        ops->writeRegister(d->REG_ESI,
                           ops->readMemory(d->REG_SS, ops->add(oldSp, ops->number_(32, 4)),
                                           ops->undefined_(32), ops->boolean_(true)));
        ops->writeRegister(d->REG_EBP,
                           ops->readMemory(d->REG_SS, ops->add(oldSp, ops->number_(32, 8)),
                                           ops->undefined_(32), ops->boolean_(true)));
        ops->writeRegister(d->REG_EBX,
                           ops->readMemory(d->REG_SS, ops->add(oldSp, ops->number_(32, 16)),
                                           ops->undefined_(32), ops->boolean_(true)));
        ops->writeRegister(d->REG_EDX,
                           ops->readMemory(d->REG_SS, ops->add(oldSp, ops->number_(32, 20)),
                                           ops->undefined_(32), ops->boolean_(true)));
        ops->writeRegister(d->REG_ECX,
                           ops->readMemory(d->REG_SS, ops->add(oldSp, ops->number_(32, 24)),
                                           ops->undefined_(32), ops->boolean_(true)));
        ops->writeRegister(d->REG_EAX,
                           ops->readMemory(d->REG_SS, ops->add(oldSp, ops->number_(32, 28)),
                                           ops->undefined_(32), ops->boolean_(true)));
        (void) ops->readMemory(d->REG_SS, ops->add(oldSp, ops->number_(32, 12)),
                               ops->undefined_(32), ops->boolean_(true));
        ops->writeRegister(d->REG_ESP, newSp);
    }
};

// Push onto stack
struct IP_push: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        size_t operandWidth = asm_type_width(args[0]->get_type());
        bool isImmediate = isSgAsmIntegerValueExpression(args[0])!=NULL;

        // Assuming the address size attribute of the stack segment is set to 32 bits.
        BaseSemantics::SValuePtr toPush;
        if (isImmediate) {
            toPush = ops->signExtend(d->read(args[0], operandWidth), 32);
        } else if (16==operandWidth || 32==operandWidth) {
            toPush = d->read(args[0], operandWidth);
        } else {
            throw BaseSemantics::Exception("invalid operand size for 32-bit push", insn);
        }
        ASSERT_not_null(toPush);
        ASSERT_require(toPush->get_width()==16 || toPush->get_width()==32);
        
        // Decrement stack pointer. The source operand must have been read already (i.e., "push esp" pushes the old value)
        int stackDelta = toPush->get_width() / 8;
        BaseSemantics::SValuePtr oldSp = d->readRegister(d->REG_ESP);
        BaseSemantics::SValuePtr newSp = ops->add(oldSp, ops->number_(32, -stackDelta));
        ops->writeRegister(d->REG_ESP, newSp);

        // Write data to stack
        ops->writeMemory(d->REG_SS, newSp, toPush, ops->boolean_(true));
    }
};

// Push all general-purpose registers
struct IP_pushad: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        if (insn->get_addressSize() != x86_insnsize_32)
            throw BaseSemantics::Exception("size not implemented", insn);
        BaseSemantics::SValuePtr oldSp = d->readRegister(d->REG_ESP);
        BaseSemantics::SValuePtr newSp = ops->add(oldSp, ops->number_(32, -32));
        ops->writeMemory(d->REG_SS, newSp,
                         d->readRegister(d->REG_EDI), ops->boolean_(true));
        ops->writeMemory(d->REG_SS, ops->add(newSp, ops->number_(32, 4)),
                         d->readRegister(d->REG_ESI), ops->boolean_(true));
        ops->writeMemory(d->REG_SS, ops->add(newSp, ops->number_(32, 8)),
                         d->readRegister(d->REG_EBP), ops->boolean_(true));
        ops->writeMemory(d->REG_SS, ops->add(newSp, ops->number_(32, 12)),
                         oldSp, ops->boolean_(true));
        ops->writeMemory(d->REG_SS, ops->add(newSp, ops->number_(32, 16)),
                         d->readRegister(d->REG_EBX), ops->boolean_(true));
        ops->writeMemory(d->REG_SS, ops->add(newSp, ops->number_(32, 20)),
                         d->readRegister(d->REG_EDX), ops->boolean_(true));
        ops->writeMemory(d->REG_SS, ops->add(newSp, ops->number_(32, 24)),
                         d->readRegister(d->REG_ECX), ops->boolean_(true));
        ops->writeMemory(d->REG_SS, ops->add(newSp, ops->number_(32, 28)),
                         d->readRegister(d->REG_EAX), ops->boolean_(true));
        ops->writeRegister(d->REG_ESP, newSp);
    }
};

// Push EFLAGS onto the stack
struct IP_pushfd: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        if (insn->get_addressSize() != x86_insnsize_32)
            throw BaseSemantics::Exception("size not implemented", insn);
        BaseSemantics::SValuePtr oldSp = d->readRegister(d->REG_ESP);
        BaseSemantics::SValuePtr newSp = ops->add(oldSp, ops->number_(32, -4));
        ops->writeMemory(d->REG_SS, newSp, d->readRegister(d->REG_EFLAGS), ops->boolean_(true));
        ops->writeRegister(d->REG_ESP, newSp);
    }
};

// Bitwise XOR
struct IP_pxor: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        size_t nbits = asm_type_width(args[0]->get_type());
        if (nbits!=asm_type_width(args[1]->get_type()))
            throw BaseSemantics::Exception("PXOR operands must be the same width", insn);
        BaseSemantics::SValuePtr result;

        // XOR of a register with itself is an x86 idiom for setting the register to zero, so treat it as such
        if (isSgAsmRegisterReferenceExpression(args[0]) && isSgAsmRegisterReferenceExpression(args[1])) {
            RegisterDescriptor r1 = isSgAsmRegisterReferenceExpression(args[0])->get_descriptor();
            RegisterDescriptor r2 = isSgAsmRegisterReferenceExpression(args[1])->get_descriptor();
            if (r1==r2)
                result = ops->number_(nbits, 0);
        }

        // The non-idiomatic behavior
        if (result==NULL)
            result = ops->xor_(d->read(args[0], nbits), d->read(args[1], nbits));

        d->write(args[0], result);
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
        BaseSemantics::SValuePtr oldSp = d->readRegister(d->REG_ESP);
        BaseSemantics::SValuePtr newSp = ops->add(oldSp, ops->add(ops->number_(32, 4), extraBytes));
        ops->writeRegister(d->REG_anyIP, ops->filterReturnTarget(ops->readMemory(d->REG_SS, oldSp,
                                                                                 ops->undefined_(d->REG_anyIP.get_nbits()),
                                                                                 ops->boolean_(true))));
        ops->writeRegister(d->REG_ESP, newSp);
    }
};

// Various rotate instructions. Handles RCL, RCR, ROL, and ROR for 8, 16, 32, and 64 bits
struct IP_rotate: P {
    const X86InstructionKind kind;
    const bool with_cf;
    IP_rotate(X86InstructionKind k): kind(k), with_cf(x86_rcl==kind || x86_rcr==kind) {
        ASSERT_require(x86_rcl==k || x86_rcr==k || x86_rol==k || x86_ror==k);
    }
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        ASSERT_require(insn->get_kind()==kind);
        size_t nbits = asm_type_width(args[0]->get_type());
        // FIXME: Intel documentation contains conflicting statements about the number of significant bits in the rotate count.
        // We're doing what seems most reasonable: 6-bit counts for any operand (inc. CF) that's wider than 32 bits.
        size_t rotateWidth = (nbits>32 || (32==nbits && with_cf)) ? 6 : 5;
        BaseSemantics::SValuePtr operand = d->read(args[0], nbits);
        if (with_cf)
            operand = ops->concat(operand, d->readRegister(d->REG_CF));
        BaseSemantics::SValuePtr rotateCount = d->read(args[1], 8);
        BaseSemantics::SValuePtr result = d->doRotateOperation(kind, operand, rotateCount, rotateWidth);
        // flags have been updated; we just need to store the result
        if (with_cf)
            result = ops->extract(result, 0, nbits);
        d->write(args[0], result);
    }
};

// Integer subtraction with borrow
struct IP_sbb: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        size_t nbits = asm_type_width(args[0]->get_type());
        BaseSemantics::SValuePtr result = d->doAddOperation(d->read(args[0], nbits), ops->invert(d->read(args[1], nbits)),
                                                            true, d->readRegister(d->REG_CF));
        d->write(args[0], result);
    }
};

// Scan string
struct IP_scanstring: P {
    const X86RepeatPrefix repeat;
    const size_t nbits;
    const size_t nbytes;
    IP_scanstring(X86RepeatPrefix repeat, size_t nbits): repeat(repeat), nbits(nbits), nbytes(nbits/8) {
        ASSERT_require(8==nbits || 16==nbits || 32==nbits);
    }
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        if (insn->get_addressSize() != x86_insnsize_32)
            throw BaseSemantics::Exception("size not implemented", insn);
        BaseSemantics::SValuePtr in_loop = d->repEnter(repeat);
        RegisterDescriptor regA = d->REG_EAX; regA.set_nbits(nbits);
        BaseSemantics::SValuePtr value = ops->readMemory(d->REG_ES, d->readRegister(d->REG_EDI),
                                                         ops->undefined_(nbits), in_loop);
        d->doAddOperation(d->readRegister(regA), ops->invert(value), true, ops->boolean_(false), in_loop);
        BaseSemantics::SValuePtr step = ops->ite(d->readRegister(d->REG_DF),
                                                 ops->number_(32, -nbytes), ops->number_(32, nbytes));
        ops->writeRegister(d->REG_EDI, 
                           ops->ite(in_loop,
                                    ops->add(d->readRegister(d->REG_EDI), step),
                                    d->readRegister(d->REG_EDI)));
        if (x86_repeat_none!=repeat)
            d->repLeave(repeat, in_loop, insn->get_address());
    }
};

// Set byte on condition
struct IP_setcc: P {
    const X86InstructionKind kind;
    IP_setcc(X86InstructionKind k): kind(k) {
        ASSERT_require(x86_setne==k || x86_sete==k || x86_setno==k || x86_seto==k || x86_setpo==k || x86_setpe==k ||
                       x86_setns==k || x86_sets==k || x86_setae==k || x86_setb==k || x86_setbe==k || x86_seta==k ||
                       x86_setle==k || x86_setg==k || x86_setge==k || x86_setl==k);
    }
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        ASSERT_require(insn->get_kind()==kind);
        BaseSemantics::SValuePtr cond = d->flagsCombo(kind);
        d->write(args[0], ops->concat(cond, ops->number_(7, 0)));
    }
};

// Shift instructions: SHL, SAR, and SHR
struct IP_shift: P {
    const X86InstructionKind kind;
    IP_shift(X86InstructionKind k): kind(k) {
        ASSERT_require(x86_shr==k || x86_sar==k || x86_shl==k || x86_shld==k || x86_shrd==k);
    }
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        ASSERT_require(insn->get_kind()==kind);
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
        ASSERT_require(8==nbits || 16==nbits || 32==nbits);
    }
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        if (insn->get_addressSize()!=x86_insnsize_32)
            throw BaseSemantics::Exception("address size must be 32 bits", insn);
        BaseSemantics::SValuePtr in_loop = d->repEnter(repeat);
        RegisterDescriptor regA = d->REG_EAX; regA.set_nbits(nbits);
        ops->writeMemory(d->REG_ES, d->readRegister(d->REG_EDI), d->readRegister(regA), in_loop);
        BaseSemantics::SValuePtr step = ops->ite(d->readRegister(d->REG_DF),
                                                 ops->number_(32, -nbytes), ops->number_(32, nbytes));

        /* Update DI */
        ops->writeRegister(d->REG_EDI,
                           ops->ite(in_loop,
                                    ops->add(d->readRegister(d->REG_EDI), step),
                                    d->readRegister(d->REG_EDI)));
        if (x86_repeat_none!=repeat)
            d->repLeave(repeat, in_loop, insn->get_address());
    }
};

// Store SSE control and status register
struct IP_stmxcsr: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        size_t nbits = asm_type_width(args[0]->get_type());
        if (nbits!=32)
            throw BaseSemantics::Exception("STMXCSR operand must be 32 bits", insn);
        d->write(args[0], d->readRegister(d->REG_MXCSR));
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
        ops->interrupt(1, 0); // 1 indicates a SYSENTER instruction as opposed to an INT instruction
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

// Wait (check for and handle unmasked floating-point exceptions)
struct IP_wait: P {
    void p(D d, Ops ops, I insn, A args) {
        // FIXME[Robb P. Matzke 2014-05-12]: currenty a no-op
        assert_args(insn, args, 0);
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
        BaseSemantics::SValuePtr result;

        // XOR of a register with itself is an x86 idiom for setting the register to zero, so treat it as such
        if (isSgAsmRegisterReferenceExpression(args[0]) && isSgAsmRegisterReferenceExpression(args[1])) {
            RegisterDescriptor r1 = isSgAsmRegisterReferenceExpression(args[0])->get_descriptor();
            RegisterDescriptor r2 = isSgAsmRegisterReferenceExpression(args[1])->get_descriptor();
            if (r1==r2)
                result = ops->number_(nbits, 0);
        }

        // The non-idiomatic behavior
        if (result==NULL)
            result = ops->xor_(d->read(args[0], nbits), d->read(args[1], nbits));

        d->setFlagsForResult(result);
        d->write(args[0], result);
        ops->writeRegister(d->REG_OF, ops->boolean_(false));
        ops->writeRegister(d->REG_AF, ops->undefined_(1));
        ops->writeRegister(d->REG_CF, ops->boolean_(false));
    }
};

} // namespace

/*******************************************************************************************************************************
 *                                      DispatcherX86
 *******************************************************************************************************************************/

void
DispatcherX86::iproc_init()
{
    iproc_set(x86_aaa,          new X86::IP_aaa);
    iproc_set(x86_aad,          new X86::IP_aad);
    iproc_set(x86_aam,          new X86::IP_aam);
    iproc_set(x86_aas,          new X86::IP_aas);
    iproc_set(x86_adc,          new X86::IP_adc);
    iproc_set(x86_add,          new X86::IP_add);
    iproc_set(x86_and,          new X86::IP_and);
    iproc_set(x86_bsf,          new X86::IP_bitscan(x86_bsf));
    iproc_set(x86_bsr,          new X86::IP_bitscan(x86_bsr));
    iproc_set(x86_bswap,        new X86::IP_bswap);
    iproc_set(x86_bt,           new X86::IP_bittest(x86_bt));
    iproc_set(x86_btc,          new X86::IP_bittest(x86_btc));
    iproc_set(x86_btr,          new X86::IP_bittest(x86_btr));
    iproc_set(x86_bts,          new X86::IP_bittest(x86_bts));
    iproc_set(x86_call,         new X86::IP_call);
    iproc_set(x86_cbw,          new X86::IP_cbw);
    iproc_set(x86_cdq,          new X86::IP_cdq);
    iproc_set(x86_cdqe,         new X86::IP_cdqe);
    iproc_set(x86_clc,          new X86::IP_clc);
    iproc_set(x86_cld,          new X86::IP_cld);
    iproc_set(x86_clflush,      new X86::IP_clflush);
    iproc_set(x86_cmc,          new X86::IP_cmc);
    iproc_set(x86_cmova,        new X86::IP_cmovcc(x86_cmova));
    iproc_set(x86_cmovae,       new X86::IP_cmovcc(x86_cmovae));
    iproc_set(x86_cmovb,        new X86::IP_cmovcc(x86_cmovb));
    iproc_set(x86_cmovbe,       new X86::IP_cmovcc(x86_cmovbe));
    iproc_set(x86_cmove,        new X86::IP_cmovcc(x86_cmove));
    iproc_set(x86_cmovg,        new X86::IP_cmovcc(x86_cmovg));
    iproc_set(x86_cmovge,       new X86::IP_cmovcc(x86_cmovge));
    iproc_set(x86_cmovl,        new X86::IP_cmovcc(x86_cmovl));
    iproc_set(x86_cmovle,       new X86::IP_cmovcc(x86_cmovle));
    iproc_set(x86_cmovne,       new X86::IP_cmovcc(x86_cmovne));
    iproc_set(x86_cmovno,       new X86::IP_cmovcc(x86_cmovno));
    iproc_set(x86_cmovns,       new X86::IP_cmovcc(x86_cmovns));
    iproc_set(x86_cmovo,        new X86::IP_cmovcc(x86_cmovo));
    iproc_set(x86_cmovpe,       new X86::IP_cmovcc(x86_cmovpe));
    iproc_set(x86_cmovpo,       new X86::IP_cmovcc(x86_cmovpo));
    iproc_set(x86_cmovs,        new X86::IP_cmovcc(x86_cmovs));
    iproc_set(x86_cmp,          new X86::IP_cmp);
    iproc_set(x86_cmpsb,        new X86::IP_cmpstrings(x86_repeat_none, 8));
    iproc_set(x86_cmpsw,        new X86::IP_cmpstrings(x86_repeat_none, 16));
    iproc_set(x86_cmpsd,        new X86::IP_cmpstrings(x86_repeat_none, 32)); // FIXME: also a floating point instruction
    iproc_set(x86_cmpxchg,      new X86::IP_cmpxchg);
    iproc_set(x86_cpuid,        new X86::IP_cpuid);
    iproc_set(x86_cwd,          new X86::IP_cwd);
    iproc_set(x86_cwde,         new X86::IP_cwde);
    iproc_set(x86_dec,          new X86::IP_dec);
    iproc_set(x86_div,          new X86::IP_divide(x86_div));
    iproc_set(x86_fld,          new X86::IP_fld);
    iproc_set(x86_fldcw,        new X86::IP_fldcw);
    iproc_set(x86_fnstcw,       new X86::IP_fnstcw);
    iproc_set(x86_fnstsw,       new X86::IP_fnstsw);
    iproc_set(x86_fst,          new X86::IP_fst);
    iproc_set(x86_fstp,         new X86::IP_fstp);
    iproc_set(x86_hlt,          new X86::IP_hlt);
    iproc_set(x86_idiv,         new X86::IP_divide(x86_idiv));
    iproc_set(x86_imul,         new X86::IP_imul);
    iproc_set(x86_inc,          new X86::IP_inc);
    iproc_set(x86_int,          new X86::IP_int);
    iproc_set(x86_ja,           new X86::IP_jcc(x86_ja));
    iproc_set(x86_jae,          new X86::IP_jcc(x86_jae));
    iproc_set(x86_jb,           new X86::IP_jcc(x86_jb));
    iproc_set(x86_jbe,          new X86::IP_jcc(x86_jbe));
    iproc_set(x86_jcxz,         new X86::IP_jcc(x86_jcxz));
    iproc_set(x86_je,           new X86::IP_jcc(x86_je));
    iproc_set(x86_jecxz,        new X86::IP_jcc(x86_jecxz));
    iproc_set(x86_jg,           new X86::IP_jcc(x86_jg));
    iproc_set(x86_jge,          new X86::IP_jcc(x86_jge));
    iproc_set(x86_jl,           new X86::IP_jcc(x86_jl));
    iproc_set(x86_jle,          new X86::IP_jcc(x86_jle));
    iproc_set(x86_jmp,          new X86::IP_jmp);
    iproc_set(x86_jne,          new X86::IP_jcc(x86_jne));
    iproc_set(x86_jno,          new X86::IP_jcc(x86_jno));
    iproc_set(x86_jns,          new X86::IP_jcc(x86_jns));
    iproc_set(x86_jo,           new X86::IP_jcc(x86_jo));
    iproc_set(x86_jpe,          new X86::IP_jcc(x86_jpe));
    iproc_set(x86_jpo,          new X86::IP_jcc(x86_jpo));
    iproc_set(x86_js,           new X86::IP_jcc(x86_js));
    iproc_set(x86_lea,          new X86::IP_lea);
    iproc_set(x86_leave,        new X86::IP_leave);
    iproc_set(x86_lodsb,        new X86::IP_loadstring(x86_repeat_none, 8));
    iproc_set(x86_lodsw,        new X86::IP_loadstring(x86_repeat_none, 16));
    iproc_set(x86_lodsd,        new X86::IP_loadstring(x86_repeat_none, 32));
    iproc_set(x86_loop,         new X86::IP_loop(x86_loop));
    iproc_set(x86_loopnz,       new X86::IP_loop(x86_loopnz));
    iproc_set(x86_loopz,        new X86::IP_loop(x86_loopz));
    iproc_set(x86_mov,          new X86::IP_mov);
    iproc_set(x86_movd,         new X86::IP_move_extend);
    iproc_set(x86_movdqa,       new X86::IP_movdqa);
    iproc_set(x86_movq,         new X86::IP_move_extend);
    iproc_set(x86_movsb,        new X86::IP_movestring(x86_repeat_none, 8));
    iproc_set(x86_movsw,        new X86::IP_movestring(x86_repeat_none, 16));
    iproc_set(x86_movsd,        new X86::IP_movestring(x86_repeat_none, 32));
    iproc_set(x86_movsx,        new X86::IP_movsx);
    iproc_set(x86_movzx,        new X86::IP_movzx);
    iproc_set(x86_mul,          new X86::IP_mul);
    iproc_set(x86_neg,          new X86::IP_neg);
    iproc_set(x86_nop,          new X86::IP_nop);
    iproc_set(x86_not,          new X86::IP_not);
    iproc_set(x86_or,           new X86::IP_or);
    iproc_set(x86_palignr,      new X86::IP_palignr);
    iproc_set(x86_pop,          new X86::IP_pop);
    iproc_set(x86_popad,        new X86::IP_popad);
    iproc_set(x86_push,         new X86::IP_push);
    iproc_set(x86_pushad,       new X86::IP_pushad);
    iproc_set(x86_pushfd,       new X86::IP_pushfd);
    iproc_set(x86_pxor,         new X86::IP_pxor);
    iproc_set(x86_rcl,          new X86::IP_rotate(x86_rcl));
    iproc_set(x86_rcr,          new X86::IP_rotate(x86_rcr));
    iproc_set(x86_rdtsc,        new X86::IP_rdtsc);
    iproc_set(x86_rep_lodsb,    new X86::IP_loadstring(x86_repeat_repe, 8));
    iproc_set(x86_rep_lodsw,    new X86::IP_loadstring(x86_repeat_repe, 16));
    iproc_set(x86_rep_lodsd,    new X86::IP_loadstring(x86_repeat_repe, 32));
    iproc_set(x86_rep_movsb,    new X86::IP_movestring(x86_repeat_repe, 8));
    iproc_set(x86_rep_movsw,    new X86::IP_movestring(x86_repeat_repe, 16));
    iproc_set(x86_rep_movsd,    new X86::IP_movestring(x86_repeat_repe, 32));
    iproc_set(x86_rep_stosb,    new X86::IP_storestring(x86_repeat_repe, 8));
    iproc_set(x86_rep_stosw,    new X86::IP_storestring(x86_repeat_repe, 16));
    iproc_set(x86_rep_stosd,    new X86::IP_storestring(x86_repeat_repe, 32));
    iproc_set(x86_repe_cmpsb,   new X86::IP_cmpstrings(x86_repeat_repe, 8));
    iproc_set(x86_repe_cmpsw,   new X86::IP_cmpstrings(x86_repeat_repe, 16));
    iproc_set(x86_repe_cmpsd,   new X86::IP_cmpstrings(x86_repeat_repe, 32));
    iproc_set(x86_repe_scasb,   new X86::IP_scanstring(x86_repeat_repe, 8));
    iproc_set(x86_repe_scasw,   new X86::IP_scanstring(x86_repeat_repe, 16));
    iproc_set(x86_repe_scasd,   new X86::IP_scanstring(x86_repeat_repe, 32));
    iproc_set(x86_repne_cmpsb,  new X86::IP_cmpstrings(x86_repeat_repne, 8));
    iproc_set(x86_repne_cmpsw,  new X86::IP_cmpstrings(x86_repeat_repne, 16));
    iproc_set(x86_repne_cmpsd,  new X86::IP_cmpstrings(x86_repeat_repne, 32));
    iproc_set(x86_repne_scasb,  new X86::IP_scanstring(x86_repeat_repne, 8));
    iproc_set(x86_repne_scasw,  new X86::IP_scanstring(x86_repeat_repne, 16));
    iproc_set(x86_repne_scasd,  new X86::IP_scanstring(x86_repeat_repne, 32));
    iproc_set(x86_ret,          new X86::IP_ret);
    iproc_set(x86_rol,          new X86::IP_rotate(x86_rol));
    iproc_set(x86_ror,          new X86::IP_rotate(x86_ror));
    iproc_set(x86_sar,          new X86::IP_shift(x86_sar));
    iproc_set(x86_sbb,          new X86::IP_sbb);
    iproc_set(x86_scasb,        new X86::IP_scanstring(x86_repeat_none, 8));
    iproc_set(x86_scasw,        new X86::IP_scanstring(x86_repeat_none, 16));
    iproc_set(x86_scasd,        new X86::IP_scanstring(x86_repeat_none, 32));
    iproc_set(x86_seta,         new X86::IP_setcc(x86_seta));
    iproc_set(x86_setae,        new X86::IP_setcc(x86_setae));
    iproc_set(x86_setb,         new X86::IP_setcc(x86_setb));
    iproc_set(x86_setbe,        new X86::IP_setcc(x86_setbe));
    iproc_set(x86_sete,         new X86::IP_setcc(x86_sete));
    iproc_set(x86_setg,         new X86::IP_setcc(x86_setg));
    iproc_set(x86_setge,        new X86::IP_setcc(x86_setge));
    iproc_set(x86_setl,         new X86::IP_setcc(x86_setl));
    iproc_set(x86_setle,        new X86::IP_setcc(x86_setle));
    iproc_set(x86_setne,        new X86::IP_setcc(x86_setne));
    iproc_set(x86_setno,        new X86::IP_setcc(x86_setno));
    iproc_set(x86_setns,        new X86::IP_setcc(x86_setns));
    iproc_set(x86_seto,         new X86::IP_setcc(x86_seto));
    iproc_set(x86_setpe,        new X86::IP_setcc(x86_setpe));
    iproc_set(x86_setpo,        new X86::IP_setcc(x86_setpo));
    iproc_set(x86_sets,         new X86::IP_setcc(x86_sets));
    iproc_set(x86_shl,          new X86::IP_shift(x86_shl));
    iproc_set(x86_shld,         new X86::IP_shift(x86_shld));
    iproc_set(x86_shr,          new X86::IP_shift(x86_shr));
    iproc_set(x86_shrd,         new X86::IP_shift(x86_shrd));
    iproc_set(x86_stc,          new X86::IP_stc);
    iproc_set(x86_std,          new X86::IP_std);
    iproc_set(x86_stosb,        new X86::IP_storestring(x86_repeat_none, 8));
    iproc_set(x86_stosw,        new X86::IP_storestring(x86_repeat_none, 16));
    iproc_set(x86_stosd,        new X86::IP_storestring(x86_repeat_none, 32));
    iproc_set(x86_stmxcsr,      new X86::IP_stmxcsr);
    iproc_set(x86_sub,          new X86::IP_sub);
    iproc_set(x86_sysenter,     new X86::IP_sysenter);
    iproc_set(x86_test,         new X86::IP_test);
    iproc_set(x86_wait,         new X86::IP_wait);
    iproc_set(x86_xadd,         new X86::IP_xadd);
    iproc_set(x86_xchg,         new X86::IP_xchg);
    iproc_set(x86_xor,          new X86::IP_xor);
}

void
DispatcherX86::regcache_init()
{
    if (regdict) {
        switch (processorMode()) {
            case x86_insnsize_64:
                REG_RAX = findRegister("rax", 64);
                REG_RDI = findRegister("rdi", 64);
                REG_RSI = findRegister("rsi", 64);
                // fall through...
            case x86_insnsize_32:
                REG_EAX = findRegister("eax", 32);
                REG_EBX = findRegister("ebx", 32);
                REG_ECX = findRegister("ecx", 32);
                REG_EDX = findRegister("edx", 32);
                REG_EDI = findRegister("edi", 32);
                REG_ESI = findRegister("esi", 32);
                REG_ESP = findRegister("esp", 32);
                REG_EBP = findRegister("ebp", 32);
                REG_EFLAGS= findRegister("eflags", 32);
                REG_ST0 = findRegister("st0", 80);
                REG_FPSTATUS = findRegister("fpstatus", 16);
                REG_FPSTATUS_TOP = findRegister("fpstatus_top", 3);
                REG_FPCTL = findRegister("fpctl", 16);
                REG_MXCSR = findRegister("mxcsr", 32);
                // fall through...
            case x86_insnsize_16:
                REG_AX = findRegister("ax", 16);
                REG_CX = findRegister("cx", 16);
                REG_DX = findRegister("dx", 16);
                REG_DI = findRegister("di", 16);
                REG_SI = findRegister("si", 16);
                REG_AL = findRegister("al", 8);
                REG_AH = findRegister("ah", 8);
                REG_AF = findRegister("af", 1);
                REG_CF = findRegister("cf", 1);
                REG_DF = findRegister("df", 1);
                REG_OF = findRegister("of", 1);
                REG_PF = findRegister("pf", 1);
                REG_SF = findRegister("sf", 1);
                REG_ZF = findRegister("zf", 1);
                REG_DS = findRegister("ds", 16);
                REG_ES = findRegister("es", 16);
                REG_SS = findRegister("ss", 16);
                break;
            default:
                ASSERT_not_reachable("invalid instruction size");
        }

        REG_anyIP = regdict->findLargestRegister(x86_regclass_ip, 0);
        REG_anySP = regdict->findLargestRegister(x86_regclass_gpr, x86_gpr_sp);
        REG_anyCX = regdict->findLargestRegister(x86_regclass_gpr, x86_gpr_cx);
    }
}

static bool
isStatusRegister(const RegisterDescriptor &reg) {
    return reg.get_major()==x86_regclass_flags && reg.get_minor()==x86_flags_status;
}

RegisterDictionary::RegisterDescriptors
DispatcherX86::get_usual_registers() const
{
    RegisterDictionary::RegisterDescriptors registers = regdict->get_largest_registers();
    registers.erase(std::remove_if(registers.begin(), registers.end(), isStatusRegister), registers.end());
    BOOST_FOREACH (const RegisterDescriptor &reg, regdict->get_smallest_registers()) {
        if (isStatusRegister(reg))
            registers.push_back(reg);
    }
    return registers;
}

void
DispatcherX86::set_register_dictionary(const RegisterDictionary *regdict)
{
    BaseSemantics::Dispatcher::set_register_dictionary(regdict);
    regcache_init();
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
    ASSERT_require(cond->get_width()==1);
    BaseSemantics::SValuePtr lo_byte = operators->extract(result, 0, 8);
    BaseSemantics::SValuePtr signbit = operators->extract(result, result->get_width()-1, result->get_width());
    operators->writeRegister(REG_PF, operators->ite(cond, parity(lo_byte), operators->readRegister(REG_PF)));
    operators->writeRegister(REG_SF, operators->ite(cond, signbit, operators->readRegister(REG_SF)));
    operators->writeRegister(REG_ZF, operators->ite(cond, operators->equalToZero(result), operators->readRegister(REG_ZF)));
}

BaseSemantics::SValuePtr
DispatcherX86::parity(const BaseSemantics::SValuePtr &v)
{
    ASSERT_require(v->get_width()==8);
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
            ASSERT_not_reachable("instruction kind not handled");
        }
}

BaseSemantics::SValuePtr
DispatcherX86::repEnter(X86RepeatPrefix repeat)
{
    if (repeat==x86_repeat_none)
        return operators->boolean_(true);
    BaseSemantics::SValuePtr cx = operators->readRegister(REG_anyCX);
    BaseSemantics::SValuePtr in_loop = operators->invert(operators->equalToZero(cx));
    return in_loop;
}

void
DispatcherX86::repLeave(X86RepeatPrefix repeat_prefix, const BaseSemantics::SValuePtr &in_loop, rose_addr_t insn_va)
{
    ASSERT_require(in_loop!=NULL && in_loop->get_width()==1);

    // conditionally decrement the CX register
    BaseSemantics::SValuePtr new_cx = operators->add(operators->readRegister(REG_anyCX),
                                                     operators->ite(in_loop,
                                                                    operators->number_(REG_anyCX.get_nbits(), -1),
                                                                    operators->number_(REG_anyCX.get_nbits(),  0)));
    operators->writeRegister(REG_anyCX, new_cx);
    BaseSemantics::SValuePtr nonzero_cx = operators->invert(operators->equalToZero(new_cx));

    // determine whether we should repeat the instruction.
    BaseSemantics::SValuePtr again;
    switch (repeat_prefix) {
        case x86_repeat_none:
            again = operators->boolean_(false);
            break;
        case x86_repeat_repe:
            again = operators->and_(operators->and_(in_loop, nonzero_cx),
                                    operators->readRegister(REG_ZF));

            break;
        case x86_repeat_repne:
            again = operators->and_(operators->and_(in_loop, nonzero_cx),
                                    operators->invert(operators->readRegister(REG_ZF)));
            break;
    }
    operators->writeRegister(REG_anyIP,
                             operators->ite(again,
                                            operators->number_(REG_anyIP.get_nbits(), insn_va),    // repeat
                                            operators->readRegister(REG_anyIP))); // exit loop
}

BaseSemantics::SValuePtr
DispatcherX86::doAddOperation(BaseSemantics::SValuePtr a, BaseSemantics::SValuePtr b,
                              bool invertCarries, const BaseSemantics::SValuePtr &carryIn)
{
    if (a->get_width() > b->get_width()) {
        b = operators->signExtend(b, a->get_width());
    } else if (a->get_width() < b->get_width()) {
        a = operators->signExtend(a, b->get_width());
    }

    ASSERT_require(1==carryIn->get_width());
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
DispatcherX86::doAddOperation(BaseSemantics::SValuePtr a, BaseSemantics::SValuePtr b,
                              bool invertCarries, const BaseSemantics::SValuePtr &carryIn,
                              const BaseSemantics::SValuePtr &cond)
{
    if (a->get_width() > b->get_width()) {
        b = operators->signExtend(b, a->get_width());
    } else if (a->get_width() < b->get_width()) {
        a = operators->signExtend(a, b->get_width());
    }

    ASSERT_require(1==carryIn->get_width());
    ASSERT_require(cond!=NULL && cond->get_width()==1);
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

BaseSemantics::SValuePtr
DispatcherX86::doRotateOperation(X86InstructionKind kind, const BaseSemantics::SValuePtr &operand,
                                 const BaseSemantics::SValuePtr &total_rotate, size_t rotateSignificantBits)
{
    ASSERT_require(x86_rcl==kind || x86_rcr==kind || x86_rol==kind || x86_ror==kind);
    ASSERT_require(total_rotate->get_width()==8 && rotateSignificantBits<8);

    // The 8086 does not mask the rotate count; processors starting with the 80286 (including virtual-8086 mode) do mask. We
    // will always mask. The effect (other than timing) is the same either way.
    BaseSemantics::SValuePtr maskedRotateCount = operators->extract(total_rotate, 0, rotateSignificantBits);
    if (operand->get_width()==9 || operand->get_width()==17) { //  RCL or RCR on an 8- or 16-bit operand
        maskedRotateCount = operators->unsignedModulo(maskedRotateCount,
                                                      number_(maskedRotateCount->get_width(), operand->get_width()));
    }
    BaseSemantics::SValuePtr isZeroRotateCount = operators->equalToZero(maskedRotateCount);

    // isOneBitRotate is true if the (masked) amount by which to rotate is equal to one.
    uintmax_t m = IntegerOps::genMask<uintmax_t>(rotateSignificantBits);
    BaseSemantics::SValuePtr mask = number_(rotateSignificantBits, m); // -1 in modulo arithmetic
    BaseSemantics::SValuePtr isOneBitRotate = operators->equalToZero(operators->add(maskedRotateCount, mask));

    // Do the actual rotate.
    BaseSemantics::SValuePtr result;
    switch (kind) {
        case x86_rcl:
        case x86_rol:
            result = operators->rotateLeft(operand, maskedRotateCount);
            break;
        case x86_rcr:
        case x86_ror:
            result = operators->rotateRight(operand, maskedRotateCount);
            break;
        default:
            ASSERT_not_reachable("instruction not handled");
    }
    ASSERT_require(result->get_width()==operand->get_width());

    // Compute the new CF value.
    BaseSemantics::SValuePtr new_cf;
    switch (kind) {
        case x86_rcl:
        case x86_rcr:
        case x86_ror:
            new_cf = operators->extract(result, result->get_width()-1, result->get_width());
            break;
        case x86_rol:
            new_cf = operators->extract(result, 0, 1);
            break;
        default:
            ASSERT_not_reachable("instruction not handled");
    }

    // Compute the new OF value.  The new OF value is only used for 1-bit rotates.
    BaseSemantics::SValuePtr new_of;
    switch (kind) {
        case x86_rcl:
        case x86_ror:
            new_of = operators->xor_(operators->extract(result, result->get_width()-1, result->get_width()),
                                     operators->extract(result, result->get_width()-2, result->get_width()-1));
            break;
        case x86_rcr:
            new_of = operators->xor_(operators->extract(operand, operand->get_width()-1, operand->get_width()),
                                     operators->extract(operand, operand->get_width()-2, operand->get_width()-1));
            break;
        case x86_rol:
            new_of = operators->xor_(new_cf,
                                     operators->extract(result, result->get_width()-1, result->get_width()));
            break;
        default:
            ASSERT_not_reachable("instruction not handled");
    }

    // Update CF and OF flags. SF, ZF, AF, and PF are not affected.
    operators->writeRegister(REG_CF, new_cf);
    operators->writeRegister(REG_OF, operators->ite(isZeroRotateCount,
                                                    operators->readRegister(REG_OF),
                                                    operators->ite(isOneBitRotate, new_of, undefined_(1))));

    return result;
}

BaseSemantics::SValuePtr
DispatcherX86::doShiftOperation(X86InstructionKind kind, const BaseSemantics::SValuePtr &operand,
                                const BaseSemantics::SValuePtr &source_bits, const BaseSemantics::SValuePtr &total_shift,
                                size_t shiftSignificantBits)
{
    ASSERT_require(x86_shr==kind || x86_sar==kind || x86_shl==kind || x86_shld==kind || x86_shrd==kind);
    ASSERT_require(operand->get_width()==source_bits->get_width());
    ASSERT_require(total_shift->get_width()==8 && shiftSignificantBits<8);

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
            ASSERT_not_reachable("instruction not handled");
    }
    ASSERT_require(operand->get_width()==result->get_width());
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

BaseSemantics::SValuePtr
DispatcherX86::readRegister(const RegisterDescriptor &reg) {
    // When reading FLAGS, EFLAGS as a whole do not coalesce individual flags into the single register.
    if (reg.get_major()==x86_regclass_flags && reg.get_offset()==0 && reg.get_nbits()>1) {
        BaseSemantics::RegisterStatePtr rs = operators->get_state()->get_register_state();
        if (BaseSemantics::RegisterStateGeneric *rsg = dynamic_cast<BaseSemantics::RegisterStateGeneric*>(rs.get())) {
            BaseSemantics::RegisterStateGeneric::NoCoalesceOnRead guard(rsg);
            return operators->readRegister(reg);
        }
    }
    return operators->readRegister(reg);
}

void
DispatcherX86::write(SgAsmExpression *e, const BaseSemantics::SValuePtr &value, size_t addr_nbits/*=0*/)
{
    if (SgAsmDirectRegisterExpression *re = isSgAsmDirectRegisterExpression(e)) {
        RegisterDescriptor reg = re->get_descriptor();
        if (reg.get_major()==x86_regclass_st && reg.get_minor()>=0 && reg.get_minor()<8 &&
            reg.get_offset()==0 && reg.get_nbits()==64) {
            // When writing to an MM register, the high-order 16 bits are set in order to make the underlying
            // ST register NaN.
            reg.set_nbits(80);
            operators->writeRegister(reg, operators->concat(value, number_(16, 0xffff)));
            return;
        }
    }
    Dispatcher::write(e, value, addr_nbits);            // defer to super class
}

void
DispatcherX86::pushFloatingPoint(const BaseSemantics::SValuePtr &value)
{
    BaseSemantics::SValuePtr topOfStack = operators->readRegister(REG_FPSTATUS_TOP);
    if (!topOfStack->is_number())
        throw BaseSemantics::Exception("FP-stack top is not concrete", NULL);
    BaseSemantics::SValuePtr newTopOfStack = operators->add(topOfStack, operators->number_(topOfStack->get_width(), -1));
    ASSERT_require2(newTopOfStack->is_number(), "constant folding is required for FP-stack");

    RegisterDescriptor reg(REG_ST0.get_major(), (REG_ST0.get_minor() + newTopOfStack->get_number()) % 8,
                           REG_ST0.get_offset(), REG_ST0.get_nbits());
    operators->writeRegister(reg, value);
    operators->writeRegister(REG_FPSTATUS_TOP, newTopOfStack);
}

BaseSemantics::SValuePtr
DispatcherX86::readFloatingPointStack(size_t position)
{
    BaseSemantics::SValuePtr topOfStack = operators->readRegister(REG_FPSTATUS_TOP);
    if (!topOfStack->is_number())
        throw BaseSemantics::Exception("FP-stack top is not concrete", NULL);
    RegisterDescriptor reg(REG_ST0.get_major(), (REG_ST0.get_minor() + topOfStack->get_number()) % 8,
                           REG_ST0.get_offset(), REG_ST0.get_nbits());
    return operators->readRegister(reg);
}

void
DispatcherX86::popFloatingPoint()
{
    BaseSemantics::SValuePtr topOfStack = operators->readRegister(REG_FPSTATUS_TOP);
    if (!topOfStack->is_number())
        throw BaseSemantics::Exception("FP-stack top is not concrete", NULL);
    BaseSemantics::SValuePtr newTopOfStack = operators->add(topOfStack, operators->number_(topOfStack->get_width(), 1));
    ASSERT_require2(newTopOfStack->is_number(), "constant folding is required for FP-stack");
    operators->writeRegister(REG_FPSTATUS_TOP, newTopOfStack);
}

} // namespace
} // namespace
} // namespace
