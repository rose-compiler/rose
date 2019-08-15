#include "sage3basic.h"
#include "BaseSemantics2.h"
#include "Diagnostics.h"
#include "DispatcherX86.h"
#include "RegisterStateGeneric.h"
#include "integerOps.h"

#undef si_value                                         // name pollution from siginfo.h

using namespace Rose::Diagnostics;

namespace Rose {
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
 *                                      Base x86 instruction processor
 *******************************************************************************************************************************/
namespace X86 {

void
InsnProcessor::process(const BaseSemantics::DispatcherPtr &dispatcher_, SgAsmInstruction *insn_) {
    DispatcherX86Ptr dispatcher = DispatcherX86::promote(dispatcher_);
    BaseSemantics::RiscOperatorsPtr operators = dispatcher->get_operators();
    SgAsmX86Instruction *insn = isSgAsmX86Instruction(insn_);
    ASSERT_require(insn!=NULL && insn==operators->currentInstruction());
    dispatcher->advanceInstructionPointer(insn);
    SgAsmExpressionPtrList &operands = insn->get_operandList()->get_operands();
    check_arg_width(dispatcher.get(), insn, operands);
    p(dispatcher.get(), operators.get(), insn, operands);
}

void
InsnProcessor::assert_args(I insn, A args, size_t nargs) {
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
void
InsnProcessor::check_arg_width(D d, I insn, A args) {
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

} // namespace


/*******************************************************************************************************************************
 *                                      Functors that handle individual x86 instructions kinds
 *******************************************************************************************************************************/
namespace X86 {

typedef InsnProcessor P;

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
                d->writeRegister(d->REG_AX, 
                                 ops->concat(ops->add(ops->ite(incAh, ops->number_(4, 6), ops->number_(4, 0)),
                                                      ops->extract(d->readRegister(d->REG_AL), 0, 4)),
                                             ops->concat(ops->number_(4, 0),
                                                         ops->add(ops->ite(incAh, ops->number_(8, 1), ops->number_(8, 0)),
                                                                  d->readRegister(d->REG_AH)))));
                d->writeRegister(d->REG_OF, ops->unspecified_(1));
                d->writeRegister(d->REG_SF, ops->unspecified_(1));
                d->writeRegister(d->REG_ZF, ops->unspecified_(1));
                d->writeRegister(d->REG_PF, ops->unspecified_(1));
                d->writeRegister(d->REG_AF, incAh);
                d->writeRegister(d->REG_CF, incAh);
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
                d->writeRegister(d->REG_AX, ops->concat(newAl, ops->number_(8, 0)));
                d->writeRegister(d->REG_OF, ops->unspecified_(1));
                d->writeRegister(d->REG_AF, ops->unspecified_(1));
                d->writeRegister(d->REG_CF, ops->unspecified_(1));
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
                d->writeRegister(d->REG_AX, ops->concat(newAl, newAh));
                d->writeRegister(d->REG_OF, ops->unspecified_(1));
                d->writeRegister(d->REG_AF, ops->unspecified_(1));
                d->writeRegister(d->REG_CF, ops->unspecified_(1));
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
                BaseSemantics::SValuePtr nybbleGte10 = d->greaterOrEqualToTen(nybble);
                BaseSemantics::SValuePtr decAh = ops->or_(d->readRegister(d->REG_AF), nybbleGte10);
                BaseSemantics::SValuePtr ah = d->readRegister(d->REG_AH);
                BaseSemantics::SValuePtr zero8 = ops->number_(8, 0);
                BaseSemantics::SValuePtr newAh = ops->add(ops->ite(decAh, ops->number_(8, -1), zero8), ah);
                BaseSemantics::SValuePtr newAhWide = ops->concat(ops->number_(4, 0), newAh);
                BaseSemantics::SValuePtr alNarrow = ops->extract(d->readRegister(d->REG_AL), 0, 4);
                BaseSemantics::SValuePtr zero4 = ops->number_(4, 0);

                d->writeRegister(d->REG_AX, 
                                 ops->concat(ops->add(ops->ite(decAh, ops->number_(4, -6), zero4), alNarrow),
                                             newAhWide));
                d->writeRegister(d->REG_OF, ops->unspecified_(1));
                d->writeRegister(d->REG_SF, ops->unspecified_(1));
                d->writeRegister(d->REG_ZF, ops->unspecified_(1));
                d->writeRegister(d->REG_PF, ops->unspecified_(1));
                d->writeRegister(d->REG_AF, decAh);
                d->writeRegister(d->REG_CF, decAh);
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
            BaseSemantics::SValuePtr cf = d->readRegister(d->REG_CF);
            BaseSemantics::SValuePtr arg1 = d->read(args[1]);
            BaseSemantics::SValuePtr result = d->doAddOperation(d->read(args[0]), arg1, false, cf);
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
            BaseSemantics::SValuePtr no = ops->boolean_(false);
            BaseSemantics::SValuePtr arg1 = d->read(args[1]);
            BaseSemantics::SValuePtr result = d->doAddOperation(d->read(args[0]), arg1, false, no);
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
            d->writeRegister(d->REG_OF, ops->boolean_(false));
            d->writeRegister(d->REG_AF, ops->unspecified_(1));
            d->writeRegister(d->REG_CF, ops->boolean_(false));
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
            d->writeRegister(d->REG_OF, ops->unspecified_(1));
            d->writeRegister(d->REG_SF, ops->unspecified_(1));
            d->writeRegister(d->REG_AF, ops->unspecified_(1));
            d->writeRegister(d->REG_PF, ops->unspecified_(1));
            d->writeRegister(d->REG_CF, ops->unspecified_(1));
            size_t nbits = asm_type_width(args[0]->get_type());
            BaseSemantics::SValuePtr src = d->read(args[1], nbits);
            BaseSemantics::SValuePtr isZero = ops->equalToZero(src);
            d->writeRegister(d->REG_ZF, isZero);
            BaseSemantics::SValuePtr bitno;
            ASSERT_require(insn->get_kind()==kind);
            switch (kind) {
                case x86_bsf: bitno = ops->leastSignificantSetBit(src); break;
                case x86_bsr: bitno = ops->mostSignificantSetBit(src); break;
                default: ASSERT_not_reachable("instruction kind not handled");
            }
            BaseSemantics::SValuePtr result = ops->ite(isZero, ops->unspecified_(nbits), bitno);
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
            const size_t addrSize = d->REG_anyIP.nBits();
            BaseSemantics::SValuePtr bitBase = ops->unsignedExtend(d->effectiveAddress(mre), addrSize);
            BaseSemantics::SValuePtr bitOffset = ops->signExtend(d->read(args[1]), addrSize);

            // Byte offset from bitBase address is signed. If negative, the bit is at the previous memory address. E.g.:
            //   if bitBase==100 && bitOffset==+13 then cf = mem[101] & (1<<5);
            //   if bitBase==100 && bitOffset==-13 then cf = mem[99] & (1<<5)
            // Notice that the bit offset within a byte is always positive (i.e., |bitOffset|%8) and that in the second
            // example (for bitOffset==-13) the bit offset within the memory byte is 5, not 3.  This information comes from
            // the Intel Instruction Set Reference and has not been tested by us. [Robb P. Matzke 2015-02-24]
            BaseSemantics::SValuePtr byteOffset = ops->shiftRightArithmetic(bitOffset, ops->number_(8, 3));
            BaseSemantics::SValuePtr addr = d->fixMemoryAddress(ops->add(bitBase, byteOffset));
            BaseSemantics::SValuePtr yes = ops->boolean_(true);
            BaseSemantics::SValuePtr undefA = ops->undefined_(8);
            BaseSemantics::SValuePtr fixedAddr = d->fixMemoryAddress(addr);
            BaseSemantics::SValuePtr byte = ops->readMemory(d->segmentRegister(mre), fixedAddr, undefA, yes);
            BaseSemantics::SValuePtr offsetA = ops->extract(bitOffset, 0, 3); // for positive bitOffset
            BaseSemantics::SValuePtr offsetB = ops->extract(ops->negate(bitOffset), 0, 3); // for negative bitOffset
            
            BaseSemantics::SValuePtr shiftAmount =
                ops->ite(ops->extract(bitOffset, bitOffset->get_width()-1, bitOffset->get_width()),
                         offsetB, offsetA);
            BaseSemantics::SValuePtr bit = ops->extract(ops->shiftRight(byte, shiftAmount), 0, 1);
            switch (kind) {
                case x86_bt:                            // test only
                    break;
                case x86_btr:                           // clear bit
                    byte = ops->and_(byte, ops->invert(ops->shiftLeft(ops->number_(8, 1), shiftAmount)));
                    ops->writeMemory(d->segmentRegister(mre), addr, byte, yes);
                    break;
                case x86_bts:                           // set bit
                    byte = ops->or_(byte, ops->shiftLeft(ops->number_(8, 1), shiftAmount));
                    ops->writeMemory(d->segmentRegister(mre), addr, byte, yes);
                    break;
                case x86_btc:                           // complement bit
                    byte = ops->xor_(byte, ops->shiftLeft(ops->number_(8, 1), shiftAmount));
                    ops->writeMemory(d->segmentRegister(mre), addr, byte, yes);
                    break;
                default:
                    ASSERT_not_reachable("instruction kind not handled");
            }
            d->writeRegister(d->REG_CF, bit);
            d->writeRegister(d->REG_OF, ops->unspecified_(1));
            d->writeRegister(d->REG_SF, ops->unspecified_(1));
            d->writeRegister(d->REG_ZF, ops->unspecified_(1));
            d->writeRegister(d->REG_AF, ops->unspecified_(1));
            d->writeRegister(d->REG_PF, ops->unspecified_(1));
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
            d->writeRegister(d->REG_CF, bit);
            d->writeRegister(d->REG_OF, ops->unspecified_(1));
            d->writeRegister(d->REG_SF, ops->unspecified_(1));
            d->writeRegister(d->REG_ZF, ops->unspecified_(1));
            d->writeRegister(d->REG_AF, ops->unspecified_(1));
            d->writeRegister(d->REG_PF, ops->unspecified_(1));
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
            d->write(args[0], ops->unspecified_(16));
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
            BaseSemantics::SValuePtr targetVa = ops->unsignedExtend(d->read(args[0]), d->REG_anyIP.nBits());
            
            // Push return address onto stack
            size_t nBytesPush = d->REG_anyIP.nBits() >> 3;
            BaseSemantics::SValuePtr oldSp = d->readRegister(d->REG_anySP);
            BaseSemantics::SValuePtr newSp = ops->add(oldSp, ops->number_(d->REG_anySP.nBits(), -nBytesPush));
            BaseSemantics::SValuePtr yes = ops->boolean_(true);
            BaseSemantics::SValuePtr ip = d->readRegister(d->REG_anyIP);
            ops->writeMemory(d->REG_SS, d->fixMemoryAddress(newSp), ip, yes);
            d->writeRegister(d->REG_anySP, newSp);;
            
            // Branch
            d->writeRegister(d->REG_anyIP, ops->filterCallTarget(targetVa));
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
            d->writeRegister(d->REG_AX, ops->signExtend(d->readRegister(d->REG_AL), 16));
        }
    }
};

// Sign extend EAX into EDX:EAX
struct IP_cdq: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            d->writeRegister(d->REG_EDX, ops->extract(ops->signExtend(d->readRegister(d->REG_EAX), 64), 32, 64));
        }
    }
};

// Sign extend EAX to RAX
struct IP_cdqe: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            d->writeRegister(d->REG_RAX, ops->signExtend(d->readRegister(d->REG_EAX), 64));
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
            d->writeRegister(d->REG_CF, ops->boolean_(false));
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
            d->writeRegister(d->REG_DF, ops->boolean_(false));
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
            d->writeRegister(d->REG_CF, ops->invert(d->readRegister(d->REG_CF)));
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
            BaseSemantics::SValuePtr arg0 = d->read(args[0]);
            BaseSemantics::SValuePtr arg1 = d->read(args[1]);
            d->write(args[0], ops->ite(d->flagsCombo(kind), arg1, arg0));
        }
    }
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
            BaseSemantics::SValuePtr no = ops->boolean_(false);
            (void) d->doAddOperation(a, ops->invert(b), true, no);
        }
    }
};

// Compare strings
// CMPSD is also a floating-point instruction when it has two operands
// The disassembler produces CMPSB, CMPSW, CMPSD, or CMPSQ without any arguments (never CMPS with an arg).
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
            throw BaseSemantics::Exception("no dispatch ability for \"" + insn->get_mnemonic() + "\" instruction", insn);
        }

        assert_args(insn, args, 0);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
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
            ASSERT_forbid(reg1.isEmpty());
            ASSERT_forbid(reg2.isEmpty());
            BaseSemantics::SValuePtr addr1 = d->readRegister(reg1);
            BaseSemantics::SValuePtr addr2 = d->readRegister(reg2);

            // Adjust address width depending on how memory is accessed. All addresses in memory have the same width.
            addr1 = d->fixMemoryAddress(addr1);
            addr2 = d->fixMemoryAddress(addr2);
            ASSERT_require(addr1->get_width() == addr2->get_width());
            
            // Read the two values from memory.
            RegisterDescriptor sr(x86_regclass_segment,
                                  insn->get_segmentOverride()!=x86_segreg_none ? insn->get_segmentOverride() : x86_segreg_ds,
                                  0, 16);
            BaseSemantics::SValuePtr val1 = ops->readMemory(sr, addr1, ops->undefined_(nbits), inLoop);
            BaseSemantics::SValuePtr val2 = ops->readMemory(d->REG_ES, addr2, ops->undefined_(nbits), inLoop);

            // Compare values and set status flags.
            BaseSemantics::SValuePtr no = ops->boolean_(false);
            (void) d->doAddOperation(val1, ops->invert(val2), true, no, inLoop);

            // Adjust the address registers
            BaseSemantics::SValuePtr positiveNBytes = ops->number_(reg1.nBits(), +nbytes);
            BaseSemantics::SValuePtr negativeNBytes = ops->number_(reg1.nBits(), -nbytes);
            BaseSemantics::SValuePtr step = ops->ite(d->readRegister(d->REG_DF), negativeNBytes, positiveNBytes);
            BaseSemantics::SValuePtr reg1val = d->readRegister(reg1);
            d->writeRegister(reg1, ops->ite(inLoop, ops->add(d->readRegister(reg1), step), reg1val));
            BaseSemantics::SValuePtr reg2val = d->readRegister(reg2);
            d->writeRegister(reg2, ops->ite(inLoop, ops->add(d->readRegister(reg2), step), reg2val));

            // Adjust instruction pointer register to either repeat the instruction or fall through
            if (x86_repeat_none!=repeat)
                d->repLeave(repeat, inLoop, insn->get_address(), true/*use ZF*/);
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
            A.nBits(nbits);
            BaseSemantics::SValuePtr oldA = d->readRegister(A);
            BaseSemantics::SValuePtr no = ops->boolean_(false);
            (void) d->doAddOperation(oldA, ops->invert(op0), true, no);
            BaseSemantics::SValuePtr zf = d->readRegister(d->REG_ZF);
            d->write(args[0], ops->ite(zf, d->read(args[1]), op0));
            d->writeRegister(A, ops->ite(zf, oldA, op0));
        }
    }
};

// Compare and exchange bytes
//   CMPXCHG8B
//   CMPXCHG16B
struct IP_cmpxchg2: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        if (!isSgAsmMemoryReferenceExpression(args[0])) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr a = d->read(args[0]);
            BaseSemantics::SValuePtr b, c;
            switch (a->get_width()) {
                case 64: {
                    BaseSemantics::SValuePtr edx = d->readRegister(d->REG_EDX);
                    b = ops->concat(d->readRegister(d->REG_EAX), edx);
                    BaseSemantics::SValuePtr ecx = d->readRegister(d->REG_ECX);
                    c = ops->concat(d->readRegister(d->REG_EBX), ecx);
                    break;
                }
                case 128: {
                    BaseSemantics::SValuePtr rdx = d->readRegister(d->REG_RDX);
                    b = ops->concat(d->readRegister(d->REG_RAX), rdx);
                    BaseSemantics::SValuePtr rcx = d->readRegister(d->REG_RCX);
                    c = ops->concat(d->readRegister(d->REG_RBX), rcx);
                    break;
                }
                default:
                    ASSERT_not_reachable("invalid operand width for CMPXCHG8B instruction");
            }
            BaseSemantics::SValuePtr eq = ops->isEqual(a, b);
            d->writeRegister(d->REG_ZF, eq);
            d->write(args[0], ops->ite(eq, c, a));
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
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            d->writeRegister(d->REG_DX, ops->extract(ops->signExtend(d->readRegister(d->REG_AX), 32), 16, 32));
        }
    }
};

// Sign extend RAX into RDX:RAX
struct IP_cqo: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            d->writeRegister(d->REG_RDX, ops->extract(ops->signExtend(d->readRegister(d->REG_RAX), 128), 64, 128));
        }
    }
};

// Sign extend AX to EAX
struct IP_cwde: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            d->writeRegister(d->REG_EAX, ops->signExtend(d->readRegister(d->REG_AX), 32));
        }
    }
};

// Decrement
struct IP_dec: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        if (insn->get_lockPrefix() && !isSgAsmMemoryReferenceExpression(args[0])) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr result = d->doIncOperation(d->read(args[0]), true, false);
            d->write(args[0], result);
        }
    }
};

// Halt
struct IP_hlt: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            ops->hlt();
            d->writeRegister(d->REG_anyIP, ops->number_(d->REG_anyIP.nBits(), insn->get_address()));
        }
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
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            size_t nbits = asm_type_width(args[0]->get_type());
            RegisterDescriptor regA = d->REG_AX; regA.nBits(8==nbits ? 16 : nbits);
            RegisterDescriptor regD = d->REG_DX; regD.nBits(8==nbits ? 16 : nbits);
            BaseSemantics::SValuePtr dividend;
            if (8==nbits) {
                dividend = d->readRegister(regA);
            } else {
                BaseSemantics::SValuePtr regDVal = d->readRegister(regD);
                dividend = ops->concat(d->readRegister(regA), regDVal);
            }
            BaseSemantics::SValuePtr divisor = d->read(args[0]);
            BaseSemantics::SValuePtr divResult, modResult;
            ASSERT_require(insn->get_kind()==kind);
            switch (kind) {
                case x86_idiv:
                    divResult = ops->signedDivide(dividend, divisor);
                    modResult = ops->signedModulo(dividend, divisor);
                    break;
                case x86_div:
                    divResult = ops->unsignedDivide(dividend, divisor);
                    modResult = ops->unsignedModulo(dividend, divisor);
                    break;
                default:
                    ASSERT_not_reachable("instruction kind not handled");
            }
            if (8==nbits) {
                d->writeRegister(regA, ops->concat(ops->extract(divResult, 0, 8), modResult));
            } else {
                d->writeRegister(regA, ops->extract(divResult, 0, nbits));
                d->writeRegister(regD, modResult);
            }
            d->writeRegister(d->REG_SF, ops->unspecified_(1));
            d->writeRegister(d->REG_ZF, ops->unspecified_(1));
            d->writeRegister(d->REG_AF, ops->unspecified_(1));
            d->writeRegister(d->REG_PF, ops->unspecified_(1));
            d->writeRegister(d->REG_CF, ops->unspecified_(1));
            d->writeRegister(d->REG_OF, ops->unspecified_(1));
        }
    }
};

// Load floating-point value
struct IP_fld: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            size_t nbits = asm_type_width(args[0]->get_type());
            if (80!=nbits)
                throw BaseSemantics::Exception(StringUtility::numberToString(nbits)+"-bit FP values not supported yet", insn);
            BaseSemantics::SValuePtr fp = d->read(args[0], nbits);
            d->pushFloatingPoint(fp);
        }
    }
};

// Restore x86 FPU control word
struct IP_fldcw: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            d->writeRegister(d->REG_FPCTL, d->read(args[0], 16));
        }
    }
};

// Floating-point no-operation
struct IP_fnop: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        if (insn->get_lockPrefix())
            ops->interrupt(x86_exception_ud, 0);
    }
};

// Store x87 FPU control word
struct IP_fnstcw: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            d->write(args[0], d->readRegister(d->REG_FPCTL));
        }
    }
};

// Store x87 FPU status word
struct IP_fnstsw: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            d->write(args[0], d->readRegister(d->REG_FPSTATUS));
        }
    }
};

// Store x87 floating point value
struct IP_fst: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            size_t nbits = asm_type_width(args[0]->get_type());
            if (80!=nbits)
                throw BaseSemantics::Exception(StringUtility::numberToString(nbits)+"-bit FP values not supported yet", insn);
            d->write(args[0], d->readFloatingPointStack(0));
        }
    }
};

// Store x87 floating point value and pop fp stack
struct IP_fstp: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            size_t nbits = asm_type_width(args[0]->get_type());
            if (80!=nbits)
                throw BaseSemantics::Exception(StringUtility::numberToString(nbits)+"-bit FP values not supported yet", insn);
            d->write(args[0], d->readFloatingPointStack(0));
            d->popFloatingPoint();
        }
    }
};

// Signed multiply.
// Note that the Intel documentation's assertion that "the two- and three-operand forms may also be used with unsigned operands
// because the lower half of the product is the same regardless if the operands are signed or unsigned" is nonsense when the
// factors are not the same width and the second factor is thus sign extended before the product is computed.  E.g., "IMUL r64,
// imm32" when applied to 1 x 4294967295 results in "-1" (0xffffffffffffffff) not 4294967295 (0x00000000ffffffff).
struct IP_imul: P {
    void p(D d, Ops ops, I insn, A args) {
        ASSERT_require(args.size() >= 1);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            // Read the two factors to be multiplied.
            size_t arg0Width = asm_type_width(args[0]->get_type());
            BaseSemantics::SValuePtr factor1, factor2;
            if (1 == args.size()) {
                if (8 == arg0Width) {
                    factor1 = d->readRegister(d->REG_AL);
                    factor2 = d->read(args[0]);
                } else {
                    RegisterDescriptor reg0 = d->REG_AX; reg0.nBits(arg0Width);
                    factor1 = d->readRegister(reg0);
                    factor2 = d->read(args[0]);
                }
            } else if (2 == args.size()) {
                ASSERT_require(arg0Width > 8);
                ASSERT_require(asm_type_width(args[1]->get_type()) <= arg0Width);
                factor1 = d->read(args[0]);
                factor2 = ops->signExtend(d->read(args[1]), arg0Width);
            } else if (3 == args.size()) {
                ASSERT_require(arg0Width > 8);
                ASSERT_require(asm_type_width(args[1]->get_type()) == arg0Width);
                ASSERT_require(asm_type_width(args[2]->get_type()) <= arg0Width);
                factor1 = d->read(args[1]);
                factor2 = ops->signExtend(d->read(args[2]), arg0Width);
            }

            // Obtain the result
            ASSERT_not_null(factor1);
            ASSERT_not_null(factor2);
            ASSERT_require(factor1->get_width() == factor2->get_width());
            BaseSemantics::SValuePtr product = ops->signedMultiply(factor1, factor2);

            // Store the result
            if (1 == args.size()) {
                if (8 == arg0Width) {
                    d->writeRegister(d->REG_AX, product);
                } else {
                    RegisterDescriptor aReg = d->REG_AX; aReg.nBits(arg0Width);
                    RegisterDescriptor dReg = d->REG_DX; dReg.nBits(arg0Width);
                    d->writeRegister(aReg, ops->extract(product, 0, arg0Width));
                    d->writeRegister(dReg, ops->extract(product, arg0Width, 2*arg0Width));
                }
            } else {
                d->write(args[0], ops->extract(product, 0, arg0Width));
            }

            // Carry flag set when high-order bits of the product are not all equal to the low-half's sign bit. In other words,
            // when the high-half bits are not all clear or not all set or when the high-half sign bit is not equal to the
            // low-half sign bit.
            ASSERT_require(product->get_width() % 2 == 0);
            size_t productHalfWidth = product->get_width() / 2;
            BaseSemantics::SValuePtr lh_signbit = ops->extract(product, productHalfWidth-1, productHalfWidth);
            BaseSemantics::SValuePtr hh_signbit = ops->extract(product, product->get_width()-1, product->get_width());
            BaseSemantics::SValuePtr hh = ops->extract(product, productHalfWidth, product->get_width());
            BaseSemantics::SValuePtr z1 = ops->equalToZero(ops->invert(hh));
            BaseSemantics::SValuePtr hh_allsame = ops->or_(ops->equalToZero(hh), z1);
            BaseSemantics::SValuePtr signsame = ops->equalToZero(ops->xor_(lh_signbit, hh_signbit));
            BaseSemantics::SValuePtr carry = ops->invert(ops->and_(hh_allsame, signsame));

            // Update status flags
            d->writeRegister(d->REG_CF, carry);
            d->writeRegister(d->REG_OF, carry);
            d->writeRegister(d->REG_SF, ops->unspecified_(1));
            d->writeRegister(d->REG_ZF, ops->unspecified_(1));
            d->writeRegister(d->REG_AF, ops->unspecified_(1));
            d->writeRegister(d->REG_PF, ops->unspecified_(1));
        }
    }
};

// Increment
struct IP_inc: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        if (insn->get_lockPrefix() && !isSgAsmMemoryReferenceExpression(args[0])) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr result = d->doIncOperation(d->read(args[0]), false, false);
            d->write(args[0], result);
        }
    }
};

// Call to interrupt procedure
struct IP_int: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            SgAsmIntegerValueExpression *bv = isSgAsmIntegerValueExpression(args[0]);
            if (!bv)
                throw BaseSemantics::Exception("operand must be a byte value expression", insn);
            ops->interrupt(x86_exception_int, bv->get_value());
        }
    }
};

// Call to the interrupt 3 procedure (for debugging), but slightly different semantics than the one-argument "INT 3"
// instruction.
struct IP_int3: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            ops->interrupt(x86_exception_int, -3);
        }
    }
};

// Jump
// The argument must be an absolute address, not an offset.  The disassembler takes care of that for us.
struct IP_jmp: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            size_t tgtWidth = d->REG_anyIP.nBits();
            BaseSemantics::SValuePtr tgt = ops->filterIndirectJumpTarget(ops->unsignedExtend(d->read(args[0]), tgtWidth));
            if (insn->get_operandSize() == x86_insnsize_16 && tgtWidth == 32) {
                BaseSemantics::SValuePtr zero16 = ops->number_(16, 0);
                tgt = ops->concat(ops->extract(tgt, 0, 16), zero16);
            }
            d->writeRegister(d->REG_anyIP, tgt);
        }
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
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            size_t tgtWidth = d->REG_anyIP.nBits();
            BaseSemantics::SValuePtr cond = d->flagsCombo(kind);
            BaseSemantics::SValuePtr ip = d->readRegister(d->REG_anyIP);
            BaseSemantics::SValuePtr tgt = ops->ite(cond,
                                                    ops->unsignedExtend(d->read(args[0]), tgtWidth),
                                                    ip);
            if (insn->get_operandSize() == x86_insnsize_16 && tgtWidth == 32) {
                BaseSemantics::SValuePtr zero16 = ops->number_(16, 0);
                tgt = ops->concat(ops->extract(tgt, 0, 16), zero16);
            }
            d->writeRegister(d->REG_anyIP, tgt);
        }
    }
};

// Load MXCSR register
struct IP_ldmxcsr: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            d->writeRegister(d->REG_MXCSR, d->read(args[0]));
        }
    }
};

// Load effective address
struct IP_lea: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            size_t nbits = asm_type_width(args[0]->get_type());
            d->write(args[0], ops->unsignedExtend(d->effectiveAddress(args[1]), nbits));
        }
    }
};

// High-level procedure exit
struct IP_leave: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            size_t nbitsBP = d->REG_anyBP.nBits();
            d->writeRegister(d->REG_anySP, d->readRegister(d->REG_anyBP));
            BaseSemantics::SValuePtr oldSP = d->readRegister(d->REG_anySP);
            BaseSemantics::SValuePtr newSP = ops->add(oldSP, ops->number_(oldSP->get_width(), nbitsBP/8));
            BaseSemantics::SValuePtr addr = d->fixMemoryAddress(oldSP);
            BaseSemantics::SValuePtr yes = ops->boolean_(true);
            d->writeRegister(d->REG_anyBP, ops->readMemory(d->REG_SS, addr, ops->undefined_(nbitsBP), yes));
            d->writeRegister(d->REG_anySP, newSP);
        }
    }
};

// Read time-stamp counter
struct IP_rdtsc: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr tsc = ops->rdtsc();
            d->writeRegister(d->REG_EAX, ops->extract(tsc, 0, 32));
            d->writeRegister(d->REG_EDX, ops->extract(tsc, 32, 64));
        }
    }
};

// Load string
// The disassembler produces LODSB, LODSW, LODSD, or LODSQ without any arguments (never LODS with an arg).
struct IP_loadstring: P {
    const X86RepeatPrefix repeat;
    const size_t nbits;
    const size_t nbytes;
    IP_loadstring(X86RepeatPrefix repeat, size_t nbits): repeat(repeat), nbits(nbits), nbytes(nbits/8) {
        ASSERT_require(8==nbits || 16==nbits || 32==nbits || 64==nbits);
    }
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr inLoop = d->repEnter(repeat);

            // Get the address for the string to load.
            RegisterDescriptor srcReg;
            switch (insn->get_addressSize()) {
                case x86_insnsize_16:
                    srcReg = d->REG_SI;
                    break;
                case x86_insnsize_32:
                    srcReg = d->REG_ESI;
                    break;
                case x86_insnsize_64:
                    srcReg = d->REG_RSI;
                    break;
                default:
                    ASSERT_not_reachable("invalid instruction address size");
            }
            ASSERT_forbid(srcReg.isEmpty());
            BaseSemantics::SValuePtr stringPtr = d->readRegister(srcReg);

            // Adjust address width based on how memory is accessed.  All addresses in memory have the same width.
            BaseSemantics::SValuePtr addr = d->fixMemoryAddress(stringPtr);

            // Load the byte, word, dword, or qword from memory.
            RegisterDescriptor sr(x86_regclass_segment,
                                  insn->get_segmentOverride()!=x86_segreg_none ? insn->get_segmentOverride() : x86_segreg_ds,
                                  0, 16);
            BaseSemantics::SValuePtr val = ops->readMemory(sr, addr, ops->undefined_(nbits), inLoop);

            // Save value into destination register
            RegisterDescriptor dstReg = d->REG_AX; dstReg.nBits(nbits);
            d->writeRegister(dstReg, val);

            // Advance pointer register
            BaseSemantics::SValuePtr positiveNBytes = ops->number_(srcReg.nBits(), +nbytes);
            BaseSemantics::SValuePtr negativeNBytes = ops->number_(srcReg.nBits(), -nbytes);
            BaseSemantics::SValuePtr step = ops->ite(d->readRegister(d->REG_DF), negativeNBytes, positiveNBytes);
            d->writeRegister(srcReg, ops->ite(inLoop, ops->add(stringPtr, step), stringPtr));

            // Adjust the instruction pointer register to either repeat the instruction or fall through
            if (x86_repeat_none != repeat)
                d->repLeave(repeat, inLoop, insn->get_address(), false/*no ZF*/);
        }
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
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            // Which register is used for counting?
            RegisterDescriptor counterReg;
            switch (insn->get_addressSize()) {
                case x86_insnsize_16:
                    counterReg = d->REG_DX;
                    break;
                case x86_insnsize_32:
                    counterReg = d->REG_EDX;
                    break;
                case x86_insnsize_64:
                    counterReg = d->REG_RDX;
                    break;
                default:
                    ASSERT_not_reachable("invalid instruction address size");
            }

            // Decrement the counter
            BaseSemantics::SValuePtr oldCounter = d->readRegister(counterReg);
            BaseSemantics::SValuePtr newCounter = ops->add(oldCounter, ops->number_(oldCounter->get_width(), -1));
            d->writeRegister(counterReg, newCounter);

            // Should we loop?
            BaseSemantics::SValuePtr doLoop;
            ASSERT_require(insn->get_kind() == kind);
            switch (kind) {
                case x86_loop:
                    doLoop = ops->invert(ops->equalToZero(newCounter));
                    break;
                case x86_loopnz: {
                    BaseSemantics::SValuePtr notZf = ops->invert(d->readRegister(d->REG_ZF));
                    doLoop = ops->and_(ops->invert(ops->equalToZero(newCounter)), notZf);
                    break;
                }
                case x86_loopz: {
                    BaseSemantics::SValuePtr zf = d->readRegister(d->REG_ZF);
                    doLoop = ops->and_(ops->invert(ops->equalToZero(newCounter)), zf);
                    break;
                }
                default:
                    ASSERT_not_reachable("instruction type not handled");
            }

            // Adjust the instruction pointer to either loop or fall through.
            BaseSemantics::SValuePtr ip = d->readRegister(d->REG_anyIP);
            d->writeRegister(d->REG_anyIP, ops->ite(doLoop, d->read(args[0], d->REG_anyIP.nBits()), ip));
        }
    }
};

// Store selected bytes
//  MASKMOVQ
struct IP_maskmov: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr src = d->read(args[0]);
            BaseSemantics::SValuePtr mask = d->read(args[1]);
            ASSERT_require(src->get_width() == mask->get_width());
            BaseSemantics::SValuePtr startVa = d->readRegister(d->REG_EDI);
            BaseSemantics::SValuePtr yes = ops->boolean_(true);
            BaseSemantics::SValuePtr mem = ops->readMemory(d->REG_DS, startVa, ops->undefined_(src->get_width()), yes);
            BaseSemantics::SValuePtr result;
            size_t nOps = src->get_width() / 8;
            for (size_t i=0; i<nOps; ++i) {
                BaseSemantics::SValuePtr partMask = ops->extract(mask, i*8, i*8+1);
                BaseSemantics::SValuePtr v2 = ops->extract(mem, i*8, i*8+8);
                BaseSemantics::SValuePtr byte = ops->ite(partMask, ops->extract(src, i*8, i*8+8), v2);
                result = result ? ops->concat(result, byte) : byte;
            }
            ops->writeMemory(d->REG_DS, startVa, result, ops->boolean_(true));
        }
    }
};

// The MOV instruction
struct IP_mov: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            size_t dstWidth = asm_type_width(args[0]->get_type());
            BaseSemantics::SValuePtr value = d->read(args[1]);
            if (dstWidth > value->get_width()) {
                // MOV r/m64, imm32 uses sign extend, but all others use unsigned extend or truncation.
                if (64==dstWidth && isSgAsmIntegerValueExpression(args[1])) {
                    value = ops->signExtend(value, 64);
                } else {
                    value = ops->unsignedExtend(value, dstWidth);
                }
            }
            d->write(args[0], value);
        }
    }
};

// Move data after swapping bytes
struct IP_movbe: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr src = d->read(args[1]);
            size_t nBytes = src->get_width() / 8;
            BaseSemantics::SValuePtr result;
            for (size_t i=0; i<nBytes; ++i) {
                BaseSemantics::SValuePtr byte = ops->extract(src, i*8, (i+1)*8);
                result = result ? ops->concat(byte, result) : byte;
            }
            d->write(args[0], result);
        }
    }
};

// Move high packed double-precision floating-point value
struct IP_movhpd: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else if (SgAsmDirectRegisterExpression *rre = isSgAsmDirectRegisterExpression(args[0])) {
            RegisterDescriptor reg = rre->get_descriptor();
            ASSERT_require(reg.offset() == 0 && reg.nBits() == 128);
            reg.offset(64);
            reg.nBits(64);
            BaseSemantics::SValuePtr val = d->read(args[1]);
            ASSERT_require(val->get_width() == 64);
            d->writeRegister(reg, val);
        } else if (SgAsmDirectRegisterExpression *rre = isSgAsmDirectRegisterExpression(args[1])) {
            RegisterDescriptor reg = rre->get_descriptor();
            ASSERT_require(reg.offset() == 0 && reg.nBits() == 128);
            reg.offset(64);
            reg.nBits(64);
            BaseSemantics::SValuePtr val = d->readRegister(reg);
            d->write(args[0], val);
        } else {
            ASSERT_not_reachable("one of the args should have been an XMM register");
        }
    }
};

// Move low packed double-precision floating-point value
struct IP_movlpd: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else if (SgAsmDirectRegisterExpression *rre = isSgAsmDirectRegisterExpression(args[0])) {
            RegisterDescriptor reg = rre->get_descriptor();
            ASSERT_require(reg.offset() == 0 && reg.nBits() == 128);
            reg.nBits(64);
            BaseSemantics::SValuePtr val = d->read(args[1]);
            ASSERT_require(val->get_width() == 64);
            d->writeRegister(reg, val);
        } else if (SgAsmDirectRegisterExpression *rre = isSgAsmDirectRegisterExpression(args[1])) {
            RegisterDescriptor reg = rre->get_descriptor();
            ASSERT_require(reg.offset() == 0 && reg.nBits() == 128);
            reg.nBits(64);
            BaseSemantics::SValuePtr val = d->readRegister(reg);
            d->write(args[0], val);
        } else {
            ASSERT_not_reachable("one of the args should have been an XMM register");
        }
    }
};

// Move scalar double-precision floating-point values
//   This is the floating point MOVSD instruction, not the string MOVSD instruction.
//   I.e., this is for x86_movsd_sse, not x86_movsd.
struct IP_movsd: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr src = ops->unsignedExtend(d->read(args[1]), 64);
            if (isSgAsmDirectRegisterExpression(args[1])) {
                // register->memory or register->register. High order 64 bits are unchanged.
                if (SgAsmDirectRegisterExpression *rre = isSgAsmDirectRegisterExpression(args[0])) {
                    RegisterDescriptor reg = rre->get_descriptor();
                    ASSERT_require(reg.offset() == 0 && reg.nBits() == 128);
                    reg.nBits(64);
                    d->writeRegister(reg, src);
                } else {
                    d->write(args[0], src);
                }
            } else {
                // memory->register. High order 64 bits are zeroed.
                d->write(args[0], ops->unsignedExtend(src, 128));
            }
        }
    }
};

// Move scalar single-precision floating-point values
struct IP_movss: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr src = ops->unsignedExtend(d->read(args[1]), 32);
            if (isSgAsmDirectRegisterExpression(args[1])) {
                // register->memory or register->register. High order 96 bits are unchanged.
                if (SgAsmDirectRegisterExpression *rre = isSgAsmDirectRegisterExpression(args[0])) {
                    RegisterDescriptor reg = rre->get_descriptor();
                    ASSERT_require(reg.offset() == 0 && reg.nBits() == 128);
                    reg.nBits(32);
                    d->writeRegister(reg, src);
                } else {
                    d->write(args[0], src);
                }
            } else {
                // memory->register. High order 96 bits are zeroed.
                d->write(args[0], ops->unsignedExtend(src, 128));
            }
        }
    }
};

// Move source to destination with truncation or zero extend
// Used for MOVD and MOVQ
struct IP_move_zero_extend: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            size_t dstWidth = asm_type_width(args[0]->get_type());
            BaseSemantics::SValuePtr value = d->read(args[1]);
            if (value->get_width() != dstWidth)
                value = ops->unsignedExtend(value, dstWidth);
            d->write(args[0], value);
        }
    }
};

// Move source to destination with sign extend
// Used for MOVD and MOVQ
struct IP_move_sign_extend: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            size_t dstWidth = asm_type_width(args[0]->get_type());
            BaseSemantics::SValuePtr value = d->read(args[1]);
            if (value->get_width() < dstWidth)
                value = ops->signExtend(value, dstWidth);
            d->write(args[0], value);
        }
    }
};

// Move from one location to another without any extension or truncation.
struct IP_move_same: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            d->write(args[0], d->read(args[1]));
        }
    }
};

// Move data from string to string
// The disassembler produces MOVSB, MOVSW, MOVSD, or MOVSQ without any arguments (never MOVS with an arg)
// The floating-point version of MOVSD is called x86_movsd_sse
struct IP_movestring: P {
    const X86RepeatPrefix repeat;
    const size_t nbits;
    const size_t nbytes;
    IP_movestring(X86RepeatPrefix repeat, size_t nbits): repeat(repeat), nbits(nbits), nbytes(nbits/8) {
        ASSERT_require(8==nbits || 16==nbits || 32==nbits || 64==nbits);
        ASSERT_require(x86_repeat_none==repeat || x86_repeat_repe==repeat);
    }
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr inLoop = d->repEnter(repeat);
            
            // Get the source and destination addresses
            RegisterDescriptor srcReg, dstReg;
            switch (insn->get_addressSize()) {
                case x86_insnsize_16:
                    srcReg = d->REG_SI;
                    dstReg = d->REG_DI;
                    break;
                case x86_insnsize_32:
                    srcReg = d->REG_ESI;
                    dstReg = d->REG_EDI;
                    break;
                case x86_insnsize_64:
                    srcReg = d->REG_RSI;
                    dstReg = d->REG_RDI;
                    break;
                default:
                    ASSERT_not_reachable("invalid instruction address size");
            }
            ASSERT_forbid(srcReg.isEmpty());
            ASSERT_forbid(dstReg.isEmpty());
            BaseSemantics::SValuePtr srcRegVal = d->readRegister(srcReg);
            BaseSemantics::SValuePtr dstRegVal = d->readRegister(dstReg);

            // Adjust address width depending on how memory is accessed.  All addresses in memory have the same width.
            BaseSemantics::SValuePtr srcAddr = d->fixMemoryAddress(srcRegVal);
            BaseSemantics::SValuePtr dstAddr = d->fixMemoryAddress(dstRegVal);
            ASSERT_require(srcAddr->get_width() == dstAddr->get_width());

            // Copy a value from source to destination
            RegisterDescriptor sr(x86_regclass_segment,
                                  insn->get_segmentOverride()!=x86_segreg_none ? insn->get_segmentOverride() : x86_segreg_ds,
                                  0, 16);
            BaseSemantics::SValuePtr value = ops->readMemory(sr, srcAddr, ops->undefined_(nbits), inLoop);
            ops->writeMemory(d->REG_ES, dstAddr, value, inLoop);

            // Adjust the address registers
            BaseSemantics::SValuePtr positiveNBytes = ops->number_(srcReg.nBits(), +nbytes);
            BaseSemantics::SValuePtr negativeNBytes = ops->number_(srcReg.nBits(), -nbytes);
            BaseSemantics::SValuePtr step = ops->ite(d->readRegister(d->REG_DF), negativeNBytes, positiveNBytes);
            d->writeRegister(srcReg, ops->ite(inLoop, ops->add(srcRegVal, step), srcRegVal));
            d->writeRegister(dstReg, ops->ite(inLoop, ops->add(dstRegVal, step), dstRegVal));

            // Adjust instruction pointer register to either repeat the instruction or fall through
            if (x86_repeat_none!=repeat)
                d->repLeave(repeat, inLoop, insn->get_address(), false/*no ZF*/);
        }
    }
};

// Unsigned multiply
struct IP_mul: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            // Read the two factors to be multiplied
            BaseSemantics::SValuePtr factor1 = d->read(args[0]);
            BaseSemantics::SValuePtr factor2;
            switch (factor1->get_width()) {
                case 8:
                    factor2 = d->readRegister(d->REG_AL);
                    break;
                case 16:
                    factor2 = d->readRegister(d->REG_AX);
                    break;
                case 32:
                    factor2 = d->readRegister(d->REG_EAX);
                    break;
                case 64:
                    factor2 = d->readRegister(d->REG_RAX);
                    break;
                default:
                    ASSERT_not_reachable("invalid operand size");
            }

            // Compute the result
            ASSERT_not_null(factor2);
            ASSERT_require(factor1->get_width() == factor2->get_width());
            BaseSemantics::SValuePtr product = ops->unsignedMultiply(factor1, factor2);

            // Save the result
            switch (factor1->get_width()) {
                case 8:
                    d->writeRegister(d->REG_AX, product);
                    break;
                case 16:
                    d->writeRegister(d->REG_AX, ops->extract(product, 0, 16));
                    d->writeRegister(d->REG_DX, ops->extract(product, 16, 32));
                    break;
                case 32:
                    d->writeRegister(d->REG_EAX, ops->extract(product, 0, 32));
                    d->writeRegister(d->REG_EDX, ops->extract(product, 32, 64));
                    break;
                case 64:
                    d->writeRegister(d->REG_RAX, ops->extract(product, 0, 64));
                    d->writeRegister(d->REG_RDX, ops->extract(product, 64, 128));
                    break;
                default:
                    ASSERT_not_reachable("invalid operand size");
            }

            // Set flags
            BaseSemantics::SValuePtr carry = ops->invert(ops->equalToZero(ops->extract(product, factor1->get_width(),
                                                                                       2 * factor1->get_width())));
            d->writeRegister(d->REG_CF, carry);
            d->writeRegister(d->REG_OF, carry);
            d->writeRegister(d->REG_SF, ops->unspecified_(1));
            d->writeRegister(d->REG_ZF, ops->unspecified_(1));
            d->writeRegister(d->REG_AF, ops->unspecified_(1));
            d->writeRegister(d->REG_PF, ops->unspecified_(1));
        }
    }
};

// Two's complement negation
struct IP_neg: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        if (insn->get_lockPrefix() && !isSgAsmMemoryReferenceExpression(args[0])) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr srcVal = d->read(args[0]);
            BaseSemantics::SValuePtr no = ops->boolean_(false);
            BaseSemantics::SValuePtr invertedSrcVal = ops->invert(srcVal);
            BaseSemantics::SValuePtr result = d->doAddOperation(ops->number_(srcVal->get_width(), 0),
                                                                invertedSrcVal, true, no);
            d->write(args[0], result);
        }
    }
};

// No operation
struct IP_nop: P {
    void p(D d, Ops ops, I insn, A args) {
        if (insn->get_lockPrefix())
            ops->interrupt(x86_exception_ud, 0);
    }
};

// Invert bits
struct IP_not: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        if (insn->get_lockPrefix() && !isSgAsmMemoryReferenceExpression(args[0])) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            d->write(args[0], ops->invert(d->read(args[0])));
        }
    }
};

// Bitwise OR
struct IP_or: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix() && !isSgAsmMemoryReferenceExpression(args[0])) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr a = d->read(args[0]);
            BaseSemantics::SValuePtr b = d->read(args[1]);
            if (a->get_width() > b->get_width())
                b = ops->signExtend(b, a->get_width());
            ASSERT_require(a->get_width() == b->get_width());
            BaseSemantics::SValuePtr result = ops->or_(a, b);
            d->setFlagsForResult(result);
            d->write(args[0], result);
            d->writeRegister(d->REG_OF, ops->boolean_(false));
            d->writeRegister(d->REG_AF, ops->unspecified_(1));
            d->writeRegister(d->REG_CF, ops->boolean_(false));
        }
    }
};

// Packed absolute value
//   PABSB
//   PABSW
//   PABSD
struct IP_pabs: P {
    size_t bitsPerOp;
    IP_pabs(size_t bitsPerOp): bitsPerOp(bitsPerOp) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr src = d->read(args[1]);
            BaseSemantics::SValuePtr result;
            size_t nOps = src->get_width() / bitsPerOp;
            for (size_t i=0; i<nOps; ++i) {
                BaseSemantics::SValuePtr part = ops->extract(src, i*bitsPerOp, (i+1)*bitsPerOp);
                BaseSemantics::SValuePtr signBit = ops->extract(src, (i+1)*bitsPerOp-1, (i+1)*bitsPerOp);
                BaseSemantics::SValuePtr absVal = ops->ite(signBit, ops->negate(part), part);
                result = result ? ops->concat(result, absVal) : absVal;
            }
            d->write(args[0], result);
        }
    }
};

// Pack with signed saturation
//   PACKSSDW
//   PACKSSWB
struct IP_packss: P {
    size_t srcBitsPerOp;
    size_t dstBitsPerOp;
    IP_packss(size_t srcBitsPerOp, size_t dstBitsPerOp): srcBitsPerOp(srcBitsPerOp), dstBitsPerOp(dstBitsPerOp) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr a = d->read(args[0]);
            BaseSemantics::SValuePtr b = d->read(args[1]);
            ASSERT_require(a->get_width() == b->get_width());
            size_t nOps = a->get_width() / srcBitsPerOp;
            BaseSemantics::SValuePtr result;
            for (size_t i=0; i<nOps; ++i) {
                BaseSemantics::SValuePtr src = ops->extract(a, i*srcBitsPerOp, (i+1)*srcBitsPerOp);
                BaseSemantics::SValuePtr dst = d->saturateSignedToSigned(src, dstBitsPerOp);
                result = result ? ops->concat(result, dst) : dst;
            }
            for (size_t i=0; i<nOps; ++i) {
                BaseSemantics::SValuePtr src = ops->extract(b, i*srcBitsPerOp, (i+1)*srcBitsPerOp);
                BaseSemantics::SValuePtr dst = d->saturateSignedToSigned(src, dstBitsPerOp);
                result = result ? ops->concat(result, dst) : dst;
            }
            d->write(args[0], result);
        }
    }
};

// Pack with unsigned saturation
//   PACKUSDW
//   PACKUSWB
struct IP_packus: P {
    size_t srcBitsPerOp;
    size_t dstBitsPerOp;
    IP_packus(size_t srcBitsPerOp, size_t dstBitsPerOp): srcBitsPerOp(srcBitsPerOp), dstBitsPerOp(dstBitsPerOp) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr a = d->read(args[0]);
            BaseSemantics::SValuePtr b = d->read(args[1]);
            ASSERT_require(a->get_width() == b->get_width());
            size_t nOps = a->get_width() / srcBitsPerOp;
            BaseSemantics::SValuePtr result;
            for (size_t i=0; i<nOps; ++i) {
                BaseSemantics::SValuePtr src = ops->extract(a, i*srcBitsPerOp, (i+1)*srcBitsPerOp);
                BaseSemantics::SValuePtr dst = d->saturateSignedToUnsigned(src, dstBitsPerOp);
                result = result ? ops->concat(result, dst) : dst;
            }
            for (size_t i=0; i<nOps; ++i) {
                BaseSemantics::SValuePtr src = ops->extract(b, i*srcBitsPerOp, (i+1)*srcBitsPerOp);
                BaseSemantics::SValuePtr dst = d->saturateSignedToUnsigned(src, dstBitsPerOp);
                result = result ? ops->concat(result, dst) : dst;
            }
            d->write(args[0], result);
        }
    }
};

// Packed integer addition
//   PADDB
//   PADDW
//   PADDD
//   PADDQ
struct IP_padd: P {
    size_t bitsPerOp;
    IP_padd(size_t bitsPerOp): bitsPerOp(bitsPerOp) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr a = d->read(args[0]);
            BaseSemantics::SValuePtr b = d->read(args[1]);
            ASSERT_require(a->get_width() == b->get_width());
            size_t nOps = a->get_width() / bitsPerOp;
            BaseSemantics::SValuePtr result;
            for (size_t i=0; i<nOps; ++i) {
                BaseSemantics::SValuePtr partA = ops->extract(a, i*bitsPerOp, (i+1)*bitsPerOp);
                BaseSemantics::SValuePtr partB = ops->extract(b, i*bitsPerOp, (i+1)*bitsPerOp);
                BaseSemantics::SValuePtr sum = ops->add(partA, partB);
                result = result ? ops->concat(result, sum) : sum;
            }
            d->write(args[0], result);
        }
    }
};

// Add packed signed integers with signed saturation
//   PADDSB
//   PADDSW
struct IP_padds: P {
    size_t bitsPerOp;
    IP_padds(size_t bitsPerOp): bitsPerOp(bitsPerOp) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr a = d->read(args[0]);
            BaseSemantics::SValuePtr b = d->read(args[1]);
            ASSERT_require(a->get_width() == b->get_width());
            size_t nOps = a->get_width() / bitsPerOp;
            BaseSemantics::SValuePtr result;
            for (size_t i=0; i<nOps; ++i) {
                BaseSemantics::SValuePtr partA = ops->signExtend(ops->extract(a, i*bitsPerOp, (i+1)*bitsPerOp), bitsPerOp+1);
                BaseSemantics::SValuePtr partB = ops->signExtend(ops->extract(b, i*bitsPerOp, (i+1)*bitsPerOp), bitsPerOp+1);
                BaseSemantics::SValuePtr sum = d->saturateSignedToSigned(ops->add(partA, partB), bitsPerOp);
                result = result ? ops->concat(result, sum) : sum;
            }
            d->write(args[0], result);
        }
    }
};

// Add packed unsigned integers with unsigned saturation
//   PADDUSB
//   PADDUSW
struct IP_paddus: P {
    size_t bitsPerOp;
    IP_paddus(size_t bitsPerOp): bitsPerOp(bitsPerOp) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr a = d->read(args[0]);
            BaseSemantics::SValuePtr b = d->read(args[1]);
            ASSERT_require(a->get_width() == b->get_width());
            size_t nOps = a->get_width() / bitsPerOp;
            BaseSemantics::SValuePtr result;
            for (size_t i=0; i<nOps; ++i) {
                BaseSemantics::SValuePtr partA = ops->signExtend(ops->extract(a, i*bitsPerOp, (i+1)*bitsPerOp), bitsPerOp+1);
                BaseSemantics::SValuePtr partB = ops->signExtend(ops->extract(b, i*bitsPerOp, (i+1)*bitsPerOp), bitsPerOp+1);
                BaseSemantics::SValuePtr sum = d->saturateUnsignedToUnsigned(ops->add(partA, partB), bitsPerOp);
                result = result ? ops->concat(result, sum) : sum;
            }
            d->write(args[0], result);
        }
    }
};

// Packed align right
struct IP_palignr: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
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
            BaseSemantics::SValuePtr arg1 = d->read(args[1]);
            BaseSemantics::SValuePtr wide = ops->concat(d->read(args[0]), arg1);
            BaseSemantics::SValuePtr result = ops->extract(ops->shiftRight(wide, d->number_(8, nBytes->get_value() * 8)),
                                                           0, nbits);
            d->write(args[0], result);
        }
    }
};

// Logical AND
//   PAND
struct IP_pand: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr a = d->read(args[0]);
            BaseSemantics::SValuePtr b = d->read(args[1]);
            BaseSemantics::SValuePtr result = ops->and_(a, b);
            d->write(args[0], result);
        }
    }
};

// Logical AND-NOT
struct IP_pandn: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr a = d->read(args[0]);
            BaseSemantics::SValuePtr b = d->read(args[1]);
            BaseSemantics::SValuePtr result = ops->invert(ops->and_(a, b));
            d->write(args[0], result);
        }
    }
};

// Packed average
//   PAVGB
//   PAVGW
struct IP_pavg: P {
    size_t bitsPerOp;
    IP_pavg(size_t bitsPerOp): bitsPerOp(bitsPerOp) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr a = d->read(args[0]);
            BaseSemantics::SValuePtr b = d->read(args[1]);
            ASSERT_require(a->get_width() == b->get_width());
            size_t nOps = a->get_width() / bitsPerOp;
            BaseSemantics::SValuePtr result;
            for (size_t i=0; i<nOps; ++i) {
                BaseSemantics::SValuePtr partA = ops->extract(a, i*bitsPerOp, (i+1)*bitsPerOp);
                BaseSemantics::SValuePtr partB = ops->extract(b, i*bitsPerOp, (i+1)*bitsPerOp);
                BaseSemantics::SValuePtr partBWide = ops->unsignedExtend(partB, bitsPerOp+1);
                BaseSemantics::SValuePtr one = ops->number_(bitsPerOp+1, 1);
                BaseSemantics::SValuePtr sum = ops->add(ops->add(ops->unsignedExtend(partA, bitsPerOp+1), partBWide),
                                                        one);
                BaseSemantics::SValuePtr ave = ops->extract(sum, 1, bitsPerOp+1);
                result = result ? ops->concat(result, ave) : ave;
            }
        }
    }
};

// Variable blend packed bytes
struct IP_pblendvb: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            static const size_t bitsPerOp = 8;
            BaseSemantics::SValuePtr a = d->read(args[0]);
            BaseSemantics::SValuePtr b = d->read(args[1]);
            BaseSemantics::SValuePtr xmm0 = d->readRegister(RegisterDescriptor(x86_regclass_xmm, 0, 0, 128));
            ASSERT_require(a->get_width() == b->get_width());
            size_t nOps = a->get_width() / bitsPerOp;
            BaseSemantics::SValuePtr result;
            for (size_t i=0; i<nOps; ++i) {
                BaseSemantics::SValuePtr partA = ops->extract(a, i*bitsPerOp, (i+1)*bitsPerOp);
                BaseSemantics::SValuePtr partB = ops->extract(b, i*bitsPerOp, (i+1)*bitsPerOp);
                BaseSemantics::SValuePtr selector = ops->extract(xmm0, i*8+7, i*8+8);
                BaseSemantics::SValuePtr selected = ops->ite(selector, b, a);
                result = result ? ops->concat(result, selected) : selected;
            }
        }
    }
};

// Blend packed words
struct IP_pblendw: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            static const size_t bitsPerOp = 16;
            BaseSemantics::SValuePtr a = d->read(args[0]);
            BaseSemantics::SValuePtr b = d->read(args[1]);
            uint64_t mask = d->read(args[2])->get_number(); // must be an immediate operand
            ASSERT_require(a->get_width() == b->get_width());
            size_t nOps = a->get_width() / bitsPerOp;
            BaseSemantics::SValuePtr result;
            for (size_t i=0; i<nOps; ++i) {
                BaseSemantics::SValuePtr partA = ops->extract(a, i*bitsPerOp, (i+1)*bitsPerOp);
                BaseSemantics::SValuePtr partB = ops->extract(b, i*bitsPerOp, (i+1)*bitsPerOp);
                bool selector = (mask >> i) & 1;
                BaseSemantics::SValuePtr selected = selector ? b : a;
                result = result ? ops->concat(result, selected) : selected;
            }
        }
    }
};

// Compare packed data for equal
struct IP_pcmpeq: P {
    size_t nCmpBits;                                    // number of bits to compare at once
    IP_pcmpeq(size_t nCmpBits): nCmpBits(nCmpBits) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr a = d->read(args[0]);
            BaseSemantics::SValuePtr b = d->read(args[1]);
            ASSERT_require(a->get_width() == b->get_width());
            ASSERT_require(a->get_width() % nCmpBits == 0);
            BaseSemantics::SValuePtr result;
            BaseSemantics::SValuePtr zero = ops->number_(nCmpBits, 0);
            BaseSemantics::SValuePtr ones = ops->invert(zero);
            for (size_t i=0; i<a->get_width()/nCmpBits; ++i) {
                BaseSemantics::SValuePtr partA = ops->extract(a, i*nCmpBits, (i+1)*nCmpBits);
                BaseSemantics::SValuePtr partB = ops->extract(b, i*nCmpBits, (i+1)*nCmpBits);
                BaseSemantics::SValuePtr diff = ops->add(partA, ops->negate(partB));
                BaseSemantics::SValuePtr c = ops->ite(ops->equalToZero(diff), ones, zero);
                if (0==i) {
                    result = c;
                } else {
                    result = ops->concat(result, c);
                }
            }
            d->write(args[0], result);
        }
    }
};

// Compare packed signed integer for greater-than
//   PCMPGTB
//   PCMPGTW
//   PCMPGTD
//   PCMPGTQ
struct IP_pcmpgt: P {
    size_t bitsPerOp;
    IP_pcmpgt(size_t bitsPerOp): bitsPerOp(bitsPerOp) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr a = d->read(args[0]);
            BaseSemantics::SValuePtr b = d->read(args[1]);
            ASSERT_require(a->get_width() == b->get_width());
            size_t nOps = a->get_width() / bitsPerOp;
            BaseSemantics::SValuePtr zero = ops->number_(bitsPerOp, 0);
            BaseSemantics::SValuePtr ones = ops->invert(zero);
            BaseSemantics::SValuePtr result;
            for (size_t i=0; i<nOps; ++i) {
                BaseSemantics::SValuePtr partA = ops->extract(a, i*bitsPerOp, (i+1)*bitsPerOp);
                BaseSemantics::SValuePtr partB = ops->extract(b, i*bitsPerOp, (i+1)*bitsPerOp);
                BaseSemantics::SValuePtr partBWide = ops->signExtend(partB, bitsPerOp+1);
                BaseSemantics::SValuePtr diff = ops->subtract(ops->signExtend(partA, bitsPerOp+1), partBWide);
                BaseSemantics::SValuePtr isLT = ops->extract(diff, bitsPerOp, bitsPerOp+1);
                BaseSemantics::SValuePtr isEQ = ops->equalToZero(diff);
                BaseSemantics::SValuePtr isLE = ops->or_(isLT, isEQ);
                BaseSemantics::SValuePtr answer = ops->ite(isLE, zero, ones);
                result = result ? ops->concat(result, answer) : answer;
            }
            d->write(args[0], result);
        }
    }
};

// Extract byte, dword, qword
//   PEXTRB
//   PEXTRD
//   PEXTRW
//   PEXTRQ
struct IP_pextr: P {
    size_t bitsPerOp;
    IP_pextr(size_t bitsPerOp): bitsPerOp(bitsPerOp) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr src = d->read(args[1]);
            uint64_t index = d->read(args[2])->get_number(); // must be an immediate operand
            switch (bitsPerOp) {
                case 8:
                    index &= 0x0f;
                    break;
                case 16:
                    if (64==src->get_width()) {
                        index &= 0x03;
                    } else {
                        index &= 0x07;
                    }
                    break;
                case 32:
                    index &= 0x03;
                    break;
                case 64:
                    index &= 0x01;
                    break;
                default:
                    ASSERT_not_reachable("invalid operand size");
            }
            BaseSemantics::SValuePtr extracted = ops->extract(src, index*bitsPerOp, (index+1)*bitsPerOp);
            size_t dstWidth = asm_type_width(args[0]->get_type());
            BaseSemantics::SValuePtr result = ops->unsignedExtend(extracted, dstWidth);
            d->write(args[0], result);
        }
    }
};

// Packed horizontal add
//   PHADDW
//   PHADDD
struct IP_phadd: P {
    size_t bitsPerOp;
    IP_phadd(size_t bitsPerOp): bitsPerOp(bitsPerOp) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr a = d->read(args[0]);
            BaseSemantics::SValuePtr b = d->read(args[1]);
            ASSERT_require(a->get_width() == b->get_width());
            size_t nOps = a->get_width() / bitsPerOp;
            BaseSemantics::SValuePtr result;
            for (size_t i=0; i<nOps/2; ++i) {
                BaseSemantics::SValuePtr term1 = ops->extract(a, (2*i+0)*bitsPerOp, (2*i+1)*bitsPerOp);
                BaseSemantics::SValuePtr term2 = ops->extract(a, (2*i+1)*bitsPerOp, (2*i+2)*bitsPerOp);
                BaseSemantics::SValuePtr sum = ops->add(term1, term2);
                result = result ? ops->concat(result, sum) : sum;
            }
            for (size_t i=0; i<nOps/2; ++i) {
                BaseSemantics::SValuePtr term1 = ops->extract(b, (2*i+0)*bitsPerOp, (2*i+1)*bitsPerOp);
                BaseSemantics::SValuePtr term2 = ops->extract(b, (2*i+1)*bitsPerOp, (2*i+2)*bitsPerOp);
                BaseSemantics::SValuePtr sum = ops->add(term1, term2);
                result = ops->concat(result, sum);
            }
            d->write(args[0], result);
        }
    }
};

// Packed horizontal add and saturate
//   PHADDSW
struct IP_phadds: P {
    size_t bitsPerOp;
    IP_phadds(size_t bitsPerOp): bitsPerOp(bitsPerOp) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr a = d->read(args[0]);
            BaseSemantics::SValuePtr b = d->read(args[1]);
            ASSERT_require(a->get_width() == b->get_width());
            size_t nOps = a->get_width() / bitsPerOp;
            BaseSemantics::SValuePtr result;
            for (size_t i=0; i<nOps/2; ++i) {
                BaseSemantics::SValuePtr term1 = ops->signExtend(ops->extract(a, (2*i+0)*bitsPerOp, (2*i+1)*bitsPerOp),
                                                                 bitsPerOp+1);
                BaseSemantics::SValuePtr term2 = ops->signExtend(ops->extract(a, (2*i+1)*bitsPerOp, (2*i+2)*bitsPerOp),
                                                                 bitsPerOp+1);
                BaseSemantics::SValuePtr sum = d->saturateSignedToSigned(ops->add(term1, term2), bitsPerOp);
                result = result ? ops->concat(result, sum) : sum;
            }
            for (size_t i=0; i<nOps/2; ++i) {
                BaseSemantics::SValuePtr term1 = ops->signExtend(ops->extract(b, (2*i+0)*bitsPerOp, (2*i+1)*bitsPerOp),
                                                                 bitsPerOp+1);
                BaseSemantics::SValuePtr term2 = ops->signExtend(ops->extract(b, (2*i+1)*bitsPerOp, (2*i+2)*bitsPerOp),
                                                                 bitsPerOp+1);
                BaseSemantics::SValuePtr sum = d->saturateSignedToSigned(ops->add(term1, term2), bitsPerOp);
                result = ops->concat(result, sum);
            }
            d->write(args[0], result);
        }
    }
};

// Packed horizontal word unsigned minimum with position information
//   PHMINPOSUW
struct IP_phminposuw: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            const size_t bitsPerOp = 16;
            BaseSemantics::SValuePtr src = d->read(args[1]);
            size_t nOps = src->get_width() / bitsPerOp;
            BaseSemantics::SValuePtr minVal;
            BaseSemantics::SValuePtr minIndex;
            for (size_t i=0; i<nOps; ++i) {
                BaseSemantics::SValuePtr part = ops->extract(src, i*bitsPerOp, (i+1)*bitsPerOp);
                if (minVal) {
                    BaseSemantics::SValuePtr isLessThan = ops->isUnsignedLessThan(part, minVal);
                    minVal = ops->ite(isLessThan, part, minVal);
                    minIndex = ops->ite(isLessThan, ops->number_(3, i), minIndex);
                } else {
                    minVal = part;
                    minIndex = ops->number_(3, i);
                }
            }
            BaseSemantics::SValuePtr result = ops->concat(minVal, minIndex);
            result = ops->unsignedExtend(result, 128);
            d->write(args[0], result);
        }
    }
};

// Packed horizontal subtract
//   PHSUBW
//   PHSUBD
struct IP_phsub: P {
    size_t bitsPerOp;
    IP_phsub(size_t bitsPerOp): bitsPerOp(bitsPerOp) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr a = d->read(args[0]);
            BaseSemantics::SValuePtr b = d->read(args[1]);
            ASSERT_require(a->get_width() == b->get_width());
            size_t nOps = a->get_width() / bitsPerOp;
            BaseSemantics::SValuePtr result;
            for (size_t i=0; i<nOps/2; ++i) {
                BaseSemantics::SValuePtr minuend = ops->extract(a, (2*i+0)*bitsPerOp, (2*i+1)*bitsPerOp);
                BaseSemantics::SValuePtr subtrahend = ops->extract(a, (2*i+1)*bitsPerOp, (2*i+2)*bitsPerOp);
                BaseSemantics::SValuePtr difference = ops->subtract(minuend, subtrahend);
                result = result ? ops->concat(result, difference) : difference;
            }
            for (size_t i=0; i<nOps/2; ++i) {
                BaseSemantics::SValuePtr minuend = ops->extract(b, (2*i+0)*bitsPerOp, (2*i+1)*bitsPerOp);
                BaseSemantics::SValuePtr subtrahend = ops->extract(b, (2*i+1)*bitsPerOp, (2*i+2)*bitsPerOp);
                BaseSemantics::SValuePtr difference = ops->subtract(minuend, subtrahend);
                result = ops->concat(result, difference);
            }
            d->write(args[0], result);
        }
    }
};

// Packed horizontal subtract and saturate
//   PHSUBSW
struct IP_phsubs: P {
    size_t bitsPerOp;
    IP_phsubs(size_t bitsPerOp): bitsPerOp(bitsPerOp) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr a = d->read(args[0]);
            BaseSemantics::SValuePtr b = d->read(args[1]);
            ASSERT_require(a->get_width() == b->get_width());
            size_t nOps = a->get_width() / bitsPerOp;
            BaseSemantics::SValuePtr result;
            for (size_t i=0; i<nOps/2; ++i) {
                BaseSemantics::SValuePtr minuend = ops->signExtend(ops->extract(a, (2*i+0)*bitsPerOp, (2*i+1)*bitsPerOp),
                                                                   bitsPerOp+1);
                BaseSemantics::SValuePtr subtrahend = ops->signExtend(ops->extract(a, (2*i+1)*bitsPerOp, (2*i+2)*bitsPerOp),
                                                                      bitsPerOp+1);
                BaseSemantics::SValuePtr difference = d->saturateSignedToSigned(ops->subtract(minuend, subtrahend), bitsPerOp);
                result = result ? ops->concat(result, difference) : difference;
            }
            for (size_t i=0; i<nOps/2; ++i) {
                BaseSemantics::SValuePtr minuend = ops->signExtend(ops->extract(b, (2*i+0)*bitsPerOp, (2*i+1)*bitsPerOp),
                                                                   bitsPerOp+1);
                BaseSemantics::SValuePtr subtrahend = ops->signExtend(ops->extract(b, (2*i+1)*bitsPerOp, (2*i+2)*bitsPerOp),
                                                                      bitsPerOp+1);
                BaseSemantics::SValuePtr difference = d->saturateSignedToSigned(ops->subtract(minuend, subtrahend), bitsPerOp);
                result = ops->concat(result, difference);
            }
            d->write(args[0], result);
        }
    }
};

// Packed insert
//   PINSRB
//   PINSRW
//   PINSRD
//   PINSRQ
struct IP_pinsr: P {
    size_t bitsPerOp;
    IP_pinsr(size_t bitsPerOp): bitsPerOp(bitsPerOp) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            size_t index = d->read(args[2])->get_number(); // argument must be immediate
            size_t dstWidth = asm_type_width(args[0]->get_type());
            switch (bitsPerOp) {
                case 8:
                    index &= 0x0f;
                    break;
                case 16:
                    if (64==dstWidth) {
                        index &= 0x03;
                    } else {
                        index &= 0x07;
                    }
                    break;
                case 32:
                    index &= 0x03;
                    break;
                case 64:
                    index &= 0x01;
                    break;
                default:
                    ASSERT_not_reachable("invalid operand size");
            }
            BaseSemantics::SValuePtr src = ops->extract(d->read(args[1]), 0, bitsPerOp);
            BaseSemantics::SValuePtr dst = d->read(args[0]);
            BaseSemantics::SValuePtr result = index > 0 ? ops->concat(ops->extract(dst, 0, index*bitsPerOp), src) : src;
            if ((index+1) * bitsPerOp < dst->get_width())
                result = ops->concat(result, ops->extract(dst, (index+1)*bitsPerOp, dst->get_width()));
            d->write(args[0], result);
        }
    }
};

// Multiply and add packed signed and unsigned bytes
//   PMADDUBSW
struct IP_pmaddubsw: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr a = d->read(args[0]);
            BaseSemantics::SValuePtr b = d->read(args[1]);
            ASSERT_require(a->get_width() == b->get_width());
            size_t nOps = a->get_width() / 16;
            BaseSemantics::SValuePtr result;
            for (size_t i=0; i<nOps; ++i) {
                BaseSemantics::SValuePtr x0 = ops->extract(a, i*16+0, i*16+8);
                BaseSemantics::SValuePtr x1 = ops->extract(a, i*16+8, i*16+16);
                BaseSemantics::SValuePtr y0 = ops->extract(b, i*16+0, i*16+8);
                BaseSemantics::SValuePtr y1 = ops->extract(b, i*16+8, i*16+16);
                BaseSemantics::SValuePtr prod0 = ops->unsignedMultiply(x0, y0);
                BaseSemantics::SValuePtr prod1 = ops->unsignedMultiply(x1, y1);
                BaseSemantics::SValuePtr prod1Wide = ops->signExtend(prod1, 17);
                BaseSemantics::SValuePtr sum = d->saturateSignedToSigned(ops->add(ops->signExtend(prod0, 17), prod1Wide), 16);
                result = result ? ops->concat(result, sum) : sum;
            }
            d->write(args[0], result);
        }
    }
};

// Multiply and add packed integers
//   PMADDWD
struct IP_pmaddwd: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            const size_t bitsPerOp = 16;
            BaseSemantics::SValuePtr dst = d->read(args[0]);
            BaseSemantics::SValuePtr src = d->read(args[1]);
            ASSERT_require(dst->get_width() == src->get_width());
            size_t nOps = dst->get_width() / bitsPerOp;
            BaseSemantics::SValuePtr result;
            for (size_t i=0; i<nOps; i+=2) {
                BaseSemantics::SValuePtr x0 = ops->extract(src, (i+0)*bitsPerOp, (i+1)*bitsPerOp);
                BaseSemantics::SValuePtr x1 = ops->extract(src, (i+1)*bitsPerOp, (i+2)*bitsPerOp);
                BaseSemantics::SValuePtr y0 = ops->extract(dst, (i+0)*bitsPerOp, (i+1)*bitsPerOp);
                BaseSemantics::SValuePtr y1 = ops->extract(dst, (i+1)*bitsPerOp, (i+2)*bitsPerOp);
                BaseSemantics::SValuePtr prod0 = ops->unsignedMultiply(x0, y0);
                BaseSemantics::SValuePtr prod1 = ops->unsignedMultiply(x1, y1);
                BaseSemantics::SValuePtr sum = ops->add(prod0, prod1);
                result = result ? ops->concat(result, sum) : sum;
            }
            d->write(args[0], result);
        }
    }
};

// Maximum of packed signed integers
//   PMAXSB
//   PMAXSW
//   PMAXSD
struct IP_pmaxs: P {
    size_t bitsPerOp;
    IP_pmaxs(size_t bitsPerOp): bitsPerOp(bitsPerOp) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr a = d->read(args[0]);
            BaseSemantics::SValuePtr b = d->read(args[1]);
            ASSERT_require(a->get_width() == b->get_width());
            size_t nOps = a->get_width() / bitsPerOp;
            BaseSemantics::SValuePtr result;
            for (size_t i=0; i<nOps; ++i) {
                BaseSemantics::SValuePtr partA = ops->extract(a, i*bitsPerOp, (i+1)*bitsPerOp);
                BaseSemantics::SValuePtr partB = ops->extract(b, i*bitsPerOp, (i+1)*bitsPerOp);
                BaseSemantics::SValuePtr maxVal = ops->ite(ops->isSignedLessThan(partA, partB), partB, partA);
                result = result ? ops->concat(result, maxVal) : maxVal;
            }
            d->write(args[0], result);
        }
    }
};

// Maximum of packed unsigned integers
//   PMAXUB
//   PMAXUW
//   PMAXUD
struct IP_pmaxu: P {
    size_t bitsPerOp;
    IP_pmaxu(size_t bitsPerOp): bitsPerOp(bitsPerOp) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr a = d->read(args[0]);
            BaseSemantics::SValuePtr b = d->read(args[1]);
            ASSERT_require(a->get_width() == b->get_width());
            size_t nOps = a->get_width() / bitsPerOp;
            BaseSemantics::SValuePtr result;
            for (size_t i=0; i<nOps; ++i) {
                BaseSemantics::SValuePtr partA = ops->extract(a, i*bitsPerOp, (i+1)*bitsPerOp);
                BaseSemantics::SValuePtr partB = ops->extract(b, i*bitsPerOp, (i+1)*bitsPerOp);
                BaseSemantics::SValuePtr maxVal = ops->ite(ops->isUnsignedLessThan(partA, partB), partB, partA);
                result = result ? ops->concat(result, maxVal) : maxVal;
            }
            d->write(args[0], result);
        }
    }
};

// Minimum of packed signed integers
//   PMINSB
//   PMINSW
//   PMINSD
struct IP_pmins: P {
    size_t bitsPerOp;
    IP_pmins(size_t bitsPerOp): bitsPerOp(bitsPerOp) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr a = d->read(args[0]);
            BaseSemantics::SValuePtr b = d->read(args[1]);
            ASSERT_require(a->get_width() == b->get_width());
            size_t nOps = a->get_width() / bitsPerOp;
            BaseSemantics::SValuePtr result;
            for (size_t i=0; i<nOps; ++i) {
                BaseSemantics::SValuePtr partA = ops->extract(a, i*bitsPerOp, (i+1)*bitsPerOp);
                BaseSemantics::SValuePtr partB = ops->extract(b, i*bitsPerOp, (i+1)*bitsPerOp);
                BaseSemantics::SValuePtr minVal = ops->ite(ops->isSignedLessThan(partA, partB), partA, partB);
                result = result ? ops->concat(result, minVal) : minVal;
            }
            d->write(args[0], result);
        }
    }
};

// Minimum of packed unsigned integers
//   PMINUB
//   PMINUW
//   PMINUD
struct IP_pminu: P {
    size_t bitsPerOp;
    IP_pminu(size_t bitsPerOp): bitsPerOp(bitsPerOp) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr a = d->read(args[0]);
            BaseSemantics::SValuePtr b = d->read(args[1]);
            ASSERT_require(a->get_width() == b->get_width());
            size_t nOps = a->get_width() / bitsPerOp;
            BaseSemantics::SValuePtr result;
            for (size_t i=0; i<nOps; ++i) {
                BaseSemantics::SValuePtr partA = ops->extract(a, i*bitsPerOp, (i+1)*bitsPerOp);
                BaseSemantics::SValuePtr partB = ops->extract(b, i*bitsPerOp, (i+1)*bitsPerOp);
                BaseSemantics::SValuePtr minVal = ops->ite(ops->isUnsignedLessThan(partA, partB), partA, partB);
                result = result ? ops->concat(result, minVal) : minVal;
            }
            d->write(args[0], result);
        }
    }
};

// Move byte mask
//   PMOVMSKB
struct IP_pmovmskb: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr src = d->read(args[1]);
            BaseSemantics::SValuePtr result;
            for (size_t byteIdx=0; byteIdx<src->get_width()/8; ++byteIdx) {
                BaseSemantics::SValuePtr bit = ops->extract(src, 8*byteIdx+7, 8*byteIdx+8);
                result = result ? ops->concat(result, bit) : bit;
            }
            result = ops->unsignedExtend(result, asm_type_width(args[0]->get_type()));
            d->write(args[0], result);
        }
    }
};

// Packed move with sign extend
//   PMOVSXBW
//   PMOVSXBD
//   PMOVSXBQ
//   PMOVSXWD
//   PMOVSXWQ
//   PMOVSXDQ
struct IP_pmovsx: P {
    size_t srcBitsPerOp;
    size_t dstBitsPerOp;
    IP_pmovsx(size_t srcBitsPerOp, size_t dstBitsPerOp): srcBitsPerOp(srcBitsPerOp), dstBitsPerOp(dstBitsPerOp) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr src = d->read(args[1]);
            BaseSemantics::SValuePtr result;
            size_t nOps = asm_type_width(args[0]->get_type()) / dstBitsPerOp;
            for (size_t i=0; i<nOps; ++i) {
                BaseSemantics::SValuePtr part = ops->extract(src, i*srcBitsPerOp, (i+1)*srcBitsPerOp);
                part = ops->signExtend(part, dstBitsPerOp);
                result = result ? ops->concat(result, part) : part;
            }
            d->write(args[0], result);
        }
    }
};

// Packed move with zero extend
//   PMOVZXBW
//   PMOVZXBD
//   PMOVZXBQ
//   PMOVZXWD
//   PMOVZXWQ
//   PMOVZXDQ
struct IP_pmovzx: P {
    size_t srcBitsPerOp;
    size_t dstBitsPerOp;
    IP_pmovzx(size_t srcBitsPerOp, size_t dstBitsPerOp): srcBitsPerOp(srcBitsPerOp), dstBitsPerOp(dstBitsPerOp) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr src = d->read(args[1]);
            BaseSemantics::SValuePtr result;
            size_t nOps = asm_type_width(args[0]->get_type()) / dstBitsPerOp;
            for (size_t i=0; i<nOps; ++i) {
                BaseSemantics::SValuePtr part = ops->extract(src, i*srcBitsPerOp, (i+1)*srcBitsPerOp);
                part = ops->unsignedExtend(part, dstBitsPerOp);
                result = result ? ops->concat(result, part) : part;
            }
            d->write(args[0], result);
        }
    }
};

// Multiply packed signed dword integers
//   PMULDQ
struct IP_pmuldq: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr dst = d->read(args[0]);
            BaseSemantics::SValuePtr src = d->read(args[1]);
            ASSERT_require(dst->get_width() == src->get_width());
            BaseSemantics::SValuePtr dst_0_32 = ops->extract(dst, 0, 32);
            BaseSemantics::SValuePtr prod0 = ops->signedMultiply(ops->extract(src, 0, 32), dst_0_32);
            BaseSemantics::SValuePtr dst_64_96 = ops->extract(dst, 64, 96);
            BaseSemantics::SValuePtr prod1 = ops->signedMultiply(ops->extract(src, 64, 96), dst_64_96);
            BaseSemantics::SValuePtr result = ops->concat(prod0, prod1);
            d->write(args[0], result);
        }
    }
};

// Packed multiply high with round and scale
//   PMULHRSW
struct IP_pmulhrsw: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr a = d->read(args[0]);
            BaseSemantics::SValuePtr b = d->read(args[1]);
            ASSERT_require(a->get_width() == b->get_width());
            size_t nOps = a->get_width() / 16;
            BaseSemantics::SValuePtr result;
            for (size_t i=0; i<nOps; ++i) {
                BaseSemantics::SValuePtr partA = ops->extract(a, i*16, (i+1)*16);
                BaseSemantics::SValuePtr partB = ops->extract(b, i*16, (i+1)*16);
                BaseSemantics::SValuePtr product = ops->unsignedMultiply(partA, partB);
                BaseSemantics::SValuePtr one32 = ops->number_(32, 1);
                BaseSemantics::SValuePtr scaled = ops->extract(ops->add(ops->shiftRight(product, ops->number_(32, 14)),
                                                                        one32),
                                                               1, 17);
                result = result ? ops->concat(result, scaled) : scaled;
            }
            d->write(args[0], result);
        }
    }
};

// Multiply packed unsigned integers and store high result
//   PMULHUW
struct IP_pmulhuw: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr dst = d->read(args[0]);
            BaseSemantics::SValuePtr src = d->read(args[1]);
            ASSERT_require(dst->get_width() == src->get_width());
            const size_t bitsPerOp = 16;
            size_t nOps = dst->get_width() / bitsPerOp;
            BaseSemantics::SValuePtr result;
            for (size_t i=0; i<nOps; ++i) {
                BaseSemantics::SValuePtr term0 = ops->extract(dst, i*bitsPerOp, (i+1)*bitsPerOp);
                BaseSemantics::SValuePtr term1 = ops->extract(src, i*bitsPerOp, (i+1)*bitsPerOp);
                BaseSemantics::SValuePtr product = ops->unsignedMultiply(term0, term1);
                BaseSemantics::SValuePtr high = ops->extract(product, bitsPerOp, 2*bitsPerOp);
                result = result ? ops->concat(result, high) : high;
            }
            d->write(args[0], result);
        }
    }
};

// Multiply packed signed integers and store high result
//   PMULHW
struct IP_pmulhw: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr dst = d->read(args[0]);
            BaseSemantics::SValuePtr src = d->read(args[1]);
            ASSERT_require(dst->get_width() == src->get_width());
            const size_t bitsPerOp = 16;
            size_t nOps = dst->get_width() / bitsPerOp;
            BaseSemantics::SValuePtr result;
            for (size_t i=0; i<nOps; ++i) {
                BaseSemantics::SValuePtr term0 = ops->extract(dst, i*bitsPerOp, (i+1)*bitsPerOp);
                BaseSemantics::SValuePtr term1 = ops->extract(src, i*bitsPerOp, (i+1)*bitsPerOp);
                BaseSemantics::SValuePtr product = ops->signedMultiply(term0, term1);
                BaseSemantics::SValuePtr high = ops->extract(product, bitsPerOp, 2*bitsPerOp);
                result = result ? ops->concat(result, high) : high;
            }
            d->write(args[0], result);
        }
    }
};

// Multiply packed unsigned dword integers
//   PMULUDQ
struct IP_pmuludq: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr dst = d->read(args[0]);
            BaseSemantics::SValuePtr src = d->read(args[1]);
            ASSERT_require(dst->get_width() == src->get_width());
            size_t nOps = dst->get_width() / 64;
            BaseSemantics::SValuePtr result;
            for (size_t i=0; i<nOps; ++i) {
                BaseSemantics::SValuePtr term0 = ops->extract(dst, (2*i+0)*32, (2*i+1)*32);
                BaseSemantics::SValuePtr term1 = ops->extract(src, (2*i+0)*32, (2*i+1)*32);
                BaseSemantics::SValuePtr product = ops->unsignedMultiply(term0, term1);
                result = result ? ops->concat(result, product) : product;
            }
            d->write(args[0], result);
        }
    }
};

// Multiply packed signed dword integers and store low result
struct IP_pmull: P {
    size_t bitsPerOp;
    IP_pmull(size_t bitsPerOp): bitsPerOp(bitsPerOp) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr dst = d->read(args[0]);
            BaseSemantics::SValuePtr src = d->read(args[1]);
            ASSERT_require(dst->get_width() == src->get_width());
            size_t nOps = dst->get_width() / bitsPerOp;
            BaseSemantics::SValuePtr result;
            for (size_t i=0; i<nOps; ++i) {
                BaseSemantics::SValuePtr term0 = ops->extract(dst, i*bitsPerOp, (i+1)*bitsPerOp);
                BaseSemantics::SValuePtr term1 = ops->extract(src, i*bitsPerOp, (i+1)*bitsPerOp);
                BaseSemantics::SValuePtr product = ops->signedMultiply(term0, term1);
                BaseSemantics::SValuePtr low = ops->extract(product, 0, bitsPerOp);
                result = result ? ops->concat(result, low) : low;
            }
            d->write(args[0], result);
        }
    }
};

// Pop from stack
struct IP_pop: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            // Stack pointer register
            RegisterDescriptor sp;
            switch (insn->get_addressSize()) {
                case x86_insnsize_16: sp = d->REG_SP;  break;
                case x86_insnsize_32: sp = d->REG_ESP; break;
                case x86_insnsize_64: sp = d->REG_RSP; break;
                default:
                    ASSERT_not_reachable("invalid address size");
            }
            ASSERT_forbid(sp.isEmpty());
        
            // Increment the stack pointer before writing to args[0] just in case args[0] is stack-relative
            size_t operandWidth = asm_type_width(args[0]->get_type());
            ASSERT_require(operandWidth % 8 == 0);
            size_t stackDelta = operandWidth / 8;
            BaseSemantics::SValuePtr oldSp = d->readRegister(sp);
            BaseSemantics::SValuePtr newSp = ops->add(oldSp, ops->number_(sp.nBits(), stackDelta));
            d->writeRegister(sp, newSp);

            // Read from stack and write to args[0]
            BaseSemantics::SValuePtr yes = ops->boolean_(true);
            BaseSemantics::SValuePtr undefA = ops->undefined_(operandWidth);
            d->write(args[0], ops->readMemory(d->REG_SS, d->fixMemoryAddress(oldSp), undefA, yes));
        }
    }
};

// Pop flags register from stack
//   POPF
//   POPFD
//   POPFQ
struct IP_pop_flags: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr stackVa = d->readRegister(d->REG_anySP);
            BaseSemantics::SValuePtr oldFlags, poppedFlags;
            BaseSemantics::SValuePtr yes = ops->boolean_(true);
            RegisterDescriptor flagsReg;
            switch (insn->get_operandSize()) {
                case x86_insnsize_16: {
                    oldFlags = d->readRegister(flagsReg = d->REG_FLAGS);
                    poppedFlags = ops->readMemory(d->REG_SS, stackVa, ops->undefined_(16), yes);
                    break;
                }
                case x86_insnsize_32: {
                    oldFlags = d->readRegister(flagsReg = d->REG_EFLAGS);
                    poppedFlags = ops->readMemory(d->REG_SS, stackVa, ops->undefined_(32), yes);
                    break;
                }
                case x86_insnsize_64: {
                    oldFlags = d->readRegister(flagsReg = d->REG_RFLAGS);
                    poppedFlags = ops->readMemory(d->REG_SS, stackVa, ops->undefined_(64), yes);
                    BaseSemantics::SValuePtr zero32 = ops->number_(32, 0);
                    poppedFlags = ops->concat(ops->unsignedExtend(poppedFlags, 32), zero32);
                    break;
                }
                default:
                    ASSERT_not_reachable("invalid operand size");
            }
            stackVa = ops->add(stackVa, ops->number_(stackVa->get_width(), poppedFlags->get_width()/8));
            d->writeRegister(d->REG_anySP, stackVa);

            // Clear VIP (bit 19) and VIF (bit 20); i.e., clear bits 0x00180000
            // Keep IOPL (bits 12 and 13) and VM (bit 17), i.e., preserve bits 0x00023000
            BaseSemantics::SValuePtr newFlags = ops->extract(poppedFlags, 0, 12);
            newFlags = ops->concat(newFlags, ops->extract(oldFlags, 12, 14));        // IOPL (bits 12 & 13) is preserved
            newFlags = ops->concat(newFlags, ops->extract(poppedFlags, 14, 16));
            if (oldFlags->get_width() >= 32) {
                newFlags = ops->concat(newFlags, ops->extract(poppedFlags, 16, 17));
                newFlags = ops->concat(newFlags, ops->extract(oldFlags, 17, 18));    // VM (bit 17) is preserved
                newFlags = ops->concat(newFlags, ops->extract(poppedFlags, 18, 19));
                newFlags = ops->concat(newFlags, ops->number_(2, 0));                // VIP (19) and VIF (20) are cleared
                newFlags = ops->concat(newFlags, ops->extract(poppedFlags, 21, 32));
                if (oldFlags->get_width() == 64)
                    newFlags = ops->concat(newFlags, ops->number_(32, 0));
            }
            d->writeRegister(flagsReg, newFlags);
        }
    }
};

// Pop all general-purpose registers
//  POPA  - 16-bit registers
//  POPAD - 32-bit registers
//  Invalid for 64-bit
struct IP_pop_gprs: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else if (insn->get_addressSize() == x86_insnsize_16) {
            BaseSemantics::SValuePtr oldSp = d->readRegister(d->REG_anySP);
            BaseSemantics::SValuePtr newSp = ops->add(oldSp, ops->number_(oldSp->get_width(), 16));
            BaseSemantics::SValuePtr base = d->fixMemoryAddress(oldSp);
            BaseSemantics::SValuePtr yes = ops->boolean_(true);
            d->writeRegister(d->REG_DI,
                             ops->readMemory(d->REG_SS, base,
                                             ops->undefined_(16), yes));
            BaseSemantics::SValuePtr undefSI = ops->undefined_(16);
            d->writeRegister(d->REG_SI,
                             ops->readMemory(d->REG_SS, ops->add(base, ops->number_(base->get_width(), 2)), undefSI, yes));
            BaseSemantics::SValuePtr undefBP = ops->undefined_(16);
            d->writeRegister(d->REG_BP,
                             ops->readMemory(d->REG_SS, ops->add(base, ops->number_(base->get_width(), 4)), undefBP, yes));
            BaseSemantics::SValuePtr undefX = ops->undefined_(16);
            (void)           ops->readMemory(d->REG_SS, ops->add(base, ops->number_(base->get_width(), 6)), undefX, yes);
            BaseSemantics::SValuePtr undefBX = ops->undefined_(16);
            d->writeRegister(d->REG_BX,
                             ops->readMemory(d->REG_SS, ops->add(base, ops->number_(base->get_width(), 8)), undefBX, yes));
            BaseSemantics::SValuePtr undefDX = ops->undefined_(16);
            d->writeRegister(d->REG_DX,
                             ops->readMemory(d->REG_SS, ops->add(base, ops->number_(base->get_width(), 10)), undefDX, yes));
            BaseSemantics::SValuePtr undefCX = ops->undefined_(16);
            d->writeRegister(d->REG_CX,
                             ops->readMemory(d->REG_SS, ops->add(base, ops->number_(base->get_width(), 12)), undefCX, yes));
            BaseSemantics::SValuePtr undefAX = ops->undefined_(16);
            d->writeRegister(d->REG_AX,
                             ops->readMemory(d->REG_SS, ops->add(base, ops->number_(base->get_width(), 14)), undefAX, yes));
            d->writeRegister(d->REG_anySP, newSp);
        } else if (insn->get_addressSize() == x86_insnsize_32) {
            BaseSemantics::SValuePtr oldSp = d->readRegister(d->REG_anySP);
            BaseSemantics::SValuePtr newSp = ops->add(oldSp, ops->number_(oldSp->get_width(), 32));
            BaseSemantics::SValuePtr base = d->fixMemoryAddress(oldSp);
            BaseSemantics::SValuePtr yes = ops->boolean_(true);
            d->writeRegister(d->REG_EDI,
                             ops->readMemory(d->REG_SS, base,
                                             ops->undefined_(32), yes));
            BaseSemantics::SValuePtr undefESI = ops->undefined_(32);
            d->writeRegister(d->REG_ESI,
                             ops->readMemory(d->REG_SS, ops->add(base, ops->number_(base->get_width(), 4)), undefESI, yes));
            BaseSemantics::SValuePtr undefEBP = ops->undefined_(32);
            d->writeRegister(d->REG_EBP,
                             ops->readMemory(d->REG_SS, ops->add(base, ops->number_(base->get_width(), 8)), undefEBP, yes));
            BaseSemantics::SValuePtr undefX = ops->undefined_(32);
            (void)           ops->readMemory(d->REG_SS, ops->add(base, ops->number_(base->get_width(), 12)), undefX, yes);
            BaseSemantics::SValuePtr undefEBX = ops->undefined_(32);
            d->writeRegister(d->REG_EBX,
                             ops->readMemory(d->REG_SS, ops->add(base, ops->number_(base->get_width(), 16)), undefEBX, yes));
            BaseSemantics::SValuePtr undefEDX = ops->undefined_(32);
            d->writeRegister(d->REG_EDX,
                             ops->readMemory(d->REG_SS, ops->add(base, ops->number_(base->get_width(), 20)), undefEDX, yes));
            BaseSemantics::SValuePtr undefECX = ops->undefined_(32);
            d->writeRegister(d->REG_ECX,
                             ops->readMemory(d->REG_SS, ops->add(base, ops->number_(base->get_width(), 24)), undefECX, yes));
            BaseSemantics::SValuePtr undefEAX = ops->undefined_(32);
            d->writeRegister(d->REG_EAX,
                             ops->readMemory(d->REG_SS, ops->add(base, ops->number_(base->get_width(), 28)), undefEAX, yes));
            d->writeRegister(d->REG_anySP, newSp);
        } else {
            ops->interrupt(x86_exception_ud, 0);          // 64-bit mode
        }
    }
};

// Count number of bits set
struct IP_popcnt: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr src = d->read(args[1]);
            BaseSemantics::SValuePtr total = ops->number_(asm_type_width(args[0]->get_type()), 0);
            for (size_t i=0; i<src->get_width(); ++i) {
                BaseSemantics::SValuePtr srcBit = ops->extract(src, i, i+1);
                total = ops->add(total, ops->unsignedExtend(srcBit, total->get_width()));
            }
            d->write(args[0], total);
        }
    }
};

// Bitwise logical-OR (no flags affected)
//   POR
struct IP_por: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr dst = d->read(args[0]);
            BaseSemantics::SValuePtr src = d->read(args[1]);
            ASSERT_require(dst->get_width() == src->get_width());
            BaseSemantics::SValuePtr result = ops->or_(dst, src);
            d->write(args[0], result);
        }
    }
};

// Compute sum of absolute differences
//   PSADBW
struct IP_psadbw: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr dst = d->read(args[0]);
            BaseSemantics::SValuePtr src = d->read(args[1]);
            ASSERT_require(dst->get_width() == src->get_width());
            ASSERT_require(dst->get_width() == 64 || dst->get_width() == 128);
            BaseSemantics::SValuePtr result;
            size_t nSums = dst->get_width() / 64;
            for (size_t i=0; i<nSums; ++i) {
                BaseSemantics::SValuePtr sum;
                for (size_t j=0; j<8; ++j) {
                    BaseSemantics::SValuePtr partA = ops->extract(dst, i*64+j*8, i*64+j*8+8);
                    BaseSemantics::SValuePtr partB = ops->extract(src, i*64+j*8, i*64+j*8+8);
                    BaseSemantics::SValuePtr diffAB = ops->subtract(partA, partB);
                    BaseSemantics::SValuePtr diffBA = ops->subtract(partB, partA);
                    BaseSemantics::SValuePtr absDiff = ops->ite(ops->isUnsignedLessThan(partA, partB), diffBA, diffAB);
                    sum = sum ? ops->add(sum, absDiff) : absDiff;
                }
                sum = ops->unsignedExtend(sum, 64);
                result = result ? ops->concat(result, sum) : sum;
            }
            d->write(args[0], result);
        }
    }
};

// Shuffle packed bytes (or set to zero)
//   PSHUFB
struct IP_pshufb: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr original = d->read(args[0]);
            BaseSemantics::SValuePtr indices = d->read(args[1]);
            ASSERT_require(original->get_width() == indices->get_width());
            size_t nOps = original->get_width() / 8;
            size_t bitsPerIndex = 64 == original->get_width() ? 3 : 4;
            BaseSemantics::SValuePtr eight = ops->number_(4, 8);
            BaseSemantics::SValuePtr result;
            for (size_t i=0; i<nOps; ++i) {
                BaseSemantics::SValuePtr indexMsb = ops->extract(indices, i*8+7, i*8+8);
                BaseSemantics::SValuePtr index = ops->extract(indices, i*8, i*8+bitsPerIndex);
                // The extract operator only works with concrete bit indices, so we use right shift and masking instead.
                BaseSemantics::SValuePtr selected = ops->shiftRight(original, ops->unsignedMultiply(index, eight));
                selected = ops->unsignedExtend(selected, 8);
                selected = ops->ite(indexMsb, ops->number_(8, 0), selected);
                result = result ? ops->concat(result, selected) : selected;
            }
            d->write(args[0], result);
        }
    }
};

// Shuffle packed doublewords
//   PSHUFD
struct IP_pshufd: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr src = d->read(args[1]);
            BaseSemantics::SValuePtr order = d->read(args[2]);
            BaseSemantics::SValuePtr result;
            ASSERT_require(order->is_number());
            for (size_t i=0; i<4; ++i) {
                size_t dwordIdx = ((order->get_number() >> (i*2)) & 0x3);
                BaseSemantics::SValuePtr dword = ops->extract(src, 32*dwordIdx, 32*(dwordIdx+1));
                result = result ? ops->concat(result, dword) : dword;
            }
            d->write(args[0], result);
        }
    }
};

// Shuffle packed high words
//   PSHUFHW
struct IP_pshufhw: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        ASSERT_require(asm_type_width(args[0]->get_type()) == 128);
        ASSERT_require(asm_type_width(args[1]->get_type()) == 128);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr src = d->read(args[0]);
            BaseSemantics::SValuePtr result = ops->extract(src, 0, 64);
            size_t order = d->read(args[2])->get_number();// must be an immediate operand
            for (size_t i=0; i<4; ++i) {
                size_t wordIdx = (order >> (2*i)) & 3;
                BaseSemantics::SValuePtr word = ops->extract(src, (4+wordIdx)*16, (4+wordIdx+1)*16);
                result = ops->concat(result, word);
            }
            d->write(args[0], result);
        }
    }
};

// Shuffle packed low words
//   PSHUFLW
struct IP_pshuflw: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        ASSERT_require(asm_type_width(args[0]->get_type()) == 128);
        ASSERT_require(asm_type_width(args[1]->get_type()) == 128);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr src = d->read(args[0]);
            BaseSemantics::SValuePtr result;
            size_t order = d->read(args[2])->get_number();// must be an immediate operand
            for (size_t i=0; i<4; ++i) {
                size_t wordIdx = (order >> (2*i)) & 3;
                BaseSemantics::SValuePtr word = ops->extract(src, wordIdx*16, (wordIdx+1)*16);
                result = result ? ops->concat(result, word) : word;
            }
            result = ops->concat(result, ops->extract(src, 64, 128));
            d->write(args[0], result);
        }
    }
};

// Shuffle packed words
//   PSHUFW
struct IP_pshufw: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        ASSERT_require(asm_type_width(args[0]->get_type()) == 64);
        ASSERT_require(asm_type_width(args[1]->get_type()) == 64);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr src = d->read(args[0]);
            BaseSemantics::SValuePtr result;
            size_t order = d->read(args[2])->get_number();// must be an immediate operand
            for (size_t i=0; i<4; ++i) {
                size_t wordIdx = (order >> (2*i)) & 3;
                BaseSemantics::SValuePtr word = ops->extract(src, wordIdx*16, (wordIdx+1)*16);
                result = result ? ops->concat(result, word) : word;
            }
            d->write(args[0], result);
        }
    }
};

// Packed sign
//   PSIGNB
//   PSIGNW
//   PSIGND
struct IP_psign: P {
    size_t bitsPerOp;
    IP_psign(size_t bitsPerOp): bitsPerOp(bitsPerOp) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr a = d->read(args[0]);
            BaseSemantics::SValuePtr b = d->read(args[1]);
            ASSERT_require(a->get_width() == b->get_width());
            size_t nOps = a->get_width() / bitsPerOp;
            BaseSemantics::SValuePtr result;
            BaseSemantics::SValuePtr zero = ops->number_(bitsPerOp, 0);
            BaseSemantics::SValuePtr allSet = ops->invert(zero);
            for (size_t i=0; i<nOps; ++i) {
                BaseSemantics::SValuePtr partA = ops->extract(a, i*bitsPerOp, (i+1)*bitsPerOp);
                BaseSemantics::SValuePtr partB = ops->extract(b, i*bitsPerOp, (i+1)*bitsPerOp);
                BaseSemantics::SValuePtr isZero = ops->equalToZero(b);
                BaseSemantics::SValuePtr isNegative = ops->extract(b, bitsPerOp-1, bitsPerOp);
                BaseSemantics::SValuePtr partC = ops->ite(isNegative, allSet, ops->ite(isZero, zero, partA));
                result = result ? ops->concat(result, partC) : partC;
            }
            d->write(args[0], result);
        }
    }
};

// Shift double quadword left logical
//   PSLLDQ
struct IP_pslldq: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr src = d->read(args[0]);
            size_t nBytesShift = d->read(args[1])->get_number();// shift amount is an immediate operand
            BaseSemantics::SValuePtr sa = ops->number_(8, 8*nBytesShift);
            BaseSemantics::SValuePtr result = ops->shiftLeft(src, sa);
            d->write(args[0], result);
        }
    }
};

// Shift packed data left logical
//   PSLLW
//   PSLLD
//   PSLLQ
struct IP_psll: P {
    size_t bitsPerOp;
    IP_psll(size_t bitsPerOp): bitsPerOp(bitsPerOp) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr src = d->read(args[0]);
            BaseSemantics::SValuePtr sa = d->read(args[1]);
            size_t nOps = src->get_width() / bitsPerOp;
            BaseSemantics::SValuePtr result;
            for (size_t i=0; i<nOps; ++i) {
                BaseSemantics::SValuePtr part = ops->extract(src, i*bitsPerOp, (i+1)*bitsPerOp);
                BaseSemantics::SValuePtr shifted = ops->shiftLeft(part, sa);
                result = result ? ops->concat(result, shifted) : shifted;
            }
            d->write(args[0], result);
        }
    }
};

// Shift packed data right arithmetic
//   PSRAW
//   PSRAD
struct IP_psra: P {
    size_t bitsPerOp;
    IP_psra(size_t bitsPerOp): bitsPerOp(bitsPerOp) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr src = d->read(args[0]);
            BaseSemantics::SValuePtr sa = d->read(args[1]);
            size_t nOps = src->get_width() / bitsPerOp;
            BaseSemantics::SValuePtr result;
            for (size_t i=0; i<nOps; ++i) {
                BaseSemantics::SValuePtr part = ops->extract(src, i*bitsPerOp, (i+1)*bitsPerOp);
                BaseSemantics::SValuePtr shifted = ops->shiftRightArithmetic(part, sa);
                result = result ? ops->concat(result, shifted) : shifted;
            }
            d->write(args[0], result);
        }
    }
};

// Shift double quadword right logical
//   PSRLDQ
struct IP_psrldq: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr src = d->read(args[0]);
            size_t nBytesShift = d->read(args[1])->get_number();// shift amount is an immediate operand
            BaseSemantics::SValuePtr sa = ops->number_(8, 8*nBytesShift);
            BaseSemantics::SValuePtr result = ops->shiftRight(src, sa);
            d->write(args[0], result);
        }
    }
};

// Shift packed data right logical
//   PSRLW
//   PSRLD
//   PSRLQ
struct IP_psrl: P {
    size_t bitsPerOp;
    IP_psrl(size_t bitsPerOp): bitsPerOp(bitsPerOp) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr src = d->read(args[0]);
            BaseSemantics::SValuePtr sa = d->read(args[1]);
            size_t nOps = src->get_width() / bitsPerOp;
            BaseSemantics::SValuePtr result;
            for (size_t i=0; i<nOps; ++i) {
                BaseSemantics::SValuePtr part = ops->extract(src, i*bitsPerOp, (i+1)*bitsPerOp);
                BaseSemantics::SValuePtr shifted = ops->shiftRight(part, sa);
                result = result ? ops->concat(result, shifted) : shifted;
            }
            d->write(args[0], result);
        }
    }
};
    
// Subtract packed integers
//   PSUBB
//   PSUBW
//   PSUBD
//   PSUBQ
struct IP_psub: P {
    size_t bitsPerOp;
    IP_psub(size_t bitsPerOp): bitsPerOp(bitsPerOp) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr dst = d->read(args[0]); // minuends
            BaseSemantics::SValuePtr src = d->read(args[1]); // subtrahends
            BaseSemantics::SValuePtr result;
            size_t nOps = dst->get_width() / bitsPerOp;
            for (size_t i=0; i<nOps; ++i) {
                BaseSemantics::SValuePtr minuend = ops->extract(dst, i*bitsPerOp, (i+1)*bitsPerOp);
                BaseSemantics::SValuePtr subtrahend = ops->extract(src, i*bitsPerOp, (i+1)*bitsPerOp);
                BaseSemantics::SValuePtr difference = ops->subtract(minuend, subtrahend);
                result = result ? ops->concat(result, difference) : difference;
            }
            d->write(args[0], result);
        }
    }
};

// Subtract packed signed integers with signed saturation
//   PSUBSB
//   PSUBSW
struct IP_psubs: P {
    size_t bitsPerOp;
    IP_psubs(size_t bitsPerOp): bitsPerOp(bitsPerOp) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr dst = d->read(args[0]); // minuends
            BaseSemantics::SValuePtr src = d->read(args[1]); // subtrahends
            BaseSemantics::SValuePtr result;
            size_t nOps = dst->get_width() / bitsPerOp;
            for (size_t i=0; i<nOps; ++i) {
                BaseSemantics::SValuePtr minuend = ops->signExtend(ops->extract(dst, i*bitsPerOp, (i+1)*bitsPerOp),
                                                                   bitsPerOp+1);
                BaseSemantics::SValuePtr subtrahend = ops->signExtend(ops->extract(src, i*bitsPerOp, (i+1)*bitsPerOp),
                                                                      bitsPerOp+1);
                BaseSemantics::SValuePtr difference = d->saturateSignedToSigned(ops->subtract(minuend, subtrahend), bitsPerOp);
                result = result ? ops->concat(result, difference) : difference;
            }
            d->write(args[0], result);
        }
    }
};

// Subtract packed unsigned integers with unsigned saturation
//   PSUBUSB
//   PSUBUSW
struct IP_psubus: P {
    size_t bitsPerOp;
    IP_psubus(size_t bitsPerOp): bitsPerOp(bitsPerOp) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr dst = d->read(args[0]); // minuends
            BaseSemantics::SValuePtr src = d->read(args[1]); // subtrahends
            BaseSemantics::SValuePtr result;
            size_t nOps = dst->get_width() / bitsPerOp;
            for (size_t i=0; i<nOps; ++i) {
                BaseSemantics::SValuePtr minuend = ops->signExtend(ops->extract(dst, i*bitsPerOp, (i+1)*bitsPerOp),
                                                                   bitsPerOp+1);
                BaseSemantics::SValuePtr subtrahend = ops->signExtend(ops->extract(src, i*bitsPerOp, (i+1)*bitsPerOp),
                                                                      bitsPerOp+1);
                BaseSemantics::SValuePtr difference = d->saturateUnsignedToUnsigned(ops->subtract(minuend, subtrahend),
                                                                                    bitsPerOp);
                result = result ? ops->concat(result, difference) : difference;
            }
            d->write(args[0], result);
        }
    }
};

// Logical compare
//   PTEST
struct IP_ptest: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr a = d->read(args[0]);
            BaseSemantics::SValuePtr b = d->read(args[1]);
            ASSERT_require(a->get_width() == b->get_width());
            BaseSemantics::SValuePtr zf = ops->equalToZero(ops->and_(a, b));
            BaseSemantics::SValuePtr cf = ops->equalToZero(ops->and_(a, ops->invert(b)));
            BaseSemantics::SValuePtr no = ops->boolean_(false);
            d->writeRegister(d->REG_ZF, zf);
            d->writeRegister(d->REG_CF, cf);
            d->writeRegister(d->REG_AF, no);
            d->writeRegister(d->REG_OF, no);
            d->writeRegister(d->REG_PF, no);
            d->writeRegister(d->REG_SF, no);
        }
    }
};

// Unpack high data
//   PUNPCKHBW
//   PUNPCKHWD
//   PUNPCKHDQ
//   PUNPCKHQDQ
struct IP_punpckh: P {
    size_t bitsPerOp;                                   // number of bits read from each source operand for each operation
    IP_punpckh(size_t bitsPerOp): bitsPerOp(bitsPerOp) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr a = d->read(args[0]);
            BaseSemantics::SValuePtr b = d->read(args[1]);
            ASSERT_require(a->get_width() == b->get_width());
            BaseSemantics::SValuePtr result;
            size_t halfWidth = a->get_width() / 2;
            size_t nOps = halfWidth / bitsPerOp;
            for (size_t i=0; i<nOps; ++i) {
                BaseSemantics::SValuePtr partA = ops->extract(a, halfWidth + i*bitsPerOp, halfWidth + (i+1)*bitsPerOp);
                BaseSemantics::SValuePtr partB = ops->extract(b, halfWidth + i*bitsPerOp, halfWidth + (i+1)*bitsPerOp);
                BaseSemantics::SValuePtr pair = ops->concat(partA, partB);
                result = result ? ops->concat(result, pair) : pair;
            }
            d->write(args[0], result);
        }
    }
};

// Unpack low data
//   PUNPCKLBW
//   PUNPCKLWD
//   PUNPCKLDQ
//   PUNPCKLQDQ
struct IP_punpckl: P {
    size_t bitsPerOp;                                   // number of bits read from each source operand for each operation
    IP_punpckl(size_t bitsPerOp): bitsPerOp(bitsPerOp) {}
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr a = d->read(args[0]);
            BaseSemantics::SValuePtr b = d->read(args[1]);
            ASSERT_require(a->get_width() == b->get_width());
            BaseSemantics::SValuePtr result;
            size_t halfWidth = a->get_width() / 2;
            size_t nOps = halfWidth / bitsPerOp;
            for (size_t i=0; i<nOps; ++i) {
                BaseSemantics::SValuePtr partA = ops->extract(a, i*bitsPerOp, (i+1)*bitsPerOp);
                BaseSemantics::SValuePtr partB = ops->extract(b, i*bitsPerOp, (i+1)*bitsPerOp);
                BaseSemantics::SValuePtr pair = ops->concat(partA, partB);
                result = result ? ops->concat(result, pair) : pair;
            }
            d->write(args[0], result);
        }
    }
};

// Push onto stack
struct IP_push: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            // Stack pointer register
            RegisterDescriptor sp;
            switch (insn->get_addressSize()) {
                case x86_insnsize_16: sp = d->REG_SP;  break;
                case x86_insnsize_32: sp = d->REG_ESP; break;
                case x86_insnsize_64: sp = d->REG_RSP; break;
                default:
                    ASSERT_not_reachable("invalid address size");
            }
            ASSERT_forbid(sp.isEmpty());

            // Read the value to push onto the stack before decrementing the stack pointer.
            BaseSemantics::SValuePtr toPush = d->read(args[0]);
            if (isSgAsmIntegerValueExpression(args[0]) && toPush->get_width() < sp.nBits()) {
                toPush = ops->signExtend(toPush, sp.nBits());
            } else if (isSgAsmRegisterReferenceExpression(args[0]) && toPush->get_width() < sp.nBits() &&
                       (isSgAsmRegisterReferenceExpression(args[0])->get_descriptor() == d->REG_FS ||
                        isSgAsmRegisterReferenceExpression(args[0])->get_descriptor() == d->REG_GS)) {
                toPush = ops->unsignedExtend(toPush, sp.nBits());
            }
            
            // Decrement the stack pointer before writing to args[0] just in case args[0] is stack-relative
            int stackDelta = toPush->get_width() / 8;
            BaseSemantics::SValuePtr oldSp = d->readRegister(sp);
            BaseSemantics::SValuePtr newSp = ops->add(oldSp, ops->number_(sp.nBits(), -stackDelta));
            d->writeRegister(sp, newSp);

            // Write data to stack
            BaseSemantics::SValuePtr addr = d->fixMemoryAddress(newSp);
            ops->writeMemory(d->REG_SS, addr, toPush, ops->boolean_(true));
        }
    }
};

// Push all general-purpose registers
struct IP_push_gprs: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else if (insn->get_addressSize() == x86_insnsize_16) {
            BaseSemantics::SValuePtr oldSp = d->readRegister(d->REG_anySP);
            BaseSemantics::SValuePtr newSp = ops->add(oldSp, ops->number_(oldSp->get_width(), -16));
            BaseSemantics::SValuePtr base = d->fixMemoryAddress(newSp);
            BaseSemantics::SValuePtr yes = ops->boolean_(true);
            BaseSemantics::SValuePtr di = d->readRegister(d->REG_DI);
            ops->writeMemory(d->REG_SS, base, di, yes);
            BaseSemantics::SValuePtr si = d->readRegister(d->REG_SI);
            ops->writeMemory(d->REG_SS, ops->add(base, ops->number_(base->get_width(), 2)), si, yes);
            BaseSemantics::SValuePtr bp = d->readRegister(d->REG_BP);
            ops->writeMemory(d->REG_SS, ops->add(base, ops->number_(base->get_width(), 4)), bp, yes);
            ops->writeMemory(d->REG_SS, ops->add(base, ops->number_(base->get_width(), 6)), oldSp, yes);
            BaseSemantics::SValuePtr bx = d->readRegister(d->REG_BX);
            ops->writeMemory(d->REG_SS, ops->add(base, ops->number_(base->get_width(), 8)), bx, yes);
            BaseSemantics::SValuePtr dx = d->readRegister(d->REG_DX);
            ops->writeMemory(d->REG_SS, ops->add(base, ops->number_(base->get_width(), 10)), dx, yes);
            BaseSemantics::SValuePtr cx = d->readRegister(d->REG_CX);
            ops->writeMemory(d->REG_SS, ops->add(base, ops->number_(base->get_width(), 12)), cx, yes);
            BaseSemantics::SValuePtr ax = d->readRegister(d->REG_AX);
            ops->writeMemory(d->REG_SS, ops->add(base, ops->number_(base->get_width(), 14)), ax, yes);
            d->writeRegister(d->REG_anySP, newSp);
        } else if (insn->get_addressSize() == x86_insnsize_32) {
            BaseSemantics::SValuePtr oldSp = d->readRegister(d->REG_anySP);
            BaseSemantics::SValuePtr newSp = ops->add(oldSp, ops->number_(oldSp->get_width(), -32));
            BaseSemantics::SValuePtr base = d->fixMemoryAddress(newSp);
            BaseSemantics::SValuePtr yes = ops->boolean_(true);
            BaseSemantics::SValuePtr edi = d->readRegister(d->REG_EDI);
            ops->writeMemory(d->REG_SS, base, edi, yes);
            BaseSemantics::SValuePtr esi = d->readRegister(d->REG_ESI);
            ops->writeMemory(d->REG_SS, ops->add(base, ops->number_(base->get_width(), 4)), esi, yes);
            BaseSemantics::SValuePtr ebp = d->readRegister(d->REG_EBP);
            ops->writeMemory(d->REG_SS, ops->add(base, ops->number_(base->get_width(), 8)), ebp, yes);
            ops->writeMemory(d->REG_SS, ops->add(base, ops->number_(base->get_width(), 12)), oldSp, yes);
            BaseSemantics::SValuePtr ebx = d->readRegister(d->REG_EBX);
            ops->writeMemory(d->REG_SS, ops->add(base, ops->number_(base->get_width(), 16)), ebx, yes);
            BaseSemantics::SValuePtr edx = d->readRegister(d->REG_EDX);
            ops->writeMemory(d->REG_SS, ops->add(base, ops->number_(base->get_width(), 20)), edx, yes);
            BaseSemantics::SValuePtr ecx = d->readRegister(d->REG_ECX);
            ops->writeMemory(d->REG_SS, ops->add(base, ops->number_(base->get_width(), 24)), ecx, yes);
            BaseSemantics::SValuePtr eax = d->readRegister(d->REG_EAX);
            ops->writeMemory(d->REG_SS, ops->add(base, ops->number_(base->get_width(), 28)), eax, yes);
            d->writeRegister(d->REG_anySP, newSp);
        } else {
            ops->interrupt(x86_exception_ud, 0);        // 64-bit mode
        }
    }
};

// Push EFLAGS onto the stack
struct IP_push_flags: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            // Get the value to be pushed. Assume non-priviledged.
            BaseSemantics::SValuePtr flags;
            switch (insn->get_operandSize()) {
                case x86_insnsize_16:
                    flags = d->readRegister(d->REG_FLAGS);
                    break;
                case x86_insnsize_32:
                    flags = d->readRegister(d->REG_EFLAGS);
                    break;
                case x86_insnsize_64:
                    flags = d->readRegister(d->REG_RFLAGS);
                    break;
                default:
                    ASSERT_not_reachable("invalid operand size");
            }

            BaseSemantics::SValuePtr valueToPush = ops->extract(flags, 0, 16);
            if (flags->get_width() >= 32) {
                valueToPush = ops->concat(valueToPush, ops->number_(2, 0)); // clear VM and RF, bits 16 and 17
                valueToPush = ops->concat(valueToPush, ops->extract(flags, 18, 32));
                if (flags->get_width() == 64)
                    valueToPush = ops->concat(valueToPush, ops->extract(flags, 32, 64));
            }

            // Push value onto stack
            ASSERT_not_null(valueToPush);
            ASSERT_require(valueToPush->get_width() % 8 == 0);
            size_t valueSize = valueToPush->get_width() / 8;
            BaseSemantics::SValuePtr oldSp = d->readRegister(d->REG_anySP);
            BaseSemantics::SValuePtr newSp = ops->add(oldSp, ops->number_(oldSp->get_width(), -valueSize));
            BaseSemantics::SValuePtr addr = d->fixMemoryAddress(newSp);
            ops->writeMemory(d->REG_SS, addr, valueToPush, ops->boolean_(true));
            d->writeRegister(d->REG_anySP, newSp);
        }
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
        if (result==NULL) {
            BaseSemantics::SValuePtr arg1 = d->read(args[1], nbits);
            result = ops->xor_(d->read(args[0], nbits), arg1);
        }

        d->write(args[0], result);
    }
};

// Return from procedure
struct IP_ret: P {
    void p(D d, Ops ops, I insn, A args) {
        if (args.size()>1)
            throw BaseSemantics::Exception("instruction must have zero or one operand", insn);
        BaseSemantics::SValuePtr oldSp = d->readRegister(d->REG_anySP);
        size_t stackDelta = d->REG_anyIP.nBits() / 8;
        if (args.size() == 1) {
            ASSERT_require(isSgAsmIntegerValueExpression(args[0]));
            stackDelta += isSgAsmIntegerValueExpression(args[0])->get_absoluteValue();
        }
        BaseSemantics::SValuePtr newSp = ops->add(oldSp, ops->number_(oldSp->get_width(), stackDelta));
        BaseSemantics::SValuePtr stackVa = d->fixMemoryAddress(oldSp);
        BaseSemantics::SValuePtr yes = ops->boolean_(true);
        BaseSemantics::SValuePtr retVa = ops->filterReturnTarget(ops->readMemory(d->REG_SS, stackVa,
                                                                                 ops->undefined_(d->REG_anyIP.nBits()),
                                                                                 yes));
        d->writeRegister(d->REG_anyIP, retVa);
        d->writeRegister(d->REG_anySP, newSp);
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
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            size_t nbits = asm_type_width(args[0]->get_type());
            // FIXME: Intel documentation contains conflicting statements about the number of significant bits in the rotate
            // count.  We're doing what seems most reasonable: 6-bit counts for any operand (inc. CF) that's wider than 32
            // bits.
            size_t rotateWidth = (nbits>32 || (32==nbits && with_cf)) ? 6 : 5;
            BaseSemantics::SValuePtr operand = d->read(args[0]);
            if (with_cf)
                operand = ops->concat(operand, d->readRegister(d->REG_CF));
            BaseSemantics::SValuePtr rotateCount = d->read(args[1], 8);
            BaseSemantics::SValuePtr result = d->doRotateOperation(kind, operand, rotateCount, rotateWidth);
            // flags have been updated; we just need to store the result
            if (with_cf)
                result = ops->extract(result, 0, nbits);
            d->write(args[0], result);
        }
    }
};

// Integer subtraction with borrow
struct IP_sbb: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix() && !isSgAsmMemoryReferenceExpression(args[0])) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr minuend = d->read(args[0]);
            BaseSemantics::SValuePtr subtrahend = d->read(args[1]);
            if (minuend->get_width() > subtrahend->get_width())
                subtrahend = ops->signExtend(subtrahend, minuend->get_width());
            ASSERT_require(minuend->get_width() == subtrahend->get_width());
            BaseSemantics::SValuePtr cf = d->readRegister(d->REG_CF);
            BaseSemantics::SValuePtr difference = d->doAddOperation(minuend, ops->invert(subtrahend), true, cf);
            d->write(args[0], difference);
        }
    }
};

// Scan string
// The disassembler produces SCASB, SCASW, SCASD, or SCASQ without any arguments (never SCAS with an arg)
struct IP_scanstring: P {
    const X86RepeatPrefix repeat;
    const size_t nbits;
    const size_t nbytes;
    IP_scanstring(X86RepeatPrefix repeat, size_t nbits): repeat(repeat), nbits(nbits), nbytes(nbits/8) {
        ASSERT_require(8==nbits || 16==nbits || 32==nbits || 64==nbits);
    }
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr inLoop = d->repEnter(repeat);

            // Get the address for the string
            RegisterDescriptor stringReg;
            switch (insn->get_addressSize()) {
                case x86_insnsize_16:
                    stringReg = d->REG_DI;
                    break;
                case x86_insnsize_32:
                    stringReg = d->REG_EDI;
                    break;
                case x86_insnsize_64:
                    stringReg = d->REG_RDI;
                    break;
                default:
                    ASSERT_not_reachable("invalid instruction address size");
            }
            ASSERT_forbid(stringReg.isEmpty());
            BaseSemantics::SValuePtr stringPtr = d->readRegister(stringReg);

            // Adjust address width based on how memory is accessed.  All addresses in memory have the same width.
            BaseSemantics::SValuePtr addr = d->fixMemoryAddress(stringPtr);

            // Compare values and set status flags.
            RegisterDescriptor compareReg = d->REG_AX; compareReg.nBits(nbits);
            BaseSemantics::SValuePtr val1 = d->readRegister(compareReg);
            BaseSemantics::SValuePtr val2 = ops->readMemory(d->REG_ES, addr, ops->undefined_(nbits), inLoop);
            BaseSemantics::SValuePtr no = ops->boolean_(false);
            (void) d->doAddOperation(val1, ops->invert(val2), true, no, inLoop);

            // Advance string pointer register
            BaseSemantics::SValuePtr positiveNBytes = ops->number_(stringReg.nBits(), +nbytes);
            BaseSemantics::SValuePtr negativeNBytes = ops->number_(stringReg.nBits(), -nbytes);
            BaseSemantics::SValuePtr step = ops->ite(d->readRegister(d->REG_DF), negativeNBytes, positiveNBytes);
            d->writeRegister(stringReg, ops->ite(inLoop, ops->add(stringPtr, step), stringPtr));

            // Adjust the instruction pointer register to either repeat the instruction or fall through
            if (x86_repeat_none != repeat)
                d->repLeave(repeat, inLoop, insn->get_address(), true/*use ZF*/);
        }
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
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr cond = d->flagsCombo(kind);
            d->write(args[0], ops->concat(cond, ops->number_(7, 0)));
        }
    }
};

// Shift instructions: SHL, SAR, and SHR
struct IP_shift_1: P {
    const X86InstructionKind kind;
    IP_shift_1(X86InstructionKind k): kind(k) {
        ASSERT_require(x86_shr==k || x86_sar==k || x86_shl==k);
    }
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        ASSERT_require(insn->get_kind()==kind);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            size_t nbits = asm_type_width(args[0]->get_type());
            size_t shiftSignificantBits = nbits <= 32 ? 5 : 6;
            BaseSemantics::SValuePtr arg1 = d->read(args[1], 8);
            BaseSemantics::SValuePtr unspecified = ops->unspecified_(nbits);
            BaseSemantics::SValuePtr result = d->doShiftOperation(kind, d->read(args[0]), unspecified,
                                                                  arg1, shiftSignificantBits);
            d->write(args[0], result);
        }
    }
};

// Double-wide shift: SHLD
struct IP_shift_2: P {
    const X86InstructionKind kind;
    IP_shift_2(X86InstructionKind k): kind(k) {
        ASSERT_require(x86_shld==k || x86_shrd==k);
    }
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        ASSERT_require(insn->get_kind()==kind);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            size_t halfWidth = asm_type_width(args[0]->get_type());
            size_t shiftSignificantBits = halfWidth <= 32 ? 5 : 6;
            BaseSemantics::SValuePtr arg2 = d->read(args[2], 8);
            BaseSemantics::SValuePtr arg1 = d->read(args[1]);
            BaseSemantics::SValuePtr result = d->doShiftOperation(kind, d->read(args[0]), arg1, arg2, shiftSignificantBits);
            d->write(args[0], result);
        }
    }
};

// Set carry flag
struct IP_stc: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            d->writeRegister(d->REG_CF, ops->boolean_(true));
        }
    }
};

// Set direction flag
struct IP_std: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            d->writeRegister(d->REG_DF, ops->boolean_(true));
        }
    }
};

// Store string
// The disassembler produces STOSB, STOSW, STOSD, or STOSQ without any arguments (never STOS with an arg)
struct IP_storestring: P {
    const X86RepeatPrefix repeat;
    const size_t nbits;
    const size_t nbytes;
    IP_storestring(X86RepeatPrefix repeat, size_t nbits): repeat(repeat), nbits(nbits), nbytes(nbits/8) {
        ASSERT_require(8==nbits || 16==nbits || 32==nbits || 64==nbits);
    }
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            // Get the address for storing.
            RegisterDescriptor dstReg;
            switch (insn->get_addressSize()) {
                case x86_insnsize_16:
                    dstReg = d->REG_DI;
                    break;
                case x86_insnsize_32:
                    dstReg = d->REG_EDI;
                    break;
                case x86_insnsize_64:
                    dstReg = d->REG_RDI;
                    break;
                default:
                    ASSERT_not_reachable("invalid instruction address size");
            }
            ASSERT_forbid(dstReg.isEmpty());
            BaseSemantics::SValuePtr stringPtr = d->readRegister(dstReg);
            BaseSemantics::SValuePtr addr = d->fixMemoryAddress(stringPtr);
            BaseSemantics::SValuePtr directionFlag = d->readRegister(d->REG_DF);
            BaseSemantics::SValuePtr positiveNBytes = ops->number_(dstReg.nBits(), +nbytes);
            BaseSemantics::SValuePtr step = ops->ite(directionFlag,
                                                     ops->number_(dstReg.nBits(), -nbytes),
                                                     positiveNBytes);

            // Source value
            RegisterDescriptor regA = d->REG_AX; regA.nBits(nbits);
            BaseSemantics::SValuePtr src = d->readRegister(regA);

            // If CX is a known value then we can unroll the loop right now.
            if (x86_repeat_repe==repeat) {
                BaseSemantics::SValuePtr cx = d->readRegister(d->REG_anyCX);
                if (cx->is_number() && cx->get_number() <= 8192 /*arbitrary*/) {
                    size_t n = cx->get_number();
                    BaseSemantics::SValuePtr inLoop = ops->boolean_(true);
                    for (size_t i=0; i<n; ++i) {
                        BaseSemantics::SValuePtr va =
                            ops->add(addr,
                                     ops->unsignedExtend(ops->unsignedMultiply(ops->number_(addr->get_width(), i), step),
                                                         addr->get_width()));
                        ops->writeMemory(d->REG_ES, va, src, inLoop);
                    }
                    d->writeRegister(d->REG_anyCX, ops->number_(d->REG_anyCX.nBits(), 0));

                    // Final value for (E)DI register
                    BaseSemantics::SValuePtr va =
                        ops->add(addr,
                                 ops->unsignedExtend(ops->unsignedMultiply(ops->number_(addr->get_width(), n), step),
                                                     addr->get_width()));
                    d->writeRegister(dstReg, va);
                    return;
                }
            }
            
            // We chose not to unroll the loop, so simulate the loop by manipulating the instruction pointer
            BaseSemantics::SValuePtr inLoop = d->repEnter(repeat);

            // Copy value from AL/AX/EAX/RAX to memory
            ops->writeMemory(d->REG_ES, addr, src, inLoop);

            // Advance pointer register
            d->writeRegister(dstReg, ops->ite(inLoop, ops->add(stringPtr, step), stringPtr));

            // Adjust the instruction pointer register to either repeat the instruction or fall through
            if (x86_repeat_none != repeat)
                d->repLeave(repeat, inLoop, insn->get_address(), false/*no ZF*/);
        }
    }
};

// Store SSE control and status register
struct IP_stmxcsr: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            d->write(args[0], d->readRegister(d->REG_MXCSR));
        }
    }
};

// Subtract two values
// Intel documentation has a "SUB r/m64, r32" mode, but I think it should be "SUB r/m64, r64".
struct IP_sub: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix() && !isSgAsmMemoryReferenceExpression(args[0])) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr minuend = d->read(args[0]);
            BaseSemantics::SValuePtr subtrahend = d->read(args[1]);
            if (minuend->get_width() > subtrahend->get_width())
                subtrahend = ops->signExtend(subtrahend, minuend->get_width());
            ASSERT_require(minuend->get_width() == subtrahend->get_width());
            BaseSemantics::SValuePtr no = ops->boolean_(false);
            BaseSemantics::SValuePtr difference = d->doAddOperation(minuend, ops->invert(subtrahend), true, no);
            d->write(args[0], difference);
        }
    }
};

// Fast system call
struct IP_syscall: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            ops->interrupt(x86_exception_syscall, 0);
        }
    }
};

// Fast system call
struct IP_sysenter: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            ops->interrupt(x86_exception_sysenter, 0);
        }
    }
};

// Same as x86_and except doesn't write result back to machine state
struct IP_test: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix()) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr a = d->read(args[0]);
            BaseSemantics::SValuePtr b = d->read(args[1]);
            if (a->get_width() > b->get_width())
                b = ops->signExtend(b, a->get_width());
            ASSERT_require(a->get_width() == b->get_width());
            BaseSemantics::SValuePtr result = ops->and_(a, b);
            d->setFlagsForResult(result);
            d->writeRegister(d->REG_OF, ops->boolean_(false));
            d->writeRegister(d->REG_AF, ops->unspecified_(1));
            d->writeRegister(d->REG_CF, ops->boolean_(false));
        }
    }
};

// Undefined instruction: UD2
struct IP_ud2: P {
    void p(D d, Ops ops, I insn, A args) {
        ops->interrupt(x86_exception_ud, 0);
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
        if (insn->get_lockPrefix() &&
            !isSgAsmMemoryReferenceExpression(args[0]) && !isSgAsmMemoryReferenceExpression(args[1])) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr no = ops->boolean_(false);
            BaseSemantics::SValuePtr arg1 = d->read(args[1]);
            BaseSemantics::SValuePtr sum = d->doAddOperation(d->read(args[0]), arg1, false, no);
            d->write(args[1], d->read(args[0]));
            d->write(args[0], sum);
        }
    }
};

// Exchange
struct IP_xchg: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix() &&
            !isSgAsmMemoryReferenceExpression(args[0]) && !isSgAsmMemoryReferenceExpression(args[1])) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr temp = d->read(args[0]);
            d->write(args[0], d->read(args[1]));
            d->write(args[1], temp);
        }
    }
};

// Bitwise XOR
//      XOR
//      XORPD
//      XORPS
struct IP_xor: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        if (insn->get_lockPrefix() && !isSgAsmMemoryReferenceExpression(args[0])) {
            ops->interrupt(x86_exception_ud, 0);
        } else {
            BaseSemantics::SValuePtr result;

            if (isSgAsmDirectRegisterExpression(args[0]) && isSgAsmDirectRegisterExpression(args[1]) &&
                (isSgAsmRegisterReferenceExpression(args[0])->get_descriptor() ==
                 isSgAsmRegisterReferenceExpression(args[1])->get_descriptor())) {
                // XOR of a register with itself is an x86 idiom for setting the register to zero, so treat it as such
                size_t nbits = asm_type_width(args[0]->get_type());
                result = ops->number_(nbits, 0);
            } else {
                // The non-idiomatic behavior
                BaseSemantics::SValuePtr a = d->read(args[0]);
                BaseSemantics::SValuePtr b = d->read(args[1]);
                if (a->get_width() > b->get_width())
                    b = ops->signExtend(b, a->get_width());
                ASSERT_require(a->get_width() == b->get_width());
                result = ops->xor_(a, b);
            }
        
            d->setFlagsForResult(result);
            d->write(args[0], result);
            d->writeRegister(d->REG_OF, ops->boolean_(false));
            d->writeRegister(d->REG_AF, ops->unspecified_(1));
            d->writeRegister(d->REG_CF, ops->boolean_(false));
        }
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
    iproc_set(x86_cmpsq,        new X86::IP_cmpstrings(x86_repeat_none, 64));
    iproc_set(x86_cmpxchg,      new X86::IP_cmpxchg);
    iproc_set(x86_cmpxchg8b,    new X86::IP_cmpxchg2);
    iproc_set(x86_cmpxchg16b,   new X86::IP_cmpxchg2);
    iproc_set(x86_cpuid,        new X86::IP_cpuid);
    iproc_set(x86_cqo,          new X86::IP_cqo);
    iproc_set(x86_cwd,          new X86::IP_cwd);
    iproc_set(x86_cwde,         new X86::IP_cwde);
    iproc_set(x86_dec,          new X86::IP_dec);
    iproc_set(x86_div,          new X86::IP_divide(x86_div));
    iproc_set(x86_fld,          new X86::IP_fld);
    iproc_set(x86_fldcw,        new X86::IP_fldcw);
    iproc_set(x86_fnop,         new X86::IP_fnop);
    iproc_set(x86_fnstcw,       new X86::IP_fnstcw);
    iproc_set(x86_fnstsw,       new X86::IP_fnstsw);
    iproc_set(x86_fst,          new X86::IP_fst);
    iproc_set(x86_fstp,         new X86::IP_fstp);
    iproc_set(x86_hlt,          new X86::IP_hlt);
    iproc_set(x86_idiv,         new X86::IP_divide(x86_idiv));
    iproc_set(x86_imul,         new X86::IP_imul);
    iproc_set(x86_inc,          new X86::IP_inc);
    iproc_set(x86_int,          new X86::IP_int);
    iproc_set(x86_int3,         new X86::IP_int3);
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
    iproc_set(x86_lddqu,        new X86::IP_move_same);
    iproc_set(x86_ldmxcsr,      new X86::IP_ldmxcsr);
    iproc_set(x86_lea,          new X86::IP_lea);
    iproc_set(x86_leave,        new X86::IP_leave);
    iproc_set(x86_lfence,       new X86::IP_nop);
    iproc_set(x86_lodsb,        new X86::IP_loadstring(x86_repeat_none, 8));
    iproc_set(x86_lodsw,        new X86::IP_loadstring(x86_repeat_none, 16));
    iproc_set(x86_lodsd,        new X86::IP_loadstring(x86_repeat_none, 32));
    iproc_set(x86_lodsq,        new X86::IP_loadstring(x86_repeat_none, 64));
    iproc_set(x86_loop,         new X86::IP_loop(x86_loop));
    iproc_set(x86_loopnz,       new X86::IP_loop(x86_loopnz));
    iproc_set(x86_loopz,        new X86::IP_loop(x86_loopz));
    iproc_set(x86_maskmovq,     new X86::IP_maskmov);
    iproc_set(x86_mfence,       new X86::IP_nop);
    iproc_set(x86_mov,          new X86::IP_mov);
    iproc_set(x86_movapd,       new X86::IP_move_same);
    iproc_set(x86_movaps,       new X86::IP_move_same);
    iproc_set(x86_movbe,        new X86::IP_movbe);
    iproc_set(x86_movd,         new X86::IP_move_zero_extend);
    iproc_set(x86_movdqa,       new X86::IP_move_same);
    iproc_set(x86_movdqu,       new X86::IP_move_same);
    iproc_set(x86_movhpd,       new X86::IP_movhpd);
    iproc_set(x86_movlpd,       new X86::IP_movlpd);
    iproc_set(x86_movq,         new X86::IP_move_zero_extend);
    iproc_set(x86_movntdqa,     new X86::IP_move_same);
    iproc_set(x86_movntdq,      new X86::IP_move_same);
    iproc_set(x86_movnti,       new X86::IP_move_same);
    iproc_set(x86_movntq,       new X86::IP_move_same);
    iproc_set(x86_movsb,        new X86::IP_movestring(x86_repeat_none, 8));
    iproc_set(x86_movsw,        new X86::IP_movestring(x86_repeat_none, 16));
    iproc_set(x86_movsd,        new X86::IP_movestring(x86_repeat_none, 32));
    iproc_set(x86_movsq,        new X86::IP_movestring(x86_repeat_none, 64));
    iproc_set(x86_movsd_sse,    new X86::IP_movsd);
    iproc_set(x86_movss,        new X86::IP_movss);
    iproc_set(x86_movsx,        new X86::IP_move_sign_extend);
    iproc_set(x86_movsxd,       new X86::IP_move_sign_extend);
    iproc_set(x86_movupd,       new X86::IP_move_same);
    iproc_set(x86_movups,       new X86::IP_move_same);
    iproc_set(x86_movzx,        new X86::IP_move_zero_extend);
    iproc_set(x86_mul,          new X86::IP_mul);
    iproc_set(x86_neg,          new X86::IP_neg);
    iproc_set(x86_nop,          new X86::IP_nop);
    iproc_set(x86_not,          new X86::IP_not);
    iproc_set(x86_or,           new X86::IP_or);
    iproc_set(x86_pabsb,        new X86::IP_pabs(8));
    iproc_set(x86_pabsw,        new X86::IP_pabs(16));
    iproc_set(x86_pabsd,        new X86::IP_pabs(32));
    iproc_set(x86_packssdw,     new X86::IP_packss(32, 16));
    iproc_set(x86_packsswb,     new X86::IP_packss(16, 8));
    iproc_set(x86_packusdw,     new X86::IP_packus(32, 16));
    iproc_set(x86_packuswb,     new X86::IP_packus(16, 8));
    iproc_set(x86_paddb,        new X86::IP_padd(8));
    iproc_set(x86_paddw,        new X86::IP_padd(16));
    iproc_set(x86_paddd,        new X86::IP_padd(32));
    iproc_set(x86_paddq,        new X86::IP_padd(64));
    iproc_set(x86_paddsb,       new X86::IP_padds(8));
    iproc_set(x86_paddsw,       new X86::IP_padds(16));
    iproc_set(x86_paddusb,      new X86::IP_paddus(8));
    iproc_set(x86_paddusw,      new X86::IP_paddus(16));
    iproc_set(x86_palignr,      new X86::IP_palignr);
    iproc_set(x86_pand,         new X86::IP_pand);
    iproc_set(x86_pandn,        new X86::IP_pandn);
    iproc_set(x86_pause,        new X86::IP_nop);
    iproc_set(x86_pavgb,        new X86::IP_pavg(8));
    iproc_set(x86_pavgw,        new X86::IP_pavg(16));
    iproc_set(x86_pblendvb,     new X86::IP_pblendvb);
    iproc_set(x86_pblendw,      new X86::IP_pblendw);
    iproc_set(x86_pcmpeqb,      new X86::IP_pcmpeq(8));
    iproc_set(x86_pcmpeqw,      new X86::IP_pcmpeq(16));
    iproc_set(x86_pcmpeqd,      new X86::IP_pcmpeq(32));
    iproc_set(x86_pcmpeqq,      new X86::IP_pcmpeq(64));
    iproc_set(x86_pcmpgtb,      new X86::IP_pcmpgt(8));
    iproc_set(x86_pcmpgtw,      new X86::IP_pcmpgt(16));
    iproc_set(x86_pcmpgtd,      new X86::IP_pcmpgt(32));
    iproc_set(x86_pcmpgtq,      new X86::IP_pcmpgt(64));
    iproc_set(x86_pextrb,       new X86::IP_pextr(8));
    iproc_set(x86_pextrw,       new X86::IP_pextr(16));
    iproc_set(x86_pextrd,       new X86::IP_pextr(32));
    iproc_set(x86_pextrq,       new X86::IP_pextr(64));
    iproc_set(x86_phaddw,       new X86::IP_phadd(16));
    iproc_set(x86_phaddd,       new X86::IP_phadd(32));
    iproc_set(x86_phaddsw,      new X86::IP_phadds(16));
    iproc_set(x86_phminposuw,   new X86::IP_phminposuw);
    iproc_set(x86_phsubw,       new X86::IP_phsub(16));
    iproc_set(x86_phsubd,       new X86::IP_phsub(32));
    iproc_set(x86_phsubsw,      new X86::IP_phsubs(16));
    iproc_set(x86_pinsrb,       new X86::IP_pinsr(8));
    iproc_set(x86_pinsrw,       new X86::IP_pinsr(16));
    iproc_set(x86_pinsrd,       new X86::IP_pinsr(32));
    iproc_set(x86_pinsrq,       new X86::IP_pinsr(64));
    iproc_set(x86_pmaddubsw,    new X86::IP_pmaddubsw);
    iproc_set(x86_pmaddwd,      new X86::IP_pmaddwd);
    iproc_set(x86_pmaxsb,       new X86::IP_pmaxs(8));
    iproc_set(x86_pmaxsw,       new X86::IP_pmaxs(16));
    iproc_set(x86_pmaxsd,       new X86::IP_pmaxs(32));
    iproc_set(x86_pmaxub,       new X86::IP_pmaxu(8));
    iproc_set(x86_pmaxuw,       new X86::IP_pmaxu(16));
    iproc_set(x86_pmaxud,       new X86::IP_pmaxu(32));
    iproc_set(x86_pminsb,       new X86::IP_pmins(8));
    iproc_set(x86_pminsw,       new X86::IP_pmins(16));
    iproc_set(x86_pminsd,       new X86::IP_pmins(32));
    iproc_set(x86_pminub,       new X86::IP_pminu(8));
    iproc_set(x86_pminuw,       new X86::IP_pminu(16));
    iproc_set(x86_pminud,       new X86::IP_pminu(32));
    iproc_set(x86_pmovmskb,     new X86::IP_pmovmskb);
    iproc_set(x86_pmovsxbw,     new X86::IP_pmovsx(8, 16));
    iproc_set(x86_pmovsxbd,     new X86::IP_pmovsx(8, 32));
    iproc_set(x86_pmovsxbq,     new X86::IP_pmovsx(8, 64));
    iproc_set(x86_pmovsxwd,     new X86::IP_pmovsx(16, 32));
    iproc_set(x86_pmovsxwq,     new X86::IP_pmovsx(16, 64));
    iproc_set(x86_pmovsxdq,     new X86::IP_pmovsx(32, 64));
    iproc_set(x86_pmovzxbw,     new X86::IP_pmovzx(8, 16));
    iproc_set(x86_pmovzxbd,     new X86::IP_pmovzx(8, 32));
    iproc_set(x86_pmovzxbq,     new X86::IP_pmovzx(8, 64));
    iproc_set(x86_pmovzxwd,     new X86::IP_pmovzx(16, 32));
    iproc_set(x86_pmovzxwq,     new X86::IP_pmovzx(16, 64));
    iproc_set(x86_pmovzxdq,     new X86::IP_pmovzx(32, 64));
    iproc_set(x86_pmuldq,       new X86::IP_pmuldq);
    iproc_set(x86_pmulhrsw,     new X86::IP_pmulhrsw);
    iproc_set(x86_pmulhuw,      new X86::IP_pmulhuw);
    iproc_set(x86_pmulhw,       new X86::IP_pmulhw);
    iproc_set(x86_pmulld,       new X86::IP_pmull(32));
    iproc_set(x86_pmullw,       new X86::IP_pmull(16));
    iproc_set(x86_pmuludq,      new X86::IP_pmuludq);
    iproc_set(x86_pop,          new X86::IP_pop);
    iproc_set(x86_popa,         new X86::IP_pop_gprs);
    iproc_set(x86_popad,        new X86::IP_pop_gprs);
    iproc_set(x86_popf,         new X86::IP_pop_flags);
    iproc_set(x86_popfd,        new X86::IP_pop_flags);
    iproc_set(x86_popfq,        new X86::IP_pop_flags);
    iproc_set(x86_popcnt,       new X86::IP_popcnt);
    iproc_set(x86_por,          new X86::IP_por);
    iproc_set(x86_prefetchnta,  new X86::IP_nop);
    iproc_set(x86_psadbw,       new X86::IP_psadbw);
    iproc_set(x86_pshufb,       new X86::IP_pshufb);
    iproc_set(x86_pshufd,       new X86::IP_pshufd);
    iproc_set(x86_pshufhw,      new X86::IP_pshufhw);
    iproc_set(x86_pshuflw,      new X86::IP_pshuflw);
    iproc_set(x86_pshufw,       new X86::IP_pshufw);
    iproc_set(x86_psignb,       new X86::IP_psign(8));
    iproc_set(x86_psignw,       new X86::IP_psign(16));
    iproc_set(x86_psignd,       new X86::IP_psign(32));
    iproc_set(x86_pslldq,       new X86::IP_pslldq);
    iproc_set(x86_psllw,        new X86::IP_psll(16));
    iproc_set(x86_pslld,        new X86::IP_psll(32));
    iproc_set(x86_psllq,        new X86::IP_psll(64));
    iproc_set(x86_psraw,        new X86::IP_psra(16));
    iproc_set(x86_psrad,        new X86::IP_psra(32));
    iproc_set(x86_psrldq,       new X86::IP_psrldq);
    iproc_set(x86_psrlw,        new X86::IP_psrl(16));
    iproc_set(x86_psrld,        new X86::IP_psrl(32));
    iproc_set(x86_psrlq,        new X86::IP_psrl(64));
    iproc_set(x86_psubb,        new X86::IP_psub(8));
    iproc_set(x86_psubw,        new X86::IP_psub(16));
    iproc_set(x86_psubd,        new X86::IP_psub(32));
    iproc_set(x86_psubq,        new X86::IP_psub(64));
    iproc_set(x86_psubsb,       new X86::IP_psubs(8));
    iproc_set(x86_psubsw,       new X86::IP_psubs(16));
    iproc_set(x86_psubusb,      new X86::IP_psubus(8));
    iproc_set(x86_psubusw,      new X86::IP_psubus(16));
    iproc_set(x86_ptest,        new X86::IP_ptest);
    iproc_set(x86_punpckhbw,    new X86::IP_punpckh(8));
    iproc_set(x86_punpckhwd,    new X86::IP_punpckh(16));
    iproc_set(x86_punpckhdq,    new X86::IP_punpckh(32));
    iproc_set(x86_punpckhqdq,   new X86::IP_punpckh(64));
    iproc_set(x86_punpcklbw,    new X86::IP_punpckl(8));
    iproc_set(x86_punpcklwd,    new X86::IP_punpckl(16));
    iproc_set(x86_punpckldq,    new X86::IP_punpckl(32));
    iproc_set(x86_punpcklqdq,   new X86::IP_punpckl(64));
    iproc_set(x86_push,         new X86::IP_push);
    iproc_set(x86_pusha,        new X86::IP_push_gprs);
    iproc_set(x86_pushad,       new X86::IP_push_gprs);
    iproc_set(x86_pushf,        new X86::IP_push_flags);
    iproc_set(x86_pushfd,       new X86::IP_push_flags);
    iproc_set(x86_pushfq,       new X86::IP_push_flags);
    iproc_set(x86_pxor,         new X86::IP_pxor);
    iproc_set(x86_rcl,          new X86::IP_rotate(x86_rcl));
    iproc_set(x86_rcr,          new X86::IP_rotate(x86_rcr));
    iproc_set(x86_rdtsc,        new X86::IP_rdtsc);
    iproc_set(x86_rep_lodsb,    new X86::IP_loadstring(x86_repeat_repe, 8));
    iproc_set(x86_rep_lodsw,    new X86::IP_loadstring(x86_repeat_repe, 16));
    iproc_set(x86_rep_lodsd,    new X86::IP_loadstring(x86_repeat_repe, 32));
    iproc_set(x86_rep_lodsq,    new X86::IP_loadstring(x86_repeat_repe, 64));
    iproc_set(x86_rep_movsb,    new X86::IP_movestring(x86_repeat_repe, 8));
    iproc_set(x86_rep_movsw,    new X86::IP_movestring(x86_repeat_repe, 16));
    iproc_set(x86_rep_movsd,    new X86::IP_movestring(x86_repeat_repe, 32));
    iproc_set(x86_rep_movsq,    new X86::IP_movestring(x86_repeat_repe, 64));
    iproc_set(x86_rep_stosb,    new X86::IP_storestring(x86_repeat_repe, 8));
    iproc_set(x86_rep_stosw,    new X86::IP_storestring(x86_repeat_repe, 16));
    iproc_set(x86_rep_stosd,    new X86::IP_storestring(x86_repeat_repe, 32));
    iproc_set(x86_rep_stosq,    new X86::IP_storestring(x86_repeat_repe, 64));
    iproc_set(x86_repe_cmpsb,   new X86::IP_cmpstrings(x86_repeat_repe, 8));
    iproc_set(x86_repe_cmpsw,   new X86::IP_cmpstrings(x86_repeat_repe, 16));
    iproc_set(x86_repe_cmpsd,   new X86::IP_cmpstrings(x86_repeat_repe, 32));
    iproc_set(x86_repe_cmpsq,   new X86::IP_cmpstrings(x86_repeat_repe, 64));
    iproc_set(x86_repe_scasb,   new X86::IP_scanstring(x86_repeat_repe, 8));
    iproc_set(x86_repe_scasw,   new X86::IP_scanstring(x86_repeat_repe, 16));
    iproc_set(x86_repe_scasd,   new X86::IP_scanstring(x86_repeat_repe, 32));
    iproc_set(x86_repe_scasq,   new X86::IP_scanstring(x86_repeat_repe, 64));
    iproc_set(x86_repne_cmpsb,  new X86::IP_cmpstrings(x86_repeat_repne, 8));
    iproc_set(x86_repne_cmpsw,  new X86::IP_cmpstrings(x86_repeat_repne, 16));
    iproc_set(x86_repne_cmpsd,  new X86::IP_cmpstrings(x86_repeat_repne, 32));
    iproc_set(x86_repne_cmpsq,  new X86::IP_cmpstrings(x86_repeat_repne, 64));
    iproc_set(x86_repne_scasb,  new X86::IP_scanstring(x86_repeat_repne, 8));
    iproc_set(x86_repne_scasw,  new X86::IP_scanstring(x86_repeat_repne, 16));
    iproc_set(x86_repne_scasd,  new X86::IP_scanstring(x86_repeat_repne, 32));
    iproc_set(x86_repne_scasq,  new X86::IP_scanstring(x86_repeat_repne, 64));
    iproc_set(x86_ret,          new X86::IP_ret);
    iproc_set(x86_rol,          new X86::IP_rotate(x86_rol));
    iproc_set(x86_ror,          new X86::IP_rotate(x86_ror));
    iproc_set(x86_sar,          new X86::IP_shift_1(x86_sar));
    iproc_set(x86_sbb,          new X86::IP_sbb);
    iproc_set(x86_scasb,        new X86::IP_scanstring(x86_repeat_none, 8));
    iproc_set(x86_scasw,        new X86::IP_scanstring(x86_repeat_none, 16));
    iproc_set(x86_scasd,        new X86::IP_scanstring(x86_repeat_none, 32));
    iproc_set(x86_scasq,        new X86::IP_scanstring(x86_repeat_none, 64));
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
    iproc_set(x86_sfence,       new X86::IP_nop);
    iproc_set(x86_shl,          new X86::IP_shift_1(x86_shl));
    iproc_set(x86_shld,         new X86::IP_shift_2(x86_shld));
    iproc_set(x86_shr,          new X86::IP_shift_1(x86_shr));
    iproc_set(x86_shrd,         new X86::IP_shift_2(x86_shrd));
    iproc_set(x86_stc,          new X86::IP_stc);
    iproc_set(x86_std,          new X86::IP_std);
    iproc_set(x86_stosb,        new X86::IP_storestring(x86_repeat_none, 8));
    iproc_set(x86_stosw,        new X86::IP_storestring(x86_repeat_none, 16));
    iproc_set(x86_stosd,        new X86::IP_storestring(x86_repeat_none, 32));
    iproc_set(x86_stosq,        new X86::IP_storestring(x86_repeat_none, 64));
    iproc_set(x86_stmxcsr,      new X86::IP_stmxcsr);
    iproc_set(x86_sub,          new X86::IP_sub);
    iproc_set(x86_syscall,      new X86::IP_syscall);
    iproc_set(x86_sysenter,     new X86::IP_sysenter);
    iproc_set(x86_test,         new X86::IP_test);
    iproc_set(x86_ud2,          new X86::IP_ud2);
    iproc_set(x86_wait,         new X86::IP_wait);
    iproc_set(x86_xadd,         new X86::IP_xadd);
    iproc_set(x86_xchg,         new X86::IP_xchg);
    iproc_set(x86_xor,          new X86::IP_xor);
    iproc_set(x86_xorpd,        new X86::IP_xor);
    iproc_set(x86_xorps,        new X86::IP_xor);
}

void
DispatcherX86::regcache_init()
{
    if (regdict) {
        switch (processorMode()) {
            case x86_insnsize_64:
                REG_RAX = findRegister("rax", 64);
                REG_RBX = findRegister("rbx", 64);
                REG_RCX = findRegister("rcx", 64);
                REG_RDX = findRegister("rdx", 64);
                REG_RDI = findRegister("rdi", 64);
                REG_RSI = findRegister("rsi", 64);
                REG_RSP = findRegister("rsp", 64);
                REG_RBP = findRegister("rbp", 64);
                REG_RIP = findRegister("rip", 64);
                REG_RFLAGS = findRegister("rflags", 64);
                REG_R8 = findRegister("r8", 64);
                REG_R9 = findRegister("r9", 64);
                REG_R10 = findRegister("r10", 64);
                REG_R11 = findRegister("r11", 64);
                REG_R12 = findRegister("r12", 64);
                REG_R13 = findRegister("r13", 64);
                REG_R14 = findRegister("r14", 64);
                REG_R15 = findRegister("r15", 64);
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
                REG_EIP = findRegister("eip", 32);
                REG_EFLAGS= findRegister("eflags", 32);
                REG_MXCSR = findRegister("mxcsr", 32);
                REG_FS = findRegister("fs", 16);
                REG_GS = findRegister("gs", 16);
                // fall through...
            case x86_insnsize_16:
                REG_AX = findRegister("ax", 16);
                REG_BX = findRegister("bx", 16);
                REG_CX = findRegister("cx", 16);
                REG_DX = findRegister("dx", 16);
                REG_DI = findRegister("di", 16);
                REG_SI = findRegister("si", 16);
                REG_SP = findRegister("sp", 16);
                REG_BP = findRegister("bp", 16);
                REG_IP = findRegister("ip", 16);
                REG_AL = findRegister("al", 8);
                REG_BL = findRegister("bl", 8);
                REG_CL = findRegister("cl", 8);
                REG_DL = findRegister("dl", 8);
                REG_AH = findRegister("ah", 8);
                REG_BH = findRegister("bh", 8);
                REG_CH = findRegister("ch", 8);
                REG_DH = findRegister("dh", 8);
                REG_FLAGS = findRegister("flags", 16);
                REG_AF = findRegister("af", 1);
                REG_CF = findRegister("cf", 1);
                REG_DF = findRegister("df", 1);
                REG_OF = findRegister("of", 1);
                REG_PF = findRegister("pf", 1);
                REG_SF = findRegister("sf", 1);
                REG_TF = findRegister("tf", 1);
                REG_ZF = findRegister("zf", 1);
                REG_CS = findRegister("cs", 16);
                REG_DS = findRegister("ds", 16);
                REG_ES = findRegister("es", 16);
                REG_SS = findRegister("ss", 16);

                // These next few are not in every 16-bit architecture
                REG_FPSTATUS = findRegister("fpstatus", 16, true);
                REG_FPSTATUS_TOP = findRegister("fpstatus_top", 3, true);
                REG_ST0 = findRegister("st0", 80, true);
                REG_FPCTL = findRegister("fpctl", 16, true);
                break;
            default:
                ASSERT_not_reachable("invalid instruction size");
        }

        size_t maxWidth = 0;
        switch (processorMode()) {
            case x86_insnsize_64: maxWidth = 64; break;
            case x86_insnsize_32: maxWidth = 32; break;
            case x86_insnsize_16: maxWidth = 16; break;
            default: ASSERT_not_reachable("invalid processor mode");
        }

        REG_anyAX = regdict->findLargestRegister(x86_regclass_gpr, x86_gpr_ax, maxWidth);
        REG_anyBX = regdict->findLargestRegister(x86_regclass_gpr, x86_gpr_bx, maxWidth);
        REG_anyCX = regdict->findLargestRegister(x86_regclass_gpr, x86_gpr_cx, maxWidth);
        REG_anyDX = regdict->findLargestRegister(x86_regclass_gpr, x86_gpr_dx, maxWidth);

        REG_anyDI = regdict->findLargestRegister(x86_regclass_gpr, x86_gpr_di, maxWidth);
        REG_anySI = regdict->findLargestRegister(x86_regclass_gpr, x86_gpr_si, maxWidth);

        REG_anyIP = regdict->findLargestRegister(x86_regclass_ip,  0,          maxWidth);
        REG_anySP = regdict->findLargestRegister(x86_regclass_gpr, x86_gpr_sp, maxWidth);
        REG_anyBP = regdict->findLargestRegister(x86_regclass_gpr, x86_gpr_bp, maxWidth);

        REG_anyFLAGS = regdict->findLargestRegister(x86_regclass_flags, x86_flags_status, maxWidth);
    }
}

void
DispatcherX86::memory_init() {
    if (BaseSemantics::StatePtr state = currentState()) {
        if (BaseSemantics::MemoryStatePtr memory = state->memoryState()) {
            switch (memory->get_byteOrder()) {
                case ByteOrder::ORDER_LSB:
                    break;
                case ByteOrder::ORDER_MSB:
                    mlog[WARN] <<"x86 memory state is using big-endian byte order\n";
                    break;
                case ByteOrder::ORDER_UNSPECIFIED:
                    memory->set_byteOrder(ByteOrder::ORDER_LSB);
                    break;
            }
        }
    }
}

RegisterDescriptor
DispatcherX86::instructionPointerRegister() const {
    return REG_anyIP;
}

RegisterDescriptor
DispatcherX86::stackPointerRegister() const {
    return REG_anySP;
}

RegisterDescriptor
DispatcherX86::callReturnRegister() const {
    return RegisterDescriptor();
}

static bool
isStatusRegister(RegisterDescriptor reg) {
    return reg.majorNumber()==x86_regclass_flags && reg.minorNumber()==x86_flags_status;
}

RegisterDictionary::RegisterDescriptors
DispatcherX86::get_usual_registers() const
{
    RegisterDictionary::RegisterDescriptors registers = regdict->get_largest_registers();
    registers.erase(std::remove_if(registers.begin(), registers.end(), isStatusRegister), registers.end());
    BOOST_FOREACH (RegisterDescriptor reg, regdict->get_smallest_registers()) {
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
    writeRegister(REG_PF, parity(operators->extract(result, 0, 8)));
    writeRegister(REG_SF, operators->extract(result, width-1, width));
    writeRegister(REG_ZF, operators->equalToZero(result));
}

void
DispatcherX86::setFlagsForResult(const BaseSemantics::SValuePtr &result, const BaseSemantics::SValuePtr &cond)
{
    ASSERT_require(cond->get_width()==1);
    BaseSemantics::SValuePtr lo_byte = operators->extract(result, 0, 8);
    BaseSemantics::SValuePtr signbit = operators->extract(result, result->get_width()-1, result->get_width());
    BaseSemantics::SValuePtr pf = readRegister(REG_PF, PEEK_REGISTER);
    writeRegister(REG_PF, operators->ite(cond, parity(lo_byte), pf));
    writeRegister(REG_SF, operators->ite(cond, signbit, readRegister(REG_SF, PEEK_REGISTER)));
    BaseSemantics::SValuePtr zf = readRegister(REG_ZF, PEEK_REGISTER);
    writeRegister(REG_ZF, operators->ite(cond, operators->equalToZero(result), zf));
}

BaseSemantics::SValuePtr
DispatcherX86::parity(const BaseSemantics::SValuePtr &v)
{
    ASSERT_require(v->get_width()==8);
    BaseSemantics::SValuePtr p1 = operators->extract(v, 1, 2);
    BaseSemantics::SValuePtr p01 = operators->xor_(operators->extract(v, 0, 1), p1);
    BaseSemantics::SValuePtr p3 = operators->extract(v, 3, 4);
    BaseSemantics::SValuePtr p23 = operators->xor_(operators->extract(v, 2, 3), p3);
    BaseSemantics::SValuePtr p5 = operators->extract(v, 5, 6);
    BaseSemantics::SValuePtr p45 = operators->xor_(operators->extract(v, 4, 5), p5);
    BaseSemantics::SValuePtr p7 = operators->extract(v, 7, 8);
    BaseSemantics::SValuePtr p67 = operators->xor_(operators->extract(v, 6, 7), p7);
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
    BaseSemantics::SValuePtr no = operators->boolean_(false);
    operators->addWithCarries(w, number_(nbits, 6), no, carries/*out*/);
    return operators->extract(carries, nbits-1, nbits);
}

BaseSemantics::SValuePtr
DispatcherX86::flagsCombo(X86InstructionKind k)
{
    switch (k) {
        case x86_jne:
        case x86_setne:
        case x86_cmovne:
            return operators->invert(readRegister(REG_ZF));
        case x86_je:
        case x86_sete:
        case x86_cmove:
            return readRegister(REG_ZF);
        case x86_jno:
        case x86_setno:
        case x86_cmovno:
            return operators->invert(readRegister(REG_OF));
        case x86_jo:
        case x86_seto:
        case x86_cmovo:
            return readRegister(REG_OF);
        case x86_jns:
        case x86_setns:
        case x86_cmovns:
            return operators->invert(readRegister(REG_SF));
        case x86_js:
        case x86_sets:
        case x86_cmovs:
            return readRegister(REG_SF);
        case x86_jpo:
        case x86_setpo:
        case x86_cmovpo:
            return operators->invert(readRegister(REG_PF));
        case x86_jpe:
        case x86_setpe:
        case x86_cmovpe:
            return readRegister(REG_PF);
        case x86_jae:
        case x86_setae:
        case x86_cmovae:
            return operators->invert(readRegister(REG_CF));
        case x86_jb:
        case x86_setb:
        case x86_cmovb:
            return readRegister(REG_CF);
        case x86_jbe:
        case x86_setbe:
        case x86_cmovbe: {
            BaseSemantics::SValuePtr zf = readRegister(REG_ZF);
            return operators->or_(readRegister(REG_CF), zf);
        }
        case x86_ja:
        case x86_seta:
        case x86_cmova: {
            BaseSemantics::SValuePtr notZf = operators->invert(readRegister(REG_ZF));
            return operators->and_(operators->invert(readRegister(REG_CF)), notZf);
        }
        case x86_jl:
        case x86_setl:
        case x86_cmovl: {
            BaseSemantics::SValuePtr of = readRegister(REG_OF);
            return operators->xor_(readRegister(REG_SF), of);
        }
        case x86_jge:
        case x86_setge:
        case x86_cmovge: {
            BaseSemantics::SValuePtr of = readRegister(REG_OF);
            return operators->invert(operators->xor_(readRegister(REG_SF), of));
        }
        case x86_jle:
        case x86_setle:
        case x86_cmovle: {
            BaseSemantics::SValuePtr of = readRegister(REG_OF);
            BaseSemantics::SValuePtr sf_xor_of = operators->xor_(readRegister(REG_SF), of);
            return operators->or_(readRegister(REG_ZF), sf_xor_of);
        }
        case x86_jg:
        case x86_setg:
        case x86_cmovg: {
            BaseSemantics::SValuePtr notZf = operators->invert(readRegister(REG_ZF));
            BaseSemantics::SValuePtr of = readRegister(REG_OF);
            return operators->and_(operators->invert(operators->xor_(readRegister(REG_SF), of)), notZf);
        }
        case x86_jcxz:
            return operators->equalToZero(readRegister(REG_CX));
        case x86_jecxz:
            return operators->equalToZero(readRegister(REG_ECX));
        default:
            ASSERT_not_reachable("instruction kind not handled");
        }
}

BaseSemantics::SValuePtr
DispatcherX86::repEnter(X86RepeatPrefix repeat)
{
    if (repeat==x86_repeat_none)
        return operators->boolean_(true);
    BaseSemantics::SValuePtr cx = readRegister(REG_anyCX);
    BaseSemantics::SValuePtr in_loop = operators->invert(operators->equalToZero(cx));
    return in_loop;
}

void
DispatcherX86::repLeave(X86RepeatPrefix repeat_prefix, const BaseSemantics::SValuePtr &in_loop, rose_addr_t insn_va,
                        bool honorZeroFlag)
{
    ASSERT_require(in_loop!=NULL && in_loop->get_width()==1);

    // conditionally decrement the CX register
    BaseSemantics::SValuePtr zero = operators->number_(REG_anyCX.nBits(),  0);
    BaseSemantics::SValuePtr maybeDecrement = operators->ite(in_loop, operators->number_(REG_anyCX.nBits(), -1), zero);
    BaseSemantics::SValuePtr new_cx = operators->add(readRegister(REG_anyCX), maybeDecrement);
    writeRegister(REG_anyCX, new_cx);
    BaseSemantics::SValuePtr nonzero_cx = operators->invert(operators->equalToZero(new_cx));

    // determine whether we should repeat the instruction.
    BaseSemantics::SValuePtr again;
    switch (repeat_prefix) {
        case x86_repeat_none:
            again = operators->boolean_(false);
            break;
        case x86_repeat_repe:
            // REPE is an alias for REP when used with certain instructions.
            if (honorZeroFlag) {
                BaseSemantics::SValuePtr zf = readRegister(REG_ZF);
                again = operators->and_(operators->and_(in_loop, nonzero_cx), zf);
            } else {
                again = operators->and_(in_loop, nonzero_cx);
            }
            break;
        case x86_repeat_repne: {
            BaseSemantics::SValuePtr notZf = operators->invert(readRegister(REG_ZF));
            again = operators->and_(operators->and_(in_loop, nonzero_cx), notZf);
            break;
        }
    }

    BaseSemantics::SValuePtr ip = readRegister(REG_anyIP);
    writeRegister(REG_anyIP,
                  operators->ite(again,
                                 operators->number_(REG_anyIP.nBits(), insn_va),    // repeat
                                 ip)); // exit loop
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
    writeRegister(REG_AF, invertMaybe(operators->extract(carries, 3, 4), invertCarries));
    writeRegister(REG_CF, invertMaybe(sign, invertCarries));
    writeRegister(REG_OF, operators->xor_(sign, ofbit));
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
    BaseSemantics::SValuePtr af = readRegister(REG_AF);
    writeRegister(REG_AF,
                  operators->ite(cond,
                                 invertMaybe(operators->extract(carries, 3, 4), invertCarries),
                                 af));
    BaseSemantics::SValuePtr cf = readRegister(REG_CF);
    writeRegister(REG_CF,
                  operators->ite(cond,
                                 invertMaybe(sign, invertCarries),
                                 cf));
    BaseSemantics::SValuePtr of = readRegister(REG_OF);
    writeRegister(REG_OF,
                  operators->ite(cond,
                                 operators->xor_(sign, ofbit),
                                 of));
    return result;
}
    


/* Does increment (decrement with DEC set), and adjusts condition flags. */
BaseSemantics::SValuePtr
DispatcherX86::doIncOperation(const BaseSemantics::SValuePtr &a, bool dec, bool setCarry)
{
    size_t nbits = a->get_width();
    BaseSemantics::SValuePtr carries;
    BaseSemantics::SValuePtr no = operators->boolean_(false);
    BaseSemantics::SValuePtr result = operators->addWithCarries(a, number_(nbits, dec?-1:1), no, carries/*out*/);
    setFlagsForResult(result);
    BaseSemantics::SValuePtr sign = operators->extract(carries, nbits-1, nbits);
    BaseSemantics::SValuePtr ofbit = operators->extract(carries, nbits-2, nbits-1);
    writeRegister(REG_AF, invertMaybe(operators->extract(carries, 3, 4), dec));
    writeRegister(REG_OF, operators->xor_(sign, ofbit));
    if (setCarry)
        writeRegister(REG_CF, invertMaybe(sign, dec));
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
        case x86_ror: {
            BaseSemantics::SValuePtr v1 = operators->extract(result, result->get_width()-2, result->get_width()-1);
            new_of = operators->xor_(operators->extract(result, result->get_width()-1, result->get_width()), v1);
            break;
        }
        case x86_rcr: {
            BaseSemantics::SValuePtr v1 = operators->extract(operand, operand->get_width()-2, operand->get_width()-1);
            new_of = operators->xor_(operators->extract(operand, operand->get_width()-1, operand->get_width()), v1);
            break;
        }
        case x86_rol:
            new_of = operators->xor_(new_cf,
                                     operators->extract(result, result->get_width()-1, result->get_width()));
            break;
        default:
            ASSERT_not_reachable("instruction not handled");
    }

    // Update CF and OF flags. SF, ZF, AF, and PF are not affected.
    writeRegister(REG_CF, new_cf);
    BaseSemantics::SValuePtr maybeOf = operators->ite(isOneBitRotate, new_of, unspecified_(1));
    writeRegister(REG_OF, operators->ite(isZeroRotateCount,
                                         readRegister(REG_OF, PEEK_REGISTER),
                                         maybeOf));

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
        case x86_shrd: {
            BaseSemantics::SValuePtr v1 = operators->shiftLeft(source_bits, operators->negate(maskedShiftCount));
            BaseSemantics::SValuePtr v2 = operators->ite(isZeroShiftCount,
                                                         number_(operand->get_width(), 0),
                                                         v1);
            BaseSemantics::SValuePtr v3 = operators->or_(operators->shiftRight(operand, maskedShiftCount), v2);
            result = operators->ite(isLargeShift, unspecified_(operand->get_width()), v3);
            break;
        }
        case x86_shld: {
            BaseSemantics::SValuePtr v1 = operators->shiftRight(source_bits, operators->negate(maskedShiftCount));
            BaseSemantics::SValuePtr v2 = operators->ite(isZeroShiftCount,
                                                         number_(operand->get_width(), 0),
                                                         v1);
            BaseSemantics::SValuePtr v3 = operators->or_(operators->shiftLeft(operand, maskedShiftCount), v2);
            result = operators->ite(isLargeShift, unspecified_(operand->get_width()), v3);
            break;
        }
        default:
            ASSERT_not_reachable("instruction not handled");
    }
    ASSERT_require(operand->get_width()==result->get_width());
    BaseSemantics::SValuePtr originalSign = operators->extract(operand, operand->get_width()-1, operand->get_width());
    BaseSemantics::SValuePtr resultSign = operators->extract(result, result->get_width()-1, result->get_width());

    // The AF flag is undefined if a shift occurs.  The documentation for SHL, SHR, and SAR are somewhat ambiguous about
    // this, but the documentation for SHLD and SHRD is more specific.  We assume that both sets of shift instructions
    // behave the same way.
    BaseSemantics::SValuePtr unspecAf = unspecified_(1);
    writeRegister(REG_AF,
                  operators->ite(isZeroShiftCount,
                                 readRegister(REG_AF, PEEK_REGISTER),
                                 unspecAf));

    // What is the last bit shifted off the operand?  If we're right shifting by N bits, then the original operand N-1 bit
    // is what should make it into the final CF; if we're left shifting by N bits then we need bit operand->get_width()-N.
    BaseSemantics::SValuePtr bitPosition;
    if (x86_shr==kind || x86_sar==kind || x86_shrd==kind) {
        bitPosition = operators->add(maskedShiftCount, mask);
    } else {
        BaseSemantics::SValuePtr one = number_(shiftSignificantBits, 1);
        BaseSemantics::SValuePtr term1 = operators->add(operators->invert(maskedShiftCount), one);
        bitPosition = operators->add(number_(shiftSignificantBits, operand->get_width() & m), // probably zero modulo
                                     term1);
    }
    BaseSemantics::SValuePtr shifted_off = operators->extract(operators->shiftRight(operand, bitPosition), 0, 1);

    // New carry flag value.  From the Intel manual, the CF flag is "undefined for SHL and SHR [and SAL] instructions where
    // the count is greater than or equal to the size (in bits) of the destination operand", and "if the count is 0, the
    // flags are not affected."  The manual is silent about the value of CF for large SAR shifts, so we use the original
    // sign bit, matching the pseudo-code in the manual.
    BaseSemantics::SValuePtr newCFite = operators->ite(isLargeShift,
                                                       (x86_sar==kind ? originalSign : unspecified_(1)),
                                                       shifted_off);
    BaseSemantics::SValuePtr newCF = operators->ite(isZeroShiftCount, readRegister(REG_CF, PEEK_REGISTER), newCFite);
    writeRegister(REG_CF, newCF);

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
        case x86_shr: {
            BaseSemantics::SValuePtr unspec = unspecified_(1);
            newOF = operators->ite(isOneBitShift,
                                   originalSign,
                                   operators->ite(isZeroShiftCount, 
                                                  readRegister(REG_OF, PEEK_REGISTER),
                                                  unspec));
            break;
        }
        case x86_sar: {
            BaseSemantics::SValuePtr unspec = unspecified_(1);
            BaseSemantics::SValuePtr v1 = operators->ite(isZeroShiftCount, readRegister(REG_OF, PEEK_REGISTER), unspec);
            newOF = operators->ite(isOneBitShift, operators->boolean_(false), v1);
            break;
        }
        case x86_shl:
        case x86_shld:
        case x86_shrd: {
            BaseSemantics::SValuePtr unspec = unspecified_(1);
            BaseSemantics::SValuePtr v1 = operators->ite(isZeroShiftCount, readRegister(REG_OF, PEEK_REGISTER), unspec);
            newOF = operators->ite(isOneBitShift, operators->xor_(originalSign, resultSign), v1);
            break;
        }
        default:
            ASSERT_not_reachable("instruction not handled");
    }
    writeRegister(REG_OF, newOF);

    // Result flags SF, ZF, and PF are set according to the result, but are unchanged if the shift count is zero.
    setFlagsForResult(result, operators->invert(isZeroShiftCount));
    return result;
}

BaseSemantics::SValuePtr
DispatcherX86::readRegister(RegisterDescriptor reg, AccessMode mode) {
    // When reading FLAGS, EFLAGS as a whole do not coalesce individual flags into the single register.
    if (reg.majorNumber()==x86_regclass_flags && reg.offset()==0 && reg.nBits()>1) {
        if (BaseSemantics::StatePtr ss = operators->currentState()) {
            BaseSemantics::RegisterStatePtr rs = ss->registerState();
            if (BaseSemantics::RegisterStateGeneric *rsg = dynamic_cast<BaseSemantics::RegisterStateGeneric*>(rs.get())) {
                BaseSemantics::RegisterStateGeneric::AccessModifiesExistingLocationsGuard guard(rsg, false);
                switch (mode) {
                    case READ_REGISTER:
                        return operators->readRegister(reg);
                    case PEEK_REGISTER:
                        return operators->peekRegister(reg, operators->undefined_(reg.nBits()));
                }
            }
        }
    }
    switch (mode) {
        case READ_REGISTER:
            return operators->readRegister(reg);
        case PEEK_REGISTER:
            return operators->peekRegister(reg, operators->undefined_(reg.nBits()));
    }
    ASSERT_not_reachable("unhandled access mode");
}

void
DispatcherX86::writeRegister(RegisterDescriptor reg, const BaseSemantics::SValuePtr &value) {
    if (reg.nBits() == 32 && reg.offset() == 0 && REG_anyIP.nBits() == 64) {
        // Writing to a 32-bit GPR in x86-64 will also clear the upper 32 bits of the same 64-bit register. E.g., "MOV EDI,
        // EDI" is one way to clear the upper 32-bits in RDI.
        if (reg.majorNumber() == x86_regclass_gpr || reg.majorNumber() == x86_regclass_ip) {
            RegisterDescriptor upperHalf(reg.majorNumber(), reg.minorNumber(), 32, 32);
            operators->writeRegister(upperHalf, operators->number_(32, 0));
        }
    } else if (reg.majorNumber()==x86_regclass_st && reg.minorNumber()<8 && reg.offset()==0 && reg.nBits()==64) {
        // When writing to an MM register, the high-order 16 bits are set in order to make the underlying
        // ST register NaN.
        RegisterDescriptor wider = reg;
        wider.nBits(80);
        operators->writeRegister(wider, operators->concat(value, number_(16, 0xffff)));
        return;
    }
    operators->writeRegister(reg, value);
}

void
DispatcherX86::write(SgAsmExpression *e, const BaseSemantics::SValuePtr &value, size_t addr_nbits/*=0*/)
{
    if (SgAsmDirectRegisterExpression *re = isSgAsmDirectRegisterExpression(e)) {
        writeRegister(re->get_descriptor(), value);
    } else {
        Dispatcher::write(e, value, addr_nbits);        // defer to super class
    }
}

void
DispatcherX86::pushFloatingPoint(const BaseSemantics::SValuePtr &value)
{
    BaseSemantics::SValuePtr topOfStack = readRegister(REG_FPSTATUS_TOP);
    if (!topOfStack->is_number())
        throw BaseSemantics::Exception("FP-stack top is not concrete", NULL);
    BaseSemantics::SValuePtr newTopOfStack = operators->add(topOfStack, operators->number_(topOfStack->get_width(), -1));
    ASSERT_require2(newTopOfStack->is_number(), "constant folding is required for FP-stack");

    RegisterDescriptor reg(REG_ST0.majorNumber(), (REG_ST0.minorNumber() + newTopOfStack->get_number()) % 8,
                           REG_ST0.offset(), REG_ST0.nBits());
    writeRegister(reg, value);
    writeRegister(REG_FPSTATUS_TOP, newTopOfStack);
}

BaseSemantics::SValuePtr
DispatcherX86::readFloatingPointStack(size_t position)
{
    BaseSemantics::SValuePtr topOfStack = readRegister(REG_FPSTATUS_TOP);
    if (!topOfStack->is_number())
        throw BaseSemantics::Exception("FP-stack top is not concrete", NULL);
    RegisterDescriptor reg(REG_ST0.majorNumber(), (REG_ST0.minorNumber() + topOfStack->get_number()) % 8,
                           REG_ST0.offset(), REG_ST0.nBits());
    return readRegister(reg);
}

void
DispatcherX86::popFloatingPoint()
{
    BaseSemantics::SValuePtr topOfStack = readRegister(REG_FPSTATUS_TOP);
    if (!topOfStack->is_number())
        throw BaseSemantics::Exception("FP-stack top is not concrete", NULL);
    BaseSemantics::SValuePtr newTopOfStack = operators->add(topOfStack, operators->number_(topOfStack->get_width(), 1));
    ASSERT_require2(newTopOfStack->is_number(), "constant folding is required for FP-stack");
    writeRegister(REG_FPSTATUS_TOP, newTopOfStack);
}

BaseSemantics::SValuePtr
DispatcherX86::fixMemoryAddress(const BaseSemantics::SValuePtr &addr) const
{
    if (size_t addrWidth = addressWidth()) {
        if (addr->get_width() < addrWidth)
            return operators->signExtend(addr, addrWidth);
        if (addr->get_width() > addrWidth)
            return operators->unsignedExtend(addr, addrWidth);
    }
    return addr;
}

BaseSemantics::SValuePtr
DispatcherX86::saturateSignedToUnsigned(const BaseSemantics::SValuePtr &src, size_t nBits) {
    ASSERT_not_null(src);
    ASSERT_require(src->get_width() >= nBits);
    if (src->get_width() == nBits)
        return src;
    BaseSemantics::SValuePtr signBit = operators->extract(src, src->get_width()-1, src->get_width());
    BaseSemantics::SValuePtr high = operators->extract(src, nBits, src->get_width());
    BaseSemantics::SValuePtr noOverflow = operators->equalToZero(high);
    BaseSemantics::SValuePtr v1 = operators->signExtend(signBit, nBits);
    return operators->ite(noOverflow, operators->extract(src, 0, nBits), v1);
}

BaseSemantics::SValuePtr
DispatcherX86::saturateSignedToSigned(const BaseSemantics::SValuePtr &src, size_t nBits) {
    ASSERT_not_null(src);
    ASSERT_require(src->get_width() >= nBits);
    if (src->get_width() == nBits)
        return src;
    BaseSemantics::SValuePtr signBit = operators->extract(src, src->get_width()-1, src->get_width());
    BaseSemantics::SValuePtr high = operators->extract(src, nBits-1, src->get_width());
    BaseSemantics::SValuePtr zero = operators->number_(high->get_width(), 0);
    BaseSemantics::SValuePtr allSet = operators->invert(zero);
    BaseSemantics::SValuePtr highZerop = operators->isEqual(high, allSet);
    BaseSemantics::SValuePtr noOverflow = operators->or_(operators->equalToZero(high), highZerop);
    BaseSemantics::SValuePtr yes = operators->boolean_(true);
    BaseSemantics::SValuePtr minResult = operators->concat(operators->number_(nBits-1, 0), yes);
    BaseSemantics::SValuePtr maxResult = operators->invert(minResult);
    BaseSemantics::SValuePtr minOrMax = operators->ite(signBit, minResult, maxResult);
    return operators->ite(noOverflow, operators->extract(src, 0, nBits), minOrMax);
}

BaseSemantics::SValuePtr
DispatcherX86::saturateUnsignedToUnsigned(const BaseSemantics::SValuePtr &src, size_t nBits) {
    ASSERT_not_null(src);
    ASSERT_require(src->get_width() >= nBits);
    if (src->get_width() == nBits)
        return src;
    BaseSemantics::SValuePtr high = operators->extract(src, nBits, src->get_width());
    BaseSemantics::SValuePtr noOverflow = operators->equalToZero(high);
    BaseSemantics::SValuePtr allBits = operators->invert(operators->number_(nBits, 0));
    return operators->ite(noOverflow, operators->extract(src, 0, nBits), allBits);
}

} // namespace
} // namespace
} // namespace

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics2::DispatcherX86);
#endif
