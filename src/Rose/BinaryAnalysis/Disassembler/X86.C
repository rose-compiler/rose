#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Disassembler/X86.h>

#include <Rose/AST/Traversal.h>
#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>

#include <SgAsmBinaryAdd.h>
#include <SgAsmBinaryMultiply.h>
#include <SgAsmDirectRegisterExpression.h>
#include <SgAsmFloatType.h>
#include <SgAsmIndirectRegisterExpression.h>
#include <SgAsmIntegerType.h>
#include <SgAsmIntegerValueExpression.h>
#include <SgAsmMemoryReferenceExpression.h>
#include <SgAsmOperandList.h>
#include <SgAsmVectorType.h>
#include <SgAsmX86Instruction.h>

#include <Cxx_GrammarDowncast.h>
#include <Assembler.h>
#include <AssemblerX86.h>
#include <SageBuilderAsm.h>
#include <integerOps.h>
#include <stringify.h>
#include <sstream>

namespace Rose {
namespace BinaryAnalysis {
namespace Disassembler {

/* See header file for full documentation. */

// These are macros to make them look like constants while they are really function calls
#define BYTET (SageBuilderAsm::buildTypeX86Byte())
#define WORDT (SageBuilderAsm::buildTypeX86Word())
#define DWORDT (SageBuilderAsm::buildTypeX86DoubleWord())
#define QWORDT (SageBuilderAsm::buildTypeX86QuadWord())
#define DQWORDT (SageBuilderAsm::buildTypeX86DoubleQuadWord())
#define FLOATT (SageBuilderAsm::buildTypeX86Float32())
#define DOUBLET (SageBuilderAsm::buildTypeX86Float64())
#define LDOUBLET (SageBuilderAsm::buildTypeX86Float80())
#define V8BYTET (SageBuilderAsm::buildTypeVector(8, BYTET))
#define V16BYTET (SageBuilderAsm::buildTypeVector(16, BYTET))
#define V4WORDT (SageBuilderAsm::buildTypeVector(4, WORDT))
#define V8WORDT (SageBuilderAsm::buildTypeVector(8, WORDT))
#define V2DWORDT (SageBuilderAsm::buildTypeVector(2, DWORDT))
#define V4DWORDT (SageBuilderAsm::buildTypeVector(4, DWORDT))
#define V2FLOATT (SageBuilderAsm::buildTypeVector(2, FLOATT))
#define V4FLOATT (SageBuilderAsm::buildTypeVector(4, FLOATT))
#define V2QWORDT (SageBuilderAsm::buildTypeVector(2, QWORDT))
#define V2DOUBLET (SageBuilderAsm::buildTypeVector(2, DOUBLET))

/*========================================================================================================================
 * X86 primary methods, mostly defined by the superclass.
 *========================================================================================================================*/

X86::X86(const Architecture::Base::ConstPtr &arch)
    : Base(arch), insnSize(x86_insnsize_none), wordSize(arch->bytesPerWord()) {
    init(arch->bytesPerWord());
}

X86::Ptr
X86::instance(const Architecture::Base::ConstPtr &arch) {
    return Ptr(new X86(arch));
}

Base::Ptr
X86::clone() const {
    return Ptr(new X86(*this));
}

void
X86::init(size_t wordsize)
{
    /* The default register dictionary.  If a register dictionary is specified in an SgAsmInterpretation, then that one will be
     * used instead of the default we set here. */
    switch (wordsize) {
        case 2:
            insnSize = x86_insnsize_16;
            // A word size of 2 bytes doesn't necessarily mean 80286. E.g., $ROSE/binaries/samples/exefmt.exe has a header that
            // advertises architecture ISA_IA32_Family with a word size of 2 and which contains an occasional 32-bit floating
            // point instruction, although perhaps because of disassembling data with a disassembler that understands 32-bit op
            // codes.
            break;
        case 4:
            insnSize = x86_insnsize_32;
            break;
        case 8:
            insnSize = x86_insnsize_64;
            break;
        default:
            ASSERT_not_reachable("instruction must be 2, 4, or 8 bytes");
    }

    REG_IP = architecture()->registerDictionary()->instructionPointerRegister();
    REG_SP = architecture()->registerDictionary()->stackPointerRegister();
    REG_SS = architecture()->registerDictionary()->stackSegmentRegister();
    REG_SF = architecture()->registerDictionary()->stackFrameRegister();
    ASSERT_require(REG_IP);
    ASSERT_require(REG_SP);
    ASSERT_require(REG_SS);
    ASSERT_require(REG_SF);
}

void
X86::commentIpRelative(SgAsmInstruction *insn) {
    ASSERT_not_null(insn);
    const Address fallThroughVa = insn->get_address() + insn->get_size();
    const size_t nBits = architecture()->bitsPerWord();
    AST::Traversal::forwardPre<SgAsmBinaryAdd>(insn, [&fallThroughVa, &nBits](SgAsmBinaryAdd *add) {
        SgAsmDirectRegisterExpression *reg = nullptr;
        SgAsmIntegerValueExpression *ival = nullptr;
        if ((reg = isSgAsmDirectRegisterExpression(add->get_lhs()))) {
            ival = isSgAsmIntegerValueExpression(add->get_rhs());
        } else if ((reg = isSgAsmDirectRegisterExpression(add->get_rhs()))) {
            ival = isSgAsmIntegerValueExpression(add->get_lhs());
        }

        if (reg && ival && reg->get_descriptor().majorNumber() == x86_regclass_ip &&
            reg->get_descriptor().minorNumber() == 0 && reg->get_descriptor().offset() == 0 &&
            reg->get_descriptor().nBits() == nBits) {
            const Address offset = ival->get_absoluteValue();
            const Address va = (fallThroughVa + offset) & BitOps::lowMask<Address>(nBits);
            const std::string vaStr = "absolute=" + StringUtility::addrToString(va, nBits);
            std::string comment = add->get_comment();
            comment = comment.empty() ? vaStr : vaStr + "," + comment;
            add->set_comment(comment);
        }
    });
}

SgAsmInstruction *
X86::disassembleOne(const MemoryMap::Ptr &map, Address start_va, AddressSet *successors)
{
    /* The low-level disassembly function don't understand MemoryMap mappings. Therefore, remap the next few bytes (enough
     * for at least one instruction) into a temporary buffer. The longest x86 instruction is 15 bytes in 16-bit mode and 13
     * bytes in 32-bit mode:
     *
     * [16-bit]: 66 67 F0 3E 81 04 4E 01234567 89ABCDEF: add [ds:esi+ecx*2+0x67452301], 0xEFCDAB89
     * [32-bit]:       F0 3E 81 04 4E 01234567 89ABCDEF: add [ds:esi+ecx*2+0x67452301], 0xEFCDAB89
     *
     * In theory, by adding all appropriate prefix bytes you can obtain an instruction that is up to 16 bytes long. However,
     * the x86 CPU will generate an exception if the instruction length exceeds 15 bytes, and so will the getByte method. */
    if (start_va % instructionAlignment_ != 0)
        throw Exception("instruction pointer not aligned", start_va);
    unsigned char temp[16];
    size_t tempsz = map->at(start_va).limit(sizeof temp).require(MemoryMap::EXECUTABLE).read(temp).size();
    if (0 == tempsz)
        throw Exception("short read", start_va);

    /* Disassemble the instruction */
    State state;
    startInstruction(state, start_va, temp, tempsz);
    SgAsmInstruction *insn = NULL;
    try {
        insn = disassemble(state); /*throws an exception on error*/
    } catch (const ExceptionX86 &e) {
        insn = makeUnknownInstruction(e);
    }
    ASSERT_not_null(insn);

    // An addressing mode of FP + (R * C1) + C2 (where FP is the frame pointer register, R is some other register and C1 and C2
    // are constants) should be rewritten to FP + C2 + (R * C1). This makes it a bit more clear that we're probably starting
    // with the frame pointer, adding or subtracting something to get to the beginning of an array or structure, and then
    // indexing into that region of memory. Similarly for using the stack pointer as the base.
    for (size_t i = 0; i < insn->nOperands(); ++i) {
        if (auto mre = isSgAsmMemoryReferenceExpression(insn->operand(i))) {
            if (auto add1 = isSgAsmBinaryAdd(mre->get_address())) {
                if (auto add2 = isSgAsmBinaryAdd(add1->get_lhs())) {
                    if (auto fp = isSgAsmDirectRegisterExpression(add2->get_lhs())) {
                        if (auto mul = isSgAsmBinaryMultiply(add2->get_rhs())) {
                            if (isSgAsmDirectRegisterExpression(mul->get_lhs()) && isSgAsmIntegerValueExpression(mul->get_rhs())) {
                                if (auto c2 = isSgAsmIntegerValueExpression(add1->get_rhs())) {
                                    const RegisterDescriptor REG_FP =
                                        architecture()->registerDictionary()->findLargestRegister(x86_regclass_gpr, x86_gpr_bp);
                                    ASSERT_require(REG_FP);
                                    ASSERT_require(REG_SP);
                                    if (fp->get_descriptor() == REG_FP || fp->get_descriptor() == REG_SP) {
                                        // Pattern found. Swap some arguments
                                        add2->set_rhs(nullptr);
                                        add1->set_rhs(nullptr);

                                        add1->set_rhs(mul);
                                        mul->set_parent(add1);

                                        add2->set_rhs(c2);
                                        c2->set_parent(add2);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    /* Note successors if necesssary */
    if (successors) {
        bool complete;
        *successors |= architecture()->getSuccessors(insn, complete/*out*/);
    }

    commentIpRelative(insn);
    return insn;
}

SgAsmInstruction *
X86::makeUnknownInstruction(const Exception &e)
{
    // We don't need any of the state except the opcodes, and since this is a variable width instruction set architecture
    // whose smallest instruction in a single byte, the unknown instructions are always single bytes.
    State state;
    state.ip = e.ip;
    ASSERT_require(e.bytes.size() >= 1);
    state.insnbuf.push_back(e.bytes[0]);
    state.insnbufat = 1;
    SgAsmX86Instruction *insn = makeInstruction(state, x86_unknown_instruction);
    return insn;
}

/*========================================================================================================================
 * Methods for reading bytes of the instruction.  These keep track of how much has been read, which in turn is used by
 * the makeInstruction method.
 *========================================================================================================================*/

uint8_t
X86::getByte(State &state) const
{
    if (state.insnbufat>=15)
        throw ExceptionX86("instruction longer than 15 bytes", state);
    if (state.insnbufat>=state.insnbuf.size())
        throw ExceptionX86("short read", state);
    return state.insnbuf[state.insnbufat++];
}

uint16_t
X86::getWord(State &state) const
{
    uint16_t lo = getByte(state);
    uint16_t hi = getByte(state);
    return (hi<<8) | lo;
}

uint32_t
X86::getDWord(State &state) const
{
    uint32_t lo = getWord(state);
    uint32_t hi = getWord(state);
    return (hi<<16) | lo;
}

uint64_t
X86::getQWord(State &state) const
{
    uint64_t lo = getDWord(state);
    uint64_t hi = getDWord(state);
    return (hi<<32) | lo;
}




/*========================================================================================================================
 * Miscellaneous helper methods
 *========================================================================================================================*/

SgAsmExpression *
X86::currentDataSegment(State &state) const {
    if (state.segOverride != x86_segreg_none)
        return makeSegmentRegister(state, state.segOverride, insnSize==x86_insnsize_64);
    return makeSegmentRegister(state, x86_segreg_ds, insnSize==x86_insnsize_64);
}

X86InstructionSize
X86::effectiveAddressSize(State &state) const
{
    if (state.addressSizeOverride) {
        switch (insnSize) {
            case x86_insnsize_16: return x86_insnsize_32;
            case x86_insnsize_32: return x86_insnsize_16;
            case x86_insnsize_64: return x86_insnsize_32;
            default: {
                ASSERT_not_reachable("not a valid effective address size: " +
                                     stringifyBinaryAnalysisX86InstructionSize(insnSize));
                /* avoid MSCV warning by adding return stmt even though it cannot be reached */
                return insnSize;
            }
        }
    } else {
        return insnSize;
    }
}

X86InstructionSize
X86::effectiveOperandSize(State &state) const
{
    if (state.operandSizeOverride) {
        switch (insnSize) {
            case x86_insnsize_16:
                return x86_insnsize_32;
            case x86_insnsize_32:
                return x86_insnsize_16;
            case x86_insnsize_64: {
                /* Some instructions (e.g., MOVD/MOVQ) have both the operandSizeOverride prefix (0x66) along with the REX.W
                 * bit. These instructions are inherently 32-bit and they use the REX.W bit to specify 64-bit operands. Also,
                 * for instructions that don't have 0x66 as part of the opcode but which are inherently 32-bit and use the
                 * REX.W bit to select 64-bit operands, the CPU seems to ignore extraneous 0x66 prefixes when REX.W is set. */
                if (state.rexPresent && state.rexW)
                    return x86_insnsize_64;
                return x86_insnsize_16;
            }
            default: {
                ASSERT_not_reachable("not a valid effective operand size: " +
                                     stringifyBinaryAnalysisX86InstructionSize(insnSize));
                /* avoid MSCV warning by adding return stmt */
                return insnSize;
            }
        }
    } else {
        X86InstructionSize s = insnSize;
        if (insnSize == x86_insnsize_64 && !state.rexW && !state.sizeMustBe64Bit)
            s = x86_insnsize_32;
        return s;
        /* FIXME: This doesn't handle all of the special cases */
    }
}

X86::MMPrefix
X86::mmPrefix(State &state) const
{
    switch (state.repeatPrefix) {
        case x86_repeat_none: {
            if (state.operandSizeOverride) {
                return mm66;
            } else {
                return mmNone;
            }
        }
        case x86_repeat_repne: {
            if (state.operandSizeOverride) {
                throw ExceptionX86("bad combination of repeat prefix and operand size override", state);
            } else {
                return mmF2;
            }
        }
        case x86_repeat_repe: {
            if (state.operandSizeOverride) {
                throw ExceptionX86("bad combination of repeat prefix and operand size override", state);
            } else {
                return mmF3;
            }
        }
        default: {
            ASSERT_not_reachable("not a valid repeat prefix: " +
                                 stringifyBinaryAnalysisX86RepeatPrefix(state.repeatPrefix));
            /* avoid MSCV warning by adding return stmt */
            return mmNone;
        }
    }
}

void
X86::setRex(State &state, uint8_t opcode) const
{
    state.rexPresent = true;
    state.rexW = (opcode & 8) != 0;
    state.rexR = (opcode & 4) != 0;
    state.rexX = (opcode & 2) != 0;
    state.rexB = (opcode & 1) != 0;
}

X86::RegisterMode
X86::sizeToMode(X86InstructionSize s)
{
    switch (s) {
        case x86_insnsize_16: return rmWord;
        case x86_insnsize_32: return rmDWord;
        case x86_insnsize_64: return rmQWord;
        default: {
            ASSERT_not_reachable("not a valid instruction size: " +
                                 stringifyBinaryAnalysisX86InstructionSize(s));
            /* avoid MSCV warning by adding return stmt */
            return rmWord;
        }
    }
}

SgAsmType *
X86::sizeToType(X86InstructionSize s)
{
    switch (s) {
        case x86_insnsize_none: return NULL;
        case x86_insnsize_16: return SageBuilderAsm::buildTypeX86Word();
        case x86_insnsize_32: return SageBuilderAsm::buildTypeX86DoubleWord();
        case x86_insnsize_64: return SageBuilderAsm::buildTypeX86QuadWord();
        default: {
            abort();
            /* avoid MSCV warning by adding return stmt */
            return NULL;
        }
    }
}





/*========================================================================================================================
 * Methods that construct something. (Their names all start with "make".)
 *========================================================================================================================*/

SgAsmExpression *
X86::makeAddrSizeValue(State &state, int64_t val, size_t bit_offset, size_t bit_size) const
{
    SgAsmValueExpression *retval = NULL;
    switch (effectiveAddressSize(state)) {
        case x86_insnsize_16:
            retval = SageBuilderAsm::buildValueX86Word((uint16_t)val);
            break;
        case x86_insnsize_32:
            retval = SageBuilderAsm::buildValueX86DWord((uint32_t)val);
            break;
        case x86_insnsize_64:
            retval = SageBuilderAsm::buildValueX86QWord((uint64_t)val);
            break;
        default:
            ASSERT_not_reachable("not a valid effective address size " +
                                 stringifyBinaryAnalysisX86InstructionSize(effectiveAddressSize(state)));
    }
    retval->set_bitOffset(bit_offset);
    retval->set_bitSize(bit_size);
    return retval;
}

SgAsmX86Instruction *
X86::makeInstruction(State &state, X86InstructionKind kind, SgAsmExpression *op1, SgAsmExpression *op2, SgAsmExpression *op3,
                     SgAsmExpression *op4) const
{
    SgAsmX86Instruction *insn = new SgAsmX86Instruction(state.ip, *architecture()->registrationId(), kind, insnSize,
                                                        effectiveOperandSize(state), effectiveAddressSize(state));
    ASSERT_not_null(insn);
    insn->set_lockPrefix(state.lock);
    insn->set_repeatPrefix(state.repeatPrefix);
    if (state.insnbufat > 0)
        insn->set_rawBytes(SgUnsignedCharList(&(state.insnbuf[0]), &(state.insnbuf[0])+state.insnbufat));
    if (state.segOverride != x86_segreg_none)
        insn->set_segmentOverride(state.segOverride);
    if (state.branchPredictionEnabled)
        insn->set_branchPrediction(state.branchPrediction);

    SgAsmOperandList *operands = new SgAsmOperandList();
    insn->set_operandList(operands);
    operands->set_parent(insn);

    /* If any operand is null, then the following operands must also be null because analysis assumes that the operand vector
     * indices correspond to argument positions and don't expect null-padding in the vector. */
    ASSERT_require2((!op1 && !op2 && !op3 && !op4) ||
                    ( op1 && !op2 && !op3 && !op4) ||
                    ( op1 &&  op2 && !op3 && !op4) ||
                    ( op1 &&  op2 &&  op3 && !op4) ||
                    ( op1 &&  op2 &&  op3 &&  op4),
                    "if any operand is null then the following operands must be null");

    if (op1)
        SageBuilderAsm::appendOperand(insn, op1);
    if (op2)
        SageBuilderAsm::appendOperand(insn, op2);
    if (op3)
        SageBuilderAsm::appendOperand(insn, op3);
    if (op4)
        SageBuilderAsm::appendOperand(insn, op4);

    return insn;
}

SgAsmRegisterReferenceExpression *
X86::makeIP() const
{
    ASSERT_forbid(REG_IP.isEmpty());
    SgAsmRegisterReferenceExpression *r = new SgAsmDirectRegisterExpression(REG_IP);
    r->set_type(sizeToType(insnSize));
    return r;
}

SgAsmRegisterReferenceExpression *
X86::makeOperandRegisterByte(State &state, bool rexExtension, uint8_t registerNumber) const
{
    return makeRegister(state, (rexExtension ? 8 : 0) + registerNumber,
                        (state.rexPresent ? rmRexByte : rmLegacyByte));
}

SgAsmRegisterReferenceExpression *
X86::makeOperandRegisterFull(State &state, bool rexExtension, uint8_t registerNumber) const
{
    return makeRegister(state, (rexExtension ? 8 : 0) + registerNumber,
                        sizeToMode(insnSize));
}

/* At one time this function created x86-specific register reference expressions (RREs) that had hard-coded values for register
 * class, register number, and register position. These values had the same meanings across all x86 architectures and
 * corresponded to various enums in ROSE.
 *
 * The new approach (added Oct 2010) replaces x86-specific values with a more generic RegisterDescriptor struct, where each
 * register is described by a major number (formerly the register class), a minor number (formerly the register number), and a
 * bit offset and size (formerly both represented by the register position).  The idea is that a RegisterDescriptor does not
 * need to contain machine-specific values. Therefore, we've added a level of indirection:  makeRegister() converts
 * machine-specific values to a register name, which is then looked up in a RegisterDictionary to return a
 * RegisterDescriptor.  The entries in the dictionary determine what registers are available to the disassembler.
 *
 * Currently (2010-10-05) the old class and numbers are used as the major and minor values but users should not assume that
 * this is the case. They can assume that unrelated registers (e.g., "eax" vs "ebx") have descriptors that map to
 * non-overlapping areas of the descriptor address space {major,minor,offset,size} while related registers (e.g., "eax" vs
 * "ax") map to overlapping areas of the descriptor address space. */
SgAsmRegisterReferenceExpression *
X86::makeRegister(State &state, uint8_t fullRegisterNumber, RegisterMode m, SgAsmType *registerType) const
{
    /* Register names for various RegisterMode, indexed by the fullRegisterNumber. The names and order of these names come from
     * Intel documentation. */
    static const char* regnames8l[16] = {
        "al",  "cl",  "dl",  "bl",  "spl", "bpl", "sil", "dil", "r8b", "r9b", "r10b", "r11b", "r12b", "r13b", "r14b", "r15b"
    };
    static const char* regnames8h[4] = {
        "ah",  "ch",  "dh",  "bh"
    };
    static const char* regnames16[16] = {
        "ax",  "cx",  "dx",  "bx",  "sp",  "bp",  "si",  "di",  "r8w", "r9w", "r10w", "r11w", "r12w", "r13w", "r14w", "r15w"
    };
    static const char* regnames32[16] = {
        "eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi", "r8d", "r9d", "r10d", "r11d", "r12d", "r13d", "r14d", "r15d"
    };
    static const char* regnames64[16] = {
        "rax", "rcx", "rdx", "rbx", "rsp", "rbp", "rsi", "rdi", "r8",  "r9",  "r10",  "r11",  "r12",  "r13",  "r14",  "r15"
    };
    static const char* regnamesSeg[6] = {
        "es", "cs", "ss", "ds", "fs", "gs"
    };

    /* Obtain a register name. Also, override the registerType value for certain registers. */
    std::string name;
    switch (m) {
        case rmLegacyByte:
            if (fullRegisterNumber >= 8)
                throw ExceptionX86("register number out of bounds", state);
            if (fullRegisterNumber & 4) {
                name = regnames8h[fullRegisterNumber % 4];
            } else {
                name = regnames8l[fullRegisterNumber % 4];
            }
            registerType = BYTET;
            break;
        case rmRexByte:
            if (fullRegisterNumber >= 16)
                throw ExceptionX86("register number out of bounds", state);
            name = regnames8l[fullRegisterNumber];
            registerType = BYTET;
            break;
        case rmWord:
            if (fullRegisterNumber >= 16)
                throw ExceptionX86("register number out of bounds", state);
            name = regnames16[fullRegisterNumber];
            registerType = WORDT;
            break;
        case rmDWord:
            if (fullRegisterNumber >= 16)
                throw ExceptionX86("register number out of bounds", state);
            name = regnames32[fullRegisterNumber];
            registerType = DWORDT;
            break;
        case rmQWord:
            if (fullRegisterNumber >= 16)
                throw ExceptionX86("register number out of bounds", state);
            name = regnames64[fullRegisterNumber];
            registerType = QWORDT;
            break;
        case rmSegment:
            if (fullRegisterNumber >= 6)
                throw ExceptionX86("register number out of bounds", state);
            name = regnamesSeg[fullRegisterNumber];
            registerType = WORDT;
            break;
        case rmST:
            name = "st0";        // the first physical "st" register. See dictionary comments.
            registerType = LDOUBLET;
            break;
        case rmMM:
            name = "mm" + StringUtility::numberToString(fullRegisterNumber);
            break;
        case rmXMM:
            name = "xmm" + StringUtility::numberToString(fullRegisterNumber);
            break;
        case rmControl:
            name = "cr" + StringUtility::numberToString(fullRegisterNumber);
            break;
        case rmDebug:
            name = "dr" + StringUtility::numberToString(fullRegisterNumber);
            break;
        case rmReturnNull:
            return NULL;
    }
    ASSERT_forbid(name.empty());

    /* Now that we have a register name, obtain the register descriptor from the dictionary. */
    const RegisterDescriptor rdesc = architecture()->registerDictionary()->find(name);
    if (!rdesc)
        throw ExceptionX86("register \"" + name + "\" is not available for " + architecture()->registerDictionary()->name(), state);

    /* Construct the return value. */
    SgAsmRegisterReferenceExpression *rre = NULL;
    if (m != rmST) {
        rre = new SgAsmDirectRegisterExpression(rdesc);
    } else {
        // ST registers are different than most others. Starting with i387, the CPU has eight physical ST registers which
        // are treated as a circular stack, with ST(0) being the top of the stack.  See comments in
        // RegisterDictionary::instanceI386Math for details.
        RegisterDescriptor stride(0, 1, 0, 0);          // increment the minor number
        RegisterDescriptor offset(x86_regclass_flags, x86_flags_fpstatus, 11, 3); // "fpstatus_top"
        size_t index = fullRegisterNumber;
        rre = new SgAsmIndirectRegisterExpression(rdesc, stride, offset, index, x86_st_nregs);
    }
    
    ASSERT_not_null(rre);
    rre->set_type(registerType);
    return rre;
}

SgAsmExpression *
X86::makeSegmentRegister(State &state, X86SegmentRegister so, bool insn64) const
{
    switch (so) {
        case x86_segreg_none: ASSERT_not_reachable("makeSegmentRegister must not be x86_segreg_none");
        case x86_segreg_cs: return makeRegister(state, insn64 ? x86_segreg_ds : x86_segreg_cs, rmSegment);
        case x86_segreg_ds: return makeRegister(state, x86_segreg_ds, rmSegment);
        case x86_segreg_es: return makeRegister(state, insn64 ? x86_segreg_ds : x86_segreg_es, rmSegment);
        case x86_segreg_fs: return makeRegister(state, x86_segreg_fs, rmSegment);
        case x86_segreg_gs: return makeRegister(state, x86_segreg_gs, rmSegment);
        case x86_segreg_ss: return makeRegister(state, insn64 ? x86_segreg_ds : x86_segreg_ss, rmSegment);
        default: ASSERT_not_reachable("bad segment register in makeSegmentRegister");
    }

    /* avoid MSCV warning by adding return stmt */
    return NULL;
}





/*========================================================================================================================
 * Methods for operating on the ModR/M byte.
 *========================================================================================================================*/

void
X86::getModRegRM(State &state, RegisterMode regMode, RegisterMode rmMode, SgAsmType *t, SgAsmType *tForReg) const
{
    if (!tForReg)
        tForReg = t;
    state.modregrmByte = getByte(state);
    state.modregrmByteSet = true;
    state.modeField = state.modregrmByte >> 6;
    state.regField = (state.modregrmByte & 070) >> 3;
    state.rmField = state.modregrmByte & 7;
    state.reg = makeModrmRegister(state, regMode, tForReg);
    state.modrm = makeModrmNormal(state, rmMode, t);
}

SgAsmMemoryReferenceExpression *
X86::decodeModrmMemory(State &state) const
{
    ASSERT_require(state.modregrmByteSet);
    SgAsmExpression* addressExpr = NULL;
    X86SegmentRegister defaultSeg = x86_segreg_ds;
    if (effectiveAddressSize(state) == x86_insnsize_16) {
        if (state.modeField == 0 && state.rmField == 6) {
            /* Special case */
            size_t bit_offset = 8*state.insnbufat;
            SgAsmValueExpression *ve = SageBuilderAsm::buildValueX86Word(getWord(state));
            ve->set_bitOffset(bit_offset);
            ve->set_bitSize(32);
            addressExpr = ve;
        } else {
            switch (state.rmField) {
                case 0:
                    defaultSeg = x86_segreg_ds;
                    addressExpr = SageBuilderAsm::buildAddExpression(makeRegister(state, 3, rmWord), makeRegister(state, 6, rmWord));
                    break;
                case 1:
                    defaultSeg = x86_segreg_ds;
                    addressExpr = SageBuilderAsm::buildAddExpression(makeRegister(state, 3, rmWord), makeRegister(state, 7, rmWord));
                    break;
                case 2:
                    defaultSeg = x86_segreg_ss;
                    addressExpr = SageBuilderAsm::buildAddExpression(makeRegister(state, 5, rmWord), makeRegister(state, 6, rmWord));
                    break;
                case 3:
                    defaultSeg = x86_segreg_ss;
                    addressExpr = SageBuilderAsm::buildAddExpression(makeRegister(state, 5, rmWord), makeRegister(state, 7, rmWord));
                    break;
                case 4:
                    defaultSeg = x86_segreg_ds;
                    addressExpr = makeRegister(state, 6, rmWord);
                    break;
                case 5:
                    defaultSeg = x86_segreg_ds;
                    addressExpr = makeRegister(state, 7, rmWord);
                    break;
                case 6:
                    defaultSeg = x86_segreg_ss;
                    addressExpr = makeRegister(state, 5, rmWord);
                    break;
                case 7:
                    defaultSeg = x86_segreg_ds;
                    addressExpr = makeRegister(state, 3, rmWord);
                    break;
                default: ASSERT_not_reachable("invalid state.rmField: " + StringUtility::numberToString(state.rmField));
            }
            switch (state.modeField) {
                case 0:
                    break; // No offset
                case 1: {
                    size_t bit_offset = 8*state.insnbufat;
                    uint8_t offset = getByte(state);
                    SgAsmValueExpression *wv = SageBuilderAsm::buildValueX86Word((int16_t)(int8_t)offset);
                    wv->set_bitOffset(bit_offset);
                    wv->set_bitSize(8);
                    addressExpr = SageBuilderAsm::buildAddExpression(addressExpr, wv);
                    break;
                }
                case 2: {
                    size_t bit_offset = 8*state.insnbufat;
                    uint16_t offset = getWord(state);
                    SgAsmValueExpression *wv = SageBuilderAsm::buildValueX86Word(offset);
                    wv->set_bitOffset(bit_offset);
                    wv->set_bitSize(16);
                    addressExpr = SageBuilderAsm::buildAddExpression(addressExpr, wv);
                    break;
                }
                default:
                    ASSERT_not_reachable("invalid mode field: " + StringUtility::numberToString(state.modeField));
            }
        }
    } else {
        /* 32 or 64 bits */
        if (state.modeField == 0 && state.rmField == 5) {
            /* Special case */
            size_t bit_offset = 8*state.insnbufat;
            uint32_t offset = getDWord(state);
            addressExpr = makeAddrSizeValue(state, IntegerOps::signExtend<32, 64>((uint64_t)offset), bit_offset, 32);
            if (insnSize == x86_insnsize_64) {
                addressExpr = SageBuilderAsm::buildAddExpression(makeIP(), addressExpr);
            }
        } else {
            if (state.rmField == 4) { /* Need SIB */
                uint8_t sib = getByte(state);
                uint8_t sibScaleField = sib >> 6;
                uint8_t sibIndexField = (sib & 0070) >> 3;
                uint8_t sibBaseField = sib & 7;
                uint8_t actualScale = (1 << sibScaleField);
                SgAsmExpression* sibBase = NULL;
                if (sibBaseField == 5) {
                    switch (state.modeField) {
                        case 0: {
                            size_t bit_offset = 8*state.insnbufat;
                            uint32_t offset = getDWord(state);
                            sibBase = makeAddrSizeValue(state, IntegerOps::signExtend<32, 64>((uint64_t)offset), bit_offset, 32);
                            break;
                        }
                        case 1: {
                            sibBase = makeRegister(state, (state.rexB ? 13 : 5), sizeToMode(insnSize));
                            defaultSeg = x86_segreg_ss;
                            break;
                        }
                        case 2: {
                            sibBase = makeRegister(state, (state.rexB ? 13 : 5), sizeToMode(insnSize));
                            defaultSeg = x86_segreg_ss;
                            break;
                        }
                        default:
                            ASSERT_not_reachable("invalid mode field: " + StringUtility::numberToString(state.modeField));
                    }
                } else {
                    sibBase = makeOperandRegisterFull(state, state.rexB, sibBaseField);
                    if (sibBaseField == 4)
                        defaultSeg = x86_segreg_ss;
                }
                if (sibIndexField == 4 && !state.rexX) {
                    addressExpr = sibBase;
                } else if (actualScale == 1) {
                    addressExpr = SageBuilderAsm::buildAddExpression(sibBase, makeOperandRegisterFull(state, state.rexX, sibIndexField));
                } else {
                    SgAsmExpression *regExpr = makeOperandRegisterFull(state, state.rexX, sibIndexField);
                    SgAsmExpression *scaleExpr = SageBuilderAsm::buildValueX86Byte(actualScale);
                    SgAsmExpression *productExpr = SageBuilderAsm::buildMultiplyExpression(regExpr, scaleExpr);
                    addressExpr = SageBuilderAsm::buildAddExpression(sibBase, productExpr);
                }
            } else {
                addressExpr = makeOperandRegisterFull(state, state.rexB, state.rmField);
                if (state.rmField == 5)
                    defaultSeg = x86_segreg_ss;
            }
            switch (state.modeField) {
                case 0:
                    break; /* No offset */
                case 1: {
                    size_t bit_offset = 8*state.insnbufat;
                    uint8_t offset = getByte(state);
                    SgAsmIntegerValueExpression *offsetExpr = SageBuilderAsm::buildValueX86Byte(offset);
                    offsetExpr->set_bitOffset(bit_offset);
                    offsetExpr->set_bitSize(8);
                    addressExpr = SageBuilderAsm::buildAddExpression(addressExpr, offsetExpr);
                    break;
                }
                case 2: {
                    size_t bit_offset = 8*state.insnbufat;
                    uint32_t offset = getDWord(state);
                    SgAsmIntegerValueExpression *offsetExpr = SageBuilderAsm::buildValueX86DWord(offset);
                    offsetExpr->set_bitOffset(bit_offset);
                    offsetExpr->set_bitSize(32);
                    addressExpr = SageBuilderAsm::buildAddExpression(addressExpr, offsetExpr);
                    break;
                }
                default:
                    ASSERT_not_reachable("invalid mode field: " + StringUtility::numberToString(state.modeField));
            }
        }
    }
    ASSERT_not_null(addressExpr);
    X86SegmentRegister seg;
    if (state.segOverride != x86_segreg_none) {
        seg = state.segOverride;
    } else {
        seg = defaultSeg;
    }
    SgAsmMemoryReferenceExpression* mr = SageBuilderAsm::buildMemoryReferenceExpression(addressExpr,
                                                                             makeSegmentRegister(state, seg, insnSize==x86_insnsize_64));
    return mr;
}

void
X86::fillInModRM(State &state, RegisterMode rmMode, SgAsmType *t) const
{
    if (state.modeField == 3) {
        state.modrm = makeRegister(state, (state.rexB ? 8 : 0) + state.rmField, rmMode, t);
    } else {
        isSgAsmMemoryReferenceExpression(state.modrm)->set_type(t);
    }
}

SgAsmExpression *
X86::makeModrmNormal(State &state, RegisterMode m, SgAsmType* mrType) const
{
    ASSERT_require(state.modregrmByteSet);
    if (state.modeField == 3) {
        /* Register */
        if (m == rmLegacyByte && state.rexPresent)
            m = rmRexByte;
        return makeRegister(state, (state.rexB ? 8 : 0) + state.rmField, m, mrType);
    } else {
        SgAsmMemoryReferenceExpression* modrm = decodeModrmMemory(state);
        modrm->set_type(mrType);
        return modrm;
    }
}

SgAsmRegisterReferenceExpression *
X86::makeModrmRegister(State &state, RegisterMode m, SgAsmType* mrType) const
{
    ASSERT_require(state.modregrmByteSet);
    if (m == rmLegacyByte && state.rexPresent)
        m = rmRexByte;
    return makeRegister(state, (state.rexR ? 8 : 0) + state.regField, m, mrType);
}





/*========================================================================================================================
 * Methods that construct an SgAsmExpression for an immediate operand.
 *========================================================================================================================*/

SgAsmExpression *
X86::getImmByte(State &state) const
{
    size_t bit_offset = 8*state.insnbufat;
    SgAsmValueExpression *retval = SageBuilderAsm::buildValueX86Byte(getByte(state));
    retval->set_bitOffset(bit_offset);
    retval->set_bitSize(8);
    return retval;
}

SgAsmExpression *
X86::getImmWord(State &state) const
{
    size_t bit_offset = 8*state.insnbufat;
    SgAsmValueExpression *retval = SageBuilderAsm::buildValueX86Word(getWord(state));
    retval->set_bitOffset(bit_offset);
    retval->set_bitSize(16);
    return retval;
}

SgAsmExpression *
X86::getImmDWord(State &state) const
{
    size_t bit_offset = 8*state.insnbufat;
    SgAsmValueExpression *retval = SageBuilderAsm::buildValueX86DWord(getDWord(state));
    retval->set_bitOffset(bit_offset);
    retval->set_bitSize(32);
    return retval;
}

SgAsmExpression *
X86::getImmQWord(State &state) const
{
    size_t bit_offset = 8*state.insnbufat;
    SgAsmValueExpression *retval = SageBuilderAsm::buildValueX86QWord(getQWord(state));
    retval->set_bitOffset(bit_offset);
    retval->set_bitSize(64);
    return retval;
}

SgAsmExpression *
X86::getImmForAddr(State &state) const
{
    switch (effectiveAddressSize(state)) {
        case x86_insnsize_16: return getImmWord(state);
        case x86_insnsize_32: return getImmDWord(state);
        case x86_insnsize_64: return getImmQWord(state);
        default: {
            ASSERT_not_reachable("invalid effective address size: " +
                                 stringifyBinaryAnalysisX86InstructionSize(effectiveAddressSize(state)));
            /* avoid MSCV warning by adding return stmt */
            return NULL;
        }
    }
}

SgAsmExpression *
X86::getImmIv(State &state) const
{
    switch (effectiveOperandSize(state)) {
        case x86_insnsize_16: return getImmWord(state);
        case x86_insnsize_32: return getImmDWord(state);
        case x86_insnsize_64: return getImmQWord(state);
        default: {
            ASSERT_not_reachable("invalid effective operand size: " +
                                 stringifyBinaryAnalysisX86InstructionSize(effectiveOperandSize(state)));
            /* avoid MSCV warning by adding return stmt */
            return NULL;
        }
    }
}

SgAsmExpression *
X86::getImmJz(State &state) const
{
    uint64_t val;
    size_t bit_offset=8*state.insnbufat, bit_size=0;
    if (effectiveOperandSize(state) == x86_insnsize_16) {
        bit_size = 16;
        uint16_t val2 = getWord(state);
        val = IntegerOps::signExtend<16, 64>((uint64_t)val2);
    } else {
        bit_size = 32;
        uint32_t val2 = getDWord(state);
        val = IntegerOps::signExtend<32, 64>((uint64_t)val2);
    }
    uint64_t target = state.ip + state.insnbufat + val;
    SgAsmValueExpression *retval = NULL;
    switch (insnSize) {
        case x86_insnsize_16:
            retval = SageBuilderAsm::buildValueX86Word(target);
            break;
        case x86_insnsize_32:
            retval = SageBuilderAsm::buildValueX86DWord(target);
            break;
        default:
            retval = SageBuilderAsm::buildValueX86QWord(target);
            break;
    }
    retval->set_bitOffset(bit_offset);
    retval->set_bitSize(bit_size);
    return retval;
}

SgAsmExpression *
X86::getImmByteAsIv(State &state) const
{
    SgAsmValueExpression *retval = NULL;
    size_t bit_offset = 8*state.insnbufat;
    uint8_t val = getByte(state);
    retval = SageBuilderAsm::buildValueX86Byte(val);
    retval->set_bitOffset(bit_offset);
    retval->set_bitSize(8);
    return retval;
}

SgAsmExpression *
X86::getImmIzAsIv(State &state) const
{
    switch (effectiveOperandSize(state)) {
        case x86_insnsize_16:
            return getImmWord(state);
        case x86_insnsize_32:
            return getImmDWord(state);
        case x86_insnsize_64: {
            return getImmDWord(state);
        }
        default: {
            ASSERT_not_reachable("invalid effective operand size: " +
                                 stringifyBinaryAnalysisX86InstructionSize(effectiveOperandSize(state)));
            /* avoid MSCV warning by adding return stmt */
            return NULL;
        }
    }
}

SgAsmExpression *
X86::getImmJb(State &state) const
{
    size_t bit_offset = 8*state.insnbufat;
    uint8_t val = getByte(state);
    uint64_t target = state.ip + state.insnbufat + IntegerOps::signExtend<8, 64>((uint64_t)val);
    SgAsmValueExpression *retval=NULL;
    switch (insnSize) {
        case x86_insnsize_16:
            retval = SageBuilderAsm::buildValueX86Word(target);
            break;
        case x86_insnsize_32:
            retval = SageBuilderAsm::buildValueX86DWord(target);
            break;
        case x86_insnsize_64:
            retval = SageBuilderAsm::buildValueX86QWord(target);
            break;
        default:
            ASSERT_not_reachable("invalid instruction size: " +
                                 stringifyBinaryAnalysisX86InstructionSize(insnSize));
            return NULL;                                // not reachable, but avoids MSCV warning
    }
    retval->set_bitOffset(bit_offset);
    retval->set_bitSize(8);
    return retval;
}





/*========================================================================================================================
 * Main disassembly functions, each generally containing a huge "switch" statement based on one of the opcode bytes.
 *========================================================================================================================*/

/* Mostly copied from the old x86Disassembler.C version */
SgAsmX86Instruction *
X86::disassemble(State &state) const
{
    uint8_t opcode = getByte(state);
    SgAsmX86Instruction *insn = 0;
    switch (opcode) {
        case 0x00: {
            getModRegRM(state, rmLegacyByte, rmLegacyByte, BYTET);
            insn = makeInstruction(state, x86_add, state.modrm, state.reg);
            goto done;
        }
        case 0x01: {
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            insn = makeInstruction(state, x86_add, state.modrm, state.reg);
            goto done;
        }
        case 0x02: {
            getModRegRM(state, rmLegacyByte, rmLegacyByte, BYTET);
            insn = makeInstruction(state, x86_add, state.reg, state.modrm);
            goto done;
        }
        case 0x03: {
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            insn = makeInstruction(state, x86_add, state.reg, state.modrm);
            goto done;
        }
        case 0x04: {
            SgAsmExpression* imm = getImmByteAsIv(state);
            insn = makeInstruction(state, x86_add, makeRegister(state, 0, rmLegacyByte), imm);
            goto done;
        }
        case 0x05: {
            SgAsmExpression* imm = getImmIzAsIv(state);
            insn = makeInstruction(state, x86_add, makeRegisterEffective(state, 0), imm);
            goto done;
        }
        case 0x06: {
            not64(state);
            insn = makeInstruction(state, x86_push, makeRegister(state, 0, rmSegment));
            goto done;
        }
        case 0x07: {
            not64(state);
            insn = makeInstruction(state, x86_pop, makeRegister(state, 0, rmSegment));
            goto done;
        }
        case 0x08: {
            getModRegRM(state, rmLegacyByte, rmLegacyByte, BYTET);
            insn = makeInstruction(state, x86_or, state.modrm, state.reg);
            goto done;
        }
        case 0x09: {
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            insn = makeInstruction(state, x86_or, state.modrm, state.reg);
            goto done;
        }
        case 0x0A: {
            getModRegRM(state, rmLegacyByte, rmLegacyByte, BYTET);
            insn = makeInstruction(state, x86_or, state.reg, state.modrm);
            goto done;
        }
        case 0x0B: {
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            insn = makeInstruction(state, x86_or, state.reg, state.modrm);
            goto done;
        }
        case 0x0C: {
            SgAsmExpression* imm = getImmByteAsIv(state);
            insn = makeInstruction(state, x86_or, makeRegister(state, 0, rmLegacyByte), imm);
            goto done;
        }
        case 0x0D: {
            SgAsmExpression* imm = getImmIzAsIv(state);
            insn = makeInstruction(state, x86_or, makeRegisterEffective(state, 0), imm);
            goto done;
        }
        case 0x0E: {
            not64(state);
            insn = makeInstruction(state, x86_push, makeRegister(state, 1, rmSegment));
            goto done;
        }
        case 0x0F: {
            insn = decodeOpcode0F(state);
            goto done;
        }
        case 0x10: {
            getModRegRM(state, rmLegacyByte, rmLegacyByte, BYTET);
            insn = makeInstruction(state, x86_adc, state.modrm, state.reg);
            goto done;
        }
        case 0x11: {
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            insn = makeInstruction(state, x86_adc, state.modrm, state.reg);
            goto done;
        }
        case 0x12: {
            getModRegRM(state, rmLegacyByte, rmLegacyByte, BYTET);
            insn = makeInstruction(state, x86_adc, state.reg, state.modrm);
            goto done;
        }
        case 0x13: {
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            insn = makeInstruction(state, x86_adc, state.reg, state.modrm);
            goto done;
        }
        case 0x14: {
            SgAsmExpression* imm = getImmByteAsIv(state);
            insn = makeInstruction(state, x86_adc, makeRegister(state, 0, rmLegacyByte), imm);
            goto done;
        }
        case 0x15: {
            SgAsmExpression* imm = getImmIzAsIv(state);
            insn = makeInstruction(state, x86_adc, makeRegisterEffective(state, 0), imm);
            goto done;
        }
        case 0x16: {
            not64(state);
            insn = makeInstruction(state, x86_push, makeRegister(state, 2, rmSegment));
            goto done;
        }
        case 0x17: {
            not64(state);
            insn = makeInstruction(state, x86_pop, makeRegister(state, 2, rmSegment));
            goto done;
        }
        case 0x18: {
            getModRegRM(state, rmLegacyByte, rmLegacyByte, BYTET);
            insn = makeInstruction(state, x86_sbb, state.modrm, state.reg);
            goto done;
        }
        case 0x19: {
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            insn = makeInstruction(state, x86_sbb, state.modrm, state.reg);
            goto done;
        }
        case 0x1A: {
            getModRegRM(state, rmLegacyByte, rmLegacyByte, BYTET);
            insn = makeInstruction(state, x86_sbb, state.reg, state.modrm);
            goto done;
        }
        case 0x1B: {
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            insn = makeInstruction(state, x86_sbb, state.reg, state.modrm);
            goto done;
        }
        case 0x1C: {
            SgAsmExpression* imm = getImmByteAsIv(state);
            insn = makeInstruction(state, x86_sbb, makeRegister(state, 0, rmLegacyByte), imm);
            goto done;
        }
        case 0x1D: {
            SgAsmExpression* imm = getImmIzAsIv(state);
            insn = makeInstruction(state, x86_sbb, makeRegisterEffective(state, 0), imm);
            goto done;
        }
        case 0x1E: {
            not64(state);
            insn = makeInstruction(state, x86_push, makeRegister(state, 3, rmSegment));
            goto done;
        }
        case 0x1F: {
            not64(state);
            insn = makeInstruction(state, x86_pop, makeRegister(state, 3, rmSegment));
            goto done;
        }
        case 0x20: {
            getModRegRM(state, rmLegacyByte, rmLegacyByte, BYTET);
            insn = makeInstruction(state, x86_and, state.modrm, state.reg);
            goto done;
        }
        case 0x21: {
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            insn = makeInstruction(state, x86_and, state.modrm, state.reg);
            goto done;
        }
        case 0x22: {
            getModRegRM(state, rmLegacyByte, rmLegacyByte, BYTET);
            insn = makeInstruction(state, x86_and, state.reg, state.modrm);
            goto done;
        }
        case 0x23: {
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            insn = makeInstruction(state, x86_and, state.reg, state.modrm);
            goto done;
        }
        case 0x24: {
            SgAsmExpression* imm = getImmByteAsIv(state);
            insn = makeInstruction(state, x86_and, makeRegister(state, 0, rmLegacyByte), imm);
            goto done;
        }
        case 0x25: {
            SgAsmExpression* imm = getImmIzAsIv(state);
            insn = makeInstruction(state, x86_and, makeRegisterEffective(state, 0), imm);
            goto done;
        }
        case 0x26: {
            state.segOverride = x86_segreg_es;
            insn = disassemble(state);
            goto done;
        }
        case 0x27: {
            not64(state);
            insn = makeInstruction(state, x86_daa);
            goto done;
        }
        case 0x28: {
            getModRegRM(state, rmLegacyByte, rmLegacyByte, BYTET);
            insn = makeInstruction(state, x86_sub, state.modrm, state.reg);
            goto done;
        }
        case 0x29: {
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            insn = makeInstruction(state, x86_sub, state.modrm, state.reg);
            goto done;
        }
        case 0x2A: {
            getModRegRM(state, rmLegacyByte, rmLegacyByte, BYTET);
            insn = makeInstruction(state, x86_sub, state.reg, state.modrm);
            goto done;
        }
        case 0x2B: {
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            insn = makeInstruction(state, x86_sub, state.reg, state.modrm);
            goto done;
        }
        case 0x2C: {
            SgAsmExpression* imm = getImmByteAsIv(state);
            insn = makeInstruction(state, x86_sub, makeRegister(state, 0, rmLegacyByte), imm);
            goto done;
        }
        case 0x2D: {
            SgAsmExpression* imm = getImmIzAsIv(state);
            insn = makeInstruction(state, x86_sub, makeRegisterEffective(state, 0), imm);
            goto done;
        }
        case 0x2E: {
            /* Example of recursive use of disassemble() */
            state.segOverride = x86_segreg_cs;
            state.branchPrediction = x86_branch_prediction_not_taken;
            insn = disassemble(state);
            goto done;
        }
        case 0x2F: {
            not64(state);
            insn = makeInstruction(state, x86_das);
            goto done;
        }
        case 0x30: {
            getModRegRM(state, rmLegacyByte, rmLegacyByte, BYTET);
            insn = makeInstruction(state, x86_xor, state.modrm, state.reg);
            goto done;
        }
        case 0x31: {
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            insn = makeInstruction(state, x86_xor, state.modrm, state.reg);
            goto done;
        }
        case 0x32: {
            getModRegRM(state, rmLegacyByte, rmLegacyByte, BYTET);
            insn = makeInstruction(state, x86_xor, state.reg, state.modrm);
            goto done;
        }
        case 0x33: {
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            insn = makeInstruction(state, x86_xor, state.reg, state.modrm);
            goto done;
        }
        case 0x34: {
            SgAsmExpression* imm = getImmByteAsIv(state);
            insn = makeInstruction(state, x86_xor, makeRegister(state, 0, rmLegacyByte), imm);
            goto done;
        }
        case 0x35: {
            SgAsmExpression* imm = getImmIzAsIv(state);
            insn = makeInstruction(state, x86_xor, makeRegisterEffective(state, 0), imm);
            goto done;
        }
        case 0x36: {
            state.segOverride = x86_segreg_ss;
            insn = disassemble(state);
            goto done;
        }
        case 0x37: {
            not64(state);
            insn = makeInstruction(state, x86_aaa);
            goto done;
        }
        case 0x38: {
            getModRegRM(state, rmLegacyByte, rmLegacyByte, BYTET);
            insn = makeInstruction(state, x86_cmp, state.modrm, state.reg);
            goto done;
        }
        case 0x39: {
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            insn = makeInstruction(state, x86_cmp, state.modrm, state.reg);
            goto done;
        }
        case 0x3A: {
            getModRegRM(state, rmLegacyByte, rmLegacyByte, BYTET);
            insn = makeInstruction(state, x86_cmp, state.reg, state.modrm);
            goto done;
        }
        case 0x3B: {
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            insn = makeInstruction(state, x86_cmp, state.reg, state.modrm);
            goto done;
        }
        case 0x3C: {
            SgAsmExpression* imm = getImmByteAsIv(state);
            insn = makeInstruction(state, x86_cmp, makeRegister(state, 0, rmLegacyByte), imm);
            goto done;
        }
        case 0x3D: {
            SgAsmExpression* imm = getImmIzAsIv(state);
            insn = makeInstruction(state, x86_cmp, makeRegisterEffective(state, 0), imm);
            goto done;
        }
        case 0x3E: {
            state.segOverride = x86_segreg_ds;
            state.branchPrediction = x86_branch_prediction_taken;
            insn = disassemble(state);
            goto done;
        }
        case 0x3F: {
            not64(state);
            insn = makeInstruction(state, x86_aas);
            goto done;
        }
        case 0x40: {
            if (longMode()) {
                setRex(state, 0x40);
                insn = disassemble(state);
                goto done;
            } else {
                insn = makeInstruction(state, x86_inc, makeRegisterEffective(state, 0));
                goto done;
            }
        }
        case 0x41: {
            if (longMode()) {
                setRex(state, 0x41);
                insn = disassemble(state);
                goto done;
            } else {
                insn = makeInstruction(state, x86_inc, makeRegisterEffective(state, 1));
                goto done;
            }
        }
        case 0x42: {
            if (longMode()) {
                setRex(state, 0x42);
                insn = disassemble(state);
                goto done;
            } else {
                insn = makeInstruction(state, x86_inc, makeRegisterEffective(state, 2));
                goto done;
            }
        }
        case 0x43: {
            if (longMode()) {
                setRex(state, 0x43);
                insn = disassemble(state);
                goto done;
            } else {
                insn = makeInstruction(state, x86_inc, makeRegisterEffective(state, 3));
                goto done;
            }
        }
        case 0x44: {
            if (longMode()) {
                setRex(state, 0x44);
                insn = disassemble(state);
                goto done;
            } else {
                insn = makeInstruction(state, x86_inc, makeRegisterEffective(state, 4));
                goto done;
            }
        }
        case 0x45: {
            if (longMode()) {
                setRex(state, 0x45);
                insn = disassemble(state);
                goto done;
            } else {
                insn = makeInstruction(state, x86_inc, makeRegisterEffective(state, 5));
                goto done;
            }
        }
        case 0x46: {
            if (longMode()) {
                setRex(state, 0x46);
                insn = disassemble(state);
                goto done;
            } else {
                insn = makeInstruction(state, x86_inc, makeRegisterEffective(state, 6));
                goto done;
            }
        }
        case 0x47: {
            if (longMode()) {
                setRex(state, 0x47);
                insn = disassemble(state);
                goto done;
            } else {
                insn = makeInstruction(state, x86_inc, makeRegisterEffective(state, 7));
                goto done;
            }
        }
        case 0x48: {
            if (longMode()) {
                setRex(state, 0x48);
                insn = disassemble(state);
                goto done;
            } else {
                insn = makeInstruction(state, x86_dec, makeRegisterEffective(state, 0));
                goto done;
            }
        }
        case 0x49: {
            if (longMode()) {
                setRex(state, 0x49);
                insn = disassemble(state);
                goto done;
            } else {
                insn = makeInstruction(state, x86_dec, makeRegisterEffective(state, 1));
                goto done;
            }
        }
        case 0x4A: {
            if (longMode()) {
                setRex(state, 0x4A);
                insn = disassemble(state);
                goto done;
            } else {
                insn = makeInstruction(state, x86_dec, makeRegisterEffective(state, 2));
                goto done;
            }
        }
        case 0x4B: {
            if (longMode()) {
                setRex(state, 0x4B);
                insn = disassemble(state);
                goto done;
            } else {
                insn = makeInstruction(state, x86_dec, makeRegisterEffective(state, 3));
                goto done;
            }
        }
        case 0x4C: {
            if (longMode()) {
                setRex(state, 0x4C);
                insn = disassemble(state);
                goto done;
            } else {
                insn = makeInstruction(state, x86_dec, makeRegisterEffective(state, 4));
                goto done;
            }
        }
        case 0x4D: {
            if (longMode()) {
                setRex(state, 0x4D);
                insn = disassemble(state);
                goto done;
            } else {
                insn = makeInstruction(state, x86_dec, makeRegisterEffective(state, 5));
                goto done;
            }
        }
        case 0x4E: {
            if (longMode()) {
                setRex(state, 0x4E);
                insn = disassemble(state);
                goto done;
            } else {
                insn = makeInstruction(state, x86_dec, makeRegisterEffective(state, 6));
                goto done;
            }
        }
        case 0x4F: {
            if (longMode()) {
                setRex(state, 0x4F);
                insn = disassemble(state);
                goto done;
            } else {
                insn = makeInstruction(state, x86_dec, makeRegisterEffective(state, 7));
                goto done;
            }
        }
        case 0x50: {
            state.sizeMustBe64Bit = true;
            insn = makeInstruction(state, x86_push, makeRegisterEffective(state, state.rexB, 0));
            goto done;
        }
        case 0x51: {
            state.sizeMustBe64Bit = true;
            insn = makeInstruction(state, x86_push, makeRegisterEffective(state, state.rexB, 1));
            goto done;
        }
        case 0x52: {
            state.sizeMustBe64Bit = true;
            insn = makeInstruction(state, x86_push, makeRegisterEffective(state, state.rexB, 2));
            goto done;
        }
        case 0x53: {
            state.sizeMustBe64Bit = true;
            insn = makeInstruction(state, x86_push, makeRegisterEffective(state, state.rexB, 3));
            goto done;
        }
        case 0x54: {
            state.sizeMustBe64Bit = true;
            insn = makeInstruction(state, x86_push, makeRegisterEffective(state, state.rexB, 4));
            goto done;
        }
        case 0x55: {
            state.sizeMustBe64Bit = true;
            insn = makeInstruction(state, x86_push, makeRegisterEffective(state, state.rexB, 5));
            goto done;
        }
        case 0x56: {
            state.sizeMustBe64Bit = true;
            insn = makeInstruction(state, x86_push, makeRegisterEffective(state, state.rexB, 6));
            goto done;
        }
        case 0x57: {
            state.sizeMustBe64Bit = true;
            insn = makeInstruction(state, x86_push, makeRegisterEffective(state, state.rexB, 7));
            goto done;
        }
        case 0x58: {
            state.sizeMustBe64Bit = true;
            insn = makeInstruction(state, x86_pop, makeRegisterEffective(state, state.rexB, 0));
            goto done;
        }
        case 0x59: {
            state.sizeMustBe64Bit = true;
            insn = makeInstruction(state, x86_pop, makeRegisterEffective(state, state.rexB, 1));
            goto done;
        }
        case 0x5A: {
            state.sizeMustBe64Bit = true;
            insn = makeInstruction(state, x86_pop, makeRegisterEffective(state, state.rexB, 2));
            goto done;
        }
        case 0x5B: {
            state.sizeMustBe64Bit = true;
            insn = makeInstruction(state, x86_pop, makeRegisterEffective(state, state.rexB, 3));
            goto done;
        }
        case 0x5C: {
            state.sizeMustBe64Bit = true;
            insn = makeInstruction(state, x86_pop, makeRegisterEffective(state, state.rexB, 4));
            goto done;
        }
        case 0x5D: {
            state.sizeMustBe64Bit = true;
            insn = makeInstruction(state, x86_pop, makeRegisterEffective(state, state.rexB, 5));
            goto done;
        }
        case 0x5E: {
            state.sizeMustBe64Bit = true;
            insn = makeInstruction(state, x86_pop, makeRegisterEffective(state, state.rexB, 6));
            goto done;
        }
        case 0x5F: {
            state.sizeMustBe64Bit = true;
            insn = makeInstruction(state, x86_pop, makeRegisterEffective(state, state.rexB, 7));
            goto done;
        }
        case 0x60: {
            not64(state);
            if (effectiveOperandSize(state) == x86_insnsize_32) {
                insn = makeInstruction(state, x86_pushad);
            } else {
                insn = makeInstruction(state, x86_pusha);
            }
            goto done;
        }
        case 0x61: {
            not64(state);
            if (effectiveOperandSize(state) == x86_insnsize_32) {
                insn = makeInstruction(state, x86_popad);
            } else {
                insn = makeInstruction(state, x86_popa);
            }
            goto done;
        }
        case 0x62: {
            not64(state);
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            requireMemory(state);
            insn = makeInstruction(state, x86_bound, state.reg, state.modrm);
            goto done;
        }
        case 0x63: {
            if (longMode()) {
                getModRegRM(state, effectiveOperandMode(state), rmDWord, DWORDT);
                insn = makeInstruction(state, x86_movsxd, state.reg, state.modrm);
                goto done;
            } else {
                getModRegRM(state, rmWord, rmWord, WORDT);
                insn = makeInstruction(state, x86_arpl, state.modrm, state.reg);
                goto done;
            }
        }
        case 0x64: {
            state.segOverride = x86_segreg_fs;
            insn = disassemble(state);
            goto done;
        }
        case 0x65: {
            state.segOverride = x86_segreg_gs;
            insn = disassemble(state);
            goto done;
        }
        case 0x66: {
            state.operandSizeOverride = true;
            insn = disassemble(state);
            goto done;
        }
        case 0x67: {
            state.addressSizeOverride = true;
            insn = disassemble(state);
            goto done;
        }
        case 0x68: {
            state.sizeMustBe64Bit = true;
            SgAsmExpression* imm = getImmIzAsIv(state);
            insn = makeInstruction(state, x86_push, imm);
            goto done;
        }
        case 0x69: {
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            SgAsmExpression* imm = getImmIzAsIv(state);
            insn = makeInstruction(state, x86_imul, state.reg, state.modrm, imm);
            goto done;
        }
        case 0x6A: {
            state.sizeMustBe64Bit = true;
            SgAsmExpression* imm = getImmByteAsIv(state);
            insn = makeInstruction(state, x86_push, imm);
            goto done;
        }
        case 0x6B: {
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            SgAsmExpression* imm = getImmByteAsIv(state);
            insn = makeInstruction(state, x86_imul, state.reg, state.modrm, imm);
            goto done;
        }
        case 0x6C: {
            switch (state.repeatPrefix) {
                case x86_repeat_none:
                    insn = makeInstruction(state, x86_insb);
                    goto done;
                case x86_repeat_repe:
                    insn = makeInstruction(state, x86_rep_insb);
                    goto done;
                default:
                    throw ExceptionX86("bad repeat prefix for insb", state);
            }
        }
        case 0x6D: {
            switch (effectiveOperandSize(state)) {
                case x86_insnsize_16:
                    switch (state.repeatPrefix) {
                        case x86_repeat_none:
                            insn = makeInstruction(state, x86_insw);
                            goto done;
                        case x86_repeat_repe:
                            insn = makeInstruction(state, x86_rep_insw);
                            goto done;
                        default:
                            throw ExceptionX86("bad repeat prefix for insw", state);
                    }
                case x86_insnsize_32:
                case x86_insnsize_64:
                    switch (state.repeatPrefix) {
                        case x86_repeat_none:
                            insn = makeInstruction(state, x86_insd);
                            goto done;
                        case x86_repeat_repe:
                            insn = makeInstruction(state, x86_rep_insd);
                            goto done;
                        default:
                            throw ExceptionX86("bad repeat prefix for insd", state);
                    }
                default:
                    ASSERT_not_reachable("invalid effective operand size: " +
                                         stringifyBinaryAnalysisX86InstructionSize(effectiveOperandSize(state)));
            }
        }
        case 0x6E: {
            switch (state.repeatPrefix) {
                case x86_repeat_none:
                    insn = makeInstruction(state, x86_outsb);
                    goto done;
                case x86_repeat_repe:
                    insn = makeInstruction(state, x86_rep_outsb);
                    goto done;
                default:
                    throw ExceptionX86("bad repeat prefix for outsb", state);
            }
        }
        case 0x6F: {
            switch (effectiveOperandSize(state)) {
                case x86_insnsize_16:
                    switch (state.repeatPrefix) {
                        case x86_repeat_none:
                            insn = makeInstruction(state, x86_outsw);
                            goto done;
                        case x86_repeat_repe:
                            insn = makeInstruction(state, x86_rep_outsw);
                            goto done;
                        default:
                            throw ExceptionX86("bad repeat prefix for outsw", state);
                    }
                case x86_insnsize_32:
                case x86_insnsize_64:
                    switch (state.repeatPrefix) {
                        case x86_repeat_none:
                            insn = makeInstruction(state, x86_outsd);
                            goto done;
                        case x86_repeat_repe:
                            insn = makeInstruction(state, x86_rep_outsd);
                            goto done;
                        default:
                            throw ExceptionX86("bad repeat prefix for outsd", state);
                    }
                default:
                    ASSERT_not_reachable("invalid effective operand size: " +
                                         stringifyBinaryAnalysisX86InstructionSize(effectiveOperandSize(state)));
            }
        }
        case 0x70: {
            SgAsmExpression* imm = getImmJb(state);
            state.branchPredictionEnabled = true;
            insn = makeInstruction(state, x86_jo, imm);
            goto done;
        }
        case 0x71: {
            SgAsmExpression* imm = getImmJb(state);
            state.branchPredictionEnabled = true;
            insn = makeInstruction(state, x86_jno, imm);
            goto done;
        }
        case 0x72: {
            SgAsmExpression* imm = getImmJb(state);
            state.branchPredictionEnabled = true;
            insn = makeInstruction(state, x86_jb, imm);
            goto done;
        }
        case 0x73: {
            SgAsmExpression* imm = getImmJb(state);
            state.branchPredictionEnabled = true;
            insn = makeInstruction(state, x86_jae, imm);
            goto done;
        }
        case 0x74: {
            SgAsmExpression* imm = getImmJb(state);
            state.branchPredictionEnabled = true;
            insn = makeInstruction(state, x86_je, imm);
            goto done;
        }
        case 0x75: {
            SgAsmExpression* imm = getImmJb(state);
            state.branchPredictionEnabled = true;
            insn = makeInstruction(state, x86_jne, imm);
            goto done;
        }
        case 0x76: {
            SgAsmExpression* imm = getImmJb(state);
            state.branchPredictionEnabled = true;
            insn = makeInstruction(state, x86_jbe, imm);
            goto done;
        }
        case 0x77: {
            SgAsmExpression* imm = getImmJb(state);
            state.branchPredictionEnabled = true;
            insn = makeInstruction(state, x86_ja, imm);
            goto done;
        }
        case 0x78: {
            SgAsmExpression* imm = getImmJb(state);
            state.branchPredictionEnabled = true;
            insn = makeInstruction(state, x86_js, imm);
            goto done;
        }
        case 0x79: {
            SgAsmExpression* imm = getImmJb(state);
            state.branchPredictionEnabled = true;
            insn = makeInstruction(state, x86_jns, imm);
            goto done;
        }
        case 0x7A: {
            SgAsmExpression* imm = getImmJb(state);
            state.branchPredictionEnabled = true;
            insn = makeInstruction(state, x86_jpe, imm);
            goto done;
        }
        case 0x7B: {
            SgAsmExpression* imm = getImmJb(state);
            state.branchPredictionEnabled = true;
            insn = makeInstruction(state, x86_jpo, imm);
            goto done;
        }
        case 0x7C: {
            SgAsmExpression* imm = getImmJb(state);
            state.branchPredictionEnabled = true;
            insn = makeInstruction(state, x86_jl, imm);
            goto done;
        }
        case 0x7D: {
            SgAsmExpression* imm = getImmJb(state);
            state.branchPredictionEnabled = true;
            insn = makeInstruction(state, x86_jge, imm);
            goto done;
        }
        case 0x7E: {
            SgAsmExpression* imm = getImmJb(state);
            state.branchPredictionEnabled = true;
            insn = makeInstruction(state, x86_jle, imm);
            goto done;
        }
        case 0x7F: {
            SgAsmExpression* imm = getImmJb(state);
            state.branchPredictionEnabled = true;
            insn = makeInstruction(state, x86_jg, imm);
            goto done;
        }
        case 0x80: {
            /* The names for groups will make more sense relative to the AMD manual. */
            getModRegRM(state, rmReturnNull, rmLegacyByte, BYTET);
            SgAsmExpression* imm = getImmByte(state);
            insn = decodeGroup1(state, imm);
            goto done;
        }
        case 0x81: {
            /* effectiveOperandMode(state) returns register mode for the effective operand size (16bit, 32, bit, 64bit)
             * effectiveOperandType(state) does the same thing but returne a SgAsmType. */
            getModRegRM(state, rmReturnNull, effectiveOperandMode(state), effectiveOperandType(state));
            SgAsmExpression* imm = getImmIzAsIv(state);
            insn = decodeGroup1(state, imm);
            goto done;
        }
        case 0x82: {
            not64(state);
            getModRegRM(state, rmReturnNull, rmLegacyByte, BYTET);
            SgAsmExpression* imm = getImmByte(state);
            insn = decodeGroup1(state, imm);
            goto done;
        }
        case 0x83: {
            getModRegRM(state, rmReturnNull, effectiveOperandMode(state), effectiveOperandType(state));
            SgAsmExpression* imm = getImmByteAsIv(state);
            insn = decodeGroup1(state, imm);
            goto done;
        }
        case 0x84: {
            getModRegRM(state, rmLegacyByte, rmLegacyByte, BYTET);
            insn = makeInstruction(state, x86_test, state.modrm, state.reg);
            goto done;
        }
        case 0x85: {
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            insn = makeInstruction(state, x86_test, state.modrm, state.reg);
            goto done;
        }
        case 0x86: {
            getModRegRM(state, rmLegacyByte, rmLegacyByte, BYTET);
            insn = makeInstruction(state, x86_xchg, state.modrm, state.reg);
            goto done;
        }
        case 0x87: {
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            insn = makeInstruction(state, x86_xchg, state.modrm, state.reg);
            goto done;
        }
        case 0x88: {
            getModRegRM(state, rmLegacyByte, rmLegacyByte, BYTET);
            insn = makeInstruction(state, x86_mov, state.modrm, state.reg);
            goto done;
        }
        case 0x89: {
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            insn = makeInstruction(state, x86_mov, state.modrm, state.reg);
            goto done;
        }
        case 0x8A: {
            getModRegRM(state, rmLegacyByte, rmLegacyByte, BYTET);
            insn = makeInstruction(state, x86_mov, state.reg, state.modrm);
            goto done;
        }
        case 0x8B: {
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            insn = makeInstruction(state, x86_mov, state.reg, state.modrm);
            goto done;
        }
        case 0x8C: {
            getModRegRM(state, rmSegment, effectiveOperandMode(state), WORDT);
            insn = makeInstruction(state, x86_mov, state.modrm, state.reg);
            goto done;
        }
        case 0x8D: {
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            requireMemory(state);
            insn = makeInstruction(state, x86_lea, state.reg, state.modrm);
            goto done;
        }
        case 0x8E: {
            getModRegRM(state, rmSegment, rmWord, WORDT);
            insn = makeInstruction(state, x86_mov, state.reg, state.modrm);
            goto done;
        }
        case 0x8F: {
            getModRegRM(state, rmReturnNull, effectiveOperandMode(state), effectiveOperandType(state));
            insn = decodeGroup1a(state);
            goto done;
        }
        case 0x90: {
            if (state.rexB) {
                insn = makeInstruction(state, x86_xchg, makeRegisterEffective(state, 8), makeRegisterEffective(state, 0));
                goto done;
            } else if (state.repeatPrefix == x86_repeat_repe) {
                insn = makeInstruction(state, x86_pause);
                goto done;
            } else {
                insn = makeInstruction(state, x86_nop);
                goto done;
            }
        }
        case 0x91: {
            insn = makeInstruction(state, x86_xchg, makeRegisterEffective(state, state.rexB, 1), makeRegisterEffective(state, 0));
            goto done;
        }
        case 0x92: {
            insn = makeInstruction(state, x86_xchg, makeRegisterEffective(state, state.rexB, 2), makeRegisterEffective(state, 0));
            goto done;
        }
        case 0x93: {
            insn = makeInstruction(state, x86_xchg, makeRegisterEffective(state, state.rexB, 3), makeRegisterEffective(state, 0));
            goto done;
        }
        case 0x94: {
            insn = makeInstruction(state, x86_xchg, makeRegisterEffective(state, state.rexB, 4), makeRegisterEffective(state, 0));
            goto done;
        }
        case 0x95: {
            insn = makeInstruction(state, x86_xchg, makeRegisterEffective(state, state.rexB, 5), makeRegisterEffective(state, 0));
            goto done;
        }
        case 0x96: {
            insn = makeInstruction(state, x86_xchg, makeRegisterEffective(state, state.rexB, 6), makeRegisterEffective(state, 0));
            goto done;
        }
        case 0x97: {
            insn = makeInstruction(state, x86_xchg, makeRegisterEffective(state, state.rexB, 7), makeRegisterEffective(state, 0));
            goto done;
        }
        case 0x98: {
            switch (effectiveOperandSize(state)) {
                case x86_insnsize_16:
                    insn = makeInstruction(state, x86_cbw);
                    goto done;
                case x86_insnsize_32:
                    insn = makeInstruction(state, x86_cwde);
                    goto done;
                case x86_insnsize_64:
                    insn = makeInstruction(state, x86_cdqe);
                    goto done;
                default:
                    ASSERT_not_reachable("invalid effective operand size: " +
                                         stringifyBinaryAnalysisX86InstructionSize(effectiveOperandSize(state)));
            }
        }
        case 0x99: {
            switch (effectiveOperandSize(state)) {
                case x86_insnsize_16:
                    insn = makeInstruction(state, x86_cwd);
                    goto done;
                case x86_insnsize_32:
                    insn = makeInstruction(state, x86_cdq);
                    goto done;
                case x86_insnsize_64:
                    insn = makeInstruction(state, x86_cqo);
                    goto done;
                default:
                    ASSERT_not_reachable("invalid effective operand size: " +
                                         stringifyBinaryAnalysisX86InstructionSize(effectiveOperandSize(state)));
            }
        }
        case 0x9A: {
            not64(state);
            SgAsmExpression* addr = getImmForAddr(state);
            SgAsmExpression* seg = getImmWord(state);
            insn = makeInstruction(state, x86_farcall, seg, addr);
            goto done;
        }
        case 0x9B: {
            insn = makeInstruction(state, x86_wait);
            goto done;
        }
        case 0x9C: {
            state.sizeMustBe64Bit = true;
            switch (effectiveOperandSize(state)) {
                case x86_insnsize_16:
                    insn = makeInstruction(state, x86_pushf);
                    goto done;
                case x86_insnsize_32:
                    insn = makeInstruction(state, x86_pushfd);
                    goto done;
                case x86_insnsize_64:
                    insn = makeInstruction(state, x86_pushfq);
                    goto done;
                default:
                    ASSERT_not_reachable("invalid effective operand size: " +
                                         stringifyBinaryAnalysisX86InstructionSize(effectiveOperandSize(state)));
            }
        }
        case 0x9D: {
            state.sizeMustBe64Bit = true;
            switch (effectiveOperandSize(state)) {
                case x86_insnsize_16:
                    insn = makeInstruction(state, x86_popf);
                    goto done;
                case x86_insnsize_32:
                    insn = makeInstruction(state, x86_popfd);
                    goto done;
                case x86_insnsize_64:
                    insn = makeInstruction(state, x86_popfq);
                    goto done;
                default:
                    ASSERT_not_reachable("invalid effective operand size: " +
                                         stringifyBinaryAnalysisX86InstructionSize(effectiveOperandSize(state)));
            }
        }
        case 0x9E: {
            insn = makeInstruction(state, x86_sahf);
            goto done;
        }
        case 0x9F: {
            insn = makeInstruction(state, x86_lahf);
            goto done;
        }
        case 0xA0: {
            SgAsmExpression* addr = getImmForAddr(state);
            insn = makeInstruction(state, x86_mov, makeRegister(state, 0, rmLegacyByte),
                                         SageBuilderAsm::buildMemoryReferenceExpression(addr, currentDataSegment(state), BYTET));
            goto done;
        }
        case 0xA1: {
            SgAsmExpression* addr = getImmForAddr(state);
            insn = makeInstruction(state, x86_mov, makeRegisterEffective(state, 0),
                                         SageBuilderAsm::buildMemoryReferenceExpression(addr, currentDataSegment(state),
                                                                                        effectiveOperandType(state)));
            goto done;
        }
        case 0xA2: {
            SgAsmExpression* addr = getImmForAddr(state);
            insn = makeInstruction(state, x86_mov,
                                         SageBuilderAsm::buildMemoryReferenceExpression(addr, currentDataSegment(state), BYTET),
                                         makeRegister(state, 0, rmLegacyByte));
            goto done;
        }
        case 0xA3: {
            SgAsmExpression* addr = getImmForAddr(state);
            insn = makeInstruction(state, x86_mov,
                                         SageBuilderAsm::buildMemoryReferenceExpression(addr, currentDataSegment(state),
                                                                                        effectiveOperandType(state)),
                                         makeRegisterEffective(state, 0));
            goto done;
        }
        case 0xA4: {
            switch (state.repeatPrefix) {
                case x86_repeat_none:
                    insn = makeInstruction(state, x86_movsb);
                    goto done;
                case x86_repeat_repe:
                    insn = makeInstruction(state, x86_rep_movsb);
                    goto done;
                default:
                    throw ExceptionX86("bad repeat prefix for movsb", state);
            }
        }
        case 0xA5: {
            switch (effectiveOperandSize(state)) {
                case x86_insnsize_16:
                    switch (state.repeatPrefix) {
                        case x86_repeat_none:
                            insn = makeInstruction(state, x86_movsw);
                            goto done;
                        case x86_repeat_repe:
                            insn = makeInstruction(state, x86_rep_movsw);
                            goto done;
                        default:
                            throw ExceptionX86("bad repeat prefix for movsw", state);
                    }
                case x86_insnsize_32:
                    switch (state.repeatPrefix) {
                        case x86_repeat_none:
                            insn = makeInstruction(state, x86_movsd);
                            goto done;
                        case x86_repeat_repe:
                            insn = makeInstruction(state, x86_rep_movsd);
                            goto done;
                        default:
                            throw ExceptionX86("bad repeat prefix for movsd", state);
                    }
                case x86_insnsize_64:
                    switch (state.repeatPrefix) {
                        case x86_repeat_none:
                            insn = makeInstruction(state, x86_movsq);
                            goto done;
                        case x86_repeat_repe:
                            insn = makeInstruction(state, x86_rep_movsq);
                            goto done;
                        default:
                            throw ExceptionX86("bad repeat prefix for movsq", state);
                    }
                default: ASSERT_not_reachable("invalid effective operand size: " +
                                              stringifyBinaryAnalysisX86InstructionSize(effectiveOperandSize(state)));
            }
        }
        case 0xA6: {
            switch (state.repeatPrefix) {
                case x86_repeat_none:
                    insn = makeInstruction(state, x86_cmpsb);
                    goto done;
                case x86_repeat_repe:
                    insn = makeInstruction(state, x86_repe_cmpsb);
                    goto done;
                case x86_repeat_repne:
                    insn = makeInstruction(state, x86_repne_cmpsb);
                    goto done;
                default:
                    throw ExceptionX86("bad repeat prefix for cmpsb", state);
            }
        }
        case 0xA7: {
            switch (effectiveOperandSize(state)) {
                case x86_insnsize_16:
                    switch (state.repeatPrefix) {
                        case x86_repeat_none:
                            insn = makeInstruction(state, x86_cmpsw);
                            goto done;
                        case x86_repeat_repe:
                            insn = makeInstruction(state, x86_repe_cmpsw);
                            goto done;
                        case x86_repeat_repne:
                            insn = makeInstruction(state, x86_repne_cmpsw);
                            goto done;
                        default:
                            throw ExceptionX86("bad repeat prefix for cmpsw", state);
                    }
                case x86_insnsize_32:
                    switch (state.repeatPrefix) {
                        case x86_repeat_none:
                            insn = makeInstruction(state, x86_cmpsd);
                            goto done;
                        case x86_repeat_repe:
                            insn = makeInstruction(state, x86_repe_cmpsd);
                            goto done;
                        case x86_repeat_repne:
                            insn = makeInstruction(state, x86_repne_cmpsd);
                            goto done;
                        default:
                            throw ExceptionX86("bad repeat prefix for cmpsd", state);
                    }
                case x86_insnsize_64:
                    switch (state.repeatPrefix) {
                        case x86_repeat_none:
                            insn = makeInstruction(state, x86_cmpsq);
                            goto done;
                        case x86_repeat_repe:
                            insn = makeInstruction(state, x86_repe_cmpsq);
                            goto done;
                        case x86_repeat_repne:
                            insn = makeInstruction(state, x86_repne_cmpsq);
                            goto done;
                        default:
                            throw ExceptionX86("bad repeat prefix for cmpsq", state);
                    }
                default: ASSERT_not_reachable("invalid effective operand size: " +
                                              stringifyBinaryAnalysisX86InstructionSize(effectiveOperandSize(state)));
            }
        }
        case 0xA8: {
            SgAsmExpression* imm = getImmByte(state);
            insn = makeInstruction(state, x86_test, makeRegister(state, 0, rmLegacyByte), imm);
            goto done;
        }
        case 0xA9: {
            SgAsmExpression* imm = getImmIzAsIv(state);
            insn = makeInstruction(state, x86_test, makeRegisterEffective(state, 0), imm);
            goto done;
        }
        case 0xAA: {
            switch (state.repeatPrefix) {
                case x86_repeat_none:
                    insn = makeInstruction(state, x86_stosb);
                    goto done;
                case x86_repeat_repe:
                    insn = makeInstruction(state, x86_rep_stosb);
                    goto done;
                default:
                    throw ExceptionX86("bad repeat prefix for stosb", state);
            }
        }
        case 0xAB: {
            switch (effectiveOperandSize(state)) {
                case x86_insnsize_16:
                    switch (state.repeatPrefix) {
                        case x86_repeat_none:
                            insn = makeInstruction(state, x86_stosw);
                            goto done;
                        case x86_repeat_repe:
                            insn = makeInstruction(state, x86_rep_stosw);
                            goto done;
                        default:
                            throw ExceptionX86("bad repeat prefix for stosw", state);
                    }
                case x86_insnsize_32:
                    switch (state.repeatPrefix) {
                        case x86_repeat_none:
                            insn = makeInstruction(state, x86_stosd);
                            goto done;
                        case x86_repeat_repe:
                            insn = makeInstruction(state, x86_rep_stosd);
                            goto done;
                        default:
                            throw ExceptionX86("bad repeat prefix for stosd", state);
                    }
                case x86_insnsize_64:
                    switch (state.repeatPrefix) {
                        case x86_repeat_none:
                            insn = makeInstruction(state, x86_stosq);
                            goto done;
                        case x86_repeat_repe:
                            insn = makeInstruction(state, x86_rep_stosq);
                            goto done;
                        default: throw ExceptionX86("bad repeat prefix for stosq", state);
                    }
                default:
                    ASSERT_not_reachable("invalid effective operand size: " +
                                         stringifyBinaryAnalysisX86InstructionSize(effectiveOperandSize(state)));
            }
        }
        case 0xAC: {
            switch (state.repeatPrefix) {
                case x86_repeat_none:
                    insn = makeInstruction(state, x86_lodsb);
                    goto done;
                case x86_repeat_repe:
                    insn = makeInstruction(state, x86_rep_lodsb);
                    goto done;
                default:
                    throw ExceptionX86("bad repeat prefix for lodsb", state);
            }
        }
        case 0xAD: {
            switch (effectiveOperandSize(state)) {
                case x86_insnsize_16:
                    switch (state.repeatPrefix) {
                        case x86_repeat_none:
                            insn = makeInstruction(state, x86_lodsw);
                            goto done;
                        case x86_repeat_repe:
                            insn = makeInstruction(state, x86_rep_lodsw);
                            goto done;
                        default:
                            throw ExceptionX86("bad repeat prefix for lodsw", state);
                    }
                case x86_insnsize_32:
                    switch (state.repeatPrefix) {
                        case x86_repeat_none:
                            insn = makeInstruction(state, x86_lodsd);
                            goto done;
                        case x86_repeat_repe:
                            insn = makeInstruction(state, x86_rep_lodsd);
                            goto done;
                        default:
                            throw ExceptionX86("bad repeat prefix for lodsd", state);
                    }
                case x86_insnsize_64:
                    switch (state.repeatPrefix) {
                        case x86_repeat_none:
                            insn = makeInstruction(state, x86_lodsq);
                            goto done;
                        case x86_repeat_repe:
                            insn = makeInstruction(state, x86_rep_lodsq);
                            goto done;
                        default:
                            throw ExceptionX86("bad repeat prefix for lodsq", state);
                    }
                default: ASSERT_not_reachable("invalid effective operand size: " +
                                              stringifyBinaryAnalysisX86InstructionSize(effectiveOperandSize(state)));
            }
        }
        case 0xAE: {
            switch (state.repeatPrefix) {
                case x86_repeat_none:
                    insn = makeInstruction(state, x86_scasb);
                    goto done;
                case x86_repeat_repe:
                    insn = makeInstruction(state, x86_repe_scasb);
                    goto done;
                case x86_repeat_repne:
                    insn = makeInstruction(state, x86_repne_scasb);
                    goto done;
                default:
                    throw ExceptionX86("bad repeat prefix for scasb", state);
            }
        }
        case 0xAF: {
            switch (effectiveOperandSize(state)) {
                case x86_insnsize_16:
                    switch (state.repeatPrefix) {
                        case x86_repeat_none:
                            insn = makeInstruction(state, x86_scasw);
                            goto done;
                        case x86_repeat_repe:
                            insn = makeInstruction(state, x86_repe_scasw);
                            goto done;
                        case x86_repeat_repne:
                            insn = makeInstruction(state, x86_repne_scasw);
                            goto done;
                        default:
                            throw ExceptionX86("bad repeat prefix for scasw", state);
                    }
                case x86_insnsize_32:
                    switch (state.repeatPrefix) {
                        case x86_repeat_none:
                            insn = makeInstruction(state, x86_scasd);
                            goto done;
                        case x86_repeat_repe:
                            insn = makeInstruction(state, x86_repe_scasd);
                            goto done;
                        case x86_repeat_repne:
                            insn = makeInstruction(state, x86_repne_scasd);
                            goto done;
                        default:
                            throw ExceptionX86("bad repeat prefix for scasd", state);
                    }
                case x86_insnsize_64:
                    switch (state.repeatPrefix) {
                        case x86_repeat_none:
                            insn = makeInstruction(state, x86_scasq);
                            goto done;
                        case x86_repeat_repe:
                            insn = makeInstruction(state, x86_repe_scasq);
                            goto done;
                        case x86_repeat_repne:
                            insn = makeInstruction(state, x86_repne_scasq);
                            goto done;
                        default:
                            throw ExceptionX86("bad repeat prefix for scasq", state);
                    }
                default: ASSERT_not_reachable("invalid effective operand size: " +
                                              stringifyBinaryAnalysisX86InstructionSize(effectiveOperandSize(state)));
            }
        }
        case 0xB0: {
            SgAsmExpression* imm = getImmByte(state);
            insn = makeInstruction(state, x86_mov, makeOperandRegisterByte(state, state.rexB, 0), imm);
            goto done;
        }
        case 0xB1: {
            SgAsmExpression* imm = getImmByte(state);
            insn = makeInstruction(state, x86_mov, makeOperandRegisterByte(state, state.rexB, 1), imm);
            goto done;
        }
        case 0xB2: {
            SgAsmExpression* imm = getImmByte(state);
            insn = makeInstruction(state, x86_mov, makeOperandRegisterByte(state, state.rexB, 2), imm);
            goto done;
        }
        case 0xB3: {
            SgAsmExpression* imm = getImmByte(state);
            insn = makeInstruction(state, x86_mov, makeOperandRegisterByte(state, state.rexB, 3), imm);
            goto done;
        }
        case 0xB4: {
            SgAsmExpression* imm = getImmByte(state);
            insn = makeInstruction(state, x86_mov, makeOperandRegisterByte(state, state.rexB, 4), imm);
            goto done;
        }
        case 0xB5: {
            SgAsmExpression* imm = getImmByte(state);
            insn = makeInstruction(state, x86_mov, makeOperandRegisterByte(state, state.rexB, 5), imm);
            goto done;
        }
        case 0xB6: {
            SgAsmExpression* imm = getImmByte(state);
            insn = makeInstruction(state, x86_mov, makeOperandRegisterByte(state, state.rexB, 6), imm);
            goto done;
        }
        case 0xB7: {
            SgAsmExpression* imm = getImmByte(state);
            insn = makeInstruction(state, x86_mov, makeOperandRegisterByte(state, state.rexB, 7), imm);
            goto done;
        }
        case 0xB8: {
            SgAsmExpression* imm = getImmIv(state);
            insn = makeInstruction(state, x86_mov, makeRegisterEffective(state, state.rexB, 0), imm);
            goto done;
        }
        case 0xB9: {
            SgAsmExpression* imm = getImmIv(state);
            insn = makeInstruction(state, x86_mov, makeRegisterEffective(state, state.rexB, 1), imm);
            goto done;
        }
        case 0xBA: {
            SgAsmExpression* imm = getImmIv(state);
            insn = makeInstruction(state, x86_mov, makeRegisterEffective(state, state.rexB, 2), imm);
            goto done;
        }
        case 0xBB: {
            SgAsmExpression* imm = getImmIv(state);
            insn = makeInstruction(state, x86_mov, makeRegisterEffective(state, state.rexB, 3), imm);
            goto done;
        }
        case 0xBC: {
            SgAsmExpression* imm = getImmIv(state);
            insn = makeInstruction(state, x86_mov, makeRegisterEffective(state, state.rexB, 4), imm);
            goto done;
        }
        case 0xBD: {
            SgAsmExpression* imm = getImmIv(state);
            insn = makeInstruction(state, x86_mov, makeRegisterEffective(state, state.rexB, 5), imm);
            goto done;
        }
        case 0xBE: {
            SgAsmExpression* imm = getImmIv(state);
            insn = makeInstruction(state, x86_mov, makeRegisterEffective(state, state.rexB, 6), imm);
            goto done;
        }
        case 0xBF: {
            SgAsmExpression* imm = getImmIv(state);
            insn = makeInstruction(state, x86_mov, makeRegisterEffective(state, state.rexB, 7), imm);
            goto done;
        }
        case 0xC0: {
            getModRegRM(state, rmReturnNull, rmLegacyByte, BYTET);
            SgAsmExpression* imm = getImmByte(state);
            insn = decodeGroup2(state, imm);
            goto done;
        }
        case 0xC1: {
            getModRegRM(state, rmReturnNull, effectiveOperandMode(state), effectiveOperandType(state));
            SgAsmExpression* imm = getImmByteAsIv(state);
            insn = decodeGroup2(state, imm);
            goto done;
        }
        case 0xC2: {
            state.isUnconditionalJump = true;
            SgAsmExpression* imm = getImmWord(state);
            insn = makeInstruction(state, x86_ret, imm);
            goto done;
        }
        case 0xC3: {
            state.isUnconditionalJump = true;
            insn = makeInstruction(state, x86_ret);
            goto done;
        }
        case 0xC4: {
            not64(state);
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            requireMemory(state);
            insn = makeInstruction(state, x86_les, state.reg, state.modrm);
            goto done;
        }
        case 0xC5: {
            not64(state);
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            requireMemory(state);
            insn = makeInstruction(state, x86_lds, state.reg, state.modrm);
            goto done;
        }
        case 0xC6: {
            getModRegRM(state, rmReturnNull, rmLegacyByte, BYTET);
            SgAsmExpression* imm = getImmByte(state);
            insn = decodeGroup11(state, imm);
            goto done;
        }
        case 0xC7: {
            getModRegRM(state, rmReturnNull, effectiveOperandMode(state), effectiveOperandType(state));
            SgAsmExpression* imm = getImmIzAsIv(state);
            insn = decodeGroup11(state, imm);
            goto done;
        }
        case 0xC8: {
            SgAsmExpression* immw = getImmWord(state);
            SgAsmExpression* imm = getImmByte(state);
            insn = makeInstruction(state, x86_enter, immw, imm);
            goto done;
        }
        case 0xC9: {
            insn = makeInstruction(state, x86_leave);
            goto done;
        }
        case 0xCA: {
            state.isUnconditionalJump = true;
            SgAsmExpression* imm = getImmWord(state);
            insn = makeInstruction(state, x86_retf, imm);
            goto done;
        }
        case 0xCB: {
            state.isUnconditionalJump = true;
            insn = makeInstruction(state, x86_retf);
            goto done;
        }
        case 0xCC: {
            insn = makeInstruction(state, x86_int3);
            goto done;
        }
        case 0xCD: {
            SgAsmExpression* imm = getImmByte(state);
            insn = makeInstruction(state, x86_int, imm);
            goto done;
        }
        case 0xCE: {
            not64(state);
            insn = makeInstruction(state, x86_into);
            goto done;
        }
        case 0xCF: {
            state.isUnconditionalJump = true;
            insn = makeInstruction(state, x86_iret);
            goto done;
        }
        case 0xD0: {
            getModRegRM(state, rmReturnNull, rmLegacyByte, BYTET);
            insn = decodeGroup2(state, SageBuilderAsm::buildValueX86Byte(1));
            goto done;
        }
        case 0xD1: {
            getModRegRM(state, rmReturnNull, effectiveOperandMode(state), effectiveOperandType(state));
            insn = decodeGroup2(state, SageBuilderAsm::buildValueX86Byte(1));
            goto done;
        }
        case 0xD2: {
            getModRegRM(state, rmReturnNull, rmLegacyByte, BYTET);
            insn = decodeGroup2(state, makeRegister(state, 1, rmLegacyByte));
            goto done;
        }
        case 0xD3: {
            getModRegRM(state, rmReturnNull, effectiveOperandMode(state), effectiveOperandType(state));
            insn = decodeGroup2(state, makeRegister(state, 1, rmLegacyByte));
            goto done;
        }
        case 0xD4: {
            not64(state);
            SgAsmExpression* imm = getImmByte(state);
            insn = makeInstruction(state, x86_aam, imm);
            goto done;
        }
        case 0xD5: {
            not64(state);
            SgAsmExpression* imm = getImmByte(state);
            insn = makeInstruction(state, x86_aad, imm);
            goto done;
        }
        case 0xD6: {
            not64(state);
            insn = makeInstruction(state, x86_salc);
            goto done;
        }
        case 0xD7: {
            insn = makeInstruction(state, x86_xlatb);
            goto done;
        }
        case 0xD8: {
            insn = decodeX87InstructionD8(state);
            goto done;
        }
        case 0xD9: {
            insn = decodeX87InstructionD9(state);
            goto done;
        }
        case 0xDA: {
            insn = decodeX87InstructionDA(state);
            goto done;
        }
        case 0xDB: {
            insn = decodeX87InstructionDB(state);
            goto done;
        }
        case 0xDC: {
            insn = decodeX87InstructionDC(state);
            goto done;
        }
        case 0xDD: {
            insn = decodeX87InstructionDD(state);
            goto done;
        }
        case 0xDE: {
            insn = decodeX87InstructionDE(state);
            goto done;
        }
        case 0xDF: {
            insn = decodeX87InstructionDF(state);
            goto done;
        }
        case 0xE0: {
            SgAsmExpression* imm = getImmJb(state);
            insn = makeInstruction(state, x86_loopnz, imm);
            goto done;
        }
        case 0xE1: {
            SgAsmExpression* imm = getImmJb(state);
            insn = makeInstruction(state, x86_loopz, imm);
            goto done;
        }
        case 0xE2: {
            SgAsmExpression* imm = getImmJb(state);
            insn = makeInstruction(state, x86_loop, imm);
            goto done;
        }
        case 0xE3: {
            SgAsmExpression* imm = getImmJb(state);
            state.branchPredictionEnabled = true;
            switch (effectiveOperandSize(state)) {
                case x86_insnsize_16:
                    insn = makeInstruction(state, x86_jcxz, imm);
                    goto done;
                case x86_insnsize_32:
                    insn = makeInstruction(state, x86_jecxz, imm);
                    goto done;
                case x86_insnsize_64:
                    insn = makeInstruction(state, x86_jrcxz, imm);
                    goto done;
                default:
                    ASSERT_not_reachable("invalid effective operand size: " +
                                         stringifyBinaryAnalysisX86InstructionSize(effectiveOperandSize(state)));
            }
        }
        case 0xE4: {
            SgAsmExpression* imm = getImmByte(state);
            insn = makeInstruction(state, x86_in, makeRegister(state, 0, rmLegacyByte), imm);
            goto done;
        }
        case 0xE5: {
            SgAsmExpression* imm = getImmByte(state);
            insn = makeInstruction(state, x86_in, makeRegisterEffective(state, 0), imm);
            goto done;
        }
        case 0xE6: {
            SgAsmExpression* imm = getImmByte(state);
            insn = makeInstruction(state, x86_out, imm, makeRegister(state, 0, rmLegacyByte));
            goto done;
        }
        case 0xE7: {
            SgAsmExpression* imm = getImmByte(state);
            insn = makeInstruction(state, x86_out, imm, makeRegisterEffective(state, 0));
            goto done;
        }
        case 0xE8: {
            SgAsmExpression* imm = getImmJz(state);
            insn = makeInstruction(state, x86_call, imm);
            goto done;
        }
        case 0xE9: {
            SgAsmExpression* imm = getImmJz(state);
            insn = makeInstruction(state, x86_jmp, imm);
            state.isUnconditionalJump = true;
            goto done;
        }
        case 0xEA: {
            not64(state);
            SgAsmExpression* addr = getImmForAddr(state);
            SgAsmExpression* seg = getImmWord(state);
            insn = makeInstruction(state, x86_farjmp, seg, addr);
            state.isUnconditionalJump = true;
            goto done;
        }
        case 0xEB: {
            SgAsmExpression* imm = getImmJb(state);
            insn = makeInstruction(state, x86_jmp, imm);
            state.isUnconditionalJump = true;
            goto done;
        }
        case 0xEC: {
            insn = makeInstruction(state, x86_in, makeRegister(state, 0, rmLegacyByte), makeRegister(state, 2, rmWord));
            goto done;
        }
        case 0xED: {
            insn = makeInstruction(state, x86_in, makeRegisterEffective(state, 0), makeRegister(state, 2, rmWord));
            goto done;
        }
        case 0xEE: {
            insn = makeInstruction(state, x86_out, makeRegister(state, 2, rmWord), makeRegister(state, 0, rmLegacyByte));
            goto done;
        }
        case 0xEF: {
            insn = makeInstruction(state, x86_out, makeRegister(state, 2, rmWord), makeRegisterEffective(state, 0));
            goto done;
        }
        case 0xF0: {
            state.lock = true;
            insn = disassemble(state);
            goto done;
        }
        case 0xF1: {
            insn = makeInstruction(state, x86_int1);
            goto done;
        }
        case 0xF2: {
            state.repeatPrefix = x86_repeat_repne;
            insn = disassemble(state);
            goto done;
        }
        case 0xF3: {
            state.repeatPrefix = x86_repeat_repe;
            insn = disassemble(state);
            goto done;
        }
        case 0xF4: {
            insn = makeInstruction(state, x86_hlt);
            state.isUnconditionalJump = true;
            goto done;
        }
        case 0xF5: {
            insn = makeInstruction(state, x86_cmc);
            goto done;
        }
        case 0xF6: {
            getModRegRM(state, rmReturnNull, rmLegacyByte, BYTET);
            SgAsmExpression* immMaybe = NULL;
            if (state.regField <= 1) {
                SgAsmExpression* imm = getImmByteAsIv(state);
                immMaybe = imm;
            }
            insn = decodeGroup3(state, immMaybe);
            goto done;
        }
        case 0xF7: {
            getModRegRM(state, rmReturnNull, effectiveOperandMode(state), effectiveOperandType(state));
            SgAsmExpression* immMaybe = NULL;
            if (state.regField <= 1) {
                SgAsmExpression* imm = getImmIzAsIv(state);
                immMaybe = imm;
            }
            insn = decodeGroup3(state, immMaybe);
            goto done;
        }
        case 0xF8: {
            insn = makeInstruction(state, x86_clc);
            goto done;
        }
        case 0xF9: {
            insn = makeInstruction(state, x86_stc);
            goto done;
        }
        case 0xFA: {
            insn = makeInstruction(state, x86_cli);
            goto done;
        }
        case 0xFB: {
            insn = makeInstruction(state, x86_sti);
            goto done;
        }
        case 0xFC: {
            insn = makeInstruction(state, x86_cld);
            goto done;
        }
        case 0xFD: {
            insn = makeInstruction(state, x86_std);
            goto done;
        }
        case 0xFE: {
            getModRegRM(state, rmReturnNull, rmLegacyByte, BYTET);
            insn = decodeGroup4(state);
            goto done;
        }
        case 0xFF: {
            getModRegRM(state, rmReturnNull, rmReturnNull, NULL);
            if (state.regField >= 2 && state.regField <= 6) {
                state.sizeMustBe64Bit = true;
            }
            fillInModRM(state, effectiveOperandMode(state), effectiveOperandType(state));
            insn = decodeGroup5(state);
            goto done;
        }
        default: ASSERT_not_reachable("should not get here");
    }
done:
    ASSERT_not_null(insn);
    return insn;
}

SgAsmX86Instruction *
X86::decodeOpcode0F(State &state) const
{
    uint8_t opcode = getByte(state);
    switch (opcode) {
        case 0x00: {
            getModRegRM(state, rmReturnNull, rmWord, WORDT);
            return decodeGroup6(state);
        }
        case 0x01:
            return decodeGroup7(state);
        case 0x02: {
            getModRegRM(state, rmWord, rmWord, WORDT);
            return makeInstruction(state, x86_lar, state.reg, state.modrm);
        }
        case 0x03: {
            getModRegRM(state, rmWord, rmWord, WORDT);
            return makeInstruction(state, x86_lsl, state.reg, state.modrm);
        }
        case 0x04:
            throw ExceptionX86("bad opcode 0x0f04", state);
        case 0x05:
            return makeInstruction(state, x86_syscall);
        case 0x06:
            return makeInstruction(state, x86_clts);
        case 0x07:
            return makeInstruction(state, x86_sysret);
        case 0x08:
            return makeInstruction(state, x86_invd);
        case 0x09:
            return makeInstruction(state, x86_wbinvd);
        case 0x0A:
            throw ExceptionX86("bad opcode 0x0f0a", state);
        case 0x0B:
            return makeInstruction(state, x86_ud2);
        case 0x0C:
            throw ExceptionX86("bad opcode 0x0f0c", state);
        case 0x0D:
            return decodeGroupP(state);
        case 0x0E:
            return makeInstruction(state, x86_femms);
        case 0x0F: {
            /* 3DNow! (AMD Specific) */
            getModRegRM(state, rmReturnNull, rmReturnNull, NULL);
            uint8_t thirdOpcodeByte = getByte(state);
            char opcodestr[16];
            snprintf(opcodestr, sizeof(opcodestr), "0x0f0f%02x", thirdOpcodeByte);
            if (thirdOpcodeByte < 0x80) {
                /* Conversions */
                switch (thirdOpcodeByte) {
                    case 0x0C: {
                        fillInModRM(state, rmMM, V4WORDT);
                        state.reg = makeModrmRegister(state, rmMM, V2FLOATT);
                        return makeInstruction(state, x86_pi2fw, state.reg, state.modrm);
                    }
                    case 0x0D: {
                        fillInModRM(state, rmMM, V2DWORDT);
                        state.reg = makeModrmRegister(state, rmMM, V2FLOATT);
                        return makeInstruction(state, x86_pi2fd, state.reg, state.modrm);
                    }
                    case 0x1C: {
                        fillInModRM(state, rmMM, V2FLOATT);
                        state.reg = makeModrmRegister(state, rmMM, V4WORDT);
                        return makeInstruction(state, x86_pf2iw, state.reg, state.modrm);
                    }
                    case 0x1D: {
                        fillInModRM(state, rmMM, V2FLOATT);
                        state.reg = makeModrmRegister(state, rmMM, V2DWORDT);
                        return makeInstruction(state, x86_pf2id, state.reg, state.modrm);
                    }
                    default:
                        throw ExceptionX86(std::string("bad opcode ")+opcodestr, state);
                }
            } else if (thirdOpcodeByte < 0xB7) {
                /* Floating-point operations */
                fillInModRM(state, rmMM, V2FLOATT);
                state.reg = makeModrmRegister(state, rmMM, V2FLOATT);
                switch (thirdOpcodeByte) {
                    case 0x8A: return makeInstruction(state, x86_pfnacc, state.reg, state.modrm);
                    case 0x8E: return makeInstruction(state, x86_pfpnacc, state.reg, state.modrm);
                    case 0x90: return makeInstruction(state, x86_pfcmpge, state.reg, state.modrm);
                    case 0x94: return makeInstruction(state, x86_pfmin, state.reg, state.modrm);
                    case 0x96: return makeInstruction(state, x86_pfrcp, state.reg, state.modrm);
                    case 0x97: return makeInstruction(state, x86_pfrsqrt, state.reg, state.modrm);
                    case 0x9A: return makeInstruction(state, x86_pfsub, state.reg, state.modrm);
                    case 0x9E: return makeInstruction(state, x86_pfadd, state.reg, state.modrm);
                    case 0xA0: return makeInstruction(state, x86_pfcmpgt, state.reg, state.modrm);
                    case 0xA4: return makeInstruction(state, x86_pfmax, state.reg, state.modrm);
                    case 0xA6: return makeInstruction(state, x86_pfrcpit1, state.reg, state.modrm);
                    case 0xA7: return makeInstruction(state, x86_pfrsqit1, state.reg, state.modrm);
                    case 0xAA: return makeInstruction(state, x86_pfsubr, state.reg, state.modrm);
                    case 0xAE: return makeInstruction(state, x86_pfacc, state.reg, state.modrm);
                    case 0xB0: return makeInstruction(state, x86_pfcmpeq, state.reg, state.modrm);
                    case 0xB4: return makeInstruction(state, x86_pfmul, state.reg, state.modrm);
                    case 0xB6: return makeInstruction(state, x86_pfrcpit2, state.reg, state.modrm);
                    default: {
                        throw ExceptionX86(std::string("bad opcode ")+opcodestr, state);
                    }
                }
            } else {
                /* Extra integer operations */
                switch (thirdOpcodeByte) {
                    case 0xB7:
                        getModRegRM(state, rmMM, rmMM, V4WORDT);
                        return makeInstruction(state, x86_pmulhrw, state.reg, state.modrm);
                    case 0xBB:
                        getModRegRM(state, rmMM, rmMM, V2DWORDT);
                        return makeInstruction(state, x86_pswapd, state.reg, state.modrm);
                    case 0xBF:
                        getModRegRM(state, rmMM, rmMM, V8BYTET);
                        return makeInstruction(state, x86_pavgusb, state.reg, state.modrm);
                    default:
                        throw ExceptionX86(std::string("bad opcode ")+opcodestr, state);
                }
            }
        }
        case 0x10: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(state, x86_movups, state.reg, state.modrm);
                case mmF3:
                    getModRegRM(state, rmXMM, rmXMM, FLOATT);
                    return makeInstruction(state, x86_movss, state.reg, state.modrm);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V2DOUBLET);
                    return makeInstruction(state, x86_movupd, state.reg, state.modrm);
                case mmF2:
                    getModRegRM(state, rmXMM, rmXMM, DOUBLET);
                    return makeInstruction(state, x86_movsd_sse, state.reg, state.modrm);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x11: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(state, x86_movups, state.modrm, state.reg);
                case mmF3:
                    getModRegRM(state, rmXMM, rmXMM, FLOATT);
                    return makeInstruction(state, x86_movss, state.modrm, state.reg);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V2DOUBLET);
                    return makeInstruction(state, x86_movupd, state.modrm, state.reg);
                case mmF2:
                    getModRegRM(state, rmXMM, rmXMM, DOUBLET);
                    return makeInstruction(state, x86_movsd_sse, state.modrm, state.reg);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x12: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmXMM, rmXMM, V4FLOATT);
                    if (state.modeField == 3) {
                        return makeInstruction(state, x86_movhlps, state.reg, state.modrm);
                    } else {
                        return makeInstruction(state, x86_movlps, state.reg, state.modrm);
                    }
                case mmF3:
                    getModRegRM(state, rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(state, x86_movsldup, state.reg, state.modrm);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, DOUBLET);
                    requireMemory(state);
                    return makeInstruction(state, x86_movlpd, state.reg, state.modrm);
                case mmF2:
                    getModRegRM(state, rmXMM, rmXMM, DOUBLET);
                    return makeInstruction(state, x86_movddup, state.reg, state.modrm);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x13: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmXMM, rmXMM, V4FLOATT);
                    requireMemory(state);
                    return makeInstruction(state, x86_movlps, state.modrm, state.reg);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f13", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, DOUBLET);
                    requireMemory(state);
                    return makeInstruction(state, x86_movlpd, state.modrm, state.reg);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f13", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x14: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmXMM, rmXMM, V2FLOATT, V4FLOATT);
                    return makeInstruction(state, x86_unpcklps, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f14", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, DOUBLET, V2DOUBLET);
                    return makeInstruction(state, x86_unpcklpd, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f14", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x15: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmXMM, rmXMM, V2FLOATT, V4FLOATT);
                    return makeInstruction(state, x86_unpckhps, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f15", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, DOUBLET, V2DOUBLET);
                    return makeInstruction(state, x86_unpckhpd, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f15", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x16: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmXMM, rmXMM, V4FLOATT);
                    if (state.modeField == 3) {
                        return makeInstruction(state, x86_movlhps, state.reg, state.modrm);
                    } else {
                        return makeInstruction(state, x86_movhps, state.reg, state.modrm);
                    }
                case mmF3:
                    getModRegRM(state, rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(state, x86_movshdup, state.reg, state.modrm);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, DOUBLET);
                    requireMemory(state);
                    return makeInstruction(state, x86_movhpd, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f16", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x17: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmXMM, rmXMM, V4FLOATT);
                    requireMemory(state);
                    return makeInstruction(state, x86_movhps, state.modrm, state.reg);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f17", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, DOUBLET);
                    requireMemory(state);
                    return makeInstruction(state, x86_movhpd, state.modrm, state.reg);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f17", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x18:
            getModRegRM(state, rmLegacyByte, rmLegacyByte, BYTET);
            return decodeGroup16(state);
        case 0x19:
        case 0x1A:
        case 0x1B:
        case 0x1C:
        case 0x1D:
        case 0x1E:
            /* Undocumented no-ops */
            getModRegRM(state, rmReturnNull, rmReturnNull, NULL);
            return makeInstruction(state, x86_nop);
        case 0x1F:
            /* Documented no-op */
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            return makeInstruction(state, x86_nop, state.modrm);

        case 0x20:
            /* BUG: The mode and type fields should forced to the current processor number of bits instead of the size
             *      determied by the operand size flag. See documentation for move  to control register ("lock mov cr0, *").
             *      This may be an AMD specific issue, but the  operand size issues is a bug everywhere. */
            getModRegRM(state, rmControl, effectiveOperandMode(state), effectiveOperandType(state));
            if (state.modeField == 3) {
                return makeInstruction(state, x86_mov, state.modrm, state.reg);
            } else {
                throw ExceptionX86("bad ModR/M value for 0x0f20", state);
            }
        case 0x21:
            getModRegRM(state, rmDebug, effectiveOperandMode(state), effectiveOperandType(state));
            if (state.modeField == 3) {
                return makeInstruction(state, x86_mov, state.modrm, state.reg);
            } else {
                throw ExceptionX86("bad ModR/M value for 0x0f21", state);
            }
        case 0x22:
            state.sizeMustBe64Bit = x86_insnsize_64 == insnSize;
            getModRegRM(state, rmControl, effectiveOperandMode(state), effectiveOperandType(state));
            if (state.modeField == 3) {
                return makeInstruction(state, x86_mov, state.reg, state.modrm);
            } else {
                throw ExceptionX86("bad ModR/M value for 0x0f22", state);
            }
        case 0x23:
            state.sizeMustBe64Bit = x86_insnsize_64 == insnSize;
            getModRegRM(state, rmDebug, effectiveOperandMode(state), effectiveOperandType(state));
            if (state.modeField == 3) {
                return makeInstruction(state, x86_mov, state.reg, state.modrm);
            } else {
                throw ExceptionX86("bad ModR/M value for 0x0f23", state);
            }
        case 0x24:
            /* Move from test register */
            throw ExceptionX86("bad opcode 0x0f24", state);
        case 0x25:
            throw ExceptionX86("bad opcode 0x0f25", state);
        case 0x26:
            /* Move to test register */
            throw ExceptionX86("bad opcode 0x0f26", state);
        case 0x27:
            throw ExceptionX86("bad opcode 0x0f27", state);
        case 0x28: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(state, x86_movaps, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f28", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V2DOUBLET);
                    return makeInstruction(state, x86_movapd, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f18", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x29: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(state, x86_movaps, state.modrm, state.reg);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f29", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V2DOUBLET);
                    return makeInstruction(state, x86_movapd, state.modrm, state.reg);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f19", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x2A: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmXMM, rmMM, V2DWORDT, V4FLOATT);
                    return makeInstruction(state, x86_cvtpi2ps, state.reg, state.modrm);
                case mmF3:
                    getModRegRM(state, rmXMM, effectiveOperandMode(state), effectiveOperandType(state), V4FLOATT);
                    return makeInstruction(state, x86_cvtsi2ss, state.reg, state.modrm);
                case mm66:
                    getModRegRM(state, rmXMM, rmMM, V2DWORDT, V2DOUBLET);
                    return makeInstruction(state, x86_cvtpi2pd, state.reg, state.modrm);
                case mmF2:
                    getModRegRM(state, rmXMM, effectiveOperandMode(state), effectiveOperandType(state), V2DOUBLET);
                    return makeInstruction(state, x86_cvtsi2sd, state.reg, state.modrm);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x2B: {
            requireMemory(state);
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(state, x86_movntps, state.modrm, state.reg);
                case mmF3:
                    getModRegRM(state, rmXMM, rmXMM, FLOATT);
                    return makeInstruction(state, x86_movntss, state.modrm, state.reg);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V2DOUBLET);
                    return makeInstruction(state, x86_movntpd, state.modrm, state.reg);
                case mmF2:
                    getModRegRM(state, rmXMM, rmXMM, DOUBLET);
                    return makeInstruction(state, x86_movntsd, state.modrm, state.reg);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x2C: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmXMM, V4FLOATT, V2DWORDT);
                    return makeInstruction(state, x86_cvttps2pi, state.reg, state.modrm);
                case mmF3:
                    getModRegRM(state, effectiveOperandMode(state), rmXMM, FLOATT, effectiveOperandType(state));
                    return makeInstruction(state, x86_cvttss2si, state.reg, state.modrm);
                case mm66:
                    getModRegRM(state, rmMM, rmXMM, V2DOUBLET, V2DWORDT);
                    return makeInstruction(state, x86_cvttpd2pi, state.reg, state.modrm);
                case mmF2:
                    getModRegRM(state, effectiveOperandMode(state), rmXMM, DOUBLET, effectiveOperandType(state));
                    return makeInstruction(state, x86_cvttsd2si, state.reg, state.modrm);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x2D: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmXMM, V4FLOATT, V2DWORDT);
                    return makeInstruction(state, x86_cvtps2pi, state.reg, state.modrm);
                case mmF3:
                    getModRegRM(state, effectiveOperandMode(state), rmXMM, V4FLOATT, effectiveOperandType(state));
                    return makeInstruction(state, x86_cvtss2si, state.reg, state.modrm);
                case mm66:
                    getModRegRM(state, rmMM, rmXMM, V2DOUBLET, V2DWORDT);
                    return makeInstruction(state, x86_cvtpd2pi, state.reg, state.modrm);
                case mmF2:
                    getModRegRM(state, effectiveOperandMode(state), rmXMM, V2DOUBLET, effectiveOperandType(state));
                    return makeInstruction(state, x86_cvtsd2si, state.reg, state.modrm);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x2E: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmXMM, rmXMM, FLOATT);
                    return makeInstruction(state, x86_ucomiss, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f2e", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, DOUBLET);
                    return makeInstruction(state, x86_ucomisd, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f2e", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x2F: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmXMM, rmXMM, FLOATT);
                    return makeInstruction(state, x86_comiss, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f2f", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, DOUBLET);
                    return makeInstruction(state, x86_comisd, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f2f", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x30:
            return makeInstruction(state, x86_wrmsr);
        case 0x31:
            return makeInstruction(state, x86_rdtsc);
        case 0x32:
            return makeInstruction(state, x86_rdmsr);
        case 0x33:
            return makeInstruction(state, x86_rdpmc);
        case 0x34:
            not64(state);
            return makeInstruction(state, x86_sysenter);
        case 0x35:
            not64(state);
            return makeInstruction(state, x86_sysexit);
        case 0x36:
            throw ExceptionX86("bad opcode 0x0f36", state);
        case 0x37:
            return makeInstruction(state, x86_getsec);
        case 0x38:
            decodeOpcode0F38(state); /*SSSE3*/
        case 0x39:
            throw ExceptionX86("bad opcode 0x0f39", state);
        case 0x3A: {
            /* more SSE3? should this be in a decodeOpcode0F3A() instead? */
            uint8_t thirdOpcodeByte = getByte(state);
            switch (thirdOpcodeByte) {
                case 0x0F: { /* palignr */
                    SgAsmExpression* shiftAmount;
                    switch (mmPrefix(state)) {
                        /* Note that getModRegRM sets the states state.reg and modrm. Also, standard prefixed used in the manual,
                         * "mm" refers to "mmx" registers and "xmm" refers to "sse" registers. */
                        case mmNone:
                            getModRegRM(state, rmMM, rmMM, QWORDT);
                            shiftAmount = getImmByte(state);
                            return makeInstruction(state, x86_palignr, state.reg, state.modrm, shiftAmount);
                        case mmF3:
                            throw ExceptionX86("bad mm prefix F3 for opcode 0x0f3a0f", state);
                        case mm66:
                            getModRegRM(state, rmXMM, rmXMM, DQWORDT);
                            shiftAmount = getImmByte(state);
                            return makeInstruction(state, x86_palignr, state.reg, state.modrm, shiftAmount);
                        case mmF2:
                            throw ExceptionX86("bad mm prefix F2 for opcode 0x0f3a0f", state);
                    }
                }
                default: {
                    char opcodestr[16];
                    snprintf(opcodestr, sizeof(opcodestr), "0x0f3a%02x", thirdOpcodeByte);
                    throw ExceptionX86(std::string("bad or unimplemented opcode ")+opcodestr, state);
                }
            }
        }
        case 0x3B:
            throw ExceptionX86("bad opcode 0x0f3b", state);
        case 0x3C:
            throw ExceptionX86("bad opcode 0x0f3c", state);
        case 0x3D:
            throw ExceptionX86("bad opcode 0x0f3d", state);
        case 0x3E:
            throw ExceptionX86("bad opcode 0x0f3e", state);
        case 0x3F:
            throw ExceptionX86("bad opcode 0x0f3f", state);
        case 0x40:
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            return makeInstruction(state, x86_cmovo, state.reg, state.modrm);
        case 0x41:
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            return makeInstruction(state, x86_cmovno, state.reg, state.modrm);
        case 0x42:
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            return makeInstruction(state, x86_cmovb, state.reg, state.modrm);
        case 0x43:
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            return makeInstruction(state, x86_cmovae, state.reg, state.modrm);
        case 0x44:
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            return makeInstruction(state, x86_cmove, state.reg, state.modrm);
        case 0x45:
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            return makeInstruction(state, x86_cmovne, state.reg, state.modrm);
        case 0x46:
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            return makeInstruction(state, x86_cmovbe, state.reg, state.modrm);
        case 0x47:
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            return makeInstruction(state, x86_cmova, state.reg, state.modrm);
        case 0x48:
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            return makeInstruction(state, x86_cmovs, state.reg, state.modrm);
        case 0x49:
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            return makeInstruction(state, x86_cmovns, state.reg, state.modrm);
        case 0x4A:
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            return makeInstruction(state, x86_cmovpe, state.reg, state.modrm);
        case 0x4B:
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            return makeInstruction(state, x86_cmovpo, state.reg, state.modrm);
        case 0x4C:
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            return makeInstruction(state, x86_cmovl, state.reg, state.modrm);
        case 0x4D:
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            return makeInstruction(state, x86_cmovge, state.reg, state.modrm);
        case 0x4E:
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            return makeInstruction(state, x86_cmovle, state.reg, state.modrm);
        case 0x4F:
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            return makeInstruction(state, x86_cmovg, state.reg, state.modrm);
        case 0x50: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmDWord, rmXMM, V4FLOATT);
                    if (state.modeField == 3) {
                        return makeInstruction(state, x86_movmskps, state.reg, state.modrm);
                    } else {
                        throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f50", state);
                    }
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f50", state);
                case mm66:
                    getModRegRM(state, rmDWord, rmXMM, V2DOUBLET);
                    if (state.modeField == 3) {
                        return makeInstruction(state, x86_movmskpd, state.reg, state.modrm);
                    } else {
                        throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f50", state);
                    }
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f50", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x51: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(state, x86_sqrtps, state.reg, state.modrm);
                case mmF3:
                    getModRegRM(state, rmXMM, rmXMM, FLOATT);
                    return makeInstruction(state, x86_sqrtss, state.reg, state.modrm);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V2DOUBLET);
                    return makeInstruction(state, x86_sqrtpd, state.reg, state.modrm);
                case mmF2:
                    getModRegRM(state, rmXMM, rmXMM, DOUBLET);
                    return makeInstruction(state, x86_sqrtsd, state.reg, state.modrm);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x52: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(state, x86_rsqrtps, state.reg, state.modrm);
                case mmF3:
                    getModRegRM(state, rmXMM, rmXMM, FLOATT);
                    return makeInstruction(state, x86_rsqrtss, state.reg, state.modrm);
                case mm66:
                    throw ExceptionX86("bad mm prefix 66 for opcode 0x0f52", state);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f52", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x53: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(state, x86_rcpps, state.reg, state.modrm);
                case mmF3:
                    getModRegRM(state, rmXMM, rmXMM, FLOATT);
                    return makeInstruction(state, x86_rcpss, state.reg, state.modrm);
                case mm66:
                    throw ExceptionX86("bad mm prefix 66 for opcode 0x0f53", state);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f53", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x54: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(state, x86_andps, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f54", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(state, x86_andpd, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f54", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x55: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(state, x86_andnps, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f55", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V2DOUBLET);
                    return makeInstruction(state, x86_andnpd, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f55", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x56: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(state, x86_orps, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f56", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V2DOUBLET);
                    return makeInstruction(state, x86_orpd, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f56", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x57: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(state, x86_xorps, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f57", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V2DOUBLET);
                    return makeInstruction(state, x86_xorpd, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f57", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x58: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(state, x86_addps, state.reg, state.modrm);
                case mmF3:
                    getModRegRM(state, rmXMM, rmXMM, FLOATT);
                    return makeInstruction(state, x86_addss, state.reg, state.modrm);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V2DOUBLET);
                    return makeInstruction(state, x86_addpd, state.reg, state.modrm);
                case mmF2:
                    getModRegRM(state, rmXMM, rmXMM, DOUBLET);
                    return makeInstruction(state, x86_addsd, state.reg, state.modrm);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x59: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(state, x86_mulps, state.reg, state.modrm);
                case mmF3:
                    getModRegRM(state, rmXMM, rmXMM, FLOATT);
                    return makeInstruction(state, x86_mulss, state.reg, state.modrm);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V2DOUBLET);
                    return makeInstruction(state, x86_mulpd, state.reg, state.modrm);
                case mmF2:
                    getModRegRM(state, rmXMM, rmXMM, DOUBLET);
                    return makeInstruction(state, x86_mulsd, state.reg, state.modrm);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x5A: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmXMM, rmXMM, V4FLOATT, V2DOUBLET);
                    return makeInstruction(state, x86_cvtps2pd, state.reg, state.modrm);
                case mmF3:
                    getModRegRM(state, rmXMM, rmXMM, FLOATT, V2DOUBLET);
                    return makeInstruction(state, x86_cvtss2sd, state.reg, state.modrm);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V2DOUBLET, V4FLOATT);
                    return makeInstruction(state, x86_cvtpd2ps, state.reg, state.modrm);
                case mmF2:
                    getModRegRM(state, rmXMM, rmXMM, DOUBLET, V4FLOATT);
                    return makeInstruction(state, x86_cvtsd2ss, state.reg, state.modrm);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x5B: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmXMM, rmXMM, V4DWORDT, V4FLOATT);
                    return makeInstruction(state, x86_cvtdq2ps, state.reg, state.modrm);
                case mmF3:
                    getModRegRM(state, rmXMM, rmXMM, V4FLOATT, V4DWORDT);
                    return makeInstruction(state, x86_cvttps2dq, state.reg, state.modrm);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V4FLOATT, V4DWORDT);
                    return makeInstruction(state, x86_cvtps2dq, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f5b", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x5C: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(state, x86_subps, state.reg, state.modrm);
                case mmF3:
                    getModRegRM(state, rmXMM, rmXMM, FLOATT);
                    return makeInstruction(state, x86_subss, state.reg, state.modrm);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V2DOUBLET);
                    return makeInstruction(state, x86_subpd, state.reg, state.modrm);
                case mmF2:
                    getModRegRM(state, rmXMM, rmXMM, DOUBLET);
                    return makeInstruction(state, x86_subsd, state.reg, state.modrm);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x5D: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(state, x86_minps, state.reg, state.modrm);
                case mmF3:
                    getModRegRM(state, rmXMM, rmXMM, FLOATT);
                    return makeInstruction(state, x86_minss, state.reg, state.modrm);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V2DOUBLET);
                    return makeInstruction(state, x86_minpd, state.reg, state.modrm);
                case mmF2:
                    getModRegRM(state, rmXMM, rmXMM, DOUBLET);
                    return makeInstruction(state, x86_minsd, state.reg, state.modrm);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x5E: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(state, x86_divps, state.reg, state.modrm);
                case mmF3:
                    getModRegRM(state, rmXMM, rmXMM, FLOATT);
                    return makeInstruction(state, x86_divss, state.reg, state.modrm);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V2DOUBLET);
                    return makeInstruction(state, x86_divpd, state.reg, state.modrm);
                case mmF2:
                    getModRegRM(state, rmXMM, rmXMM, DOUBLET);
                    return makeInstruction(state, x86_divsd, state.reg, state.modrm);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x5F: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(state, x86_maxps, state.reg, state.modrm);
                case mmF3:
                    getModRegRM(state, rmXMM, rmXMM, FLOATT);
                    return makeInstruction(state, x86_maxss, state.reg, state.modrm);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V2DOUBLET);
                    return makeInstruction(state, x86_maxpd, state.reg, state.modrm);
                case mmF2:
                    getModRegRM(state, rmXMM, rmXMM, DOUBLET);
                    return makeInstruction(state, x86_maxsd, state.reg, state.modrm);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x60: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V8BYTET, V4WORDT);
                    return makeInstruction(state, x86_punpcklbw, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f60", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V16BYTET, V8WORDT);
                    return makeInstruction(state, x86_punpcklbw, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f60", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x61: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V4WORDT, V2DWORDT);
                    return makeInstruction(state, x86_punpcklwd, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f61", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V8WORDT, V4DWORDT);
                    return makeInstruction(state, x86_punpcklwd, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f61", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x62: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V2DWORDT, QWORDT);
                    return makeInstruction(state, x86_punpckldq, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f62", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V4DWORDT, V2QWORDT);
                    return makeInstruction(state, x86_punpckldq, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f62", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x63: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V4WORDT, V8BYTET);
                    return makeInstruction(state, x86_packsswb, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f63", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V8WORDT, V16BYTET);
                    return makeInstruction(state, x86_packsswb, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f63", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x64: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V8BYTET);
                    return makeInstruction(state, x86_pcmpgtb, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f64", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V16BYTET);
                    return makeInstruction(state, x86_pcmpgtb, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f64", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x65: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V4WORDT);
                    return makeInstruction(state, x86_pcmpgtw, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f65", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V8WORDT);
                    return makeInstruction(state, x86_pcmpgtw, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f65", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x66: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V2DWORDT);
                    return makeInstruction(state, x86_pcmpgtd, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f66", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V4DWORDT);
                    return makeInstruction(state, x86_pcmpgtd, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f66", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x67: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V4WORDT, V8BYTET);
                    return makeInstruction(state, x86_packuswb, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f67", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V8WORDT, V16BYTET);
                    return makeInstruction(state, x86_packuswb, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f67", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x68: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V8BYTET, V4WORDT);
                    return makeInstruction(state, x86_punpckhbw, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f68", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V16BYTET, V8WORDT);
                    return makeInstruction(state, x86_punpckhbw, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f68", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x69: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V4WORDT, V2DWORDT);
                    return makeInstruction(state, x86_punpckhwd, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f69", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V8WORDT, V4DWORDT);
                    return makeInstruction(state, x86_punpckhwd, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f69", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x6A: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V2DWORDT, QWORDT);
                    return makeInstruction(state, x86_punpckhdq, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f6a", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V4DWORDT, V2QWORDT);
                    return makeInstruction(state, x86_punpckhdq, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f6a", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x6B: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V2DWORDT, V4WORDT);
                    return makeInstruction(state, x86_packssdw, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f6b", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V4DWORDT, V8WORDT);
                    return makeInstruction(state, x86_packssdw, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f6b", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x6C: {
            switch (mmPrefix(state)) {
                case mmNone:
                    throw ExceptionX86("bad mm prefix None for opcode 0x0f6c", state);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f6c", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V2QWORDT, DQWORDT);
                    return makeInstruction(state, x86_punpcklqdq, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f6c", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x6D: {
            switch (mmPrefix(state)) {
                case mmNone:
                    throw ExceptionX86("bad mm prefix None for opcode 0x0f6d", state);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f6d", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V2QWORDT, DQWORDT);
                    return makeInstruction(state, x86_punpckhqdq, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f6d", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x6E: {
            switch (mmPrefix(state)) {
                case mmNone:
                    if (effectiveOperandSize(state)==x86_insnsize_64) {
                        getModRegRM(state, rmMM, effectiveOperandMode(state), effectiveOperandType(state), QWORDT);
                        return makeInstruction(state, x86_movq, state.reg, state.modrm);
                    } else {
                        getModRegRM(state, rmMM, effectiveOperandMode(state), effectiveOperandType(state), V2DWORDT);
                        return makeInstruction(state, x86_movd, state.reg, state.modrm);
                    }
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f6e", state);
                case mm66:
                    /* 0x66 is part of the opcode rather than an operand size override and used to distinguish between mm and
                     * xmm registers. The operands are 32 bits unless the REX.W bit is set, in which case they're 64 bits. */
                    state.operandSizeOverride = false;
                    if (effectiveOperandSize(state)==x86_insnsize_64) {
                        getModRegRM(state, rmXMM, effectiveOperandMode(state), effectiveOperandType(state), V2QWORDT);
                        return makeInstruction(state, x86_movq, state.reg, state.modrm);
                    } else {
                        getModRegRM(state, rmXMM, effectiveOperandMode(state), effectiveOperandType(state), V4DWORDT);
                        return makeInstruction(state, x86_movd, state.reg, state.modrm);
                    }
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f6e", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x6F: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, QWORDT);
                    return makeInstruction(state, x86_movq, state.reg, state.modrm);
                case mmF3:
                    getModRegRM(state, rmXMM, rmXMM, DQWORDT);
                    return makeInstruction(state, x86_movdqu, state.reg, state.modrm);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, DQWORDT);
                    return makeInstruction(state, x86_movdqa, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f6f", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x70: {
            switch (mmPrefix(state)) {
                case mmNone: {
                    getModRegRM(state, rmMM, rmMM, V4WORDT);
                    SgAsmExpression* shufConstant = getImmByte(state);
                    return makeInstruction(state, x86_pshufw, state.reg, state.modrm, shufConstant);
                }
                case mmF3: {
                    getModRegRM(state, rmXMM, rmXMM, V8WORDT);
                    SgAsmExpression* shufConstant = getImmByte(state);
                    return makeInstruction(state, x86_pshufhw, state.reg, state.modrm, shufConstant);
                }
                case mm66: {
                    getModRegRM(state, rmXMM, rmXMM, V4DWORDT);
                    SgAsmExpression* shufConstant = getImmByte(state);
                    return makeInstruction(state, x86_pshufd, state.reg, state.modrm, shufConstant);
                }
                case mmF2: {
                    getModRegRM(state, rmXMM, rmXMM, V8WORDT);
                    SgAsmExpression* shufConstant = getImmByte(state);
                    return makeInstruction(state, x86_pshuflw, state.reg, state.modrm, shufConstant);
                }
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x71: {
            /* Group 12 */
            switch (mmPrefix(state)) {
                case mmNone: {
                    getModRegRM(state, rmReturnNull, rmMM, V4WORDT);
                    if (state.modeField != 3)
                        throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f71", state);
                    SgAsmExpression* shiftAmount = getImmByte(state);
                    switch (state.regField) {
                        case 0: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f71", state);
                        case 1: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f71", state);
                        case 2: return makeInstruction(state, x86_psrlw, state.modrm, shiftAmount);
                        case 3: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f71", state);
                        case 4: return makeInstruction(state, x86_psraw, state.modrm, shiftAmount);
                        case 5: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f71", state);
                        case 6: return makeInstruction(state, x86_psllw, state.modrm, shiftAmount);
                        case 7: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f71", state);
                        default: ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(state.regField));
                    }
                }
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f71", state);
                case mm66: {
                    getModRegRM(state, rmReturnNull, rmXMM, V8WORDT);
                    if (state.modeField != 3)
                        throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f71", state);
                    SgAsmExpression* shiftAmount = getImmByte(state);
                    switch (state.regField) {
                        case 0: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f71", state);
                        case 1: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f71", state);
                        case 2: return makeInstruction(state, x86_psrlw, state.modrm, shiftAmount);
                        case 3: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f71", state);
                        case 4: return makeInstruction(state, x86_psraw, state.modrm, shiftAmount);
                        case 5: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f71", state);
                        case 6: return makeInstruction(state, x86_psllw, state.modrm, shiftAmount);
                        case 7: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f71", state);
                        default: ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(state.regField));
                    }
                }
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f71", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x72: {
            /* Group 13 */
            switch (mmPrefix(state)) {
                case mmNone: {
                    getModRegRM(state, rmReturnNull, rmMM, V2DWORDT);
                    if (state.modeField != 3)
                        throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f72", state);
                    SgAsmExpression* shiftAmount = getImmByte(state);
                    switch (state.regField) {
                        case 0: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f72", state);
                        case 1: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f72", state);
                        case 2: return makeInstruction(state, x86_psrld, state.modrm, shiftAmount);
                        case 3: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f72", state);
                        case 4: return makeInstruction(state, x86_psrad, state.modrm, shiftAmount);
                        case 5: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f72", state);
                        case 6: return makeInstruction(state, x86_pslld, state.modrm, shiftAmount);
                        case 7: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f72", state);
                        default: ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(state.regField));
                    }
                }
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f72", state);
                case mm66: {
                    getModRegRM(state, rmReturnNull, rmXMM, V4DWORDT);
                    if (state.modeField != 3)
                        throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f72", state);
                    SgAsmExpression* shiftAmount = getImmByte(state);
                    switch (state.regField) {
                        case 0: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f72", state);
                        case 1: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f72", state);
                        case 2: return makeInstruction(state, x86_psrld, state.modrm, shiftAmount);
                        case 3: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f72", state);
                        case 4: return makeInstruction(state, x86_psrad, state.modrm, shiftAmount);
                        case 5: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f72", state);
                        case 6: return makeInstruction(state, x86_pslld, state.modrm, shiftAmount);
                        case 7: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f72", state);
                        default: ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(state.regField));
                    }
                }
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f72", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x73: {
            /* Group 14 */
            switch (mmPrefix(state)) {
                case mmNone: {
                    getModRegRM(state, rmReturnNull, rmMM, QWORDT);
                    if (state.modeField != 3)
                        throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f73", state);
                    SgAsmExpression* shiftAmount = getImmByte(state);
                    switch (state.regField) {
                        case 0: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f73", state);
                        case 1: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f73", state);
                        case 2: return makeInstruction(state, x86_psrlq, state.modrm, shiftAmount);
                        case 3: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f73", state);
                        case 4: return makeInstruction(state, x86_psraq, state.modrm, shiftAmount);
                        case 5: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f73", state);
                        case 6: return makeInstruction(state, x86_psllq, state.modrm, shiftAmount);
                        case 7: throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f73", state);
                        default: ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(state.regField));
                    }
                }
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f73", state);
                case mm66: {
                    getModRegRM(state, rmReturnNull, rmXMM, V2QWORDT);
                    if (state.modeField != 3)
                        throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f73", state);
                    SgAsmExpression* shiftAmount = getImmByte(state);
                    switch (state.regField) {
                        case 0:
                            throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f73", state);
                        case 1:
                            throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f73", state);
                        case 2:
                            return makeInstruction(state, x86_psrlq, state.modrm, shiftAmount);
                        case 3:
                            isSgAsmRegisterReferenceExpression(state.modrm)->set_type(DQWORDT);
                            return makeInstruction(state, x86_psrldq, state.modrm, shiftAmount);
                        case 4:
                            throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f73", state);
                        case 5:
                            throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f73", state);
                        case 6:
                            return makeInstruction(state, x86_psllq, state.modrm, shiftAmount);
                        case 7:
                            isSgAsmRegisterReferenceExpression(state.modrm)->set_type(DQWORDT);
                            return makeInstruction(state, x86_pslldq, state.modrm, shiftAmount);
                        default:
                            ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(state.regField));
                    }
                }
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f73", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x74: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V8BYTET);
                    return makeInstruction(state, x86_pcmpeqb, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f74", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V16BYTET);
                    return makeInstruction(state, x86_pcmpeqb, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f74", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x75: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V4WORDT);
                    return makeInstruction(state, x86_pcmpeqw, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f75", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V8WORDT);
                    return makeInstruction(state, x86_pcmpeqw, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f75", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x76: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V2DWORDT);
                    return makeInstruction(state, x86_pcmpeqd, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f76", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V4DWORDT);
                    return makeInstruction(state, x86_pcmpeqd, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f76", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x77: {
            switch (mmPrefix(state)) {
                case mmNone:
                    return makeInstruction(state, x86_emms);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f77", state);
                case mm66:
                    throw ExceptionX86("bad mm prefix 66 for opcode 0x0f77", state);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f77", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x78: {
            switch (mmPrefix(state)) {
                case mmNone:
                    throw ExceptionX86("vmread not supported", state);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f78", state);
                case mm66: {
                    /* Group 17 */
                    getModRegRM(state, rmReturnNull, rmXMM, DQWORDT);
                    SgAsmExpression* imm1 = getImmByte(state);
                    SgAsmExpression* imm2 = getImmByte(state);
                    switch (state.regField) {
                        case 0:
                            if (state.modeField == 3) {
                                return makeInstruction(state, x86_extrq, state.modrm, imm1, imm2);
                            } else {
                                throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f78", state);
                            }
                        default:
                            throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f78", state);
                    }
                }
                case mmF2: {
                    getModRegRM(state, rmXMM, rmXMM, QWORDT, DQWORDT);
                    SgAsmExpression* imm1 = getImmByte(state);
                    SgAsmExpression* imm2 = getImmByte(state);
                    if (state.modeField == 3) {
                        return makeInstruction(state, x86_insertq, state.reg, state.modrm, imm1, imm2);
                    } else {
                        throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f78", state);
                    }
                }
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x79: {
            switch (mmPrefix(state)) {
                case mmNone:
                    throw ExceptionX86("vmwrite not supported", state);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f79", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, QWORDT, DQWORDT);
                    if (state.modeField == 3) {
                        return makeInstruction(state, x86_extrq, state.reg, state.modrm);
                    } else {
                        throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f79", state);
                    }
                case mmF2:
                    getModRegRM(state, rmXMM, rmXMM, DQWORDT);
                    if (state.modeField == 3) {
                        return makeInstruction(state, x86_insertq, state.reg, state.modrm);
                    } else {
                        throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0f79", state);
                    }
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x7A:
            throw ExceptionX86("bad opcode 0x0f7a", state);
        case 0x7B:
            throw ExceptionX86("bad opcode 0x0f7b", state);
        case 0x7C: {
            switch (mmPrefix(state)) {
                case mmNone:
                    throw ExceptionX86("bad mm prefix None for opcode 0x0f7c", state);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f7c", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V2DOUBLET);
                    return makeInstruction(state, x86_haddpd, state.reg, state.modrm);
                case mmF2:
                    getModRegRM(state, rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(state, x86_haddps, state.reg, state.modrm);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x7D: {
            switch (mmPrefix(state)) {
                case mmNone:
                    throw ExceptionX86("bad mm prefix None for opcode 0x0f7d", state);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f7d", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V2DOUBLET);
                    return makeInstruction(state, x86_hsubpd, state.reg, state.modrm);
                case mmF2:
                    getModRegRM(state, rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(state, x86_hsubps, state.reg, state.modrm);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x7E: {
            switch (mmPrefix(state)) {
                case mmNone:
                    if (effectiveOperandSize(state)==x86_insnsize_64) {
                        getModRegRM(state, rmMM, effectiveOperandMode(state), effectiveOperandType(state), QWORDT);
                        return makeInstruction(state, x86_movq, state.modrm, state.reg);
                    } else {
                        getModRegRM(state, rmMM, effectiveOperandMode(state), effectiveOperandType(state), V2DWORDT);
                        return makeInstruction(state, x86_movd, state.modrm, state.reg);
                    }
                case mmF3:
                    getModRegRM(state, rmXMM, rmXMM, V2QWORDT);
                    return makeInstruction(state, x86_movq, state.reg, state.modrm);
                case mm66:
                    /* 0x66 is part of the opcode rather than an operand size override and used to distinguish between mm and
                     * xmm registers. The operands are 32 bits unless the REX.W bit is set, in which case they're 64 bits. */
                    state.operandSizeOverride = false;
                    if (effectiveOperandSize(state)==x86_insnsize_64) {
                        getModRegRM(state, rmXMM, effectiveOperandMode(state), effectiveOperandType(state), V2QWORDT);
                        return makeInstruction(state, x86_movq, state.modrm, state.reg);
                    } else {
                        getModRegRM(state, rmXMM, effectiveOperandMode(state), effectiveOperandType(state), V4DWORDT);
                        return makeInstruction(state, x86_movd, state.modrm, state.reg);
                    }
                    
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f7e", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x7F: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, QWORDT);
                    return makeInstruction(state, x86_movq, state.modrm, state.reg);
                case mmF3:
                    getModRegRM(state, rmXMM, rmXMM, DQWORDT);
                    return makeInstruction(state, x86_movdqu, state.modrm, state.reg);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, DQWORDT);
                    return makeInstruction(state, x86_movdqa, state.modrm, state.reg);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f7f", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0x80: {
            SgAsmExpression* imm = getImmJz(state);
            state.branchPredictionEnabled = true;
            return makeInstruction(state, x86_jo, imm);
        }
        case 0x81: {
            SgAsmExpression* imm = getImmJz(state);
            state.branchPredictionEnabled = true;
            return makeInstruction(state, x86_jno, imm);
        }
        case 0x82: {
            SgAsmExpression* imm = getImmJz(state);
            state.branchPredictionEnabled = true;
            return makeInstruction(state, x86_jb, imm);
        }
        case 0x83: {
            SgAsmExpression* imm = getImmJz(state);
            state.branchPredictionEnabled = true;
            return makeInstruction(state, x86_jae, imm);
        }
        case 0x84: {
            SgAsmExpression* imm = getImmJz(state);
            state.branchPredictionEnabled = true;
            return makeInstruction(state, x86_je, imm);
        }
        case 0x85: {
            SgAsmExpression* imm = getImmJz(state);
            state.branchPredictionEnabled = true;
            return makeInstruction(state, x86_jne, imm);
        }
        case 0x86: {
            SgAsmExpression* imm = getImmJz(state);
            state.branchPredictionEnabled = true;
            return makeInstruction(state, x86_jbe, imm);
        }
        case 0x87: {
            SgAsmExpression* imm = getImmJz(state);
            state.branchPredictionEnabled = true;
            return makeInstruction(state, x86_ja, imm);
        }
        case 0x88: {
            SgAsmExpression* imm = getImmJz(state);
            state.branchPredictionEnabled = true;
            return makeInstruction(state, x86_js, imm);
        }
        case 0x89: {
            SgAsmExpression* imm = getImmJz(state);
            state.branchPredictionEnabled = true;
            return makeInstruction(state, x86_jns, imm);
        }
        case 0x8A: {
            SgAsmExpression* imm = getImmJz(state);
            state.branchPredictionEnabled = true;
            return makeInstruction(state, x86_jpe, imm);
        }
        case 0x8B: {
            SgAsmExpression* imm = getImmJz(state);
            state.branchPredictionEnabled = true;
            return makeInstruction(state, x86_jpo, imm);
        }
        case 0x8C: {
            SgAsmExpression* imm = getImmJz(state);
            state.branchPredictionEnabled = true;
            return makeInstruction(state, x86_jl, imm);
        }
        case 0x8D: {
            SgAsmExpression* imm = getImmJz(state);
            state.branchPredictionEnabled = true;
            return makeInstruction(state, x86_jge, imm);
        }
        case 0x8E: {
            SgAsmExpression* imm = getImmJz(state);
            state.branchPredictionEnabled = true;
            return makeInstruction(state, x86_jle, imm);
        }
        case 0x8F: {
            SgAsmExpression* imm = getImmJz(state);
            state.branchPredictionEnabled = true;
            return makeInstruction(state, x86_jg, imm);
        }
        case 0x90:
            getModRegRM(state, rmLegacyByte, rmLegacyByte, BYTET);
            return makeInstruction(state, x86_seto, state.modrm);
        case 0x91:
            getModRegRM(state, rmLegacyByte, rmLegacyByte, BYTET);
            return makeInstruction(state, x86_setno, state.modrm);
        case 0x92:
            getModRegRM(state, rmLegacyByte, rmLegacyByte, BYTET);
            return makeInstruction(state, x86_setb, state.modrm);
        case 0x93:
            getModRegRM(state, rmLegacyByte, rmLegacyByte, BYTET);
            return makeInstruction(state, x86_setae, state.modrm);
        case 0x94:
            getModRegRM(state, rmLegacyByte, rmLegacyByte, BYTET);
            return makeInstruction(state, x86_sete, state.modrm);
        case 0x95:
            getModRegRM(state, rmLegacyByte, rmLegacyByte, BYTET);
            return makeInstruction(state, x86_setne, state.modrm);
        case 0x96:
            getModRegRM(state, rmLegacyByte, rmLegacyByte, BYTET);
            return makeInstruction(state, x86_setbe, state.modrm);
        case 0x97:
            getModRegRM(state, rmLegacyByte, rmLegacyByte, BYTET);
            return makeInstruction(state, x86_seta, state.modrm);
        case 0x98:
            getModRegRM(state, rmLegacyByte, rmLegacyByte, BYTET);
            return makeInstruction(state, x86_sets, state.modrm);
        case 0x99:
            getModRegRM(state, rmLegacyByte, rmLegacyByte, BYTET);
            return makeInstruction(state, x86_setns, state.modrm);
        case 0x9A:
            getModRegRM(state, rmLegacyByte, rmLegacyByte, BYTET);
            return makeInstruction(state, x86_setpe, state.modrm);
        case 0x9B:
            getModRegRM(state, rmLegacyByte, rmLegacyByte, BYTET);
            return makeInstruction(state, x86_setpo, state.modrm);
        case 0x9C:
            getModRegRM(state, rmLegacyByte, rmLegacyByte, BYTET);
            return makeInstruction(state, x86_setl, state.modrm);
        case 0x9D:
            getModRegRM(state, rmLegacyByte, rmLegacyByte, BYTET);
            return makeInstruction(state, x86_setge, state.modrm);
        case 0x9E:
            getModRegRM(state, rmLegacyByte, rmLegacyByte, BYTET);
            return makeInstruction(state, x86_setle, state.modrm);
        case 0x9F:
            getModRegRM(state, rmLegacyByte, rmLegacyByte, BYTET);
            return makeInstruction(state, x86_setg, state.modrm);
        case 0xA0:
            return makeInstruction(state, x86_push, makeRegister(state, 4, rmSegment));
        case 0xA1:
            return makeInstruction(state, x86_pop, makeRegister(state, 4, rmSegment));
        case 0xA2:
            return makeInstruction(state, x86_cpuid);
        case 0xA3:
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            return makeInstruction(state, x86_bt, state.modrm, state.reg);
        case 0xA4:
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            return makeInstruction(state, x86_shld, state.modrm, state.reg, getImmByte(state));
        case 0xA5:
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            return makeInstruction(state, x86_shld, state.modrm, state.reg, makeRegister(state, 1, rmLegacyByte));
        case 0xA6:
            throw ExceptionX86("bad opcode 0x0fa6", state);
        case 0xA7:
            throw ExceptionX86("bad opcode 0x0fa7", state);
        case 0xA8:
            return makeInstruction(state, x86_push, makeRegister(state, 5, rmSegment));
        case 0xA9:
            return makeInstruction(state, x86_pop, makeRegister(state, 5, rmSegment));
        case 0xAA:
            return makeInstruction(state, x86_rsm);
        case 0xAB:
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            return makeInstruction(state, x86_btr, state.modrm, state.reg);
        case 0xAC:
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            return makeInstruction(state, x86_shrd, state.modrm, state.reg, getImmByte(state));
        case 0xAD:
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            return makeInstruction(state, x86_shrd, state.modrm, state.reg, makeRegister(state, 1, rmLegacyByte));
        case 0xAE:
            return decodeGroup15(state);
        case 0xAF:
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            return makeInstruction(state, x86_imul, state.reg, state.modrm);
        case 0xB0:
            getModRegRM(state, rmLegacyByte, rmLegacyByte, BYTET);
            return makeInstruction(state, x86_cmpxchg, state.modrm, state.reg);
        case 0xB1:
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            return makeInstruction(state, x86_cmpxchg, state.modrm, state.reg);
        case 0xB2:
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            requireMemory(state);
            return makeInstruction(state, x86_lss, state.reg, state.modrm);
        case 0xB3:
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            return makeInstruction(state, x86_bts, state.modrm, state.reg);
        case 0xB4:
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            requireMemory(state);
            return makeInstruction(state, x86_lfs, state.reg, state.modrm);
        case 0xB5:
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            requireMemory(state);
            return makeInstruction(state, x86_lgs, state.reg, state.modrm);
        case 0xB6:
            getModRegRM(state, effectiveOperandMode(state), rmLegacyByte, BYTET);
            return makeInstruction(state, x86_movzx, state.reg, state.modrm);
        case 0xB7:
            getModRegRM(state, effectiveOperandMode(state), rmWord, WORDT);
            return makeInstruction(state, x86_movzx, state.reg, state.modrm);
        case 0xB8: {
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            /* Here is an example of the existence of a prefix leading to two very different instructions. */
            switch (mmPrefix(state)) {
                case mmNone:
                    state.isUnconditionalJump = true;
                    return makeInstruction(state, x86_jmpe, state.modrm);
                case mmF3:
                    return makeInstruction(state, x86_popcnt, state.reg, state.modrm);
                default:
                    throw ExceptionX86("bad mm prefix for opcode 0x0fb8", state);
            }
        }
        case 0xB9:
            throw ExceptionX86("bad opcode 0x0fb9", state);
        case 0xBA:
            getModRegRM(state, rmReturnNull, effectiveOperandMode(state), effectiveOperandType(state));
            return decodeGroup8(state, getImmByte(state));
        case 0xBB:
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            return makeInstruction(state, x86_btc, state.modrm, state.reg);
        case 0xBC:
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            return makeInstruction(state, x86_bsf, state.reg, state.modrm);
        case 0xBD:
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            if (state.repeatPrefix == x86_repeat_repe) return makeInstruction(state, x86_lzcnt, state.reg, state.modrm);
            else return makeInstruction(state, x86_bsr, state.reg, state.modrm);
        case 0xBE:
            getModRegRM(state, effectiveOperandMode(state), rmLegacyByte, BYTET);
            return makeInstruction(state, x86_movsx, state.reg, state.modrm);
        case 0xBF:
            getModRegRM(state, effectiveOperandMode(state), rmWord, WORDT);
            return makeInstruction(state, x86_movsx, state.reg, state.modrm);
        case 0xC0:
            getModRegRM(state, rmLegacyByte, rmLegacyByte, BYTET);
            return makeInstruction(state, x86_xadd, state.modrm, state.reg);
        case 0xC1:
            getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
            return makeInstruction(state, x86_xadd, state.modrm, state.reg);
        case 0xC2: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(state, x86_cmpps, state.reg, state.modrm, getImmByte(state));
                case mmF3:
                    getModRegRM(state, rmXMM, rmXMM, FLOATT);
                    return makeInstruction(state, x86_cmpss, state.reg, state.modrm, getImmByte(state));
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V2DOUBLET);
                    return makeInstruction(state, x86_cmppd, state.reg, state.modrm, getImmByte(state));
                case mmF2:
                    getModRegRM(state, rmXMM, rmXMM, DOUBLET);
                    return makeInstruction(state, x86_cmpsd, state.reg, state.modrm, getImmByte(state));
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xC3: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, effectiveOperandMode(state), effectiveOperandMode(state), effectiveOperandType(state));
                    requireMemory(state);
                    return makeInstruction(state, x86_movnti, state.modrm, state.reg);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fc3", state);
                case mm66:
                    throw ExceptionX86("bad mm prefix 66 for opcode 0x0fc3", state);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fc3", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xC4: {
            switch (mmPrefix(state)) {
                case mmNone: {
                    getModRegRM(state, rmMM, rmWord, WORDT, QWORDT);
                    SgAsmExpression* imm = getImmByte(state);
                    return makeInstruction(state, x86_pinsrw, state.reg, state.modrm, imm);
                }
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fc4", state);
                case mm66: {
                    getModRegRM(state, rmXMM, rmWord, WORDT, DQWORDT);
                    SgAsmExpression* imm = getImmByte(state);
                    return makeInstruction(state, x86_pinsrw, state.reg, state.modrm, imm);
                }
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fc4", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xC5: {
            switch (mmPrefix(state)) {
                case mmNone: {
                    getModRegRM(state, rmDWord, rmMM, V4WORDT, DWORDT);
                    SgAsmExpression* imm = getImmByte(state);
                    if (state.modeField == 3) {
                        return makeInstruction(state, x86_pextrw, state.reg, state.modrm, imm);
                    } else {
                        throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0fc5", state);
                    }
                }
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fc5", state);
                case mm66: {
                    getModRegRM(state, rmDWord, rmXMM, V8WORDT, DWORDT);
                    SgAsmExpression* imm = getImmByte(state);
                    if (state.modeField == 3) {
                        return makeInstruction(state, x86_pextrw, state.reg, state.modrm, imm);
                    } else {
                        throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0fc5", state);
                    }
                }
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fc5", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xC6: {
            switch (mmPrefix(state)) {
                case mmNone: {
                    getModRegRM(state, rmXMM, rmXMM, V4FLOATT);
                    SgAsmExpression* shufConstant = getImmByte(state);
                    return makeInstruction(state, x86_shufps, state.reg, state.modrm, shufConstant);
                }
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fc6", state);
                case mm66: {
                    getModRegRM(state, rmXMM, rmXMM, V2DOUBLET);
                    SgAsmExpression* shufConstant = getImmByte(state);
                    return makeInstruction(state, x86_shufpd, state.reg, state.modrm, shufConstant);
                }
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fc6", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xC7: {
            /* Group 9 */
            getModRegRM(state, rmReturnNull, rmReturnNull, NULL);
            requireMemory(state);
            switch (state.regField) {
                case 1: {
                    if (effectiveOperandSize(state) == x86_insnsize_64) {
                        fillInModRM(state, rmReturnNull, DQWORDT);
                        return makeInstruction(state, x86_cmpxchg16b, state.modrm);
                    } else {
                        fillInModRM(state, rmReturnNull, QWORDT);
                        return makeInstruction(state, x86_cmpxchg8b, state.modrm);
                    }
                }
                case 6: {
                    fillInModRM(state, rmReturnNull, QWORDT);
                    switch (mmPrefix(state)) {
                        case mmNone: return makeInstruction(state, x86_vmptrld, state.modrm);
                        case mmF3:   return makeInstruction(state, x86_vmxon, state.modrm);
                        case mm66:   return makeInstruction(state, x86_vmclear, state.modrm);
                        default:     throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0fc7", state);
                    }
                }
                case 7: {
                    fillInModRM(state, rmReturnNull, QWORDT);
                    return makeInstruction(state, x86_vmptrst, state.modrm);
                }
                default:
                    throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0fc7", state);
            }
        }
        case 0xC8:
            return makeInstruction(state, x86_bswap, makeRegisterEffective(state, state.rexB, 0));
        case 0xC9:
            return makeInstruction(state, x86_bswap, makeRegisterEffective(state, state.rexB, 1));
        case 0xCA:
            return makeInstruction(state, x86_bswap, makeRegisterEffective(state, state.rexB, 2));
        case 0xCB:
            return makeInstruction(state, x86_bswap, makeRegisterEffective(state, state.rexB, 3));
        case 0xCC:
            return makeInstruction(state, x86_bswap, makeRegisterEffective(state, state.rexB, 4));
        case 0xCD:
            return makeInstruction(state, x86_bswap, makeRegisterEffective(state, state.rexB, 5));
        case 0xCE:
            return makeInstruction(state, x86_bswap, makeRegisterEffective(state, state.rexB, 6));
        case 0xCF:
            return makeInstruction(state, x86_bswap, makeRegisterEffective(state, state.rexB, 7));
        case 0xD0: {
            switch (mmPrefix(state)) {
                case mmNone:
                    throw ExceptionX86("bad mm prefix None for opcode 0x0fd0", state);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fd0", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V2DOUBLET);
                    return makeInstruction(state, x86_addsubpd, state.reg, state.modrm);
                case mmF2:
                    getModRegRM(state, rmXMM, rmXMM, V4FLOATT);
                    return makeInstruction(state, x86_addsubps, state.reg, state.modrm);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xD1: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V4WORDT);
                    return makeInstruction(state, x86_psrlw, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fd1", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V8WORDT);
                    return makeInstruction(state, x86_psrlw, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fd1", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xD2: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V2DWORDT);
                    return makeInstruction(state, x86_psrld, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fd2", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V4DWORDT);
                    return makeInstruction(state, x86_psrld, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fd2", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xD3: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, QWORDT);
                    return makeInstruction(state, x86_psrlq, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fd3", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V2QWORDT);
                    return makeInstruction(state, x86_psrlq, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fd3", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xD4: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, QWORDT);
                    return makeInstruction(state, x86_paddq, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fd4", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V2QWORDT);
                    return makeInstruction(state, x86_paddq, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fd4", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xD5: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V4WORDT);
                    return makeInstruction(state, x86_pmullw, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fd5", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V8WORDT);
                    return makeInstruction(state, x86_pmullw, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fd5", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xD6: {
            switch (mmPrefix(state)) {
                case mmNone:
                    throw ExceptionX86("bad mm prefix None for opcode 0x0fd6", state);
                case mmF3:
                    getModRegRM(state, rmMM, rmXMM, QWORDT, DQWORDT);
                    if (state.modeField == 3) {
                        return makeInstruction(state, x86_movq2dq, state.reg, state.modrm);
                    } else {
                        throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0fd6", state);
                    }
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, QWORDT);
                    return makeInstruction(state, x86_movq, state.modrm, state.reg);
                case mmF2:
                    getModRegRM(state, rmXMM, rmMM, DQWORDT, QWORDT);
                    if (state.modeField == 3) {
                        return makeInstruction(state, x86_movdq2q, state.reg, state.modrm);
                    } else {
                        throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0fd6", state);
                    }
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xD7: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmDWord, rmMM, V8BYTET, DWORDT);
                    if (state.modeField == 3) {
                        return makeInstruction(state, x86_pmovmskb, state.reg, state.modrm);
                    } else {
                        throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0fd7", state);
                    }
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fd7", state);
                case mm66:
                    getModRegRM(state, rmDWord, rmXMM, V16BYTET, DWORDT);
                    if (state.modeField == 3) {
                        return makeInstruction(state, x86_pmovmskb, state.reg, state.modrm);
                    } else {
                        throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0fd7", state);
                    }
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fd7", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xD8: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V8BYTET);
                    return makeInstruction(state, x86_psubusb, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fd8", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V16BYTET);
                    return makeInstruction(state, x86_psubusb, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fd8", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xD9: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V4WORDT);
                    return makeInstruction(state, x86_psubusw, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fd9", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V8WORDT);
                    return makeInstruction(state, x86_psubusw, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fd9", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xDA: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V8BYTET);
                    return makeInstruction(state, x86_pminub, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fda", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V16BYTET);
                    return makeInstruction(state, x86_pminub, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fda", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xDB: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, QWORDT);
                    return makeInstruction(state, x86_pand, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fdb", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, DQWORDT);
                    return makeInstruction(state, x86_pand, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fdb", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xDC: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V8BYTET);
                    return makeInstruction(state, x86_paddusb, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fdc", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V16BYTET);
                    return makeInstruction(state, x86_paddusb, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fdc", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xDD: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V4WORDT);
                    return makeInstruction(state, x86_paddusw, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fdd", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V8WORDT);
                    return makeInstruction(state, x86_paddusw, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fdd", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xDE: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V8BYTET);
                    return makeInstruction(state, x86_pmaxub, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fde", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V16BYTET);
                    return makeInstruction(state, x86_pmaxub, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fde", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xDF: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, QWORDT);
                    return makeInstruction(state, x86_pandn, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fdf", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, DQWORDT);
                    return makeInstruction(state, x86_pandn, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fdf", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xE0: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V8BYTET);
                    return makeInstruction(state, x86_pavgb, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fe0", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V16BYTET);
                    return makeInstruction(state, x86_pavgb, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fe0", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xE1: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V4WORDT);
                    return makeInstruction(state, x86_psraw, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fe1", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V8WORDT);
                    return makeInstruction(state, x86_psraw, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fe1", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xE2: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V2DWORDT);
                    return makeInstruction(state, x86_psrad, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fe2", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V4DWORDT);
                    return makeInstruction(state, x86_psrad, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fe2", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xE3: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V4WORDT);
                    return makeInstruction(state, x86_pavgw, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fe3", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V8WORDT);
                    return makeInstruction(state, x86_pavgw, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fe3", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xE4: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V4WORDT);
                    return makeInstruction(state, x86_pmulhuw, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fe4", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V8WORDT);
                    return makeInstruction(state, x86_pmulhuw, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fe4", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xE5: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V4WORDT);
                    return makeInstruction(state, x86_pmulhw, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fe5", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V8WORDT);
                    return makeInstruction(state, x86_pmulhw, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fe5", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xE6: {
            switch (mmPrefix(state)) {
                case mmNone:
                    throw ExceptionX86("bad mm prefix None for opcode 0x0fe6", state);
                case mmF3:
                    getModRegRM(state, rmXMM, rmXMM, V2QWORDT, V2DOUBLET);
                    return makeInstruction(state, x86_cvtdq2pd, state.reg, state.modrm);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V2DOUBLET, V2QWORDT);
                    return makeInstruction(state, x86_cvttpd2dq, state.reg, state.modrm);
                case mmF2:
                    getModRegRM(state, rmXMM, rmXMM, V2DOUBLET, V2QWORDT);
                    return makeInstruction(state, x86_cvtpd2dq, state.reg, state.modrm);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xE7: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, QWORDT);
                    requireMemory(state);
                    return makeInstruction(state, x86_movntq, state.modrm, state.reg);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fe7", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, DQWORDT);
                    requireMemory(state);
                    return makeInstruction(state, x86_movntdq, state.modrm, state.reg);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fe7", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xE8: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V8BYTET);
                    return makeInstruction(state, x86_psubsb, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fe8", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V16BYTET);
                    return makeInstruction(state, x86_psubsb, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fe8", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xE9: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V4WORDT);
                    return makeInstruction(state, x86_psubsw, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fe9", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V8WORDT);
                    return makeInstruction(state, x86_psubsw, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fe9", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xEA: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V4WORDT);
                    return makeInstruction(state, x86_pminsw, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fea", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V8WORDT);
                    return makeInstruction(state, x86_pminsw, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fea", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xEB: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, QWORDT);
                    return makeInstruction(state, x86_por, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0feb", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, DQWORDT);
                    return makeInstruction(state, x86_por, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0feb", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xEC: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V8BYTET);
                    return makeInstruction(state, x86_paddsb, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fec", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V16BYTET);
                    return makeInstruction(state, x86_paddsb, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fec", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xED: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V4WORDT);
                    return makeInstruction(state, x86_paddsw, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fed", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V8WORDT);
                    return makeInstruction(state, x86_paddsw, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fed", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xEE: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V4WORDT);
                    return makeInstruction(state, x86_pmaxsw, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fee", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V8WORDT);
                    return makeInstruction(state, x86_pmaxsw, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fee", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xEF: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, QWORDT);
                    return makeInstruction(state, x86_pxor, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0fef", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, DQWORDT);
                    return makeInstruction(state, x86_pxor, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0fef", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xF0: {
            switch (mmPrefix(state)) {
                case mmNone:
                    throw ExceptionX86("bad mm prefix None for opcode 0x0ff0", state);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0ff0", state);
                case mm66:
                    throw ExceptionX86("bad mm prefix 66 for opcode 0x0ff0", state);
                case mmF2:
                    getModRegRM(state, rmXMM, rmXMM, DQWORDT);
                    requireMemory(state);
                    return makeInstruction(state, x86_lddqu, state.reg, state.modrm);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xF1: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V4WORDT);
                    return makeInstruction(state, x86_psllw, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0ff1", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V8WORDT);
                    return makeInstruction(state, x86_psllw, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0ff1", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xF2: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V2DWORDT);
                    return makeInstruction(state, x86_pslld, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0ff2", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V4DWORDT);
                    return makeInstruction(state, x86_pslld, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0ff2", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xF3: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, QWORDT);
                    return makeInstruction(state, x86_psllq, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0ff3", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V2QWORDT);
                    return makeInstruction(state, x86_psllq, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0ff3", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xF4: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V2DWORDT, QWORDT);
                    return makeInstruction(state, x86_pmuludq, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0ff4", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V4DWORDT, V2QWORDT);
                    return makeInstruction(state, x86_pmuludq, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0ff4", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xF5: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V4WORDT, V2DWORDT);
                    return makeInstruction(state, x86_pmaddwd, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0ff5", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V8WORDT, V4DWORDT);
                    return makeInstruction(state, x86_pmaddwd, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0ff5", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xF6: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V8BYTET, V4WORDT);
                    return makeInstruction(state, x86_psadbw, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0ff6", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V16BYTET, V8WORDT);
                    return makeInstruction(state, x86_psadbw, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0ff6", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xF7: {
            /* FIXME: The MOVNTQ and MOVNTDQ are at 0F E7 instead. This should be MASKMOVDQU. See Intel documentation.
             *        [RPM 2009-07-02] */
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, QWORDT);
                    if (state.modeField == 3) {
                        return makeInstruction(state, x86_movntq, state.reg, state.modrm);
                    } else {
                        throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0ff7", state);
                    }
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0ff7", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, DQWORDT);
                    if (state.modeField == 3) {
                        return makeInstruction(state, x86_movntdq, state.reg, state.modrm);
                    } else {
                        throw ExceptionX86("bad combination of mm prefix and ModR/M for opcode 0x0ff7", state);
                    }
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0ff7", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xF8: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V8BYTET);
                    return makeInstruction(state, x86_psubb, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0ff8", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V16BYTET);
                    return makeInstruction(state, x86_psubb, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0ff8", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xF9: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V4WORDT);
                    return makeInstruction(state, x86_psubw, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0ff9", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V8WORDT);
                    return makeInstruction(state, x86_psubw, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0ff9", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xFA: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V2DWORDT);
                    return makeInstruction(state, x86_psubd, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0ffa", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V4DWORDT);
                    return makeInstruction(state, x86_psubd, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0ffa", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xFB: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, QWORDT);
                    return makeInstruction(state, x86_psubq, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0ffb", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V2QWORDT);
                    return makeInstruction(state, x86_psubq, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0ffb", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xFC: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V8BYTET);
                    return makeInstruction(state, x86_paddb, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0ffc", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V16BYTET);
                    return makeInstruction(state, x86_paddb, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0ffc", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xFD: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V4WORDT);
                    return makeInstruction(state, x86_paddw, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0ffd", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V8WORDT);
                    return makeInstruction(state, x86_paddw, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0ffd", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xFE: {
            switch (mmPrefix(state)) {
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V2DWORDT);
                    return makeInstruction(state, x86_paddd, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0ffe", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V4DWORDT);
                    return makeInstruction(state, x86_paddd, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0ffe", state);
            }
            ASSERT_not_reachable("mmPrefix");
        }
        case 0xFF:
            throw ExceptionX86("bad opcode 0x0fff", state);
    }

    // avoid MSCV warning by adding return stmt
    return NULL;
}

/* SSSE3 (opcode 0F38) */
SgAsmX86Instruction *
X86::decodeOpcode0F38(State &state) const
{
    // Get the third byte of the opcode (the first two were read by the caller (decodeOpcode0F())
    uint8_t opcode = getByte(state);
    switch (opcode) {
        case 0x00: {
            switch (mmPrefix(state)) {
                /* Note that getModRegRM sets the states reg and modrm. Also, standard prefixed used in the manual, "mm"
                 * refers to "mmx" registers and "xmm" refers to "sse" registers. */
                case mmNone:
                    getModRegRM(state, rmMM, rmMM, V2DWORDT);
                    return makeInstruction(state, x86_pshufb, state.reg, state.modrm);
                case mmF3:
                    throw ExceptionX86("bad mm prefix F3 for opcode 0x0f3800", state);
                case mm66:
                    getModRegRM(state, rmXMM, rmXMM, V4DWORDT);
                    return makeInstruction(state, x86_pshufb, state.reg, state.modrm);
                case mmF2:
                    throw ExceptionX86("bad mm prefix F2 for opcode 0x0f3800", state);
            }
        }
        default:
            throw ExceptionX86("bad SSE3 opcode", state);
    }
}

SgAsmX86Instruction *
X86::decodeX87InstructionD8(State &state) const
{
    getModRegRM(state, rmReturnNull, rmST, FLOATT);
    if (isSgAsmMemoryReferenceExpression(state.modrm)) {
        isSgAsmMemoryReferenceExpression(state.modrm)->set_type(FLOATT);
    }
    if (state.modregrmByte < 0xC0) { // Using memory
        switch (state.regField) {
            case 0: return makeInstruction(state, x86_fadd,  state.modrm);
            case 1: return makeInstruction(state, x86_fmul,  state.modrm);
            case 2: return makeInstruction(state, x86_fcom,  state.modrm);
            case 3: return makeInstruction(state, x86_fcomp, state.modrm);
            case 4: return makeInstruction(state, x86_fsub,  state.modrm);
            case 5: return makeInstruction(state, x86_fsubr, state.modrm);
            case 6: return makeInstruction(state, x86_fdiv,  state.modrm);
            case 7: return makeInstruction(state, x86_fdivr, state.modrm);
            default: {
                ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(state.regField));
                /* avoid MSCV warning by adding return stmt */
                return NULL;
            }
        }
    } else { // Two-operand register forms
        switch (state.regField) {
            case 0: return makeInstruction(state, x86_fadd, makeRegister(state, 0, rmST), state.modrm);
            case 1: return makeInstruction(state, x86_fmul, makeRegister(state, 0, rmST), state.modrm);
            case 2: return makeInstruction(state, x86_fcom, makeRegister(state, 0, rmST), state.modrm);
            case 3: return makeInstruction(state, x86_fcomp, makeRegister(state, 0, rmST), state.modrm);
            case 4: return makeInstruction(state, x86_fsub, makeRegister(state, 0, rmST), state.modrm);
            case 5: return makeInstruction(state, x86_fsubr, makeRegister(state, 0, rmST), state.modrm);
            case 6: return makeInstruction(state, x86_fdiv, makeRegister(state, 0, rmST), state.modrm);
            case 7: return makeInstruction(state, x86_fdivr, makeRegister(state, 0, rmST), state.modrm);
            default: {
                ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(state.regField));
                /* avoid MSCV warning by adding return stmt */
                return NULL;
            }
        }
    }
}

SgAsmX86Instruction *
X86::decodeX87InstructionD9(State &state) const
{
    getModRegRM(state, rmReturnNull, rmReturnNull, NULL);
    if (state.modeField < 3) {
        SgAsmMemoryReferenceExpression* mr = isSgAsmMemoryReferenceExpression(state.modrm);
        ASSERT_not_null(mr);
        switch (state.regField) {
            case 0:
                mr->set_type(FLOATT);
                return makeInstruction(state, x86_fld, state.modrm);
            case 1:
                throw ExceptionX86("bad ModR/M for x87 opcode 0xd9", state);
            case 2:
                mr->set_type(FLOATT);
                return makeInstruction(state, x86_fst, state.modrm);
            case 3:
                mr->set_type(FLOATT);
                return makeInstruction(state, x86_fstp, state.modrm);
            case 4:
                mr->set_type(BYTET);
                return makeInstruction(state, x86_fldenv, state.modrm);
            case 5:
                mr->set_type(WORDT);
                return makeInstruction(state, x86_fldcw, state.modrm);
            case 6:
                mr->set_type(BYTET);
                return makeInstruction(state, x86_fnstenv, state.modrm);
            case 7:
                mr->set_type(WORDT);
                return makeInstruction(state, x86_fnstcw, state.modrm);
            default:
                ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(state.regField));
      }
    } else if (state.regField == 0 || state.regField == 1) { // FLD and FXCH on registers
        state.modrm = makeModrmNormal(state, rmST, NULL);
        switch (state.regField) {
            case 0: return makeInstruction(state, x86_fld, state.modrm);
            case 1: return makeInstruction(state, x86_fxch, state.modrm);
            default: ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(state.regField));
        }
    } else {
        switch (state.modregrmByte) {
            case 0xD0: return makeInstruction(state, x86_fnop);
            case 0xE0: return makeInstruction(state, x86_fchs);
            case 0xE1: return makeInstruction(state, x86_fabs);
            case 0xE4: return makeInstruction(state, x86_ftst);
            case 0xE5: return makeInstruction(state, x86_fxam);
            case 0xE8: return makeInstruction(state, x86_fld1);
            case 0xE9: return makeInstruction(state, x86_fldl2t);
            case 0xEA: return makeInstruction(state, x86_fldl2e);
            case 0xEB: return makeInstruction(state, x86_fldpi);
            case 0xEC: return makeInstruction(state, x86_fldlg2);
            case 0xED: return makeInstruction(state, x86_fldln2);
            case 0xEE: return makeInstruction(state, x86_fldz);
            case 0xF0: return makeInstruction(state, x86_f2xm1);
            case 0xF1: return makeInstruction(state, x86_fyl2x);
            case 0xF2: return makeInstruction(state, x86_fptan);
            case 0xF3: return makeInstruction(state, x86_fpatan);
            case 0xF4: return makeInstruction(state, x86_fxtract);
            case 0xF5: return makeInstruction(state, x86_fprem1);
            case 0xF6: return makeInstruction(state, x86_fdecstp);
            case 0xF7: return makeInstruction(state, x86_fincstp);
            case 0xF8: return makeInstruction(state, x86_fprem);
            case 0xF9: return makeInstruction(state, x86_fyl2xp1);
            case 0xFA: return makeInstruction(state, x86_fsqrt);
            case 0xFB: return makeInstruction(state, x86_fsincos);
            case 0xFC: return makeInstruction(state, x86_frndint);
            case 0xFD: return makeInstruction(state, x86_fscale);
            case 0xFE: return makeInstruction(state, x86_fsin);
            case 0xFF: return makeInstruction(state, x86_fcos);
            default: throw ExceptionX86("bad ModR/M value for x87 opcode 0xd9", state);
        }
    }
    /* avoid MSCV warning by adding return stmt */
    return NULL;
}

SgAsmX86Instruction *
X86::decodeX87InstructionDA(State &state) const
{
    getModRegRM(state, rmReturnNull, rmReturnNull, DWORDT);
    if (state.modeField < 3) {
        switch (state.regField) {
            case 0: return makeInstruction(state, x86_fiadd, state.modrm);
            case 1: return makeInstruction(state, x86_fimul, state.modrm);
            case 2: return makeInstruction(state, x86_ficom, state.modrm);
            case 3: return makeInstruction(state, x86_ficomp, state.modrm);
            case 4: return makeInstruction(state, x86_fisub, state.modrm);
            case 5: return makeInstruction(state, x86_fisubr, state.modrm);
            case 6: return makeInstruction(state, x86_fidiv, state.modrm);
            case 7: return makeInstruction(state, x86_fidivr, state.modrm);
            default: ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(state.regField));
        }
    } else if (state.regField < 4) { // FCMOV{B,E,BE,U}
        state.modrm = makeModrmRegister(state, rmST);
        switch (state.regField) {
            case 0: return makeInstruction(state, x86_fcmovb, makeRegister(state, 0, rmST), state.modrm);
            case 1: return makeInstruction(state, x86_fcmove, makeRegister(state, 0, rmST), state.modrm);
            case 2: return makeInstruction(state, x86_fcmovbe, makeRegister(state, 0, rmST), state.modrm);
            case 3: return makeInstruction(state, x86_fcmovu, makeRegister(state, 0, rmST), state.modrm);
            default: ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(state.regField));
        }
    } else {
        switch (state.modregrmByte) {
            case 0xE9: return makeInstruction(state, x86_fucompp);
            default: throw ExceptionX86("bad ModR/M value for x87 opcode 0xda", state);
        }
    }
    /* avoid MSCV warning by adding return stmt */
    return NULL;
}

SgAsmX86Instruction *
X86::decodeX87InstructionDB(State &state) const
{
    getModRegRM(state, rmReturnNull, rmReturnNull, NULL);
    if (state.modeField < 3) {
        SgAsmMemoryReferenceExpression* mr = isSgAsmMemoryReferenceExpression(state.modrm);
        ASSERT_not_null(mr);
        if (state.regField <= 3) {
            mr->set_type(DWORDT);
        } else {
            mr->set_type(LDOUBLET);
        }
        switch (state.regField) {
            case 0: return makeInstruction(state, x86_fild, state.modrm);
            case 1: return makeInstruction(state, x86_fisttp, state.modrm);
            case 2: return makeInstruction(state, x86_fist, state.modrm);
            case 3: return makeInstruction(state, x86_fistp, state.modrm);
            case 4: throw ExceptionX86("bad ModR/M value for x87 opcode 0xdb", state);
            case 5: return makeInstruction(state, x86_fld, state.modrm);
            case 6: throw ExceptionX86("bad ModR/M value for x87 opcode 0xdb", state);
            case 7: return makeInstruction(state, x86_fstp, state.modrm);
            default: ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(state.regField));
        }
    } else if (state.regField <= 3 || state.regField == 5 || state.regField == 6) { // FCMOV{NB,NE,NBE,NU}, FUCOMI, FCOMI
        state.modrm = makeModrmNormal(state, rmST, NULL);
        switch (state.regField) {
            case 0: return makeInstruction(state, x86_fcmovnb, makeRegister(state, 0, rmST), state.modrm);
            case 1: return makeInstruction(state, x86_fcmovne, makeRegister(state, 0, rmST), state.modrm);
            case 2: return makeInstruction(state, x86_fcmovnbe, makeRegister(state, 0, rmST), state.modrm);
            case 3: return makeInstruction(state, x86_fcmovnu, makeRegister(state, 0, rmST), state.modrm);
            case 5: return makeInstruction(state, x86_fucomi, makeRegister(state, 0, rmST), state.modrm);
            case 6: return makeInstruction(state, x86_fcomi, makeRegister(state, 0, rmST), state.modrm);
            default: ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(state.regField));
      }
    } else {
        switch (state.modregrmByte) {
            case 0xE2: return makeInstruction(state, x86_fnclex);
            case 0xE3: return makeInstruction(state, x86_fninit);
            default: throw ExceptionX86("bad ModR/M value for x87 opcode 0xdb", state);
        }
    }
    /* avoid MSCV warning by adding return stmt */
    return NULL;
}

SgAsmX86Instruction *
X86::decodeX87InstructionDC(State &state) const
{
    getModRegRM(state, rmReturnNull, rmST, DOUBLET);
    if (state.modeField < 3) { // Using memory
        switch (state.regField & 7) {
            case 0: return makeInstruction(state, x86_fadd, state.modrm);
            case 1: return makeInstruction(state, x86_fmul, state.modrm);
            case 2: return makeInstruction(state, x86_fcom, state.modrm);
            case 3: return makeInstruction(state, x86_fcomp, state.modrm);
            case 4: return makeInstruction(state, x86_fsub, state.modrm);
            case 5: return makeInstruction(state, x86_fsubr, state.modrm);
            case 6: return makeInstruction(state, x86_fdiv, state.modrm);
            case 7: return makeInstruction(state, x86_fdivr, state.modrm);
            default: ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(state.regField));
        }
    } else { // Two-operand register forms
        switch (state.regField & 7) {
            case 0: return makeInstruction(state, x86_fadd,  state.modrm, makeRegister(state, 0, rmST));
            case 1: return makeInstruction(state, x86_fmul,  state.modrm, makeRegister(state, 0, rmST));
            case 2: throw ExceptionX86("bad ModR/M value for x87 opcode 0xdc", state);
            case 3: throw ExceptionX86("bad ModR/M value for x87 opcode 0xdc", state);
            case 4: return makeInstruction(state, x86_fsubr, state.modrm, makeRegister(state, 0, rmST));
            case 5: return makeInstruction(state, x86_fsub,  state.modrm, makeRegister(state, 0, rmST));
            case 6: return makeInstruction(state, x86_fdivr, state.modrm, makeRegister(state, 0, rmST));
            case 7: return makeInstruction(state, x86_fdiv,  state.modrm, makeRegister(state, 0, rmST));
            default: ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(state.regField));
        }
    }
    /* avoid MSCV warning by adding return stmt */
    return NULL;
}

SgAsmX86Instruction *
X86::decodeX87InstructionDD(State &state) const
{
    getModRegRM(state, rmReturnNull, rmST, NULL);
    if (state.modeField < 3) { // Using memory
        SgAsmMemoryReferenceExpression* mr = isSgAsmMemoryReferenceExpression(state.modrm);
        ASSERT_not_null(mr);
        switch (state.regField) {
            case 0:
                mr->set_type(DOUBLET);
                return makeInstruction(state, x86_fld, state.modrm);
            case 1:
                mr->set_type(QWORDT);
                return makeInstruction(state, x86_fisttp, state.modrm);
            case 2:
                mr->set_type(DOUBLET);
                return makeInstruction(state, x86_fst, state.modrm);
            case 3:
                mr->set_type(DOUBLET);
                return makeInstruction(state, x86_fstp, state.modrm);
            case 4:
                mr->set_type(BYTET);
                return makeInstruction(state, x86_frstor, state.modrm);
            case 5:
                throw ExceptionX86("bad ModR/M value for x87 opcode 0xdd", state);
            case 6:
                mr->set_type(BYTET);
                return makeInstruction(state, x86_fnsave, state.modrm);
            case 7:
                mr->set_type(WORDT);
                return makeInstruction(state, x86_fnstsw, state.modrm);
            default:
                ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(state.regField));
        }
    } else { // Register forms
        switch (state.regField) {
            case 0: return makeInstruction(state, x86_ffree, state.modrm);
            case 1: throw ExceptionX86("bad ModR/M value for x87 opcode 0xdd", state);
            case 2: return makeInstruction(state, x86_fst, state.modrm);
            case 3: return makeInstruction(state, x86_fstp, state.modrm);
            case 4: return makeInstruction(state, x86_fucom, state.modrm, makeRegister(state, 0, rmST));
            case 5: return makeInstruction(state, x86_fucomp, state.modrm, makeRegister(state, 0, rmST));
            case 6: throw ExceptionX86("bad ModR/M value for x87 opcode 0xdd", state);
            case 7: throw ExceptionX86("bad ModR/M value for x87 opcode 0xdd", state);
            default: ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(state.regField));
        }
    }
    /* avoid MSCV warning by adding return stmt */
    return NULL;
}

SgAsmX86Instruction *
X86::decodeX87InstructionDE(State &state) const
{
    getModRegRM(state, rmReturnNull, rmST, WORDT);
    if (state.modeField < 3) { // Using memory
        switch (state.regField & 7) {
            case 0: return makeInstruction(state, x86_fiadd, state.modrm);
            case 1: return makeInstruction(state, x86_fimul, state.modrm);
            case 2: return makeInstruction(state, x86_ficom, state.modrm);
            case 3: return makeInstruction(state, x86_ficomp, state.modrm);
            case 4: return makeInstruction(state, x86_fisub, state.modrm);
            case 5: return makeInstruction(state, x86_fisubr, state.modrm);
            case 6: return makeInstruction(state, x86_fidiv, state.modrm);
            case 7: return makeInstruction(state, x86_fidivr, state.modrm);
            default: ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(state.regField));
        }
    } else {
        switch (state.regField & 7) {
            case 0: return makeInstruction(state, x86_faddp, state.modrm, makeRegister(state, 0, rmST));
            case 1: return makeInstruction(state, x86_fmulp, state.modrm, makeRegister(state, 0, rmST));
            case 2: throw ExceptionX86("bad ModR/M value for x87 opcode 0xde", state);
            case 3: {
                switch (state.modregrmByte) {
                    case 0xD9: delete state.modrm; delete state.reg; return makeInstruction(state, x86_fcompp);
                    default: throw ExceptionX86("bad ModR/M value for x87 opcode 0xde", state);
                }
            }
            case 4: return makeInstruction(state, x86_fsubrp, state.modrm, makeRegister(state, 0, rmST));
            case 5: return makeInstruction(state, x86_fsubp, state.modrm, makeRegister(state, 0, rmST));
            case 6: return makeInstruction(state, x86_fdivrp, state.modrm, makeRegister(state, 0, rmST));
            case 7: return makeInstruction(state, x86_fdivp, state.modrm, makeRegister(state, 0, rmST));
            default: ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(state.regField));
        }
    }
    /* avoid MSCV warning by adding return stmt */
    return NULL;
}

SgAsmX86Instruction *
X86::decodeX87InstructionDF(State &state) const
{
    getModRegRM(state, rmReturnNull, rmReturnNull, NULL);
    if (state.modeField < 3) { // Using memory
        SgAsmMemoryReferenceExpression* mr = isSgAsmMemoryReferenceExpression(state.modrm);
        ASSERT_not_null(mr);
        switch (state.regField) {
            case 0: mr->set_type(WORDT); return makeInstruction(state, x86_fild, state.modrm);
            case 1: mr->set_type(WORDT); return makeInstruction(state, x86_fisttp, state.modrm);
            case 2: mr->set_type(WORDT); return makeInstruction(state, x86_fist, state.modrm);
            case 3: mr->set_type(WORDT); return makeInstruction(state, x86_fistp, state.modrm);
            case 4: mr->set_type(BYTET); return makeInstruction(state, x86_fbld, state.modrm);
            case 5: mr->set_type(QWORDT); return makeInstruction(state, x86_fild, state.modrm);
            case 6: mr->set_type(BYTET); return makeInstruction(state, x86_fbstp, state.modrm);
            case 7: mr->set_type(QWORDT); return makeInstruction(state, x86_fistp, state.modrm);
            default: ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(state.regField));
        }
    } else {
        state.modrm = makeModrmNormal(state, rmST, NULL);
        switch (state.regField) {
            case 0: throw ExceptionX86("bad ModR/M value for x87 opcode 0xdf", state);
            case 1: throw ExceptionX86("bad ModR/M value for x87 opcode 0xdf", state);
            case 2: throw ExceptionX86("bad ModR/M value for x87 opcode 0xdf", state);
            case 3: throw ExceptionX86("bad ModR/M value for x87 opcode 0xdf", state);
            case 4: {
                if (state.modregrmByte == 0xE0) {
                    return makeInstruction(state, x86_fnstsw, makeRegister(state, 0, rmWord));
                } else {
                    throw ExceptionX86("bad ModR/M value for x87 opcode 0xdf", state);
                }
            }
            case 5: return makeInstruction(state, x86_fucomip, makeRegister(state, 0, rmST), state.modrm);
            case 6: return makeInstruction(state, x86_fcomip, makeRegister(state, 0, rmST), state.modrm);
            case 7: throw ExceptionX86("bad ModR/M value for x87 opcode 0xdf", state);
            default: ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(state.regField));
        }
    }
    /* avoid MSCV warning by adding return stmt */
    return NULL;
}

SgAsmX86Instruction *
X86::decodeGroup1(State &state, SgAsmExpression* imm) const
{
    switch (state.regField) {
        case 0: return makeInstruction(state, x86_add, state.modrm, imm);
        case 1: return makeInstruction(state, x86_or, state.modrm, imm);
        case 2: return makeInstruction(state, x86_adc, state.modrm, imm);
        case 3: return makeInstruction(state, x86_sbb, state.modrm, imm);
        case 4: return makeInstruction(state, x86_and, state.modrm, imm);
        case 5: return makeInstruction(state, x86_sub, state.modrm, imm);
        case 6: return makeInstruction(state, x86_xor, state.modrm, imm);
        case 7: return makeInstruction(state, x86_cmp, state.modrm, imm);
        default: ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(state.regField));
    }
    /* avoid MSCV warning by adding return stmt */
    return NULL;
}

SgAsmX86Instruction *
X86::decodeGroup1a(State &state) const
{
    if (state.regField != 0)
        throw ExceptionX86("bad ModR/M value for Group 1a opcode", state);
    return makeInstruction(state, x86_pop, state.modrm);
}

SgAsmX86Instruction *
X86::decodeGroup2(State &state, SgAsmExpression* count) const
{
    switch (state.regField) {
        case 0: return makeInstruction(state, x86_rol, state.modrm, count);
        case 1: return makeInstruction(state, x86_ror, state.modrm, count);
        case 2: return makeInstruction(state, x86_rcl, state.modrm, count);
        case 3: return makeInstruction(state, x86_rcr, state.modrm, count);
        case 4: return makeInstruction(state, x86_shl, state.modrm, count);
        case 5: return makeInstruction(state, x86_shr, state.modrm, count);
        case 6: return makeInstruction(state, x86_shl, state.modrm, count);
        case 7: return makeInstruction(state, x86_sar, state.modrm, count);
        default: ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(state.regField));
    }
    /* avoid MSCV warning by adding return stmt */
    return NULL;
}

SgAsmX86Instruction *
X86::decodeGroup3(State &state, SgAsmExpression* immMaybe) const
{
    switch (state.regField) {
        case 0:
        case 1:
            ASSERT_require(immMaybe);
            return makeInstruction(state, x86_test, state.modrm, immMaybe);
        case 2:
            return makeInstruction(state, x86_not, state.modrm);
        case 3:
            return makeInstruction(state, x86_neg, state.modrm);
        case 4:
            return makeInstruction(state, x86_mul, state.modrm);
        case 5:
            return makeInstruction(state, x86_imul, state.modrm);
        case 6:
            return makeInstruction(state, x86_div, state.modrm);
        case 7:
            return makeInstruction(state, x86_idiv, state.modrm);
        default:
            ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(state.regField));
    }
    /* avoid MSCV warning by adding return stmt */
    return NULL;
}

SgAsmX86Instruction *
X86::decodeGroup4(State &state) const
{
    switch (state.regField) {
        case 0: return makeInstruction(state, x86_inc, state.modrm);
        case 1: return makeInstruction(state, x86_dec, state.modrm);
        default: throw ExceptionX86("bad ModR/M value for Group 4 opcode", state);
    }
    /* avoid MSCV warning by adding return stmt */
    return NULL;
}

SgAsmX86Instruction *
X86::decodeGroup5(State &state) const
{
    switch (state.regField) {
        case 0:
            return makeInstruction(state, x86_inc, state.modrm);
        case 1:
            return makeInstruction(state, x86_dec, state.modrm);
        case 2:
            return makeInstruction(state, x86_call, state.modrm);
        case 3:
            return makeInstruction(state, x86_farcall, state.modrm);
        case 4:
            state.isUnconditionalJump = true;
            return makeInstruction(state, x86_jmp, state.modrm);
        case 5:
            state.isUnconditionalJump = true;
            return makeInstruction(state, x86_farjmp, state.modrm);
        case 6:
            return makeInstruction(state, x86_push, state.modrm);
        case 7:
            throw ExceptionX86("bad ModR/M value for Group 5 opcode", state);
        default:
            ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(state.regField));
    }
    /* avoid MSCV warning by adding return stmt */
    return NULL;
}

SgAsmX86Instruction *
X86::decodeGroup6(State &state) const
{
    switch (state.regField) {
        case 0: return makeInstruction(state, x86_sldt, state.modrm); // FIXME adjust register size
        case 1: return makeInstruction(state, x86_str, state.modrm); // FIXME adjust register size
        case 2: return makeInstruction(state, x86_lldt, state.modrm);
        case 3: return makeInstruction(state, x86_ltr, state.modrm);
        case 4: return makeInstruction(state, x86_verr, state.modrm);
        case 5: return makeInstruction(state, x86_verw, state.modrm);
        case 6: throw ExceptionX86("bad ModR/M value for Group 6 opcode", state);
        case 7: throw ExceptionX86("bad ModR/M value for Group 6 opcode", state);
        default: ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(state.regField));
    }
    /* avoid MSCV warning by adding return stmt */
    return NULL;
}

SgAsmX86Instruction *
X86::decodeGroup7(State &state) const
{
    getModRegRM(state, rmReturnNull, rmReturnNull, NULL);
    switch (state.regField) {
        case 0: {
            if (state.modeField == 3) {
                switch (state.rmField) {
                    case 1: return makeInstruction(state, x86_vmcall);
                    case 2: return makeInstruction(state, x86_vmlaunch);
                    case 3: return makeInstruction(state, x86_vmresume);
                    case 4: return makeInstruction(state, x86_vmxoff);
                    default: throw ExceptionX86("bad ModR/M value for Group 7 opcode", state);
                }
            } else {
                fillInModRM(state, rmReturnNull, BYTET /* pseudo-descriptor */ );
                return makeInstruction(state, x86_sgdt, state.modrm);
            }
        }
        case 1: {
            if (state.modeField == 3) {
                switch (state.rmField) {
                    case 0: return makeInstruction(state, x86_monitor);
                    case 1: return makeInstruction(state, x86_mwait);
                    default: throw ExceptionX86("bad ModR/M value for Group 7 opcode", state);
                }
            } else {
                fillInModRM(state, rmReturnNull, BYTET /* pseudo-descriptor */ );
                return makeInstruction(state, x86_sidt, state.modrm);
            }
        }
        case 2: {
            if (state.modeField ==3) {
                switch (state.rmField) {
                    case 0: return makeInstruction(state, x86_xgetbv);
                    case 1: return makeInstruction(state, x86_xsetbv);
                    default: throw ExceptionX86("bad ModR/M value for Group 7 opcode", state);
                }
            } else {
                fillInModRM(state, rmReturnNull, BYTET /* pseudo-descriptor */ );
                return makeInstruction(state, x86_lgdt, state.modrm);
            }
        }
        case 3: {
            if (state.modeField == 3) {
                switch (state.rmField) {
                    case 0: return makeInstruction(state, x86_vmrun);
                    case 1: return makeInstruction(state, x86_vmmcall);
                    case 2: return makeInstruction(state, x86_vmload);
                    case 3: return makeInstruction(state, x86_vmsave);
                    case 4: return makeInstruction(state, x86_stgi);
                    case 5: return makeInstruction(state, x86_clgi);
                    case 6: return makeInstruction(state, x86_skinit);
                    case 7: return makeInstruction(state, x86_invlpga);
                    default: ASSERT_not_reachable("invalid rm field: " + StringUtility::numberToString(state.rmField));
                }
            } else {
                fillInModRM(state, rmReturnNull, BYTET /* pseudo-descriptor */ );
                return makeInstruction(state, x86_lidt, state.modrm);
            }
        }
        case 4: {
            fillInModRM(state, effectiveOperandMode(state), WORDT);
            return makeInstruction(state, x86_smsw, state.modrm);
        }
        case 5:
            throw ExceptionX86("bad ModR/M value for Group 7 opcode", state);
        case 6: {
            fillInModRM(state, rmWord, WORDT);
            return makeInstruction(state, x86_lmsw, state.modrm);
        }
        case 7: {
            if (state.modeField == 3) {
                switch (state.rmField) {
                    case 0: return makeInstruction(state, x86_swapgs);
                    case 1: return makeInstruction(state, x86_rdtscp);
                    default: throw ExceptionX86("bad ModR/M value for Group 7 opcode", state);
                }
            } else {
                fillInModRM(state, rmReturnNull, BYTET);
                return makeInstruction(state, x86_invlpg, state.modrm);
            }
        }
        default:
            ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(state.regField));
    }
    /* avoid MSCV warning by adding return stmt */
    return NULL;
}

SgAsmX86Instruction *
X86::decodeGroup8(State &state, SgAsmExpression* imm) const
{
    switch (state.regField) {
        case 0: throw ExceptionX86("bad ModR/M value for Group 8 opcode", state);
        case 1: throw ExceptionX86("bad ModR/M value for Group 8 opcode", state);
        case 2: throw ExceptionX86("bad ModR/M value for Group 8 opcode", state);
        case 3: throw ExceptionX86("bad ModR/M value for Group 8 opcode", state);
        case 4: return makeInstruction(state, x86_bt, state.modrm, imm);
        case 5: return makeInstruction(state, x86_bts, state.modrm, imm);
        case 6: return makeInstruction(state, x86_btr, state.modrm, imm);
        case 7: return makeInstruction(state, x86_btc, state.modrm, imm);
        default: ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(state.regField));
    }
    /* avoid MSCV warning by adding return stmt */
    return NULL;
}

SgAsmX86Instruction *
X86::decodeGroup11(State &state, SgAsmExpression* imm) const
{
    switch (state.regField) {
        case 0: return makeInstruction(state, x86_mov, state.modrm, imm);
        default: throw ExceptionX86("bad ModR/M value for Group 11 opcode", state);
    }
    /* avoid MSCV warning by adding return stmt */
    return NULL;
}

SgAsmX86Instruction *
X86::decodeGroup15(State &state) const
{
    getModRegRM(state, rmReturnNull, rmReturnNull, NULL);
    switch (state.regField) {
        case 0:
            requireMemory(state);
            fillInModRM(state, rmReturnNull, BYTET);
            return makeInstruction(state, x86_fxsave, state.modrm);
        case 1:
            requireMemory(state);
            fillInModRM(state, rmReturnNull, BYTET);
            return makeInstruction(state, x86_fxrstor, state.modrm);
        case 2:
            requireMemory(state);
            fillInModRM(state, rmReturnNull, DWORDT);
            return makeInstruction(state, x86_ldmxcsr, state.modrm);
        case 3:
            requireMemory(state);
            fillInModRM(state, rmReturnNull, DWORDT);
            return makeInstruction(state, x86_stmxcsr, state.modrm);
        case 4:
            requireMemory(state);
            fillInModRM(state, rmReturnNull, BYTET);
            return makeInstruction(state, x86_xsave, state.modrm);
        case 5:
            if (state.modeField == 3) {
                return makeInstruction(state, x86_lfence);
            } else {
                return makeInstruction(state, x86_xrstor, state.modrm);
            }
        case 6:
            if (state.modeField == 3) {
                return makeInstruction(state, x86_mfence);
            } else {
                throw ExceptionX86("bad ModR/M value for Group 15 opcode", state);
            }
        case 7:
            if (state.modeField == 3) {
                return makeInstruction(state, x86_sfence);
            } else {
                fillInModRM(state, rmReturnNull, BYTET);
                return makeInstruction(state, x86_clflush, state.modrm);
            }
        default:
            ASSERT_not_reachable("invalid reg field: " + StringUtility::numberToString(state.regField));
    }
    /* avoid MSCV warning by adding return stmt */
    return NULL;
}

SgAsmX86Instruction *
X86::decodeGroup16(State &state) const
{
    requireMemory(state);
    switch (state.regField) {
        case 0: return makeInstruction(state, x86_prefetchnta, state.modrm);
        case 1: return makeInstruction(state, x86_prefetcht0, state.modrm);
        case 2: return makeInstruction(state, x86_prefetcht1, state.modrm);
        case 3: return makeInstruction(state, x86_prefetcht2, state.modrm);
        default: return makeInstruction(state, x86_prefetch, state.modrm);
    }
}

SgAsmX86Instruction *
X86::decodeGroupP(State &state) const
{
    getModRegRM(state, rmReturnNull, rmLegacyByte, BYTET);
    requireMemory(state);
    switch (state.regField) {
        case 0: return makeInstruction(state, x86_prefetch, state.modrm);
        case 1: return makeInstruction(state, x86_prefetchw, state.modrm);
        case 3: return makeInstruction(state, x86_prefetchw, state.modrm);
        default: return makeInstruction(state, x86_prefetch, state.modrm);
    }
}

} // namespace
} // namespace
} // namespace

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::Disassembler::X86);
#endif

#endif
